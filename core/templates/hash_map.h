/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  hash_map.h                                                            */
/**************************************************************************/

#pragma once

#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <cassert>
#include <initializer_list>
#include <utility>

namespace Beyota {

template <typename TKey, typename TValue>
struct HashMapElement {
    HashMapElement *next{nullptr};
    HashMapElement *prev{nullptr};
    TKey key;
    TValue value;

    HashMapElement(const TKey &p_key, const TValue &p_val)
        : key(p_key), value(p_val) {}

    HashMapElement(TKey &&p_key, TValue &&p_val)
        : key(std::move(p_key)), value(std::move(p_val)) {}
};

struct HashMapComparatorDefault {
    template <typename T>
    [[nodiscard]] static bool compare(const T &a, const T &b) noexcept {
        return a == b;
    }
};

template <typename TKey, typename TValue, typename Hasher = HashMapHasherDefault, typename Comparator = HashMapComparatorDefault>
class HashMap {
public:
    using Element = HashMapElement<TKey, TValue>;

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
    HashMap() noexcept = default;

    HashMap(const HashMap &p_other) {
        if (p_other.size_ > 0) {
            Element *curr = p_other.head_;
            while (curr != nullptr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
        }
    }

    HashMap(HashMap &&p_other) noexcept
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

    HashMap &operator=(const HashMap &p_other) {
        if (this != &p_other) {
            clear();
            Element *curr = p_other.head_;
            while (curr != nullptr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
        }
        return *this;
    }

    HashMap &operator=(HashMap &&p_other) noexcept {
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

    ~HashMap() noexcept {
        free_all();
    }

    [[nodiscard]] usize size() const noexcept {
        return size_;
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return size_ == 0;
    }

    void clear() noexcept {
        Element *curr = head_;
        while (curr != nullptr) {
            Element *next = curr->next;
            delete curr;
            curr = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
        if (hashes_ != nullptr) {
            for (u32 i = 0; i < capacity_; ++i) {
                hashes_[i] = EMPTY_HASH;
                elements_[i] = nullptr;
            }
        }
    }

    [[nodiscard]] bool has(const TKey &p_key) const noexcept {
        return lookup(p_key) != nullptr;
    }

    [[nodiscard]] const TValue *getptr(const TKey &p_key) const noexcept {
        Element *elem = lookup(p_key);
        return elem ? &elem->value : nullptr;
    }

    [[nodiscard]] TValue *getptr(const TKey &p_key) noexcept {
        Element *elem = lookup(p_key);
        return elem ? &elem->value : nullptr;
    }

    [[nodiscard]] const TValue &get(const TKey &p_key, const TValue &p_default) const noexcept {
        Element *elem = lookup(p_key);
        return elem ? elem->value : p_default;
    }

    TValue &get_or_add(const TKey &p_key, const TValue &p_default) {
        Element *existing = lookup(p_key);
        if (existing != nullptr) {
            return existing->value;
        }
        if (capacity_ == 0 || (size_ + 1) * 4 > capacity_ * 3) {
            grow();
        }
        u32 h = calc_hash(p_key);
        Element *node = new Element(p_key, p_default);
        insert_node(node, h);
        return node->value;
    }

    TValue &operator[](const TKey &p_key) {
        Element *existing = lookup(p_key);
        if (existing != nullptr) {
            return existing->value;
        }
        if (capacity_ == 0 || (size_ + 1) * 4 > capacity_ * 3) {
            grow();
        }
        u32 h = calc_hash(p_key);
        Element *node = new Element(p_key, TValue{});
        insert_node(node, h);
        return node->value;
    }

    const TValue &operator[](const TKey &p_key) const {
        Element *elem = lookup(p_key);
        assert(elem != nullptr);
        return elem->value;
    }

    void insert(const TKey &p_key, const TValue &p_value) {
        Element *existing = lookup(p_key);
        if (existing != nullptr) {
            existing->value = p_value;
            return;
        }
        if (capacity_ == 0 || (size_ + 1) * 4 > capacity_ * 3) {
            grow();
        }
        u32 h = calc_hash(p_key);
        Element *node = new Element(p_key, p_value);
        insert_node(node, h);
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

                u32 curr_slot = slot;
                while (true) {
                    u32 next_slot = (curr_slot + 1) & mask_;
                    u32 next_hash = hashes_[next_slot];
                    if (next_hash == EMPTY_HASH) {
                        hashes_[curr_slot] = EMPTY_HASH;
                        elements_[curr_slot] = nullptr;
                        break;
                    }
                    u32 next_dist = probe_distance(next_hash, next_slot, mask_);
                    if (next_dist == 0) {
                        hashes_[curr_slot] = EMPTY_HASH;
                        elements_[curr_slot] = nullptr;
                        break;
                    }
                    hashes_[curr_slot] = next_hash;
                    elements_[curr_slot] = elements_[next_slot];
                    curr_slot = next_slot;
                }

                size_--;
                return true;
            }
            cur_dist++;
            slot = (slot + 1) & mask_;
        }
    }

    [[nodiscard]] Element *front() const noexcept { return head_; }
    [[nodiscard]] Element *back() const noexcept { return tail_; }

    struct ConstIterator {
        const Element *node_{nullptr};

        [[nodiscard]] const TKey &key() const noexcept { assert(node_); return node_->key; }
        [[nodiscard]] const TValue &value() const noexcept { assert(node_); return node_->value; }
        [[nodiscard]] const Element &operator*() const noexcept { assert(node_); return *node_; }
        [[nodiscard]] const Element *operator->() const noexcept { assert(node_); return node_; }

        ConstIterator &operator++() noexcept {
            if (node_ != nullptr) {
                node_ = node_->next;
            }
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] bool operator==(const ConstIterator &p_other) const noexcept { return node_ == p_other.node_; }
        [[nodiscard]] bool operator!=(const ConstIterator &p_other) const noexcept { return node_ != p_other.node_; }
    };

    struct Iterator {
        Element *node_{nullptr};

        [[nodiscard]] const TKey &key() const noexcept { assert(node_); return node_->key; }
        [[nodiscard]] TValue &value() noexcept { assert(node_); return node_->value; }
        [[nodiscard]] Element &operator*() noexcept { assert(node_); return *node_; }
        [[nodiscard]] Element *operator->() noexcept { assert(node_); return node_; }

        Iterator &operator++() noexcept {
            if (node_ != nullptr) {
                node_ = node_->next;
            }
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] bool operator==(const Iterator &p_other) const noexcept { return node_ == p_other.node_; }
        [[nodiscard]] bool operator!=(const Iterator &p_other) const noexcept { return node_ != p_other.node_; }
    };

    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator{head_}; }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator{nullptr}; }
    [[nodiscard]] Iterator begin() noexcept { return Iterator{head_}; }
    [[nodiscard]] Iterator end() noexcept { return Iterator{nullptr}; }
};

} // namespace Beyota
