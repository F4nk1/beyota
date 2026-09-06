/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  hash_set.h                                                            */
/**************************************************************************/

#pragma once

#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <cassert>
#include <initializer_list>
#include <utility>

namespace Beyota {

template <typename TKey>
struct HashSetElement {
    HashSetElement *next{nullptr};
    HashSetElement *prev{nullptr};
    TKey key;

    explicit HashSetElement(const TKey &p_key) : key(p_key) {}
    explicit HashSetElement(TKey &&p_key) : key(std::move(p_key)) {}
};

template <typename TKey, typename Hasher = HashMapHasherDefault, typename Comparator = HashMapComparatorDefault>
class HashSet {
public:
    using Element = HashSetElement<TKey>;

private:
    static constexpr u32 EMPTY_HASH = 0;

    Element **elements_{nullptr};
    u32 *hashes_{nullptr};
    Element *head_{nullptr};
    Element *tail_{nullptr};
    u32 capacity_{0};
    u32 mask_{0};
    u32 size_{0};

    [[nodiscard]] static u32 calc_hash(const TKey &p_key) noexcept {
        u32 h = Hasher::hash(p_key);
        h = hash_fmix32(h);
        if (h == EMPTY_HASH) {
            h = 1;
        }
        return h;
    }

    [[nodiscard]] static constexpr u32 probe_distance(u32 p_hash, u32 p_slot, u32 p_mask) noexcept {
        u32 desired_slot = p_hash & p_mask;
        return (p_slot - desired_slot) & p_mask;
    }

    void grow() {
        u32 new_capacity = (capacity_ == 0) ? 8 : (capacity_ * 2);
        rehash(new_capacity);
    }

    void rehash(u32 p_new_capacity) {
        u32 old_capacity = capacity_;
        u32 *old_hashes = hashes_;
        Element **old_elements = elements_;

        capacity_ = p_new_capacity;
        mask_ = capacity_ - 1;
        hashes_ = new u32[capacity_]{0};
        elements_ = new Element *[capacity_]{nullptr};

        for (u32 i = 0; i < old_capacity; ++i) {
            if (old_hashes[i] != EMPTY_HASH) {
                u32 h = old_hashes[i];
                Element *node = old_elements[i];
                u32 slot = h & mask_;
                u32 cur_dist = 0;

                while (true) {
                    if (hashes_[slot] == EMPTY_HASH) {
                        hashes_[slot] = h;
                        elements_[slot] = node;
                        break;
                    }

                    u32 occupant_dist = probe_distance(hashes_[slot], slot, mask_);
                    if (cur_dist > occupant_dist) {
                        std::swap(h, hashes_[slot]);
                        std::swap(node, elements_[slot]);
                        cur_dist = occupant_dist;
                    }

                    cur_dist++;
                    slot = (slot + 1) & mask_;
                }
            }
        }

        delete[] old_hashes;
        delete[] old_elements;
    }

    void insert_node(Element *p_node, u32 p_hash) {
        if (tail_ == nullptr) {
            head_ = p_node;
            tail_ = p_node;
            p_node->prev = nullptr;
            p_node->next = nullptr;
        } else {
            tail_->next = p_node;
            p_node->prev = tail_;
            p_node->next = nullptr;
            tail_ = p_node;
        }

        u32 h = p_hash;
        Element *curr_node = p_node;
        u32 slot = h & mask_;
        u32 cur_dist = 0;

        while (true) {
            if (hashes_[slot] == EMPTY_HASH) {
                hashes_[slot] = h;
                elements_[slot] = curr_node;
                size_++;
                return;
            }

            u32 occupant_dist = probe_distance(hashes_[slot], slot, mask_);
            if (cur_dist > occupant_dist) {
                std::swap(h, hashes_[slot]);
                std::swap(curr_node, elements_[slot]);
                cur_dist = occupant_dist;
            }

            cur_dist++;
            slot = (slot + 1) & mask_;
        }
    }

