#include "DirectoryScanner.hpp"
#include "FormatDetector.hpp"
#include <algorithm>
#include <system_error>

std::vector<FileEntry> DirectoryScanner::list(const std::filesystem::path& dir) {
    std::vector<FileEntry> dirs, audios;

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        if (ec) break;

        FileEntry fe;
        fe.path         = entry.path();
        fe.display_name = entry.path().filename().string();

        if (entry.is_directory(ec)) {
            fe.is_dir = true;
            dirs.push_back(std::move(fe));
        } else if (entry.is_regular_file(ec) && is_audio_extension(entry.path())) {
            fe.is_audio = true;
            fe.format   = detect_format(entry.path());
            audios.push_back(std::move(fe));
        }
    }

    auto alpha = [](const FileEntry& a, const FileEntry& b) {
        return a.display_name < b.display_name;
    };
    std::sort(dirs.begin(),   dirs.end(),   alpha);
    std::sort(audios.begin(), audios.end(), alpha);

    std::vector<FileEntry> result;
    result.reserve(1 + dirs.size() + audios.size());

    // ".." entry for parent directory navigation
    if (dir.has_parent_path() && dir.parent_path() != dir) {
        FileEntry parent;
        parent.path         = dir.parent_path();
        parent.display_name = "..";
        parent.is_dir       = true;
        result.push_back(std::move(parent));
    }

    for (auto& d : dirs)   result.push_back(std::move(d));
    for (auto& a : audios) result.push_back(std::move(a));

    return result;
}
