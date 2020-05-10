#include <iostream>
#include <string>
using namespace std;

#include "Person.h"
#include "Employee.h"

Employee::Employee(string stName, string stEmployer)
 :Person(stName),
  m_stEmployer(stEmployer)
{
  // Empty.
}

void Employee::Print() const
{
  Person::Print();
  cout << "Company: " << m_stEmployer << endl;
}
