/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  face3.h                                                               */
/**************************************************************************/

#pragma once

#include "core/primitives.h"
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector3.h"
#include "core/math/aabb.h"
#include "core/math/plane.h"
#include "core/math/transform_3d.h"

#include <cassert>
#include <format>
#include <algorithm>

namespace Beyota::Math {

struct [[nodiscard]] Face3 {
    enum class Side : u8 {
        Over = 0,
        Under = 1,
        Spanning = 2,
        Coplanar = 3,
        SIDE_OVER = 0,
        SIDE_UNDER = 1,
        SIDE_SPANNING = 2,
        SIDE_COPLANAR = 3
    };

    Vector3 vertex[3];

    constexpr Face3() = default;

    constexpr Face3(const Vector3 &p_v1, const Vector3 &p_v2, const Vector3 &p_v3) noexcept
        : vertex{ p_v1, p_v2, p_v3 } {}

    [[nodiscard]] bool is_degenerate() const noexcept {
        Vector3 normal = (vertex[0] - vertex[1]).cross(vertex[0] - vertex[2]);
        return (normal.length_squared() < CMP_EPSILON2<real_t>);
    }

    [[nodiscard]] real_t get_area() const noexcept {
        return (vertex[0] - vertex[1]).cross(vertex[0] - vertex[2]).length() * (real_t)0.5;
    }

    [[nodiscard]] Plane get_plane(ClockDirection p_dir = ClockDirection::Clockwise) const noexcept {
        return Plane(vertex[0], vertex[1], vertex[2], p_dir);
    }

    [[nodiscard]] constexpr Vector3 get_point_inside(real_t a, real_t b) const noexcept {
        if (a > b) std::swap(a, b);
        return vertex[0] * a + vertex[1] * (b - a) + vertex[2] * ((real_t)1.0 - b);
    }

    [[nodiscard]] Vector3 get_random_point_inside() const noexcept {
        static thread_local u64 rng_state = 0x853c49e6748fea9bULL;
        auto next_float = []() -> real_t {
            rng_state ^= rng_state >> 12;
            rng_state ^= rng_state << 25;
            rng_state ^= rng_state >> 27;
            return (real_t)((rng_state * 0x2545F4914F6CDD1DULL) >> 40) * (real_t)(1.0 / 16777216.0);
        };
        real_t a = next_float();
        real_t b = next_float();
        return get_point_inside(a, b);
    }

    [[nodiscard]] AABB get_aabb() const noexcept {
        AABB aabb(vertex[0], Vector3());
        aabb.expand_to(vertex[1]);
        aabb.expand_to(vertex[2]);
        return aabb;
    }

    [[nodiscard]] Vector3 get_closest_point_to(const Vector3 &p_point) const noexcept {
        Vector3 edge0 = vertex[1] - vertex[0];
        Vector3 edge1 = vertex[2] - vertex[0];
        Vector3 v0 = vertex[0] - p_point;

        real_t a = edge0.dot(edge0);
        real_t b = edge0.dot(edge1);
        real_t c = edge1.dot(edge1);
        real_t d = edge0.dot(v0);
        real_t e = edge1.dot(v0);

        real_t det = a * c - b * b;
        real_t s = b * e - c * d;
        real_t t = b * d - a * e;

        if (s + t < det) {
            if (s < (real_t)0.0) {
                if (t < (real_t)0.0) {
                    if (d < (real_t)0.0) {
                        s = std::clamp(-d / a, (real_t)0.0, (real_t)1.0);
                        t = (real_t)0.0;
                    } else {
                        s = (real_t)0.0;
                        t = std::clamp(-e / c, (real_t)0.0, (real_t)1.0);
                    }
                } else {
                    s = (real_t)0.0;
                    t = std::clamp(-e / c, (real_t)0.0, (real_t)1.0);
                }
            } else if (t < (real_t)0.0) {
                s = std::clamp(-d / a, (real_t)0.0, (real_t)1.0);
                t = (real_t)0.0;
            } else {
                real_t invDet = (real_t)1.0 / det;
                s *= invDet;
                t *= invDet;
            }
        } else {
            if (s < (real_t)0.0) {
                real_t tmp0 = b + d;
                real_t tmp1 = c + e;
                if (tmp1 > tmp0) {
                    real_t numer = tmp1 - tmp0;
                    real_t denom = a - (real_t)2.0 * b + c;
                    s = std::clamp(numer / denom, (real_t)0.0, (real_t)1.0);
                    t = (real_t)1.0 - s;
                } else {
                    t = std::clamp(-e / c, (real_t)0.0, (real_t)1.0);
                    s = (real_t)0.0;
                }
            } else if (t < (real_t)0.0) {
                if (a + d > b + e) {
                    real_t numer = c + e - b - d;
                    real_t denom = a - (real_t)2.0 * b + c;
                    s = std::clamp(numer / denom, (real_t)0.0, (real_t)1.0);
                    t = (real_t)1.0 - s;
                } else {
                    s = std::clamp(-d / a, (real_t)0.0, (real_t)1.0);
                    t = (real_t)0.0;
                }
            } else {
                real_t numer = c + e - b - d;
                real_t denom = a - (real_t)2.0 * b + c;
                s = std::clamp(numer / denom, (real_t)0.0, (real_t)1.0);
                t = (real_t)1.0 - s;
            }
        }

        return vertex[0] + s * edge0 + t * edge1;
    }

