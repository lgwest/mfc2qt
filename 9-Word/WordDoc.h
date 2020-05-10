static const int PAGE_TOTALWIDTH = 21000;
static const int PAGE_TOTALHEIGHT = 29700;
static const int PAGE_MARGIN = 2500;

static const int PAGE_WIDTH = (PAGE_TOTALWIDTH - 2 * PAGE_MARGIN);
static const int PAGE_HEIGHT = (PAGE_TOTALHEIGHT - 2 * PAGE_MARGIN);

enum WordState {WS_EDIT, WS_MARK};
typedef CArray<Page> PageArray;

class CWordDoc : public CDocument
{
  private:
    DECLARE_DYNCREATE(CWordDoc)
    DECLARE_MESSAGE_MAP()
    CWordDoc();

  public:
    virtual ~CWordDoc();

  public:
    void Serialize(CArchive& archive);
    virtual BOOL OnNewDocument();

    ParagraphPtrArray* GetParagraphArray() {return &m_paragraphArray;}

    void KeyDown(UINT uChar, CDC* pDC);
    void ShiftKeyDown(UINT uChar, CDC* pDC);

  private:
    void EnsureEditStatus();
    void EnsureMarkStatus();

    void LeftArrowKey();
    void ShiftLeftArrowKey();

    void RightArrowKey();
    void ShiftRightArrowKey();

    void UpArrowKey();
    void ShiftUpArrowKey();

    void DownArrowKey();
    void ShiftDownArrowKey();

    void PageUpKey(CDC* pDC);
    void ShiftPageUpKey(CDC* pDC);

    void PageDownKey(CDC* pDC);
    void ShiftPageDownKey(CDC* pDC);

    void HomeKey();
    void ShiftHomeKey();

    void EndKey();
    void ShiftEndKey();

    void DeleteKey(CDC* pDC);
    void BackspaceKey(CDC* pDC);
    void ReturnKey(CDC* pDC);
    void InsertKey();

  public:
    void CharDown(UINT uChar, CDC* pDC);
    int GetPageNum() const {return (int) m_pageArray.GetSize();}

  private:
    int PointToParagraph(const CPoint& ptMouse) const;

  public:
    Position PointToChar(const CPoint& ptMouse) const;

    void MouseDown(const CPoint& ptMouse);
    void MouseDrag(const CPoint& ptMouse);
    void MouseUp();
    void DoubleClick();

    void MakeVisible();
    void UpdateCaret();

    void GetRepaintSet(RectSet& repaintSet, Position psFirst,
                            Position psLast);
    void DeleteText(RectSet& repaintSet, CDC* pDC, Position psFirst,
                    Position psLast);

    void UpdateParagraphAndPageArray();

    void SetDocumentHome();
    void SetDocumentEnd();

    afx_msg void OnUpdateAlignLeft(CCmdUI *pCmdUI);
    afx_msg void OnAlignLeft();

    afx_msg void OnUpdateAlignCenter(CCmdUI *pCmdUI);
    afx_msg void OnAlignRight();

    afx_msg void OnUpdateAlignRight(CCmdUI *pCmdUI);
    afx_msg void OnAlignCenter();

    afx_msg void OnUpdateAlignJustifed(CCmdUI *pCmdUI);
    afx_msg void OnAlignJustified();

    BOOL IsAlignment(Alignment eAlignment) const;
    void SetAlignment(Alignment eAlignment);

    afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);
    void ClearCopyArray();
    afx_msg void OnCopy();

    afx_msg void OnUpdateCut(CCmdUI *pCmdUI);
    afx_msg void OnCut();
    
    afx_msg void OnUpdatePaste(CCmdUI *pCmdUI);
    afx_msg void OnPaste();

    Font GetFont() const;
    afx_msg void OnFont();

    void OnSetFocus(CWordView* pView) {m_pView = pView;
                                        m_caret.OnSetFocus(pView);}
    void OnKillFocus() {m_pView = NULL; m_caret.OnKillFocus();}

    WordState GetWordStatus() {return m_eWordState;}
    Position GetFirstMarked() {return m_psFirstMark;}
    Position GetLastMarked() {return m_psLastMark;}

  private:
    WordState m_eWordState;
    KeyboardState m_eKeyboardState;

    CWordView* m_pView;
    Caret m_caret;

    ParagraphPtrArray m_paragraphArray, m_copyArray;
    PageArray m_pageArray;

    Position m_psEdit, m_psFirstMark, m_psLastMark;
    Font *m_pNextFont;
};
