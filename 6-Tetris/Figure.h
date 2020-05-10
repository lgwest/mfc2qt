const COLORREF BLACK = RGB(0, 0, 0);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF DEFAULT_COLOR = WHITE;

class ColorGrid;
extern int g_iRowHeight, g_iColWidth;
enum {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};

const int SQUARE_ARRAY_SIZE = 4;
const int SQUARE_INFO_SIZE = 4;

typedef Square SquareArray[SQUARE_ARRAY_SIZE];
typedef SquareArray* SquareInfo[SQUARE_INFO_SIZE];

class Figure
{
  public:
    Figure();
    Figure(int iDirection, COLORREF rfColor,
           const SquareInfo& squareInfo);

    Figure operator=(const Figure& figure);
    void SetColorGrid(ColorGrid* pColorGrid)
                     {m_pColorGrid = pColorGrid;}

  private:
    BOOL IsSquareValid(int iRow, int iCol) const;

  public:
    BOOL IsFigureValid() const;

    BOOL MoveLeft();
    BOOL MoveRight();

  private:
    void RotateClockwiseOneQuarter();
    void RotateAnticlockwiseOneQuarter();

  public: 
    BOOL RotateClockwise();
    BOOL RotateAnticlockwise();
    BOOL MoveDown();

    void AddToGrid();
    CRect GetArea() const;

  public:
    void Draw(int iColorStatus, CDC* pDC) const;
    friend void DrawSquare(int iRow, int iCol, CDC* pDC);

  public:
    void Serialize(CArchive& archive);

  private:
    COLORREF m_rfColor;
    ColorGrid* m_pColorGrid;
    int m_iRow, m_iCol, m_iDirection;
    SquareInfo m_squareInfo;
};

typedef CArray<const Figure> FigurePtrArray;
