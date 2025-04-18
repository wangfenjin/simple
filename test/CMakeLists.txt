cmake_minimum_required(VERSION 3.19)
project(simple_tests)

# https://github.com/maps-gpu/MAPS/issues/7#issuecomment-418200278
cmake_policy(SET CMP0057 NEW)

# https://github.com/google/googletest/issues/2791
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)

# Download and unpack googletest at configure time
set(GTEST_CMAKE_ARGS
    "-DCMAKE_POLICY_DEFAULT_CMP0048=NEW"
)
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${GTEST_CMAKE_ARGS} .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

# Set policy version minimum before adding googletest
set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "Minimum CMake policy version" FORCE)

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

include_directories(${SIMPLE_HEADERS_DIR})
include_directories(${SQLITE3_HEADERS_DIR})
set(SOURCE_FILES main.cpp pinyin_test.cc tokenizer_test.cc)

include(GoogleTest)
add_executable(simple_tests ${SOURCE_FILES})
target_link_libraries(simple_tests PUBLIC coverage_config PRIVATE gtest_main simple)
target_compile_options(simple_tests PUBLIC -Wall -pedantic)
# install(TARGETS simple_tests DESTINATION bin)
gtest_add_tests(TARGET simple_tests ${SOURCE_FILES})
