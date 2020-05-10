#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"

#include "Line.h"
#include "Position.h"
#include "Paragraph.h"

#include "Page.h"
#include "WordView.h"
#include "WordDoc.h"
#include "Word.h"

IMPLEMENT_DYNCREATE(CWordView, CView)

BEGIN_MESSAGE_MAP(CWordView, CView)
  // As this application support printing as well as print preview, the print
  // messages are caught. 

  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

  ON_WM_CREATE()
  ON_WM_SIZE()

  ON_WM_VSCROLL()
  ON_WM_HSCROLL()

  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()

  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()

  ON_WM_PAINT()

  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()

  ON_WM_MOVE()
  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CWordView::CWordView()
 :m_pWordDoc(NULL),
  m_bDoubleClick(FALSE)
{
  // Empty.
}

// OnCreate is called after the view has been created but before it has been
// showed.

int CWordView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  // The pointer to the document class object is set and tested. Remember
  // that an application can have several views, but only one document.

  m_pWordDoc = (CWordDoc*) m_pDocument;
  ASSERT_VALID(m_pWordDoc);

  m_pWordDoc->OnSetFocus(this);
  return 0;
}

// OnInitialUpdate is called once after the view has been created and shown.
// Its task is to initialize the scroll bars.

void CWordView::OnInitialUpdate()
{
  SCROLLINFO scrollInfo;

  scrollInfo.fMask = SIF_RANGE | SIF_POS;
  scrollInfo.nPos = 0;
  scrollInfo.nMin = 0;
  scrollInfo.nMax = PAGE_WIDTH;
  SetScrollInfo(SB_HORZ, &scrollInfo);

  scrollInfo.fMask = SIF_RANGE | SIF_POS;
  scrollInfo.nPos = 0;
  scrollInfo.nMin = 0;
  scrollInfo.nMax = (m_pWordDoc->GetPageNum() * PAGE_HEIGHT) - 1;
  SetScrollInfo(SB_VERT, &scrollInfo);

  m_pWordDoc->UpdateCaret();
  CView::OnInitialUpdate();
}

// OnPrepareDC is called directly after a device context object has been
// created. Its task is to set the relation between the logical and device
// coordinates.

void CWordView::OnPrepareDC(CDC* pDC, CPrintInfo* /* pInfo */)
{
  // We choose the isotropic mode, this implies that the units are equal in
  // the horizontal and vertical directions (circles will be round).

  pDC->SetMapMode(MM_ISOTROPIC);

  // We call GetDeviceCaps to get the size of the screen in millimeters
  // (HORZSIZE and VERTSIZE) and in pixels (HORZRES and VERTRES).

  CSize szWindow(100 * pDC->GetDeviceCaps(HORZSIZE),
                 100 * pDC->GetDeviceCaps(VERTSIZE));
  CSize szViewport(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

  // We then set the screen to in hundreds of millimeters to correspond to
  // the screen in pixels. This gives that one logical unit is one hundred of
  // a millimeter.

  pDC->SetWindowExt(szWindow);
  pDC->SetViewportExt(szViewport);

  // We also set origin of the client area to be at the bottom left corner by
  // looking up the current positions of the scroll bars.

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_HORZ, &scrollInfo, SIF_POS);
  int xOrg = scrollInfo.nPos;

  GetScrollInfo(SB_VERT, &scrollInfo, SIF_POS);
  int yOrg = scrollInfo.nPos;

  pDC->SetWindowOrg(xOrg, yOrg);
}

// OnSize is called every time the user changes the size of the window. We
// look up the size of the client area and set the size of the horizontal and
// vertical scroll bars to reflect the size of the visible client area
// compared to the size of the whole document.

void CWordView::OnSize(UINT /* uType */, int cxClient, int cyClient)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  // We translate the size of the client area from device to logical units.

  CSize szClient(cxClient, cyClient);
  dc.DPtoLP(&szClient);

  // We set the size of a page at the scroll bars.

  SCROLLINFO scrollInfo;
  scrollInfo.fMask = SIF_PAGE;
  scrollInfo.nPage = szClient.cx;
  SetScrollInfo(SB_HORZ, &scrollInfo);

  scrollInfo.fMask = SIF_PAGE;
  scrollInfo.nPage = szClient.cy;
  SetScrollInfo(SB_VERT, &scrollInfo);
}

