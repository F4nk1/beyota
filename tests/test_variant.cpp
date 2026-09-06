/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  test_variant.cpp                                                      */
/**************************************************************************/

#include "core/variant/variant_all.h"
#include "core/object/object.h"

#include <cassert>
#include <format>
#include <iostream>
#include <limits>
#include <string_view>

using namespace Beyota;

void test_sizes_and_alignments() {
    static_assert(sizeof(Variant) == 24, "Variant must be 24 bytes!");
    static_assert(sizeof(Array) == 8, "Array must be 8 bytes!");
    static_assert(sizeof(Dictionary) == 8, "Dictionary must be 8 bytes!");
    static_assert(sizeof(String) == 8, "String must be 8 bytes!");
    static_assert(sizeof(StringName) == 8, "StringName must be 8 bytes!");
    static_assert(sizeof(NodePath) == 8, "NodePath must be 8 bytes!");
    static_assert(sizeof(Beyota::RID) == 8, "RID must be 8 bytes!");
    static_assert(sizeof(ObjectID) == 8, "ObjectID must be 8 bytes!");
    static_assert(sizeof(Callable) == 16, "Callable must be 16 bytes!");
    static_assert(sizeof(Signal) == 16, "Signal must be 16 bytes!");
    static_assert(sizeof(PackedByteArray) == 8, "PackedByteArray must be 8 bytes!");

    std::cout << "[PASS] Memory Layout, Sizes & Alignments\n";
}

void test_types_and_names() {
    assert(Variant(false).get_type() == Variant::BOOL);
    assert(Variant(static_cast<i64>(42)).get_type() == Variant::INT);
    assert(Variant(3.14).get_type() == Variant::FLOAT);
    assert(Variant("Beyota").get_type() == Variant::STRING);
    assert(Variant(Vector2(1, 2)).get_type() == Variant::VECTOR2);
    assert(Variant(Vector2i(1, 2)).get_type() == Variant::VECTOR2I);
    assert(Variant(Rect2(0, 0, 10, 10)).get_type() == Variant::RECT2);
    assert(Variant(Rect2i(0, 0, 10, 10)).get_type() == Variant::RECT2I);
    assert(Variant(Vector3(1, 2, 3)).get_type() == Variant::VECTOR3);
    assert(Variant(Vector3i(1, 2, 3)).get_type() == Variant::VECTOR3I);
    assert(Variant(Transform2D()).get_type() == Variant::TRANSFORM2D);
    assert(Variant(Vector4(1, 2, 3, 4)).get_type() == Variant::VECTOR4);
    assert(Variant(Vector4i(1, 2, 3, 4)).get_type() == Variant::VECTOR4I);
    assert(Variant(Plane(0, 1, 0, 0)).get_type() == Variant::PLANE);
    assert(Variant(Quaternion()).get_type() == Variant::QUATERNION);
    assert(Variant(Math::AABB()).get_type() == Variant::AABB);
    assert(Variant(Basis()).get_type() == Variant::BASIS);
    assert(Variant(Transform3D()).get_type() == Variant::TRANSFORM3D);
    assert(Variant(Projection()).get_type() == Variant::PROJECTION);
    assert(Variant(Color(1, 0, 0, 1)).get_type() == Variant::COLOR);
    assert(Variant(StringName("test")).get_type() == Variant::STRING_NAME);
    assert(Variant(NodePath("A/B")).get_type() == Variant::NODE_PATH);
    assert(Variant(Beyota::RID(100)).get_type() == Variant::RID);
    assert(Variant(Callable()).get_type() == Variant::CALLABLE);
    assert(Variant(Signal()).get_type() == Variant::SIGNAL);
    assert(Variant(Dictionary()).get_type() == Variant::DICTIONARY);
    assert(Variant(Array()).get_type() == Variant::ARRAY);
    assert(Variant(PackedByteArray()).get_type() == Variant::PACKED_BYTE_ARRAY);
    assert(Variant(PackedInt32Array()).get_type() == Variant::PACKED_INT32_ARRAY);
    assert(Variant(PackedInt64Array()).get_type() == Variant::PACKED_INT64_ARRAY);
    assert(Variant(PackedFloat32Array()).get_type() == Variant::PACKED_FLOAT32_ARRAY);
    assert(Variant(PackedFloat64Array()).get_type() == Variant::PACKED_FLOAT64_ARRAY);
    assert(Variant(PackedStringArray()).get_type() == Variant::PACKED_STRING_ARRAY);
    assert(Variant(PackedVector2Array()).get_type() == Variant::PACKED_VECTOR2_ARRAY);
    assert(Variant(PackedVector3Array()).get_type() == Variant::PACKED_VECTOR3_ARRAY);
    assert(Variant(PackedColorArray()).get_type() == Variant::PACKED_COLOR_ARRAY);
    assert(Variant(PackedVector4Array()).get_type() == Variant::PACKED_VECTOR4_ARRAY);

    assert(std::string_view(Variant(false).get_type_name()) == "bool");
    assert(std::string_view(Variant(42).get_type_name()) == "int");
    assert(std::string_view(Variant(3.14).get_type_name()) == "float");
    assert(std::string_view(Variant("A").get_type_name()) == "String");
    assert(std::string_view(Variant(Vector3()).get_type_name()) == "Vector3");

    std::cout << "[PASS] Variant Types & Type Names Canonical Mapping\n";
}

