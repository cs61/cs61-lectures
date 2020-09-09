#ifndef CS61_MEMBENCH_HH
#define CS61_MEMBENCH_HH
#include "hexdump.hh"
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

unsigned long memnode_benchmark(unsigned noperations, unsigned step);


/// This magic class lets standard C++ containers use the system allocator,
/// instead of the debugging allocator.
template <typename T>
class system_allocator {
public:
    typedef T value_type;
    system_allocator() noexcept = default;
    system_allocator(const system_allocator<T>&) noexcept = default;
    template <typename U> system_allocator(system_allocator<U>&) noexcept {}

    T* allocate(size_t n) {
        return reinterpret_cast<T*>((malloc)(n * sizeof(T)));
    }
    void deallocate(T* ptr, size_t) {
        (free)(ptr);
    }
};
template <typename T, typename U>
inline constexpr bool operator==(const system_allocator<T>&, const system_allocator<U>&) {
    return true;
}
template <typename T, typename U>
inline constexpr bool operator!=(const system_allocator<T>&, const system_allocator<U>&) {
    return false;
}

#define COMPLETE_ALLOCATION_OPERATORS \
    void* operator new[](size_t sz) { return ::operator new(sz); } \
    void operator delete(void* ptr, size_t) noexcept { return ::operator delete(ptr); } \
    void operator delete[](void* ptr) noexcept { return ::operator delete(ptr); } \
    void operator delete[](void* ptr, size_t) noexcept { return ::operator delete(ptr); }

#endif
