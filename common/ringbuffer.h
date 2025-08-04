#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <array>
#include <span>


constexpr int DEFAULT_RINGBUF_SIZE = 1 << 13;

template <typename T, std::size_t S>
class RingBuffer {
private:
    std::array<T, S> m_container{};
    std::size_t m_current_idx{0};
public:
    void push(const T& value) {
        if (m_current_idx >= m_container.size()) {
            m_current_idx = 0;
        }
        m_container[m_current_idx] = value;
        ++m_current_idx;
    }
    void push(T&& value) {
        if (m_current_idx >= m_container.size()) {
            m_current_idx = 0;
        }
        m_container[m_current_idx] = std::move(value);
        ++m_current_idx;
    }

    void push_range(std::span<const T> range) {
        for (auto element : range) {
            push(element);
        }
    }

    void clear() {
        m_container.fill(T{});
        m_current_idx = 0;
    }

    const std::array<T, S>& get_data() const {
        return m_container;
    }
};
#endif // RINGBUFFER_H
