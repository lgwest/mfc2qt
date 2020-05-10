#include "StdAfx.h"
#include "Color.h"

Color::Color()
 :m_crRedGreenBlue(0)
{
  // Empty.
}

Color::Color(const Color& color)
 :m_crRedGreenBlue(color.m_crRedGreenBlue)
{
  // Empty.
}

Color::Color(const COLORREF crRedGreenBlue)
 :m_crRedGreenBlue(crRedGreenBlue)
{
  // Empty.
}

Color::operator COLORREF () const
{
  return m_crRedGreenBlue;
}

Color& Color::operator=(const Color& color)
{
  if (this != &color)
  {
    m_crRedGreenBlue = color.m_crRedGreenBlue;
  }

  return *this;
}

void Color::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive << m_crRedGreenBlue;
  }

  if (archive.IsLoading())
  {
    archive >> m_crRedGreenBlue;
  }
}

Color Color::Inverse() const
{
  int iRed = GetRValue(m_crRedGreenBlue);
  int iGreen = GetGValue(m_crRedGreenBlue);
  int iBlue = GetBValue(m_crRedGreenBlue);

  return Color(RGB(255 - iRed, 255 - iGreen, 255 - iBlue));
}
