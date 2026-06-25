include(FetchContent)

# ─── FTXUI ───────────────────────────────────────────────────────────────────
# Modern C++17 reactive TUI library (https://github.com/ArthurSonzogni/FTXUI)
# Check https://github.com/ArthurSonzogni/FTXUI/releases for the latest tag.
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v0.4.1
  GIT_SHALLOW    TRUE
)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ftxui)

# ─── TagLib ──────────────────────────────────────────────────────────────────
# Audio metadata library — gold standard for ID3, Vorbis, FLAC, MP4, etc.
FetchContent_Declare(taglib
  GIT_REPOSITORY https://github.com/taglib/taglib.git
  GIT_TAG        v2.0.2
  GIT_SHALLOW    TRUE
)
set(BUILD_SHARED_LIBS        OFF CACHE BOOL "" FORCE)
set(TAGLIB_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(TAGLIB_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(taglib)

# ─── nlohmann/json ───────────────────────────────────────────────────────────
# Header-only JSON library for parsing MusicBrainz / AcoustID responses.
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
  GIT_SHALLOW    TRUE
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# ─── cpp-httplib ─────────────────────────────────────────────────────────────
# Header-only HTTP/HTTPS client library.  OpenSSL is enabled project-wide via
# the CPPHTTPLIB_OPENSSL_SUPPORT compile definition in CMakeLists.txt.
FetchContent_Declare(httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        v0.15.3
  GIT_SHALLOW    TRUE
)
set(HTTPLIB_REQUIRE_OPENSSL ON  CACHE BOOL "" FORCE)
set(HTTPLIB_USE_BROTLI      OFF CACHE BOOL "" FORCE)
set(HTTPLIB_COMPILE         OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(httplib)

# ─── OpenSSL ─────────────────────────────────────────────────────────────────
# Required by cpp-httplib for HTTPS.  Install with:
#   sudo apt install libssl-dev
find_package(OpenSSL REQUIRED)

# ─── Catch2 (tests only) ─────────────────────────────────────────────────────
if(BUILD_TESTS)
  FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()
