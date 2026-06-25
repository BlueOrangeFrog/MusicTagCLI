#pragma once
#include <filesystem>
#include <optional>
#include "TagData.hpp"

class TagReader {
public:
    // Returns nullopt if the file cannot be opened, is not a recognised audio
    // format, or TagLib returns isNull().
    static std::optional<TagData> read(const std::filesystem::path& path);
};
