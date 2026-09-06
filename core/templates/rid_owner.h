/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  rid_owner.h                                                           */
/**************************************************************************/

#pragma once

#include "core/templates/local_vector.h"
#include "core/templates/rid.h"
#include "core/type_primitives.h"

#include <algorithm>
#include <cassert>
#include <mutex>
#include <new>

namespace Beyota {

template <typename T, bool THREAD_SAFE = false>
class RID_Owner {
    struct Slot {
        alignas(T) u8 storage[sizeof(T)];
        u32 validator{1};
        bool initialized{false};

        T *get_ptr() noexcept {
            return reinterpret_cast<T *>(storage);
        }

        const T *get_ptr() const noexcept {
            return reinterpret_cast<const T *>(storage);
        }
    };

    LocalVector<Slot *> chunks_;
    LocalVector<u32> free_slots_;
    usize elements_per_chunk_{64};
    u32 total_slots_{0};
    u32 alloc_count_{0};
    mutable std::mutex mutex_;

    Slot *get_slot(u32 p_idx) const noexcept {
        usize chunk_idx = p_idx / elements_per_chunk_;
        usize elem_idx = p_idx % elements_per_chunk_;
        return &chunks_[chunk_idx][elem_idx];
    }

    void allocate_new_chunk() {
        Slot *chunk = static_cast<Slot *>(::operator new(sizeof(Slot) * elements_per_chunk_));
        for (usize i = 0; i < elements_per_chunk_; ++i) {
            new (&chunk[i]) Slot();
        }
        chunks_.push_back(chunk);
    }

    void free_all() noexcept {
        for (usize i = 0; i < total_slots_; ++i) {
            Slot *slot = get_slot((u32)i);
            if (slot->initialized) {
                slot->get_ptr()->~T();
                slot->initialized = false;
            }
        }
        for (usize i = 0; i < chunks_.size(); ++i) {
            for (usize j = 0; j < elements_per_chunk_; ++j) {
                chunks_[i][j].~Slot();
            }
            ::operator delete(static_cast<void *>(chunks_[i]));
        }
        chunks_.reset();
        free_slots_.reset();
        total_slots_ = 0;
        alloc_count_ = 0;
    }

public:
    explicit RID_Owner(usize p_target_chunk_bytes = 65536) {
        usize slot_size = sizeof(Slot);
        elements_per_chunk_ = std::max<usize>(1, p_target_chunk_bytes / slot_size);
    }

    RID_Owner(const RID_Owner &) = delete;
    RID_Owner &operator=(const RID_Owner &) = delete;

    ~RID_Owner() noexcept {
        free_all();
    }

    [[nodiscard]] usize get_elements_in_chunk() const noexcept {
        return elements_per_chunk_;
    }

    [[nodiscard]] u32 get_alloc_count() const noexcept {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            return alloc_count_;
        } else {
            return alloc_count_;
        }
    }

    RID allocate_rid() {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            return allocate_rid_internal();
        } else {
            return allocate_rid_internal();
        }
    }

private:
    RID allocate_rid_internal() {
        u32 slot_idx;
        if (!free_slots_.is_empty()) {
            slot_idx = free_slots_[free_slots_.size() - 1];
            free_slots_.pop_back();
        } else {
            slot_idx = total_slots_++;
            usize needed_chunks = (total_slots_ + elements_per_chunk_ - 1) / elements_per_chunk_;
            while (chunks_.size() < needed_chunks) {
                allocate_new_chunk();
            }
        }

        Slot *slot = get_slot(slot_idx);
        alloc_count_++;
        u64 full_id = (static_cast<u64>(slot->validator) << 32) | static_cast<u64>(slot_idx);
        return RID::from_uint64(full_id);
    }

public:
    void initialize_rid(RID p_rid) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            initialize_rid_internal(p_rid);
        } else {
            initialize_rid_internal(p_rid);
        }
    }

    void initialize_rid(RID p_rid, const T &p_value) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            initialize_rid_internal(p_rid, p_value);
        } else {
            initialize_rid_internal(p_rid, p_value);
        }
    }

    void initialize_rid(RID p_rid, T &&p_value) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            initialize_rid_internal(p_rid, std::move(p_value));
        } else {
            initialize_rid_internal(p_rid, std::move(p_value));
        }
    }

