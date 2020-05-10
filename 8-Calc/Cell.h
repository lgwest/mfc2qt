static const int CELL_MARGIN = 100;
enum CellState {CELL_TEXT, CELL_VALUE, CELL_FORMULA};

typedef CArray<CRect> RectArray;

enum Alignment {HALIGN_LEFT = DT_LEFT,
     HALIGN_CENTER = DT_CENTER, HALIGN_RIGHT = DT_RIGHT,
     HALIGN_JUSTIFIED = DT_LEFT + DT_CENTER + DT_RIGHT,
     VALIGN_TOP = DT_TOP, VALIGN_CENTER = DT_VCENTER,
     VALIGN_BOTTOM = DT_BOTTOM};

enum KeyboardState {KM_INSERT, KM_OVERWRITE};
enum Direction {HORIZONTAL, VERTICAL};
enum ColorStatus {TEXT, BACKGROUND};

class CellMatrix;
class TSetMatrix;
class CCalcDoc;

class Cell
{
public:
  Cell();
  Cell(const Cell& cell);
  Cell& operator=(const Cell& cell);
  void CopyCell(const Cell& cell);

  void SetCellMatrix(CellMatrix* pCellMatrix)
       {m_pCellMatrix = pCellMatrix;}
  void SetTargetSetMatrix(TSetMatrix* pTargetSetMatrix)
       {m_pTargetSetMatrix = pTargetSetMatrix;}
  BOOL IsEmpty() const
       {return (m_eCellState == CELL_TEXT) &&
  m_stText.IsEmpty();}

  void Clear(Reference home);
  void Serialize(CArchive& archive);

  void CharDown(UINT cChar, int iEditIndex,
                KeyboardState eKeyBoardMode);
  void GenerateCaretArray(CDC* pDC);

  CString GetInputText() {return m_stInput;}
  void SetInputText(CString stInput) {m_stInput = stInput;}

  int MouseToIndex(CPoint ptMouse);
  CRect IndexToCaret(int iIndex);

  void Draw(CPoint ptTopLeft, BOOL bEdit, BOOL bMarked,
    CDC *pDC);

  Font GetFont() const {return m_font;}
  void SetFont(const Font& font) {m_font = font;}

  Color GetColor(int iColorType) const;
  void SetColor(int iColorType, const Color& textColor);

  Alignment GetAlignment(Direction eDirection) const;
  void SetAlignment(Direction eDirection, Alignment eAlignment);

  void GenerateInputText();
  void EndEdit(Reference home);
  BOOL IsNumeric(CString stText);

  BOOL HasValue(BOOL bRecursive);
  double GetValue() const {return m_dValue;}

  void EvaluateValue(BOOL bRecursive);
  void UpdateSyntaxTree(int iAddRows, int iAddCols);

  ReferenceSet GetSourceSet() const {return m_sourceSet;};

private:
  CellState m_eCellState;

  CString m_stText;
  double m_dValue;
  SyntaxTree m_syntaxTree;
  BOOL m_bHasValue;
  RectArray m_caretRectArray;
  CString m_stInput, m_stOutput;

  Font m_font;
  Color m_textColor, m_backgroundColor;
  Alignment m_eHorizontalAlignment, m_eVerticalAlignment;

  ReferenceSet m_sourceSet;
  CellMatrix* m_pCellMatrix;
  TSetMatrix* m_pTargetSetMatrix;
};
