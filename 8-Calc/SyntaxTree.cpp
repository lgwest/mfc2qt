#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"
#include "Font.h"
#include "Color.h"
#include "Caret.h"
#include "Check.h"

#include "Reference.h"
#include "Token.h"
#include "SyntaxTree.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "CalcView.h"
#include "CalcDoc.h"

// The syntax tree must have a default constructor as it is serialized. The
// identity ST_EMPTY is not used in any other part of the application. Its
// purpose is to represent an empty syntax tree in the case of a cell holding
// a text or value, not a formula.

SyntaxTree::SyntaxTree()
 :m_eTreeId(ST_EMPTY)
{
  // Empty.
}

// The copy constructor and the assignment operator just call the copy
// method, which dynamically creates a copy of the given syntax tree.

SyntaxTree::SyntaxTree(const SyntaxTree& syntaxTree)
{
  CopySyntaxTree(syntaxTree);
}

SyntaxTree& SyntaxTree::operator=(const SyntaxTree&syntaxTree)
{
  if (this != &syntaxTree)
  {
    CopySyntaxTree(syntaxTree);
  }

  return *this;
}

void SyntaxTree::CopySyntaxTree(const SyntaxTree&syntaxTree)
{
  // First, we copy the identity of the syntax tree.
  m_eTreeId = syntaxTree.m_eTreeId;

  switch (m_eTreeId)
  {
    // In case of addition, subtraction, multiplication, and division, we 
    // dynamically create copies of the left and right sub trees with the
    // copy constructor of SyntaxTree.

    case ST_ADD:
    case ST_SUB:
    case ST_MUL:
    case ST_DIV:
      check_memory(m_pLeftTree = new SyntaxTree(*syntaxTree.m_pLeftTree));
      check_memory(m_pRightTree = new SyntaxTree(*syntaxTree.m_pRightTree));
      break;

    case ST_PARENTHESES:
      check_memory(m_pLeftTree = new SyntaxTree(*syntaxTree.m_pLeftTree));
      break;

    case ST_REFERENCE:
      m_reference = syntaxTree.m_reference;
      break;

    case ST_VALUE:
      m_dValue = syntaxTree.m_dValue;
      break;

    case ST_EMPTY:
      break;  
  }
}

// The constructors build the tree and allocate memory dynamically when
// needed. The destructor de-allocates that memory.

SyntaxTree::SyntaxTree(SyntaxTreeIdentity eTreeId, SyntaxTree* pLeftTree,
                       SyntaxTree* pRightTree)
 :m_eTreeId(eTreeId),
  m_pLeftTree(pLeftTree),
  m_pRightTree(pRightTree)
{
  // Empty.
}

SyntaxTree::SyntaxTree(double dValue)
 :m_eTreeId(ST_VALUE),
  m_dValue(dValue)
{
  // Empty.
}

SyntaxTree::SyntaxTree(Reference& reference)
 :m_eTreeId(ST_REFERENCE),
  m_reference(reference)
{
  // Empty.
}

// The task of the destructor is to de-allocate all dynamically allocated
// memory.

SyntaxTree::~SyntaxTree()
{
  switch (m_eTreeId)
  {
    case ST_ADD:
    case ST_SUB:
    case ST_MUL:
    case ST_DIV:
      delete m_pLeftTree;
      delete m_pRightTree;
      break;

    case ST_PARENTHESES:
      delete m_pLeftTree;
      break;
  }
}

// When the user input new data into a cell, the values of the cells
// referring to that cell (its target set) need to be evaluated. Evaluate is
// called on each referring cell. It calculates a value depending on the
// structure of the tree. If the formula of the cell has a reference, we need
// to look up its value, that why pCellMatrix is given as a parameter. If that
// cell referred to does not have a value, an exception is thrown. An
// exception is also thrown in the case of division by zero.

// If the parameter bRecursive is true, the user has cut and pasted a block of
// cells, in which case we have to recursively evaluate the values of the
// cells referred to by this syntax tree to catch the correct values.

