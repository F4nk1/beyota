/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  ustring.cpp                                                           */
/**************************************************************************/

#include "core/string/ustring.h"

#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <string>

namespace Beyota {

String::CowData *String::alloc_data(usize p_len) {
    if (p_len == 0) {
        return nullptr;
    }
    void *mem = ::operator new(sizeof(CowData) + p_len + 1);
    CowData *d = static_cast<CowData *>(mem);
    d->refcount.store(1, std::memory_order_relaxed);
    d->length = static_cast<u32>(p_len);
    return d;
}

void String::free_data(CowData *p_data) noexcept {
    if (p_data != nullptr) {
        ::operator delete(static_cast<void *>(p_data));
    }
}

void String::ref_data(const CowData *p_data) noexcept {
    if (p_data != nullptr) {
        const_cast<CowData *>(p_data)->refcount.fetch_add(1, std::memory_order_relaxed);
    }
}

void String::unref_data() noexcept {
    if (data_ != nullptr) {
        if (data_->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            free_data(data_);
        }
        data_ = nullptr;
    }
}

void String::detach_if_shared() {
    if (data_ != nullptr && data_->refcount.load(std::memory_order_relaxed) > 1) {
        CowData *new_data = alloc_data(data_->length);
        std::memcpy(reinterpret_cast<char *>(new_data + 1), data_ptr(), data_->length + 1);
        unref_data();
        data_ = new_data;
    }
}

String::String(const char *p_str) {
    if (p_str != nullptr) {
        usize len = std::strlen(p_str);
        if (len > 0) {
            data_ = alloc_data(len);
            std::memcpy(data_ptr(), p_str, len);
            data_ptr()[len] = '\0';
        }
    }
}

String::String(std::string_view p_view) {
    if (!p_view.empty()) {
        data_ = alloc_data(p_view.length());
        std::memcpy(data_ptr(), p_view.data(), p_view.length());
        data_ptr()[p_view.length()] = '\0';
    }
}

String::String(const char *p_str, usize p_len) {
    if (p_str != nullptr && p_len > 0) {
        data_ = alloc_data(p_len);
        std::memcpy(data_ptr(), p_str, p_len);
        data_ptr()[p_len] = '\0';
    }
}

String::String(const String &p_other) noexcept : data_(p_other.data_) {
    ref_data(data_);
}

String::String(String &&p_other) noexcept : data_(p_other.data_) {
    p_other.data_ = nullptr;
}

String::~String() noexcept {
    unref_data();
}

String &String::operator=(const String &p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        ref_data(data_);
    }
    return *this;
}

String &String::operator=(String &&p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        p_other.data_ = nullptr;
    }
    return *this;
}

String &String::operator=(const char *p_str) {
    *this = String(p_str);
    return *this;
}

String &String::operator=(std::string_view p_view) {
    *this = String(p_view);
    return *this;
}

String &String::operator+=(const String &p_other) {
    if (p_other.is_empty()) {
        return *this;
    }
    if (is_empty()) {
        *this = p_other;
        return *this;
    }
    usize new_len = length() + p_other.length();
    CowData *new_data = alloc_data(new_len);
    char *dst = reinterpret_cast<char *>(new_data + 1);
    std::memcpy(dst, data_ptr(), length());
    std::memcpy(dst + length(), p_other.data_ptr(), p_other.length());
    dst[new_len] = '\0';
    unref_data();
    data_ = new_data;
    return *this;
}

String &String::operator+=(std::string_view p_other) {
    if (p_other.empty()) {
        return *this;
    }
    if (is_empty()) {
        *this = String(p_other);
        return *this;
    }
    usize new_len = length() + p_other.length();
    CowData *new_data = alloc_data(new_len);
    char *dst = reinterpret_cast<char *>(new_data + 1);
    std::memcpy(dst, data_ptr(), length());
    std::memcpy(dst + length(), p_other.data(), p_other.length());
    dst[new_len] = '\0';
    unref_data();
    data_ = new_data;
    return *this;
}

String &String::operator+=(const char *p_other) {
    if (p_other != nullptr) {
        *this += std::string_view(p_other);
    }
    return *this;
}

String &String::operator+=(char p_char) {
    return *this += std::string_view(&p_char, 1);
}

