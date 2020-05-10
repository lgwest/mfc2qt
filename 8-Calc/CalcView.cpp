#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"
#include "Font.h"
#include "Color.h"
#include "Caret.h"
#include "Check.h"

#include "Reference.h"
#include "SyntaxTree.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "CalcView.h"
#include "CalcDoc.h"
#include "Calc.h"

IMPLEMENT_DYNCREATE(CCalcView, CView)

BEGIN_MESSAGE_MAP(CCalcView, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()

  ON_WM_VSCROLL()
  ON_WM_HSCROLL()

  ON_WM_KEYDOWN()
  ON_WM_CHAR()

  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONUP()

  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()

  ON_WM_PAINT()
END_MESSAGE_MAP()

CCalcView::CCalcView()
 :m_bDoubleClick(FALSE),
  m_pCalcDoc(NULL)
{
  // Empty.
}

// OnCreate is called when view is created, but before it is visible. It
// sets the value of m_pCalcDoc, the pointer to the document object.
// Remember that an application may have several view objects, but only one
// document object.

int CCalcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  // We check that the view has been correctly created.
  if (CView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  m_pCalcDoc = (CCalcDoc*) m_pDocument;
  ASSERT_VALID(m_pCalcDoc);

  return 0;
}

// OnInitialUpdate is called when the view is first visible. It sets the
// scroll views. It is slightly complicated as we have to take the row and
// column headers into consideration.

void CCalcView::OnInitialUpdate()
{
  // First, we need to create and prepare a device context.

  CClientDC dc(this);
  OnPrepareDC(&dc);

  // We convert the device coordinates (pixels) of the client area to
  // logical coordinates (hundreds of millimeters).

  CRect rcClient;
  GetClientRect(rcClient);
  dc.DPtoLP(&rcClient);

  // The width and height of the client area that is to the cells disposal are
  // the size of the client area minus the size of the row and column headers.

  int iPageWidth = rcClient.right - HEADER_WIDTH;
  int iPageHeight = rcClient.bottom - HEADER_HEIGHT;

  // The size of the horizontal scroll bar is the size of the columns plus the
  // rest between the page and column width. This make sure the cells will fit
  // nicely into the client area.

  SCROLLINFO scrollInfo;
  scrollInfo.fMask = SIF_ALL;
  scrollInfo.nPos = 0;
  scrollInfo.nMin = 0;
  scrollInfo.nPage = rcClient.right - HEADER_WIDTH;
  scrollInfo.nMax = COLS * COL_WIDTH + iPageWidth % COL_WIDTH - 1;
  SetScrollInfo(SB_HORZ, &scrollInfo);

  // In the same way, the size of the scroll bar is the size of the rows plus
  // the rest between the page and row height.

  scrollInfo.fMask = SIF_ALL;
  scrollInfo.nPos = 0;
  scrollInfo.nMin = 0;
  scrollInfo.nPage = rcClient.bottom - HEADER_HEIGHT;
  scrollInfo.nMax = ROWS * ROW_HEIGHT + iPageHeight % ROW_HEIGHT - 1;
  SetScrollInfo(SB_VERT, &scrollInfo);

  // Finally, the caret needs to be updated.
  m_pCalcDoc->UpdateCaret();
}

// OnSize is called every time the user change the size of the window. It sets
// the scroll bars to reflect the new size.

void CCalcView::OnSize(UINT /* uType */, int cxClient, int cyClient)
{
  // First, we need to create and prepare a device context.

  CClientDC dc(this);
  OnPrepareDC(&dc);

  // We convert the device coordinates (pixels) of the client area to
  // logical coordinates (hundreds of millimeters).

  CRect rcClient(0, 0, cxClient, cyClient);
  dc.DPtoLP(&rcClient);

  // We set the horizontal and vertical scroll bars.

  SCROLLINFO scrollInfo;
  scrollInfo.fMask = SIF_PAGE;
  scrollInfo.nPage = rcClient.right - HEADER_WIDTH;
  SetScrollInfo(SB_HORZ, &scrollInfo);

  scrollInfo.fMask = SIF_PAGE;
  scrollInfo.nPage = rcClient.bottom - HEADER_HEIGHT;
  SetScrollInfo(SB_VERT, &scrollInfo);

  // Finally, the caret need to be updated.
  m_pCalcDoc->UpdateCaret();
}

// OnPrepareDC is called directly after a device context has been created. It
// sets the coordinate mapping of the application. The isotropic mode means
// that the horizontal and vertical units are equals (circles are round).
// GetDeviceCaps gives the size of the screen in pixels and millimeters. With
// that information, we set the logical unit to one hundreds of a millimeter.

void CCalcView::OnPrepareDC(CDC* pDC, CPrintInfo* /* pInfo */)
{
  pDC->SetMapMode(MM_ISOTROPIC);

  CSize szWindow(100 * pDC->GetDeviceCaps(HORZSIZE),
                 100 * pDC->GetDeviceCaps(VERTSIZE));
  CSize szViewport(pDC->GetDeviceCaps(HORZRES),
                   pDC->GetDeviceCaps(VERTRES));

  pDC->SetWindowExt(szWindow);
  pDC->SetViewportExt(szViewport);
}

// LogicalPointToSheetPoint translates a logical point to a logical point
// with regards to the scrollbars’ current positions.

void CCalcView::LogicalPointToSheetPoint(CPoint& ptPoint)
{
  ptPoint.x += GetScrollPos(SB_HORZ) - HEADER_WIDTH;
  ptPoint.y += GetScrollPos(SB_VERT) - HEADER_HEIGHT;
}

void CCalcView::LogicalPointToSheetPoint(CRect& rcRect)
{
  LogicalPointToSheetPoint(rcRect.TopLeft());
  LogicalPointToSheetPoint(rcRect.BottomRight());
}

// SheetPointToLogicalPoint translates a logical point with regards to the
// scrollbars’ current positions to a regular logical point; that is, a
// logical point without regards to the scroll bars.

void CCalcView::SheetPointToLogicalPoint(CPoint& ptPoint)
{
  ptPoint.x += HEADER_WIDTH - GetScrollPos(SB_HORZ);
  ptPoint.y += HEADER_HEIGHT - GetScrollPos(SB_VERT);
}

void CCalcView::SheetPointToLogicalPoint(CRect& rcRect)
{
  SheetPointToLogicalPoint(rcRect.TopLeft());
  SheetPointToLogicalPoint(rcRect.BottomRight());
}

// OnSetFocus and OnKillFocus are called when the view receives or loses
// the input focus, respectively. They notify the caret connected to the
// document about the event.

void CCalcView::OnSetFocus(CWnd* /* pOldWnd */)
{
  Caret* pCaret = m_pCalcDoc->GetCaret();
  pCaret->OnSetFocus(this);
}

void CCalcView::OnKillFocus(CWnd* /* pNewWnd */)
{
  Caret* pCaret = m_pCalcDoc->GetCaret();
  pCaret->OnKillFocus();
}

// IsCellVisible decided if the given cell is located in the part of the
// spreadsheet visible in the client area.

BOOL CCalcView::IsCellVisible(int iRow, int iCol)
{
  // With the scroll bar settings we find the first and last visible row and
  // column in the client area and compares them to the given row and column.

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_VERT, &scrollInfo, SIF_POS | SIF_PAGE);
  int iFirstVisibleRow = scrollInfo.nPos / ROW_HEIGHT;
  int iLastVisibleRow = iFirstVisibleRow +
                        scrollInfo.nPage / ROW_HEIGHT;

  GetScrollInfo(SB_HORZ, &scrollInfo, SIF_POS | SIF_PAGE);
  int iFirstVisibleCol = scrollInfo.nPos / COL_WIDTH;
  int iLastVisibleCol = iFirstVisibleCol +
                        scrollInfo.nPage / COL_WIDTH;

  return ((iRow >= iFirstVisibleRow) && (iRow <= iLastVisibleRow) &&
          (iCol >= iFirstVisibleCol) && (iCol <= iLastVisibleCol));
}

