#include "Car.h"

Car::Car()
 :m_iSpeed(0),
  m_iDirection(0)
{
    // Empty.
}

Car::Car(int iSpeed, int iDirection)
 :m_iSpeed(iSpeed),
  m_iDirection(iDirection)
{
  // Empty.
}

Car::~Car()
{
  // Empty.
}

void Car::IncreaseSpeed(int iSpeed)
{
  m_iSpeed += iSpeed;
}

void Car::DecreaseSpeed(int iSpeed)
{
  m_iSpeed -= iSpeed;
}

void Car::TurnLeft(int iAngle)
{
  m_iDirection -= iAngle;
}

void Car::TurnRight(int iAngle)
{
  m_iDirection += iAngle;
}

int Car::GetSpeed()
{
  return m_iSpeed;
}

int Car::GetDirection()
{
  return m_iDirection;
}
