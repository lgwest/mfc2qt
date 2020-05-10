#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"
#include "Font.h"
#include "Color.h"
#include "Caret.h"
#include "Check.h"

#include "Reference.h"
#include "SyntaxTree.h"

#include "Token.h"
#include "Scanner.h"

#include "Parser.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "CalcView.h"
#include "CalcDoc.h"

// Formula is the start method of the class. It is called in order to
// interpret the text the user has input.

SyntaxTree Parser::Formula(const CString& stBuffer)
{
  // The input string is saved in case we need it in error messages.
  m_stBuffer = stBuffer;

  // We scan the input string, receive the token list, and initialize the
  // first token in the list. Even if the input string is completely empty,
  // there is still the token T_EOL in the list.

  Scanner scanner(m_stBuffer);
  m_pTokenList = scanner.GetTokenList();
  m_nextToken = m_pTokenList->GetHead();

  // We parse the token list and receive a pointer to a syntax tree. If
  // there was a parse error; instead, an exception is thrown. When the
  // token list has been parsed, we have to make there is no extra token
  // left in the list except for the end-of-line.

  SyntaxTree* pExpr = Expression();
  Match(T_EOL);

  // For the purpose of avoiding a classic mistake (dangling pointers), we
  // create and return a static syntax tree, which is initialized with the
  // pointer generated from the parsing. We also delete the generated
  // syntax tree in order to avoid another classic mistake (memory leaks).

  SyntaxTree syntaxTree(*pExpr);
  delete pExpr;
  return syntaxTree;
}

// Match is used to match the next token with the expected one. If they do
// not match, an exception is thrown. Otherwise, the next token is removed
// from the list and if the there is another token in the list, is becomes
// the next one.

void Parser::Match(TokenIdentity eTokenId)
{
  if (m_nextToken.GetId() != eTokenId)
  {
    CString stMessage;
    stMessage.Format(TEXT("Invalid Expression: \"") + m_stBuffer + TEXT("\"."));
    throw stMessage;
  }

  m_pTokenList->RemoveHead();

  if (!m_pTokenList->IsEmpty())
  {
    m_nextToken = m_pTokenList->GetHead();
  }
}

// The rest of the functions implement the grammar above, there is one
// function for each for the symbols Formula, Expression, NextExpression,
// Term, NextTerm, and Factor.

SyntaxTree* Parser::Expression()
{
  SyntaxTree* pTerm = Term();
  SyntaxTree* pNextExpression = NextExpression(pTerm);
  return pNextExpression;
}

// NextExpression takes care of addition and subtraction. If the next token is
// T_ADD or T_SUB, we match the operator and parse its right operand. Then
// we create and return a new syntax tree with the operator in question.
// If the next token is neither T_ADD nor T_SUB, we just assume that the
// this rule does not apply and return the given left syntax tree.

SyntaxTree* Parser::NextExpression(SyntaxTree* pLeftTerm)
{
  switch (m_nextToken.GetId())
  {
    case T_ADD:
      {
        Match(T_ADD);
        SyntaxTree *pRightTerm = Term(), *pResult;
        check_memory(pResult = new SyntaxTree(ST_ADD, pLeftTerm,pRightTerm));
        SyntaxTree* pNextExpression = NextExpression(pResult);
        return pNextExpression;
      }
      break;

    case T_SUB:
      {
        Match(T_SUB);
        SyntaxTree *pRightTerm = Term(), *pResult;
        check_memory(pResult = new SyntaxTree(ST_SUB, pLeftTerm,pRightTerm));
        SyntaxTree* pNextExpression = NextExpression(pResult);
        return pNextExpression;
      }
      break;

    default:
      return pLeftTerm;
  }
}

SyntaxTree* Parser::Term()
{
  SyntaxTree* pFactor = Factor();
  SyntaxTree* pNextTerm = NextTerm(pFactor);
  return pNextTerm;
}

// NextTerm works in a way similar to NextExpression above, with the
// difference that it handles multiplication and division. It calls Factor
// instead of Term to parse to right operand.

SyntaxTree* Parser::NextTerm(SyntaxTree* pLeftFactor)
{
  switch (m_nextToken.GetId())
  {
    case T_MUL:
      {
        Match(T_MUL);
        SyntaxTree *pRightFactor = Factor(), *pResult;
        check_memory(pResult = new SyntaxTree
                     (ST_MUL, pLeftFactor, pRightFactor));
        SyntaxTree* pNextTerm = NextTerm(pResult);
        return pNextTerm;
      }
      break;

    case T_DIV:
      {
        Match(T_DIV);
        SyntaxTree *pRightFactor = Factor(), *pResult;
        check_memory(pResult = new SyntaxTree
                     (ST_DIV, pLeftFactor, pRightFactor));
        SyntaxTree* pNextTerm = NextTerm(pResult);
        return pNextTerm;
      }
      break;

    default:
      return pLeftFactor;
  }
}

// Factor parses values, references, and expression surrounded by
// parentheses.

SyntaxTree* Parser::Factor()
{
  switch (m_nextToken.GetId())
  {
     // If the next token is a left parenthesis, we match it and parse the
     // following expression as well as the closing right parenthesis.

    case T_LEFT_PAREN:
      {
        Match(T_LEFT_PAREN);
        SyntaxTree *pExpr = Expression(), *pResult;
        check_memory(pResult = new SyntaxTree(ST_PARENTHESES, pExpr, NULL));
        Match(T_RIGHT_PAREN);
        return pResult;
      }
      break;

    // If the next token is a reference or a value, we match it.

    case T_REFERENCE:
      {
        // We receive the reference attribute with its row and column and
        // match the reference token.

        Reference reference = m_nextToken.GetReference();
        Match(T_REFERENCE);

        int iRow = reference.GetRow();
        int iCol = reference.GetCol();

        // If the user has given an reference outside the spread sheet, an
        // exception is thrown.

        if ((iRow < 0) || (iRow >= ROWS) || (iCol < 0) || (iCol >= COLS))
        {
          CString stMessage = TEXT("Reference Out Of Range: \"") + m_stBuffer + TEXT("\".");
          throw stMessage;
        }

        // We create and return a new syntax tree holding the reference.

        check_memory(return (new SyntaxTree(reference)));
      }
      break;

    case T_VALUE:
      {
        // First we receive the reference attribute with its value and match
        // the value token. Then we create and return a new syntax tree
        // holding the value.

        double dValue = m_nextToken.GetValue();
        Match(T_VALUE);

        check_memory(return (new SyntaxTree(dValue)));
      }
      break;

    // If none of the tokens above applies, the user has input an invalid
    // expression.

    default:
      CString stMessage = TEXT("Invalid Expression: \"") + m_stBuffer + TEXT("\".");
      throw stMessage;
      break;
  }
}
