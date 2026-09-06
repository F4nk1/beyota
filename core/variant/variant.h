/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant.h                                                             */
/**************************************************************************/

#pragma once

#include "core/math/math_all.h"
#include "core/object/object_id.h"
#include "core/string/node_path.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/templates/rid.h"
#include "core/type_primitives.h"
#include "core/variant/array.h"
#include "core/variant/callable.h"
#include "core/variant/dictionary.h"
#include "core/variant/packed_arrays.h"
#include "core/variant/type_info.h"
#include "core/variant/variant_pools.h"

#include <cmath>
#include <cstring>
#include <format>
#include <new>
#include <string_view>

namespace Beyota {

class Object;
class VariantInternal;

class Variant {
public:
    using Type = VariantType;
    using Operator = VariantOperator;

    // Type constants for Godot syntax parity
    static constexpr Type NIL = VariantType::NIL;
    static constexpr Type BOOL = VariantType::BOOL;
    static constexpr Type INT = VariantType::INT;
    static constexpr Type FLOAT = VariantType::FLOAT;
    static constexpr Type STRING = VariantType::STRING;
    static constexpr Type VECTOR2 = VariantType::VECTOR2;
    static constexpr Type VECTOR2I = VariantType::VECTOR2I;
    static constexpr Type RECT2 = VariantType::RECT2;
    static constexpr Type RECT2I = VariantType::RECT2I;
    static constexpr Type VECTOR3 = VariantType::VECTOR3;
    static constexpr Type VECTOR3I = VariantType::VECTOR3I;
    static constexpr Type TRANSFORM2D = VariantType::TRANSFORM2D;
    static constexpr Type VECTOR4 = VariantType::VECTOR4;
    static constexpr Type VECTOR4I = VariantType::VECTOR4I;
    static constexpr Type PLANE = VariantType::PLANE;
    static constexpr Type QUATERNION = VariantType::QUATERNION;
    static constexpr Type AABB = VariantType::AABB;
    static constexpr Type BASIS = VariantType::BASIS;
    static constexpr Type TRANSFORM3D = VariantType::TRANSFORM3D;
    static constexpr Type PROJECTION = VariantType::PROJECTION;
    static constexpr Type COLOR = VariantType::COLOR;
    static constexpr Type STRING_NAME = VariantType::STRING_NAME;
    static constexpr Type NODE_PATH = VariantType::NODE_PATH;
    static constexpr Type RID = VariantType::RID;
    static constexpr Type OBJECT = VariantType::OBJECT;
    static constexpr Type CALLABLE = VariantType::CALLABLE;
    static constexpr Type SIGNAL = VariantType::SIGNAL;
    static constexpr Type DICTIONARY = VariantType::DICTIONARY;
    static constexpr Type ARRAY = VariantType::ARRAY;
    static constexpr Type PACKED_BYTE_ARRAY = VariantType::PACKED_BYTE_ARRAY;
    static constexpr Type PACKED_INT32_ARRAY = VariantType::PACKED_INT32_ARRAY;
    static constexpr Type PACKED_INT64_ARRAY = VariantType::PACKED_INT64_ARRAY;
    static constexpr Type PACKED_FLOAT32_ARRAY = VariantType::PACKED_FLOAT32_ARRAY;
    static constexpr Type PACKED_FLOAT64_ARRAY = VariantType::PACKED_FLOAT64_ARRAY;
    static constexpr Type PACKED_STRING_ARRAY = VariantType::PACKED_STRING_ARRAY;
    static constexpr Type PACKED_VECTOR2_ARRAY = VariantType::PACKED_VECTOR2_ARRAY;
    static constexpr Type PACKED_VECTOR3_ARRAY = VariantType::PACKED_VECTOR3_ARRAY;
    static constexpr Type PACKED_COLOR_ARRAY = VariantType::PACKED_COLOR_ARRAY;
    static constexpr Type PACKED_VECTOR4_ARRAY = VariantType::PACKED_VECTOR4_ARRAY;
    static constexpr Type VARIANT_MAX = VariantType::VARIANT_MAX;

