/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  math_funcs.h                                                          */
/**************************************************************************/

#pragma once

#include "core/math/math_defs.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace Beyota::Math {

template <typename T>
[[nodiscard]] constexpr bool is_nan(T val) noexcept {
    return val != val;
}

template <typename T>
[[nodiscard]] constexpr bool is_inf(T val) noexcept {
    return val == INF<T> || val == -INF<T>;
}

template <typename T>
[[nodiscard]] constexpr bool is_finite(T val) noexcept {
    return !is_nan(val) && !is_inf(val);
}

template <typename T = real_t>
[[nodiscard]] constexpr bool is_zero_approx(T val) noexcept {
    return (val < (T)0 ? -val : val) < CMP_EPSILON<T>;
}

template <typename T = real_t>
[[nodiscard]] constexpr bool is_equal_approx(T a, T b, T tolerance) noexcept {
    if (a == b) return true;
    const T diff = a - b;
    return (diff < (T)0 ? -diff : diff) < tolerance;
}

template <typename T = real_t>
[[nodiscard]] constexpr bool is_equal_approx(T a, T b) noexcept {
    if (a == b) return true;
    T tol = CMP_EPSILON<T> * (a < (T)0 ? -a : a);
    if (tol < CMP_EPSILON<T>) tol = CMP_EPSILON<T>;
    const T diff = a - b;
    return (diff < (T)0 ? -diff : diff) < tol;
}

template <typename T = real_t>
[[nodiscard]] constexpr bool is_same(T a, T b) noexcept {
    return (a == b) || (is_nan(a) && is_nan(b));
}

[[nodiscard]] constexpr i32 division_no_overflow(i32 num, i32 den) noexcept {
    assert(den != 0);
    if (den == -1) {
        return (i32)(-(u32)num);
    }
    return num / den;
}

[[nodiscard]] constexpr i64 division_no_overflow(i64 num, i64 den) noexcept {
    assert(den != 0);
    if (den == -1) {
        return (i64)(-(u64)num);
    }
    return num / den;
}

[[nodiscard]] constexpr i32 modulo_no_overflow(i32 num, i32 den) noexcept {
    assert(den != 0);
    if (den == -1) {
        return 0;
    }
    return num % den;
}

[[nodiscard]] constexpr i64 modulo_no_overflow(i64 num, i64 den) noexcept {
    assert(den != 0);
    if (den == -1) {
        return 0;
    }
    return num % den;
}

[[nodiscard]] constexpr i32 division_round_up(i32 num, i32 den) noexcept {
    assert(den != 0);
    i32 offset = (num < 0 && den < 0) ? 1 : -1;
    return (num + den + offset) / den;
}

[[nodiscard]] constexpr u32 division_round_up(u32 num, u32 den) noexcept {
    assert(den != 0);
    return (num + den - 1) / den;
}

[[nodiscard]] constexpr i64 division_round_up(i64 num, i64 den) noexcept {
    assert(den != 0);
    i64 offset = (num < 0 && den < 0) ? 1 : -1;
    return (num + den + offset) / den;
}

[[nodiscard]] constexpr u64 division_round_up(u64 num, u64 den) noexcept {
    assert(den != 0);
    return (num + den - 1) / den;
}

template <typename T>
[[nodiscard]] constexpr T abs(T val) noexcept {
    return val < (T)0 ? -val : val;
}

template <typename T>
[[nodiscard]] constexpr T sign(T val) noexcept {
    return (val < (T)0) ? (T)-1 : ((val > (T)0) ? (T)1 : (T)0);
}

template <typename T = real_t>
[[nodiscard]] constexpr T deg_to_rad(T deg) noexcept {
    return deg * (PI<T> / (T)180);
}

template <typename T = real_t>
[[nodiscard]] constexpr T rad_to_deg(T rad) noexcept {
    return rad * ((T)180 / PI<T>);
}

[[nodiscard]] inline f32 sin(f32 x) noexcept { return std::sin(x); }
[[nodiscard]] inline f64 sin(f64 x) noexcept { return std::sin(x); }
[[nodiscard]] inline f32 cos(f32 x) noexcept { return std::cos(x); }
[[nodiscard]] inline f64 cos(f64 x) noexcept { return std::cos(x); }
[[nodiscard]] inline f32 tan(f32 x) noexcept { return std::tan(x); }
[[nodiscard]] inline f64 tan(f64 x) noexcept { return std::tan(x); }

