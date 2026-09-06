/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  test_math.cpp                                                         */
/**************************************************************************/

#include "core/math/math_all.h"

#include <cassert>
#include <iostream>
#include <format>
#include <string_view>
#include <limits>
#include <type_traits>

using namespace Beyota;
using namespace Beyota::Math;

void test_math_funcs() {
    // division_no_overflow & modulo_no_overflow INT_MIN / -1
    i32 min_val = std::numeric_limits<i32>::min();
    assert(Math::division_no_overflow(min_val, -1) == min_val);
    assert(Math::modulo_no_overflow(min_val, -1) == 0);
    assert(Math::division_round_up(7, 3) == 3);
    assert(Math::division_round_up(6, 3) == 2);

    // Posmod and wrap
    assert(Math::posmod(-1, 5) == 4);
    assert(Math::is_equal_approx(Math::fposmod((real_t)-1.0, (real_t)5.0), (real_t)4.0));
    assert(Math::wrapi(7, 0, 5) == 2);
    assert(Math::is_equal_approx(Math::wrapf((real_t)7.0, (real_t)0.0, (real_t)5.0), (real_t)2.0));

    // Snapped
    assert(Math::is_equal_approx(Math::snapped((real_t)3.14159, (real_t)0.01), (real_t)3.14));

    // Float conversions
    u16 half = Math::make_half_float(1.0f);
    float back = Math::half_to_float(half);
    assert(Math::is_equal_approx(back, 1.0f));

    std::cout << "[PASS] Math functions & overflow safety\n";
}

void test_vector2() {
    Vector2 v1(3.0f, 4.0f);
    assert(Math::is_equal_approx(v1.length(), 5.0f));
    assert(Math::is_equal_approx(v1.length_squared(), 25.0f));

    Vector2 normalized = v1.normalized();
    assert(Math::is_equal_approx(normalized.length(), 1.0f));

    Vector2 rotated = Vector2(1.0f, 0.0f).rotated(Math::HALF_PI<real_t>);
    assert(Math::is_equal_approx(rotated.x, 0.0f));
    assert(Math::is_equal_approx(rotated.y, 1.0f));

    // Formatter
    std::string s = std::format("{}", v1);
    assert(s == "(3, 4)");

    std::cout << "[PASS] Vector2\n";
}

void test_vector2i() {
    Vector2i vi(1000000, 1000000);
    // Invariant: length_squared returns i64 to prevent integer overflow
    static_assert(std::is_same_v<decltype(vi.length_squared()), i64>);
    i64 lsq = vi.length_squared();
    assert(lsq == 2000000000000LL);

    // Invariant: explicit conversion required between discrete and continuous
    static_assert(!std::is_convertible_v<Vector2i, Vector2>);
    static_assert(!std::is_convertible_v<Vector2, Vector2i>);
    Vector2 vf = (Vector2)vi;
    assert(vf.x == 1000000.0f);

    Vector2i div = vi / 2;
    assert(div.x == 500000 && div.y == 500000);

    // Safe division: INT_MIN / -1
    Vector2i min_vec(std::numeric_limits<i32>::min(), std::numeric_limits<i32>::min());
    Vector2i safe_res = min_vec / -1;
    assert(safe_res.x == std::numeric_limits<i32>::min());

    // Formatter
    std::string s = std::format("{}", vi);
    assert(s == "(1000000, 1000000)");

    std::cout << "[PASS] Vector2i & safe integer math\n";
}

void test_vector3() {
    Vector3 v(1.0f, 2.0f, 2.0f);
    assert(Math::is_equal_approx(v.length(), 3.0f));

    Vector3 cross = Vector3(1, 0, 0).cross(Vector3(0, 1, 0));
    assert(cross == Vector3(0, 0, 1));

    // Rodrigues rotation
    Vector3 rotated = Vector3(1, 0, 0).rotated(Vector3(0, 0, 1), Math::HALF_PI<real_t>);
    assert(Math::is_equal_approx(rotated.x, 0.0f));
    assert(Math::is_equal_approx(rotated.y, 1.0f));

    // Octahedron encode/decode
    Vector2 oct = v.normalized().octahedron_encode();
    Vector3 v_dec = Vector3::octahedron_decode(oct);
    assert(v.normalized().is_equal_approx(v_dec));

    // Formatter
    std::string s = std::format("{}", v);
    assert(s == "(1, 2, 2)");

    std::cout << "[PASS] Vector3\n";
}

