class TSetMatrix
{
  public:
    TSetMatrix();
    TSetMatrix(const TSetMatrix& tSetMatrix);
    TSetMatrix operator=(const TSetMatrix& tSetMatrix);

    void SetCellMatrix(CellMatrix* pCellMatrix);
    void Serialize(CArchive& archive);

    ReferenceSet* Get(int iRow, int iCol) const;
    ReferenceSet* Get(Reference home) const;

    void CheckCircular(Reference home,
                       ReferenceSet sourceSet);
    ReferenceSet EvaluateTargets(Reference home);
    void AddTargets(Reference home);
    void RemoveTargets(Reference home);

  private:
    ReferenceSet m_buffer[ROWS][COLS];
    CellMatrix* m_pCellMatrix;
};
