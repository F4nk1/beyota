/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector3.h                                                             */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector2.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct Vector3i;

struct [[nodiscard]] Vector3 {
    static const Vector3 ZERO;
    static const Vector3 ONE;
    static const Vector3 INF;
    static const Vector3 LEFT;
    static const Vector3 RIGHT;
    static const Vector3 UP;
    static const Vector3 DOWN;
    static const Vector3 FORWARD;
    static const Vector3 BACK;
    static const Vector3 MODEL_LEFT;
    static const Vector3 MODEL_RIGHT;
    static const Vector3 MODEL_TOP;
    static const Vector3 MODEL_BOTTOM;
    static const Vector3 MODEL_FRONT;
    static const Vector3 MODEL_REAR;

    real_t x{0};
    real_t y{0};
    real_t z{0};

    constexpr Vector3() = default;
    constexpr Vector3(real_t x, real_t y, real_t z) noexcept : x(x), y(y), z(z) {}
    explicit constexpr Vector3(const Vector3i &v) noexcept;

    [[nodiscard]] constexpr real_t &operator[](usize axis) noexcept {
        assert(axis < 3);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const real_t &operator[](usize axis) const noexcept {
        assert(axis < 3);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        return x < y ? (x < z ? Axis::X : Axis::Z) : (y < z ? Axis::Y : Axis::Z);
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        return x < y ? (y < z ? Axis::Z : Axis::Y) : (x < z ? Axis::Z : Axis::X);
    }

    [[nodiscard]] real_t length() const noexcept {
        return Math::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] constexpr real_t length_squared() const noexcept {
        return x * x + y * y + z * z;
    }

    constexpr void zero() noexcept {
        x = (real_t)0;
        y = (real_t)0;
        z = (real_t)0;
    }

    void normalize() noexcept {
        if (!is_finite()) {
            zero();
            return;
        }
        real_t l = length_squared();
        if (l == (real_t)0) {
            zero();
        } else {
            l = Math::sqrt(l);
            x /= l;
            y /= l;
            z /= l;
        }
    }

    [[nodiscard]] Vector3 normalized() const noexcept {
        Vector3 v = *this;
        v.normalize();
        return v;
    }

    [[nodiscard]] constexpr bool is_normalized() const noexcept {
        return Math::is_equal_approx(length_squared(), (real_t)1, (real_t)UNIT_EPSILON<real_t>);
    }

    [[nodiscard]] constexpr Vector3 inverse() const noexcept {
        return Vector3((real_t)1 / x, (real_t)1 / y, (real_t)1 / z);
    }

    [[nodiscard]] Vector3 limit_length(real_t len = (real_t)1) const noexcept {
        const real_t l = length();
        Vector3 v = *this;
        if (l > (real_t)0 && len < l) {
            v /= l;
            v *= len;
        }
        return v;
    }

    [[nodiscard]] Vector3 min(const Vector3 &other) const noexcept {
        return Vector3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }

    [[nodiscard]] Vector3 minf(real_t scalar) const noexcept {
        return Vector3(std::min(x, scalar), std::min(y, scalar), std::min(z, scalar));
    }

    [[nodiscard]] Vector3 max(const Vector3 &other) const noexcept {
        return Vector3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }

    [[nodiscard]] Vector3 maxf(real_t scalar) const noexcept {
        return Vector3(std::max(x, scalar), std::max(y, scalar), std::max(z, scalar));
    }

    [[nodiscard]] Vector3 clamp(const Vector3 &min_v, const Vector3 &max_v) const noexcept {
        return Vector3(std::clamp(x, min_v.x, max_v.x), std::clamp(y, min_v.y, max_v.y), std::clamp(z, min_v.z, max_v.z));
    }

    [[nodiscard]] Vector3 clampf(real_t min_v, real_t max_v) const noexcept {
        return Vector3(std::clamp(x, min_v, max_v), std::clamp(y, min_v, max_v), std::clamp(z, min_v, max_v));
    }

    void snap(const Vector3 &step) noexcept {
        x = Math::snapped(x, step.x);
        y = Math::snapped(y, step.y);
        z = Math::snapped(z, step.z);
    }

    void snapf(real_t step) noexcept {
        x = Math::snapped(x, step);
        y = Math::snapped(y, step);
        z = Math::snapped(z, step);
    }

    [[nodiscard]] Vector3 snapped(const Vector3 &step) const noexcept {
        Vector3 v = *this;
        v.snap(step);
        return v;
    }

    [[nodiscard]] Vector3 snappedf(real_t step) const noexcept {
        Vector3 v = *this;
        v.snapf(step);
        return v;
    }

    void rotate(const Vector3 &axis, real_t angle) noexcept {
        *this = rotated(axis, angle);
    }

    [[nodiscard]] Vector3 rotated(const Vector3 &axis, real_t angle) const noexcept {
        assert(axis.is_normalized());
        const real_t s = Math::sin(angle);
        const real_t c = Math::cos(angle);
        return *this * c + axis.cross(*this) * s + axis * (axis.dot(*this) * ((real_t)1.0 - c));
    }

    [[nodiscard]] constexpr Vector3 lerp(const Vector3 &to, real_t weight) const noexcept {
        return Vector3(
            Math::lerp(x, to.x, weight),
            Math::lerp(y, to.y, weight),
            Math::lerp(z, to.z, weight)
        );
    }

    [[nodiscard]] Vector3 slerp(const Vector3 &to, real_t weight) const noexcept {
        const real_t start_len_sq = length_squared();
        const real_t end_len_sq = to.length_squared();
        if (start_len_sq == (real_t)0 || end_len_sq == (real_t)0) {
            return lerp(to, weight);
        }
        const real_t start_len = Math::sqrt(start_len_sq);
        const real_t result_len = Math::lerp(start_len, Math::sqrt(end_len_sq), weight);
        const real_t ang = angle_to(to);
        if (ang < CMP_EPSILON<real_t>) {
            return *this;
        }
        const Vector3 axis = cross(to).normalized();
        return rotated(axis, ang * weight) * (result_len / start_len);
    }

    [[nodiscard]] constexpr Vector3 cubic_interpolate(const Vector3 &b, const Vector3 &pre_a, const Vector3 &post_b, real_t weight) const noexcept {
        return Vector3(
            Math::cubic_interpolate(x, b.x, pre_a.x, post_b.x, weight),
            Math::cubic_interpolate(y, b.y, pre_a.y, post_b.y, weight),
            Math::cubic_interpolate(z, b.z, pre_a.z, post_b.z, weight)
        );
    }

    [[nodiscard]] constexpr Vector3 cubic_interpolate_in_time(const Vector3 &b, const Vector3 &pre_a, const Vector3 &post_b, real_t weight, real_t b_t, real_t pre_a_t, real_t post_b_t) const noexcept {
        return Vector3(
            Math::cubic_interpolate_in_time(x, b.x, pre_a.x, post_b.x, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(y, b.y, pre_a.y, post_b.y, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(z, b.z, pre_a.z, post_b.z, weight, b_t, pre_a_t, post_b_t)
        );
    }

    [[nodiscard]] constexpr Vector3 bezier_interpolate(const Vector3 &c1, const Vector3 &c2, const Vector3 &end, real_t t) const noexcept {
        return Vector3(
            Math::bezier_interpolate(x, c1.x, c2.x, end.x, t),
            Math::bezier_interpolate(y, c1.y, c2.y, end.y, t),
            Math::bezier_interpolate(z, c1.z, c2.z, end.z, t)
        );
    }

    [[nodiscard]] constexpr Vector3 bezier_derivative(const Vector3 &c1, const Vector3 &c2, const Vector3 &end, real_t t) const noexcept {
        return Vector3(
            Math::bezier_derivative(x, c1.x, c2.x, end.x, t),
            Math::bezier_derivative(y, c1.y, c2.y, end.y, t),
            Math::bezier_derivative(z, c1.z, c2.z, end.z, t)
        );
    }

    [[nodiscard]] Vector3 move_toward(const Vector3 &to, real_t delta) const noexcept {
        const Vector3 vd = to - *this;
        const real_t len = vd.length();
        return (len <= delta || len < CMP_EPSILON<real_t>) ? to : (*this + vd / len * delta);
    }

    [[nodiscard]] Vector2 octahedron_encode() const noexcept {
        Vector3 n = *this;
        n /= Math::abs(n.x) + Math::abs(n.y) + Math::abs(n.z);
        Vector2 o;
        if (n.z >= (real_t)0.0) {
            o.x = n.x;
            o.y = n.y;
        } else {
            o.x = ((real_t)1.0 - Math::abs(n.y)) * (n.x >= (real_t)0.0 ? (real_t)1.0 : (real_t)-1.0);
            o.y = ((real_t)1.0 - Math::abs(n.x)) * (n.y >= (real_t)0.0 ? (real_t)1.0 : (real_t)-1.0);
        }
        o.x = o.x * (real_t)0.5 + (real_t)0.5;
        o.y = o.y * (real_t)0.5 + (real_t)0.5;
        return o;
    }

    [[nodiscard]] static Vector3 octahedron_decode(const Vector2 &oct) noexcept {
        Vector2 f(oct.x * (real_t)2.0 - (real_t)1.0, oct.y * (real_t)2.0 - (real_t)1.0);
        Vector3 n(f.x, f.y, (real_t)1.0 - Math::abs(f.x) - Math::abs(f.y));
        const real_t t = std::clamp(-n.z, (real_t)0.0, (real_t)1.0);
        n.x += (n.x >= (real_t)0.0 ? -t : t);
        n.y += (n.y >= (real_t)0.0 ? -t : t);
        return n.normalized();
    }

    [[nodiscard]] Vector2 octahedron_tangent_encode(real_t sign_val) const noexcept {
        Vector2 res = octahedron_encode();
        res.y /= (real_t)2.0;
        if (sign_val < (real_t)0.0) {
            res.y += (real_t)0.5;
        }
        return res;
    }

    [[nodiscard]] static Vector3 octahedron_tangent_decode(const Vector2 &oct, real_t *sign_val) noexcept {
        Vector2 o = oct;
        o.y *= (real_t)2.0;
        if (o.y >= (real_t)1.0) {
            o.y -= (real_t)1.0;
            if (sign_val) *sign_val = (real_t)-1.0;
        } else {
            if (sign_val) *sign_val = (real_t)1.0;
        }
        return octahedron_decode(o);
    }

    [[nodiscard]] constexpr Vector3 cross(const Vector3 &with) const noexcept {
        return Vector3(
            (y * with.z) - (z * with.y),
            (z * with.x) - (x * with.z),
            (x * with.y) - (y * with.x)
        );
    }

    [[nodiscard]] constexpr real_t dot(const Vector3 &with) const noexcept {
        return x * with.x + y * with.y + z * with.z;
    }

    [[nodiscard]] Vector3 get_any_perpendicular() const noexcept {
        return is_zero_approx() ? Vector3::ZERO : (cross((x < (real_t)0.9 && x > (real_t)-0.9) ? Vector3(1, 0, 0) : Vector3(0, 1, 0)).normalized());
    }

    [[nodiscard]] constexpr Vector3 abs() const noexcept {
        return Vector3(Math::abs(x), Math::abs(y), Math::abs(z));
    }

    [[nodiscard]] Vector3 floor() const noexcept {
        return Vector3(Math::floor(x), Math::floor(y), Math::floor(z));
    }

    [[nodiscard]] constexpr Vector3 sign() const noexcept {
        return Vector3(Math::sign(x), Math::sign(y), Math::sign(z));
    }

    [[nodiscard]] Vector3 ceil() const noexcept {
        return Vector3(Math::ceil(x), Math::ceil(y), Math::ceil(z));
    }

    [[nodiscard]] Vector3 round() const noexcept {
        return Vector3(Math::round(x), Math::round(y), Math::round(z));
    }

    [[nodiscard]] real_t distance_to(const Vector3 &to) const noexcept {
        return Math::sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z));
    }

    [[nodiscard]] constexpr real_t distance_squared_to(const Vector3 &to) const noexcept {
        return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y) + (z - to.z) * (z - to.z);
    }

