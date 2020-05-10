#include "StdAfx.h"

#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"
#include "Check.h"

#include "Figure.h"
#include "TwoDimensionalFigure.h"
#include "RectangleFigure.h"
#include "TextFigure.h"

#include "DrawView.h"
#include "DrawDoc.h"
#include "Draw.h"

IMPLEMENT_DYNCREATE(CDrawView, CScrollView)

BEGIN_MESSAGE_MAP(CDrawView, CScrollView)
  ON_WM_CREATE()
  
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()

  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()

  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CDrawView::CDrawView()
 :m_pDrawDoc(NULL)
{
    // Empty.
}

// OnCreate is called after the view is created but before it is showed. It
// sets and checks the m_pDrawDoc field.

int CDrawView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  // We check that the view has been correctly created.
  if (CScrollView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  m_pDrawDoc = (CDrawDoc*) m_pDocument;
  ASSERT_VALID(m_pDrawDoc);

  CSize szTotal(TOTAL_WIDTH, TOTAL_HEIGHT);
  SetScrollSizes(MM_HIMETRIC, szTotal);

  return 0;
}

// OnLButtonDown, OnMouseMove, OnLButtonUp, and OnLButtonDblClk call
// MouseDown, MouseMove, MouseUp, and DoubleClick of the document class.

// The mouse point is given in device coordinates, we need to translate it
// into logical coordinates by creating and preparing a device context and
// calling DPtoLP.

void CDrawView::OnLButtonDown(UINT uFlags, CPoint ptMouse)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);
  dc.DPtoLP(&ptMouse);

  BOOL bControlKeyDown = (uFlags & MK_CONTROL);
  m_pDrawDoc->MouseDown(ptMouse, bControlKeyDown, &dc);
}

void CDrawView::OnMouseMove(UINT uFlags, CPoint ptMouse)
{
  BOOL bLeftButtonDown = (uFlags & MK_LBUTTON);

  if (bLeftButtonDown)
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);

    dc.DPtoLP(&ptMouse);
    m_pDrawDoc->MouseDrag(ptMouse);
  }
}

void CDrawView::OnLButtonUp(UINT /* uFlags */, CPoint ptMouse)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  dc.DPtoLP(&ptMouse);
  m_pDrawDoc->MouseUp();
}

void CDrawView::OnLButtonDblClk(UINT uFlags, CPoint ptMouse)
{
  BOOL bControlKeyDown = (uFlags & MK_CONTROL);

  if (!bControlKeyDown)
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);

    dc.DPtoLP(&ptMouse);
    m_pDrawDoc->DoubleClick(ptMouse);
  }
}

// OnKeyDown is called when the user presses a key at the keyboard.

void CDrawView::OnKeyDown(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags */)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  switch (uChar)
  {
    // On home, we scroll to the top left corner.

    case VK_HOME:
      if (!m_pDrawDoc->KeyDown(VK_HOME, &dc))
      {
        OnVScroll(SB_TOP, 0, NULL);
        OnHScroll(SB_LEFT, 0, NULL);
      }
      break;

    // On end, we scroll to bottom right corner.

    case VK_END:
      if (!m_pDrawDoc->KeyDown(VK_END, &dc))
      {
        OnVScroll(SB_BOTTOM, 0, NULL);
        OnHScroll(SB_RIGHT, 0, NULL);
      }
      break;

    // The up and down arrow keys scroll one row.

    case VK_UP:
      OnVScroll(SB_LINEUP, 0, NULL);
      break;

    case VK_DOWN:
      OnVScroll(SB_LINEDOWN, 0, NULL);
      break;

    // In case the left or right arrow key was not used to edit a text, we
    // scroll one line.

    case VK_LEFT:
      if (!m_pDrawDoc->KeyDown(VK_LEFT, &dc))
      {
        OnHScroll(SB_LINELEFT, 0, NULL);
      }
      break;

    case VK_RIGHT:
      if (!m_pDrawDoc->KeyDown(VK_RIGHT, &dc))
      {
        OnHScroll(SB_LINERIGHT, 0, NULL);
      }
      break;

    // In case of the page up or page down key, we just scroll the bars.

    case VK_PRIOR:
      OnVScroll(SB_PAGEUP, 0, NULL);
      break;

    case VK_NEXT:
      OnVScroll(SB_PAGEDOWN, 0, NULL);
      break;

    // In case of the insert, delete, backspace, return, or escape key,
    // we call KeyDown in the document class.

    case VK_INSERT:
    case VK_DELETE:
    case VK_BACK:
    case VK_RETURN:
    case VK_ESCAPE:
      m_pDrawDoc->KeyDown(uChar, &dc);
      break;
  }
}