void test_vector3i() {
    Vector3i vi(1000000, 1000000, 1000000);
    static_assert(std::is_same_v<decltype(vi.length_squared()), i64>);
    assert(vi.length_squared() == 3000000000000LL);

    static_assert(!std::is_convertible_v<Vector3i, Vector3>);
    static_assert(!std::is_convertible_v<Vector3, Vector3i>);

    std::string s = std::format("{}", vi);
    assert(s == "(1000000, 1000000, 1000000)");

    std::cout << "[PASS] Vector3i\n";
}

void test_vector4_and_4i() {
    Vector4 v(1, 2, 3, 4);
    assert(Math::is_equal_approx(v.length_squared(), 30.0f));
    std::string s = std::format("{}", v);
    assert(s == "(1, 2, 3, 4)");

    Vector4i vi(1000000, 1000000, 1000000, 1000000);
    static_assert(std::is_same_v<decltype(vi.length_squared()), i64>);
    assert(vi.length_squared() == 4000000000000LL);

    static_assert(!std::is_convertible_v<Vector4i, Vector4>);
    static_assert(!std::is_convertible_v<Vector4, Vector4i>);

    std::cout << "[PASS] Vector4 & Vector4i\n";
}

void test_rect2_and_2i() {
    Rect2 r(Vector2(0, 0), Vector2(10, 20));
    assert(r.get_area() == 200.0f);
    assert(r.has_point(Vector2(5, 5)));
    assert(!r.has_point(Vector2(15, 5)));

    Rect2i ri(Vector2i(0, 0), Vector2i(1000000, 1000000));
    static_assert(std::is_same_v<decltype(ri.get_area()), i64>);
    assert(ri.get_area() == 1000000000000LL);

    static_assert(!std::is_convertible_v<Rect2i, Rect2>);
    static_assert(!std::is_convertible_v<Rect2, Rect2i>);

    std::cout << "[PASS] Rect2 & Rect2i\n";
}

void test_transform_2d() {
    Transform2D t(Math::HALF_PI<real_t>, Vector2(2.0f, 2.0f), (real_t)0.0, Vector2(10.0f, 20.0f));

    Vector2 pt(1.0f, 0.0f);
    Vector2 xformed = t.xform(pt);
    assert(Math::is_equal_approx(xformed.x, 10.0f));
    assert(Math::is_equal_approx(xformed.y, 22.0f));

    Transform2D inv = t.affine_inverse();
    Vector2 restored = inv.xform(xformed);
    assert(restored.is_equal_approx(pt));

    std::string s = std::format("{}", t);
    assert(!s.empty());

    std::cout << "[PASS] Transform2D\n";
}

void test_plane() {
    Plane p(Vector3(0, 1, 0), 5.0f);
    assert(p.has_point(Vector3(10, 5, 20)));
    assert(p.distance_to(Vector3(0, 8, 0)) == 3.0f);

    Vector3 proj = p.project(Vector3(0, 8, 0));
    assert(proj.is_equal_approx(Vector3(0, 5, 0)));

    std::string s = std::format("{}", p);
    assert(!s.empty());

    std::cout << "[PASS] Plane\n";
}

void test_quaternion() {
    Quaternion q1(Vector3(0, 1, 0), Math::HALF_PI<real_t>);
    Vector3 rotated = q1.xform(Vector3(1, 0, 0));
    assert(Math::is_equal_approx(rotated.x, 0.0f));
    assert(Math::is_equal_approx(rotated.z, -1.0f));

    Quaternion q2(Vector3(0, 1, 0), 0.0f);
    Quaternion slerped = q2.slerp(q1, 0.5f);
    Vector3 half_rotated = slerped.xform(Vector3(1, 0, 0));
    assert(Math::is_equal_approx(half_rotated.x, Math::INV_SQRT2<real_t>));
    assert(Math::is_equal_approx(half_rotated.z, -Math::INV_SQRT2<real_t>));

    std::string s = std::format("{}", q1);
    assert(!s.empty());

    std::cout << "[PASS] Quaternion\n";
}

