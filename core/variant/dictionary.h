/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  dictionary.h                                                          */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <initializer_list>
#include <utility>

namespace Beyota {

class Variant;
class Array;
struct DictionaryPrivate;

class Dictionary {
    mutable DictionaryPrivate *_p{nullptr};

    void _ref(const Dictionary &p_from) const noexcept;
    void _unref() const noexcept;
    void _ensure_p();

public:
    Dictionary() noexcept;
    Dictionary(const Dictionary &p_other) noexcept;
    Dictionary(Dictionary &&p_other) noexcept;
    Dictionary(std::initializer_list<std::pair<Variant, Variant>> p_list);
    ~Dictionary() noexcept;

    Dictionary &operator=(const Dictionary &p_other) noexcept;
    Dictionary &operator=(Dictionary &&p_other) noexcept;

    [[nodiscard]] usize size() const noexcept;
    [[nodiscard]] bool is_empty() const noexcept;
    void clear() noexcept;

    [[nodiscard]] const Variant &operator[](const Variant &p_key) const;
    [[nodiscard]] Variant &operator[](const Variant &p_key);

    [[nodiscard]] const Variant *getptr(const Variant &p_key) const noexcept;
    [[nodiscard]] Variant *getptr(const Variant &p_key) noexcept;

    [[nodiscard]] Variant get(const Variant &p_key, const Variant &p_default) const;
    [[nodiscard]] Variant &get_or_add(const Variant &p_key, const Variant &p_default);

    [[nodiscard]] bool has(const Variant &p_key) const;
    bool erase(const Variant &p_key);

    [[nodiscard]] Array keys() const;
    [[nodiscard]] Array values() const;

    [[nodiscard]] Dictionary duplicate(bool p_deep = false) const;

    [[nodiscard]] bool operator==(const Dictionary &p_other) const noexcept;
    [[nodiscard]] bool operator!=(const Dictionary &p_other) const noexcept { return !(*this == p_other); }

    [[nodiscard]] u32 hash() const noexcept;

    struct Entry {
        const Variant &key;
        const Variant &value;
    };

    // Iterators over keys/values preserving insertion order
    struct ConstIterator {
        const void *node_{nullptr};

        [[nodiscard]] const Variant &key() const;
        [[nodiscard]] const Variant &value() const;
        [[nodiscard]] Entry operator*() const { return Entry{key(), value()}; }
        ConstIterator &operator++() noexcept;
        ConstIterator operator++(int) noexcept;
        [[nodiscard]] bool operator==(const ConstIterator &p_other) const noexcept { return node_ == p_other.node_; }
        [[nodiscard]] bool operator!=(const ConstIterator &p_other) const noexcept { return node_ != p_other.node_; }
    };

    [[nodiscard]] ConstIterator begin() const noexcept;
    [[nodiscard]] ConstIterator end() const noexcept;
};

} // namespace Beyota
