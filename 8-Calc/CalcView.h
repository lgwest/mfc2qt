enum SpreadSheetArea {MS_ALL, MS_ROW, MS_COL, MS_SHEET};

class CCalcView : public CView
{
  protected:
    DECLARE_DYNCREATE(CCalcView)
    DECLARE_MESSAGE_MAP()

    CCalcView();

  public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnInitialUpdate();

    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnPrepareDC(CDC* pDC,
                             CPrintInfo* pInfo = NULL);

    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    void LogicalPointToSheetPoint(CPoint& ptPoint);
    void LogicalPointToSheetPoint(CRect& rcRect);

    void SheetPointToLogicalPoint(CPoint& ptPoint);
    void SheetPointToLogicalPoint(CRect& rcRect);

    void MakeCellVisible(CRect rcArea);
    BOOL IsCellVisible(int iRow, int iCol);

    afx_msg void OnVScroll(UINT nSBCode, UINT nPos,
                           CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos,
                           CScrollBar* pScrollBar);

  private:
    SpreadSheetArea GetMouseLocation(CPoint ptMouse,
                                     Reference& rcCell);

  public:
    afx_msg void OnLButtonDown(UINT uFlags, CPoint ptMouse);
    afx_msg void OnMouseMove(UINT uFlags, CPoint ptMouse);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint ptMouse);

    afx_msg void OnKeyDown(UINT uChar, UINT nRepCnt,
                           UINT uFlags);
    afx_msg void OnChar(UINT uChar, UINT nRepCnt,
                        UINT uFlags);

    virtual void OnUpdate(CView* pSender, LPARAM lHint,
                          CObject* pHint);
    virtual void OnDraw(CDC* pDC);

  private:
    CCalcDoc* m_pCalcDoc;
    BOOL m_bDoubleClick;
    Reference m_rfFirstCell;
};
