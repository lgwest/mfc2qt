project(7-Draw)

include(${UTILITY_SOURCE})
assign_source_group(${UTILITY_SOURCE})

set(SOURCE_FILES
ArrowFigure.cpp
ArrowFigure.h
ChildFrm.cpp
ChildFrm.h
Draw.cpp
Draw.h
Draw.rc
DrawDoc.cpp
DrawDoc.h
DrawView.cpp
DrawView.h
EllipseFigure.cpp
EllipseFigure.h
Figure.cpp
Figure.h
FigureFileManager.cpp
FigureFileManager.h
LineFigure.cpp
LineFigure.h
MainFrm.cpp
MainFrm.h
ReadMe.txt
RectangleFigure.cpp
RectangleFigure.h
res/Draw.ico
res/Draw.rc2
res/DrawDoc.ico
res/Toolbar.bmp
Resource.h
stdafx.cpp
stdafx.h
targetver.h
TextFigure.cpp
TextFigure.h
TwoDimensionalFigure.cpp
TwoDimensionalFigure.h
)

include_directories(${PROJECT_SOURCE_DIR} ${UTILITY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${UTILITY_SOURCE})
