#include "StdAfx.h"
#include "Line.h"

Line::Line()
 :m_iFirstChar(0),
  m_iLastChar(0),
  m_iHeight(0)
{
  // Empty.
}

Line::Line(int iFirstChar, int iLastChar, int iHeight)
 :m_iFirstChar(iFirstChar),
  m_iLastChar(iLastChar),
  m_iHeight(iHeight)
{
  // Empty.
}

void Line::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive >> m_iFirstChar >> m_iLastChar >> m_iHeight;
  }

  if (archive.IsLoading())
  {
    archive >> m_iFirstChar >> m_iLastChar >> m_iHeight;
  }
}