// OnVScroll is called when the user scrolls vertically directly with the
// mouse, or indirectly with the arrow, home, end, page up and down keys.

void CCalcView::OnVScroll(UINT uSBCode, UINT /* yThumbPos */,
                          CScrollBar* /* pScrollBar */)
{
  // First, we extract all information about the vertical scroll bar and
  // find the current scroll position and the current top row.

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_VERT, &scrollInfo);

  int yPos = scrollInfo.nPos;
  int iOldRow = yPos / ROW_HEIGHT;

  switch (uSBCode)
  {
    // We increase or decrease the line and check that the scroll position
    // has not excluded the scroll limits. The scroll position cannot be
    // less than zero and it cannot be greater than the height of the
    // spreadsheet minus the height of the client area.

    case SB_LINEUP:
      yPos = max(0, yPos - ROW_HEIGHT);
      break;

    case SB_LINEDOWN:
      yPos = min(yPos + ROW_HEIGHT, scrollInfo.nMax);
      break;

    // We scroll one page up or down. Note the difference between scrolling
    // one line. A line has always the same height (ROW_HEIGHT), but a page
    // is defined by the current size of the client are, excluding the
    // headers (scrollInfo.nMax)

    case SB_PAGEUP:
      yPos = max(0, yPos - (int) scrollInfo.nPage);
      break;

    case SB_PAGEDOWN:
      yPos = min(yPos + (int) scrollInfo.nPage, scrollInfo.nMax);
      break;

    // If the user drag the scroll thumb manually, we catch its new
    // position.

    case SB_THUMBPOSITION:
      yPos = scrollInfo.nTrackPos;
      break;
  }

  int iNewRow = (int) ((double) yPos / ROW_HEIGHT + 0.5);

  if (iOldRow != iNewRow)
  {
    SetScrollPos(SB_VERT, iNewRow * ROW_HEIGHT);

    CRect rcClient;
    GetClientRect(&rcClient);

    // We need a device context to transform the headers to device
    // coordinates.

    CClientDC dc(this);
    OnPrepareDC(&dc);

    CSize szHeader(HEADER_WIDTH, HEADER_HEIGHT);
    dc.LPtoDP(&szHeader);

    // We invalidate the client area excluding the header in device
    // coordinates.

    CRect rcUpdate(0, szHeader.cy, rcClient.right, rcClient.bottom);
    InvalidateRect(rcUpdate);

    // Finally, we update the caret as the scroll position has been altered.
    m_pCalcDoc->UpdateCaret();
  }
}

