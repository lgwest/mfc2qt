class FigureFileManager : public CObject
{
  public:
    FigureFileManager(Figure* pFigure = NULL);
    Figure* GetFigure() const {return m_pFigure;}

  private:
    int GetId() const;
    void CreateFigure(int iId);

  public:
    void Serialize(CArchive& archive);

  private:
    enum {LINE, ARROW, RECTANGLE, ELLIPSE, TEXT};
    Figure* m_pFigure;
};