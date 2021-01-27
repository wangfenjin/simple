cmake_minimum_required(VERSION 3.2)
project(simple_cpp_example)

include_directories(${SQLITE3_HEADERS_DIR})
set(SOURCE_FILES main.cc)

add_executable(simple_cpp_example ${SOURCE_FILES})
target_link_libraries(simple_cpp_example PUBLIC SQLite3)
install(TARGETS simple_cpp_example DESTINATION bin)
