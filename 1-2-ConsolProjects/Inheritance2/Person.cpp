#include <string>
using namespace std;

#include "Person.h"

Person::Person(const string& stName)
 :m_stName(stName)
{
  // Empty.
}

class Student : public Person
{
  public:
    Student(const string& stName, const string& stUniversity);
    void Print() const;

  private:
    string m_stUniversity;
};
