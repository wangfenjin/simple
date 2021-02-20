cmake_minimum_required(VERSION 3.2)
project(simple_tests)

# https://github.com/maps-gpu/MAPS/issues/7#issuecomment-418200278
cmake_policy(SET CMP0057 NEW)

# https://github.com/google/googletest/issues/2791
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include_directories(${SIMPLE_HEADERS_DIR})
include_directories(${SQLITE3_HEADERS_DIR})
set(SOURCE_FILES main.cpp pinyin_test.cc tokenizer_test.cc)

include(GoogleTest)
add_executable(simple_tests ${SOURCE_FILES})
target_link_libraries(simple_tests PUBLIC coverage_config PRIVATE gtest_main simple)
target_compile_options(simple_tests PUBLIC -Wall -pedantic)
# install(TARGETS simple_tests DESTINATION bin)
gtest_add_tests(TARGET simple_tests ${SOURCE_FILES})
