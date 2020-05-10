// RingView.cpp : implementation of the CRingView class
//

#include "StdAfx.h"
#include <cassert>
#include "Ring.h"

#include "RingDoc.h"
#include "RingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRingView

IMPLEMENT_DYNCREATE(CRingView, CScrollView)

BEGIN_MESSAGE_MAP(CRingView, CScrollView)
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
  ON_WM_LBUTTONDOWN()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CRingView construction/destruction

CRingView::CRingView()
{
  // TODO: add construction code here

}

CRingView::~CRingView()
{
}

BOOL CRingView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CScrollView::PreCreateWindow(cs);
}

// CRingView drawing

void CRingView::OnDraw(CDC* pDC)
{
  CRingDoc* pDoc = GetDocument();
  assert(pDoc != NULL);
  ASSERT_VALID(pDoc);

  if (!pDoc)
    return;

  PointArray& pointArray = pDoc->GetPointArray();
  ColorArray& colorArray = pDoc->GetColorArray();

  int iSize = (int) pointArray.GetSize();
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    CPoint point = pointArray[iIndex];
    COLORREF color = colorArray[iIndex];

    CPen pen(PS_SOLID, 0, BLACK);
    CBrush brush(color);

    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Ellipse(point.x - RADIUS, point.y - RADIUS,
                 point.x + RADIUS, point.y + RADIUS);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}

void CRingView::OnInitialUpdate()
{
  CScrollView::OnInitialUpdate();

  CSize sizeLine(500, 500);
  CSize sizePage(5000, 5000);
  CSize sizeTotal(29000, 20700);
  SetScrollSizes(MM_HIMETRIC, sizeTotal, sizePage, sizeLine);
}


// CRingView printing

BOOL CRingView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CRingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void CRingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}


// CRingView diagnostics

#ifdef _DEBUG
void CRingView::AssertValid() const
{
  CScrollView::AssertValid();
}

void CRingView::Dump(CDumpContext& dc) const
{
  CScrollView::Dump(dc);
}

CRingDoc* CRingView::GetDocument() const // non-debug version is inline
{
  assert(m_pDocument->IsKindOf(RUNTIME_CLASS(CRingDoc)));
  return (CRingDoc*) m_pDocument;
}
#endif //_DEBUG


// CRingView message handlers

void CRingView::OnLButtonDown(UINT nFlags, CPoint point)
{
  CRingDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  CClientDC dc(this);
  OnPrepareDC(&dc);
  dc.DPtoLP(&point);

  pDoc->MouseDown(point);
  CScrollView::OnLButtonDown(nFlags, point);
}

void CRingView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  switch (nChar)
  {
    case VK_UP:
      OnVScroll(SB_LINEUP, 0, NULL);
      break;

    case VK_DOWN:
      OnVScroll(SB_LINEDOWN, 0, NULL);
      break;

    case VK_PRIOR:
      OnVScroll(SB_PAGEUP, 0, NULL);
      break;

    case VK_NEXT:
      OnVScroll(SB_PAGEDOWN, 0, NULL);
      break;

    case VK_LEFT:
      OnHScroll(SB_LINELEFT, 0, NULL);
      break;

    case VK_RIGHT:
      OnHScroll(SB_LINERIGHT, 0, NULL);
      break;

    case VK_HOME:
      OnHScroll(SB_LEFT, 0, NULL);
      break;

    case VK_END:
      OnHScroll(SB_RIGHT, 0, NULL);
      break;
  }

  CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
