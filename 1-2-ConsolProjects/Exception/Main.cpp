// ch 2, page 76
#include <iostream>
using namespace std;

double divide(double dNumerator, double dDenominator)
{
  if (dDenominator == 0)
  {
    throw "Division by zero.";
  }

  return dNumerator / dDenominator;
}

double invers(double dValue)
{
  return divide(1, dValue);
}

void main()
{
  double dValue;

  cout << ": ";
  cin >> dValue;

  try
  {
    cout << "1 / " << dValue << " = " << invers(dValue) << endl;
  }

  catch (char* szMessage)
  {
    cout << szMessage << endl;
  }
}
