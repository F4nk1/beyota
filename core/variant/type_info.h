/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  type_info.h                                                           */
/**************************************************************************/

#pragma once

#include "core/math/math_all.h"
#include "core/object/object_id.h"
#include "core/string/node_path.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/templates/rid.h"
#include "core/variant/packed_arrays.h"

namespace Beyota {

using Math::real_t;
using Math::Vector2;
using Math::Vector2i;
using Math::Rect2;
using Math::Rect2i;
using Math::Vector3;
using Math::Vector3i;
using Math::Transform2D;
using Math::Vector4;
using Math::Vector4i;
using Math::Plane;
using Math::Quaternion;
using Math::AABB;
using Math::Basis;
using Math::Transform3D;
using Math::Projection;
using Math::Color;
using Math::Face3;

class Object;
class RefCounted;
template <typename T> class Ref;

class Callable;
class Signal;
class Dictionary;
class Array;

class Variant;

enum class VariantType : u8 {
    NIL = 0,

    // Atomic types
    BOOL,
    INT,
    FLOAT,
    STRING,

    // Math types
    VECTOR2,
    VECTOR2I,
    RECT2,
    RECT2I,
    VECTOR3,
    VECTOR3I,
    TRANSFORM2D,
    VECTOR4,
    VECTOR4I,
    PLANE,
    QUATERNION,
    AABB,
    BASIS,
    TRANSFORM3D,
    PROJECTION,

    // Misc types
    COLOR,
    STRING_NAME,
    NODE_PATH,
    RID,
    OBJECT,
    CALLABLE,
    SIGNAL,
    DICTIONARY,
    ARRAY,

    // Typed arrays
    PACKED_BYTE_ARRAY,
    PACKED_INT32_ARRAY,
    PACKED_INT64_ARRAY,
    PACKED_FLOAT32_ARRAY,
    PACKED_FLOAT64_ARRAY,
    PACKED_STRING_ARRAY,
    PACKED_VECTOR2_ARRAY,
    PACKED_VECTOR3_ARRAY,
    PACKED_COLOR_ARRAY,
    PACKED_VECTOR4_ARRAY,

    VARIANT_MAX
};

enum class VariantOperator : u8 {
    // Comparison
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,

    // Mathematic
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_POSITIVE,
    OP_MODULE,
    OP_POWER,

    // Bitwise
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_XOR,
    OP_BIT_NEGATE,

    // Logic
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,

    // Containment
    OP_IN,

