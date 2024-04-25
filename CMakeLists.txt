cmake_minimum_required(VERSION 3.27)

project(bats_documentation)

# Settings and Checking build binay in the root of source dir IS NOT ALLOWED
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(
    FATAL_ERROR
      "FATAL: In-source builds are not allowed.
           You should create a separate directory for build files.
           ")
endif()

# set the default path for built executables to the "build" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generating a compilation database: compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c++17 checking
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)

if(COMPILER_SUPPORTS_CXX17)
  message(STATUS "c++17 supported")
else()
  message(FATAL_ERROR "
            FATAL: gcc or g++ do not support c++17")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# change it to Debug by `-DCMAKE_BUILD_TYPE=Debug` Acceptable build types:
# Debug, Release, RelWithDebInfo, MinSizeRel
option(CMAKE_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "CMAKE Version: ${CMAKE_VERSION}")
message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Host System name: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

# Set build specific compiler flags
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -O0 -g"
)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -O0")

# DISABLE TEST BUILD FOR GFLAGS
set(BUILD_TESTING OFF)
if(NOT BUILD_TESTING)
  message(STATUS "Unit test build is disabled for thirdparties.")
endif()

include(FetchContent)
# for duplicated build of gtest (google_highway will build gtest too)
cmake_policy(SET CMP0079 NEW)

FetchContent_Declare(
  gflags
  GIT_REPOSITORY https://github.com/gflags/gflags.git
  GIT_TAG v2.2.2)
# uncomment this line if you want to use the source code of gflags without
# downloading it. set(FETCHCONTENT_SOURCE_DIR_GFLAGS
# ${CMAKE_BINARY_DIR}/_deps/gflags-src)

FetchContent_MakeAvailable(gflags)
message("gflags source directory is :" ${gflags_BINARY_DIR})
include_directories(${gflags_BINARY_DIR}/include/)

# strip all symbols
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-s -z now -pie")
endif()

# more compile info dump if set it to true
set(CMAKE_VERBOSE_MAKEFILE false)

include_directories(src/)
add_subdirectory(src)
