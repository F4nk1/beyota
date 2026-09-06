/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  method_bind.h                                                         */
/**************************************************************************/

#pragma once

#include "core/string/string_name.h"
#include "core/type_primitives.h"
#include "core/variant/callable.h"
#include "core/variant/variant.h"

#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Beyota {

class Object;

class MethodBind {
protected:
    StringName name_{};
    StringName class_name_{};
    int argument_count_{0};
    bool is_const_{false};
    bool is_static_{false};
    bool has_return_{false};
    std::vector<Variant> default_arguments_{};

public:
    MethodBind() = default;
    virtual ~MethodBind() = default;

    [[nodiscard]] StringName get_name() const noexcept { return name_; }
    void set_name(const StringName &p_name) noexcept { name_ = p_name; }

    [[nodiscard]] StringName get_class_name() const noexcept { return class_name_; }
    void set_class_name(const StringName &p_class_name) noexcept { class_name_ = p_class_name; }

    [[nodiscard]] int get_argument_count() const noexcept { return argument_count_; }
    [[nodiscard]] bool is_const() const noexcept { return is_const_; }
    [[nodiscard]] bool is_static() const noexcept { return is_static_; }
    [[nodiscard]] bool has_return() const noexcept { return has_return_; }

    void set_default_arguments(std::vector<Variant> p_defs) {
        default_arguments_ = std::move(p_defs);
    }
    [[nodiscard]] const std::vector<Variant> &get_default_arguments() const noexcept {
        return default_arguments_;
    }

    [[nodiscard]] virtual Variant call(Object *p_instance, const Variant **p_args,
                                       int p_arg_count, Callable::CallError &r_error) const = 0;
};

namespace Detail {

template <typename T>
struct VariantArgCaster {
    using Bare = std::decay_t<T>;

    static decltype(auto) cast(const Variant &p_val) {
        if constexpr (std::is_same_v<Bare, Variant>) {
            return p_val;
        } else if constexpr (std::is_pointer_v<Bare> && std::is_base_of_v<Object, std::remove_pointer_t<Bare>>) {
            using ObjType = std::remove_pointer_t<Bare>;
            return static_cast<ObjType *>((Object *)p_val);
        } else {
            return (Bare)p_val;
        }
    }
};

} // namespace Detail

template <typename Class, typename Ret, typename... Args>
class MethodBindMethod : public MethodBind {
    using MethodType = Ret (Class::*)(Args...);
    MethodType method_{nullptr};

    template <std::size_t... Is>
    Variant call_internal(Class *p_instance, const Variant **p_args, int p_arg_count,
                          std::index_sequence<Is...>) const {
        constexpr int total_args = (int)sizeof...(Args);
        int def_offset = total_args - (int)default_arguments_.size();

        auto get_arg = [&](int idx) -> const Variant & {
            if (idx < p_arg_count) {
                return *p_args[idx];
            }
            int def_idx = idx - def_offset;
            assert(def_idx >= 0 && def_idx < (int)default_arguments_.size());
            return default_arguments_[def_idx];
        };

        if constexpr (std::is_void_v<Ret>) {
            (p_instance->*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...);
            return Variant();
        } else {
            return Variant((p_instance->*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...));
        }
    }

public:
    MethodBindMethod(MethodType p_method) : method_(p_method) {
        argument_count_ = (int)sizeof...(Args);
        is_const_ = false;
        is_static_ = false;
        has_return_ = !std::is_void_v<Ret>;
    }

    [[nodiscard]] Variant call(Object *p_instance, const Variant **p_args,
                               int p_arg_count, Callable::CallError &r_error) const override {
        if (!p_instance) {
            r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
            return Variant();
        }

        constexpr int total_args = (int)sizeof...(Args);
        int min_args = total_args - (int)default_arguments_.size();

        if (p_arg_count < min_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
            r_error.expected = min_args;
            r_error.argument = p_arg_count;
            return Variant();
        }
        if (p_arg_count > total_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
            r_error.expected = total_args;
            r_error.argument = p_arg_count;
            return Variant();
        }

        r_error.error = Callable::CallError::CALL_OK;
        return call_internal(static_cast<Class *>(p_instance), p_args, p_arg_count,
                             std::index_sequence_for<Args...>{});
    }
};

template <typename Class, typename Ret, typename... Args>
class MethodBindConstMethod : public MethodBind {
    using MethodType = Ret (Class::*)(Args...) const;
    MethodType method_{nullptr};

