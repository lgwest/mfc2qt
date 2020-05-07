cmake_minimum_required(VERSION 3.11)

project(mfc2qt)

include(FetchContent)

FetchContent_Declare( 
  qt_solutions
  GIT_REPOSITORY https://github.com/qtproject/qt-solutions.git
)

FetchContent_MakeAvailable(qt_solutions)

set(QTWINMIGRATE ${qt_solutions_SOURCE_DIR}/qtwinmigrate)

message("QTWINMIGRATE is ${QTWINMIGRATE}")

add_subdirectory(Ring)
