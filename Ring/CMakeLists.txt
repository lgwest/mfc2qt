cmake_minimum_required(VERSION 3.11)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 1)

set(SOURCE_FILES
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
)
add_executable(Ring WIN32 ${SOURCE_FILES})
