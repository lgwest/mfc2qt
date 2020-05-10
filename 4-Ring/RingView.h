// RingView.h : interface of the CRingView class
//

//#pragma once

class CRingView : public CScrollView
{
protected: // create from serialization only
  CRingView();
  DECLARE_DYNCREATE(CRingView)

// Attributes
public:
  CRingDoc* GetDocument() const;

// Operations
public:

// Overrides
  public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
  virtual void OnInitialUpdate(); // called first time after construct
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
  virtual ~CRingView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in RingView.cpp
inline CRingDoc* CRingView::GetDocument() const
   { return reinterpret_cast<CRingDoc*>(m_pDocument); }
#endif

