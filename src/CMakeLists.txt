cmake_minimum_required(VERSION 3.2)
project(simple CXX)

set(SOURCE_FILES
    pinyin.h
    simple_highlight.h
    simple_tokenizer.h
    pinyin.cc
    simple_highlight.cc
    simple_tokenizer.cc
    entry.cc
)

include_directories(${SQLITE3_HEADERS_DIR})
add_library(simple SHARED ${SOURCE_FILES})
target_link_libraries(simple PUBLIC coverage_config  PRIVATE PINYIN_TEXT SQLite3)

install(TARGETS simple DESTINATION bin)
