/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  rect2.h                                                               */
/**************************************************************************/

#pragma once

#include "core/math/vector2.h"

#include <algorithm>
#include <cassert>
#include <format>

namespace Beyota::Math {

struct Rect2i;
struct Transform2D;

struct [[nodiscard]] Rect2 {
    Point2 position;
    Size2 size;

    constexpr Rect2() = default;
    constexpr Rect2(real_t x, real_t y, real_t width, real_t height) noexcept : position(x, y), size(width, height) {}
    constexpr Rect2(const Point2 &pos, const Size2 &sz) noexcept : position(pos), size(sz) {}
    explicit constexpr Rect2(const Rect2i &r) noexcept;

    [[nodiscard]] constexpr const Vector2 &get_position() const noexcept { return position; }
    constexpr void set_position(const Vector2 &pos) noexcept { position = pos; }
    [[nodiscard]] constexpr const Vector2 &get_size() const noexcept { return size; }
    constexpr void set_size(const Vector2 &sz) noexcept { size = sz; }

    [[nodiscard]] constexpr real_t get_area() const noexcept { return size.width * size.height; }
    [[nodiscard]] constexpr Vector2 get_center() const noexcept { return position + (size * (real_t)0.5); }
    [[nodiscard]] constexpr Vector2 get_end() const noexcept { return position + size; }
    constexpr void set_end(const Vector2 &end) noexcept { size = end - position; }

    [[nodiscard]] constexpr bool intersects(const Rect2 &other, bool include_borders = false) const noexcept {
        if (include_borders) {
            if (position.x > (other.position.x + other.size.width)) return false;
            if ((position.x + size.width) < other.position.x) return false;
            if (position.y > (other.position.y + other.size.height)) return false;
            if ((position.y + size.height) < other.position.y) return false;
        } else {
            if (position.x >= (other.position.x + other.size.width)) return false;
            if ((position.x + size.width) <= other.position.x) return false;
            if (position.y >= (other.position.y + other.size.height)) return false;
            if ((position.y + size.height) <= other.position.y) return false;
        }
        return true;
    }

    [[nodiscard]] real_t distance_to(const Vector2 &point) const noexcept {
        real_t dist = (real_t)0.0;
        bool inside = true;

        if (point.x < position.x) {
            real_t d = position.x - point.x;
            dist = d;
            inside = false;
        }
        if (point.x > position.x + size.x) {
            real_t d = point.x - (position.x + size.x);
            dist = (!inside) ? Math::hypot(dist, d) : d;
            inside = false;
        }
        if (point.y < position.y) {
            real_t d = position.y - point.y;
            dist = (!inside) ? Math::hypot(dist, d) : d;
            inside = false;
        }
        if (point.y > position.y + size.y) {
            real_t d = point.y - (position.y + size.y);
            dist = (!inside) ? Math::hypot(dist, d) : d;
            inside = false;
        }

        return inside ? (real_t)0.0 : dist;
    }

    [[nodiscard]] bool intersects_segment(const Point2 &from, const Point2 &to, Point2 *pos = nullptr, Point2 *normal = nullptr) const noexcept {
        real_t min = (real_t)0.0, max = (real_t)1.0;
        usize axis = 0;
        real_t sgn = (real_t)0.0;

        for (usize i = 0; i < 2; i++) {
            real_t seg_from = from[i];
            real_t seg_to = to[i];
            real_t box_begin = position[i];
            real_t box_end = box_begin + size[i];
            real_t cmin, cmax;
            real_t csign;

            if (seg_from < seg_to) {
                if (seg_from > box_end || seg_to < box_begin) return false;
                real_t length = seg_to - seg_from;
                cmin = (seg_from < box_begin) ? ((box_begin - seg_from) / length) : (real_t)0.0;
                cmax = (seg_to > box_end) ? ((box_end - seg_from) / length) : (real_t)1.0;
                csign = (real_t)-1.0;
            } else {
                if (seg_to > box_end || seg_from < box_begin) return false;
                real_t length = seg_to - seg_from;
                cmin = (seg_from > box_end) ? ((box_end - seg_from) / length) : (real_t)0.0;
                cmax = (seg_to < box_begin) ? ((box_begin - seg_from) / length) : (real_t)1.0;
                csign = (real_t)1.0;
            }

            if (cmin > min) {
                min = cmin;
                axis = i;
                sgn = csign;
            }
            if (cmax < max) {
                max = cmax;
            }
            if (max < min) {
                return false;
            }
        }

        Vector2 rel = to - from;
        if (normal) {
            Vector2 norm;
            norm[axis] = sgn;
            *normal = norm;
        }
        if (pos) {
            *pos = from + rel * min;
        }
        return true;
    }

