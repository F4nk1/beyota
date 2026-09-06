/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  test_templates.cpp                                                    */
/**************************************************************************/

#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"
#include "core/templates/hashfuncs.h"
#include "core/templates/local_vector.h"
#include "core/templates/paged_allocator.h"
#include "core/templates/rid.h"
#include "core/templates/rid_owner.h"
#include "core/templates/ring_buffer.h"
#include "core/templates/self_list.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace Beyota;

// 1. HashMap Test Suite
void test_hash_map() {
    HashMap<int, std::string> map;
    assert(map.size() == 0);
    assert(map.is_empty());

    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    assert(map.size() == 3);
    assert(!map.is_empty());
    assert(map.has(1));
    assert(map.has(2));
    assert(map.has(3));
    assert(!map.has(4));

    assert(map[1] == "one");
    assert(map[2] == "two");
    assert(map[3] == "three");

    // Overwrite
    map[2] = "TWO_MODIFIED";
    assert(map[2] == "TWO_MODIFIED");
    assert(map.size() == 3);

    // Insertion order preservation in iteration
    std::vector<int> keys;
    for (const auto &elem : map) {
        keys.push_back(elem.key);
    }
    assert(keys.size() == 3);
    assert(keys[0] == 1);
    assert(keys[1] == 2);
    assert(keys[2] == 3);

    // Erase
    bool erased = map.erase(2);
    assert(erased);
    assert(!map.has(2));
    assert(map.size() == 2);

    // Stress test with Robin Hood collision resolution
    const int ELEM_COUNT = 5000;
    HashMap<int, int> stress_map;
    for (int i = 0; i < ELEM_COUNT; ++i) {
        stress_map.insert(i, i * 10);
    }
    assert((int)stress_map.size() == ELEM_COUNT);

    int iter_idx = 0;
    for (const auto &elem : stress_map) {
        assert(elem.key == iter_idx);
        assert(elem.value == iter_idx * 10);
        iter_idx++;
    }

    for (int i = 0; i < ELEM_COUNT; i += 2) {
        stress_map.erase(i);
    }
    assert((int)stress_map.size() == ELEM_COUNT / 2);
    for (int i = 0; i < ELEM_COUNT; ++i) {
        if (i % 2 == 0) {
            assert(!stress_map.has(i));
        } else {
            assert(stress_map.has(i));
            assert(stress_map[i] == i * 10);
        }
    }

    std::cout << "[PASS] HashMap (Robin Hood hashing & order preservation)" << std::endl;
}

// 2. HashSet Test Suite (Matching Godot test_hash_set.cpp)
void test_hash_set() {
    // List initialization
    HashSet<int> set{0, 1, 2, 3, 4};
    assert(set.size() == 5);
    assert(set.has(0));
    assert(set.has(1));
    assert(set.has(2));
    assert(set.has(3));
    assert(set.has(4));
    assert(!set.has(5));

    // Duplicate rejection
    HashSet<int> set_dups{0, 0, 0, 0, 0};
    assert(set_dups.size() == 1);
    assert(set_dups.has(0));

    // Insert element & iterator
    HashSet<int> s;
    auto it = s.insert(42);
    assert(it);
    assert(*it == 42);
    assert(s.has(42));
    assert(s.find(42));
    s.reset();
    assert(s.is_empty());

    // Godot canonical: insert, iterate and remove many elements with order kept
    const int elem_max = 5000;
    HashSet<int> big_set;
    for (int i = 0; i < elem_max; ++i) {
        big_set.insert(i);
    }
    int idx = 0;
    for (const int &k : big_set) {
        assert(idx == k);
        assert(big_set.has(idx));
        idx++;
    }

    std::vector<int> still_valid;
    for (int i = 0; i < elem_max; ++i) {
        if ((i % 5) == 0) {
            big_set.erase(i);
        } else {
            still_valid.push_back(i);
        }
    }
    assert(big_set.size() == still_valid.size());
    for (int v : still_valid) {
        assert(big_set.has(v));
    }

    // Equality operator
    HashSet<int> empty1;
    HashSet<int> empty2;
    assert(empty1 == empty2);
    assert(HashSet<int>{1, 2, 3} == HashSet<int>{1, 2, 3});
    assert(HashSet<int>{1, 2, 3} == HashSet<int>{3, 2, 1});
    assert(HashSet<int>{1, 2, 3} != HashSet<int>{1, 2, 8});
    assert(HashSet<int>{1, 2, 3} != HashSet<int>{1, 2});

    std::cout << "[PASS] HashSet (Canonical Godot parity & order preservation)" << std::endl;
}