    template <std::size_t... Is>
    Variant call_internal(const Class *p_instance, const Variant **p_args, int p_arg_count,
                          std::index_sequence<Is...>) const {
        constexpr int total_args = (int)sizeof...(Args);
        int def_offset = total_args - (int)default_arguments_.size();

        auto get_arg = [&](int idx) -> const Variant & {
            if (idx < p_arg_count) {
                return *p_args[idx];
            }
            int def_idx = idx - def_offset;
            assert(def_idx >= 0 && def_idx < (int)default_arguments_.size());
            return default_arguments_[def_idx];
        };

        if constexpr (std::is_void_v<Ret>) {
            (p_instance->*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...);
            return Variant();
        } else {
            return Variant((p_instance->*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...));
        }
    }

public:
    MethodBindConstMethod(MethodType p_method) : method_(p_method) {
        argument_count_ = (int)sizeof...(Args);
        is_const_ = true;
        is_static_ = false;
        has_return_ = !std::is_void_v<Ret>;
    }

    [[nodiscard]] Variant call(Object *p_instance, const Variant **p_args,
                               int p_arg_count, Callable::CallError &r_error) const override {
        if (!p_instance) {
            r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
            return Variant();
        }

        constexpr int total_args = (int)sizeof...(Args);
        int min_args = total_args - (int)default_arguments_.size();

        if (p_arg_count < min_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
            r_error.expected = min_args;
            r_error.argument = p_arg_count;
            return Variant();
        }
        if (p_arg_count > total_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
            r_error.expected = total_args;
            r_error.argument = p_arg_count;
            return Variant();
        }

        r_error.error = Callable::CallError::CALL_OK;
        return call_internal(static_cast<const Class *>(p_instance), p_args, p_arg_count,
                             std::index_sequence_for<Args...>{});
    }
};

template <typename Ret, typename... Args>
class MethodBindStaticMethod : public MethodBind {
    using MethodType = Ret (*)(Args...);
    MethodType method_{nullptr};

    template <std::size_t... Is>
    Variant call_internal(const Variant **p_args, int p_arg_count,
                          std::index_sequence<Is...>) const {
        constexpr int total_args = (int)sizeof...(Args);
        int def_offset = total_args - (int)default_arguments_.size();

        auto get_arg = [&](int idx) -> const Variant & {
            if (idx < p_arg_count) {
                return *p_args[idx];
            }
            int def_idx = idx - def_offset;
            assert(def_idx >= 0 && def_idx < (int)default_arguments_.size());
            return default_arguments_[def_idx];
        };

        if constexpr (std::is_void_v<Ret>) {
            (*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...);
            return Variant();
        } else {
            return Variant((*method_)(Detail::VariantArgCaster<Args>::cast(get_arg((int)Is))...));
        }
    }

public:
    MethodBindStaticMethod(MethodType p_method) : method_(p_method) {
        argument_count_ = (int)sizeof...(Args);
        is_const_ = false;
        is_static_ = true;
        has_return_ = !std::is_void_v<Ret>;
    }

    [[nodiscard]] Variant call(Object *p_instance, const Variant **p_args,
                               int p_arg_count, Callable::CallError &r_error) const override {
        (void)p_instance;
        constexpr int total_args = (int)sizeof...(Args);
        int min_args = total_args - (int)default_arguments_.size();

        if (p_arg_count < min_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
            r_error.expected = min_args;
            r_error.argument = p_arg_count;
            return Variant();
        }
        if (p_arg_count > total_args) {
            r_error.error = Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
            r_error.expected = total_args;
            r_error.argument = p_arg_count;
            return Variant();
        }

        r_error.error = Callable::CallError::CALL_OK;
        return call_internal(p_args, p_arg_count, std::index_sequence_for<Args...>{});
    }
};

template <typename Class, typename Ret, typename... Args>
inline MethodBind *create_method_bind(Ret (Class::*p_method)(Args...)) {
    return new MethodBindMethod<Class, Ret, Args...>(p_method);
}

template <typename Class, typename Ret, typename... Args>
inline MethodBind *create_method_bind(Ret (Class::*p_method)(Args...) const) {
    return new MethodBindConstMethod<Class, Ret, Args...>(p_method);
}

template <typename Ret, typename... Args>
inline MethodBind *create_method_bind(Ret (*p_method)(Args...)) {
    return new MethodBindStaticMethod<Ret, Args...>(p_method);
}

} // namespace Beyota
