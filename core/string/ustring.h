/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  ustring.h                                                             */
/**************************************************************************/

#pragma once

#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <atomic>
#include <cassert>
#include <compare>
#include <cstring>
#include <format>
#include <string_view>
#include <vector>

namespace Beyota {

class String {
    struct CowData {
        std::atomic<u32> refcount{1};
        u32 length{0};
    };

    CowData *data_{nullptr};

    [[nodiscard]] static CowData *alloc_data(usize p_len);
    static void free_data(CowData *p_data) noexcept;
    void ref_data(const CowData *p_data) noexcept;
    void unref_data() noexcept;

    [[nodiscard]] char *data_ptr() noexcept {
        return data_ ? reinterpret_cast<char *>(data_ + 1) : nullptr;
    }

    [[nodiscard]] const char *data_ptr() const noexcept {
        return data_ ? reinterpret_cast<const char *>(data_ + 1) : nullptr;
    }

    void detach_if_shared();

public:
    constexpr String() noexcept = default;

    String(const char *p_str);
    String(std::string_view p_view);
    String(const char *p_str, usize p_len);
    String(const String &p_other) noexcept;
    String(String &&p_other) noexcept;
    ~String() noexcept;

    String &operator=(const String &p_other) noexcept;
    String &operator=(String &&p_other) noexcept;
    String &operator=(const char *p_str);
    String &operator=(std::string_view p_view);

    [[nodiscard]] usize length() const noexcept {
        return data_ ? data_->length : 0;
    }

    [[nodiscard]] usize size() const noexcept {
        return length();
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return data_ == nullptr || data_->length == 0;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        const char *p = data_ptr();
        return p ? p : "";
    }

    [[nodiscard]] const char *utf8() const noexcept {
        return c_str();
    }

    [[nodiscard]] std::string_view as_string_view() const noexcept {
        return data_ ? std::string_view(data_ptr(), data_->length) : std::string_view{};
    }

    [[nodiscard]] operator std::string_view() const noexcept {
        return as_string_view();
    }

    [[nodiscard]] char operator[](usize p_idx) const noexcept {
        assert(p_idx < length());
        return data_ptr()[p_idx];
    }

    [[nodiscard]] char &operator[](usize p_idx) {
        assert(p_idx < length());
        detach_if_shared();
        return data_ptr()[p_idx];
    }

    [[nodiscard]] u32 hash() const noexcept {
        return hash_make_uint32_t(as_string_view());
    }

    [[nodiscard]] bool operator==(const String &p_other) const noexcept {
        if (data_ == p_other.data_) {
            return true;
        }
        return as_string_view() == p_other.as_string_view();
    }

    [[nodiscard]] bool operator==(std::string_view p_other) const noexcept {
        return as_string_view() == p_other;
    }

    [[nodiscard]] bool operator==(const char *p_other) const noexcept {
        return as_string_view() == (p_other ? std::string_view(p_other) : std::string_view{});
    }

    [[nodiscard]] auto operator<=>(const String &p_other) const noexcept {
        if (data_ == p_other.data_) {
            return std::strong_ordering::equal;
        }
        return as_string_view() <=> p_other.as_string_view();
    }

    [[nodiscard]] auto operator<=>(std::string_view p_other) const noexcept {
        return as_string_view() <=> p_other;
    }

    String &operator+=(const String &p_other);
    String &operator+=(std::string_view p_other);
    String &operator+=(const char *p_other);
    String &operator+=(char p_char);

    [[nodiscard]] String operator+(const String &p_other) const;
    [[nodiscard]] String operator+(std::string_view p_other) const;
    [[nodiscard]] String operator+(const char *p_other) const;

    // Substring and Navigation
    [[nodiscard]] String substr(usize p_from, usize p_len = static_cast<usize>(-1)) const;
    [[nodiscard]] String left(i64 p_len) const;
    [[nodiscard]] String right(i64 p_len) const;

    // Search and Predicates
    [[nodiscard]] bool contains(std::string_view p_sub) const noexcept;
    [[nodiscard]] i64 find(std::string_view p_sub, usize p_from = 0) const noexcept;
    [[nodiscard]] i64 rfind(std::string_view p_sub, i64 p_from = -1) const noexcept;
    [[nodiscard]] i64 findn(std::string_view p_sub, usize p_from = 0) const noexcept;
    [[nodiscard]] i64 rfindn(std::string_view p_sub, i64 p_from = -1) const noexcept;
    [[nodiscard]] bool begins_with(std::string_view p_prefix) const noexcept;
    [[nodiscard]] bool ends_with(std::string_view p_suffix) const noexcept;