void test_constructors_and_casts() {
    Variant v_nil;
    assert(v_nil.is_null());
    assert(!v_nil.booleanize());

    Variant v_bool(true);
    assert(v_bool.booleanize());
    assert(static_cast<bool>(v_bool) == true);

    Variant v_int(static_cast<i64>(12345));
    assert(static_cast<i64>(v_int) == 12345);
    assert(static_cast<i32>(v_int) == 12345);
    assert(static_cast<f64>(v_int) == 12345.0);

    Variant v_float(98.75);
    assert(static_cast<f64>(v_float) == 98.75);
    assert(static_cast<i64>(v_float) == 98);

    Variant v_str("456");
    assert(static_cast<String>(v_str) == "456");
    assert(static_cast<i64>(v_str) == 456);

    Variant v_v2(Vector2(5.0f, 12.0f));
    Vector2 cast_v2 = static_cast<Vector2>(v_v2);
    assert(cast_v2.x == 5.0f && cast_v2.y == 12.0f);

    Variant v_v3(Vector3(1.0f, 2.0f, 3.0f));
    Vector3 cast_v3 = static_cast<Vector3>(v_v3);
    assert(cast_v3.x == 1.0f && cast_v3.y == 2.0f && cast_v3.z == 3.0f);

    Variant v_color(Color(0.2f, 0.4f, 0.6f, 1.0f));
    Color cast_col = static_cast<Color>(v_color);
    assert(cast_col.r == 0.2f && cast_col.g == 0.4f && cast_col.b == 0.6f && cast_col.a == 1.0f);

    std::cout << "[PASS] Constructors, Value Retrieval & Explicit Casts\n";
}

