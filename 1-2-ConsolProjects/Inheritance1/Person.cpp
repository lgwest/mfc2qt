#include <iostream>
#include <string>

using namespace std;
#include "Person.h"

Person::Person(string stName)
 :m_stName(stName) 
{
  // Empty.
}

void Person::Print() const
{
  cout << "Name: " << m_stName << endl;
}
