#pragma once
#include <cstdint>
#include <cstring>

namespace mem {

inline bool valid(uintptr_t addr) {
    return addr > 0x10000 && addr < 0x0000FFFFFFFFFFFFull;
}

template <typename T>
inline T read(uintptr_t addr) {
    T out{};
    if (!valid(addr)) return out;
    std::memcpy(&out, reinterpret_cast<const void*>(addr), sizeof(T));
    return out;
}

template <typename T>
inline bool write(uintptr_t addr, const T& v) {
    if (!valid(addr)) return false;
    std::memcpy(reinterpret_cast<void*>(addr), &v, sizeof(T));
    return true;
}

void* read_ptr(uintptr_t addr);

} // namespace mem
