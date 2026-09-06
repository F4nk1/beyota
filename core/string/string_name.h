/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  string_name.h                                                         */
/**************************************************************************/

#pragma once

#include "core/string/ustring.h"
#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <atomic>
#include <compare>
#include <functional>
#include <mutex>
#include <string_view>
#include <unordered_map>

namespace Beyota {

class StringName {
    struct Data {
        std::atomic<u32> refcount{1};
        u32 hash{0};
        String name;

        Data(String p_name, u32 p_hash) : hash(p_hash), name(std::move(p_name)) {}
    };

    struct Table {
        std::mutex mutex;
        std::unordered_map<std::string_view, Data *> map;
    };

    static Table &get_table() {
        static Table table;
        return table;
    }

    Data *_data{nullptr};

    static Data *_get_or_create(std::string_view p_name) {
        if (p_name.empty()) {
            return nullptr;
        }
        Table &t = get_table();
        std::lock_guard<std::mutex> lock(t.mutex);
        auto it = t.map.find(p_name);
        if (it != t.map.end()) {
            it->second->refcount.fetch_add(1, std::memory_order_relaxed);
            return it->second;
        }
        u32 h = hash_make_uint32_t(p_name);
        String s(p_name);
        Data *d = new Data(s, h);
        t.map[d->name.as_string_view()] = d;
        return d;
    }

    void _unref() noexcept {
        if (_data != nullptr) {
            if (_data->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                Table &t = get_table();
                std::lock_guard<std::mutex> lock(t.mutex);
                t.map.erase(_data->name.as_string_view());
                delete _data;
            }
            _data = nullptr;
        }
    }

public:
    constexpr StringName() noexcept = default;

    StringName(const char *p_str) {
        if (p_str && p_str[0] != '\0') {
            _data = _get_or_create(p_str);
        }
    }

    StringName(std::string_view p_view) {
        if (!p_view.empty()) {
            _data = _get_or_create(p_view);
        }
    }

    StringName(const String &p_str) {
        if (!p_str.is_empty()) {
            _data = _get_or_create(p_str.as_string_view());
        }
    }

    StringName(const StringName &p_other) noexcept : _data(p_other._data) {
        if (_data != nullptr) {
            _data->refcount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    StringName(StringName &&p_other) noexcept : _data(p_other._data) {
        p_other._data = nullptr;
    }

    ~StringName() noexcept {
        _unref();
    }

    StringName &operator=(const StringName &p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            if (_data != nullptr) {
                _data->refcount.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    StringName &operator=(StringName &&p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            p_other._data = nullptr;
        }
        return *this;
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return _data == nullptr;
    }

    [[nodiscard]] u32 hash() const noexcept {
        return _data ? _data->hash : 0;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        return _data ? _data->name.c_str() : "";
    }

    [[nodiscard]] std::string_view as_string_view() const noexcept {
        return _data ? _data->name.as_string_view() : std::string_view{};
    }

    [[nodiscard]] operator std::string_view() const noexcept {
        return as_string_view();
    }

    [[nodiscard]] operator String() const {
        return _data ? _data->name : String();
    }

    [[nodiscard]] bool operator==(const StringName &p_other) const noexcept {
        return _data == p_other._data; // O(1) comparison!
    }

    [[nodiscard]] bool operator==(std::string_view p_other) const noexcept {
        return as_string_view() == p_other;
    }

    [[nodiscard]] bool operator==(const char *p_other) const noexcept {
        return as_string_view() == (p_other ? std::string_view(p_other) : std::string_view{});
    }

    [[nodiscard]] auto operator<=>(const StringName &p_other) const noexcept {
        if (_data == p_other._data) {
            return std::strong_ordering::equal;
        }
        return as_string_view() <=> p_other.as_string_view();
    }
};

} // namespace Beyota

template <>
struct std::formatter<Beyota::StringName> : std::formatter<std::string_view> {
    auto format(const Beyota::StringName &p_sn, std::format_context &p_ctx) const {
        return std::formatter<std::string_view>::format(p_sn.as_string_view(), p_ctx);
    }
};

template <>
struct std::hash<Beyota::StringName> {
    [[nodiscard]] std::size_t operator()(const Beyota::StringName &p_sn) const noexcept {
        return p_sn.hash();
    }
};
