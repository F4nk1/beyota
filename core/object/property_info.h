/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  property_info.h                                                       */
/**************************************************************************/

#pragma once

#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/type_primitives.h"
#include "core/variant/type_info.h"

#include <vector>

namespace Beyota {

class Variant;

enum class PropertyHint : u32 {
    NONE = 0,
    RANGE,
    ENUM,
    EXP_EASING,
    LENGTH,
    KEY_ACCEL,
    FLAGS,
    LAYERS_2D_RENDER,
    LAYERS_2D_PHYSICS,
    LAYERS_3D_RENDER,
    LAYERS_3D_PHYSICS,
    FILE,
    DIR,
    GLOBAL_FILE,
    GLOBAL_DIR,
    RESOURCE_TYPE,
    MULTILINE_TEXT,
    EXPRESSION,
    PLACEHOLDER_TEXT,
    COLOR_NO_ALPHA,
    OBJECT_ID,
    TYPE_STRING,
    ARRAY_TYPE,
    DICTIONARY_TYPE,
    MAX
};

struct PropertyUsage {
    static constexpr u32 NONE = 0;
    static constexpr u32 STORAGE = 1 << 1;
    static constexpr u32 EDITOR = 1 << 2;
    static constexpr u32 INTERNAL = 1 << 3;
    static constexpr u32 DEFAULT = STORAGE | EDITOR;
};

struct PropertyInfo {
    VariantType type{VariantType::NIL};
    String name;
    StringName class_name;
    PropertyHint hint{PropertyHint::NONE};
    String hint_string;
    u32 usage{PropertyUsage::DEFAULT};

    PropertyInfo() = default;

    PropertyInfo(VariantType p_type, const String &p_name,
                 PropertyHint p_hint = PropertyHint::NONE,
                 const String &p_hint_string = String(),
                 u32 p_usage = PropertyUsage::DEFAULT,
                 const StringName &p_class_name = StringName())
        : type(p_type), name(p_name), class_name(p_class_name),
          hint(p_hint), hint_string(p_hint_string), usage(p_usage) {}

    PropertyInfo(const StringName &p_class_name)
        : type(VariantType::OBJECT), class_name(p_class_name) {}

    bool operator==(const PropertyInfo &p_other) const noexcept {
        return type == p_other.type && name == p_other.name &&
               class_name == p_other.class_name && hint == p_other.hint &&
               hint_string == p_other.hint_string && usage == p_other.usage;
    }
};

struct MethodInfo {
    StringName name;
    PropertyInfo return_val;
    std::vector<PropertyInfo> arguments;
    u32 flags{0};

    MethodInfo() = default;
    explicit MethodInfo(const StringName &p_name) : name(p_name) {}
    MethodInfo(const StringName &p_name, const PropertyInfo &p_return_val)
        : name(p_name), return_val(p_return_val) {}
};

} // namespace Beyota
