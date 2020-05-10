const int HEADER_WIDTH = 1000;
const int HEADER_HEIGHT = 500;

const int COL_WIDTH = 4000;
const int ROW_HEIGHT = 1000;

const int TOTAL_WIDTH = HEADER_WIDTH + COLS * COL_WIDTH;
const int TOTAL_HEIGHT = HEADER_HEIGHT + ROWS * ROW_HEIGHT;

enum CalcState {CS_MARK, CS_EDIT};

class CCalcDoc : public CDocument
{
  protected:
    DECLARE_DYNCREATE(CCalcDoc)
    DECLARE_MESSAGE_MAP()
    CCalcDoc();

  public:
    virtual void Serialize(CArchive& archive);
    CellMatrix* GetCellMatrix() {return &m_cellMatrix;}

    int GetCalcStatus() {return m_eCalcStatus;}
    Caret* GetCaret() {return &m_caret;}

    Reference GetEdit() const {return m_rfEdit;}
    Reference GetFirstMark() const {return m_rfFirstMark;}
    Reference GetLastMark() const {return m_rfLastMark;}

    void RepaintEditArea();
    void RepaintMarkedArea();
    void RepaintSet(const ReferenceSet& referenceSet);

    void DoubleClick(Reference rfCell, CPoint ptMouse,
                     CDC* pDC);
    void MakeCellVisible(Reference rfCell);
    void MakeCellVisible(int iRow, int iCol);
    void UpdateCaret();

    void UnmarkAndMark(int iMinRow, int iMinCol,
                       int iMaxRow, int iMaxCol);

    void KeyDown(UINT uChar, CDC* pDC, BOOL bShiftKeyDown);
    void CharDown(UINT uChar, CDC* pDC);

    void LeftArrowKey(BOOL bShiftKeyDown);
    void RightArrowKey(BOOL bShiftKeyDown);
    void UpArrowKey(BOOL bShiftKeyDown);
    void DownArrowKey(BOOL bShiftKeyDown);
    void HomeKey(BOOL bShiftKeyDown);
    void EndKey(BOOL bShiftKeyDown);

    void DeleteKey(CDC* pDC);
    void BackspaceKey(CDC* pDC);

    afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);
    afx_msg void OnCopy();

    afx_msg void OnUpdateCut(CCmdUI *pCmdUI);
    afx_msg void OnCut();

    afx_msg void OnUpdatePaste(CCmdUI *pCmdUI);
    afx_msg void OnPaste();

    afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
    afx_msg void OnDelete();

    afx_msg void OnUpdateAlignmentHorizontalLeft
      (CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlignmentHorizontalCenter
      (CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlignmentHorizontalRight
      (CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlignmentHorizontalJustified
      (CCmdUI *pCmdUI);

    afx_msg void OnUpdateAlignmentVerticalTop(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlignmentVerticalCenter
                 (CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlignmentVerticalBottom
                 (CCmdUI *pCmdUI);

    void UpdateAlignment(Direction eDirection, Alignment
                         eAlignment, CCmdUI *pCmdUI);
    BOOL IsAlignment(Direction eDirection,
                     Alignment eAlignment);

    afx_msg void OnAlignmentHorizontalLeft();
    afx_msg void OnAlignmentHorizontalCenter();
    afx_msg void OnAlignmentHorizontalRight();
    afx_msg void OnAlignmentHorizontalJustified();

    afx_msg void OnAlignmentVerticalTop();
    afx_msg void OnAlignmentVerticalCenter();
    afx_msg void OnAlignmentVerticalBottom();

    void SetAlignment(Direction eDirection,
                      Alignment eAlignment);

    afx_msg void OnUpdateColorText(CCmdUI *pCmdUI);
    afx_msg void OnUpdateColorBackground(CCmdUI *pCmdUI);

    afx_msg void OnTextColor();
    afx_msg void OnBackgroundColor();
    void OnColor(int iColorType);

    afx_msg void OnUpdateFont(CCmdUI *pCmdUI);
    afx_msg void OnFont();

  private:
    Caret m_caret;

    CalcState m_eCalcStatus;
    KeyboardState m_eKeyboardState;

    int m_iInputIndex;
    Reference m_rfEdit, m_rfFirstMark, m_rfLastMark,
              m_rfMinCopy, m_rfMaxCopy;

    CellMatrix m_cellMatrix, m_copyMatrix;
    TSetMatrix m_tSetMatrix;
};
