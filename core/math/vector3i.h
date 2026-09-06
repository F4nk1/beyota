/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector3i.h                                                            */
/**************************************************************************/

#pragma once

#include "core/math/vector3.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Vector3i {
    static const Vector3i ZERO;
    static const Vector3i ONE;
    static const Vector3i LEFT;
    static const Vector3i RIGHT;
    static const Vector3i UP;
    static const Vector3i DOWN;
    static const Vector3i FORWARD;
    static const Vector3i BACK;

    i32 x{0};
    i32 y{0};
    i32 z{0};

    constexpr Vector3i() = default;
    constexpr Vector3i(i32 x, i32 y, i32 z) noexcept : x(x), y(y), z(z) {}
    explicit constexpr Vector3i(const Vector3 &v) noexcept : x((i32)v.x), y((i32)v.y), z((i32)v.z) {}

    [[nodiscard]] constexpr i32 &operator[](usize axis) noexcept {
        assert(axis < 3);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const i32 &operator[](usize axis) const noexcept {
        assert(axis < 3);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        return x < y ? (x < z ? Axis::X : Axis::Z) : (y < z ? Axis::Y : Axis::Z);
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        return x < y ? (y < z ? Axis::Z : Axis::Y) : (x < z ? Axis::Z : Axis::X);
    }

    [[nodiscard]] constexpr i64 length_squared() const noexcept {
        return (i64)x * (i64)x + (i64)y * (i64)y + (i64)z * (i64)z;
    }

    [[nodiscard]] double length() const noexcept {
        return std::sqrt((double)length_squared());
    }

    [[nodiscard]] double distance_to(const Vector3i &to) const noexcept {
        return (to - *this).length();
    }

    [[nodiscard]] constexpr i64 distance_squared_to(const Vector3i &to) const noexcept {
        return (to - *this).length_squared();
    }

    constexpr void zero() noexcept {
        x = 0;
        y = 0;
        z = 0;
    }

    [[nodiscard]] Vector3i min(const Vector3i &other) const noexcept {
        return Vector3i(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }

    [[nodiscard]] Vector3i mini(i32 scalar) const noexcept {
        return Vector3i(std::min(x, scalar), std::min(y, scalar), std::min(z, scalar));
    }

    [[nodiscard]] Vector3i max(const Vector3i &other) const noexcept {
        return Vector3i(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }

    [[nodiscard]] Vector3i maxi(i32 scalar) const noexcept {
        return Vector3i(std::max(x, scalar), std::max(y, scalar), std::max(z, scalar));
    }

    [[nodiscard]] Vector3i clamp(const Vector3i &min_v, const Vector3i &max_v) const noexcept {
        return Vector3i(std::clamp(x, min_v.x, max_v.x), std::clamp(y, min_v.y, max_v.y), std::clamp(z, min_v.z, max_v.z));
    }

    [[nodiscard]] Vector3i clampi(i32 min_v, i32 max_v) const noexcept {
        return Vector3i(std::clamp(x, min_v, max_v), std::clamp(y, min_v, max_v), std::clamp(z, min_v, max_v));
    }

    [[nodiscard]] Vector3i snapped(const Vector3i &step) const noexcept {
        return Vector3i(
            (i32)Math::snapped((real_t)x, (real_t)step.x),
            (i32)Math::snapped((real_t)y, (real_t)step.y),
            (i32)Math::snapped((real_t)z, (real_t)step.z)
        );
    }

    [[nodiscard]] Vector3i snappedi(i32 step) const noexcept {
        return Vector3i(
            (i32)Math::snapped((real_t)x, (real_t)step),
            (i32)Math::snapped((real_t)y, (real_t)step),
            (i32)Math::snapped((real_t)z, (real_t)step)
        );
    }

    [[nodiscard]] constexpr Vector3i sign() const noexcept {
        return Vector3i(Math::sign(x), Math::sign(y), Math::sign(z));
    }

    [[nodiscard]] constexpr Vector3i abs() const noexcept {
        return Vector3i(Math::abs(x), Math::abs(y), Math::abs(z));
    }

    constexpr Vector3i operator+(const Vector3i &rhs) const noexcept { return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z); }
    constexpr Vector3i operator-(const Vector3i &rhs) const noexcept { return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z); }
    constexpr Vector3i operator*(const Vector3i &rhs) const noexcept { return Vector3i(x * rhs.x, y * rhs.y, z * rhs.z); }
    constexpr Vector3i operator/(const Vector3i &rhs) const noexcept {
        return Vector3i(
            Math::division_no_overflow(x, rhs.x),
            Math::division_no_overflow(y, rhs.y),
            Math::division_no_overflow(z, rhs.z)
        );
    }
    constexpr Vector3i operator%(const Vector3i &rhs) const noexcept {
        return Vector3i(
            Math::modulo_no_overflow(x, rhs.x),
            Math::modulo_no_overflow(y, rhs.y),
            Math::modulo_no_overflow(z, rhs.z)
        );
    }

    constexpr Vector3i operator*(i32 scalar) const noexcept { return Vector3i(x * scalar, y * scalar, z * scalar); }
    constexpr Vector3i operator/(i32 scalar) const noexcept {
        return Vector3i(
            Math::division_no_overflow(x, scalar),
            Math::division_no_overflow(y, scalar),
            Math::division_no_overflow(z, scalar)
        );
    }
    constexpr Vector3i operator%(i32 scalar) const noexcept {
        return Vector3i(
            Math::modulo_no_overflow(x, scalar),
            Math::modulo_no_overflow(y, scalar),
            Math::modulo_no_overflow(z, scalar)
        );
    }

    constexpr Vector3i operator-() const noexcept { return Vector3i(-x, -y, -z); }

    constexpr Vector3i &operator+=(const Vector3i &rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    constexpr Vector3i &operator-=(const Vector3i &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    constexpr Vector3i &operator*=(const Vector3i &rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    constexpr Vector3i &operator/=(const Vector3i &rhs) noexcept {
        x = Math::division_no_overflow(x, rhs.x);
        y = Math::division_no_overflow(y, rhs.y);
        z = Math::division_no_overflow(z, rhs.z);
        return *this;
    }
    constexpr Vector3i &operator%=(const Vector3i &rhs) noexcept {
        x = Math::modulo_no_overflow(x, rhs.x);
        y = Math::modulo_no_overflow(y, rhs.y);
        z = Math::modulo_no_overflow(z, rhs.z);
        return *this;
    }

    constexpr Vector3i &operator*=(i32 scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    constexpr Vector3i &operator/=(i32 scalar) noexcept {
        x = Math::division_no_overflow(x, scalar);
        y = Math::division_no_overflow(y, scalar);
        z = Math::division_no_overflow(z, scalar);
        return *this;
    }
    constexpr Vector3i &operator%=(i32 scalar) noexcept {
        x = Math::modulo_no_overflow(x, scalar);
        y = Math::modulo_no_overflow(y, scalar);
        z = Math::modulo_no_overflow(z, scalar);
        return *this;
    }

    constexpr bool operator==(const Vector3i &rhs) const noexcept {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    constexpr auto operator<=>(const Vector3i &rhs) const noexcept {
        if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
        if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
        return z <=> rhs.z;
    }

    [[nodiscard]] explicit constexpr operator Vector3() const noexcept {
        return Vector3((real_t)x, (real_t)y, (real_t)z);
    }
};

inline constexpr Vector3i Vector3i::ZERO = { 0, 0, 0 };
inline constexpr Vector3i Vector3i::ONE = { 1, 1, 1 };
inline constexpr Vector3i Vector3i::LEFT = { -1, 0, 0 };
inline constexpr Vector3i Vector3i::RIGHT = { 1, 0, 0 };
inline constexpr Vector3i Vector3i::UP = { 0, 1, 0 };
inline constexpr Vector3i Vector3i::DOWN = { 0, -1, 0 };
inline constexpr Vector3i Vector3i::FORWARD = { 0, 0, -1 };
inline constexpr Vector3i Vector3i::BACK = { 0, 0, 1 };

[[nodiscard]] constexpr Vector3i operator*(i32 scalar, const Vector3i &vec) noexcept {
    return vec * scalar;
}

constexpr Vector3::Vector3(const Vector3i &v) noexcept : x((real_t)v.x), y((real_t)v.y), z((real_t)v.z) {}

constexpr Vector3::operator Vector3i() const noexcept {
    return Vector3i((i32)x, (i32)y, (i32)z);
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector3i> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector3i &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};
