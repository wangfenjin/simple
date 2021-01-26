cmake_minimum_required(VERSION 3.2)
project(simple-tokenizer)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)
# if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#   # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# elseif (MSVC)
#   set(CMAKE_GENERATOR "Visual Studio 15")
#   set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "default install path" FORCE )
#   # https://stackoverflow.com/a/65128497/1203241
#   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
#   set(BUILD_SHARED_LIBS TRUE)
# endif()

set(SQLITE3_HEADERS_DIR ${PROJECT_SOURCE_DIR}/contrib/sqlite3)
set(SIMPLE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src)

# setup sqlite
option(SQLITE_ENABLE_FTS5         "enables full text searches version 5"         ON)
option(BUILD_SHELL "build SQLite3 shell application" ON)
set(SQLITE_ROOT ${PROJECT_SOURCE_DIR}/build/sqlite)
set(SQLITE_INSTALL_DIR ${SQLITE_ROOT})
set(SQLITE_INCLUDE_DIR ${SQLITE_INSTALL_DIR}/include)
set(SQLITE_LIB_DIR ${SQLITE_INSTALL_DIR}/lib)
add_subdirectory(contrib/sqlite3)

include(contrib/CMakeRC.cmake)
cmrc_add_resource_library(PINYIN_TEXT NAMESPACE pinyin_text contrib/pinyin.txt)
# https://github.com/vector-of-bool/cmrc/issues/17#issuecomment-659501280
set_property(TARGET PINYIN_TEXT PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)
enable_testing()
add_subdirectory(test)

