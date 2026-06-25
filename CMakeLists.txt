cmake_minimum_required(VERSION 3.16)
project(MusicTagCLI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)

include(cmake/Dependencies.cmake)

set(SOURCES
    src/main.cpp
    src/app/App.cpp
    src/app/KeyBindings.cpp
    src/core/DirectoryScanner.cpp
    src/core/TagData.cpp
    src/core/TagReader.cpp
    src/core/TagWriter.cpp
    src/online/HttpClient.cpp
    src/online/MusicBrainzClient.cpp
    src/online/AcoustIdClient.cpp
    src/online/CoverArtClient.cpp
    src/ui/MainLayout.cpp
    src/ui/FileBrowser.cpp
    src/ui/TagEditor.cpp
    src/ui/StatusBar.cpp
    src/ui/widgets/TagField.cpp
    src/ui/dialogs/ConfirmDialog.cpp
    src/ui/dialogs/BatchEditDialog.cpp
    src/ui/dialogs/CoverArtDialog.cpp
    src/ui/dialogs/OnlineLookupDialog.cpp
)

add_executable(musictagcli ${SOURCES})

target_include_directories(musictagcli PRIVATE
    src
    ${taglib_SOURCE_DIR}
    ${taglib_SOURCE_DIR}/taglib
    ${taglib_SOURCE_DIR}/taglib/toolkit
    ${taglib_SOURCE_DIR}/taglib/mpeg
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v1
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2
    ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2/frames
    ${taglib_SOURCE_DIR}/taglib/ogg
    ${taglib_SOURCE_DIR}/taglib/ogg/flac
    ${taglib_SOURCE_DIR}/taglib/ogg/vorbis
    ${taglib_SOURCE_DIR}/taglib/ogg/speex
    ${taglib_SOURCE_DIR}/taglib/ogg/opus
    ${taglib_SOURCE_DIR}/taglib/flac
    ${taglib_SOURCE_DIR}/taglib/mpc
    ${taglib_SOURCE_DIR}/taglib/mp4
    ${taglib_SOURCE_DIR}/taglib/ape
    ${taglib_SOURCE_DIR}/taglib/wavpack
    ${taglib_SOURCE_DIR}/taglib/trueaudio
    ${taglib_SOURCE_DIR}/taglib/riff
    ${taglib_SOURCE_DIR}/taglib/riff/aiff
    ${taglib_SOURCE_DIR}/taglib/riff/wav
    ${taglib_SOURCE_DIR}/taglib/mod
    ${taglib_SOURCE_DIR}/taglib/s3m
    ${taglib_SOURCE_DIR}/taglib/it
    ${taglib_SOURCE_DIR}/taglib/xm
    ${taglib_SOURCE_DIR}/taglib/dsf
    ${taglib_SOURCE_DIR}/taglib/dsdiff
    ${taglib_BINARY_DIR}
    ${taglib_BINARY_DIR}/taglib
)

target_compile_definitions(musictagcli PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

target_link_libraries(musictagcli PRIVATE
    ftxui::component
    TagLib::TagLib
    nlohmann_json::nlohmann_json
    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(musictagcli PRIVATE
      -static-libgcc
      -static-libstdc++
  )
endif()

install(TARGETS musictagcli DESTINATION bin)
install(FILES musictagcli.1 DESTINATION share/man/man1 OPTIONAL)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
