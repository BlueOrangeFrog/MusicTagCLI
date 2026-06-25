#include "AcoustIdClient.hpp"
#include "HttpClient.hpp"
#include "MusicBrainzClient.hpp"
#include <nlohmann/json.hpp>
#include <cstdio>
#include <sstream>
#include <array>

using json = nlohmann::json;

AcoustIdClient::AcoustIdClient(std::string api_key)
    : api_key_(std::move(api_key)) {}

bool AcoustIdClient::fpcalc_available() {
    // Probe by running fpcalc --version and checking the return code.
    return std::system("fpcalc --version > /dev/null 2>&1") == 0;
}

// Run fpcalc and capture its JSON output.
static std::string run_fpcalc(const std::filesystem::path& path) {
    std::string cmd = "fpcalc -json \"" + path.string() + "\" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return {};

    std::string output;
    std::array<char, 256> buf;
    while (fgets(buf.data(), buf.size(), pipe)) output += buf.data();
    pclose(pipe);
    return output;
}

std::vector<SearchResult> AcoustIdClient::lookup(
    const std::filesystem::path& path,
    std::string& error_out)
{
    std::vector<SearchResult> results;

    if (!fpcalc_available()) {
        error_out = "fpcalc not found. Install: sudo apt install chromaprint-tools";
        return results;
    }

    std::string fpcalc_out = run_fpcalc(path);
    if (fpcalc_out.empty()) {
        error_out = "fpcalc failed to process the file.";
        return results;
    }

    std::string fingerprint;
    int duration = 0;
    try {
        auto j    = json::parse(fpcalc_out);
        fingerprint = j.value("fingerprint", "");
        duration    = j.value("duration", 0);
    } catch (...) {
        error_out = "Could not parse fpcalc output.";
        return results;
    }

    if (fingerprint.empty() || duration <= 0) {
        error_out = "Invalid fingerprint data from fpcalc.";
        return results;
    }

    // Build AcoustID lookup URL
    std::ostringstream path_ss;
    path_ss << "/v2/lookup?client=" << api_key_
            << "&meta=recordings+releasegroups+compress"
            << "&duration=" << duration
            << "&fingerprint=" << fingerprint;

    HttpClient http(kBaseUrl, "MusicTagCLI/0.1");
    auto resp = http.get(path_ss.str());
    if (!resp || !resp->ok()) {
        error_out = "AcoustID request failed (status "
                  + (resp ? std::to_string(resp->status) : "0") + ").";
        return results;
    }

    try {
        auto j = json::parse(resp->body);
        if (j.value("status", "") != "ok") {
            error_out = "AcoustID error: " + j.value("error", json::object()).value("message", "unknown");
            return results;
        }

        for (const auto& result : j.value("results", json::array())) {
            float score = result.value("score", 0.f);
            for (const auto& rec : result.value("recordings", json::array())) {
                SearchResult sr;
                sr.mbid  = rec.value("id", "");
                sr.title = rec.value("title", "");
                sr.score = score;

                auto credits = rec.value("artists", json::array());
                if (!credits.empty())
                    sr.artist = credits[0].value("name", "");

                auto rgs = rec.value("releasegroups", json::array());
                if (!rgs.empty())
                    sr.album = rgs[0].value("title", "");

                if (!sr.mbid.empty()) results.push_back(std::move(sr));
            }
        }
    } catch (...) {
        error_out = "Failed to parse AcoustID response.";
    }

    return results;
}
