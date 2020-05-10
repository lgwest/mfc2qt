class Font
{
  public:
    Font();
    Font(CString stName, int iSize);
    Font(const LOGFONT& logFont);
    Font(const Font& font);

    operator LOGFONT() {return m_logFont;}
    operator PLOGFONT() {return &m_logFont;}
    Font PointsToMeters() const;
    
    Font& operator=(const Font& font);
    BOOL operator==(const Font& font) const;
    BOOL operator!=(const Font& font) const;
    
    void Serialize(CArchive& archive);
    BOOL IsItalic() const {return m_logFont.lfItalic;}

  private:
    LOGFONT m_logFont;
};
