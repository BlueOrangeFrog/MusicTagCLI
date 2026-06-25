#pragma once
#include <string>

struct SearchResult {
    std::string mbid;        // MusicBrainz recording/release ID
    std::string title;
    std::string artist;
    std::string album;
    std::string year;
    std::string track_num;
    float       score = 0.f; // 0.0 – 1.0 confidence
};
