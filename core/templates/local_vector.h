/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  local_vector.h                                                        */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <new>
#include <type_traits>
#include <utility>

namespace Beyota {

template <typename T, typename U = u32>
class LocalVector {
    U count_{0};
    U capacity_{0};
    T *data_{nullptr};

    void reallocate(U p_new_capacity) {
        if (p_new_capacity == 0) {
            if (data_ != nullptr) {
                ::operator delete(static_cast<void *>(data_));
                data_ = nullptr;
            }
            capacity_ = 0;
            return;
        }

        T *new_data = static_cast<T *>(::operator new(sizeof(T) * p_new_capacity));
        if (data_ != nullptr) {
            if constexpr (std::is_trivially_copyable_v<T>) {
                std::memcpy(new_data, data_, sizeof(T) * count_);
            } else {
                for (U i = 0; i < count_; ++i) {
                    new (&new_data[i]) T(std::move(data_[i]));
                    data_[i].~T();
                }
            }
            ::operator delete(static_cast<void *>(data_));
        }
        data_ = new_data;
        capacity_ = p_new_capacity;
    }

    template <bool Init>
    void resize_internal(U p_size) {
        if (p_size < count_) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (U i = p_size; i < count_; ++i) {
                    data_[i].~T();
                }
            }
            count_ = p_size;
        } else if (p_size > count_) {
            reserve(p_size);
            if constexpr (Init) {
                for (U i = count_; i < p_size; ++i) {
                    new (&data_[i]) T();
                }
            }
            count_ = p_size;
        }
    }

