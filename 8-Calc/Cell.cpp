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

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "Token.h"
#include "Scanner.h"
#include "Parser.h"

#include "CalcView.h"
#include "CalcDoc.h"

// A newly created cell is empty, have cell style text, is centered both in
// horizontal and vertical view, and have black text color on white
// background. Remember that the caret array contains one rectangle for
// each character together with one extra one for the caret position one
// step to the right of the text. So even thought the text is empty, we add
// one empty rectangle to the caret array.

Cell::Cell()
 :m_eCellState(CELL_TEXT),
  m_eHorizontalAlignment(HALIGN_CENTER),
  m_eVerticalAlignment(VALIGN_CENTER),
  m_textColor(BLACK),
  m_backgroundColor(WHITE),
  m_pCellMatrix(NULL),
  m_pTargetSetMatrix(NULL)
{
  m_caretRectArray.Add(CRect(0, 0, 0, 0));
}

// The copy constructor and the assignment operator simply call the copy
// method, which copies every field. As every field is static (there are no
// pointers in this class and we have no need for dynamically created memory)
// we can just copy the fields one by one. The MFC class CArray, however, does
// not have an overloaded assignment operator. Instead, we call Copy, which
// copies the array.

Cell::Cell(const Cell& cell)
{
  CopyCell(cell);
}

Cell& Cell::operator=(const Cell& cell)
{
  if (this != &cell)
  {
    CopyCell(cell);
  }

  return *this;
}

void Cell::CopyCell(const Cell& cell)
{
  m_eCellState = cell.m_eCellState;

  m_syntaxTree = cell.m_syntaxTree;
  m_sourceSet = cell.m_sourceSet;

  m_stText = cell.m_stText;
  m_dValue = cell.m_dValue;

  m_stInput = cell.m_stInput;
  m_stOutput = cell.m_stOutput;

  m_eHorizontalAlignment = cell.m_eHorizontalAlignment;
  m_eVerticalAlignment = cell.m_eVerticalAlignment;

  m_textColor = cell.m_textColor;
  m_backgroundColor = cell.m_backgroundColor;

  m_font = cell.m_font;
  m_caretRectArray.Copy(cell.m_caretRectArray);
}

// Clear clears the cell. It is called when the user deletes one or several
// cells. If the cell contains a formula, we first have to go thought its
// source set and for each source cell in the set, we remove this cell as a
// target by calling RemoveTargets.

void Cell::Clear(Reference home)
{
  if (m_eCellState == CELL_FORMULA)
  {
    m_pTargetSetMatrix->RemoveTargets(home);
  }

  m_eCellState = CELL_TEXT;
  m_stText = TEXT("");
  m_stOutput = TEXT("");
}

// Serialize is called for each cell in the cell matrix when the user saves
// the spreadsheet. It is rather straightforward, the Font, Color, CArray,
// SyntaxTree, and Set classes have their own implementations of Serialize.
// The rest of the fields are stored or loaded, the stream operators << and >>
// are overloaded for the MFC CArchive class.

void Cell::Serialize(CArchive& archive)
{
  m_font.Serialize(archive);
  m_textColor.Serialize(archive);
  m_backgroundColor.Serialize(archive);
  m_caretRectArray.Serialize(archive);
  m_syntaxTree.Serialize(archive);
  m_sourceSet.Serialize(archive);

  if (archive.IsStoring())
  {
    archive << (int) m_eCellState << m_stText << m_dValue << m_stOutput
            << (int) m_eHorizontalAlignment << (int) m_eVerticalAlignment;
  }

  if (archive.IsLoading())
  {
    int iCellState, iHorizontalAlignment, iVerticalAlignment;
    archive >> iCellState >> m_stText >> m_dValue >> m_stOutput
            >> iHorizontalAlignment >> iVerticalAlignment;

    m_eCellState = (CellState) iCellState;
    m_eHorizontalAlignment = (Alignment) iHorizontalAlignment;
    m_eVerticalAlignment = (Alignment) iVerticalAlignment;
  }
}

