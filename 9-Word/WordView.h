const int LINE_WIDTH = 500;
const int LINE_HEIGHT = 500;

class CWordView : public CView
{
  private:
    DECLARE_DYNCREATE(CWordView)
    DECLARE_MESSAGE_MAP()

    CWordView();

  public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    virtual void OnInitialUpdate();
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

    afx_msg void OnSize(UINT uType, int cxClient, int cyClient);

    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    afx_msg void OnVScroll(UINT nSBCode, UINT nPos,
                            CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos,
                            CScrollBar* pScrollBar);

    afx_msg void OnLButtonDown(UINT uFlags, CPoint ptMouse);
    afx_msg void OnMouseMove(UINT uFlags, CPoint ptMouse);
    afx_msg void OnLButtonUp(UINT uFlags, CPoint ptMouse);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint ptMouse);

    void MakeVisible(CRect rcArea);

    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnUpdate(CView* pSender, LPARAM lHint,
                          CObject* pHint);

    afx_msg void OnPaint();
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnDraw(CDC* pDC);

  private:
    CWordDoc* m_pWordDoc;
    BOOL m_bDoubleClick;
};
