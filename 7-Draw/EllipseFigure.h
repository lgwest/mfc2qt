class EllipseFigure: public virtual TwoDimensionalFigure,
                     private RectangleFigure
{
  public:
    EllipseFigure();
    EllipseFigure(const Color& color, const CPoint& ptTopLeft,
                  BOOL bFilled);

    EllipseFigure(const EllipseFigure& ellipse);
    Figure* Copy() const;

    void Serialize(CArchive& archive)
         {return RectangleFigure::Serialize(archive);}
    HCURSOR GetCursor() const;

    BOOL Click(const CPoint& ptMouse);
    BOOL DoubleClick(const CPoint& ptMouse)
         {return RectangleFigure::DoubleClick(ptMouse);}
    BOOL Inside(const CRect& rcInside) const
         {return RectangleFigure::Inside(rcInside);}

    void MoveOrModify(const CSize& szDistance);
    void Move(const CSize& szDistance)
         {return RectangleFigure::Move(szDistance);}

    void Draw(CDC* pDC) const;
    CRect GetArea() const
          {return RectangleFigure::GetArea();}

  private:
    enum {CREATE_ELLIPSE, MODIFY_LEFT, MODIFY_RIGHT,
          MODIFY_TOP, MODIFY_BOTTOM, MOVE_ELLIPSE}
         m_eDragMode;
};
