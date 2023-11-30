cmake_minimum_required(VERSION 3.13)

project(main)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
  SQLiteCpp
  sqlite3
  pthread
  dl
  )

