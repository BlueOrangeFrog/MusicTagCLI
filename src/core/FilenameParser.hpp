#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include "TagData.hpp"

namespace FilenameParser {

// Split s by sep; returns parts (never empty — at least one element).
inline std::vector<std::string> split(const std::string& s, const std::string& sep) {
    std::vector<std::string> parts;
    std::string::size_type pos = 0, hit;
    while ((hit = s.find(sep, pos)) != std::string::npos) {
        parts.push_back(s.substr(pos, hit - pos));
        pos = hit + sep.size();
    }
    parts.push_back(s.substr(pos));
    return parts;
}

// Normalize en-dash " – " (UTF-8: \xe2\x80\x93) → " - " so one split covers both.
inline std::string normalize_dashes(std::string s) {
    const std::string endash = " \xe2\x80\x93 ";
    const std::string hyphen = " - ";
    std::string::size_type pos = 0;
    while ((pos = s.find(endash, pos)) != std::string::npos) {
        s.replace(pos, endash.size(), hyphen);
        pos += hyphen.size();
    }
    return s;
}

inline bool is_year(const std::string& s) {
    if (s.size() != 4) return false;
    return std::all_of(s.begin(), s.end(), [](char c){ return c >= '0' && c <= '9'; });
}

inline bool is_track(const std::string& s) {
    if (s.empty() || s.size() > 3) return false;
    return std::all_of(s.begin(), s.end(), [](char c){ return c >= '0' && c <= '9'; });
}

// Parse "Artist - Album", "Artist - Year - Album", "Year - Album", etc.
inline void parse_directory_name(const std::string& raw, TagData& out) {
    auto norm  = normalize_dashes(raw);
    auto parts = split(norm, " - ");

    // Strip trailing parenthetical like " (24bit-96kHz)"
    for (auto& p : parts) {
        auto paren = p.find(" (");
        if (paren != std::string::npos) p = p.substr(0, paren);
    }

    if (parts.size() == 1) {
        if (out.album.empty()) out.album = parts[0];
    } else if (parts.size() == 2) {
        if (is_year(parts[0])) {
            if (out.year == 0)   try { out.year = static_cast<uint32_t>(std::stoul(parts[0])); } catch(...) {}
            if (out.album.empty()) out.album = parts[1];
        } else {
            if (out.artist.empty()) out.artist = parts[0];
            if (out.album.empty())  out.album  = parts[1];
        }
    } else {
        // ≥3 parts: look for a year in the middle
        int year_idx = -1;
        for (int i = 1; i < static_cast<int>(parts.size()) - 1; ++i) {
            if (is_year(parts[i])) { year_idx = i; break; }
        }
        if (year_idx > 0) {
            if (out.artist.empty()) out.artist = parts[0];
            if (out.year == 0)      try { out.year = static_cast<uint32_t>(std::stoul(parts[year_idx])); } catch(...) {}
            if (out.album.empty())  out.album = parts.back();
        } else {
            if (out.artist.empty()) out.artist = parts[0];
            if (out.album.empty())  out.album  = parts.back();
        }
    }
}

// Parse "01 Title", "01 - Title", "01. Title", "Artist - Title", etc.
inline void parse_filename_stem(const std::string& raw, TagData& out) {
    auto norm = normalize_dashes(raw);

    // 1. Try to strip leading track number: up to 3 digits followed by [. - ] or space
    std::string rest = norm;
    {
        std::string::size_type i = 0;
        while (i < rest.size() && rest[i] >= '0' && rest[i] <= '9') ++i;
        if (i > 0 && i <= 3 && i < rest.size()) {
            char sep = rest[i];
            if (sep == '.' || sep == ' ' || sep == '-') {
                std::string track_s = rest.substr(0, i);
                if (out.track == 0)
                    try { out.track = static_cast<uint32_t>(std::stoul(track_s)); } catch(...) {}
                // skip separator chars
                while (i < rest.size() && (rest[i] == '.' || rest[i] == ' ' || rest[i] == '-'))
                    ++i;
                rest = rest.substr(i);
            }
        }
    }

    // 2. If remainder contains " - ", split as "Artist - Title"
    auto dash = rest.find(" - ");
    if (dash != std::string::npos) {
        std::string left  = rest.substr(0, dash);
        std::string right = rest.substr(dash + 3);
        // Only treat as artist - title if left is not a year
        if (!is_year(left)) {
            if (out.artist.empty()) out.artist = left;
            if (out.title.empty())  out.title  = right;
        } else {
            if (out.title.empty()) out.title = rest;
        }
    } else {
        if (out.title.empty()) out.title = rest;
    }
}

// Fill empty fields in `out` from path (filename stem + parent directory name).
// Never overwrites non-empty fields.
inline void parse_path_metadata(const std::filesystem::path& path, TagData& out) {
    parse_filename_stem(path.stem().string(), out);
    if (path.has_parent_path())
        parse_directory_name(path.parent_path().filename().string(), out);
}

} // namespace FilenameParser
