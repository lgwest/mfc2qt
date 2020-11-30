cmake_minimum_required(VERSION 3.11)

project(mfc2qt)

add_definitions(-DWINVER=0x0601)
add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-D_AFXDLL)
add_definitions(-DNO_WARN_MBCS_MFC_DEPRECATION)
set(CMAKE_MFC_FLAG 1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

function(assign_source_group)
  foreach(_source IN ITEMS ${ARGN})
	if (IS_ABSOLUTE "${_source}")
	  file(RELATIVE_PATH _source_rel "${CMAKE_SOURCE_DIR}" "${_source}")
	else()
	  set(_source_rel "${_source}")
	endif()
	get_filename_component(_source_path "${_source_rel}" PATH)
	string(REPLACE "/" "\\" _source_path_msvc ${_source_path})
	source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endfunction(assign_source_group)


set(QTWINMIGRATE_SRC 
${PROJECT_SOURCE_DIR}/qtwinmigrate/qmfcapp.cpp
${PROJECT_SOURCE_DIR}/qtwinmigrate/qmfcapp.h
${PROJECT_SOURCE_DIR}/qtwinmigrate/qwinhost.cpp
${PROJECT_SOURCE_DIR}/qtwinmigrate/qwinhost.h
${PROJECT_SOURCE_DIR}/qtwinmigrate/qwinwidget.cpp
${PROJECT_SOURCE_DIR}/qtwinmigrate/qwinwidget.h)
set(QTWINMIGRATE_DIR ${PROJECT_SOURCE_DIR}/qtwinmigrate)

set(UTILITY_DIR ${PROJECT_SOURCE_DIR}/5-Utility)
set(UTILITY_SOURCE ${UTILITY_DIR}/utility.cmake)

set(TOP_DIR ${PROJECT_SOURCE_DIR})

#add_subdirectory(1-2-ConsolProjects)
add_subdirectory(4-Ring)
add_subdirectory(6-Tetris)
add_subdirectory(7-Draw)
add_subdirectory(8-Calc)
add_subdirectory(9-Word)
add_subdirectory(Ring)
