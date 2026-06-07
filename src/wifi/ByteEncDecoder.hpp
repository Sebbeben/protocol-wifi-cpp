#pragma once
//---------------------------------------------------------------------------
// JURA WiFi Connect (TT237W "WiFiFrog") `*`-channel obfuscation codec.
//
// This is the SAME keyed nibble shuffle JURA uses on Bluetooth — see
// Jutta-Proto/protocol-bt-cpp `src/bt/ByteEncDecoder.cpp` (bt::shuffle / bt::encDecBytes).
// Only the 16-entry S-box tables differ between products. The shuffle is an involution
// (the same routine both encodes and decodes). See protocol/wifi-local-protocol.md.
//---------------------------------------------------------------------------
#include <cstdint>
#include <vector>

namespace wifi {
//---------------------------------------------------------------------------

/// Keyed, position-dependent nibble involution. `nibbleCount` is the running nibble
/// index across the message (0,1,2,…); `keyLeftNibble`/`keyRightNibble` are the high/low
/// nibbles of the per-message key byte. Mirrors bt::shuffle with the WiFi S-boxes.
uint8_t shuffle(int dataNibble, int nibbleCount, int keyLeftNibble, int keyRightNibble);

/// Encode OR decode a flat byte buffer (same routine both ways), as used inside a frame.
/// `key` is the per-message key byte (its nibbles drive the shuffle).
std::vector<uint8_t> encDecBytes(const std::vector<uint8_t>& data, uint8_t key);

/// Decode a full on-the-wire `*`-frame ("*" + key + obfuscated payload + "\r\n",
/// with 0x1B high-bit escaping) into the plaintext payload.
std::vector<uint8_t> decodeStarFrame(const std::vector<uint8_t>& frame);

/// Build a `*`-frame for `plain` using key byte `key` (the real firmware randomizes it).
std::vector<uint8_t> encodeStarFrame(const std::vector<uint8_t>& plain, uint8_t key);

//---------------------------------------------------------------------------
}  // namespace wifi
//---------------------------------------------------------------------------
