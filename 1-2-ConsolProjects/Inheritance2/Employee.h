class Employee : public Person
{
  public:
    Employee(const string& stName, const string& stEmployer);
    void Print() const;

  private:
    string m_stEmployer;
};
