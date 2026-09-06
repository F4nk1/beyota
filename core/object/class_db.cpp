/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  class_db.cpp                                                          */
/**************************************************************************/

#include "core/object/class_db.h"
#include "core/object/object.h"

#include <mutex>

namespace Beyota {

void ClassDB::setup() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    classes_.clear();
}

void ClassDB::cleanup() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    for (auto &pair : classes_) {
        for (auto &m_pair : pair.second.methods) {
            delete m_pair.second;
        }
        pair.second.methods.clear();
    }
    classes_.clear();
}

void ClassDB::register_class_internal(const StringName &p_class_name,
                                      const StringName &p_parent_name,
                                      bool p_is_virtual,
                                      ObjectCreator p_creator) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto &info = classes_[p_class_name];
    info.name = p_class_name;
    info.parent_name = p_parent_name;
    info.is_virtual = p_is_virtual;
    info.creator = p_creator;
}

void ClassDB::bind_method_internal(const StringName &p_class_name, MethodBind *p_bind) {
    assert(p_bind != nullptr);
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto it = classes_.find(p_class_name);
    assert(it != classes_.end());

    auto &methods = it->second.methods;
    auto m_it = methods.find(p_bind->get_name());
    if (m_it != methods.end()) {
        delete m_it->second;
    }
    methods[p_bind->get_name()] = p_bind;
}

void ClassDB::add_property(const StringName &p_class_name, const PropertyInfo &p_info,
                           const StringName &p_setter, const StringName &p_getter) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto it = classes_.find(p_class_name);
    assert(it != classes_.end());
    it->second.properties[p_info.name] = PropertyRegistration{p_info, p_setter, p_getter};
}

void ClassDB::add_signal(const StringName &p_class_name, const MethodInfo &p_signal) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto it = classes_.find(p_class_name);
    assert(it != classes_.end());
    it->second.signals[p_signal.name] = p_signal;
}

bool ClassDB::is_class_registered(const StringName &p_class_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return classes_.find(p_class_name) != classes_.end();
}

bool ClassDB::is_parent_class(const StringName &p_class_name, const StringName &p_parent_name) {
    if (p_class_name == p_parent_name) return true;

    std::shared_lock<std::shared_mutex> lock(mutex_);
    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        if (it->second.parent_name == p_parent_name) return true;
        current = it->second.parent_name;
    }
    return false;
}

StringName ClassDB::get_parent_class(const StringName &p_class_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = classes_.find(p_class_name);
    if (it != classes_.end()) {
        return it->second.parent_name;
    }
    return StringName();
}

bool ClassDB::can_instantiate(const StringName &p_class_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = classes_.find(p_class_name);
    if (it != classes_.end()) {
        return !it->second.is_virtual && it->second.creator != nullptr;
    }
    return false;
}

Object *ClassDB::instantiate(const StringName &p_class_name) {
    ObjectCreator creator = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = classes_.find(p_class_name);
        if (it != classes_.end() && !it->second.is_virtual) {
            creator = it->second.creator;
        }
    }
    if (creator) {
        return creator();
    }
    return nullptr;
}

MethodBind *ClassDB::get_method(const StringName &p_class_name, const StringName &p_method_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        auto m_it = it->second.methods.find(p_method_name);
        if (m_it != it->second.methods.end()) {
            return m_it->second;
        }
        current = it->second.parent_name;
    }
    return nullptr;
}

bool ClassDB::has_method(const StringName &p_class_name, const StringName &p_method_name,
                         bool p_no_inheritance) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (p_no_inheritance) {
        auto it = classes_.find(p_class_name);
        if (it != classes_.end()) {
            return it->second.methods.find(p_method_name) != it->second.methods.end();
        }
        return false;
    }

    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        if (it->second.methods.find(p_method_name) != it->second.methods.end()) {
            return true;
        }
        current = it->second.parent_name;
    }
    return false;
}

bool ClassDB::has_property(const StringName &p_class_name, const StringName &p_property_name,
                           bool p_no_inheritance) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (p_no_inheritance) {
        auto it = classes_.find(p_class_name);
        if (it != classes_.end()) {
            return it->second.properties.find(p_property_name) != it->second.properties.end();
        }
        return false;
    }

    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        if (it->second.properties.find(p_property_name) != it->second.properties.end()) {
            return true;
        }
        current = it->second.parent_name;
    }
    return false;
}

