/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  ref_counted.h                                                         */
/**************************************************************************/

#pragma once

#include "core/object/object.h"
#include "core/variant/variant.h"

#include <atomic>
#include <concepts>
#include <type_traits>

namespace Beyota {

class RefCounted : public Object {
    mutable std::atomic<u32> refcount_{1};
    mutable std::atomic<u32> refcount_init_{1};

public:
    RefCounted() noexcept : Object(true) {}
    ~RefCounted() override = default;

    static StringName get_class_name_static() {
        return StringName("RefCounted");
    }

    [[nodiscard]] StringName get_class_name() const override {
        return get_class_name_static();
    }

    bool init_ref();
    bool reference() const noexcept;
    bool unreference() const noexcept;

    [[nodiscard]] u32 get_reference_count() const noexcept {
        return refcount_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] bool is_referenced() const noexcept {
        return refcount_init_.load(std::memory_order_relaxed) == 0;
    }
};

template <typename T>
class Ref {
    static_assert(std::is_base_of_v<RefCounted, T>, "Ref<T> requires T to derive from RefCounted.");

    T *ptr_{nullptr};

    template <bool Init>
    void ref_pointer(T *p_ptr) noexcept {
        if (p_ptr == ptr_) return;

        Ref cleanup;
        cleanup.ptr_ = ptr_;
        ptr_ = p_ptr;

        if (ptr_) {
            if constexpr (Init) {
                if (!ptr_->init_ref()) {
                    ptr_ = nullptr;
                }
            } else {
                if (!ptr_->reference()) {
                    ptr_ = nullptr;
                }
            }
        }
    }

public:
    constexpr Ref() noexcept = default;

    Ref(T *p_ptr) noexcept {
        if (p_ptr) {
            ref_pointer<true>(p_ptr);
        }
    }

    Ref(const Ref &p_other) noexcept {
        if (p_other.ptr_) {
            ref_pointer<false>(p_other.ptr_);
        }
    }

    Ref(Ref &&p_other) noexcept : ptr_(p_other.ptr_) {
        p_other.ptr_ = nullptr;
    }

    template <typename U>
        requires std::derived_from<U, T>
    Ref(const Ref<U> &p_other) noexcept {
        if (p_other.ptr()) {
            ref_pointer<false>(p_other.ptr());
        }
    }

    template <typename U>
        requires std::derived_from<U, T>
    Ref(Ref<U> &&p_other) noexcept : ptr_(p_other.ptr()) {
        p_other.release();
    }

    Ref(const Variant &p_variant) {
        Object *obj = (Object *)p_variant;
        T *cast_obj = Object::cast_to<T>(obj);
        if (cast_obj) {
            ref_pointer<false>(cast_obj);
        }
    }

    ~Ref() noexcept {
        unref();
    }

    void unref() noexcept {
        if (ptr_) {
            if (ptr_->unreference()) {
                delete ptr_;
            }
            ptr_ = nullptr;
        }
    }

    T *release() noexcept {
        T *ret = ptr_;
        ptr_ = nullptr;
        return ret;
    }

    Ref &operator=(const Ref &p_other) noexcept {
        if (this != &p_other) {
            ref_pointer<false>(p_other.ptr_);
        }
        return *this;
    }

    Ref &operator=(Ref &&p_other) noexcept {
        if (this != &p_other) {
            unref();
            ptr_ = p_other.ptr_;
            p_other.ptr_ = nullptr;
        }
        return *this;
    }

    Ref &operator=(T *p_ptr) noexcept {
        ref_pointer<true>(p_ptr);
        return *this;
    }

    template <typename U>
        requires std::derived_from<U, T>
    Ref &operator=(const Ref<U> &p_other) noexcept {
        ref_pointer<false>(p_other.ptr());
        return *this;
    }

    template <typename U>
        requires std::derived_from<U, T>
    Ref &operator=(Ref<U> &&p_other) noexcept {
        unref();
        ptr_ = p_other.release();
        return *this;
    }

    Ref &operator=(const Variant &p_variant) {
        Object *obj = (Object *)p_variant;
        T *cast_obj = Object::cast_to<T>(obj);
        ref_pointer<false>(cast_obj);
        return *this;
    }

    [[nodiscard]] T *ptr() const noexcept { return ptr_; }
    [[nodiscard]] T *operator->() const noexcept { return ptr_; }
    [[nodiscard]] T &operator*() const noexcept { return *ptr_; }

    [[nodiscard]] bool is_valid() const noexcept { return ptr_ != nullptr; }
    [[nodiscard]] bool is_null() const noexcept { return ptr_ == nullptr; }
    [[nodiscard]] explicit operator bool() const noexcept { return ptr_ != nullptr; }

    operator Variant() const {
        return Variant((Object *)ptr_);
    }

    template <typename U>
    [[nodiscard]] Ref<U> cast_to() const {
        if (!ptr_) return Ref<U>();
        U *cast_obj = Object::cast_to<U>(ptr_);
        if (!cast_obj) return Ref<U>();
        return Ref<U>(cast_obj);
    }

    [[nodiscard]] bool operator==(const Ref &p_other) const noexcept {
        return ptr_ == p_other.ptr_;
    }

    [[nodiscard]] bool operator!=(const Ref &p_other) const noexcept {
        return ptr_ != p_other.ptr_;
    }

    [[nodiscard]] bool operator==(const T *p_ptr) const noexcept {
        return ptr_ == p_ptr;
    }

    [[nodiscard]] bool operator!=(const T *p_ptr) const noexcept {
        return ptr_ != p_ptr;
    }

    [[nodiscard]] auto operator<=>(const Ref &p_other) const noexcept {
        return ptr_ <=> p_other.ptr_;
    }
};

} // namespace Beyota