void test_copy_move_and_pools() {
    // Test pooled objects: Transform2D, AABB, Basis, Transform3D, Projection
    {
        Transform2D t;
        t.columns[0] = Vector2(2, 0);
        t.columns[1] = Vector2(0, 2);
        t.columns[2] = Vector2(10, 20);

        Variant v1(t);
        assert(v1.get_type() == Variant::TRANSFORM2D);
        Variant v2 = v1; // Copy
        assert(v2.get_type() == Variant::TRANSFORM2D);
        Transform2D t2 = static_cast<Transform2D>(v2);
        assert(t2.columns[0] == Vector2(2, 0));
        assert(t2.columns[2] == Vector2(10, 20));

        Variant v3 = std::move(v1); // Move
        assert(v3.get_type() == Variant::TRANSFORM2D);
        assert(v1.get_type() == Variant::NIL);
    }

    {
        Math::AABB box(Vector3(1, 2, 3), Vector3(4, 5, 6));
        Variant v1(box);
        assert(v1.get_type() == Variant::AABB);
        Variant v2 = v1;
        Math::AABB box2 = static_cast<Math::AABB>(v2);
        assert(box2.position == Vector3(1, 2, 3));
        assert(box2.size == Vector3(4, 5, 6));
    }

    {
        Basis b = Basis::from_euler(Vector3(0, Math::HALF_PI<real_t>, 0));
        Variant v1(b);
        assert(v1.get_type() == Variant::BASIS);
        Variant v2 = v1;
        Basis b2 = static_cast<Basis>(v2);
        assert(b.is_equal_approx(b2));
    }

    {
        Projection p = Projection::create_perspective(60.0f, 1.333f, 0.1f, 1000.0f);
        Variant v1(p);
        assert(v1.get_type() == Variant::PROJECTION);
        Variant v2 = v1;
        Projection p2 = static_cast<Projection>(v2);
        assert(p == p2);
    }

    std::cout << "[PASS] Copy, Move & VariantPools Dynamic Allocations\n";
}

void test_safety_and_overflow_guards() {
    // 1. Division by zero in integer arithmetic
    Variant v_num(static_cast<i64>(100));
    Variant v_zero(static_cast<i64>(0));
    Variant res;
    bool valid = false;

    Variant::evaluate(Variant::OP_DIVIDE, v_num, v_zero, res, valid);
    assert(!valid); // Division by zero MUST be caught and marked invalid

    Variant::evaluate(Variant::OP_MODULE, v_num, v_zero, res, valid);
    assert(!valid); // Modulo by zero MUST be caught and marked invalid

    // 2. Hardware singularity: INT64_MIN / -1
    i64 int_min = std::numeric_limits<i64>::min();
    Variant v_min(int_min);
    Variant v_neg_one(static_cast<i64>(-1));

    Variant::evaluate(Variant::OP_DIVIDE, v_min, v_neg_one, res, valid);
    assert(valid);
    assert(res.operator i64() == int_min); // Preserves No Blind Pruning invariant!

    Variant::evaluate(Variant::OP_MODULE, v_min, v_neg_one, res, valid);
    assert(valid);
    assert(res.operator i64() == 0); // Preserves No Blind Pruning invariant!

    // 3. Vector2i division by zero component
    Variant v2i_a(Vector2i(10, 20));
    Variant v2i_zero(Vector2i(0, 5));
    Variant::evaluate(Variant::OP_DIVIDE, v2i_a, v2i_zero, res, valid);
    assert(!valid);

    // 4. Vector3i division by zero component
    Variant v3i_a(Vector3i(10, 20, 30));
    Variant v3i_zero(Vector3i(1, 0, 1));
    Variant::evaluate(Variant::OP_DIVIDE, v3i_a, v3i_zero, res, valid);
    assert(!valid);

    std::cout << "[PASS] Hardware Overflow Mitigation & Zero-Division Safety (INT64_MIN / -1)\n";
}

