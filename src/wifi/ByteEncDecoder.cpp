#include "wifi/ByteEncDecoder.hpp"
#include <array>
#include <cstddef>

//---------------------------------------------------------------------------
namespace wifi {
//---------------------------------------------------------------------------
// WiFi Connect (TT237W) S-boxes for the `*` channel (read from the dongle firmware).
// Counterparts of protocol-bt-cpp's numbers1/numbers2 — each table is an involution.
const std::array<uint8_t, 16> numbers1 = {1, 0, 3, 2, 15, 14, 8, 10, 6, 13, 7, 12, 11, 9, 5, 4};
const std::array<uint8_t, 16> numbers2 = {9, 12, 6, 11, 10, 15, 2, 14, 13, 0, 4, 3, 1, 8, 7, 5};

// Bytes that must be escaped on the wire (a 0x1B prefix, then the byte OR 0x80).
static bool isEscaped(uint8_t b) {
    return b == 0x0A || b == 0x0D || b == 0x2A || b == 0x2B || b == 0x26 || b == 0x1B || b == 0x00;
}

static uint8_t mod16(int i) {
    return static_cast<uint8_t>(((i % 16) + 16) % 16);
}

uint8_t shuffle(int dataNibble, int nibbleCount, int keyLeftNibble, int keyRightNibble) {
    int i5 = (nibbleCount >> 4) & 0xFF;
    uint8_t t1 = numbers1[mod16(dataNibble + nibbleCount + keyLeftNibble)];
    uint8_t t2 = numbers2[mod16(t1 + keyRightNibble + i5 - nibbleCount - keyLeftNibble)];
    uint8_t t3 = numbers1[mod16(t2 + keyLeftNibble + nibbleCount - keyRightNibble - i5)];
    // Final step (firmware common tail; the part missing from earlier write-ups) makes it an involution.
    return mod16(t3 - nibbleCount - keyLeftNibble);
}

std::vector<uint8_t> encDecBytes(const std::vector<uint8_t>& data, uint8_t key) {
    std::vector<uint8_t> result(data.size());
    int keyLeftNibble = key >> 4;
    int keyRightNibble = key & 0x0F;
    int nibbleCount = 0;
    for (std::size_t offset = 0; offset < data.size(); ++offset) {
        uint8_t d = data[offset];
        uint8_t hi = shuffle(d >> 4, nibbleCount++, keyLeftNibble, keyRightNibble);
        uint8_t lo = shuffle(d & 0x0F, nibbleCount++, keyLeftNibble, keyRightNibble);
        result[offset] = static_cast<uint8_t>((hi << 4) | lo);
    }
    return result;
}

std::vector<uint8_t> decodeStarFrame(const std::vector<uint8_t>& frame) {
    std::vector<uint8_t> out;
    if (frame.size() < 2) {
        return out;
    }
    std::size_t i = 1;  // skip the leading '*'
    uint8_t key;
    if (frame[i] == 0x1B) {
        ++i;
        key = frame[i] & 0x7F;
    } else {
        key = frame[i];
    }
    ++i;
    int keyLeftNibble = key >> 4;
    int keyRightNibble = key & 0x0F;
    int nibbleCount = 0;
    for (; i < frame.size(); ++i) {
        uint8_t b = frame[i];
        if (b == 0x0D) {  // '\r' terminates the payload
            break;
        }
        if (b == 0x1B) {  // ESC: high bit was set on the next byte
            ++i;
            b = frame[i] & 0x7F;
        }
        uint8_t hi = shuffle(b >> 4, nibbleCount++, keyLeftNibble, keyRightNibble);
        uint8_t lo = shuffle(b & 0x0F, nibbleCount++, keyLeftNibble, keyRightNibble);
        out.push_back(static_cast<uint8_t>((hi << 4) | lo));
    }
    return out;
}

std::vector<uint8_t> encodeStarFrame(const std::vector<uint8_t>& plain, uint8_t key) {
    std::vector<uint8_t> out;
    out.push_back(0x2A);  // '*'
    if (isEscaped(key)) {
        out.push_back(0x1B);
        out.push_back(key | 0x80);
    } else {
        out.push_back(key);
    }
    int keyLeftNibble = key >> 4;
    int keyRightNibble = key & 0x0F;
    int nibbleCount = 0;
    for (uint8_t d : plain) {
        uint8_t hi = shuffle(d >> 4, nibbleCount++, keyLeftNibble, keyRightNibble);
        uint8_t lo = shuffle(d & 0x0F, nibbleCount++, keyLeftNibble, keyRightNibble);
        uint8_t enc = static_cast<uint8_t>((hi << 4) | lo);
        if (isEscaped(enc)) {
            out.push_back(0x1B);
            out.push_back(enc | 0x80);
        } else {
            out.push_back(enc);
        }
    }
    out.push_back(0x0D);
    out.push_back(0x0A);
    return out;
}

//---------------------------------------------------------------------------
}  // namespace wifi
//---------------------------------------------------------------------------
