class Line
{
  public:
    Line();
    Line(int iFirstChar, int iLastChar, int iHeight);

    int GetFirstChar() const {return m_iFirstChar;}
    int GetLastChar() const {return m_iLastChar;}
    int GetHeight() const {return m_iHeight;}

    void Serialize(CArchive& archive);

  private:
    int m_iFirstChar, m_iLastChar, m_iHeight;
};