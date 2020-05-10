#include <iostream>
using namespace std;

#include <cstdlib>
#include <cassert>

#include "Rational.h"

Rational::Rational(int iNumerator, int iDenumerator)
 :m_iNumerator(iNumerator),
  m_iDenumerator(iDenumerator)  
{
  assert(m_iDenumerator != 0);
  Normalize();
}

Rational::Rational(const Rational &rational)
 :m_iNumerator(rational.m_iNumerator),
  m_iDenumerator(rational.m_iDenumerator)  
{
  // Empty.
}

Rational Rational::operator=(const Rational &rational)
{
  if (this != &rational)
  {
    m_iNumerator = rational.m_iNumerator;
    m_iDenumerator = rational.m_iDenumerator;
  }

  return *this;
}

bool Rational::operator==(const Rational &rational) const
{
  return (m_iNumerator == rational.m_iNumerator) &&
         (m_iDenumerator == rational.m_iDenumerator);
}

bool Rational::operator!=(const Rational &rational) const
{
  return !operator==(rational);
}

bool Rational::operator<(const Rational &rational) const
{
  return m_iNumerator * rational.m_iDenumerator <
         rational.m_iNumerator * m_iDenumerator;
}

bool Rational::operator<=(const Rational &rational) const
{
  return operator<(rational) || operator==(rational);
}

bool Rational::operator>(const Rational &rational) const
{
  return !operator<=(rational);
}

bool Rational::operator>=(const Rational &rational) const
{
  return !operator<(rational);
}

Rational Rational::operator+(const Rational &rational) const
{
  int iResultNumerator = m_iNumerator * rational.m_iDenumerator +
                         rational.m_iNumerator * m_iDenumerator;
  int iResultDenumerator = m_iDenumerator * rational.m_iDenumerator;

  Rational result(iResultNumerator, iResultDenumerator);
  result.Normalize();
  return result;
}

Rational Rational::operator-(const Rational &rational) const
{
  int iResultNumerator = m_iNumerator * rational.m_iDenumerator -
                         rational.m_iNumerator * m_iDenumerator;
  int iResultDenumerator = m_iDenumerator * rational.m_iDenumerator;

  Rational result(iResultNumerator, iResultDenumerator);
  result.Normalize();
  return result;
}

Rational Rational::operator*(const Rational &rational) const
{
  int iResultNumerator = m_iNumerator * rational.m_iNumerator;
  int iResultDenumerator = m_iDenumerator * rational.m_iDenumerator;

  Rational result(iResultNumerator, iResultDenumerator);
  result.Normalize();
  return result;
}

Rational Rational::operator/(const Rational &rational) const
{
  assert(rational.m_iNumerator != 0);
 
  int iResultNumerator = m_iDenumerator * rational.m_iDenumerator;
  int iResultDenumerator = m_iNumerator * rational.m_iNumerator;

  Rational result(iResultNumerator, iResultDenumerator);
  result.Normalize();
  return result;
}

istream &operator>>(istream &inputStream, Rational &rational)
{
  inputStream >> rational.m_iNumerator >> rational.m_iDenumerator;
  return inputStream;
}

ostream &operator<<(ostream &outputStream, const Rational &rational)
{
  if (rational.m_iNumerator == 0)
  {
    outputStream << "0";
  }

  else if (rational.m_iDenumerator == 1)
  {
    outputStream << "1";
  }

  else
  {
    outputStream << "(" << rational.m_iNumerator << "/"
                 << rational.m_iDenumerator << ")";
  }

  return outputStream;
}

void Rational::Normalize()
{
  if (m_iNumerator == 0)
  {
    m_iDenumerator = 1;
    return;
  }

  if (m_iDenumerator < 0)
  {
    m_iNumerator = -m_iNumerator;
    m_iDenumerator = -m_iDenumerator;
  }

  int iGcd = GreatestCommonDivider(abs(m_iNumerator), m_iDenumerator);

  m_iNumerator /= iGcd;
  m_iDenumerator /= iGcd;
}

int Rational::GreatestCommonDivider(int iNum1, int iNum2)
{
  if (iNum1 > iNum2)
  {
    return GreatestCommonDivider(iNum1 - iNum2, iNum2);
  }

  else if (iNum2 > iNum1)
  {
    return GreatestCommonDivider(iNum1, iNum2 - iNum1);
  }

  else
  {
    return iNum1;
  }
}
