/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  version.h                                                     */
/**************************************************************************/

#pragma once

#include "core/primitives.h"
#include <string_view>
#include <compare>

// Archivo generado automáticamente por CMake/Meson con los números base
// (BEYOTA_VERSION_MAJOR, BEYOTA_VERSION_MINOR, BEYOTA_VERSION_PATCH, etc.)
#include "core/version_generated.h" 

namespace Beyota {

struct Version {
    u32 major{0};
    u32 minor{0};
    u32 patch{0};

    // C++20 genera automáticamente ==, !=, <, <=, >, >=
    constexpr auto operator<=>(const Version&) const = default;

    // Representación empaquetada si la necesitas para serialización binaria
    [[nodiscard]] constexpr u32 to_hex() const noexcept {
        return (major << 16) | (minor << 8) | patch;
    }
};

struct EngineInfo {
    static constexpr std::string_view NAME = "Beyota Engine";
    static constexpr Version VERSION{
        BEYOTA_VERSION_MAJOR,
        BEYOTA_VERSION_MINOR,
        BEYOTA_VERSION_PATCH
    };
    static constexpr std::string_view STATUS = BEYOTA_VERSION_STATUS; // "dev", "alpha", "rc", "stable"

    // Git metadata (generados en un .cpp por CMake para no invalidar cachés de compilación)
    static const char* get_git_hash() noexcept;
    static u64         get_build_timestamp() noexcept;

    // Helpers informativos
    static constexpr bool is_debug_build() noexcept {
#if defined(BEYOTA_DEBUG)
        return true;
#else
        return false;
#endif
    }

    static constexpr bool is_double_precision() noexcept {
#if defined(BEYOTA_REAL_IS_DOUBLE)
        return true;
#else
        return false;
#endif
    }
};

} // namespace Beyota