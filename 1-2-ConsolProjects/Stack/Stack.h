#pragma once

class Cell;

class Stack
{
  public:
    Stack();
    ~Stack();

    void Push(int iValue);
    void Pop();

    int Top() const;
    bool IsEmpty() const;

  private:
    Cell* m_pFirstCell;
};
