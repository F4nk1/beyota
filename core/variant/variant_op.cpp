/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_op.cpp                                                        */
/**************************************************************************/

#include "core/variant/variant_op.h"
#include "core/math/math_funcs.h"
#include "core/variant/variant_internal.h"

#include <cmath>
#include <limits>

namespace Beyota {

void Variant::evaluate(Operator p_op, const Variant &p_left, const Variant &p_right, Variant &r_ret, bool &r_valid) {
    VariantOp::evaluate(p_op, p_left, p_right, r_ret, r_valid);
}

void VariantOp::evaluate(Variant::Operator p_op, const Variant &p_left, const Variant &p_right, Variant &r_ret, bool &r_valid) {
    r_valid = false;

    Variant::Type t1 = p_left.get_type();
    Variant::Type t2 = p_right.get_type();

    // 1. Comparison: Equal
    if (p_op == Variant::OP_EQUAL) {
        r_valid = true;
        if (t1 == Variant::NIL && t2 == Variant::NIL) {
            r_ret = true;
            return;
        }
        if (t1 == Variant::NIL || t2 == Variant::NIL) {
            r_ret = false;
            return;
        }
        if (t1 == Variant::BOOL && t2 == Variant::BOOL) {
            r_ret = VariantInternal::get_bool(&p_left) == VariantInternal::get_bool(&p_right);
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_int(&p_left) == VariantInternal::get_int(&p_right);
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_float(&p_left) == VariantInternal::get_float(&p_right);
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            r_ret = (f64)VariantInternal::get_int(&p_left) == VariantInternal::get_float(&p_right);
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_float(&p_left) == (f64)VariantInternal::get_int(&p_right);
            return;
        }
        if (t1 == Variant::STRING && t2 == Variant::STRING) {
            r_ret = VariantInternal::get_string(&p_left) == VariantInternal::get_string(&p_right);
            return;
        }
        if (t1 == Variant::STRING && t2 == Variant::STRING_NAME) {
            r_ret = VariantInternal::get_string(&p_left).as_string_view() == VariantInternal::get_string_name(&p_right).as_string_view();
            return;
        }
        if (t1 == Variant::STRING_NAME && t2 == Variant::STRING) {
            r_ret = VariantInternal::get_string_name(&p_left).as_string_view() == VariantInternal::get_string(&p_right).as_string_view();
            return;
        }
        if (t1 == Variant::STRING_NAME && t2 == Variant::STRING_NAME) {
            r_ret = VariantInternal::get_string_name(&p_left) == VariantInternal::get_string_name(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_left) == VariantInternal::get_vector2(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            r_ret = VariantInternal::get_vector2i(&p_left) == VariantInternal::get_vector2i(&p_right);
            return;
        }
        if (t1 == Variant::RECT2 && t2 == Variant::RECT2) {
            r_ret = VariantInternal::get_rect2(&p_left) == VariantInternal::get_rect2(&p_right);
            return;
        }
        if (t1 == Variant::RECT2I && t2 == Variant::RECT2I) {
            r_ret = VariantInternal::get_rect2i(&p_left) == VariantInternal::get_rect2i(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_left) == VariantInternal::get_vector3(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR3I && t2 == Variant::VECTOR3I) {
            r_ret = VariantInternal::get_vector3i(&p_left) == VariantInternal::get_vector3i(&p_right);
            return;
        }
        if (t1 == Variant::TRANSFORM2D && t2 == Variant::TRANSFORM2D) {
            r_ret = *VariantInternal::get_transform2d(&p_left) == *VariantInternal::get_transform2d(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR4 && t2 == Variant::VECTOR4) {
            r_ret = VariantInternal::get_vector4(&p_left) == VariantInternal::get_vector4(&p_right);
            return;
        }
        if (t1 == Variant::VECTOR4I && t2 == Variant::VECTOR4I) {
            r_ret = VariantInternal::get_vector4i(&p_left) == VariantInternal::get_vector4i(&p_right);
            return;
        }
        if (t1 == Variant::PLANE && t2 == Variant::PLANE) {
            r_ret = VariantInternal::get_plane(&p_left) == VariantInternal::get_plane(&p_right);
            return;
        }
        if (t1 == Variant::QUATERNION && t2 == Variant::QUATERNION) {
            r_ret = VariantInternal::get_quaternion(&p_left) == VariantInternal::get_quaternion(&p_right);
            return;
        }
        if (t1 == Variant::AABB && t2 == Variant::AABB) {
            r_ret = *VariantInternal::get_aabb(&p_left) == *VariantInternal::get_aabb(&p_right);
            return;
        }
        if (t1 == Variant::BASIS && t2 == Variant::BASIS) {
            r_ret = *VariantInternal::get_basis(&p_left) == *VariantInternal::get_basis(&p_right);
            return;
        }
        if (t1 == Variant::TRANSFORM3D && t2 == Variant::TRANSFORM3D) {
            r_ret = *VariantInternal::get_transform3d(&p_left) == *VariantInternal::get_transform3d(&p_right);
            return;
        }
        if (t1 == Variant::PROJECTION && t2 == Variant::PROJECTION) {
            r_ret = *VariantInternal::get_projection(&p_left) == *VariantInternal::get_projection(&p_right);
            return;
        }
        if (t1 == Variant::COLOR && t2 == Variant::COLOR) {
            r_ret = VariantInternal::get_color(&p_left) == VariantInternal::get_color(&p_right);
            return;
        }
        if (t1 == Variant::NODE_PATH && t2 == Variant::NODE_PATH) {
            r_ret = VariantInternal::get_node_path(&p_left) == VariantInternal::get_node_path(&p_right);
            return;
        }
        if (t1 == Variant::RID && t2 == Variant::RID) {
            r_ret = VariantInternal::get_rid(&p_left) == VariantInternal::get_rid(&p_right);
            return;
        }
        if (t1 == Variant::OBJECT && t2 == Variant::OBJECT) {
            r_ret = VariantInternal::get_obj(&p_left).id == VariantInternal::get_obj(&p_right).id;
            return;
        }
        if (t1 == Variant::CALLABLE && t2 == Variant::CALLABLE) {
            r_ret = VariantInternal::get_callable(&p_left) == VariantInternal::get_callable(&p_right);
            return;
        }
        if (t1 == Variant::SIGNAL && t2 == Variant::SIGNAL) {
            r_ret = VariantInternal::get_signal(&p_left) == VariantInternal::get_signal(&p_right);
            return;
        }
        if (t1 == Variant::DICTIONARY && t2 == Variant::DICTIONARY) {
            r_ret = VariantInternal::get_dictionary(&p_left) == VariantInternal::get_dictionary(&p_right);
            return;
        }
        if (t1 == Variant::ARRAY && t2 == Variant::ARRAY) {
            r_ret = VariantInternal::get_array(&p_left) == VariantInternal::get_array(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_BYTE_ARRAY && t2 == Variant::PACKED_BYTE_ARRAY) {
            r_ret = VariantInternal::get_packed_array<u8>(&p_left) == VariantInternal::get_packed_array<u8>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_INT32_ARRAY && t2 == Variant::PACKED_INT32_ARRAY) {
            r_ret = VariantInternal::get_packed_array<i32>(&p_left) == VariantInternal::get_packed_array<i32>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_INT64_ARRAY && t2 == Variant::PACKED_INT64_ARRAY) {
            r_ret = VariantInternal::get_packed_array<i64>(&p_left) == VariantInternal::get_packed_array<i64>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_FLOAT32_ARRAY && t2 == Variant::PACKED_FLOAT32_ARRAY) {
            r_ret = VariantInternal::get_packed_array<f32>(&p_left) == VariantInternal::get_packed_array<f32>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_FLOAT64_ARRAY && t2 == Variant::PACKED_FLOAT64_ARRAY) {
            r_ret = VariantInternal::get_packed_array<f64>(&p_left) == VariantInternal::get_packed_array<f64>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_STRING_ARRAY && t2 == Variant::PACKED_STRING_ARRAY) {
            r_ret = VariantInternal::get_packed_array<String>(&p_left) == VariantInternal::get_packed_array<String>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_VECTOR2_ARRAY && t2 == Variant::PACKED_VECTOR2_ARRAY) {
            r_ret = VariantInternal::get_packed_array<Vector2>(&p_left) == VariantInternal::get_packed_array<Vector2>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_VECTOR3_ARRAY && t2 == Variant::PACKED_VECTOR3_ARRAY) {
            r_ret = VariantInternal::get_packed_array<Vector3>(&p_left) == VariantInternal::get_packed_array<Vector3>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_COLOR_ARRAY && t2 == Variant::PACKED_COLOR_ARRAY) {
            r_ret = VariantInternal::get_packed_array<Color>(&p_left) == VariantInternal::get_packed_array<Color>(&p_right);
            return;
        }
        if (t1 == Variant::PACKED_VECTOR4_ARRAY && t2 == Variant::PACKED_VECTOR4_ARRAY) {
            r_ret = VariantInternal::get_packed_array<Vector4>(&p_left) == VariantInternal::get_packed_array<Vector4>(&p_right);
            return;
        }
        r_ret = false;
        return;
    }

    if (p_op == Variant::OP_NOT_EQUAL) {
        evaluate(Variant::OP_EQUAL, p_left, p_right, r_ret, r_valid);
        if (r_valid) {
            r_ret = !r_ret.booleanize();
        }
        return;
    }

    // 2. Comparison: Less
    if (p_op == Variant::OP_LESS) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_int(&p_left) < VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_float(&p_left) < VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            r_ret = (f64)VariantInternal::get_int(&p_left) < VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_float(&p_left) < (f64)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING && t2 == Variant::STRING) {
            r_ret = VariantInternal::get_string(&p_left) < VariantInternal::get_string(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING_NAME && t2 == Variant::STRING_NAME) {
            r_ret = VariantInternal::get_string_name(&p_left) < VariantInternal::get_string_name(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_left) < VariantInternal::get_vector2(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            r_ret = VariantInternal::get_vector2i(&p_left) < VariantInternal::get_vector2i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_left) < VariantInternal::get_vector3(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3I && t2 == Variant::VECTOR3I) {
            r_ret = VariantInternal::get_vector3i(&p_left) < VariantInternal::get_vector3i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4 && t2 == Variant::VECTOR4) {
            r_ret = VariantInternal::get_vector4(&p_left) < VariantInternal::get_vector4(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4I && t2 == Variant::VECTOR4I) {
            r_ret = VariantInternal::get_vector4i(&p_left) < VariantInternal::get_vector4i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::OBJECT && t2 == Variant::OBJECT) {
            r_ret = VariantInternal::get_obj(&p_left).id < VariantInternal::get_obj(&p_right).id;
            r_valid = true;
            return;
        }
        if (t1 == Variant::RID && t2 == Variant::RID) {
            r_ret = VariantInternal::get_rid(&p_left) < VariantInternal::get_rid(&p_right);
            r_valid = true;
            return;
        }
        return;
    }

    if (p_op == Variant::OP_LESS_EQUAL) {
        Variant r1, r2;
        bool v1 = false, v2 = false;
        evaluate(Variant::OP_LESS, p_left, p_right, r1, v1);
        evaluate(Variant::OP_EQUAL, p_left, p_right, r2, v2);
        if (v1 && v2) {
            r_ret = r1.booleanize() || r2.booleanize();
            r_valid = true;
        }
        return;
    }

    if (p_op == Variant::OP_GREATER) {
        evaluate(Variant::OP_LESS, p_right, p_left, r_ret, r_valid);
        return;
    }

    if (p_op == Variant::OP_GREATER_EQUAL) {
        evaluate(Variant::OP_LESS_EQUAL, p_right, p_left, r_ret, r_valid);
        return;
    }

    // 3. Mathematic: Add
    if (p_op == Variant::OP_ADD) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_int(&p_left) + VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            r_ret = (f64)VariantInternal::get_int(&p_left) + VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_float(&p_left) + (f64)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_float(&p_left) + VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_left) + VariantInternal::get_vector2(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            r_ret = VariantInternal::get_vector2i(&p_left) + VariantInternal::get_vector2i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_left) + VariantInternal::get_vector3(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3I && t2 == Variant::VECTOR3I) {
            r_ret = VariantInternal::get_vector3i(&p_left) + VariantInternal::get_vector3i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4 && t2 == Variant::VECTOR4) {
            r_ret = VariantInternal::get_vector4(&p_left) + VariantInternal::get_vector4(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4I && t2 == Variant::VECTOR4I) {
            r_ret = VariantInternal::get_vector4i(&p_left) + VariantInternal::get_vector4i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::COLOR && t2 == Variant::COLOR) {
            r_ret = VariantInternal::get_color(&p_left) + VariantInternal::get_color(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::QUATERNION && t2 == Variant::QUATERNION) {
            r_ret = VariantInternal::get_quaternion(&p_left) + VariantInternal::get_quaternion(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING && t2 == Variant::STRING) {
            r_ret = VariantInternal::get_string(&p_left) + VariantInternal::get_string(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING && t2 == Variant::STRING_NAME) {
            r_ret = VariantInternal::get_string(&p_left) + VariantInternal::get_string_name(&p_right).as_string_view();
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING_NAME && t2 == Variant::STRING) {
            r_ret = String(VariantInternal::get_string_name(&p_left).as_string_view()) + VariantInternal::get_string(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING_NAME && t2 == Variant::STRING_NAME) {
            r_ret = String(VariantInternal::get_string_name(&p_left).as_string_view()) + VariantInternal::get_string_name(&p_right).as_string_view();
            r_valid = true;
            return;
        }
        if (t1 == Variant::ARRAY && t2 == Variant::ARRAY) {
            Array res = VariantInternal::get_array(&p_left).duplicate();
            const auto &r = VariantInternal::get_array(&p_right);
            for (usize i = 0; i < r.size(); ++i) {
                res.push_back(r[i]);
            }
            r_ret = res;
            r_valid = true;
            return;
        }
        return;
    }

    // 4. Mathematic: Subtract
    if (p_op == Variant::OP_SUBTRACT) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_int(&p_left) - VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            r_ret = (f64)VariantInternal::get_int(&p_left) - VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_float(&p_left) - (f64)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_float(&p_left) - VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_left) - VariantInternal::get_vector2(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            r_ret = VariantInternal::get_vector2i(&p_left) - VariantInternal::get_vector2i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_left) - VariantInternal::get_vector3(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3I && t2 == Variant::VECTOR3I) {
            r_ret = VariantInternal::get_vector3i(&p_left) - VariantInternal::get_vector3i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4 && t2 == Variant::VECTOR4) {
            r_ret = VariantInternal::get_vector4(&p_left) - VariantInternal::get_vector4(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4I && t2 == Variant::VECTOR4I) {
            r_ret = VariantInternal::get_vector4i(&p_left) - VariantInternal::get_vector4i(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::COLOR && t2 == Variant::COLOR) {
            r_ret = VariantInternal::get_color(&p_left) - VariantInternal::get_color(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::QUATERNION && t2 == Variant::QUATERNION) {
            r_ret = VariantInternal::get_quaternion(&p_left) - VariantInternal::get_quaternion(&p_right);
            r_valid = true;
            return;
        }
        return;
    }

    // 5. Mathematic: Multiply
    if (p_op == Variant::OP_MULTIPLY) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_int(&p_left) * VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            r_ret = (f64)VariantInternal::get_int(&p_left) * VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            r_ret = VariantInternal::get_float(&p_left) * (f64)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_float(&p_left) * VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_left) * VariantInternal::get_vector2(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_vector2(&p_left) * (real_t)VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::INT) {
            r_ret = VariantInternal::get_vector2(&p_left) * (real_t)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_right) * (real_t)VariantInternal::get_float(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_vector2(&p_right) * (real_t)VariantInternal::get_int(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::TRANSFORM2D) {
            r_ret = VariantInternal::get_transform2d(&p_right)->xform(VariantInternal::get_vector2(&p_left));
            r_valid = true;
            return;
        }
        if (t1 == Variant::TRANSFORM2D && t2 == Variant::VECTOR2) {
            r_ret = VariantInternal::get_transform2d(&p_left)->xform(VariantInternal::get_vector2(&p_right));
            r_valid = true;
            return;
        }
        if (t1 == Variant::TRANSFORM2D && t2 == Variant::TRANSFORM2D) {
            r_ret = *VariantInternal::get_transform2d(&p_left) * *VariantInternal::get_transform2d(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_left) * VariantInternal::get_vector3(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_vector3(&p_left) * (real_t)VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && t2 == Variant::INT) {
            r_ret = VariantInternal::get_vector3(&p_left) * (real_t)VariantInternal::get_int(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_right) * (real_t)VariantInternal::get_float(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_vector3(&p_right) * (real_t)VariantInternal::get_int(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::BASIS && t2 == Variant::BASIS) {
            r_ret = *VariantInternal::get_basis(&p_left) * *VariantInternal::get_basis(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::BASIS && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_basis(&p_left)->xform(VariantInternal::get_vector3(&p_right));
            r_valid = true;
            return;
        }
        if (t1 == Variant::TRANSFORM3D && t2 == Variant::TRANSFORM3D) {
            r_ret = *VariantInternal::get_transform3d(&p_left) * *VariantInternal::get_transform3d(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::TRANSFORM3D && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_transform3d(&p_left)->xform(VariantInternal::get_vector3(&p_right));
            r_valid = true;
            return;
        }
        if (t1 == Variant::QUATERNION && t2 == Variant::QUATERNION) {
            r_ret = VariantInternal::get_quaternion(&p_left) * VariantInternal::get_quaternion(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::QUATERNION && t2 == Variant::VECTOR3) {
            r_ret = VariantInternal::get_quaternion(&p_left).xform(VariantInternal::get_vector3(&p_right));
            r_valid = true;
            return;
        }
        if (t1 == Variant::COLOR && t2 == Variant::COLOR) {
            r_ret = VariantInternal::get_color(&p_left) * VariantInternal::get_color(&p_right);
            r_valid = true;
            return;
        }
        if (t1 == Variant::COLOR && t2 == Variant::FLOAT) {
            r_ret = VariantInternal::get_color(&p_left) * (f32)VariantInternal::get_float(&p_right);
            r_valid = true;
            return;
        }
        return;
    }

    // 6. Mathematic: Divide (Guarded against division by zero and hardware overflow!)
    if (p_op == Variant::OP_DIVIDE) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            i64 divisor = VariantInternal::get_int(&p_right);
            if (divisor == 0) {
                r_valid = false;
                return;
            }
            r_ret = Math::division_no_overflow(VariantInternal::get_int(&p_left), divisor);
            r_valid = true;
            return;
        }
        if (t1 == Variant::INT && t2 == Variant::FLOAT) {
            f64 divisor = VariantInternal::get_float(&p_right);
            if (divisor == 0.0) {
                r_valid = false;
                return;
            }
            r_ret = (f64)VariantInternal::get_int(&p_left) / divisor;
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::INT) {
            i64 divisor = VariantInternal::get_int(&p_right);
            if (divisor == 0) {
                r_valid = false;
                return;
            }
            r_ret = VariantInternal::get_float(&p_left) / (f64)divisor;
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            f64 divisor = VariantInternal::get_float(&p_right);
            if (divisor == 0.0) {
                r_valid = false;
                return;
            }
            r_ret = VariantInternal::get_float(&p_left) / divisor;
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && t2 == Variant::VECTOR2) {
            const auto &b = VariantInternal::get_vector2(&p_right);
            if (b.x == 0.0f || b.y == 0.0f) {
                r_valid = false;
                return;
            }
            r_ret = VariantInternal::get_vector2(&p_left) / b;
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2 && (t2 == Variant::FLOAT || t2 == Variant::INT)) {
            real_t d = (t2 == Variant::FLOAT) ? (real_t)VariantInternal::get_float(&p_right) : (real_t)VariantInternal::get_int(&p_right);
            if (d == 0.0f) {
                r_valid = false;
                return;
            }
            r_ret = VariantInternal::get_vector2(&p_left) / d;
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            const auto &b = VariantInternal::get_vector2i(&p_right);
            if (b.x == 0 || b.y == 0) {
                r_valid = false;
                return;
            }
            const auto &a = VariantInternal::get_vector2i(&p_left);
            r_ret = Vector2i(Math::division_no_overflow(a.x, b.x), Math::division_no_overflow(a.y, b.y));
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3 && (t2 == Variant::FLOAT || t2 == Variant::INT)) {
            real_t d = (t2 == Variant::FLOAT) ? (real_t)VariantInternal::get_float(&p_right) : (real_t)VariantInternal::get_int(&p_right);
            if (d == 0.0f) {
                r_valid = false;
                return;
            }
            r_ret = VariantInternal::get_vector3(&p_left) / d;
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3I && t2 == Variant::VECTOR3I) {
            const auto &b = VariantInternal::get_vector3i(&p_right);
            if (b.x == 0 || b.y == 0 || b.z == 0) {
                r_valid = false;
                return;
            }
            const auto &a = VariantInternal::get_vector3i(&p_left);
            r_ret = Vector3i(Math::division_no_overflow(a.x, b.x), Math::division_no_overflow(a.y, b.y), Math::division_no_overflow(a.z, b.z));
            r_valid = true;
            return;
        }
        return;
    }

    // 7. Mathematic: Modulo (Guarded against modulo by zero and INT_MIN / -1 overflow!)
    if (p_op == Variant::OP_MODULE) {
        if (t1 == Variant::INT && t2 == Variant::INT) {
            i64 divisor = VariantInternal::get_int(&p_right);
            if (divisor == 0) {
                r_valid = false;
                return;
            }
            r_ret = Math::modulo_no_overflow(VariantInternal::get_int(&p_left), divisor);
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT && t2 == Variant::FLOAT) {
            f64 divisor = VariantInternal::get_float(&p_right);
            if (divisor == 0.0) {
                r_valid = false;
                return;
            }
            r_ret = Math::fposmod(VariantInternal::get_float(&p_left), divisor);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I && t2 == Variant::VECTOR2I) {
            const auto &b = VariantInternal::get_vector2i(&p_right);
            if (b.x == 0 || b.y == 0) {
                r_valid = false;
                return;
            }
            const auto &a = VariantInternal::get_vector2i(&p_left);
            r_ret = Vector2i(Math::modulo_no_overflow(a.x, b.x), Math::modulo_no_overflow(a.y, b.y));
            r_valid = true;
            return;
        }
        if (t1 == Variant::STRING) {
            // String formatting: s % v
            r_ret = VariantInternal::get_string(&p_left).replace("%s", p_right.stringify().as_string_view());
            r_valid = true;
            return;
        }
        return;
    }

    // 8. Mathematic: Negate (Unary -)
    if (p_op == Variant::OP_NEGATE) {
        if (t1 == Variant::INT) {
            i64 val = VariantInternal::get_int(&p_left);
            if (val == std::numeric_limits<i64>::min()) {
                r_ret = std::numeric_limits<i64>::max(); // Mitigate signed overflow
            } else {
                r_ret = -val;
            }
            r_valid = true;
            return;
        }
        if (t1 == Variant::FLOAT) {
            r_ret = -VariantInternal::get_float(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2) {
            r_ret = -VariantInternal::get_vector2(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR2I) {
            r_ret = -VariantInternal::get_vector2i(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3) {
            r_ret = -VariantInternal::get_vector3(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR3I) {
            r_ret = -VariantInternal::get_vector3i(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4) {
            r_ret = -VariantInternal::get_vector4(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::VECTOR4I) {
            r_ret = -VariantInternal::get_vector4i(&p_left);
            r_valid = true;
            return;
        }
        if (t1 == Variant::COLOR) {
            r_ret = -VariantInternal::get_color(&p_left);
            r_valid = true;
            return;
        }
        return;
    }

    // 9. Positive (Unary +)
    if (p_op == Variant::OP_POSITIVE) {
        r_ret = p_left;
        r_valid = true;
        return;
    }

    // 10. Power
    if (p_op == Variant::OP_POWER) {
        if ((t1 == Variant::INT || t1 == Variant::FLOAT) && (t2 == Variant::INT || t2 == Variant::FLOAT)) {
            f64 base = (t1 == Variant::INT) ? (f64)VariantInternal::get_int(&p_left) : VariantInternal::get_float(&p_left);
            f64 exp = (t2 == Variant::INT) ? (f64)VariantInternal::get_int(&p_right) : VariantInternal::get_float(&p_right);
            r_ret = std::pow(base, exp);
            r_valid = true;
            return;
        }
        return;
    }

    // 11. Bitwise operations
    if (p_op == Variant::OP_BIT_AND && t1 == Variant::INT && t2 == Variant::INT) {
        r_ret = VariantInternal::get_int(&p_left) & VariantInternal::get_int(&p_right);
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_BIT_OR && t1 == Variant::INT && t2 == Variant::INT) {
        r_ret = VariantInternal::get_int(&p_left) | VariantInternal::get_int(&p_right);
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_BIT_XOR && t1 == Variant::INT && t2 == Variant::INT) {
        r_ret = VariantInternal::get_int(&p_left) ^ VariantInternal::get_int(&p_right);
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_BIT_NEGATE && t1 == Variant::INT) {
        r_ret = ~VariantInternal::get_int(&p_left);
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_SHIFT_LEFT && t1 == Variant::INT && t2 == Variant::INT) {
        i64 shift = VariantInternal::get_int(&p_right);
        if (shift < 0 || shift >= 64) {
            r_valid = false;
            return;
        }
        r_ret = VariantInternal::get_int(&p_left) << shift;
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_SHIFT_RIGHT && t1 == Variant::INT && t2 == Variant::INT) {
        i64 shift = VariantInternal::get_int(&p_right);
        if (shift < 0 || shift >= 64) {
            r_valid = false;
            return;
        }
        r_ret = VariantInternal::get_int(&p_left) >> shift;
        r_valid = true;
        return;
    }

    // 12. Logical operations
    if (p_op == Variant::OP_AND) {
        r_ret = p_left.booleanize() && p_right.booleanize();
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_OR) {
        r_ret = p_left.booleanize() || p_right.booleanize();
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_XOR) {
        r_ret = p_left.booleanize() ^ p_right.booleanize();
        r_valid = true;
        return;
    }
    if (p_op == Variant::OP_NOT) {
        r_ret = !p_left.booleanize();
        r_valid = true;
        return;
    }

    // 13. Containment: IN
    if (p_op == Variant::OP_IN) {
        if (t2 == Variant::ARRAY) {
            r_ret = VariantInternal::get_array(&p_right).has(p_left);
            r_valid = true;
            return;
        }
        if (t2 == Variant::DICTIONARY) {
            r_ret = VariantInternal::get_dictionary(&p_right).has(p_left);
            r_valid = true;
            return;
        }
        if (t2 == Variant::STRING && t1 == Variant::STRING) {
            r_ret = VariantInternal::get_string(&p_right).contains(VariantInternal::get_string(&p_left).as_string_view());
            r_valid = true;
            return;
        }
        if (t2 == Variant::PACKED_STRING_ARRAY && t1 == Variant::STRING) {
            const auto &arr = VariantInternal::get_packed_array<String>(&p_right);
            for (usize i = 0; i < arr.size(); ++i) {
                if (arr[i] == VariantInternal::get_string(&p_left)) {
                    r_ret = true;
                    r_valid = true;
                    return;
                }
            }
            r_ret = false;
            r_valid = true;
            return;
        }
        if (t2 == Variant::PACKED_INT32_ARRAY && (t1 == Variant::INT || t1 == Variant::FLOAT)) {
            const auto &arr = VariantInternal::get_packed_array<i32>(&p_right);
            i32 target = (i32)p_left.operator i64();
            for (usize i = 0; i < arr.size(); ++i) {
                if (arr[i] == target) {
                    r_ret = true;
                    r_valid = true;
                    return;
                }
            }
            r_ret = false;
            r_valid = true;
            return;
        }
        return;
    }
}

} // namespace Beyota
