#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include "hexdump.hh"

using namespace std;


/*
Basic fixed point class for representing real numbers.
The value of the represented real number is _val / 2 ^ (31 - _log_R)
For simplicity, we only allow _log_R >= 0, which means that numbers smaller than 1 may be represented less accurately.
*/
class FixedPoint_32_8 {
public:
    uint32_t _val;
    uint8_t _log_R;

    FixedPoint_32_8() { _val = _log_R = 0; }; //Default c'tor

    FixedPoint_32_8(uint32_t val, uint8_t log_R) : _val(val), _log_R(log_R) {}; //c'tor that gets val and log_R as parameters

    FixedPoint_32_8(double x) { //c'tor that gets a real number and computes val and log_R
        if (x < 1)
            _log_R = 0;
        else
            _log_R = (uint8_t)ceil(log2(x));
        _val = x * ((uint32_t)1 << (31 - _log_R));
    }
    
    FixedPoint_32_8(FixedPoint_32_8 const& obj) : _val(obj._val), _log_R(obj._log_R) {}

    ~FixedPoint_32_8() = default;

    void set_log_R(uint8_t target_log_R) { //changing the range  without affecting the represented value
        if (target_log_R >= _log_R) {
            _val = _val >> (target_log_R - _log_R);
        }
        else {
            assert(_val < ((uint32_t)1 << (32 - (_log_R - target_log_R)))); //avoid overflows
            _val = (uint32_t)_val << (_log_R - target_log_R);
        }
        _log_R = target_log_R;
    }
    
    double value() const {
        return ((double)_val) / ((uint32_t)1 << (31 - _log_R));
    } //get the represented value
    
    FixedPoint_32_8 operator + (FixedPoint_32_8 const& obj) const { //summing real numbers with fixed-point representatio
        if (obj._log_R > _log_R) {
            FixedPoint_32_8 res(*this);
            res.set_log_R(obj._log_R);
            res._val += obj._val;
            return res;
        }
        FixedPoint_32_8 res(obj);
        res.set_log_R(_log_R);
        uint64_t targetVal = res._val + _val;
        if (targetVal > (uint64_t)1 << 31) { //would overflow, we need to increase the range
            res.set_log_R(_log_R+1);
            res._val += (_val >> 1);
        }
        else {
            res._val += _val;
        }	
        return res;
    }

    FixedPoint_32_8 operator - (FixedPoint_32_8 const& obj) const { //summing real numbers with fixed-point representatio
        if (obj._log_R > _log_R) {
            FixedPoint_32_8 res(*this);
            res.set_log_R(obj._log_R);
            assert(res._val >= obj._val); //return error if the result is a negative number
            res._val -= obj._val;
            return res;
        }
        FixedPoint_32_8 res(obj);
        res.set_log_R(_log_R);
        assert(res._val <= _val); 
        res._val = _val - res._val;
        return res;
    }
    
    FixedPoint_32_8 operator + (int const& x) const { //summing real numbers with fixed-point representatio
        if (x >= 0)
            return (*this) + FixedPoint_32_8((double)x);
        else
            return (*this) - FixedPoint_32_8((double)-x);
    }

};


/*
A more advanced class that approximates logarithms, exponentiation, and multiplication using bitwise and addition operations.
It uses two lookup tables that can be computed offline and placed in the switch's memory.
We use around 100K entries total. The number of entries would affect the accuracy
*/
#define LOG_TABLE_SIZE 65536
#define EXP_TABLE_ENTRY 1024 
#define EXP_TABLE_EXP 42
//supports exponentiating numbers smaller than 2^(42-10)=2^32

class FixedPoint_32_8_table : public FixedPoint_32_8 {
    static FixedPoint_32_8 _log_lookup_table[LOG_TABLE_SIZE];
    static FixedPoint_32_8 _exp_lookup_table[EXP_TABLE_ENTRY][EXP_TABLE_EXP]; 

public:

    //compute the lookup tables. Can be done offline and is only required once, regardless of the number of variables.
    static void init_lookup_tables() { 
        _log_lookup_table[0] = 0.0;
        for (int i = 1; i < LOG_TABLE_SIZE; ++i) {
            _log_lookup_table[i] = FixedPoint_32_8(log2((double)i));
        }
        
        for (int i = 0; i < EXP_TABLE_ENTRY; ++i) {
            for (int j = 0; j < EXP_TABLE_EXP; ++j) {
                _exp_lookup_table[i][j] = FixedPoint_32_8(pow(2, (double)i * pow(2, j - 10)));
            }
        }
    };
    
    FixedPoint_32_8_table(double frac) : FixedPoint_32_8(frac) { 
    };
    
