/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector4i.h                                                            */
/**************************************************************************/

#pragma once

#include "core/math/vector4.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Vector4i {
    static const Vector4i ZERO;
    static const Vector4i ONE;

    i32 x{0};
    i32 y{0};
    i32 z{0};
    i32 w{0};

    constexpr Vector4i() = default;
    constexpr Vector4i(i32 x, i32 y, i32 z, i32 w) noexcept : x(x), y(y), z(z), w(w) {}
    explicit constexpr Vector4i(const Vector4 &v) noexcept : x((i32)v.x), y((i32)v.y), z((i32)v.z), w((i32)v.w) {}

    [[nodiscard]] constexpr i32 &operator[](usize axis) noexcept {
        assert(axis < 4);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const i32 &operator[](usize axis) const noexcept {
        assert(axis < 4);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        i32 min_val = x;
        Axis min_axis = Axis::X;
        if (y < min_val) { min_val = y; min_axis = Axis::Y; }
        if (z < min_val) { min_val = z; min_axis = Axis::Z; }
        if (w < min_val) { min_axis = Axis::W; }
        return min_axis;
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        i32 max_val = x;
        Axis max_axis = Axis::X;
        if (y > max_val) { max_val = y; max_axis = Axis::Y; }
        if (z > max_val) { max_val = z; max_axis = Axis::Z; }
        if (w > max_val) { max_axis = Axis::W; }
        return max_axis;
    }

    [[nodiscard]] constexpr i64 length_squared() const noexcept {
        return (i64)x * (i64)x + (i64)y * (i64)y + (i64)z * (i64)z + (i64)w * (i64)w;
    }

    [[nodiscard]] double length() const noexcept {
        return std::sqrt((double)length_squared());
    }

    [[nodiscard]] double distance_to(const Vector4i &to) const noexcept {
        return (to - *this).length();
    }

    [[nodiscard]] constexpr i64 distance_squared_to(const Vector4i &to) const noexcept {
        return (to - *this).length_squared();
    }

    constexpr void zero() noexcept {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }

    [[nodiscard]] Vector4i min(const Vector4i &other) const noexcept {
        return Vector4i(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z), std::min(w, other.w));
    }

    [[nodiscard]] Vector4i mini(i32 scalar) const noexcept {
        return Vector4i(std::min(x, scalar), std::min(y, scalar), std::min(z, scalar), std::min(w, scalar));
    }

    [[nodiscard]] Vector4i max(const Vector4i &other) const noexcept {
        return Vector4i(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z), std::max(w, other.w));
    }

    [[nodiscard]] Vector4i maxi(i32 scalar) const noexcept {
        return Vector4i(std::max(x, scalar), std::max(y, scalar), std::max(z, scalar), std::max(w, scalar));
    }

    [[nodiscard]] Vector4i clamp(const Vector4i &min_v, const Vector4i &max_v) const noexcept {
        return Vector4i(
            std::clamp(x, min_v.x, max_v.x),
            std::clamp(y, min_v.y, max_v.y),
            std::clamp(z, min_v.z, max_v.z),
            std::clamp(w, min_v.w, max_v.w)
        );
    }

    [[nodiscard]] Vector4i clampi(i32 min_v, i32 max_v) const noexcept {
        return Vector4i(
            std::clamp(x, min_v, max_v),
            std::clamp(y, min_v, max_v),
            std::clamp(z, min_v, max_v),
            std::clamp(w, min_v, max_v)
        );
    }

    [[nodiscard]] Vector4i snapped(const Vector4i &step) const noexcept {
        return Vector4i(
            (i32)Math::snapped((real_t)x, (real_t)step.x),
            (i32)Math::snapped((real_t)y, (real_t)step.y),
            (i32)Math::snapped((real_t)z, (real_t)step.z),
            (i32)Math::snapped((real_t)w, (real_t)step.w)
        );
    }

    [[nodiscard]] Vector4i snappedi(i32 step) const noexcept {
        return Vector4i(
            (i32)Math::snapped((real_t)x, (real_t)step),
            (i32)Math::snapped((real_t)y, (real_t)step),
            (i32)Math::snapped((real_t)z, (real_t)step),
            (i32)Math::snapped((real_t)w, (real_t)step)
        );
    }

    [[nodiscard]] constexpr Vector4i sign() const noexcept {
        return Vector4i(Math::sign(x), Math::sign(y), Math::sign(z), Math::sign(w));
    }

    [[nodiscard]] constexpr Vector4i abs() const noexcept {
        return Vector4i(Math::abs(x), Math::abs(y), Math::abs(z), Math::abs(w));
    }

    constexpr Vector4i operator+(const Vector4i &rhs) const noexcept { return Vector4i(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
    constexpr Vector4i operator-(const Vector4i &rhs) const noexcept { return Vector4i(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
    constexpr Vector4i operator*(const Vector4i &rhs) const noexcept { return Vector4i(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
    constexpr Vector4i operator/(const Vector4i &rhs) const noexcept {
        return Vector4i(
            Math::division_no_overflow(x, rhs.x),
            Math::division_no_overflow(y, rhs.y),
            Math::division_no_overflow(z, rhs.z),
            Math::division_no_overflow(w, rhs.w)
        );
    }
    constexpr Vector4i operator%(const Vector4i &rhs) const noexcept {
        return Vector4i(
            Math::modulo_no_overflow(x, rhs.x),
            Math::modulo_no_overflow(y, rhs.y),
            Math::modulo_no_overflow(z, rhs.z),
            Math::modulo_no_overflow(w, rhs.w)
        );
    }

    constexpr Vector4i operator*(i32 scalar) const noexcept { return Vector4i(x * scalar, y * scalar, z * scalar, w * scalar); }
    constexpr Vector4i operator/(i32 scalar) const noexcept {
        return Vector4i(
            Math::division_no_overflow(x, scalar),
            Math::division_no_overflow(y, scalar),
            Math::division_no_overflow(z, scalar),
            Math::division_no_overflow(w, scalar)
        );
    }
    constexpr Vector4i operator%(i32 scalar) const noexcept {
        return Vector4i(
            Math::modulo_no_overflow(x, scalar),
            Math::modulo_no_overflow(y, scalar),
            Math::modulo_no_overflow(z, scalar),
            Math::modulo_no_overflow(w, scalar)
        );
    }

    constexpr Vector4i operator-() const noexcept { return Vector4i(-x, -y, -z, -w); }

    constexpr Vector4i &operator+=(const Vector4i &rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
    constexpr Vector4i &operator-=(const Vector4i &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
    constexpr Vector4i &operator*=(const Vector4i &rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
    constexpr Vector4i &operator/=(const Vector4i &rhs) noexcept {
        x = Math::division_no_overflow(x, rhs.x);
        y = Math::division_no_overflow(y, rhs.y);
        z = Math::division_no_overflow(z, rhs.z);
        w = Math::division_no_overflow(w, rhs.w);
        return *this;
    }
    constexpr Vector4i &operator%=(const Vector4i &rhs) noexcept {
        x = Math::modulo_no_overflow(x, rhs.x);
        y = Math::modulo_no_overflow(y, rhs.y);
        z = Math::modulo_no_overflow(z, rhs.z);
        w = Math::modulo_no_overflow(w, rhs.w);
        return *this;
    }

    constexpr Vector4i &operator*=(i32 scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    constexpr Vector4i &operator/=(i32 scalar) noexcept {
        x = Math::division_no_overflow(x, scalar);
        y = Math::division_no_overflow(y, scalar);
        z = Math::division_no_overflow(z, scalar);
        w = Math::division_no_overflow(w, scalar);
        return *this;
    }
    constexpr Vector4i &operator%=(i32 scalar) noexcept {
        x = Math::modulo_no_overflow(x, scalar);
        y = Math::modulo_no_overflow(y, scalar);
        z = Math::modulo_no_overflow(z, scalar);
        w = Math::modulo_no_overflow(w, scalar);
        return *this;
    }

    constexpr bool operator==(const Vector4i &rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    constexpr auto operator<=>(const Vector4i &rhs) const noexcept {
        if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
        if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
        if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
        return w <=> rhs.w;
    }

    [[nodiscard]] explicit constexpr operator Vector4() const noexcept {
        return Vector4((real_t)x, (real_t)y, (real_t)z, (real_t)w);
    }
};

inline constexpr Vector4i Vector4i::ZERO = { 0, 0, 0, 0 };
inline constexpr Vector4i Vector4i::ONE = { 1, 1, 1, 1 };

[[nodiscard]] constexpr Vector4i operator*(i32 scalar, const Vector4i &vec) noexcept {
    return vec * scalar;
}

constexpr Vector4::Vector4(const Vector4i &v) noexcept : x((real_t)v.x), y((real_t)v.y), z((real_t)v.z), w((real_t)v.w) {}

constexpr Vector4::operator Vector4i() const noexcept {
    return Vector4i((i32)x, (i32)y, (i32)z, (i32)w);
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector4i> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector4i &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};
