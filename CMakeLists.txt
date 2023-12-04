cmake_minimum_required(VERSION 3.13)

project(StudentScoreSystem)

set(HEADER_FILES include/sqlite3/sqlite3.h include/sqlite3pp/sqlite3pp.h)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

add_executable(main src/main.cpp ${HEADER_FILES})

target_include_directories(main PRIVATE include/sqlite3)
target_include_directories(main PRIVATE include/sqlite3pp)
target_include_directories(main PRIVATE thirdparty/SQLiteCpp/include/SQLiteCpp)
target_link_libraries(main
  SQLiteCpp
  sqlite3
  pthread
  dl
  )