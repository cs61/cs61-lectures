#ifndef WEENSYOS_LIB_HH
#define WEENSYOS_LIB_HH
#include "types.h"
#include <new>              // for placement new
#include <type_traits>
#define BUFSIZ 1024

// lib.hh
//
//    Functions, constants, and definitions useful in both the kernel
//    and applications.
//
//    Contents: (1) C library subset, (2) system call numbers, (3) console.

extern "C" {
void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* a, const void* b, size_t n);
void* memchr(const void* s, int c, size_t n);
size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t maxlen);
size_t strlcpy(char* dst, const char* src, size_t maxlen);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t maxlen);
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, size_t maxlen);
char* strchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
long strtol(const char* s, char** endptr = nullptr, int base = 0);
unsigned long strtoul(const char* s, char** endptr = nullptr, int base = 0);
ssize_t snprintf(char* s, size_t size, const char* format, ...);
ssize_t vsnprintf(char* s, size_t size, const char* format, va_list val);
inline bool isspace(int c);
inline bool isdigit(int c);
inline bool islower(int c);
inline bool isupper(int c);
inline bool isalpha(int c);
inline bool isalnum(int c);
inline int tolower(int c);
inline int toupper(int c);
}

#define RAND_MAX 0x7FFFFFFF
int rand();
void srand(unsigned seed);

// rand(min, max)
//    Return a pseudorandom number roughly evenly distributed between
//    `min` and `max`, inclusive. Requires `min <= max` and
//    `max - min <= RAND_MAX`.
int rand(int min, int max);

// rand_engine
//    A `rand`-style pseudorandom number generator lacking global state.
struct rand_engine {
    using result_type = unsigned;
    unsigned long seed_;

    inline rand_engine()                   { seed(819234718U); }
    inline rand_engine(unsigned s)         { seed(s); }
    inline rand_engine(unsigned long s)    { seed(s); }
    inline static constexpr unsigned min() { return 0; }
    inline static constexpr unsigned max() { return RAND_MAX; }
    inline void seed(unsigned s)           { seed(((unsigned long) s) << 32 | s); }
    inline void seed(unsigned long s)      { seed_ = s; }
    unsigned operator()();
    unsigned operator()(unsigned min, unsigned max);
    int operator()(int min, int max);
};

// from_chars, to_chars
//    C++-style functions that parse integers from the start of a string,
//    or unparse integers into a string, with error detection.
struct from_chars_result {
    const char* ptr;
    int ec;
};
from_chars_result from_chars(const char* first, const char* last,
                             long& value, int base = 10);
from_chars_result from_chars(const char* first, const char* last,
                             unsigned long& value, int base = 10);
using to_chars_result = from_chars_result;
to_chars_result to_chars(char* first, char* last,
                         long value, int base = 10);
to_chars_result to_chars(char* first, char* last,
                         unsigned long value, int base = 10);
inline to_chars_result to_chars(char* first, char* last,
                                int value, int base = 10) {
    return to_chars(first, last, long(value), base);
}


// Return the offset of `member` relative to the beginning of a struct type
#ifndef offsetof
#define offsetof(type, member)  __builtin_offsetof(type, member)
#endif

// Return the number of elements in an array
#define arraysize(array)        (sizeof(array) / sizeof(array[0]))


// Arithmetic

// min(a, b, ...)
//    Return the minimum of the arguments.
template <typename T>
inline constexpr T min(T a, T b) {
    return a < b ? a : b;
}
template <typename T, typename... Rest>
inline constexpr T min(T a, T b, Rest... c) {
    return min(min(a, b), c...);
}

// max(a, b, ...)
//    Return the maximum of the arguments.
template <typename T>
inline constexpr T max(T a, T b) {
    return b < a ? a : b;
}
template <typename T, typename... Rest>
inline constexpr T max(T a, T b, Rest... c) {
    return max(max(a, b), c...);
}

// msb(x)
//    Return index of most significant one bit in `x`, plus one.
//    Returns 0 if `x == 0`.
inline constexpr int msb(int x) {
    return x ? sizeof(x) * 8 - __builtin_clz(x) : 0;
}
inline constexpr int msb(unsigned x) {
    return x ? sizeof(x) * 8 - __builtin_clz(x) : 0;
}
inline constexpr int msb(long x) {
    return x ? sizeof(x) * 8 - __builtin_clzl(x) : 0;
}
inline constexpr int msb(unsigned long x) {
    return x ? sizeof(x) * 8 - __builtin_clzl(x) : 0;
}
inline constexpr int msb(long long x) {
    return x ? sizeof(x) * 8 - __builtin_clzll(x) : 0;
}
inline constexpr int msb(unsigned long long x) {
    return x ? sizeof(x) * 8 - __builtin_clzll(x) : 0;
}

