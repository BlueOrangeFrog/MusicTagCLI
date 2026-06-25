#pragma once
#include <filesystem>
#include <set>
#include <string>
#include <vector>
#include "TagData.hpp"

struct BatchResult {
    int ok    = 0;
    int error = 0;
};

class TagWriter {
public:
    // Write all tag fields (and cover art) to the file. Returns true on success.
    static bool write(const TagData& td, const std::filesystem::path& path);

    // Write only the fields listed in `fields` from `td` to each file.
    // Valid field names: "title", "artist", "album", "genre", "comment",
    //                    "year", "track", "cover"
    static BatchResult write_batch(
        const std::vector<std::filesystem::path>& paths,
        const TagData& td,
        const std::set<std::string>& fields);
};