    // Operator constants for Godot syntax parity
    static constexpr Operator OP_EQUAL = VariantOperator::OP_EQUAL;
    static constexpr Operator OP_NOT_EQUAL = VariantOperator::OP_NOT_EQUAL;
    static constexpr Operator OP_LESS = VariantOperator::OP_LESS;
    static constexpr Operator OP_LESS_EQUAL = VariantOperator::OP_LESS_EQUAL;
    static constexpr Operator OP_GREATER = VariantOperator::OP_GREATER;
    static constexpr Operator OP_GREATER_EQUAL = VariantOperator::OP_GREATER_EQUAL;
    static constexpr Operator OP_ADD = VariantOperator::OP_ADD;
    static constexpr Operator OP_SUBTRACT = VariantOperator::OP_SUBTRACT;
    static constexpr Operator OP_MULTIPLY = VariantOperator::OP_MULTIPLY;
    static constexpr Operator OP_DIVIDE = VariantOperator::OP_DIVIDE;
    static constexpr Operator OP_NEGATE = VariantOperator::OP_NEGATE;
    static constexpr Operator OP_POSITIVE = VariantOperator::OP_POSITIVE;
    static constexpr Operator OP_MODULE = VariantOperator::OP_MODULE;
    static constexpr Operator OP_POWER = VariantOperator::OP_POWER;
    static constexpr Operator OP_SHIFT_LEFT = VariantOperator::OP_SHIFT_LEFT;
    static constexpr Operator OP_SHIFT_RIGHT = VariantOperator::OP_SHIFT_RIGHT;
    static constexpr Operator OP_BIT_AND = VariantOperator::OP_BIT_AND;
    static constexpr Operator OP_BIT_OR = VariantOperator::OP_BIT_OR;
    static constexpr Operator OP_BIT_XOR = VariantOperator::OP_BIT_XOR;
    static constexpr Operator OP_BIT_NEGATE = VariantOperator::OP_BIT_NEGATE;
    static constexpr Operator OP_AND = VariantOperator::OP_AND;
    static constexpr Operator OP_OR = VariantOperator::OP_OR;
    static constexpr Operator OP_XOR = VariantOperator::OP_XOR;
    static constexpr Operator OP_NOT = VariantOperator::OP_NOT;
    static constexpr Operator OP_IN = VariantOperator::OP_IN;
    static constexpr Operator OP_MAX = VariantOperator::OP_MAX;

    struct ObjData {
        ObjectID id;
        Object *obj{nullptr};

        ObjData() = default;
        ObjData(const ObjectID &p_id, Object *p_obj) noexcept;
        ObjData(const ObjData &p_other) noexcept;
        ObjData(ObjData &&p_other) noexcept;
        ~ObjData() noexcept;
        ObjData &operator=(const ObjData &p_other) noexcept;
        ObjData &operator=(ObjData &&p_other) noexcept;

        void ref(const ObjData &p_from) noexcept;
        void ref_pointer(Object *p_obj) noexcept;
        void unref() noexcept;
    };

    struct PackedArrayRefBase {
        std::atomic<u32> refcount{1};
        virtual ~PackedArrayRefBase() = default;

        void ref() noexcept {
            refcount.fetch_add(1, std::memory_order_relaxed);
        }

        bool unref() noexcept {
            return refcount.fetch_sub(1, std::memory_order_acq_rel) == 1;
        }
    };

    template <typename T>
    struct PackedArrayRef : public PackedArrayRefBase {
        PackedArray<T> array;
        PackedArrayRef() = default;
        explicit PackedArrayRef(const PackedArray<T> &p_arr) : array(p_arr) {}
        explicit PackedArrayRef(PackedArray<T> &&p_arr) : array(std::move(p_arr)) {}
    };

private:
    friend class VariantInternal;

    Type type{NIL};

    union {
        bool _bool;
        i64 _int;
        f64 _float;
        Transform2D *_transform2d;
        Math::AABB *_aabb;
        Basis *_basis;
        Transform3D *_transform3d;
        Projection *_projection;
        PackedArrayRefBase *packed_array;
        void *_ptr;
        u8 _mem[sizeof(ObjData) > (sizeof(real_t) * 4) ? sizeof(ObjData) : (sizeof(real_t) * 4)]{0};
    } _data alignas(8);

    static constexpr bool needs_deinit[static_cast<usize>(VARIANT_MAX)] = {
        false, // NIL
        false, // BOOL
        false, // INT
        false, // FLOAT
        true,  // STRING
        false, // VECTOR2
        false, // VECTOR2I
        false, // RECT2
        false, // RECT2I
        false, // VECTOR3
        false, // VECTOR3I
        true,  // TRANSFORM2D
        false, // VECTOR4
        false, // VECTOR4I
        false, // PLANE
        false, // QUATERNION
        true,  // AABB
        true,  // BASIS
        true,  // TRANSFORM3D
        true,  // PROJECTION
        false, // COLOR
        true,  // STRING_NAME
        true,  // NODE_PATH
        false, // RID
        true,  // OBJECT
        true,  // CALLABLE
        true,  // SIGNAL
        true,  // DICTIONARY
        true,  // ARRAY
        true,  // PACKED_BYTE_ARRAY
        true,  // PACKED_INT32_ARRAY
        true,  // PACKED_INT64_ARRAY
        true,  // PACKED_FLOAT32_ARRAY
        true,  // PACKED_FLOAT64_ARRAY
        true,  // PACKED_STRING_ARRAY
        true,  // PACKED_VECTOR2_ARRAY
        true,  // PACKED_VECTOR3_ARRAY
        true,  // PACKED_COLOR_ARRAY
        true   // PACKED_VECTOR4_ARRAY
    };

