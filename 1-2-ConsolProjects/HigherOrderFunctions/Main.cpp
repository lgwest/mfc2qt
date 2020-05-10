// ch 1, page 43
#include <iostream>
using namespace std;

void ApplyArray(int intArray[], int iSize, int Apply(int))
{
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    intArray[iIndex] = Apply(intArray[iIndex]);
  }
}

int Double(int iNumber)
{
  return 2 * iNumber;
}

int Square(int iNumber)
{
  return iNumber * iNumber;
}

int FilterArray(int intArray[], int iSize, bool Filter(int))
{
  int iCount = 0;

  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    if (Filter(intArray[iIndex]))
    {
      intArray[iCount] = intArray[iIndex];
      iCount++;
    }
  }

  return iCount;
}

bool EndsWithSix(int iNumber)
{
  return (iNumber % 10) == 6;
}

bool LessThanOneHundred(int iNumber)
{
  return iNumber < 100;
}

void PrintArray(int intArray[], int iSize)
{
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    cout << intArray[iIndex] << " ";
  }

  cout << endl;
}

void main()
{
  int numberArray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int iArraySize = sizeof numberArray / sizeof numberArray[0];
  PrintArray(numberArray, iArraySize);

  // Doubles every value in the array.
  ApplyArray(numberArray, iArraySize, Double);
  PrintArray(numberArray, iArraySize);

   // Squares every value in the array.
  ApplyArray(numberArray, iArraySize, Square);
  PrintArray(numberArray, iArraySize);

  // Filters the even number in the array.
  int iEndsWithSixs = FilterArray(numberArray, iArraySize, EndsWithSix);
  PrintArray(numberArray, iEndsWithSixs);

  // Filters the number greater than five in the array.
  int iLessThanOneHundreds = FilterArray(numberArray, iEndsWithSixs,
                                         LessThanOneHundred);
  PrintArray(numberArray, iLessThanOneHundreds);
}
