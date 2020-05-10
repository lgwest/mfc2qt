project(8-Calc)

include(${UTILITY_SOURCE})
assign_source_group(${UTILITY_SOURCE})

set(SOURCE_FILES
Calc.cpp
Calc.h
Calc.rc
CalcDoc.cpp
CalcDoc.h
CalcView.cpp
CalcView.h
Cell.cpp
Cell.h
CellMatrix.cpp
CellMatrix.h
ChildFrm.cpp
ChildFrm.h
CMakeLists.txt
MainFrm.cpp
MainFrm.h
Parser.cpp
Parser.h
Reference.cpp
Reference.h
res/Calc.ico
res/Calc.manifest
res/Calc.rc2
res/CalcDoc.ico
res/idr_calc.ico
res/idr_main.ico
res/Thumbs.db
res/Toolbar.bmp
res/Word.ico
res/WordDoc.ico
Resource.h
Scanner.cpp
Scanner.h
stdafx.cpp
stdafx.h
SyntaxTree.cpp
SyntaxTree.h
targetver.h
Token.cpp
Token.h
TSetMatrix.cpp
TSetMatrix.h
)

include_directories(${PROJECT_SOURCE_DIR} ${UTILITY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${UTILITY_SOURCE})
