typedef CList<int> IntList;

const int FIGURE_ARRAY_SIZE = 7;

class CTetrisDoc : public CDocument
{
  protected:
    CTetrisDoc();

  public:
    virtual ~CTetrisDoc();
    void SaveScoreList();

  protected:
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CTetrisDoc)

  public:
    BOOL OnNewDocument();

    virtual void Serialize(CArchive& archive);
    int GetScore() const {return m_iScore;}

    const IntList* GetScoreList() {return &m_scoreList;}
    const ColorGrid* GetGrid() {return &m_colorGrid;}

    const Figure& GetActiveFigure() const {return
                                           m_activeFigure;}
    const Figure& GetNextFigure() const {return m_nextFigure;}

  public:
    void LeftArrowKey();
    void RightArrowKey();
    void UpArrowKey();
    void DownArrowKey();

    BOOL Timer();
    void SpaceKey();

  private:
    void GameOver();
    BOOL NewGame();
    int AddScoreToList();

    void DeleteFullRows();
    BOOL IsRowFull(int iRow);

    void FlashRow(int iFlashRow);
    void DeleteRow(int iDeleteRow);

  private:
    ColorGrid m_colorGrid;
    Figure m_activeFigure, m_nextFigure;

    int m_iScore;
    IntList m_scoreList;

    const CRect NEXT_AREA, SCORE_AREA;
    static Figure m_figureArray[FIGURE_ARRAY_SIZE];
};
