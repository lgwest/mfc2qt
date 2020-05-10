#include "StdAfx.h"

#include "Square.h"
#include "Figure.h"
#include "ColorGrid.h"
#include "TetrisDoc.h"
#include "TetrisView.h"

// These global variables are set by the view class OnSize method and used by
// Draw to draw the figures relative this current size of the view.

int g_iRowHeight = 0, g_iColWidth = 0;

// The default constructor sets every field to zero. It is only called in
// by Serialize, so the field will be initialized.

Figure::Figure()
 :m_iRow(0),
  m_iCol(0),
  m_iDirection(NORTH),
  m_rfColor(WHITE)
{
  ::memset(&m_squareInfo, 0, sizeof m_squareInfo);
}

// This in the main constructor. It initializes the direction (north, east,
// south, or west), the color (red,  brown, turquoise, green, yellow, blue, or
// purple), the color grid, and the square arrays. The red figure sub class
// will initialize all four arrays with the same values because it cannot be
// rotated. The brown, turquoise, and green figure sub classes will initialize
// both the north and south array to its vertical direction as well as the 
// east and west direction to its horizontal direction. The yellow, blue, and
// purple figure sub classes, finally, will initialize all four array with
// different values because the can be rotated in all four directions.

Figure::Figure(int iDirection, COLORREF rfColor,
               const SquareInfo& squareInfo)
 :m_iRow(0),
  m_iCol(COLS / 2),
  m_iDirection(iDirection),
  m_rfColor(rfColor),
  m_pColorGrid(NULL)
{
  ::memcpy(&m_squareInfo, &squareInfo, sizeof m_squareInfo);
}

// The copy constructor is called by the Copy constructors of the sub classes
// to create a copy of the object.

Figure Figure::operator=(const Figure& figure)
{
  if (this != &figure)
  {
    m_iRow = figure.m_iRow;
    m_iCol = figure.m_iCol;
    m_iDirection = figure.m_iDirection;
    m_rfColor = figure.m_rfColor;
    m_pColorGrid = figure.m_pColorGrid;
    ::memcpy(&m_squareInfo, &figure.m_squareInfo, sizeof m_squareInfo);
  }

  return *this;
}

// IsSquareValid is called by IsFigureValid. It checks whether the
// given square is in the grid and that it is not already occupied by another
// color.

BOOL Figure::IsSquareValid(int iRow, int iCol) const
{
  return (iRow >= 0) && (iRow < ROWS) &&
         (iCol >= 0) && (iCol < COLS) &&
         (m_pColorGrid->Index(iRow, iCol) == DEFAULT_COLOR);
}

// IsFigureValid checks whether the figure is at an valid position by examine
// the four squares of the figure. It is called by MoveLeft, MoveRight,
// Rotate, and MoveDown.

BOOL Figure::IsFigureValid() const
{
  SquareArray* pSquareArray = m_squareInfo[m_iDirection];

  for (int iIndex = 0; iIndex < SQUARE_ARRAY_SIZE; ++iIndex)
  {    
    Square& square = (*pSquareArray)[iIndex];

    if (!IsSquareValid(m_iRow + square.Row(), m_iCol + square.Col()))
    {
      return FALSE;
    }
  }

  return TRUE;
}

// RotateClockwiseOneQuarter and RotateAnticlockwiseOneQuarter moves the
// direction one quarter of the compass rose.

void Figure::RotateClockwiseOneQuarter()
{
    switch (m_iDirection)
    {
      case NORTH:
        m_iDirection = EAST;
        break;

      case EAST:
        m_iDirection = SOUTH;
        break;

      case SOUTH:
        m_iDirection = WEST;
        break;

      case WEST:
        m_iDirection = NORTH;
        break;
    }
}

void Figure::RotateAnticlockwiseOneQuarter()
{
    switch (m_iDirection)
    {
      case NORTH:
        m_iDirection = WEST;
        break;

      case WEST:
        m_iDirection = SOUTH;
        break;

      case SOUTH:
        m_iDirection = EAST;
        break;

      case EAST:
        m_iDirection = NORTH;
        break;
    }
}

// MoveLeft, MoveRight, RotateClockwise, RotateAnticlockwise, and MoveDown all
// works in the same way. They make the change and test whether the figure is
// still valid. If it is, true is returned. If not, the change is undone and
// false is returned. Remember that the rows are increased downwards and the
// columns are increased to the right.

BOOL Figure::MoveLeft()
{
    --m_iCol;

    if (IsFigureValid())
    {
      return TRUE;
    }

    else
    {
      ++m_iCol;
      return FALSE;
    }
}

BOOL Figure::MoveRight()
{
    ++m_iCol;

    if (IsFigureValid())
    {
      return TRUE;
    }

    else
    {
      --m_iCol;
      return FALSE;
    }
}

