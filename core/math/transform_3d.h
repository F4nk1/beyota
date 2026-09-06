/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  transform_3d.h                                                        */
/**************************************************************************/

#pragma once

#include "core/primitives.h"
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector3.h"
#include "core/math/basis.h"
#include "core/math/aabb.h"
#include "core/math/plane.h"

#include <cassert>
#include <format>
#include <algorithm>

namespace Beyota::Math {

struct [[nodiscard]] Transform3D {
    Basis basis;
    Vector3 origin;

    static const Transform3D FLIP_X;
    static const Transform3D FLIP_Y;
    static const Transform3D FLIP_Z;

    constexpr Transform3D() = default;

    constexpr Transform3D(const Basis &p_basis, const Vector3 &p_origin = Vector3()) noexcept
        : basis(p_basis), origin(p_origin) {}

    constexpr Transform3D(const Vector3 &p_x, const Vector3 &p_y, const Vector3 &p_z, const Vector3 &p_origin) noexcept
        : basis(p_x, p_y, p_z), origin(p_origin) {}

    constexpr Transform3D(real_t p_xx, real_t p_xy, real_t p_xz,
                          real_t p_yx, real_t p_yy, real_t p_yz,
                          real_t p_zx, real_t p_zy, real_t p_zz,
                          real_t p_ox, real_t p_oy, real_t p_oz) noexcept
        : basis(p_xx, p_xy, p_xz, p_yx, p_yy, p_yz, p_zx, p_zy, p_zz),
          origin(p_ox, p_oy, p_oz) {}

    constexpr void set(real_t p_xx, real_t p_xy, real_t p_xz,
                       real_t p_yx, real_t p_yy, real_t p_yz,
                       real_t p_zx, real_t p_zy, real_t p_zz,
                       real_t p_tx, real_t p_ty, real_t p_tz) noexcept {
        basis.set(p_xx, p_xy, p_xz, p_yx, p_yy, p_yz, p_zx, p_zy, p_zz);
        origin.x = p_tx;
        origin.y = p_ty;
        origin.z = p_tz;
    }

    [[nodiscard]] constexpr const Basis &get_basis() const noexcept { return basis; }
    constexpr void set_basis(const Basis &p_basis) noexcept { basis = p_basis; }

    [[nodiscard]] constexpr const Vector3 &get_origin() const noexcept { return origin; }
    constexpr void set_origin(const Vector3 &p_origin) noexcept { origin = p_origin; }

    void affine_invert() noexcept {
        basis.invert();
        origin = basis.xform(-origin);
    }

    [[nodiscard]] Transform3D affine_inverse() const noexcept {
        Transform3D ret = *this;
        ret.affine_invert();
        return ret;
    }

    constexpr void invert() noexcept {
        basis.transpose();
        origin = basis.xform(-origin);
    }

    [[nodiscard]] constexpr Transform3D inverse() const noexcept {
        Transform3D ret = *this;
        ret.invert();
        return ret;
    }

    [[nodiscard]] Transform3D rotated(const Vector3 &p_axis, real_t p_angle) const noexcept {
        Basis p_basis(p_axis, p_angle);
        return Transform3D(p_basis * basis, p_basis.xform(origin));
    }

    [[nodiscard]] Transform3D rotated_local(const Vector3 &p_axis, real_t p_angle) const noexcept {
        Basis p_basis(p_axis, p_angle);
        return Transform3D(basis * p_basis, origin);
    }

    void rotate(const Vector3 &p_axis, real_t p_angle) noexcept {
        *this = rotated(p_axis, p_angle);
    }

    void rotate_basis(const Vector3 &p_axis, real_t p_angle) noexcept {
        basis.rotate(p_axis, p_angle);
    }

    [[nodiscard]] Transform3D looking_at(const Vector3 &p_target, const Vector3 &p_up = Vector3::UP, bool p_use_model_front = false) const noexcept {
        assert(!origin.is_equal_approx(p_target));
        Transform3D t = *this;
        t.basis = Basis::looking_at(p_target - origin, p_up, p_use_model_front);
        return t;
    }

    void set_look_at(const Vector3 &p_eye, const Vector3 &p_target, const Vector3 &p_up = Vector3::UP, bool p_use_model_front = false) noexcept {
        assert(!p_eye.is_equal_approx(p_target));
        basis = Basis::looking_at(p_target - p_eye, p_up, p_use_model_front);
        origin = p_eye;
    }

    void scale(const Vector3 &p_scale) noexcept {
        basis.scale(p_scale);
        origin *= p_scale;
    }

    [[nodiscard]] Transform3D scaled(const Vector3 &p_scale) const noexcept {
        return Transform3D(basis.scaled(p_scale), origin * p_scale);
    }

