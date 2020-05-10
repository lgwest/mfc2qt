class Employee : public Person
{
  public:
    Employee(string stName, string stEmployer);
    void Print() const;

  private:
    string m_stEmployer;
};