// Similar to OnVScroll, OnHScroll is called when the user scrolls
// horizontally directly with the mouse, or indirectly with the special keys.

void CCalcView::OnHScroll(UINT uSBCode, UINT /* xThumbPos */,
                          CScrollBar* /* pScrollBar */)
{
  // First, we extract all information about the vertical scroll bar and
  // find the current scroll position and the current top row.

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_HORZ, &scrollInfo);

  int xPos = scrollInfo.nPos;
  int iOldCol = xPos / COL_WIDTH;

  switch (uSBCode)
  {
    // We increase or decrease the line and check that the scroll position
    // has not excluded the scroll limits. The scroll position cannot be
    // less than zero and it cannot be greater than the width of the
    // spreadsheet minus the width of the client area.

    case SB_LINELEFT:
      xPos = max(0, xPos - COL_WIDTH);
      break;

    case SB_LINERIGHT:
      xPos = min(xPos + COL_WIDTH, scrollInfo.nMax - 1);
      break;

    // We scroll one page up or down. Note the difference between scrolling
    // one line. A line has always the same height (ROW_HEIGHT), but a page
    // is defined by the current size of the client are, excluding the
    // headers (scrollInfo.nMax)

    case SB_PAGELEFT:
      xPos = max(0, xPos - (int) scrollInfo.nPage);
      break;

    case SB_PAGERIGHT:
      xPos = min(xPos + (int) scrollInfo.nPage, scrollInfo.nMax - 1);
      break;

    // If the user drag the scroll thumb manually, we catch its new
    // position.

    case SB_THUMBPOSITION:
      xPos = scrollInfo.nTrackPos;
      break;
  }

  int iNewCol = (int) ((double) xPos / COL_WIDTH + 0.5);

  if (iOldCol != iNewCol)
  {
    SetScrollPos(SB_HORZ, iNewCol * COL_WIDTH);

    CRect rcClient;
    GetClientRect(&rcClient);

    CClientDC dc(this);
    OnPrepareDC(&dc);

    // We need a device context to transform the headers to devcie
    // coordinates.

    CSize szHeader(HEADER_WIDTH, HEADER_HEIGHT);
    dc.LPtoDP(&szHeader);

    // We invalidate the client area excluding the header in device
    // coordinates.

    CRect rcUpdate(szHeader.cx, 0, rcClient.right, rcClient.bottom);
    InvalidateRect(rcUpdate);

    // Finally, we update the caret as the scroll position has been altered.
    m_pCalcDoc->UpdateCaret();
  }
}