    [[nodiscard]] bool intersects_ray(const Vector3 &p_from, const Vector3 &p_dir, Vector3 *p_intersection = nullptr) const noexcept {
        Vector3 e1 = vertex[1] - vertex[0];
        Vector3 e2 = vertex[2] - vertex[0];
        Vector3 h = p_dir.cross(e2);
        real_t a = e1.dot(h);
        if (Math::is_zero_approx(a)) {
            return false;
        }

        real_t f = (real_t)1.0 / a;
        Vector3 s = p_from - vertex[0];
        real_t u = f * s.dot(h);
        if (u < (real_t)0.0 || u > (real_t)1.0) {
            return false;
        }

        Vector3 q = s.cross(e1);
        real_t v = f * p_dir.dot(q);
        if (v < (real_t)0.0 || (u + v) > (real_t)1.0) {
            return false;
        }

        real_t t = f * e2.dot(q);
        if (t > (real_t)0.00001) {
            if (p_intersection) {
                *p_intersection = p_from + p_dir * t;
            }
            return true;
        }
        return false;
    }

    [[nodiscard]] bool intersects_segment(const Vector3 &p_from, const Vector3 &p_to, Vector3 *p_intersection = nullptr) const noexcept {
        Vector3 rel = p_to - p_from;
        Vector3 e1 = vertex[1] - vertex[0];
        Vector3 e2 = vertex[2] - vertex[0];
        Vector3 h = rel.cross(e2);
        real_t a = e1.dot(h);
        if (Math::is_zero_approx(a)) {
            return false;
        }

        real_t f = (real_t)1.0 / a;
        Vector3 s = p_from - vertex[0];
        real_t u = f * s.dot(h);
        if (u < (real_t)0.0 || u > (real_t)1.0) {
            return false;
        }

        Vector3 q = s.cross(e1);
        real_t v = f * rel.dot(q);
        if (v < (real_t)0.0 || (u + v) > (real_t)1.0) {
            return false;
        }

        real_t t = f * e2.dot(q);
        if (t > CMP_EPSILON<real_t> && t <= (real_t)1.0) {
            if (p_intersection) {
                *p_intersection = p_from + rel * t;
            }
            return true;
        }
        return false;
    }

    void project_range(const Vector3 &p_normal, const Transform3D &p_transform, real_t &r_min, real_t &r_max) const noexcept {
        for (usize i = 0; i < 3; ++i) {
            Vector3 v = p_transform.xform(vertex[i]);
            real_t d = p_normal.dot(v);
            if (i == 0 || d > r_max) r_max = d;
            if (i == 0 || d < r_min) r_min = d;
        }
    }

    void get_support(const Vector3 &p_normal, const Transform3D &p_transform, Vector3 *p_vertices, int *p_count, int p_max) const noexcept {
        constexpr double face_support_threshold = 0.98;
        constexpr double edge_support_threshold = 0.05;

        if (p_max <= 0) return;

        Vector3 n = p_transform.basis.xform_inv(p_normal);

        if (get_plane().normal.dot(n) > face_support_threshold) {
            *p_count = std::min(3, p_max);
            for (int i = 0; i < *p_count; ++i) {
                p_vertices[i] = p_transform.xform(vertex[i]);
            }
            return;
        }

        usize vert_support_idx = 0;
        real_t support_max = (real_t)0.0;

        for (usize i = 0; i < 3; ++i) {
            real_t d = n.dot(vertex[i]);
            if (i == 0 || d > support_max) {
                support_max = d;
                vert_support_idx = i;
            }
        }

        for (usize i = 0; i < 3; ++i) {
            if (i != vert_support_idx && (i + 1) % 3 != vert_support_idx) {
                continue;
            }

            real_t dot = (vertex[i] - vertex[(i + 1) % 3]).normalized().dot(n);
            dot = Math::abs(dot);
            if (dot < (real_t)edge_support_threshold) {
                *p_count = std::min(2, p_max);
                for (int j = 0; j < *p_count; ++j) {
                    p_vertices[j] = p_transform.xform(vertex[(j + i) % 3]);
                }
                return;
            }
        }

        *p_count = 1;
        p_vertices[0] = p_transform.xform(vertex[vert_support_idx]);
    }

