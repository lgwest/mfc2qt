#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"

#include "Reference.h"
#include "Token.h"

// There are five constructors altogether. The default constructor is
// necessary because we store tokens in a list, we also have a token field in
// the Parser class. The other three constructors are used by the scanner to
// create tokens with  or without attributes.

Token::Token()
 :m_eTokenId(T_EOL),
  m_dValue(0)
{
  // Empty
}

// The copy constructor and the assignment operator copy the field of the
// class.

Token::Token(const Token& token)
 :m_eTokenId(token.m_eTokenId),
  m_dValue(token.m_dValue),
  m_reference(token.m_reference)
{
  // Empty.
}

Token Token::operator=(const Token& token)
{
  if (this != &token)
  {
    m_eTokenId = token.m_eTokenId;
    m_dValue = token.m_dValue;
    m_reference = token.m_reference;
  }

  return *this;
}

// This constructor is called when a token object of identity T_VALUE with
// attribute value is created.

Token::Token(double dValue)
 :m_eTokenId(T_VALUE),
  m_dValue(dValue)
{
  // Empty.
}

// This constructor is called when a token object of identity T_REFERENCE
// with attribute reference is created.

Token::Token(Reference reference)
 :m_eTokenId(T_REFERENCE),
  m_reference(reference),
  m_dValue(0)
{
  // Empty
}

// This constructor is called when a token object without attribute is
// created.

Token::Token(TokenIdentity eTokenId)
 :m_eTokenId(eTokenId),
  m_dValue(0)
{
  // Empty.
}
