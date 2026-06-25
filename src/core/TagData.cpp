#include "TagData.hpp"
#include <sstream>
#include <iomanip>

bool TagData::operator==(const TagData& o) const {
    return title    == o.title
        && artist   == o.artist
        && album    == o.album
        && genre    == o.genre
        && comment  == o.comment
        && year     == o.year
        && track    == o.track
        && cover_bytes == o.cover_bytes;
}

std::string TagData::audio_info() const {
    if (bitrate == 0 && sample_rate == 0) return "";

    std::ostringstream oss;
    if (bitrate > 0)     oss << bitrate << " kbps";
    if (sample_rate > 0) oss << " · " << sample_rate << " Hz";
    if (channels == 1)   oss << " · Mono";
    if (channels == 2)   oss << " · Stereo";
    if (channels > 2)    oss << " · " << channels << "ch";
    if (length_sec > 0) {
        int m = length_sec / 60;
        int s = length_sec % 60;
        oss << " · " << m << ":" << std::setfill('0') << std::setw(2) << s;
    }
    return oss.str();
}
