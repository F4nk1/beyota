/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  rid.h                                                                 */
/**************************************************************************/

#pragma once

#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <compare>

namespace Beyota {

class RID {
    u64 id_{0};

public:
    constexpr RID() noexcept = default;
    explicit constexpr RID(u64 p_id) noexcept : id_(p_id) {}

    [[nodiscard]] static constexpr RID from_uint64(u64 p_id) noexcept {
        return RID(p_id);
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept { return id_ != 0; }
    [[nodiscard]] constexpr bool is_null() const noexcept { return id_ == 0; }
    [[nodiscard]] constexpr u64 get_id() const noexcept { return id_; }
    [[nodiscard]] constexpr u32 get_local_index() const noexcept {
        return static_cast<u32>(id_ & 0xFFFFFFFF);
    }

    constexpr auto operator<=>(const RID &) const noexcept = default;

    [[nodiscard]] constexpr u32 hash() const noexcept {
        return hash_make_uint32_t(id_);
    }
};

} // namespace Beyota
