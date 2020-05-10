typedef CArray<int> IntArray;
enum KeyboardState {KS_INSERT, KS_OVERWRITE};

class TextFigure: public Figure
{
  public:
    TextFigure();
    TextFigure(const Color& color, const CPoint& ptMouse,
               const Font& font, CDC* pDC);

    TextFigure(const TextFigure& text);
    Figure* Copy() const;

    void Serialize(CArchive& archive);

    BOOL Click(const CPoint& ptMouse);
    BOOL DoubleClick(const CPoint& ptMouse);
    BOOL Inside(const CRect& rcInside) const;

    void MoveOrModify(const CSize& szDistance);
    void Move(const CSize& szDistance);

    BOOL KeyDown(UINT uChar, CDC* pDC);
    void CharDown(UINT uChar, CDC* pDC,
                  KeyboardState eKeyboardState);
    void SetPreviousText(CDC* pDC);

    void Draw(CDC* pDC) const;
    CRect GetArea() const;
    
    Font* GetFont() {return &m_font;}
    void SetFont(const Font& font, CDC* pDC);

  private:
    void GenerateCaretArray(CDC* pDC);

  public:
    CRect GetCaretArea(KeyboardState eKeyboardState);
    HCURSOR GetCursor() const;

  private:
    enum {CREATE_TEXT, MOVE_TEXT, EDIT_TEXT, NONE_TEXT}
         m_eDragMode;

    CPoint m_ptText;
    CSize m_szText;

    CString m_stText, m_stPreviousText;
    int m_iAverageWidth;

    Font m_font;
    int m_iEditIndex;
    IntArray m_caretArray;
};
