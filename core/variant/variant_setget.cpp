/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_setget.cpp                                                    */
/**************************************************************************/

#include "core/variant/variant_setget.h"
#include "core/variant/variant_internal.h"

namespace Beyota {

Variant Variant::get_indexed(usize p_idx) const {
    return VariantSetGet::get_indexed(*this, p_idx);
}

bool Variant::set_indexed(usize p_idx, const Variant &p_val) {
    return VariantSetGet::set_indexed(*this, p_idx, p_val);
}

Variant Variant::get_named(const StringName &p_name) const {
    return VariantSetGet::get_named(*this, p_name);
}

bool Variant::set_named(const StringName &p_name, const Variant &p_val) {
    return VariantSetGet::set_named(*this, p_name, p_val);
}

Variant VariantSetGet::get_indexed(const Variant &p_variant, usize p_idx) {
    switch (p_variant.get_type()) {
        case Variant::VECTOR2: {
            if (p_idx >= 2) return Variant();
            const auto &v = VariantInternal::get_vector2(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::VECTOR2I: {
            if (p_idx >= 2) return Variant();
            const auto &v = VariantInternal::get_vector2i(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::VECTOR3: {
            if (p_idx >= 3) return Variant();
            const auto &v = VariantInternal::get_vector3(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::VECTOR3I: {
            if (p_idx >= 3) return Variant();
            const auto &v = VariantInternal::get_vector3i(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::VECTOR4: {
            if (p_idx >= 4) return Variant();
            const auto &v = VariantInternal::get_vector4(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::VECTOR4I: {
            if (p_idx >= 4) return Variant();
            const auto &v = VariantInternal::get_vector4i(&p_variant);
            return Variant(v[p_idx]);
        }
        case Variant::COLOR: {
            if (p_idx >= 4) return Variant();
            const auto &c = VariantInternal::get_color(&p_variant);
            return Variant(c[p_idx]);
        }
        case Variant::ARRAY: {
            const auto &arr = VariantInternal::get_array(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return arr[p_idx];
        }
        case Variant::PACKED_BYTE_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<u8>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(static_cast<i64>(arr[p_idx]));
        }
        case Variant::PACKED_INT32_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<i32>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(static_cast<i64>(arr[p_idx]));
        }
        case Variant::PACKED_INT64_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<i64>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_FLOAT32_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<f32>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(static_cast<f64>(arr[p_idx]));
        }
        case Variant::PACKED_FLOAT64_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<f64>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_STRING_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<String>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_VECTOR2_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<Vector2>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_VECTOR3_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<Vector3>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_COLOR_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<Color>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        case Variant::PACKED_VECTOR4_ARRAY: {
            const auto &arr = VariantInternal::get_packed_array<Vector4>(&p_variant);
            if (p_idx >= arr.size()) return Variant();
            return Variant(arr[p_idx]);
        }
        default:
            return Variant();
    }
}

bool VariantSetGet::set_indexed(Variant &r_variant, usize p_idx, const Variant &p_val) {
    switch (r_variant.get_type()) {
        case Variant::VECTOR2: {
            if (p_idx >= 2) return false;
            auto &v = VariantInternal::get_vector2(&r_variant);
            v[p_idx] = (real_t)p_val.operator f64();
            return true;
        }
        case Variant::VECTOR2I: {
            if (p_idx >= 2) return false;
            auto &v = VariantInternal::get_vector2i(&r_variant);
            v[p_idx] = (i32)p_val.operator i64();
            return true;
        }
        case Variant::VECTOR3: {
            if (p_idx >= 3) return false;
            auto &v = VariantInternal::get_vector3(&r_variant);
            v[p_idx] = (real_t)p_val.operator f64();
            return true;
        }
        case Variant::VECTOR3I: {
            if (p_idx >= 3) return false;
            auto &v = VariantInternal::get_vector3i(&r_variant);
            v[p_idx] = (i32)p_val.operator i64();
            return true;
        }
        case Variant::VECTOR4: {
            if (p_idx >= 4) return false;
            auto &v = VariantInternal::get_vector4(&r_variant);
            v[p_idx] = (real_t)p_val.operator f64();
            return true;
        }
        case Variant::VECTOR4I: {
            if (p_idx >= 4) return false;
            auto &v = VariantInternal::get_vector4i(&r_variant);
            v[p_idx] = (i32)p_val.operator i64();
            return true;
        }
        case Variant::COLOR: {
            if (p_idx >= 4) return false;
            auto &c = VariantInternal::get_color(&r_variant);
            c[p_idx] = (f32)p_val.operator f64();
            return true;
        }
        case Variant::ARRAY: {
            auto &arr = VariantInternal::get_array(&r_variant);
            if (p_idx >= arr.size()) return false;
            arr[p_idx] = p_val;
            return true;
        }
        default:
            return false;
    }
}

Variant VariantSetGet::get_named(const Variant &p_variant, const StringName &p_name) {
    std::string_view name = p_name.as_string_view();

    switch (p_variant.get_type()) {
        case Variant::VECTOR2: {
            const auto &v = VariantInternal::get_vector2(&p_variant);
            if (name == "x" || name == "width") return Variant(v.x);
            if (name == "y" || name == "height") return Variant(v.y);
            return Variant();
        }
        case Variant::VECTOR2I: {
            const auto &v = VariantInternal::get_vector2i(&p_variant);
            if (name == "x" || name == "width") return Variant(v.x);
            if (name == "y" || name == "height") return Variant(v.y);
            return Variant();
        }
        case Variant::RECT2: {
            const auto &r = VariantInternal::get_rect2(&p_variant);
            if (name == "position") return Variant(r.position);
            if (name == "size") return Variant(r.size);
            if (name == "end") return Variant(r.get_end());
            return Variant();
        }
        case Variant::RECT2I: {
            const auto &r = VariantInternal::get_rect2i(&p_variant);
            if (name == "position") return Variant(r.position);
            if (name == "size") return Variant(r.size);
            if (name == "end") return Variant(r.get_end());
            return Variant();
        }
        case Variant::VECTOR3: {
            const auto &v = VariantInternal::get_vector3(&p_variant);
            if (name == "x") return Variant(v.x);
            if (name == "y") return Variant(v.y);
            if (name == "z") return Variant(v.z);
            return Variant();
        }
        case Variant::VECTOR3I: {
            const auto &v = VariantInternal::get_vector3i(&p_variant);
            if (name == "x") return Variant(v.x);
            if (name == "y") return Variant(v.y);
            if (name == "z") return Variant(v.z);
            return Variant();
        }
        case Variant::VECTOR4: {
            const auto &v = VariantInternal::get_vector4(&p_variant);
            if (name == "x") return Variant(v.x);
            if (name == "y") return Variant(v.y);
            if (name == "z") return Variant(v.z);
            if (name == "w") return Variant(v.w);
            return Variant();
        }
        case Variant::VECTOR4I: {
            const auto &v = VariantInternal::get_vector4i(&p_variant);
            if (name == "x") return Variant(v.x);
            if (name == "y") return Variant(v.y);
            if (name == "z") return Variant(v.z);
            if (name == "w") return Variant(v.w);
            return Variant();
        }
        case Variant::PLANE: {
            const auto &p = VariantInternal::get_plane(&p_variant);
            if (name == "normal") return Variant(p.normal);
            if (name == "d") return Variant(p.d);
            if (name == "x") return Variant(p.normal.x);
            if (name == "y") return Variant(p.normal.y);
            if (name == "z") return Variant(p.normal.z);
            return Variant();
        }
        case Variant::QUATERNION: {
            const auto &q = VariantInternal::get_quaternion(&p_variant);
            if (name == "x") return Variant(q.x);
            if (name == "y") return Variant(q.y);
            if (name == "z") return Variant(q.z);
            if (name == "w") return Variant(q.w);
            return Variant();
        }
        case Variant::COLOR: {
            const auto &c = VariantInternal::get_color(&p_variant);
            if (name == "r") return Variant(c.r);
            if (name == "g") return Variant(c.g);
            if (name == "b") return Variant(c.b);
            if (name == "a") return Variant(c.a);
            if (name == "h") return Variant(c.get_h());
            if (name == "s") return Variant(c.get_s());
            if (name == "v") return Variant(c.get_v());
            return Variant();
        }
        case Variant::AABB: {
            const auto *a = VariantInternal::get_aabb(&p_variant);
            if (a == nullptr) return Variant();
            if (name == "position") return Variant(a->position);
            if (name == "size") return Variant(a->size);
            if (name == "end") return Variant(a->get_end());
            return Variant();
        }
        case Variant::TRANSFORM2D: {
            const auto *t = VariantInternal::get_transform2d(&p_variant);
            if (t == nullptr) return Variant();
            if (name == "x") return Variant(t->columns[0]);
            if (name == "y") return Variant(t->columns[1]);
            if (name == "origin") return Variant(t->columns[2]);
            return Variant();
        }
        case Variant::TRANSFORM3D: {
            const auto *t = VariantInternal::get_transform3d(&p_variant);
            if (t == nullptr) return Variant();
            if (name == "basis") return Variant(t->basis);
            if (name == "origin") return Variant(t->origin);
            return Variant();
        }
        case Variant::DICTIONARY: {
            const auto &d = VariantInternal::get_dictionary(&p_variant);
            return d.get(Variant(p_name), Variant());
        }
        default:
            return Variant();
    }
}

bool VariantSetGet::set_named(Variant &r_variant, const StringName &p_name, const Variant &p_val) {
    std::string_view name = p_name.as_string_view();

    switch (r_variant.get_type()) {
        case Variant::VECTOR2: {
            auto &v = VariantInternal::get_vector2(&r_variant);
            if (name == "x" || name == "width") { v.x = (real_t)p_val.operator f64(); return true; }
            if (name == "y" || name == "height") { v.y = (real_t)p_val.operator f64(); return true; }
            return false;
        }
        case Variant::VECTOR2I: {
            auto &v = VariantInternal::get_vector2i(&r_variant);
            if (name == "x" || name == "width") { v.x = (i32)p_val.operator i64(); return true; }
            if (name == "y" || name == "height") { v.y = (i32)p_val.operator i64(); return true; }
            return false;
        }
        case Variant::VECTOR3: {
            auto &v = VariantInternal::get_vector3(&r_variant);
            if (name == "x") { v.x = (real_t)p_val.operator f64(); return true; }
            if (name == "y") { v.y = (real_t)p_val.operator f64(); return true; }
            if (name == "z") { v.z = (real_t)p_val.operator f64(); return true; }
            return false;
        }
        case Variant::VECTOR3I: {
            auto &v = VariantInternal::get_vector3i(&r_variant);
            if (name == "x") { v.x = (i32)p_val.operator i64(); return true; }
            if (name == "y") { v.y = (i32)p_val.operator i64(); return true; }
            if (name == "z") { v.z = (i32)p_val.operator i64(); return true; }
            return false;
        }
        case Variant::COLOR: {
            auto &c = VariantInternal::get_color(&r_variant);
            if (name == "r") { c.r = (f32)p_val.operator f64(); return true; }
            if (name == "g") { c.g = (f32)p_val.operator f64(); return true; }
            if (name == "b") { c.b = (f32)p_val.operator f64(); return true; }
            if (name == "a") { c.a = (f32)p_val.operator f64(); return true; }
            return false;
        }
        case Variant::DICTIONARY: {
            auto &d = VariantInternal::get_dictionary(&r_variant);
            d[Variant(p_name)] = p_val;
            return true;
        }
        default:
            return false;
    }
}

} // namespace Beyota
