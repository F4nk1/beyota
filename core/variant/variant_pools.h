/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_pools.h                                                       */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/type_primitives.h"

#include <cstddef>
#include <new>

namespace Beyota {

namespace VariantPools {

inline constexpr usize BUCKET_SMALL = 2 * 3 * sizeof(Math::real_t);  // 24 bytes (Transform2D, AABB)
inline constexpr usize BUCKET_MEDIUM = 4 * 3 * sizeof(Math::real_t); // 48 bytes (Basis, Transform3D)
inline constexpr usize BUCKET_LARGE = 4 * 4 * sizeof(Math::real_t);  // 64 bytes (Projection)

void *alloc_small();
void *alloc_medium();
void *alloc_large();

void free_small(void *p_ptr) noexcept;
void free_medium(void *p_ptr) noexcept;
void free_large(void *p_ptr) noexcept;

template <typename T>
[[nodiscard]] inline T *alloc() {
    if constexpr (sizeof(T) <= BUCKET_SMALL && alignof(Math::real_t) % alignof(T) == 0) {
        return static_cast<T *>(alloc_small());
    } else if constexpr (sizeof(T) <= BUCKET_MEDIUM && alignof(Math::real_t) % alignof(T) == 0) {
        return static_cast<T *>(alloc_medium());
    } else if constexpr (sizeof(T) <= BUCKET_LARGE && alignof(Math::real_t) % alignof(T) == 0) {
        return static_cast<T *>(alloc_large());
    } else {
        return static_cast<T *>(::operator new(sizeof(T)));
    }
}

template <typename T>
inline void free(T *p_ptr) noexcept {
    if (p_ptr == nullptr) return;
    if constexpr (sizeof(T) <= BUCKET_SMALL && alignof(Math::real_t) % alignof(T) == 0) {
        free_small(static_cast<void *>(p_ptr));
    } else if constexpr (sizeof(T) <= BUCKET_MEDIUM && alignof(Math::real_t) % alignof(T) == 0) {
        free_medium(static_cast<void *>(p_ptr));
    } else if constexpr (sizeof(T) <= BUCKET_LARGE && alignof(Math::real_t) % alignof(T) == 0) {
        free_large(static_cast<void *>(p_ptr));
    } else {
        ::operator delete(static_cast<void *>(p_ptr));
    }
}

} // namespace VariantPools

} // namespace Beyota