[[nodiscard]] inline f32 sinh(f32 x) noexcept { return std::sinh(x); }
[[nodiscard]] inline f64 sinh(f64 x) noexcept { return std::sinh(x); }
[[nodiscard]] inline f32 cosh(f32 x) noexcept { return std::cosh(x); }
[[nodiscard]] inline f64 cosh(f64 x) noexcept { return std::cosh(x); }
[[nodiscard]] inline f32 tanh(f32 x) noexcept { return std::tanh(x); }
[[nodiscard]] inline f64 tanh(f64 x) noexcept { return std::tanh(x); }

[[nodiscard]] inline f32 sinc(f32 x) noexcept { return x == 0.0f ? 1.0f : std::sin(x) / x; }
[[nodiscard]] inline f64 sinc(f64 x) noexcept { return x == 0.0 ? 1.0 : std::sin(x) / x; }
[[nodiscard]] inline f32 sincn(f32 x) noexcept { return sinc(PI<f32> * x); }
[[nodiscard]] inline f64 sincn(f64 x) noexcept { return sinc(PI<f64> * x); }

[[nodiscard]] inline f32 asin(f32 x) noexcept {
    return x < -1.0f ? (-PI<f32> * 0.5f) : (x > 1.0f ? (PI<f32> * 0.5f) : std::asin(x));
}
[[nodiscard]] inline f64 asin(f64 x) noexcept {
    return x < -1.0 ? (-PI<f64> * 0.5) : (x > 1.0 ? (PI<f64> * 0.5) : std::asin(x));
}
[[nodiscard]] inline f32 acos(f32 x) noexcept {
    return x < -1.0f ? PI<f32> : (x > 1.0f ? 0.0f : std::acos(x));
}
[[nodiscard]] inline f64 acos(f64 x) noexcept {
    return x < -1.0 ? PI<f64> : (x > 1.0 ? 0.0 : std::acos(x));
}
[[nodiscard]] inline f32 atan(f32 x) noexcept { return std::atan(x); }
[[nodiscard]] inline f64 atan(f64 x) noexcept { return std::atan(x); }
[[nodiscard]] inline f32 atan2(f32 y, f32 x) noexcept { return std::atan2(y, x); }
[[nodiscard]] inline f64 atan2(f64 y, f64 x) noexcept { return std::atan2(y, x); }

[[nodiscard]] inline f32 asinh(f32 x) noexcept { return std::asinh(x); }
[[nodiscard]] inline f64 asinh(f64 x) noexcept { return std::asinh(x); }
[[nodiscard]] inline f32 acosh(f32 x) noexcept { return x < 1.0f ? 0.0f : std::acosh(x); }
[[nodiscard]] inline f64 acosh(f64 x) noexcept { return x < 1.0 ? 0.0 : std::acosh(x); }
[[nodiscard]] inline f32 atanh(f32 x) noexcept {
    return x <= -1.0f ? -INF<f32> : (x >= 1.0f ? INF<f32> : std::atanh(x));
}
[[nodiscard]] inline f64 atanh(f64 x) noexcept {
    return x <= -1.0 ? -INF<f64> : (x >= 1.0 ? INF<f64> : std::atanh(x));
}

[[nodiscard]] inline f32 sqrt(f32 x) noexcept { return std::sqrt(x); }
[[nodiscard]] inline f64 sqrt(f64 x) noexcept { return std::sqrt(x); }
[[nodiscard]] inline f32 cbrt(f32 x) noexcept { return std::cbrt(x); }
[[nodiscard]] inline f64 cbrt(f64 x) noexcept { return std::cbrt(x); }
[[nodiscard]] inline f32 hypot(f32 x, f32 y) noexcept { return std::hypot(x, y); }
[[nodiscard]] inline f64 hypot(f64 x, f64 y) noexcept { return std::hypot(x, y); }

[[nodiscard]] inline f32 fmod(f32 x, f32 y) noexcept { return std::fmod(x, y); }
[[nodiscard]] inline f64 fmod(f64 x, f64 y) noexcept { return std::fmod(x, y); }
[[nodiscard]] inline f32 floor(f32 x) noexcept { return std::floor(x); }
[[nodiscard]] inline f64 floor(f64 x) noexcept { return std::floor(x); }
[[nodiscard]] inline f32 ceil(f32 x) noexcept { return std::ceil(x); }
[[nodiscard]] inline f64 ceil(f64 x) noexcept { return std::ceil(x); }
[[nodiscard]] inline f32 round(f32 x) noexcept { return std::round(x); }
[[nodiscard]] inline f64 round(f64 x) noexcept { return std::round(x); }
[[nodiscard]] inline f32 trunc(f32 x) noexcept { return std::trunc(x); }
[[nodiscard]] inline f64 trunc(f64 x) noexcept { return std::trunc(x); }
[[nodiscard]] inline f32 fract(f32 x) noexcept { return x - std::floor(x); }
[[nodiscard]] inline f64 fract(f64 x) noexcept { return x - std::floor(x); }