// OnChar is called when the user presses a printable character (ASCII table
// value 32 – 122). It calls CharDown in the document class.

void CDrawView::OnChar(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags */)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  m_pDrawDoc->CharDown(uChar, &dc);
}

// OnSetFocus and OnKillFocus are called when the input view gains or looses
// the focus. They notify the caret object about the change.

void CDrawView::OnSetFocus(CWnd* /* pOldWnd */)
{
  Caret* pCaret = m_pDrawDoc->GetCaret();
  pCaret->OnSetFocus(this);
}

void CDrawView::OnKillFocus(CWnd* /* pNewWnd */)
{
  Caret* pCaret = m_pDrawDoc->GetCaret();
  pCaret->OnKillFocus();
}

// OnSetCursor asks the document class about the current cursor.

BOOL CDrawView::OnSetCursor(CWnd* /* pWnd */, UINT /* uHitTest */,
                            UINT /* uMmessage */)
{
  HCURSOR hCursor = m_pDrawDoc->GetCursor();
  SetCursor(hCursor);

  return TRUE;
}

// OnUpdate is indirectly called by UpdateAllViews in the document class. Its
// second parameter is a pointer to the area to be repainted, which is given
// device coordinates that need to be translated into logical coordinates.

void CDrawView::OnUpdate(CView* /* pSender */, LPARAM lHint,
                         CObject* /* pHint */)
{
  // If lHint is not null, it is a pointer to a rectangle containing the area
  // to update.

  if (lHint != NULL)
  {
    CRect rcClip = *(CRect*) lHint;

    // We add a small margin.

    int cxMargin = (int) (0.05 * rcClip.Width());
    int cyMargin = (int) (0.05 * rcClip.Height());

    rcClip.left   -= cxMargin;
    rcClip.right  += cxMargin;
    rcClip.top    -= cyMargin;
    rcClip.bottom += cyMargin;

    // We need to translate the rectangle to device coordinates before we
    // invalidate it.

    CClientDC dc(this);
    OnPrepareDC(&dc);
    dc.LPtoDP(rcClip);

    InvalidateRect(rcClip);
  }

  // OnUpdate is also called by OnIntialUpdate in CScrollView with a zero
  // hint, in which case we invalidate the whole client area.

  else
  {
    Invalidate();
  }

  // Finally, we update the invalidated area by calling UpdateWindow, which in
  // turn indirectly calls OnDraw.

  UpdateWindow();
}

// OnDraw traverses the figure list and draws the figures. It does also
// draw the inside rectangle unless its pointer is null.

void CDrawView::OnDraw(CDC* pDC)
{
  const FigurePointerList* pFigurePtrList =
                           m_pDrawDoc->GetFigurePtrList();

  for (POSITION position = pFigurePtrList->GetHeadPosition();
       position != NULL; pFigurePtrList->GetNext(position))
  {
    Figure* pFigure = pFigurePtrList->GetAt(position);
    CRect rcFigure = pFigure->GetArea();
    pFigure->Draw(pDC);
  }

  const RectangleFigure* pInsideRectangle =
                         m_pDrawDoc->GetInsideRectangle();
  if (pInsideRectangle != NULL)
  {
      pInsideRectangle->Draw(pDC);
  }
}
