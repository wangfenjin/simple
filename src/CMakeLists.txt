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

add_library(simple SHARED ${SOURCE_FILES})
target_link_libraries(simple PRIVATE PINYIN_TEXT)
add_dependencies(simple sqlite-amalgamation)

install(TARGETS simple DESTINATION bin)