// 3. LocalVector Test Suite (Matching Godot test_local_vector.cpp)
void test_local_vector() {
    LocalVector<int> vec;
    assert(vec.is_empty());
    assert(vec.size() == 0);

    // push_back
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    assert(vec.size() == 100);
    for (int i = 0; i < 100; ++i) {
        assert(vec[(u32)i] == i);
    }

    // pop_back
    vec.pop_back();
    assert(vec.size() == 99);
    assert(vec[98] == 98);

    // remove_at (ordered)
    vec.remove_at(0);
    assert(vec.size() == 98);
    assert(vec[0] == 1);

    // remove_at_unordered (swap last)
    int last_val = vec[vec.size() - 1];
    vec.remove_at_unordered(0);
    assert(vec[0] == last_val);

    // reverse
    LocalVector<int> rev{1, 2, 3, 4, 5};
    rev.reverse();
    assert(rev[0] == 5 && rev[1] == 4 && rev[2] == 3 && rev[3] == 2 && rev[4] == 1);

    // sort
    rev.sort();
    assert(rev[0] == 1 && rev[1] == 2 && rev[2] == 3 && rev[3] == 4 && rev[4] == 5);

    // ordered_insert
    rev.ordered_insert(3);
    assert(rev.size() == 6);
    assert(rev[2] == 3 && rev[3] == 3);

    // find and has
    assert(rev.has(4));
    assert(rev.find(4) != -1);
    assert(!rev.has(999));
    assert(rev.find(999) == -1);

    // resize & clear
    rev.resize(2);
    assert(rev.size() == 2);
    rev.clear();
    assert(rev.is_empty());

    std::cout << "[PASS] LocalVector (Contiguous cache-friendly array)" << std::endl;
}

// 4. SelfList Test Suite (Matching Godot test_self_list.cpp)
void test_self_list() {
    const int SIZE = 5;
    int numbers[SIZE]{3, 2, 5, 1, 4};
    SelfList<int> elements[SIZE]{
        SelfList<int>(&numbers[0]),
        SelfList<int>(&numbers[1]),
        SelfList<int>(&numbers[2]),
        SelfList<int>(&numbers[3]),
        SelfList<int>(&numbers[4]),
    };

    SelfList<int>::List list;
    for (int i = 0; i < SIZE; i++) {
        list.add_last(&elements[i]);
    }

    SelfList<int> *it = list.first();
    for (int i = 0; i < SIZE; i++) {
        assert(it != nullptr);
        assert(numbers[i] == *it->self());
        it = it->next();
    }

    list.sort();
    it = list.first();
    for (int i = 1; i <= SIZE; i++) {
        assert(it != nullptr);
        assert(i == *it->self());
        it = it->next();
    }

    for (SelfList<int> &elem : elements) {
        elem.remove_from_list();
        assert(!elem.in_list());
    }

    std::cout << "[PASS] SelfList (Intrusive doubly-linked list)" << std::endl;
}

// 5. RingBuffer Test Suite
void test_ring_buffer() {
    // 2^4 = 16 elements
    RingBuffer<int> rb(4);
    assert(rb.size() == 16);
    assert(rb.data_left() == 0);
    assert(rb.space_left() == 15); // one slot reserved for full/empty distinction

    // Write elements
    for (int i = 0; i < 10; ++i) {
        bool ok = rb.write(i);
        assert(ok);
    }
    assert(rb.data_left() == 10);
    assert(rb.space_left() == 5);

    // Read 5 elements
    for (int i = 0; i < 5; ++i) {
        int v = rb.read();
        assert(v == i);
    }
    assert(rb.data_left() == 5);

    // Write another 8 elements (wraparound)
    for (int i = 10; i < 18; ++i) {
        bool ok = rb.write(i);
        assert(ok);
    }
    assert(rb.data_left() == 13);

    // Copy without advancing
    int copy_buf[5];
    int copied = rb.copy(copy_buf, 0, 5);
    assert(copied == 5);
    for (int i = 0; i < 5; ++i) {
        assert(copy_buf[i] == 5 + i);
    }
    assert(rb.data_left() == 13);

    // Batch read
    int read_buf[13];
    int read_cnt = rb.read(read_buf, 13);
    assert(read_cnt == 13);
    for (int i = 0; i < 13; ++i) {
        assert(read_buf[i] == 5 + i);
    }
    assert(rb.data_left() == 0);

    std::cout << "[PASS] RingBuffer (Circular buffer with power-of-two mask)" << std::endl;
}

