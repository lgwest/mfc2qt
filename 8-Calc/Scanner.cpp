#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"

#include "Reference.h"
#include "Token.h"
#include "Scanner.h"

// The constructor takes a string as parameter and generates m_tokenList by
// repeatedly calling NextToken until the input string is empty. A null
// character (’\0’) is added to the string by the constructor in order not to
// have to check for the end of the text. NextToken returns EOL (End of Line)
// when it encounters the end of the string.

Scanner::Scanner(const CString& m_stBuffer)
 :m_stBuffer(m_stBuffer + TEXT('\0'))
{
  Token token;

  do
  {
    token = NextToken();
    m_tokenList.AddTail(token);
  }
  while (token.GetId() != T_EOL);
}

// NextToken does the actual work of the scanner and divides the text into
// token, one by one. First, it skips the blanks and tabulators. It is
// rather simple to extract the token regarding the arithmetic symbols and
// the parentheses, we just have to check the next character of the buffer.
// It becomes slightly more difficult when it comes to numerical values,
// references, or text. We have two auxiliary functions for that purpose,
// ScanValue and ScanReference.

Token Scanner::NextToken()
{
  // Sequences of spaces and tabulators, known as white spaces, are
  // removed before the actual scanning begins.

  while ((m_stBuffer[0] == TEXT(' ')) || (m_stBuffer[0] == TEXT('\t')))
  {
    m_stBuffer.Delete(0);
  }

  // The first cases are rather trivial, they just check the next character
  // in the input string.

  switch (m_stBuffer[0])
  {
    case TEXT('\0'):
      return Token(T_EOL);

    case TEXT('+'):
      {
        double dValue;

        if (ScanValue(dValue))
        {
          return Token(dValue);
        }

        else
        {
          m_stBuffer.Delete(0);
          return Token(T_ADD);
        }
      }

    case TEXT('-'):
      {
        double dValue;
        
        if (ScanValue(dValue))
        {
          return Token(dValue);
        }

        else
        {
          m_stBuffer.Delete(0);
          return Token(T_SUB);
        }
      }

    case TEXT('*'):
      m_stBuffer.Delete(0);
      return Token(T_MUL);

    case TEXT('/'):
      m_stBuffer.Delete(0);
      return Token(T_DIV);

    case TEXT('('):
      m_stBuffer.Delete(0);
      return Token(T_LEFT_PAREN);

    case TEXT(')'):
      m_stBuffer.Delete(0);
      return Token(T_RIGHT_PAREN);

    // If none of the above cases apply, the token may be a value or a
    // reference. The two methods ScanValue and ScanReference finds out
    // if that is the case. If not, the scanner has encountered an unknown
    // character and an exception is thrown.

    default:
      double dValue;
      Reference reference;

      if (ScanValue(dValue))
      {
        return Token(dValue);
      }

      else if (ScanReference(reference))
      {
        return Token(reference);
      }

      else
      {
        CString stMessage;
        stMessage.Format(TEXT("Unknown character: \"%c\"."), m_stBuffer[0]);
        throw stMessage;
      }
      break;
  }
}

// ScanValue first checks that the next character is a digit or a plus or
// minus sign followed by a digit. Then it scans the characters as long it
// founds digits, and if it finds a decimal dot it continues scanning for
// the decimal part of the value.

BOOL Scanner::ScanValue(double& dValue)
{
  CString stValue = ScanSign();
  stValue.Append(ScanDigits());

  if (m_stBuffer[0] == TEXT('.'))
  {
    m_stBuffer.Delete(0);
    stValue += TEXT('.') + ScanDigits();
  }

  if (stValue.FindOneOf(TEXT("0123456789")) != -1)
  {
    dValue = _tstof(stValue);
    return TRUE;
  }

  else
  {
    m_stBuffer.Insert(0, stValue);
    return FALSE;
  }
}

// ScanReference checks that the next character is a letter and that the
// characters thereafter is a sequence of at least one digit. If so, we
// extracts the column and the row of the reference.

BOOL Scanner::ScanReference(Reference& reference)
{
  if (isalpha(m_stBuffer[0]) && isdigit(m_stBuffer[1]))
  {
    reference.SetCol(tolower(m_stBuffer[0]) - TEXT('a'));
    m_stBuffer.Delete(0);
    CString stRow = ScanDigits();
    reference.SetRow(_tstoi(stRow) - 1);
    return TRUE;
  }

  return FALSE;
}

CString Scanner::ScanSign()
{
  if ((m_stBuffer[0] == TEXT('+')) ||
      (m_stBuffer[0] == TEXT('-')))
  {
    CString stSign = m_stBuffer.Mid(0, 1);
    m_stBuffer.Delete(0);
    return stSign;
  }

  return CString();
}

CString Scanner::ScanDigits()
{
  int iCount;
  for (iCount = 0; isdigit(m_stBuffer[iCount]); ++iCount)
  {
    // Empty.
  }

  CString stDigits = m_stBuffer.Mid(0, iCount);
  m_stBuffer.Delete(0, iCount);
  return stDigits;
}
