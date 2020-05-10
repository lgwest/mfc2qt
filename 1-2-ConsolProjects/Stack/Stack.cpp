#include <cstdlib>
#include <cassert>

#include "../LinkedList/Cell.h"
#include "Stack.h"

Stack::Stack()
 :m_pFirstCell(NULL)
{
  // Empty.
}

Stack::~Stack()
{
  Cell* pCurrCell = m_pFirstCell;

  while (pCurrCell != NULL)
  {
    Cell* pRemoveCell = pCurrCell;
    pCurrCell = pCurrCell->NextLink();
    delete pRemoveCell;
  }
}

void Stack::Push(int iValue)
{
  Cell* pNewCell = new Cell(iValue, m_pFirstCell);
  assert(pNewCell != NULL);
  m_pFirstCell = pNewCell;
}

void Stack::Pop()
{
  assert(m_pFirstCell != NULL);
  Cell* pTempCell = m_pFirstCell;
  m_pFirstCell = m_pFirstCell->NextLink();
  delete pTempCell;
}

int Stack::Top() const
{
  assert(m_pFirstCell != NULL);
  return m_pFirstCell->Value();
}

bool Stack::IsEmpty() const
{
  return (m_pFirstCell == NULL);
}