// lsb(x)
//    Return index of least significant one bit in `x`, plus one.
//    Returns 0 if `x == 0`.
inline constexpr int lsb(int x) {
    return __builtin_ffs(x);
}
inline constexpr int lsb(unsigned x) {
    return __builtin_ffs(x);
}
inline constexpr int lsb(long x) {
    return __builtin_ffsl(x);
}
inline constexpr int lsb(unsigned long x) {
    return __builtin_ffsl(x);
}
inline constexpr int lsb(long long x) {
    return __builtin_ffsll(x);
}
inline constexpr int lsb(unsigned long long x) {
    return __builtin_ffsll(x);
}

// round_down(x, m)
//    Return the largest multiple of `m` less than or equal to `x`.
//    Equivalently, round `x` down to the nearest multiple of `m`.
template <typename T>
inline constexpr T round_down(T x, unsigned m) {
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return x - (x % m);
}

// round_up(x, m)
//    Return the smallest multiple of `m` greater than or equal to `x`.
//    Equivalently, round `x` up to the nearest multiple of `m`.
template <typename T>
inline constexpr T round_up(T x, unsigned m) {
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return round_down(x + m - 1, m);
}

// round_down_pow2(x)
//    Return the largest power of 2 less than or equal to `x`.
//    Equivalently, round `x` down to the nearest power of 2.
//    Returns 0 if `x == 0`.
template <typename T>
inline constexpr T round_down_pow2(T x) {
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return x ? T(1) << (msb(x) - 1) : 0;
}

// round_up_pow2(x)
//    Return the smallest power of 2 greater than or equal to `x`.
//    Equivalently, round `x` up to the nearest power of 2.
//    Returns 0 if `x == 0`.
template <typename T>
inline constexpr T round_up_pow2(T x) {
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return x ? T(1) << msb(x - 1) : 0;
}


// Character traits

inline bool isspace(int c) {
    return (c >= '\t' && c <= '\r') || c == ' ';
}
inline bool isdigit(int c) {
    return (unsigned(c) - unsigned('0')) < 10;
}
inline bool islower(int c) {
    return (unsigned(c) - unsigned('a')) < 26;
}
inline bool isupper(int c) {
    return (unsigned(c) - unsigned('A')) < 26;
}
inline bool isalpha(int c) {
    return ((unsigned(c) | 0x20) - unsigned('a')) < 26;
}
inline bool isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

inline int tolower(int c) {
    return isupper(c) ? c + 'a' - 'A' : c;
}
inline int toupper(int c) {
    return islower(c) ? c + 'A' - 'a' : c;
}


// Checksums

uint32_t crc32c(uint32_t crc, const void* buf, size_t sz);
inline uint32_t crc32c(const void* buf, size_t sz) {
    return crc32c(0, buf, sz);
}


// System call numbers: the `syscall` instruction initiates a system
// call. The system call number `SYSCALL_XXX` is in the `%rax`
// register, and other arguments are arranged according to the system
// call calling convention.

#define SYSCALL_GETPID          1
#define SYSCALL_YIELD           2
#define SYSCALL_PANIC           3
#define SYSCALL_GETSYSNAME      4
#define SYSCALL_WRITE           5
#define SYSCALL_PAGE_ALLOC      6
#define SYSCALL_EXIT            7


// System call error return values

#define E_INVAL         -22        // Invalid argument
#define E_OVERFLOW      -75        // Value too large for data type
#define E_RANGE         -34        // Out of range

#define E_MINERROR      -100

inline bool is_error(uintptr_t r) {
    return r >= static_cast<uintptr_t>(E_MINERROR);
}


// Maximum number of processes

#ifndef MAXNPROC
#define MAXNPROC         16
#endif


// CGA console printing

