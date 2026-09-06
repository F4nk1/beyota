/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  projection.h                                                          */
/**************************************************************************/

#pragma once

#include "core/primitives.h"
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
#include "core/math/rect2.h"
#include "core/math/aabb.h"
#include "core/math/plane.h"
#include "core/math/transform_3d.h"

#include <cassert>
#include <format>
#include <algorithm>
#include <array>

namespace Beyota::Math {

struct [[nodiscard]] Projection {
    enum class Planes : u32 {
        Near = 0,
        Far,
        Left,
        Top,
        Right,
        Bottom,
        PLANE_NEAR = 0,
        PLANE_FAR = 1,
        PLANE_LEFT = 2,
        PLANE_TOP = 3,
        PLANE_RIGHT = 4,
        PLANE_BOTTOM = 5
    };

    Vector4 columns[4] = {
        { (real_t)1.0, (real_t)0.0, (real_t)0.0, (real_t)0.0 },
        { (real_t)0.0, (real_t)1.0, (real_t)0.0, (real_t)0.0 },
        { (real_t)0.0, (real_t)0.0, (real_t)1.0, (real_t)0.0 },
        { (real_t)0.0, (real_t)0.0, (real_t)0.0, (real_t)1.0 }
    };

    constexpr Projection() = default;

    constexpr Projection(const Vector4 &p_x, const Vector4 &p_y, const Vector4 &p_z, const Vector4 &p_w) noexcept
        : columns{ p_x, p_y, p_z, p_w } {}

    constexpr Projection(real_t p_xx, real_t p_xy, real_t p_xz, real_t p_xw,
                         real_t p_yx, real_t p_yy, real_t p_yz, real_t p_yw,
                         real_t p_zx, real_t p_zy, real_t p_zz, real_t p_zw,
                         real_t p_wx, real_t p_wy, real_t p_wz, real_t p_ww) noexcept
        : columns{
            { p_xx, p_xy, p_xz, p_xw },
            { p_yx, p_yy, p_yz, p_yw },
            { p_zx, p_zy, p_zz, p_zw },
            { p_wx, p_wy, p_wz, p_ww }
        } {}

    explicit constexpr Projection(const Transform3D &p_transform) noexcept {
        columns[0] = Vector4(p_transform.basis.rows[0][0], p_transform.basis.rows[1][0], p_transform.basis.rows[2][0], (real_t)0.0);
        columns[1] = Vector4(p_transform.basis.rows[0][1], p_transform.basis.rows[1][1], p_transform.basis.rows[2][1], (real_t)0.0);
        columns[2] = Vector4(p_transform.basis.rows[0][2], p_transform.basis.rows[1][2], p_transform.basis.rows[2][2], (real_t)0.0);
        columns[3] = Vector4(p_transform.origin.x, p_transform.origin.y, p_transform.origin.z, (real_t)1.0);
    }

    [[nodiscard]] constexpr const Vector4 &operator[](usize p_axis) const noexcept {
        assert(p_axis < 4);
        return columns[p_axis];
    }

    [[nodiscard]] constexpr Vector4 &operator[](usize p_axis) noexcept {
        assert(p_axis < 4);
        return columns[p_axis];
    }

    [[nodiscard]] constexpr real_t determinant() const noexcept {
        return columns[0][3] * columns[1][2] * columns[2][1] * columns[3][0] - columns[0][2] * columns[1][3] * columns[2][1] * columns[3][0] -
               columns[0][3] * columns[1][1] * columns[2][2] * columns[3][0] + columns[0][1] * columns[1][3] * columns[2][2] * columns[3][0] +
               columns[0][2] * columns[1][1] * columns[2][3] * columns[3][0] - columns[0][1] * columns[1][2] * columns[2][3] * columns[3][0] -
               columns[0][3] * columns[1][2] * columns[2][0] * columns[3][1] + columns[0][2] * columns[1][3] * columns[2][0] * columns[3][1] +
               columns[0][3] * columns[1][0] * columns[2][2] * columns[3][1] - columns[0][0] * columns[1][3] * columns[2][2] * columns[3][1] -
               columns[0][2] * columns[1][0] * columns[2][3] * columns[3][1] + columns[0][0] * columns[1][2] * columns[2][3] * columns[3][1] +
               columns[0][3] * columns[1][1] * columns[2][0] * columns[3][2] - columns[0][1] * columns[1][3] * columns[2][0] * columns[3][2] -
               columns[0][3] * columns[1][0] * columns[2][1] * columns[3][2] + columns[0][0] * columns[1][3] * columns[2][1] * columns[3][2] +
               columns[0][1] * columns[1][0] * columns[2][3] * columns[3][2] - columns[0][0] * columns[1][1] * columns[2][3] * columns[3][2] -
               columns[0][2] * columns[1][1] * columns[2][0] * columns[3][3] + columns[0][1] * columns[1][2] * columns[2][0] * columns[3][3] +
               columns[0][2] * columns[1][0] * columns[2][1] * columns[3][3] - columns[0][0] * columns[1][2] * columns[2][1] * columns[3][3] -
               columns[0][1] * columns[1][0] * columns[2][2] * columns[3][3] + columns[0][0] * columns[1][1] * columns[2][2] * columns[3][3];
    }

