#include <iostream>
#include <string>
using namespace std;

#include "Person.h"
#include "Student.h"

Student::Student(string stName, string stUniversity)
 :Person(stName),
  m_stUniversity(stUniversity)
{
  // Empty.
}

void Student::Print() const
{
  Person::Print();
  cout << "University: " << m_stUniversity << endl;
}