// 6. RID & RID_Owner Test Suite (Matching Godot test_rid.cpp)
void test_rid_and_owner() {
    // RID basics
    RID r_null;
    assert(r_null.is_null());
    assert(!r_null.is_valid());
    assert(r_null.get_id() == 0);

    RID r1 = RID::from_uint64(1);
    assert(!r1.is_null());
    assert(r1.is_valid());
    assert(r1.get_id() == 1);
    assert(r1.get_local_index() == 1);

    RID r_large = RID::from_uint64(4294967297ULL); // (1 << 32) | 1
    assert(r_large.get_local_index() == 1);

    // Operators
    RID r2 = RID::from_uint64(2);
    assert(r1 == r1);
    assert(r1 != r2);
    assert(r1 < r2);

    // RID_Owner single threaded
    struct Resource {
        int value{0};
        std::string name;
    };

    RID_Owner<Resource, false> owner;
    RID res1_id = owner.make_rid(Resource{100, "Texture"});
    assert(res1_id.is_valid());
    assert(owner.owns(res1_id));

    Resource *res1 = owner.get_or_null(res1_id);
    assert(res1 != nullptr);
    assert(res1->value == 100);
    assert(res1->name == "Texture");

    RID res2_id = owner.make_rid(Resource{200, "Shader"});
    assert(res2_id.is_valid());
    assert(res1_id != res2_id);

    owner.free(res1_id);
    assert(!owner.owns(res1_id));
    assert(owner.get_or_null(res1_id) == nullptr);

    // Reallocation reuse slot with new validator
    RID res3_id = owner.make_rid(Resource{300, "Mesh"});
    assert(res3_id.is_valid());
    assert(res3_id != res1_id); // Generational safety: different validator!
    assert(owner.get_or_null(res1_id) == nullptr);
    assert(owner.get(res3_id)->value == 300);

    // Multi-threaded stress test with RID_Owner<T, true> (Matching Godot multi-threaded test)
    struct ThreadData {
        char bytes[64];
    };
    RID_Owner<ThreadData, true> mt_owner(128); // 2 elements per chunk to stress chunk allocation
    constexpr int THREAD_COUNT = 8;
    constexpr int OPS_PER_THREAD = 200;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (int t = 0; t < THREAD_COUNT; ++t) {
        threads.emplace_back([&mt_owner, &start_flag, t]() {
            while (!start_flag.load(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }
            std::vector<RID> local_rids;
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                ThreadData td;
                std::memset(td.bytes, (char)(t + 1), sizeof(td.bytes));
                RID rid = mt_owner.make_rid(td);
                local_rids.push_back(rid);
            }
            for (RID rid : local_rids) {
                ThreadData *d = mt_owner.get_or_null(rid);
                assert(d != nullptr);
                assert(d->bytes[0] == (char)(t + 1));
            }
            for (RID rid : local_rids) {
                mt_owner.free(rid);
            }
        });
    }

    start_flag.store(true, std::memory_order_release);
    for (auto &th : threads) {
        th.join();
    }

    std::cout << "[PASS] RID & RID_Owner (Generational pool & thread safety)" << std::endl;
}

// 7. PagedAllocator Test Suite
void test_paged_allocator() {
    struct Particle {
        float x, y, z;
        int id;
    };

    PagedAllocator<Particle, false, 64> allocator;
    assert(allocator.is_configured());

    std::vector<Particle *> particles;
    for (int i = 0; i < 500; ++i) {
        Particle *p = allocator.alloc(Particle{(float)i, (float)(i * 2), (float)(i * 3), i});
        assert(p != nullptr);
        assert(p->id == i);
        particles.push_back(p);
    }

    for (int i = 0; i < 250; ++i) {
        allocator.free(particles[i]);
    }

    // Reallocate should reuse freed slots
    for (int i = 0; i < 250; ++i) {
        Particle *p = allocator.alloc(Particle{1.0f, 2.0f, 3.0f, 1000 + i});
        assert(p != nullptr);
        assert(p->id == 1000 + i);
        particles[i] = p;
    }

    for (int i = 0; i < 500; ++i) {
        allocator.free(particles[i]);
    }

    // Multi-threaded PagedAllocator<T, true>
    PagedAllocator<int, true, 32> mt_allocator;
    constexpr int THREAD_COUNT = 8;
    constexpr int ALLOCS_PER_THREAD = 300;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (int t = 0; t < THREAD_COUNT; ++t) {
        threads.emplace_back([&mt_allocator, &start_flag, t]() {
            while (!start_flag.load(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }
            std::vector<int *> ptrs;
            for (int i = 0; i < ALLOCS_PER_THREAD; ++i) {
                int *ptr = mt_allocator.alloc(t * 1000 + i);
                assert(ptr != nullptr);
                ptrs.push_back(ptr);
            }
            for (size_t i = 0; i < ptrs.size(); ++i) {
                assert(*ptrs[i] == (int)(t * 1000 + i));
                mt_allocator.free(ptrs[i]);
            }
        });
    }

    start_flag.store(true, std::memory_order_release);
    for (auto &th : threads) {
        th.join();
    }

    std::cout << "[PASS] PagedAllocator (Chunked memory pool & thread safety)" << std::endl;
}

int main() {
    std::cout << "Running exhaustive Beyota core/templates subsystem test suite..." << std::endl;
    std::cout << "================================================================" << std::endl;

    test_hash_map();
    test_hash_set();
    test_local_vector();
    test_self_list();
    test_ring_buffer();
    test_rid_and_owner();
    test_paged_allocator();

    std::cout << "================================================================" << std::endl;
    std::cout << "ALL 7 TEMPLATE SUBSYSTEM TEST SUITES PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "================================================================" << std::endl;
    return 0;
}
