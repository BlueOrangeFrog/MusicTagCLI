#include "MusicBrainzClient.hpp"
#include "HttpClient.hpp"
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <sstream>

using json = nlohmann::json;

// URL-encode a string (minimal — escapes spaces and common special chars)
static std::string url_encode(const std::string& s) {
    std::ostringstream enc;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            enc << c;
        } else {
            enc << '%' << std::uppercase << std::hex
                << std::setw(2) << std::setfill('0')
                << static_cast<int>(c);
        }
    }
    return enc.str();
}

void MusicBrainzClient::rate_limit() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
}

std::vector<SearchResult> MusicBrainzClient::search(
    const std::string& title,
    const std::string& artist,
    const std::string& album)
{
    std::vector<SearchResult> results;

    // Build Lucene query
    std::ostringstream q;
    if (!title.empty())  q << "recording:" << url_encode(title)  << " ";
    if (!artist.empty()) q << "artist:"    << url_encode(artist) << " ";
    if (!album.empty())  q << "release:"   << url_encode(album)  << " ";
    if (q.str().empty()) return results;

    std::string path = "/ws/2/recording/?query=" + url_encode(q.str())
                     + "&limit=10&fmt=json";

    HttpClient http(kBaseUrl, kUserAgent);
    auto resp = http.get(path);
    rate_limit();

    if (!resp || !resp->ok()) return results;

    try {
        auto j = json::parse(resp->body);
        for (const auto& rec : j.value("recordings", json::array())) {
            SearchResult sr;
            sr.mbid  = rec.value("id", "");
            sr.title = rec.value("title", "");
            sr.score = rec.value("score", 0) / 100.f;

            // First artist credit
            auto credits = rec.value("artist-credit", json::array());
            if (!credits.empty()) {
                auto artist_obj = credits[0].value("artist", json::object());
                sr.artist = artist_obj.value("name", "");
            }

            // First release
            auto releases = rec.value("releases", json::array());
            if (!releases.empty()) {
                sr.album = releases[0].value("title", "");
                auto date = releases[0].value("date", "");
                if (date.size() >= 4) sr.year = date.substr(0, 4);

                auto media = releases[0].value("media", json::array());
                if (!media.empty()) {
                    auto tracks = media[0].value("track", json::array());
                    if (!tracks.empty())
                        sr.track_num = tracks[0].value("number", "");
                }
            }

            if (!sr.mbid.empty()) results.push_back(std::move(sr));
        }
    } catch (...) {}

    return results;
}

std::optional<TagData> MusicBrainzClient::get_recording(const std::string& mbid) {
    std::string path = "/ws/2/recording/" + mbid
                     + "?inc=artist-credits+releases+genres&fmt=json";

    HttpClient http(kBaseUrl, kUserAgent);
    auto resp = http.get(path);
    rate_limit();

    if (!resp || !resp->ok()) return std::nullopt;

    try {
        auto j = json::parse(resp->body);
        TagData td;

        td.title = j.value("title", "");

        auto credits = j.value("artist-credit", json::array());
        if (!credits.empty()) {
            auto ao = credits[0].value("artist", json::object());
            td.artist = ao.value("name", "");
        }

        auto releases = j.value("releases", json::array());
        if (!releases.empty()) {
            td.album = releases[0].value("title", "");
            auto date = releases[0].value("date", "");
            if (date.size() >= 4) {
                try { td.year = static_cast<uint32_t>(std::stoul(date.substr(0, 4))); }
                catch (...) {}
            }

            auto media = releases[0].value("media", json::array());
            if (!media.empty()) {
                auto tracks = media[0].value("track", json::array());
                if (!tracks.empty()) {
                    auto num_str = tracks[0].value("number", "");
                    if (!num_str.empty()) {
                        try { td.track = static_cast<uint32_t>(std::stoul(num_str)); }
                        catch (...) {}
                    }
                }
            }
        }

        auto genres = j.value("genres", json::array());
        if (!genres.empty()) td.genre = genres[0].value("name", "");

        return td;
    } catch (...) {
        return std::nullopt;
    }
}
