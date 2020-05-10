#include "StdAfx.h"

#include "List.h"
#include "Font.h"
#include "Color.h"
#include "Check.h"

#include "Figure.h"
#include "TwoDimensionalFigure.h"
#include "RectangleFigure.h"
#include "EllipseFigure.h"

// For serialization purposes, we need to implement the serial macro and we
// also need a default constructor.

EllipseFigure::EllipseFigure()
 :m_eDragMode(CREATE_ELLIPSE)
{
  // Empty.
}

// When the ellipse is created, both the corners of the rectangle surrounding
// the ellipse (m_ptTopLeft and m_ptBottomRight) is set the given point.

EllipseFigure::EllipseFigure(const Color& color, const CPoint& ptTopLeft,
                             BOOL bFilled)
 :TwoDimensionalFigure(color, bFilled),
  RectangleFigure(color, ptTopLeft, bFilled),
  m_eDragMode(CREATE_ELLIPSE)
{
  // Empty.
}

// The copy constructor is called by Copy, which dynamically creates and
// returns a pointer to a new object.

EllipseFigure::EllipseFigure(const EllipseFigure& ellipse)
 :TwoDimensionalFigure(ellipse),
  RectangleFigure(ellipse),
  m_eDragMode(ellipse.m_eDragMode)
{
  // Empty.
}

Figure* EllipseFigure::Copy() const
{
  check_memory(return (new EllipseFigure(*this)));
}

// GetCursor returns a hairline when the ellipse is in the process of being
// created, a vertical or horizontal arrow for modifying the ellipse, and an
// arrow in all four directions for movements.

HCURSOR EllipseFigure::GetCursor() const
{
  switch (m_eDragMode)
  {
    case CREATE_ELLIPSE:
      return AfxGetApp()->LoadStandardCursor(IDC_CROSS);

    case MODIFY_TOP:
    case MODIFY_BOTTOM:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZENS);

    case MODIFY_LEFT:
    case MODIFY_RIGHT:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);

    case MOVE_ELLIPSE:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
  }

  // As all possible cases are covered by the switch statement above, this
  // point of the method is never reached. The assertion is for debugging
  // purposes only.

  check(FALSE);
  return NULL;
}

// Just as in the rectangle case, Click first decides if the user has clicked
// on one of the four endpoints in a way similar to the rectangle case, the
// only real difference is that the positions are different in relation to the
// figure.

// If the user has not clicked at one of the modifying positions, we have to
// decide if the user has clicked on the ellipse itself. It is rather easy if
// the ellipse is not filled, we create an elliptic region by using the MFC
// class CRgn and test if the mouse position is in it. If the ellipse is not
// filled, we create two regions, one slightly smaller than the ellipse and on
// slightly larger. If the mouse position is included in the larger region but
// not in the smaller one, we have a hit.

BOOL EllipseFigure::Click(const CPoint& ptMouse)
{
  int xCenter = (m_ptTopLeft.x + m_ptBottomRight.x) / 2;
  int yCenter = (m_ptTopLeft.y + m_ptBottomRight.y) / 2;

  // Has the user clicked at the leftmost point?

  CRect rcLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2),
               yCenter - (SQUARE_SIDE / 2),
               m_ptTopLeft.x + (SQUARE_SIDE / 2),
               yCenter + (SQUARE_SIDE / 2));

  if (rcLeft.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_LEFT;
    return TRUE;
  }

  // Or the rightmost point?

  CRect rcRight(m_ptBottomRight.x - (SQUARE_SIDE / 2),
                yCenter - (SQUARE_SIDE / 2),
                m_ptBottomRight.x + (SQUARE_SIDE / 2),
                yCenter + (SQUARE_SIDE / 2));

  if (rcRight.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_RIGHT;
    return TRUE;
  }

  // Or the topmost point?

  CRect rcTop(xCenter - (SQUARE_SIDE / 2),
              m_ptTopLeft.y - (SQUARE_SIDE / 2),
              xCenter + (SQUARE_SIDE / 2),
              m_ptTopLeft.y + (SQUARE_SIDE / 2));

  if (rcTop.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_TOP;
    return TRUE;
  }

  // Or the bottommost point?

  CRect rcBottom(xCenter - (SQUARE_SIDE / 2),
                 m_ptBottomRight.y - (SQUARE_SIDE / 2), 
                 xCenter + (SQUARE_SIDE / 2),
                 m_ptBottomRight.y + (SQUARE_SIDE / 2));

  if (rcBottom.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_BOTTOM;
    return TRUE;
  }

  CRgn rgArea;
  rgArea.CreateEllipticRgn(m_ptTopLeft.x, m_ptTopLeft.y,
                           m_ptBottomRight.x, m_ptBottomRight.y);

  // Is the ellipse filled?

  if (IsFilled())
  {
    m_eDragMode = MOVE_ELLIPSE;
    return rgArea.PtInRegion(ptMouse);
  }

  // Or unfilled?

  else
  {
    int xMin = min(m_ptTopLeft.x, m_ptBottomRight.x);
    int xMax = max(m_ptTopLeft.x, m_ptBottomRight.x);

    int yMin = min(m_ptTopLeft.y, m_ptBottomRight.y);
    int yMax = max(m_ptTopLeft.y, m_ptBottomRight.y);

    CRgn rgSmallArea, rgLargeArea;
    rgSmallArea.CreateEllipticRgn(xMin + (SQUARE_SIDE / 2),
                                  yMin + (SQUARE_SIDE / 2),
                                  xMax - (SQUARE_SIDE / 2),
                                  yMax - (SQUARE_SIDE / 2));
    rgLargeArea.CreateEllipticRgn(xMin - (SQUARE_SIDE / 2),
                                  yMin - (SQUARE_SIDE / 2),
                                  xMax + (SQUARE_SIDE / 2),
                                  yMax + (SQUARE_SIDE / 2));

    m_eDragMode = MOVE_ELLIPSE;
    return rgLargeArea.PtInRegion(ptMouse) &&
           !rgSmallArea.PtInRegion(ptMouse);
  }
}

