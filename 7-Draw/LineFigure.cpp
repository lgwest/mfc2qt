#include "StdAfx.h"
#include <cmath>

#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Check.h"

#include "Figure.h"
#include "LineFigure.h"

// we also need a default constructor because we are about to serialize
// objects of the class.

LineFigure::LineFigure()
 :m_ptFirst(0, 0),
  m_ptLast(0, 0),
  m_eDragMode(CREATE_LINE)
{
  // Empty.
}

// When a new line is created, it is given a color and a starting point. From
// the beginning, both the end points of the line is set to the given point.

LineFigure::LineFigure(const Color& color, const CPoint& ptMouse)
 :Figure(color),
  m_ptFirst(ptMouse),
  m_ptLast(ptMouse),
  m_eDragMode(CREATE_LINE)
{
  // Empty.
}

// The copy constructor must call the constructor of Figure in order to set
// the color and the mark status.

LineFigure::LineFigure(const LineFigure& line)
 :Figure(line),
  m_ptFirst(line.m_ptFirst),
  m_ptLast(line.m_ptLast),
  m_eDragMode(line.m_eDragMode)
{
  // Empty.
}

// Copy dynamically creates and returns a pointer to a copy of its own object
// by calling the copy constructor.

Figure* LineFigure::Copy() const
{
  check_memory(return (new LineFigure(*this)));
}

// Serialize stores and loads the two end points of the line. We must also
// call Serialize in Figure to store and load its color. It calls in turn
// Serialize in CObject in order to store and load information about the
// line object.

void LineFigure::Serialize(CArchive& archive)
{
  Figure::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_ptFirst << m_ptLast;
  }

  if (archive.IsLoading())
  {
    archive >> m_ptFirst >> m_ptLast;
  }
}

BOOL LineFigure::Click(const CPoint& ptMouse)
{
  // Did the user click on the first end point?

  CRect rcFirst(m_ptFirst.x - (SQUARE_SIDE / 2),
    m_ptFirst.y - (SQUARE_SIDE / 2),
    m_ptFirst.x + (SQUARE_SIDE / 2),
    m_ptFirst.y + (SQUARE_SIDE / 2));

  if (rcFirst.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_FIRST;
    return TRUE;
  }

  // Or the second one?

  CRect rcLast(m_ptLast.x - (SQUARE_SIDE / 2),
               m_ptLast.y - (SQUARE_SIDE / 2),
               m_ptLast.x + (SQUARE_SIDE / 2),
               m_ptLast.y + (SQUARE_SIDE / 2));

  if (rcLast.PtInRect(ptMouse))
  {
    m_eDragMode = MODIFY_LAST;
    return TRUE;
  }

  m_eDragMode = MOVE_LINE;

  // If the line completely vertical?

  if (m_ptFirst.x == m_ptLast.x)
  {
    CRect rcLine(m_ptFirst.x - (SQUARE_SIDE / 2), m_ptFirst.y,
                 m_ptLast.x + (SQUARE_SIDE / 2), m_ptLast.y);
    rcLine.NormalizeRect();
    return rcLine.PtInRect(ptMouse);
  }

  // Or not?

  else
  {
    CRect rcLine(m_ptFirst, m_ptLast);
    rcLine.NormalizeRect();

    if (rcLine.PtInRect(ptMouse))
    {
      CPoint ptMin = (m_ptFirst.x < m_ptLast.x)
                   ? m_ptFirst : m_ptLast;
      CPoint ptMax = (m_ptFirst.x < m_ptLast.x)
                   ? m_ptLast : m_ptFirst;

      int cxLine = ptMax.x - ptMin.x;
      int cyLine = ptMax.y - ptMin.y;

      int cxMouse = ptMouse.x - ptMin.x;
      int cyMouse = ptMouse.y - ptMin.y;

      return fabs(cyMouse - cxMouse * (double) cyLine /
                  cxLine) <= (SQUARE_SIDE / 2);
    }

    return FALSE;
  }
}

// As we do not consider double clicks on lines, we just ignore the
// DoubleClick function.

BOOL LineFigure::DoubleClick(const CPoint& /* ptMouse */)
{
  return FALSE;
}

// Inside takes a rectangle and decides whether the line is located completely
// inside the rectangle. It is easy to implement; we just test if both
// endpoints are inside the rectangle.

BOOL LineFigure::Inside(const CRect& rcInside) const
{
  return rcInside.PtInRect(m_ptFirst) && rcInside.PtInRect(m_ptLast);
}

