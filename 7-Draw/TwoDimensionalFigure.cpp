#include "StdAfx.h"

#include "List.h"
#include "Color.h"
#include "Font.h"

#include "Figure.h"
#include "TwoDimensionalFigure.h"

TwoDimensionalFigure::TwoDimensionalFigure()
 :m_bFilled(FALSE)
{
  // Empty.
}

TwoDimensionalFigure::TwoDimensionalFigure(const Color& color, BOOL bFilled)
 :Figure(color),
  m_bFilled(bFilled)
{
  // Empty.
}

void TwoDimensionalFigure::Serialize(CArchive& archive)
{
  Figure::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_bFilled;
  }

  if (archive.IsLoading())
  {
    archive >> m_bFilled;
  }
}
