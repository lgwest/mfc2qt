template <typename Type>
class Cell
{
    public:
        Cell(Type value, Cell<Type>* pNextCell);

        Type& Value() {return m_value;}
        const Type Value() const {return m_value;}

        Cell<Type>*& Next() {return m_pNextCell;}
        const Cell<Type>* Next() const {return m_pNextCell;}

    private:
        Type m_value;
        Cell<Type>* m_pNextCell;
};

template <typename Type>
Cell<Type>::Cell(Type value, Cell<Type>* pNextCell)
  : m_value(value),
    m_pNextCell(pNextCell)
{
    // Empty.
}
