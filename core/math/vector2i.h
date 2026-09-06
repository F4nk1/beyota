/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  vector2i.h                                                            */
/**************************************************************************/

#pragma once

#include "core/math/vector2.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Vector2i {
    static const Vector2i ZERO;
    static const Vector2i ONE;
    static const Vector2i LEFT;
    static const Vector2i RIGHT;
    static const Vector2i UP;
    static const Vector2i DOWN;

    union {
        i32 x{0};
        i32 width;
    };
    union {
        i32 y{0};
        i32 height;
    };

    constexpr Vector2i() = default;
    constexpr Vector2i(i32 x, i32 y) noexcept : x(x), y(y) {}
    explicit constexpr Vector2i(const Vector2 &v) noexcept : x((i32)v.x), y((i32)v.y) {}

    [[nodiscard]] constexpr i32 &operator[](usize axis) noexcept {
        assert(axis < 2);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr const i32 &operator[](usize axis) const noexcept {
        assert(axis < 2);
        return (&x)[axis];
    }

    [[nodiscard]] constexpr Axis min_axis_index() const noexcept {
        return x < y ? Axis::X : Axis::Y;
    }

    [[nodiscard]] constexpr Axis max_axis_index() const noexcept {
        return x < y ? Axis::Y : Axis::X;
    }

    [[nodiscard]] constexpr i64 length_squared() const noexcept {
        return (i64)x * (i64)x + (i64)y * (i64)y;
    }

    [[nodiscard]] double length() const noexcept {
        return std::sqrt((double)length_squared());
    }

    [[nodiscard]] double distance_to(const Vector2i &to) const noexcept {
        return (to - *this).length();
    }

    [[nodiscard]] constexpr i64 distance_squared_to(const Vector2i &to) const noexcept {
        return (to - *this).length_squared();
    }

    constexpr void zero() noexcept {
        x = 0;
        y = 0;
    }

    [[nodiscard]] Vector2i min(const Vector2i &other) const noexcept {
        return Vector2i(std::min(x, other.x), std::min(y, other.y));
    }

    [[nodiscard]] Vector2i mini(i32 scalar) const noexcept {
        return Vector2i(std::min(x, scalar), std::min(y, scalar));
    }

    [[nodiscard]] Vector2i max(const Vector2i &other) const noexcept {
        return Vector2i(std::max(x, other.x), std::max(y, other.y));
    }

    [[nodiscard]] Vector2i maxi(i32 scalar) const noexcept {
        return Vector2i(std::max(x, scalar), std::max(y, scalar));
    }

    [[nodiscard]] Vector2i clamp(const Vector2i &min_v, const Vector2i &max_v) const noexcept {
        return Vector2i(std::clamp(x, min_v.x, max_v.x), std::clamp(y, min_v.y, max_v.y));
    }

    [[nodiscard]] Vector2i clampi(i32 min_v, i32 max_v) const noexcept {
        return Vector2i(std::clamp(x, min_v, max_v), std::clamp(y, min_v, max_v));
    }

    [[nodiscard]] Vector2i snapped(const Vector2i &step) const noexcept {
        return Vector2i(
            (i32)Math::snapped((real_t)x, (real_t)step.x),
            (i32)Math::snapped((real_t)y, (real_t)step.y)
        );
    }

    [[nodiscard]] Vector2i snappedi(i32 step) const noexcept {
        return Vector2i(
            (i32)Math::snapped((real_t)x, (real_t)step),
            (i32)Math::snapped((real_t)y, (real_t)step)
        );
    }

    [[nodiscard]] constexpr Vector2i sign() const noexcept {
        return Vector2i(Math::sign(x), Math::sign(y));
    }

    [[nodiscard]] constexpr Vector2i abs() const noexcept {
        return Vector2i(Math::abs(x), Math::abs(y));
    }

    [[nodiscard]] constexpr real_t aspect() const noexcept {
        return (real_t)width / (real_t)height;
    }

    constexpr Vector2i operator+(const Vector2i &rhs) const noexcept { return Vector2i(x + rhs.x, y + rhs.y); }
    constexpr Vector2i operator-(const Vector2i &rhs) const noexcept { return Vector2i(x - rhs.x, y - rhs.y); }
    constexpr Vector2i operator*(const Vector2i &rhs) const noexcept { return Vector2i(x * rhs.x, y * rhs.y); }
    constexpr Vector2i operator/(const Vector2i &rhs) const noexcept {
        return Vector2i(Math::division_no_overflow(x, rhs.x), Math::division_no_overflow(y, rhs.y));
    }
    constexpr Vector2i operator%(const Vector2i &rhs) const noexcept {
        return Vector2i(Math::modulo_no_overflow(x, rhs.x), Math::modulo_no_overflow(y, rhs.y));
    }

    constexpr Vector2i operator*(i32 scalar) const noexcept { return Vector2i(x * scalar, y * scalar); }
    constexpr Vector2i operator/(i32 scalar) const noexcept {
        return Vector2i(Math::division_no_overflow(x, scalar), Math::division_no_overflow(y, scalar));
    }
    constexpr Vector2i operator%(i32 scalar) const noexcept {
        return Vector2i(Math::modulo_no_overflow(x, scalar), Math::modulo_no_overflow(y, scalar));
    }

    constexpr Vector2i operator-() const noexcept { return Vector2i(-x, -y); }

    constexpr Vector2i &operator+=(const Vector2i &rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vector2i &operator-=(const Vector2i &rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vector2i &operator*=(const Vector2i &rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
    constexpr Vector2i &operator/=(const Vector2i &rhs) noexcept {
        x = Math::division_no_overflow(x, rhs.x);
        y = Math::division_no_overflow(y, rhs.y);
        return *this;
    }
    constexpr Vector2i &operator%=(const Vector2i &rhs) noexcept {
        x = Math::modulo_no_overflow(x, rhs.x);
        y = Math::modulo_no_overflow(y, rhs.y);
        return *this;
    }

    constexpr Vector2i &operator*=(i32 scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    constexpr Vector2i &operator/=(i32 scalar) noexcept {
        x = Math::division_no_overflow(x, scalar);
        y = Math::division_no_overflow(y, scalar);
        return *this;
    }
    constexpr Vector2i &operator%=(i32 scalar) noexcept {
        x = Math::modulo_no_overflow(x, scalar);
        y = Math::modulo_no_overflow(y, scalar);
        return *this;
    }

    constexpr bool operator==(const Vector2i &rhs) const noexcept { return x == rhs.x && y == rhs.y; }
    constexpr auto operator<=>(const Vector2i &rhs) const noexcept { if (auto cmp = x <=> rhs.x; cmp != 0) return cmp; return y <=> rhs.y; }

    [[nodiscard]] explicit constexpr operator Vector2() const noexcept {
        return Vector2((real_t)x, (real_t)y);
    }
};

inline constexpr Vector2i Vector2i::ZERO = { 0, 0 };
inline constexpr Vector2i Vector2i::ONE = { 1, 1 };
inline constexpr Vector2i Vector2i::LEFT = { -1, 0 };
inline constexpr Vector2i Vector2i::RIGHT = { 1, 0 };
inline constexpr Vector2i Vector2i::UP = { 0, -1 };
inline constexpr Vector2i Vector2i::DOWN = { 0, 1 };

[[nodiscard]] constexpr Vector2i operator*(i32 scalar, const Vector2i &vec) noexcept {
    return vec * scalar;
}

constexpr Vector2::Vector2(const Vector2i &v) noexcept : x((real_t)v.x), y((real_t)v.y) {}

constexpr Vector2::operator Vector2i() const noexcept {
    return Vector2i((i32)x, (i32)y);
}

using Point2i = Vector2i;
using Size2i = Vector2i;

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Vector2i> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Vector2i &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};
