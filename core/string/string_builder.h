/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  string_builder.h                                                      */
/**************************************************************************/

#pragma once

#include "core/string/ustring.h"
#include "core/templates/local_vector.h"
#include "core/type_primitives.h"

namespace Beyota {

class StringBuilder {
    u32 string_length_{0};
    LocalVector<String> strings_;
    LocalVector<const char *> c_strings_;
    LocalVector<i32> appended_strings_; // -1 indicates Beyota String, >= 0 is C string length

public:
    StringBuilder() = default;

    StringBuilder &append(const String &p_string);
    StringBuilder &append(const char *p_cstring);

    StringBuilder &operator+(const String &p_string) {
        return append(p_string);
    }

    StringBuilder &operator+(const char *p_cstring) {
        return append(p_cstring);
    }

    void operator+=(const String &p_string) {
        append(p_string);
    }

    void operator+=(const char *p_cstring) {
        append(p_cstring);
    }

    [[nodiscard]] i32 num_strings_appended() const noexcept {
        return (i32)appended_strings_.size();
    }

    [[nodiscard]] u32 get_string_length() const noexcept {
        return string_length_;
    }

    [[nodiscard]] String as_string() const;

    [[nodiscard]] operator String() const {
        return as_string();
    }
};

} // namespace Beyota
