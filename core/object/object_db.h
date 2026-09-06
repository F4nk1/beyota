/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  object_db.h                                                           */
/**************************************************************************/

#pragma once

#include "core/object/object_id.h"
#include "core/type_primitives.h"

#include <cassert>
#include <shared_mutex>

namespace Beyota {

class Object;

class ObjectDB {
public:
    static constexpr u32 SLOT_BITS = 24;
    static constexpr u64 SLOT_MASK = (1ULL << SLOT_BITS) - 1;
    static constexpr u32 VALIDATOR_BITS = 39;
    static constexpr u64 VALIDATOR_MASK = (1ULL << VALIDATOR_BITS) - 1;
    static constexpr u64 REFERENCE_BIT = 1ULL << 63;

private:
    struct ObjectSlot {
        u64 validator{0};
        u32 next_free{0};
        bool is_ref_counted{false};
        Object *object{nullptr};
    };

    static inline std::shared_mutex mutex_{};
    static inline ObjectSlot *slots_{nullptr};
    static inline u32 slot_count_{0};
    static inline u32 slot_max_{0};
    static inline u32 free_list_head_{0};
    static inline u64 validator_counter_{0};

    static void grow_slots();

public:
    static void setup();
    static void cleanup();

    static ObjectID add_instance(Object *p_object);
    static void remove_instance(Object *p_object);

    [[nodiscard]] static Object *get_instance(ObjectID p_id) noexcept;

    template <typename T>
    [[nodiscard]] static T *get_instance(ObjectID p_id) noexcept;

    [[nodiscard]] static u32 get_object_count() noexcept;
};

} // namespace Beyota