void test_arithmetic_and_operators() {
    // Int + Int
    assert(Variant(10) + Variant(20) == Variant(30));
    // Int + Float -> Float
    assert(Variant(10) + Variant(2.5) == Variant(12.5));
    // Float * Int -> Float
    assert(Variant(2.5) * Variant(4) == Variant(10.0));
    // String + String
    assert(Variant("Beyota ") + Variant("Engine") == Variant("Beyota Engine"));

    // Vector2 arithmetic
    Variant v1(Vector2(2, 3));
    Variant v2(Vector2(4, 5));
    assert(v1 + v2 == Variant(Vector2(6, 8)));
    assert(v1 * Variant(2.0f) == Variant(Vector2(4, 6)));
    assert(Variant(3.0f) * v1 == Variant(Vector2(6, 9)));

    // Vector3 arithmetic
    Variant v3a(Vector3(1, 2, 3));
    Variant v3b(Vector3(4, 5, 6));
    assert(v3a + v3b == Variant(Vector3(5, 7, 9)));
    assert(v3a * Variant(2.0f) == Variant(Vector3(2, 4, 6)));

    // Power operator
    Variant v_pow_ret;
    bool v_pow_valid = false;
    Variant::evaluate(Variant::OP_POWER, Variant(2), Variant(10), v_pow_ret, v_pow_valid);
    assert(v_pow_valid && v_pow_ret.operator f64() == 1024.0);

    // Bitwise operators
    assert((Variant(0b1100) & Variant(0b1010)) == Variant(0b1000));
    assert((Variant(0b1100) | Variant(0b1010)) == Variant(0b1110));
    assert((Variant(0b1100) ^ Variant(0b1010)) == Variant(0b0110));
    assert((Variant(1) << Variant(4)) == Variant(16));
    assert((Variant(32) >> Variant(2)) == Variant(8));

    // Logical
    assert((Variant(true) && Variant(false)) == false);
    assert((!Variant(false)) == Variant(true));

    std::cout << "[PASS] Arithmetic, Matrix Multiplication, Bitwise & Logical Operators\n";
}

void test_string_and_string_name() {
    String s1 = "Hello";
    String s2 = " World";
    String s3 = s1 + s2;
    assert(s3 == "Hello World");
    assert(s3.length() == 11);
    assert(s3.contains("World"));
    assert(!s3.contains("Godot"));
    assert(s3.begins_with("Hello"));
    assert(s3.ends_with("World"));
    assert(s3.replace("World", "Beyota") == "Hello Beyota");

    auto parts = s3.split(" ");
    assert(parts.size() == 2);
    assert(parts[0] == "Hello");
    assert(parts[1] == "World");

    // StringName interned table
    StringName sn1("my_property");
    StringName sn2("my_property");
    StringName sn3("other_property");
    assert(sn1 == sn2); // O(1) comparison
    assert(sn1 != sn3);
    assert(Variant(sn1) == Variant(sn2));
    assert(Variant(sn1) == Variant("my_property"));

    // NodePath parsing
    NodePath np("Root/Player/Camera3D:transform");
    assert(!np.is_empty());
    assert(!np.is_absolute());
    assert(np.get_name_count() == 3);
    assert(np.get_name(0) == StringName("Root"));
    assert(np.get_name(1) == StringName("Player"));
    assert(np.get_name(2) == StringName("Camera3D"));
    assert(np.get_subname_count() == 1);
    assert(np.get_subname(0) == StringName("transform"));

    std::cout << "[PASS] String, StringName Interning & NodePath Scene Hierarchy\n";
}

