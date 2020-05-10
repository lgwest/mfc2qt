#include "StdAfx.h"
#include <cmath>

#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"
#include "Check.h"

#include "Figure.h"
#include "LineFigure.h"
#include "ArrowFigure.h"

// Similar to LineFigure, we need to implement the serial macro and write a default 
// constructor for serialization reasons.

ArrowFigure::ArrowFigure()
{
  // Empty.
}

// The constructors call the constructor of LineFigure to initialize the color and
// end points.

ArrowFigure::ArrowFigure(const ArrowFigure& arrow)
 :Figure(arrow),
  LineFigure(arrow)
{
  m_ptLeft = arrow.m_ptLeft;
  m_ptRight = arrow.m_ptRight;
}

ArrowFigure::ArrowFigure(const Color& color, const CPoint& ptMouse)
 :Figure(color),
  LineFigure(color, ptMouse)
{
  // Empty.
}

// Copy dynamically creates and returns a pointer to a copy of its own object
// by calling the copy constructor.

Figure* ArrowFigure::Copy() const
{
  check_memory(return (new ArrowFigure(*this)));
}

// Serialize in LineFigure takes care of the end points. This method takes care of
// the left and right points or the arrowhead.

void ArrowFigure::Serialize(CArchive& archive)
{
  LineFigure::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_ptLeft << m_ptRight;
  }

  if (archive.IsLoading())
  {
    archive >> m_ptLeft >> m_ptRight;
  }
}

// When the user double clicks on the arrow, its head switches location.

BOOL ArrowFigure::DoubleClick(const CPoint& ptMouse)
{
  if (LineFigure::Click(ptMouse))
  {
    CPoint ptTemp = m_ptFirst;
    m_ptFirst = m_ptLast;
    m_ptLast = ptTemp;
 
    SetArrowPoints();
    return TRUE;
  }

  return FALSE;
}

// MoveOrModify and Move just calls their matching methods in the base class
// LineFigure.

void ArrowFigure::MoveOrModify(const CSize& szDistance)
{
  LineFigure::MoveOrModify(szDistance);
  SetArrowPoints();
}

void ArrowFigure::Move(const CSize& szDistance)
{
  LineFigure::Move(szDistance);
  SetArrowPoints();
}

// Most of the drawing is done by Draw in LineFigure. The method just adds the arrow
// head.

void ArrowFigure::Draw(CDC *pDC) const
{
  LineFigure::Draw(pDC);

  CPen pen(PS_SOLID, 0, (COLORREF) Figure::GetColor());
  CPen* pOldPen = pDC->SelectObject(&pen);

  pDC->MoveTo(m_ptLast);
  pDC->LineTo(m_ptLeft);

  pDC->MoveTo(m_ptLast);
  pDC->LineTo(m_ptRight);

  pDC->SelectObject(pOldPen);
}

// GetArea calls GetArea in LineFigure, and adds the rectangles of the arrowhead.

CRect ArrowFigure::GetArea() const
{
  CRect rcLine = LineFigure::GetArea();

  CRect rcLeftArrow(m_ptLast, m_ptLeft);
  rcLeftArrow.NormalizeRect();
  rcLine.UnionRect(rcLine, rcLeftArrow);

  CRect rcRightArrow(m_ptLast, m_ptRight);
  rcRightArrow.NormalizeRect();
  rcLine.UnionRect(rcLine, rcRightArrow);

  return rcLine;
}
  
// SetArrowPoints calculates the value of m_ptLeft and m_ptRight, the
// endpoints of the arrow head.

void ArrowFigure::SetArrowPoints()
{
  int iHeight = m_ptLast.y - m_ptFirst.y;
  int iWidth = m_ptLast.x - m_ptFirst.x;

  const double PI = 3.14159265;
  double dAlpha = atan2((double) iHeight, (double) iWidth);
  double dBeta = dAlpha + PI;

  double dLeftAngle = dBeta - PI / 4;
  double dRightAngle = dBeta + PI / 4;

  m_ptLeft.x = m_ptLast.x +
               (int) (ARROW_LENGTH * cos(dLeftAngle));
  m_ptLeft.y = m_ptLast.y +
               (int) (ARROW_LENGTH * sin(dLeftAngle));

  m_ptRight.x = m_ptLast.x +
                (int) (ARROW_LENGTH * cos(dRightAngle));
  m_ptRight.y = m_ptLast.y +
                (int) (ARROW_LENGTH * sin(dRightAngle));
}