// CharDown is called every time the user presses a printable character. If
// the correct edit index is at the end of the text (that is, one step to the
// right of the text), we just add the character. If not, we have to take
// into consideration the keyboard mode, it can be in either insert or
// overwrite mode. In the case of insert mode, we insert the character. In
// case of overwrite mode, we overwrite the old character at the edit
// index.

void Cell::CharDown(UINT uNewChar, int iInputIndex, KeyboardState eKeyBoardMode)
{
  // If the input index is at the end of the text (to the right of the last
  // character), we just add it to the text regardless of whether the
  // keyboard is in insert or overwrite mode.

  if (iInputIndex == m_stInput.GetLength())
  {
     m_stInput.AppendChar((TCHAR) uNewChar);
  }

  // If the input index is not at the end of the text, we have to take into
  // consideration the keyboard mode.

  else
  {
    switch (eKeyBoardMode)
    {
      // If the keyboard is in insert mode, we insert the new character at 
      // the input index.

      case KM_INSERT:
        m_stInput.Insert(iInputIndex, (TCHAR) uNewChar);
        break;

      // If the keyboard is in overwrite mode, we overwrite the character 
      // at the input index by calling SetAt.

      case KM_OVERWRITE:
        m_stInput.SetAt(iInputIndex, (TCHAR) uNewChar);
        break;            
    }
  }
}

// When the user adds or removes a character of the text of a cell, the
// position of the caret must be updated. GenerateCaretArray takes care of
// that. Note that this is necessary only when the cell has input focus.