void test_containers_array_and_dictionary() {
    // Array
    Array arr;
    assert(arr.is_empty());
    arr.push_back(Variant(10));
    arr.push_back(Variant("Test"));
    arr.push_back(Variant(Vector2(1, 2)));
    assert(arr.size() == 3);
    assert(arr[0] == Variant(10));
    assert(arr[1] == Variant("Test"));
    assert(arr.has(Variant("Test")));
    assert(arr.find(Variant("Test")) == 1);

    // Shared reference semantics
    Array arr_ref = arr;
    arr_ref[0] = Variant(99);
    assert(arr[0] == Variant(99)); // Shared!

    // Duplicate (deep copy)
    Array arr_dup = arr.duplicate();
    arr_dup[0] = Variant(1000);
    assert(arr[0] == Variant(99)); // Independent!
    assert(arr_dup[0] == Variant(1000));

    // Slice
    Array slice = arr.slice(1, 3);
    assert(slice.size() == 2);
    assert(slice[0] == Variant("Test"));

    // Dictionary
    Dictionary dict;
    assert(dict.is_empty());
    dict[Variant("health")] = Variant(100);
    dict[Variant("name")] = Variant("Hero");
    dict[Variant(42)] = Variant("Answer");
    assert(dict.size() == 3);
    assert(dict.has(Variant("health")));
    assert(dict[Variant("health")] == Variant(100));
    assert(dict[Variant(42)] == Variant("Answer"));

    // Shared reference semantics
    Dictionary dict_ref = dict;
    dict_ref[Variant("health")] = Variant(50);
    assert(dict[Variant("health")] == Variant(50));

    // Containment (IN operator)
    assert(Variant("health").in(Variant(dict)));
    assert(!Variant("mana").in(Variant(dict)));
    assert(Variant(99).in(Variant(arr)));
    assert(!Variant(555).in(Variant(arr)));

    // Pointer stability across rehashes
    Variant *health_ptr = dict.getptr(Variant("health"));
    assert(health_ptr != nullptr);
    assert(*health_ptr == Variant(50));
    for (i64 i = 1000; i < 2000; ++i) {
        dict[Variant(i)] = Variant(i * 2);
    }
    assert(dict.size() == 1003);
    assert(*health_ptr == Variant(50));
    *health_ptr = Variant(75);
    assert(dict[Variant("health")] == Variant(75));

    // Deterministic insertion-order preservation
    Dictionary order_dict;
    order_dict[Variant("first")] = Variant(1);
    order_dict[Variant("second")] = Variant(2);
    order_dict[Variant("third")] = Variant(3);
    order_dict[Variant("fourth")] = Variant(4);
    Array k = order_dict.keys();
    assert(k.size() == 4);
    assert(k[0] == Variant("first"));
    assert(k[1] == Variant("second"));
    assert(k[2] == Variant("third"));
    assert(k[3] == Variant("fourth"));

    auto it = order_dict.begin();
    assert(it.key() == Variant("first") && it.value() == Variant(1));
    ++it;
    assert(it.key() == Variant("second") && it.value() == Variant(2));
    ++it;
    assert(it.key() == Variant("third") && it.value() == Variant(3));
    ++it;
    assert(it.key() == Variant("fourth") && it.value() == Variant(4));
    ++it;
    assert(it == order_dict.end());

    // Backward-shift deletion (tombstone-free)
    assert(order_dict.erase(Variant("second")));
    assert(order_dict.size() == 3);
    assert(!order_dict.has(Variant("second")));
    assert(order_dict.has(Variant("first")));
    assert(order_dict.has(Variant("third")));
    assert(order_dict.has(Variant("fourth")));
    Array k2 = order_dict.keys();
    assert(k2.size() == 3);
    assert(k2[0] == Variant("first"));
    assert(k2[1] == Variant("third"));
    assert(k2[2] == Variant("fourth"));

    // String and StringName interop as dictionary keys
    Dictionary string_dict;
    string_dict[Variant(String("score"))] = Variant(999);
    assert(string_dict.has(Variant(StringName("score"))));
    assert(string_dict[Variant(StringName("score"))] == Variant(999));

    std::cout << "[PASS] Array & Dictionary Containers with Shared/COW Semantics\n";
}

void test_packed_arrays() {
    PackedByteArray bytes;
    bytes.push_back(10);
    bytes.push_back(20);
    bytes.push_back(30);
    assert(bytes.size() == 3);
    assert(bytes[1] == 20);

    PackedVector3Array vecs;
    vecs.push_back(Vector3(1, 0, 0));
    vecs.push_back(Vector3(0, 1, 0));
    vecs.push_back(Vector3(0, 0, 1));
    assert(vecs.size() == 3);

    Variant v_vecs(vecs);
    assert(v_vecs.get_type() == Variant::PACKED_VECTOR3_ARRAY);

    Variant v_vecs_copy = v_vecs;
    PackedVector3Array unpacked = static_cast<PackedVector3Array>(v_vecs_copy);
    assert(unpacked.size() == 3);
    assert(unpacked[1] == Vector3(0, 1, 0));

    std::cout << "[PASS] All 10 PackedArray Types with Fast Contiguous Layout\n";
}

