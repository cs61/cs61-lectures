#ifndef WEENSYOS_CBYTESWAP_HH
#define WEENSYOS_CBYTESWAP_HH
#if defined(WEENSYOS_KERNEL) || defined(WEENSYOS_PROCESS)

// WeensyOS requires x86-64, which is little-endian.
template <typename T>
inline T to_le(T x) {
    return x;
}
template <typename T>
inline T from_le(T x) {
    return x;
}

#else
#include <sys/types.h>
#ifdef __APPLE__
# include <libkern/OSByteOrder.h>
# define htole16(x) OSSwapHostToLittleInt16((x))
# define htole32(x) OSSwapHostToLittleInt32((x))
# define htole64(x) OSSwapHostToLittleInt64((x))
# define le16toh(x) OSSwapLittleToHostInt16((x))
# define le32toh(x) OSSwapLittleToHostInt32((x))
# define le64toh(x) OSSwapLittleToHostInt64((x))
#else
# include <endian.h>
#endif

template <size_t n> struct sized_to_from_le;
template <> struct sized_to_from_le<1UL> {
    static inline uint8_t to_le(uint8_t x) { return x; }
    static inline uint8_t from_le(uint8_t x) { return x; }
};
template <> struct sized_to_from_le<2UL> {
    static inline uint16_t to_le(uint16_t x) { return htole16(x); }
    static inline uint16_t from_le(uint16_t x) { return le16toh(x); }
};
template <> struct sized_to_from_le<4UL> {
    static inline uint32_t to_le(uint32_t x) { return htole32(x); }
    static inline uint32_t from_le(uint32_t x) { return le32toh(x); }
};
template <> struct sized_to_from_le<8UL> {
    static inline uint64_t to_le(uint64_t x) { return htole64(x); }
    static inline uint64_t from_le(uint64_t x) { return le64toh(x); }
};

template <typename T>
inline T to_le(T x) {
    return sized_to_from_le<sizeof(x)>::to_le(x);
}
template <typename T>
inline T from_le(T x) {
    return sized_to_from_le<sizeof(x)>::from_le(x);
}

#endif
#endif