    [[nodiscard]] Transform3D scaled_local(const Vector3 &p_scale) const noexcept {
        return Transform3D(basis.scaled_local(p_scale), origin);
    }

    void scale_basis(const Vector3 &p_scale) noexcept {
        basis.scale(p_scale);
    }

    void translate_local(real_t p_tx, real_t p_ty, real_t p_tz) noexcept {
        translate_local(Vector3(p_tx, p_ty, p_tz));
    }

    void translate_local(const Vector3 &p_translation) noexcept {
        for (usize i = 0; i < 3; ++i) {
            origin[i] += basis[i].dot(p_translation);
        }
    }

    [[nodiscard]] constexpr Transform3D translated(const Vector3 &p_translation) const noexcept {
        return Transform3D(basis, origin + p_translation);
    }

    [[nodiscard]] Transform3D translated_local(const Vector3 &p_translation) const noexcept {
        return Transform3D(basis, origin + basis.xform(p_translation));
    }

    void orthonormalize() noexcept {
        basis.orthonormalize();
    }

    [[nodiscard]] Transform3D orthonormalized() const noexcept {
        Transform3D copy = *this;
        copy.orthonormalize();
        return copy;
    }

    void orthogonalize() noexcept {
        basis.orthogonalize();
    }

    [[nodiscard]] Transform3D orthogonalized() const noexcept {
        Transform3D copy = *this;
        copy.orthogonalize();
        return copy;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Transform3D &p_transform) const noexcept {
        return basis.is_equal_approx(p_transform.basis) && origin.is_equal_approx(p_transform.origin);
    }

    [[nodiscard]] constexpr bool is_same(const Transform3D &p_transform) const noexcept {
        return basis.is_same(p_transform.basis) && origin.is_same(p_transform.origin);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return basis.is_finite() && origin.is_finite();
    }

    [[nodiscard]] constexpr Vector3 xform(const Vector3 &p_vector) const noexcept {
        return Vector3(
            basis[0].dot(p_vector) + origin.x,
            basis[1].dot(p_vector) + origin.y,
            basis[2].dot(p_vector) + origin.z
        );
    }

    [[nodiscard]] constexpr Vector3 xform_inv(const Vector3 &p_vector) const noexcept {
        Vector3 v = p_vector - origin;
        return Vector3(
            (basis.rows[0][0] * v.x) + (basis.rows[1][0] * v.y) + (basis.rows[2][0] * v.z),
            (basis.rows[0][1] * v.x) + (basis.rows[1][1] * v.y) + (basis.rows[2][1] * v.z),
            (basis.rows[0][2] * v.x) + (basis.rows[1][2] * v.y) + (basis.rows[2][2] * v.z)
        );
    }

    [[nodiscard]] AABB xform(const AABB &p_aabb) const noexcept {
        Vector3 min = p_aabb.position;
        Vector3 max = p_aabb.position + p_aabb.size;
        Vector3 tmin;
        Vector3 tmax;
        for (usize i = 0; i < 3; ++i) {
            tmin[i] = origin[i];
            tmax[i] = origin[i];
            for (usize j = 0; j < 3; ++j) {
                real_t e = basis[i][j] * min[j];
                real_t f = basis[i][j] * max[j];
                if (e < f) {
                    tmin[i] += e;
                    tmax[i] += f;
                } else {
                    tmin[i] += f;
                    tmax[i] += e;
                }
            }
        }
        return AABB(tmin, tmax - tmin);
    }

    [[nodiscard]] AABB xform_inv(const AABB &p_aabb) const noexcept {
        Vector3 vertices[8] = {
            Vector3(p_aabb.position.x + p_aabb.size.x, p_aabb.position.y + p_aabb.size.y, p_aabb.position.z + p_aabb.size.z),
            Vector3(p_aabb.position.x + p_aabb.size.x, p_aabb.position.y + p_aabb.size.y, p_aabb.position.z),
            Vector3(p_aabb.position.x + p_aabb.size.x, p_aabb.position.y, p_aabb.position.z + p_aabb.size.z),
            Vector3(p_aabb.position.x + p_aabb.size.x, p_aabb.position.y, p_aabb.position.z),
            Vector3(p_aabb.position.x, p_aabb.position.y + p_aabb.size.y, p_aabb.position.z + p_aabb.size.z),
            Vector3(p_aabb.position.x, p_aabb.position.y + p_aabb.size.y, p_aabb.position.z),
            Vector3(p_aabb.position.x, p_aabb.position.y, p_aabb.position.z + p_aabb.size.z),
            Vector3(p_aabb.position.x, p_aabb.position.y, p_aabb.position.z)
        };

        AABB ret;
        ret.position = xform_inv(vertices[0]);
        for (usize i = 1; i < 8; ++i) {
            ret.expand_to(xform_inv(vertices[i]));
        }
        return ret;
    }

