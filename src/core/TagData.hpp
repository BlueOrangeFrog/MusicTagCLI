#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct TagData {
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string comment;
    uint32_t    year  = 0;
    uint32_t    track = 0;

    // Cover art (empty if not present)
    std::vector<uint8_t> cover_bytes;
    std::string          cover_mime;   // e.g. "image/jpeg"

    // Audio properties (read-only, not written back)
    int  bitrate     = 0;  // kbps
    int  sample_rate = 0;  // Hz
    int  channels    = 0;
    int  length_sec  = 0;

    bool operator==(const TagData& o) const;
    bool operator!=(const TagData& o) const { return !(*this == o); }

    // Returns human-readable audio info string, e.g. "320 kbps · 44100 Hz · Stereo · 3:45"
    std::string audio_info() const;
};