// OnSetFocus and OnKillFocus is called when the view receives and loses the
// input focus. They notify the document object about the change.

void CWordView::OnSetFocus(CWnd* /* pOldWnd */)
{
  m_pWordDoc->OnSetFocus(this);
}

void CWordView::OnKillFocus(CWnd* /* pNewWnd */)
{
  m_pWordDoc->OnKillFocus();
}

// OnVScroll is called every time the user scrolls the vertical bar. It is
// also called when the user presses some key, see OnKeyDown below. The scroll
// bar and the client area are updated due to the changes. In order to fast
// update the area, we call the ScrollWindow method. It moves a part of the
// window and repaints the area to be repainted.

void CWordView::OnVScroll(UINT uSBCode, UINT /* yThumbPos */,
                          CScrollBar* /* pScrollBar */)
{
  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_VERT, &scrollInfo);
  int yScrollPos = scrollInfo.nPos;

  switch (uSBCode)
  {
    // The top scroll position is always zero. The bottom position, however,
    // is decided by the size of the client area (scrollInfo.nPage) because 
    // the scroll position is the top position of the visible part of the
    // document.

    case SB_TOP:
      yScrollPos = 0;
      break;

    case SB_BOTTOM:
      yScrollPos = scrollInfo.nMax - scrollInfo.nPage + 1;
      break;

    case SB_LINEUP:
      yScrollPos -= LINE_HEIGHT;
      break;

    case SB_LINEDOWN:
      yScrollPos += LINE_HEIGHT;
      break;

    // Note the difference between scrolling a line and a page, the line is
    // of constant height (LINE_HEIGHT) while the page height depends on the
    // size of the client area (scrollInfo.nPage).

    case SB_PAGEUP:
        yScrollPos -= scrollInfo.nPage;
        break;

    case SB_PAGEDOWN:
      yScrollPos += scrollInfo.nPage;
      break;

    // When the user grabs and moves the scroll thumb, we can track the new
    // position with scrollInfo.nTrackPos.

    case SB_THUMBPOSITION:
      yScrollPos = scrollInfo.nTrackPos;
      break;
  }

  // We have to check that the new position does not exceed the limits of the
  // scroll thumb.

  yScrollPos = max(yScrollPos, 0);
  yScrollPos = min(yScrollPos, scrollInfo.nMax - (int) scrollInfo.nPage + 1);

  // If the scroll position has been altered, we scroll the window the altered
  // distance.

  if (yScrollPos != scrollInfo.nPos)
  {
    CSize szDistance(0, scrollInfo.nPos - yScrollPos);

    scrollInfo.fMask = SIF_POS;
    scrollInfo.nPos = yScrollPos;
    SetScrollInfo(SB_VERT, &scrollInfo);

    // We need to translate the distance into device coordinates before we
    // scroll the window.

    CClientDC dc(this);
    OnPrepareDC(&dc);
    dc.LPtoDP(&szDistance);

    ScrollWindow(0, szDistance.cy);
    UpdateWindow();
  }
}

// Also in the case of horizontal scrolling, we use ScrollWindow. Similar to
// the vertical case, it moves a part of the area and repaints the rest of
// the area.

