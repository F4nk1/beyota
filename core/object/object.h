/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  object.h                                                              */
/**************************************************************************/

#pragma once

#include "core/object/object_db.h"
#include "core/object/object_id.h"
#include "core/object/property_info.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/type_primitives.h"
#include "core/variant/callable.h"
#include "core/variant/variant.h"

#include <cassert>
#include <format>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Beyota {

class Array;

struct StringNameHasher {
    [[nodiscard]] std::size_t operator()(const StringName &p_name) const noexcept {
        return p_name.hash();
    }
};

class Object {
    ObjectID instance_id_{};
    bool is_ref_counted_{false};
    std::unordered_map<StringName, Variant, StringNameHasher> metadata_{};

protected:
    explicit Object(bool p_ref_counted) noexcept;

public:
    enum Notification {
        NOTIFICATION_POSTINITIALIZE = 0,
        NOTIFICATION_PREDELETE = 1
    };

    Object() noexcept : Object(false) {}
    virtual ~Object();

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
    Object(Object &&) = delete;
    Object &operator=(Object &&) = delete;

    [[nodiscard]] ObjectID get_instance_id() const noexcept {
        return instance_id_;
    }

    [[nodiscard]] bool is_ref_counted() const noexcept {
        return is_ref_counted_;
    }

    static StringName get_class_name_static() {
        return StringName("Object");
    }

    [[nodiscard]] virtual StringName get_class_name() const {
        return get_class_name_static();
    }

    [[nodiscard]] bool is_class(const StringName &p_name) const;

    template <typename T>
    [[nodiscard]] static T *cast_to(Object *p_object) noexcept {
        return dynamic_cast<T *>(p_object);
    }

    template <typename T>
    [[nodiscard]] static const T *cast_to(const Object *p_object) noexcept {
        return dynamic_cast<const T *>(p_object);
    }

    void notification(int p_what, bool p_reversed = false);
    virtual void on_notification(int p_what);

    void set_meta(const StringName &p_name, const Variant &p_value);
    [[nodiscard]] Variant get_meta(const StringName &p_name, const Variant &p_default = Variant()) const;
    [[nodiscard]] bool has_meta(const StringName &p_name) const;
    bool remove_meta(const StringName &p_name);
    [[nodiscard]] std::vector<StringName> get_meta_list() const;

    bool set(const StringName &p_name, const Variant &p_value);
    [[nodiscard]] Variant get(const StringName &p_name, bool *r_valid = nullptr) const;

    virtual bool set_property(const StringName &p_name, const Variant &p_value);
    virtual bool get_property(const StringName &p_name, Variant &r_ret) const;
    virtual void get_property_list(std::vector<PropertyInfo> *r_list) const;

    Variant callp(const StringName &p_method, const Variant **p_args, int p_arg_count,
                  Callable::CallError &r_error);
    Variant callv(const StringName &p_method, const Array &p_args);

    template <typename... Args>
    Variant call(const StringName &p_method, Args &&...p_args) {
        Callable::CallError err;
        if constexpr (sizeof...(Args) == 0) {
            return callp(p_method, nullptr, 0, err);
        } else {
            const Variant vargs[] = {Variant(std::forward<Args>(p_args))...};
            const Variant *ptrs[sizeof...(Args)];
            for (std::size_t i = 0; i < sizeof...(Args); ++i) {
                ptrs[i] = &vargs[i];
            }
            return callp(p_method, ptrs, (int)sizeof...(Args), err);
        }
    }

    [[nodiscard]] bool has_method(const StringName &p_method) const;

    void connect(const StringName &p_signal_name, const Callable &p_callable);
    void disconnect(const StringName &p_signal_name, const Callable &p_callable);
    [[nodiscard]] bool is_connected(const StringName &p_signal_name, const Callable &p_callable) const;
    void emit_signal(const StringName &p_signal_name, const Variant **p_args, int p_arg_count);
    [[nodiscard]] Array get_signal_connection_list(const StringName &p_signal_name) const;

    template <typename... Args>
    void emit_signal(const StringName &p_signal_name, Args &&...p_args) {
        if constexpr (sizeof...(Args) == 0) {
            emit_signal(p_signal_name, nullptr, 0);
        } else {
            const Variant vargs[] = {Variant(std::forward<Args>(p_args))...};
            const Variant *ptrs[sizeof...(Args)];
            for (std::size_t i = 0; i < sizeof...(Args); ++i) {
                ptrs[i] = &vargs[i];
            }
            emit_signal(p_signal_name, ptrs, (int)sizeof...(Args));
        }
    }

    [[nodiscard]] virtual String to_string() const;
};

template <typename T>
inline T *ObjectDB::get_instance(ObjectID p_id) noexcept {
    return Object::cast_to<T>(get_instance(p_id));
}

} // namespace Beyota
