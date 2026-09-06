/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  typed_dictionary.h                                                    */
/**************************************************************************/

#pragma once

#include "core/variant/dictionary.h"
#include "core/variant/variant.h"

namespace Beyota {

template <typename K, typename V>
class TypedDictionary {
    Dictionary _dict;

public:
    TypedDictionary() = default;
    TypedDictionary(const Dictionary &p_dict) : _dict(p_dict) {}
    TypedDictionary(Dictionary &&p_dict) noexcept : _dict(std::move(p_dict)) {}

    [[nodiscard]] usize size() const noexcept { return _dict.size(); }
    [[nodiscard]] bool is_empty() const noexcept { return _dict.is_empty(); }
    void clear() noexcept { _dict.clear(); }

    [[nodiscard]] V operator[](const K &p_key) const {
        return static_cast<V>(_dict[Variant(p_key)]);
    }

    void set(const K &p_key, const V &p_val) {
        _dict[Variant(p_key)] = Variant(p_val);
    }

    [[nodiscard]] bool has(const K &p_key) const {
        return _dict.has(Variant(p_key));
    }

    bool erase(const K &p_key) {
        return _dict.erase(Variant(p_key));
    }

    [[nodiscard]] const Dictionary &to_dictionary() const noexcept { return _dict; }
    [[nodiscard]] operator Dictionary() const noexcept { return _dict; }
    [[nodiscard]] operator Variant() const { return Variant(_dict); }
};

} // namespace Beyota
