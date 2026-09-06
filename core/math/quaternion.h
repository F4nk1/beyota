/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  quaternion.h                                                          */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector3.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct Basis;

struct [[nodiscard]] Quaternion {
    static const Quaternion IDENTITY;

    real_t x{0};
    real_t y{0};
    real_t z{0};
    real_t w{1};

    constexpr Quaternion() = default;
    constexpr Quaternion(real_t x, real_t y, real_t z, real_t w) noexcept : x(x), y(y), z(z), w(w) {}

    Quaternion(const Vector3 &axis, real_t angle) noexcept {
        assert(axis.is_normalized());
        real_t d = axis.length();
        if (d == (real_t)0.0) {
            x = (real_t)0;
            y = (real_t)0;
            z = (real_t)0;
            w = (real_t)0;
        } else {
            real_t sin_a = Math::sin(angle * (real_t)0.5);
            real_t cos_a = Math::cos(angle * (real_t)0.5);
            real_t s = sin_a / d;
            x = axis.x * s;
            y = axis.y * s;
            z = axis.z * s;
            w = cos_a;
        }
    }

    Quaternion(const Vector3 &v0, const Vector3 &v1) noexcept {
        assert(!v0.is_zero_approx() && !v1.is_zero_approx());
        constexpr real_t ALMOST_ONE = (sizeof(real_t) == 8) ? (real_t)0.999999999999999 : (real_t)0.99999975f;
        Vector3 n0 = v0.normalized();
        Vector3 n1 = v1.normalized();
        real_t d = n0.dot(n1);
        if (Math::abs(d) > ALMOST_ONE) {
            if (d >= (real_t)0.0) {
                return;
            }
            Vector3 axis = n0.get_any_perpendicular();
            x = axis.x;
            y = axis.y;
            z = axis.z;
            w = (real_t)0.0;
        } else {
            Vector3 c = n0.cross(n1);
            real_t s = Math::sqrt(((real_t)1.0 + d) * (real_t)2.0);
            real_t rs = (real_t)1.0 / s;
            x = c.x * rs;
            y = c.y * rs;
            z = c.z * rs;
            w = s * (real_t)0.5;
        }
        normalize();
    }

    [[nodiscard]] constexpr real_t &operator[](usize idx) noexcept {
        assert(idx < 4);
        return (&x)[idx];
    }

    [[nodiscard]] constexpr const real_t &operator[](usize idx) const noexcept {
        assert(idx < 4);
        return (&x)[idx];
    }

    [[nodiscard]] constexpr real_t dot(const Quaternion &q) const noexcept {
        return x * q.x + y * q.y + z * q.z + w * q.w;
    }

    [[nodiscard]] constexpr real_t length_squared() const noexcept {
        return dot(*this);
    }

    [[nodiscard]] real_t length() const noexcept {
        return Math::sqrt(length_squared());
    }

    void normalize() noexcept {
        real_t l = length();
        if (l == (real_t)0.0) {
            x = 0; y = 0; z = 0; w = 1;
            return;
        }
        *this /= l;
    }

    [[nodiscard]] Quaternion normalized() const noexcept {
        Quaternion q = *this;
        q.normalize();
        return q;
    }

    [[nodiscard]] constexpr bool is_normalized() const noexcept {
        return Math::is_equal_approx(length_squared(), (real_t)1, (real_t)UNIT_EPSILON<real_t>);
    }

    [[nodiscard]] constexpr Quaternion inverse() const noexcept {
        assert(is_normalized());
        return Quaternion(-x, -y, -z, w);
    }

    [[nodiscard]] real_t angle_to(const Quaternion &to) const noexcept {
        real_t d = dot(to);
        return Math::acos(d * d * (real_t)2.0 - (real_t)1.0);
    }

    [[nodiscard]] Vector3 get_axis() const noexcept {
        if (Math::abs(w) > (real_t)1.0 - CMP_EPSILON<real_t>) {
            return Vector3(x, y, z);
        }
        real_t r = (real_t)1.0 / Math::sqrt((real_t)1.0 - w * w);
        return Vector3(x * r, y * r, z * r);
    }

    [[nodiscard]] real_t get_angle() const noexcept {
        return (real_t)2.0 * Math::acos(w);
    }

    void get_axis_angle(Vector3 &r_axis, real_t &r_angle) const noexcept {
        r_angle = (real_t)2.0 * Math::acos(w);
        real_t r = (real_t)1.0 / Math::sqrt((real_t)1.0 - w * w);
        r_axis.x = x * r;
        r_axis.y = y * r;
        r_axis.z = z * r;
    }

    [[nodiscard]] Quaternion log() const noexcept {
        Vector3 src_v = get_axis() * get_angle();
        return Quaternion(src_v.x, src_v.y, src_v.z, (real_t)0.0);
    }

    [[nodiscard]] Quaternion exp() const noexcept {
        Vector3 src_v(x, y, z);
        real_t theta = src_v.length();
        src_v = src_v.normalized();
        if (theta < CMP_EPSILON<real_t> || !src_v.is_normalized()) {
            return Quaternion(0, 0, 0, 1);
        }
        return Quaternion(src_v, theta);
    }

