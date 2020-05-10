class Color
{
  public:
    Color();
    Color(const COLORREF crRedGreenBlue);
    Color(const Color& color);

    operator COLORREF() const;
    Color& operator=(const Color& color);

    void Serialize(CArchive& archive);
    Color Inverse() const;

  private:
    COLORREF m_crRedGreenBlue;
};

static Color LIGHT_GRAY = RGB(80, 80, 80);
static Color GRAY = RGB(128, 128, 128);
static Color WHITE = RGB(255, 255, 255);
static Color BLACK = RGB(0, 0, 0);