[[nodiscard]] inline f32 pow(f32 x, f32 y) noexcept { return std::pow(x, y); }
[[nodiscard]] inline f64 pow(f64 x, f64 y) noexcept { return std::pow(x, y); }
[[nodiscard]] inline f32 exp(f32 x) noexcept { return std::exp(x); }
[[nodiscard]] inline f64 exp(f64 x) noexcept { return std::exp(x); }
[[nodiscard]] inline f32 log(f32 x) noexcept { return std::log(x); }
[[nodiscard]] inline f64 log(f64 x) noexcept { return std::log(x); }
[[nodiscard]] inline f32 log2(f32 x) noexcept { return std::log2(x); }
[[nodiscard]] inline f64 log2(f64 x) noexcept { return std::log2(x); }

template <typename T>
[[nodiscard]] inline T fposmod(T x, T y) noexcept {
    T val = std::fmod(x, y);
    if (((val < (T)0) && (y > (T)0)) || ((val > (T)0) && (y < (T)0))) {
        val += y;
    }
    return val + (T)0.0;
}

template <typename T>
[[nodiscard]] inline T fposmodp(T x, T y) noexcept {
    T val = std::fmod(x, y);
    if (val < (T)0) {
        val += y;
    }
    return val + (T)0.0;
}

[[nodiscard]] constexpr i64 posmod(i64 x, i64 y) noexcept {
    assert(y != 0);
    i64 val = x % y;
    if (((val < 0) && (y > 0)) || ((val > 0) && (y < 0))) {
        val += y;
    }
    return val;
}

[[nodiscard]] constexpr i32 posmod(i32 x, i32 y) noexcept {
    assert(y != 0);
    i32 val = x % y;
    if (((val < 0) && (y > 0)) || ((val > 0) && (y < 0))) {
        val += y;
    }
    return val;
}

template <typename T, typename W = T>
[[nodiscard]] constexpr T lerp(T from, T to, W weight) noexcept {
    return from + (to - from) * weight;
}

template <typename T>
[[nodiscard]] inline T angle_difference(T from, T to) noexcept {
    T diff = std::fmod(to - from, TAU<T>);
    return std::fmod((T)2 * diff, TAU<T>) - diff;
}

template <typename T, typename W = T>
[[nodiscard]] inline T lerp_angle(T from, T to, W weight) noexcept {
    return from + angle_difference(from, to) * weight;
}

template <typename T>
[[nodiscard]] constexpr T inverse_lerp(T from, T to, T val) noexcept {
    return (val - from) / (to - from);
}

template <typename T>
[[nodiscard]] constexpr T remap(T val, T in_start, T in_stop, T out_start, T out_stop) noexcept {
    return lerp(out_start, out_stop, inverse_lerp(in_start, in_stop, val));
}

template <typename T>
[[nodiscard]] constexpr T cubic_interpolate(T from, T to, T pre, T post, T weight) noexcept {
    return (T)0.5 *
        (((from * (T)2.0) +
          (-pre + to) * weight +
          ((T)2.0 * pre - (T)5.0 * from + (T)4.0 * to - post) * (weight * weight) +
          (-pre + (T)3.0 * from - (T)3.0 * to + post) * (weight * weight * weight)));
}

template <typename T>
[[nodiscard]] inline T cubic_interpolate_angle(T from, T to, T pre, T post, T weight) noexcept {
    T from_rot = std::fmod(from, TAU<T>);
    T pre_diff = std::fmod(pre - from_rot, TAU<T>);
    T pre_rot = from_rot + std::fmod((T)2.0 * pre_diff, TAU<T>) - pre_diff;
    T to_diff = std::fmod(to - from_rot, TAU<T>);
    T to_rot = from_rot + std::fmod((T)2.0 * to_diff, TAU<T>) - to_diff;
    T post_diff = std::fmod(post - to_rot, TAU<T>);
    T post_rot = to_rot + std::fmod((T)2.0 * post_diff, TAU<T>) - post_diff;
    return cubic_interpolate(from_rot, to_rot, pre_rot, post_rot, weight);
}

