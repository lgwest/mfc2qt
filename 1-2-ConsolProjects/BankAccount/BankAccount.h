class BankAccount
{
  public:
    BankAccount(int iNumber, double dSaldo = 0);
    BankAccount(const BankAccount& bankAccount);

    void Deposit(double dAmount)  {m_dSaldo += dAmount;}
    void Withdraw(double dAmount) {m_dSaldo -= dAmount;}

    int GetNumber()   const {return m_iNUMBER;}
    double GetSaldo() const {return m_dSaldo; }

  private:
    const int m_iNUMBER;
    double m_dSaldo;
};