// MoveOrModify works in a way similar to the rectangle case. Depending on the
// settings of m_eDragMode by the preceding call to Click, DoubleClick, or
// Inside, the fields m_ptTopLeft or m_ptBottomRight (or both) will be
// modified.

void EllipseFigure::MoveOrModify(const CSize& szDistance)
{
  switch (m_eDragMode)
  {
    case CREATE_ELLIPSE:
      m_ptBottomRight += szDistance;
      break;

    case MODIFY_LEFT:
      m_ptTopLeft.x += szDistance.cx;
      break;

    case MODIFY_RIGHT:
      m_ptBottomRight.x += szDistance.cx;
      break;

    case MODIFY_TOP:
      m_ptTopLeft.y += szDistance.cy;
      break;

    case MODIFY_BOTTOM:
      m_ptBottomRight.y += szDistance.cy;
      break;

    case MOVE_ELLIPSE:
      Move(szDistance);
      break;
  }
}

// Draw works in a way similar to Draw in RectangleFigure. If the ellipse is not
// filled, we use a hollow brush when we paint it. Again, we only need to
// set the field lbStyle to BS_HOLLOW (BS = Brush Style).

void EllipseFigure::Draw(CDC *pDC) const
{
  CPen pen(PS_SOLID, 0, (COLORREF) GetColor());
  CPen* pOldPen = pDC->SelectObject(&pen);

  if (IsFilled())
  {
    CBrush brush((COLORREF) GetColor());
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Ellipse(m_ptTopLeft.x, m_ptTopLeft.y,
                 m_ptBottomRight.x, m_ptBottomRight.y);
    pDC->SelectObject(pOldBrush);
  }

  else
  {
    LOGBRUSH lbBrush;
    lbBrush.lbStyle = BS_HOLLOW;

    CBrush brush;
    brush.CreateBrushIndirect(&lbBrush);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Ellipse(m_ptTopLeft.x, m_ptTopLeft.y,
                 m_ptBottomRight.x, m_ptBottomRight.y);
    pDC->SelectObject(pOldBrush);
  }

  pDC->SelectObject(pOldPen);

  if (IsMarked())
  {
    CPen pen(PS_SOLID, 0, BLACK);
    CPen* pOldPen = pDC->SelectObject(&pen);

    CBrush brush(BLACK);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    int xCenter = (m_ptTopLeft.x + m_ptBottomRight.x) / 2;
    int yCenter = (m_ptTopLeft.y + m_ptBottomRight.y) / 2;

    CRect rcLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2), yCenter - (SQUARE_SIDE / 2),
                 m_ptTopLeft.x + (SQUARE_SIDE / 2), yCenter + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcLeft);

    CRect rcRight(m_ptBottomRight.x - (SQUARE_SIDE / 2), yCenter - (SQUARE_SIDE / 2),
                  m_ptBottomRight.x + (SQUARE_SIDE / 2), yCenter + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcRight);

    CRect rcTop(xCenter - (SQUARE_SIDE / 2), m_ptTopLeft.y - (SQUARE_SIDE / 2),
                xCenter + (SQUARE_SIDE / 2), m_ptTopLeft.y + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcTop);

    CRect rcBottom(xCenter - (SQUARE_SIDE / 2), m_ptBottomRight.y - (SQUARE_SIDE / 2),
                   xCenter + (SQUARE_SIDE / 2), m_ptBottomRight.y + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcBottom);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}
