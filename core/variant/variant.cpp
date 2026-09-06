/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant.cpp                                                           */
/**************************************************************************/

#include "core/variant/variant.h"
#include "core/object/object.h"
#include "core/object/object_db.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant_internal.h"
#include "core/variant/variant_op.h"

#include <cmath>
#include <format>
#include <new>

namespace Beyota {

Callable::Callable(const Object *p_object, const StringName &p_method) noexcept
    : method(p_method), object(p_object ? static_cast<u64>(p_object->get_instance_id()) : 0) {}

Signal::Signal(const Object *p_object, const StringName &p_name) noexcept
    : name(p_name), object(p_object ? p_object->get_instance_id() : ObjectID{}) {}

Variant::ObjData::ObjData(const ObjectID &p_id, Object *p_obj) noexcept {
    ref_pointer(p_obj);
    if (!p_obj && p_id.is_valid()) {
        id = p_id;
    }
}

Variant::ObjData::ObjData(const ObjData &p_other) noexcept {
    ref(p_other);
}

Variant::ObjData::ObjData(ObjData &&p_other) noexcept : id(p_other.id), obj(p_other.obj) {
    p_other.id = ObjectID();
    p_other.obj = nullptr;
}

Variant::ObjData::~ObjData() noexcept {
    unref();
}

Variant::ObjData &Variant::ObjData::operator=(const ObjData &p_other) noexcept {
    if (this != &p_other) {
        ref(p_other);
    }
    return *this;
}

Variant::ObjData &Variant::ObjData::operator=(ObjData &&p_other) noexcept {
    if (this != &p_other) {
        unref();
        id = p_other.id;
        obj = p_other.obj;
        p_other.id = ObjectID();
        p_other.obj = nullptr;
    }
    return *this;
}

void Variant::ObjData::ref(const ObjData &p_from) noexcept {
    if (this == &p_from) return;
    ObjectID old_id = id;
    Object *old_obj = obj;

    id = p_from.id;
    obj = p_from.obj;

    if (id.is_ref_counted() && obj) {
        RefCounted *rc = static_cast<RefCounted *>(obj);
        if (!rc->reference()) {
            id = ObjectID();
            obj = nullptr;
        }
    }

    if (old_id.is_ref_counted() && old_obj) {
        RefCounted *rc = static_cast<RefCounted *>(old_obj);
        if (rc->unreference()) {
            delete rc;
        }
    }
}

void Variant::ObjData::ref_pointer(Object *p_obj) noexcept {
    if (p_obj == obj) return;
    ObjectID old_id = id;
    Object *old_obj = obj;

    if (p_obj) {
        id = p_obj->get_instance_id();
        obj = p_obj;
        if (id.is_ref_counted()) {
            RefCounted *rc = static_cast<RefCounted *>(obj);
            if (!rc->init_ref()) {
                id = ObjectID();
                obj = nullptr;
            }
        }
    } else {
        id = ObjectID();
        obj = nullptr;
    }

    if (old_id.is_ref_counted() && old_obj) {
        RefCounted *rc = static_cast<RefCounted *>(old_obj);
        if (rc->unreference()) {
            delete rc;
        }
    }
}

void Variant::ObjData::unref() noexcept {
    if (id.is_ref_counted() && obj) {
        RefCounted *rc = static_cast<RefCounted *>(obj);
        if (rc->unreference()) {
            delete rc;
        }
    }
    id = ObjectID();
    obj = nullptr;
}

void Variant::_clear_internal() noexcept {
    switch (type) {
        case STRING:
            VariantInternal::get_string(this).~String();
            break;
        case TRANSFORM2D:
            VariantPools::free(_data._transform2d);
            _data._transform2d = nullptr;
            break;
        case AABB:
            VariantPools::free(_data._aabb);
            _data._aabb = nullptr;
            break;
        case BASIS:
            VariantPools::free(_data._basis);
            _data._basis = nullptr;
            break;
        case TRANSFORM3D:
            VariantPools::free(_data._transform3d);
            _data._transform3d = nullptr;
            break;
        case PROJECTION:
            VariantPools::free(_data._projection);
            _data._projection = nullptr;
            break;
        case STRING_NAME:
            VariantInternal::get_string_name(this).~StringName();
            break;
        case NODE_PATH:
            VariantInternal::get_node_path(this).~NodePath();
            break;
        case OBJECT:
            VariantInternal::get_obj(this).~ObjData();
            break;
        case CALLABLE:
            VariantInternal::get_callable(this).~Callable();
            break;
        case SIGNAL:
            VariantInternal::get_signal(this).~Signal();
            break;
        case DICTIONARY:
            VariantInternal::get_dictionary(this).~Dictionary();
            break;
        case ARRAY:
            VariantInternal::get_array(this).~Array();
            break;
        case PACKED_BYTE_ARRAY:
        case PACKED_INT32_ARRAY:
        case PACKED_INT64_ARRAY:
        case PACKED_FLOAT32_ARRAY:
        case PACKED_FLOAT64_ARRAY:
        case PACKED_STRING_ARRAY:
        case PACKED_VECTOR2_ARRAY:
        case PACKED_VECTOR3_ARRAY:
        case PACKED_COLOR_ARRAY:
        case PACKED_VECTOR4_ARRAY:
            if (_data.packed_array != nullptr) {
                if (_data.packed_array->unref()) {
                    delete _data.packed_array;
                }
                _data.packed_array = nullptr;
            }
            break;
        default:
            break;
    }
}

// Primitive constructors
Variant::Variant(bool p_bool) noexcept : type(BOOL) {
    _data._bool = p_bool;
}

Variant::Variant(i64 p_int) noexcept : type(INT) {
    _data._int = p_int;
}

Variant::Variant(f64 p_float) noexcept : type(FLOAT) {
    _data._float = p_float;
}

// String constructors
Variant::Variant(const String &p_string) : type(STRING) {
    new (_data._mem) String(p_string);
}

Variant::Variant(String &&p_string) : type(STRING) {
    new (_data._mem) String(std::move(p_string));
}

Variant::Variant(const char *p_cstring) : type(STRING) {
    new (_data._mem) String(p_cstring);
}

Variant::Variant(std::string_view p_str) : type(STRING) {
    new (_data._mem) String(p_str);
}

// Math constructors
Variant::Variant(const Vector2 &p_vec2) noexcept : type(VECTOR2) {
    *reinterpret_cast<Vector2 *>(_data._mem) = p_vec2;
}

Variant::Variant(const Vector2i &p_vec2i) noexcept : type(VECTOR2I) {
    *reinterpret_cast<Vector2i *>(_data._mem) = p_vec2i;
}

Variant::Variant(const Rect2 &p_rect2) noexcept : type(RECT2) {
    *reinterpret_cast<Rect2 *>(_data._mem) = p_rect2;
}

Variant::Variant(const Rect2i &p_rect2i) noexcept : type(RECT2I) {
    *reinterpret_cast<Rect2i *>(_data._mem) = p_rect2i;
}

Variant::Variant(const Vector3 &p_vec3) noexcept : type(VECTOR3) {
    *reinterpret_cast<Vector3 *>(_data._mem) = p_vec3;
}

Variant::Variant(const Vector3i &p_vec3i) noexcept : type(VECTOR3I) {
    *reinterpret_cast<Vector3i *>(_data._mem) = p_vec3i;
}

Variant::Variant(const Transform2D &p_trans2d) : type(TRANSFORM2D) {
    _data._transform2d = VariantPools::alloc<Transform2D>();
    *_data._transform2d = p_trans2d;
}

Variant::Variant(const Vector4 &p_vec4) noexcept : type(VECTOR4) {
    *reinterpret_cast<Vector4 *>(_data._mem) = p_vec4;
}

Variant::Variant(const Vector4i &p_vec4i) noexcept : type(VECTOR4I) {
    *reinterpret_cast<Vector4i *>(_data._mem) = p_vec4i;
}

Variant::Variant(const Plane &p_plane) noexcept : type(PLANE) {
    *reinterpret_cast<Plane *>(_data._mem) = p_plane;
}

Variant::Variant(const Quaternion &p_quat) noexcept : type(QUATERNION) {
    *reinterpret_cast<Quaternion *>(_data._mem) = p_quat;
}

Variant::Variant(const Math::AABB &p_aabb) : type(AABB) {
    _data._aabb = VariantPools::alloc<Math::AABB>();
    *_data._aabb = p_aabb;
}

Variant::Variant(const Basis &p_basis) : type(BASIS) {
    _data._basis = VariantPools::alloc<Basis>();
    *_data._basis = p_basis;
}

Variant::Variant(const Transform3D &p_trans3d) : type(TRANSFORM3D) {
    _data._transform3d = VariantPools::alloc<Transform3D>();
    *_data._transform3d = p_trans3d;
}

Variant::Variant(const Projection &p_proj) : type(PROJECTION) {
    _data._projection = VariantPools::alloc<Projection>();
    *_data._projection = p_proj;
}

Variant::Variant(const Color &p_color) noexcept : type(COLOR) {
    *reinterpret_cast<Color *>(_data._mem) = p_color;
}

// Misc constructors
Variant::Variant(const StringName &p_name) : type(STRING_NAME) {
    new (_data._mem) StringName(p_name);
}

Variant::Variant(StringName &&p_name) : type(STRING_NAME) {
    new (_data._mem) StringName(std::move(p_name));
}

Variant::Variant(const NodePath &p_path) : type(NODE_PATH) {
    new (_data._mem) NodePath(p_path);
}

Variant::Variant(NodePath &&p_path) : type(NODE_PATH) {
    new (_data._mem) NodePath(std::move(p_path));
}

Variant::Variant(const Beyota::RID &p_rid) noexcept : type(RID) {
    new (_data._mem) Beyota::RID(p_rid);
}

Variant::Variant(const ObjectID &p_id) noexcept : type(OBJECT) {
    new (_data._mem) ObjData{p_id, nullptr};
}

Variant::Variant(const Object *p_obj) noexcept : type(OBJECT) {
    new (_data._mem) ObjData{p_obj ? p_obj->get_instance_id() : ObjectID{}, const_cast<Object *>(p_obj)};
}

Variant::Variant(const Callable &p_callable) : type(CALLABLE) {
    new (_data._mem) Callable(p_callable);
}

Variant::Variant(Callable &&p_callable) : type(CALLABLE) {
    new (_data._mem) Callable(std::move(p_callable));
}

Variant::Variant(const Signal &p_signal) : type(SIGNAL) {
    new (_data._mem) Signal(p_signal);
}

Variant::Variant(Signal &&p_signal) : type(SIGNAL) {
    new (_data._mem) Signal(std::move(p_signal));
}

Variant::Variant(const Dictionary &p_dict) : type(DICTIONARY) {
    new (_data._mem) Dictionary(p_dict);
}

Variant::Variant(Dictionary &&p_dict) : type(DICTIONARY) {
    new (_data._mem) Dictionary(std::move(p_dict));
}

Variant::Variant(const Array &p_array) : type(ARRAY) {
    new (_data._mem) Array(p_array);
}

Variant::Variant(Array &&p_array) : type(ARRAY) {
    new (_data._mem) Array(std::move(p_array));
}

// Packed arrays constructors
Variant::Variant(const PackedByteArray &p_array) : type(PACKED_BYTE_ARRAY) {
    _data.packed_array = new PackedArrayRef<u8>(p_array);
}

Variant::Variant(const PackedInt32Array &p_array) : type(PACKED_INT32_ARRAY) {
    _data.packed_array = new PackedArrayRef<i32>(p_array);
}

Variant::Variant(const PackedInt64Array &p_array) : type(PACKED_INT64_ARRAY) {
    _data.packed_array = new PackedArrayRef<i64>(p_array);
}

Variant::Variant(const PackedFloat32Array &p_array) : type(PACKED_FLOAT32_ARRAY) {
    _data.packed_array = new PackedArrayRef<f32>(p_array);
}

Variant::Variant(const PackedFloat64Array &p_array) : type(PACKED_FLOAT64_ARRAY) {
    _data.packed_array = new PackedArrayRef<f64>(p_array);
}

Variant::Variant(const PackedStringArray &p_array) : type(PACKED_STRING_ARRAY) {
    _data.packed_array = new PackedArrayRef<String>(p_array);
}

Variant::Variant(const PackedVector2Array &p_array) : type(PACKED_VECTOR2_ARRAY) {
    _data.packed_array = new PackedArrayRef<Vector2>(p_array);
}

Variant::Variant(const PackedVector3Array &p_array) : type(PACKED_VECTOR3_ARRAY) {
    _data.packed_array = new PackedArrayRef<Vector3>(p_array);
}

Variant::Variant(const PackedColorArray &p_array) : type(PACKED_COLOR_ARRAY) {
    _data.packed_array = new PackedArrayRef<Color>(p_array);
}

Variant::Variant(const PackedVector4Array &p_array) : type(PACKED_VECTOR4_ARRAY) {
    _data.packed_array = new PackedArrayRef<Vector4>(p_array);
}

// Copy constructor
Variant::Variant(const Variant &p_other) : type(p_other.type) {
    switch (type) {
        case NIL:
            _data._int = 0;
            break;
        case BOOL:
            _data._bool = p_other._data._bool;
            break;
        case INT:
            _data._int = p_other._data._int;
            break;
        case FLOAT:
            _data._float = p_other._data._float;
            break;
        case STRING:
            new (_data._mem) String(VariantInternal::get_string(&p_other));
            break;
        case VECTOR2:
            *reinterpret_cast<Vector2 *>(_data._mem) = VariantInternal::get_vector2(&p_other);
            break;
        case VECTOR2I:
            *reinterpret_cast<Vector2i *>(_data._mem) = VariantInternal::get_vector2i(&p_other);
            break;
        case RECT2:
            *reinterpret_cast<Rect2 *>(_data._mem) = VariantInternal::get_rect2(&p_other);
            break;
        case RECT2I:
            *reinterpret_cast<Rect2i *>(_data._mem) = VariantInternal::get_rect2i(&p_other);
            break;
        case VECTOR3:
            *reinterpret_cast<Vector3 *>(_data._mem) = VariantInternal::get_vector3(&p_other);
            break;
        case VECTOR3I:
            *reinterpret_cast<Vector3i *>(_data._mem) = VariantInternal::get_vector3i(&p_other);
            break;
        case TRANSFORM2D:
            _data._transform2d = VariantPools::alloc<Transform2D>();
            *_data._transform2d = *p_other._data._transform2d;
            break;
        case VECTOR4:
            *reinterpret_cast<Vector4 *>(_data._mem) = VariantInternal::get_vector4(&p_other);
            break;
        case VECTOR4I:
            *reinterpret_cast<Vector4i *>(_data._mem) = VariantInternal::get_vector4i(&p_other);
            break;
        case PLANE:
            *reinterpret_cast<Plane *>(_data._mem) = VariantInternal::get_plane(&p_other);
            break;
        case QUATERNION:
            *reinterpret_cast<Quaternion *>(_data._mem) = VariantInternal::get_quaternion(&p_other);
            break;
        case AABB:
            _data._aabb = VariantPools::alloc<Math::AABB>();
            *_data._aabb = *p_other._data._aabb;
            break;
        case BASIS:
            _data._basis = VariantPools::alloc<Basis>();
            *_data._basis = *p_other._data._basis;
            break;
        case TRANSFORM3D:
            _data._transform3d = VariantPools::alloc<Transform3D>();
            *_data._transform3d = *p_other._data._transform3d;
            break;
        case PROJECTION:
            _data._projection = VariantPools::alloc<Projection>();
            *_data._projection = *p_other._data._projection;
            break;
        case COLOR:
            *reinterpret_cast<Color *>(_data._mem) = VariantInternal::get_color(&p_other);
            break;
        case STRING_NAME:
            new (_data._mem) StringName(VariantInternal::get_string_name(&p_other));
            break;
        case NODE_PATH:
            new (_data._mem) NodePath(VariantInternal::get_node_path(&p_other));
            break;
        case RID:
            new (_data._mem) Beyota::RID(VariantInternal::get_rid(&p_other));
            break;
        case OBJECT:
            new (_data._mem) ObjData(VariantInternal::get_obj(&p_other));
            break;
        case CALLABLE:
            new (_data._mem) Callable(VariantInternal::get_callable(&p_other));
            break;
        case SIGNAL:
            new (_data._mem) Signal(VariantInternal::get_signal(&p_other));
            break;
        case DICTIONARY:
            new (_data._mem) Dictionary(VariantInternal::get_dictionary(&p_other));
            break;
        case ARRAY:
            new (_data._mem) Array(VariantInternal::get_array(&p_other));
            break;
        case PACKED_BYTE_ARRAY:
        case PACKED_INT32_ARRAY:
        case PACKED_INT64_ARRAY:
        case PACKED_FLOAT32_ARRAY:
        case PACKED_FLOAT64_ARRAY:
        case PACKED_STRING_ARRAY:
        case PACKED_VECTOR2_ARRAY:
        case PACKED_VECTOR3_ARRAY:
        case PACKED_COLOR_ARRAY:
        case PACKED_VECTOR4_ARRAY:
            _data.packed_array = p_other._data.packed_array;
            if (_data.packed_array != nullptr) {
                _data.packed_array->ref();
            }
            break;
        default:
            std::memcpy(_data._mem, p_other._data._mem, sizeof(_data._mem));
            break;
    }
}

// Move constructor
Variant::Variant(Variant &&p_other) noexcept : type(p_other.type) {
    std::memcpy(&_data, &p_other._data, sizeof(_data));
    p_other.type = NIL;
    p_other._data._int = 0;
}

Variant &Variant::operator=(const Variant &p_other) {
    if (this != &p_other) {
        clear();
        new (this) Variant(p_other);
    }
    return *this;
}

Variant &Variant::operator=(Variant &&p_other) noexcept {
    if (this != &p_other) {
        clear();
        type = p_other.type;
        std::memcpy(&_data, &p_other._data, sizeof(_data));
        p_other.type = NIL;
        p_other._data._int = 0;
    }
    return *this;
}

bool Variant::booleanize() const noexcept {
    switch (type) {
        case NIL: return false;
        case BOOL: return _data._bool;
        case INT: return _data._int != 0;
        case FLOAT: return _data._float != 0.0 && !std::isnan(_data._float);
        case STRING: return !VariantInternal::get_string(this).is_empty();
        case VECTOR2: return VariantInternal::get_vector2(this) != Vector2();
        case VECTOR2I: return VariantInternal::get_vector2i(this) != Vector2i();
        case RECT2: return VariantInternal::get_rect2(this) != Rect2();
        case RECT2I: return VariantInternal::get_rect2i(this) != Rect2i();
        case VECTOR3: return VariantInternal::get_vector3(this) != Vector3();
        case VECTOR3I: return VariantInternal::get_vector3i(this) != Vector3i();
        case TRANSFORM2D: return true;
        case VECTOR4: return VariantInternal::get_vector4(this) != Vector4();
        case VECTOR4I: return VariantInternal::get_vector4i(this) != Vector4i();
        case PLANE: return true;
        case QUATERNION: return true;
        case AABB: return true;
        case BASIS: return true;
        case TRANSFORM3D: return true;
        case PROJECTION: return true;
        case COLOR: return true;
        case STRING_NAME: return !VariantInternal::get_string_name(this).is_empty();
        case NODE_PATH: return !VariantInternal::get_node_path(this).is_empty();
        case RID: return VariantInternal::get_rid(this).is_valid();
        case OBJECT: return VariantInternal::get_obj(this).id.is_valid();
        case CALLABLE: return VariantInternal::get_callable(this).is_valid();
        case SIGNAL: return !VariantInternal::get_signal(this).is_null();
        case DICTIONARY: return !VariantInternal::get_dictionary(this).is_empty();
        case ARRAY: return !VariantInternal::get_array(this).is_empty();
        case PACKED_BYTE_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<u8>(this).is_empty();
        case PACKED_INT32_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<i32>(this).is_empty();
        case PACKED_INT64_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<i64>(this).is_empty();
        case PACKED_FLOAT32_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<f32>(this).is_empty();
        case PACKED_FLOAT64_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<f64>(this).is_empty();
        case PACKED_STRING_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<String>(this).is_empty();
        case PACKED_VECTOR2_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<Vector2>(this).is_empty();
        case PACKED_VECTOR3_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<Vector3>(this).is_empty();
        case PACKED_COLOR_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<Color>(this).is_empty();
        case PACKED_VECTOR4_ARRAY: return _data.packed_array && !VariantInternal::get_packed_array<Vector4>(this).is_empty();
        default: return false;
    }
}

// Cast operators
Variant::operator i64() const noexcept {
    switch (type) {
        case NIL: return 0;
        case BOOL: return _data._bool ? 1 : 0;
        case INT: return _data._int;
        case FLOAT: return static_cast<i64>(_data._float);
        case STRING: return VariantInternal::get_string(this).to_int();
        default: return 0;
    }
}

Variant::operator f64() const noexcept {
    switch (type) {
        case NIL: return 0.0;
        case BOOL: return _data._bool ? 1.0 : 0.0;
        case INT: return static_cast<f64>(_data._int);
        case FLOAT: return _data._float;
        case STRING: return VariantInternal::get_string(this).to_float();
        default: return 0.0;
    }
}

Variant::operator String() const {
    return stringify();
}

Variant::operator Vector2() const {
    if (type == VECTOR2) return VariantInternal::get_vector2(this);
    if (type == VECTOR2I) return Vector2(VariantInternal::get_vector2i(this));
    return Vector2();
}

Variant::operator Vector2i() const {
    if (type == VECTOR2I) return VariantInternal::get_vector2i(this);
    if (type == VECTOR2) return Vector2i(VariantInternal::get_vector2(this));
    return Vector2i();
}

Variant::operator Rect2() const {
    if (type == RECT2) return VariantInternal::get_rect2(this);
    if (type == RECT2I) return Rect2(VariantInternal::get_rect2i(this));
    return Rect2();
}

Variant::operator Rect2i() const {
    if (type == RECT2I) return VariantInternal::get_rect2i(this);
    if (type == RECT2) return Rect2i(VariantInternal::get_rect2(this));
    return Rect2i();
}

Variant::operator Vector3() const {
    if (type == VECTOR3) return VariantInternal::get_vector3(this);
    if (type == VECTOR3I) return Vector3(VariantInternal::get_vector3i(this));
    return Vector3();
}

Variant::operator Vector3i() const {
    if (type == VECTOR3I) return VariantInternal::get_vector3i(this);
    if (type == VECTOR3) return Vector3i(VariantInternal::get_vector3(this));
    return Vector3i();
}

Variant::operator Transform2D() const {
    if (type == TRANSFORM2D && _data._transform2d) return *_data._transform2d;
    return Transform2D();
}

Variant::operator Vector4() const {
    if (type == VECTOR4) return VariantInternal::get_vector4(this);
    if (type == VECTOR4I) return Vector4(VariantInternal::get_vector4i(this));
    return Vector4();
}

Variant::operator Vector4i() const {
    if (type == VECTOR4I) return VariantInternal::get_vector4i(this);
    if (type == VECTOR4) return Vector4i(VariantInternal::get_vector4(this));
    return Vector4i();
}

Variant::operator Plane() const {
    if (type == PLANE) return VariantInternal::get_plane(this);
    return Plane();
}

Variant::operator Quaternion() const {
    if (type == QUATERNION) return VariantInternal::get_quaternion(this);
    if (type == BASIS && _data._basis) return _data._basis->get_quaternion();
    return Quaternion();
}

Variant::operator Math::AABB() const {
    if (type == AABB && _data._aabb) return *_data._aabb;
    return Math::AABB();
}

Variant::operator Basis() const {
    if (type == BASIS && _data._basis) return *_data._basis;
    if (type == QUATERNION) return Basis(VariantInternal::get_quaternion(this));
    return Basis();
}

Variant::operator Transform3D() const {
    if (type == TRANSFORM3D && _data._transform3d) return *_data._transform3d;
    return Transform3D();
}

Variant::operator Projection() const {
    if (type == PROJECTION && _data._projection) return *_data._projection;
    if (type == TRANSFORM3D && _data._transform3d) return Projection(*_data._transform3d);
    return Projection();
}

Variant::operator Color() const {
    if (type == COLOR) return VariantInternal::get_color(this);
    return Color();
}

Variant::operator StringName() const {
    if (type == STRING_NAME) return VariantInternal::get_string_name(this);
    if (type == STRING) return StringName(VariantInternal::get_string(this));
    return StringName();
}

Variant::operator NodePath() const {
    if (type == NODE_PATH) return VariantInternal::get_node_path(this);
    if (type == STRING) return NodePath(VariantInternal::get_string(this));
    return NodePath();
}

Variant::operator Beyota::RID() const {
    if (type == RID) return VariantInternal::get_rid(this);
    return Beyota::RID();
}

Variant::operator Object *() const {
    if (type == OBJECT) {
        const auto &objdata = VariantInternal::get_obj(this);
        if (objdata.id.is_null()) return nullptr;
        return ObjectDB::get_instance(objdata.id);
    }
    return nullptr;
}

Variant::operator ObjectID() const {
    if (type == OBJECT) return VariantInternal::get_obj(this).id;
    return ObjectID{};
}

Variant::operator Callable() const {
    if (type == CALLABLE) return VariantInternal::get_callable(this);
    return Callable();
}

Variant::operator Signal() const {
    if (type == SIGNAL) return VariantInternal::get_signal(this);
    return Signal();
}

Variant::operator Dictionary() const {
    if (type == DICTIONARY) return VariantInternal::get_dictionary(this);
    return Dictionary();
}

Variant::operator Array() const {
    if (type == ARRAY) return VariantInternal::get_array(this);
    return Array();
}

Variant::operator PackedByteArray() const {
    if (type == PACKED_BYTE_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<u8>(this);
    return PackedByteArray();
}

Variant::operator PackedInt32Array() const {
    if (type == PACKED_INT32_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<i32>(this);
    return PackedInt32Array();
}

Variant::operator PackedInt64Array() const {
    if (type == PACKED_INT64_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<i64>(this);
    return PackedInt64Array();
}

Variant::operator PackedFloat32Array() const {
    if (type == PACKED_FLOAT32_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<f32>(this);
    return PackedFloat32Array();
}

Variant::operator PackedFloat64Array() const {
    if (type == PACKED_FLOAT64_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<f64>(this);
    return PackedFloat64Array();
}

Variant::operator PackedStringArray() const {
    if (type == PACKED_STRING_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<String>(this);
    return PackedStringArray();
}

Variant::operator PackedVector2Array() const {
    if (type == PACKED_VECTOR2_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<Vector2>(this);
    return PackedVector2Array();
}

Variant::operator PackedVector3Array() const {
    if (type == PACKED_VECTOR3_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<Vector3>(this);
    return PackedVector3Array();
}

Variant::operator PackedColorArray() const {
    if (type == PACKED_COLOR_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<Color>(this);
    return PackedColorArray();
}

Variant::operator PackedVector4Array() const {
    if (type == PACKED_VECTOR4_ARRAY && _data.packed_array) return VariantInternal::get_packed_array<Vector4>(this);
    return PackedVector4Array();
}

u32 Variant::hash() const noexcept {
    switch (type) {
        case NIL: return 0;
        case BOOL: return hash_make_uint32_t(_data._bool ? 1U : 0U);
        case INT: return hash_make_uint32_t(_data._int);
        case FLOAT: return hash_make_uint32_t(_data._float);
        case STRING: return VariantInternal::get_string(this).hash();
        case VECTOR2: {
            const auto &v = VariantInternal::get_vector2(this);
            return hash_make_uint32_t(v.x) ^ (hash_make_uint32_t(v.y) << 1);
        }
        case VECTOR2I: {
            const auto &v = VariantInternal::get_vector2i(this);
            return hash_make_uint32_t(v.x) ^ (hash_make_uint32_t(v.y) << 1);
        }
        case VECTOR3: {
            const auto &v = VariantInternal::get_vector3(this);
            return hash_make_uint32_t(v.x) ^ (hash_make_uint32_t(v.y) << 1) ^ (hash_make_uint32_t(v.z) << 2);
        }
        case VECTOR3I: {
            const auto &v = VariantInternal::get_vector3i(this);
            return hash_make_uint32_t(v.x) ^ (hash_make_uint32_t(v.y) << 1) ^ (hash_make_uint32_t(v.z) << 2);
        }
        case STRING_NAME: return VariantInternal::get_string_name(this).hash();
        case NODE_PATH: return VariantInternal::get_node_path(this).hash();
        case RID: return VariantInternal::get_rid(this).hash();
        case OBJECT: return VariantInternal::get_obj(this).id.hash();
        case CALLABLE: return VariantInternal::get_callable(this).hash();
        case SIGNAL: return VariantInternal::get_signal(this).hash();
        case DICTIONARY: return VariantInternal::get_dictionary(this).hash();
        case ARRAY: return VariantInternal::get_array(this).hash();
        default:
            return hash_make_uint32_t(hash_djb2_buffer(_data._mem, sizeof(_data._mem)));
    }
}

String Variant::stringify() const {
    switch (type) {
        case NIL: return "null";
        case BOOL: return _data._bool ? "true" : "false";
        case INT: return String::num_int64(_data._int);
        case FLOAT: return String::num_real(_data._float);
        case STRING: return VariantInternal::get_string(this);
        case VECTOR2: {
            const auto &v = VariantInternal::get_vector2(this);
            return String(std::format("({}, {})", v.x, v.y));
        }
        case VECTOR2I: {
            const auto &v = VariantInternal::get_vector2i(this);
            return String(std::format("({}, {})", v.x, v.y));
        }
        case RECT2: {
            const auto &r = VariantInternal::get_rect2(this);
            return String(std::format("[P: ({}, {}), S: ({}, {})]", r.position.x, r.position.y, r.size.x, r.size.y));
        }
        case RECT2I: {
            const auto &r = VariantInternal::get_rect2i(this);
            return String(std::format("[P: ({}, {}), S: ({}, {})]", r.position.x, r.position.y, r.size.x, r.size.y));
        }
        case VECTOR3: {
            const auto &v = VariantInternal::get_vector3(this);
            return String(std::format("({}, {}, {})", v.x, v.y, v.z));
        }
        case VECTOR3I: {
            const auto &v = VariantInternal::get_vector3i(this);
            return String(std::format("({}, {}, {})", v.x, v.y, v.z));
        }
        case VECTOR4: {
            const auto &v = VariantInternal::get_vector4(this);
            return String(std::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w));
        }
        case VECTOR4I: {
            const auto &v = VariantInternal::get_vector4i(this);
            return String(std::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w));
        }
        case COLOR: {
            const auto &c = VariantInternal::get_color(this);
            return String(std::format("Color({}, {}, {}, {})", c.r, c.g, c.b, c.a));
        }
        case STRING_NAME: return String(VariantInternal::get_string_name(this));
        case NODE_PATH: return String(VariantInternal::get_node_path(this));
        case RID: return String(std::format("RID({})", VariantInternal::get_rid(this).get_id()));
        case OBJECT: {
            Object *obj = VariantInternal::get_obj(this).obj;
            if (obj != nullptr) {
                return obj->to_string();
            }
            return String(std::format("Object:{}", static_cast<u64>(VariantInternal::get_obj(this).id)));
        }
        case ARRAY: {
            const Array &arr = VariantInternal::get_array(this);
            String s = "[";
            for (usize i = 0; i < arr.size(); ++i) {
                if (i > 0) s += ", ";
                s += arr[i].stringify();
            }
            s += "]";
            return s;
        }
        case DICTIONARY: {
            const Dictionary &dict = VariantInternal::get_dictionary(this);
            String s = "{";
            usize i = 0;
            for (const auto &it : dict) {
                if (i > 0) s += ", ";
                s += it.key.stringify();
                s += ": ";
                s += it.value.stringify();
                ++i;
            }
            s += "}";
            return s;
        }
        default:
            return String(variant_type_to_string(type));
    }
}

// Operators
bool Variant::operator==(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_EQUAL, *this, p_other, ret, valid);
    return valid && ret.booleanize();
}

bool Variant::operator<(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_LESS, *this, p_other, ret, valid);
    return valid && ret.booleanize();
}

bool Variant::operator<=(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_LESS_EQUAL, *this, p_other, ret, valid);
    return valid && ret.booleanize();
}

bool Variant::operator>(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_GREATER, *this, p_other, ret, valid);
    return valid && ret.booleanize();
}

bool Variant::operator>=(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_GREATER_EQUAL, *this, p_other, ret, valid);
    return valid && ret.booleanize();
}

Variant Variant::operator+(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_ADD, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator-(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_SUBTRACT, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator*(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_MULTIPLY, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator/(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_DIVIDE, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator%(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_MODULE, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator-() const {
    Variant ret;
    bool valid = false;
    evaluate(OP_NEGATE, *this, Variant(), ret, valid);
    return ret;
}

Variant Variant::operator+() const {
    Variant ret;
    bool valid = false;
    evaluate(OP_POSITIVE, *this, Variant(), ret, valid);
    return ret;
}

Variant Variant::operator!() const {
    return Variant(!booleanize());
}

Variant Variant::operator&(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_BIT_AND, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator|(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_BIT_OR, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator^(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_BIT_XOR, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator~() const {
    Variant ret;
    bool valid = false;
    evaluate(OP_BIT_NEGATE, *this, Variant(), ret, valid);
    return ret;
}

Variant Variant::operator<<(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_SHIFT_LEFT, *this, p_other, ret, valid);
    return ret;
}

Variant Variant::operator>>(const Variant &p_other) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_SHIFT_RIGHT, *this, p_other, ret, valid);
    return ret;
}

bool Variant::in(const Variant &p_container) const {
    Variant ret;
    bool valid = false;
    evaluate(OP_IN, *this, p_container, ret, valid);
    return valid && ret.booleanize();
}

bool Variant::can_convert(Type p_from, Type p_to) noexcept {
    if (p_from == p_to) return true;
    if (p_from == NIL) return true;
    if (p_to == STRING) return true;
    if (p_to == BOOL) return true;
    if ((p_from == INT || p_from == FLOAT) && (p_to == INT || p_to == FLOAT)) return true;
    if (p_from == STRING && (p_to == INT || p_to == FLOAT)) return true;
    if (p_from == VECTOR2 && p_to == VECTOR2I) return true;
    if (p_from == VECTOR2I && p_to == VECTOR2) return true;
    if (p_from == VECTOR3 && p_to == VECTOR3I) return true;
    if (p_from == VECTOR3I && p_to == VECTOR3) return true;
    if (p_from == VECTOR4 && p_to == VECTOR4I) return true;
    if (p_from == VECTOR4I && p_to == VECTOR4) return true;
    if (p_from == RECT2 && p_to == RECT2I) return true;
    if (p_from == RECT2I && p_to == RECT2) return true;
    if (p_from == STRING && p_to == STRING_NAME) return true;
    if (p_from == STRING_NAME && p_to == STRING) return true;
    if (p_from == STRING && p_to == NODE_PATH) return true;
    if (p_from == NODE_PATH && p_to == STRING) return true;
    return false;
}

bool Variant::can_convert_strict(Type p_from, Type p_to) noexcept {
    if (p_from == p_to) return true;
    if ((p_from == INT && p_to == FLOAT) || (p_from == FLOAT && p_to == INT)) return true;
    if (p_from == STRING && p_to == STRING_NAME) return true;
    if (p_from == STRING_NAME && p_to == STRING) return true;
    return false;
}

} // namespace Beyota
