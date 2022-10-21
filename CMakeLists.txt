cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
if (APPLE)
   set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version")
endif()

if (IOS)
  list(APPEND CMAKE_ARGS
    "-DPLATFORM=${PLATFORM}"
  )
endif()

# put project after CMAKE_OSX_DEPLOYMENT_TARGET
project(simple-tokenizer)

if (MSVC)
   # https://stackoverflow.com/a/65128497/1203241
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
   set(BUILD_SHARED_LIBS TRUE)
   add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(SQLITE3_HEADERS_DIR ${PROJECT_SOURCE_DIR}/contrib/sqlite3)
set(SIMPLE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src)

# setup sqlite
option(BUILD_SQLITE3 "Enable build SQLite3" ON)
if(BUILD_SQLITE3)
  set(SQLITE_ROOT ${PROJECT_SOURCE_DIR}/build/sqlite)
  set(SQLITE_INSTALL_DIR ${SQLITE_ROOT})
  set(SQLITE_INCLUDE_DIR ${SQLITE_INSTALL_DIR}/include)
  set(SQLITE_LIB_DIR ${SQLITE_INSTALL_DIR}/lib)
  if (NOT IOS)
    option(SQLITE_ENABLE_FTS5         "enables full text searches version 5"         ON)
    option(BUILD_SHELL "build SQLite3 shell application" ON)
    add_subdirectory(contrib/sqlite3)
  endif()
endif()

include(contrib/CMakeRC.cmake)
cmrc_add_resource_library(PINYIN_TEXT TYPE OBJECT NAMESPACE pinyin_text contrib/pinyin.txt)
# https://github.com/vector-of-bool/cmrc/issues/17#issuecomment-659501280
set_property(TARGET PINYIN_TEXT PROPERTY POSITION_INDEPENDENT_CODE ON)

# Code Coverage Configuration
if(NOT TARGET coverage_config)
  add_library(coverage_config INTERFACE)
endif()
option(CODE_COVERAGE "Enable coverage reporting" OFF)
# if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
if(CODE_COVERAGE)
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif(CODE_COVERAGE)
# endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

# https://stackoverflow.com/a/15212881/1203241
OPTION(SIMPLE_WITH_JIEBA "Option to build with cppjieba" ON)
if(SIMPLE_WITH_JIEBA)
  add_definitions(-DUSE_JIEBA=1)
endif()

add_subdirectory(src)

OPTION(BUILD_TEST_EXAMPLE "Option to build tests and examples" ON)
if (NOT IOS AND BUILD_TEST_EXAMPLE)
  add_subdirectory(examples/cpp)
  enable_testing()
  add_subdirectory(test)
endif()