    [[nodiscard]] Element *lookup(const TKey &p_key) const noexcept {
        if (capacity_ == 0 || size_ == 0) {
            return nullptr;
        }
        u32 h = calc_hash(p_key);
        u32 slot = h & mask_;
        u32 cur_dist = 0;

        while (true) {
            u32 existing_hash = hashes_[slot];
            if (existing_hash == EMPTY_HASH) {
                return nullptr;
            }
            u32 occupant_dist = probe_distance(existing_hash, slot, mask_);
            if (cur_dist > occupant_dist) {
                return nullptr;
            }
            if (existing_hash == h && Comparator::compare(elements_[slot]->key, p_key)) {
                return elements_[slot];
            }
            cur_dist++;
            slot = (slot + 1) & mask_;
        }
    }

    void free_all() noexcept {
        Element *curr = head_;
        while (curr != nullptr) {
            Element *next = curr->next;
            delete curr;
            curr = next;
        }
        delete[] hashes_;
        delete[] elements_;
        hashes_ = nullptr;
        elements_ = nullptr;
        head_ = nullptr;
        tail_ = nullptr;
        capacity_ = 0;
        mask_ = 0;
        size_ = 0;
    }

public:
    HashSet() noexcept = default;

    HashSet(std::initializer_list<TKey> p_init) {
        for (const TKey &key : p_init) {
            insert(key);
        }
    }

    HashSet(const HashSet &p_other) {
        if (p_other.size_ > 0) {
            Element *curr = p_other.head_;
            while (curr != nullptr) {
                insert(curr->key);
                curr = curr->next;
            }
        }
    }

    HashSet(HashSet &&p_other) noexcept
        : elements_(p_other.elements_),
          hashes_(p_other.hashes_),
          head_(p_other.head_),
          tail_(p_other.tail_),
          capacity_(p_other.capacity_),
          mask_(p_other.mask_),
          size_(p_other.size_) {
        p_other.elements_ = nullptr;
        p_other.hashes_ = nullptr;
        p_other.head_ = nullptr;
        p_other.tail_ = nullptr;
        p_other.capacity_ = 0;
        p_other.mask_ = 0;
        p_other.size_ = 0;
    }

    HashSet &operator=(const HashSet &p_other) {
        if (this != &p_other) {
            clear();
            Element *curr = p_other.head_;
            while (curr != nullptr) {
                insert(curr->key);
                curr = curr->next;
            }
        }
        return *this;
    }

    HashSet &operator=(HashSet &&p_other) noexcept {
        if (this != &p_other) {
            free_all();
            elements_ = p_other.elements_;
            hashes_ = p_other.hashes_;
            head_ = p_other.head_;
            tail_ = p_other.tail_;
            capacity_ = p_other.capacity_;
            mask_ = p_other.mask_;
            size_ = p_other.size_;

            p_other.elements_ = nullptr;
            p_other.hashes_ = nullptr;
            p_other.head_ = nullptr;
            p_other.tail_ = nullptr;
            p_other.capacity_ = 0;
            p_other.mask_ = 0;
            p_other.size_ = 0;
        }
        return *this;
    }

    ~HashSet() noexcept {
        free_all();
    }

    struct Iterator {
        Element *elem{nullptr};

        Iterator() = default;
        explicit Iterator(Element *p_elem) : elem(p_elem) {}

        [[nodiscard]] const TKey &operator*() const noexcept {
            assert(elem != nullptr);
            return elem->key;
        }

        [[nodiscard]] const TKey *operator->() const noexcept {
            assert(elem != nullptr);
            return &elem->key;
        }

        Iterator &operator++() noexcept {
            if (elem != nullptr) {
                elem = elem->next;
            }
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return elem != nullptr;
        }

        [[nodiscard]] bool operator==(const Iterator &p_other) const noexcept {
            return elem == p_other.elem;
        }

        [[nodiscard]] bool operator!=(const Iterator &p_other) const noexcept {
            return elem != p_other.elem;
        }
    };

    struct ConstIterator {
        const Element *elem{nullptr};

        ConstIterator() = default;
        explicit ConstIterator(const Element *p_elem) : elem(p_elem) {}
        ConstIterator(const Iterator &p_it) : elem(p_it.elem) {}

        [[nodiscard]] const TKey &operator*() const noexcept {
            assert(elem != nullptr);
            return elem->key;
        }

        [[nodiscard]] const TKey *operator->() const noexcept {
            assert(elem != nullptr);
            return &elem->key;
        }

        ConstIterator &operator++() noexcept {
            if (elem != nullptr) {
                elem = elem->next;
            }
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return elem != nullptr;
        }

