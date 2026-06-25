#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "SearchResult.hpp"

// Generates an audio fingerprint via the `fpcalc` binary (chromaprint-tools),
// then queries the AcoustID web service.  If fpcalc is not installed the
// method returns an empty vector with an error string set.
class AcoustIdClient {
public:
    // api_key: your AcoustID API key (register free at https://acoustid.org)
    explicit AcoustIdClient(std::string api_key);

    // Run fpcalc on the file, obtain the fingerprint, query AcoustID, and
    // return matching recordings.  error_out is set on failure.
    std::vector<SearchResult> lookup(
        const std::filesystem::path& path,
        std::string& error_out);

    static bool fpcalc_available();

private:
    std::string api_key_;
    static constexpr const char* kBaseUrl = "api.acoustid.org";
};
