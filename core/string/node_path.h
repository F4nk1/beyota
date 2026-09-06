/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  node_path.h                                                           */
/**************************************************************************/

#pragma once

#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <atomic>
#include <compare>
#include <string_view>
#include <vector>

namespace Beyota {

class NodePath {
    struct Data {
        std::atomic<u32> refcount{1};
        bool absolute{false};
        std::vector<StringName> names;
        std::vector<StringName> subnames;
        String raw_path;
        u32 hash{0};
    };

    Data *_data{nullptr};

    void _unref() noexcept {
        if (_data != nullptr) {
            if (_data->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete _data;
            }
            _data = nullptr;
        }
    }

    void _parse(std::string_view p_path) {
        if (p_path.empty()) {
            return;
        }
        _data = new Data();
        _data->raw_path = String(p_path);
        _data->hash = hash_make_uint32_t(p_path);

        std::string_view remaining = p_path;
        if (remaining.starts_with('/')) {
            _data->absolute = true;
            remaining.remove_prefix(1);
        }

        // Split property subnames by ':'
        auto colon_pos = remaining.find(':');
        std::string_view node_part = (colon_pos == std::string_view::npos) ? remaining : remaining.substr(0, colon_pos);
        std::string_view prop_part = (colon_pos == std::string_view::npos) ? std::string_view{} : remaining.substr(colon_pos + 1);

        // Parse node names separated by '/'
        usize start = 0;
        while (start <= node_part.length() && !node_part.empty()) {
            usize next = node_part.find('/', start);
            if (next == std::string_view::npos) {
                std::string_view segment = node_part.substr(start);
                if (!segment.empty()) {
                    _data->names.emplace_back(segment);
                }
                break;
            }
            std::string_view segment = node_part.substr(start, next - start);
            if (!segment.empty()) {
                _data->names.emplace_back(segment);
            }
            start = next + 1;
        }

        // Parse subnames separated by ':'
        start = 0;
        while (start <= prop_part.length() && !prop_part.empty()) {
            usize next = prop_part.find(':', start);
            if (next == std::string_view::npos) {
                std::string_view segment = prop_part.substr(start);
                if (!segment.empty()) {
                    _data->subnames.emplace_back(segment);
                }
                break;
            }
            std::string_view segment = prop_part.substr(start, next - start);
            if (!segment.empty()) {
                _data->subnames.emplace_back(segment);
            }
            start = next + 1;
        }
    }

public:
    constexpr NodePath() noexcept = default;

    NodePath(const char *p_path) {
        if (p_path && p_path[0] != '\0') {
            _parse(p_path);
        }
    }

    NodePath(std::string_view p_path) {
        if (!p_path.empty()) {
            _parse(p_path);
        }
    }

    NodePath(const String &p_path) {
        if (!p_path.is_empty()) {
            _parse(p_path.as_string_view());
        }
    }

    NodePath(const NodePath &p_other) noexcept : _data(p_other._data) {
        if (_data != nullptr) {
            _data->refcount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    NodePath(NodePath &&p_other) noexcept : _data(p_other._data) {
        p_other._data = nullptr;
    }

    ~NodePath() noexcept {
        _unref();
    }

    NodePath &operator=(const NodePath &p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            if (_data != nullptr) {
                _data->refcount.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    NodePath &operator=(NodePath &&p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            p_other._data = nullptr;
        }
        return *this;
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return _data == nullptr || (_data->names.empty() && _data->subnames.empty());
    }

    [[nodiscard]] bool is_absolute() const noexcept {
        return _data ? _data->absolute : false;
    }

    [[nodiscard]] usize get_name_count() const noexcept {
        return _data ? _data->names.size() : 0;
    }

    [[nodiscard]] StringName get_name(usize p_idx) const noexcept {
        if (_data && p_idx < _data->names.size()) {
            return _data->names[p_idx];
        }
        return StringName();
    }

    [[nodiscard]] usize get_subname_count() const noexcept {
        return _data ? _data->subnames.size() : 0;
    }

    [[nodiscard]] StringName get_subname(usize p_idx) const noexcept {
        if (_data && p_idx < _data->subnames.size()) {
            return _data->subnames[p_idx];
        }
        return StringName();
    }

    [[nodiscard]] u32 hash() const noexcept {
        return _data ? _data->hash : 0;
    }

    [[nodiscard]] operator String() const {
        return _data ? _data->raw_path : String();
    }

    [[nodiscard]] String get_concatenated_names() const {
        if (!_data || _data->names.empty()) return String();
        String res;
        for (usize i = 0; i < _data->names.size(); ++i) {
            if (i > 0) res += "/";
            res += String(_data->names[i]);
        }
        return res;
    }

    [[nodiscard]] String get_concatenated_subnames() const {
        if (!_data || _data->subnames.empty()) return String();
        String res;
        for (usize i = 0; i < _data->subnames.size(); ++i) {
            if (i > 0) res += ":";
            res += String(_data->subnames[i]);
        }
        return res;
    }

    [[nodiscard]] bool operator==(const NodePath &p_other) const noexcept {
        if (_data == p_other._data) {
            return true;
        }
        if (is_empty() && p_other.is_empty()) {
            return true;
        }
        if (!is_empty() && !p_other.is_empty()) {
            return _data->raw_path == p_other._data->raw_path;
        }
        return false;
    }

    [[nodiscard]] auto operator<=>(const NodePath &p_other) const noexcept {
        if (_data == p_other._data) {
            return std::strong_ordering::equal;
        }
        String s1 = _data ? _data->raw_path : String();
        String s2 = p_other._data ? p_other._data->raw_path : String();
        return s1 <=> s2;
    }
};

} // namespace Beyota
