class RectangleFigure: public virtual TwoDimensionalFigure
{
  public:
    RectangleFigure();
    RectangleFigure(const Color& color, const CPoint&
                    ptTopLeft, BOOL bFilled);

    RectangleFigure(const RectangleFigure& rectangle);
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

  private:
    enum {CREATE_RECTANGLE, MODIFY_TOPLEFT, MODIFY_TOPRIGHT,
          MODIFY_BOTTOMRIGHT, MODIFY_BOTTOMLEFT,
          MOVE_RECTANGLE} m_eDragMode;

  protected:
    CPoint m_ptTopLeft, m_ptBottomRight;
};
