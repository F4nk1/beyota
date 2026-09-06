/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  self_list.h                                                           */
/**************************************************************************/

#pragma once

#include "core/type_primitives.h"

#include <cassert>
#include <functional>
#include <utility>

namespace Beyota {

template <typename T>
class SelfList {
public:
    class List {
        SelfList<T> *first_{nullptr};
        SelfList<T> *last_{nullptr};

        static SelfList<T> *merge(SelfList<T> *a, SelfList<T> *b, auto comp) {
            if (a == nullptr) return b;
            if (b == nullptr) return a;

            SelfList<T> *head = nullptr;
            SelfList<T> **tail = &head;

            while (a != nullptr && b != nullptr) {
                if (comp(*a->self_, *b->self_)) {
                    *tail = a;
                    a->prev_ = (tail == &head) ? nullptr : reinterpret_cast<SelfList<T> *>(reinterpret_cast<char *>(tail) - offsetof(SelfList<T>, next_));
                    tail = &a->next_;
                    a = a->next_;
                } else {
                    *tail = b;
                    b->prev_ = (tail == &head) ? nullptr : reinterpret_cast<SelfList<T> *>(reinterpret_cast<char *>(tail) - offsetof(SelfList<T>, next_));
                    tail = &b->next_;
                    b = b->next_;
                }
            }

            *tail = (a != nullptr) ? a : b;
            return head;
        }

    public:
        List() noexcept = default;
        List(const List &) = delete;
        List &operator=(const List &) = delete;

        ~List() noexcept {
            while (first_ != nullptr) {
                remove(first_);
            }
        }

        void add(SelfList<T> *p_elem) {
            assert(p_elem != nullptr);
            assert(p_elem->root_ == nullptr);

            p_elem->root_ = this;
            p_elem->next_ = first_;
            p_elem->prev_ = nullptr;

            if (first_ != nullptr) {
                first_->prev_ = p_elem;
            } else {
                last_ = p_elem;
            }
            first_ = p_elem;
        }

        void add_last(SelfList<T> *p_elem) {
            assert(p_elem != nullptr);
            assert(p_elem->root_ == nullptr);

            p_elem->root_ = this;
            p_elem->next_ = nullptr;
            p_elem->prev_ = last_;

            if (last_ != nullptr) {
                last_->next_ = p_elem;
            } else {
                first_ = p_elem;
            }
            last_ = p_elem;
        }

        void remove(SelfList<T> *p_elem) {
            assert(p_elem != nullptr);
            if (p_elem->root_ != this) {
                return;
            }

            if (p_elem->next_ != nullptr) {
                p_elem->next_->prev_ = p_elem->prev_;
            }
            if (p_elem->prev_ != nullptr) {
                p_elem->prev_->next_ = p_elem->next_;
            }
            if (first_ == p_elem) {
                first_ = p_elem->next_;
            }
            if (last_ == p_elem) {
                last_ = p_elem->prev_;
            }

            p_elem->next_ = nullptr;
            p_elem->prev_ = nullptr;
            p_elem->root_ = nullptr;
        }

        void clear() noexcept {
            while (first_ != nullptr) {
                remove(first_);
            }
        }

        template <typename Comparator>
        void sort_custom(Comparator p_comp) {
            if (first_ == nullptr || first_ == last_) {
                return;
            }

            // Insertion sort for intrusive doubly linked list
            SelfList<T> *sorted_head = nullptr;
            SelfList<T> *curr = first_;

            while (curr != nullptr) {
                SelfList<T> *next = curr->next_;
                curr->prev_ = nullptr;
                curr->next_ = nullptr;

                if (sorted_head == nullptr || p_comp(*curr->self_, *sorted_head->self_)) {
                    curr->next_ = sorted_head;
                    if (sorted_head != nullptr) {
                        sorted_head->prev_ = curr;
                    }
                    sorted_head = curr;
                } else {
                    SelfList<T> *search = sorted_head;
                    while (search->next_ != nullptr && !p_comp(*curr->self_, *search->next_->self_)) {
                        search = search->next_;
                    }
                    curr->next_ = search->next_;
                    if (search->next_ != nullptr) {
                        search->next_->prev_ = curr;
                    }
                    search->next_ = curr;
                    curr->prev_ = search;
                }

                curr = next;
            }

            first_ = sorted_head;
            SelfList<T> *tail = first_;
            while (tail != nullptr && tail->next_ != nullptr) {
                tail = tail->next_;
            }
            last_ = tail;
        }

        void sort() {
            sort_custom(std::less<T>{});
        }

        [[nodiscard]] SelfList<T> *first() noexcept { return first_; }
        [[nodiscard]] const SelfList<T> *first() const noexcept { return first_; }
        [[nodiscard]] SelfList<T> *last() noexcept { return last_; }
        [[nodiscard]] const SelfList<T> *last() const noexcept { return last_; }
    };

private:
    List *root_{nullptr};
    T *self_{nullptr};
    SelfList<T> *next_{nullptr};
    SelfList<T> *prev_{nullptr};

public:
    explicit SelfList(T *p_self) noexcept : self_(p_self) {}

    SelfList(const SelfList &) = delete;
    SelfList &operator=(const SelfList &) = delete;

    ~SelfList() noexcept {
        remove_from_list();
    }

    [[nodiscard]] bool in_list() const noexcept { return root_ != nullptr; }

    void remove_from_list() noexcept {
        if (root_ != nullptr) {
            root_->remove(this);
        }
    }

    [[nodiscard]] SelfList<T> *next() noexcept { return next_; }
    [[nodiscard]] const SelfList<T> *next() const noexcept { return next_; }
    [[nodiscard]] SelfList<T> *prev() noexcept { return prev_; }
    [[nodiscard]] const SelfList<T> *prev() const noexcept { return prev_; }
    [[nodiscard]] T *self() const noexcept { return self_; }
};

} // namespace Beyota
