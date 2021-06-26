cmake_minimum_required (VERSION 3.16)

set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_STANDARD 17)

project (FiveCrowns CXX)
set (Target_main fivecrowns)

file (GLOB SOURCES src/*.cc)
add_executable (${Target_main} ${SOURCES})