    [[nodiscard]] Vector3 posmod(real_t mod) const noexcept {
        return Vector3(Math::fposmod(x, mod), Math::fposmod(y, mod), Math::fposmod(z, mod));
    }

    [[nodiscard]] Vector3 posmodv(const Vector3 &modv) const noexcept {
        return Vector3(Math::fposmod(x, modv.x), Math::fposmod(y, modv.y), Math::fposmod(z, modv.z));
    }

    [[nodiscard]] constexpr Vector3 project(const Vector3 &to) const noexcept {
        return to * (dot(to) / to.length_squared());
    }

    [[nodiscard]] real_t angle_to(const Vector3 &to) const noexcept {
        return Math::atan2(cross(to).length(), dot(to));
    }

    [[nodiscard]] real_t signed_angle_to(const Vector3 &to, const Vector3 &axis) const noexcept {
        Vector3 cr = cross(to);
        real_t sn = cr.dot(axis);
        real_t cs = dot(to);
        return Math::atan2(sn, cs);
    }

    [[nodiscard]] Vector3 direction_to(const Vector3 &to) const noexcept {
        Vector3 ret(to.x - x, to.y - y, to.z - z);
        ret.normalize();
        return ret;
    }

    [[nodiscard]] constexpr Vector3 slide(const Vector3 &normal) const noexcept {
        assert(normal.is_normalized());
        return *this - normal * dot(normal);
    }

