/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  aabb.h                                                                */
/**************************************************************************/

#pragma once

#include "core/math/plane.h"
#include "core/math/vector3.h"

#include <algorithm>
#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] AABB {
    Vector3 position;
    Vector3 size;

    constexpr AABB() = default;
    constexpr AABB(const Vector3 &pos, const Vector3 &sz) noexcept : position(pos), size(sz) {}

    [[nodiscard]] constexpr real_t get_volume() const noexcept { return size.x * size.y * size.z; }
    [[nodiscard]] constexpr bool has_volume() const noexcept { return size.x > (real_t)0 && size.y > (real_t)0 && size.z > (real_t)0; }
    [[nodiscard]] constexpr bool has_surface() const noexcept { return size.x > (real_t)0 || size.y > (real_t)0 || size.z > (real_t)0; }

    [[nodiscard]] constexpr const Vector3 &get_position() const noexcept { return position; }
    constexpr void set_position(const Vector3 &pos) noexcept { position = pos; }
    [[nodiscard]] constexpr const Vector3 &get_size() const noexcept { return size; }
    constexpr void set_size(const Vector3 &sz) noexcept { size = sz; }

    [[nodiscard]] constexpr Vector3 get_end() const noexcept { return position + size; }
    constexpr void set_end(const Vector3 &end) noexcept { size = end - position; }
    [[nodiscard]] constexpr Vector3 get_center() const noexcept { return position + size * (real_t)0.5; }

    [[nodiscard]] constexpr bool intersects(const AABB &other) const noexcept {
        if (position.x >= (other.position.x + other.size.x)) return false;
        if ((position.x + size.x) <= other.position.x) return false;
        if (position.y >= (other.position.y + other.size.y)) return false;
        if ((position.y + size.y) <= other.position.y) return false;
        if (position.z >= (other.position.z + other.size.z)) return false;
        if ((position.z + size.z) <= other.position.z) return false;
        return true;
    }

    [[nodiscard]] constexpr bool intersects_inclusive(const AABB &other) const noexcept {
        if (position.x > (other.position.x + other.size.x)) return false;
        if ((position.x + size.x) < other.position.x) return false;
        if (position.y > (other.position.y + other.size.y)) return false;
        if ((position.y + size.y) < other.position.y) return false;
        if (position.z > (other.position.z + other.size.z)) return false;
        if ((position.z + size.z) < other.position.z) return false;
        return true;
    }

    [[nodiscard]] constexpr bool encloses(const AABB &other) const noexcept {
        Vector3 src_min = position;
        Vector3 src_max = position + size;
        Vector3 dst_min = other.position;
        Vector3 dst_max = other.position + other.size;

        return (src_min.x <= dst_min.x) &&
               (src_max.x >= dst_max.x) &&
               (src_min.y <= dst_min.y) &&
               (src_max.y >= dst_max.y) &&
               (src_min.z <= dst_min.z) &&
               (src_max.z >= dst_max.z);
    }

    [[nodiscard]] AABB merge(const AABB &with) const noexcept {
        AABB copy = *this;
        copy.merge_with(with);
        return copy;
    }

    void merge_with(const AABB &other) noexcept {
        Vector3 beg_1 = position;
        Vector3 beg_2 = other.position;
        Vector3 end_1 = position + size;
        Vector3 end_2 = other.position + other.size;

        Vector3 min_v(
            std::min(beg_1.x, beg_2.x),
            std::min(beg_1.y, beg_2.y),
            std::min(beg_1.z, beg_2.z)
        );
        Vector3 max_v(
            std::max(end_1.x, end_2.x),
            std::max(end_1.y, end_2.y),
            std::max(end_1.z, end_2.z)
        );

        position = min_v;
        size = max_v - min_v;
    }

    [[nodiscard]] AABB intersection(const AABB &other) const noexcept {
        Vector3 src_min = position;
        Vector3 src_max = position + size;
        Vector3 dst_min = other.position;
        Vector3 dst_max = other.position + other.size;

        if (src_min.x > dst_max.x || src_max.x < dst_min.x) return AABB();
        if (src_min.y > dst_max.y || src_max.y < dst_min.y) return AABB();
        if (src_min.z > dst_max.z || src_max.z < dst_min.z) return AABB();

        Vector3 min_v(
            std::max(src_min.x, dst_min.x),
            std::max(src_min.y, dst_min.y),
            std::max(src_min.z, dst_min.z)
        );
        Vector3 max_v(
            std::min(src_max.x, dst_max.x),
            std::min(src_max.y, dst_max.y),
            std::min(src_max.z, dst_max.z)
        );

        return AABB(min_v, max_v - min_v);
    }

    [[nodiscard]] bool find_intersects_ray(const Vector3 &from, const Vector3 &dir, bool &inside, Vector3 *intersection_pt = nullptr, Vector3 *normal = nullptr) const noexcept {
        Vector3 end = position + size;
        real_t tmin = (real_t)-1e20;
        real_t tmax = (real_t)1e20;
        usize axis = 0;

        inside = false;

        for (usize i = 0; i < 3; i++) {
            if (dir[i] == (real_t)0.0) {
                if ((from[i] < position[i]) || (from[i] > end[i])) {
                    return false;
                }
            } else {
                real_t t1 = (position[i] - from[i]) / dir[i];
                real_t t2 = (end[i] - from[i]) / dir[i];

                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                if (t1 >= tmin) {
                    tmin = t1;
                    axis = i;
                }
                if (t2 < tmax) {
                    if (t2 < (real_t)0.0) return false;
                    tmax = t2;
                }
                if (tmin > tmax) return false;
            }
        }

        inside = tmin < (real_t)0.0;
        if (intersection_pt) {
            *intersection_pt = from + dir * tmin;
            (*intersection_pt)[axis] = (dir[axis] >= (real_t)0.0) ? position[axis] : end[axis];
        }
        if (normal) {
            *normal = Vector3();
            (*normal)[axis] = (dir[axis] >= (real_t)0.0) ? (real_t)-1.0 : (real_t)1.0;
        }
        return true;
    }

    [[nodiscard]] bool intersects_ray(const Vector3 &from, const Vector3 &dir) const noexcept {
        bool inside = false;
        return find_intersects_ray(from, dir, inside);
    }

    [[nodiscard]] bool intersects_segment(const Vector3 &from, const Vector3 &to, Vector3 *intersection_pt = nullptr, Vector3 *normal = nullptr) const noexcept {
        real_t min = (real_t)0.0, max = (real_t)1.0;
        usize axis = 0;
        real_t sgn = (real_t)0.0;

        for (usize i = 0; i < 3; i++) {
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

        Vector3 rel = to - from;
        if (normal) {
            Vector3 norm;
            norm[axis] = sgn;
            *normal = norm;
        }
        if (intersection_pt) {
            *intersection_pt = from + rel * min;
        }
        return true;
    }

    [[nodiscard]] bool intersects_plane(const Plane &plane) const noexcept {
        Vector3 points[8] = {
            Vector3(position.x, position.y, position.z),
            Vector3(position.x, position.y, position.z + size.z),
            Vector3(position.x, position.y + size.y, position.z),
            Vector3(position.x, position.y + size.y, position.z + size.z),
            Vector3(position.x + size.x, position.y, position.z),
            Vector3(position.x + size.x, position.y, position.z + size.z),
            Vector3(position.x + size.x, position.y + size.y, position.z),
            Vector3(position.x + size.x, position.y + size.y, position.z + size.z),
        };

        bool over = false;
        bool under = false;
        for (usize i = 0; i < 8; i++) {
            if (plane.distance_to(points[i]) > (real_t)0.0) {
                over = true;
            } else {
                under = true;
            }
        }
        return under && over;
    }

    [[nodiscard]] constexpr bool has_point(const Vector3 &point) const noexcept {
        if (point.x < position.x) return false;
        if (point.y < position.y) return false;
        if (point.z < position.z) return false;
        if (point.x >= (position.x + size.x)) return false;
        if (point.y >= (position.y + size.y)) return false;
        if (point.z >= (position.z + size.z)) return false;
        return true;
    }

    [[nodiscard]] Vector3 get_support(const Vector3 &dir) const noexcept {
        Vector3 half_extents = size * (real_t)0.5;
        Vector3 ofs = position + half_extents;
        return Vector3(
            (dir.x > (real_t)0.0) ? half_extents.x : -half_extents.x,
            (dir.y > (real_t)0.0) ? half_extents.y : -half_extents.y,
            (dir.z > (real_t)0.0) ? half_extents.z : -half_extents.z
        ) + ofs;
    }

    constexpr void get_edge(usize edge_idx, Vector3 &r_from, Vector3 &r_to) const noexcept {
        assert(edge_idx < 12);
        switch (edge_idx) {
            case 0:
                r_from = Vector3(position.x + size.x, position.y, position.z);
                r_to = Vector3(position.x, position.y, position.z);
                break;
            case 1:
                r_from = Vector3(position.x + size.x, position.y, position.z + size.z);
                r_to = Vector3(position.x + size.x, position.y, position.z);
                break;
            case 2:
                r_from = Vector3(position.x, position.y, position.z + size.z);
                r_to = Vector3(position.x + size.x, position.y, position.z + size.z);
                break;
            case 3:
                r_from = Vector3(position.x, position.y, position.z);
                r_to = Vector3(position.x, position.y, position.z + size.z);
                break;
            case 4:
                r_from = Vector3(position.x, position.y + size.y, position.z);
                r_to = Vector3(position.x + size.x, position.y + size.y, position.z);
                break;
            case 5:
                r_from = Vector3(position.x + size.x, position.y + size.y, position.z);
                r_to = Vector3(position.x + size.x, position.y + size.y, position.z + size.z);
                break;
            case 6:
                r_from = Vector3(position.x + size.x, position.y + size.y, position.z + size.z);
                r_to = Vector3(position.x, position.y + size.y, position.z + size.z);
                break;
            case 7:
                r_from = Vector3(position.x, position.y + size.y, position.z + size.z);
                r_to = Vector3(position.x, position.y + size.y, position.z);
                break;
            case 8:
                r_from = Vector3(position.x, position.y, position.z + size.z);
                r_to = Vector3(position.x, position.y + size.y, position.z + size.z);
                break;
            case 9:
                r_from = Vector3(position.x, position.y, position.z);
                r_to = Vector3(position.x, position.y + size.y, position.z);
                break;
            case 10:
                r_from = Vector3(position.x + size.x, position.y, position.z);
                r_to = Vector3(position.x + size.x, position.y + size.y, position.z);
                break;
            default: // 11
                r_from = Vector3(position.x + size.x, position.y, position.z + size.z);
                r_to = Vector3(position.x + size.x, position.y + size.y, position.z + size.z);
                break;
        }
    }

    constexpr void project_range_in_plane(const Plane &p_plane, real_t &r_min, real_t &r_max) const noexcept {
        Vector3 half_extents = size * (real_t)0.5;
        Vector3 center = position + half_extents;
        real_t length = p_plane.normal.abs().dot(half_extents);
        real_t distance = p_plane.distance_to(center);
        r_min = distance - length;
        r_max = distance + length;
    }

    [[nodiscard]] constexpr Vector3 get_longest_axis() const noexcept {
        Vector3 axis(1, 0, 0);
        real_t max_s = size.x;
        if (size.y > max_s) {
            axis = Vector3(0, 1, 0);
            max_s = size.y;
        }
        if (size.z > max_s) {
            axis = Vector3(0, 0, 1);
        }
        return axis;
    }

    [[nodiscard]] constexpr usize get_longest_axis_index() const noexcept {
        usize axis = 0;
        real_t max_s = size.x;
        if (size.y > max_s) {
            axis = 1;
            max_s = size.y;
        }
        if (size.z > max_s) {
            axis = 2;
        }
        return axis;
    }

    [[nodiscard]] constexpr real_t get_longest_axis_size() const noexcept {
        real_t max_s = size.x;
        if (size.y > max_s) max_s = size.y;
        if (size.z > max_s) max_s = size.z;
        return max_s;
    }

    [[nodiscard]] constexpr Vector3 get_shortest_axis() const noexcept {
        Vector3 axis(1, 0, 0);
        real_t min_s = size.x;
        if (size.y < min_s) {
            axis = Vector3(0, 1, 0);
            min_s = size.y;
        }
        if (size.z < min_s) {
            axis = Vector3(0, 0, 1);
        }
        return axis;
    }

    [[nodiscard]] constexpr usize get_shortest_axis_index() const noexcept {
        usize axis = 0;
        real_t min_s = size.x;
        if (size.y < min_s) {
            axis = 1;
            min_s = size.y;
        }
        if (size.z < min_s) {
            axis = 2;
        }
        return axis;
    }

    [[nodiscard]] constexpr real_t get_shortest_axis_size() const noexcept {
        real_t min_s = size.x;
        if (size.y < min_s) min_s = size.y;
        if (size.z < min_s) min_s = size.z;
        return min_s;
    }

    [[nodiscard]] constexpr Vector3 get_endpoint(usize idx) const noexcept {
        assert(idx < 8);
        switch (idx) {
            case 0: return Vector3(position.x, position.y, position.z);
            case 1: return Vector3(position.x, position.y, position.z + size.z);
            case 2: return Vector3(position.x, position.y + size.y, position.z);
            case 3: return Vector3(position.x, position.y + size.y, position.z + size.z);
            case 4: return Vector3(position.x + size.x, position.y, position.z);
            case 5: return Vector3(position.x + size.x, position.y, position.z + size.z);
            case 6: return Vector3(position.x + size.x, position.y + size.y, position.z);
            case 7: return Vector3(position.x + size.x, position.y + size.y, position.z + size.z);
        }
        return Vector3();
    }

    [[nodiscard]] constexpr AABB grow(real_t amount) const noexcept {
        AABB res = *this;
        res.position.x -= amount;
        res.position.y -= amount;
        res.position.z -= amount;
        res.size.x += (real_t)2.0 * amount;
        res.size.y += (real_t)2.0 * amount;
        res.size.z += (real_t)2.0 * amount;
        return res;
    }

    [[nodiscard]] AABB expand(const Vector3 &vector) const noexcept {
        AABB res = *this;
        res.expand_to(vector);
        return res;
    }

    void expand_to(const Vector3 &vector) noexcept {
        Vector3 beg = position;
        Vector3 end = position + size;

        if (vector.x < beg.x) beg.x = vector.x;
        if (vector.y < beg.y) beg.y = vector.y;
        if (vector.z < beg.z) beg.z = vector.z;

        if (vector.x > end.x) end.x = vector.x;
        if (vector.y > end.y) end.y = vector.y;
        if (vector.z > end.z) end.z = vector.z;

        position = beg;
        size = end - beg;
    }

    [[nodiscard]] constexpr AABB abs() const noexcept {
        return AABB(Vector3(
            position.x + (size.x < (real_t)0 ? size.x : (real_t)0),
            position.y + (size.y < (real_t)0 ? size.y : (real_t)0),
            position.z + (size.z < (real_t)0 ? size.z : (real_t)0)),
            Vector3(Math::abs(size.x),
            Math::abs(size.y),
            Math::abs(size.z))
        );
    }

    [[nodiscard]] constexpr bool is_equal_approx(const AABB &other) const noexcept {
        return position.is_equal_approx(other.position) && size.is_equal_approx(other.size);
    }

    [[nodiscard]] constexpr bool is_same(const AABB &other) const noexcept {
        return position.is_same(other.position) && size.is_same(other.size);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return position.is_finite() && size.is_finite();
    }

    constexpr bool operator==(const AABB &rhs) const noexcept {
        return position == rhs.position && size == rhs.size;
    }
};

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::AABB> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::AABB &a, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[P: ({}, {}, {}), S: ({}, {}, {})]",
            a.position.x, a.position.y, a.position.z,
            a.size.x, a.size.y, a.size.z
        );
    }
};