void Cell::GenerateCaretArray(CDC* pDC)
{
  // We create, initialize, and select the font of the cell text. Remember
  // that the font is stored in typographical point, which have to be
  // converted to logical units (hundreds of millimeters) by calling
  // PointsToMeters.
 
  CFont cFont;
  cFont.CreateFontIndirect(m_font.PointsToMeters());
  CFont* pPrevFont = pDC->SelectObject(&cFont);

  // First, we need the width and height of the text (in logical units).
  // If the text is non-empty, we call GetTextExtent to measure the input
  // text.

  int iTextWidth, iTextHeight;
  if (!m_stInput.IsEmpty())
  {
    CSize szText = pDC->GetTextExtent(m_stInput);  
    iTextWidth = szText.cx;
    iTextHeight = szText.cy;
  }

  // If the text is empty, the width is zero and the height is set to the
  // height of the font by calling GetTextMetric.

  else
  {
    TEXTMETRIC textMetric;
    pDC->GetTextMetrics(&textMetric);
    iTextWidth = 0;
    iTextHeight = textMetric.tmHeight;
  }

  // The writable part of the cell area is slightly smaller than the
  // cell area in order to prevent the text to overwrite the cell borders.
  // The width and height of the cell is subtracted by the margin.

  const int CELL_WIDTH = COL_WIDTH - 2 * CELL_MARGIN;
  const int CELL_HEIGHT = ROW_HEIGHT - 2 * CELL_MARGIN;

  // The beginning of the text (xLeftPos) shall be decided. The horizontal 
  // alignment can be set in four different modes: left, centered, right,
  // and justified mode.

  // The variables are initialized in order to avoid compiler warnings.
  int xLeftPos = 0;    // The start position of the text in horizontal direction.
  int iSpaceWidth = 0; //The width of a space in justified horizontal alignment.
  int yTopPos = 0;     //The start position of the text in vertical direction.

  switch (m_eHorizontalAlignment)
  {
    // In case of left alignment, the text starts at the beginning of the
    // cell; so the position is set to zero.

    case HALIGN_LEFT:
      xLeftPos = 0;
      break;

    // In case of centered alignment, the text shall be placed in the 
    // middle of the cell. 

    case HALIGN_CENTER:
      xLeftPos = (CELL_WIDTH - iTextWidth) / 2;
      break;

    // In case of right alignment, the text shall be placed in the 
    // middle of the cell. 

    case HALIGN_RIGHT:
      xLeftPos = CELL_WIDTH - iTextWidth;
      break;

    // In case of left alignment, the text shall be equally divided along
    // the cell by stretching the spaces in the text. For that purpose, we
    // need to count the number of spaces in the text by calling Remove. 

    case HALIGN_JUSTIFIED:
      CString stInputNoSpaces = m_stInput;
      int iSpaceCount = stInputNoSpaces.Remove(TEXT(' '));

      // If there is at least one space in the text, we decide the width of         
      // each space by subtracting the width of the text without spaces
      // from the area width and then divide with the number of spaces.

      if (iSpaceCount > 0)
      {
        xLeftPos = 0;
        CSize szInputNoSpaces = pDC->GetTextExtent(stInputNoSpaces );
        iSpaceWidth = (CELL_WIDTH  - szInputNoSpaces.cx) / iSpaceCount;
      }

      // If there are no spaces in the text, we regard the alignment to
      // be left, so the start position is zero.

      else
      {
        xLeftPos = 0;
      }
      break;
  }

  // The top position of the text (yTopPos) is assessed in a way
  // similar to the horizontal alignment. The vertical alignment can be set
  // in three different modes: top, centered, and bottom mode.

  switch (m_eVerticalAlignment)
  {
    // In case of top alignment, the text starts at the beginning of the
    // cell, so the position is set to zero.

    case VALIGN_TOP:
      yTopPos = 0;
      break;

    case VALIGN_CENTER:
      yTopPos = (CELL_HEIGHT - iTextHeight) / 2;
      break;

    case VALIGN_BOTTOM:
      yTopPos = CELL_HEIGHT - iTextHeight;
      break;
  }

  // Finally, the text is traversed and the rectangle for each character is
  // calculated. The start position in the y direction and the height of
  // the text is the same for all characters. The position in the x
  // direction start by the position calculated above, and is increased for
  // each character. If the character is a space and the text has
  // justified horizontal alignment, we use the space width calculated
  // above. Otherwise, we call GetTextExtent to get the size width of the
  // character.

  int xPos = xLeftPos;
  int iLength = m_stInput.GetLength();
  m_caretRectArray.SetSize(iLength + 1);

  for (int iIndex = 0; iIndex < iLength; ++iIndex)
  {
    CString stChar = m_stInput.Mid(iIndex, 1);
    int iCharWidth;

    if ((stChar == TEXT(" ")) && (m_eHorizontalAlignment == HALIGN_JUSTIFIED))
    {
      iCharWidth = iSpaceWidth;
    }
    else
    {
      iCharWidth = pDC->GetTextExtent(stChar).cx;
    }

    CRect rcChar(xPos, yTopPos, xPos + iCharWidth, yTopPos + iTextHeight);
    m_caretRectArray[iIndex] = rcChar;
    xPos += iCharWidth;
  }

  // Finally, we add the size of a character of average size because the user
  // may put the caret marker to the right of the last character.

  TEXTMETRIC textMetric;
  pDC->GetTextMetrics(&textMetric);
  int iAverageCharWidth = textMetric.tmAveCharWidth;

  CRect rcLastChar(xPos, yTopPos, xPos + iAverageCharWidth,
                   yTopPos + iTextHeight);
  m_caretRectArray[iLength] = rcLastChar;
  pDC->SelectObject(pPrevFont);
}

// MouseToIndex examines the text of the cell with the help of the caret
// array and finds the index of the matching character.

