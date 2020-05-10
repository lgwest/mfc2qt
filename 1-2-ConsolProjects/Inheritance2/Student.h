class Student : public Person
{
  public:
    Student(const string& stName, const string& stUniversity);
    void Print() const;

  private:
    string m_stUniversity;
};
