#pragma once
#include <filesystem>
#include <string>

enum class AudioFormat { MP3, FLAC, OGG, M4A, OPUS, Unknown };

inline const char* format_label(AudioFormat f) {
    switch (f) {
        case AudioFormat::MP3:  return "MP3";
        case AudioFormat::FLAC: return "FLA";
        case AudioFormat::OGG:  return "OGG";
        case AudioFormat::M4A:  return "M4A";
        case AudioFormat::OPUS: return "OPS";
        default:                return "   ";
    }
}

struct FileEntry {
    std::filesystem::path path;
    std::string           display_name;
    bool                  is_dir   = false;
    bool                  is_audio = false;
    AudioFormat           format   = AudioFormat::Unknown;
};
