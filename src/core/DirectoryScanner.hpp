#pragma once
#include <filesystem>
#include <vector>
#include "FileEntry.hpp"

class DirectoryScanner {
public:
    // Returns entries in this order: ".." first, then dirs (alpha), then audio files (alpha).
    // Returns empty vector on permission error or non-existent path.
    static std::vector<FileEntry> list(const std::filesystem::path& dir);
};
