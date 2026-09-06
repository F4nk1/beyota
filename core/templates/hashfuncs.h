/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  hashfuncs.h                                                           */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <bit>
#include <cstring>
#include <string_view>
#include <type_traits>

namespace Beyota {

[[nodiscard]] constexpr u64 hash_djb2_buffer(const void *p_buff, usize p_len, u64 p_prev = 5381) noexcept {
    const u8 *buff = static_cast<const u8 *>(p_buff);
    u64 hash = p_prev;
    for (usize i = 0; i < p_len; ++i) {
        hash = ((hash << 5) + hash) + buff[i];
    }
    return hash;
}

[[nodiscard]] constexpr u64 hash_fnv1a_64(const void *p_buff, usize p_len) noexcept {
    const u8 *ptr = static_cast<const u8 *>(p_buff);
    u64 hash = 0xcbf29ce484222325ULL;
    for (usize i = 0; i < p_len; ++i) {
        hash ^= static_cast<u64>(ptr[i]);
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

[[nodiscard]] constexpr u32 hash_fmix32(u32 h) noexcept {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

[[nodiscard]] constexpr u64 hash_fmix64(u64 k) noexcept {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return k;
}

[[nodiscard]] constexpr u32 hash_make_uint32_t(u64 p_int) noexcept {
    u64 v = hash_fmix64(p_int);
    return static_cast<u32>(v ^ (v >> 32));
}

[[nodiscard]] constexpr u32 hash_make_uint32_t(i64 p_int) noexcept {
    return hash_make_uint32_t(static_cast<u64>(p_int));
}

[[nodiscard]] constexpr u32 hash_make_uint32_t(u32 p_int) noexcept {
    return hash_fmix32(p_int);
}

[[nodiscard]] constexpr u32 hash_make_uint32_t(i32 p_int) noexcept {
    return hash_fmix32(static_cast<u32>(p_int));
}

[[nodiscard]] inline u32 hash_make_uint32_t(f64 p_float) noexcept {
    if (p_float == 0.0) {
        return 0;
    }
    u64 bits = std::bit_cast<u64>(p_float);
    return hash_make_uint32_t(bits);
}

[[nodiscard]] inline u32 hash_make_uint32_t(f32 p_float) noexcept {
    if (p_float == 0.0f) {
        return 0;
    }
    u32 bits = std::bit_cast<u32>(p_float);
    return hash_make_uint32_t(bits);
}

[[nodiscard]] constexpr u32 hash_make_uint32_t(std::string_view p_str) noexcept {
    u64 h = 5381;
    for (char c : p_str) {
        h = ((h << 5) + h) + static_cast<u8>(c);
    }
    return hash_make_uint32_t(h);
}

struct HashMapHasherDefault {
    template <typename T>
    [[nodiscard]] static u32 hash(const T &p_val) noexcept {
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return hash_make_uint32_t(p_val);
        } else if constexpr (std::is_pointer_v<T>) {
            return hash_make_uint32_t(reinterpret_cast<uptr>(p_val));
        } else if constexpr (requires { p_val.hash(); }) {
            return p_val.hash();
        } else {
            return hash_make_uint32_t(hash_djb2_buffer(&p_val, sizeof(T)));
        }
    }
};

} // namespace Beyota