double SyntaxTree::Evaluate(BOOL bRecursive, const CellMatrix* pCellMatrix)
const
{
  switch (m_eTreeId)
  {
    // In the case of addition, subtraction, or multiplication, we extract
    // the values of the left and right operand by calling Evaluate one the
    // sub trees. Then we carry out the operation and return the result.

    case ST_ADD:
      {
        double dLeftValue = m_pLeftTree->Evaluate(bRecursive, pCellMatrix);
        double dRightValue=m_pRightTree->Evaluate(bRecursive, pCellMatrix);
        return dLeftValue + dRightValue;
      }
      break;

    case ST_SUB:
      {
        double dLeftValue = m_pLeftTree->Evaluate(bRecursive, pCellMatrix);
        double dRightValue= m_pRightTree->Evaluate(bRecursive,pCellMatrix);
        return dLeftValue - dRightValue;
      }
      break;

    case ST_MUL:
      {
        double dLeftValue = m_pLeftTree->Evaluate(bRecursive, pCellMatrix);
        double dRightValue=m_pRightTree->Evaluate(bRecursive, pCellMatrix);
        return dLeftValue * dRightValue;
      }
      break;

    // In the case of division, we extract the values of the left and right
    // operand in a way similar toe the cases above. However, before we
    // carry out the division we have to check whether the right operand is
    // zero; if so, we throw(an exception instead of returning a value.

    case ST_DIV:
      {
        double dLeftValue = m_pLeftTree->Evaluate(bRecursive, pCellMatrix);
        double dRightValue= m_pRightTree->Evaluate(bRecursive,pCellMatrix);

        if (dRightValue != 0)
        {
          return dLeftValue / dRightValue;
        }
        else
        {
          CString stMessage = TEXT("#DIVISION_BY_ZERO");
          throw stMessage;
        }
      }
      break;

    // In the case of parenthesis, we just return the value. However, we still
    // need the parentheses case in order to generate the string of the
    // syntax tree. See ToString below.

    case ST_PARENTHESES:
      return m_pLeftTree->Evaluate(bRecursive, pCellMatrix);

    // If the referred cell has a value, it is returned. If not, an
    // exception is thrown.

    case ST_REFERENCE:
      {
        int iRow = m_reference.GetRow();
        int iCol = m_reference.GetCol();

        Cell* pCell = pCellMatrix->Get(iRow, iCol);

        if (pCell->HasValue(bRecursive))
        {
          return pCell->GetValue();
        }

        else
        {
          CString stMessage = TEXT("#MISSING_VALUE");
          throw stMessage;
        }
      }
      break;

    case ST_VALUE:
      return m_dValue;
  }

  // As all possible cases have been covered above, this point of the code
  // will never be reached. The assertion is for debugging purposes only.

  check(FALSE);
  return 0;
}

// The source set of a formula is the union of all its references. In the
// case of addition, subtraction, multiplication, and division, we return
// the union of the source sets of the two sub trees.

ReferenceSet SyntaxTree::GetSourceSet() const
{
  switch (m_eTreeId)
  {
    case ST_ADD:
    case ST_SUB:
    case ST_MUL:
    case ST_DIV:
      {
        ReferenceSet leftSet = m_pLeftTree->GetSourceSet();
        ReferenceSet rightSet = m_pRightTree->GetSourceSet();
        return ReferenceSet::Union(leftSet, rightSet);
      }

    case ST_PARENTHESES:
      return m_pLeftTree->GetSourceSet();

    case ST_REFERENCE:
      {
        ReferenceSet resultSet;
        resultSet.Add(m_reference);
        return resultSet;
      }

    default:
      ReferenceSet emptySet;
      return emptySet;
  }
}

// When the user cuts or copies a block of cells, and pastes it at another
// location in the spreadsheet, the references shall be updated as they
// are relative. UpdateReference takes care of that task. When it comes to
// the arithmetic operators, it just calls itself recursively on the left
// and right tree. The same goes for the expression surrounded by brackets,
// with the difference that it only examines the left tree. In the case of
// a reference, the row and column are updated and then the function checks
// the reference remains inside the spreadsheet.

