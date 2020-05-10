class Reference
{
  public:
    Reference();
    Reference(int iRow, int iCol);

    Reference(const Reference& reference);
    Reference operator=(const Reference& reference);

    int GetRow() const {return m_iRow;}
    int GetCol() const {return m_iCol;}

    void SetRow(int iRow) {m_iRow = iRow;}
    void SetCol(int iCol) {m_iCol = iCol;}

    friend BOOL operator==(const Reference &ref1,
                           const Reference &ref2);
    friend BOOL operator<(const Reference& ref1,
                          const Reference& ref2);

    CString ToString() const;
    void Serialize(CArchive& archive);

  private:
    int m_iRow, m_iCol;
};

// A ReferenceSet is a set of references.
typedef Set<Reference> ReferenceSet;
