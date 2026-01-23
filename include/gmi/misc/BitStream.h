#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <gmi/misc/Buffer.h>

namespace gmi {

class BitStream {
public:
    BitStream(uint8_t* data, size_t size) : m_buff(data, size) { };
    BitStream(const gmi::Uint8Buffer& buff) : m_buff(buff) { };

    [[nodiscard]] size_t size() const {
        return m_buff.size();
    }

    [[nodiscard]] size_t bitSize() const {
        return m_buff.size() * 8;
    }

    [[nodiscard]] size_t index() const {
        return std::ceil(m_bit_index / 8.0);
    }

    [[nodiscard]] size_t bitIndex() const {
        return m_bit_index;
    }

    void setBitIndex(size_t index) {
        if (index >= size()) {
            throw std::out_of_range(
                std::format(
                    "setIndex: index out of range, size: {}, index: {}",
                    size(),
                    index
                )
            );
        }
        m_bit_index = index;
    }

    template<typename T>
        requires(std::is_integral_v<T>)
    [[nodiscard]] T readBits(uint8_t bitCount);

    template<typename T>
        requires(std::is_integral_v<T>)
    void writeBits(T value, uint8_t bitCount);

#define DECLARE_READ_WRITE_FNS(SIZE)                \
    void writeUint##SIZE(uint##SIZE##_t value) {    \
        writeBits<uint##SIZE##_t>(value, SIZE);     \
    }                                               \
    void writeInt##SIZE(int##SIZE##_t value) {      \
        writeBits<int##SIZE##_t>(value, SIZE);      \
    }                                               \
    [[nodiscard]] uint##SIZE##_t readUint##SIZE() { \
        return readBits<uint##SIZE##_t>(SIZE);      \
    }                                               \
    [[nodiscard]] int##SIZE##_t readInt##SIZE() {   \
        return readBits<int##SIZE##_t>(SIZE);       \
    }

    DECLARE_READ_WRITE_FNS(8)
    DECLARE_READ_WRITE_FNS(16)
    DECLARE_READ_WRITE_FNS(32)
    DECLARE_READ_WRITE_FNS(64)
#undef DECLARE_READ_WRITE_FNS

    void writeFloat32(float value) {
        writeUint32(std::bit_cast<uint32_t>(value));
    }

    [[nodiscard]] float readFloat32() {
        return std::bit_cast<float>(readUint32());
    }

    void writeFloat64(double value) {
        writeUint64(std::bit_cast<uint64_t>(value));
    }

    [[nodiscard]] double readFloat64() {
        return std::bit_cast<double>(readUint64());
    }

    void writeFloat(float value, float min, float max, uint8_t bitCount);

    float readFloat(float min, float max, uint8_t bitCount);

    void writeBool(bool value) {
        writeBits<uint8_t>(value ? 1 : 0, 1);
    }

    [[nodiscard]] bool readBool() {
        return readBits<uint8_t>(1) != 0;
    }

    void writeString(const std::string_view& view, size_t maxSize = 0);

    [[nodiscard]] std::string readString(size_t maxSize = 0);

private:
    gmi::Uint8Buffer m_buff;

    size_t m_bit_index = 0;
};

template<typename T>
    requires(std::is_integral_v<T>)
T BitStream::readBits(uint8_t bitCount) {
    assert(bitCount <= (sizeof(T) * 8));

    size_t byteIndex = this->index();
    size_t bitIndex = this->bitIndex();
    size_t byteCount = bitCount / 8;

    size_t newIndex = (bitIndex + bitCount);
    if (newIndex > this->bitSize()) {
        throw std::out_of_range(
            std::format(
                "readBits: trying to read past the end of the buffer, buff size: {}, buff index: {}, bits to read: {}",
                this->bitSize(),
                bitIndex,
                bitCount
            )
        );
    }

    T result = 0;

    size_t offset = bitIndex;

    // Ported from bitBuffer https://github.com/inolen/bit-buffer/blob/7b6ec454866090fac7c1b2e5d67f43ffe9ad6942/bit-buffer.js
    // licensed under MIT
    for (size_t i = 0; i < bitCount;) {
        uint8_t remaining = bitCount - i;
        uint8_t bitOffset = offset & 7;

        // the max number of bits we can read from the current byte
        uint8_t read = std::min<size_t>(remaining, 8 - bitOffset);

        // create a mask with the correct bit width
        uint8_t mask = (1 << read) - 1;

        // shift the bits we want to the start of the byte and mask off the rest
        T val = (m_buff[offset / 8] >> bitOffset) & mask;

        result |= val << i;

        offset += read;
        i += read;
    }

    // if working with a signed number and the bit count is not the full type size
    // we need to add the sign bit to the first bit
    if (
        std::is_signed_v<T>
        && bitCount != (sizeof(T) * 8)
        && result & (1 << (bitCount - 1))
    ) {
        result |= -1 ^ ((1 << bitCount) - 1);
    }

    m_bit_index = newIndex;

    assert(offset == newIndex);

    return result;
}

template<typename T>
    requires(std::is_integral_v<T>)
void BitStream::writeBits(T value, uint8_t bitCount) {
    assert(bitCount <= (sizeof(T) * 8));

    size_t byteIndex = this->index();
    size_t bitIndex = this->bitIndex();
    size_t byteCount = bitCount / 8;

    size_t newIndex = (bitIndex + bitCount);
    if (newIndex > this->bitSize()) {
        throw std::out_of_range(
            std::format(
                "writeBits: trying to write past the end of the buffer, buff size: {}, buff index: {}, bits to write: {}",
                this->bitSize(),
                bitIndex,
                bitCount
            )
        );
    }

    size_t offset = bitIndex;

    // Ported from bitBuffer https://github.com/inolen/bit-buffer/blob/7b6ec454866090fac7c1b2e5d67f43ffe9ad6942/bit-buffer.js
    // licensed under MIT
    for (size_t i = 0; i < bitCount;) {
        uint8_t wrote = 0;

        // Write an entire byte if we can.
        if (bitCount - i >= 8 && (offset & 7) == 0) {
            m_buff[offset / 8] = value & 0xff;
            wrote = 8;
        } else {
            uint8_t remaining = bitCount - i;
            uint8_t bitOffset = offset & 7;
            size_t byteOffset = offset / 8;
            wrote = std::min<uint8_t>(remaining, 8 - bitOffset);

            // create a mask with the correct bit width
            uint8_t mask = ~(0xff << wrote);
            // shift the bits we want to the start of the byte and mask off the rest
            uint8_t writeBits = value & mask;

            // destination mask to zero all the bits we're changing first
            uint8_t destMask = ~(mask << bitOffset);

            m_buff[byteOffset] = (m_buff[byteOffset] & destMask) | (writeBits << bitOffset);
        }

        offset += wrote;
        i += wrote;
        value = value >> wrote;
    }

    m_bit_index = newIndex;

    assert(offset == newIndex);
}

}