BOOL Figure::RotateClockwise()
{
    RotateClockwiseOneQuarter();

    if (IsFigureValid())
    {
      return TRUE;
    }

    else
    {
      RotateAnticlockwiseOneQuarter();
      return FALSE;
    }
}

BOOL Figure::RotateAnticlockwise()
{
    RotateAnticlockwiseOneQuarter();

    if (IsFigureValid())
    {
      return TRUE;
    }

    else
    {
      RotateClockwiseOneQuarter();
      return FALSE;
    }
}

BOOL Figure::MoveDown()
{
    ++m_iRow;

    if (IsFigureValid())
    {
      return TRUE;
    }

    else
    {
      --m_iRow;
      return FALSE;
    }
}

// AddToGrid is called by the document class when the figure can not be moved
// another step downwards. In that case, a new figure is introduced and the
// squares of the figure is added to the grid.

void Figure::AddToGrid()
{
  SquareArray* pSquareArray = m_squareInfo[m_iDirection];

  for (int iIndex = 0; iIndex < SQUARE_ARRAY_SIZE; ++iIndex)
  {    
    Square& square = (*pSquareArray)[iIndex];
    m_pColorGrid->Index(m_iRow + square.Row(),
                        m_iCol + square.Col()) = m_rfColor;
  }
}

// When a figure has been moved and rotated, it needs to be repainted. In
// order to do so without repainting the whole game grid, we need the figures
// area. The rectangle returned holds the coordinates of the squares, not
// pixel coordinates. The translation is done by OnUpdate in the view class.

CRect Figure::GetArea() const
{
  int iMinRow = 0, iMaxRow = 0, iMinCol = 0, iMaxCol = 0;
  SquareArray* pSquareArray = m_squareInfo[m_iDirection];

  for (int iIndex = 0; iIndex < SQUARE_ARRAY_SIZE; ++iIndex)
  {    
    Square& square = (*pSquareArray)[iIndex];

    int iRow = square.Row();
    iMinRow = (iRow < iMinRow) ? iRow : iMinRow;
    iMaxRow = (iRow > iMaxRow) ? iRow : iMaxRow;

    int iCol = square.Col();
    iMinCol = (iCol < iMinCol) ? iCol : iMinCol;
    iMaxCol = (iCol > iMaxCol) ? iCol : iMaxCol;
  }

  return CRect(m_iCol + iMinCol, m_iRow + iMinRow,
               m_iCol + iMaxCol + 1, m_iRow + iMaxRow + 1);
}

// Draw is called when the figure needs to be repainted. It selected a black
// pen and a brush with the figure's color. Then it draws the four square of
// the figure. The iColorStatus parameter makes the figure appear in color or
// in gray scale.

void Figure::Draw(int iColorStatus, CDC* pDC) const
{
  CPen pen(PS_SOLID, 0, BLACK);
  CPen* pOldPen = pDC->SelectObject(&pen);

  CBrush brush((iColorStatus == COLOR) ? m_rfColor : GrayScale(m_rfColor));
  CBrush* pOldBrush = pDC->SelectObject(&brush);

  SquareArray* pSquareArray = m_squareInfo[m_iDirection];

  for (int iIndex = 0; iIndex < SQUARE_ARRAY_SIZE; ++iIndex)
  {    
    Square& square = (*pSquareArray)[iIndex];
    DrawSquare(m_iRow + square.Row(), m_iCol + square.Col(), pDC);
  }

  pDC->SelectObject(&pOldBrush);
  pDC->SelectObject(&pOldPen);
}

// DrawSquare is a freestanding method because it is called by both the 
// figure and the view class. Given the row and column of a square as well as 
// a pointer to a device context, it draws the square. It uses the public
// Figure fields g_iColWidth and g_iRowHeight to calculate the size and
// position in pixels of the square.

void DrawSquare(int iRow, int iCol, CDC* pDC)
{
  CPoint ptTopLeft(iCol * g_iColWidth, iRow * g_iRowHeight);
  CSize szSquare(g_iColWidth, g_iRowHeight);
  CRect rcSquare(ptTopLeft, szSquare);
  pDC->Rectangle(rcSquare);
}

// Serialize is called when the user chooses the Save of Open menu item. The
// current row, column, and direction as well as the figure's color and its
// four square arrays are stored or loaded, respectively.

void Figure::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive << m_iRow << m_iCol << m_iDirection << m_rfColor;
    archive.Write(&m_squareInfo, sizeof m_squareInfo);
  }

  if (archive.IsLoading())
  {
    archive >> m_iRow >> m_iCol >> m_iDirection >> m_rfColor;
    archive.Read(&m_squareInfo, sizeof m_squareInfo);
  }
}
