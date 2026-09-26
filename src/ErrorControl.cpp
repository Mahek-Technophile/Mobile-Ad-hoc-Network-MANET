#include "../include/ErrorControl.hpp"
#include <iostream>

namespace manet {

// -------------------------------------------------------------
// Unit II: CRC-16-CCITT (Polynomial 0x1021)
// -------------------------------------------------------------
uint16_t ErrorControl::computeCRC16(const std::string& data) {
    uint16_t crc = 0xFFFF; // Standard CCITT initial value

    for (unsigned char byte : data) {
        crc ^= (static_cast<uint16_t>(byte) << 8);
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

bool ErrorControl::verifyCRC16(const std::string& data, uint16_t expectedCrc) {
    return computeCRC16(data) == expectedCrc;
}

// -------------------------------------------------------------
// Unit II: Internet 16-bit 1's Complement Checksum
// -------------------------------------------------------------
uint16_t ErrorControl::computeInternetChecksum(const std::string& data) {
    uint32_t sum = 0;
    size_t length = data.length();
    size_t i = 0;

    // Sum 16-bit words
    while (i + 1 < length) {
        uint16_t word = (static_cast<uint8_t>(data[i]) << 8) | static_cast<uint8_t>(data[i + 1]);
        sum += word;
        i += 2;
    }

    // Handle odd byte if any
    if (i < length) {
        sum += (static_cast<uint8_t>(data[i]) << 8);
    }

    // Fold 32-bit sum into 16 bits (add end-around carry)
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // 1's complement
    return static_cast<uint16_t>(~sum);
}

bool ErrorControl::verifyInternetChecksum(const std::string& data, uint16_t expectedChecksum) {
    return computeInternetChecksum(data) == expectedChecksum;
}

// -------------------------------------------------------------
// Unit II: Hamming (7, 4) Code Single-Bit Error Correction
// Encodes 4 data bits (d1, d2, d3, d4) into 7-bit codeword:
// Positions: 1  2  3  4  5  6  7
// Bits:      p1 p2 d1 p3 d2 d3 d4
// -------------------------------------------------------------
uint8_t ErrorControl::encodeHamming74(uint8_t nibble) {
    uint8_t d1 = (nibble >> 0) & 1;
    uint8_t d2 = (nibble >> 1) & 1;
    uint8_t d3 = (nibble >> 2) & 1;
    uint8_t d4 = (nibble >> 3) & 1;

    // Parity calculations (even parity)
    uint8_t p1 = d1 ^ d2 ^ d4;
    uint8_t p2 = d1 ^ d3 ^ d4;
    uint8_t p3 = d2 ^ d3 ^ d4;

    uint8_t code = (p1 << 0) | (p2 << 1) | (d1 << 2) | (p3 << 3) | 
                   (d2 << 4) | (d3 << 5) | (d4 << 6);
    return code;
}

uint8_t ErrorControl::decodeHamming74(uint8_t code, bool& hadError, bool& wasCorrected) {
    uint8_t b1 = (code >> 0) & 1;
    uint8_t b2 = (code >> 1) & 1;
    uint8_t b3 = (code >> 2) & 1;
    uint8_t b4 = (code >> 3) & 1;
    uint8_t b5 = (code >> 4) & 1;
    uint8_t b6 = (code >> 5) & 1;
    uint8_t b7 = (code >> 6) & 1;

    // Syndrome computation
    uint8_t s1 = b1 ^ b3 ^ b5 ^ b7;
    uint8_t s2 = b2 ^ b3 ^ b6 ^ b7;
    uint8_t s3 = b4 ^ b5 ^ b6 ^ b7;

    uint8_t syndrome = (s3 << 2) | (s2 << 1) | (s1 << 0);

    hadError = (syndrome != 0);
    wasCorrected = false;

    if (hadError && syndrome <= 7) {
        // Invert error bit at syndrome position (1-indexed)
        code ^= (1 << (syndrome - 1));
        wasCorrected = true;
    }

    // Extract original 4 data bits: d1, d2, d3, d4
    uint8_t d1 = (code >> 2) & 1;
    uint8_t d2 = (code >> 4) & 1;
    uint8_t d3 = (code >> 5) & 1;
    uint8_t d4 = (code >> 6) & 1;

    return (d4 << 3) | (d3 << 2) | (d2 << 1) | d1;
}

std::string ErrorControl::corruptBit(const std::string& data, size_t bitIndex) {
    if (data.empty()) return data;
    std::string corrupted = data;
    size_t byteIdx = (bitIndex / 8) % corrupted.size();
    uint8_t bitMask = 1 << (bitIndex % 8);
    corrupted[byteIdx] ^= bitMask;
    return corrupted;
}

} // namespace manet