String String::operator+(const String &p_other) const {
    String s = *this;
    s += p_other;
    return s;
}

String String::operator+(std::string_view p_other) const {
    String s = *this;
    s += p_other;
    return s;
}

String String::operator+(const char *p_other) const {
    String s = *this;
    s += p_other;
    return s;
}

String String::substr(usize p_from, usize p_len) const {
    if (p_from >= length()) {
        return String();
    }
    std::string_view sv = as_string_view().substr(p_from, p_len);
    return String(sv);
}

String String::left(i64 p_len) const {
    if (p_len < 0) {
        p_len = (i64)length() + p_len;
    }
    if (p_len <= 0) {
        return String();
    }
    if (p_len >= (i64)length()) {
        return *this;
    }
    return substr(0, (usize)p_len);
}

String String::right(i64 p_len) const {
    if (p_len < 0) {
        p_len = (i64)length() + p_len;
    }
    if (p_len <= 0) {
        return String();
    }
    if (p_len >= (i64)length()) {
        return *this;
    }
    return substr(length() - (usize)p_len, (usize)p_len);
}

bool String::contains(std::string_view p_sub) const noexcept {
    return as_string_view().find(p_sub) != std::string_view::npos;
}

i64 String::find(std::string_view p_sub, usize p_from) const noexcept {
    auto pos = as_string_view().find(p_sub, p_from);
    return pos == std::string_view::npos ? -1 : static_cast<i64>(pos);
}

i64 String::rfind(std::string_view p_sub, i64 p_from) const noexcept {
    if (is_empty() || p_sub.empty()) {
        return -1;
    }
    usize from = (p_from < 0 || (usize)p_from >= length()) ? length() : (usize)p_from;
    auto pos = as_string_view().rfind(p_sub, from);
    return pos == std::string_view::npos ? -1 : static_cast<i64>(pos);
}

static bool char_equals_ci(char a, char b) noexcept {
    return std::tolower(static_cast<u8>(a)) == std::tolower(static_cast<u8>(b));
}

i64 String::findn(std::string_view p_sub, usize p_from) const noexcept {
    if (p_sub.empty()) {
        return (p_from <= length()) ? (i64)p_from : -1;
    }
    if (p_from + p_sub.length() > length()) {
        return -1;
    }
    std::string_view src = as_string_view();
    for (usize i = p_from; i <= src.length() - p_sub.length(); ++i) {
        bool match = true;
        for (usize j = 0; j < p_sub.length(); ++j) {
            if (!char_equals_ci(src[i + j], p_sub[j])) {
                match = false;
                break;
            }
        }
        if (match) {
            return (i64)i;
        }
    }
    return -1;
}

