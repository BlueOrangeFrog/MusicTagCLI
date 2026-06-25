#include "App.hpp"
#include "core/DirectoryScanner.hpp"

void App::set_status(const std::string& msg, int ms) {
    status_msg    = msg;
    status_expiry = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds(ms);
}

std::string App::active_status() const {
    if (std::chrono::steady_clock::now() < status_expiry)
        return status_msg;
    return {};
}

void App::load_directory(const std::filesystem::path& dir) {
    current_dir    = dir;
    entries        = DirectoryScanner::list(dir);
    browser_cursor = 0;
    selected_indices.clear();
}

std::vector<std::filesystem::path> App::batch_paths() const {
    std::vector<std::filesystem::path> paths;
    if (!selected_indices.empty()) {
        for (int i : selected_indices) {
            if (i < static_cast<int>(entries.size()) && entries[i].is_audio)
                paths.push_back(entries[i].path);
        }
    } else if (!current_file.empty()) {
        paths.push_back(current_file);
    }
    return paths;
}
