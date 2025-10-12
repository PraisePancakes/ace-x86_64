#pragma once
#include <iostream>
#include <unordered_map>

namespace acc {

template <typename Key, typename Value, typename Item>
class environment {
    environment<Key, Value, Item>* m_parent;
    std::unordered_map<Key, Value> m_symbols;
    std::vector<Item> m_items;

   public:
    environment() : m_parent{nullptr}, m_symbols{}, m_items{} {};
    environment* resolve(Key key) {
        if (m_symbols.contains(key)) {
            return this;
        }
        if (!m_parent) return nullptr;
        return m_parent->resolve(key);
    };
    auto get_parent() const {
        return m_parent;
    }
    void set(Key key, Value v) {
        m_symbols.insert(std::make_pair(key, v));
    };

    std::vector<Item>& get_items() {
        return m_items;
    }

    environment<Key, Value, Item>* get_root() {
        auto* curr = this;
        while (curr->m_parent) {
            curr = curr->m_parent;
        }
        return curr;
    };

    void set_parent(acc::environment<Key, Value, Item>* parent) {
        m_parent = parent;
    };

    Value& get(Key key) {
        if (auto* ptr = this->resolve(key)) {
            return ptr->m_symbols.at(key);
        }
        throw std::runtime_error("failed to resolve key");
    };
};
}  // namespace acc