    int split_by_plane(const Plane &p_plane, Face3 *p_res, bool *p_is_point_over) const noexcept {
        if (is_degenerate()) return 0;

        Vector3 above[4];
        int above_count = 0;
        Vector3 below[4];
        int below_count = 0;

        for (usize i = 0; i < 3; ++i) {
            if (p_plane.has_point(vertex[i], CMP_EPSILON<real_t>)) {
                if (above_count >= 4 || below_count >= 4) return 0;
                above[above_count++] = vertex[i];
                below[below_count++] = vertex[i];
            } else {
                if (p_plane.is_point_over(vertex[i])) {
                    if (above_count >= 4) return 0;
                    above[above_count++] = vertex[i];
                } else {
                    if (below_count >= 4) return 0;
                    below[below_count++] = vertex[i];
                }

                Vector3 inters;
                if (!p_plane.intersects_segment(vertex[i], vertex[(i + 1) % 3], &inters)) {
                    continue;
                }

                if (above_count >= 4 || below_count >= 4) return 0;
                above[above_count++] = inters;
                below[below_count++] = inters;
            }
        }

        if (above_count >= 4 && below_count >= 4) return 0;

        int polygons_created = 0;

        if (above_count >= 3) {
            p_res[polygons_created] = Face3(above[0], above[1], above[2]);
            p_is_point_over[polygons_created] = true;
            polygons_created++;

            if (above_count == 4) {
                p_res[polygons_created] = Face3(above[2], above[3], above[0]);
                p_is_point_over[polygons_created] = true;
                polygons_created++;
            }
        }

        if (below_count >= 3) {
            p_res[polygons_created] = Face3(below[0], below[1], below[2]);
            p_is_point_over[polygons_created] = false;
            polygons_created++;

            if (below_count == 4) {
                p_res[polygons_created] = Face3(below[2], below[3], below[0]);
                p_is_point_over[polygons_created] = false;
                polygons_created++;
            }
        }

        return polygons_created;
    }

    [[nodiscard]] bool intersects_aabb(const AABB &p_aabb) const noexcept {
        if (!p_aabb.intersects_plane(get_plane())) {
            return false;
        }

        real_t aabb_min_x = p_aabb.position.x;
        real_t aabb_max_x = p_aabb.position.x + p_aabb.size.x;
        real_t tri_min_x = std::min(vertex[0].x, std::min(vertex[1].x, vertex[2].x));
        real_t tri_max_x = std::max(vertex[0].x, std::max(vertex[1].x, vertex[2].x));
        if (tri_max_x < aabb_min_x || aabb_max_x < tri_min_x) return false;

        real_t aabb_min_y = p_aabb.position.y;
        real_t aabb_max_y = p_aabb.position.y + p_aabb.size.y;
        real_t tri_min_y = std::min(vertex[0].y, std::min(vertex[1].y, vertex[2].y));
        real_t tri_max_y = std::max(vertex[0].y, std::max(vertex[1].y, vertex[2].y));
        if (tri_max_y < aabb_min_y || aabb_max_y < tri_min_y) return false;

        real_t aabb_min_z = p_aabb.position.z;
        real_t aabb_max_z = p_aabb.position.z + p_aabb.size.z;
        real_t tri_min_z = std::min(vertex[0].z, std::min(vertex[1].z, vertex[2].z));
        real_t tri_max_z = std::max(vertex[0].z, std::max(vertex[1].z, vertex[2].z));
        if (tri_max_z < aabb_min_z || aabb_max_z < tri_min_z) return false;

        const Vector3 edge_norms[3] = {
            vertex[0] - vertex[1],
            vertex[1] - vertex[2],
            vertex[2] - vertex[0],
        };

        for (usize i = 0; i < 12; ++i) {
            Vector3 from, to;
            p_aabb.get_edge(i, from, to);
            Vector3 e1 = from - to;
            for (usize j = 0; j < 3; ++j) {
                Vector3 e2 = edge_norms[j];
                Vector3 axis = e1.cross(e2);
                if (axis.length_squared() < (real_t)0.0001) {
                    continue;
                }
                axis.normalize();

                real_t minA, maxA, minB, maxB;
                p_aabb.project_range_in_plane(Plane(axis, (real_t)0.0), minA, maxA);
                project_range(axis, Transform3D(), minB, maxB);

                if (maxA < minB || maxB < minA) {
                    return false;
                }
            }
        }
        return true;
    }

    constexpr bool operator==(const Face3 &other) const noexcept {
        return vertex[0] == other.vertex[0] && vertex[1] == other.vertex[1] && vertex[2] == other.vertex[2];
    }
};

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Face3> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Face3 &f, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "([{}, {}, {}], [{}, {}, {}], [{}, {}, {}])",
            f.vertex[0].x, f.vertex[0].y, f.vertex[0].z,
            f.vertex[1].x, f.vertex[1].y, f.vertex[1].z,
            f.vertex[2].x, f.vertex[2].y, f.vertex[2].z
        );
    }
};