// GetMouseLocation takes a the position of a mouse click (in device
// coordinates) and returns one of four areas of the client window:
// the top left corner, the row header, the column header or a cell in the
// spread sheet.

SpreadSheetArea CCalcView::GetMouseLocation(CPoint ptMouse, Reference& rfCell)
{
  // We translate the mouse position into logical coordinates, but we have
  // yet to consider scroll positions.

  CClientDC dc(this);
  OnPrepareDC(&dc);
  dc.DPtoLP(&ptMouse);
  
  // Is the mouse if the top left header box?  

  if ((ptMouse.x <= HEADER_WIDTH) && (ptMouse.y <= HEADER_HEIGHT))
  {
    rfCell.SetRow(0);
    rfCell.SetCol(0);

    return MS_ALL;
  }

  // Or is it in the row header?

  else if (ptMouse.x <= HEADER_WIDTH)
  {
    LogicalPointToSheetPoint(ptMouse);

    rfCell.SetRow(min(ROWS - 1, ptMouse.y / ROW_HEIGHT));
    rfCell.SetCol(0);

    return MS_ROW;
  }

  // Or is it in the column header?

  else if (ptMouse.y <= HEADER_HEIGHT)
  {
    LogicalPointToSheetPoint(ptMouse);

    rfCell.SetRow(0);
    rfCell.SetCol(min(COLS - 1, ptMouse.x / COL_WIDTH));

    return MS_COL;
  }

  // If not, is has to be in the actual cell space.

  else
  {
    LogicalPointToSheetPoint(ptMouse);

    rfCell.SetRow(min(ROWS - 1, ptMouse.y / ROW_HEIGHT));
    rfCell.SetCol(min(COLS - 1, ptMouse.x / COL_WIDTH));

    return MS_SHEET;
  }
}

// When the user clicks the mouse, two messages are sent: WM_LBUTTONDOWN
// followed by WM_LBUTTONUP. In practice, however, it is almost impossible
// for the user to press and release the button without move the mouse some
// millimeter, which correspondent to a pixel. First, WM_LBUTTONis sent, which
// is caught by OnLButtonDown, then one or several WM_MOUSEMOVE messages are
// sent, which are caught by OnMouseMove, and finally WM_LBUTTONDOWN is sent,
// which is ignored in this application.

// When we catch the mouse click, we have to find out where it hit. We have
// five parts of the client area to examine: the top left corner, the row
// header, the column header, and the cell space. If users click in the
// top left corner, all cells in the spreadsheet is marked. If they click
// in the row header, the whole row is marked. If they click in the column
// header, the whole column is marked. If they click in a cell in the cell
// space, that particular cell is marked.

void CCalcView::OnLButtonDown(UINT /* uFlags */, CPoint ptMouse)
{
  m_bDoubleClick = FALSE;
  SpreadSheetArea eArea = GetMouseLocation(ptMouse, m_rfFirstCell);

  switch (eArea)
  {
    case MS_ALL:
      m_pCalcDoc->UnmarkAndMark(0, 0, ROWS - 1, COLS - 1);
      break;

    case MS_ROW:
      m_pCalcDoc->UnmarkAndMark(m_rfFirstCell.GetRow(), 0,
                                m_rfFirstCell.GetRow(), COLS - 1);
      break;

    case MS_COL:
      m_pCalcDoc->UnmarkAndMark(0, m_rfFirstCell.GetCol(),
                                ROWS - 1, m_rfFirstCell.GetCol());
      break;

    case MS_SHEET:
      m_pCalcDoc->UnmarkAndMark(m_rfFirstCell.GetRow(), m_rfFirstCell.GetCol(),
                                m_rfFirstCell.GetRow(), m_rfFirstCell.GetCol());
      break;
  };

  m_pCalcDoc->UpdateCaret();
}

// One important detail is the m_bDoubleClick field. It is set to false in
// OnLButtonDown above simply because the user has not yet double clicked.
// However, if the user double clicks and move the mouse, it shall be
// ignored. It is due to the fact that the user pressing and dragging the
// mouse causes one or more cell to become marked. However, when the user
// double clicks, the application shall enter edit mode. Therefore,
// m_bDoubleClick is set to true in OnLDoubleClick below.

