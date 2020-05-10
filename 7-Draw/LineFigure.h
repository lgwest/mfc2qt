class LineFigure: public virtual Figure
{
  public:
    LineFigure();
    LineFigure(const Color& color, const CPoint& ptMouse);

    LineFigure(const LineFigure& line);
    Figure* Copy() const;

    void Serialize(CArchive& archive);
    HCURSOR GetCursor() const;

    BOOL Click(const CPoint& ptMouse);
    BOOL DoubleClick(const CPoint& ptMouse);
    BOOL Inside(const CRect& rcInside) const;

    void MoveOrModify(const CSize& szDistance);
    void Move(const CSize& szDistance);

    void Draw(CDC* pDC) const;
    CRect GetArea() const;

  protected:
    CPoint m_ptFirst, m_ptLast;
    enum {CREATE_LINE, MODIFY_FIRST, MODIFY_LAST, MOVE_LINE} m_eDragMode;
};
