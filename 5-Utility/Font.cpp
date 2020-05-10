#include "StdAfx.h"
#include "Font.h"

Font::Font()
{
  ::memset(&m_logFont, 0, sizeof m_logFont);
}

Font::Font(CString stName, int iSize)
{
  ::memset(&m_logFont, 0, sizeof m_logFont);
  strcpy_s(m_logFont.lfFaceName, stName);
  m_logFont.lfHeight = iSize;
}

Font::Font(const LOGFONT& logFont)
 :m_logFont(logFont)
{
  // Empty.
}

Font::Font(const Font& font)
 :m_logFont(font.m_logFont)
{
  // Empty.
}

Font Font::PointsToMeters() const
{
  LOGFONT logFont = m_logFont;

  logFont.lfWidth = (int) ((double) 2540 * logFont.lfWidth / 72);
  logFont.lfHeight = (int) ((double) 2540 * logFont.lfHeight / 72);

  return Font(logFont);
}

Font& Font::operator=(const Font& font)
{
  if (this != &font)
  {
    m_logFont = font.m_logFont;
  }

  return *this;
}

BOOL Font::operator==(const Font& font) const
{
  return (::memcmp(&m_logFont, &font.m_logFont, sizeof m_logFont) == 0);
}

BOOL Font::operator!=(const Font& font) const
{
  return !operator==(font);
}

void Font::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive.Write(&m_logFont, sizeof m_logFont);
  }

  if (archive.IsLoading())
  {
    archive.Read(&m_logFont, sizeof m_logFont);
  }
}
