cmake_minimum_required(VERSION 3.2)
project(simple-tokenizer)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
if (MSVC)
   # https://stackoverflow.com/a/65128497/1203241
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
   set(BUILD_SHARED_LIBS TRUE)
endif()

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

add_subdirectory(examples/cpp)
enable_testing()
add_subdirectory(test)