    constexpr void set_identity() noexcept {
        for (usize i = 0; i < 4; ++i) {
            for (usize j = 0; j < 4; ++j) {
                columns[i][j] = (i == j) ? (real_t)1.0 : (real_t)0.0;
            }
        }
    }

    constexpr void set_zero() noexcept {
        for (usize i = 0; i < 4; ++i) {
            for (usize j = 0; j < 4; ++j) {
                columns[i][j] = (real_t)0.0;
            }
        }
    }

    [[nodiscard]] constexpr Plane xform4(const Plane &p_vec4) const noexcept {
        Plane ret;
        ret.normal.x = columns[0][0] * p_vec4.normal.x + columns[1][0] * p_vec4.normal.y + columns[2][0] * p_vec4.normal.z + columns[3][0] * p_vec4.d;
        ret.normal.y = columns[0][1] * p_vec4.normal.x + columns[1][1] * p_vec4.normal.y + columns[2][1] * p_vec4.normal.z + columns[3][1] * p_vec4.d;
        ret.normal.z = columns[0][2] * p_vec4.normal.x + columns[1][2] * p_vec4.normal.y + columns[2][2] * p_vec4.normal.z + columns[3][2] * p_vec4.d;
        ret.d = columns[0][3] * p_vec4.normal.x + columns[1][3] * p_vec4.normal.y + columns[2][3] * p_vec4.normal.z + columns[3][3] * p_vec4.d;
        return ret;
    }

    [[nodiscard]] constexpr Vector3 xform(const Vector3 &p_vec3) const noexcept {
        Vector3 ret(
            columns[0][0] * p_vec3.x + columns[1][0] * p_vec3.y + columns[2][0] * p_vec3.z + columns[3][0],
            columns[0][1] * p_vec3.x + columns[1][1] * p_vec3.y + columns[2][1] * p_vec3.z + columns[3][1],
            columns[0][2] * p_vec3.x + columns[1][2] * p_vec3.y + columns[2][2] * p_vec3.z + columns[3][2]
        );
        real_t w = columns[0][3] * p_vec3.x + columns[1][3] * p_vec3.y + columns[2][3] * p_vec3.z + columns[3][3];
        assert(w != (real_t)0.0);
        return ret / w;
    }

    [[nodiscard]] constexpr Vector4 xform(const Vector4 &p_vec4) const noexcept {
        return Vector4(
            columns[0][0] * p_vec4.x + columns[1][0] * p_vec4.y + columns[2][0] * p_vec4.z + columns[3][0] * p_vec4.w,
            columns[0][1] * p_vec4.x + columns[1][1] * p_vec4.y + columns[2][1] * p_vec4.z + columns[3][1] * p_vec4.w,
            columns[0][2] * p_vec4.x + columns[1][2] * p_vec4.y + columns[2][2] * p_vec4.z + columns[3][2] * p_vec4.w,
            columns[0][3] * p_vec4.x + columns[1][3] * p_vec4.y + columns[2][3] * p_vec4.z + columns[3][3] * p_vec4.w
        );
    }

    [[nodiscard]] constexpr Vector4 xform_inv(const Vector4 &p_vec4) const noexcept {
        return Vector4(
            columns[0][0] * p_vec4.x + columns[0][1] * p_vec4.y + columns[0][2] * p_vec4.z + columns[0][3] * p_vec4.w,
            columns[1][0] * p_vec4.x + columns[1][1] * p_vec4.y + columns[1][2] * p_vec4.z + columns[1][3] * p_vec4.w,
            columns[2][0] * p_vec4.x + columns[2][1] * p_vec4.y + columns[2][2] * p_vec4.z + columns[2][3] * p_vec4.w,
            columns[3][0] * p_vec4.x + columns[3][1] * p_vec4.y + columns[3][2] * p_vec4.z + columns[3][3] * p_vec4.w
        );
    }

