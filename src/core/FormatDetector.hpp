#pragma once
#include <filesystem>
#include <string>
#include "FileEntry.hpp"

inline AudioFormat detect_format(const std::filesystem::path& p) {
    std::string ext = p.extension().string();
    for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if (ext == ".mp3")                        return AudioFormat::MP3;
    if (ext == ".flac")                       return AudioFormat::FLAC;
    if (ext == ".ogg" || ext == ".oga")       return AudioFormat::OGG;
    if (ext == ".m4a" || ext == ".aac"
        || ext == ".m4b" || ext == ".mp4")    return AudioFormat::M4A;
    if (ext == ".opus")                       return AudioFormat::OPUS;
    return AudioFormat::Unknown;
}

inline bool is_audio_extension(const std::filesystem::path& p) {
    return detect_format(p) != AudioFormat::Unknown;
}
