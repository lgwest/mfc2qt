const int TOTAL_WIDTH = 29000, TOTAL_HEIGHT = 20700;

enum ApplicationState {SINGLE_DRAG, MULTIPLE_DRAG,
                       RECTANGLE_DRAG, EDIT_TEXT, IDLE};
enum NextActionState {ADD_LINE, ADD_ARROW, ADD_RECTANGLE,
                      ADD_ELLIPSE, ADD_TEXT, MODIFY_FIGURE};

class CDrawDoc: public CDocument
{
private:
  DECLARE_DYNCREATE(CDrawDoc)
  DECLARE_MESSAGE_MAP()

  CDrawDoc();
  ~CDrawDoc();

public:
  void Serialize(CArchive& ar);

  void MouseDown(CPoint ptMouse, BOOL bControlKeyDown,
    CDC* pDC);
  void MouseDrag(const CPoint& ptMouse);
  void MouseUp();

  void DoubleClick(const CPoint& ptMouse);
  BOOL KeyDown(UINT cChar, CDC* pDC);
  void CharDown(UINT cChar, CDC* pDC);

  const FigurePointerList* GetFigurePtrList() const
  {return &m_figurePtrList;}
  const RectangleFigure* GetInsideRectangle() const
  {return m_pDragRectangle;}

  Caret* GetCaret() {return &m_caret;}
  const HCURSOR GetCursor() const;

  afx_msg void OnUpdateAddLine(CCmdUI *pCmdUI);
  afx_msg void OnUpdateAddArrow(CCmdUI *pCmdUI);
  afx_msg void OnUpdateAddRectangle(CCmdUI *pCmdUI);
  afx_msg void OnUpdateAddEllipse(CCmdUI *pCmdUI);
  afx_msg void OnUpdateAddText(CCmdUI *pCmdUI);
  afx_msg void OnUpdateModifyFigure(CCmdUI *pCmdUI);

  afx_msg void OnAddLine();
  afx_msg void OnAddArrow();
  afx_msg void OnAddRectangle();
  afx_msg void OnAddEllipse();
  afx_msg void OnAddText();
  afx_msg void OnModifyFigure();

  afx_msg void OnUpdateCut(CCmdUI *pCmdUI);
  afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);

  afx_msg void OnCut();
  afx_msg void OnCopy();

  afx_msg void OnUpdatePaste(CCmdUI *pCmdUI);
  afx_msg void OnPaste();

  afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
  afx_msg void OnDelete();

  afx_msg void OnUpdateColor(CCmdUI *pCmdUI);
  afx_msg void OnColor();

  afx_msg void OnUpdateFont(CCmdUI *pCmdUI);
  afx_msg void OnFont();

  afx_msg void OnUpdateFill(CCmdUI *pCmdUI);
  afx_msg void OnFill();

private:
  static BOOL IsMarked(Figure* pFigure);
  static BOOL IsMarkedText(Figure* pFigure);
  static BOOL IsMarkedAndFilled(Figure* pFigure);
  static BOOL IsMarkedAndNotFilled(Figure* pFigure);

  void UnmarkAllFigures();
  void ClearCopyList();

private:
  Caret m_caret;

  ApplicationState m_eApplicationState;
  NextActionState m_eNextActionState;
  KeyboardState m_eKeyboardState;

  Color m_nextColor;
  Font m_nextFont;
  BOOL m_bNextFill;

  Figure *m_pSingleFigure;
  TextFigure* m_pEditText;
  RectangleFigure *m_pDragRectangle;

  FigurePointerList m_figurePtrList, m_copyPtrList;
  CPoint m_ptPrevMouse;
};
