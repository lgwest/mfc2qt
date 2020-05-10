#include <iostream>
#include <fstream>
using namespace std;

void main(void)
{
  ifstream inFile("Input.txt", ios::in);
  ofstream outFile("Output.txt", ios::out);

  int iValue;
  while (inFile >> iValue)
  {
    outFile << (iValue * iValue) << endl;
  }
}