    OP_MAX
};

[[nodiscard]] constexpr const char *variant_type_to_string(VariantType p_type) noexcept {
    switch (p_type) {
        case VariantType::NIL: return "Nil";
        case VariantType::BOOL: return "bool";
        case VariantType::INT: return "int";
        case VariantType::FLOAT: return "float";
        case VariantType::STRING: return "String";
        case VariantType::VECTOR2: return "Vector2";
        case VariantType::VECTOR2I: return "Vector2i";
        case VariantType::RECT2: return "Rect2";
        case VariantType::RECT2I: return "Rect2i";
        case VariantType::VECTOR3: return "Vector3";
        case VariantType::VECTOR3I: return "Vector3i";
        case VariantType::TRANSFORM2D: return "Transform2D";
        case VariantType::VECTOR4: return "Vector4";
        case VariantType::VECTOR4I: return "Vector4i";
        case VariantType::PLANE: return "Plane";
        case VariantType::QUATERNION: return "Quaternion";
        case VariantType::AABB: return "AABB";
        case VariantType::BASIS: return "Basis";
        case VariantType::TRANSFORM3D: return "Transform3D";
        case VariantType::PROJECTION: return "Projection";
        case VariantType::COLOR: return "Color";
        case VariantType::STRING_NAME: return "StringName";
        case VariantType::NODE_PATH: return "NodePath";
        case VariantType::RID: return "RID";
        case VariantType::OBJECT: return "Object";
        case VariantType::CALLABLE: return "Callable";
        case VariantType::SIGNAL: return "Signal";
        case VariantType::DICTIONARY: return "Dictionary";
        case VariantType::ARRAY: return "Array";
        case VariantType::PACKED_BYTE_ARRAY: return "PackedByteArray";
        case VariantType::PACKED_INT32_ARRAY: return "PackedInt32Array";
        case VariantType::PACKED_INT64_ARRAY: return "PackedInt64Array";
        case VariantType::PACKED_FLOAT32_ARRAY: return "PackedFloat32Array";
        case VariantType::PACKED_FLOAT64_ARRAY: return "PackedFloat64Array";
        case VariantType::PACKED_STRING_ARRAY: return "PackedStringArray";
        case VariantType::PACKED_VECTOR2_ARRAY: return "PackedVector2Array";
        case VariantType::PACKED_VECTOR3_ARRAY: return "PackedVector3Array";
        case VariantType::PACKED_COLOR_ARRAY: return "PackedColorArray";
        case VariantType::PACKED_VECTOR4_ARRAY: return "PackedVector4Array";
        case VariantType::VARIANT_MAX: return "<invalid>";
    }
    return "<unknown>";
}

[[nodiscard]] constexpr const char *variant_operator_to_string(VariantOperator p_op) noexcept {
    switch (p_op) {
        case VariantOperator::OP_EQUAL: return "==";
        case VariantOperator::OP_NOT_EQUAL: return "!=";
        case VariantOperator::OP_LESS: return "<";
        case VariantOperator::OP_LESS_EQUAL: return "<=";
        case VariantOperator::OP_GREATER: return ">";
        case VariantOperator::OP_GREATER_EQUAL: return ">=";
        case VariantOperator::OP_ADD: return "+";
        case VariantOperator::OP_SUBTRACT: return "-";
        case VariantOperator::OP_MULTIPLY: return "*";
        case VariantOperator::OP_DIVIDE: return "/";
        case VariantOperator::OP_NEGATE: return "unary -";
        case VariantOperator::OP_POSITIVE: return "unary +";
        case VariantOperator::OP_MODULE: return "%";
        case VariantOperator::OP_POWER: return "**";
        case VariantOperator::OP_SHIFT_LEFT: return "<<";
        case VariantOperator::OP_SHIFT_RIGHT: return ">>";
        case VariantOperator::OP_BIT_AND: return "&";
        case VariantOperator::OP_BIT_OR: return "|";
        case VariantOperator::OP_BIT_XOR: return "^";
        case VariantOperator::OP_BIT_NEGATE: return "~";
        case VariantOperator::OP_AND: return "and";
        case VariantOperator::OP_OR: return "or";
        case VariantOperator::OP_XOR: return "xor";
        case VariantOperator::OP_NOT: return "not";
        case VariantOperator::OP_IN: return "in";
        case VariantOperator::OP_MAX: return "<invalid>";
    }
    return "<unknown>";
}

template <typename T>
struct GetTypeInfo;

template <>
struct GetTypeInfo<void> {
    static constexpr VariantType VARIANT_TYPE = VariantType::NIL;
    static constexpr std::string_view VARIANT_NAME = "Nil";
};

template <>
struct GetTypeInfo<std::nullptr_t> {
    static constexpr VariantType VARIANT_TYPE = VariantType::NIL;
    static constexpr std::string_view VARIANT_NAME = "Nil";
};

template <>
struct GetTypeInfo<bool> {
    static constexpr VariantType VARIANT_TYPE = VariantType::BOOL;
    static constexpr std::string_view VARIANT_NAME = "bool";
};

template <typename T>
requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
struct GetTypeInfo<T> {
    static constexpr VariantType VARIANT_TYPE = VariantType::INT;
    static constexpr std::string_view VARIANT_NAME = "int";
};

template <typename T>
requires std::is_floating_point_v<T>
struct GetTypeInfo<T> {
    static constexpr VariantType VARIANT_TYPE = VariantType::FLOAT;
    static constexpr std::string_view VARIANT_NAME = "float";
};

template <> struct GetTypeInfo<String> { static constexpr VariantType VARIANT_TYPE = VariantType::STRING; static constexpr std::string_view VARIANT_NAME = "String"; };
template <> struct GetTypeInfo<std::string_view> { static constexpr VariantType VARIANT_TYPE = VariantType::STRING; static constexpr std::string_view VARIANT_NAME = "String"; };
template <> struct GetTypeInfo<const char *> { static constexpr VariantType VARIANT_TYPE = VariantType::STRING; static constexpr std::string_view VARIANT_NAME = "String"; };
template <> struct GetTypeInfo<Vector2> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR2; static constexpr std::string_view VARIANT_NAME = "Vector2"; };
template <> struct GetTypeInfo<Vector2i> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR2I; static constexpr std::string_view VARIANT_NAME = "Vector2i"; };
template <> struct GetTypeInfo<Rect2> { static constexpr VariantType VARIANT_TYPE = VariantType::RECT2; static constexpr std::string_view VARIANT_NAME = "Rect2"; };
template <> struct GetTypeInfo<Rect2i> { static constexpr VariantType VARIANT_TYPE = VariantType::RECT2I; static constexpr std::string_view VARIANT_NAME = "Rect2i"; };
template <> struct GetTypeInfo<Vector3> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR3; static constexpr std::string_view VARIANT_NAME = "Vector3"; };
template <> struct GetTypeInfo<Vector3i> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR3I; static constexpr std::string_view VARIANT_NAME = "Vector3i"; };
template <> struct GetTypeInfo<Transform2D> { static constexpr VariantType VARIANT_TYPE = VariantType::TRANSFORM2D; static constexpr std::string_view VARIANT_NAME = "Transform2D"; };
template <> struct GetTypeInfo<Vector4> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR4; static constexpr std::string_view VARIANT_NAME = "Vector4"; };
template <> struct GetTypeInfo<Vector4i> { static constexpr VariantType VARIANT_TYPE = VariantType::VECTOR4I; static constexpr std::string_view VARIANT_NAME = "Vector4i"; };
template <> struct GetTypeInfo<Plane> { static constexpr VariantType VARIANT_TYPE = VariantType::PLANE; static constexpr std::string_view VARIANT_NAME = "Plane"; };
template <> struct GetTypeInfo<Quaternion> { static constexpr VariantType VARIANT_TYPE = VariantType::QUATERNION; static constexpr std::string_view VARIANT_NAME = "Quaternion"; };
template <> struct GetTypeInfo<AABB> { static constexpr VariantType VARIANT_TYPE = VariantType::AABB; static constexpr std::string_view VARIANT_NAME = "AABB"; };
template <> struct GetTypeInfo<Basis> { static constexpr VariantType VARIANT_TYPE = VariantType::BASIS; static constexpr std::string_view VARIANT_NAME = "Basis"; };
template <> struct GetTypeInfo<Transform3D> { static constexpr VariantType VARIANT_TYPE = VariantType::TRANSFORM3D; static constexpr std::string_view VARIANT_NAME = "Transform3D"; };
template <> struct GetTypeInfo<Projection> { static constexpr VariantType VARIANT_TYPE = VariantType::PROJECTION; static constexpr std::string_view VARIANT_NAME = "Projection"; };
template <> struct GetTypeInfo<Color> { static constexpr VariantType VARIANT_TYPE = VariantType::COLOR; static constexpr std::string_view VARIANT_NAME = "Color"; };
template <> struct GetTypeInfo<StringName> { static constexpr VariantType VARIANT_TYPE = VariantType::STRING_NAME; static constexpr std::string_view VARIANT_NAME = "StringName"; };
template <> struct GetTypeInfo<NodePath> { static constexpr VariantType VARIANT_TYPE = VariantType::NODE_PATH; static constexpr std::string_view VARIANT_NAME = "NodePath"; };
template <> struct GetTypeInfo<RID> { static constexpr VariantType VARIANT_TYPE = VariantType::RID; static constexpr std::string_view VARIANT_NAME = "RID"; };
template <> struct GetTypeInfo<ObjectID> { static constexpr VariantType VARIANT_TYPE = VariantType::OBJECT; static constexpr std::string_view VARIANT_NAME = "Object"; };
template <> struct GetTypeInfo<Object *> { static constexpr VariantType VARIANT_TYPE = VariantType::OBJECT; static constexpr std::string_view VARIANT_NAME = "Object"; };
template <> struct GetTypeInfo<const Object *> { static constexpr VariantType VARIANT_TYPE = VariantType::OBJECT; static constexpr std::string_view VARIANT_NAME = "Object"; };
template <> struct GetTypeInfo<Callable> { static constexpr VariantType VARIANT_TYPE = VariantType::CALLABLE; static constexpr std::string_view VARIANT_NAME = "Callable"; };
template <> struct GetTypeInfo<Signal> { static constexpr VariantType VARIANT_TYPE = VariantType::SIGNAL; static constexpr std::string_view VARIANT_NAME = "Signal"; };
template <> struct GetTypeInfo<Dictionary> { static constexpr VariantType VARIANT_TYPE = VariantType::DICTIONARY; static constexpr std::string_view VARIANT_NAME = "Dictionary"; };
template <> struct GetTypeInfo<Array> { static constexpr VariantType VARIANT_TYPE = VariantType::ARRAY; static constexpr std::string_view VARIANT_NAME = "Array"; };
template <> struct GetTypeInfo<PackedByteArray> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_BYTE_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedByteArray"; };
template <> struct GetTypeInfo<PackedInt32Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_INT32_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedInt32Array"; };
template <> struct GetTypeInfo<PackedInt64Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_INT64_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedInt64Array"; };
template <> struct GetTypeInfo<PackedFloat32Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_FLOAT32_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedFloat32Array"; };
template <> struct GetTypeInfo<PackedFloat64Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_FLOAT64_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedFloat64Array"; };
template <> struct GetTypeInfo<PackedStringArray> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_STRING_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedStringArray"; };
template <> struct GetTypeInfo<PackedVector2Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_VECTOR2_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedVector2Array"; };
template <> struct GetTypeInfo<PackedVector3Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_VECTOR3_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedVector3Array"; };
template <> struct GetTypeInfo<PackedColorArray> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_COLOR_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedColorArray"; };
template <> struct GetTypeInfo<PackedVector4Array> { static constexpr VariantType VARIANT_TYPE = VariantType::PACKED_VECTOR4_ARRAY; static constexpr std::string_view VARIANT_NAME = "PackedVector4Array"; };

template <typename T>
inline constexpr VariantType variant_type_of_v = GetTypeInfo<std::remove_cvref_t<T>>::VARIANT_TYPE;

} // namespace Beyota
