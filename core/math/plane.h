/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  plane.h                                                               */
/**************************************************************************/

#pragma once

#include "core/math/vector3.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Plane {
    static const Plane PLANE_YZ;
    static const Plane PLANE_XZ;
    static const Plane PLANE_XY;

    Vector3 normal;
    real_t d{0};

    constexpr Plane() = default;
    constexpr Plane(real_t a, real_t b, real_t c, real_t d) noexcept : normal(a, b, c), d(d) {}
    constexpr Plane(const Vector3 &normal, real_t d = (real_t)0.0) noexcept : normal(normal), d(d) {}
    constexpr Plane(const Vector3 &normal, const Vector3 &point) noexcept : normal(normal), d(normal.dot(point)) {}
    Plane(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, ClockDirection dir = ClockDirection::Clockwise) noexcept {
        if (dir == ClockDirection::Clockwise) {
            normal = (p1 - p3).cross(p1 - p2);
        } else {
            normal = (p1 - p2).cross(p1 - p3);
        }
        normal.normalize();
        d = normal.dot(p1);
    }

    constexpr void set_normal(const Vector3 &n) noexcept { normal = n; }
    [[nodiscard]] constexpr const Vector3 &get_normal() const noexcept { return normal; }

    constexpr void zero() noexcept {
        normal.zero();
        d = (real_t)0.0;
    }

    void normalize() noexcept {
        real_t l = normal.length();
        if (l == (real_t)0.0) {
            zero();
            return;
        }
        normal /= l;
        d /= l;
    }

    [[nodiscard]] Plane normalized() const noexcept {
        Plane p = *this;
        p.normalize();
        return p;
    }

    [[nodiscard]] constexpr Vector3 get_center() const noexcept { return normal * d; }

    [[nodiscard]] Vector3 get_any_perpendicular_normal() const noexcept {
        static constexpr Vector3 p1 = Vector3(1, 0, 0);
        static constexpr Vector3 p2 = Vector3(0, 1, 0);
        Vector3 p = (Math::abs(normal.dot(p1)) > (real_t)0.99) ? p2 : p1;
        p -= normal * normal.dot(p);
        p.normalize();
        return p;
    }

    [[nodiscard]] constexpr bool is_point_over(const Vector3 &point) const noexcept {
        return normal.dot(point) > d;
    }

    [[nodiscard]] constexpr real_t distance_to(const Vector3 &point) const noexcept {
        return normal.dot(point) - d;
    }

    [[nodiscard]] constexpr bool has_point(const Vector3 &point, real_t tolerance = CMP_EPSILON<real_t>) const noexcept {
        const real_t dist = distance_to(point);
        return (dist < (real_t)0 ? -dist : dist) <= tolerance;
    }

    [[nodiscard]] constexpr Vector3 project(const Vector3 &point) const noexcept {
        return point - normal * distance_to(point);
    }

    [[nodiscard]] bool intersect_3(const Plane &p1, const Plane &p2, Vector3 *result = nullptr) const noexcept {
        const Vector3 n0 = normal;
        const Vector3 n1 = p1.normal;
        const Vector3 n2 = p2.normal;

        const real_t denom = n0.cross(n1).dot(n2);
        if (Math::is_zero_approx(denom)) {
            return false;
        }

        if (result) {
            *result = ((n1.cross(n2) * d) +
                       (n2.cross(n0) * p1.d) +
                       (n0.cross(n1) * p2.d)) / denom;
        }
        return true;
    }

    [[nodiscard]] bool intersects_ray(const Vector3 &from, const Vector3 &dir, Vector3 *intersection = nullptr) const noexcept {
        const real_t den = normal.dot(dir);
        if (Math::is_zero_approx(den)) {
            return false;
        }

        const real_t dist = (normal.dot(from) - d) / den;
        if (dist > CMP_EPSILON<real_t>) {
            return false;
        }

        if (intersection) {
            *intersection = from - dir * dist;
        }
        return true;
    }

    [[nodiscard]] bool intersects_segment(const Vector3 &begin, const Vector3 &end, Vector3 *intersection = nullptr) const noexcept {
        const Vector3 segment = begin - end;
        const real_t den = normal.dot(segment);
        if (Math::is_zero_approx(den)) {
            return false;
        }

        const real_t dist = (normal.dot(begin) - d) / den;
        if (dist < -CMP_EPSILON<real_t> || dist > ((real_t)1.0 + CMP_EPSILON<real_t>)) {
            return false;
        }

        if (intersection) {
            *intersection = begin - segment * dist;
        }
        return true;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Plane &other) const noexcept {
        return normal.is_equal_approx(other.normal) && Math::is_equal_approx(d, other.d);
    }

    [[nodiscard]] constexpr bool is_equal_approx_any_side(const Plane &other) const noexcept {
        return (normal.is_equal_approx(other.normal) && Math::is_equal_approx(d, other.d)) ||
               (normal.is_equal_approx(-other.normal) && Math::is_equal_approx(d, -other.d));
    }

    [[nodiscard]] constexpr bool is_same(const Plane &other) const noexcept {
        return normal.is_same(other.normal) && Math::is_same(d, other.d);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return normal.is_finite() && Math::is_finite(d);
    }

    constexpr Plane operator-() const noexcept { return Plane(-normal, -d); }
    constexpr bool operator==(const Plane &rhs) const noexcept {
        return normal == rhs.normal && d == rhs.d;
    }
};

inline constexpr Plane Plane::PLANE_YZ = { 1, 0, 0, 0 };
inline constexpr Plane Plane::PLANE_XZ = { 0, 1, 0, 0 };
inline constexpr Plane Plane::PLANE_XY = { 0, 0, 1, 0 };

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Plane> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Plane &p, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[N: ({}, {}, {}), D: {}]", p.normal.x, p.normal.y, p.normal.z, p.d);
    }
};
