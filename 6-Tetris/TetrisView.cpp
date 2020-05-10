#include "StdAfx.h"

#include "Check.h"
#include "ColorGrid.h"
#include "Square.h"
#include "Figure.h"
#include "Tetris.h"
#include "TetrisDoc.h"
#include "TetrisView.h"

IMPLEMENT_DYNCREATE(CTetrisView, CView)

// The message map routes the WM_CREATE, WM_SIZE, WM_SETFOCUS, WM_KILLFOCUS,
// WM_TIMER, WM_KEYDOWN, and WM_TIMER messages to this class. They are handled
// by OnCreate, OnSize, OnSetFocus, OnKillFocus, OnTimer, and OnKeyDown.

BEGIN_MESSAGE_MAP(CTetrisView, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_TIMER()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CTetrisView::CTetrisView()
 :m_iColorStatus(COLOR),
  m_pTetrisDoc(NULL)
{
  // Empty.
}

// The field m_pTetrisDoc is a pointer to the document class object. The
// address is already stored in the CView field m_pDocument. That is, however,
// a pointer to a CDocument object, not a CTetrisDoc object. Therefore, for
// convenience we assign it to the CTetrisDoc pointer m_pTetrisDoc.

int CTetrisView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  // We check that the view has been correctly created.
  if (CView::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  m_pTetrisDoc = (CTetrisDoc*) m_pDocument;
  check(m_pTetrisDoc != NULL);
  ASSERT_VALID(m_pTetrisDoc);

  return 0;
}

// The game grid is dimensioned by the constants ROWS and COLS; g_iRowHeight
// and g_iColWidth (defined in Figure.h) stores the height and width in pixels
// of one square.

void CTetrisView::OnSize(UINT /* uType */,int iClientWidth, int iClientHeight)
{
  g_iRowHeight = iClientHeight / ROWS;
  g_iColWidth = (iClientWidth / 2) / COLS;
}

// The timer message is sent to the view object once per second (1,000
// milliseconds). The idea is that the game shall go one as long as the view
// is in focus, and take a break when it is not. Therefore, the timer is
// activated when the view gets focus and deactivated when it loses focus.
// When the timer is active, the view receives a timer message each second,
// and it notifies the document by calling its method Timer. As we only have
// one timer in this application, we do not have to check the identity of the
// timer.

void CTetrisView::OnSetFocus(CWnd* /* pOldWnd */)
{
  SetTimer(TIMER_ID, 1000, NULL);
}

void CTetrisView::OnKillFocus(CWnd* /* pNewWnd */)
{
  KillTimer(TIMER_ID);
}	

void CTetrisView::OnTimer(UINT /* iIDEvent */)
{
  m_pTetrisDoc->Timer();
}

// The users may move the current figure to the left and right with the left
// and right arrow keys. They may rotate the figure clockwise and
// anticlockwise with the up and down arrow key and they may drop it with the
// space key. OnKeyDown catches the messages and notifies the document class
// object by calling the of of the matching methods LeftArrowKey,
// RightArrowKey, UpArrowKey, DownArrowKey, and SpaceKey in the document
// class.

void CTetrisView::OnKeyDown(UINT uChar, UINT /* uRepCnt */, UINT /* uFlags */)
{
  switch (uChar)
  {
    case VK_LEFT:
      m_pTetrisDoc->LeftArrowKey();
      break;

    case VK_RIGHT:
      m_pTetrisDoc->RightArrowKey();
      break;

    case VK_UP:
      m_pTetrisDoc->UpArrowKey();
      break;

    case VK_DOWN:
      m_pTetrisDoc->DownArrowKey();
      break;

    case VK_SPACE:
      m_pTetrisDoc->SpaceKey();
      break;
  }
}

// OnUpdate is called by the system when the window needs to be updated; in
// that case, the parameters lHint and pHint are zero and the whole client
// area is repainted. However, this method is also indirectly called when the
// document class calls UpdateAllView. In that case, lHint has the value COLOR
// or GRAY, depending on whether the client area shall be repainted in color
// or in a gray scale.

// If pHint is non-zero, it stores the coordinates of the area to be
// repainted. The coordinates are given in grid coordinates that have to be
// translated into pixel coordinates before the area is invalidated. If pHint
// is zero, the whole client area is repainted.

// The method first call Invalidate or InvalidateRect to define the area to be
// repainted, then the call to UpdateWindow does the actual repainting by
// calling OnPaint in CView, which in turn calls OnDraw.

void CTetrisView::OnUpdate(CView* /* pSender */, LPARAM lHint, CObject* pHint)
{
  m_iColorStatus = (int) lHint;

  if (pHint != NULL)
  {
    CRect rcArea = *(CRect*) pHint;

    rcArea.left *= g_iColWidth;
    rcArea.right *= g_iColWidth;
    rcArea.top *= g_iRowHeight;
    rcArea.bottom *= g_iRowHeight;

    InvalidateRect(&rcArea);
  }

  else
  {
    Invalidate();
  }

  UpdateWindow();
}

// OnDraw is called when the client area needs to be repainted, by the system
// or by UpdateWindow in OnUpdate. It draws a vertical line in the middle of
// the client area, and then draws the game grid, the high score list, and the
// current figures.

void CTetrisView::OnDraw(CDC* pDC)
{
  CPen pen(PS_SOLID, 0, BLACK);
  CPen* pOldPen = pDC->SelectObject(&pen);

  pDC->MoveTo(COLS * g_iColWidth, 0);
  pDC->LineTo(COLS * g_iColWidth, ROWS * g_iRowHeight);

  DrawGrid(pDC);
  DrawScoreAndScoreList(pDC);
  DrawActiveAndNextFigure(pDC);

  pDC->SelectObject(&pOldPen);
}

// DrawGrid traverses through the game grid and paint each non-white square.
// If a square is not occupied, it has the color white and it not painted.

void CTetrisView::DrawGrid(CDC* pDC)
{
  const ColorGrid* pGrid = m_pTetrisDoc->GetGrid();

  for (int iRow = 0; iRow < ROWS; ++iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      COLORREF rfColor = pGrid->Index(iRow, iCol);

      if (rfColor != WHITE)
      {
        CBrush brush((m_iColorStatus == COLOR) ? rfColor :GrayScale(rfColor));
        CBrush* pOldBrush = pDC->SelectObject(&brush);
        DrawSquare(iRow, iCol, pDC);
        pDC->SelectObject(pOldBrush);
      }
    }
  }
}

