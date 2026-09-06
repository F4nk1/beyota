/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_utility.cpp                                                   */
/**************************************************************************/

#include "core/variant/variant_utility.h"
#include "core/math/math_funcs.h"
#include "core/variant/variant_internal.h"

#include <algorithm>
#include <cmath>

namespace Beyota {

Variant::Type VariantUtility::type_of(const Variant &p_var) noexcept {
    return p_var.get_type();
}

String VariantUtility::type_string(Variant::Type p_type) {
    return String(variant_type_to_string(p_type));
}

String VariantUtility::str(const Variant &p_var) {
    return p_var.stringify();
}

bool VariantUtility::is_same(const Variant &p_a, const Variant &p_b) noexcept {
    if (p_a.get_type() != p_b.get_type()) {
        return false;
    }
    switch (p_a.get_type()) {
        case Variant::NIL:
            return true;
        case Variant::FLOAT: {
            f64 a = VariantInternal::get_float(&p_a);
            f64 b = VariantInternal::get_float(&p_b);
            if (std::isnan(a) && std::isnan(b)) return true;
            return a == b;
        }
        case Variant::OBJECT: {
            return VariantInternal::get_obj(&p_a).id == VariantInternal::get_obj(&p_b).id;
        }
        case Variant::ARRAY: {
            return &VariantInternal::get_array(&p_a) == &VariantInternal::get_array(&p_b);
        }
        case Variant::DICTIONARY: {
            return &VariantInternal::get_dictionary(&p_a) == &VariantInternal::get_dictionary(&p_b);
        }
        default:
            return p_a == p_b;
    }
}

bool VariantUtility::is_instance_valid(const Variant &p_var) noexcept {
    if (p_var.get_type() != Variant::OBJECT) {
        return false;
    }
    const auto &obj_data = VariantInternal::get_obj(&p_var);
    return obj_data.id.is_valid() && obj_data.obj != nullptr;
}

bool VariantUtility::is_equal_approx(const Variant &p_a, const Variant &p_b) noexcept {
    if (p_a.get_type() != p_b.get_type()) {
        if (p_a.is_num() && p_b.is_num()) {
            return Math::is_equal_approx(p_a.operator f64(), p_b.operator f64());
        }
        return false;
    }
    switch (p_a.get_type()) {
        case Variant::FLOAT:
            return Math::is_equal_approx(VariantInternal::get_float(&p_a), VariantInternal::get_float(&p_b));
        case Variant::VECTOR2: {
            const auto &a = VariantInternal::get_vector2(&p_a);
            const auto &b = VariantInternal::get_vector2(&p_b);
            return a.is_equal_approx(b);
        }
        case Variant::VECTOR3: {
            const auto &a = VariantInternal::get_vector3(&p_a);
            const auto &b = VariantInternal::get_vector3(&p_b);
            return a.is_equal_approx(b);
        }
        case Variant::VECTOR4: {
            const auto &a = VariantInternal::get_vector4(&p_a);
            const auto &b = VariantInternal::get_vector4(&p_b);
            return a.is_equal_approx(b);
        }
        case Variant::COLOR: {
            const auto &a = VariantInternal::get_color(&p_a);
            const auto &b = VariantInternal::get_color(&p_b);
            return a.is_equal_approx(b);
        }
        default:
            return p_a == p_b;
    }
}

Variant VariantUtility::lerp(const Variant &p_from, const Variant &p_to, f64 p_weight) {
    if (p_from.get_type() == Variant::FLOAT && p_to.get_type() == Variant::FLOAT) {
        return Variant(Math::lerp(VariantInternal::get_float(&p_from), VariantInternal::get_float(&p_to), p_weight));
    }
    if (p_from.get_type() == Variant::VECTOR2 && p_to.get_type() == Variant::VECTOR2) {
        return Variant(VariantInternal::get_vector2(&p_from).lerp(VariantInternal::get_vector2(&p_to), static_cast<real_t>(p_weight)));
    }
    if (p_from.get_type() == Variant::VECTOR3 && p_to.get_type() == Variant::VECTOR3) {
        return Variant(VariantInternal::get_vector3(&p_from).lerp(VariantInternal::get_vector3(&p_to), static_cast<real_t>(p_weight)));
    }
    if (p_from.get_type() == Variant::COLOR && p_to.get_type() == Variant::COLOR) {
        return Variant(VariantInternal::get_color(&p_from).lerp(VariantInternal::get_color(&p_to), static_cast<f32>(p_weight)));
    }
    return p_from;
}

Variant VariantUtility::clamp(const Variant &p_val, const Variant &p_min, const Variant &p_max) {
    if (p_val.get_type() == Variant::INT) {
        i64 v = VariantInternal::get_int(&p_val);
        i64 mn = p_min.operator i64();
        i64 mx = p_max.operator i64();
        return Variant(std::clamp(v, mn, mx));
    }
    if (p_val.is_num()) {
        f64 v = p_val.operator f64();
        f64 mn = p_min.operator f64();
        f64 mx = p_max.operator f64();
        return Variant(std::clamp(v, mn, mx));
    }
    if (p_val.get_type() == Variant::VECTOR2) {
        Vector2 v = VariantInternal::get_vector2(&p_val);
        Vector2 mn = p_min.operator Vector2();
        Vector2 mx = p_max.operator Vector2();
        return Variant(v.clamp(mn, mx));
    }
    if (p_val.get_type() == Variant::VECTOR3) {
        Vector3 v = VariantInternal::get_vector3(&p_val);
        Vector3 mn = p_min.operator Vector3();
        Vector3 mx = p_max.operator Vector3();
        return Variant(v.clamp(mn, mx));
    }
    return p_val;
}

Variant VariantUtility::min(const Variant &p_a, const Variant &p_b) {
    if (p_a < p_b) return p_a;
    return p_b;
}

Variant VariantUtility::max(const Variant &p_a, const Variant &p_b) {
    if (p_a > p_b) return p_a;
    return p_b;
}

Variant VariantUtility::posmod(const Variant &p_x, const Variant &p_y) {
    if (p_x.get_type() == Variant::INT && p_y.get_type() == Variant::INT) {
        return Variant(Math::posmod(VariantInternal::get_int(&p_x), VariantInternal::get_int(&p_y)));
    }
    if (p_x.is_num() && p_y.is_num()) {
        return Variant(Math::fposmod(p_x.operator f64(), p_y.operator f64()));
    }
    return Variant();
}

Variant VariantUtility::snapped(const Variant &p_val, const Variant &p_step) {
    if (p_val.is_num() && p_step.is_num()) {
        return Variant(Math::snapped(p_val.operator f64(), p_step.operator f64()));
    }
    if (p_val.get_type() == Variant::VECTOR2 && p_step.get_type() == Variant::VECTOR2) {
        return Variant(VariantInternal::get_vector2(&p_val).snapped(VariantInternal::get_vector2(&p_step)));
    }
    if (p_val.get_type() == Variant::VECTOR3 && p_step.get_type() == Variant::VECTOR3) {
        return Variant(VariantInternal::get_vector3(&p_val).snapped(VariantInternal::get_vector3(&p_step)));
    }
    return p_val;
}

} // namespace Beyota
