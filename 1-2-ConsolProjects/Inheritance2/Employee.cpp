#include <string>
#include <iostream>
using namespace std;

#include "Person.h"
#include "Employee.h"

Employee::Employee(const string& stName, const string& stEmployer)
 :Person(stName),
  m_stEmployer(stEmployer)
{
  // Empty.
}

void Employee::Print() const
{
  cout << "Name: " << m_stName << endl;
  cout << "Company: " << m_stEmployer << endl;
}
