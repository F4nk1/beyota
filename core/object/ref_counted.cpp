/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  ref_counted.cpp                                                       */
/**************************************************************************/

#include "core/object/ref_counted.h"

namespace Beyota {

bool RefCounted::init_ref() {
    if (reference()) {
        if (!is_referenced()) {
            u32 expected = 1;
            if (refcount_init_.compare_exchange_strong(expected, 0, std::memory_order_acq_rel, std::memory_order_relaxed)) {
                unreference();
            }
        }
        return true;
    }
    return false;
}

bool RefCounted::reference() const noexcept {
    u32 cur = refcount_.load(std::memory_order_relaxed);
    while (cur != 0) {
        if (refcount_.compare_exchange_weak(cur, cur + 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return true;
        }
    }
    return false;
}

bool RefCounted::unreference() const noexcept {
    return refcount_.fetch_sub(1, std::memory_order_acq_rel) == 1;
}

} // namespace Beyota