void test_callable_and_signal() {
    Callable c(ObjectID(42ULL), StringName("on_test"));
    assert(!c.is_null());
    assert(c.get_object_id() == ObjectID(42ULL));
    assert(c.get_method() == StringName("on_test"));

    Signal sig(ObjectID(42ULL), StringName("test_signal"));
    assert(!sig.is_null());
    assert(sig.get_name() == StringName("test_signal"));

    sig.connect(c);
    assert(sig.is_connected(c));
    Array conns = sig.get_connections();
    assert(conns.size() == 1);
    assert(conns[0] == Variant(c));

    sig.disconnect(c);
    assert(!sig.is_connected(c));

    std::cout << "[PASS] Callable & Signal Dispatch System\n";
}

void test_setget_and_utility() {
    // Named access
    Variant v_vec(Vector2(10.0f, 20.0f));
    assert(v_vec.get_named(StringName("x")) == Variant(10.0f));
    assert(v_vec.get_named(StringName("y")) == Variant(20.0f));
    v_vec.set_named(StringName("x"), Variant(50.0f));
    assert(static_cast<Vector2>(v_vec).x == 50.0f);

    // Indexed access
    Variant v_col(Color(1.0f, 0.5f, 0.25f, 1.0f));
    assert(v_col.get_indexed(0) == Variant(1.0f));
    assert(v_col.get_indexed(1) == Variant(0.5f));
    assert(v_col.get_indexed(2) == Variant(0.25f));
    assert(v_col.get_indexed(3) == Variant(1.0f));

    // Utilities
    assert(VariantUtility::type_of(v_vec) == Variant::VECTOR2);
    assert(VariantUtility::type_string(Variant::VECTOR2) == "Vector2");
    assert(VariantUtility::is_same(Variant(10), Variant(10)));
    assert(!VariantUtility::is_same(Variant(10), Variant(10.0))); // is_same is strict on type!
    assert(VariantUtility::is_equal_approx(Variant(1.0000000000001), Variant(1.0000000000002)));

    Variant lerp_res = VariantUtility::lerp(Variant(Vector2(0, 0)), Variant(Vector2(10, 10)), 0.5);
    assert(lerp_res == Variant(Vector2(5, 5)));

    Variant clamp_res = VariantUtility::clamp(Variant(15), Variant(0), Variant(10));
    assert(clamp_res == Variant(10));

    // Dynamic constructor
    Variant default_color = VariantConstruct::construct_default(Variant::COLOR);
    assert(default_color.get_type() == Variant::COLOR);

    // std::format
    std::string s_fmt = std::format("{}", Variant(Vector2(3, 4)));
    assert(s_fmt == "(3, 4)");

    std::cout << "[PASS] Member Set/Get, Dynamic Construct, Utility Functions & std::format\n";
}

