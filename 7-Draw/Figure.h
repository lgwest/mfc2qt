class Figure : public CObject
{
  public:
    Figure();
    Figure(const Color& color, BOOL bMarked = FALSE);

    Figure(const Figure& figure);
    virtual Figure* Copy() const = 0;

    BOOL IsMarked() const {return m_bMarked;}
    void Mark(const BOOL bMarked) {m_bMarked = bMarked;}

    Color GetColor() const {return m_figureColor;}
    void SetColor(const Color& color) {m_figureColor = color;}

    virtual void Serialize(CArchive& archive);
    virtual HCURSOR GetCursor() const = 0;

    virtual BOOL Click(const CPoint& ptMouse) = 0;
    virtual BOOL DoubleClick(const CPoint& ptMouse) = 0;
    virtual BOOL Inside(const CRect& rcInside) const = 0;

    virtual void MoveOrModify(const CSize& szDistance) = 0;
    virtual void Move(const CSize& szDistance) = 0;

    virtual void Draw(CDC* pDC) const = 0;
    virtual CRect GetArea() const = 0;

  private:
    BOOL m_bMarked;
    Color m_figureColor;

  protected:
    static const int SQUARE_SIDE = 200;
};

typedef List<Figure*> FigurePointerList;
