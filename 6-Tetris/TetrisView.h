const int TIMER_ID = 0;
enum {COLOR = 0, GRAY = 1};

class CTetrisDoc;
COLORREF GrayScale(COLORREF rfColor);

class CTetrisView : public CView
{
  protected:
    CTetrisView();

    DECLARE_DYNCREATE(CTetrisView)
    DECLARE_MESSAGE_MAP()

  public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int iClientWidth,
                        int iClientHeight);
    
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt,
                           UINT nFlags);
    afx_msg void OnTimer(UINT nIDEvent);

    void OnUpdate(CView* /* pSender */, LPARAM lHint,
                  CObject* pHint);
    void OnDraw(CDC* pDC);

  private:
    void DrawGrid(CDC* pDC);

    void DrawScoreAndScoreList(CDC* pDC);
    void DrawActiveAndNextFigure(CDC* pDC);

  private:
    CTetrisDoc* m_pTetrisDoc;
    int m_iColorStatus;
};
