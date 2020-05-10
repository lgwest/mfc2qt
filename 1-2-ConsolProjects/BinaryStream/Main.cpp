#include <iostream>
#include <fstream>
using namespace std;

void main(void)
{
  ofstream outFile("Numbers.bin", ios::out);
  for (int iIndex = 1; iIndex <= 10; ++iIndex)
  {
    outFile.write((char*) &iIndex, sizeof iIndex);
  }
  outFile.close();

  ifstream inFile("Numbers.bin", ios::in);
  int iValue;
  while (inFile.read((char*) &iValue, sizeof iValue) != 0)
  {
    cout << iValue << endl;
  }
}