    [[nodiscard]] static real_t get_fovy(real_t p_fovx, real_t p_aspect) noexcept {
        return Math::rad_to_deg(Math::atan(p_aspect * Math::tan(Math::deg_to_rad(p_fovx) * (real_t)0.5)) * (real_t)2.0);
    }

    [[nodiscard]] constexpr real_t get_z_far() const noexcept {
        return (columns[3][3] - columns[3][2]) / (columns[2][3] - columns[2][2]);
    }

    [[nodiscard]] constexpr real_t get_z_near() const noexcept {
        return (columns[3][3] + columns[3][2]) / (columns[2][3] + columns[2][2]);
    }

    [[nodiscard]] constexpr real_t get_aspect() const noexcept {
        return columns[1][1] / columns[0][0];
    }

    [[nodiscard]] real_t get_fov() const noexcept {
        if (columns[2][0] == (real_t)0.0) {
            return Math::rad_to_deg((real_t)2.0 * Math::atan2((real_t)1.0, columns[0][0]));
        } else {
            real_t right = Math::atan2(columns[2][0] + (real_t)1.0, columns[0][0]);
            real_t left = Math::atan2(columns[2][0] - (real_t)1.0, columns[0][0]);
            return Math::rad_to_deg(right - left);
        }
    }

    [[nodiscard]] constexpr bool is_orthogonal() const noexcept {
        return columns[2][3] == (real_t)0.0;
    }

    [[nodiscard]] constexpr Vector2 get_viewport_half_extents() const noexcept {
        real_t w = -get_z_near() * columns[2][3] + columns[3][3];
        return Vector2(w / columns[0][0], w / columns[1][1]);
    }

    [[nodiscard]] constexpr Vector2 get_far_plane_half_extents() const noexcept {
        real_t w = -get_z_far() * columns[2][3] + columns[3][3];
        return Vector2(w / columns[0][0], w / columns[1][1]);
    }

    void adjust_perspective_znear(real_t p_new_znear) noexcept {
        real_t zfar = get_z_far();
        real_t znear = p_new_znear;
        real_t deltaZ = zfar - znear;
        assert(deltaZ != (real_t)0.0);
        columns[2][2] = -(zfar + znear) / deltaZ;
        columns[3][2] = (real_t)-2.0 * znear * zfar / deltaZ;
    }

    [[nodiscard]] Projection perspective_znear_adjusted(real_t p_new_znear) const noexcept {
        Projection proj = *this;
        proj.adjust_perspective_znear(p_new_znear);
        return proj;
    }

    void set_depth_correction(bool p_flip_y = true, bool p_reverse_z = true, bool p_remap_z = true) noexcept {
        columns[0] = Vector4((real_t)1.0, (real_t)0.0, (real_t)0.0, (real_t)0.0);
        columns[1] = Vector4((real_t)0.0, p_flip_y ? (real_t)-1.0 : (real_t)1.0, (real_t)0.0, (real_t)0.0);
        columns[2] = Vector4((real_t)0.0, (real_t)0.0, p_remap_z ? (p_reverse_z ? (real_t)-0.5 : (real_t)0.5) : (p_reverse_z ? (real_t)-1.0 : (real_t)1.0), (real_t)0.0);
        columns[3] = Vector4((real_t)0.0, (real_t)0.0, p_remap_z ? (real_t)0.5 : (real_t)0.0, (real_t)1.0);
    }

    void set_light_bias() noexcept {
        columns[0] = Vector4((real_t)0.5, (real_t)0.0, (real_t)0.0, (real_t)0.0);
        columns[1] = Vector4((real_t)0.0, (real_t)0.5, (real_t)0.0, (real_t)0.0);
        columns[2] = Vector4((real_t)0.0, (real_t)0.0, (real_t)0.5, (real_t)0.0);
        columns[3] = Vector4((real_t)0.5, (real_t)0.5, (real_t)0.5, (real_t)1.0);
    }

    void set_light_atlas_rect(const Rect2 &p_rect) noexcept {
        columns[0] = Vector4(p_rect.size.width, (real_t)0.0, (real_t)0.0, (real_t)0.0);
        columns[1] = Vector4((real_t)0.0, p_rect.size.height, (real_t)0.0, (real_t)0.0);
        columns[2] = Vector4((real_t)0.0, (real_t)0.0, (real_t)1.0, (real_t)0.0);
        columns[3] = Vector4(p_rect.position.x, p_rect.position.y, (real_t)0.0, (real_t)1.0);
    }