template <typename T>
[[nodiscard]] constexpr T cubic_interpolate_in_time(T from, T to, T pre, T post, T weight,
                                                     T to_t, T pre_t, T post_t) noexcept {
    T t = lerp((T)0.0, to_t, weight);
    T a1 = lerp(pre, from, pre_t == (T)0 ? (T)0.0 : (t - pre_t) / -pre_t);
    T a2 = lerp(from, to, to_t == (T)0 ? (T)0.5 : t / to_t);
    T a3 = lerp(to, post, post_t - to_t == (T)0 ? (T)1.0 : (t - to_t) / (post_t - to_t));
    T b1 = lerp(a1, a2, to_t - pre_t == (T)0 ? (T)0.0 : (t - pre_t) / (to_t - pre_t));
    T b2 = lerp(a2, a3, post_t == (T)0 ? (T)1.0 : t / post_t);
    return lerp(b1, b2, to_t == (T)0 ? (T)0.5 : t / to_t);
}

template <typename T>
[[nodiscard]] inline T cubic_interpolate_angle_in_time(T from, T to, T pre, T post, T weight,
                                                        T to_t, T pre_t, T post_t) noexcept {
    T from_rot = std::fmod(from, TAU<T>);
    T pre_diff = std::fmod(pre - from_rot, TAU<T>);
    T pre_rot = from_rot + std::fmod((T)2.0 * pre_diff, TAU<T>) - pre_diff;
    T to_diff = std::fmod(to - from_rot, TAU<T>);
    T to_rot = from_rot + std::fmod((T)2.0 * to_diff, TAU<T>) - to_diff;
    T post_diff = std::fmod(post - to_rot, TAU<T>);
    T post_rot = to_rot + std::fmod((T)2.0 * post_diff, TAU<T>) - post_diff;
    return cubic_interpolate_in_time(from_rot, to_rot, pre_rot, post_rot, weight, to_t, pre_t, post_t);
}

template <typename T>
[[nodiscard]] constexpr T bezier_interpolate(T start, T control_1, T control_2, T end, T t) noexcept {
    T omt = (T)1.0 - t;
    T omt2 = omt * omt;
    T omt3 = omt2 * omt;
    T t2 = t * t;
    T t3 = t2 * t;
    return start * omt3 + control_1 * omt2 * t * (T)3.0 + control_2 * omt * t2 * (T)3.0 + end * t3;
}

template <typename T>
[[nodiscard]] constexpr T bezier_derivative(T start, T control_1, T control_2, T end, T t) noexcept {
    T omt = (T)1.0 - t;
    T omt2 = omt * omt;
    T t2 = t * t;
    return (control_1 - start) * (T)3.0 * omt2 + (control_2 - control_1) * (T)6.0 * omt * t + (end - control_2) * (T)3.0 * t2;
}

template <typename T>
[[nodiscard]] constexpr T smoothstep(T from, T to, T s) noexcept {
    if (is_equal_approx(from, to)) {
        return from <= to ? (s <= from ? (T)0.0 : (T)1.0) : (s <= to ? (T)1.0 : (T)0.0);
    }
    T t = std::clamp((s - from) / (to - from), (T)0.0, (T)1.0);
    return t * t * ((T)3.0 - (T)2.0 * t);
}

template <typename T>
[[nodiscard]] constexpr T move_toward(T from, T to, T delta) noexcept {
    return abs(to - from) <= delta ? to : from + sign(to - from) * delta;
}

template <typename T>
[[nodiscard]] inline T rotate_toward(T from, T to, T delta) noexcept {
    T diff = angle_difference(from, to);
    T abs_diff = abs(diff);
    return from + std::clamp(delta, abs_diff - PI<T>, abs_diff) * (diff >= (T)0.0 ? (T)1.0 : (T)-1.0);
}

template <typename T>
[[nodiscard]] inline T pingpong(T val, T length) noexcept {
    return (length != (T)0.0) ? abs(fract((val - length) / (length * (T)2.0)) * length * (T)2.0 - length) : (T)0.0;
}

template <typename T>
[[nodiscard]] inline T wrapf(T val, T min_val, T max_val) noexcept {
    T range = max_val - min_val;
    if (is_zero_approx(range)) {
        return min_val;
    }
    T result = val - (range * std::floor((val - min_val) / range));
    if (is_equal_approx(result, max_val)) {
        return min_val;
    }
    return result;
}

