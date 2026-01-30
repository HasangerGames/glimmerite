#include "gmi/misc/BitStream.h"
#include <cmath>

using gmi::BitStream;

void BitStream::writeFloat(float value, float min, float max, uint8_t bitCount) {
    assert(bitCount <= 32);

    if (value < min || value > max) {
        throw std::out_of_range(
            std::format("writeFloat: value out of range: ${}, range: [{}, {}]", value, min, max)
        );
    }

    uint32_t range = (1 << bitCount) - 1;
    float t = (value - min) / (max - min);
    uint32_t valueToWrite = std::round(t * range + 0.5);
    writeBits<uint32_t>(valueToWrite, bitCount);
}

float BitStream::readFloat(float min, float max, uint8_t bitCount) {
    assert(bitCount <= 32);

    uint32_t range = (1 << bitCount) - 1;

    float read = readBits<uint32_t>(bitCount);
    float t = read / range;

    return min + t * (max - min);
}

void BitStream::writeString(const std::string_view& view, size_t maxSize) {
    bool noMaxSize = maxSize == 0;

    size_t sizeToWrite = noMaxSize
        ? view.size()
        : std::min(view.size(), maxSize);

    size_t i = 0;
    for (; i < sizeToWrite; i++) {
        writeUint8(view[i]);

        if (view[i] == 0) {
            return;
        }
    }

    // add null terminator if:
    // we have a max size and we wrote less than it
    // or if we don't have a max size
    if (i != maxSize || noMaxSize) {
        writeUint8(0);
    }
}

std::string BitStream::readString(size_t maxSize) {
    std::string str;

    size_t i = 0;
    while (true) {
        i++;

        uint8_t val = readUint8();

        // break if we found a null terminator
        if (val == 0) {
            break;
        }

        str.push_back(val);

        if (maxSize != 0 && i >= maxSize) {
            break;
        }
    }

    return str;
}
