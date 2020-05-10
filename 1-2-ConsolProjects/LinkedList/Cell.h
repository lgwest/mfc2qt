#pragma once
class Cell;
typedef Cell* Link;

class Cell
{
  public:
    Cell(int iValue, Cell* pNextCell);

    int& Value() {return m_iValue;}
    const int Value() const {return m_iValue;}
    
    Link& NextLink() {return m_pNextLink;}
    const Link NextLink() const {return m_pNextLink;}

  private:
    int m_iValue;
    Link m_pNextLink;
};
