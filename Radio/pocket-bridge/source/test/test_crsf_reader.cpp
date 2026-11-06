// Host test for CrsfReader: c++ -std=c++11 -Isrc test/test_crsf_reader.cpp src/utils/CrsfReader.cpp
#include <cassert>
#include <cstdio>
#include <vector>
#include "utils/CrsfReader.h"

static uint8_t crc8(const std::vector<uint8_t> &d, size_t from) {
    uint8_t crc = 0;
    for (size_t i = from; i < d.size(); i++) {
        crc ^= d[i];
        for (int b = 0; b < 8; b++) crc = (crc & 0x80) ? ((crc << 1) ^ 0xD5) : (crc << 1);
    }
    return crc;
}

static std::vector<uint8_t> rc_frame(const uint16_t ch[16]) {
    std::vector<uint8_t> f = {0xC8, 24, 0x16};
    uint32_t bits = 0; int nbits = 0;
    for (int i = 0; i < 16; i++) {
        bits |= (uint32_t)(ch[i] & 0x7FF) << nbits; nbits += 11;
        while (nbits >= 8) { f.push_back(bits & 0xFF); bits >>= 8; nbits -= 8; }
    }
    f.push_back(crc8(f, 2));
    return f;
}

static std::vector<uint8_t> link_stats_frame() {
    std::vector<uint8_t> f = {0xC8, 12, 0x14, 50, 50, 100, 5, 0, 2, 1, 50, 100, 5};
    f.push_back(crc8(f, 2));
    return f;
}

struct Feeder {
    CrsfReader r; uint32_t t = 0; int frames = 0;
    void bytes(const std::vector<uint8_t> &v) { for (uint8_t b : v) { t += 87; if (r.feed(b, t)) frames++; } }
    void gap(uint32_t us) { t += us; }
};

int main() {
    uint16_t ch[16];
    for (int i = 0; i < 16; i++) ch[i] = 172 + i * 100;  // distinct values across the range
    ch[0] = 992; ch[15] = 1811;
    auto frame = rc_frame(ch);
    assert(frame.size() == 26);

    // 1. One clean frame decodes every channel exactly.
    { Feeder f; f.bytes(frame);
      assert(f.frames == 1);
      for (int i = 0; i < 16; i++) assert(f.r.channels[i] == ch[i]); }

    // 2. Joining mid-frame: the partial frame is dropped, the next one decodes.
    { Feeder f; f.bytes(std::vector<uint8_t>(frame.begin() + 7, frame.end())); f.gap(4000); f.bytes(frame);
      assert(f.frames == 1); assert(f.r.channels[5] == ch[5]); }

    // 3. A corrupted byte fails the CRC and is never reported; the next frame is fine.
    { Feeder f; auto bad = frame; bad[10] ^= 0x01; f.bytes(bad); f.gap(4000); f.bytes(frame);
      assert(f.frames == 1); assert(f.r.crc_errors == 1); assert(f.r.channels[3] == ch[3]); }

    // 4. Link-statistics frames back to back with RC frames are skipped, not misread.
    { Feeder f; for (int i = 0; i < 20; i++) { f.bytes(frame); f.bytes(link_stats_frame()); }
      assert(f.frames == 20); assert(f.r.crc_errors == 0); }

    // 5. A stray 0xC8 in line noise before a frame doesn't swallow it (the gap resets).
    { Feeder f; f.bytes({0x00, 0xC8, 0x30, 0x11}); f.gap(4000); f.bytes(frame);
      assert(f.frames == 1); }

    // 6. Back-to-back RC frames with no gap all decode.
    { Feeder f; for (int i = 0; i < 50; i++) f.bytes(frame); assert(f.frames == 50); }

    puts("crsf reader: all 6 cases pass");
    return 0;
}
