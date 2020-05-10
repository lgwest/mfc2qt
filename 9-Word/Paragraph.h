typedef CArray<int> IntArray;
typedef CArray<CSize> SizeArray;
typedef CArray<CRect> RectArray;
typedef CArray<Font> FontArray;
typedef CArray<Line> LineArray;
typedef Set<CRect> RectSet;

enum Alignment {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER, ALIGN_JUSTIFIED};
enum KeyboardState {KM_INSERT, KM_OVERWRITE};

class CWordDoc;

class Paragraph
{
  public:
    Paragraph();
    Paragraph(Font emptyFont, Alignment eAlignment);

    Paragraph(const Paragraph& paragraph);
    void Serialize(CArchive& archive);

    void Draw(CDC* pDC, int iFirstMarkedChar, int iLastMarkedChar) const;

    int GetLength() const {return m_stText.GetLength();}
    int GetHeight() const {return m_iHeight;}

    void SetStartPos(int yPos) {m_yStartPos = yPos;}
    int GetStartPos() const {return m_yStartPos;}

    void AddChar(int iChar, UINT cChar, Font* pNextFont,
                 KeyboardState eKeyboardState);
    void DeleteText(int iFirstIndex = 0, int iLastIndex = -1);

    Alignment GetAlignment() const {return m_eAlignment;}
    void SetAlignment(Alignment eAlignment) {m_eAlignment = eAlignment;}

    Font GetFont(int iChar) const;
    void SetFont(Font font, int iFirstIndex = 0, int iLastindex = -1);

    void GetRepaintSet(RectSet& repaintSet, int iFirstIndex = 0,
                          int iLastIndex = -1);
    BOOL GetWord(int iEditChar, int& iFirstChar, int& iLastChar);

    int GetHomeChar(int iChar) const;
    int GetEndChar(int iChar) const;

    Paragraph* ExtractText(int iFirstIndex = 0, int iLastIndex = -1) const;
    void Insert(int iChar, Paragraph* pInsertParagraph);

    void Append(Paragraph* pSecondParagraph);
    Paragraph* Split(int iChar);

    int PointToChar(CPoint ptMouse);
    CRect CharToRect(int iChar);
    CRect GetCaretRect(int iChar);
    BOOL isHomeChar(int iChar);
    CRect CharToLineRect(int iChar);

    void Recalculate(CDC* pDC, RectSet* pRepaintSet = NULL);
    void ClearRectArray();

  private:
    void GenerateSizeArray(SizeArray& sizeArray, CDC* pDC);
    void GenerateAscentArray(IntArray& ascentArray, CDC* pDC);
    void GenerateLineArray(SizeArray& sizeArray);
    void GenerateRectArray(SizeArray& sizeArray, IntArray& ascentArray);
    void GenerateRepaintSet(RectArray& oldRectArray,
                               RectSet* pRepaintSet);

  private:
    CString m_stText;

    Font m_emptyFont;
    int m_yStartPos, m_iEmptyAverageWidth, m_iHeight;
    Alignment m_eAlignment;

    FontArray m_fontArray;
    LineArray m_lineArray;
    RectArray m_rectArray;
};

typedef CArray<Paragraph*> ParagraphPtrArray;
