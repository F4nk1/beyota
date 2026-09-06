/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_internal.h                                                    */
/**************************************************************************/

#pragma once

#include "core/variant/variant.h"

namespace Beyota {

class VariantInternal {
public:
    static void set_type(Variant &r_variant, Variant::Type p_type) noexcept {
        r_variant.type = p_type;
    }

    // Atomic types
    [[nodiscard]] static bool &get_bool(Variant *p_v) noexcept { return p_v->_data._bool; }
    [[nodiscard]] static const bool &get_bool(const Variant *p_v) noexcept { return p_v->_data._bool; }

    [[nodiscard]] static i64 &get_int(Variant *p_v) noexcept { return p_v->_data._int; }
    [[nodiscard]] static const i64 &get_int(const Variant *p_v) noexcept { return p_v->_data._int; }

    [[nodiscard]] static f64 &get_float(Variant *p_v) noexcept { return p_v->_data._float; }
    [[nodiscard]] static const f64 &get_float(const Variant *p_v) noexcept { return p_v->_data._float; }

    [[nodiscard]] static String &get_string(Variant *p_v) noexcept {
        return *reinterpret_cast<String *>(p_v->_data._mem);
    }
    [[nodiscard]] static const String &get_string(const Variant *p_v) noexcept {
        return *reinterpret_cast<const String *>(p_v->_data._mem);
    }

    // Math types in _mem
    [[nodiscard]] static Vector2 &get_vector2(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector2 *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector2 &get_vector2(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector2 *>(p_v->_data._mem);
    }

    [[nodiscard]] static Vector2i &get_vector2i(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector2i *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector2i &get_vector2i(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector2i *>(p_v->_data._mem);
    }

    [[nodiscard]] static Rect2 &get_rect2(Variant *p_v) noexcept {
        return *reinterpret_cast<Rect2 *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Rect2 &get_rect2(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Rect2 *>(p_v->_data._mem);
    }

    [[nodiscard]] static Rect2i &get_rect2i(Variant *p_v) noexcept {
        return *reinterpret_cast<Rect2i *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Rect2i &get_rect2i(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Rect2i *>(p_v->_data._mem);
    }

    [[nodiscard]] static Vector3 &get_vector3(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector3 *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector3 &get_vector3(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector3 *>(p_v->_data._mem);
    }

    [[nodiscard]] static Vector3i &get_vector3i(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector3i *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector3i &get_vector3i(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector3i *>(p_v->_data._mem);
    }

    [[nodiscard]] static Vector4 &get_vector4(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector4 *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector4 &get_vector4(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector4 *>(p_v->_data._mem);
    }

    [[nodiscard]] static Vector4i &get_vector4i(Variant *p_v) noexcept {
        return *reinterpret_cast<Vector4i *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Vector4i &get_vector4i(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Vector4i *>(p_v->_data._mem);
    }

    [[nodiscard]] static Plane &get_plane(Variant *p_v) noexcept {
        return *reinterpret_cast<Plane *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Plane &get_plane(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Plane *>(p_v->_data._mem);
    }

    [[nodiscard]] static Quaternion &get_quaternion(Variant *p_v) noexcept {
        return *reinterpret_cast<Quaternion *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Quaternion &get_quaternion(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Quaternion *>(p_v->_data._mem);
    }

    [[nodiscard]] static Color &get_color(Variant *p_v) noexcept {
        return *reinterpret_cast<Color *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Color &get_color(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Color *>(p_v->_data._mem);
    }

    // Pooled pointer types
    [[nodiscard]] static Transform2D *get_transform2d(Variant *p_v) noexcept { return p_v->_data._transform2d; }
    [[nodiscard]] static const Transform2D *get_transform2d(const Variant *p_v) noexcept { return p_v->_data._transform2d; }

    [[nodiscard]] static Math::AABB *get_aabb(Variant *p_v) noexcept { return p_v->_data._aabb; }
    [[nodiscard]] static const Math::AABB *get_aabb(const Variant *p_v) noexcept { return p_v->_data._aabb; }

    [[nodiscard]] static Basis *get_basis(Variant *p_v) noexcept { return p_v->_data._basis; }
    [[nodiscard]] static const Basis *get_basis(const Variant *p_v) noexcept { return p_v->_data._basis; }

    [[nodiscard]] static Transform3D *get_transform3d(Variant *p_v) noexcept { return p_v->_data._transform3d; }
    [[nodiscard]] static const Transform3D *get_transform3d(const Variant *p_v) noexcept { return p_v->_data._transform3d; }

    [[nodiscard]] static Projection *get_projection(Variant *p_v) noexcept { return p_v->_data._projection; }
    [[nodiscard]] static const Projection *get_projection(const Variant *p_v) noexcept { return p_v->_data._projection; }

    // Misc types in _mem
    [[nodiscard]] static StringName &get_string_name(Variant *p_v) noexcept {
        return *reinterpret_cast<StringName *>(p_v->_data._mem);
    }
    [[nodiscard]] static const StringName &get_string_name(const Variant *p_v) noexcept {
        return *reinterpret_cast<const StringName *>(p_v->_data._mem);
    }

    [[nodiscard]] static NodePath &get_node_path(Variant *p_v) noexcept {
        return *reinterpret_cast<NodePath *>(p_v->_data._mem);
    }
    [[nodiscard]] static const NodePath &get_node_path(const Variant *p_v) noexcept {
        return *reinterpret_cast<const NodePath *>(p_v->_data._mem);
    }

    [[nodiscard]] static RID &get_rid(Variant *p_v) noexcept {
        return *reinterpret_cast<RID *>(p_v->_data._mem);
    }
    [[nodiscard]] static const RID &get_rid(const Variant *p_v) noexcept {
        return *reinterpret_cast<const RID *>(p_v->_data._mem);
    }

    [[nodiscard]] static Variant::ObjData &get_obj(Variant *p_v) noexcept {
        return *reinterpret_cast<Variant::ObjData *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Variant::ObjData &get_obj(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Variant::ObjData *>(p_v->_data._mem);
    }

    [[nodiscard]] static Callable &get_callable(Variant *p_v) noexcept {
        return *reinterpret_cast<Callable *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Callable &get_callable(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Callable *>(p_v->_data._mem);
    }

    [[nodiscard]] static Signal &get_signal(Variant *p_v) noexcept {
        return *reinterpret_cast<Signal *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Signal &get_signal(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Signal *>(p_v->_data._mem);
    }

    [[nodiscard]] static Dictionary &get_dictionary(Variant *p_v) noexcept {
        return *reinterpret_cast<Dictionary *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Dictionary &get_dictionary(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Dictionary *>(p_v->_data._mem);
    }

    [[nodiscard]] static Array &get_array(Variant *p_v) noexcept {
        return *reinterpret_cast<Array *>(p_v->_data._mem);
    }
    [[nodiscard]] static const Array &get_array(const Variant *p_v) noexcept {
        return *reinterpret_cast<const Array *>(p_v->_data._mem);
    }

    // Packed arrays
    template <typename T>
    [[nodiscard]] static PackedArray<T> &get_packed_array(Variant *p_v) noexcept {
        return static_cast<Variant::PackedArrayRef<T> *>(p_v->_data.packed_array)->array;
    }

    template <typename T>
    [[nodiscard]] static const PackedArray<T> &get_packed_array(const Variant *p_v) noexcept {
        return static_cast<const Variant::PackedArrayRef<T> *>(p_v->_data.packed_array)->array;
    }
};

} // namespace Beyota
