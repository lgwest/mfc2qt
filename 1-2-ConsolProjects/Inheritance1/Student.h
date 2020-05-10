class Student : public Person
{
  public:
    Student(string stName, string stUniversity);
    void Print() const;

  private:
    string m_stUniversity;
};