void CCalcView::OnMouseMove(UINT uFlags, CPoint ptMouse)
{
  BOOL bLeftButtonDown = (uFlags & MK_LBUTTON);

  if (bLeftButtonDown && !m_bDoubleClick)
  {
    Reference rcCurrCell;
    SpreadSheetArea eArea = GetMouseLocation(ptMouse, rcCurrCell);

    switch (eArea)
    {
      case MS_ALL:
        m_pCalcDoc->UnmarkAndMark(0, 0, ROWS - 1, COLS - 1);
        break;

      case MS_ROW:
        m_pCalcDoc->UnmarkAndMark(rcCurrCell.GetRow(), 0,
                                  rcCurrCell.GetRow(), COLS - 1);
        break;

      case MS_COL:
        m_pCalcDoc->UnmarkAndMark(0, rcCurrCell.GetCol(),
                                  ROWS - 1, rcCurrCell.GetCol());
        break;

      case MS_SHEET:
        m_pCalcDoc->UnmarkAndMark(m_rfFirstCell.GetRow(), m_rfFirstCell.GetCol(),
                                  rcCurrCell.GetRow(), rcCurrCell.GetCol());
        break;
    }
  }
}

// OnLButtonDblClk is called when the user double clicks the left button on
// the mouse.

void CCalcView::OnLButtonDblClk(UINT /* nFlags */, CPoint ptMouse)
{
  m_bDoubleClick = TRUE;

  Reference rfCell;
  SpreadSheetArea eArea = GetMouseLocation(ptMouse, rfCell);

  if (eArea == MS_SHEET)
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);

    dc.DPtoLP(&ptMouse);
    LogicalPointToSheetPoint(ptMouse);

    m_pCalcDoc->DoubleClick(rfCell, ptMouse, &dc);
  }
}

// OnKeyDown is called every time the user presses a character, and in case of
// left or right arrow, page up and down, home and end, return, tabulator,
// insert, delete, or backspace, the document class object is notified. We do
// need to create a device context, as the character may alter the text of a
// cell, with the result of regenerating the caret array for that cell.

// We do also need to find out whether the shift key is pressed
// simultaneously. The Win32 API function GetKeyState returns a negative value
// if the shift key is pressed. It can also be called with the parameters
// VK_CONTROL (control key), and VK_MENU (menu key). If we need more specific
// information about which key on the keyboard is pressed, we can also use
// VK_LSHIFT (left shift key), VK_RSHIFT (right shift key), VK_LCONTROL (left
// control key), VK_RCONTROL (right control key), VK_LMENU (left menu key),
// VK_RMENU (right menu key).

void CCalcView::OnKeyDown(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags*/)
{
  switch (uChar)
  {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_HOME:
    case VK_END:
    case VK_RETURN:
    case VK_ESCAPE:
    case VK_TAB:
    case VK_INSERT:
    case VK_DELETE:
    case VK_BACK:
      CClientDC dc(this);
      OnPrepareDC(&dc);

      BOOL bShiftKeyDown = (::GetKeyState(VK_SHIFT) < 0);
      m_pCalcDoc->KeyDown(uChar, &dc, bShiftKeyDown);
      break;
  }
}

// OnChar is called every time the user presses a key at the keyboard. If
// the character is printable it notifies the document class. A character
// is printable if its ASCII value is between 32 and 122; that is, if it is
// letter, a digit, an arithmetic character, or a punctuation mark.

void CCalcView::OnChar(UINT uChar, UINT  /* uRepCnt */, UINT /* uFlags */)
{
  if (isprint(uChar))
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);
    m_pCalcDoc->CharDown(uChar, &dc);
  }
}

// MakeCellVisible is called by the document class on several occasions. Its
// task is to make the given area visible. 