#define CONSOLE_COLUMNS     80
#define CONSOLE_ROWS        25
#define CPOS(row, col)      ((row) * 80 + (col))
#define CROW(cpos)          ((cpos) / 80)
#define CCOL(cpos)          ((cpos) % 80)
#define END_CPOS            (CONSOLE_ROWS * CONSOLE_COLUMNS)

extern volatile uint16_t console[CONSOLE_ROWS * CONSOLE_COLUMNS];

// current position of the cursor (80 * ROW + COL)
extern volatile int cursorpos;

// Console colors
//    `COLOR_*` constants are CGA colors: numbers between 0x0000 and 0xFF00.
//    Bits 8-11 set the foreground color and bits 12-15 set the background.
//    https://en.wikipedia.org/wiki/Color_Graphics_Adapter
//    They can be passed to `console_puts` or a `%C` format specification.
//
//    `CS_*` constants are ANSI terminal escape sequences, and are typically
//    prefixed to a print format, as in `console_printf(CS_WHITE "hello\n")`.
//    https://en.wikipedia.org/wiki/ANSI_escape_code

#define COLOR_GRAY          0x0700    // gray foreground, black background
#define COLOR_WHITE         0x0F00    // white foreground, black background
#define COLOR_ERROR         0xCF00    // white foreground, red background
#define COLOR_SUCCESS       0x0A00    // green foreground, black background

#define CS_NORMAL           "\x1b[m"
#define CS_WHITE            "\x1b[1m"
#define CS_GREEN            "\x1b[32m"
#define CS_YELLOW           "\x1b[93m"
#define CS_ERROR            "\x1b[41;1m"
#define CS_SUCCESS          "\x1b[32;1m"
#define CS_ECHO             "\x1b[36m"


// console_clear
//    Erases the console and moves the cursor to the upper left (CPOS(0, 0)).
void console_clear();


// console_puts(cpos, color, s, len)
//    Write a string to the CGA console. Writes exactly `len` characters.
//
//    The `cpos` argument is a cursor position, such as `CPOS(r, c)`
//    for row number `r` and column number `c`. `cpos == -1` prints at the
//    current cursor position. The `color` argument is an initial color.
//
//    Returns the final position of the cursor.
int console_puts(int cpos, int color, const char* s, size_t len);


// console_printf(cpos, format, ...)
//    Print a formatted message to the CGA console.
//
//    The `format` argument supports some of the C printf function’s format
//    specifications: `%d` prints an integer in decimal notation, `%u` prints
//    an unsigned integer in decimal notation, `%x` prints an unsigned integer
//    in hexadecimal notation, `%c` prints a character, and `%s` prints a
//    string. Field widths and precisions are also supported.
//
//    The `cpos` argument is a cursor position, such as `CPOS(r, c)` for
//    row number `r` and column number `c`.
//
//    The initial color is gray on black. To change the color, use a color
//    escape sequence such as `CS_ERROR`, or a format specification `%C`,
//    which reads an integer color from the parameter list.
//
//    Returns the final position of the cursor.
int console_printf(int cpos, const char* format, ...);

// console_vprintf(cpos, color, format val)
//    The vprintf version of console_printf.
int console_vprintf(int cpos, const char* format, va_list val);

// console_printf(format, ...)
//    Print a formatted message to the console at the current cursor position.
void console_printf(const char* format, ...);


// Generic print library

struct printer;

struct ansi_escape_buffer {
    char buf_[12];
    int len_ = 0;
    inline bool putc(unsigned char c, printer& pr);
    void flush(printer& pr);
    [[gnu::noinline, gnu::cold]] void putc_impl(unsigned char c, printer& pr);
};

struct printer {
    int color_ = COLOR_GRAY;
    virtual void putc(unsigned char c) = 0;
    void printf(const char* format, ...);
    void vprintf(const char* format, va_list val);
};

struct console_printer : public printer {
    volatile uint16_t* cell_;
    unsigned short scroll_mode_;
    short scroll_blank_ = -1;
    ansi_escape_buffer ebuf_;
    enum { scroll_off = 0, scroll_on = 1, scroll_blank = 2 };
    console_printer(int cpos, int scroll_mode);
    void putc(unsigned char c) override;
    void scroll();
    void move_cursor();
};


// error_printf([cursor,] format, ...)
//    Like `console_printf`, but for errors. In the kernel, the message
//    is printed to the botttom of the screen, initially in white on
//    red, and the message is also printed to the log.
[[gnu::noinline, gnu::cold]]
void error_printf(const char* format, ...);
[[gnu::noinline, gnu::cold]]
void error_vprintf(const char* format, va_list val);


