#include "StdAfx.h"
#include "Page.h"

Page::Page()
 :m_iFirstParagraph(0),
  m_iLastParagraph(0)
{
  // Empty.
}

Page::Page(int iFirstParagraph, int iLastParagraph)
 :m_iFirstParagraph(iFirstParagraph),
  m_iLastParagraph(iLastParagraph)
{
  // Empty.
}

void Page::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive >> m_iFirstParagraph >> m_iLastParagraph;
  }

  if (archive.IsLoading())
  {
    archive >> m_iFirstParagraph >> m_iLastParagraph;
  }
}
