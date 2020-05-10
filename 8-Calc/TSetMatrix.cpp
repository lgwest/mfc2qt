#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Font.h"
#include "Color.h"
#include "Caret.h"
#include "Check.h"

#include "Reference.h"
#include "SyntaxTree.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "CalcView.h"
#include "CalcDoc.h"

// The default constructor is necessary because the document has a member
// object of this class.

TSetMatrix::TSetMatrix()
{
  // Empty.
}

// The copy constructor and the assignment operator, copies the reference sets
// one by one.

TSetMatrix::TSetMatrix(const TSetMatrix& tSetMatrix)
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_buffer[iRow][iCol] = tSetMatrix.m_buffer[iRow][iCol];
    }
  }
}

TSetMatrix TSetMatrix::operator=(const TSetMatrix& tSetMatrix)
{
  if (this != &tSetMatrix)
  {
    for (int iRow = 0; iRow < ROWS; ++iRow)
    {
      for (int iCol = 0; iCol < COLS; ++iCol)
      {
        m_buffer[iRow][iCol] = tSetMatrix.m_buffer[iRow][iCol];
      }
    }
  }

  return *this;
}

// The target set matrix needs a pointer to the cell matrix in order to
// look up cells during searches.

void TSetMatrix::SetCellMatrix(CellMatrix* pCellMatrix)
{
  m_pCellMatrix = pCellMatrix;
}

// Serialize simply traverse the matrix and serializes the target sets one
// by one.

void TSetMatrix::Serialize(CArchive& archive)
{
  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_buffer[iRow][iCol].Serialize(archive);
    }
  }
}

// Get returns a pointer to the target set indicated by the given row and
// column or by the given reference. The row and column are checked to be
// inside the limits of the matrix. However, similar to the cell matrix
// above, the assertions are for debugging purpose only. The method will
// never be called with invalid parameters.

ReferenceSet* TSetMatrix::Get(int iRow, int iCol) const
{
  check((iRow >= 0) && (iRow < ROWS));
  check((iCol >= 0) && (iCol < COLS));

  return (ReferenceSet*) &m_buffer[iRow][iCol];
}

ReferenceSet* TSetMatrix::Get(Reference home) const
{
  return Get(home.GetRow(), home.GetCol());
}

// When the user adds or alters a formula, it is essential that no cycles are
// added to the graph. CheckCircular throws an exception in case it finds a
// cycle. It performs a depth-first search backwards by following the source
// set.

void TSetMatrix::CheckCircular(Reference home,
                               ReferenceSet sourceSet)
{
  for (POSITION position = sourceSet.GetHeadPosition();
       position != NULL; sourceSet.GetNext(position))
  {
    Reference source = sourceSet.GetAt(position);

    if (source == home)
    {
      CString stMessage = TEXT("Circular Reference.");
      throw stMessage;
    }

    Cell* pCell = m_pCellMatrix->Get(source);
    ReferenceSet nextSourceSet = pCell->GetSourceSet();
    CheckCircular(home, nextSourceSet);
  }
}

// When the value of a cell is updated, it is essential that the formulas
// having references to the cell are notified and that their value is re-
// evaluated. EvaluateTargets performs a breath-first search by following the
// target sets forward. Unlike the check for circular cycles above, we cannot
// perform a depth-first search. That would introduce the risk of the cell
// being evaluated in the wrong order.

ReferenceSet TSetMatrix::EvaluateTargets(Reference home)
{
  Cell* pHome = m_pCellMatrix->Get(home);
  pHome->EvaluateValue(FALSE);

  ReferenceSet resultSet;
  resultSet.Add(home);

  ReferenceSet* pTargetSet = Get(home);
  ReferenceSet updateSet = *pTargetSet;

  while (!updateSet.IsEmpty())
  {
    Reference target = updateSet.GetHead();
    resultSet.Add(target);
    updateSet.Remove(target);

    Cell* pTarget = m_pCellMatrix->Get(target);
    pTarget->EvaluateValue(FALSE);

    ReferenceSet* pNextTargetSet = Get(target);
    updateSet.AddAll(*pNextTargetSet);
  }

  return resultSet;
}

// AddTargets traverses the source set of the cell with the given reference
// in the cell matrix and, for each source cell, adds the given cell as a
// target in the target set of the source cell.

void TSetMatrix::AddTargets(Reference home)
{
  Cell* pCell = m_pCellMatrix->Get(home);
  ReferenceSet sourceSet = pCell->GetSourceSet();

  for (POSITION position = sourceSet.GetHeadPosition();
       position != NULL; sourceSet.GetNext(position))
  {
    Reference source = sourceSet.GetAt(position);
    ReferenceSet* pTargetSet = Get(source);
    pTargetSet->Add(home);
  }
}

// RemoveTargets traverses the source set of the cell with the given
// reference in the cell matrix and, for each source cell, removes the
// given cell as a target in the target set of the source cell.

void TSetMatrix::RemoveTargets(Reference home)
{
  Cell* pCell = m_pCellMatrix->Get(home);
  ReferenceSet sourceSet = pCell->GetSourceSet();

  for (POSITION position = sourceSet.GetHeadPosition();
       position != NULL; sourceSet.GetNext(position))
  {
    Reference source = sourceSet.GetAt(position);
    ReferenceSet* pTargetSet = Get(source);
    pTargetSet->Remove(home);
  }
}