        [[nodiscard]] bool operator==(const ConstIterator &p_other) const noexcept {
            return elem == p_other.elem;
        }

        [[nodiscard]] bool operator!=(const ConstIterator &p_other) const noexcept {
            return elem != p_other.elem;
        }
    };

    [[nodiscard]] Iterator begin() noexcept { return Iterator(head_); }
    [[nodiscard]] Iterator end() noexcept { return Iterator(nullptr); }

    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(head_); }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator(nullptr); }

    [[nodiscard]] ConstIterator cbegin() const noexcept { return ConstIterator(head_); }
    [[nodiscard]] ConstIterator cend() const noexcept { return ConstIterator(nullptr); }

    [[nodiscard]] usize size() const noexcept { return size_; }
    [[nodiscard]] bool is_empty() const noexcept { return size_ == 0; }
    [[nodiscard]] u32 get_capacity() const noexcept { return capacity_; }

    void clear() noexcept {
        free_all();
    }

    void reset() noexcept {
        free_all();
    }

    [[nodiscard]] bool has(const TKey &p_key) const noexcept {
        return lookup(p_key) != nullptr;
    }

    [[nodiscard]] Iterator find(const TKey &p_key) noexcept {
        return Iterator(lookup(p_key));
    }

    [[nodiscard]] ConstIterator find(const TKey &p_key) const noexcept {
        return ConstIterator(lookup(p_key));
    }

    Iterator insert(const TKey &p_key) {
        Element *existing = lookup(p_key);
        if (existing != nullptr) {
            return Iterator(existing);
        }

        if (size_ + 1 > capacity_ * 0.75f || capacity_ == 0) {
            grow();
        }

        u32 h = calc_hash(p_key);
        Element *new_elem = new Element(p_key);
        insert_node(new_elem, h);
        return Iterator(new_elem);
    }

    Iterator insert(TKey &&p_key) {
        Element *existing = lookup(p_key);
        if (existing != nullptr) {
            return Iterator(existing);
        }

        if (size_ + 1 > capacity_ * 0.75f || capacity_ == 0) {
            grow();
        }

        u32 h = calc_hash(p_key);
        Element *new_elem = new Element(std::move(p_key));
        insert_node(new_elem, h);
        return Iterator(new_elem);
    }

    bool erase(const TKey &p_key) {
        if (capacity_ == 0 || size_ == 0) {
            return false;
        }

        u32 h = calc_hash(p_key);
        u32 slot = h & mask_;
        u32 cur_dist = 0;

        while (true) {
            u32 existing_hash = hashes_[slot];
            if (existing_hash == EMPTY_HASH) {
                return false;
            }
            u32 occupant_dist = probe_distance(existing_hash, slot, mask_);
            if (cur_dist > occupant_dist) {
                return false;
            }
            if (existing_hash == h && Comparator::compare(elements_[slot]->key, p_key)) {
                Element *node = elements_[slot];

                // Unlink from intrusive doubly linked list
                if (node->prev != nullptr) {
                    node->prev->next = node->next;
                } else {
                    head_ = node->next;
                }
                if (node->next != nullptr) {
                    node->next->prev = node->prev;
                } else {
                    tail_ = node->prev;
                }
                delete node;

                // Backward shift deletion for Robin Hood hashing
                u32 next_slot = (slot + 1) & mask_;
                while (hashes_[next_slot] != EMPTY_HASH && probe_distance(hashes_[next_slot], next_slot, mask_) > 0) {
                    hashes_[slot] = hashes_[next_slot];
                    elements_[slot] = elements_[next_slot];
                    slot = next_slot;
                    next_slot = (slot + 1) & mask_;
                }

                hashes_[slot] = EMPTY_HASH;
                elements_[slot] = nullptr;
                size_--;
                return true;
            }
            cur_dist++;
            slot = (slot + 1) & mask_;
        }
    }

    bool remove(const Iterator &p_it) {
        if (p_it.elem == nullptr) {
            return false;
        }
        return erase(p_it.elem->key);
    }

    [[nodiscard]] bool operator==(const HashSet &p_other) const noexcept {
        if (size_ != p_other.size_) {
            return false;
        }
        for (const auto &key : *this) {
            if (!p_other.has(key)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool operator!=(const HashSet &p_other) const noexcept {
        return !(*this == p_other);
    }
};

} // namespace Beyota
