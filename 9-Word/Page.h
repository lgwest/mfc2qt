class Page
{
  public:
    Page();
    Page(int iFirstParagraph, int iLastParagraph);

    int GetFirstParagraph() const {return m_iFirstParagraph;}
    int GetLastParagraph() const {return m_iLastParagraph;}
 
    void Serialize(CArchive& archive);

  private:
    int m_iFirstParagraph, m_iLastParagraph;
};