/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  basis.h                                                               */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"

#include <algorithm>
#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Basis {
    static const Basis IDENTITY;
    static const Basis FLIP_X;
    static const Basis FLIP_Y;
    static const Basis FLIP_Z;

    Vector3 rows[3] = {
        Vector3(1, 0, 0),
        Vector3(0, 1, 0),
        Vector3(0, 0, 1)
    };

    constexpr Basis() = default;
    constexpr Basis(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2) noexcept :
        rows{ row0, row1, row2 } {}

    constexpr Basis(real_t xx, real_t xy, real_t xz,
                    real_t yx, real_t yy, real_t yz,
                    real_t zx, real_t zy, real_t zz) noexcept :
        rows{ Vector3(xx, xy, xz), Vector3(yx, yy, yz), Vector3(zx, zy, zz) } {}

    constexpr void set(real_t xx, real_t xy, real_t xz,
                       real_t yx, real_t yy, real_t yz,
                       real_t zx, real_t zy, real_t zz) noexcept {
        rows[0][0] = xx;
        rows[0][1] = xy;
        rows[0][2] = xz;
        rows[1][0] = yx;
        rows[1][1] = yy;
        rows[1][2] = yz;
        rows[2][0] = zx;
        rows[2][1] = zy;
        rows[2][2] = zz;
    }

    [[nodiscard]] static Basis looking_at(const Vector3 &target, const Vector3 &up = Vector3::UP, bool use_model_front = false) noexcept {
        assert(!target.is_zero_approx());
        assert(!up.is_zero_approx());
        Vector3 v_z = target.normalized();
        if (!use_model_front) {
            v_z = -v_z;
        }
        Vector3 v_x = up.cross(v_z);
        if (v_x.is_zero_approx()) {
            v_x = up.get_any_perpendicular();
        }
        v_x.normalize();
        Vector3 v_y = v_z.cross(v_x);

        Basis b;
        b.set_columns(v_x, v_y, v_z);
        return b;
    }

    Basis(const Quaternion &q) noexcept {
        set_quaternion(q);
    }

    Basis(const Quaternion &q, const Vector3 &scale_val) noexcept {
        set_quaternion_scale(q, scale_val);
    }

    Basis(const Vector3 &axis, real_t angle) noexcept {
        set_axis_angle(axis, angle);
    }

    Basis(const Vector3 &axis, real_t angle, const Vector3 &scale_val) noexcept {
        set_axis_angle_scale(axis, angle, scale_val);
    }

    Basis(const Vector3 &euler, EulerOrder order = EulerOrder::YXZ) noexcept {
        set_euler(euler, order);
    }

    [[nodiscard]] constexpr const Vector3 &operator[](usize row) const noexcept {
        assert(row < 3);
        return rows[row];
    }

    [[nodiscard]] constexpr Vector3 &operator[](usize row) noexcept {
        assert(row < 3);
        return rows[row];
    }

    [[nodiscard]] constexpr real_t tdotx(const Vector3 &v) const noexcept {
        return rows[0][0] * v.x + rows[1][0] * v.y + rows[2][0] * v.z;
    }

    [[nodiscard]] constexpr real_t tdoty(const Vector3 &v) const noexcept {
        return rows[0][1] * v.x + rows[1][1] * v.y + rows[2][1] * v.z;
    }

    [[nodiscard]] constexpr real_t tdotz(const Vector3 &v) const noexcept {
        return rows[0][2] * v.x + rows[1][2] * v.y + rows[2][2] * v.z;
    }

    [[nodiscard]] constexpr Vector3 get_column(usize i) const noexcept {
        assert(i < 3);
        return Vector3(rows[0][i], rows[1][i], rows[2][i]);
    }

    constexpr void set_column(usize i, const Vector3 &col) noexcept {
        assert(i < 3);
        rows[0][i] = col.x;
        rows[1][i] = col.y;
        rows[2][i] = col.z;
    }

    constexpr void set_columns(const Vector3 &col0, const Vector3 &col1, const Vector3 &col2) noexcept {
        set_column(0, col0);
        set_column(1, col1);
        set_column(2, col2);
    }

    [[nodiscard]] constexpr const Vector3 &get_row(usize i) const noexcept {
        assert(i < 3);
        return rows[i];
    }

    constexpr void set_row(usize i, const Vector3 &row) noexcept {
        assert(i < 3);
        rows[i] = row;
    }

    [[nodiscard]] constexpr Vector3 get_main_diagonal() const noexcept {
        return Vector3(rows[0][0], rows[1][1], rows[2][2]);
    }

    [[nodiscard]] constexpr real_t determinant() const noexcept {
        return rows[0][0] * (rows[1][1] * rows[2][2] - rows[1][2] * rows[2][1]) -
               rows[0][1] * (rows[1][0] * rows[2][2] - rows[1][2] * rows[2][0]) +
               rows[0][2] * (rows[1][0] * rows[2][1] - rows[1][1] * rows[2][0]);
    }

    constexpr void invert() noexcept {
        auto cofac = [this](usize r1, usize c1, usize r2, usize c2) {
            return rows[r1][c1] * rows[r2][c2] - rows[r1][c2] * rows[r2][c1];
        };
        real_t co[3] = {
            cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1)
        };
        real_t det = rows[0][0] * co[0] + rows[0][1] * co[1] + rows[0][2] * co[2];
        assert(det != (real_t)0.0);
        real_t s = (real_t)1.0 / det;

        set(
            co[0] * s, cofac(0, 2, 2, 1) * s, cofac(0, 1, 1, 2) * s,
            co[1] * s, cofac(0, 0, 2, 2) * s, cofac(0, 2, 1, 0) * s,
            co[2] * s, cofac(0, 1, 2, 0) * s, cofac(0, 0, 1, 1) * s
        );
    }

    [[nodiscard]] constexpr Basis inverse() const noexcept {
        Basis b = *this;
        b.invert();
        return b;
    }

    constexpr void transpose() noexcept {
        std::swap(rows[0][1], rows[1][0]);
        std::swap(rows[0][2], rows[2][0]);
        std::swap(rows[1][2], rows[2][1]);
    }

    [[nodiscard]] constexpr Basis transposed() const noexcept {
        Basis b = *this;
        b.transpose();
        return b;
    }

    [[nodiscard]] constexpr Basis transpose_xform(const Basis &m) const noexcept {
        return Basis(
            rows[0].x * m[0].x + rows[1].x * m[1].x + rows[2].x * m[2].x,
            rows[0].x * m[0].y + rows[1].x * m[1].y + rows[2].x * m[2].y,
            rows[0].x * m[0].z + rows[1].x * m[1].z + rows[2].x * m[2].z,
            rows[0].y * m[0].x + rows[1].y * m[1].x + rows[2].y * m[2].x,
            rows[0].y * m[0].y + rows[1].y * m[1].y + rows[2].y * m[2].y,
            rows[0].y * m[0].z + rows[1].y * m[1].z + rows[2].y * m[2].z,
            rows[0].z * m[0].x + rows[1].z * m[1].x + rows[2].z * m[2].x,
            rows[0].z * m[0].y + rows[1].z * m[1].y + rows[2].z * m[2].y,
            rows[0].z * m[0].z + rows[1].z * m[1].z + rows[2].z * m[2].z
        );
    }

    void orthonormalize() noexcept {
        Vector3 x = get_column(0);
        Vector3 y = get_column(1);
        Vector3 z = get_column(2);

        x.normalize();
        y = (y - x * (x.dot(y)));
        y.normalize();
        z = (z - x * (x.dot(z)) - y * (y.dot(z)));
        z.normalize();

        set_column(0, x);
        set_column(1, y);
        set_column(2, z);
    }

    [[nodiscard]] Basis orthonormalized() const noexcept {
        Basis c = *this;
        c.orthonormalize();
        return c;
    }

    void orthogonalize() noexcept {
        Vector3 scl = get_scale();
        orthonormalize();
        scale_local(scl);
    }

    [[nodiscard]] Basis orthogonalized() const noexcept {
        Basis c = *this;
        c.orthogonalize();
        return c;
    }

    [[nodiscard]] bool is_orthogonal() const noexcept {
        const Vector3 x = get_column(0);
        const Vector3 y = get_column(1);
        const Vector3 z = get_column(2);
        return Math::is_zero_approx(x.dot(y)) && Math::is_zero_approx(x.dot(z)) && Math::is_zero_approx(y.dot(z));
    }

    [[nodiscard]] bool is_diagonal() const noexcept {
        return Math::is_zero_approx(rows[0][1]) && Math::is_zero_approx(rows[0][2]) &&
               Math::is_zero_approx(rows[1][0]) && Math::is_zero_approx(rows[1][2]) &&
               Math::is_zero_approx(rows[2][0]) && Math::is_zero_approx(rows[2][1]);
    }

    [[nodiscard]] bool is_rotation() const noexcept {
        return Math::is_equal_approx(determinant(), (real_t)1.0, (real_t)UNIT_EPSILON<real_t>) && is_orthogonal();
    }

    [[nodiscard]] bool is_symmetric() const noexcept {
        return Math::is_equal_approx(rows[0][1], rows[1][0]) &&
               Math::is_equal_approx(rows[0][2], rows[2][0]) &&
               Math::is_equal_approx(rows[1][2], rows[2][1]);
    }

    [[nodiscard]] bool is_conformal() const noexcept {
        const Vector3 x = get_column(0);
        const Vector3 y = get_column(1);
        const Vector3 z = get_column(2);
        const real_t lx = x.length_squared();
        const real_t ly = y.length_squared();
        const real_t lz = z.length_squared();
        return Math::is_equal_approx(lx, ly) && Math::is_equal_approx(lx, lz) && is_orthogonal();
    }

    [[nodiscard]] Vector3 get_scale() const noexcept {
        real_t det_sign = Math::sign(determinant());
        return Vector3(
            get_column(0).length(),
            get_column(1).length(),
            det_sign * get_column(2).length()
        );
    }

    [[nodiscard]] Vector3 get_scale_abs() const noexcept {
        return Vector3(
            get_column(0).length(),
            get_column(1).length(),
            get_column(2).length()
        );
    }

    [[nodiscard]] Vector3 get_scale_local() const noexcept {
        real_t det_sign = Math::sign(determinant());
        return Vector3(
            rows[0].length(),
            rows[1].length(),
            det_sign * rows[2].length()
        );
    }

    void set_scale(const Vector3 &scale_val) noexcept {
        rows[0].normalize();
        rows[1].normalize();
        rows[2].normalize();
        rows[0] *= scale_val.x;
        rows[1] *= scale_val.y;
        rows[2] *= scale_val.z;
    }

    void scale(const Vector3 &scale_val) noexcept {
        rows[0] *= scale_val.x;
        rows[1] *= scale_val.y;
        rows[2] *= scale_val.z;
    }

    [[nodiscard]] Basis scaled(const Vector3 &scale_val) const noexcept {
        Basis b = *this;
        b.scale(scale_val);
        return b;
    }

    void scale_local(const Vector3 &scale_val) noexcept {
        set_column(0, get_column(0) * scale_val.x);
        set_column(1, get_column(1) * scale_val.y);
        set_column(2, get_column(2) * scale_val.z);
    }

    [[nodiscard]] Basis scaled_local(const Vector3 &scale_val) const noexcept {
        Basis b = *this;
        b.scale_local(scale_val);
        return b;
    }

    void scale_orthogonal(const Vector3 &scale_val) noexcept {
        Vector3 cur_scale = get_scale();
        if (Math::is_zero_approx(cur_scale.x) || Math::is_zero_approx(cur_scale.y) || Math::is_zero_approx(cur_scale.z)) {
            return;
        }
        scale_local(scale_val / cur_scale);
    }

    [[nodiscard]] Basis scaled_orthogonal(const Vector3 &scale_val) const noexcept {
        Basis b = *this;
        b.scale_orthogonal(scale_val);
        return b;
    }

    void rotate(const Vector3 &axis, real_t angle) noexcept {
        *this = Basis(axis, angle) * (*this);
    }

    [[nodiscard]] Basis rotated(const Vector3 &axis, real_t angle) const noexcept {
        return Basis(axis, angle) * (*this);
    }

    void rotate_local(const Vector3 &axis, real_t angle) noexcept {
        *this = (*this) * Basis(axis, angle);
    }

    [[nodiscard]] Basis rotated_local(const Vector3 &axis, real_t angle) const noexcept {
        return (*this) * Basis(axis, angle);
    }

    void rotate(const Quaternion &q) noexcept {
        *this = Basis(q) * (*this);
    }

    [[nodiscard]] Basis rotated(const Quaternion &q) const noexcept {
        return Basis(q) * (*this);
    }

    void rotate_to_align(const Vector3 &start_dir, const Vector3 &end_dir) noexcept {
        const Vector3 axis = start_dir.cross(end_dir).normalized();
        if (axis.length_squared() != (real_t)0.0) {
            real_t d = std::clamp(start_dir.dot(end_dir), (real_t)-1.0, (real_t)1.0);
            const real_t angle_rads = Math::acos(d);
            *this = Basis(axis, angle_rads) * (*this);
        }
    }

    void set_quaternion(const Quaternion &q) noexcept {
        real_t d = q.length_squared();
        real_t s = (real_t)2.0 / d;
        real_t xs = q.x * s, ys = q.y * s, zs = q.z * s;
        real_t wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
        real_t xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
        real_t yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
        rows[0] = Vector3((real_t)1.0 - (yy + zz), xy - wz, xz + wy);
        rows[1] = Vector3(xy + wz, (real_t)1.0 - (xx + zz), yz - wx);
        rows[2] = Vector3(xz - wy, yz + wx, (real_t)1.0 - (xx + yy));
    }

    void set_quaternion_scale(const Quaternion &q, const Vector3 &scale_val) noexcept {
        rows[0] = Vector3(scale_val.x, 0, 0);
        rows[1] = Vector3(0, scale_val.y, 0);
        rows[2] = Vector3(0, 0, scale_val.z);
        rotate(q);
    }

    void set_axis_angle(const Vector3 &axis, real_t angle) noexcept {
        assert(axis.is_normalized());
        Vector3 axis_sq(axis.x * axis.x, axis.y * axis.y, axis.z * axis.z);
        real_t cosine = Math::cos(angle);
        rows[0][0] = axis_sq.x + cosine * ((real_t)1.0 - axis_sq.x);
        rows[1][1] = axis_sq.y + cosine * ((real_t)1.0 - axis_sq.y);
        rows[2][2] = axis_sq.z + cosine * ((real_t)1.0 - axis_sq.z);

        real_t sine = Math::sin(angle);
        real_t t = (real_t)1.0 - cosine;

        real_t xyzt = axis.x * axis.y * t;
        real_t zyxs = axis.z * sine;
        rows[0][1] = xyzt - zyxs;
        rows[1][0] = xyzt + zyxs;

        xyzt = axis.x * axis.z * t;
        zyxs = axis.y * sine;
        rows[0][2] = xyzt + zyxs;
        rows[2][0] = xyzt - zyxs;

        xyzt = axis.y * axis.z * t;
        zyxs = axis.x * sine;
        rows[1][2] = xyzt - zyxs;
        rows[2][1] = xyzt + zyxs;
    }

    void set_axis_angle_scale(const Vector3 &axis, real_t angle, const Vector3 &scale_val) noexcept {
        set_axis_angle(axis, angle);
        scale(scale_val);
    }

    [[nodiscard]] Quaternion get_quaternion() const noexcept {
        assert(is_rotation());
        Basis m = *this;
        real_t trace = m.rows[0][0] + m.rows[1][1] + m.rows[2][2];
        real_t temp[4];

        if (trace > (real_t)0.0) {
            real_t s = Math::sqrt(trace + (real_t)1.0);
            temp[3] = (s * (real_t)0.5);
            s = (real_t)0.5 / s;
            temp[0] = ((m.rows[2][1] - m.rows[1][2]) * s);
            temp[1] = ((m.rows[0][2] - m.rows[2][0]) * s);
            temp[2] = ((m.rows[1][0] - m.rows[0][1]) * s);
        } else {
            int i = m.rows[0][0] < m.rows[1][1]
                ? (m.rows[1][1] < m.rows[2][2] ? 2 : 1)
                : (m.rows[0][0] < m.rows[2][2] ? 2 : 0);
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;

            real_t s = Math::sqrt(m.rows[i][i] - m.rows[j][j] - m.rows[k][k] + (real_t)1.0);
            temp[i] = s * (real_t)0.5;
            s = (real_t)0.5 / s;
            temp[3] = (m.rows[k][j] - m.rows[j][k]) * s;
            temp[j] = (m.rows[j][i] + m.rows[i][j]) * s;
            temp[k] = (m.rows[k][i] + m.rows[i][k]) * s;
        }

        return Quaternion(temp[0], temp[1], temp[2], temp[3]);
    }

    [[nodiscard]] Quaternion get_rotation_quaternion() const noexcept {
        Basis m = orthonormalized();
        if (m.determinant() < (real_t)0.0) {
            m.scale(Vector3(-1, -1, -1));
        }
        return m.get_quaternion();
    }

    void set_euler(const Vector3 &euler, EulerOrder order = EulerOrder::YXZ) noexcept {
        real_t cx = Math::cos(euler.x);
        real_t sx = Math::sin(euler.x);
        Basis xmat(1, 0, 0, 0, cx, -sx, 0, sx, cx);

        real_t cy = Math::cos(euler.y);
        real_t sy = Math::sin(euler.y);
        Basis ymat(cy, 0, sy, 0, 1, 0, -sy, 0, cy);

        real_t cz = Math::cos(euler.z);
        real_t sz = Math::sin(euler.z);
        Basis zmat(cz, -sz, 0, sz, cz, 0, 0, 0, 1);

        switch (order) {
            case EulerOrder::XYZ: *this = xmat * (ymat * zmat); break;
            case EulerOrder::XZY: *this = xmat * zmat * ymat; break;
            case EulerOrder::YXZ: *this = ymat * xmat * zmat; break;
            case EulerOrder::YZX: *this = ymat * zmat * xmat; break;
            case EulerOrder::ZXY: *this = zmat * xmat * ymat; break;
            case EulerOrder::ZYX: *this = zmat * ymat * xmat; break;
        }
    }

    [[nodiscard]] static Basis from_euler(const Vector3 &euler, EulerOrder order = EulerOrder::YXZ) noexcept {
        Basis b;
        b.set_euler(euler, order);
        return b;
    }

    [[nodiscard]] Vector3 get_euler(EulerOrder order = EulerOrder::YXZ) const noexcept {
        constexpr real_t epsilon = (real_t)0.00000025;
        switch (order) {
            case EulerOrder::XYZ: {
                Vector3 euler;
                real_t sy = rows[0][2];
                if (sy < ((real_t)1.0 - epsilon)) {
                    if (sy > -((real_t)1.0 - epsilon)) {
                        if (rows[1][0] == (real_t)0 && rows[0][1] == (real_t)0 && rows[1][2] == (real_t)0 && rows[2][1] == (real_t)0 && rows[1][1] == (real_t)1) {
                            euler.x = (real_t)0;
                            euler.y = Math::atan2(rows[0][2], rows[0][0]);
                            euler.z = (real_t)0;
                        } else {
                            euler.x = Math::atan2(-rows[1][2], rows[2][2]);
                            euler.y = Math::asin(sy);
                            euler.z = Math::atan2(-rows[0][1], rows[0][0]);
                        }
                    } else {
                        euler.x = Math::atan2(rows[2][1], rows[1][1]);
                        euler.y = -HALF_PI<real_t>;
                        euler.z = (real_t)0.0;
                    }
                } else {
                    euler.x = Math::atan2(rows[2][1], rows[1][1]);
                    euler.y = HALF_PI<real_t>;
                    euler.z = (real_t)0.0;
                }
                return euler;
            }
            case EulerOrder::XZY: {
                Vector3 euler;
                real_t sz = rows[0][1];
                if (sz < ((real_t)1.0 - epsilon)) {
                    if (sz > -((real_t)1.0 - epsilon)) {
                        euler.x = Math::atan2(rows[2][1], rows[1][1]);
                        euler.y = Math::atan2(rows[0][2], rows[0][0]);
                        euler.z = Math::asin(-sz);
                    } else {
                        euler.x = -Math::atan2(rows[1][2], rows[2][2]);
                        euler.y = (real_t)0.0;
                        euler.z = HALF_PI<real_t>;
                    }
                } else {
                    euler.x = -Math::atan2(rows[1][2], rows[2][2]);
                    euler.y = (real_t)0.0;
                    euler.z = -HALF_PI<real_t>;
                }
                return euler;
            }
            case EulerOrder::YXZ: {
                Vector3 euler;
                real_t m12 = rows[1][2];
                if (m12 < ((real_t)1.0 - epsilon)) {
                    if (m12 > -((real_t)1.0 - epsilon)) {
                        if (rows[1][0] == (real_t)0 && rows[0][1] == (real_t)0 && rows[0][2] == (real_t)0 && rows[2][0] == (real_t)0 && rows[0][0] == (real_t)1) {
                            euler.x = Math::atan2(-m12, rows[1][1]);
                            euler.y = (real_t)0;
                            euler.z = (real_t)0;
                        } else {
                            euler.x = Math::asin(-m12);
                            euler.y = Math::atan2(rows[0][2], rows[2][2]);
                            euler.z = Math::atan2(rows[1][0], rows[1][1]);
                        }
                    } else {
                        euler.x = HALF_PI<real_t>;
                        euler.y = Math::atan2(rows[0][1], rows[0][0]);
                        euler.z = (real_t)0;
                    }
                } else {
                    euler.x = -HALF_PI<real_t>;
                    euler.y = -Math::atan2(rows[0][1], rows[0][0]);
                    euler.z = (real_t)0;
                }
                return euler;
            }
            case EulerOrder::YZX: {
                Vector3 euler;
                real_t sz = rows[1][0];
                if (sz < ((real_t)1.0 - epsilon)) {
                    if (sz > -((real_t)1.0 - epsilon)) {
                        euler.x = Math::atan2(-rows[1][2], rows[1][1]);
                        euler.y = Math::atan2(-rows[2][0], rows[0][0]);
                        euler.z = Math::asin(sz);
                    } else {
                        euler.x = Math::atan2(rows[2][1], rows[2][2]);
                        euler.y = (real_t)0.0;
                        euler.z = -HALF_PI<real_t>;
                    }
                } else {
                    euler.x = Math::atan2(rows[2][1], rows[2][2]);
                    euler.y = (real_t)0.0;
                    euler.z = HALF_PI<real_t>;
                }
                return euler;
            }
            case EulerOrder::ZXY: {
                Vector3 euler;
                real_t sx = rows[2][1];
                if (sx < ((real_t)1.0 - epsilon)) {
                    if (sx > -((real_t)1.0 - epsilon)) {
                        euler.x = Math::asin(sx);
                        euler.y = Math::atan2(-rows[2][0], rows[2][2]);
                        euler.z = Math::atan2(-rows[0][1], rows[1][1]);
                    } else {
                        euler.x = -HALF_PI<real_t>;
                        euler.y = Math::atan2(rows[0][2], rows[0][0]);
                        euler.z = (real_t)0;
                    }
                } else {
                    euler.x = HALF_PI<real_t>;
                    euler.y = Math::atan2(rows[0][2], rows[0][0]);
                    euler.z = (real_t)0;
                }
                return euler;
            }
            case EulerOrder::ZYX: {
                Vector3 euler;
                real_t sy = rows[2][0];
                if (sy < ((real_t)1.0 - epsilon)) {
                    if (sy > -((real_t)1.0 - epsilon)) {
                        euler.x = Math::atan2(rows[2][1], rows[2][2]);
                        euler.y = Math::asin(-sy);
                        euler.z = Math::atan2(rows[1][0], rows[0][0]);
                    } else {
                        euler.x = (real_t)0;
                        euler.y = HALF_PI<real_t>;
                        euler.z = -Math::atan2(rows[0][1], rows[1][1]);
                    }
                } else {
                    euler.x = (real_t)0;
                    euler.y = -HALF_PI<real_t>;
                    euler.z = -Math::atan2(rows[0][1], rows[1][1]);
                }
                return euler;
            }
        }
        return Vector3();
    }

    [[nodiscard]] Vector3 get_euler_normalized(EulerOrder order = EulerOrder::YXZ) const noexcept {
        Basis m = orthonormalized();
        if (m.determinant() < (real_t)0.0) {
            m.scale(Vector3(-1, -1, -1));
        }
        return m.get_euler(order);
    }

    void get_axis_angle(Vector3 &axis, real_t &angle) const noexcept {
        Quaternion q = get_rotation_quaternion();
        q.get_axis_angle(axis, angle);
    }

    void get_rotation_axis_angle(Vector3 &axis, real_t &angle) const noexcept {
        get_axis_angle(axis, angle);
    }

    [[nodiscard]] constexpr Vector3 xform(const Vector3 &v) const noexcept {
        return Vector3(rows[0].dot(v), rows[1].dot(v), rows[2].dot(v));
    }

    [[nodiscard]] constexpr Vector3 xform_inv(const Vector3 &v) const noexcept {
        return Vector3(tdotx(v), tdoty(v), tdotz(v));
    }

    [[nodiscard]] Basis lerp(const Basis &to, real_t weight) const noexcept {
        return Basis(
            rows[0].lerp(to.rows[0], weight),
            rows[1].lerp(to.rows[1], weight),
            rows[2].lerp(to.rows[2], weight)
        );
    }

    [[nodiscard]] Basis slerp(const Basis &to, real_t weight) const noexcept {
        Quaternion from_q = get_rotation_quaternion();
        Quaternion to_q = to.get_rotation_quaternion();
        Basis b(from_q.slerp(to_q, weight));
        b.scale_local(get_scale().lerp(to.get_scale(), weight));
        return b;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Basis &other) const noexcept {
        return rows[0].is_equal_approx(other.rows[0]) &&
               rows[1].is_equal_approx(other.rows[1]) &&
               rows[2].is_equal_approx(other.rows[2]);
    }

    [[nodiscard]] constexpr bool is_same(const Basis &other) const noexcept {
        return rows[0].is_same(other.rows[0]) &&
               rows[1].is_same(other.rows[1]) &&
               rows[2].is_same(other.rows[2]);
    }

    [[nodiscard]] constexpr bool is_finite() const noexcept {
        return rows[0].is_finite() && rows[1].is_finite() && rows[2].is_finite();
    }

    constexpr void operator*=(const Basis &rhs) noexcept {
        set(
            rhs.tdotx(rows[0]), rhs.tdoty(rows[0]), rhs.tdotz(rows[0]),
            rhs.tdotx(rows[1]), rhs.tdoty(rows[1]), rhs.tdotz(rows[1]),
            rhs.tdotx(rows[2]), rhs.tdoty(rows[2]), rhs.tdotz(rows[2])
        );
    }

    [[nodiscard]] constexpr Basis operator*(const Basis &rhs) const noexcept {
        return Basis(
            rhs.tdotx(rows[0]), rhs.tdoty(rows[0]), rhs.tdotz(rows[0]),
            rhs.tdotx(rows[1]), rhs.tdoty(rows[1]), rhs.tdotz(rows[1]),
            rhs.tdotx(rows[2]), rhs.tdoty(rows[2]), rhs.tdotz(rows[2])
        );
    }

    constexpr void operator+=(const Basis &rhs) noexcept {
        rows[0] += rhs.rows[0];
        rows[1] += rhs.rows[1];
        rows[2] += rhs.rows[2];
    }

    [[nodiscard]] constexpr Basis operator+(const Basis &rhs) const noexcept {
        Basis b = *this;
        b += rhs;
        return b;
    }

    constexpr void operator-=(const Basis &rhs) noexcept {
        rows[0] -= rhs.rows[0];
        rows[1] -= rhs.rows[1];
        rows[2] -= rhs.rows[2];
    }

    [[nodiscard]] constexpr Basis operator-(const Basis &rhs) const noexcept {
        Basis b = *this;
        b -= rhs;
        return b;
    }

    constexpr void operator*=(real_t scalar) noexcept {
        rows[0] *= scalar;
        rows[1] *= scalar;
        rows[2] *= scalar;
    }

    [[nodiscard]] constexpr Basis operator*(real_t scalar) const noexcept {
        Basis b = *this;
        b *= scalar;
        return b;
    }

    constexpr void operator/=(real_t scalar) noexcept {
        assert(scalar != (real_t)0.0);
        rows[0] /= scalar;
        rows[1] /= scalar;
        rows[2] /= scalar;
    }

    [[nodiscard]] constexpr Basis operator/(real_t scalar) const noexcept {
        assert(scalar != (real_t)0.0);
        Basis b = *this;
        b /= scalar;
        return b;
    }

    constexpr bool operator==(const Basis &rhs) const noexcept {
        return rows[0] == rhs.rows[0] && rows[1] == rhs.rows[1] && rows[2] == rhs.rows[2];
    }
};

