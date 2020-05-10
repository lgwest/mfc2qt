class TwoDimensionalFigure : public Figure
{
  public:
    TwoDimensionalFigure();
    TwoDimensionalFigure(const Color& color, BOOL bFilled);

    BOOL IsFilled() const {return m_bFilled;}
    void Fill(const BOOL bFill) {m_bFilled = bFill;}

    void Serialize(CArchive& archive);

  private:
    BOOL m_bFilled;
};