int Cell::MouseToIndex(CPoint ptMouse)
{
  ptMouse -= CSize(CELL_MARGIN, CELL_MARGIN);
  int iLength = m_stInput.GetLength();

  // If the mouse position in the x direction is to the left of leftmost
  // character, we return index zero.

  if (ptMouse.x < m_caretRectArray[0].left)
  {
    return 0;
  }

  // We traverse the rectangle array of the text, and return the index for
  // the character whose rectangle includes the x position of the mouse.

  for (int iIndex = 0; iIndex < iLength; ++iIndex)
  {
    if ((ptMouse.x >= m_caretRectArray[iIndex].left) &&
        (ptMouse.x < m_caretRectArray[iIndex].right))
    {
      return iIndex;
    }
  }

  // If we have not found the index so far, it is located to the right of the
  // text, so we return the length of the text (the position to the right of
  // the text.

  return iLength;
}

// IndexToCaret is called to find out the caret position of the character
// with the given index.

CRect Cell::IndexToCaret(int iIndex)
{
  CSize szMargin(CELL_MARGIN, CELL_MARGIN);
  return szMargin + m_caretRectArray[iIndex];
}

// The drawing of the text is rather straightforward, we just simple state the
// horizontal and vertical alignment together with the dimensions of the cell
// when we call DrawText. However, there is one exception: the justified
// alignment. In order to obtain justified horizontal alignment, we call the
// device context method SetTextJustification that makes the text in the
// DrawText call be equally distributed in the cell.

void Cell::Draw(CPoint ptTopLeft, BOOL bEdit, BOOL bMarked, CDC *pDC)
{
  // In order not to overwrite the border of the cell, we introduce a cell
  // margin.

  CRect rcCell(ptTopLeft, CSize(COL_WIDTH, ROW_HEIGHT));
  CRect rcMargin(rcCell.left + CELL_MARGIN, rcCell.top + CELL_MARGIN,
                 rcCell.right - CELL_MARGIN, rcCell.bottom - CELL_MARGIN);

  // The pen and background colors are inversed if the cell is in edit or 
  // marked mode.

  Color penColor = (bEdit || bMarked) ? m_textColor.Inverse()
                                      : m_textColor;
  Color brushColor  = (bEdit || bMarked) ? m_backgroundColor.Inverse()
                                         : m_backgroundColor;

  // The pen and brush are selected to the device context, and the cell is
  // painted.

  CPen pen(PS_SOLID, 0, penColor);
  CPen* pPrevPen = pDC->SelectObject(&pen);

  CBrush brush(brushColor);
  CBrush* pPrevBrush = pDC->SelectObject(&brush);

  pDC->Rectangle(rcCell);

  // When the pen and brush have been used, we should select the previous
  // pen and brush to the device context.

  pDC->SelectObject(pPrevPen);
  pDC->SelectObject(pPrevBrush);

  // In order to draw text we set the text and background color instead of
  // selecting a pen and brush.

  pDC->SetTextColor(penColor);
  pDC->SetBkColor(brushColor);

  // We need also to set the text font. As the size of the font is stored in
  // typographical points, we convert the size to logical units (hundreds of
  // millimeters) by calling PointsToMeters before we select the font to the
  // device context.

  CFont cFont;
  cFont.CreateFontIndirect(m_font.PointsToMeters());
  CFont* pPrevFont = pDC->SelectObject(&cFont);

  // If the cell is in edit mode, we choose to display the input text;
  // otherwise, we display the output text.

  CString stDisplay = bEdit ? m_stInput : m_stOutput;

  // If the text has justified horizontal alignment, we have to set the
  // space distribution by calling SetTextJustification. After the call to
  // DrawText, we should reset the space distribution. 

  if (m_eHorizontalAlignment == HALIGN_JUSTIFIED)
  {
    CString stTemp = stDisplay;
    int iSpaceCount = stTemp.Replace(TEXT(' '), TEXT('.'));

    CSize szDisplay = pDC->GetTextExtent(stDisplay);
    pDC->SetTextJustification(rcMargin.Width() - szDisplay.cx,iSpaceCount);

    pDC->DrawText(stDisplay, &rcMargin,
                  DT_SINGLELINE | m_eVerticalAlignment);
    pDC->SetTextJustification(0, 0);
  }

  // If the cell is in left, center, or right horizontal mode, we just draw
  // the text with the horizontal and vertical alignment of the cell.

  else
  {
    pDC->DrawText(stDisplay, &rcMargin, DT_SINGLELINE |
                  m_eHorizontalAlignment | m_eVerticalAlignment);
  }

  // Similar to the pen and brush above, we should select the previous
  // font.

  pDC->SelectObject(pPrevFont);
}