    [[nodiscard]] bool intersects_ray(const Point2 &from, const Vector2 &dir_vec, Point2 *pos = nullptr) const noexcept {
        Vector2 dir = dir_vec.normalized();
        if (Math::is_zero_approx(dir.x)) dir.x = (real_t)1e-10;
        if (Math::is_zero_approx(dir.y)) dir.y = (real_t)1e-10;

        real_t t_x1 = (position.x - from.x) / dir.x;
        real_t t_x2 = (position.x + size.x - from.x) / dir.x;
        real_t t_y1 = (position.y - from.y) / dir.y;
        real_t t_y2 = (position.y + size.y - from.y) / dir.y;

        real_t t_min = std::max(std::min(t_x1, t_x2), std::min(t_y1, t_y2));
        real_t t_max = std::min(std::max(t_x1, t_x2), std::max(t_y1, t_y2));

        if (t_max < (real_t)0.0 || t_min > t_max) return false;
        if (pos) {
            *pos = from + dir * t_min;
        }
        return true;
    }

    [[nodiscard]] constexpr bool encloses(const Rect2 &other) const noexcept {
        return (other.position.x >= position.x) && (other.position.y >= position.y) &&
               ((other.position.x + other.size.x) <= (position.x + size.x)) &&
               ((other.position.y + other.size.y) <= (position.y + size.y));
    }

    [[nodiscard]] constexpr bool has_point(const Vector2 &point) const noexcept {
        if (point.x < position.x) return false;
        if (point.y < position.y) return false;
        if (point.x >= (position.x + size.x)) return false;
        if (point.y >= (position.y + size.y)) return false;
        return true;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Rect2 &other) const noexcept {
        return position.is_equal_approx(other.position) && size.is_equal_approx(other.size);
    }

    [[nodiscard]] constexpr bool is_same(const Rect2 &other) const noexcept {
        return position.is_same(other.position) && size.is_same(other.size);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return position.is_finite() && size.is_finite();
    }

    [[nodiscard]] Rect2 merge(const Rect2 &other) const noexcept {
        Rect2 new_rect;
        new_rect.position.x = std::min(position.x, other.position.x);
        new_rect.position.y = std::min(position.y, other.position.y);
        new_rect.size.x = std::max(position.x + size.x, other.position.x + other.size.x) - new_rect.position.x;
        new_rect.size.y = std::max(position.y + size.y, other.position.y + other.size.y) - new_rect.position.y;
        return new_rect;
    }

    [[nodiscard]] Rect2 intersection(const Rect2 &other) const noexcept {
        Rect2 new_rect = other;
        if (!intersects(new_rect)) return Rect2();
        new_rect.position.x = std::max(other.position.x, position.x);
        new_rect.position.y = std::max(other.position.y, position.y);
        Point2 other_end = other.position + other.size;
        Point2 this_end = position + size;
        new_rect.size.x = std::min(other_end.x, this_end.x) - new_rect.position.x;
        new_rect.size.y = std::min(other_end.y, this_end.y) - new_rect.position.y;
        return new_rect;
    }

    [[nodiscard]] Rect2 clip(const Rect2 &other) const noexcept {
        return intersection(other);
    }

    [[nodiscard]] constexpr Rect2 grow(real_t amount) const noexcept {
        Rect2 r = *this;
        r.position.x -= amount;
        r.position.y -= amount;
        r.size.x += (real_t)2.0 * amount;
        r.size.y += (real_t)2.0 * amount;
        return r;
    }

    [[nodiscard]] constexpr Rect2 grow_side(Side side, real_t amount) const noexcept {
        Rect2 r = *this;
        switch (side) {
            case Side::Left:
                r.position.x -= amount;
                r.size.x += amount;
                break;
            case Side::Top:
                r.position.y -= amount;
                r.size.y += amount;
                break;
            case Side::Right:
                r.size.x += amount;
                break;
            case Side::Bottom:
                r.size.y += amount;
                break;
        }
        return r;
    }

    [[nodiscard]] constexpr Rect2 grow_individual(real_t left, real_t top, real_t right, real_t bottom) const noexcept {
        Rect2 r = *this;
        r.position.x -= left;
        r.position.y -= top;
        r.size.x += left + right;
        r.size.y += top + bottom;
        return r;
    }

    [[nodiscard]] Rect2 expand(const Vector2 &vector) const noexcept {
        Rect2 r = *this;
        r.expand_to(vector);
        return r;
    }

    void expand_to(const Vector2 &vector) noexcept {
        Point2 begin = position;
        Point2 end = position + size;

        if (vector.x < begin.x) begin.x = vector.x;
        if (vector.y < begin.y) begin.y = vector.y;
        if (vector.x > end.x) end.x = vector.x;
        if (vector.y > end.y) end.y = vector.y;

        position = begin;
        size = end - begin;
    }

    [[nodiscard]] constexpr Rect2 abs() const noexcept {
        return Rect2(
            position.x + (size.x < (real_t)0 ? size.x : (real_t)0),
            position.y + (size.y < (real_t)0 ? size.y : (real_t)0),
            Math::abs(size.x),
            Math::abs(size.y)
        );
    }

    constexpr bool operator==(const Rect2 &rhs) const noexcept {
        return position == rhs.position && size == rhs.size;
    }

    [[nodiscard]] explicit constexpr operator Rect2i() const noexcept;
};

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Rect2> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Rect2 &r, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[P: ({}, {}), S: ({}, {})]", r.position.x, r.position.y, r.size.x, r.size.y);
    }
};
