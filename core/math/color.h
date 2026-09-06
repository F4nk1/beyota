/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  color.h                                                               */
/**************************************************************************/

#pragma once

#include "core/primitives.h"
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/color_spaces.h"

#include <cassert>
#include <format>
#include <algorithm>
#include <array>
#include <string_view>
#include <cmath>

namespace Beyota::Math {

struct [[nodiscard]] Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    constexpr Color() = default;

    constexpr Color(float p_r, float p_g, float p_b, float p_a = 1.0f) noexcept
        : r(p_r), g(p_g), b(p_b), a(p_a) {}

    constexpr Color(const Color &p_c, float p_a) noexcept
        : r(p_c.r), g(p_c.g), b(p_c.b), a(p_a) {}

    [[nodiscard]] constexpr float &operator[](usize p_idx) noexcept {
        assert(p_idx < 4);
        return (&r)[p_idx];
    }

    [[nodiscard]] constexpr const float &operator[](usize p_idx) const noexcept {
        assert(p_idx < 4);
        return (&r)[p_idx];
    }

    [[nodiscard]] constexpr const float *as_float4_buffer() const noexcept {
        return &r;
    }

    [[nodiscard]] u32 to_rgba32() const noexcept {
        u32 c = (u8)Math::round(r * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(g * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(b * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(a * 255.0f);
        return c;
    }

    [[nodiscard]] u32 to_argb32() const noexcept {
        u32 c = (u8)Math::round(a * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(r * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(g * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(b * 255.0f);
        return c;
    }

    [[nodiscard]] u32 to_abgr32() const noexcept {
        u32 c = (u8)Math::round(a * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(b * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(g * 255.0f);
        c <<= 8;
        c |= (u8)Math::round(r * 255.0f);
        return c;
    }

    [[nodiscard]] u64 to_rgba64() const noexcept {
        u64 c = (u16)Math::round(r * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(g * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(b * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(a * 65535.0f);
        return c;
    }

    [[nodiscard]] u64 to_argb64() const noexcept {
        u64 c = (u16)Math::round(a * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(r * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(g * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(b * 65535.0f);
        return c;
    }

    [[nodiscard]] u64 to_abgr64() const noexcept {
        u64 c = (u16)Math::round(a * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(b * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(g * 65535.0f);
        c <<= 16;
        c |= (u16)Math::round(r * 65535.0f);
        return c;
    }

    [[nodiscard]] u32 to_rgbe9995() const noexcept {
        const float kMaxVal = (float)(0x1FF << 7);
        const float kMinVal = (float)(1.0f / (1 << 16));

        const float _r = std::clamp(r, 0.0f, kMaxVal);
        const float _g = std::clamp(g, 0.0f, kMaxVal);
        const float _b = std::clamp(b, 0.0f, kMaxVal);

        const float MaxChannel = std::max(std::max(_r, _g), std::max(_b, kMinVal));

        union {
            float f;
            u32 i;
        } R, G, B, E;

        E.f = MaxChannel;
        E.i += 0x07804000;
        E.i &= 0x7F800000;

        R.f = _r + E.f;
        G.f = _g + E.f;
        B.f = _b + E.f;

        E.i <<= 4;
        E.i += 0x10000000;

        return E.i | (B.i << 18U) | (G.i << 9U) | (R.i & 511U);
    }

    [[nodiscard]] static Color from_rgbe9995(u32 p_rgbe) noexcept {
        float r_val = (float)(p_rgbe & 0x1ff);
        float g_val = (float)((p_rgbe >> 9) & 0x1ff);
        float b_val = (float)((p_rgbe >> 18) & 0x1ff);
        float e_val = (float)(p_rgbe >> 27);
        float m = Math::pow(2.0f, e_val - 15.0f - 9.0f);

        return Color(r_val * m, g_val * m, b_val * m, 1.0f);
    }

    [[nodiscard]] static constexpr Color hex(u32 p_hex) noexcept {
        float a_val = (float)(p_hex & 0xFF) / 255.0f;
        p_hex >>= 8;
        float b_val = (float)(p_hex & 0xFF) / 255.0f;
        p_hex >>= 8;
        float g_val = (float)(p_hex & 0xFF) / 255.0f;
        p_hex >>= 8;
        float r_val = (float)(p_hex & 0xFF) / 255.0f;

        return Color(r_val, g_val, b_val, a_val);
    }

    [[nodiscard]] static constexpr Color hex64(u64 p_hex) noexcept {
        float a_val = (float)(p_hex & 0xFFFF) / 65535.0f;
        p_hex >>= 16;
        float b_val = (float)(p_hex & 0xFFFF) / 65535.0f;
        p_hex >>= 16;
        float g_val = (float)(p_hex & 0xFFFF) / 65535.0f;
        p_hex >>= 16;
        float r_val = (float)(p_hex & 0xFFFF) / 65535.0f;

        return Color(r_val, g_val, b_val, a_val);
    }

    [[nodiscard]] static constexpr Color from_rgba8(i64 p_r8, i64 p_g8, i64 p_b8, i64 p_a8 = 255) noexcept {
        return Color((float)p_r8 / 255.0f, (float)p_g8 / 255.0f, (float)p_b8 / 255.0f, (float)p_a8 / 255.0f);
    }

    constexpr void set_r8(i32 p_r8) noexcept { r = (float)std::clamp(p_r8, 0, 255) / 255.0f; }
    [[nodiscard]] i32 get_r8() const noexcept { return (i32)std::clamp(Math::round(r * 255.0f), 0.0f, 255.0f); }

    constexpr void set_g8(i32 p_g8) noexcept { g = (float)std::clamp(p_g8, 0, 255) / 255.0f; }
    [[nodiscard]] i32 get_g8() const noexcept { return (i32)std::clamp(Math::round(g * 255.0f), 0.0f, 255.0f); }

    constexpr void set_b8(i32 p_b8) noexcept { b = (float)std::clamp(p_b8, 0, 255) / 255.0f; }
    [[nodiscard]] i32 get_b8() const noexcept { return (i32)std::clamp(Math::round(b * 255.0f), 0.0f, 255.0f); }

    constexpr void set_a8(i32 p_a8) noexcept { a = (float)std::clamp(p_a8, 0, 255) / 255.0f; }
    [[nodiscard]] i32 get_a8() const noexcept { return (i32)std::clamp(Math::round(a * 255.0f), 0.0f, 255.0f); }

    [[nodiscard]] float get_h() const noexcept {
        float min_v = std::min(r, std::min(g, b));
        float max_v = std::max(r, std::max(g, b));
        float delta = max_v - min_v;

        if (delta == 0.0f) {
            return 0.0f;
        }

        float h_val;
        if (r == max_v) {
            h_val = (g - b) / delta;
        } else if (g == max_v) {
            h_val = 2.0f + (b - r) / delta;
        } else {
            h_val = 4.0f + (r - g) / delta;
        }

        h_val /= 6.0f;
        if (h_val < 0.0f) {
            h_val += 1.0f;
        }
        return h_val;
    }

    [[nodiscard]] constexpr float get_s() const noexcept {
        float min_v = std::min(r, std::min(g, b));
        float max_v = std::max(r, std::max(g, b));
        float delta = max_v - min_v;
        return (max_v != 0.0f) ? (delta / max_v) : 0.0f;
    }

    [[nodiscard]] constexpr float get_v() const noexcept {
        return std::max(r, std::max(g, b));
    }

    void set_hsv(float p_h, float p_s, float p_v, float p_alpha = 1.0f) noexcept {
        a = p_alpha;

        if (p_s == 0.0f) {
            r = g = b = p_v;
            return;
        }

        p_h *= 6.0f;
        p_h = Math::fposmod(p_h, 6.0f);
        int i = (int)Math::floor(p_h);

        float f = p_h - (float)i;
        float p = p_v * (1.0f - p_s);
        float q = p_v * (1.0f - p_s * f);
        float t = p_v * (1.0f - p_s * (1.0f - f));

        switch (i) {
            case 0: r = p_v; g = t; b = p; break;
            case 1: r = q; g = p_v; b = p; break;
            case 2: r = p; g = p_v; b = t; break;
            case 3: r = p; g = q; b = p_v; break;
            case 4: r = t; g = p; b = p_v; break;
            default: r = p_v; g = p; b = q; break;
        }
    }

    [[nodiscard]] static Color from_hsv(float p_h, float p_s, float p_v, float p_alpha = 1.0f) noexcept {
        Color c;
        c.set_hsv(p_h, p_s, p_v, p_alpha);
        return c;
    }

    void set_h(float p_h) noexcept { set_hsv(p_h, get_s(), get_v(), a); }
    void set_s(float p_s) noexcept { set_hsv(get_h(), p_s, get_v(), a); }
    void set_v(float p_v) noexcept { set_hsv(get_h(), get_s(), p_v, a); }

    void set_ok_hsl(float p_h, float p_s, float p_l, float p_alpha = 1.0f) noexcept {
        ok_color::HSL hsl;
        hsl.h = p_h;
        hsl.s = p_s;
        hsl.l = p_l;
        ok_color::RGB rgb = ok_color::okhsl_to_srgb(hsl);
        Color c = Color(rgb.r, rgb.g, rgb.b, p_alpha).clamp();
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }

    void set_ok_hsv(float p_h, float p_s, float p_v, float p_alpha = 1.0f) noexcept {
        ok_color::HSV hsv;
        hsv.h = p_h;
        hsv.s = p_s;
        hsv.v = p_v;
        ok_color::RGB rgb = ok_color::okhsv_to_srgb(hsv);
        Color c = Color(rgb.r, rgb.g, rgb.b, p_alpha).clamp();
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }

    [[nodiscard]] static Color from_ok_hsl(float p_h, float p_s, float p_l, float p_alpha = 1.0f) noexcept {
        Color c;
        c.set_ok_hsl(p_h, p_s, p_l, p_alpha);
        return c;
    }

    [[nodiscard]] static Color from_ok_hsv(float p_h, float p_s, float p_v, float p_alpha = 1.0f) noexcept {
        Color c;
        c.set_ok_hsv(p_h, p_s, p_v, p_alpha);
        return c;
    }

    [[nodiscard]] float get_ok_hsl_h() const noexcept {
        ok_color::RGB rgb{ r, g, b };
        ok_color::HSL ok_hsl = ok_color::srgb_to_okhsl(rgb);
        if (Math::is_nan(ok_hsl.h)) return 0.0f;
        return std::clamp(ok_hsl.h, 0.0f, 1.0f);
    }

    [[nodiscard]] float get_ok_hsl_s() const noexcept {
        ok_color::RGB rgb{ r, g, b };
        ok_color::HSL ok_hsl = ok_color::srgb_to_okhsl(rgb);
        if (Math::is_nan(ok_hsl.s)) return 0.0f;
        return std::clamp(ok_hsl.s, 0.0f, 1.0f);
    }

    [[nodiscard]] float get_ok_hsl_l() const noexcept {
        ok_color::RGB rgb{ r, g, b };
        ok_color::HSL ok_hsl = ok_color::srgb_to_okhsl(rgb);
        if (Math::is_nan(ok_hsl.l)) return 0.0f;
        return std::clamp(ok_hsl.l, 0.0f, 1.0f);
    }

    void set_ok_hsl_h(float p_h) noexcept { set_ok_hsl(p_h, get_ok_hsl_s(), get_ok_hsl_l(), a); }
    void set_ok_hsl_s(float p_s) noexcept { set_ok_hsl(get_ok_hsl_h(), p_s, get_ok_hsl_l(), a); }
    void set_ok_hsl_l(float p_l) noexcept { set_ok_hsl(get_ok_hsl_h(), get_ok_hsl_s(), p_l, a); }

    [[nodiscard]] constexpr float get_luminance() const noexcept {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    [[nodiscard]] constexpr Color lerp(const Color &p_to, float p_weight) const noexcept {
        return Color(
            Math::lerp(r, p_to.r, p_weight),
            Math::lerp(g, p_to.g, p_weight),
            Math::lerp(b, p_to.b, p_weight),
            Math::lerp(a, p_to.a, p_weight)
        );
    }

    [[nodiscard]] constexpr Color darkened(float p_amount) const noexcept {
        return Color(
            r * (1.0f - p_amount),
            g * (1.0f - p_amount),
            b * (1.0f - p_amount),
            a
        );
    }

    [[nodiscard]] constexpr Color lightened(float p_amount) const noexcept {
        return Color(
            r + (1.0f - r) * p_amount,
            g + (1.0f - g) * p_amount,
            b + (1.0f - b) * p_amount,
            a
        );
    }

    [[nodiscard]] constexpr Color blend(const Color &p_over) const noexcept {
        Color res;
        float sa = 1.0f - p_over.a;
        res.a = a * sa + p_over.a;
        if (res.a == 0.0f) {
            return Color(0.0f, 0.0f, 0.0f, 0.0f);
        }
        res.r = (r * a * sa + p_over.r * p_over.a) / res.a;
        res.g = (g * a * sa + p_over.g * p_over.a) / res.a;
        res.b = (b * a * sa + p_over.b * p_over.a) / res.a;
        return res;
    }

    [[nodiscard]] Color srgb_to_linear() const noexcept {
        return Color(
            r < 0.04045f ? r * (1.0f / 12.92f) : Math::pow((r + 0.055f) * (1.0f / (1.0f + 0.055f)), 2.4f),
            g < 0.04045f ? g * (1.0f / 12.92f) : Math::pow((g + 0.055f) * (1.0f / (1.0f + 0.055f)), 2.4f),
            b < 0.04045f ? b * (1.0f / 12.92f) : Math::pow((b + 0.055f) * (1.0f / (1.0f + 0.055f)), 2.4f),
            a
        );
    }

    [[nodiscard]] Color linear_to_srgb() const noexcept {
        return Color(
            r < 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * Math::pow(r, 1.0f / 2.4f) - 0.055f,
            g < 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * Math::pow(g, 1.0f / 2.4f) - 0.055f,
            b < 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * Math::pow(b, 1.0f / 2.4f) - 0.055f,
            a
        );
    }

    [[nodiscard]] constexpr Color clamp(const Color &p_min = Color(0.0f, 0.0f, 0.0f, 0.0f), const Color &p_max = Color(1.0f, 1.0f, 1.0f, 1.0f)) const noexcept {
        return Color(
            std::clamp(r, p_min.r, p_max.r),
            std::clamp(g, p_min.g, p_max.g),
            std::clamp(b, p_min.b, p_max.b),
            std::clamp(a, p_min.a, p_max.a)
        );
    }

    constexpr void invert() noexcept {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
    }

    [[nodiscard]] constexpr Color inverted() const noexcept {
        Color c = *this;
        c.invert();
        return c;
    }

    [[nodiscard]] constexpr bool is_equal_approx(const Color &p_color) const noexcept {
        return Math::is_equal_approx(r, p_color.r) &&
               Math::is_equal_approx(g, p_color.g) &&
               Math::is_equal_approx(b, p_color.b) &&
               Math::is_equal_approx(a, p_color.a);
    }

    [[nodiscard]] constexpr bool is_same(const Color &p_color) const noexcept {
        return Math::is_same(r, p_color.r) &&
               Math::is_same(g, p_color.g) &&
               Math::is_same(b, p_color.b) &&
               Math::is_same(a, p_color.a);
    }

    [[nodiscard]] static constexpr bool html_is_valid(std::string_view p_color) noexcept {
        if (p_color.empty()) return false;
        usize current_pos = (p_color[0] == '#') ? 1 : 0;
        usize num_of_digits = p_color.length() - current_pos;
        if (!(num_of_digits == 3 || num_of_digits == 4 || num_of_digits == 6 || num_of_digits == 8)) {
            return false;
        }
        for (usize i = current_pos; i < p_color.length(); ++i) {
            char c = p_color[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] static constexpr Color html(std::string_view p_color) noexcept {
        if (p_color.empty()) return Color();
        usize current_pos = (p_color[0] == '#') ? 1 : 0;
        usize num_of_digits = p_color.length() - current_pos;

        auto parse_digit = [](char c) constexpr -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return 0;
        };

        float r_val = 0.0f;
        float g_val = 0.0f;
        float b_val = 0.0f;
        float a_val = 1.0f;

        if (num_of_digits == 3) {
            r_val = (float)parse_digit(p_color[current_pos]) / 15.0f;
            g_val = (float)parse_digit(p_color[current_pos + 1]) / 15.0f;
            b_val = (float)parse_digit(p_color[current_pos + 2]) / 15.0f;
        } else if (num_of_digits == 4) {
            r_val = (float)parse_digit(p_color[current_pos]) / 15.0f;
            g_val = (float)parse_digit(p_color[current_pos + 1]) / 15.0f;
            b_val = (float)parse_digit(p_color[current_pos + 2]) / 15.0f;
            a_val = (float)parse_digit(p_color[current_pos + 3]) / 15.0f;
        } else if (num_of_digits == 6) {
            r_val = (float)(parse_digit(p_color[current_pos]) * 16 + parse_digit(p_color[current_pos + 1])) / 255.0f;
            g_val = (float)(parse_digit(p_color[current_pos + 2]) * 16 + parse_digit(p_color[current_pos + 3])) / 255.0f;
            b_val = (float)(parse_digit(p_color[current_pos + 4]) * 16 + parse_digit(p_color[current_pos + 5])) / 255.0f;
        } else if (num_of_digits == 8) {
            r_val = (float)(parse_digit(p_color[current_pos]) * 16 + parse_digit(p_color[current_pos + 1])) / 255.0f;
            g_val = (float)(parse_digit(p_color[current_pos + 2]) * 16 + parse_digit(p_color[current_pos + 3])) / 255.0f;
            b_val = (float)(parse_digit(p_color[current_pos + 4]) * 16 + parse_digit(p_color[current_pos + 5])) / 255.0f;
            a_val = (float)(parse_digit(p_color[current_pos + 6]) * 16 + parse_digit(p_color[current_pos + 7])) / 255.0f;
        }
        return Color(r_val, g_val, b_val, a_val);
    }

    [[nodiscard]] static constexpr Color from_string(std::string_view p_str, const Color &p_default) noexcept {
        if (html_is_valid(p_str)) {
            return html(p_str);
        }
        return p_default;
    }

    [[nodiscard]] static constexpr Color from_string(std::string_view p_str) noexcept {
        if (html_is_valid(p_str)) {
            return html(p_str);
        }
        return Color();
    }

    [[nodiscard]] std::array<char, 10> to_html_buffer(bool p_alpha = true) const noexcept {
        auto hex_char = [](u8 v) -> char {
            return "0123456789abcdef"[v & 0xf];
        };
        auto append_hex = [&](float val, char *dst) {
            int v = (int)Math::round(val * 255.0f);
            v = std::clamp(v, 0, 255);
            dst[0] = hex_char((u8)(v >> 4));
            dst[1] = hex_char((u8)(v & 0xf));
        };

        std::array<char, 10> res{};
        res[0] = '#';
        append_hex(r, &res[1]);
        append_hex(g, &res[3]);
        append_hex(b, &res[5]);
        if (p_alpha) {
            append_hex(a, &res[7]);
            res[9] = '\0';
        } else {
            res[7] = '\0';
        }
        return res;
    }

    constexpr bool operator==(const Color &p_color) const noexcept {
        return (r == p_color.r && g == p_color.g && b == p_color.b && a == p_color.a);
    }

    constexpr Color operator+(const Color &p_color) const noexcept {
        return Color(r + p_color.r, g + p_color.g, b + p_color.b, a + p_color.a);
    }

    constexpr void operator+=(const Color &p_color) noexcept {
        r += p_color.r;
        g += p_color.g;
        b += p_color.b;
        a += p_color.a;
    }

    constexpr Color operator-() const noexcept {
        return Color(1.0f - r, 1.0f - g, 1.0f - b, 1.0f - a);
    }

    constexpr Color operator-(const Color &p_color) const noexcept {
        return Color(r - p_color.r, g - p_color.g, b - p_color.b, a - p_color.a);
    }

    constexpr void operator-=(const Color &p_color) noexcept {
        r -= p_color.r;
        g -= p_color.g;
        b -= p_color.b;
        a -= p_color.a;
    }

    constexpr Color operator*(const Color &p_color) const noexcept {
        return Color(r * p_color.r, g * p_color.g, b * p_color.b, a * p_color.a);
    }

    constexpr Color operator*(float p_scalar) const noexcept {
        return Color(r * p_scalar, g * p_scalar, b * p_scalar, a * p_scalar);
    }

    constexpr void operator*=(const Color &p_color) noexcept {
        r *= p_color.r;
        g *= p_color.g;
        b *= p_color.b;
        a *= p_color.a;
    }

    constexpr void operator*=(float p_scalar) noexcept {
        r *= p_scalar;
        g *= p_scalar;
        b *= p_scalar;
        a *= p_scalar;
    }

    constexpr Color operator/(const Color &p_color) const noexcept {
        assert(p_color.r != 0.0f && p_color.g != 0.0f && p_color.b != 0.0f && p_color.a != 0.0f);
        return Color(r / p_color.r, g / p_color.g, b / p_color.b, a / p_color.a);
    }

    constexpr Color operator/(float p_scalar) const noexcept {
        assert(p_scalar != 0.0f);
        return Color(r / p_scalar, g / p_scalar, b / p_scalar, a / p_scalar);
    }

    constexpr void operator/=(const Color &p_color) noexcept {
        assert(p_color.r != 0.0f && p_color.g != 0.0f && p_color.b != 0.0f && p_color.a != 0.0f);
        r /= p_color.r;
        g /= p_color.g;
        b /= p_color.b;
        a /= p_color.a;
    }

    constexpr void operator/=(float p_scalar) noexcept {
        assert(p_scalar != 0.0f);
        r /= p_scalar;
        g /= p_scalar;
        b /= p_scalar;
        a /= p_scalar;
    }

    constexpr bool operator<(const Color &p_color) const noexcept {
        if (r == p_color.r) {
            if (g == p_color.g) {
                if (b == p_color.b) {
                    return (a < p_color.a);
                }
                return (b < p_color.b);
            }
            return g < p_color.g;
        }
        return r < p_color.r;
    }
};

[[nodiscard]] constexpr Color operator*(float p_scalar, const Color &p_color) noexcept {
    return p_color * p_scalar;
}

} // namespace Beyota::Math

template <>
struct std::formatter<Beyota::Math::Color> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Beyota::Math::Color &c, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {}, {})", c.r, c.g, c.b, c.a);
    }
};
