cmake_minimum_required(VERSION 3.5)

project(nlib LANGUAGES CXX)

add_subdirectory(lib/oead)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
include_directories(lib/oead/src/include)

add_library(nlib
    include/nlib/types.h
    include/nlib/util.h
    src/util.cpp

    src/binaryreader.cpp
    include/nlib/binaryreader.h
    src/binarywriter.cpp
    include/nlib/binarywriter.h

    src/narc/narc_detail.cpp
    src/narc/narc.cpp
    include/nlib/file/narc.h
)
target_link_libraries(nlib PUBLIC oead)

target_compile_definitions(nlib PRIVATE DEBUG)