    void set_perspective(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov = false) noexcept {
        if (p_flip_fov) {
            p_fovy_degrees = get_fovy(p_fovy_degrees, (real_t)1.0 / p_aspect);
        }

        real_t radians = Math::deg_to_rad(p_fovy_degrees / (real_t)2.0);
        real_t deltaZ = p_z_far - p_z_near;
        real_t sine = Math::sin(radians);

        if ((deltaZ == (real_t)0.0) || (sine == (real_t)0.0) || (p_aspect == (real_t)0.0)) {
            return;
        }
        real_t cotangent = Math::cos(radians) / sine;

        set_identity();
        columns[0][0] = cotangent / p_aspect;
        columns[1][1] = cotangent;
        columns[2][2] = -(p_z_far + p_z_near) / deltaZ;
        columns[2][3] = (real_t)-1.0;
        columns[3][2] = (real_t)-2.0 * p_z_near * p_z_far / deltaZ;
        columns[3][3] = (real_t)0.0;
    }

    void set_orthogonal(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_znear, real_t p_zfar) noexcept {
        set_identity();
        assert(p_right != p_left);
        assert(p_top != p_bottom);
        assert(p_zfar != p_znear);

        columns[0][0] = (real_t)2.0 / (p_right - p_left);
        columns[3][0] = -((p_right + p_left) / (p_right - p_left));
        columns[1][1] = (real_t)2.0 / (p_top - p_bottom);
        columns[3][1] = -((p_top + p_bottom) / (p_top - p_bottom));
        columns[2][2] = (real_t)-2.0 / (p_zfar - p_znear);
        columns[3][2] = -((p_zfar + p_znear) / (p_zfar - p_znear));
        columns[3][3] = (real_t)1.0;
    }

    void set_orthogonal(real_t p_size, real_t p_aspect, real_t p_znear, real_t p_zfar, bool p_flip_fov = false) noexcept {
        if (!p_flip_fov) {
            p_size *= p_aspect;
        }
        set_orthogonal(-p_size / (real_t)2.0, +p_size / (real_t)2.0, -p_size / p_aspect / (real_t)2.0, +p_size / p_aspect / (real_t)2.0, p_znear, p_zfar);
    }

    void set_frustum(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_near, real_t p_far) noexcept {
        assert(p_right > p_left);
        assert(p_top > p_bottom);
        assert(p_far > p_near);

        real_t x = (real_t)2.0 * p_near / (p_right - p_left);
        real_t y = (real_t)2.0 * p_near / (p_top - p_bottom);
        real_t a = (p_right + p_left) / (p_right - p_left);
        real_t b = (p_top + p_bottom) / (p_top - p_bottom);
        real_t c = -(p_far + p_near) / (p_far - p_near);
        real_t d = (real_t)-2.0 * p_far * p_near / (p_far - p_near);

        columns[0] = Vector4(x, (real_t)0.0, (real_t)0.0, (real_t)0.0);
        columns[1] = Vector4((real_t)0.0, y, (real_t)0.0, (real_t)0.0);
        columns[2] = Vector4(a, b, c, (real_t)-1.0);
        columns[3] = Vector4((real_t)0.0, (real_t)0.0, d, (real_t)0.0);
    }

    void set_frustum(real_t p_size, real_t p_aspect, Vector2 p_offset, real_t p_near, real_t p_far, bool p_flip_fov = false) noexcept {
        if (!p_flip_fov) {
            p_size *= p_aspect;
        }
        set_frustum(-p_size / (real_t)2.0 + p_offset.x, +p_size / (real_t)2.0 + p_offset.x, -p_size / p_aspect / (real_t)2.0 + p_offset.y, +p_size / p_aspect / (real_t)2.0 + p_offset.y, p_near, p_far);
    }

    void set_perspective(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov, int p_eye, real_t p_intraocular_dist, real_t p_convergence_dist) noexcept {
        if (p_flip_fov) {
            p_fovy_degrees = get_fovy(p_fovy_degrees, (real_t)1.0 / p_aspect);
        }

        real_t ymax = p_z_near * Math::tan(Math::deg_to_rad(p_fovy_degrees / (real_t)2.0));
        real_t xmax = ymax * p_aspect;
        assert(p_convergence_dist != (real_t)0.0);
        real_t frustumshift = (p_intraocular_dist / (real_t)2.0) * p_z_near / p_convergence_dist;

        real_t left, right, modeltranslation;
        switch (p_eye) {
            case 1: { // left eye
                left = -xmax + frustumshift;
                right = xmax + frustumshift;
                modeltranslation = p_intraocular_dist / (real_t)2.0;
            } break;
            case 2: { // right eye
                left = -xmax - frustumshift;
                right = xmax - frustumshift;
                modeltranslation = -p_intraocular_dist / (real_t)2.0;
            } break;
            default: { // mono
                left = -xmax;
                right = xmax;
                modeltranslation = (real_t)0.0;
            } break;
        }

        set_frustum(left, right, -ymax, ymax, p_z_near, p_z_far);

        Projection cm;
        cm.set_identity();
        cm.columns[3][0] = modeltranslation;
        *this = *this * cm;
    }