[[nodiscard]] constexpr i64 wrapi(i64 val, i64 min_val, i64 max_val) noexcept {
    i64 range = max_val - min_val;
    return range == 0 ? min_val : min_val + ((((val - min_val) % range) + range) % range);
}

template <typename T>
[[nodiscard]] inline T snapped(T val, T step) noexcept {
    if (step != (T)0) {
        val = std::floor(val / step + (T)0.5) * step;
    }
    return val;
}

template <typename T>
[[nodiscard]] inline T ease(T x, T c) noexcept {
    if (x < (T)0) {
        x = (T)0;
    } else if (x > (T)1.0) {
        x = (T)1.0;
    }
    if (c > (T)0) {
        if (c < (T)1.0) {
            return (T)1.0 - std::pow((T)1.0 - x, (T)1.0 / c);
        } else {
            return std::pow(x, c);
        }
    } else if (c < (T)0) {
        if (x < (T)0.5) {
            return std::pow(x * (T)2.0, -c) * (T)0.5;
        } else {
            return ((T)1.0 - std::pow((T)1.0 - (x - (T)0.5) * (T)2.0, -c)) * (T)0.5 + (T)0.5;
        }
    }
    return (T)0;
}

[[nodiscard]] inline int step_decimals(f64 step) noexcept {
    static constexpr int maxn = 10;
    static constexpr f64 sd[maxn] = {
        0.9999,
        0.09999,
        0.009999,
        0.0009999,
        0.00009999,
        0.000009999,
        0.0000009999,
        0.00000009999,
        0.000000009999,
        0.0000000009999
    };
    f64 val = abs(step);
    f64 decs = val - (i64)val;
    for (int i = 0; i < maxn; i++) {
        if (decs >= sd[i]) {
            return i;
        }
    }
    return 0;
}

[[nodiscard]] inline f32 linear_to_db(f32 linear) noexcept {
    return std::log(linear) * 8.6858896380650365530225783783321f;
}
[[nodiscard]] inline f64 linear_to_db(f64 linear) noexcept {
    return std::log(linear) * 8.6858896380650365530225783783321;
}

[[nodiscard]] inline f32 db_to_linear(f32 db) noexcept {
    return std::exp(db * 0.11512925464970228420089957273422f);
}
[[nodiscard]] inline f64 db_to_linear(f64 db) noexcept {
    return std::exp(db * 0.11512925464970228420089957273422);
}

[[nodiscard]] constexpr u32 halfbits_to_floatbits(u16 half) noexcept {
    u16 h_exp = (half & 0x7c00u);
    u32 f_sgn = ((u32)half & 0x8000u) << 16;
    switch (h_exp) {
        case 0x0000u: {
            u16 h_sig = (half & 0x03ffu);
            if (h_sig == 0) {
                return f_sgn;
            }
            h_sig <<= 1;
            while ((h_sig & 0x0400u) == 0) {
                h_sig <<= 1;
                h_exp++;
            }
            u32 f_exp = ((u32)(127 - 15 - h_exp)) << 23;
            u32 f_sig = ((u32)(h_sig & 0x03ffu)) << 13;
            return f_sgn + f_exp + f_sig;
        }
        case 0x7c00u:
            return f_sgn + 0x7f800000u + (((u32)(half & 0x03ffu)) << 13);
        default:
            return f_sgn + (((u32)(half & 0x7fffu) + 0x1c000u) << 13);
    }
}

[[nodiscard]] inline f32 half_to_float(u16 half) noexcept {
    union {
        u32 u;
        f32 f;
    } conv;
    conv.u = halfbits_to_floatbits(half);
    return conv.f;
}

[[nodiscard]] inline u16 make_half_float(f32 value) noexcept {
    union {
        f32 f;
        u32 u;
    } conv;
    conv.f = value;
    u32 x = conv.u;
    u16 sign_bit = (u16)(x >> 31);
    u32 mantissa = x & ((1 << 23) - 1);
    u32 exponent = x & (0xFF << 23);
    if (exponent >= 0x47800000) {
        if (mantissa && (exponent == (0xFF << 23))) {
            mantissa = (1 << 23) - 1;
        } else {
            mantissa = 0;
        }
        return (sign_bit << 15) | (0x1F << 10) | (u16)(mantissa >> 13);
    } else if (exponent <= 0x38000000) {
        return 0;
    }
    return (sign_bit << 15) | (u16)((exponent - 0x38000000) >> 13) | (u16)(mantissa >> 13);
}

} // namespace Beyota::Math
