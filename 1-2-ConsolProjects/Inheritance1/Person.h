class Person
{
  public:
    Person(string stName);
    virtual void Print() const;

  private:
    string m_stName;
};
