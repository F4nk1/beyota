/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_pools.cpp                                                     */
/**************************************************************************/

#include "core/variant/variant_pools.h"

#include <mutex>
#include <vector>

namespace Beyota::VariantPools {

namespace {

template <usize BlockSize, usize SlabCount = 64>
class BucketPool {
    struct FreeNode {
        FreeNode *next;
    };

    static_assert(BlockSize >= sizeof(FreeNode));

    std::mutex _mutex;
    FreeNode *_free_list{nullptr};
    std::vector<void *> _slabs;

    void _grow() {
        constexpr usize slab_bytes = BlockSize * SlabCount;
        void *slab = ::operator new(slab_bytes);
        _slabs.push_back(slab);
        u8 *ptr = static_cast<u8 *>(slab);
        for (usize i = 0; i < SlabCount; ++i) {
            FreeNode *node = reinterpret_cast<FreeNode *>(ptr + i * BlockSize);
            node->next = _free_list;
            _free_list = node;
        }
    }

public:
    BucketPool() = default;

    ~BucketPool() {
        for (void *slab : _slabs) {
            ::operator delete(slab);
        }
        _slabs.clear();
        _free_list = nullptr;
    }

    void *alloc() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_free_list == nullptr) {
            _grow();
        }
        FreeNode *node = _free_list;
        _free_list = _free_list->next;
        return static_cast<void *>(node);
    }

    void free(void *p_ptr) noexcept {
        if (p_ptr == nullptr) return;
        std::lock_guard<std::mutex> lock(_mutex);
        FreeNode *node = static_cast<FreeNode *>(p_ptr);
        node->next = _free_list;
        _free_list = node;
    }
};

BucketPool<BUCKET_SMALL>  g_pool_small;
BucketPool<BUCKET_MEDIUM> g_pool_medium;
BucketPool<BUCKET_LARGE>  g_pool_large;

} // namespace

void *alloc_small() {
    return g_pool_small.alloc();
}

void *alloc_medium() {
    return g_pool_medium.alloc();
}

void *alloc_large() {
    return g_pool_large.alloc();
}

void free_small(void *p_ptr) noexcept {
    g_pool_small.free(p_ptr);
}

void free_medium(void *p_ptr) noexcept {
    g_pool_medium.free(p_ptr);
}

void free_large(void *p_ptr) noexcept {
    g_pool_large.free(p_ptr);
}

} // namespace Beyota::VariantPools
