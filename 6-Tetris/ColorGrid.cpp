#include "StdAfx.h"
#include "Check.h"

#include "Square.h"
#include "Figure.h"
#include "ColorGrid.h"

ColorGrid::ColorGrid()
{
  Clear();
}

void ColorGrid::Clear()
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      Index(iRow, iCol) = DEFAULT_COLOR;
    }
  }
}

COLORREF& ColorGrid::Index(int iRow, int iCol)
{
  check((iRow >= 0) && (iRow < ROWS));
  check((iCol >= 0) && (iCol < COLS));  
  return m_buffer[iRow * COLS + iCol];
}

const COLORREF ColorGrid::Index(int iRow, int iCol) const
{
  check((iRow >= 0) && (iRow < ROWS));
  check((iCol >= 0) && (iCol < COLS));  
  return m_buffer[iRow * COLS + iCol];
}

void ColorGrid::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        archive << Index(iRow, iCol);
      }
    }
  }

  if (archive.IsLoading())
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        archive >> Index(iRow, iCol);
      }
    }
  }
}
