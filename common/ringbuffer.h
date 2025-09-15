#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <array>
#include <span>
#include <cstring>

constexpr int DEFAULT_RINGBUF_SIZE = 1 << 13;

template <typename T, std::size_t S> requires std::is_trivial<T>::value
class RingBuffer { // Since elements can only be trivial, we can use memcpy and memset
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
        auto space_left = m_container.size() - m_current_idx;
        if (range.size() <= space_left) {
            std::memcpy(m_container.data() + m_current_idx, range.data(), range.size() * sizeof(T));
            m_current_idx += range.size();
        } else {
            auto current_range_pos = 0;
            std::memcpy(m_container.data() + m_current_idx, range.data(), space_left * sizeof(T)); // Copy as much as we can into the end of container
            current_range_pos += space_left;
            m_current_idx = 0;

            auto range_left = range.size() - current_range_pos;
            std::memcpy(m_container.data(), range.data() + current_range_pos, range_left * sizeof(T));
            m_current_idx += range_left;
        }
    }

    void clear() {
        std::memset(m_container.data(), 0, m_container.size() * sizeof(T));
        m_current_idx = 0;
    }

    const std::array<T, S>& get_data() const {
        return m_container;
    }
};
#endif // RINGBUFFER_H
