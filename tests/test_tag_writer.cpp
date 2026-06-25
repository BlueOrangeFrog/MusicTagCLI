#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "core/TagReader.hpp"
#include "core/TagWriter.hpp"

namespace fs = std::filesystem;

static fs::path fixture(const std::string& name) {
    return fs::path(MUSICTAGCLI_SOURCE_DIR) / "tests" / "fixtures" / name;
}

// Copy a fixture to a temp path, write tags, re-read and verify.
static fs::path make_temp_copy(const std::string& fixture_name) {
    fs::path src = fixture(fixture_name);
    if (!fs::exists(src)) return {};

    fs::path dst = fs::temp_directory_path()
                 / ("musictagcli_test_" + fixture_name);
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
    return dst;
}

TEST_CASE("TagWriter round-trip: write and re-read MP3") {
    auto tmp = make_temp_copy("sample_tagged.mp3");
    if (tmp.empty()) SKIP("fixture not present");

    TagData td;
    td.title   = "Test Title äöü";
    td.artist  = "Test Artist";
    td.album   = "Test Album";
    td.year    = 2024;
    td.track   = 5;
    td.genre   = "Electronic";
    td.comment = "Automated test";

    bool ok = TagWriter::write(td, tmp);
    REQUIRE(ok);

    auto result = TagReader::read(tmp);
    REQUIRE(result.has_value());
    CHECK(result->title   == td.title);
    CHECK(result->artist  == td.artist);
    CHECK(result->album   == td.album);
    CHECK(result->year    == td.year);
    CHECK(result->track   == td.track);
    CHECK(result->genre   == td.genre);
    CHECK(result->comment == td.comment);

    fs::remove(tmp);
}

TEST_CASE("TagWriter round-trip: write and re-read FLAC") {
    auto tmp = make_temp_copy("sample_tagged.flac");
    if (tmp.empty()) SKIP("fixture not present");

    TagData td;
    td.title  = "FLAC Test";
    td.artist = "FLAC Artist";
    td.year   = 2023;

    REQUIRE(TagWriter::write(td, tmp));

    auto result = TagReader::read(tmp);
    REQUIRE(result.has_value());
    CHECK(result->title  == td.title);
    CHECK(result->artist == td.artist);
    CHECK(result->year   == td.year);

    fs::remove(tmp);
}

TEST_CASE("TagWriter batch: only checked fields are modified") {
    auto tmp1 = make_temp_copy("sample_tagged.mp3");
    auto tmp2 = make_temp_copy("sample_tagged.mp3");
    if (tmp1.empty() || tmp2.empty()) SKIP("fixture not present");

    // Read original artist from first copy
    auto orig = TagReader::read(tmp1);
    REQUIRE(orig.has_value());
    std::string original_artist = orig->artist;

    TagData patch;
    patch.title  = "Patched Title";
    patch.artist = "SHOULD NOT APPEAR";  // not in fields set

    BatchResult res = TagWriter::write_batch({tmp1, tmp2}, patch, {"title"});
    REQUIRE(res.ok    == 2);
    REQUIRE(res.error == 0);

    auto r1 = TagReader::read(tmp1);
    auto r2 = TagReader::read(tmp2);
    REQUIRE(r1.has_value());
    REQUIRE(r2.has_value());

    CHECK(r1->title  == "Patched Title");
    CHECK(r1->artist == original_artist);   // unchanged
    CHECK(r2->title  == "Patched Title");
    CHECK(r2->artist == original_artist);   // unchanged

    fs::remove(tmp1);
    fs::remove(tmp2);
}
