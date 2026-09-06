/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  dictionary.cpp                                                        */
/**************************************************************************/

#include "core/variant/dictionary.h"
#include "core/templates/hash_map.h"
#include "core/variant/array.h"
#include "core/variant/variant.h"

#include <atomic>

namespace Beyota {

struct DictionaryPrivate {
    std::atomic<u32> refcount{1};
    HashMap<Variant, Variant> map;

    DictionaryPrivate() = default;
    explicit DictionaryPrivate(const HashMap<Variant, Variant> &p_map) : map(p_map) {}
    explicit DictionaryPrivate(HashMap<Variant, Variant> &&p_map) : map(std::move(p_map)) {}
};

void Dictionary::_ref(const Dictionary &p_from) const noexcept {
    if (p_from._p != nullptr) {
        p_from._p->refcount.fetch_add(1, std::memory_order_relaxed);
        _p = p_from._p;
    } else {
        _p = nullptr;
    }
}

void Dictionary::_unref() const noexcept {
    if (_p != nullptr) {
        if (_p->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete _p;
        }
        _p = nullptr;
    }
}

void Dictionary::_ensure_p() {
    if (_p == nullptr) {
        _p = new DictionaryPrivate();
    }
}

Dictionary::Dictionary() noexcept : _p(nullptr) {}

Dictionary::Dictionary(const Dictionary &p_other) noexcept {
    _ref(p_other);
}

Dictionary::Dictionary(Dictionary &&p_other) noexcept : _p(p_other._p) {
    p_other._p = nullptr;
}

Dictionary::Dictionary(std::initializer_list<std::pair<Variant, Variant>> p_list) {
    if (p_list.size() > 0) {
        _ensure_p();
        for (const auto &item : p_list) {
            _p->map.insert(item.first, item.second);
        }
    }
}

Dictionary::~Dictionary() noexcept {
    _unref();
}

Dictionary &Dictionary::operator=(const Dictionary &p_other) noexcept {
    if (this != &p_other) {
        _unref();
        _ref(p_other);
    }
    return *this;
}

Dictionary &Dictionary::operator=(Dictionary &&p_other) noexcept {
    if (this != &p_other) {
        _unref();
        _p = p_other._p;
        p_other._p = nullptr;
    }
    return *this;
}

usize Dictionary::size() const noexcept {
    return _p ? _p->map.size() : 0;
}

bool Dictionary::is_empty() const noexcept {
    return _p == nullptr || _p->map.is_empty();
}

void Dictionary::clear() noexcept {
    _unref();
}

const Variant *Dictionary::getptr(const Variant &p_key) const noexcept {
    return _p ? _p->map.getptr(p_key) : nullptr;
}

Variant *Dictionary::getptr(const Variant &p_key) noexcept {
    if (_p == nullptr) {
        return nullptr;
    }
    return _p->map.getptr(p_key);
}

const Variant &Dictionary::operator[](const Variant &p_key) const {
    const Variant *v = getptr(p_key);
    if (v != nullptr) {
        return *v;
    }
    static const Variant nil;
    return nil;
}

Variant &Dictionary::operator[](const Variant &p_key) {
    _ensure_p();
    return _p->map[p_key];
}

Variant Dictionary::get(const Variant &p_key, const Variant &p_default) const {
    return _p ? _p->map.get(p_key, p_default) : p_default;
}

Variant &Dictionary::get_or_add(const Variant &p_key, const Variant &p_default) {
    _ensure_p();
    return _p->map.get_or_add(p_key, p_default);
}

bool Dictionary::has(const Variant &p_key) const {
    return _p ? _p->map.has(p_key) : false;
}

bool Dictionary::erase(const Variant &p_key) {
    return _p ? _p->map.erase(p_key) : false;
}

Array Dictionary::keys() const {
    Array arr;
    if (is_empty()) {
        return arr;
    }
    for (auto it = _p->map.begin(); it != _p->map.end(); ++it) {
        arr.push_back(it.key());
    }
    return arr;
}

Array Dictionary::values() const {
    Array arr;
    if (is_empty()) {
        return arr;
    }
    for (auto it = _p->map.begin(); it != _p->map.end(); ++it) {
        arr.push_back(it.value());
    }
    return arr;
}

Dictionary Dictionary::duplicate(bool p_deep) const {
    Dictionary res;
    if (is_empty()) {
        return res;
    }
    res._ensure_p();
    for (auto it = _p->map.begin(); it != _p->map.end(); ++it) {
        Variant key = it.key();
        Variant val = it.value();
        if (p_deep && val.get_type() == Variant::DICTIONARY) {
            val = ((Dictionary)val).duplicate(true);
        } else if (p_deep && val.get_type() == Variant::ARRAY) {
            val = ((Array)val).duplicate(true);
        }
        res._p->map.insert(std::move(key), std::move(val));
    }
    return res;
}

bool Dictionary::operator==(const Dictionary &p_other) const noexcept {
    if (_p == p_other._p) {
        return true;
    }
    if (size() != p_other.size()) {
        return false;
    }
    for (auto it = _p->map.begin(); it != _p->map.end(); ++it) {
        const Variant *other_val = p_other.getptr(it.key());
        if (other_val == nullptr || *other_val != it.value()) {
            return false;
        }
    }
    return true;
}

u32 Dictionary::hash() const noexcept {
    if (is_empty()) {
        return 0;
    }
    u32 h = 5381;
    for (auto it = _p->map.begin(); it != _p->map.end(); ++it) {
        h = ((h << 5) + h) + (it.key().hash() ^ it.value().hash());
    }
    return h;
}

const Variant &Dictionary::ConstIterator::key() const {
    assert(node_ != nullptr);
    return static_cast<const HashMap<Variant, Variant>::Element *>(node_)->key;
}

const Variant &Dictionary::ConstIterator::value() const {
    assert(node_ != nullptr);
    return static_cast<const HashMap<Variant, Variant>::Element *>(node_)->value;
}

Dictionary::ConstIterator &Dictionary::ConstIterator::operator++() noexcept {
    if (node_ != nullptr) {
        node_ = static_cast<const HashMap<Variant, Variant>::Element *>(node_)->next;
    }
    return *this;
}

Dictionary::ConstIterator Dictionary::ConstIterator::operator++(int) noexcept {
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
}

Dictionary::ConstIterator Dictionary::begin() const noexcept {
    return ConstIterator{_p ? _p->map.front() : nullptr};
}

Dictionary::ConstIterator Dictionary::end() const noexcept {
    return ConstIterator{nullptr};
}

} // namespace Beyota