void test_basis() {
    Basis b;
    b.rotate(Vector3(0, 1, 0), Math::HALF_PI<real_t>);
    Vector3 v = b.xform(Vector3(1, 0, 0));
    assert(Math::is_equal_approx(v.x, 0.0f));
    assert(Math::is_equal_approx(v.z, -1.0f));

    Basis inv = b.inverse();
    Vector3 restored = inv.xform(v);
    assert(restored.is_equal_approx(Vector3(1, 0, 0)));

    // Test non-orthogonal matrix inversion
    Basis non_ortho(
        1.5f, 0.5f, -0.2f,
        -0.3f, 2.0f, 0.4f,
        0.8f, -0.6f, 1.2f
    );
    Basis non_ortho_inv = non_ortho.inverse();
    Basis id = non_ortho * non_ortho_inv;
    for (usize i = 0; i < 3; ++i) {
        for (usize j = 0; j < 3; ++j) {
            real_t expected = (i == j) ? 1.0f : 0.0f;
            assert(Math::is_equal_approx(id.rows[i][j], expected));
        }
    }

    // Test all 6 Euler orders round-trip
    EulerOrder orders[] = {
        EulerOrder::XYZ, EulerOrder::XZY, EulerOrder::YXZ,
        EulerOrder::YZX, EulerOrder::ZXY, EulerOrder::ZYX
    };
    Vector3 euler_in(0.2f, 0.4f, 0.6f);
    for (EulerOrder order : orders) {
        Basis be(euler_in, order);
        Vector3 euler_out = be.get_euler(order);
        assert(euler_in.is_equal_approx(euler_out));
    }

    // Godot 348 Euler test vectors parity
    Vector3 godot_test_vectors[] = {
        Vector3(0.0, 0.0, 0.0), Vector3(0.5, 0.5, 0.5), Vector3(-0.5, -0.5, -0.5),
        Vector3(40.0, 40.0, 40.0), Vector3(-40.0, -40.0, -40.0),
        Vector3(0.0, 0.0, -90.0), Vector3(0.0, -90.0, 0.0), Vector3(-90.0, 0.0, 0.0),
        Vector3(0.0, 0.0, 90.0), Vector3(0.0, 90.0, 0.0), Vector3(90.0, 0.0, 0.0),
        Vector3(0.0, 0.0, -30.0), Vector3(0.0, -30.0, 0.0), Vector3(-30.0, 0.0, 0.0),
        Vector3(0.0, 0.0, 30.0), Vector3(0.0, 30.0, 0.0), Vector3(30.0, 0.0, 0.0),
        Vector3(0.5, 50.0, 20.0), Vector3(-0.5, -50.0, -20.0),
        Vector3(0.5, 0.0, 90.0), Vector3(0.5, 0.0, -90.0),
        Vector3(360.0, 360.0, 360.0), Vector3(-360.0, -360.0, -360.0),
        Vector3(-90.0, 60.0, -90.0), Vector3(90.0, 60.0, -90.0),
        Vector3(90.0, -60.0, -90.0), Vector3(-90.0, -60.0, -90.0),
        Vector3(-90.0, 60.0, 90.0), Vector3(90.0, 60.0, 90.0),
        Vector3(90.0, -60.0, 90.0), Vector3(-90.0, -60.0, 90.0),
        Vector3(60.0, 90.0, -40.0), Vector3(60.0, -90.0, -40.0),
        Vector3(-60.0, -90.0, -40.0), Vector3(-60.0, 90.0, 40.0),
        Vector3(60.0, 90.0, 40.0), Vector3(60.0, -90.0, 40.0),
        Vector3(-60.0, -90.0, 40.0), Vector3(-90.0, 90.0, -90.0),
        Vector3(90.0, 90.0, -90.0), Vector3(90.0, -90.0, -90.0),
        Vector3(-90.0, -90.0, -90.0), Vector3(-90.0, 90.0, 90.0),
        Vector3(90.0, 90.0, 90.0), Vector3(90.0, -90.0, 90.0),
        Vector3(20.0, 150.0, 30.0), Vector3(20.0, -150.0, 30.0),
        Vector3(-120.0, -150.0, 30.0), Vector3(-120.0, -150.0, -130.0),
        Vector3(120.0, -150.0, -130.0), Vector3(120.0, 150.0, -130.0),
        Vector3(120.0, 150.0, 130.0),
        Vector3(89.9, 0.0, 0.0), Vector3(-89.9, 0.0, 0.0),
        Vector3(0.0, 89.9, 0.0), Vector3(0.0, -89.9, 0.0),
        Vector3(0.0, 0.0, 89.9), Vector3(0.0, 0.0, -89.9)
    };

    for (EulerOrder order : orders) {
        for (const auto &deg : godot_test_vectors) {
            Vector3 orig_euler = deg / 180.0f * (real_t)Math::PI<real_t>;
            Basis to_rot = Basis::from_euler(orig_euler, order);
            Vector3 euler_from_rot = to_rot.get_euler(order);
            Basis rot_from_computed = Basis::from_euler(euler_from_rot, order);

            Basis res = to_rot.inverse() * rot_from_computed;
            assert((res.get_column(0) - Vector3(1, 0, 0)).length() <= 0.001f);
            assert((res.get_column(1) - Vector3(0, 1, 0)).length() <= 0.001f);
            assert((res.get_column(2) - Vector3(0, 0, 1)).length() <= 0.001f);
        }
    }

    std::string s = std::format("{}", b);
    assert(!s.empty());

    std::cout << "[PASS] Basis & all 6 Euler orders\n";
}

