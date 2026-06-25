#pragma once
#include <optional>
#include <string>
#include <vector>
#include <cstdint>

// Downloads cover art from the MusicBrainz Cover Art Archive.
// Requires a release MBID (not a recording MBID).
class CoverArtClient {
public:
    // Returns front cover JPEG bytes for the given release MBID.
    // Returns nullopt if no cover is available or on network error.
    // size: "250", "500", "1200", or "large" (original)
    static std::optional<std::vector<uint8_t>> download_front(
        const std::string& release_mbid,
        const std::string& size = "500");

private:
    static constexpr const char* kBaseUrl = "coverartarchive.org";
};