void CWordView::OnHScroll(UINT uSBCode, UINT /* xThumbPos */,
                          CScrollBar* /* pScrollBar */)
{
  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_HORZ, &scrollInfo);
  int xScrollPos = scrollInfo.nPos;

  switch (uSBCode)
  {
    // The left scroll position is always zero. The right position, however,
    // is decided by the size of the client area (scrollInfo.nPage) because 
    // the scroll position is the top position of the visible part of the
    // document.
    case SB_LEFT:
      xScrollPos = 0;
      break;

    case SB_RIGHT:
      xScrollPos = scrollInfo.nMax - scrollInfo.nPage + 1;
      break;

    // Note the difference between scrolling a line and a page, the line is
    // of constant width (LINE_WIDTH) while the page width depends on the
    // size of the client area (scrollInfo.nPage).

    case SB_LINELEFT:
      xScrollPos -= LINE_WIDTH;
      break;

    case SB_LINERIGHT:
      xScrollPos += LINE_WIDTH;
      break;

    case SB_PAGELEFT:
      xScrollPos -= scrollInfo.nPage;
      break;

    case SB_PAGERIGHT:
      xScrollPos += scrollInfo.nPage;
      break;

    // When the user grabs and moves the scroll thumb, we can track the new
    // position with scrollInfo.nTrackPos.

    case SB_THUMBPOSITION:
      xScrollPos = scrollInfo.nTrackPos;
      break;
  }

  // We have to check that the new position does not exceed the limits of the
  // scroll thumb.

  xScrollPos = max(xScrollPos, 0);
  xScrollPos = min(xScrollPos, scrollInfo.nMax - (int) scrollInfo.nPage + 1);

  // If the scroll position has been altered, we scroll the window the altered
  // distance.

  if (xScrollPos != scrollInfo.nPos)
  {
    CSize szDistance(scrollInfo.nPos - xScrollPos, 0);

    scrollInfo.fMask = SIF_POS;
    scrollInfo.nPos = xScrollPos;
    SetScrollInfo(SB_HORZ, &scrollInfo);

    // We need to translate the distance into device coordinates before we
    // scroll the window.

    CClientDC dc(this);
    OnPrepareDC(&dc);
    dc.LPtoDP(&szDistance);

    ScrollWindow(szDistance.cx, 0);
    UpdateWindow();
  }
}

// OnLButtonDown is called every time the user presses the left button of the
// mouse. The position of the mouse is given in device units that have to be
// translated into logical units. For that, we need a device context. It is
// prepared, that is modified with respect to the scroll bars current position
// and then used to translate the device units into logical units. Finally,
// the document object is notified.

void CWordView::OnLButtonDown(UINT /* uFlags */, CPoint ptMouse)
{
  m_bDoubleClick = FALSE;

  CClientDC dc(this);
  OnPrepareDC(&dc);
  dc.DPtoLP(&ptMouse);

  m_pWordDoc->MouseDown(ptMouse);
}

// OnMouseMove first checks whether the users move the mouse as the same time
// as they presses the left button. It is called mouse dragging and that is
// the only movement that interests us. We must also check that the user has
// not double clicked. In that case, there is a possibility that a word is
// marked by now, and if we allow the document to deal with this movement, the
// word will be partly unmarked.

void CWordView::OnMouseMove(UINT uFlags, CPoint ptMouse)
{
  BOOL bLeftButtonDown = (uFlags & MK_LBUTTON);

  if (bLeftButtonDown && !m_bDoubleClick)
  {
    CClientDC dc(this);
    OnPrepareDC(&dc);
    dc.DPtoLP(&ptMouse);

    m_pWordDoc->MouseDrag(ptMouse);
  }
}

// OnLButtonUp is called when the user releases the mouse button, and it calls
// MouseUp in the document class.

void CWordView::OnLButtonUp(UINT /* uFlags */, CPoint /* ptMouse */)
{
  m_pWordDoc->MouseUp();
}

// OnLButtonDblClk is called when the user double clicks. It set the double
// click field and calls Double Click in the document class.

void CWordView::OnLButtonDblClk(UINT /* nFlags */, CPoint /* ptMouse */)
{
  m_bDoubleClick = TRUE;
  m_pWordDoc->DoubleClick();
}

// MakeVisible makes sure the given area is visible in the window. If
// necessary, it moves the scroll bar positions.

void CWordView::MakeVisible(CRect rcArea)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  // We find the size of the document in device units. We will if necessary
  // update the area of the document, not the whole client area, in order to
  // avoid the gray area to the right of the document being updated.

  int iPageNum = m_pWordDoc->GetPageNum();
  CRect rcDocument(0, 0, PAGE_WIDTH, iPageNum * PAGE_HEIGHT);
  dc.LPtoDP(rcDocument);

  // We find the first and last position of the visible part of the document
  // in the x direction.

  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_HORZ, &scrollInfo);
  int xFirst = scrollInfo.nPos;
  int xPage = scrollInfo.nPage;
  int xLast = xFirst + xPage;

  // If the given area is to the left of the visible part of the client area,
  // we simple change the scroll position.

  if (rcArea.left < xFirst)
  {
    SetScrollPos(SB_HORZ, rcArea.left);
    InvalidateRect(rcDocument);
    UpdateWindow();
  }

  // If the given area is to the right of the visible part of the client area,
  // we also change the scroll position. We need to add a distance to the
  // scroll bar position. In order to make that distance as small as possible,
  // we take the difference between the right corner of the given area and the
  // client area.

  if (rcArea.right > xLast)
  {
    SetScrollPos(SB_HORZ, xFirst + (rcArea.right - xLast));
    InvalidateRect(rcDocument);
    UpdateWindow();
  }

  // The vertical scroll bar is changed in a way similar to the horizontal bar
  // above.

  GetScrollInfo(SB_VERT, &scrollInfo);
  int yFirst = scrollInfo.nPos;
  int yPage = scrollInfo.nPage;
  int yLast = yFirst + yPage;

  if (rcArea.top < yFirst)
  {
    SetScrollPos(SB_VERT, rcArea.top);
    InvalidateRect(rcDocument);
    UpdateWindow();
  }

  if (rcArea.bottom > yLast)
  {
    SetScrollPos(SB_VERT, yFirst + (rcArea.bottom - yLast));
    InvalidateRect(rcDocument);
    UpdateWindow();
  }
}

