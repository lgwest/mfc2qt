#include "StdAfx.h"
#include "Caret.h"

Caret::Caret()
 :m_pFocusView(NULL),
  m_bVisible(FALSE),
  m_rcCaret(0, 0, 0, 0)
{
  // Empty.
}

void Caret::SetAndShowCaret(const CRect rcCaret)
{
  m_rcCaret = rcCaret;

  CClientDC dc(m_pFocusView);
  m_pFocusView->OnPrepareDC(&dc);

  dc.LPtoDP(m_rcCaret);
  m_rcCaret.left = min(m_rcCaret.left, m_rcCaret.right - 1);

  if (m_rcCaret.left < 0)
  {
    m_rcCaret.right += abs(m_rcCaret.left);
    m_rcCaret.left = 0;
  }

  m_pFocusView->CreateSolidCaret(m_rcCaret.Width(), m_rcCaret.Height());
  m_pFocusView->SetCaretPos(m_rcCaret.TopLeft());

  m_bVisible = TRUE;
  m_pFocusView->ShowCaret();
}

void Caret::HideCaret()
{
  if (m_pFocusView != NULL)
  {
    m_bVisible = FALSE;
    ::DestroyCaret();
  }
}

void Caret::OnSetFocus(CView* pView)
{
  m_pFocusView = pView;

  if (m_bVisible)
  {
    m_pFocusView->CreateSolidCaret(m_rcCaret.Width(), m_rcCaret.Height());
    m_pFocusView->SetCaretPos(m_rcCaret.TopLeft());
    m_pFocusView->ShowCaret();
  }
}

void Caret::OnKillFocus()
{
  m_pFocusView = NULL;
  ::DestroyCaret();
}
