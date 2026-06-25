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

target_include_directories(musictagcli PRIVATE src)

target_compile_definitions(musictagcli PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

target_link_libraries(musictagcli PRIVATE
    ftxui::component
    TagLib::tag
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
