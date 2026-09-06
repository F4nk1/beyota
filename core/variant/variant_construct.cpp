/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_construct.cpp                                                 */
/**************************************************************************/

#include "core/variant/variant_construct.h"

namespace Beyota {

Variant VariantConstruct::construct_default(Variant::Type p_type) {
    switch (p_type) {
        case Variant::NIL: return Variant();
        case Variant::BOOL: return Variant(false);
        case Variant::INT: return Variant(static_cast<i64>(0));
        case Variant::FLOAT: return Variant(0.0);
        case Variant::STRING: return Variant(String());
        case Variant::VECTOR2: return Variant(Vector2());
        case Variant::VECTOR2I: return Variant(Vector2i());
        case Variant::RECT2: return Variant(Rect2());
        case Variant::RECT2I: return Variant(Rect2i());
        case Variant::VECTOR3: return Variant(Vector3());
        case Variant::VECTOR3I: return Variant(Vector3i());
        case Variant::TRANSFORM2D: return Variant(Transform2D());
        case Variant::VECTOR4: return Variant(Vector4());
        case Variant::VECTOR4I: return Variant(Vector4i());
        case Variant::PLANE: return Variant(Plane());
        case Variant::QUATERNION: return Variant(Quaternion());
        case Variant::AABB: return Variant(Math::AABB());
        case Variant::BASIS: return Variant(Basis());
        case Variant::TRANSFORM3D: return Variant(Transform3D());
        case Variant::PROJECTION: return Variant(Projection());
        case Variant::COLOR: return Variant(Color());
        case Variant::STRING_NAME: return Variant(StringName());
        case Variant::NODE_PATH: return Variant(NodePath());
        case Variant::RID: return Variant(Beyota::RID());
        case Variant::OBJECT: return Variant(static_cast<Object *>(nullptr));
        case Variant::CALLABLE: return Variant(Callable());
        case Variant::SIGNAL: return Variant(Signal());
        case Variant::DICTIONARY: return Variant(Dictionary());
        case Variant::ARRAY: return Variant(Array());
        case Variant::PACKED_BYTE_ARRAY: return Variant(PackedByteArray());
        case Variant::PACKED_INT32_ARRAY: return Variant(PackedInt32Array());
        case Variant::PACKED_INT64_ARRAY: return Variant(PackedInt64Array());
        case Variant::PACKED_FLOAT32_ARRAY: return Variant(PackedFloat32Array());
        case Variant::PACKED_FLOAT64_ARRAY: return Variant(PackedFloat64Array());
        case Variant::PACKED_STRING_ARRAY: return Variant(PackedStringArray());
        case Variant::PACKED_VECTOR2_ARRAY: return Variant(PackedVector2Array());
        case Variant::PACKED_VECTOR3_ARRAY: return Variant(PackedVector3Array());
        case Variant::PACKED_COLOR_ARRAY: return Variant(PackedColorArray());
        case Variant::PACKED_VECTOR4_ARRAY: return Variant(PackedVector4Array());
        default: return Variant();
    }
}

Variant VariantConstruct::construct(Variant::Type p_type, const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    r_error.error = Callable::CallError::CALL_OK;

    if (p_argcount == 0) {
        return construct_default(p_type);
    }

    if (p_argcount == 1) {
        const Variant &arg = *p_args[0];
        switch (p_type) {
            case Variant::BOOL: return Variant(arg.booleanize());
            case Variant::INT: return Variant(arg.operator i64());
            case Variant::FLOAT: return Variant(arg.operator f64());
            case Variant::STRING: return Variant(arg.stringify());
            case Variant::VECTOR2: return Variant(arg.operator Vector2());
            case Variant::VECTOR2I: return Variant(arg.operator Vector2i());
            case Variant::VECTOR3: return Variant(arg.operator Vector3());
            case Variant::VECTOR3I: return Variant(arg.operator Vector3i());
            case Variant::VECTOR4: return Variant(arg.operator Vector4());
            case Variant::VECTOR4I: return Variant(arg.operator Vector4i());
            case Variant::COLOR: return Variant(arg.operator Color());
            case Variant::STRING_NAME: return Variant(arg.operator StringName());
            case Variant::NODE_PATH: return Variant(arg.operator NodePath());
            default: break;
        }
    }

    if (p_argcount == 2) {
        if (p_type == Variant::VECTOR2) {
            return Variant(Vector2((real_t)p_args[0]->operator f64(), (real_t)p_args[1]->operator f64()));
        }
        if (p_type == Variant::VECTOR2I) {
            return Variant(Vector2i((i32)p_args[0]->operator i64(), (i32)p_args[1]->operator i64()));
        }
        if (p_type == Variant::RECT2) {
            return Variant(Rect2(p_args[0]->operator Vector2(), p_args[1]->operator Vector2()));
        }
        if (p_type == Variant::RECT2I) {
            return Variant(Rect2i(p_args[0]->operator Vector2i(), p_args[1]->operator Vector2i()));
        }
    }

    if (p_argcount == 3) {
        if (p_type == Variant::VECTOR3) {
            return Variant(Vector3((real_t)p_args[0]->operator f64(), (real_t)p_args[1]->operator f64(), (real_t)p_args[2]->operator f64()));
        }
        if (p_type == Variant::VECTOR3I) {
            return Variant(Vector3i((i32)p_args[0]->operator i64(), (i32)p_args[1]->operator i64(), (i32)p_args[2]->operator i64()));
        }
        if (p_type == Variant::COLOR) {
            return Variant(Color((f32)p_args[0]->operator f64(), (f32)p_args[1]->operator f64(), (f32)p_args[2]->operator f64()));
        }
    }

    if (p_argcount == 4) {
        if (p_type == Variant::VECTOR4) {
            return Variant(Vector4((real_t)p_args[0]->operator f64(), (real_t)p_args[1]->operator f64(), (real_t)p_args[2]->operator f64(), (real_t)p_args[3]->operator f64()));
        }
        if (p_type == Variant::VECTOR4I) {
            return Variant(Vector4i((i32)p_args[0]->operator i64(), (i32)p_args[1]->operator i64(), (i32)p_args[2]->operator i64(), (i32)p_args[3]->operator i64()));
        }
        if (p_type == Variant::COLOR) {
            return Variant(Color((f32)p_args[0]->operator f64(), (f32)p_args[1]->operator f64(), (f32)p_args[2]->operator f64(), (f32)p_args[3]->operator f64()));
        }
        if (p_type == Variant::QUATERNION) {
            return Variant(Quaternion((real_t)p_args[0]->operator f64(), (real_t)p_args[1]->operator f64(), (real_t)p_args[2]->operator f64(), (real_t)p_args[3]->operator f64()));
        }
        if (p_type == Variant::PLANE) {
            return Variant(Plane((real_t)p_args[0]->operator f64(), (real_t)p_args[1]->operator f64(), (real_t)p_args[2]->operator f64(), (real_t)p_args[3]->operator f64()));
        }
    }

    r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
    return Variant();
}

} // namespace Beyota