    FixedPoint_32_8_table(FixedPoint_32_8 base) : FixedPoint_32_8(base) { 
    };

    /*
	A function that approximates log2 of the current real number using the _log_lookup_table lookup table.
	It looks for the 16 bits following the most significant set bit and look them up in the table. Then it adds the range, number of leading zeros, etc.
    */
    FixedPoint_32_8_table log_2() const {
        assert(_val > 0);
        int ms_one_index = 32 - __builtin_clz(_val); // number of leading 0-bits in _val; implementable using TCAM
        int shift_len = ms_one_index > 16 ? ms_one_index - 16 : 0; // 16 is chosen based on 65536 LOG_TABLE_SIZE
        uint32_t lookup_key = _val >> shift_len;
        FixedPoint_32_8 res = _log_lookup_table[lookup_key] + (int)(shift_len + _log_R - 31);
        return FixedPoint_32_8_table(res);
    };

    /*
      A function that approximates 2^x for the current real number x using the _exp_lookup_table lookup table.
      It looks for the ten bits following the most significant set bit and look them up in the table together with the range.
    */
    FixedPoint_32_8_table exp_2() const {
        assert(_val > 0);
        uint8_t ms_one_index = 32 - __builtin_clz(_val); // implementable using TCAM
        int shift_len = ms_one_index > 10 ? ms_one_index - 10 : 0;
        uint32_t lookup_key = _val >> shift_len;
        assert(lookup_key < EXP_TABLE_ENTRY);
        return FixedPoint_32_8_table(_exp_lookup_table[lookup_key][shift_len+_log_R-31+10]);
    }
    
    /*
      A function that multiplies the current number by another (obj). 
      It uses the fact that cur * obj = 2^(log2(cur) + log2(obj)). 
      Since we implemented logarithms and exponentiation, and summation is supported, this can be deployed on switches.
    */
    FixedPoint_32_8_table operator * (FixedPoint_32_8_table const& obj) const {
        FixedPoint_32_8 my_log = log_2();
        FixedPoint_32_8 obj_log = obj.log_2();
        return FixedPoint_32_8_table(my_log + obj_log).exp_2();
    }
    
};

FixedPoint_32_8 FixedPoint_32_8_table::_log_lookup_table[LOG_TABLE_SIZE];
FixedPoint_32_8 FixedPoint_32_8_table::_exp_lookup_table[EXP_TABLE_ENTRY][EXP_TABLE_EXP];

int main(int argc, char* argv[]) {

    // offline initialization stage
    FixedPoint_32_8_table::init_lookup_tables();

    //note: multiply only works for positive values for now; and y must be at least 1/x
    if (argc <= 2) {
        fprintf(stderr, "Usage: Input two double values x and y; Output x+y, x*y, log2(x), log2(y), 2^log2(x), 2^log2(y)\n");
        exit(1);
    }
    
    double xVal = atof(argv[1]); 
    double yVal = atof(argv[2]);

    if (xVal < 0 || yVal < 0) {
        fprintf(stderr, "Negative numbers not supported right now\n");
        exit(1);
    }
	
    FixedPoint_32_8_table x(xVal);
    FixedPoint_32_8_table y(yVal);
    
    FixedPoint_32_8_table logx(x.log_2().value());
    FixedPoint_32_8_table logy(y.log_2().value());

    cout << "True Values: " << std::setprecision(20) << xVal << " " << std::setprecision(20) << yVal << endl;
    cout << "Represented Values: " << x.value() << " " << y.value() << endl << endl;

    printf("x._val %x log_R %hhu\n", x._val, x._log_R);
    printf("y._val %x log_R %hhu\n", y._val, y._log_R);
    
    cout << "True x+y: " << xVal + yVal << endl;
    cout << "Approx x+y: " << (x + y).value() << endl << endl;
    
    cout << "True log2(x): " << log2(xVal) << endl;
    cout << "Approx log2(x): " << logx.value() << endl << endl;
    
    cout << "True x = " << xVal << endl;
    cout << "Approx 2^log2(x) = " << logx.exp_2().value() << endl << endl; //Show that 2^log2(x) is pretty close to x
    cout << "True log2(y): " << log2(yVal) << endl;
    cout << "Approx log2(y): " << logy.value() << endl << endl;
    
    cout << "True y = " << yVal << endl;
    cout << "Approx 2^log2(y) = " << logy.exp_2().value() << endl << endl;

    if (yVal > 1.0/xVal) {
        cout << "True x*y = " << xVal * yVal << endl;
        cout << "Approx x*y = " << (x * y).value() << endl;
    } else {
        cout << "x*y not supported for x*y < 1 cases yet" << endl;
    }
    
    return 0;
}
