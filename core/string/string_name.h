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
#include <string_view>

namespace Beyota {

class StringName {
    friend struct StringNameTable;

public:
    struct Data {
        std::atomic<u32> refcount{1};
        u32 hash{0};
        String name;

        Data(String p_name, u32 p_hash) : hash(p_hash), name(std::move(p_name)) {}
    };

private:

    Data *data_{nullptr};

    [[nodiscard]] static Data *get_or_create(std::string_view p_name);
    void ref_data() noexcept;
    void unref_data() noexcept;

public:
    constexpr StringName() noexcept = default;

    StringName(const char *p_str);
    StringName(std::string_view p_view);
    StringName(const String &p_str);
    StringName(const StringName &p_other) noexcept;
    StringName(StringName &&p_other) noexcept;
    ~StringName() noexcept;

    StringName &operator=(const StringName &p_other) noexcept;
    StringName &operator=(StringName &&p_other) noexcept;

    [[nodiscard]] bool is_empty() const noexcept {
        return data_ == nullptr;
    }

    [[nodiscard]] u32 hash() const noexcept {
        return data_ ? data_->hash : 0;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        return data_ ? data_->name.c_str() : "";
    }

    [[nodiscard]] std::string_view as_string_view() const noexcept {
        return data_ ? data_->name.as_string_view() : std::string_view{};
    }

    [[nodiscard]] operator std::string_view() const noexcept {
        return as_string_view();
    }

    [[nodiscard]] operator String() const {
        return data_ ? data_->name : String();
    }

    [[nodiscard]] bool operator==(const StringName &p_other) const noexcept {
        return data_ == p_other.data_; // O(1) comparison!
    }

    [[nodiscard]] bool operator==(std::string_view p_other) const noexcept {
        return as_string_view() == p_other;
    }

    [[nodiscard]] bool operator==(const char *p_other) const noexcept {
        return as_string_view() == (p_other ? std::string_view(p_other) : std::string_view{});
    }

    [[nodiscard]] auto operator<=>(const StringName &p_other) const noexcept {
        if (data_ == p_other.data_) {
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
