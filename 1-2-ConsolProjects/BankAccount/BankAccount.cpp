#include "BankAccount.h"

BankAccount::BankAccount(int iNumber, double dSaldo /* = 0 */)
 :m_iNUMBER(iNumber),
  m_dSaldo(dSaldo)
{
  // Empty.
}

BankAccount::BankAccount(const BankAccount& bankAccount)
 :m_iNUMBER(bankAccount.m_iNUMBER),
  m_dSaldo(bankAccount.m_dSaldo)
{
  // Empty.
}
