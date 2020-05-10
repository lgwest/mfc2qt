#include <string>
#include <iostream>
using namespace std;

#include "Person.h"
#include "Student.h"
#include "Employee.h"

void main()
{
// Would not work as Person is an abstract class.
// Person person("John Smith");
// person.Print();
// cout << endl;

  Student student("Mark Jones", "Berkeley");
  student.Print();
  cout << endl;

  Employee employee("Adam Brown", "Microsoft");
  employee.Print();
  cout << endl;

// Person* pPerson = &person;
// pPerson->Print();
// cout << endl;

  Person* pPerson = &student;
  pPerson->Print();
  cout << endl;

  pPerson = &employee;
  pPerson->Print();
}
