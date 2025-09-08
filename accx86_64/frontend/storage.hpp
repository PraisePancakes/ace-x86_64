#pragma once
#include <iostream>
namespace acc {

template <typename T>
class fsm_storage {
   public:
    using value_type = T::value_type;
    using size_type = T::size_type;
    using difference_type = T::difference_type;
    using reference = T::reference;
    using const_reference = T::const_reference;
    using pointer = T::pointer;
    using const_pointer = T::const_pointer;
    using iterator = T::iterator;
    using const_iterator = T::const_iterator;
    using reverse_iterator = T::reverse_iterator;
    using const_reverse_iterator = T::const_reverse_iterator;

    using storage_type = T;

   protected:
    storage_type m_input;
    size_type m_start{0};
    size_type m_end{0};

   public:
    fsm_storage() = default;
    fsm_storage(fsm_storage&& o) : m_input(std::move(o.m_input)),
                                   m_start(o.m_start),
                                   m_end(o.m_end) {};
    fsm_storage(const fsm_storage& o) : m_input(o.m_input),
                                        m_start(o.m_start),
                                        m_end(o.m_end) {};

    fsm_storage& operator=(const fsm_storage& o) {
        this->m_input = o.m_input;
        this->m_start = o.m_start;
        this->m_end = o.m_end;
        return *this;
    };

    fsm_storage(const storage_type& in) : m_input(in) {};

    [[nodiscard]] bool is_end() const noexcept {
        return m_end >= m_input.size();
    };

    [[nodiscard]] value_type peek() const noexcept {
        return m_input[m_end];
    };

    [[nodiscard]] value_type peek_prev() const noexcept {
        return m_input[m_end - 1];
    }

    [[nodiscard]] value_type peek_next() const noexcept {
        return m_input[m_end + 1];
    }

    value_type advance() noexcept {
        if (is_end()) {
            return peek_prev();
        }
        return m_input[m_end++];
    };
    ~fsm_storage() = default;
};
}  // namespace acc