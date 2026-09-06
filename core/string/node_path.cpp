/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  node_path.cpp                                                         */
/**************************************************************************/

#include "core/string/node_path.h"

#include <algorithm>

namespace Beyota {

void NodePath::ref_data() noexcept {
    if (data_ != nullptr) {
        data_->refcount.fetch_add(1, std::memory_order_relaxed);
    }
}

void NodePath::unref_data() noexcept {
    if (data_ != nullptr) {
        if (data_->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete data_;
        }
        data_ = nullptr;
    }
}

void NodePath::parse_path(std::string_view p_path) {
    if (p_path.empty()) {
        return;
    }

    std::string_view path = p_path;
    std::vector<StringName> subnames;
    bool absolute = (path.front() == '/');

    auto subpath_pos = path.find(':');
    if (subpath_pos != std::string_view::npos) {
        std::string_view prop_part = path.substr(subpath_pos + 1);
        path = path.substr(0, subpath_pos);

        usize from = 0;
        while (from <= prop_part.length()) {
            auto next_colon = prop_part.find(':', from);
            if (next_colon == std::string_view::npos) {
                std::string_view seg = prop_part.substr(from);
                if (!seg.empty()) {
                    subnames.emplace_back(seg);
                }
                break;
            }
            std::string_view seg = prop_part.substr(from, next_colon - from);
            if (!seg.empty()) {
                subnames.emplace_back(seg);
            }
            from = next_colon + 1;
        }
    }

    std::vector<StringName> names;
    usize start = absolute ? 1 : 0;
    while (start <= path.length() && !path.empty()) {
        auto next_slash = path.find('/', start);
        if (next_slash == std::string_view::npos) {
            std::string_view seg = path.substr(start);
            if (!seg.empty()) {
                names.emplace_back(seg);
            }
            break;
        }
        std::string_view seg = path.substr(start, next_slash - start);
        if (!seg.empty()) {
            names.emplace_back(seg);
        }
        start = next_slash + 1;
    }

    if (names.empty() && subnames.empty() && !absolute) {
        return;
    }

    data_ = new Data();
    data_->absolute = absolute;
    data_->names = std::move(names);
    data_->subnames = std::move(subnames);
}

NodePath::NodePath(const char *p_path) {
    if (p_path != nullptr && p_path[0] != '\0') {
        parse_path(p_path);
    }
}

NodePath::NodePath(std::string_view p_path) {
    if (!p_path.empty()) {
        parse_path(p_path);
    }
}

NodePath::NodePath(const String &p_path) {
    if (!p_path.is_empty()) {
        parse_path(p_path.as_string_view());
    }
}

NodePath::NodePath(const std::vector<StringName> &p_names, bool p_absolute) {
    if (p_names.empty() && !p_absolute) {
        return;
    }
    data_ = new Data();
    data_->absolute = p_absolute;
    data_->names = p_names;
}

NodePath::NodePath(const std::vector<StringName> &p_names, const std::vector<StringName> &p_subnames, bool p_absolute) {
    if (p_names.empty() && p_subnames.empty() && !p_absolute) {
        return;
    }
    data_ = new Data();
    data_->absolute = p_absolute;
    data_->names = p_names;
    data_->subnames = p_subnames;
}

NodePath::NodePath(const NodePath &p_other) noexcept : data_(p_other.data_) {
    ref_data();
}

NodePath::NodePath(NodePath &&p_other) noexcept : data_(p_other.data_) {
    p_other.data_ = nullptr;
}

NodePath::~NodePath() noexcept {
    unref_data();
}

NodePath &NodePath::operator=(const NodePath &p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        ref_data();
    }
    return *this;
}

NodePath &NodePath::operator=(NodePath &&p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        p_other.data_ = nullptr;
    }
    return *this;
}

StringName NodePath::get_name(i64 p_idx) const noexcept {
    if (data_ != nullptr && p_idx >= 0 && (usize)p_idx < data_->names.size()) {
        return data_->names[(usize)p_idx];
    }
    return StringName();
}

StringName NodePath::get_subname(i64 p_idx) const noexcept {
    if (data_ != nullptr && p_idx >= 0 && (usize)p_idx < data_->subnames.size()) {
        return data_->subnames[(usize)p_idx];
    }
    return StringName();
}

