/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  class_db.h                                                            */
/**************************************************************************/

#pragma once

#include "core/object/method_bind.h"
#include "core/object/property_info.h"
#include "core/string/string_name.h"
#include "core/templates/hash_map.h"
#include "core/type_primitives.h"

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Beyota {

class Object;

class ClassDB {
public:
    using ObjectCreator = Object *(*)();

    struct PropertyRegistration {
        PropertyInfo info;
        StringName setter;
        StringName getter;
    };

    struct ClassInfo {
        StringName name;
        StringName parent_name;
        bool is_virtual{false};
        ObjectCreator creator{nullptr};
        std::unordered_map<StringName, MethodBind *> methods;
        std::unordered_map<StringName, PropertyRegistration> properties;
        std::unordered_map<StringName, MethodInfo> signals;
    };

private:
    static inline std::shared_mutex mutex_{};
    static inline std::unordered_map<StringName, ClassInfo> classes_{};

public:
    static void setup();
    static void cleanup();

    static void register_class_internal(const StringName &p_class_name,
                                        const StringName &p_parent_name,
                                        bool p_is_virtual,
                                        ObjectCreator p_creator);

    template <typename T, typename Parent = Object>
    static void register_class() {
        register_class_internal(T::get_class_name_static(),
                                Parent::get_class_name_static(),
                                false,
                                []() -> Object * { return new T(); });
    }

    template <typename T, typename Parent = Object>
    static void register_abstract_class() {
        register_class_internal(T::get_class_name_static(),
                                Parent::get_class_name_static(),
                                true,
                                nullptr);
    }

    static void bind_method_internal(const StringName &p_class_name, MethodBind *p_bind);

    template <typename Class, typename Ret, typename... Args>
    static MethodBind *bind_method(const StringName &p_method_name,
                                   Ret (Class::*p_method)(Args...),
                                   std::vector<Variant> p_defs = {}) {
        MethodBind *bind = create_method_bind(p_method);
        bind->set_name(p_method_name);
        bind->set_class_name(Class::get_class_name_static());
        if (!p_defs.empty()) {
            bind->set_default_arguments(std::move(p_defs));
        }
        bind_method_internal(Class::get_class_name_static(), bind);
        return bind;
    }

    template <typename Class, typename Ret, typename... Args>
    static MethodBind *bind_method(const StringName &p_method_name,
                                   Ret (Class::*p_method)(Args...) const,
                                   std::vector<Variant> p_defs = {}) {
        MethodBind *bind = create_method_bind(p_method);
        bind->set_name(p_method_name);
        bind->set_class_name(Class::get_class_name_static());
        if (!p_defs.empty()) {
            bind->set_default_arguments(std::move(p_defs));
        }
        bind_method_internal(Class::get_class_name_static(), bind);
        return bind;
    }

    template <typename Ret, typename... Args>
    static MethodBind *bind_static_method(const StringName &p_class_name,
                                          const StringName &p_method_name,
                                          Ret (*p_method)(Args...),
                                          std::vector<Variant> p_defs = {}) {
        MethodBind *bind = create_method_bind(p_method);
        bind->set_name(p_method_name);
        bind->set_class_name(p_class_name);
        if (!p_defs.empty()) {
            bind->set_default_arguments(std::move(p_defs));
        }
        bind_method_internal(p_class_name, bind);
        return bind;
    }

    static void add_property(const StringName &p_class_name, const PropertyInfo &p_info,
                             const StringName &p_setter, const StringName &p_getter);

    static void add_signal(const StringName &p_class_name, const MethodInfo &p_signal);

    [[nodiscard]] static bool is_class_registered(const StringName &p_class_name);
    [[nodiscard]] static bool is_parent_class(const StringName &p_class_name, const StringName &p_parent_name);
    [[nodiscard]] static StringName get_parent_class(const StringName &p_class_name);

    [[nodiscard]] static bool can_instantiate(const StringName &p_class_name);
    [[nodiscard]] static Object *instantiate(const StringName &p_class_name);

    [[nodiscard]] static MethodBind *get_method(const StringName &p_class_name,
                                                const StringName &p_method_name);
    [[nodiscard]] static bool has_method(const StringName &p_class_name,
                                         const StringName &p_method_name,
                                         bool p_no_inheritance = false);

    [[nodiscard]] static bool has_property(const StringName &p_class_name,
                                           const StringName &p_property_name,
                                           bool p_no_inheritance = false);
    [[nodiscard]] static PropertyInfo get_property_info(const StringName &p_class_name,
                                                        const StringName &p_property_name);

    static bool set_property(Object *p_instance, const StringName &p_property_name, const Variant &p_value);
    static Variant get_property(const Object *p_instance, const StringName &p_property_name, bool *r_valid = nullptr);

    static void get_property_list(const StringName &p_class_name,
                                  std::vector<PropertyInfo> *r_list,
                                  bool p_no_inheritance = false);

    [[nodiscard]] static bool has_signal(const StringName &p_class_name,
                                         const StringName &p_signal_name,
                                         bool p_no_inheritance = false);
    [[nodiscard]] static MethodInfo get_signal(const StringName &p_class_name,
                                               const StringName &p_signal_name);

    static Variant call(Object *p_instance, const StringName &p_method_name,
                        const Variant **p_args, int p_arg_count, Callable::CallError &r_error);
};

} // namespace Beyota
