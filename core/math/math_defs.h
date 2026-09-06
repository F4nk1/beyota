/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  math_defs.h                                                           */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>

namespace Beyota::Math {

#if defined(BEYOTA_REAL_IS_DOUBLE)
using real_t = f64;
#else
using real_t = f32;
#endif

template <typename T = real_t>
inline constexpr T PI = std::numbers::pi_v<T>;

template <typename T = real_t>
inline constexpr T TWO_PI = (T)2 * std::numbers::pi_v<T>;

template <typename T = real_t>
inline constexpr T TAU = TWO_PI<T>;

template <typename T = real_t>
inline constexpr T HALF_PI = (T)0.5 * std::numbers::pi_v<T>;

template <typename T = real_t>
inline constexpr T E = std::numbers::e_v<T>;

template <typename T = real_t>
inline constexpr T SQRT2 = std::numbers::sqrt2_v<T>;

template <typename T = real_t>
inline constexpr T SQRT3 = (T)1.732050807568877293527446341505872366;

template <typename T = real_t>
inline constexpr T INV_SQRT2 = (T)1 / std::numbers::sqrt2_v<T>;

template <typename T = real_t>
inline constexpr T LN2 = std::numbers::ln2_v<T>;

template <typename T = real_t>
inline constexpr T INF = std::numeric_limits<T>::infinity();

template <typename T = real_t>
inline constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

template <typename T = real_t>
inline constexpr T CMP_EPSILON = (sizeof(T) == 8) ? (T)1e-12 : (T)1e-5;

template <typename T = real_t>
inline constexpr T CMP_EPSILON2 = CMP_EPSILON<T> * CMP_EPSILON<T>;

template <typename T = real_t>
inline constexpr T UNIT_EPSILON = (sizeof(T) == 8) ? (T)1e-10 : (T)0.001;

template <typename T = real_t>
inline constexpr T CMP_NORMALIZE_TOLERANCE = (sizeof(T) == 8) ? (T)1e-12 : (T)1e-6;

template <typename T = real_t>
inline constexpr T CMP_POINT_IN_PLANE_EPSILON = (sizeof(T) == 8) ? (T)1e-10 : (T)1e-5;

enum class EulerOrder : u8 {
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX
};

enum class ClockDirection : u8 {
    Clockwise,
    CounterClockwise
};

enum class Axis : u8 {
    X,
    Y,
    Z,
    W
};

enum class Side : u8 {
    Left,
    Top,
    Right,
    Bottom
};

} // namespace Beyota::Math
