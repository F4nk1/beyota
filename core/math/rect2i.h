/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  rect2i.h                                                              */
/**************************************************************************/

#pragma once

#include "core/math/rect2.h"
#include "core/math/vector2i.h"

#include <algorithm>
#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Rect2i {
    Point2i position;
    Size2i size;

    constexpr Rect2i() = default;
    constexpr Rect2i(i32 x, i32 y, i32 width, i32 height) noexcept : position(x, y), size(width, height) {}
    constexpr Rect2i(const Point2i &pos, const Size2i &sz) noexcept : position(pos), size(sz) {}
    explicit constexpr Rect2i(const Rect2 &r) noexcept : position((Point2i)r.position), size((Size2i)r.size) {}

    [[nodiscard]] constexpr const Point2i &get_position() const noexcept { return position; }
    constexpr void set_position(const Point2i &pos) noexcept { position = pos; }
    [[nodiscard]] constexpr const Size2i &get_size() const noexcept { return size; }
    constexpr void set_size(const Size2i &sz) noexcept { size = sz; }

    [[nodiscard]] constexpr i64 get_area() const noexcept { return (i64)size.width * (i64)size.height; }
    [[nodiscard]] constexpr Point2i get_center() const noexcept { return position + (size / 2); }
    [[nodiscard]] constexpr Point2i get_end() const noexcept { return position + size; }
    constexpr void set_end(const Point2i &end) noexcept { size = end - position; }

    [[nodiscard]] constexpr bool intersects(const Rect2i &other) const noexcept {
        if (position.x >= (other.position.x + other.size.width)) return false;
        if ((position.x + size.width) <= other.position.x) return false;
        if (position.y >= (other.position.y + other.size.height)) return false;
        if ((position.y + size.height) <= other.position.y) return false;
        return true;
    }

    [[nodiscard]] constexpr bool encloses(const Rect2i &other) const noexcept {
        return (other.position.x >= position.x) && (other.position.y >= position.y) &&
               ((other.position.x + other.size.x) <= (position.x + size.x)) &&
               ((other.position.y + other.size.y) <= (position.y + size.y));
    }

    [[nodiscard]] constexpr bool has_point(const Point2i &point) const noexcept {
        if (point.x < position.x) return false;
        if (point.y < position.y) return false;
        if (point.x >= (position.x + size.x)) return false;
        if (point.y >= (position.y + size.y)) return false;
        return true;
    }

    [[nodiscard]] Rect2i merge(const Rect2i &other) const noexcept {
        Rect2i new_rect;
        new_rect.position.x = std::min(position.x, other.position.x);
        new_rect.position.y = std::min(position.y, other.position.y);
        new_rect.size.x = std::max(position.x + size.x, other.position.x + other.size.x) - new_rect.position.x;
        new_rect.size.y = std::max(position.y + size.y, other.position.y + other.size.y) - new_rect.position.y;
        return new_rect;
    }

    [[nodiscard]] Rect2i intersection(const Rect2i &other) const noexcept {
        Rect2i new_rect = other;
        if (!intersects(new_rect)) return Rect2i();
        new_rect.position.x = std::max(other.position.x, position.x);
        new_rect.position.y = std::max(other.position.y, position.y);
        Point2i other_end = other.position + other.size;
        Point2i this_end = position + size;
        new_rect.size.x = std::min(other_end.x, this_end.x) - new_rect.position.x;
        new_rect.size.y = std::min(other_end.y, this_end.y) - new_rect.position.y;
        return new_rect;
    }

    [[nodiscard]] constexpr Rect2i grow(i32 amount) const noexcept {
        Rect2i r = *this;
        r.position.x -= amount;
        r.position.y -= amount;
        r.size.x += 2 * amount;
        r.size.y += 2 * amount;
        return r;
    }

    [[nodiscard]] constexpr Rect2i grow_side(Side side, i32 amount) const noexcept {
        Rect2i r = *this;
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

    [[nodiscard]] constexpr Rect2i grow_individual(i32 left, i32 top, i32 right, i32 bottom) const noexcept {
        Rect2i r = *this;
        r.position.x -= left;
        r.position.y -= top;
        r.size.x += left + right;
        r.size.y += top + bottom;
        return r;
    }

    [[nodiscard]] Rect2i expand(const Point2i &point) const noexcept {
        Rect2i r = *this;
        r.expand_to(point);
        return r;
    }

    void expand_to(const Point2i &vector) noexcept {
        Point2i begin = position;
        Point2i end = position + size;

        if (vector.x < begin.x) begin.x = vector.x;
        if (vector.y < begin.y) begin.y = vector.y;
        if (vector.x > end.x) end.x = vector.x;
        if (vector.y > end.y) end.y = vector.y;

        position = begin;
        size = end - begin;
    }

    [[nodiscard]] constexpr Rect2i abs() const noexcept {
        return Rect2i(
            position.x + (size.x < 0 ? size.x : 0),
            position.y + (size.y < 0 ? size.y : 0),
            Math::abs(size.x),
            Math::abs(size.y)
        );
    }

    constexpr bool operator==(const Rect2i &rhs) const noexcept {
        return position == rhs.position && size == rhs.size;
    }

    [[nodiscard]] explicit constexpr operator Rect2() const noexcept {
        return Rect2((Point2)position, (Size2)size);
    }
};

constexpr Rect2::Rect2(const Rect2i &r) noexcept : position((Point2)r.position), size((Size2)r.size) {}

constexpr Rect2::operator Rect2i() const noexcept {
    return Rect2i((Point2i)position, (Size2i)size);
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Rect2i> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Rect2i &r, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[P: ({}, {}), S: ({}, {})]", r.position.x, r.position.y, r.size.x, r.size.y);
    }
};
