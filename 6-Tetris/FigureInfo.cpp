#include "StdAfx.h"
#include "Square.h"
#include "Figure.h"
#include "FigureInfo.h"

const COLORREF RED = RGB(255, 0, 0);
const COLORREF BROWN = RGB(255, 128, 0);
const COLORREF TURQUOISE = RGB(0, 255, 255);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF PURPLE = RGB(255, 0, 255);
const COLORREF YELLOW = RGB(255, 255, 0);

SquareArray RedGeneric = {Square(0, 0), Square(0, 1),
                          Square(1, 1), Square(1, 0)};
SquareInfo RedInfo = {&RedGeneric, &RedGeneric,
                      &RedGeneric, &RedGeneric};

SquareArray BrownVertical = {Square(0, 0), Square(-1, 0),
                             Square(1, 0), Square(2, 0)};
SquareArray BrownHorizontal = {Square(0, 0), Square(0, -1),
                               Square(0, 1),Square(0, 2)};
SquareInfo BrownInfo = {&BrownVertical, &BrownHorizontal,
                        &BrownVertical, &BrownHorizontal};

SquareArray TurquoiseVertical = {Square(0, 0), Square(-1, 0),
                                 Square(0, 1), Square(1, 1)};
SquareArray TurquoiseHorizontal = {Square(0, 0), Square(1, -1),
                                   Square(1, 0), Square(0, 1)};
SquareInfo TurquoiseInfo = {&TurquoiseVertical, &TurquoiseHorizontal,
                            &TurquoiseVertical,&TurquoiseHorizontal};

SquareArray GreenVertical = {Square(0, 0), Square(1, -1),
                             Square(0, -1), Square(-1, 0)};
SquareArray GreenHorizontal = {Square(0, 0), Square(0, -1),
                               Square(1, 0), Square(1, 1)};
SquareInfo GreenInfo = {&GreenVertical, &GreenHorizontal,
                        &GreenVertical, &GreenHorizontal};

SquareArray YellowNorth = {Square(0, 0), Square(0, -1),
                           Square(-1, 0), Square(0, 1)};
SquareArray YellowEast = {Square(0, 0), Square(-1, 0),
                          Square(0, 1), Square(1, 0)};
SquareArray YellowSouth = {Square(0, 0), Square(0, -1),
                           Square(1, 0), Square(0, 1)};
SquareArray YellowWest = {Square(0, 0), Square(-1, 0),
                          Square(0, -1), Square(1, 0)};
SquareInfo YellowInfo = {&YellowNorth, &YellowEast,
                         &YellowSouth, &YellowWest};

SquareArray BlueNorth = {Square(0, 0), Square(0, -2),
                         Square(0, -1),Square(-1, 0)};
SquareArray BlueEast = {Square(0, 0), Square(-2, 0),
                        Square(-1, 0), Square(0, 1)};
SquareArray BlueSouth = {Square(0, 0), Square(1, 0),
                         Square(0, 1), Square(0, 2)};
SquareArray BlueWest = {Square(0, 0), Square(0, -1),
                        Square(1, 0), Square(2, 0)};
SquareInfo BlueInfo = {&BlueNorth, &BlueEast,
                       &BlueSouth, &BlueWest};

SquareArray PurpleNorth = {Square(0, 0), Square(-1, 0),
                           Square(0, 1), Square(0, 2)};
SquareArray PurpleEast = {Square(0, 0), Square(1, 0),
                          Square(2, 0), Square(0, 1)};
SquareArray PurpleSouth = {Square(0, 0), Square(0, -2),
                           Square(0, -1), Square(1, 0)};
SquareArray PurpleWest = {Square(0, 0), Square(0, -1),
                          Square(-2, 0), Square(-1, 0)};
SquareInfo PurpleInfo = {&PurpleNorth, &PurpleEast,
                         &PurpleSouth, &PurpleWest};
