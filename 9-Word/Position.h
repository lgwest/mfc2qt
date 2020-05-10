class Position
{
  public:
    Position(int iParagraph, int iCharacter);
    Position(const Position& position);
    Position& operator=(const Position& position);

    BOOL operator==(const Position& position) const;
    BOOL operator!=(const Position& position) const;
    BOOL operator<(const Position& position) const;
    BOOL operator>(const Position& position) const;

    int& Paragraph() {return m_iParagraph;}
    int& Character() {return m_iCharacter;}

    int Paragraph() const {return m_iParagraph;}
    int Character() const {return m_iCharacter;}

  private:
    int m_iParagraph, m_iCharacter;
};