    void _clear_internal() noexcept;

public:
    constexpr Variant() noexcept : type(NIL) {
        _data._int = 0;
    }

    // Primitive constructors
    Variant(bool p_bool) noexcept;
    Variant(i64 p_int) noexcept;
    Variant(i32 p_int) noexcept : Variant(static_cast<i64>(p_int)) {}
    Variant(i16 p_int) noexcept : Variant(static_cast<i64>(p_int)) {}
    Variant(i8 p_int) noexcept  : Variant(static_cast<i64>(p_int)) {}
    Variant(u64 p_int) noexcept : Variant(static_cast<i64>(p_int)) {}
    Variant(u32 p_int) noexcept : Variant(static_cast<i64>(p_int)) {}
    Variant(u16 p_int) noexcept : Variant(static_cast<i64>(p_int)) {}
    Variant(u8 p_int) noexcept  : Variant(static_cast<i64>(p_int)) {}
    Variant(f64 p_float) noexcept;
    Variant(f32 p_float) noexcept : Variant(static_cast<f64>(p_float)) {}

    // String constructors
    Variant(const String &p_string);
    Variant(String &&p_string);
    Variant(const char *p_cstring);
    Variant(std::string_view p_str);

    // Math constructors
    Variant(const Vector2 &p_vec2) noexcept;
    Variant(const Vector2i &p_vec2i) noexcept;
    Variant(const Rect2 &p_rect2) noexcept;
    Variant(const Rect2i &p_rect2i) noexcept;
    Variant(const Vector3 &p_vec3) noexcept;
    Variant(const Vector3i &p_vec3i) noexcept;
    Variant(const Transform2D &p_trans2d);
    Variant(const Vector4 &p_vec4) noexcept;
    Variant(const Vector4i &p_vec4i) noexcept;
    Variant(const Plane &p_plane) noexcept;
    Variant(const Quaternion &p_quat) noexcept;
    Variant(const Math::AABB &p_aabb);
    Variant(const Basis &p_basis);
    Variant(const Transform3D &p_trans3d);
    Variant(const Projection &p_proj);
    Variant(const Color &p_color) noexcept;

    // Misc constructors
    Variant(const StringName &p_name);
    Variant(StringName &&p_name);
    Variant(const NodePath &p_path);
    Variant(NodePath &&p_path);
    Variant(const Beyota::RID &p_rid) noexcept;
    Variant(const ObjectID &p_id) noexcept;
    Variant(const Object *p_obj) noexcept;
    Variant(const Callable &p_callable);
    Variant(Callable &&p_callable);
    Variant(const Signal &p_signal);
    Variant(Signal &&p_signal);
    Variant(const Dictionary &p_dict);
    Variant(Dictionary &&p_dict);
    Variant(const Array &p_array);
    Variant(Array &&p_array);

    // Packed arrays constructors
    Variant(const PackedByteArray &p_array);
    Variant(const PackedInt32Array &p_array);
    Variant(const PackedInt64Array &p_array);
    Variant(const PackedFloat32Array &p_array);
    Variant(const PackedFloat64Array &p_array);
    Variant(const PackedStringArray &p_array);
    Variant(const PackedVector2Array &p_array);
    Variant(const PackedVector3Array &p_array);
    Variant(const PackedColorArray &p_array);
    Variant(const PackedVector4Array &p_array);

    // Copy and Move
    Variant(const Variant &p_other);
    Variant(Variant &&p_other) noexcept;
    ~Variant() noexcept { clear(); }

    Variant &operator=(const Variant &p_other);
    Variant &operator=(Variant &&p_other) noexcept;

    void clear() noexcept {
        if (needs_deinit[static_cast<usize>(type)]) {
            _clear_internal();
        }
        type = NIL;
        _data._int = 0;
    }

    [[nodiscard]] Type get_type() const noexcept { return type; }
    [[nodiscard]] const char *get_type_name() const noexcept { return variant_type_to_string(type); }

    [[nodiscard]] bool is_null() const noexcept { return type == NIL; }
    [[nodiscard]] bool is_num() const noexcept { return type == INT || type == FLOAT; }

    [[nodiscard]] bool booleanize() const noexcept;

    // Implicit / explicit type casts
    [[nodiscard]] operator bool() const noexcept { return booleanize(); }
    [[nodiscard]] operator i64() const noexcept;
    [[nodiscard]] operator i32() const noexcept { return static_cast<i32>(operator i64()); }
    [[nodiscard]] operator u64() const noexcept { return static_cast<u64>(operator i64()); }
    [[nodiscard]] operator u32() const noexcept { return static_cast<u32>(operator i64()); }
    [[nodiscard]] operator f64() const noexcept;
    [[nodiscard]] operator f32() const noexcept { return static_cast<f32>(operator f64()); }

