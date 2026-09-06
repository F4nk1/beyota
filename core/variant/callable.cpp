/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  callable.cpp                                                          */
/**************************************************************************/

#include "core/variant/callable.h"
#include "core/object/object.h"
#include "core/object/object_db.h"
#include "core/variant/array.h"
#include "core/variant/variant.h"

#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Beyota {

namespace {

struct SignalHash {
    usize operator()(const Signal &p_signal) const noexcept {
        return p_signal.hash();
    }
};

struct SignalRegistry {
    std::mutex mutex;
    std::unordered_map<Signal, std::vector<Callable>, SignalHash> table;
};

SignalRegistry &get_signal_registry() {
    static SignalRegistry reg;
    return reg;
}

} // namespace

void Callable::callp(const Variant **p_args, int p_argcount, Variant &r_ret, CallError &r_error) const {
    r_error.error = CallError::CALL_OK;

    if (is_null()) {
        r_error.error = CallError::CALL_ERROR_INSTANCE_IS_NULL;
        r_ret = Variant();
        return;
    }

    if (is_custom()) {
        r_ret = Variant();
        return;
    }

    Object *obj = ObjectDB::get_instance(ObjectID(object));
    if (!obj) {
        r_error.error = CallError::CALL_ERROR_INSTANCE_IS_NULL;
        r_ret = Variant();
        return;
    }

    r_ret = obj->callp(method, p_args, p_argcount, r_error);
}

Variant Callable::callv(const Array &p_args) const {
    std::vector<const Variant *> ptrs(p_args.size());
    for (usize i = 0; i < p_args.size(); ++i) {
        ptrs[i] = &p_args[i];
    }
    Variant ret;
    CallError err;
    callp(ptrs.empty() ? nullptr : ptrs.data(), static_cast<int>(p_args.size()), ret, err);
    return ret;
}

void Signal::connect(const Callable &p_callable) {
    if (is_null() || p_callable.is_null()) return;
    auto &reg = get_signal_registry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    auto &list = reg.table[*this];
    if (std::find(list.begin(), list.end(), p_callable) == list.end()) {
        list.push_back(p_callable);
    }
}

void Signal::disconnect(const Callable &p_callable) {
    if (is_null()) return;
    auto &reg = get_signal_registry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    auto it = reg.table.find(*this);
    if (it != reg.table.end()) {
        auto &list = it->second;
        auto pos = std::find(list.begin(), list.end(), p_callable);
        if (pos != list.end()) {
            list.erase(pos);
        }
        if (list.empty()) {
            reg.table.erase(it);
        }
    }
}

bool Signal::is_connected(const Callable &p_callable) const {
    if (is_null()) return false;
    auto &reg = get_signal_registry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    auto it = reg.table.find(*this);
    if (it != reg.table.end()) {
        const auto &list = it->second;
        return std::find(list.begin(), list.end(), p_callable) != list.end();
    }
    return false;
}

void Signal::emit(const Variant **p_args, int p_argcount) const {
    if (is_null()) return;
    std::vector<Callable> copy_list;
    {
        auto &reg = get_signal_registry();
        std::lock_guard<std::mutex> lock(reg.mutex);
        auto it = reg.table.find(*this);
        if (it != reg.table.end()) {
            copy_list = it->second;
        }
    }
    for (const auto &c : copy_list) {
        Variant ret;
        Callable::CallError err;
        c.callp(p_args, p_argcount, ret, err);
    }
}

Array Signal::get_connections() const {
    Array arr;
    if (is_null()) return arr;
    auto &reg = get_signal_registry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    auto it = reg.table.find(*this);
    if (it != reg.table.end()) {
        for (const auto &c : it->second) {
            arr.push_back(Variant(c));
        }
    }
    return arr;
}

} // namespace Beyota
