#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Reference.h"

// The default constructor is used for serialization purposes and for storing
// references in sets. The copy constructor and the assignment operator are
// necessary for the same reason. The second constructor initializes the field
// with the given row and column.

Reference::Reference()
 :m_iRow(0),
  m_iCol(0)
{
  // Empty.
}

Reference::Reference(int iRow, int iCol)
 :m_iRow(iRow),
  m_iCol(iCol)
{
  // Empty.
}

Reference::Reference(const Reference& reference)
 :m_iRow(reference.m_iRow),
  m_iCol(reference.m_iCol)
{
  // Empty.
}

Reference Reference::operator=(const Reference& reference)
{
  if (this != &reference)
  {
    m_iRow = reference.m_iRow;
    m_iCol = reference.m_iCol;
  }

  return *this;
}

// The left and right references are equal if their rows and column are
// equal.

BOOL operator==(const Reference& rfLeft, const Reference& rfRight)
{
  return (rfLeft.m_iRow == rfRight.m_iRow) &&
         (rfLeft.m_iCol == rfRight.m_iCol);
}

// The left reference is less that the right reference if its row is less than
// the right ones, or (if the rows are equal) the left column is less that the
// right one.

BOOL operator<(const Reference& rfLeft, const Reference& rfRight)
{
  return (rfLeft.m_iRow < rfRight.m_iRow) ||
         ((rfLeft.m_iRow == rfRight.m_iRow) &&
          (rfLeft.m_iCol < rfRight.m_iCol));
}

// ToString returns the reference as a string. The zero row is written as one
// and the zero column is written as a small ’a’.

CString Reference::ToString() const
{
  CString stBuffer;
  stBuffer.Format(TEXT("%c%d"), (TCHAR) (TEXT('a') + m_iCol),  m_iRow + 1);
  return stBuffer;
}

// Serialize stores and loads the row and column of the reference.

void Reference::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive << m_iRow << m_iCol;
  }
  
  if (archive.IsLoading())
  {
    archive >> m_iRow >> m_iCol;
  }
}
