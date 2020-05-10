#include <iostream>
using namespace std;

#include <climits> // The integral type limit constants.
#include <cfloat>  // The floating type limit constants.

void main()
{
  int iIntSize1 = sizeof (int);
  int iIntSize2 = sizeof iIntSize1;

  cout << "integer size: " << iIntSize1 << " " << iIntSize2 << endl;

  int* pSize = &iIntSize1;
  int iPtrSize = sizeof pSize;

  cout << "pointer size: " << iPtrSize << endl;

  int array[3] = {1, 2, 3};
  int iArraySize = sizeof array;

  cout << "array size: " << iArraySize << endl << endl;

  cout << "Minimum signed char: " << SCHAR_MIN << endl;
  cout << "Maximum signed char: " << SCHAR_MAX << endl;
  cout << "Minimum signed short int: " << SHRT_MIN << endl;
  cout << "Maximum signed short int: " << SHRT_MAX << endl;
  cout << "Minimum signed int: " << INT_MIN << endl;
  cout << "Maximum signed int: " << INT_MAX << endl;
  cout << "Minimum signed long int: " << LONG_MIN << endl;
  cout << "Maximum signed long int: " << LONG_MAX << endl << endl;

  // The minimum value of an unsigned integral type is always zero.
  cout << "Maximum unsigned char: " << UCHAR_MAX << endl;
  cout << "Maximum unsigned short int: " << USHRT_MAX << endl;
  cout << "Maximum unsigned int: " << UINT_MAX << endl;
  cout << "Maximum unsigned long int: " << ULONG_MAX << endl << endl;

  // There are no constants for long double.
  cout << "Minimum float: " << FLT_MIN << endl;
  cout << "Maximum float: " << FLT_MAX << endl;
  cout << "Minimum double: " << DBL_MIN << endl;
  cout << "Maximum double: " << DBL_MAX << endl;
}
