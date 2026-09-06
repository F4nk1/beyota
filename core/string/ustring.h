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
#include <charconv>
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
        // Data follows immediately in memory: char data[length + 1]
    };

    CowData *_data{nullptr};

    [[nodiscard]] static CowData *alloc_data(usize p_len) {
        if (p_len == 0) {
            return nullptr;
        }
        void *mem = ::operator new(sizeof(CowData) + p_len + 1);
        CowData *d = static_cast<CowData *>(mem);
        d->refcount.store(1, std::memory_order_relaxed);
        d->length = static_cast<u32>(p_len);
        return d;
    }

    static void free_data(CowData *p_data) noexcept {
        if (p_data != nullptr) {
            ::operator delete(static_cast<void *>(p_data));
        }
    }

    void _ref(const CowData *p_data) noexcept {
        if (p_data != nullptr) {
            const_cast<CowData *>(p_data)->refcount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void _unref() noexcept {
        if (_data != nullptr) {
            if (_data->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                free_data(_data);
            }
            _data = nullptr;
        }
    }

    [[nodiscard]] char *data_ptr() noexcept {
        return _data ? reinterpret_cast<char *>(_data + 1) : nullptr;
    }

    [[nodiscard]] const char *data_ptr() const noexcept {
        return _data ? reinterpret_cast<const char *>(_data + 1) : nullptr;
    }

    void _detach_if_shared() {
        if (_data && _data->refcount.load(std::memory_order_relaxed) > 1) {
            CowData *new_data = alloc_data(_data->length);
            std::memcpy(reinterpret_cast<char *>(new_data + 1), data_ptr(), _data->length + 1);
            _unref();
            _data = new_data;
        }
    }

public:
    constexpr String() noexcept = default;

    String(const char *p_str) {
        if (p_str != nullptr) {
            usize len = std::strlen(p_str);
            if (len > 0) {
                _data = alloc_data(len);
                std::memcpy(data_ptr(), p_str, len);
                data_ptr()[len] = '\0';
            }
        }
    }

    String(std::string_view p_view) {
        if (!p_view.empty()) {
            _data = alloc_data(p_view.length());
            std::memcpy(data_ptr(), p_view.data(), p_view.length());
            data_ptr()[p_view.length()] = '\0';
        }
    }

    String(const char *p_str, usize p_len) {
        if (p_str != nullptr && p_len > 0) {
            _data = alloc_data(p_len);
            std::memcpy(data_ptr(), p_str, p_len);
            data_ptr()[p_len] = '\0';
        }
    }

    String(const String &p_other) noexcept : _data(p_other._data) {
        _ref(_data);
    }

    String(String &&p_other) noexcept : _data(p_other._data) {
        p_other._data = nullptr;
    }

    ~String() noexcept {
        _unref();
    }

    String &operator=(const String &p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            _ref(_data);
        }
        return *this;
    }

    String &operator=(String &&p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            p_other._data = nullptr;
        }
        return *this;
    }

    String &operator=(const char *p_str) {
        *this = String(p_str);
        return *this;
    }

    String &operator=(std::string_view p_view) {
        *this = String(p_view);
        return *this;
    }

    [[nodiscard]] usize length() const noexcept {
        return _data ? _data->length : 0;
    }

    [[nodiscard]] usize size() const noexcept {
        return length();
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return _data == nullptr || _data->length == 0;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        const char *p = data_ptr();
        return p ? p : "";
    }

    [[nodiscard]] const char *utf8() const noexcept {
        return c_str();
    }

    [[nodiscard]] std::string_view as_string_view() const noexcept {
        return _data ? std::string_view(data_ptr(), _data->length) : std::string_view{};
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
        _detach_if_shared();
        return data_ptr()[p_idx];
    }

    [[nodiscard]] u32 hash() const noexcept {
        return hash_make_uint32_t(as_string_view());
    }

    [[nodiscard]] bool contains(std::string_view p_sub) const noexcept {
        return as_string_view().find(p_sub) != std::string_view::npos;
    }

    [[nodiscard]] i64 find(std::string_view p_sub, usize p_from = 0) const noexcept {
        auto pos = as_string_view().find(p_sub, p_from);
        return pos == std::string_view::npos ? -1 : static_cast<i64>(pos);
    }

    [[nodiscard]] bool begins_with(std::string_view p_prefix) const noexcept {
        return as_string_view().starts_with(p_prefix);
    }

    [[nodiscard]] bool ends_with(std::string_view p_suffix) const noexcept {
        return as_string_view().ends_with(p_suffix);
    }

    [[nodiscard]] String substr(usize p_from, usize p_len = static_cast<usize>(-1)) const {
        if (p_from >= length()) {
            return String();
        }
        std::string_view sv = as_string_view().substr(p_from, p_len);
        return String(sv);
    }

    [[nodiscard]] String replace(std::string_view p_what, std::string_view p_for_what) const {
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

    [[nodiscard]] std::vector<String> split(std::string_view p_delim, bool p_allow_empty = true) const {
        std::vector<String> result;
        if (p_delim.empty()) {
            result.push_back(*this);
            return result;
        }
        std::string_view sv = as_string_view();
        usize start = 0;
        while (start <= sv.length()) {
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

    [[nodiscard]] i64 to_int() const noexcept {
        i64 val = 0;
        std::string_view sv = as_string_view();
        while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) {
            sv.remove_prefix(1);
        }
        if (sv.empty()) return 0;
        std::from_chars(sv.data(), sv.data() + sv.size(), val);
        return val;
    }

    [[nodiscard]] f64 to_float() const noexcept {
        std::string_view sv = as_string_view();
        while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) {
            sv.remove_prefix(1);
        }
        if (sv.empty()) return 0.0;
        char *endptr = nullptr;
        return std::strtod(sv.data(), &endptr);
    }

    static String num_int64(i64 p_num) {
        char buf[32];
        auto res = std::to_chars(buf, buf + sizeof(buf), p_num);
        return String(buf, res.ptr - buf);
    }

    static String num_real(f64 p_num) {
        return String(std::format("{}", p_num));
    }

    String &operator+=(const String &p_other) {
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
        _unref();
        _data = new_data;
        return *this;
    }

    String &operator+=(std::string_view p_other) {
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
        _unref();
        _data = new_data;
        return *this;
    }

    String &operator+=(const char *p_other) {
        if (p_other != nullptr) {
            *this += std::string_view(p_other);
        }
        return *this;
    }

    String &operator+=(char p_char) {
        return *this += std::string_view(&p_char, 1);
    }

    [[nodiscard]] String operator+(const String &p_other) const {
        String s = *this;
        s += p_other;
        return s;
    }

    [[nodiscard]] String operator+(std::string_view p_other) const {
        String s = *this;
        s += p_other;
        return s;
    }

    [[nodiscard]] String operator+(const char *p_other) const {
        String s = *this;
        s += p_other;
        return s;
    }

    [[nodiscard]] bool operator==(const String &p_other) const noexcept {
        if (_data == p_other._data) {
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
        if (_data == p_other._data) {
            return std::strong_ordering::equal;
        }
        return as_string_view() <=> p_other.as_string_view();
    }

    [[nodiscard]] auto operator<=>(std::string_view p_other) const noexcept {
        return as_string_view() <=> p_other;
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