inline bool ansi_escape_buffer::putc(unsigned char c, printer& pr) {
    if (len_ < 0 || (len_ == 0 && c != '\x1b' /* ESC */)) {
        return false;
    }
    putc_impl(c, pr);
    return true;
}


// Type information

// printfmt<T>
//    `printfmt<T>::spec` defines a printf specifier for type T.
//    E.g., `printfmt<int>::spec` is `"d"`.

template <typename T> struct printfmt {};
template <> struct printfmt<bool>           { static constexpr char spec[] = "d"; };
template <> struct printfmt<char>           { static constexpr char spec[] = "c"; };
template <> struct printfmt<signed char>    { static constexpr char spec[] = "d"; };
template <> struct printfmt<unsigned char>  { static constexpr char spec[] = "u"; };
template <> struct printfmt<short>          { static constexpr char spec[] = "d"; };
template <> struct printfmt<unsigned short> { static constexpr char spec[] = "u"; };
template <> struct printfmt<int>            { static constexpr char spec[] = "d"; };
template <> struct printfmt<unsigned>       { static constexpr char spec[] = "u"; };
template <> struct printfmt<long>           { static constexpr char spec[] = "ld"; };
template <> struct printfmt<unsigned long>  { static constexpr char spec[] = "lu"; };
template <typename T> struct printfmt<T*>   { static constexpr char spec[] = "p"; };

template <typename T> constexpr char printfmt<T*>::spec[];


// Assertions

// assert(x)
//    If `x == 0`, print a message and fail.
#define assert(x, ...)       do {                                       \
        if (!(x)) {                                                     \
            assert_fail(__FILE__, __LINE__, #x, ## __VA_ARGS__);        \
        }                                                               \
    } while (false)

[[noreturn, gnu::noinline, gnu::cold]]
void assert_fail(const char* file, int line, const char* msg,
                 const char* description = nullptr);


// assert_[eq, ne, lt, le, gt, ge](x, y)
//    Like `assert(x OP y)`, but also prints the values of `x` and `y` on
//    failure.
#define assert_op(x, op, y) do {                                        \
        auto __x = (x); auto __y = (y);                                 \
        using __t = typename std::common_type<typeof(__x), typeof(__y)>::type; \
        if (!(__x op __y)) {                                            \
            assert_op_fail<__t>(__FILE__, __LINE__, #x " " #op " " #y,  \
                                __x, #op, __y);                         \
        } } while (0)
#define assert_eq(x, y) assert_op(x, ==, y)
#define assert_ne(x, y) assert_op(x, !=, y)
#define assert_lt(x, y) assert_op(x, <, y)
#define assert_le(x, y) assert_op(x, <=, y)
#define assert_gt(x, y) assert_op(x, >, y)
#define assert_ge(x, y) assert_op(x, >=, y)

template <typename T>
[[noreturn, gnu::noinline, gnu::cold]]
void assert_op_fail(const char* file, int line, const char* msg,
                    const T& x, const char* op, const T& y) {
    char fmt[48];
    snprintf(fmt, sizeof(fmt), "%%s:%%d: expected %%%s %s %%%s\n",
             printfmt<T>::spec, op, printfmt<T>::spec);
    error_printf(fmt, file, line, x, y);
    assert_fail(file, line, msg);
}


// assert_memeq(x, y, sz)
//    If `memcmp(x, y, sz) != 0`, print a message and fail.
#define assert_memeq(x, y, sz)    do {                                  \
        auto __x = (x); auto __y = (y); size_t __sz = (sz);             \
        if (memcmp(__x, __y, __sz) != 0) {                              \
            assert_memeq_fail(__FILE__, __LINE__, "memcmp(" #x ", " #y ", " #sz ") == 0", __x, __y, __sz); \
        }                                                               \
    } while (0)
[[noreturn, gnu::noinline, gnu::cold]]
void assert_memeq_fail(const char* file, int line, const char* msg,
                       const char* x, const char* y, size_t sz);


// panic(format, ...)
//    Print the message determined by `format` and fail.
[[noreturn, gnu::noinline, gnu::cold]]
void panic(const char* format, ...);

#endif /* !WEENSYOS_LIB_HH */
