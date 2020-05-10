project(9-Word)

include(${UTILITY_SOURCE})
assign_source_group(${UTILITY_SOURCE})

set(SOURCE_FILES
ChildFrm.cpp
ChildFrm.h
CMakeLists.txt
Line.cpp
Line.h
MainFrm.cpp
MainFrm.h
Page.cpp
Page.h
Paragraph.cpp
Paragraph.h
Position.cpp
Position.h
res/Toolbar.bmp
res/Word.ico
res/Word.rc2
res/WordDoc.ico
Resource.h
stdafx.cpp
stdafx.h
targetver.h
Word.aps
Word.cpp
Word.h
Word.rc
WordDoc.cpp
WordDoc.h
WordView.cpp
WordView.h
)

include_directories(${PROJECT_SOURCE_DIR} ${UTILITY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${UTILITY_SOURCE})
