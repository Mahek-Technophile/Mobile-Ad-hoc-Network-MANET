#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace manet {

class ErrorControl {
public:
    // Unit II: CRC-16-CCITT (Polynomial: 0x1021 -> X^16 + X^12 + X^5 + 1)
    static uint16_t computeCRC16(const std::string& data);
    static bool verifyCRC16(const std::string& data, uint16_t expectedCrc);

    // Unit II: Internet 16-bit 1's Complement Checksum
    static uint16_t computeInternetChecksum(const std::string& data);
    static bool verifyInternetChecksum(const std::string& data, uint16_t expectedChecksum);

    // Unit II: Hamming (7, 4) Code Single-Bit Error Correction Demonstration
    // Encodes a 4-bit nibble into a 7-bit Hamming code
    static uint8_t encodeHamming74(uint8_t dataNibble);
    // Decodes and corrects single-bit error if present
    static uint8_t decodeHamming74(uint8_t receivedCode, bool& hadError, bool& wasCorrected);

    // Simulated Wireless Noise: Artificially inject a single-bit flip into payload
    static std::string corruptBit(const std::string& data, size_t bitIndex = 0);
};

} // namespace manet