    [[nodiscard]] constexpr Vector3 bounce(const Vector3 &normal) const noexcept {
        return -reflect(normal);
    }

    [[nodiscard]] constexpr Vector3 reflect(const Vector3 &normal) const noexcept {
        assert(normal.is_normalized());
        return normal * ((real_t)2 * dot(normal)) - *this;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Vector3 &other) const noexcept {
        return Math::is_equal_approx(x, other.x) && Math::is_equal_approx(y, other.y) && Math::is_equal_approx(z, other.z);
    }

    [[nodiscard]] constexpr bool is_same(const Vector3 &other) const noexcept {
        return Math::is_same(x, other.x) && Math::is_same(y, other.y) && Math::is_same(z, other.z);
    }

    [[nodiscard]] constexpr bool is_zero_approx() const noexcept {
        return Math::is_zero_approx(x) && Math::is_zero_approx(y) && Math::is_zero_approx(z);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return Math::is_finite(x) && Math::is_finite(y) && Math::is_finite(z);
    }

    constexpr Vector3 operator+(const Vector3 &rhs) const noexcept { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
    constexpr Vector3 operator-(const Vector3 &rhs) const noexcept { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
    constexpr Vector3 operator*(const Vector3 &rhs) const noexcept { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
    constexpr Vector3 operator/(const Vector3 &rhs) const noexcept { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
    constexpr Vector3 operator*(real_t scalar) const noexcept { return Vector3(x * scalar, y * scalar, z * scalar); }
    constexpr Vector3 operator/(real_t scalar) const noexcept { return Vector3(x / scalar, y / scalar, z / scalar); }
    constexpr Vector3 operator-() const noexcept { return Vector3(-x, -y, -z); }

    constexpr Vector3 &operator+=(const Vector3 &rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    constexpr Vector3 &operator-=(const Vector3 &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    constexpr Vector3 &operator*=(const Vector3 &rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    constexpr Vector3 &operator/=(const Vector3 &rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
    constexpr Vector3 &operator*=(real_t scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    constexpr Vector3 &operator/=(real_t scalar) noexcept { x /= scalar; y /= scalar; z /= scalar; return *this; }

    constexpr bool operator==(const Vector3 &rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    constexpr auto operator<=>(const Vector3 &rhs) const noexcept {
        if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
        if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
        return z <=> rhs.z;
    }

    [[nodiscard]] explicit constexpr operator Vector3i() const noexcept;
};

inline constexpr Vector3 Vector3::ZERO = { 0, 0, 0 };
inline constexpr Vector3 Vector3::ONE = { 1, 1, 1 };
inline constexpr Vector3 Vector3::INF = { Math::INF<real_t>, Math::INF<real_t>, Math::INF<real_t> };
inline constexpr Vector3 Vector3::LEFT = { -1, 0, 0 };
inline constexpr Vector3 Vector3::RIGHT = { 1, 0, 0 };
inline constexpr Vector3 Vector3::UP = { 0, 1, 0 };
inline constexpr Vector3 Vector3::DOWN = { 0, -1, 0 };
inline constexpr Vector3 Vector3::FORWARD = { 0, 0, -1 };
inline constexpr Vector3 Vector3::BACK = { 0, 0, 1 };
inline constexpr Vector3 Vector3::MODEL_LEFT = { 1, 0, 0 };
inline constexpr Vector3 Vector3::MODEL_RIGHT = { -1, 0, 0 };
inline constexpr Vector3 Vector3::MODEL_TOP = { 0, 1, 0 };
inline constexpr Vector3 Vector3::MODEL_BOTTOM = { 0, -1, 0 };
inline constexpr Vector3 Vector3::MODEL_FRONT = { 0, 0, 1 };
inline constexpr Vector3 Vector3::MODEL_REAR = { 0, 0, -1 };

[[nodiscard]] constexpr Vector3 operator*(real_t scalar, const Vector3 &vec) noexcept {
    return vec * scalar;
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector3> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector3 &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};
