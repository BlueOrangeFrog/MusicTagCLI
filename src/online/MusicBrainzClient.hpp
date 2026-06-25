#pragma once
#include <string>
#include <vector>
#include <optional>
#include "SearchResult.hpp"
#include "core/TagData.hpp"

class MusicBrainzClient {
public:
    // Search recordings by free-text fields. Returns up to 10 results sorted by score.
    static std::vector<SearchResult> search(
        const std::string& title,
        const std::string& artist,
        const std::string& album);

    // Fetch full release metadata for a given recording MBID.
    // Returns nullopt if the recording cannot be found.
    static std::optional<TagData> get_recording(const std::string& mbid);

private:
    // MusicBrainz requires 1 request/second from any given client.
    static void rate_limit();

    static constexpr const char* kBaseUrl = "musicbrainz.org";
    static constexpr const char* kUserAgent =
        "MusicTagCLI/0.1 ( marco.amore87@protonmail.com )";
};