// OnKeyDown is called every time the user presses a key. The application
// behaves differently if the shift or control key is pressed at the same
// time, so first we have to decide whether they are pressed by calling the
// Win32 function GetKeyState, and it returns a value less than zero if the
// given key is pressed.

// When the control key is pressed, the view is be scrolled without
// notification to the document object by calling OnVScroll or OnHScroll.
// Otherwise, one of the document class methods KeyDown and ShiftKeyDown is
// called, depending on whether the shift key was pressed.

void CWordView::OnKeyDown(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags */)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);

  BOOL bShiftKeyDown = (::GetKeyState(VK_SHIFT) < 0);
  BOOL bControlKeyDown = (::GetKeyState(VK_CONTROL) < 0);

  if (bControlKeyDown)
  {
    switch (uChar)
    {
      case VK_PRIOR:
        OnVScroll(SB_PAGEUP, 0, NULL);
        break;

      case VK_NEXT:
        OnVScroll(SB_PAGEDOWN, 0, NULL);
        break;

      case VK_UP:
        OnVScroll(SB_LINEUP, 0, NULL);
        break;

      case VK_DOWN:
        OnVScroll(SB_LINEDOWN, 0, NULL);
        break;

      case VK_LEFT:
        OnHScroll(SB_LINELEFT, 0, NULL);
        break;

      case VK_RIGHT:
        OnHScroll(SB_LINERIGHT, 0, NULL);
        break;

      // If the visible part of the document is not already located at the top
      // left position, we set the scroll position and update the window.

      case VK_HOME:
        m_pWordDoc->SetDocumentHome();
        /*
        if ((GetScrollPos(SB_HORZ) > 0) || (GetScrollPos(SB_VERT) > 0))
        {
          SetScrollPos(SB_HORZ, 0);
          SetScrollPos(SB_VERT, 0);
          Invalidate();
          UpdateWindow();
        }
        */
        break;

        // If the visible part of the document is not already located at the
      // right bottom position, we set the scroll position and update the
      // window.

      case VK_END:
        m_pWordDoc->SetDocumentEnd();
        /*
        {
          SCROLLINFO scrollInfo;
          GetScrollInfo(SB_HORZ, &scrollInfo, SIF_PAGE | SIF_RANGE);
          int xNewPos = scrollInfo.nMax - scrollInfo.nPage;

          GetScrollInfo(SB_VERT, &scrollInfo, SIF_PAGE | SIF_RANGE);
          int yNewPos = scrollInfo.nMax - scrollInfo.nPage;

          if ((GetScrollPos(SB_HORZ) != xNewPos) ||
              (GetScrollPos(SB_VERT) != yNewPos))
          {
            SetScrollPos(SB_HORZ, xNewPos);
            SetScrollPos(SB_VERT, yNewPos);
            Invalidate();
            UpdateWindow();
          }
        }
        */
        break;

      // The rest of the characters are sent to the document object.

      default:
        m_pWordDoc->KeyDown(uChar, &dc);
        break;
      }
  }

  // If the control key is not pressed, we send the key to the document
  // object.
  
  else if (bShiftKeyDown)
  {
    m_pWordDoc->ShiftKeyDown(uChar, &dc);
  }

  else
  {
    m_pWordDoc->KeyDown(uChar, &dc);
  }
}

