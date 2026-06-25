#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include "core/TagReader.hpp"

namespace fs = std::filesystem;

// Assumes the test is run from the build directory.
// Fixtures live in tests/fixtures/ relative to the source root.
static fs::path fixture(const std::string& name) {
    return fs::path(MUSICTAGCLI_SOURCE_DIR) / "tests" / "fixtures" / name;
}

TEST_CASE("TagReader returns nullopt for non-existent file") {
    auto result = TagReader::read("/does/not/exist.mp3");
    REQUIRE(!result.has_value());
}

TEST_CASE("TagReader reads basic fields from MP3") {
    auto f = fixture("sample_tagged.mp3");
    if (!fs::exists(f)) SKIP("fixture not present");

    auto result = TagReader::read(f);
    REQUIRE(result.has_value());
    CHECK(!result->title.empty());
    CHECK(!result->artist.empty());
}

TEST_CASE("TagReader reads audio properties") {
    auto f = fixture("sample_tagged.mp3");
    if (!fs::exists(f)) SKIP("fixture not present");

    auto result = TagReader::read(f);
    REQUIRE(result.has_value());
    CHECK(result->bitrate     > 0);
    CHECK(result->sample_rate > 0);
    CHECK(result->channels    > 0);
    CHECK(result->length_sec  > 0);
}

TEST_CASE("TagReader reads FLAC tags") {
    auto f = fixture("sample_tagged.flac");
    if (!fs::exists(f)) SKIP("fixture not present");

    auto result = TagReader::read(f);
    REQUIRE(result.has_value());
    CHECK(!result->title.empty());
}

TEST_CASE("TagReader reads OGG Vorbis tags") {
    auto f = fixture("sample_tagged.ogg");
    if (!fs::exists(f)) SKIP("fixture not present");

    auto result = TagReader::read(f);
    REQUIRE(result.has_value());
    CHECK(!result->title.empty());
}

TEST_CASE("TagReader reads Opus tags") {
    auto f = fixture("sample_tagged.opus");
    if (!fs::exists(f)) SKIP("fixture not present");

    auto result = TagReader::read(f);
    REQUIRE(result.has_value());
    CHECK(!result->title.empty());
}
