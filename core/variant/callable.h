/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  callable.h                                                            */
/**************************************************************************/

#pragma once

#include "core/object/object_id.h"
#include "core/string/string_name.h"
#include "core/templates/hashfuncs.h"
#include "core/type_primitives.h"

#include <compare>
#include <vector>

namespace Beyota {

class Variant;
class Array;
class Object;

class CallableCustom {
protected:
    std::atomic<u32> _refcount{1};

public:
    virtual ~CallableCustom() = default;

    void ref() noexcept {
        _refcount.fetch_add(1, std::memory_order_relaxed);
    }

    bool unref() noexcept {
        return _refcount.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }

    [[nodiscard]] u32 get_refcount() const noexcept {
        return _refcount.load(std::memory_order_relaxed);
    }

    [[nodiscard]] virtual u32 hash() const = 0;
    [[nodiscard]] virtual String get_as_text() const = 0;
    [[nodiscard]] virtual ObjectID get_object_id() const = 0;
};

class alignas(8) Callable {
public:
    struct CallError {
        enum Error {
            CALL_OK,
            CALL_ERROR_INVALID_METHOD,
            CALL_ERROR_INVALID_ARGUMENT,
            CALL_ERROR_TOO_MANY_ARGUMENTS,
            CALL_ERROR_TOO_FEW_ARGUMENTS,
            CALL_ERROR_INSTANCE_IS_NULL,
            CALL_ERROR_METHOD_NOT_CONST
        };
        Error error{CALL_OK};
        int argument{0};
        int expected{0};
    };

private:
    StringName method;
    union {
        u64 object{0};
        CallableCustom *custom;
    };

    static constexpr u64 CUSTOM_FLAG = 1ULL << 63;

public:
    constexpr Callable() noexcept = default;

    Callable(ObjectID p_object, const StringName &p_method) noexcept
        : method(p_method), object(static_cast<u64>(p_object)) {}

    Callable(const Object *p_object, const StringName &p_method) noexcept;

    Callable(CallableCustom *p_custom) noexcept
        : method(), custom(p_custom) {
        if (p_custom) {
            p_custom->ref();
            object |= CUSTOM_FLAG;
        }
    }

    Callable(const Callable &p_other) noexcept
        : method(p_other.method), object(p_other.object) {
        if (is_custom() && custom_ptr()) {
            custom_ptr()->ref();
        }
    }

    Callable(Callable &&p_other) noexcept
        : method(std::move(p_other.method)), object(p_other.object) {
        p_other.object = 0;
    }

    ~Callable() noexcept {
        if (is_custom() && custom_ptr()) {
            if (custom_ptr()->unref()) {
                delete custom_ptr();
            }
        }
    }

    Callable &operator=(const Callable &p_other) noexcept {
        if (this != &p_other) {
            if (is_custom() && custom_ptr()) {
                if (custom_ptr()->unref()) {
                    delete custom_ptr();
                }
            }
            method = p_other.method;
            object = p_other.object;
            if (is_custom() && custom_ptr()) {
                custom_ptr()->ref();
            }
        }
        return *this;
    }

    Callable &operator=(Callable &&p_other) noexcept {
        if (this != &p_other) {
            if (is_custom() && custom_ptr()) {
                if (custom_ptr()->unref()) {
                    delete custom_ptr();
                }
            }
            method = std::move(p_other.method);
            object = p_other.object;
            p_other.object = 0;
        }
        return *this;
    }

    [[nodiscard]] bool is_null() const noexcept {
        return object == 0 && method.is_empty();
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return !is_null();
    }

    [[nodiscard]] bool is_custom() const noexcept {
        return (object & CUSTOM_FLAG) != 0;
    }

    [[nodiscard]] CallableCustom *custom_ptr() const noexcept {
        if (!is_custom()) return nullptr;
        u64 raw = object & ~CUSTOM_FLAG;
        return reinterpret_cast<CallableCustom *>(raw);
    }

    [[nodiscard]] ObjectID get_object_id() const noexcept {
        if (is_custom() && custom_ptr()) {
            return custom_ptr()->get_object_id();
        }
        return ObjectID(object);
    }

    [[nodiscard]] StringName get_method() const noexcept {
        return method;
    }

    [[nodiscard]] u32 hash() const noexcept {
        if (is_custom() && custom_ptr()) {
            return custom_ptr()->hash();
        }
        return hash_make_uint32_t(object) ^ method.hash();
    }

    [[nodiscard]] bool operator==(const Callable &p_other) const noexcept {
        return object == p_other.object && method == p_other.method;
    }

    [[nodiscard]] auto operator<=>(const Callable &p_other) const noexcept {
        if (object != p_other.object) {
            return object <=> p_other.object;
        }
        return method <=> p_other.method;
    }

    void callp(const Variant **p_args, int p_argcount, Variant &r_ret, CallError &r_error) const;
    Variant callv(const Array &p_args) const;
};

class alignas(8) Signal {
    StringName name;
    ObjectID object;

public:
    constexpr Signal() noexcept = default;

    Signal(ObjectID p_object, const StringName &p_name) noexcept
        : name(p_name), object(p_object) {}

    Signal(const Object *p_object, const StringName &p_name) noexcept;

    [[nodiscard]] bool is_null() const noexcept {
        return object.is_null() && name.is_empty();
    }

    [[nodiscard]] ObjectID get_object_id() const noexcept {
        return object;
    }

    [[nodiscard]] StringName get_name() const noexcept {
        return name;
    }

    [[nodiscard]] u32 hash() const noexcept {
        return object.hash() ^ name.hash();
    }

    [[nodiscard]] bool operator==(const Signal &p_other) const noexcept {
        return object == p_other.object && name == p_other.name;
    }

    [[nodiscard]] auto operator<=>(const Signal &p_other) const noexcept {
        if (object != p_other.object) {
            return object <=> p_other.object;
        }
        return name <=> p_other.name;
    }

    void emit(const Variant **p_args, int p_argcount) const;
    void connect(const Callable &p_callable);
    void disconnect(const Callable &p_callable);
    [[nodiscard]] bool is_connected(const Callable &p_callable) const;
    [[nodiscard]] Array get_connections() const;
};

} // namespace Beyota
