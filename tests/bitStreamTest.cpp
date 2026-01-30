#include "gmi/misc/BitStream.h"
#include "gmi/math/math.h"
#include <numbers>

int main() {
    auto* data = new uint8_t[100];

    gmi::BitStream s{data, 100};

    s.writeUint8(10);
    s.writeUint8(15);
    s.writeInt8(-15);
    s.writeInt16(-100);
    s.writeInt32(-99999);

    s.writeBits<uint8_t>(1, 1);
    s.writeBits<uint8_t>(7, 4);
    s.writeBits<uint8_t>(3, 2);
    s.writeBits<int16_t>(-405, 13);

    s.writeUint64((uint64_t)1 << 50);
    s.writeInt64(-99999999999999);
    s.writeFloat64(std::numbers::pi);

    s.writeUint16(3000);
    s.writeUint32(99999);

    s.writeBits<uint32_t>(0xff00ff, 24);

    s.writeString("Deers are cool and gay 🦌");
    s.writeString("meow meow meow", 9);

    s.writeFloat(-10.5, -20, 20, 16);

    s.setBitIndex(0);

    assert(s.readUint8() == 10);
    assert(s.readUint8() == 15);
    assert(s.readInt8() == -15);
    assert(s.readInt16() == -100);
    assert(s.readInt32() == -99999);

    assert(s.readBits<uint8_t>(1) == 1);
    assert(s.readBits<uint8_t>(4) == 7);
    assert(s.readBits<uint8_t>(2) == 3);
    assert(s.readBits<int16_t>(13) == -405);

    assert(s.readUint64() == ((uint64_t)1 << 50));
    assert(s.readInt64() == -99999999999999);
    assert(s.readFloat64() == std::numbers::pi);

    assert(s.readUint16() == 3000);
    assert(s.readUint32() == 99999);
    assert(s.readBits<uint32_t>(24) == 0xff00ff);

    assert(s.readString() == "Deers are cool and gay 🦌");
    assert(s.readString(9) == "meow meow");

    assert(gmi::math::nearlyEqual(s.readFloat(-20, 20, 16), -10.5));

    // string null terminator tests
    s.setBitIndex(0);
    s.writeString("test");
    assert(s.index() == 5);
    s.setBitIndex(0);
    assert(s.readString(5) == "test");

    s.setBitIndex(0);
    s.writeString("test", 4);
    assert(s.index() == 4);
    s.setBitIndex(0);
    assert(s.readString(4) == "test");

    s.setBitIndex(0);
    s.writeString("bleh", 4);
    assert(s.index() == 4);
    s.setBitIndex(0);
    assert(s.readString(4) == "bleh");
}
