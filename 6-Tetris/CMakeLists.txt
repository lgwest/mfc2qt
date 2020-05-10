project(6-Tetris)

include(${UTILITY_SOURCE})
assign_source_group(${UTILITY_SOURCE})

set(SOURCE_FILES
ColorGrid.cpp
ColorGrid.h
Figure.cpp
Figure.h
FigureInfo.cpp
FigureInfo.h
MainFrm.cpp
MainFrm.h
ReadMe.txt
res/Tetris.ico
res/Tetris.rc2
res/TetrisDoc.ico
res/Toolbar.bmp
Resource.h
Square.cpp
Square.h
stdafx.cpp
stdafx.h
targetver.h
Tetris.cpp
Tetris.h
Tetris.rc
TetrisDoc.cpp
TetrisDoc.h
TetrisView.cpp
TetrisView.h
)

include_directories(${PROJECT_SOURCE_DIR} ${UTILITY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${UTILITY_SOURCE})
