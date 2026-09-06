/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector2.h                                                             */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct Vector2i;

struct [[nodiscard]] Vector2 {
    static const Vector2 ZERO;
    static const Vector2 ONE;
    static const Vector2 LEFT;
    static const Vector2 RIGHT;
    static const Vector2 UP;
    static const Vector2 DOWN;

    union {
        real_t x{0};
        real_t width;
    };
    union {
        real_t y{0};
        real_t height;
    };

    constexpr Vector2() = default;
    constexpr Vector2(real_t x, real_t y) noexcept : x(x), y(y) {}
    explicit constexpr Vector2(const Vector2i &v) noexcept;

    [[nodiscard]] constexpr real_t &operator[](usize axis) noexcept {
        assert(axis < 2);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const real_t &operator[](usize axis) const noexcept {
        assert(axis < 2);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        return x < y ? Axis::X : Axis::Y;
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        return x < y ? Axis::Y : Axis::X;
    }

    [[nodiscard]] real_t length() const noexcept {
        return Math::sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr real_t length_squared() const noexcept {
        return x * x + y * y;
    }

    constexpr void zero() noexcept {
        x = (real_t)0;
        y = (real_t)0;
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
        }
    }

    [[nodiscard]] Vector2 normalized() const noexcept {
        Vector2 v = *this;
        v.normalize();
        return v;
    }

    [[nodiscard]] constexpr bool is_normalized() const noexcept {
        return Math::is_equal_approx(length_squared(), (real_t)1, (real_t)UNIT_EPSILON<real_t>);
    }

    [[nodiscard]] Vector2 limit_length(real_t len = (real_t)1) const noexcept {
        const real_t l = length();
        Vector2 v = *this;
        if (l > (real_t)0 && len < l) {
            v /= l;
            v *= len;
        }
        return v;
    }

    [[nodiscard]] constexpr real_t dot(const Vector2 &other) const noexcept {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] constexpr real_t cross(const Vector2 &other) const noexcept {
        return x * other.y - y * other.x;
    }

    [[nodiscard]] real_t distance_to(const Vector2 &to) const noexcept {
        return Math::sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y));
    }

    [[nodiscard]] constexpr real_t distance_squared_to(const Vector2 &to) const noexcept {
        return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y);
    }

    [[nodiscard]] real_t angle() const noexcept {
        return Math::atan2(y, x);
    }

    [[nodiscard]] static Vector2 from_angle(real_t angle_radians) noexcept {
        return Vector2(Math::cos(angle_radians), Math::sin(angle_radians));
    }

    [[nodiscard]] real_t angle_to(const Vector2 &to) const noexcept {
        return Math::atan2(cross(to), dot(to));
    }

    [[nodiscard]] real_t angle_to_point(const Vector2 &to) const noexcept {
        return (to - *this).angle();
    }

    [[nodiscard]] Vector2 direction_to(const Vector2 &to) const noexcept {
        Vector2 ret(to.x - x, to.y - y);
        ret.normalize();
        return ret;
    }

    [[nodiscard]] constexpr Vector2 orthogonal() const noexcept {
        return Vector2(y, -x);
    }

    [[nodiscard]] Vector2 rotated(real_t radians) const noexcept {
        const real_t s = Math::sin(radians);
        const real_t c = Math::cos(radians);
        return Vector2(x * c - y * s, x * s + y * c);
    }

    [[nodiscard]] Vector2 posmod(real_t mod) const noexcept {
        return Vector2(Math::fposmod(x, mod), Math::fposmod(y, mod));
    }

    [[nodiscard]] Vector2 posmodv(const Vector2 &modv) const noexcept {
        return Vector2(Math::fposmod(x, modv.x), Math::fposmod(y, modv.y));
    }

    [[nodiscard]] constexpr Vector2 project(const Vector2 &to) const noexcept {
        return to * (dot(to) / to.length_squared());
    }

    [[nodiscard]] constexpr Vector2 plane_project(real_t d, const Vector2 &vec) const noexcept {
        return vec - *this * (dot(vec) - d);
    }

    [[nodiscard]] constexpr Vector2 lerp(const Vector2 &to, real_t weight) const noexcept {
        return Vector2(Math::lerp(x, to.x, weight), Math::lerp(y, to.y, weight));
    }

    [[nodiscard]] Vector2 slerp(const Vector2 &to, real_t weight) const noexcept {
        const real_t start_len_sq = length_squared();
        const real_t end_len_sq = to.length_squared();
        if (start_len_sq == (real_t)0 || end_len_sq == (real_t)0) {
            return lerp(to, weight);
        }
        const real_t start_len = Math::sqrt(start_len_sq);
        const real_t result_len = Math::lerp(start_len, Math::sqrt(end_len_sq), weight);
        const real_t ang = angle_to(to);
        return rotated(ang * weight) * (result_len / start_len);
    }

    [[nodiscard]] constexpr Vector2 cubic_interpolate(const Vector2 &b, const Vector2 &pre_a, const Vector2 &post_b, real_t weight) const noexcept {
        return Vector2(
            Math::cubic_interpolate(x, b.x, pre_a.x, post_b.x, weight),
            Math::cubic_interpolate(y, b.y, pre_a.y, post_b.y, weight)
        );
    }

    [[nodiscard]] constexpr Vector2 cubic_interpolate_in_time(const Vector2 &b, const Vector2 &pre_a, const Vector2 &post_b, real_t weight, real_t b_t, real_t pre_a_t, real_t post_b_t) const noexcept {
        return Vector2(
            Math::cubic_interpolate_in_time(x, b.x, pre_a.x, post_b.x, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(y, b.y, pre_a.y, post_b.y, weight, b_t, pre_a_t, post_b_t)
        );
    }

    [[nodiscard]] constexpr Vector2 bezier_interpolate(const Vector2 &c1, const Vector2 &c2, const Vector2 &end, real_t t) const noexcept {
        return Vector2(
            Math::bezier_interpolate(x, c1.x, c2.x, end.x, t),
            Math::bezier_interpolate(y, c1.y, c2.y, end.y, t)
        );
    }

    [[nodiscard]] constexpr Vector2 bezier_derivative(const Vector2 &c1, const Vector2 &c2, const Vector2 &end, real_t t) const noexcept {
        return Vector2(
            Math::bezier_derivative(x, c1.x, c2.x, end.x, t),
            Math::bezier_derivative(y, c1.y, c2.y, end.y, t)
        );
    }

    [[nodiscard]] Vector2 move_toward(const Vector2 &to, real_t delta) const noexcept {
        const Vector2 vd = to - *this;
        const real_t len = vd.length();
        return (len <= delta || len < CMP_EPSILON<real_t>) ? to : (*this + vd / len * delta);
    }

    [[nodiscard]] constexpr Vector2 slide(const Vector2 &normal) const noexcept {
        assert(normal.is_normalized());
        return *this - normal * dot(normal);
    }

    [[nodiscard]] constexpr Vector2 bounce(const Vector2 &normal) const noexcept {
        return -reflect(normal);
    }

    [[nodiscard]] constexpr Vector2 reflect(const Vector2 &normal) const noexcept {
        assert(normal.is_normalized());
        return normal * ((real_t)2 * dot(normal)) - *this;
    }

    [[nodiscard]] Vector2 min(const Vector2 &other) const noexcept {
        return Vector2(std::min(x, other.x), std::min(y, other.y));
    }

    [[nodiscard]] Vector2 minf(real_t scalar) const noexcept {
        return Vector2(std::min(x, scalar), std::min(y, scalar));
    }

    [[nodiscard]] Vector2 max(const Vector2 &other) const noexcept {
        return Vector2(std::max(x, other.x), std::max(y, other.y));
    }

    [[nodiscard]] Vector2 maxf(real_t scalar) const noexcept {
        return Vector2(std::max(x, scalar), std::max(y, scalar));
    }

    [[nodiscard]] Vector2 clamp(const Vector2 &min_v, const Vector2 &max_v) const noexcept {
        return Vector2(std::clamp(x, min_v.x, max_v.x), std::clamp(y, min_v.y, max_v.y));
    }

    [[nodiscard]] Vector2 clampf(real_t min_v, real_t max_v) const noexcept {
        return Vector2(std::clamp(x, min_v, max_v), std::clamp(y, min_v, max_v));
    }

    [[nodiscard]] Vector2 snapped(const Vector2 &step) const noexcept {
        return Vector2(Math::snapped(x, step.x), Math::snapped(y, step.y));
    }

    [[nodiscard]] Vector2 snappedf(real_t step) const noexcept {
        return Vector2(Math::snapped(x, step), Math::snapped(y, step));
    }

    [[nodiscard]] Vector2 floor() const noexcept {
        return Vector2(Math::floor(x), Math::floor(y));
    }

    [[nodiscard]] Vector2 ceil() const noexcept {
        return Vector2(Math::ceil(x), Math::ceil(y));
    }

    [[nodiscard]] Vector2 round() const noexcept {
        return Vector2(Math::round(x), Math::round(y));
    }

    [[nodiscard]] constexpr Vector2 sign() const noexcept {
        return Vector2(Math::sign(x), Math::sign(y));
    }

    [[nodiscard]] constexpr Vector2 abs() const noexcept {
        return Vector2(Math::abs(x), Math::abs(y));
    }

    [[nodiscard]] constexpr real_t aspect() const noexcept {
        return width / height;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Vector2 &other) const noexcept {
        return Math::is_equal_approx(x, other.x) && Math::is_equal_approx(y, other.y);
    }

    [[nodiscard]] constexpr bool is_zero_approx() const noexcept {
        return Math::is_zero_approx(x) && Math::is_zero_approx(y);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return Math::is_finite(x) && Math::is_finite(y);
    }

    [[nodiscard]] constexpr bool is_same(const Vector2 &other) const noexcept {
        return Math::is_same(x, other.x) && Math::is_same(y, other.y);
    }

    constexpr Vector2 operator+(const Vector2 &rhs) const noexcept { return Vector2(x + rhs.x, y + rhs.y); }
    constexpr Vector2 operator-(const Vector2 &rhs) const noexcept { return Vector2(x - rhs.x, y - rhs.y); }
    constexpr Vector2 operator*(const Vector2 &rhs) const noexcept { return Vector2(x * rhs.x, y * rhs.y); }
    constexpr Vector2 operator/(const Vector2 &rhs) const noexcept { return Vector2(x / rhs.x, y / rhs.y); }
    constexpr Vector2 operator*(real_t scalar) const noexcept { return Vector2(x * scalar, y * scalar); }
    constexpr Vector2 operator/(real_t scalar) const noexcept { return Vector2(x / scalar, y / scalar); }
    constexpr Vector2 operator-() const noexcept { return Vector2(-x, -y); }

    constexpr Vector2 &operator+=(const Vector2 &rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vector2 &operator-=(const Vector2 &rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vector2 &operator*=(const Vector2 &rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
    constexpr Vector2 &operator/=(const Vector2 &rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }
    constexpr Vector2 &operator*=(real_t scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    constexpr Vector2 &operator/=(real_t scalar) noexcept { x /= scalar; y /= scalar; return *this; }

    constexpr bool operator==(const Vector2 &rhs) const noexcept { return x == rhs.x && y == rhs.y; }
    constexpr auto operator<=>(const Vector2 &rhs) const noexcept { if (auto cmp = x <=> rhs.x; cmp != 0) return cmp; return y <=> rhs.y; }

    [[nodiscard]] explicit constexpr operator Vector2i() const noexcept;
};

inline constexpr Vector2 Vector2::ZERO = { 0, 0 };
inline constexpr Vector2 Vector2::ONE = { 1, 1 };
inline constexpr Vector2 Vector2::LEFT = { -1, 0 };
inline constexpr Vector2 Vector2::RIGHT = { 1, 0 };
inline constexpr Vector2 Vector2::UP = { 0, -1 };
inline constexpr Vector2 Vector2::DOWN = { 0, 1 };

[[nodiscard]] constexpr Vector2 operator*(real_t scalar, const Vector2 &vec) noexcept {
    return vec * scalar;
}

using Point2 = Vector2;
using Size2 = Vector2;

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector2> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector2 &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};