// GrayScale returns the gray scale of the given color, which is obtained by
// mixing the average of the red, blue, and green component of the color.

COLORREF GrayScale(COLORREF rfColor)
{
  int iRed = GetRValue(rfColor);
  int iGreen = GetGValue(rfColor);
  int iBlue = GetBValue(rfColor);

  int iAverage = (iRed + iGreen + iBlue) / 3;
  return RGB(iAverage, iAverage, iAverage);
}

// When the players are playing the game, they have the score list presented
// at the right side of the client area. The high score list stores at most
// the ten top scores of the game. If the list is non-empty, it is written in
// the client area.

void CTetrisView::DrawScoreAndScoreList(CDC* pDC)
{
  CString stScore;
  int iScore = m_pTetrisDoc->GetScore();
  stScore.Format(TEXT("Score: %d."), iScore);
  pDC->TextOut((COLS + 1) * g_iColWidth, 4 * g_iRowHeight, stScore);

  const IntList* pScoreList = m_pTetrisDoc->GetScoreList();
  if (!pScoreList->IsEmpty())
  {
    pDC->TextOut((COLS + 1) * g_iColWidth, 6 * g_iRowHeight, TEXT("High Score:"));

    int iScoreNumber = 0;
    for (POSITION pos = pScoreList->GetHeadPosition();
         pos != NULL; pScoreList->GetNext(pos))
    {
      int iRow = iScoreNumber + 7;
      int iScore = pScoreList->GetAt(pos);

      CString stScore;
      stScore.Format(TEXT("%2d. %d"), ++iScoreNumber, iScore);

      pDC->TextOut((COLS + 1) * g_iColWidth, iRow * g_iRowHeight, stScore);
    }
  }
}

// The active figure (m_activeFigure) is the figure falling down on the game
// grid. The next figure (m_nextFigure) is the figure announced at the right
// side of the client area. In order for it to be painted at the right hand
// side, we alter the origin to the middle of the client area, and one row
// under the upper border by calling SetWindowOrg.

void CTetrisView::DrawActiveAndNextFigure(CDC* pDC)
{
  const Figure activeFigure = m_pTetrisDoc->GetActiveFigure();
  activeFigure.Draw(m_iColorStatus, pDC);

  const Figure nextFigure = m_pTetrisDoc->GetNextFigure();
  CPoint ptOrigin(-COLS * g_iColWidth, -g_iRowHeight);
  pDC->SetWindowOrg(ptOrigin);
  nextFigure.Draw(m_iColorStatus, pDC);
}
