/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  object_db.cpp                                                         */
/**************************************************************************/

#include "core/object/object_db.h"
#include "core/object/object.h"

#include <cstdlib>

namespace Beyota {

constexpr u32 INITIAL_SLOT_CAPACITY = 1024;
constexpr u32 NO_FREE_SLOT = 0xFFFFFFFF;

void ObjectDB::setup() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (slots_ != nullptr) return;

    slot_max_ = INITIAL_SLOT_CAPACITY;
    slots_ = (ObjectSlot *)::malloc(sizeof(ObjectSlot) * slot_max_);
    assert(slots_ != nullptr);

    for (u32 i = 0; i < slot_max_; ++i) {
        slots_[i].validator = 1;
        slots_[i].next_free = (i + 1 < slot_max_) ? (i + 1) : NO_FREE_SLOT;
        slots_[i].is_ref_counted = false;
        slots_[i].object = nullptr;
    }
    free_list_head_ = 0;
    slot_count_ = 0;
    validator_counter_ = 1;
}

void ObjectDB::cleanup() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (slots_ == nullptr) return;

    ::free(slots_);
    slots_ = nullptr;
    slot_count_ = 0;
    slot_max_ = 0;
    free_list_head_ = NO_FREE_SLOT;
    validator_counter_ = 0;
}

void ObjectDB::grow_slots() {
    u32 new_max = (slot_max_ == 0) ? INITIAL_SLOT_CAPACITY : (slot_max_ * 2);
    assert(new_max <= (1U << SLOT_BITS));

    ObjectSlot *new_slots = (ObjectSlot *)::realloc(slots_, sizeof(ObjectSlot) * new_max);
    assert(new_slots != nullptr);
    slots_ = new_slots;

    for (u32 i = slot_max_; i < new_max; ++i) {
        slots_[i].validator = 1;
        slots_[i].next_free = (i + 1 < new_max) ? (i + 1) : NO_FREE_SLOT;
        slots_[i].is_ref_counted = false;
        slots_[i].object = nullptr;
    }
    free_list_head_ = slot_max_;
    slot_max_ = new_max;
}

ObjectID ObjectDB::add_instance(Object *p_object) {
    assert(p_object != nullptr);

    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (slots_ == nullptr) {
        slot_max_ = INITIAL_SLOT_CAPACITY;
        slots_ = (ObjectSlot *)::malloc(sizeof(ObjectSlot) * slot_max_);
        assert(slots_ != nullptr);
        for (u32 i = 0; i < slot_max_; ++i) {
            slots_[i].validator = 1;
            slots_[i].next_free = (i + 1 < slot_max_) ? (i + 1) : NO_FREE_SLOT;
            slots_[i].is_ref_counted = false;
            slots_[i].object = nullptr;
        }
        free_list_head_ = 0;
        slot_count_ = 0;
        validator_counter_ = 1;
    }

    if (free_list_head_ == NO_FREE_SLOT) {
        grow_slots();
    }

    u32 slot_idx = free_list_head_;
    free_list_head_ = slots_[slot_idx].next_free;

    validator_counter_ = (validator_counter_ + 1) & VALIDATOR_MASK;
    if (validator_counter_ == 0) {
        validator_counter_ = 1;
    }

    slots_[slot_idx].validator = validator_counter_;
    slots_[slot_idx].object = p_object;
    slots_[slot_idx].is_ref_counted = p_object->is_ref_counted();

    u64 id = validator_counter_;
    id <<= SLOT_BITS;
    id |= (u64)slot_idx;
    if (slots_[slot_idx].is_ref_counted) {
        id |= REFERENCE_BIT;
    }

    ++slot_count_;
    return ObjectID(id);
}

void ObjectDB::remove_instance(Object *p_object) {
    assert(p_object != nullptr);
    ObjectID id = p_object->get_instance_id();
    if (id.is_null()) return;

    u64 raw = (u64)id;
    u32 slot_idx = (u32)(raw & SLOT_MASK);

    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (slot_idx >= slot_max_) return;

    u64 validator = (raw >> SLOT_BITS) & VALIDATOR_MASK;
    if (slots_[slot_idx].validator != validator || slots_[slot_idx].object != p_object) {
        return;
    }

    slots_[slot_idx].object = nullptr;
    slots_[slot_idx].validator = (slots_[slot_idx].validator + 1) & VALIDATOR_MASK;
    if (slots_[slot_idx].validator == 0) {
        slots_[slot_idx].validator = 1;
    }
    slots_[slot_idx].next_free = free_list_head_;
    free_list_head_ = slot_idx;

    assert(slot_count_ > 0);
    --slot_count_;
}

Object *ObjectDB::get_instance(ObjectID p_id) noexcept {
    if (p_id.is_null()) return nullptr;

    u64 raw = (u64)p_id;
    u32 slot_idx = (u32)(raw & SLOT_MASK);

    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (slot_idx >= slot_max_ || slots_ == nullptr) return nullptr;

    u64 validator = (raw >> SLOT_BITS) & VALIDATOR_MASK;
    if (slots_[slot_idx].validator != validator) {
        return nullptr;
    }

    return slots_[slot_idx].object;
}

u32 ObjectDB::get_object_count() noexcept {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return slot_count_;
}

} // namespace Beyota