inline constexpr Basis Basis::IDENTITY = {
    Vector3(1, 0, 0),
    Vector3(0, 1, 0),
    Vector3(0, 0, 1)
};
inline constexpr Basis Basis::FLIP_X = {
    Vector3(-1, 0, 0),
    Vector3(0, 1, 0),
    Vector3(0, 0, 1)
};
inline constexpr Basis Basis::FLIP_Y = {
    Vector3(1, 0, 0),
    Vector3(0, -1, 0),
    Vector3(0, 0, 1)
};
inline constexpr Basis Basis::FLIP_Z = {
    Vector3(1, 0, 0),
    Vector3(0, 1, 0),
    Vector3(0, 0, -1)
};

[[nodiscard]] constexpr Basis operator*(real_t scalar, const Basis &b) noexcept {
    return b * scalar;
}

inline Vector3 Quaternion::get_euler(EulerOrder order) const noexcept {
    assert(is_normalized());
    return Basis(*this).get_euler(order);
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Basis> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Basis &b, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[X: ({}, {}, {}), Y: ({}, {}, {}), Z: ({}, {}, {})]",
            b.rows[0].x, b.rows[0].y, b.rows[0].z,
            b.rows[1].x, b.rows[1].y, b.rows[1].z,
            b.rows[2].x, b.rows[2].y, b.rows[2].z
        );
    }
};
