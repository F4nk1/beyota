/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  object.cpp                                                            */
/**************************************************************************/

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/variant/array.h"

namespace Beyota {

Object::Object(bool p_ref_counted) noexcept : is_ref_counted_(p_ref_counted) {
    instance_id_ = ObjectDB::add_instance(this);
}

Object::~Object() {
    ObjectDB::remove_instance(this);
}

bool Object::is_class(const StringName &p_name) const {
    return ClassDB::is_parent_class(get_class_name(), p_name);
}

void Object::notification(int p_what, bool p_reversed) {
    (void)p_reversed;
    on_notification(p_what);
}

void Object::on_notification(int p_what) {
    (void)p_what;
}

void Object::set_meta(const StringName &p_name, const Variant &p_value) {
    metadata_[p_name] = p_value;
}

Variant Object::get_meta(const StringName &p_name, const Variant &p_default) const {
    auto it = metadata_.find(p_name);
    if (it != metadata_.end()) {
        return it->second;
    }
    return p_default;
}

bool Object::has_meta(const StringName &p_name) const {
    return metadata_.find(p_name) != metadata_.end();
}

bool Object::remove_meta(const StringName &p_name) {
    return metadata_.erase(p_name) > 0;
}

std::vector<StringName> Object::get_meta_list() const {
    std::vector<StringName> list;
    list.reserve(metadata_.size());
    for (const auto &pair : metadata_) {
        list.push_back(pair.first);
    }
    return list;
}

bool Object::set(const StringName &p_name, const Variant &p_value) {
    if (set_property(p_name, p_value)) {
        return true;
    }
    return ClassDB::set_property(this, p_name, p_value);
}

Variant Object::get(const StringName &p_name, bool *r_valid) const {
    Variant ret;
    if (get_property(p_name, ret)) {
        if (r_valid) *r_valid = true;
        return ret;
    }
    return ClassDB::get_property(this, p_name, r_valid);
}

bool Object::set_property(const StringName &p_name, const Variant &p_value) {
    (void)p_name;
    (void)p_value;
    return false;
}

bool Object::get_property(const StringName &p_name, Variant &r_ret) const {
    (void)p_name;
    (void)r_ret;
    return false;
}

void Object::get_property_list(std::vector<PropertyInfo> *r_list) const {
    ClassDB::get_property_list(get_class_name(), r_list);
}

Variant Object::callp(const StringName &p_method, const Variant **p_args, int p_arg_count,
                      Callable::CallError &r_error) {
    return ClassDB::call(this, p_method, p_args, p_arg_count, r_error);
}

Variant Object::callv(const StringName &p_method, const Array &p_args) {
    std::vector<const Variant *> ptrs(p_args.size());
    for (usize i = 0; i < p_args.size(); ++i) {
        ptrs[i] = &p_args[i];
    }
    Callable::CallError err;
    return callp(p_method, ptrs.empty() ? nullptr : ptrs.data(), (int)p_args.size(), err);
}

bool Object::has_method(const StringName &p_method) const {
    return ClassDB::has_method(get_class_name(), p_method);
}

void Object::connect(const StringName &p_signal_name, const Callable &p_callable) {
    Signal(this, p_signal_name).connect(p_callable);
}

void Object::disconnect(const StringName &p_signal_name, const Callable &p_callable) {
    Signal(this, p_signal_name).disconnect(p_callable);
}

bool Object::is_connected(const StringName &p_signal_name, const Callable &p_callable) const {
    return Signal(this, p_signal_name).is_connected(p_callable);
}

void Object::emit_signal(const StringName &p_signal_name, const Variant **p_args, int p_arg_count) {
    Signal(this, p_signal_name).emit(p_args, p_arg_count);
}

Array Object::get_signal_connection_list(const StringName &p_signal_name) const {
    return Signal(this, p_signal_name).get_connections();
}

String Object::to_string() const {
    return String(std::format("<{}:{}>", get_class_name().operator String().c_str(), (u64)instance_id_));
}

} // namespace Beyota
