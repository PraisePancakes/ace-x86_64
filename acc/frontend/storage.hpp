#pragma once
#include <iostream>
namespace acc {

template <typename T>
class storage;

template <template <typename> class Ty,
          typename T>
class storage<Ty<T>> {
   public:
    using underlying_type = T;
    using storage_type = Ty<T>;

   protected:
    storage_type m_input;
    std::size_t m_start{0};
    std::size_t m_end{0};

   public:
    storage() = default;
    storage(storage&& o) : m_input(std::move(o.m_input)),
                           m_start(o.m_start),
                           m_end(o.m_end) {};
    storage(const storage& o) : m_input(o.m_input),
                                m_start(o.m_start),
                                m_end(o.m_end) {};

    storage& operator=(const storage& o) {
        this->m_input = o.m_input;
        this->m_start = o.m_start;
        this->m_end = o.m_end;
        return *this;
    };

    storage(const Ty<T>& in) : m_input(in) {};

    [[nodiscard]] bool is_end() const noexcept {
        return m_end >= m_input.size();
    };

    [[nodiscard]] underlying_type peek() const noexcept {
        return m_input[m_end];
    };

    [[nodiscard]] underlying_type peek_prev() const noexcept {
        return m_input[m_end - 1];
    }

    [[nodiscard]] underlying_type peek_next() const noexcept {
        return m_input[m_end + 1];
    }

    underlying_type advance() noexcept {
        if (is_end()) {
            return peek_prev();
        }
        return m_input[m_end++];
    };
    ~storage() = default;
};
}  // namespace acc