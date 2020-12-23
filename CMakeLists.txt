cmake_minimum_required(VERSION 3.2)
project(simple-tokenizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# for build sqlite with fts5
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(SIMPLE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src)
set(SIMPLE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

include_directories(${SIMPLE_INSTALL_INCLUDE_DIR})
include_directories(${SIMPLE_HEADERS_DIR})

# setup sqlite
option(SQLITE_ENABLE_FTS5         "enables full text searches version 5"         ON)
option(BUILD_SHELL "build SQLite3 shell application" ON)
set(SQLITE_ROOT ${PROJECT_SOURCE_DIR}/build/sqlite)
set(SQLITE_INSTALL_DIR ${SQLITE_ROOT})
set(SQLITE_INCLUDE_DIR ${SQLITE_INSTALL_DIR}/include)
set(SQLITE_LIB_DIR ${SQLITE_INSTALL_DIR}/lib)
include(ExternalProject)
ExternalProject_Add(sqlite-amalgamation
                    PREFIX ${SQLITE_ROOT}
                    INSTALL_DIR ${SQLITE_INSTALL_DIR}
                    INSTALL_COMMAND make install
                    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_ROOT}/src/sqlite-amalgamation/sqlite3ext.h ${SQLITE_INCLUDE_DIR}
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${SQLITE_INSTALL_DIR}
                                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                -DCMAKE_C_FLAGS="-lm" 
                                -DSQLITE_ENABLE_FTS5=ON
                                -DBUILD_SHELL=ON
                    GIT_REPOSITORY "git@github.com:azadkuh/sqlite-amalgamation.git")
include_directories(BEFORE SYSTEM ${SQLITE_INCLUDE_DIR})
link_directories(${SQLITE_LIB_DIR})
install(PROGRAMS ${SQLITE_INSTALL_DIR}/bin/sqlite3 DESTINATION bin)

include(contrib/CMakeRC.cmake)
cmrc_add_resource_library(PINYIN_TEXT NAMESPACE pinyin_text contrib/pinyin.txt)
# https://github.com/vector-of-bool/cmrc/issues/17#issuecomment-659501280
set_property(TARGET PINYIN_TEXT PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)
add_subdirectory(test)

