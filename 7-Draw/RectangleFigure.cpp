#include "StdAfx.h"

#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Check.h"

#include "Figure.h"
#include "TwoDimensionalFigure.h"
#include "RectangleFigure.h"

// Similar to LineFigure and ArrowFigure, we need to implement the serial macro and write
// a default constructor for serialization reasons.

RectangleFigure::RectangleFigure()
: m_ptTopLeft(0, 0),
  m_ptBottomRight(0, 0),
  m_eDragMode(CREATE_RECTANGLE)
{
  // Empty.
}

// When the rectangle is created, both the corners (m_ptTopLeft and
// m_ptBottomRight) are set to the given point.

RectangleFigure::RectangleFigure(const Color& color, const CPoint& ptTopLeft,
                                 BOOL bFilled)
: TwoDimensionalFigure(color, bFilled),
  m_ptTopLeft(ptTopLeft),
  m_ptBottomRight(ptTopLeft),
  m_eDragMode(CREATE_RECTANGLE)
{
  // Empty.
}

// The copy constructor is called by Copy, which dynamically creates and
// returns a pointer to a new object.

RectangleFigure::RectangleFigure(const RectangleFigure& rectangle)
: TwoDimensionalFigure(rectangle),
  m_ptTopLeft(rectangle.m_ptTopLeft),
  m_ptBottomRight(rectangle.m_ptBottomRight),
  m_eDragMode(rectangle.m_eDragMode)
{
  // Empty.
}

Figure* RectangleFigure::Copy() const
{
  check_memory(return (new RectangleFigure(*this)));
}

// When the user clicks at a rectangle, Click is called. The user may hit one
// of the four corners of the rectangle, the borders of the rectangle, or (if
// the rectangle if filled) the interior of the rectangle. The field
// m_eDragMode is set to an appropriate value.

// Click works in a way similar to its equivalent function in the line class.
// It tests in turn the fours corners of the rectangle. If the rectangle is
// filled, it tests if the user has clicked in the rectangle. Otherwise, it
// tests if the user has clicked on any of the four lines constituting the
// rectangle by constructing a slightly small rectangle and a slightly larger
// one. If the mouse position is included in the larger rectangle but not in
// the smaller one, the user has hit the rectangle.

BOOL RectangleFigure::Click(const CPoint& ptMouse)
{
  // Did the user click on the top left corner?

  CRect rcTopLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2),
                  m_ptTopLeft.y - (SQUARE_SIDE / 2),
                  m_ptTopLeft.x + (SQUARE_SIDE / 2),
                  m_ptTopLeft.y + (SQUARE_SIDE / 2));

  if (rcTopLeft.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_TOPLEFT;
    return TRUE;
  }

  // Or the top right corner?

  CRect rcTopRight(m_ptBottomRight.x - (SQUARE_SIDE / 2),
                   m_ptTopLeft.y - (SQUARE_SIDE / 2),
                   m_ptBottomRight.x + (SQUARE_SIDE / 2),
                   m_ptTopLeft.y + (SQUARE_SIDE / 2));

  if (rcTopRight.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_TOPRIGHT;
    return TRUE;
  }

  // Or the bottom right corner?

  CRect rcBottomRight(m_ptBottomRight.x - (SQUARE_SIDE / 2),
                      m_ptBottomRight.y - (SQUARE_SIDE / 2),
                      m_ptBottomRight.x + (SQUARE_SIDE / 2),
                      m_ptBottomRight.y + (SQUARE_SIDE / 2));

  if (rcBottomRight.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_BOTTOMRIGHT;
    return TRUE;
  }

  // Or the bottom left corner?

  CRect rcBottomLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2),
                     m_ptBottomRight.y - (SQUARE_SIDE / 2),
                     m_ptTopLeft.x + (SQUARE_SIDE / 2),
                     m_ptBottomRight.y +(SQUARE_SIDE / 2));

  if (rcBottomLeft.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_BOTTOMLEFT;
    return TRUE;
  }

  CRect rcArea(m_ptTopLeft, m_ptBottomRight);
  rcArea.NormalizeRect();

  // Is the rectangle filled?

  if (IsFilled())
  {
    m_eDragMode = MOVE_RECTANGLE;
    return rcArea.PtInRect(ptMouse);
  }

  // Or is it unfilled?

  else
  {
    CSize szMargin((SQUARE_SIDE / 2), (SQUARE_SIDE / 2));

    CRect rcSmallArea(rcArea.TopLeft() + szMargin,
                      rcArea.BottomRight() - szMargin);
    CRect rcLargeArea(rcArea.TopLeft() - szMargin,
                      rcArea.BottomRight() + szMargin);

    m_eDragMode = MOVE_RECTANGLE;
    return rcLargeArea.PtInRect(ptMouse) &&
      !rcSmallArea.PtInRect(ptMouse);
  }
}

// DoubleClick fills the rectangle if it is unfilled, and unfills it if it
// filled.

BOOL RectangleFigure::DoubleClick(const CPoint& ptMouse)
{
  if (Click(ptMouse))
  {
    Fill(!IsFilled());
    return TRUE;
  }

  return FALSE;
}

// Inside is quite easy to implement, we just test that all the top most and
// bottom right corners are inside the given rectangle.

BOOL RectangleFigure::Inside(const CRect& rcInside) const
{
  return rcInside.PtInRect(m_ptTopLeft) &&
         rcInside.PtInRect(m_ptBottomRight);
}

// MoveOrModify and Move works as expected; depending on the setting of
// m_eDragMode by the preceding call to the constructor or Click, m_ptTopLeft
// or m_ptBottomRight (or both) are modified.

