cmake_minimum_required (VERSION 3.16)

set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_STANDARD 17)

project (FiveCrowns CXX)
set (Target_lib fivecrowns)
set (Target_client fivecrowns-client)
set (Target_tests fivecrowns-tests)

include (FetchContent)
FetchContent_Declare (googletest URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip)
FetchContent_MakeAvailable (googletest)
enable_testing ()

include_directories (src)
include_directories (${googletest_SOURCE_DIR}/googletest/include)

file (GLOB LIB_SOURCES src/*.cc)
add_library (${Target_lib} ${LIB_SOURCES})

file (GLOB CLIENT_SOURCES client/*.cc)
add_executable (${Target_client} ${CLIENT_SOURCES})
target_link_libraries (${Target_client} ${Target_lib})

file (GLOB TESTS_SOURCES tests/*.cc)
add_executable (${Target_tests} ${TESTS_SOURCES})
target_link_libraries (${Target_tests} ${Target_lib} gtest_main)

include (GoogleTest)
gtest_discover_tests (${Target_tests})