    void set_for_hmd(int p_eye, real_t p_aspect, real_t p_intraocular_dist, real_t p_display_width, real_t p_display_to_lens, real_t p_oversample, real_t p_z_near, real_t p_z_far) noexcept {
        assert(p_display_to_lens != (real_t)0.0);
        assert(p_aspect != (real_t)0.0);
        real_t f1 = (p_intraocular_dist * (real_t)0.5) / p_display_to_lens;
        real_t f2 = ((p_display_width - p_intraocular_dist) * (real_t)0.5) / p_display_to_lens;
        real_t f3 = (p_display_width / (real_t)4.0) / p_display_to_lens;

        real_t add = ((f1 + f2) * (p_oversample - (real_t)1.0)) / (real_t)2.0;
        f1 += add;
        f2 += add;
        f3 *= p_oversample;
        f3 /= p_aspect;

        switch (p_eye) {
            case 1: {
                set_frustum(-f2 * p_z_near, f1 * p_z_near, -f3 * p_z_near, f3 * p_z_near, p_z_near, p_z_far);
            } break;
            case 2: {
                set_frustum(-f1 * p_z_near, f2 * p_z_near, -f3 * p_z_near, f3 * p_z_near, p_z_near, p_z_far);
            } break;
            default: break;
        }
    }

    [[nodiscard]] static Projection create_depth_correction(bool p_flip_y) noexcept {
        Projection proj;
        proj.set_depth_correction(p_flip_y);
        return proj;
    }

    [[nodiscard]] static Projection create_light_atlas_rect(const Rect2 &p_rect) noexcept {
        Projection proj;
        proj.set_light_atlas_rect(p_rect);
        return proj;
    }

    [[nodiscard]] static Projection create_perspective(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov = false) noexcept {
        Projection proj;
        proj.set_perspective(p_fovy_degrees, p_aspect, p_z_near, p_z_far, p_flip_fov);
        return proj;
    }

    [[nodiscard]] static Projection create_perspective_hmd(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov, int p_eye, real_t p_intraocular_dist, real_t p_convergence_dist) noexcept {
        Projection proj;
        proj.set_perspective(p_fovy_degrees, p_aspect, p_z_near, p_z_far, p_flip_fov, p_eye, p_intraocular_dist, p_convergence_dist);
        return proj;
    }

    [[nodiscard]] static Projection create_for_hmd(int p_eye, real_t p_aspect, real_t p_intraocular_dist, real_t p_display_width, real_t p_display_to_lens, real_t p_oversample, real_t p_z_near, real_t p_z_far) noexcept {
        Projection proj;
        proj.set_for_hmd(p_eye, p_aspect, p_intraocular_dist, p_display_width, p_display_to_lens, p_oversample, p_z_near, p_z_far);
        return proj;
    }

    [[nodiscard]] static Projection create_orthogonal(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_znear, real_t p_zfar) noexcept {
        Projection proj;
        proj.set_orthogonal(p_left, p_right, p_bottom, p_top, p_znear, p_zfar);
        return proj;
    }

    [[nodiscard]] static Projection create_orthogonal_aspect(real_t p_size, real_t p_aspect, real_t p_znear, real_t p_zfar, bool p_flip_fov = false) noexcept {
        Projection proj;
        proj.set_orthogonal(p_size, p_aspect, p_znear, p_zfar, p_flip_fov);
        return proj;
    }

    [[nodiscard]] static Projection create_frustum(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_near, real_t p_far) noexcept {
        Projection proj;
        proj.set_frustum(p_left, p_right, p_bottom, p_top, p_near, p_far);
        return proj;
    }

    [[nodiscard]] static Projection create_frustum_aspect(real_t p_size, real_t p_aspect, Vector2 p_offset, real_t p_near, real_t p_far, bool p_flip_fov = false) noexcept {
        Projection proj;
        proj.set_frustum(p_size, p_aspect, p_offset, p_near, p_far, p_flip_fov);
        return proj;
    }