void SyntaxTree::UpdateReference(int iRows, int iCols) 
{
  switch (m_eTreeId)
  {
    case ST_ADD:
    case ST_SUB:
    case ST_MUL:
    case ST_DIV:
      m_pLeftTree->UpdateReference(iRows, iCols);
      m_pRightTree->UpdateReference(iRows, iCols);
      break;

    case ST_PARENTHESES:
      m_pLeftTree->UpdateReference(iRows, iCols);

    case ST_REFERENCE:
      int iRow = m_reference.GetRow();
      int iCol = m_reference.GetCol();

      int iNewRow = iRow + iRows;
      int iNewCol = iCol + iCols;

      if ((iNewRow < 0) || (iNewRow >= ROWS) ||
          (iNewCol < 0) || (iNewCol >= COLS))
      {
        CString stMessage;
        stMessage.Format(TEXT("Invalid reference: \"%c%d\"."),
                         (TCHAR) (TEXT('a') + iNewCol), iNewRow + 1);
        throw stMessage;
      }

      m_reference.SetRow(iNewRow);
      m_reference.SetCol(iNewCol);
      break;
  }
}

// When the user has cut and pasted a cell, and by that action updated the
// rows and columns of the references in the formula of the cell, we need to
// generate a new string representing the formula. That is the task of
// ToString, it traverses the tree and generate a string for each part
// tree, which are joined into the final string.

CString SyntaxTree::ToString() const
{
  CString stResult;

  switch (m_eTreeId)
  {
    case ST_ADD:
      {
        CString stLeftTree = m_pLeftTree->ToString();
        CString stRightTree = m_pRightTree->ToString();
        stResult.Format(TEXT("%s+%s"), stLeftTree, stRightTree);
      }
      break;

    case ST_SUB:
      {
        CString stLeftTree = m_pLeftTree->ToString();
        CString stRightTree = m_pRightTree->ToString();
        stResult.Format(TEXT("%s-%s"), stLeftTree, stRightTree);
      }
      break;

    case ST_MUL:
      {
        CString stLeftTree = m_pLeftTree->ToString();
        CString stRightTree = m_pRightTree->ToString();
        stResult.Format(TEXT("%s*%s"), stLeftTree, stRightTree);
      }
      break;

    case ST_DIV:
      {
        CString stLeftTree = m_pLeftTree->ToString();
        CString stRightTree = m_pRightTree->ToString();
        stResult.Format(TEXT("%s/%s"), stLeftTree, stRightTree);
      }
      break;

    case ST_PARENTHESES:
      {
        CString stTree = m_pLeftTree->ToString();
        stResult.Format(TEXT("(%s)"), stTree);
      }
      break;

    case ST_REFERENCE:
      stResult = m_reference.ToString();
      break;

    case ST_VALUE:
      {
        stResult.Format(TEXT("%f"), m_dValue);
        stResult.TrimRight(TEXT('0'));
        stResult.TrimRight(TEXT('.'));
      }
      break;
  }

  return stResult;
}

// Serialize stores and loads the values of the object. We need to store
// and load the type of syntax tree (m_eTreeId). Then we serialize the sub
// trees, the reference, or the value, respectively. As the sub tree are
// created dynamically when they are needed, we use the check macro to
// make sure the memory is sufficient.

void SyntaxTree::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive << (int) m_eTreeId;

    switch (m_eTreeId)
    {
      case ST_ADD:
      case ST_SUB:
      case ST_MUL:
      case ST_DIV:
        m_pLeftTree->Serialize(archive);
        m_pRightTree->Serialize(archive);
        break;

      case ST_PARENTHESES:
        m_pLeftTree->Serialize(archive);
        break;

      case ST_REFERENCE:
        m_reference.Serialize(archive);
        break;

      case ST_VALUE:
        archive << m_dValue;
        break;
    }
  }

  if (archive.IsLoading())
  {
    int iTreeId;
    archive >> iTreeId;
    m_eTreeId = (SyntaxTreeIdentity) iTreeId;

    switch (m_eTreeId)
    {
      case ST_ADD:
      case ST_SUB:
      case ST_MUL:
      case ST_DIV:
        check_memory(m_pLeftTree = new SyntaxTree());
        check_memory(m_pRightTree = new SyntaxTree());

        m_pLeftTree->Serialize(archive);
        m_pRightTree->Serialize(archive);
        break;

      case ST_PARENTHESES:
        check_memory(m_pLeftTree = new SyntaxTree());
        m_pLeftTree->Serialize(archive);
        break;

      case ST_REFERENCE:
        m_reference.Serialize(archive);
        break;

      case ST_VALUE:
        archive >> m_dValue;
        break;
    }
  }
}
