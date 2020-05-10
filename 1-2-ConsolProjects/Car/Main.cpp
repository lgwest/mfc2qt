// ch 2, page 52
#include <iostream>
using namespace std;

#include "Car.h"

void main()
{
  Car car1(100, 90);

  cout << "Car1: " << car1.GetSpeed()
       << " degrees, " << car1.GetDirection()
       << " miles per hour" << endl;

  Car car2(150, 0);
  car2.TurnRight(180);

  cout << "Car2: " << car2.GetSpeed()
       << " degrees, " << car2.GetDirection()
       << " miles per hour" << endl;

  Car car3;
  car3.IncreaseSpeed(200);
  car3.TurnRight(270);

  cout << "Car3: " << car3.GetSpeed()
     << " degrees, " << car3.GetDirection()
     << " miles per hour" << endl;

// Would cause a compiler error.
// cout << "Speed: " << car3.m_iSpeed << endl;
}