    [[nodiscard]] static Projection create_fit_aabb(const AABB &p_aabb) noexcept {
        Projection proj;
        proj.scale_translate_to_fit(p_aabb);
        return proj;
    }

    [[nodiscard]] Plane get_projection_plane(Planes p_plane) const noexcept {
        const real_t *m = &columns[0][0];
        Plane p;
        switch (p_plane) {
            case Planes::Near:
                p = Plane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
                break;
            case Planes::Far:
                p = Plane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
                break;
            case Planes::Left:
                p = Plane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
                break;
            case Planes::Top:
                p = Plane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
                break;
            case Planes::Right:
                p = Plane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
                break;
            case Planes::Bottom:
                p = Plane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
                break;
            default:
                return Plane();
        }
        p.normal = -p.normal;
        p.normalize();
        return p;
    }

    [[nodiscard]] std::array<Plane, 6> get_projection_planes(const Transform3D &p_transform) const noexcept {
        const real_t *m = &columns[0][0];
        std::array<Plane, 6> planes;

        // Near
        Plane p = Plane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        p.normal = -p.normal;
        p.normalize();
        planes[0] = p_transform.xform(p);

        // Far
        p = Plane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
        p.normal = -p.normal;
        p.normalize();
        planes[1] = p_transform.xform(p);

        // Left
        p = Plane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        p.normal = -p.normal;
        p.normalize();
        planes[2] = p_transform.xform(p);

        // Top
        p = Plane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        p.normal = -p.normal;
        p.normalize();
        planes[3] = p_transform.xform(p);

        // Right
        p = Plane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        p.normal = -p.normal;
        p.normalize();
        planes[4] = p_transform.xform(p);

        // Bottom
        p = Plane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        p.normal = -p.normal;
        p.normalize();
        planes[5] = p_transform.xform(p);

        return planes;
    }

    bool get_endpoints(const Transform3D &p_transform, Vector3 *p_8points) const noexcept {
        assert(p_8points != nullptr);
        std::array<Plane, 6> planes = get_projection_planes(Transform3D());
        const Planes intersections[8][3] = {
            { Planes::Far, Planes::Left, Planes::Top },
            { Planes::Far, Planes::Left, Planes::Bottom },
            { Planes::Far, Planes::Right, Planes::Top },
            { Planes::Far, Planes::Right, Planes::Bottom },
            { Planes::Near, Planes::Left, Planes::Top },
            { Planes::Near, Planes::Left, Planes::Bottom },
            { Planes::Near, Planes::Right, Planes::Top },
            { Planes::Near, Planes::Right, Planes::Bottom },
        };

        for (usize i = 0; i < 8; ++i) {
            Vector3 point;
            const Plane &a = planes[(usize)intersections[i][0]];
            const Plane &b = planes[(usize)intersections[i][1]];
            const Plane &c = planes[(usize)intersections[i][2]];
            if (!a.intersect_3(b, c, &point)) {
                return false;
            }
            p_8points[i] = p_transform.xform(point);
        }
        return true;
    }

    void flip_y() noexcept {
        for (usize i = 0; i < 4; ++i) {
            columns[1][i] = -columns[1][i];
        }
    }

    [[nodiscard]] Projection flipped_y() const noexcept {
        Projection proj = *this;
        proj.flip_y();
        return proj;
    }

    void add_jitter_offset(const Vector2 &p_offset) noexcept {
        columns[3][0] += p_offset.x;
        columns[3][1] += p_offset.y;
    }

    [[nodiscard]] Projection jitter_offseted(const Vector2 &p_offset) const noexcept {
        Projection proj = *this;
        proj.add_jitter_offset(p_offset);
        return proj;
    }

    void make_scale(const Vector3 &p_scale) noexcept {
        set_identity();
        columns[0][0] = p_scale.x;
        columns[1][1] = p_scale.y;
        columns[2][2] = p_scale.z;
    }

    void scale_translate_to_fit(const AABB &p_aabb) noexcept {
        Vector3 min = p_aabb.position;
        Vector3 max = p_aabb.position + p_aabb.size;

        columns[0] = Vector4((real_t)2.0 / (max.x - min.x), (real_t)0.0, (real_t)0.0, (real_t)0.0);
        columns[1] = Vector4((real_t)0.0, (real_t)2.0 / (max.y - min.y), (real_t)0.0, (real_t)0.0);
        columns[2] = Vector4((real_t)0.0, (real_t)0.0, (real_t)2.0 / (max.z - min.z), (real_t)0.0);
        columns[3] = Vector4(
            -(max.x + min.x) / (max.x - min.x),
            -(max.y + min.y) / (max.y - min.y),
            -(max.z + min.z) / (max.z - min.z),
            (real_t)1.0
        );
    }