i64 String::rfindn(std::string_view p_sub, i64 p_from) const noexcept {
    if (p_sub.empty()) {
        return -1;
    }
    if (p_sub.length() > length()) {
        return -1;
    }
    usize from = (p_from < 0 || (usize)p_from > length() - p_sub.length()) ? (length() - p_sub.length()) : (usize)p_from;
    std::string_view src = as_string_view();
    for (i64 i = (i64)from; i >= 0; --i) {
        bool match = true;
        for (usize j = 0; j < p_sub.length(); ++j) {
            if (!char_equals_ci(src[(usize)i + j], p_sub[j])) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;
}

bool String::begins_with(std::string_view p_prefix) const noexcept {
    return as_string_view().starts_with(p_prefix);
}

bool String::ends_with(std::string_view p_suffix) const noexcept {
    return as_string_view().ends_with(p_suffix);
}

// UTF-8 decoding & encoding helpers
static char32_t utf8_decode_next(const char *&p_ptr, const char *p_end) noexcept {
    if (p_ptr >= p_end) return 0;
    u8 c = static_cast<u8>(*p_ptr++);
    if (c < 0x80) return c;
    if ((c & 0xE0) == 0xC0) {
        if (p_ptr >= p_end) return 0;
        char32_t c2 = static_cast<u8>(*p_ptr++);
        return ((c & 0x1F) << 6) | (c2 & 0x3F);
    }
    if ((c & 0xF0) == 0xE0) {
        if (p_ptr + 1 >= p_end) return 0;
        char32_t c2 = static_cast<u8>(*p_ptr++);
        char32_t c3 = static_cast<u8>(*p_ptr++);
        return ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
    }
    if ((c & 0xF8) == 0xF0) {
        if (p_ptr + 2 >= p_end) return 0;
        char32_t c2 = static_cast<u8>(*p_ptr++);
        char32_t c3 = static_cast<u8>(*p_ptr++);
        char32_t c4 = static_cast<u8>(*p_ptr++);
        return ((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
    }
    return c;
}

static void utf8_append(std::string &p_out, char32_t p_cp) {
    if (p_cp <= 0x7F) {
        p_out.push_back(static_cast<char>(p_cp));
    } else if (p_cp <= 0x7FF) {
        p_out.push_back(static_cast<char>(0xC0 | (p_cp >> 6)));
        p_out.push_back(static_cast<char>(0x80 | (p_cp & 0x3F)));
    } else if (p_cp <= 0xFFFF) {
        p_out.push_back(static_cast<char>(0xE0 | (p_cp >> 12)));
        p_out.push_back(static_cast<char>(0x80 | ((p_cp >> 6) & 0x3F)));
        p_out.push_back(static_cast<char>(0x80 | (p_cp & 0x3F)));
    } else if (p_cp <= 0x10FFFF) {
        p_out.push_back(static_cast<char>(0xF0 | (p_cp >> 18)));
        p_out.push_back(static_cast<char>(0x80 | ((p_cp >> 12) & 0x3F)));
        p_out.push_back(static_cast<char>(0x80 | ((p_cp >> 6) & 0x3F)));
        p_out.push_back(static_cast<char>(0x80 | (p_cp & 0x3F)));
    }
}

static bool is_unicode_upper(char32_t c) noexcept {
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 0x0410 && c <= 0x042F) return true; // Cyrillic А-Я
    if (c == 0x0401) return true;               // Cyrillic Ё
    if (c >= 0x0391 && c <= 0x03A9) return true; // Greek
    if (c >= 0x0531 && c <= 0x0556) return true; // Armenian
    return false;
}

static bool is_unicode_lower(char32_t c) noexcept {
    if (c >= 'a' && c <= 'z') return true;
    if (c >= 0x0430 && c <= 0x044F) return true; // Cyrillic а-я
    if (c == 0x0451) return true;               // Cyrillic ё
    if (c >= 0x03B1 && c <= 0x03C9) return true; // Greek
    if (c >= 0x0561 && c <= 0x0586) return true; // Armenian
    return false;
}

static char32_t unicode_to_lower(char32_t c) noexcept {
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    if (c >= 0x0410 && c <= 0x042F) return c + 0x20;
    if (c == 0x0401) return 0x0451;
    if (c >= 0x0391 && c <= 0x03A9) return c + 0x20;
    if (c >= 0x0531 && c <= 0x0556) return c + 0x30;
    return c;
}

static char32_t unicode_to_upper(char32_t c) noexcept {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    if (c >= 0x0430 && c <= 0x044F) return c - 0x20;
    if (c == 0x0451) return 0x0401;
    if (c >= 0x03B1 && c <= 0x03C9) return c - 0x20;
    if (c >= 0x0561 && c <= 0x0586) return c - 0x30;
    return c;
}

String String::to_lower() const {
    if (is_empty()) return *this;
    std::string out;
    out.reserve(length());
    const char *ptr = data_ptr();
    const char *end = ptr + length();
    while (ptr < end) {
        char32_t cp = utf8_decode_next(ptr, end);
        utf8_append(out, unicode_to_lower(cp));
    }
    return String(out);
}

String String::to_upper() const {
    if (is_empty()) return *this;
    std::string out;
    out.reserve(length());
    const char *ptr = data_ptr();
    const char *end = ptr + length();
    while (ptr < end) {
        char32_t cp = utf8_decode_next(ptr, end);
        utf8_append(out, unicode_to_upper(cp));
    }
    return String(out);
}

// Godot canonical compound word separation
static std::vector<char32_t> string_to_codepoints(std::string_view sv) {
    std::vector<char32_t> cps;
    const char *ptr = sv.data();
    const char *end = ptr + sv.length();
    while (ptr < end) {
        cps.push_back(utf8_decode_next(ptr, end));
    }
    return cps;
}

static String codepoints_to_string(const std::vector<char32_t> &cps) {
    std::string out;
    for (char32_t cp : cps) {
        utf8_append(out, cp);
    }
    return String(out);
}

static String separate_compound_words(std::string_view sv) {
    if (sv.empty()) return String();
    std::vector<char32_t> cstr = string_to_codepoints(sv);
    if (cstr.empty()) return String();

    std::vector<char32_t> out;
    usize start_index = 0;

    bool is_prev_upper = is_unicode_upper(cstr[0]);
    bool is_prev_lower = is_unicode_lower(cstr[0]);
    bool is_prev_digit = (cstr[0] >= '0' && cstr[0] <= '9');

    for (usize i = 1; i < cstr.size(); ++i) {
        bool is_curr_upper = is_unicode_upper(cstr[i]);
        bool is_curr_lower = is_unicode_lower(cstr[i]);
        bool is_curr_digit = (cstr[i] >= '0' && cstr[i] <= '9');

        bool is_next_lower = false;
        if (i + 1 < cstr.size()) {
            is_next_lower = is_unicode_lower(cstr[i + 1]);
        }

        bool cond_a = is_prev_lower && is_curr_upper;
        bool cond_b = (is_prev_upper || is_prev_digit) && is_curr_upper && is_next_lower;
        bool cond_c = is_prev_digit && is_curr_lower && is_next_lower;
        bool cond_d = (is_prev_upper || is_prev_lower) && is_curr_digit;

        if (cond_a || cond_b || cond_c || cond_d) {
            for (usize k = start_index; k < i; ++k) {
                out.push_back(cstr[k]);
            }
            out.push_back(' ');
            start_index = i;
        }

        is_prev_upper = is_curr_upper;
        is_prev_lower = is_curr_lower;
        is_prev_digit = is_curr_digit;
    }

    for (usize k = start_index; k < cstr.size(); ++k) {
        out.push_back(cstr[k]);
    }

    for (usize i = 0; i < out.size(); ++i) {
        char32_t c = out[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '_' || c == '-') {
            out[i] = ' ';
        }
    }

    return codepoints_to_string(out).to_lower();
}

String String::capitalize() const {
    String words = separate_compound_words(as_string_view()).strip_edges();
    std::vector<String> slices = words.split(" ", false);
    String ret;
    for (usize i = 0; i < slices.size(); ++i) {
        String slice = slices[i];
        if (!slice.is_empty()) {
            std::vector<char32_t> cps = string_to_codepoints(slice.as_string_view());
            cps[0] = unicode_to_upper(cps[0]);
            if (i > 0) {
                ret += " ";
            }
            ret += codepoints_to_string(cps);
        }
    }
    return ret;
}

String String::to_camel_case() const {
    String words = separate_compound_words(as_string_view()).strip_edges();
    std::vector<String> slices = words.split(" ", false);
    String ret;
    for (usize i = 0; i < slices.size(); ++i) {
        String slice = slices[i];
        if (!slice.is_empty()) {
            std::vector<char32_t> cps = string_to_codepoints(slice.as_string_view());
            if (i == 0) {
                cps[0] = unicode_to_lower(cps[0]);
            } else {
                cps[0] = unicode_to_upper(cps[0]);
            }
            ret += codepoints_to_string(cps);
        }
    }
    return ret;
}

String String::to_pascal_case() const {
    String words = separate_compound_words(as_string_view()).strip_edges();
    std::vector<String> slices = words.split(" ", false);
    String ret;
    for (usize i = 0; i < slices.size(); ++i) {
        String slice = slices[i];
        if (!slice.is_empty()) {
            std::vector<char32_t> cps = string_to_codepoints(slice.as_string_view());
            cps[0] = unicode_to_upper(cps[0]);
            ret += codepoints_to_string(cps);
        }
    }
    return ret;
}

String String::to_snake_case() const {
    String words = separate_compound_words(as_string_view());
    std::vector<char32_t> cps = string_to_codepoints(words.as_string_view());
    for (usize i = 0; i < cps.size(); ++i) {
        if (cps[i] == ' ') {
            cps[i] = '_';
        } else {
            cps[i] = unicode_to_lower(cps[i]);
        }
    }
    return codepoints_to_string(cps);
}

String String::to_kebab_case() const {
    String words = separate_compound_words(as_string_view());
    std::vector<char32_t> cps = string_to_codepoints(words.as_string_view());
    for (usize i = 0; i < cps.size(); ++i) {
        if (cps[i] == ' ') {
            cps[i] = '-';
        } else {
            cps[i] = unicode_to_lower(cps[i]);
        }
    }
    return codepoints_to_string(cps);
}

String String::camelcase_to_underscore() const {
    return to_snake_case();
}

String String::strip_edges(bool p_left, bool p_right) const {
    if (is_empty()) return *this;
    std::string_view sv = as_string_view();
    auto is_space = [](char c) noexcept {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    };

    usize start = 0;
    if (p_left) {
        while (start < sv.length() && is_space(sv[start])) {
            start++;
        }
    }

    usize end = sv.length();
    if (p_right) {
        while (end > start && is_space(sv[end - 1])) {
            end--;
        }
    }

    return String(sv.substr(start, end - start));
}

String String::strip_escapes() const {
    if (is_empty()) return *this;
    std::string res;
    res.reserve(length());
    bool in_space = false;

    for (usize i = 0; i < length(); ++i) {
        char c = operator[](i);
        if (c == '\t' || c == '\r' || c == '\n' || c == ' ') {
            if (!in_space) {
                res.push_back(' ');
                in_space = true;
            }
        } else {
            res.push_back(c);
            in_space = false;
        }
    }
    return String(res).strip_edges();
}

String String::lstrip(std::string_view p_chars) const {
    if (is_empty()) return *this;
    std::string_view sv = as_string_view();
    usize start = 0;
    while (start < sv.length() && p_chars.find(sv[start]) != std::string_view::npos) {
        start++;
    }
    return String(sv.substr(start));
}

String String::rstrip(std::string_view p_chars) const {
    if (is_empty()) return *this;
    std::string_view sv = as_string_view();
    usize end = sv.length();
    while (end > 0 && p_chars.find(sv[end - 1]) != std::string_view::npos) {
        end--;
    }
    return String(sv.substr(0, end));
}

String String::trim_prefix(std::string_view p_prefix) const {
    if (begins_with(p_prefix)) {
        return substr(p_prefix.length());
    }
    return *this;
}

String String::trim_suffix(std::string_view p_suffix) const {
    if (ends_with(p_suffix)) {
        return substr(0, length() - p_suffix.length());
    }
    return *this;
}

String String::lpad(usize p_min_len, char p_character) const {
    if (length() >= p_min_len) {
        return *this;
    }
    usize pad = p_min_len - length();
    std::string res(pad, p_character);
    res.append(as_string_view());
    return String(res);
}

String String::rpad(usize p_min_len, char p_character) const {
    if (length() >= p_min_len) {
        return *this;
    }
    usize pad = p_min_len - length();
    std::string res;
    res.reserve(p_min_len);
    res.append(as_string_view());
    res.append(pad, p_character);
    return String(res);
}

String String::pad_zeros(usize p_digits) const {
    String s = *this;
    usize end = s.find(".");
    if (end == static_cast<usize>(-1)) {
        end = s.length();
    }
    usize start = 0;
    if (s.begins_with("-")) {
        start = 1;
    }
    usize cur_digits = end - start;
    if (cur_digits < p_digits) {
        usize diff = p_digits - cur_digits;
        std::string zeros(diff, '0');
        if (start == 1) {
            return "-" + String(zeros) + s.substr(1);
        } else {
            return String(zeros) + s;
        }
    }
    return s;
}

String String::pad_decimals(usize p_digits) const {
    String s = *this;
    usize dot = s.find(".");
    if (dot == static_cast<usize>(-1)) {
        if (p_digits > 0) {
            return s + "." + String(std::string(p_digits, '0'));
        }
        return s;
    }
    usize cur_decimals = s.length() - dot - 1;
    if (cur_decimals < p_digits) {
        return s + String(std::string(p_digits - cur_decimals, '0'));
    }
    if (p_digits == 0) {
        return s.substr(0, dot);
    }
    return s.substr(0, dot + 1 + p_digits);
}

String String::replace(std::string_view p_what, std::string_view p_for_what) const {
    if (p_what.empty() || is_empty()) {
        return *this;
    }
    std::string_view src = as_string_view();
    std::string res;
    usize pos = 0;
    while (pos < src.length()) {
        usize next = src.find(p_what, pos);
        if (next == std::string_view::npos) {
            res.append(src.substr(pos));
            break;
        }
        res.append(src.substr(pos, next - pos));
        res.append(p_for_what);
        pos = next + p_what.length();
    }
    return String(res);
}

String String::replacen(std::string_view p_what, std::string_view p_for_what) const {
    if (p_what.empty() || is_empty()) {
        return *this;
    }
    std::string res;
    usize pos = 0;
    while (pos < length()) {
        i64 next = findn(p_what, pos);
        if (next == -1) {
            res.append(as_string_view().substr(pos));
            break;
        }
        res.append(as_string_view().substr(pos, (usize)next - pos));
        res.append(p_for_what);
        pos = (usize)next + p_what.length();
    }
    return String(res);
}

std::vector<String> String::split(std::string_view p_delim, bool p_allow_empty, i32 p_maxsplit) const {
    std::vector<String> result;
    if (is_empty()) {
        if (p_allow_empty) {
            result.emplace_back("");
        }
        return result;
    }

    if (p_delim.empty()) {
        for (usize i = 0; i < length(); ++i) {
            char c = operator[](i);
            result.emplace_back(String(&c, 1));
        }
        return result;
    }

    std::string_view sv = as_string_view();
    usize start = 0;
    while (start <= sv.length()) {
        if (p_maxsplit > 0 && (i32)result.size() == p_maxsplit) {
            result.emplace_back(sv.substr(start));
            break;
        }
        usize end = sv.find(p_delim, start);
        if (end == std::string_view::npos) {
            std::string_view part = sv.substr(start);
            if (p_allow_empty || !part.empty()) {
                result.emplace_back(part);
            }
            break;
        }
        std::string_view part = sv.substr(start, end - start);
        if (p_allow_empty || !part.empty()) {
            result.emplace_back(part);
        }
        start = end + p_delim.length();
    }
    return result;
}

std::vector<String> String::rsplit(std::string_view p_delim, bool p_allow_empty, i32 p_maxsplit) const {
    std::vector<String> ret;
    const usize len = length();
    usize remaining_len = len;

    while (true) {
        if (remaining_len < p_delim.length() || (p_maxsplit > 0 && (i32)ret.size() == p_maxsplit)) {
            if (p_allow_empty || remaining_len > 0) {
                ret.push_back(substr(0, remaining_len));
            }
            break;
        }

        i64 left_edge;
        if (p_delim.empty()) {
            left_edge = (i64)remaining_len - 1;
        } else {
            left_edge = rfind(p_delim, (i64)remaining_len - (i64)p_delim.length());
        }

        if (left_edge < 0) {
            ret.push_back(substr(0, remaining_len));
            break;
        }

        usize substr_start = (usize)left_edge + p_delim.length();
        if (p_allow_empty || substr_start < remaining_len) {
            ret.push_back(substr(substr_start, remaining_len - substr_start));
        }

        remaining_len = (usize)left_edge;
    }

    std::reverse(ret.begin(), ret.end());
    return ret;
}

String String::join(const std::vector<String> &p_parts) const {
    String res;
    for (usize i = 0; i < p_parts.size(); ++i) {
        if (i > 0) {
            res += *this;
        }
        res += p_parts[i];
    }
    return res;
}

bool String::is_absolute_path() const noexcept {
    if (length() > 1) {
        return (operator[](0) == '/' || operator[](0) == '\\' || find(":/") != -1 || find(":\\") != -1);
    } else if (length() == 1) {
        return (operator[](0) == '/' || operator[](0) == '\\');
    }
    return false;
}

bool String::is_relative_path() const noexcept {
    return !is_absolute_path();
}

String String::get_base_dir() const {
    usize end = 0;
    i64 basepos = find("://");
    if (basepos != -1) {
        end = (usize)basepos + 3;
    } else {
        basepos = find(":/");
        if (basepos == -1) {
            basepos = find(":\\");
        }
        if (basepos != -1) {
            end = (usize)basepos + 2;
        } else if (begins_with("/")) {
            end = 1;
        }
    }

    String rs;
    String base;
    if (end != 0) {
        rs = substr(end);
        base = substr(0, end);
    } else {
        rs = *this;
    }

    i64 sep = std::max(rs.rfind("/"), rs.rfind("\\"));
    if (sep == -1) {
        return base;
    }
    return base + rs.substr(0, (usize)sep);
}

String String::get_file() const {
    i64 sep = std::max(rfind("/"), rfind("\\"));
    if (sep == -1) {
        return *this;
    }
    return substr((usize)sep + 1);
}

String String::get_extension() const {
    i64 pos = rfind(".");
    if (pos < 0 || pos < std::max(rfind("/"), rfind("\\"))) {
        return String();
    }
    return substr((usize)pos + 1);
}

String String::get_basename() const {
    i64 pos = rfind(".");
    if (pos < 0 || pos < std::max(rfind("/"), rfind("\\"))) {
        return *this;
    }
    return substr(0, (usize)pos);
}

String String::path_join(const String &p_file) const {
    if (is_empty()) return p_file;
    if (operator[](length() - 1) == '/' || (!p_file.is_empty() && p_file[0] == '/')) {
        return *this + p_file;
    }
    return *this + "/" + p_file;
}

String String::simplify_path() const {
    String s = *this;
    String drive;

    i64 p = s.find("://");
    bool found = false;
    if (p > 0) {
        bool only_chars = true;
        for (usize i = 0; i < (usize)p; ++i) {
            if (!std::isalnum(static_cast<u8>(s[i]))) {
                only_chars = false;
                break;
            }
        }
        if (only_chars) {
            found = true;
            drive = s.substr(0, (usize)p + 3);
            s = s.substr((usize)p + 3);
        }
    }
    if (!found) {
        if (s.begins_with("//") || s.begins_with("\\\\")) {
            drive = s.substr(0, 2);
            s = s.substr(2);
        } else if (s.begins_with("/") || s.begins_with("\\")) {
            drive = s.substr(0, 1);
            s = s.substr(1);
        } else {
            p = s.find(":/");
            if (p == -1) p = s.find(":\\");
            if (p != -1) {
                drive = s.substr(0, (usize)p + 2);
                s = s.substr((usize)p + 2);
            }
        }
    }

    s = s.replace("\\", "/");
    while (true) {
        String comp = s.replace("//", "/");
        if (s == comp) break;
        s = comp;
    }

    std::vector<String> dirs = s.split("/", false);
    bool abs_path = is_absolute_path() && !begins_with("res://");

    for (usize i = 0; i < dirs.size(); ++i) {
        String d = dirs[i];
        if (d == ".") {
            dirs.erase(dirs.begin() + i);
            i--;
        } else if (d == "..") {
            if (i != 0 && dirs[i - 1] != "..") {
                dirs.erase(dirs.begin() + i);
                dirs.erase(dirs.begin() + i - 1);
                i -= 2;
            } else if (abs_path && i == 0) {
                dirs.erase(dirs.begin() + i);
                i--;
            }
        }
    }

    s = "";
    for (usize i = 0; i < dirs.size(); ++i) {
        if (i > 0) s += "/";
        s += dirs[i];
    }
    return drive + s;
}

String String::c_escape() const {
    String escaped = *this;
    escaped = escaped.replace("\\", "\\\\");
    escaped = escaped.replace("\a", "\\a");
    escaped = escaped.replace("\b", "\\b");
    escaped = escaped.replace("\f", "\\f");
    escaped = escaped.replace("\n", "\\n");
    escaped = escaped.replace("\r", "\\r");
    escaped = escaped.replace("\t", "\\t");
    escaped = escaped.replace("\v", "\\v");
    escaped = escaped.replace("\'", "\\'");
    escaped = escaped.replace("\"", "\\\"");
    return escaped;
}

String String::c_unescape() const {
    String escaped = *this;
    escaped = escaped.replace("\\a", "\a");
    escaped = escaped.replace("\\b", "\b");
    escaped = escaped.replace("\\f", "\f");
    escaped = escaped.replace("\\n", "\n");
    escaped = escaped.replace("\\r", "\r");
    escaped = escaped.replace("\\t", "\t");
    escaped = escaped.replace("\\v", "\v");
    escaped = escaped.replace("\\'", "\'");
    escaped = escaped.replace("\\\"", "\"");
    escaped = escaped.replace("\\\\", "\\");
    return escaped;
}

String String::json_escape() const {
    String escaped = *this;
    escaped = escaped.replace("\\", "\\\\");
    escaped = escaped.replace("\b", "\\b");
    escaped = escaped.replace("\f", "\\f");
    escaped = escaped.replace("\n", "\\n");
    escaped = escaped.replace("\r", "\\r");
    escaped = escaped.replace("\t", "\\t");
    escaped = escaped.replace("\v", "\\v");
    escaped = escaped.replace("\"", "\\\"");
    return escaped;
}

String String::xml_escape(bool p_escape_quotes) const {
    String str = *this;
    str = str.replace("&", "&amp;");
    str = str.replace("<", "&lt;");
    str = str.replace(">", "&gt;");
    if (p_escape_quotes) {
        str = str.replace("'", "&apos;");
        str = str.replace("\"", "&quot;");
    }
    return str;
}

i64 String::to_int() const noexcept {
    i64 val = 0;
    std::string_view sv = as_string_view();
    while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) {
        sv.remove_prefix(1);
    }
    if (sv.empty()) return 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), val);
    return val;
}

