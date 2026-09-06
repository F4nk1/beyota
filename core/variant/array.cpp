/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  array.cpp                                                             */
/**************************************************************************/

#include "core/variant/array.h"
#include "core/variant/variant.h"

#include <algorithm>
#include <atomic>
#include <vector>

namespace Beyota {

struct ArrayPrivate {
    std::atomic<u32> refcount{1};
    std::vector<Variant> elements;

    ArrayPrivate() = default;
    explicit ArrayPrivate(usize p_size) : elements(p_size) {}
    explicit ArrayPrivate(const std::vector<Variant> &p_elements) : elements(p_elements) {}
    explicit ArrayPrivate(std::vector<Variant> &&p_elements) : elements(std::move(p_elements)) {}
};

void Array::_ref(const Array &p_from) const noexcept {
    if (p_from._p != nullptr) {
        p_from._p->refcount.fetch_add(1, std::memory_order_relaxed);
        _p = p_from._p;
    } else {
        _p = nullptr;
    }
}

void Array::_unref() const noexcept {
    if (_p != nullptr) {
        if (_p->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete _p;
        }
        _p = nullptr;
    }
}

void Array::_ensure_p() {
    if (_p == nullptr) {
        _p = new ArrayPrivate();
    }
}

Array::Array() noexcept : _p(nullptr) {}

Array::Array(const Array &p_other) noexcept {
    _ref(p_other);
}

Array::Array(Array &&p_other) noexcept : _p(p_other._p) {
    p_other._p = nullptr;
}

Array::Array(std::initializer_list<Variant> p_list) {
    if (p_list.size() > 0) {
        _p = new ArrayPrivate();
        _p->elements.assign(p_list.begin(), p_list.end());
    }
}

Array::~Array() noexcept {
    _unref();
}

Array &Array::operator=(const Array &p_other) noexcept {
    if (this != &p_other) {
        _unref();
        _ref(p_other);
    }
    return *this;
}

Array &Array::operator=(Array &&p_other) noexcept {
    if (this != &p_other) {
        _unref();
        _p = p_other._p;
        p_other._p = nullptr;
    }
    return *this;
}

usize Array::size() const noexcept {
    return _p ? _p->elements.size() : 0;
}

bool Array::is_empty() const noexcept {
    return _p == nullptr || _p->elements.empty();
}

void Array::clear() noexcept {
    _unref();
}

void Array::resize(usize p_size) {
    _ensure_p();
    _p->elements.resize(p_size);
}

const Variant &Array::operator[](usize p_idx) const {
    assert(_p && p_idx < _p->elements.size());
    return _p->elements[p_idx];
}

Variant &Array::operator[](usize p_idx) {
    assert(_p && p_idx < _p->elements.size());
    _ensure_p();
    return _p->elements[p_idx];
}

void Array::push_back(const Variant &p_value) {
    _ensure_p();
    _p->elements.push_back(p_value);
}

void Array::push_back(Variant &&p_value) {
    _ensure_p();
    _p->elements.push_back(std::move(p_value));
}

Variant Array::pop_back() {
    if (is_empty()) return Variant();
    _ensure_p();
    Variant val = std::move(_p->elements.back());
    _p->elements.pop_back();
    return val;
}

void Array::push_front(const Variant &p_value) {
    _ensure_p();
    _p->elements.insert(_p->elements.begin(), p_value);
}

Variant Array::pop_front() {
    if (is_empty()) return Variant();
    _ensure_p();
    Variant val = std::move(_p->elements.front());
    _p->elements.erase(_p->elements.begin());
    return val;
}

void Array::insert(usize p_pos, const Variant &p_value) {
    _ensure_p();
    usize pos = std::min(p_pos, _p->elements.size());
    _p->elements.insert(_p->elements.begin() + pos, p_value);
}

void Array::remove_at(usize p_pos) {
    if (is_empty() || p_pos >= size()) return;
    _ensure_p();
    _p->elements.erase(_p->elements.begin() + p_pos);
}

bool Array::erase(const Variant &p_value) {
    if (is_empty()) return false;
    _ensure_p();
    auto it = std::find(_p->elements.begin(), _p->elements.end(), p_value);
    if (it != _p->elements.end()) {
        _p->elements.erase(it);
        return true;
    }
    return false;
}

bool Array::has(const Variant &p_value) const {
    if (is_empty()) return false;
    return std::find(_p->elements.begin(), _p->elements.end(), p_value) != _p->elements.end();
}

i64 Array::find(const Variant &p_value, usize p_from) const {
    if (is_empty() || p_from >= size()) return -1;
    for (usize i = p_from; i < _p->elements.size(); ++i) {
        if (_p->elements[i] == p_value) return static_cast<i64>(i);
    }
    return -1;
}

usize Array::count(const Variant &p_value) const {
    if (is_empty()) return 0;
    return std::count(_p->elements.begin(), _p->elements.end(), p_value);
}

Array Array::duplicate(bool p_deep) const {
    Array res;
    if (is_empty()) return res;
    res._p = new ArrayPrivate();
    res._p->elements.reserve(_p->elements.size());
    for (const Variant &elem : _p->elements) {
        if (p_deep && elem.get_type() == Variant::ARRAY) {
            Array child = static_cast<Array>(elem);
            res._p->elements.push_back(child.duplicate(true));
        } else if (p_deep && elem.get_type() == Variant::DICTIONARY) {
            Dictionary child = static_cast<Dictionary>(elem);
            res._p->elements.push_back(child.duplicate(true));
        } else {
            res._p->elements.push_back(elem);
        }
    }
    return res;
}

Array Array::slice(usize p_begin, usize p_end) const {
    Array res;
    if (is_empty() || p_begin >= size()) return res;
    usize end = std::min(p_end, size());
    if (end <= p_begin) return res;
    res._p = new ArrayPrivate();
    res._p->elements.reserve(end - p_begin);
    for (usize i = p_begin; i < end; ++i) {
        res._p->elements.push_back(_p->elements[i]);
    }
    return res;
}

void Array::reverse() {
    if (size() <= 1) return;
    _ensure_p();
    std::reverse(_p->elements.begin(), _p->elements.end());
}

bool Array::operator==(const Array &p_other) const noexcept {
    if (_p == p_other._p) return true;
    if (size() != p_other.size()) return false;
    for (usize i = 0; i < size(); ++i) {
        if (_p->elements[i] != p_other._p->elements[i]) return false;
    }
    return true;
}

u32 Array::hash() const noexcept {
    if (is_empty()) return 0;
    u32 h = 5381;
    for (const Variant &elem : _p->elements) {
        h = ((h << 5) + h) + elem.hash();
    }
    return h;
}

const Variant &Array::ConstIterator::operator*() const {
    return (*_arr)[_idx];
}

Variant &Array::Iterator::operator*() const {
    return (*_arr)[_idx];
}

} // namespace Beyota
