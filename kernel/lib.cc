#include "lib.hh"
#include "x86-64.h"
#include <atomic>
#if WEENSYOS_KERNEL
# include "kernel.hh"
#endif

// lib.cc
//
//    Functions useful in both kernel and applications.


extern "C" {

// memcpy, memmove, memset, memcmp, memchr, strlen, strnlen,
// strcpy, strncpy, strlcpy, strcmp, strncmp, strchr, strstr,
// strtoul, strtol
//    We must provide our own implementations.

void* memcpy(void* dst, const void* src, size_t n) {
    const char* s = reinterpret_cast<const char*>(src);
    char* d = reinterpret_cast<char*>(dst);
    for (; n > 0; --n, ++s, ++d) {
        *d = *s;
    }
    return dst;
}

void* memmove(void* dst, const void* src, size_t n) {
    const char* s = reinterpret_cast<const char*>(src);
    char* d = reinterpret_cast<char*>(dst);
    if (s < d && s + n > d) {
        s += n, d += n;
        while (n-- > 0) {
            *--d = *--s;
        }
    } else {
        while (n-- > 0) {
            *d++ = *s++;
        }
    }
    return dst;
}

void* memset(void* v, int c, size_t n) {
    char* p = reinterpret_cast<char*>(v);
    for (; n > 0; ++p, --n) {
        *p = c;
    }
    return v;
}

int memcmp(const void* a, const void* b, size_t n) {
    const uint8_t* sa = reinterpret_cast<const uint8_t*>(a);
    const uint8_t* sb = reinterpret_cast<const uint8_t*>(b);
    for (; n > 0; ++sa, ++sb, --n) {
        if (*sa != *sb) {
            return (*sa > *sb) - (*sa < *sb);
        }
    }
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* ss = reinterpret_cast<const unsigned char*>(s);
    for (; n != 0; ++ss, --n) {
        if (*ss == (unsigned char) c) {
            return (void*) ss;
        }
    }
    return nullptr;
}

size_t strlen(const char* s) {
    size_t n;
    for (n = 0; *s != '\0'; ++s) {
        ++n;
    }
    return n;
}

size_t strnlen(const char* s, size_t maxlen) {
    size_t n;
    for (n = 0; n != maxlen && *s != '\0'; ++s) {
        ++n;
    }
    return n;
}

char* strcpy(char* dst, const char* src) {
    char* d = dst;
    do {
        *d++ = *src++;
    } while (d[-1] != '\0');
    return dst;
}

char* strncpy(char* dst, const char* src, size_t maxlen) {
    char* d = dst;
    while (maxlen != 0 && *src != '\0') {
        *d++ = *src++;
        --maxlen;
    }
    while (maxlen != 0) {
        *d++ = '\0';
        --maxlen;
    }
    return dst;
}

size_t strlcpy(char* dst, const char* src, size_t maxlen) {
    size_t n = strlen(src);
    if (maxlen) {
        size_t nn = n > maxlen - 1 ? maxlen - 1 : n;
        memcpy(dst, src, nn);
        dst[nn] = '\0';
    }
    return n;
}

int strcmp(const char* a, const char* b) {
    while (true) {
        unsigned char ac = *a, bc = *b;
        if (ac == '\0' || bc == '\0' || ac != bc) {
            return (ac > bc) - (ac < bc);
        }
        ++a, ++b;
    }
}

int strncmp(const char* a, const char* b, size_t n) {
    while (true) {
        unsigned char ac = n ? *a : '\0', bc = n ? *b : '\0';
        if (ac == '\0' || bc == '\0' || ac != bc) {
            return (ac > bc) - (ac < bc);
        }
        ++a, ++b, --n;
    }
}

int strcasecmp(const char* a, const char* b) {
    while (true) {
        unsigned char ac = tolower((unsigned char) *a);
        unsigned char bc = tolower((unsigned char) *b);
        if (ac == '\0' || bc == '\0' || ac != bc) {
            return (ac > bc) - (ac < bc);
        }
        ++a, ++b;
    }
}

int strncasecmp(const char* a, const char* b, size_t n) {
    while (true) {
        unsigned char ac = n ? tolower((unsigned char) *a) : '\0';
        unsigned char bc = n ? tolower((unsigned char) *b) : '\0';
        if (ac == '\0' || bc == '\0' || ac != bc) {
            return (ac > bc) - (ac < bc);
        }
        ++a, ++b, --n;
    }
}

char* strchr(const char* s, int c) {
    while (*s != '\0' && *s != (char) c) {
        ++s;
    }
    if (*s == (char) c) {
        return (char*) s;
    } else {
        return nullptr;
    }
}

char* strstr(const char* hs, const char* ns) {
    size_t i = 0, j = 0;
    while (hs[i] != '\0' && ns[j] != '\0') {
        if (hs[i + j] == ns[j]) {
            ++j;
        } else {
            ++i;
            j = 0;
        }
    }
    if (ns[j] == '\0') {
        return const_cast<char*>(&hs[i]);
    } else {
        return nullptr;
    }
}

static inline void strtol_fix_base(const char*& t, int& base) {
    if (base == 0) {
        if (t[0] == '0') {
            if (t[1] == 'x' || t[1] == 'X') {
                base = 16;
                t += 2;
            } else if (t[1] == 'o' || t[1] == 'O') {
                base = 8;
                t += 2;
            } else if (t[1] == 'b' || t[1] == 'B') {
                base = 2;
                t += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && t[0] == '0' && (t[1] == 'x' || t[1] == 'X')) {
        t += 2;
    }
}

unsigned long strtoul(const char* s, char** endptr, int base) {
    const char* t = s;
    while (isspace(*t)) {
        ++t;
    }
    bool negative = *t == '-';
    t += negative || *t == '+';
    strtol_fix_base(t, base);
    unsigned long x = 0;
    auto [p, ec] = from_chars(t, nullptr, x, base);
    if (endptr) {
        *endptr = const_cast<char*>(ec == E_INVAL ? s : p);
    }
    if (ec == E_RANGE) {
        x = -1UL;
    }
    return negative ? -x : x;
}

long strtol(const char* s, char** endptr, int base) {
    const char* t = s;
    while (isspace(*t)) {
        ++t;
    }
    bool negative = *t == '-';
    t += negative || *t == '+';
    strtol_fix_base(t, base);
    unsigned long x = 0;
    auto [p, ec] = from_chars(t, nullptr, x, base);
    if (endptr) {
        *endptr = const_cast<char*>(ec == E_INVAL ? s : p);
    }
    unsigned long bound = (1UL << (8 * sizeof(unsigned long) - 1)) - !negative;
    if (ec == E_RANGE || x > bound) {
        x = bound;
    }
    return long(negative ? -x : x);
}

} // extern "C"

from_chars_result from_chars(const char* first, const char* last,
                             unsigned long& value, int base) {
    unsigned long x = 0;
    bool overflow = false;
    const char* s = first;
    while (s != last) {
        unsigned digit;
        if (*s >= '0' && *s < '0' + base) {
            digit = *s - '0';
        } else if (*s >= 'a' && *s < 'a' + base - 10) {
            digit = *s - 'a' + 10;
        } else if (*s >= 'A' && *s < 'A' + base - 10) {
            digit = *s - 'A' + 10;
        } else {
            break;
        }
        if (x > (-1UL - digit) / base) {
            overflow = true;
        } else {
            x = x * base + digit;
        }
        ++s;
    }
    if (s == first) {
        return from_chars_result{s, E_INVAL};
    } else if (overflow) {
        return from_chars_result{s, E_RANGE};
    } else {
        value = x;
        return from_chars_result{s, 0};
    }
}

from_chars_result from_chars(const char* first, const char* last,
                             long& value, int base) {
    bool negative = first != last && *first == '-';
    unsigned long x = 0;
    auto [p, ec] = from_chars(first + negative, last, x, base);
    if (ec == E_INVAL) {
        return from_chars_result{first, E_INVAL};
    } else if (ec == E_RANGE
               || x > (1UL << (8 * sizeof(unsigned long) - 1)) - !negative) {
        return from_chars_result{p, E_RANGE};
    } else {
        value = negative ? -x : x;
        return from_chars_result{p, 0};
    }
}

to_chars_result to_chars(char* first, char* last,
                         unsigned long value, int base) {
    // determine length of `value` in characters
    unsigned long v = value;
    unsigned b2 = unsigned(base) * unsigned(base),
        b3 = b2 * unsigned(base),
        n = 0;
    while (true) {
        if (v < unsigned(base)) {
            ++n;
        } else if (v < b2) {
            n += 2;
        } else {
            n += 3;
        }
        if (v < b3) {
            break;
        }
        v /= b3;
    }
    if (last - first < n) {
        return to_chars_result{last, E_OVERFLOW};
    }

    // unparse `value` right to left
    char* s = first + n;
    while (s > first) {
        --s;
        unsigned digit = value % base;
        value /= base;
        *s = digit < 10 ? '0' + digit : 'a' + digit - 10;
    }
    return to_chars_result{first + n, 0};
}

to_chars_result to_chars(char* first, char* last,
                         long value, int base) {
    if (first == last) {
        return to_chars_result{last, E_OVERFLOW};
    } else if (value < 0) {
        *first = '-';
        return to_chars(first + 1, last, -(unsigned long) value, base);
    } else {
        return to_chars(first, last, (unsigned long) value, base);
    }
}


// pseudorandom number generators

static std::atomic<int> rand_seed_set;
static std::atomic<unsigned long> rand_seed;

int rand() {
    if (!rand_seed_set) {
        srand(819234718U);
    }
    unsigned long rs = rand_seed, next_rs;
    do {
        next_rs = rs * 6364136223846793005UL + 1;
    } while (!rand_seed.compare_exchange_weak(rs, next_rs));
    return (next_rs >> 33) & RAND_MAX;
}

void srand(unsigned seed) {
    rand_seed = ((unsigned long) seed << 32) | seed;
    rand_seed_set = 1;
}

namespace {
template <typename T, typename F>
T uniform_int_distribution(T min, T max, F&& engine) {
    assert(min <= max);
    assert(max - min <= RAND_MAX);
    unsigned long amount = max - min + 1;
    unsigned long per = (unsigned(RAND_MAX) + 1) / amount;
    unsigned long bound = per * amount;
    unsigned long r;
    do {
        r = engine();
    } while (r >= bound);
    return min + r / per;
}
}

int rand(int min, int max) {
    return uniform_int_distribution(min, max, (int (*)()) rand);
}

unsigned rand_engine::operator()() {
    seed_ = seed_ * 6364136223846793005UL + 1;
    return (seed_ >> 33) & RAND_MAX;
}

unsigned rand_engine::operator()(unsigned min, unsigned max) {
    return uniform_int_distribution(min, max, *this);
}

int rand_engine::operator()(int min, int max) {
    return uniform_int_distribution(min, max, *this);
}


// declare printfmt specializations (g++-6 requires)

constexpr char printfmt<bool>::spec[];
constexpr char printfmt<char>::spec[];
constexpr char printfmt<signed char>::spec[];
constexpr char printfmt<unsigned char>::spec[];
constexpr char printfmt<short>::spec[];
constexpr char printfmt<unsigned short>::spec[];
constexpr char printfmt<int>::spec[];
constexpr char printfmt<unsigned>::spec[];
constexpr char printfmt<long>::spec[];
constexpr char printfmt<unsigned long>::spec[];


// printer::vprintf
//    Format and print a string into a generic printer object.

#define FLAG_ALT                (1<<0)
#define FLAG_ZERO               (1<<1)
#define FLAG_LEFTJUSTIFY        (1<<2)
#define FLAG_SPACEPOSITIVE      (1<<3)
#define FLAG_PLUSPOSITIVE       (1<<4)
#define FLAG_THOUSANDS          (1<<5)
static const char flag_chars[] = "#0- +'";

#define FLAG_NUMERIC            (1<<6)
#define FLAG_SIGNED             (1<<7)
#define FLAG_NEGATIVE           (1<<8)
#define FLAG_ALT2               (1<<9)

static char* print_number(char* buf, size_t sz,
                          unsigned long val, int base, int flags) {
    static const char upper_digits[] = "0123456789ABCDEF";
    static const char lower_digits[] = "0123456789abcdef";

    const char* digits = upper_digits;
    if (base < 0) {
        digits = lower_digits;
        base = -base;
    }

    char* pos = buf + sz;
    *--pos = '\0';
    char* thousands_pos;
    if (flags & FLAG_THOUSANDS) {
        thousands_pos = pos - (base == 10 ? 3 : 4);
    } else {
        thousands_pos = nullptr;
    }
    do {
        if (pos == thousands_pos) {
            *--pos = base == 10 ? ',' : '\'';
            thousands_pos = pos - (base == 10 ? 3 : 4);
        } else {
            *--pos = digits[val % base];
            val /= base;
        }
    } while (val != 0 && pos != buf);
    return pos;
}

void printer::printf(const char* format, ...) {
    va_list val;
    va_start(val, format);
    vprintf(format, val);
    va_end(val);
}

void printer::vprintf(const char* format, va_list val) {
#define NUMBUFSIZ 32
    char numbuf[NUMBUFSIZ];

    for (; *format; ++format) {
        if (*format != '%') {
            putc(*format);
            continue;
        }

        // process flags
        int flags = 0;
        for (++format; *format; ++format) {
            const char* flagc = strchr(flag_chars, *format);
            if (flagc) {
                flags |= 1 << (flagc - flag_chars);
            } else {
                break;
            }
        }

        // process width
        int width = -1;
        if (*format >= '1' && *format <= '9') {
            for (width = 0; *format >= '0' && *format <= '9'; ) {
                width = 10 * width + *format++ - '0';
            }
        } else if (*format == '*') {
            width = va_arg(val, int);
            ++format;
        }

        // process precision
        int precision = -1;
        if (*format == '.') {
            ++format;
            if (*format >= '0' && *format <= '9') {
                for (precision = 0; *format >= '0' && *format <= '9'; ) {
                    precision = 10 * precision + *format++ - '0';
                }
            } else if (*format == '*') {
                precision = va_arg(val, int);
                ++format;
            }
            if (precision < 0) {
                precision = 0;
            }
        }

        // process length
        int length = 0;
        switch (*format) {
        case 'l':
        case 't': // ptrdiff_t
        case 'z': // size_t, ssize_t
            length = 1;
            ++format;
            break;
        case 'h':
            ++format;
            break;
        }

        // process main conversion character
        int base = 10;
        unsigned long num = 0;
        const char* data = "";

        switch (*format) {
        case 'd':
        case 'i': {
            long x = length ? va_arg(val, long) : va_arg(val, int);
            int negative = x < 0 ? FLAG_NEGATIVE : 0;
            num = negative ? -x : x;
            flags |= FLAG_NUMERIC | FLAG_SIGNED | negative;
            break;
        }
        case 'u':
        format_unsigned:
            num = length ? va_arg(val, unsigned long) : va_arg(val, unsigned);
            flags |= FLAG_NUMERIC;
            break;
        case 'x':
            base = -16;
            goto format_unsigned;
        case 'X':
            base = 16;
            goto format_unsigned;
        case 'p':
            num = va_arg(val, uintptr_t);
            base = -16;
            flags |= FLAG_ALT | FLAG_ALT2 | FLAG_NUMERIC;
            break;
        case 's':
            data = va_arg(val, char*);
            break;
        case 'C':
            color_ = va_arg(val, int);
            continue;
        case 'c':
            data = numbuf;
            numbuf[0] = va_arg(val, int);
            numbuf[1] = '\0';
            break;
        default:
            data = numbuf;
            numbuf[0] = (*format ? *format : '%');
            numbuf[1] = '\0';
            if (!*format) {
                format--;
            }
            break;
        }

        if (flags & FLAG_NUMERIC) {
            data = print_number(numbuf, NUMBUFSIZ, num, base, flags);
        }

        const char* prefix = "";
        if ((flags & FLAG_NUMERIC) && (flags & FLAG_SIGNED)) {
            if (flags & FLAG_NEGATIVE) {
                prefix = "-";
            } else if (flags & FLAG_PLUSPOSITIVE) {
                prefix = "+";
            } else if (flags & FLAG_SPACEPOSITIVE) {
                prefix = " ";
            }
        } else if ((flags & FLAG_NUMERIC) && (flags & FLAG_ALT)
                   && (base == 16 || base == -16)
                   && (num || (flags & FLAG_ALT2))) {
            prefix = (base == -16 ? "0x" : "0X");
        }

        int datalen;
        if (precision >= 0 && !(flags & FLAG_NUMERIC)) {
            datalen = strnlen(data, precision);
        } else {
            datalen = strlen(data);
        }

        int zeros;
        if ((flags & FLAG_NUMERIC)
            && precision >= 0) {
            zeros = precision > datalen ? precision - datalen : 0;
        } else if ((flags & FLAG_NUMERIC)
                   && (flags & FLAG_ZERO)
                   && !(flags & FLAG_LEFTJUSTIFY)
                   && datalen + (int) strlen(prefix) < width) {
            zeros = width - datalen - strlen(prefix);
        } else {
            zeros = 0;
        }

        width -= datalen + zeros + strlen(prefix);
        for (; !(flags & FLAG_LEFTJUSTIFY) && width > 0; --width) {
            putc(' ');
        }
        for (; *prefix; ++prefix) {
            putc(*prefix);
        }
        for (; zeros > 0; --zeros) {
            putc('0');
        }
        for (; datalen > 0; ++data, --datalen) {
            putc(*data);
        }
        for (; width > 0; --width) {
            putc(' ');
        }
    }
}


// vsnprintf, snprintf
//    Format a string into a sized buffer.

struct string_printer : public printer {
    char* s_;
    char* end_;
    size_t n_;
    string_printer(char* s, size_t size)
        : s_(s), end_(s + size), n_(0) {
    }
    void putc(unsigned char c) override {
        if (s_ < end_) {
            *s_++ = c;
        }
        ++n_;
    }
};

ssize_t vsnprintf(char* s, size_t size, const char* format, va_list val) {
    string_printer sp(s, size);
    sp.vprintf(format, val);
    if (size && sp.s_ < sp.end_) {
        *sp.s_ = 0;
    } else if (size) {
        sp.end_[-1] = 0;
    }
    return sp.n_;
}

ssize_t snprintf(char* s, size_t size, const char* format, ...) {
    va_list val;
    va_start(val, format);
    int n = vsnprintf(s, size, format, val);
    va_end(val);
    return n;
}


// console_clear
//    Erases the console and moves the cursor to the upper left (CPOS(0, 0)).

void console_clear() {
    for (int i = 0; i < CONSOLE_ROWS * CONSOLE_COLUMNS; ++i) {
        console[i] = ' ' | 0x0700;
    }
    cursorpos = 0;
#if WEENSYOS_KERNEL
    console_show_cursor();
#endif
}


// console_printer
//    A `printer` for writing strings to the CGA console.
//    Understands limited ANSI terminal escape sequences.

console_printer::console_printer(int cpos, int scroll_mode)
    : cell_(console), scroll_mode_(scroll_mode) {
    if (cpos < 0) {
        cell_ += cursorpos;
    } else if (cpos <= CONSOLE_ROWS * CONSOLE_COLUMNS) {
        cell_ += cpos;
    }
}

static bool console_line_is_blank(int row) {
    int cpos = row * CONSOLE_COLUMNS, ecpos = cpos + CONSOLE_COLUMNS;
    while (cpos < ecpos && (console[cpos] & 0xF0DF) == 0x0000) {
        ++cpos;
    }
    return cpos == ecpos;
}

[[gnu::noinline]]
void console_printer::scroll() {
    assert(cell_ >= console + END_CPOS);
    if (scroll_mode_ == scroll_off) {
        cell_ = console;
        return;
    }
    if (scroll_mode_ == scroll_blank && scroll_blank_ < 0) {
        int row = CONSOLE_ROWS - 1;
        while (row > 0 && !console_line_is_blank(row - 1)) {
            --row;
        }
        while (row > 0 && console_line_is_blank(row - 1)) {
            --row;
        }
        scroll_blank_ = row;
    } else if (scroll_blank_ > 0 && !console_line_is_blank(scroll_blank_)) {
        scroll_blank_ = 0;
    }
    int sp = scroll_blank_ > 0 ? scroll_blank_ * CONSOLE_COLUMNS : 0;
    for (int i = sp; i != END_CPOS - CONSOLE_COLUMNS; ++i) {
        console[i] = console[i + CONSOLE_COLUMNS];
    }
    for (int i = END_CPOS - CONSOLE_COLUMNS; i != END_CPOS; ++i) {
        console[i] = 0;
    }
    cell_ -= CONSOLE_COLUMNS;
}

void console_printer::move_cursor() {
    ebuf_.flush(*this);
    cursorpos = cell_ - console;
#if CHICKADEE_KERNEL
    console_show_cursor();
#endif
}

[[gnu::noinline]]
void ansi_escape_buffer::flush(printer& pr) {
    int len = len_;
    if (len <= 0) {
        return;
    }
    len_ = -1;
    for (int i = 0; i != len; ++i) {
        pr.putc(buf_[i]);
    }
    len_ = 0;
}

[[gnu::noinline]]
void ansi_escape_buffer::putc_impl(unsigned char c, printer& pr) {
    const unsigned char colormap[] = {0, 4, 2, 6, 1, 5, 3, 7};

    // collect prefix of an `ESC [ ... m` sequence in ebuf_
    buf_[len_] = c;
    ++len_;
    if (len_ == 1
        || (len_ == 2 && c == '[')
        || (len_ > 2 && len_ < (int) sizeof(buf_) && (isdigit(c) || c == ';'))) {
        return;
    }

    // not `ESC [ ... m`: output as normal
    if (c != 'm') {
        flush(pr);
        return;
    }

    // full `ESC [ ... m`: parse into color_ setting
    int x = 0;
    pr.color_ = COLOR_GRAY;
    for (int i = 2; i != len_; ++i) {
        if (isdigit(buf_[i])) {
            x = x * 10 + buf_[i] - '0';
        } else {
            if (x == 0) {
                pr.color_ = COLOR_GRAY;
            } else if (x == 1) {
                pr.color_ |= 0x0800;
            } else if (x == 2) {
                pr.color_ &= ~0x0800;
            } else if (x == 7) {
                pr.color_ = ((pr.color_ >> 4) | (pr.color_ << 4)) & 0xFF00;
            } else if (x >= 30 && x <= 37) {
                pr.color_ = (pr.color_ & 0xF000) | (colormap[x - 30] << 8);
            } else if (x >= 40 && x <= 47) {
                pr.color_ = (pr.color_ & 0x0F00) | (colormap[x - 40] << 12);
            } else if (x >= 90 && x <= 97) {
                pr.color_ = (pr.color_ & 0xF000) | (colormap[x - 90] << 8) | 0x0800;
            } else if (x >= 100 && x <= 107) {
                pr.color_ = (pr.color_ & 0x0F00) | (colormap[x - 100] << 12) | 0x8000;
            }
            x = 0;
        }
    }
    len_ = 0;
}

void console_printer::putc(unsigned char c) {
    if (ebuf_.putc(c, *this)) {
        return;
    }
    while (cell_ >= console + END_CPOS) {
        scroll();
    }
    if (c == '\n') {
        int pos = (cell_ - console) % 80;
        while (pos != 80) {
            *cell_++ = ' ' | color_;
            ++pos;
        }
    } else {
        *cell_++ = c | color_;
    }
}

[[gnu::noinline]]
int console_puts(int cpos, int color, const char* s, size_t len) {
    console_printer cp(cpos, cpos < 0);
    cp.color_ = color;
    while (len > 0) {
        cp.putc(*s);
        ++s;
        --len;
    }
    if (cpos < 0) {
        cp.move_cursor();
    }
    return cp.cell_ - console;
}


// console_vprintf, console_printf
//    Print a message onto the console, starting at the given cursor position.

[[gnu::noinline]]
int console_vprintf(int cpos, const char* format, va_list val) {
    console_printer cp(cpos, cpos < 0);
    cp.vprintf(format, val);
    if (cpos < 0) {
        cp.move_cursor();
    }
    return cp.cell_ - console;
}

[[gnu::noinline]]
int console_printf(int cpos, const char* format, ...) {
    va_list val;
    va_start(val, format);
    cpos = console_vprintf(cpos, format, val);
    va_end(val);
    return cpos;
}

[[gnu::noinline]]
void console_printf(const char* format, ...) {
    va_list val;
    va_start(val, format);
    console_vprintf(-1, format, val);
    va_end(val);
}


// k-hardware.cc/u-lib.cc supply error_vprintf

[[gnu::noinline]]
void error_printf(const char* format, ...) {
    va_list val;
    va_start(val, format);
    error_vprintf(format, val);
    va_end(val);
}


// assertion support

void assert_memeq_fail(const char* file, int line, const char* msg,
                       const char* x, const char* y, size_t sz) {
    size_t pos = 0;
    while (pos < sz && x[pos] == y[pos]) {
        ++pos;
    }
    size_t spos = pos <= 10 ? 0 : pos - 10;
    size_t epos = pos + 10 < sz ? pos + 10 : sz;
    const char* ellipsis1 = spos > 0 ? "..." : "";
    const char* ellipsis2 = epos < sz ? "..." : "";
    error_printf("%s:%d: \"%s%.*s%s\" != \"%s%.*s%s\" @%zu\n",
                 file, line,
                 ellipsis1, int(epos - spos), x + spos, ellipsis2,
                 ellipsis1, int(epos - spos), y + spos, ellipsis2, pos);
    assert_fail(file, line, msg);
}


// Some static tests of our arithmetic functions

static_assert(msb(0) == 0, "msb failure");
static_assert(msb(1) == 1, "msb failure");
static_assert(msb(2) == 2, "msb failure");
static_assert(msb(3) == 2, "msb failure");
static_assert(msb(0x1FABC) == 17, "msb failure");
static_assert(msb(0x1FFFF) == 17, "msb failure");

static_assert(round_down_pow2(0U) == 0U, "round_down_pow2 failure");
static_assert(round_down_pow2(1U) == 1U, "round_down_pow2 failure");
static_assert(round_down_pow2(2U) == 2U, "round_down_pow2 failure");
static_assert(round_down_pow2(3U) == 2U, "round_down_pow2 failure");
static_assert(round_down_pow2(0x1FABCU) == 0x10000U, "round_down_pow2 failure");

static_assert(round_up_pow2(0U) == 0U, "round_up_pow2 failure");
static_assert(round_up_pow2(1U) == 1U, "round_up_pow2 failure");
static_assert(round_up_pow2(2U) == 2U, "round_up_pow2 failure");
static_assert(round_up_pow2(3U) == 4U, "round_up_pow2 failure");
static_assert(round_up_pow2(0x1FABCU) == 0x20000U, "round_up_pow2 failure");
static_assert(round_up_pow2(0x1FFFFU) == 0x20000U, "round_up_pow2 failure");
