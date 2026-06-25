include(FetchContent)

# ─── FTXUI ───────────────────────────────────────────────────────────────────
# Modern C++17 reactive TUI library (https://github.com/ArthurSonzogni/FTXUI)
# Check https://github.com/ArthurSonzogni/FTXUI/releases for the latest tag.
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v6.1.9
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
# FetchContent exposes the build-tree target as "tag"; create the namespaced
# alias so the rest of the project can use TagLib::TagLib consistently.
if(TARGET tag AND NOT TARGET TagLib::TagLib)
  add_library(TagLib::TagLib ALIAS tag)
endif()

# TagLib's FetchContent target does not propagate its include dirs as INTERFACE.
# Add them manually so <taglib/fileref.h> etc. can be found by consumers.
# taglib_SOURCE_DIR contains the top-level "taglib/" subdirectory; taglib_BINARY_DIR
# holds generated headers such as taglib_config.h.
# TagLib uses include_directories() internally for its subdirs — these are NOT
# propagated to consumers.  Mirror the same list so our code can use
# <taglib/fileref.h> and have the cascading relative includes resolve correctly.
set(_tl "${taglib_SOURCE_DIR}/taglib")
target_include_directories(tag INTERFACE
    "$<BUILD_INTERFACE:${taglib_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${_tl}>"
    "$<BUILD_INTERFACE:${_tl}/toolkit>"
    "$<BUILD_INTERFACE:${_tl}/asf>"
    "$<BUILD_INTERFACE:${_tl}/mpeg>"
    "$<BUILD_INTERFACE:${_tl}/mpeg/id3v1>"
    "$<BUILD_INTERFACE:${_tl}/mpeg/id3v2>"
    "$<BUILD_INTERFACE:${_tl}/mpeg/id3v2/frames>"
    "$<BUILD_INTERFACE:${_tl}/ogg>"
    "$<BUILD_INTERFACE:${_tl}/ogg/flac>"
    "$<BUILD_INTERFACE:${_tl}/ogg/vorbis>"
    "$<BUILD_INTERFACE:${_tl}/ogg/speex>"
    "$<BUILD_INTERFACE:${_tl}/ogg/opus>"
    "$<BUILD_INTERFACE:${_tl}/flac>"
    "$<BUILD_INTERFACE:${_tl}/mpc>"
    "$<BUILD_INTERFACE:${_tl}/mp4>"
    "$<BUILD_INTERFACE:${_tl}/ape>"
    "$<BUILD_INTERFACE:${_tl}/wavpack>"
    "$<BUILD_INTERFACE:${_tl}/trueaudio>"
    "$<BUILD_INTERFACE:${_tl}/riff>"
    "$<BUILD_INTERFACE:${_tl}/riff/aiff>"
    "$<BUILD_INTERFACE:${_tl}/riff/wav>"
    "$<BUILD_INTERFACE:${_tl}/mod>"
    "$<BUILD_INTERFACE:${_tl}/s3m>"
    "$<BUILD_INTERFACE:${_tl}/it>"
    "$<BUILD_INTERFACE:${_tl}/xm>"
    "$<BUILD_INTERFACE:${_tl}/dsf>"
    "$<BUILD_INTERFACE:${_tl}/dsdiff>"
    "$<BUILD_INTERFACE:${taglib_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${taglib_BINARY_DIR}/taglib>"
)

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
  GIT_TAG        v0.40.0
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
