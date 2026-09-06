/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector4.h                                                             */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct Vector4i;

struct [[nodiscard]] Vector4 {
    static const Vector4 ZERO;
    static const Vector4 ONE;
    static const Vector4 INF;

    real_t x{0};
    real_t y{0};
    real_t z{0};
    real_t w{0};

    constexpr Vector4() = default;
    constexpr Vector4(real_t x, real_t y, real_t z, real_t w) noexcept : x(x), y(y), z(z), w(w) {}
    explicit constexpr Vector4(const Vector4i &v) noexcept;

    [[nodiscard]] constexpr real_t &operator[](usize axis) noexcept {
        assert(axis < 4);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const real_t &operator[](usize axis) const noexcept {
        assert(axis < 4);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        real_t min_val = x;
        Axis min_axis = Axis::X;
        if (y < min_val) { min_val = y; min_axis = Axis::Y; }
        if (z < min_val) { min_val = z; min_axis = Axis::Z; }
        if (w < min_val) { min_axis = Axis::W; }
        return min_axis;
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        real_t max_val = x;
        Axis max_axis = Axis::X;
        if (y > max_val) { max_val = y; max_axis = Axis::Y; }
        if (z > max_val) { max_val = z; max_axis = Axis::Z; }
        if (w > max_val) { max_axis = Axis::W; }
        return max_axis;
    }

    [[nodiscard]] real_t length() const noexcept {
        return Math::sqrt(x * x + y * y + z * z + w * w);
    }

    [[nodiscard]] constexpr real_t length_squared() const noexcept {
        return x * x + y * y + z * z + w * w;
    }

    constexpr void zero() noexcept {
        x = (real_t)0;
        y = (real_t)0;
        z = (real_t)0;
        w = (real_t)0;
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
            w /= l;
        }
    }

    [[nodiscard]] Vector4 normalized() const noexcept {
        Vector4 v = *this;
        v.normalize();
        return v;
    }

    [[nodiscard]] constexpr bool is_normalized() const noexcept {
        return Math::is_equal_approx(length_squared(), (real_t)1, (real_t)UNIT_EPSILON<real_t>);
    }

    [[nodiscard]] constexpr Vector4 inverse() const noexcept {
        return Vector4((real_t)1 / x, (real_t)1 / y, (real_t)1 / z, (real_t)1 / w);
    }

