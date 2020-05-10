#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Font.h"
#include "Color.h"
#include "Check.h"

#include "Reference.h"
#include "SyntaxTree.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "Caret.h"
#include "CalcView.h"
#include "CalcDoc.h"

// The default constructor sets the pointer to this cell matrix for each cell.

CellMatrix::CellMatrix()
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_buffer[iRow][iCol].SetCellMatrix(this);
    }
  }
}

// The copy constructor and the assignment operator copy the cells one by
// one and set the cell matrix pointer for each cell. This gives that every
// cell has a pointer to the matrix it belongs to.

CellMatrix::CellMatrix(const CellMatrix& cellMatrix)
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_buffer[iRow][iCol] = cellMatrix.m_buffer[iRow][iCol];
      m_buffer[iRow][iCol].SetCellMatrix(this);
    }
  }
}

CellMatrix CellMatrix::operator=(const CellMatrix& cellMatrix)
{
  if (this != &cellMatrix)
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        m_buffer[iRow][iCol] = cellMatrix.m_buffer[iRow][iCol];
        m_buffer[iRow][iCol].SetCellMatrix(this);
      }
    }
  }

  return *this;
}

// SetTargetSetMatrix sets the set target matrix pointer for each cell in
// the matrix.

void CellMatrix::SetTargetSetMatrix(TSetMatrix* pTargetSetMatrix)
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_buffer[iRow][iCol].SetTargetSetMatrix(pTargetSetMatrix);
    }
  }
}

// Get returns a pointer to the cell indicated by the given row and column
// or by the given reference. The row and column are checked to be inside
// the limits of the matrix. However, the assertions are for debugging
// purpose only, the method will never be called with invalid parameters.

Cell* CellMatrix::Get(int iRow, int iCol) const
{
  check((iRow >= 0) && (iRow < ROWS));
  check((iCol >= 0) && (iCol < COLS));

  return (Cell*) &m_buffer[iRow][iCol];
}

Cell* CellMatrix::Get(Reference home) const
{
  return Get(home.GetRow(), home.GetCol());
}

// Serialize is called when the user choose the save or open menu item. It
// serializes the matrix one cell at a time. In case of loading, it does
// also set the cell matrix pointer of the cell. Every cell will have a
// pointer to the matrix it belong to.

void CellMatrix::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        m_buffer[iRow][iCol].Serialize(archive);
      }
    }
  }

  if (archive.IsLoading())
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        m_buffer[iRow][iCol].Serialize(archive);
        m_buffer[iRow][iCol].SetCellMatrix(this);
      }
    }
  }
}
