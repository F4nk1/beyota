/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  packed_arrays.h                                                       */
/**************************************************************************/

#pragma once

#include "core/math/math_all.h"
#include "core/string/ustring.h"
#include "core/type_primitives.h"

#include <atomic>
#include <cassert>
#include <compare>
#include <initializer_list>
#include <span>
#include <vector>

namespace Beyota {

using Math::Vector2;
using Math::Vector3;
using Math::Vector4;
using Math::Color;

template <typename T>
class PackedArray {
    struct CowData {
        std::atomic<u32> refcount{1};
        std::vector<T> vec;

        CowData() = default;
        explicit CowData(usize p_size) : vec(p_size) {}
        explicit CowData(const std::vector<T> &p_vec) : vec(p_vec) {}
        explicit CowData(std::vector<T> &&p_vec) : vec(std::move(p_vec)) {}
    };

    CowData *_data{nullptr};

    void _ref(CowData *p_data) noexcept {
        if (p_data != nullptr) {
            p_data->refcount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void _unref() noexcept {
        if (_data != nullptr) {
            if (_data->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete _data;
            }
            _data = nullptr;
        }
    }

    void _detach_if_shared() {
        if (_data == nullptr) {
            _data = new CowData();
        } else if (_data->refcount.load(std::memory_order_relaxed) > 1) {
            CowData *new_data = new CowData(_data->vec);
            _unref();
            _data = new_data;
        }
    }

public:
    PackedArray() noexcept = default;

    explicit PackedArray(usize p_size) {
        if (p_size > 0) {
            _data = new CowData(p_size);
        }
    }

    PackedArray(std::initializer_list<T> p_list) {
        if (p_list.size() > 0) {
            _data = new CowData();
            _data->vec.assign(p_list.begin(), p_list.end());
        }
    }

    explicit PackedArray(std::span<const T> p_span) {
        if (!p_span.empty()) {
            _data = new CowData();
            _data->vec.assign(p_span.begin(), p_span.end());
        }
    }

    PackedArray(const PackedArray &p_other) noexcept : _data(p_other._data) {
        _ref(_data);
    }

    PackedArray(PackedArray &&p_other) noexcept : _data(p_other._data) {
        p_other._data = nullptr;
    }

    ~PackedArray() noexcept {
        _unref();
    }

    PackedArray &operator=(const PackedArray &p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            _ref(_data);
        }
        return *this;
    }

    PackedArray &operator=(PackedArray &&p_other) noexcept {
        if (this != &p_other) {
            _unref();
            _data = p_other._data;
            p_other._data = nullptr;
        }
        return *this;
    }

    [[nodiscard]] usize size() const noexcept {
        return _data ? _data->vec.size() : 0;
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return _data == nullptr || _data->vec.empty();
    }

    void clear() noexcept {
        _unref();
    }

    void resize(usize p_new_size) {
        _detach_if_shared();
        _data->vec.resize(p_new_size);
    }

    void push_back(const T &p_val) {
        _detach_if_shared();
        _data->vec.push_back(p_val);
    }

    void push_back(T &&p_val) {
        _detach_if_shared();
        _data->vec.push_back(std::move(p_val));
    }

    void append(const T &p_val) {
        push_back(p_val);
    }

    [[nodiscard]] const T &operator[](usize p_idx) const noexcept {
        assert(_data && p_idx < _data->vec.size());
        return _data->vec[p_idx];
    }

    [[nodiscard]] T &operator[](usize p_idx) {
        assert(_data && p_idx < _data->vec.size());
        _detach_if_shared();
        return _data->vec[p_idx];
    }

    [[nodiscard]] const T *ptr() const noexcept {
        return (_data && !_data->vec.empty()) ? _data->vec.data() : nullptr;
    }

    [[nodiscard]] T *ptrw() {
        if (is_empty()) return nullptr;
        _detach_if_shared();
        return _data->vec.data();
    }

    void fill(const T &p_val) {
        if (is_empty()) return;
        _detach_if_shared();
        std::fill(_data->vec.begin(), _data->vec.end(), p_val);
    }

    [[nodiscard]] PackedArray slice(usize p_begin, usize p_end = static_cast<usize>(-1)) const {
        PackedArray res;
        if (is_empty() || p_begin >= size()) return res;
        usize end = std::min(p_end, size());
        if (end <= p_begin) return res;
        res.resize(end - p_begin);
        for (usize i = p_begin; i < end; ++i) {
            res[i - p_begin] = (*this)[i];
        }
        return res;
    }

    [[nodiscard]] PackedArray duplicate() const {
        PackedArray res;
        if (!is_empty()) {
            res._data = new CowData(_data->vec);
        }
        return res;
    }

    [[nodiscard]] bool operator==(const PackedArray &p_other) const noexcept {
        if (_data == p_other._data) return true;
        if (size() != p_other.size()) return false;
        return _data->vec == p_other._data->vec;
    }

    [[nodiscard]] auto operator<=>(const PackedArray &p_other) const noexcept {
        if (_data == p_other._data) return std::strong_ordering::equal;
        if (is_empty() && p_other.is_empty()) return std::strong_ordering::equal;
        if (is_empty()) return std::strong_ordering::less;
        if (p_other.is_empty()) return std::strong_ordering::greater;
        return _data->vec <=> p_other._data->vec;
    }

    // Iterators
    [[nodiscard]] const T *begin() const noexcept { return ptr(); }
    [[nodiscard]] const T *end() const noexcept { return ptr() + size(); }
};

using PackedByteArray    = PackedArray<u8>;
using PackedInt32Array   = PackedArray<i32>;
using PackedInt64Array   = PackedArray<i64>;
using PackedFloat32Array = PackedArray<f32>;
using PackedFloat64Array = PackedArray<f64>;
using PackedStringArray  = PackedArray<String>;
using PackedVector2Array = PackedArray<Vector2>;
using PackedVector3Array = PackedArray<Vector3>;
using PackedColorArray   = PackedArray<Color>;
using PackedVector4Array = PackedArray<Vector4>;

} // namespace Beyota