String NodePath::get_concatenated_names() const {
    if (data_ == nullptr || data_->names.empty()) {
        return String();
    }
    if (data_->concatenated_names.is_empty()) {
        String res;
        for (usize i = 0; i < data_->names.size(); ++i) {
            if (i > 0) res += "/";
            res += String(data_->names[i]);
        }
        data_->concatenated_names = res;
    }
    return data_->concatenated_names;
}

String NodePath::get_concatenated_subnames() const {
    if (data_ == nullptr || data_->subnames.empty()) {
        return String();
    }
    if (data_->concatenated_subnames.is_empty()) {
        String res;
        for (usize i = 0; i < data_->subnames.size(); ++i) {
            if (i > 0) res += ":";
            res += String(data_->subnames[i]);
        }
        data_->concatenated_subnames = res;
    }
    return data_->concatenated_subnames;
}

NodePath NodePath::get_as_property_path() const {
    if (data_ == nullptr || data_->names.empty()) {
        return *this;
    }
    std::vector<StringName> new_subpath = data_->subnames;
    String initial_subname = String(data_->names[0]);
    for (usize i = 1; i < data_->names.size(); ++i) {
        initial_subname += "/" + String(data_->names[i]);
    }
    new_subpath.insert(new_subpath.begin(), StringName(initial_subname));
    return NodePath(std::vector<StringName>{}, new_subpath, false);
}

NodePath NodePath::slice(i64 p_begin, i64 p_end) const {
    const i64 name_count = (i64)get_name_count();
    const i64 total_count = (i64)get_total_name_count();
    if (total_count == 0) {
        if (is_absolute()) {
            return NodePath(std::vector<StringName>{}, true);
        }
        return NodePath();
    }

    i64 begin = std::clamp(p_begin, -total_count, total_count);
    if (begin < 0) {
        begin += total_count;
    }
    i64 end = std::clamp(p_end, -total_count, total_count);
    if (end < 0) {
        end += total_count;
    }

    if (begin >= end) {
        return NodePath();
    }

    i64 sub_begin = std::max(begin - name_count, (i64)0);
    i64 sub_end = std::max(end - name_count, (i64)0);

    std::vector<StringName> names_slice;
    if (begin < name_count) {
        i64 n_end = std::min(end, name_count);
        for (i64 i = begin; i < n_end; ++i) {
            names_slice.push_back(get_name(i));
        }
    }

    std::vector<StringName> subnames_slice;
    if (end > name_count) {
        for (i64 i = sub_begin; i < sub_end && i < (i64)get_subname_count(); ++i) {
            subnames_slice.push_back(get_subname(i));
        }
    }

    bool abs = is_absolute() && (begin == 0);
    return NodePath(names_slice, subnames_slice, abs);
}

u32 NodePath::hash() const noexcept {
    if (data_ == nullptr) {
        return 0;
    }
    if (!data_->hash_cache_valid) {
        u32 h = hash_make_uint32_t(static_cast<String>(*this).as_string_view());
        data_->hash_cache = data_->absolute ? h : ~h;
        data_->hash_cache_valid = true;
    }
    return data_->hash_cache;
}

NodePath::operator String() const {
    if (data_ == nullptr) {
        return String();
    }
    String ret = data_->absolute ? "/" : "";
    ret += get_concatenated_names();
    String subpath = get_concatenated_subnames();
    if (!subpath.is_empty()) {
        ret += ":" + subpath;
    }
    return ret;
}

bool NodePath::operator==(const NodePath &p_other) const noexcept {
    if (data_ == p_other.data_) {
        return true;
    }
    if (is_empty() && p_other.is_empty()) {
        return true;
    }
    if (data_ == nullptr || p_other.data_ == nullptr) {
        return false;
    }
    if (data_->absolute != p_other.data_->absolute) {
        return false;
    }
    if (data_->names != p_other.data_->names) {
        return false;
    }
    if (data_->subnames != p_other.data_->subnames) {
        return false;
    }
    return true;
}

auto NodePath::operator<=>(const NodePath &p_other) const noexcept {
    if (data_ == p_other.data_) {
        return std::strong_ordering::equal;
    }
    String s1 = static_cast<String>(*this);
    String s2 = static_cast<String>(p_other);
    return s1 <=> s2;
}

} // namespace Beyota
