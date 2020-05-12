project(4-Ring)

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
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets)