/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  transform_2d.h                                                        */
/**************************************************************************/

#pragma once

#include "core/math/rect2.h"
#include "core/math/vector2.h"

#include <cassert>
#include <format>

namespace Beyota::Math {

struct [[nodiscard]] Transform2D {
    static const Transform2D IDENTITY;
    static const Transform2D FLIP_X;
    static const Transform2D FLIP_Y;

    Vector2 columns[3] = {
        { 1, 0 },
        { 0, 1 },
        { 0, 0 },
    };

    constexpr Transform2D() = default;
    constexpr Transform2D(real_t xx, real_t xy, real_t yx, real_t yy, real_t ox, real_t oy) noexcept :
        columns{ { xx, xy }, { yx, yy }, { ox, oy } } {}
    constexpr Transform2D(const Vector2 &x_axis, const Vector2 &y_axis, const Vector2 &origin) noexcept :
        columns{ x_axis, y_axis, origin } {}

    Transform2D(real_t rot, const Vector2 &pos) noexcept {
        const real_t cr = Math::cos(rot);
        const real_t sr = Math::sin(rot);
        columns[0][0] = cr;
        columns[0][1] = sr;
        columns[1][0] = -sr;
        columns[1][1] = cr;
        columns[2] = pos;
    }

    Transform2D(real_t rot, const Size2 &scale, real_t skew, const Vector2 &pos) noexcept {
        columns[0][0] = Math::cos(rot) * scale.x;
        columns[1][1] = Math::cos(rot + skew) * scale.y;
        columns[1][0] = -Math::sin(rot + skew) * scale.y;
        columns[0][1] = Math::sin(rot) * scale.x;
        columns[2] = pos;
    }

    [[nodiscard]] constexpr const Vector2 &operator[](usize idx) const noexcept {
        assert(idx < 3);
        return columns[idx];
    }

    [[nodiscard]] constexpr Vector2 &operator[](usize idx) noexcept {
        assert(idx < 3);
        return columns[idx];
    }

    [[nodiscard]] constexpr real_t tdotx(const Vector2 &v) const noexcept {
        return columns[0][0] * v.x + columns[1][0] * v.y;
    }

    [[nodiscard]] constexpr real_t tdoty(const Vector2 &v) const noexcept {
        return columns[0][1] * v.x + columns[1][1] * v.y;
    }

    [[nodiscard]] constexpr const Vector2 &get_origin() const noexcept { return columns[2]; }
    constexpr void set_origin(const Vector2 &origin) noexcept { columns[2] = origin; }

    [[nodiscard]] constexpr Vector2 basis_xform(const Vector2 &vec) const noexcept {
        return Vector2(tdotx(vec), tdoty(vec));
    }

    [[nodiscard]] constexpr Vector2 basis_xform_inv(const Vector2 &vec) const noexcept {
        return Vector2(columns[0].dot(vec), columns[1].dot(vec));
    }

    [[nodiscard]] constexpr Vector2 xform(const Vector2 &vec) const noexcept {
        return Vector2(tdotx(vec), tdoty(vec)) + columns[2];
    }

    [[nodiscard]] constexpr Vector2 xform_inv(const Vector2 &vec) const noexcept {
        const Vector2 v = vec - columns[2];
        return Vector2(columns[0].dot(v), columns[1].dot(v));
    }

    [[nodiscard]] Rect2 xform(const Rect2 &rect) const noexcept {
        Vector2 x = columns[0] * rect.size.x;
        Vector2 y = columns[1] * rect.size.y;
        Vector2 pos = xform(rect.position);

        Rect2 new_rect;
        new_rect.position = pos;
        new_rect.expand_to(pos + x);
        new_rect.expand_to(pos + y);
        new_rect.expand_to(pos + x + y);
        return new_rect;
    }

    [[nodiscard]] Rect2 xform_inv(const Rect2 &rect) const noexcept {
        return affine_inverse().xform(rect);
    }

    [[nodiscard]] constexpr real_t determinant() const noexcept {
        return columns[0].x * columns[1].y - columns[0].y * columns[1].x;
    }

