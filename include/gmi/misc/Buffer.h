#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace gmi {

template<typename T>
class Buffer {
public:
    Buffer(T* data, size_t size) : m_data(data), m_size(size) { }

    [[nodiscard]] size_t size() const {
        return m_size;
    }

    [[nodiscard]] T* data() const {
        return m_data;
    }

    [[nodiscard]] uint8_t operator[](size_t index) const {
        assert(index < m_size);
        return m_data[index];
    }

    [[nodiscard]] uint8_t& operator[](size_t index) {
        assert(index < m_size);
        return m_data[index];
    }

private:
    T* m_data;
    size_t m_size;
};

using Uint8Buffer = Buffer<uint8_t>;
}