// OnChar is called when the user presses a regular character (ascii 32 –
// 122). It calls CharDown in the document class.

void CWordView::OnChar(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags */)
{
  CClientDC dc(this);
  OnPrepareDC(&dc);
  m_pWordDoc->CharDown(uChar, &dc);
}

// OnUpdate is called indirectly by the document class when it calls
// UpdateAllViews. It takes two parameters, lHint and pHint, that are used
// to update the vertical scroll bar when the number of pages has been changed
// and to partly repaint the view when the document text has been changed.

void CWordView::OnUpdate(CView* /* pSender */, LPARAM lHint, CObject* pHint)
{
  // If lHint is not zero the number of pages has been changed and we change
  // the range of the vertical scroll bar. Note that the limits of the
  // horizontal scroll bar never change as the width of the document is
  // constant (stored in PAGE_WIDTH).

  if (lHint != 0)
  {
    int iPages = (int) lHint;
    SetScrollRange(SB_VERT, 0, iPages * PAGE_HEIGHT);

    Invalidate();
    UpdateWindow();
  }

  // If pHint is not null the document needs to be repainted, pHint is a
  // pointer to an array of rectangles to be repainted. We translate them
  // into device units and repaint them. Finally, we update the window.

  else if (pHint != NULL)
  {
    RectSet* pRepaintSet = (RectSet*) pHint;

    if (!pRepaintSet->IsEmpty())
    {
      CClientDC dc(this);
      OnPrepareDC(&dc);

      for (POSITION position = pRepaintSet->GetHeadPosition();
           position != NULL; pRepaintSet->GetNext(position))
      {
        CRect rcRepaint = pRepaintSet->GetAt(position);
        dc.LPtoDP(&rcRepaint);
        InvalidateRect(rcRepaint);
      }

      UpdateWindow();
    }
  }

  // If lHint is zero and pHint is null, the window has just been
  // created. OnUpdate is indirectly called by OnInitialUpdate.
  // In that case, we just repaint the whole client area.

  else
  {
    Invalidate();
    UpdateWindow();
  }
}

// OnPaint is called by the system every time the client area of the window
// needs to be (partly or completely) repainted or when the client area is
// repainted and UpdateWindow is called. In the applications of the earlier
// chapters of this book, OnDraw was called instead. In those cases, OnPaint
// in CView was called, which in turns called OnDraw. In this application,
// there is a difference between whether the text shall be written in a window
// on the screen of sent to a printer (or print preview). In the case of
// printing, see OnPrint below.

// When a document is to be written to the client area of a window, if it is
// constituted by more than one page, and we add page break markers to the
// text. When it is done, we call OnDraw to do the writing of the actual
// text.

// OnPaint has two tasks before if finally calls OnDraw. First, we need to
// fill the area to the right of the document, if any (rcClient.right >
// PAGE_WIDTH). We do that by loading a brush with a light gray color and draw
// a rectangle at the right of the document. Note that we do not need to fill
// any space below the document as the vertical scroll bar is set to match the
// height of the document.

// Second, we mark pages breaks in case the document is constituted by more
// than one page (iPageNum > 1). A document does always have at least one
// paragraph and one page, it is never completely empty. We pick a black pen
// and set the text output centered on the x position. The x position is in
// the middle of the document or in the middle of the client area, whatever is
// least. The y position is the page height for each page.

void CWordView::OnPaint()
{
  CPaintDC dc(this);
  OnPrepareDC(&dc);

  CRect rcClient;
  GetClientRect(&rcClient);
  dc.DPtoLP(&rcClient);

  // If the client area is wider than the document, we paint it with light
  // gray color.

  if (rcClient.right > PAGE_WIDTH)
  {
    CBrush brush(LIGHT_GRAY);
    CBrush *pOldBrush = dc.SelectObject(&brush);

    dc.Rectangle(PAGE_WIDTH, 0, rcClient.right, rcClient.bottom);
    dc.SelectObject(pOldBrush);
  }

  // If the client area is higher than the document, we also paint it with
  // light gray color.

  int iPageNum = m_pWordDoc->GetPageNum();

  if (rcClient.bottom > (iPageNum * PAGE_HEIGHT))
  {
    CBrush brush(LIGHT_GRAY);
    CBrush *pOldBrush = dc.SelectObject(&brush);

    dc.Rectangle(iPageNum * PAGE_HEIGHT, 0, rcClient.right, rcClient.bottom);
    dc.SelectObject(pOldBrush);
  }

  if (iPageNum > 1)
  {
    dc.SetTextColor(BLACK);
    dc.SetTextAlign(TA_CENTER | TA_BASELINE);

    int xPos = min(PAGE_WIDTH / 2, (rcClient.left + rcClient.right) / 2);
    for (int iPage = 1; iPage < iPageNum; ++iPage)
    {
      int yPos = iPage * PAGE_HEIGHT;
      dc.TextOut(xPos, yPos, TEXT("-- Page Break --"));
    }

    dc.SetTextAlign(TA_LEFT | TA_TOP);
  }

  // In order to not write character outside the page, we clip the writing
  // area to match the document before we call OnDraw to do the actual writing.

  dc.IntersectClipRect(0, 0, PAGE_WIDTH,
                       max(1, iPageNum) * PAGE_HEIGHT);
  OnDraw(&dc);
}