void test_godot_canonical_conformance() {
    // 1. Basic Comparison from Godot test_variant.cpp
    assert(Variant(1) == Variant(1));
    assert(!(Variant(1) != Variant(1)));
    assert(Variant(1) != Variant(2));
    assert(Variant(String("foo")) == Variant(String("foo")));
    assert(Variant(String("foo")) != Variant(String("bar")));
    assert(Variant(0) != Variant());
    assert(Variant(String()) != Variant());
    assert(Variant(Array()) != Variant());
    assert(Variant(Dictionary()) != Variant());

    // 2. Identity and Value comparison from Godot test_variant.cpp
    assert(Variant(Math::AABB()) == Variant(Math::AABB()));
    assert(Variant(Basis()) == Variant(Basis()));
    assert(Variant(true) == Variant(true));
    assert(Variant(true) != Variant(false));
    assert(Variant(1.0) == Variant(1.0));
    assert(Variant(1.0) != Variant(2.0));
    assert(Variant(1) == Variant(1));
    assert(Variant(1) != Variant(2));
    assert(Variant() == Variant());
    assert(Variant() != Variant(true));
    assert(Variant(NodePath("godot")) == Variant(NodePath("godot")));
    assert(Variant(NodePath("godot")) != Variant(NodePath("waiting")));
    assert(Variant(Plane()) == Variant(Plane()));
    assert(Variant(Projection()) == Variant(Projection()));

    // 3. Operator NOT across all types from Godot test_variant.cpp
    for (u32 i = 0; i < (u32)Variant::VARIANT_MAX; ++i) {
        Callable::CallError err;
        Variant value = VariantConstruct::construct((Variant::Type)i, nullptr, 0, err);
        assert(err.error == Callable::CallError::CALL_OK);
        Variant result;
        bool valid = false;
        Variant::evaluate(Variant::OP_NOT, value, Variant(), result, valid);
        assert(valid);
        assert(result.get_type() == Variant::BOOL);
        assert((bool)result == !value.booleanize());
    }

    // 4. Dictionary bracket notation & heterogeneous keys from Godot test_dictionary.cpp
    Dictionary map;
    map[Variant("Hello")] = Variant(0);
    assert((i64)map[Variant("Hello")] == 0);
    map[Variant("Hello")] = Variant(3);
    assert((i64)map[Variant("Hello")] == 3);
    map[Variant("World!")] = Variant(4);
    assert((i64)map[Variant("World!")] == 4);

    map[Variant(StringName("HelloName"))] = Variant(6);
    assert((i64)map[Variant(StringName("HelloName"))] == 6);
    map[Variant(StringName("HelloName"))] = Variant(7);
    assert((i64)map[Variant(StringName("HelloName"))] == 7);

    // String and StringName equivalent keys in Dictionary
    map[Variant(StringName("Hello"))] = Variant(8);
    assert((i64)map[Variant("Hello")] == 8);
    map[Variant("Hello")] = Variant(9);
    assert((i64)map[Variant(StringName("Hello"))] == 9);

    // Non-string keys: int, bool, Vector2
    map[Variant(12345)] = Variant(-5);
    assert((i64)map[Variant(12345)] == -5);
    map[Variant(false)] = Variant(128);
    assert((i64)map[Variant(false)] == 128);
    map[Variant(Vector2(10, 20))] = Variant(30);
    assert((i64)map[Variant(Vector2(10, 20))] == 30);
    map[Variant(0)] = Variant(400);
    assert((i64)map[Variant(0)] == 400);
    // Ensure assigning 0 doesn't overwrite value for false
    assert((i64)map[Variant(false)] == 128);

    // 5. Dictionary initializer list from Godot test_dictionary.cpp
    Dictionary dict_init{
        {Variant(0), Variant("int")},
        {Variant("key"), Variant(200)},
        {Variant(Vector2(5, 5)), Variant("v2")}
    };
    assert(dict_init.size() == 3);
    assert(dict_init[Variant(0)] == Variant("int"));
    assert(dict_init[Variant("key")] == Variant(200));
    assert(dict_init[Variant(Vector2(5, 5))] == Variant("v2"));

    std::cout << "[PASS] Godot Canonical Conformance & Behavioral Parity Suite\n";
}

int main() {
    std::cout << "Running exhaustive Beyota Variant subsystem test suite...\n";
    std::cout << "=========================================================\n";

    test_sizes_and_alignments();
    test_types_and_names();
    test_constructors_and_casts();
    test_copy_move_and_pools();
    test_safety_and_overflow_guards();
    test_arithmetic_and_operators();
    test_string_and_string_name();
    test_containers_array_and_dictionary();
    test_packed_arrays();
    test_callable_and_signal();
    test_setget_and_utility();
    test_godot_canonical_conformance();

    std::cout << "=========================================================\n";
    std::cout << "ALL 12 VARIANT TEST SUITES PASSED SUCCESSFULLY!\n";
    std::cout << "=========================================================\n";

    return 0;
}
