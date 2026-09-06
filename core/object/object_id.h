/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  object_id.h                                                           */
/**************************************************************************/

#pragma once

#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <compare>
#include <type_traits>

namespace Beyota {

class ObjectID {
public:
    static constexpr u64 REF_COUNTED_FLAG = 1ULL << 63;

private:
    u64 id_{0};

public:
    constexpr ObjectID() noexcept = default;

    template <typename T>
        requires std::is_integral_v<T>
    constexpr ObjectID(T p_id) noexcept : id_((u64)p_id) {}

    [[nodiscard]] constexpr bool is_ref_counted() const noexcept {
        return (id_ & REF_COUNTED_FLAG) != 0;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return id_ != 0;
    }

    [[nodiscard]] constexpr bool is_null() const noexcept {
        return id_ == 0;
    }

    [[nodiscard]] constexpr explicit operator u64() const noexcept {
        return id_;
    }

    [[nodiscard]] constexpr explicit operator i64() const noexcept {
        return (i64)id_;
    }

    constexpr auto operator<=>(const ObjectID &) const noexcept = default;

    [[nodiscard]] constexpr u32 hash() const noexcept {
        return hash_make_uint32_t(id_);
    }
};

} // namespace Beyota