    [[nodiscard]] Plane xform_fast(const Plane &p_plane, const Basis &p_basis_inverse_transpose) const noexcept {
        Vector3 point = p_plane.normal * p_plane.d;
        point = xform(point);

        Vector3 normal = p_basis_inverse_transpose.xform(p_plane.normal);
        normal.normalize();

        real_t d = normal.dot(point);
        return Plane(normal, d);
    }

    [[nodiscard]] static Plane xform_inv_fast(const Plane &p_plane, const Transform3D &p_inverse, const Basis &p_basis_transpose) noexcept {
        Vector3 point = p_plane.normal * p_plane.d;
        point = p_inverse.xform(point);

        Vector3 normal = p_basis_transpose.xform(p_plane.normal);
        normal.normalize();

        real_t d = normal.dot(point);
        return Plane(normal, d);
    }

    [[nodiscard]] Plane xform(const Plane &p_plane) const noexcept {
        Basis b = basis.inverse();
        b.transpose();
        return xform_fast(p_plane, b);
    }

    [[nodiscard]] Plane xform_inv(const Plane &p_plane) const noexcept {
        Transform3D inv = affine_inverse();
        Basis basis_transpose = basis.transposed();
        return xform_inv_fast(p_plane, inv, basis_transpose);
    }

    [[nodiscard]] constexpr Transform3D inverse_xform(const Transform3D &p_transform) const noexcept {
        Vector3 v = p_transform.origin - origin;
        return Transform3D(basis.transpose_xform(p_transform.basis), basis.xform(v));
    }

    [[nodiscard]] Transform3D interpolate_with(const Transform3D &p_transform, real_t p_c) const noexcept {
        Transform3D interp;

        Vector3 src_scale = basis.get_scale();
        Quaternion src_rot = basis.get_rotation_quaternion();
        Vector3 src_loc = origin;

        Vector3 dst_scale = p_transform.basis.get_scale();
        Quaternion dst_rot = p_transform.basis.get_rotation_quaternion();
        Vector3 dst_loc = p_transform.origin;

        interp.basis.set_quaternion_scale(src_rot.slerp(dst_rot, p_c).normalized(), src_scale.lerp(dst_scale, p_c));
        interp.origin = src_loc.lerp(dst_loc, p_c);

        return interp;
    }

    constexpr bool operator==(const Transform3D &p_transform) const noexcept {
        return (basis == p_transform.basis && origin == p_transform.origin);
    }

    constexpr void operator*=(const Transform3D &p_transform) noexcept {
        origin = xform(p_transform.origin);
        basis *= p_transform.basis;
    }

    [[nodiscard]] constexpr Transform3D operator*(const Transform3D &p_transform) const noexcept {
        Transform3D t = *this;
        t *= p_transform;
        return t;
    }

    constexpr void operator*=(real_t p_val) noexcept {
        origin *= p_val;
        basis *= p_val;
    }

    [[nodiscard]] constexpr Transform3D operator*(real_t p_val) const noexcept {
        Transform3D ret(*this);
        ret *= p_val;
        return ret;
    }

    constexpr void operator/=(real_t p_val) noexcept {
        assert(p_val != (real_t)0.0);
        basis /= p_val;
        origin /= p_val;
    }

    [[nodiscard]] constexpr Transform3D operator/(real_t p_val) const noexcept {
        assert(p_val != (real_t)0.0);
        Transform3D ret(*this);
        ret /= p_val;
        return ret;
    }
};

inline constexpr Transform3D Transform3D::FLIP_X = { Basis::FLIP_X };
inline constexpr Transform3D Transform3D::FLIP_Y = { Basis::FLIP_Y };
inline constexpr Transform3D Transform3D::FLIP_Z = { Basis::FLIP_Z };

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Transform3D> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Transform3D &t, std::format_context &ctx) const {
        const auto c0 = t.basis.get_column(0);
        const auto c1 = t.basis.get_column(1);
        const auto c2 = t.basis.get_column(2);
        return std::format_to(ctx.out(), "[X: ({}, {}, {}), Y: ({}, {}, {}), Z: ({}, {}, {}), O: ({}, {}, {})]",
            c0.x, c0.y, c0.z,
            c1.x, c1.y, c1.z,
            c2.x, c2.y, c2.z,
            t.origin.x, t.origin.y, t.origin.z
        );
    }
};
