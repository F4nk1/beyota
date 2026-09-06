/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  string_name.cpp                                                       */
/**************************************************************************/

#include "core/string/string_name.h"

#include <mutex>
#include <unordered_map>

namespace Beyota {

struct StringNameTable {
    std::mutex mutex;
    std::unordered_map<std::string_view, StringName::Data *> map;
};

static StringNameTable &get_string_name_table() {
    static StringNameTable table;
    return table;
}

StringName::Data *StringName::get_or_create(std::string_view p_name) {
    if (p_name.empty()) {
        return nullptr;
    }
    StringNameTable &t = get_string_name_table();
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

void StringName::ref_data() noexcept {
    if (data_ != nullptr) {
        data_->refcount.fetch_add(1, std::memory_order_relaxed);
    }
}

void StringName::unref_data() noexcept {
    if (data_ != nullptr) {
        if (data_->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            StringNameTable &t = get_string_name_table();
            std::lock_guard<std::mutex> lock(t.mutex);
            t.map.erase(data_->name.as_string_view());
            delete data_;
        }
        data_ = nullptr;
    }
}

StringName::StringName(const char *p_str) {
    if (p_str != nullptr && p_str[0] != '\0') {
        data_ = get_or_create(p_str);
    }
}

StringName::StringName(std::string_view p_view) {
    if (!p_view.empty()) {
        data_ = get_or_create(p_view);
    }
}

StringName::StringName(const String &p_str) {
    if (!p_str.is_empty()) {
        data_ = get_or_create(p_str.as_string_view());
    }
}

StringName::StringName(const StringName &p_other) noexcept : data_(p_other.data_) {
    ref_data();
}

StringName::StringName(StringName &&p_other) noexcept : data_(p_other.data_) {
    p_other.data_ = nullptr;
}

StringName::~StringName() noexcept {
    unref_data();
}

StringName &StringName::operator=(const StringName &p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        ref_data();
    }
    return *this;
}

StringName &StringName::operator=(StringName &&p_other) noexcept {
    if (this != &p_other) {
        unref_data();
        data_ = p_other.data_;
        p_other.data_ = nullptr;
    }
    return *this;
}

} // namespace Beyota
