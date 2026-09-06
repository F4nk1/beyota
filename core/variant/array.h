/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  array.h                                                               */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <initializer_list>

namespace Beyota {

class Variant;
class String;
struct ArrayPrivate;

class Array {
    mutable ArrayPrivate *_p{nullptr};

    void _ref(const Array &p_from) const noexcept;
    void _unref() const noexcept;
    void _ensure_p();

public:
    Array() noexcept;
    Array(const Array &p_other) noexcept;
    Array(Array &&p_other) noexcept;
    Array(std::initializer_list<Variant> p_list);
    ~Array() noexcept;

    Array &operator=(const Array &p_other) noexcept;
    Array &operator=(Array &&p_other) noexcept;

    [[nodiscard]] usize size() const noexcept;
    [[nodiscard]] bool is_empty() const noexcept;
    void clear() noexcept;
    void resize(usize p_size);

    [[nodiscard]] const Variant &operator[](usize p_idx) const;
    [[nodiscard]] Variant &operator[](usize p_idx);

    void push_back(const Variant &p_value);
    void push_back(Variant &&p_value);
    void append(const Variant &p_value) { push_back(p_value); }

    [[nodiscard]] Variant pop_back();
    void push_front(const Variant &p_value);
    [[nodiscard]] Variant pop_front();

    void insert(usize p_pos, const Variant &p_value);
    void remove_at(usize p_pos);
    bool erase(const Variant &p_value);

    [[nodiscard]] bool has(const Variant &p_value) const;
    [[nodiscard]] i64 find(const Variant &p_value, usize p_from = 0) const;
    [[nodiscard]] usize count(const Variant &p_value) const;

    [[nodiscard]] Array duplicate(bool p_deep = false) const;
    [[nodiscard]] Array slice(usize p_begin, usize p_end = static_cast<usize>(-1)) const;
    void reverse();

    [[nodiscard]] bool operator==(const Array &p_other) const noexcept;
    [[nodiscard]] bool operator!=(const Array &p_other) const noexcept { return !(*this == p_other); }

    [[nodiscard]] u32 hash() const noexcept;

    // Iterators
    struct ConstIterator {
        const Array *_arr{nullptr};
        usize _idx{0};

        [[nodiscard]] const Variant &operator*() const;
        ConstIterator &operator++() noexcept { ++_idx; return *this; }
        ConstIterator operator++(int) noexcept { ConstIterator tmp = *this; ++_idx; return tmp; }
        [[nodiscard]] bool operator==(const ConstIterator &p_other) const noexcept { return _arr == p_other._arr && _idx == p_other._idx; }
        [[nodiscard]] bool operator!=(const ConstIterator &p_other) const noexcept { return !(*this == p_other); }
    };

    struct Iterator {
        Array *_arr{nullptr};
        usize _idx{0};

        [[nodiscard]] Variant &operator*() const;
        Iterator &operator++() noexcept { ++_idx; return *this; }
        Iterator operator++(int) noexcept { Iterator tmp = *this; ++_idx; return tmp; }
        [[nodiscard]] bool operator==(const Iterator &p_other) const noexcept { return _arr == p_other._arr && _idx == p_other._idx; }
        [[nodiscard]] bool operator!=(const Iterator &p_other) const noexcept { return !(*this == p_other); }
    };

    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator{this, 0}; }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator{this, size()}; }
    [[nodiscard]] Iterator begin() noexcept { return Iterator{this, 0}; }
    [[nodiscard]] Iterator end() noexcept { return Iterator{this, size()}; }
};

} // namespace Beyota
