#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "core/DirectoryScanner.hpp"

namespace fs = std::filesystem;

// Create a temporary directory tree for each test.
struct TmpDir {
    fs::path root;
    explicit TmpDir(const std::string& name) {
        root = fs::temp_directory_path() / ("musictagcli_dstest_" + name);
        fs::create_directories(root);
    }
    ~TmpDir() { fs::remove_all(root); }

    void mkdir(const std::string& rel) { fs::create_directories(root / rel); }
    void touch(const std::string& rel) { std::ofstream(root / rel); }
};

TEST_CASE("DirectoryScanner: empty directory") {
    TmpDir d("empty");
    auto entries = DirectoryScanner::list(d.root);
    // Only ".." entry expected
    REQUIRE(entries.size() == 1);
    CHECK(entries[0].display_name == "..");
    CHECK(entries[0].is_dir);
}

TEST_CASE("DirectoryScanner: dirs come before audio files") {
    TmpDir d("order");
    d.mkdir("zz_subdir");
    d.mkdir("aa_subdir");
    d.touch("zzz.mp3");
    d.touch("aaa.mp3");
    d.touch("readme.txt");   // should be excluded

    auto entries = DirectoryScanner::list(d.root);

    // Entries: "..", "aa_subdir", "zz_subdir", "aaa.mp3", "zzz.mp3"
    REQUIRE(entries.size() == 5);
    CHECK(entries[0].display_name == "..");
    CHECK(entries[1].display_name == "aa_subdir");
    CHECK(entries[2].display_name == "zz_subdir");
    CHECK(entries[3].display_name == "aaa.mp3");
    CHECK(entries[4].display_name == "zzz.mp3");
}

TEST_CASE("DirectoryScanner: audio formats are detected") {
    TmpDir d("formats");
    d.touch("track.mp3");
    d.touch("track.flac");
    d.touch("track.ogg");
    d.touch("track.m4a");
    d.touch("track.opus");
    d.touch("track.txt");    // excluded

    auto entries = DirectoryScanner::list(d.root);
    // ".." + 5 audio files
    REQUIRE(entries.size() == 6);

    bool found_mp3 = false, found_flac = false, found_ogg = false,
         found_m4a = false, found_opus = false;
    for (const auto& e : entries) {
        if (!e.is_audio) continue;
        switch (e.format) {
            case AudioFormat::MP3:  found_mp3  = true; break;
            case AudioFormat::FLAC: found_flac = true; break;
            case AudioFormat::OGG:  found_ogg  = true; break;
            case AudioFormat::M4A:  found_m4a  = true; break;
            case AudioFormat::OPUS: found_opus = true; break;
            default: break;
        }
    }
    CHECK(found_mp3);
    CHECK(found_flac);
    CHECK(found_ogg);
    CHECK(found_m4a);
    CHECK(found_opus);
}

TEST_CASE("DirectoryScanner: non-existent path returns empty list") {
    auto entries = DirectoryScanner::list("/does/not/exist/at/all");
    CHECK(entries.empty());
}