private:
    void initialize_rid_internal(RID p_rid) {
        Slot *slot = get_valid_slot(p_rid);
        assert(slot != nullptr);
        assert(!slot->initialized);
        new (slot->storage) T();
        slot->initialized = true;
    }

    void initialize_rid_internal(RID p_rid, const T &p_value) {
        Slot *slot = get_valid_slot(p_rid);
        assert(slot != nullptr);
        assert(!slot->initialized);
        new (slot->storage) T(p_value);
        slot->initialized = true;
    }

    void initialize_rid_internal(RID p_rid, T &&p_value) {
        Slot *slot = get_valid_slot(p_rid);
        assert(slot != nullptr);
        assert(!slot->initialized);
        new (slot->storage) T(std::move(p_value));
        slot->initialized = true;
    }

    Slot *get_valid_slot(RID p_rid) const noexcept {
        if (p_rid.is_null()) {
            return nullptr;
        }
        u32 slot_idx = p_rid.get_local_index();
        if (slot_idx >= total_slots_) {
            return nullptr;
        }
        Slot *slot = get_slot(slot_idx);
        u32 expected_validator = static_cast<u32>(p_rid.get_id() >> 32);
        if (slot->validator != expected_validator) {
            return nullptr;
        }
        return slot;
    }

public:
    RID make_rid() {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid);
            return rid;
        } else {
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid);
            return rid;
        }
    }

    RID make_rid(const T &p_value) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid, p_value);
            return rid;
        } else {
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid, p_value);
            return rid;
        }
    }

    RID make_rid(T &&p_value) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid, std::move(p_value));
            return rid;
        } else {
            RID rid = allocate_rid_internal();
            initialize_rid_internal(rid, std::move(p_value));
            return rid;
        }
    }

    [[nodiscard]] T *get_or_null(RID p_rid) noexcept {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            Slot *slot = get_valid_slot(p_rid);
            return (slot != nullptr && slot->initialized) ? slot->get_ptr() : nullptr;
        } else {
            Slot *slot = get_valid_slot(p_rid);
            return (slot != nullptr && slot->initialized) ? slot->get_ptr() : nullptr;
        }
    }

    [[nodiscard]] const T *get_or_null(RID p_rid) const noexcept {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            Slot *slot = get_valid_slot(p_rid);
            return (slot != nullptr && slot->initialized) ? slot->get_ptr() : nullptr;
        } else {
            Slot *slot = get_valid_slot(p_rid);
            return (slot != nullptr && slot->initialized) ? slot->get_ptr() : nullptr;
        }
    }

    [[nodiscard]] T *get(RID p_rid) {
        T *ptr = get_or_null(p_rid);
        assert(ptr != nullptr);
        return ptr;
    }

    [[nodiscard]] const T *get(RID p_rid) const {
        const T *ptr = get_or_null(p_rid);
        assert(ptr != nullptr);
        return ptr;
    }

    [[nodiscard]] bool owns(RID p_rid) const noexcept {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            Slot *slot = get_valid_slot(p_rid);
            return slot != nullptr && slot->initialized;
        } else {
            Slot *slot = get_valid_slot(p_rid);
            return slot != nullptr && slot->initialized;
        }
    }

    void free(RID p_rid) {
        if constexpr (THREAD_SAFE) {
            std::lock_guard<std::mutex> lock(mutex_);
            free_internal(p_rid);
        } else {
            free_internal(p_rid);
        }
    }

private:
    void free_internal(RID p_rid) {
        Slot *slot = get_valid_slot(p_rid);
        if (slot == nullptr) {
            return;
        }
        if (slot->initialized) {
            slot->get_ptr()->~T();
            slot->initialized = false;
        }
        slot->validator++;
        if (slot->validator == 0) {
            slot->validator = 1;
        }
        free_slots_.push_back(p_rid.get_local_index());
        alloc_count_--;
    }
};

} // namespace Beyota