void CCalcView::MakeCellVisible(CRect rcArea)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_HORZ, &scrollInfo);
  int xFirst = scrollInfo.nPos;
  int xPage = scrollInfo.nPage;
  int xLast = xFirst + xPage;

  // If the cell is to the left of the first visible cell, we scroll the
  // horizontal bar to the left side of the cell and update the window and
  // the caret.

  if (rcArea.left < xFirst)
  {
    SetScrollPos(SB_HORZ, rcArea.left);

    CRect rcUpdate(HEADER_WIDTH, 0, TOTAL_WIDTH, TOTAL_HEIGHT);
    dc.LPtoDP(rcUpdate);
    InvalidateRect(rcUpdate);
    UpdateWindow();

    m_pCalcDoc->UpdateCaret();
  }

  // If the cell is to the right of the last visible cell, we scroll the
  // horizontal bar to first visible cell added with the distance between
  // given cell and the last cell and update the window and the caret.

  if (rcArea.right > xLast)
  {
    int iDistance = rcArea.right - xLast;
    iDistance += COL_WIDTH - iDistance % COL_WIDTH;
    SetScrollPos(SB_HORZ, xFirst + iDistance);

    CRect rcUpdate(HEADER_WIDTH, 0, TOTAL_WIDTH, TOTAL_HEIGHT);
    dc.LPtoDP(rcUpdate);
    InvalidateRect(rcUpdate);
    UpdateWindow();

    m_pCalcDoc->UpdateCaret();
  }

  GetScrollInfo(SB_VERT, &scrollInfo);
  int yFirst = scrollInfo.nPos;
  int yPage = scrollInfo.nPage;
  int yLast = yFirst + yPage;

  // If the cell is above the top visible cell, we scroll the vertical
  // bar to the top side of the cell and update the window and the caret.

  if (rcArea.top < yFirst)
  {
    SetScrollPos(SB_VERT, rcArea.top);

    CRect rcUpdate(0, HEADER_HEIGHT, TOTAL_WIDTH, TOTAL_HEIGHT);
    dc.LPtoDP(rcUpdate);
    InvalidateRect(rcUpdate);
    UpdateWindow();

    m_pCalcDoc->UpdateCaret();
  }

  // If the cell is below the last visible cell, we scroll the vertical
  // bar to top visible cell added with the distance between given cell and
  // the bottom and update the window and the caret.

  if (rcArea.bottom > yLast)
  {
    int iDistance = rcArea.bottom - yLast;
    iDistance += ROW_HEIGHT - iDistance % ROW_HEIGHT;
    SetScrollPos(SB_VERT, yFirst + iDistance);

    CRect rcUpdate(0, HEADER_HEIGHT, TOTAL_WIDTH, TOTAL_HEIGHT);
    dc.LPtoDP(rcUpdate);
    InvalidateRect(rcUpdate);
    UpdateWindow();

    m_pCalcDoc->UpdateCaret();
  }
}

// OnUpdate is indirectly called by UpdateAllViews in the document class
// when one or more cells need to be repainted. It is called on two
// occasions; indirectly, by UpdateAllViews in the document class with
// lHint pointed to a CRect object holding the area to be updateed in the
// client area of the view. It is also called by OnInitialUpdate in the MFC
// class CView with lHint set to NULL. In that case, we do nothing.

void CCalcView::OnUpdate(CView* /* pSender */, LPARAM lHint,
                         CObject* /* pHint */)
{
  if (lHint != NULL)
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);

    CRect rcUpdate = *(CRect*) lHint;
    SheetPointToLogicalPoint(rcUpdate);
    dc.LPtoDP(&rcUpdate);
    InvalidateRect(&rcUpdate);
    UpdateWindow();
  }
}

// OnDraw is called when the view needs to be repainted, partly or
// completely. Several areas need to be repainted. The client area can be
// divided into five parts: the top left corner, the row header, the column
// header, the cell space, and the area out the spreadsheet.