    [[nodiscard]] Vector4 min(const Vector4 &other) const noexcept {
        return Vector4(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z), std::min(w, other.w));
    }

    [[nodiscard]] Vector4 minf(real_t scalar) const noexcept {
        return Vector4(std::min(x, scalar), std::min(y, scalar), std::min(z, scalar), std::min(w, scalar));
    }

    [[nodiscard]] Vector4 max(const Vector4 &other) const noexcept {
        return Vector4(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z), std::max(w, other.w));
    }

    [[nodiscard]] Vector4 maxf(real_t scalar) const noexcept {
        return Vector4(std::max(x, scalar), std::max(y, scalar), std::max(z, scalar), std::max(w, scalar));
    }

    [[nodiscard]] Vector4 clamp(const Vector4 &min_v, const Vector4 &max_v) const noexcept {
        return Vector4(
            std::clamp(x, min_v.x, max_v.x),
            std::clamp(y, min_v.y, max_v.y),
            std::clamp(z, min_v.z, max_v.z),
            std::clamp(w, min_v.w, max_v.w)
        );
    }

    [[nodiscard]] Vector4 clampf(real_t min_v, real_t max_v) const noexcept {
        return Vector4(
            std::clamp(x, min_v, max_v),
            std::clamp(y, min_v, max_v),
            std::clamp(z, min_v, max_v),
            std::clamp(w, min_v, max_v)
        );
    }

    [[nodiscard]] constexpr real_t dot(const Vector4 &other) const noexcept {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    [[nodiscard]] real_t distance_to(const Vector4 &to) const noexcept {
        return (to - *this).length();
    }

    [[nodiscard]] constexpr real_t distance_squared_to(const Vector4 &to) const noexcept {
        return (to - *this).length_squared();
    }

    [[nodiscard]] Vector4 direction_to(const Vector4 &to) const noexcept {
        return (to - *this).normalized();
    }

    [[nodiscard]] constexpr Vector4 abs() const noexcept {
        return Vector4(Math::abs(x), Math::abs(y), Math::abs(z), Math::abs(w));
    }

    [[nodiscard]] Vector4 floor() const noexcept {
        return Vector4(Math::floor(x), Math::floor(y), Math::floor(z), Math::floor(w));
    }

    [[nodiscard]] Vector4 ceil() const noexcept {
        return Vector4(Math::ceil(x), Math::ceil(y), Math::ceil(z), Math::ceil(w));
    }

    [[nodiscard]] Vector4 round() const noexcept {
        return Vector4(Math::round(x), Math::round(y), Math::round(z), Math::round(w));
    }

    [[nodiscard]] constexpr Vector4 sign() const noexcept {
        return Vector4(Math::sign(x), Math::sign(y), Math::sign(z), Math::sign(w));
    }

    [[nodiscard]] Vector4 posmod(real_t mod) const noexcept {
        return Vector4(Math::fposmod(x, mod), Math::fposmod(y, mod), Math::fposmod(z, mod), Math::fposmod(w, mod));
    }

    [[nodiscard]] Vector4 posmodv(const Vector4 &modv) const noexcept {
        return Vector4(Math::fposmod(x, modv.x), Math::fposmod(y, modv.y), Math::fposmod(z, modv.z), Math::fposmod(w, modv.w));
    }

    [[nodiscard]] Vector4 snapped(const Vector4 &step) const noexcept {
        return Vector4(Math::snapped(x, step.x), Math::snapped(y, step.y), Math::snapped(z, step.z), Math::snapped(w, step.w));
    }

    [[nodiscard]] Vector4 snappedf(real_t step) const noexcept {
        return Vector4(Math::snapped(x, step), Math::snapped(y, step), Math::snapped(z, step), Math::snapped(w, step));
    }

    [[nodiscard]] constexpr Vector4 lerp(const Vector4 &to, real_t weight) const noexcept {
        return Vector4(
            Math::lerp(x, to.x, weight),
            Math::lerp(y, to.y, weight),
            Math::lerp(z, to.z, weight),
            Math::lerp(w, to.w, weight)
        );
    }

    [[nodiscard]] constexpr Vector4 cubic_interpolate(const Vector4 &b, const Vector4 &pre_a, const Vector4 &post_b, real_t weight) const noexcept {
        return Vector4(
            Math::cubic_interpolate(x, b.x, pre_a.x, post_b.x, weight),
            Math::cubic_interpolate(y, b.y, pre_a.y, post_b.y, weight),
            Math::cubic_interpolate(z, b.z, pre_a.z, post_b.z, weight),
            Math::cubic_interpolate(w, b.w, pre_a.w, post_b.w, weight)
        );
    }

    [[nodiscard]] constexpr Vector4 cubic_interpolate_in_time(const Vector4 &b, const Vector4 &pre_a, const Vector4 &post_b, real_t weight, real_t b_t, real_t pre_a_t, real_t post_b_t) const noexcept {
        return Vector4(
            Math::cubic_interpolate_in_time(x, b.x, pre_a.x, post_b.x, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(y, b.y, pre_a.y, post_b.y, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(z, b.z, pre_a.z, post_b.z, weight, b_t, pre_a_t, post_b_t),
            Math::cubic_interpolate_in_time(w, b.w, pre_a.w, post_b.w, weight, b_t, pre_a_t, post_b_t)
        );
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Vector4 &other) const noexcept {
        return Math::is_equal_approx(x, other.x) && Math::is_equal_approx(y, other.y) && Math::is_equal_approx(z, other.z) && Math::is_equal_approx(w, other.w);
    }

    [[nodiscard]] constexpr bool is_same(const Vector4 &other) const noexcept {
        return Math::is_same(x, other.x) && Math::is_same(y, other.y) && Math::is_same(z, other.z) && Math::is_same(w, other.w);
    }

    [[nodiscard]] constexpr bool is_zero_approx() const noexcept {
        return Math::is_zero_approx(x) && Math::is_zero_approx(y) && Math::is_zero_approx(z) && Math::is_zero_approx(w);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return Math::is_finite(x) && Math::is_finite(y) && Math::is_finite(z) && Math::is_finite(w);
    }

    constexpr Vector4 operator+(const Vector4 &rhs) const noexcept { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
    constexpr Vector4 operator-(const Vector4 &rhs) const noexcept { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
    constexpr Vector4 operator*(const Vector4 &rhs) const noexcept { return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
    constexpr Vector4 operator/(const Vector4 &rhs) const noexcept { return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
    constexpr Vector4 operator*(real_t scalar) const noexcept { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
    constexpr Vector4 operator/(real_t scalar) const noexcept { return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); }
    constexpr Vector4 operator-() const noexcept { return Vector4(-x, -y, -z, -w); }

    constexpr Vector4 &operator+=(const Vector4 &rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
    constexpr Vector4 &operator-=(const Vector4 &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
    constexpr Vector4 &operator*=(const Vector4 &rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
    constexpr Vector4 &operator/=(const Vector4 &rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
    constexpr Vector4 &operator*=(real_t scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    constexpr Vector4 &operator/=(real_t scalar) noexcept { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

    constexpr bool operator==(const Vector4 &rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    constexpr auto operator<=>(const Vector4 &rhs) const noexcept {
        if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
        if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
        if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
        return w <=> rhs.w;
    }

    [[nodiscard]] explicit constexpr operator Vector4i() const noexcept;
};

inline constexpr Vector4 Vector4::ZERO = { 0, 0, 0, 0 };
inline constexpr Vector4 Vector4::ONE = { 1, 1, 1, 1 };
inline constexpr Vector4 Vector4::INF = { Math::INF<real_t>, Math::INF<real_t>, Math::INF<real_t>, Math::INF<real_t> };

[[nodiscard]] constexpr Vector4 operator*(real_t scalar, const Vector4 &vec) noexcept {
    return vec * scalar;
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector4> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector4 &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};
