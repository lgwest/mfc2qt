// ch 2, page 55
#include <iostream>
using namespace std;

#include "BankAccount.h"

void main()
{
  BankAccount account1(123);
  account1.Deposit(100);
  cout << "Account1: number " << account1.GetNumber() // 123
        << ", $" << account1.GetSaldo() << endl;      // 100

  account1.Withdraw(50);
  cout << "Account1: number " << account1.GetNumber() // 123
       << ", $" << account1.GetSaldo() << endl;       // 50

  const BankAccount account2(124, 200);
  cout << "Account2: number " << account2.GetNumber() // 124
       << ", $" << account2.GetSaldo() << endl;       // 200

// Would cause a compiler error.
// account2.Withdraw(50);
}