public:
    LocalVector() noexcept = default;

    LocalVector(std::initializer_list<T> p_init) {
        reserve(static_cast<U>(p_init.size()));
        for (const T &val : p_init) {
            push_back(val);
        }
    }

    LocalVector(const LocalVector &p_other) {
        if (p_other.count_ > 0) {
            reserve(p_other.count_);
            for (U i = 0; i < p_other.count_; ++i) {
                new (&data_[i]) T(p_other.data_[i]);
            }
            count_ = p_other.count_;
        }
    }

    LocalVector(LocalVector &&p_other) noexcept
        : count_(p_other.count_),
          capacity_(p_other.capacity_),
          data_(p_other.data_) {
        p_other.count_ = 0;
        p_other.capacity_ = 0;
        p_other.data_ = nullptr;
    }

    ~LocalVector() noexcept {
        reset();
    }

    LocalVector &operator=(const LocalVector &p_other) {
        if (this != &p_other) {
            clear();
            if (p_other.count_ > 0) {
                reserve(p_other.count_);
                for (U i = 0; i < p_other.count_; ++i) {
                    new (&data_[i]) T(p_other.data_[i]);
                }
                count_ = p_other.count_;
            }
        }
        return *this;
    }

    LocalVector &operator=(LocalVector &&p_other) noexcept {
        if (this != &p_other) {
            reset();
            count_ = p_other.count_;
            capacity_ = p_other.capacity_;
            data_ = p_other.data_;

            p_other.count_ = 0;
            p_other.capacity_ = 0;
            p_other.data_ = nullptr;
        }
        return *this;
    }

    [[nodiscard]] T *ptr() noexcept { return data_; }
    [[nodiscard]] const T *ptr() const noexcept { return data_; }
    [[nodiscard]] U size() const noexcept { return count_; }
    [[nodiscard]] bool is_empty() const noexcept { return count_ == 0; }
    [[nodiscard]] U get_capacity() const noexcept { return capacity_; }

    [[nodiscard]] T &operator[](U p_index) noexcept {
        assert(p_index < count_);
        return data_[p_index];
    }

    [[nodiscard]] const T &operator[](U p_index) const noexcept {
        assert(p_index < count_);
        return data_[p_index];
    }

    void reserve(U p_size) {
        if (p_size > capacity_) {
            U new_cap = (capacity_ == 0) ? 2 : (capacity_ + (capacity_ >> 1));
            if (new_cap < p_size) {
                new_cap = p_size;
            }
            reallocate(new_cap);
        }
    }

    void push_back(const T &p_elem) {
        if (count_ == capacity_) {
            T copy(p_elem);
            reserve(count_ + 1);
            new (&data_[count_++]) T(std::move(copy));
        } else {
            new (&data_[count_++]) T(p_elem);
        }
    }

    void push_back(T &&p_elem) {
        if (count_ == capacity_) {
            T copy(std::move(p_elem));
            reserve(count_ + 1);
            new (&data_[count_++]) T(std::move(copy));
        } else {
            new (&data_[count_++]) T(std::move(p_elem));
        }
    }

    void pop_back() noexcept {
        assert(count_ > 0);
        count_--;
        if constexpr (!std::is_trivially_destructible_v<T>) {
            data_[count_].~T();
        }
    }

    void insert(U p_pos, const T &p_val) {
        assert(p_pos <= count_);
        if (p_pos == count_) {
            push_back(p_val);
            return;
        }
        T copy(p_val);
        resize(count_ + 1);
        for (U i = count_ - 1; i > p_pos; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[p_pos] = std::move(copy);
    }

    void insert(U p_pos, T &&p_val) {
        assert(p_pos <= count_);
        if (p_pos == count_) {
            push_back(std::move(p_val));
            return;
        }
        T copy(std::move(p_val));
        resize(count_ + 1);
        for (U i = count_ - 1; i > p_pos; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[p_pos] = std::move(copy);
    }

    void remove_at(U p_index) {
        assert(p_index < count_);
        count_--;
        for (U i = p_index; i < count_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        if constexpr (!std::is_trivially_destructible_v<T>) {
            data_[count_].~T();
        }
    }

    void remove_at_unordered(U p_index) {
        assert(p_index < count_);
        count_--;
        if (count_ > p_index) {
            data_[p_index] = std::move(data_[count_]);
        }
        if constexpr (!std::is_trivially_destructible_v<T>) {
            data_[count_].~T();
        }
    }

    [[nodiscard]] i64 find(const T &p_val, i64 p_from = 0) const noexcept {
        if (p_from < 0) {
            p_from = (i64)count_ + p_from;
        }
        if (p_from < 0 || p_from >= (i64)count_) {
            return -1;
        }
        for (U i = (U)p_from; i < count_; ++i) {
            if (data_[i] == p_val) {
                return (i64)i;
            }
        }
        return -1;
    }

    [[nodiscard]] bool has(const T &p_val) const noexcept {
        return find(p_val) != -1;
    }

    bool erase(const T &p_val) {
        i64 idx = find(p_val);
        if (idx >= 0) {
            remove_at(static_cast<U>(idx));
            return true;
        }
        return false;
    }

    bool erase_unordered(const T &p_val) {
        i64 idx = find(p_val);
        if (idx >= 0) {
            remove_at_unordered(static_cast<U>(idx));
            return true;
        }
        return false;
    }

    U erase_multiple_unordered(const T &p_val) {
        U from = 0;
        U occurrences = 0;
        while (true) {
            i64 idx = find(p_val, from);
            if (idx == -1) {
                break;
            }
            remove_at_unordered(static_cast<U>(idx));
            from = static_cast<U>(idx);
            occurrences++;
        }
        return occurrences;
    }

    void reverse() noexcept {
        for (U i = 0; i < count_ / 2; ++i) {
            std::swap(data_[i], data_[count_ - i - 1]);
        }
    }

    void clear() noexcept {
        resize_internal<false>(0);
    }

    void reset() noexcept {
        clear();
        if (data_ != nullptr) {
            ::operator delete(static_cast<void *>(data_));
            data_ = nullptr;
            capacity_ = 0;
        }
    }

    void resize(U p_size) {
        resize_internal<!std::is_trivially_constructible_v<T>>(p_size);
    }

    void resize_initialized(U p_size) {
        resize_internal<true>(p_size);
    }

    void resize_uninitialized(U p_size) {
        static_assert(std::is_trivially_destructible_v<T>, "Type must be trivially destructible for uninitialized resize");
        resize_internal<false>(p_size);
    }

    template <typename Comparator>
    void sort_custom(Comparator p_comp) {
        if (count_ > 1) {
            std::sort(data_, data_ + count_, p_comp);
        }
    }

    void sort() {
        if (count_ > 1) {
            std::sort(data_, data_ + count_);
        }
    }

    void ordered_insert(const T &p_val) {
        auto it = std::lower_bound(data_, data_ + count_, p_val);
        U idx = static_cast<U>(it - data_);
        insert(idx, p_val);
    }

    [[nodiscard]] T *begin() noexcept { return data_; }
    [[nodiscard]] T *end() noexcept { return data_ + count_; }
    [[nodiscard]] const T *begin() const noexcept { return data_; }
    [[nodiscard]] const T *end() const noexcept { return data_ + count_; }
    [[nodiscard]] const T *cbegin() const noexcept { return data_; }
    [[nodiscard]] const T *cend() const noexcept { return data_ + count_; }
};

} // namespace Beyota