    [[nodiscard]] real_t get_lod_multiplier() const noexcept {
        assert(columns[0][0] != (real_t)0.0);
        return (real_t)2.0 / columns[0][0];
    }

    [[nodiscard]] int get_pixels_per_meter(int p_for_pixel_width) const noexcept {
        real_t width = (real_t)2.0 * (-get_z_near() * columns[2][3] + columns[3][3]) / columns[0][0];
        assert(width != (real_t)0.0);
        return (int)((real_t)p_for_pixel_width / width);
    }

    void invert() noexcept {
        Projection temp;
        const real_t *m = &columns[0][0];

        real_t wtmp[4][8];
        real_t *r0 = wtmp[0];
        real_t *r1 = wtmp[1];
        real_t *r2 = wtmp[2];
        real_t *r3 = wtmp[3];

        auto mat = [m](usize r, usize c) noexcept -> real_t {
            return m[c * 4 + r];
        };

        r0[0] = mat(0, 0); r0[1] = mat(0, 1); r0[2] = mat(0, 2); r0[3] = mat(0, 3);
        r0[4] = (real_t)1.0; r0[5] = (real_t)0.0; r0[6] = (real_t)0.0; r0[7] = (real_t)0.0;

        r1[0] = mat(1, 0); r1[1] = mat(1, 1); r1[2] = mat(1, 2); r1[3] = mat(1, 3);
        r1[4] = (real_t)0.0; r1[5] = (real_t)1.0; r1[6] = (real_t)0.0; r1[7] = (real_t)0.0;

        r2[0] = mat(2, 0); r2[1] = mat(2, 1); r2[2] = mat(2, 2); r2[3] = mat(2, 3);
        r2[4] = (real_t)0.0; r2[5] = (real_t)0.0; r2[6] = (real_t)1.0; r2[7] = (real_t)0.0;

        r3[0] = mat(3, 0); r3[1] = mat(3, 1); r3[2] = mat(3, 2); r3[3] = mat(3, 3);
        r3[4] = (real_t)0.0; r3[5] = (real_t)0.0; r3[6] = (real_t)0.0; r3[7] = (real_t)1.0;

        if (Math::abs(r3[0]) > Math::abs(r2[0])) std::swap(r3, r2);
        if (Math::abs(r2[0]) > Math::abs(r1[0])) std::swap(r2, r1);
        if (Math::abs(r1[0]) > Math::abs(r0[0])) std::swap(r1, r0);
        assert(r0[0] != (real_t)0.0);

        real_t m1 = r1[0] / r0[0];
        real_t m2 = r2[0] / r0[0];
        real_t m3 = r3[0] / r0[0];
        real_t s = r0[1];
        r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
        s = r0[2];
        r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
        s = r0[3];
        r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
        s = r0[4];
        if (s != (real_t)0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
        s = r0[5];
        if (s != (real_t)0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
        s = r0[6];
        if (s != (real_t)0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
        s = r0[7];
        if (s != (real_t)0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

        if (Math::abs(r3[1]) > Math::abs(r2[1])) std::swap(r3, r2);
        if (Math::abs(r2[1]) > Math::abs(r1[1])) std::swap(r2, r1);
        assert(r1[1] != (real_t)0.0);

        m2 = r2[1] / r1[1];
        m3 = r3[1] / r1[1];
        r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
        r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
        s = r1[4]; if (s != (real_t)0.0) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
        s = r1[5]; if (s != (real_t)0.0) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
        s = r1[6]; if (s != (real_t)0.0) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
        s = r1[7]; if (s != (real_t)0.0) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

        if (Math::abs(r3[2]) > Math::abs(r2[2])) std::swap(r3, r2);
        assert(r2[2] != (real_t)0.0);

        m3 = r3[2] / r2[2];
        r3[3] -= m3 * r2[3];
        r3[4] -= m3 * r2[4];
        r3[5] -= m3 * r2[5];
        r3[6] -= m3 * r2[6];
        r3[7] -= m3 * r2[7];

        assert(r3[3] != (real_t)0.0);

        s = (real_t)1.0 / r3[3];
        r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

        m2 = r2[3];
        s = (real_t)1.0 / r2[2];
        r2[4] = s * (r2[4] - r3[4] * m2);
        r2[5] = s * (r2[5] - r3[5] * m2);
        r2[6] = s * (r2[6] - r3[6] * m2);
        r2[7] = s * (r2[7] - r3[7] * m2);

        m1 = r1[3];
        r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1; r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;

        real_t m0 = r0[3];
        r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0; r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;

        m1 = r1[2];
        s = (real_t)1.0 / r1[1];
        r1[4] = s * (r1[4] - r2[4] * m1);
        r1[5] = s * (r1[5] - r2[5] * m1);
        r1[6] = s * (r1[6] - r2[6] * m1);
        r1[7] = s * (r1[7] - r2[7] * m1);

        m0 = r0[2];
        r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0; r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;

        m0 = r0[1];
        s = (real_t)1.0 / r0[0];
        r0[4] = s * (r0[4] - r1[4] * m0);
        r0[5] = s * (r0[5] - r1[5] * m0);
        r0[6] = s * (r0[6] - r1[6] * m0);
        r0[7] = s * (r0[7] - r1[7] * m0);

        auto set_out = [&temp](usize r, usize c, real_t val) noexcept {
            temp.columns[c][r] = val;
        };

        set_out(0, 0, r0[4]); set_out(0, 1, r0[5]); set_out(0, 2, r0[6]); set_out(0, 3, r0[7]);
        set_out(1, 0, r1[4]); set_out(1, 1, r1[5]); set_out(1, 2, r1[6]); set_out(1, 3, r1[7]);
        set_out(2, 0, r2[4]); set_out(2, 1, r2[5]); set_out(2, 2, r2[6]); set_out(2, 3, r2[7]);
        set_out(3, 0, r3[4]); set_out(3, 1, r3[5]); set_out(3, 2, r3[6]); set_out(3, 3, r3[7]);

        *this = temp;
    }

    [[nodiscard]] Projection inverse() const noexcept {
        Projection cm = *this;
        cm.invert();
        return cm;
    }

    [[nodiscard]] explicit operator Transform3D() const noexcept {
        Transform3D tr;
        tr.basis.rows[0][0] = columns[0][0];
        tr.basis.rows[1][0] = columns[0][1];
        tr.basis.rows[2][0] = columns[0][2];

        tr.basis.rows[0][1] = columns[1][0];
        tr.basis.rows[1][1] = columns[1][1];
        tr.basis.rows[2][1] = columns[1][2];

        tr.basis.rows[0][2] = columns[2][0];
        tr.basis.rows[1][2] = columns[2][1];
        tr.basis.rows[2][2] = columns[2][2];

        tr.origin.x = columns[3][0];
        tr.origin.y = columns[3][1];
        tr.origin.z = columns[3][2];

        return tr;
    }

    [[nodiscard]] constexpr bool is_same(const Projection &other) const noexcept {
        return columns[0].is_same(other.columns[0]) &&
               columns[1].is_same(other.columns[1]) &&
               columns[2].is_same(other.columns[2]) &&
               columns[3].is_same(other.columns[3]);
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Projection &other) const noexcept {
        return columns[0].is_equal_approx(other.columns[0]) &&
               columns[1].is_equal_approx(other.columns[1]) &&
               columns[2].is_equal_approx(other.columns[2]) &&
               columns[3].is_equal_approx(other.columns[3]);
    }

    constexpr bool operator==(const Projection &other) const noexcept {
        for (usize i = 0; i < 4; ++i) {
            if (columns[i] != other.columns[i]) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr Projection operator*(const Projection &matrix) const noexcept {
        Projection new_matrix;
        for (usize j = 0; j < 4; ++j) {
            for (usize i = 0; i < 4; ++i) {
                real_t ab = (real_t)0.0;
                for (usize k = 0; k < 4; ++k) {
                    ab += columns[k][i] * matrix.columns[j][k];
                }
                new_matrix.columns[j][i] = ab;
            }
        }
        return new_matrix;
    }
};

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Projection> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Projection &p, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[X: ({}, {}, {}, {}), Y: ({}, {}, {}, {}), Z: ({}, {}, {}, {}), W: ({}, {}, {}, {})]",
            p.columns[0].x, p.columns[0].y, p.columns[0].z, p.columns[0].w,
            p.columns[1].x, p.columns[1].y, p.columns[1].z, p.columns[1].w,
            p.columns[2].x, p.columns[2].y, p.columns[2].z, p.columns[2].w,
            p.columns[3].x, p.columns[3].y, p.columns[3].z, p.columns[3].w
        );
    }
};
