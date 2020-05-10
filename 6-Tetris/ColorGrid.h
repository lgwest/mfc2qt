const int ROWS = 20;
const int COLS = 10;

class ColorGrid
{
  public:
    ColorGrid();
    void Clear();

    COLORREF& Index(int iRow, int iCol);
    const COLORREF Index(int iRow, int iCol) const;

    void Serialize(CArchive& archive);

  private:
    COLORREF m_buffer[ROWS * COLS];
};
