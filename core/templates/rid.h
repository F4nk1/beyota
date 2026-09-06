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
    u64 _id{0};

public:
    constexpr RID() noexcept = default;
    explicit constexpr RID(u64 p_id) noexcept : _id(p_id) {}

    [[nodiscard]] constexpr bool is_valid() const noexcept { return _id != 0; }
    [[nodiscard]] constexpr bool is_null() const noexcept { return _id == 0; }
    [[nodiscard]] constexpr u64 get_id() const noexcept { return _id; }

    constexpr auto operator<=>(const RID &) const noexcept = default;

    [[nodiscard]] constexpr u32 hash() const noexcept {
        return hash_make_uint32_t(_id);
    }
};

} // namespace Beyota
