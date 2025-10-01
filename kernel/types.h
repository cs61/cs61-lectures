#ifndef WEENSYOS_TYPES_H
#define WEENSYOS_TYPES_H

// types.h
//
//    Types and macros useful in both kernel and applications.

#if __cplusplus
#define NULL nullptr
#else
#define NULL ((void*) 0)
#endif

typedef __builtin_va_list va_list;
#define va_start(val, last) __builtin_va_start(val, last)
#define va_arg(val, type) __builtin_va_arg(val, type)
#define va_end(val) __builtin_va_end(val)

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int64_t;
typedef unsigned long uint64_t;

typedef long intptr_t;                // ints big enough to hold pointers
typedef unsigned long uintptr_t;

#define SIZE_MAX  ((unsigned long) -1)
#define SSIZE_MAX ((long) 0x7FFFFFFFFFFFFFFF)
typedef unsigned long size_t;         // sizes and offsets
typedef long ssize_t;
typedef long off_t;

typedef int pid_t;                    // process IDs

#if __cplusplus
#define NO_COPY_OR_ASSIGN(t) \
    t(const t&) = delete; t(t&&) = delete; \
    t& operator=(const t&) = delete; \
    t& operator=(t&&) = delete;

template <typename T, typename U>
inline T* mem_container(U* ptr, U (T::* mem_ptr)) {
    alignas(T) char space[sizeof(T)] = {};
    T* dummy = reinterpret_cast<T*>(space);
    uintptr_t offset = reinterpret_cast<uintptr_t>(&(dummy->*mem_ptr))
        - reinterpret_cast<uintptr_t>(dummy);
    return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) - offset);
}
#endif

#define __section(x) __attribute__((section(x)))
#define __no_asan    __attribute__((no_sanitize_address))
#define __noinline   __attribute__((noinline))

#if __cplusplus
#define __always_inline inline __attribute__((always_inline))
#else
#define __always_inline static inline __attribute__((always_inline))
#endif

#if __cplusplus
#define __constexpr constexpr
#else
#define __constexpr
#endif

#endif /* !WEENSYOS_TYPES_H */
