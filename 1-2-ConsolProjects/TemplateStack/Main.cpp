#include <iostream>
#include <string>
using namespace std;

#include <cstdlib>
#include <cassert>

#include "TemplateCell.h"
#include "TemplateStack.h"


template <typename Type>
Type Min(Type value1, Type value2)
{
  return (value1 < value2) ? value1 : value2;
}

void main()
{
  TemplateStack<int> intStack;
  intStack.Push(1);
  intStack.Push(2);
  intStack.Push(3);

  TemplateStack<double> doubleStack;
  doubleStack.Push(1.2);
  doubleStack.Push(2.3);
  doubleStack.Push(3.4);

  int i1 = 2, i2 = 2;
  cout << Min(i1, i2) << endl; // 2

  string s1 = "abc", s2 = "def";
  cout << Min(s1, s2) << endl; // "def"
}