PropertyInfo ClassDB::get_property_info(const StringName &p_class_name,
                                        const StringName &p_property_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        auto p_it = it->second.properties.find(p_property_name);
        if (p_it != it->second.properties.end()) {
            return p_it->second.info;
        }
        current = it->second.parent_name;
    }
    return PropertyInfo();
}

bool ClassDB::set_property(Object *p_instance, const StringName &p_property_name, const Variant &p_value) {
    if (!p_instance) return false;

    StringName setter;
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        StringName current = p_instance->get_class_name();
        while (!current.is_empty()) {
            auto it = classes_.find(current);
            if (it == classes_.end()) break;
            auto p_it = it->second.properties.find(p_property_name);
            if (p_it != it->second.properties.end()) {
                setter = p_it->second.setter;
                break;
            }
            current = it->second.parent_name;
        }
    }

    if (setter.is_empty()) {
        return false;
    }

    Callable::CallError err;
    const Variant *args[1] = {&p_value};
    call(p_instance, setter, args, 1, err);
    return err.error == Callable::CallError::CALL_OK;
}

Variant ClassDB::get_property(const Object *p_instance, const StringName &p_property_name, bool *r_valid) {
    if (r_valid) *r_valid = false;
    if (!p_instance) return Variant();

    StringName getter;
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        StringName current = p_instance->get_class_name();
        while (!current.is_empty()) {
            auto it = classes_.find(current);
            if (it == classes_.end()) break;
            auto p_it = it->second.properties.find(p_property_name);
            if (p_it != it->second.properties.end()) {
                getter = p_it->second.getter;
                break;
            }
            current = it->second.parent_name;
        }
    }

    if (getter.is_empty()) {
        return Variant();
    }

    Callable::CallError err;
    Variant ret = call(const_cast<Object *>(p_instance), getter, nullptr, 0, err);
    if (err.error == Callable::CallError::CALL_OK) {
        if (r_valid) *r_valid = true;
        return ret;
    }
    return Variant();
}

void ClassDB::get_property_list(const StringName &p_class_name,
                                std::vector<PropertyInfo> *r_list,
                                bool p_no_inheritance) {
    if (!r_list) return;

    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (p_no_inheritance) {
        auto it = classes_.find(p_class_name);
        if (it != classes_.end()) {
            for (const auto &p_pair : it->second.properties) {
                r_list->push_back(p_pair.second.info);
            }
        }
        return;
    }

    std::vector<StringName> hierarchy;
    StringName current = p_class_name;
    while (!current.is_empty()) {
        hierarchy.push_back(current);
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        current = it->second.parent_name;
    }

    // Add properties starting from base class down to derived
    for (auto it = hierarchy.rbegin(); it != hierarchy.rend(); ++it) {
        auto c_it = classes_.find(*it);
        if (c_it != classes_.end()) {
            for (const auto &p_pair : c_it->second.properties) {
                r_list->push_back(p_pair.second.info);
            }
        }
    }
}

bool ClassDB::has_signal(const StringName &p_class_name, const StringName &p_signal_name,
                         bool p_no_inheritance) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (p_no_inheritance) {
        auto it = classes_.find(p_class_name);
        if (it != classes_.end()) {
            return it->second.signals.find(p_signal_name) != it->second.signals.end();
        }
        return false;
    }

    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        if (it->second.signals.find(p_signal_name) != it->second.signals.end()) {
            return true;
        }
        current = it->second.parent_name;
    }
    return false;
}

MethodInfo ClassDB::get_signal(const StringName &p_class_name, const StringName &p_signal_name) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    StringName current = p_class_name;
    while (!current.is_empty()) {
        auto it = classes_.find(current);
        if (it == classes_.end()) break;
        auto s_it = it->second.signals.find(p_signal_name);
        if (s_it != it->second.signals.end()) {
            return s_it->second;
        }
        current = it->second.parent_name;
    }
    return MethodInfo();
}

Variant ClassDB::call(Object *p_instance, const StringName &p_method_name,
                     const Variant **p_args, int p_arg_count, Callable::CallError &r_error) {
    if (!p_instance) {
        r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
        return Variant();
    }

    MethodBind *bind = get_method(p_instance->get_class_name(), p_method_name);
    if (!bind) {
        r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
        return Variant();
    }

    return bind->call(p_instance, p_args, p_arg_count, r_error);
}

} // namespace Beyota
