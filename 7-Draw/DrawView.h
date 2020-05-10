class CDrawDoc;

class CDrawView: public CScrollView
{
  private:
    DECLARE_DYNCREATE(CDrawView)
    DECLARE_MESSAGE_MAP()

    CDrawView();

  public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest,
                             UINT message);

    afx_msg void OnLButtonDown(UINT uFlags, CPoint ptMouse);
    afx_msg void OnMouseMove(UINT uFlags, CPoint ptMouse);
    afx_msg void OnLButtonUp(UINT uFlags, CPoint ptMouse);
    afx_msg void OnLButtonDblClk(UINT uFlags, CPoint ptMouse);

    afx_msg void OnChar(UINT uChar, UINT nRepCnt,UINT uFlags);
    afx_msg void OnKeyDown(UINT uChar, UINT nRepCnt,
                           UINT uFlags);

    void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    void OnDraw(CDC* pDC);

  private:
    CDrawDoc* m_pDrawDoc;
};