// GetAlignment and SetAlignment return and set the alignment of the given
// direction.

Alignment Cell::GetAlignment(Direction eDirection) const
{
  switch (eDirection)
  {
    case HORIZONTAL:
        return m_eHorizontalAlignment;

    case VERTICAL:
        return m_eVerticalAlignment;
  }

  return (Alignment) 0;
}

void Cell::SetAlignment(Direction eDirection, Alignment eAlignment)
{
  switch (eDirection)
  {
    case HORIZONTAL:
      m_eHorizontalAlignment = eAlignment;
      break;

   case VERTICAL:
     m_eVerticalAlignment = eAlignment;
     break;
  }
}

// GetColor and SetColor return and set the text and background color.

Color Cell::GetColor(int iColorType) const
{
  switch (iColorType)
  {
    case TEXT: 
      return m_textColor;

    case VERTICAL:
      return m_backgroundColor;
  }

  return 0;
}

void Cell::SetColor(int iColorType, const Color& color)
{
  switch (iColorType)
  {
    case HORIZONTAL:
      m_textColor = color;
      break;

    case VERTICAL:
      m_backgroundColor = color;
      break;
  }
}

// GenerateInputText is called by the document class when the user wants to
// start editing a cell. We have to find the input text of the cell, which is
// stored in the field m_stInput.

void Cell::GenerateInputText()
{
  switch (m_eCellState)
  {
    // If the cell is in text mode, we just use the value of m_stText.

    case CELL_TEXT:
      m_stInput = m_stText;
      break;

    // If the cell is in value mode, we convert the value to a text by
    // calling Format in the MFC CString class. We also remove ending zeros
    // and the dot if there are no decimals.

    case CELL_VALUE:
      m_stInput.Format(TEXT("%f"), m_dValue);
      m_stInput.TrimRight(TEXT('0'));
      m_stInput.TrimRight(TEXT('.'));
      break;

    // If the cell is in formula mode, we call the syntax tree to evaluate
    // a string matching the formula. We also introduce an equals sign.

    case CELL_FORMULA:
      m_stInput = TEXT("=") + m_syntaxTree.ToString();
      break;
  }
}

// EditEnd is call by the document class when the user presses the return or
// tab key, or presses the mouse. We interpret the input or editing of the
// cell. Depending of the text, the input cell may be set in text, value, or
// formula mode.

void Cell::EndEdit(Reference home)
{
  // First, we get rid of trailing blanks in order to decide whether the
  // first character is an equals sign.

  CString stTrimInput = m_stInput;
  stTrimInput.Trim();

  // If the text without trailing blanks is non-empty and begins with an
  // equals sign, it is a formula.

  if ((!stTrimInput.IsEmpty()) && (stTrimInput[0] == TEXT('=')))
  {
    Parser parser;
    SyntaxTree newSyntaxTree = parser.Formula(stTrimInput.Mid(1));

    ReferenceSet newSourceSet = newSyntaxTree.GetSourceSet();
    m_pTargetSetMatrix->CheckCircular(home, newSourceSet);

    m_eCellState = CELL_FORMULA;
    m_pTargetSetMatrix->RemoveTargets(home);

    m_syntaxTree = newSyntaxTree;
    m_sourceSet = newSourceSet;

    m_pTargetSetMatrix->AddTargets(home);
  }

  else if (IsNumeric(stTrimInput))
  {
    m_eCellState = CELL_VALUE;
    m_dValue = _tstof(stTrimInput);

    m_stOutput.Format(TEXT("%f"), m_dValue);
    m_stOutput.TrimRight(TEXT('0'));
    m_stOutput.TrimRight(TEXT('.'));

    m_pTargetSetMatrix->RemoveTargets(home);
    m_sourceSet.RemoveAll();
  }

  else
  {
    m_eCellState = CELL_TEXT;
    m_stText = m_stInput;
    m_stOutput = m_stText;

    m_pTargetSetMatrix->RemoveTargets(home);
    m_sourceSet.RemoveAll();
  }
}