void CCalcView::OnDraw(CDC* pDC)
{
  CRect rcClient;
  GetClientRect(&rcClient);
  pDC->DPtoLP(&rcClient);

  // The area outside the spreadsheet has light gray background color.
  CPen pen(PS_SOLID, 0, LIGHT_GRAY);
  CPen *pOldPen = pDC->SelectObject(&pen);

  // The area outside the spreadsheet has light gray background color.
  CBrush grayBrush(LIGHT_GRAY);
  CBrush *pOldBrush = pDC->SelectObject(&grayBrush);

  int iTotalWidth = HEADER_WIDTH + COLS * COL_WIDTH;
  int iTotalHeight = HEADER_HEIGHT + ROWS*ROW_HEIGHT;

  // The area outside the spread sheet.
  pDC->Rectangle(iTotalWidth, 0, rcClient.right, iTotalHeight);
  pDC->Rectangle(0, iTotalHeight, rcClient.right,rcClient.bottom);

  // The headers have white background color.
  CBrush whiteBrush(WHITE);
  pDC->SelectObject(&whiteBrush);

  // Top left corner of the spread sheet (the all button).
  pDC->Rectangle(0, 0, HEADER_WIDTH, HEADER_HEIGHT);

  // The row header of the spreadsheet.

  int xScrollPos = GetScrollPos(SB_HORZ);
  int yScrollPos = GetScrollPos(SB_VERT);

  int iStartRow = yScrollPos / ROW_HEIGHT;
  int iStartCol = xScrollPos / COL_WIDTH;

  for (int iRow = iStartRow; iRow < ROWS; ++iRow)
  {
    int yPos = iRow * ROW_HEIGHT;
    yPos += HEADER_HEIGHT - yScrollPos;

    CString stBuffer;
    stBuffer.Format(TEXT("%d"), iRow + 1);

    CRect rcHeader(0, yPos, HEADER_WIDTH, yPos + ROW_HEIGHT);
    pDC->Rectangle(&rcHeader);
    pDC->DrawText(stBuffer, &rcHeader, DT_SINGLELINE | HALIGN_CENTER |VALIGN_CENTER);
  }

  // The column header of the spreadsheet.

  for (int iCol = iStartCol; iCol < COLS; ++iCol)
  {
    int xPos = iCol * COL_WIDTH;
    xPos += HEADER_WIDTH - xScrollPos;

    CString stBuffer;
    stBuffer.Format(TEXT("%c"), (TCHAR) (TEXT('A') + iCol));

    CRect rcHeader(xPos, 0, xPos + COL_WIDTH, HEADER_HEIGHT);
    pDC->Rectangle(&rcHeader);
    pDC->DrawText(stBuffer, &rcHeader, DT_SINGLELINE | HALIGN_CENTER |VALIGN_CENTER);
  }

  pDC->SelectObject(pOldPen);
  pDC->SelectObject(pOldBrush);

  CPoint ptScroll(xScrollPos, yScrollPos);
  CSize szHeader(HEADER_WIDTH, HEADER_HEIGHT);
  pDC->SetWindowOrg(ptScroll - szHeader);

  int iCellStatus = m_pCalcDoc->GetCalcStatus();
  CellMatrix* pCellMatrix = m_pCalcDoc->GetCellMatrix();

  Reference rfEdit = m_pCalcDoc->GetEdit();
  Reference rfFirstMark = m_pCalcDoc->GetFirstMark();
  Reference rfLastMark = m_pCalcDoc->GetLastMark();

  // The cell space.

  int iMinRow = min(rfFirstMark.GetRow(), rfLastMark.GetRow());
  int iMaxRow = max(rfFirstMark.GetRow(), rfLastMark.GetRow());
  int iMinCol = min(rfFirstMark.GetCol(), rfLastMark.GetCol());
  int iMaxCol = max(rfFirstMark.GetCol(), rfLastMark.GetCol());

  for (int iRow = iStartRow; iRow < ROWS; ++iRow)
  {
    for (int iCol = iStartCol; iCol < COLS; ++iCol)
    {
      // The variables are initalized to to avoid compiler warnings.      
      BOOL bEdit = FALSE, bMark = FALSE;

      switch (iCellStatus)
      {
        // The cell is in edit mode if the application is in edit mode and
        // this particular cell is to be edited.

        case CS_EDIT:
          bEdit = (iRow == rfEdit.GetRow()) && (iCol == rfEdit.GetCol());
          bMark = FALSE;
          break;

        // The cell is in marked mode if the application is in mark mode
        // and the cell is inside the marked block.

        case CS_MARK:
          bEdit = FALSE;
          bMark = (iRow >= iMinRow) && (iRow <= iMaxRow) &&
                  (iCol >= iMinCol) && (iCol <= iMaxCol);
          break;
      }

      // The cell is drawn relative the top left corner of the cell.

      CPoint ptTopLeft(iCol * COL_WIDTH, iRow * ROW_HEIGHT);
      Cell* pCell = pCellMatrix->Get(iRow, iCol);
      pCell->Draw(ptTopLeft, bEdit, bMark, pDC);
    }
  }
}
