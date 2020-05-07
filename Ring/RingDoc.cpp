// RingDoc.cpp : implementation of the CRingDoc class
//

#include "StdAfx.h"
#include "Ring.h"

#include "RingDoc.h"
//#include ".\ringdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRingDoc

IMPLEMENT_DYNCREATE(CRingDoc, CDocument)

BEGIN_MESSAGE_MAP(CRingDoc, CDocument)
  ON_COMMAND(ID_COLOR_WHITE, OnColorWhite)
  ON_COMMAND(ID_COLOR_GRAY, OnColorGray)
  ON_COMMAND(ID_COLOR_BLACK, OnColorBlack)
  ON_UPDATE_COMMAND_UI(ID_COLOR_WHITE, OnUpdateColorWhite)
  ON_UPDATE_COMMAND_UI(ID_COLOR_GRAY, OnUpdateColorGray)
  ON_UPDATE_COMMAND_UI(ID_COLOR_BLACK, OnUpdateColorBlack)
  ON_COMMAND(ID_COLOR_DIALOG, OnColorDialog)
END_MESSAGE_MAP()


// CRingDoc construction/destruction

CRingDoc::CRingDoc()
{
  m_nextColor = (COLORREF) AfxGetApp()->GetProfileInt(TEXT("Ring"), TEXT("Color"), WHITE);
}

CRingDoc::~CRingDoc()
{
  AfxGetApp()->WriteProfileInt(TEXT("Ring"), TEXT("Color"), m_nextColor);
}

BOOL CRingDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}




// CRingDoc serialization

void CRingDoc::Serialize(CArchive& ar)
{
  m_pointArray.Serialize(ar);
  m_colorArray.Serialize(ar);

  if (ar.IsStoring())
  {
    ar << m_nextColor;
  }
  
  if (ar.IsLoading())
  {
    ar >> m_nextColor;
  }
}


// CRingDoc diagnostics

#ifdef _DEBUG
void CRingDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CRingDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

void CRingDoc::MouseDown(CPoint point)
{
  m_pointArray.Add(point);
  m_colorArray.Add(m_nextColor);

  SetModifiedFlag(TRUE);
  UpdateAllViews(NULL);
}

// CRingDoc commands

void CRingDoc::OnColorWhite()
{
  m_nextColor = WHITE;
}

void CRingDoc::OnColorGray()
{
  m_nextColor = GRAY;
}

void CRingDoc::OnColorBlack()
{
  m_nextColor = BLACK;
}

void CRingDoc::OnUpdateColorWhite(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(m_nextColor == WHITE);
}

void CRingDoc::OnUpdateColorGray(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(m_nextColor == GRAY);
}

void CRingDoc::OnUpdateColorBlack(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(m_nextColor == BLACK);
}

void CRingDoc::OnColorDialog()
{
  CColorDialog colorDialog(m_nextColor);

  if (colorDialog.DoModal() == IDOK)
  {
    m_nextColor = colorDialog.GetColor();
  }
}