// IsNumeric is a help function that return true if the given text holds a
// numerical value. It users the scanner to decide the whther the text
// represents a value.

BOOL Cell::IsNumeric(CString stText)
{
  try
  {
    Scanner scanner(stText);
    TokenList* pTokenList = scanner.GetTokenList();

    if (pTokenList->GetSize() == 2)
    {
      Token firstToken = pTokenList->GetHead();
      Token lastToken = pTokenList->GetTail();

      return (firstToken.GetId() == T_VALUE) &&
             (lastToken.GetId() == T_EOL);
    }
  }

  catch (const CString)
  {
    // Empty.
  }

  return FALSE;
}

// HasValue is called when the value of a formula in another cell is to be
// evaluated. A cell has a value if holds a numerical value of a formula
// that has been successfully evaluated (m_bHasValue is true).

// This method is called at two occasions. When the user ends editing a cell
// by pressing return or tab, or pressing the mouse. In that case, we do only
// have to evaluate the value of this cell because no other cell has been
// updated (bRecursice is false). However, it is also called when the user has
// pasted a block of cells. In that case, we have do evaluate the values of
// the cells in the source set recursively because their values might have
// been updated too (bRecursive is true).

BOOL Cell::HasValue(BOOL bRecursive)
{
  switch (m_eCellState)
  {
    // A text can never be interpreted as a value.

    case CELL_TEXT:
      return FALSE;

    // A value is naturally always interpreted as a value.

    case CELL_VALUE:
      return TRUE;

    // A cell in formula mode may or may not has a valid value depending on
    // whether the formula was correctly evaluated, m_bHasValue holds the
    // status.

    case CELL_FORMULA:
      if (bRecursive)
      {
        EvaluateValue(TRUE);
      }

      return m_bHasValue;
  }

  // As the cell always is in text, value, or formula mode, this part of the
  // method will never be reached. However, in order to avoid compilation
  // warnings, we return false.

  return FALSE;
}

// EvaluateValue is called when some of the source cell of this call has
// been altered. If this cell holds a formula, its value is evaluated by
// calling Evaluate of its syntax tree.

void Cell::EvaluateValue(BOOL bRecursive)
{
  if (m_eCellState == CELL_FORMULA)
  {
    // If the value of the formula is successfully evaluated, the value
    // flag (m_bHasValue) is set to true and the evaluated value is
    // converted to the output text (m_stOutput).

    try
    {
      m_dValue = m_syntaxTree.Evaluate(bRecursive, m_pCellMatrix);
      m_bHasValue = TRUE;

      m_stOutput.Format(TEXT("%f"), m_dValue);
      m_stOutput.TrimRight(TEXT('0'));
      m_stOutput.TrimRight(TEXT('.'));
    }

    // If the text cannot not evaluated due to division by zero or missing
    // value, an exception is thrown. The output text (m_stOutput) is set to
    // the error message and the value flag (m_bHasValue) is set to false.

    catch (const CString stMessage)
    {
      m_bHasValue = FALSE;
      m_stOutput = stMessage;
    }
  }
}

// Finally, UpdateSyntaxTree is called when a block of cells has been
// copied and pasted into another location in the spreadsheet. If this
// cell holds a formula, it calls UpdateReference of its syntax tree and
// sets a new source set.

void Cell::UpdateSyntaxTree(int iRows, int iCols)
{
  if (m_eCellState == CELL_FORMULA)
  {
     m_syntaxTree.UpdateReference(iRows, iCols);
     m_sourceSet = m_syntaxTree.GetSourceSet();
  }
}