// OnPreparePrinting is used to set the range of pages to print. The
// paragraphs of the document are spread over a number of pages. GetPageNum in
// the document class returns the number of pages.

BOOL CWordView::OnPreparePrinting(CPrintInfo* pInfo)
{
  pInfo->SetMinPage(1);
  pInfo->SetMaxPage(m_pWordDoc->GetPageNum());
  return DoPreparePrinting(pInfo);
}

// OnPrint is called by the Application Framework when the user chooses the
// file print menu item. First, OnPreparePrinting is called to decide the
// number of pages to be printed and then OnPrint is called once for each page
// to be printed. The task for OnPrint is to write the file name of the
// document on top of the page and the page number on bottom of the page as
// well as draw a rectangle around the text. Finally, OnDraw is called to
// write the actual text. Not that both OnPaint and OnPrint call OnDraw to
// draw the text of the document.

// First, we draw the surrounding rectangle. Second, we write the header with
// the path name of the document and the footer with the page number and the
// total number of pages. Moreover, we need to set the offset so that this
// page is printed of the first page, no matter which page it actually is.
// Finally, we need to exclude the merging from the draw area in order for the
// page not to write outside its area.

void CWordView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
  // One initial problem with OnPrint is pDC has already been set with
  // OnPrepareDC. We have to start with undoing that operation. After that,
  // OnPrint has several tasks before it finally calls OnDraw.

  int xScrollPos = GetScrollPos(SB_HORZ);
  int yScrollPos = GetScrollPos(SB_VERT);
  pDC->OffsetWindowOrg(-xScrollPos, -yScrollPos);

  // Next, we draw a rectangle around the text of each document. We define the 
  // border of that rectangle.

  int xLeft = PAGE_MARGIN / 2;
  int xRight = PAGE_TOTALWIDTH - PAGE_MARGIN / 2 - xLeft / 2;
  int yTop = PAGE_MARGIN / 2;
  int yBottom = PAGE_TOTALHEIGHT - PAGE_MARGIN / 2 - yTop / 2;

  // We select a black pen and draw the surrounding rectangle.

  CPen pen(PS_SOLID, 0, BLACK);
  CPen* pOldPen = pDC->SelectObject(&pen);
  pDC->Rectangle(xLeft, yTop, xRight, yBottom);
  pDC->SelectObject(pOldPen);

  // In order to write the header and footer of the document, we need to
  // select a font. If we create a font object with the default constructor,
  // the system font will be the result. It is often the Ariel font of size 10
  // points.

  CFont cFont;
  Font defaultFont;
  cFont.CreateFontIndirect(defaultFont.PointsToMeters());
  CFont* pPrevFont = pDC->SelectObject(&cFont);

  // We write the path name of the document at the top of the page.
  // GetPathName return the saved pathname, it returnr an empty string if the
  // document has not yet been saved.

  CString stPath = m_pWordDoc->GetPathName();
  CRect rcHeader(xLeft, 0, xRight, 2 * yTop);
  pDC->DrawText(stPath, rcHeader, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

  // Then we write the pages number together with total number of pages at
  // the bottom of the page.

  CString stPage;
  int iPageNum = pInfo->m_nCurPage - 1;
  stPage.Format(TEXT("Page %d of %d"), iPageNum + 1, m_pWordDoc->GetPageNum());
  CRect rcFooter(xLeft, PAGE_TOTALHEIGHT - 2 * (PAGE_TOTALHEIGHT - yBottom),
                 xRight, PAGE_TOTALHEIGHT);
  pDC->DrawText(stPage, rcFooter, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  pDC->SelectObject(pPrevFont);

  // Before we call OnDraw for writing the paragraphs, we have to redo the
  // setting of the window origin at the beginning of this method.

  int yPagePos = (iPageNum * PAGE_HEIGHT);
  pDC->OffsetWindowOrg(-PAGE_MARGIN, yPagePos - PAGE_MARGIN);

  // As OnDraw tries to write all paragraph (not only those on the current
  // page) we have to exclude the area of the document not on the current
  // page.

  CRect rcPage(0, iPageNum * PAGE_HEIGHT, PAGE_WIDTH,
               (iPageNum + 1) * PAGE_HEIGHT);
  pDC->IntersectClipRect(&rcPage);

  // Finally, we call OnDraw to do the actual writing of the paragraphs.
  OnDraw(pDC);
}

// OnDraw is called by both OnPaint and OnPrint to do the actual writing by
// calling Draw of each paragraph. One thing that complicates matter is that
// some portion of the text to be written could be marked. If the application
// is in edit mode, we just call Draw for each paragraph. If it is in mark
// mode, we have four possible cases for the current paragraph. The paragraph
// may be the only marked one; in that case, we call Draw with the indexes of
// the minimum and maximum character. The paragraph may the first of several
// marked paragraph. In that case, we mark to the end of the paragraph. It may
// be the last of several marked paragraph; in that case, we mark from the
// beginning. Finally, in may be not marked at all, then we just draw it.

void CWordView::OnDraw(CDC* pDC)
{
  int eWordStatus = m_pWordDoc->GetWordStatus();
  ParagraphPtrArray* pParagraphArray = m_pWordDoc->GetParagraphArray();

  Position psFirstMarked = m_pWordDoc->GetFirstMarked();
  Position psLastMarked = m_pWordDoc->GetLastMarked();

  Position psMinMarked = min(psFirstMarked, psLastMarked);
  Position psMaxMarked = max(psFirstMarked, psLastMarked);

  int iParagraphs = (int) pParagraphArray->GetSize();
  for (int iParagraph = 0; iParagraph < iParagraphs; ++iParagraph)
  {
    Paragraph* pParagraph = pParagraphArray->GetAt(iParagraph);

    switch (eWordStatus)
    {
      // If the application is in edit mode, we just write the paragraph.

      case WS_EDIT:
        pParagraph->Draw(pDC, 0, -1);
        break;

      // If the application is in mark mode, we have to check if the paragraph
      // is marked, partly or completely.

      case WS_MARK:
        int iLength = pParagraph->GetLength();

        // If this paragraph is the only one marked in the document, we write
        // it and dispatch the beginning and end of the marked area.

        if ((iParagraph == psMinMarked.Paragraph()) &&
            (iParagraph == psMaxMarked.Paragraph()))
        {
          pParagraph->Draw(pDC, psMinMarked.Character(), psMaxMarked.Character());
        }

        // If the paragraph is at the beginning of the marked area, we write
        // it and dispatch the beginning and end of the marked area. The end
        // of the marked area for this paragraph is the end of the paragraph.

        else if (iParagraph == psMinMarked.Paragraph())
        {
          pParagraph->Draw(pDC, psMinMarked.Character(), iLength);
        }

        // If the paragraph is completely inside the marked area, we write
        // it and dispatch its beginning and end of the paragraph as the
        // limits of the marked area.

        else if ((iParagraph > psMinMarked.Paragraph()) && 
                 (iParagraph < psMaxMarked.Paragraph()))
        {
          pParagraph->Draw(pDC, 0, iLength);
        }

        // If the paragraph is the end of the marked area, we write it and
        // dispatch the beginning and end of the marked area. The beginning of
        // the marked area for this paragraph is the beginning of the
        // paragraph.

        else if (iParagraph == psMaxMarked.Paragraph())
        {
          pParagraph->Draw(pDC, 0, psMaxMarked.Character());
        }

        // If the paragraph is not marked at all, we just write it.

        else
        {
          pParagraph->Draw(pDC, 0, -1);
        }
        break;
    }
  }
}