void test_aabb() {
    AABB box(Vector3(0, 0, 0), Vector3(2, 2, 2));
    assert(box.has_point(Vector3(1, 1, 1)));
    assert(!box.has_point(Vector3(3, 1, 1)));

    Vector3 ray_from(-1, 1, 1);
    Vector3 ray_dir(1, 0, 0);
    bool inside = false;
    Vector3 inters;
    Vector3 norm;
    assert(box.find_intersects_ray(ray_from, ray_dir, inside, &inters, &norm));
    assert(inters.is_equal_approx(Vector3(0, 1, 1)));
    assert(norm.is_equal_approx(Vector3(-1, 0, 0)));

    Vector3 e_from, e_to;
    box.get_edge(0, e_from, e_to);
    assert(e_from == Vector3(2, 0, 0));
    assert(e_to == Vector3(0, 0, 0));

    std::string s = std::format("{}", box);
    assert(!s.empty());

    std::cout << "[PASS] AABB\n";
}

void test_transform_3d() {
    Transform3D t;
    t.rotate(Vector3(0, 1, 0), Math::HALF_PI<real_t>);
    t.set_origin(Vector3(10, 20, 30));

    Vector3 pt(1, 0, 0);
    Vector3 xformed = t.xform(pt);
    assert(Math::is_equal_approx(xformed.x, 10.0f));
    assert(Math::is_equal_approx(xformed.y, 20.0f));
    assert(Math::is_equal_approx(xformed.z, 29.0f));

    Transform3D inv = t.affine_inverse();
    Vector3 restored = inv.xform(xformed);
    assert(restored.is_equal_approx(pt));

    // Affine inverse with non-uniform scale
    Basis non_uniform_basis = Basis::from_euler(Vector3(0.3f, -0.4f, 0.7f)) * Basis().scaled(Vector3(2.5f, 0.4f, 1.8f));
    Transform3D t_affine(non_uniform_basis, Vector3(14.2f, -7.8f, 103.5f));
    Transform3D inv_affine = t_affine.affine_inverse();
    Vector3 pt2(5.0f, -3.0f, 12.0f);
    assert(inv_affine.xform(t_affine.xform(pt2)).is_equal_approx(pt2));

    // Plane transformation and inverse transformation
    Plane plane(Vector3(1.0f, 2.0f, -3.0f).normalized(), 7.5f);
    Plane transformed_plane = t_affine.xform(plane);
    Plane restored_plane = t_affine.xform_inv(transformed_plane);
    assert(restored_plane.normal.is_equal_approx(plane.normal));
    assert(Math::is_equal_approx(restored_plane.d, plane.d));

    AABB box(Vector3(0, 0, 0), Vector3(2, 2, 2));
    AABB box_xformed = t.xform(box);
    assert(box_xformed.size.is_equal_approx(Vector3(2, 2, 2)));

    std::string s = std::format("{}", t);
    assert(!s.empty());

    std::cout << "[PASS] Transform3D\n";
}

