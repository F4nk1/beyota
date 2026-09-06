/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  string_builder.cpp                                                    */
/**************************************************************************/

#include "core/string/string_builder.h"

#include <cstring>
#include <string>

namespace Beyota {

StringBuilder &StringBuilder::append(const String &p_string) {
    if (p_string.is_empty()) {
        return *this;
    }

    strings_.push_back(p_string);
    appended_strings_.push_back(-1);
    string_length_ += (u32)p_string.length();

    return *this;
}

StringBuilder &StringBuilder::append(const char *p_cstring) {
    if (p_cstring == nullptr || *p_cstring == '\0') {
        return *this;
    }

    i32 len = static_cast<i32>(std::strlen(p_cstring));
    c_strings_.push_back(p_cstring);
    appended_strings_.push_back(len);
    string_length_ += (u32)len;

    return *this;
}

String StringBuilder::as_string() const {
    if (string_length_ == 0) {
        return String();
    }

    std::string buffer;
    buffer.reserve(string_length_);

    u32 godot_string_elem = 0;
    u32 c_string_elem = 0;

    for (u32 i = 0; i < appended_strings_.size(); ++i) {
        i32 str_len = appended_strings_[i];
        if (str_len == -1) {
            const String &s = strings_[godot_string_elem++];
            buffer.append(s.as_string_view());
        } else {
            const char *s = c_strings_[c_string_elem++];
            buffer.append(s, (usize)str_len);
        }
    }

    return String(buffer);
}

} // namespace Beyota