    // Case Transformations
    [[nodiscard]] String to_lower() const;
    [[nodiscard]] String to_upper() const;
    [[nodiscard]] String capitalize() const;
    [[nodiscard]] String to_camel_case() const;
    [[nodiscard]] String to_pascal_case() const;
    [[nodiscard]] String to_snake_case() const;
    [[nodiscard]] String to_kebab_case() const;
    [[nodiscard]] String camelcase_to_underscore() const;

    // Trimming and Padding
    [[nodiscard]] String strip_edges(bool p_left = true, bool p_right = true) const;
    [[nodiscard]] String strip_escapes() const;
    [[nodiscard]] String lstrip(std::string_view p_chars) const;
    [[nodiscard]] String rstrip(std::string_view p_chars) const;
    [[nodiscard]] String trim_prefix(std::string_view p_prefix) const;
    [[nodiscard]] String trim_suffix(std::string_view p_suffix) const;
    [[nodiscard]] String lpad(usize p_min_len, char p_character = ' ') const;
    [[nodiscard]] String rpad(usize p_min_len, char p_character = ' ') const;
    [[nodiscard]] String pad_zeros(usize p_digits) const;
    [[nodiscard]] String pad_decimals(usize p_digits) const;

    // Replacement, Split and Join
    [[nodiscard]] String replace(std::string_view p_what, std::string_view p_for_what) const;
    [[nodiscard]] String replacen(std::string_view p_what, std::string_view p_for_what) const;
    [[nodiscard]] std::vector<String> split(std::string_view p_delim, bool p_allow_empty = true, i32 p_maxsplit = 0) const;
    [[nodiscard]] std::vector<String> rsplit(std::string_view p_delim, bool p_allow_empty = true, i32 p_maxsplit = 0) const;
    [[nodiscard]] String join(const std::vector<String> &p_parts) const;

    // Path Operations
    [[nodiscard]] String get_base_dir() const;
    [[nodiscard]] String get_file() const;
    [[nodiscard]] String get_extension() const;
    [[nodiscard]] String get_basename() const;
    [[nodiscard]] String path_join(const String &p_file) const;
    [[nodiscard]] String simplify_path() const;
    [[nodiscard]] bool is_absolute_path() const noexcept;
    [[nodiscard]] bool is_relative_path() const noexcept;

    // Escaping
    [[nodiscard]] String c_escape() const;
    [[nodiscard]] String c_unescape() const;
    [[nodiscard]] String json_escape() const;
    [[nodiscard]] String xml_escape(bool p_escape_quotes = false) const;

    // Conversions and Validation
    [[nodiscard]] i64 to_int() const noexcept;
    [[nodiscard]] f64 to_float() const noexcept;
    [[nodiscard]] i64 hex_to_int() const noexcept;
    [[nodiscard]] i64 bin_to_int() const noexcept;
    [[nodiscard]] bool is_valid_int() const noexcept;
    [[nodiscard]] bool is_valid_float() const noexcept;
    [[nodiscard]] bool is_valid_hex_number() const noexcept;
    [[nodiscard]] bool is_valid_identifier() const noexcept;
    [[nodiscard]] bool is_valid_filename() const noexcept;

    // Static Helpers
    static String num_int64(i64 p_num);
    static String num_uint64(u64 p_num);
    static String num_real(f64 p_num);
    static String chr(char32_t p_char);
    static char32_t ord(char p_char) noexcept;

    void clear() noexcept {
        unref_data();
    }
};

[[nodiscard]] inline String operator+(std::string_view p_left, const String &p_right) {
    String res(p_left);
    res += p_right;
    return res;
}

[[nodiscard]] inline String operator+(const char *p_left, const String &p_right) {
    String res(p_left);
    res += p_right;
    return res;
}

} // namespace Beyota

template <>
struct std::formatter<Beyota::String> : std::formatter<std::string_view> {
    auto format(const Beyota::String &p_str, std::format_context &p_ctx) const {
        return std::formatter<std::string_view>::format(p_str.as_string_view(), p_ctx);
    }
};