void test_projection() {
    Projection p;
    p.set_perspective(60.0f, 16.0f / 9.0f, 1.0f, 100.0f);
    assert(Math::is_equal_approx(p.get_z_near(), 1.0f));
    assert(Math::is_equal_approx(p.get_z_far(), 100.0f));
    assert(!p.is_orthogonal());

    Projection ortho;
    ortho.set_orthogonal(-10.0f, 10.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    assert(ortho.is_orthogonal());

    // Invert test
    Projection inv = ortho.inverse();
    Projection identity = ortho * inv;
    for (usize i = 0; i < 4; ++i) {
        for (usize j = 0; j < 4; ++j) {
            real_t expected = (i == j) ? 1.0f : 0.0f;
            assert(Math::is_equal_approx(identity[i][j], expected));
        }
    }

    std::string s = std::format("{}", p);
    assert(!s.empty());

    std::cout << "[PASS] Projection\n";
}

void test_color() {
    Color red(1.0f, 0.0f, 0.0f, 1.0f);
    assert(red.to_rgba32() == 0xFF0000FF);
    assert(red.to_argb32() == 0xFFFF0000);
    assert(red.to_abgr32() == 0xFF0000FF);

    Color from_hex = Color::hex(0x00FF00FF);
    assert(from_hex == Color(0.0f, 1.0f, 0.0f, 1.0f));

    // HSV round trip
    Color c(0.2f, 0.5f, 0.8f, 1.0f);
    Color c_hsv = Color::from_hsv(c.get_h(), c.get_s(), c.get_v(), 1.0f);
    assert(c.is_equal_approx(c_hsv));

    // OKLab / OKHSL round trip
    Color ok_c = Color::from_ok_hsl(c.get_ok_hsl_h(), c.get_ok_hsl_s(), c.get_ok_hsl_l(), 1.0f);
    assert(c.is_equal_approx(ok_c));

    // Linear / sRGB
    Color linear = c.srgb_to_linear();
    Color srgb = linear.linear_to_srgb();
    assert(c.is_equal_approx(srgb));

    // HTML hex
    Color html_col = Color::html("#1a2b3c");
    assert(Color::html_is_valid("#1a2b3c"));
    auto buf = html_col.to_html_buffer(false);
    assert(std::string_view(buf.data()) == "#1a2b3c");

    // Blending
    Color base(1.0f, 0.0f, 0.0f, 0.5f);
    Color over(0.0f, 0.0f, 1.0f, 0.5f);
    Color blended = base.blend(over);
    assert(blended.a == 0.75f);

    std::string s = std::format("{}", c);
    assert(!s.empty());

    std::cout << "[PASS] Color & OKLab/HSV/sRGB\n";
}

void test_face3() {
    Face3 f(Vector3(0, 0, 0), Vector3(2, 0, 0), Vector3(0, 2, 0));
    assert(!f.is_degenerate());
    assert(Math::is_equal_approx(f.get_area(), 2.0f));

    Vector3 closest = f.get_closest_point_to(Vector3(0.5f, 0.5f, 5.0f));
    assert(closest.is_equal_approx(Vector3(0.5f, 0.5f, 0.0f)));

    Vector3 ray_inters;
    assert(f.intersects_ray(Vector3(0.5f, 0.5f, 5.0f), Vector3(0, 0, -1), &ray_inters));
    assert(ray_inters.is_equal_approx(Vector3(0.5f, 0.5f, 0.0f)));

    AABB box(Vector3(-1, -1, -1), Vector3(3, 3, 2));
    assert(f.intersects_aabb(box));

    std::string s = std::format("{}", f);
    assert(!s.empty());

    std::cout << "[PASS] Face3\n";
}

int main() {
    std::cout << "Running exhaustive Beyota math module test suite...\n";

    test_math_funcs();
    test_vector2();
    test_vector2i();
    test_vector3();
    test_vector3i();
    test_vector4_and_4i();
    test_rect2_and_2i();
    test_transform_2d();
    test_plane();
    test_quaternion();
    test_basis();
    test_aabb();
    test_transform_3d();
    test_projection();
    test_color();
    test_face3();

    std::cout << "\n============================================\n";
    std::cout << "ALL 16 TEST SUITES PASSED SUCCESSFULLY!\n";
    std::cout << "============================================\n";
    return 0;
}