f64 String::to_float() const noexcept {
    std::string_view sv = as_string_view();
    while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) {
        sv.remove_prefix(1);
    }
    if (sv.empty()) return 0.0;
    char *endptr = nullptr;
    return std::strtod(sv.data(), &endptr);
}

i64 String::hex_to_int() const noexcept {
    std::string_view sv = as_string_view();
    if (sv.starts_with("0x") || sv.starts_with("0X")) {
        sv.remove_prefix(2);
    }
    if (sv.starts_with("#")) {
        sv.remove_prefix(1);
    }
    i64 val = 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), val, 16);
    return val;
}

i64 String::bin_to_int() const noexcept {
    std::string_view sv = as_string_view();
    if (sv.starts_with("0b") || sv.starts_with("0B")) {
        sv.remove_prefix(2);
    }
    i64 val = 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), val, 2);
    return val;
}

bool String::is_valid_int() const noexcept {
    std::string_view sv = as_string_view();
    if (sv.empty()) return false;
    usize i = 0;
    if (sv[0] == '-' || sv[0] == '+') i++;
    if (i >= sv.length()) return false;
    for (; i < sv.length(); ++i) {
        if (!std::isdigit(static_cast<u8>(sv[i]))) return false;
    }
    return true;
}

bool String::is_valid_float() const noexcept {
    std::string_view sv = as_string_view();
    if (sv.empty()) return false;
    char *endptr = nullptr;
    std::strtod(sv.data(), &endptr);
    return endptr == sv.data() + sv.length();
}

