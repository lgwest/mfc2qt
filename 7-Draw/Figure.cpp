#include "StdAfx.h"

#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Figure.h"

Figure::Figure()
 :m_figureColor(0),
  m_bMarked(FALSE)
{
    // Empty.
}

Figure::Figure(const Figure& figure)
 :m_figureColor(figure.m_figureColor),
  m_bMarked(figure.m_bMarked)
{
    // Empty.
}

Figure::Figure(const Color& color, BOOL bMark /* = FALSE */)
 :m_figureColor(color),
  m_bMarked(bMark)
{
    // Empty.
}

// We have to call Serialize in CObject because we want to store information
// about the object, not just it fields. The color is serialized, but not the
// mark status.

void Figure::Serialize(CArchive& archive)
{
  CObject::Serialize(archive);
  m_figureColor.Serialize(archive);
}
