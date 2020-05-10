#include <string>
#include <iostream>
using namespace std;

#include "Person.h"
#include "Student.h"

Student::Student(const string& stName, const string& stUniversity)
 :Person(stName),
  m_stUniversity(stUniversity)
{
  // Empty.
}

void Student::Print() const
{
  cout << "Name: " << m_stName << endl;
  cout << "University: " << m_stUniversity << endl;
}
