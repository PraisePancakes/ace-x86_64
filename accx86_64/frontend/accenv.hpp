#pragma once
#include <iostream>
#include <unordered_map>
namespace acc {

template <typename Key, typename Value>
class environment {
    environment<Key, Value>* m_parent;
    std::unordered_map<Key, Value> m_symbols;

   public:
    environment() : m_parent{nullptr} {};
    environment* resolve(Key key) {
        if (!m_parent && !m_symbols.contains(key)) return nullptr;

        if (m_symbols.contains(key)) {
            return this;
        }
        return m_parent->resolve(key);
    };

    void set(Key key, Value v) {
        m_symbols.insert(std::make_pair(key, v));
    };

    Value& get(Key key) noexcept {
        return this->m_symbols.at(key);
    };
};
}  // namespace acc