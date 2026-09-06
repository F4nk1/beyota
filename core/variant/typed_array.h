/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  typed_array.h                                                         */
/**************************************************************************/

#pragma once

#include "core/variant/array.h"
#include "core/variant/variant.h"

namespace Beyota {

template <typename T>
class TypedArray {
    Array _array;

public:
    TypedArray() = default;
    TypedArray(const Array &p_array) : _array(p_array) {}
    TypedArray(Array &&p_array) noexcept : _array(std::move(p_array)) {}

    [[nodiscard]] usize size() const noexcept { return _array.size(); }
    [[nodiscard]] bool is_empty() const noexcept { return _array.is_empty(); }
    void clear() noexcept { _array.clear(); }
    void resize(usize p_size) { _array.resize(p_size); }

    [[nodiscard]] T operator[](usize p_idx) const {
        return static_cast<T>(_array[p_idx]);
    }

    void push_back(const T &p_val) {
        _array.push_back(Variant(p_val));
    }

    void append(const T &p_val) {
        push_back(p_val);
    }

    [[nodiscard]] const Array &to_array() const noexcept { return _array; }
    [[nodiscard]] operator Array() const noexcept { return _array; }
    [[nodiscard]] operator Variant() const { return Variant(_array); }
};

} // namespace Beyota
