cmake_minimum_required(VERSION 3.2)
project(simple CXX)

if(SIMPLE_WITH_JIEBA)
  include(ExternalProject)
  ExternalProject_Add(
      cppjieba
      PREFIX ${CMAKE_BINARY_DIR}/cppjieba
      GIT_REPOSITORY https://github.com/yanyiwu/cppjieba.git
      CONFIGURE_COMMAND ""
      BUILD_COMMAND cmake -E echo "Skipping build cppjieba."
      INSTALL_COMMAND cmake -E echo "Skipping install cppjieba."
      LOG_DOWNLOAD ON
      )
  ExternalProject_Get_Property(cppjieba source_dir)
endif()

set(SOURCE_FILES
    pinyin.h
    simple_highlight.h
    simple_tokenizer.h
    pinyin.cc
    simple_highlight.cc
    simple_tokenizer.cc
    entry.cc
)

if(SIMPLE_WITH_JIEBA)
  include_directories(${SQLITE3_HEADERS_DIR} ${source_dir}/include ${source_dir}/deps)
  INSTALL(DIRECTORY ${source_dir}/dict/ DESTINATION bin/dict FILES_MATCHING PATTERN "*.utf8")
else()
  include_directories(${SQLITE3_HEADERS_DIR})
endif()

add_library(simple SHARED ${SOURCE_FILES})

if(SIMPLE_WITH_JIEBA)
  target_include_directories(simple INTERFACE ${SQLITE3_HEADERS_DIR} ${source_dir}/include ${source_dir}/deps)
  # for tests only
  add_custom_command(TARGET simple PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                     ${source_dir}/dict/ $<TARGET_FILE_DIR:simple>/../test/dict/)
else()
  target_include_directories(simple INTERFACE ${SQLITE3_HEADERS_DIR})
endif()

target_link_libraries(simple PUBLIC coverage_config PRIVATE PINYIN_TEXT SQLite3)

install(TARGETS simple DESTINATION bin)
