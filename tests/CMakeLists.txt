cmake_minimum_required(VERSION 3.16)

include(CTest)
include(Catch2WithMain)

# Helper: compile the core library objects once for all tests
add_library(musictagcli_core OBJECT
    ${CMAKE_SOURCE_DIR}/src/core/DirectoryScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TagData.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TagReader.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TagWriter.cpp
    ${CMAKE_SOURCE_DIR}/src/app/App.cpp
    ${CMAKE_SOURCE_DIR}/src/app/KeyBindings.cpp
)
target_include_directories(musictagcli_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(musictagcli_core PUBLIC TagLib::tag)
target_compile_features(musictagcli_core PUBLIC cxx_std_17)

function(add_test_exe name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE musictagcli_core Catch2::Catch2WithMain)
    # Expose source root so tests can find fixtures/
    target_compile_definitions(${name} PRIVATE
        MUSICTAGCLI_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    catch_discover_tests(${name})
endfunction()

add_test_exe(test_tag_reader       test_tag_reader.cpp)
add_test_exe(test_tag_writer       test_tag_writer.cpp)
add_test_exe(test_directory_scanner test_directory_scanner.cpp)