void RectangleFigure::MoveOrModify(const CSize& szDistance)
{
  switch (m_eDragMode)
  {
    case CREATE_RECTANGLE:
      m_ptBottomRight += szDistance;
      break;

    case MODIFY_TOPLEFT:
      m_ptTopLeft += szDistance;
      break;

    case MODIFY_TOPRIGHT:
      m_ptBottomRight.x += szDistance.cx;
      m_ptTopLeft.y += szDistance.cy;
      break;

    case MODIFY_BOTTOMLEFT:
      m_ptTopLeft.x += szDistance.cx;
      m_ptBottomRight.y += szDistance.cy;
      break;

    case MODIFY_BOTTOMRIGHT:
      m_ptBottomRight += szDistance;
      break;

    case MOVE_RECTANGLE:
      Move(szDistance);
      break;
  }
}

void RectangleFigure::Move(const CSize& szDistance)
{
  m_ptTopLeft += szDistance;
  m_ptBottomRight += szDistance;
}

// Draw is a little bit different compared with Draw in LineFigure and ArrowFigure. If the
// rectangle is not filled, we use a hollow brush when we paint it. It is
// somewhat more complicated to create that brush, we need to use the Win32
// structure LOGBRUSH. Fortunately, we only need to set the field lbStyle to
// BS_HOLLOW (BS stands for Brush Style). If the rectangle is marked, we just
// create and paint the four corner squares by creating object of CRect.
 
void RectangleFigure::Draw(CDC *pDC) const
{
  CPen pen(PS_SOLID, 0, (COLORREF) GetColor());
  CPen* pOldPen = pDC->SelectObject(&pen);

  if (IsFilled())
  {
    CBrush brush((COLORREF) GetColor());
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Rectangle(m_ptTopLeft.x, m_ptTopLeft.y,
                   m_ptBottomRight.x, m_ptBottomRight.y);
    pDC->SelectObject(pOldBrush);
  }

  else
  {
    CBrush brush;
    LOGBRUSH lbBrush;
    lbBrush.lbStyle = BS_HOLLOW;
    brush.CreateBrushIndirect(&lbBrush);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Rectangle(m_ptTopLeft.x, m_ptTopLeft.y,
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

    CRect rcTopLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2), m_ptTopLeft.y - (SQUARE_SIDE / 2),
                    m_ptTopLeft.x + (SQUARE_SIDE / 2), m_ptTopLeft.y + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcTopLeft);

    CRect rcTopRight(m_ptBottomRight.x - (SQUARE_SIDE / 2), m_ptTopLeft.y - (SQUARE_SIDE / 2),
                      m_ptBottomRight.x + (SQUARE_SIDE / 2), m_ptTopLeft.y+(SQUARE_SIDE / 2));
    pDC->Rectangle(rcTopRight);

    CRect rcBottomRight(m_ptBottomRight.x - (SQUARE_SIDE / 2),
                        m_ptBottomRight.y - (SQUARE_SIDE / 2),
                        m_ptBottomRight.x + (SQUARE_SIDE / 2),
                        m_ptBottomRight.y + (SQUARE_SIDE / 2));
    pDC->Rectangle(rcBottomRight);

    CRect rcBottomLeft(m_ptTopLeft.x - (SQUARE_SIDE / 2), m_ptBottomRight.y-(SQUARE_SIDE / 2),
                       m_ptTopLeft.x + (SQUARE_SIDE / 2),m_ptBottomRight.y+(SQUARE_SIDE / 2));
    pDC->Rectangle(rcBottomLeft);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}

// GetArea simple creates and returns a CRect object with m_ptTopLeft and
// m_ptBottomRight as its corners. If the rectangle is marked, we have increase
// the surrounding area in order to include the four squares.

CRect RectangleFigure::GetArea() const
{
  CRect rcRectangle(m_ptTopLeft, m_ptBottomRight);
  rcRectangle.NormalizeRect();

  if (IsMarked())
  {
    rcRectangle.left   -= (SQUARE_SIDE / 2);
    rcRectangle.right  += (SQUARE_SIDE / 2);
    rcRectangle.top    -= (SQUARE_SIDE / 2);
    rcRectangle.bottom += (SQUARE_SIDE / 2);
  }

  return rcRectangle;
}

// Serialize calls the matching method in Figure to save or load the color.
// Then it saves or loads the positions of the two corners of the rectangle as
// well as its fill status.

void RectangleFigure::Serialize(CArchive& archive)
{
  TwoDimensionalFigure::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_ptTopLeft << m_ptBottomRight;
  }

  if (archive.IsLoading())
  {
    archive >> m_ptTopLeft >> m_ptBottomRight;
  }
}

// GetCursor returns a handle to a hairline cursor if the rectangle is in the
// process of being created and size modification cursors if the rectangle is
// being modified.

HCURSOR RectangleFigure::GetCursor() const
{
  switch (m_eDragMode)
  {
    // Hair cross.
    case CREATE_RECTANGLE:
      return AfxGetApp()->LoadStandardCursor(IDC_CROSS);

    // Arrows northwest and southeast.
    case MODIFY_TOPLEFT:
    case MODIFY_BOTTOMRIGHT:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);

    // Arrows northeast and southwest.
    case MODIFY_TOPRIGHT:
    case MODIFY_BOTTOMLEFT:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);

    // Arrows in all four directions.
    case MOVE_RECTANGLE:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
  }

  // As all possible cases are covered by the switch statement above, this
  // point of the method is never reached. The assertion is for debugging
  // purposes only.

  check(FALSE);
  return NULL;
}
