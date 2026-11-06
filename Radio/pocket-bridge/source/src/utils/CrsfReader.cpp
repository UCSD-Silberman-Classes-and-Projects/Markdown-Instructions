#include "CrsfReader.h"

// CRSF frame: [address][length][type][payload...][crc8]
// length counts type + payload + crc; crc8 (DVB-S2, poly 0xD5) covers type + payload.
#define CRSF_ADDRESS_FLIGHT_CONTROLLER 0xC8  // what an ELRS receiver addresses its output to
#define CRSF_TYPE_RC_CHANNELS 0x16
#define CRSF_RC_FRAME_LENGTH 24              // type + 22 bytes of 11-bit channels + crc
#define CRSF_MAX_FRAME_LENGTH 62
// Frames arrive with idle time between them; a pause this long mid-frame means we
// were not really in a frame. Well above one byte (87 us at 115200).
#define CRSF_GAP_US 2000

static uint8_t crc8_dvb_s2(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0xD5) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

bool CrsfReader::feed(uint8_t b, uint32_t now_us) {
    if (pos > 0 && (uint32_t)(now_us - last_byte_us) > CRSF_GAP_US) {
        pos = 0;
    }
    last_byte_us = now_us;

    if (pos == 0) {
        if (b == CRSF_ADDRESS_FLIGHT_CONTROLLER) {
            buf[pos++] = b;
        }
        return false;
    }
    if (pos == 1) {
        if (b < 2 || b > CRSF_MAX_FRAME_LENGTH) {
            pos = 0;
            return false;
        }
        buf[pos++] = b;
        return false;
    }

    buf[pos++] = b;
    uint8_t length = buf[1];
    if (pos < length + 2) {
        return false;
    }
    pos = 0;

    if (crc8_dvb_s2(&buf[2], length - 1) != buf[length + 1]) {
        crc_errors++;
        return false;
    }
    if (buf[2] != CRSF_TYPE_RC_CHANNELS || length != CRSF_RC_FRAME_LENGTH) {
        return false;  // a valid frame of another type, e.g. link statistics
    }

    // 16 channels x 11 bits, little-endian, packed into the 22-byte payload.
    const uint8_t *payload = &buf[3];
    uint32_t bits = 0;
    uint8_t nbits = 0;
    uint8_t byte_index = 0;
    for (uint8_t ch = 0; ch < CRSF_NUM_CHANNELS; ch++) {
        while (nbits < 11) {
            bits |= (uint32_t)payload[byte_index++] << nbits;
            nbits += 8;
        }
        channels[ch] = bits & 0x07FF;
        bits >>= 11;
        nbits -= 11;
    }
    return true;
}
