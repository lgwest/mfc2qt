#include "StdAfx.h"
#include "Position.h"

// The constructors initializes the paragraph and character fields.

Position::Position(int iParagraph, int iCharacter)
 :m_iParagraph(iParagraph),
  m_iCharacter(iCharacter)
{
  // Empty.
}

Position::Position(const Position& position)
 :m_iParagraph(position.m_iParagraph),
  m_iCharacter(position.m_iCharacter)
{
  // Empty.
}

Position& Position::operator=(const Position& position)
{
  if (this != &position)
  {
    m_iParagraph = position.m_iParagraph;
    m_iCharacter = position.m_iCharacter;
  }

  return *this;
}

// Two positions are equal if they have the same paragraph and character.

BOOL Position::operator==(const Position& position) const
{
  return (m_iParagraph == position.m_iParagraph) &&
         (m_iCharacter == position.m_iCharacter);
}

// In order to test whether two positions are not equal, we can call the
// equality operator.

BOOL Position::operator!=(const Position& position) const
{
  return !(*this == position);
}

// This position is less than the given one if the paragraph is less than
// the given one, or if the paragraphs are equal and the character is less
// the given one.

BOOL Position::operator<(const Position& position) const
{
  return (m_iParagraph < position.m_iParagraph) ||
         ((m_iParagraph == position.m_iParagraph) &&
          (m_iCharacter < position.m_iCharacter));
}

// In order to test whether this positions is greater than the given one,
// we can call the equality and less-than operators.

BOOL Position::operator>(const Position& position) const
{
  return !operator==(position) && !operator<(position);
}
