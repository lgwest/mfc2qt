cmake_minimum_required(VERSION 3.11)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${QTWINMIGRATE_DIR})

add_executable(QtRing WIN32 
${TOP_DIR}/readme.md
ChildFrm.cpp
ChildFrm.h
MainFrm.cpp
MainFrm.h
ReadMe.txt
res/Ring.ico
res/Ring.rc2
res/RingDoc.ico
res/Toolbar.bmp
resource.h
Ring.cpp
Ring.h
Ring.rc
RingDoc.cpp
RingDoc.h
RingView.cpp
RingView.h
stdafx.cpp
stdafx.h
targetver.h
${QTWINMIGRATE_SRC}
)
target_link_libraries(QtRing Qt5::Core Qt5::Widgets Qt5::Gui qtwinmigrate::qtwinmigrate)