bool String::is_valid_hex_number() const noexcept {
    std::string_view sv = as_string_view();
    if (sv.starts_with("0x") || sv.starts_with("0X")) sv.remove_prefix(2);
    if (sv.empty()) return false;
    for (char c : sv) {
        if (!std::isxdigit(static_cast<u8>(c))) return false;
    }
    return true;
}

bool String::is_valid_identifier() const noexcept {
    if (is_empty()) return false;
    if (!std::isalpha(static_cast<u8>(operator[](0))) && operator[](0) != '_') return false;
    for (usize i = 1; i < length(); ++i) {
        char c = operator[](i);
        if (!std::isalnum(static_cast<u8>(c)) && c != '_') return false;
    }
    return true;
}

bool String::is_valid_filename() const noexcept {
    if (is_empty()) return false;
    for (usize i = 0; i < length(); ++i) {
        char c = operator[](i);
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            return false;
        }
    }
    return true;
}

String String::num_int64(i64 p_num) {
    char buf[32];
    auto res = std::to_chars(buf, buf + sizeof(buf), p_num);
    return String(buf, res.ptr - buf);
}

String String::num_uint64(u64 p_num) {
    char buf[32];
    auto res = std::to_chars(buf, buf + sizeof(buf), p_num);
    return String(buf, res.ptr - buf);
}

String String::num_real(f64 p_num) {
    return String(std::format("{}", p_num));
}

String String::chr(char32_t p_char) {
    std::string s;
    utf8_append(s, p_char);
    return String(s);
}

char32_t String::ord(char p_char) noexcept {
    return static_cast<u8>(p_char);
}

} // namespace Beyota
