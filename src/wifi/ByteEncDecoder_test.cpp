// Self-test for wifi::ByteEncDecoder — decodes real captured *-frames to exact ASCII
// and checks the shuffle is a perfect involution.
//
//   g++ -std=c++17 -I. wifi/ByteEncDecoder.cpp wifi/ByteEncDecoder_test.cpp -o codec_test && ./codec_test
//
#include "wifi/ByteEncDecoder.hpp"
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <vector>

static std::vector<uint8_t> H(std::initializer_list<int> b) { return {b.begin(), b.end()}; }
static std::string S(const std::vector<uint8_t>& v) { return std::string(v.begin(), v.end()); }

int main() {
    int fails = 0;

    // 1) involution: decode(encode(x)) == x for all nibbles / positions / keys
    for (int k1 = 0; k1 < 16; ++k1)
        for (int k2 = 0; k2 < 16; ++k2)
            for (int pos = 0; pos < 60; ++pos)
                for (int x = 0; x < 16; ++x)
                    if (wifi::shuffle(wifi::shuffle(x, pos, k1, k2), pos, k1, k2) != x) ++fails;
    std::printf("involution: %s\n", fails == 0 ? "OK" : "FAIL");

    // 2) real captured frames (dongle replies, see protocol/wifi-local-protocol.md)
    struct Case { std::vector<uint8_t> frame; std::string want; };
    std::vector<Case> cases = {
        {H({0x2a,0x12,0xea,0x0f,0xcf,0x56,0x4b,0x44,0x0d,0x0a}), "@hp4\r\n"},
        {H({0x2a,0x69,0xad,0x84,0xec,0x8b,0xbb,0xe6,0x4c,0xdc,0x1b,0x80,0x85,0x84,0x19,0x08,
            0xa1,0x67,0xec,0xa8,0x1a,0x1f,0x0d,0x0a}), "@hy:TT237W V05.26\r\n"},
        {H({0x2a,0xeb,0xda,0x1e,0x9a,0xa0,0x59,0xdf,0xb5,0xfd,0xfd,0xa3,0x90,0x69,0xb4,0x8b,
            0x1e,0x88,0xbd,0x85,0xcf,0xff,0xc2,0xb3,0x53,0x0b,0xfc,0xc2,0x83,0xde,0x4c,0x49,
            0xa2,0x0d,0x0a}), "@hl:ESP32LDRSIM TT237W V05.26\r\n"},
    };
    for (auto& c : cases) {
        std::string got = S(wifi::decodeStarFrame(c.frame));
        bool ok = got == c.want;
        fails += !ok;
        std::printf("  %s decode -> \"%s\"\n", ok ? "OK  " : "FAIL", got.c_str());
    }

    std::printf("%s\n", fails == 0 ? "ALL PASS" : "FAILURES");
    return fails == 0 ? 0 : 1;
}
