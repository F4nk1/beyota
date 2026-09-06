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
        mutable String concatenated_names;
        mutable String concatenated_subnames;
        mutable u32 hash_cache{0};
        mutable bool hash_cache_valid{false};
    };

    Data *data_{nullptr};

    void ref_data() noexcept;
    void unref_data() noexcept;
    void parse_path(std::string_view p_path);

public:
    constexpr NodePath() noexcept = default;

    NodePath(const char *p_path);
    NodePath(std::string_view p_path);
    NodePath(const String &p_path);
    NodePath(const std::vector<StringName> &p_names, bool p_absolute = false);
    NodePath(const std::vector<StringName> &p_names, const std::vector<StringName> &p_subnames, bool p_absolute = false);
    NodePath(const NodePath &p_other) noexcept;
    NodePath(NodePath &&p_other) noexcept;
    ~NodePath() noexcept;

    NodePath &operator=(const NodePath &p_other) noexcept;
    NodePath &operator=(NodePath &&p_other) noexcept;

    [[nodiscard]] bool is_empty() const noexcept {
        return data_ == nullptr || (data_->names.empty() && data_->subnames.empty() && !data_->absolute);
    }

    [[nodiscard]] bool is_absolute() const noexcept {
        return data_ ? data_->absolute : false;
    }

    [[nodiscard]] usize get_name_count() const noexcept {
        return data_ ? data_->names.size() : 0;
    }

    [[nodiscard]] StringName get_name(i64 p_idx) const noexcept;

    [[nodiscard]] usize get_subname_count() const noexcept {
        return data_ ? data_->subnames.size() : 0;
    }

    [[nodiscard]] StringName get_subname(i64 p_idx) const noexcept;

    [[nodiscard]] usize get_total_name_count() const noexcept {
        return get_name_count() + get_subname_count();
    }

    [[nodiscard]] String get_concatenated_names() const;
    [[nodiscard]] String get_concatenated_subnames() const;

    [[nodiscard]] NodePath get_as_property_path() const;
    [[nodiscard]] NodePath slice(i64 p_begin, i64 p_end = 0x7FFFFFFF) const;

    [[nodiscard]] u32 hash() const noexcept;
    [[nodiscard]] operator String() const;

    [[nodiscard]] bool operator==(const NodePath &p_other) const noexcept;
    [[nodiscard]] bool operator!=(const NodePath &p_other) const noexcept {
        return !(*this == p_other);
    }
    [[nodiscard]] auto operator<=>(const NodePath &p_other) const noexcept;
};

} // namespace Beyota