    void invert() noexcept {
        std::swap(columns[0][1], columns[1][0]);
        columns[2] = basis_xform(-columns[2]);
    }

    [[nodiscard]] Transform2D inverse() const noexcept {
        Transform2D inv = *this;
        inv.invert();
        return inv;
    }

    void affine_invert() noexcept {
        const real_t det = determinant();
        assert(det != (real_t)0.0);
        const real_t idet = (real_t)1.0 / det;

        std::swap(columns[0][0], columns[1][1]);
        columns[0] *= Vector2(idet, -idet);
        columns[1] *= Vector2(-idet, idet);
        columns[2] = basis_xform(-columns[2]);
    }

    [[nodiscard]] Transform2D affine_inverse() const noexcept {
        Transform2D inv = *this;
        inv.affine_invert();
        return inv;
    }

    [[nodiscard]] real_t get_rotation() const noexcept {
        return Math::atan2(columns[0].y, columns[0].x);
    }

    void set_rotation(real_t rot) noexcept {
        const Size2 scale_val = get_scale();
        const real_t cr = Math::cos(rot);
        const real_t sr = Math::sin(rot);
        columns[0][0] = cr;
        columns[0][1] = sr;
        columns[1][0] = -sr;
        columns[1][1] = cr;
        set_scale(scale_val);
    }

    [[nodiscard]] real_t get_skew() const noexcept {
        const real_t det = determinant();
        return Math::acos(columns[0].normalized().dot(columns[1].normalized() * Math::sign(det))) - HALF_PI<real_t>;
    }

    void set_skew(real_t angle) noexcept {
        const real_t det = determinant();
        columns[1] = columns[0].rotated(HALF_PI<real_t> + angle).normalized() * (columns[1].length() * Math::sign(det));
    }

    void rotate(real_t angle) noexcept {
        *this = Transform2D(angle, Vector2()) * (*this);
    }

    [[nodiscard]] Size2 get_scale() const noexcept {
        const real_t det_sign = Math::sign(determinant());
        return Size2(columns[0].length(), det_sign * columns[1].length());
    }

    void set_scale(const Size2 &scale_val) noexcept {
        columns[0].normalize();
        columns[1].normalize();
        columns[0] *= scale_val.x;
        columns[1] *= scale_val.y;
    }

    void scale(const Size2 &scale_val) noexcept {
        scale_basis(scale_val);
        columns[2] *= scale_val;
    }

    void scale_basis(const Size2 &scale_val) noexcept {
        columns[0][0] *= scale_val.x;
        columns[0][1] *= scale_val.y;
        columns[1][0] *= scale_val.x;
        columns[1][1] *= scale_val.y;
    }

    void translate_local(real_t tx, real_t ty) noexcept {
        translate_local(Vector2(tx, ty));
    }

    void translate_local(const Vector2 &translation) noexcept {
        columns[2] += basis_xform(translation);
    }

    void orthonormalize() noexcept {
        Vector2 x = columns[0];
        Vector2 y = columns[1];
        x.normalize();
        y = y - x * x.dot(y);
        y.normalize();
        columns[0] = x;
        columns[1] = y;
    }

    [[nodiscard]] Transform2D orthonormalized() const noexcept {
        Transform2D ortho = *this;
        ortho.orthonormalize();
        return ortho;
    }

    [[nodiscard]] bool is_conformal() const noexcept {
        if (Math::is_equal_approx(columns[0][0], columns[1][1]) && Math::is_equal_approx(columns[0][1], -columns[1][0])) {
            return true;
        }
        if (Math::is_equal_approx(columns[0][0], -columns[1][1]) && Math::is_equal_approx(columns[0][1], columns[1][0])) {
            return true;
        }
        return false;
    }

    [[nodiscard]] bool is_orthogonal() const noexcept {
        return Math::is_zero_approx(columns[0].dot(columns[1]));
    }

    [[nodiscard]] bool is_equal_approx(const Transform2D &other) const noexcept {
        return columns[0].is_equal_approx(other.columns[0]) &&
               columns[1].is_equal_approx(other.columns[1]) &&
               columns[2].is_equal_approx(other.columns[2]);
    }