    [[nodiscard]] Quaternion slerp(const Quaternion &to, real_t weight) const noexcept {
        assert(is_normalized());
        assert(to.is_normalized());

        Quaternion to1;
        real_t cosom = dot(to);
        if (cosom < (real_t)0.0) {
            cosom = -cosom;
            to1 = -to;
        } else {
            to1 = to;
        }

        real_t scale0, scale1;
        if (((real_t)1.0 - cosom) > CMP_EPSILON<real_t>) {
            real_t omega = Math::acos(cosom);
            real_t sinom = Math::sin(omega);
            scale0 = Math::sin(((real_t)1.0 - weight) * omega) / sinom;
            scale1 = Math::sin(weight * omega) / sinom;
        } else {
            scale0 = (real_t)1.0 - weight;
            scale1 = weight;
        }

        return Quaternion(
            scale0 * x + scale1 * to1.x,
            scale0 * y + scale1 * to1.y,
            scale0 * z + scale1 * to1.z,
            scale0 * w + scale1 * to1.w
        );
    }

    [[nodiscard]] Quaternion slerpni(const Quaternion &to, real_t weight) const noexcept {
        assert(is_normalized());
        assert(to.is_normalized());

        real_t dot_val = dot(to);
        if (Math::abs(dot_val) > (real_t)0.9999) {
            return *this;
        }

        real_t theta = Math::acos(dot_val);
        real_t sinT = (real_t)1.0 / Math::sin(theta);
        real_t newFactor = Math::sin(weight * theta) * sinT;
        real_t invFactor = Math::sin(((real_t)1.0 - weight) * theta) * sinT;

        return Quaternion(
            invFactor * x + newFactor * to.x,
            invFactor * y + newFactor * to.y,
            invFactor * z + newFactor * to.z,
            invFactor * w + newFactor * to.w
        );
    }

    [[nodiscard]] constexpr Vector3 xform(const Vector3 &v) const noexcept {
        assert(is_normalized());
        Vector3 u(x, y, z);
        Vector3 uv = u.cross(v);
        Vector3 uuv = u.cross(uv);
        return v + ((uv * w) + uuv) * (real_t)2.0;
    }

    [[nodiscard]] constexpr Vector3 xform_inv(const Vector3 &v) const noexcept {
        return inverse().xform(v);
    }

    [[nodiscard]] static Quaternion from_euler(const Vector3 &euler) noexcept {
        real_t half_a1 = euler.y * (real_t)0.5;
        real_t half_a2 = euler.x * (real_t)0.5;
        real_t half_a3 = euler.z * (real_t)0.5;

        real_t cos_a1 = Math::cos(half_a1);
        real_t sin_a1 = Math::sin(half_a1);
        real_t cos_a2 = Math::cos(half_a2);
        real_t sin_a2 = Math::sin(half_a2);
        real_t cos_a3 = Math::cos(half_a3);
        real_t sin_a3 = Math::sin(half_a3);

        return Quaternion(
            sin_a1 * cos_a2 * sin_a3 + cos_a1 * sin_a2 * cos_a3,
            sin_a1 * cos_a2 * cos_a3 - cos_a1 * sin_a2 * sin_a3,
            -sin_a1 * sin_a2 * cos_a3 + cos_a1 * cos_a2 * sin_a3,
            sin_a1 * sin_a2 * sin_a3 + cos_a1 * cos_a2 * cos_a3
        );
    }

    [[nodiscard]] Vector3 get_euler(EulerOrder order = EulerOrder::YXZ) const noexcept;

    [[nodiscard]] constexpr bool is_equal_approx(const Quaternion &other) const noexcept {
        return Math::is_equal_approx(x, other.x) &&
               Math::is_equal_approx(y, other.y) &&
               Math::is_equal_approx(z, other.z) &&
               Math::is_equal_approx(w, other.w);
    }

    [[nodiscard]] constexpr bool is_same(const Quaternion &other) const noexcept {
        return Math::is_same(x, other.x) &&
               Math::is_same(y, other.y) &&
               Math::is_same(z, other.z) &&
               Math::is_same(w, other.w);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return Math::is_finite(x) && Math::is_finite(y) && Math::is_finite(z) && Math::is_finite(w);
    }

    constexpr Quaternion operator+(const Quaternion &q) const noexcept { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
    constexpr Quaternion operator-(const Quaternion &q) const noexcept { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }
    constexpr Quaternion operator-() const noexcept { return Quaternion(-x, -y, -z, -w); }
    constexpr Quaternion operator*(real_t s) const noexcept { return Quaternion(x * s, y * s, z * s, w * s); }
    constexpr Quaternion operator/(real_t s) const noexcept { return *this * ((real_t)1.0 / s); }

    constexpr Quaternion &operator+=(const Quaternion &q) noexcept { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
    constexpr Quaternion &operator-=(const Quaternion &q) noexcept { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
    constexpr Quaternion &operator*=(real_t s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
    constexpr Quaternion &operator/=(real_t s) noexcept { return *this *= ((real_t)1.0 / s); }

    constexpr Quaternion operator*(const Quaternion &q) const noexcept {
        return Quaternion(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y + y * q.w + z * q.x - x * q.z,
            w * q.z + z * q.w + x * q.y - y * q.x,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    }

    constexpr void operator*=(const Quaternion &q) noexcept {
        *this = *this * q;
    }

    [[nodiscard]] constexpr Vector3 operator*(const Vector3 &v) const noexcept {
        return xform(v);
    }

    constexpr bool operator==(const Quaternion &rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
};

inline constexpr Quaternion Quaternion::IDENTITY = { 0, 0, 0, 1 };

[[nodiscard]] constexpr Quaternion operator*(real_t scalar, const Quaternion &q) noexcept {
    return q * scalar;
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Quaternion> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Quaternion &q, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {}, {})", q.x, q.y, q.z, q.w);
    }
};
