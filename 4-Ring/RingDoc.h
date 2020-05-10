// RingDoc.h : interface of the CRingDoc class
//

//#pragma once

#include <afxtempl.h>

typedef CArray<CPoint> PointArray;
typedef CArray<COLORREF> ColorArray;

static const COLORREF WHITE = RGB(255, 255, 255);
static const COLORREF GRAY = RGB(128, 128, 128);
static const COLORREF BLACK = RGB(0, 0, 0);

static const int RADIUS = 500;

class CRingDoc : public CDocument
{
protected: // create from serialization only
  CRingDoc();
  DECLARE_DYNCREATE(CRingDoc)

// Attributes
public:

// Operations
public:

// Overrides
  public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

// Implementation
public:
  virtual ~CRingDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  DECLARE_MESSAGE_MAP()

private:
  PointArray m_pointArray;
  ColorArray m_colorArray;

public:
  void MouseDown(CPoint point);

  PointArray& GetPointArray() {return m_pointArray;}
  ColorArray& GetColorArray() {return m_colorArray;}

private:
  COLORREF m_nextColor;

public:
  afx_msg void OnColorWhite();
  afx_msg void OnColorGray();
  afx_msg void OnColorBlack();
  afx_msg void OnUpdateColorWhite(CCmdUI *pCmdUI);
  afx_msg void OnUpdateColorGray(CCmdUI *pCmdUI);
  afx_msg void OnUpdateColorBlack(CCmdUI *pCmdUI);
  afx_msg void OnColorDialog();
};