    [[nodiscard]] bool is_same(const Transform2D &other) const noexcept {
        return columns[0].is_same(other.columns[0]) &&
               columns[1].is_same(other.columns[1]) &&
               columns[2].is_same(other.columns[2]);
    }

    [[nodiscard]] bool is_finite() const noexcept {
        return columns[0].is_finite() && columns[1].is_finite() && columns[2].is_finite();
    }

    [[nodiscard]] Transform2D looking_at(const Vector2 &target) const noexcept {
        Transform2D ret = Transform2D(get_rotation(), get_origin());
        Vector2 target_pos = affine_inverse().xform(target);
        ret.set_rotation(ret.get_rotation() + (target_pos * get_scale()).angle());
        return ret;
    }

    [[nodiscard]] Transform2D scaled(const Size2 &scale_val) const noexcept {
        Transform2D copy = *this;
        copy.scale(scale_val);
        return copy;
    }

    [[nodiscard]] Transform2D scaled_local(const Size2 &scale_val) const noexcept {
        return Transform2D(columns[0] * scale_val.x, columns[1] * scale_val.y, columns[2]);
    }

    [[nodiscard]] Transform2D untranslated() const noexcept {
        Transform2D copy = *this;
        copy.columns[2] = Vector2();
        return copy;
    }

    [[nodiscard]] Transform2D translated(const Vector2 &offset) const noexcept {
        return Transform2D(columns[0], columns[1], columns[2] + offset);
    }

    [[nodiscard]] Transform2D translated_local(const Vector2 &offset) const noexcept {
        return Transform2D(columns[0], columns[1], columns[2] + basis_xform(offset));
    }

    [[nodiscard]] Transform2D rotated(real_t angle) const noexcept {
        return Transform2D(angle, Vector2()) * (*this);
    }

    [[nodiscard]] Transform2D rotated_local(real_t angle) const noexcept {
        return (*this) * Transform2D(angle, Vector2());
    }

    [[nodiscard]] Transform2D interpolate_with(const Transform2D &to, real_t weight) const noexcept {
        return Transform2D(
            Math::lerp_angle(get_rotation(), to.get_rotation(), weight),
            get_scale().lerp(to.get_scale(), weight),
            Math::lerp_angle(get_skew(), to.get_skew(), weight),
            get_origin().lerp(to.get_origin(), weight)
        );
    }

    constexpr void operator*=(const Transform2D &rhs) noexcept {
        columns[2] = xform(rhs.columns[2]);
        const real_t x0 = tdotx(rhs.columns[0]);
        const real_t x1 = tdoty(rhs.columns[0]);
        const real_t y0 = tdotx(rhs.columns[1]);
        const real_t y1 = tdoty(rhs.columns[1]);
        columns[0][0] = x0;
        columns[0][1] = x1;
        columns[1][0] = y0;
        columns[1][1] = y1;
    }

    [[nodiscard]] constexpr Transform2D operator*(const Transform2D &rhs) const noexcept {
        Transform2D t = *this;
        t *= rhs;
        return t;
    }

    constexpr bool operator==(const Transform2D &rhs) const noexcept {
        return columns[0] == rhs.columns[0] && columns[1] == rhs.columns[1] && columns[2] == rhs.columns[2];
    }
};

inline constexpr Transform2D Transform2D::IDENTITY = {
    { 1, 0 },
    { 0, 1 },
    { 0, 0 },
};
inline constexpr Transform2D Transform2D::FLIP_X = {
    { -1, 0 },
    { 0, 1 },
    { 0, 0 },
};
inline constexpr Transform2D Transform2D::FLIP_Y = {
    { 1, 0 },
    { 0, -1 },
    { 0, 0 },
};

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Transform2D> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Transform2D &t, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "[X: ({}, {}), Y: ({}, {}), O: ({}, {})]",
            t.columns[0].x, t.columns[0].y,
            t.columns[1].x, t.columns[1].y,
            t.columns[2].x, t.columns[2].y
        );
    }
};
