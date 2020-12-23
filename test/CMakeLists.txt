cmake_minimum_required(VERSION 3.2)
project(simple_tests)

enable_testing()
find_package(GTest REQUIRED)

include_directories(${SIMPLE_HEADERS_DIR})
set(SOURCE_FILES main.cpp pinyin_test.cc tokenizer_test.cc)

add_executable(simple_tests ${SOURCE_FILES})
target_link_libraries(simple_tests GTest::GTest GTest::Main simple pthread)
install(TARGETS simple_tests DESTINATION bin)
