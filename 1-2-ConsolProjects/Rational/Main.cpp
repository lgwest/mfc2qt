#include <iostream>
using namespace std;

#include "Rational.h"

void main()
{
  Rational a, b;
  cout << "Rational number 1: ";
  cin >> a;

  cout << "Rational number 2: ";
  cin >> b;
  cout << endl;

  cout << "a: " << a << endl;
  cout << "b: " << b << endl << endl;

  cout << "a == b: " << (a == b ? "Yes" : "No") << endl;
  cout << "a != b: " << (a != b ? "Yes" : "No") << endl;
  cout << "a <  b: " << (a <  b ? "Yes" : "No") << endl;
  cout << "a <= b: " << (a <= b ? "Yes" : "No") << endl;
  cout << "a >  b: " << (a >  b ? "Yes" : "No") << endl;
  cout << "a >= b: " << (a >= b ? "Yes" : "No") << endl << endl;

  cout << "a + b: " << a + b << endl;
  cout << "a - b: " << a - b << endl;
  cout << "a * b: " << a * b << endl;
  cout << "a / b: " << a / b << endl;
}