    [[nodiscard]] operator String() const;
    [[nodiscard]] operator Vector2() const;
    [[nodiscard]] operator Vector2i() const;
    [[nodiscard]] operator Rect2() const;
    [[nodiscard]] operator Rect2i() const;
    [[nodiscard]] operator Vector3() const;
    [[nodiscard]] operator Vector3i() const;
    [[nodiscard]] operator Transform2D() const;
    [[nodiscard]] operator Vector4() const;
    [[nodiscard]] operator Vector4i() const;
    [[nodiscard]] operator Plane() const;
    [[nodiscard]] operator Quaternion() const;
    [[nodiscard]] operator Math::AABB() const;
    [[nodiscard]] operator Basis() const;
    [[nodiscard]] operator Transform3D() const;
    [[nodiscard]] operator Projection() const;
    [[nodiscard]] operator Color() const;
    [[nodiscard]] operator StringName() const;
    [[nodiscard]] operator NodePath() const;
    [[nodiscard]] operator Beyota::RID() const;
    [[nodiscard]] operator Object *() const;
    [[nodiscard]] operator ObjectID() const;
    [[nodiscard]] operator Callable() const;
    [[nodiscard]] operator Signal() const;
    [[nodiscard]] operator Dictionary() const;
    [[nodiscard]] operator Array() const;
    [[nodiscard]] operator PackedByteArray() const;
    [[nodiscard]] operator PackedInt32Array() const;
    [[nodiscard]] operator PackedInt64Array() const;
    [[nodiscard]] operator PackedFloat32Array() const;
    [[nodiscard]] operator PackedFloat64Array() const;
    [[nodiscard]] operator PackedStringArray() const;
    [[nodiscard]] operator PackedVector2Array() const;
    [[nodiscard]] operator PackedVector3Array() const;
    [[nodiscard]] operator PackedColorArray() const;
    [[nodiscard]] operator PackedVector4Array() const;

    [[nodiscard]] u32 hash() const noexcept;
    [[nodiscard]] String stringify() const;

    // Canonical operator evaluation
    static void evaluate(Operator p_op, const Variant &p_left, const Variant &p_right, Variant &r_ret, bool &r_valid);

    // Operator overloads
    [[nodiscard]] bool operator==(const Variant &p_other) const;
    [[nodiscard]] bool operator!=(const Variant &p_other) const { return !(*this == p_other); }
    [[nodiscard]] bool operator<(const Variant &p_other) const;
    [[nodiscard]] bool operator<=(const Variant &p_other) const;
    [[nodiscard]] bool operator>(const Variant &p_other) const;
    [[nodiscard]] bool operator>=(const Variant &p_other) const;

    [[nodiscard]] Variant operator+(const Variant &p_other) const;
    [[nodiscard]] Variant operator-(const Variant &p_other) const;
    [[nodiscard]] Variant operator*(const Variant &p_other) const;
    [[nodiscard]] Variant operator/(const Variant &p_other) const;
    [[nodiscard]] Variant operator%(const Variant &p_other) const;

    [[nodiscard]] Variant operator-() const;
    [[nodiscard]] Variant operator+() const;
    [[nodiscard]] Variant operator!() const;

    [[nodiscard]] Variant operator&(const Variant &p_other) const;
    [[nodiscard]] Variant operator|(const Variant &p_other) const;
    [[nodiscard]] Variant operator^(const Variant &p_other) const;
    [[nodiscard]] Variant operator~() const;
    [[nodiscard]] Variant operator<<(const Variant &p_other) const;
    [[nodiscard]] Variant operator>>(const Variant &p_other) const;

    [[nodiscard]] bool in(const Variant &p_container) const;

    // Indexed and named access
    [[nodiscard]] Variant get_indexed(usize p_idx) const;
    bool set_indexed(usize p_idx, const Variant &p_val);
    [[nodiscard]] Variant get_named(const StringName &p_name) const;
    bool set_named(const StringName &p_name, const Variant &p_val);

    // Compatibility check
    static bool can_convert(Type p_from, Type p_to) noexcept;
    static bool can_convert_strict(Type p_from, Type p_to) noexcept;
};

static_assert(sizeof(Variant) == 24, "Variant size contract violated: must be 24 bytes in single-precision mode!");

} // namespace Beyota

template <>
struct std::formatter<Beyota::Variant> : std::formatter<std::string_view> {
    auto format(const Beyota::Variant &p_var, std::format_context &p_ctx) const {
        Beyota::String s = p_var.stringify();
        return std::formatter<std::string_view>::format(s.as_string_view(), p_ctx);
    }
};