// When the user drags the mouse on a marked figure, it is supposed to be 
// moved or modified. The document class calls MoveOrModiy, but it does not 
// really know whether the user has hit one the endpoints (modify) or the line
// itself (move), that information is stored locally by the line object. The
// document class just calls MoveOrModify, and the line object takes care of
// the rest.

// MoveOrModify takes the distance of the mouse transfer since the previous
// call. If we are in the process of creating the line, the distance shall
// affect the last endpoint of the line. If the users clicked on one of the
// endpoints, we modify that endpoint. If they clicked on the line, it moves
// the line by calling Move, which is easy to implement: we just modify both
// the endpoints.

void LineFigure::MoveOrModify(const CSize& szDistance)
{
  switch (m_eDragMode)
  {
    case CREATE_LINE:
      m_ptLast += szDistance;
      break;

    case MODIFY_FIRST:
      m_ptFirst += szDistance;
      break;

    case MODIFY_LAST:
      m_ptLast += szDistance;
      break;

    case MOVE_LINE:
      Move(szDistance);
      break;
  }
}

void LineFigure::Move(const CSize& szDistance)
{
  m_ptFirst += szDistance;
  m_ptLast += szDistance;
}

// Draw is called every time the figure needs to be repainted, partly or
// completely. If the line is unmarked, we just select our pen and draw the
// line. When we select a new pen we save the old one, which is later
// restored. If the line is marked, we add two squares at each endpoint. For
// that, we need a brush. We create and select our brush in a way similar to
// the pen. We create and paint the squares by calling RectangleFigure, there is no
// function in the CDC class to paint a square.

void LineFigure::Draw(CDC *pDC) const
{
  CPen pen(PS_SOLID, 0, (COLORREF) GetColor());
  CPen* pOldPen = pDC->SelectObject(&pen);

  pDC->MoveTo(m_ptFirst.x, m_ptFirst.y);
  pDC->LineTo(m_ptLast.x, m_ptLast.y);

  pDC->SelectObject(pOldPen);

  if (IsMarked())
  {
      CPen pen(PS_SOLID, 0, BLACK);
      CPen* pOldPen = pDC->SelectObject(&pen);

      CBrush brush(BLACK);
      CBrush* pOldBrush = pDC->SelectObject(&brush);

      CRect rcFirst(m_ptFirst.x - (SQUARE_SIDE / 2),
                    m_ptFirst.y - (SQUARE_SIDE / 2),
                    m_ptFirst.x + (SQUARE_SIDE / 2),
                    m_ptFirst.y + (SQUARE_SIDE / 2));
      pDC->Rectangle(rcFirst);

      CRect rcLast(m_ptLast.x - (SQUARE_SIDE / 2),
                   m_ptLast.y - (SQUARE_SIDE / 2),
                   m_ptLast.x + (SQUARE_SIDE / 2),
                   m_ptLast.y + (SQUARE_SIDE / 2));
      pDC->Rectangle(rcLast);

      pDC->SelectObject(pOldPen);
      pDC->SelectObject(pOldBrush);
    }
}
 
// GetArea calculates the area of the line by creating a rectangle with the
// endpoints of the line as opposite corners. If the line is marked, we need
// to slightly increase the rectangle in order to fit in the squares at the
// endpoints.

CRect LineFigure::GetArea() const
{
  CRect rcLine(m_ptFirst, m_ptLast);
  rcLine.NormalizeRect();

  if (IsMarked())
  {
    rcLine.left   -= (SQUARE_SIDE / 2);
    rcLine.right  += (SQUARE_SIDE / 2);
    rcLine.top    -= (SQUARE_SIDE / 2);
    rcLine.bottom += (SQUARE_SIDE / 2);
  }

  return rcLine;
}

// GetCursor returns a handle to a cursor appropriate to the current state of
// the line. If we were in the process of creating a line, the cursor would be
// a hair cross (IDC_CROSS). Otherwise, it would be an icon symbolizing
// resizing in all directions (IDC_SIZEALL).

HCURSOR LineFigure::GetCursor() const
{
  switch (m_eDragMode)
  {
    case CREATE_LINE:
      return AfxGetApp()->LoadStandardCursor(IDC_CROSS);

    case MODIFY_FIRST:
    case MODIFY_LAST:
    case MOVE_LINE:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
  }

  return NULL;
}
