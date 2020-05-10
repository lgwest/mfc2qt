class Person
{
  public:
    Person(const string& stName);
    virtual void Print() const = 0;

  protected:
    string m_stName;
};
