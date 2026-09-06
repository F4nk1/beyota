/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  paged_allocator.h                                                     */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <bit>
#include <cassert>
#include <mutex>
#include <new>
#include <type_traits>
#include <utility>

namespace Beyota {

template <typename T, bool THREAD_SAFE = false, u32 DEFAULT_PAGE_SIZE = 4096>
class PagedAllocator {
    T **page_pool_{nullptr};
    T ***available_pool_{nullptr};
    u32 pages_allocated_{0};
    u32 allocs_available_{0};

    u32 page_shift_{0};
    u32 page_mask_{0};
    u32 page_size_{0};
    mutable std::mutex mutex_;

    void free_all(bool p_allow_unfreed) {
        if (!p_allow_unfreed || !std::is_trivially_destructible_v<T>) {
            assert(allocs_available_ == pages_allocated_ * page_size_);
        }
        if (pages_allocated_ > 0) {
            for (u32 i = 0; i < pages_allocated_; ++i) {
                ::operator delete(static_cast<void *>(page_pool_[i]));
                ::operator delete(static_cast<void *>(available_pool_[i]));
            }
            ::operator delete(static_cast<void *>(page_pool_));
            ::operator delete(static_cast<void *>(available_pool_));
            page_pool_ = nullptr;
            available_pool_ = nullptr;
            pages_allocated_ = 0;
            allocs_available_ = 0;
        }
    }

public:
    explicit PagedAllocator(u32 p_page_size = DEFAULT_PAGE_SIZE) {
        configure(p_page_size);
    }

    PagedAllocator(const PagedAllocator &) = delete;
    PagedAllocator &operator=(const PagedAllocator &) = delete;

    ~PagedAllocator() noexcept {
        reset(true);
    }

    void configure(u32 p_page_size) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            configure_internal(p_page_size);
        } else {
            configure_internal(p_page_size);
        }
    }

    [[nodiscard]] bool is_configured() const noexcept {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            return page_size_ > 0;
        } else {
            return page_size_ > 0;
        }
    }

    template <typename... Args>
    T *alloc(Args &&...p_args) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            return alloc_internal(std::forward<Args>(p_args)...);
        } else {
            return alloc_internal(std::forward<Args>(p_args)...);
        }
    }

    void free(T *p_mem) {
        if (p_mem == nullptr) {
            return;
        }
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            free_internal(p_mem);
        } else {
            free_internal(p_mem);
        }
    }

    template <typename... Args>
    T *new_allocation(Args &&...p_args) {
        return alloc(std::forward<Args>(p_args)...);
    }

    void delete_allocation(T *p_mem) {
        free(p_mem);
    }

    void reset(bool p_allow_unfreed = false) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            free_all(p_allow_unfreed);
        } else {
            free_all(p_allow_unfreed);
        }
    }

private:
    void configure_internal(u32 p_page_size) {
        assert(page_pool_ == nullptr);
        assert(p_page_size > 0);
        u32 power = std::bit_ceil(p_page_size);
        page_size_ = power;
        page_mask_ = page_size_ - 1;
        page_shift_ = (u32)std::countr_zero(page_size_);
    }

    template <typename... Args>
    T *alloc_internal(Args &&...p_args) {
        if (allocs_available_ == 0) {
            u32 pages_used = pages_allocated_;
            pages_allocated_++;

            T **new_page_pool = static_cast<T **>(::operator new(sizeof(T *) * pages_allocated_));
            T ***new_available_pool = static_cast<T ***>(::operator new(sizeof(T **) * pages_allocated_));

            if (page_pool_ != nullptr) {
                std::memcpy(new_page_pool, page_pool_, sizeof(T *) * pages_used);
                std::memcpy(new_available_pool, available_pool_, sizeof(T **) * pages_used);
                ::operator delete(static_cast<void *>(page_pool_));
                ::operator delete(static_cast<void *>(available_pool_));
            }

            page_pool_ = new_page_pool;
            available_pool_ = new_available_pool;

            page_pool_[pages_used] = static_cast<T *>(::operator new(sizeof(T) * page_size_));
            available_pool_[pages_used] = static_cast<T **>(::operator new(sizeof(T *) * page_size_));

            for (u32 i = 0; i < page_size_; ++i) {
                available_pool_[0][i] = &page_pool_[pages_used][i];
            }
            allocs_available_ += page_size_;
        }

        allocs_available_--;
        T *mem = available_pool_[allocs_available_ >> page_shift_][allocs_available_ & page_mask_];
        new (mem) T(std::forward<Args>(p_args)...);
        return mem;
    }

    void free_internal(T *p_mem) {
        p_mem->~T();
        available_pool_[allocs_available_ >> page_shift_][allocs_available_ & page_mask_] = p_mem;
        allocs_available_++;
    }
};

} // namespace Beyota
