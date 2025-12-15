#pragma once
#include <iostream>
#include <unordered_map>

namespace acc {

template <typename Key, typename Item>
class environment {
    environment<Key, Item>* m_parent;         // parent environment with its own set of symbols and data.
    std::unordered_map<Key, Item> m_symbols;  // map of the name of the symbol if any and the item itself
    std::vector<Item> m_items;                // list of items in the environment ( all the statement types in block )
    std::unordered_set<Key> types;            // type registrar

   public:
    bool in_type = false;
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
    void set(Key key, Item v) {
        m_symbols.insert(std::make_pair(key, v));
    };

    std::vector<Item>& get_items() {
        return m_items;
    }

    environment<Key, Item>* get_root() {
        auto* curr = this;
        while (curr->m_parent) {
            curr = curr->m_parent;
        }
        return curr;
    };

    void set_parent(acc::environment<Key, Item>* parent) {
        m_parent = parent;
    };

    void set_type(Key key) {
        types.insert(key);
    };

    bool has_type(Key key) {
        if (types.contains(key)) {
            return true;
        }
        if (!m_parent) return false;
        return m_parent->has_type(key);
    }

    template <typename CastType>
    CastType& get(Key key) {
        if (auto* ptr = this->resolve(key)) {
            // ensure key and casttype match
            try {
                return std::get<CastType>(ptr->m_symbols.at(key));
            } catch (std::bad_variant_access& bac) {
                std::cout << typeid(CastType{}).name();
                throw std::runtime_error("Mismatched CastType on id : " + key);
            }
        }
        throw std::runtime_error("failed to resolve key");
    }
};
}  // namespace acc