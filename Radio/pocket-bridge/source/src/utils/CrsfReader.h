// CrsfReader.h
// Byte-at-a-time CRSF frame reader. Accepts only RC-channel frames (type 0x16)
// whose CRC8 matches, and drops back to hunting for a frame start after any bad
// byte or an idle gap, so one corrupted frame can't shift every frame after it.
#pragma once
#include <stdint.h>

#define CRSF_NUM_CHANNELS 16

class CrsfReader {
    public:
        // Feed one received byte; now_us is micros() at read time.
        // Returns true when a valid RC-channels frame has just completed.
        bool feed(uint8_t b, uint32_t now_us);

        uint16_t channels[CRSF_NUM_CHANNELS];  // raw CRSF values, 172..1811
        uint16_t crc_errors = 0;

    private:
        uint8_t buf[64];
        uint8_t pos = 0;
        uint32_t last_byte_us = 0;
};
