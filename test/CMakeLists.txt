cmake_minimum_required(VERSION 3.2)
project(simple_tests)

include(ExternalProject)
ExternalProject_Add(gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG master
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)
ExternalProject_Get_Property(gtest source_dir binary_dir)

include_directories(${SIMPLE_HEADERS_DIR})

set(SOURCE_FILES main.cpp pinyin_test.cc tokenizer_test.cc)

add_executable(simple_tests ${SOURCE_FILES})
add_dependencies(simple_tests gtest)
include_directories(${source_dir}/include)
target_link_libraries(simple_tests ${binary_dir}/lib/libgtest.a ${binary_dir}/lib/libgtest_main.a simple pthread)
# install(TARGETS simple_tests DESTINATION bin)

