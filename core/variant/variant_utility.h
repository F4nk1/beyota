/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_utility.h                                                     */
/**************************************************************************/

#pragma once

#include "core/string/ustring.h"
#include "core/variant/variant.h"

namespace Beyota {

class VariantUtility {
public:
    [[nodiscard]] static Variant::Type type_of(const Variant &p_var) noexcept;
    [[nodiscard]] static String type_string(Variant::Type p_type);
    [[nodiscard]] static String str(const Variant &p_var);

    [[nodiscard]] static bool is_same(const Variant &p_a, const Variant &p_b) noexcept;
    [[nodiscard]] static bool is_instance_valid(const Variant &p_var) noexcept;
    [[nodiscard]] static bool is_equal_approx(const Variant &p_a, const Variant &p_b) noexcept;

    [[nodiscard]] static Variant lerp(const Variant &p_from, const Variant &p_to, f64 p_weight);
    [[nodiscard]] static Variant clamp(const Variant &p_val, const Variant &p_min, const Variant &p_max);
    [[nodiscard]] static Variant min(const Variant &p_a, const Variant &p_b);
    [[nodiscard]] static Variant max(const Variant &p_a, const Variant &p_b);

    [[nodiscard]] static Variant posmod(const Variant &p_x, const Variant &p_y);
    [[nodiscard]] static Variant snapped(const Variant &p_val, const Variant &p_step);
};

} // namespace Beyota
