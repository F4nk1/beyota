/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  type_primitives.h                                                     */
/**************************************************************************/

#pragma once

// 1. C++20 Standard Verification
#if defined(_MSVC_LANG)
    static_assert(_MSVC_LANG >= 202002L, "Beyota Engine requiere soporte minimo de C++20.");
#else
    static_assert(__cplusplus >= 202002L, "Beyota Engine requiere soporte minimo de C++20.");
#endif

#include <cstddef>
#include <cstdint>

// 2. Fixed-width Primitive Types
using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;

using i8   = std::int8_t;
using i16  = std::int16_t;
using i32  = std::int32_t;
using i64  = std::int64_t;

using f32  = float;
using f64  = double;

using usize = std::size_t;
using uptr  = std::uintptr_t;

// 3. Compiler & Inlining Control
#if defined(_MSC_VER)
    #define BEYOTA_FORCE_INLINE __forceinline
    #define BEYOTA_NO_INLINE    __declspec(noinline)
    #define BEYOTA_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define BEYOTA_FORCE_INLINE __attribute__((always_inline)) inline
    #define BEYOTA_NO_INLINE    __attribute__((noinline))
    #define BEYOTA_DEBUG_BREAK() __builtin_trap()
#else
    #define BEYOTA_FORCE_INLINE inline
    #define BEYOTA_NO_INLINE
    #define BEYOTA_DEBUG_BREAK()
#endif

// 4. Static Analysis Attributes
#if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(clang::lifetimebound)
        #define BEYOTA_LIFETIMEBOUND [[clang::lifetimebound]]
    #elif __has_cpp_attribute(msvc::lifetimebound)
        #define BEYOTA_LIFETIMEBOUND [[msvc::lifetimebound]]
    #else
        #define BEYOTA_LIFETIMEBOUND
    #endif
#else
    #define BEYOTA_LIFETIMEBOUND
#endif

// 5. Preprocessor Utilities
#define BEYOTA_STRINGIFY_IMPL(x) #x
#define BEYOTA_STRINGIFY(x) BEYOTA_STRINGIFY_IMPL(x)

#define BEYOTA_CONCAT_IMPL(a, b) a##b
#define BEYOTA_CONCAT(a, b) BEYOTA_CONCAT_IMPL(a, b)

#define BEYOTA_ANONYMOUS_VARIABLE(prefix) BEYOTA_CONCAT(prefix, __COUNTER__)

// 6. Diagnostics & Warning Suppression
#if defined(__clang__)
    #define BEYOTA_PRAGMA(x) _Pragma(#x)
    #define BEYOTA_WARNING_PUSH            BEYOTA_PRAGMA(clang diagnostic push)
    #define BEYOTA_WARNING_POP             BEYOTA_PRAGMA(clang diagnostic pop)
    #define BEYOTA_DISABLE_WARNING(warn)   BEYOTA_PRAGMA(clang diagnostic ignored warn)
#elif defined(__GNUC__)
    #define BEYOTA_PRAGMA(x) _Pragma(#x)
    #define BEYOTA_WARNING_PUSH            BEYOTA_PRAGMA(GCC diagnostic push)
    #define BEYOTA_WARNING_POP             BEYOTA_PRAGMA(GCC diagnostic pop)
    #define BEYOTA_DISABLE_WARNING(warn)   BEYOTA_PRAGMA(GCC diagnostic ignored warn)
#elif defined(_MSC_VER)
    #define BEYOTA_PRAGMA(x) __pragma(x)
    #define BEYOTA_WARNING_PUSH            BEYOTA_PRAGMA(warning(push))
    #define BEYOTA_WARNING_POP             BEYOTA_PRAGMA(warning(pop))
    #define BEYOTA_DISABLE_WARNING(warnNumber) BEYOTA_PRAGMA(warning(disable : warnNumber))
#else
    #define BEYOTA_WARNING_PUSH
    #define BEYOTA_WARNING_POP
    #define BEYOTA_DISABLE_WARNING(x)
#endif

