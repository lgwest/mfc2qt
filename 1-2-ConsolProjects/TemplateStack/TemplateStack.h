template <typename Type>
class TemplateStack
{
    public:
        TemplateStack();
        ~TemplateStack();

        void Push(Type value);
        void Pop();

        Type Top();
        bool IsEmpty();

    private:
        Cell<Type>* m_pFirstCell;
};

template <typename Type>
TemplateStack<Type>::TemplateStack()
  : m_pFirstCell(NULL)
{
    // Empty.
}

template <typename Type>
TemplateStack<Type>::~TemplateStack()
{
    Cell<Type>* pCurrCell = m_pFirstCell;
    while (pCurrCell != NULL)
    {
        Cell<Type>* pRemoveCell = pCurrCell;
        pCurrCell = pCurrCell->Next();
        delete pRemoveCell;
    }
}

template <typename Type>
void TemplateStack<Type>::Push(Type value)
{
    Cell<Type>* pNewCell = new Cell<Type>(value, m_pFirstCell);
    assert(pNewCell != NULL);
    m_pFirstCell = pNewCell;
}

template <typename Type>
void TemplateStack<Type>::Pop()
{
    assert(m_pFirstCell != NULL);
    Cell<Type>* pRemoveCell = m_pFirstCell;
    m_pFirstCell = m_pFirstCell->Next();
    delete pRemoveCell;
}

template <typename Type>
Type TemplateStack<Type>::Top()
{
    assert(m_pFirstCell != NULL);
    return m_pFirstCell->Value();
}

template <typename Type>
bool TemplateStack<Type>::IsEmpty()
{
    return m_pFirstCell == NULL;
}
