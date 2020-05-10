#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"
#include "Check.h"

#include "Line.h"
#include "Position.h"
#include "Paragraph.h"

#include "Page.h"
#include "WordView.h"
#include "WordDoc.h"

// A new paragraph has left alignment, later on a call to Recalculate will
// initializes its start position and height.

Paragraph::Paragraph()
 :m_yStartPos(0),
  m_iHeight(0),
  m_eAlignment(ALIGN_LEFT)
{
  // Empty.
}

Paragraph::Paragraph(Font emptyFont, Alignment eAlignment)
 :m_yStartPos(0),
  m_iHeight(0),
  m_eAlignment(eAlignment),
  m_emptyFont(emptyFont),
  m_iEmptyAverageWidth(0)
{
  // Empty.
}

// The copy constructor initializes the fields of the class, it is called when
// a paragraph is copied or pasted. Note that the assignment operator is not
// defined on the MFC class CArray, for which reason Copy must be called
// instead.

Paragraph::Paragraph(const Paragraph &paragraph)
 :m_stText(paragraph.m_stText),
  m_yStartPos(paragraph.m_yStartPos),
  m_iHeight(paragraph.m_iHeight),
  m_eAlignment(paragraph.m_eAlignment),
  m_emptyFont(paragraph.m_emptyFont),
  m_iEmptyAverageWidth(paragraph.m_iEmptyAverageWidth)
{
  m_fontArray.Copy(paragraph.m_fontArray);
  m_lineArray.Copy(paragraph.m_lineArray);
  m_rectArray.Copy(paragraph.m_rectArray);
}

// Serialize reads or writes the values of the class fields, respectively.
// Note that the Font and CArray classes have Serialize implementations of
// their own.

void Paragraph::Serialize(CArchive& archive)
{
  m_emptyFont.Serialize(archive);
  m_fontArray.Serialize(archive);
  m_lineArray.Serialize(archive);
  m_rectArray.Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_yStartPos << m_iHeight << (int) m_eAlignment
            << m_stText << m_iEmptyAverageWidth;
  }

  if (archive.IsLoading())
  {
    int eAlignment;
    archive >> m_yStartPos >> m_iHeight >> eAlignment
            >> m_stText >> m_iEmptyAverageWidth;
    m_eAlignment = (Alignment) eAlignment;
  }
}

// Draw is called by the view class every time it needs to be redrawn, partly
// or completely. Some part of the document may be marked. If this particular
// paragraph is marked, the parameters iFirstMarkedChar and iLastMarkedChar
// holds the first and last position of the marked area of this paragraph.
// Note that it only applies to this paragraph, other paragraphs may also be
// marked. If the paragraph is completely unmarked, the view class calls this
// method with the values 0 and -1, respectively. This method also need a
// pointer to a device context in order to write the characters.

void Paragraph::Draw(CDC* pDC, int iFirstMarkedChar, int iLastMarkedChar)const
{
  CSize szUpperLeft(0, m_yStartPos);
  int iSize = m_stText.GetLength();

  if (!m_stText.IsEmpty())
  {
    for (int iChar = 0; iChar < iSize; ++iChar)
    {
      // If the character is inside the marked area, we inverse the text and
      // background color.

      if ((iChar >= iFirstMarkedChar) && (iChar < iLastMarkedChar))
      {
          pDC->SetTextColor(WHITE);
          pDC->SetBkColor(BLACK);
      }

      else
      {
          pDC->SetTextColor(BLACK);
          pDC->SetBkColor(WHITE);
      }

      // We select the font of the character. Every character of the paragraph
      // have its own font. We have to translate the size of the font from
      // typographical points to logical units (hundreds of millimeters).

      CFont cFont;
      Font font = m_fontArray[iChar];
      cFont.CreateFontIndirect(font.PointsToMeters());
      CFont* pPrevFont = pDC->SelectObject(&cFont);

      // The charactr is written relative its top left corner.

      CString stChar = m_stText.Mid(iChar, 1);
      pDC->DrawText(stChar, m_rectArray[iChar] + szUpperLeft, TA_LEFT|TA_TOP);

      pDC->SelectObject(pPrevFont);
    }
  }

  // If the text is empty and the paragraph is marked, we paint a black
  // rectangle of average width and height. If the paragraph is empty and
  // unmarked, we do nothing.

  else if ((iFirstMarkedChar != 0) && (iLastMarkedChar != 0))
  {
    CPen pen(PS_SOLID, 0, BLACK);
    CBrush brush(BLACK);

    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    CRect rcChar = CRect(0, 0, m_iEmptyAverageWidth, m_iHeight) + szUpperLeft;
    pDC->Rectangle(rcChar);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}

// AddChar is called every time the user adds a character to this particular
// paragraph. Its first task is to decide the font of the new character. The
// document class has a field m_pNextFont, which is set to a new font when the
// user chooses a new font. If the user moves the caret or pastes a text
// block, m_pNextFont is set to null. The value of the document class field
// m_pNextFont is passed on to the parameter pNextFont.

// If the pointer is not null, we simply set the font of the new character to
// that value. Otherwise, we have to examine the text. If the paragraph lacks
// text (m_stText.IsEmpty()) we use the empty font. If it has text, but the
// new character is to be inserted at the beginning of the paragraph (iIndex
// == 0), we use the font of the first character. Finally, if the paragraph
// has text and the new character is not to be inserted at the beginning of
// the paragraph we use the font of the precedingcharacter.

// In the document class, there is a field m_eKeyboardState. It holds the mode
// of the keyboard, which can be either insert or overwrite. Its value is
// passed as the parameter eKeyboardState. The character and its font are
// inserted if the keyboard is in insert mode. If it is in overwrite mode they
// are overwritten unless the insert position is at the end of the text.

void Paragraph::AddChar(int iIndex, UINT uNewChar, Font* pNextFont,
                        KeyboardState eKeyboardState)
{
  Font newFont;

  // If they user has chosen a new font by a Font Dialog, the new character is
  // given that font.

  if (pNextFont != NULL)
  {
    newFont = *pNextFont;
  }

  // If the text is empty, we use the empty font.

  else if (m_stText.IsEmpty())
  {
    newFont = m_emptyFont;
  }

  // If the input index is at the beginning of the text, we use the font of
  // the first character.

  else if (iIndex == 0)
  {
    newFont = m_fontArray[0];
  }

  // If the input index is not at the beginning of the text, we use the font 
  // the precedingcharacter.

  else
  {
    newFont = m_fontArray[iIndex - 1];
  }

  CRect emptyRect(0, 0, 0, 0);

  switch (eKeyboardState)
  {
    // If the keyboard is in insert mode, we insert the character at the given
    // index. The InsertAt method works also if the input index is one step to
    // the right of the text.

    case KM_INSERT:
      m_stText.Insert(iIndex, (TCHAR) uNewChar);
      m_fontArray.InsertAt(iIndex, newFont);
      m_rectArray.InsertAt(iIndex, emptyRect);
      break;

    case KM_OVERWRITE:
      // In overwrite mode, we overwrite the character at the input index with
      // SetAt if it is not at the end of the text. In that case, we use
      // AppendChar and Add instead.

      if (iIndex < m_stText.GetLength())
      {
        m_stText.SetAt(iIndex, (TCHAR) uNewChar);
        m_fontArray.SetAt(iIndex, newFont);
        m_rectArray.SetAt(iIndex, emptyRect);
      }

      else
      {
        m_stText.AppendChar((TCHAR) uNewChar);
        m_fontArray.Add(newFont);
        m_rectArray.Add(emptyRect);
      }
      break;
  }
}

// GetRepaintSet is called when a part of the text is to be marked or
// unmarked. It adds to repaintSet the rectangles (from m_rectArray) of the
// characters in question. The index parameters have default value 0 and -1.
// If the last index is -1, the rest of the paragraph's rectangles shall be
// included in the set. In that case, it is set to the length of the text.

void Paragraph::GetRepaintSet(RectSet& repaintSet, int iFirstIndex
                                 /*= 0 */, int iLastIndex /* = -1 */)
{
  if (iLastIndex == -1)
  {
    iLastIndex = m_stText.GetLength();
  }

  CSize szUpperLeft(0, m_yStartPos);

  for (int iIndex = iFirstIndex; iIndex < iLastIndex; ++iIndex)
  {
    CRect rcChar = m_rectArray[iIndex];
    repaintSet.Add(rcChar + szUpperLeft);
  }
}

// DeleteText is called when the one or several (possible all) characters of
// the paragraph is to be deleted. The index parameters indicate the first and
// last index of the part of the text. They can be omitted in the call as they
// are default parameters. If the last parameter is omitted, the rest of the
// text shall be deleted and the parameter is set to the length of the text.

// If the whole of the text is to be deleted, we set the empty font to the one
// of the first character. Note that this method is not called if the
// paragraph is empty. Also note the difference between deleteing the whole
// text of the paragraph and deleteing the paragraph itself. In the first
// case, the paragraph is a part of the document; in the second case, the
// paragraph object is de-allocated and removed from the paragraph array of
// the document class and method is not called.

void Paragraph::DeleteText(int iFirstIndex /* = 0*/, int iLastIndex /* = -1*/)
{
  int iLength = m_stText.GetLength();

  if (iLastIndex == -1)
  {
    iLastIndex = iLength;
  }

  if ((iFirstIndex == 0) && (iLastIndex == iLength))
  {
    m_emptyFont = m_fontArray[0];
  }

  m_stText.Delete(iFirstIndex, iLastIndex - iFirstIndex);
  m_fontArray.RemoveAt(iFirstIndex, iLastIndex - iFirstIndex);
  m_rectArray.RemoveAt(iFirstIndex, iLastIndex - iFirstIndex);
}

// GetFont is called by the document class in order to set the default font in
// the font dialog that appears when the user wants to set the font. If the
// text is empty we return the empty font. If the index of the caret in the
// paragraph is zero we return the font of the first character. Otherwise, we
// return the font of the position precedingthat of the caret.

Font Paragraph::GetFont(int iCaretIndex) const
{
  if (m_stText.IsEmpty())
  {
    return m_emptyFont;
  }

  else if (iCaretIndex == 0)
  {
    return m_fontArray[0];
  }

  else
  {
    return m_fontArray[iCaretIndex - 1];
  }
}

// SetFont is called when one or several characters of the paragraph is given
// a new font. Unlike GetFont above, SetFont may affect more than one
// character in case the user has marked a portion of the text and then change
// the font. Like GetRepaintSet above, the two indexes parameters are
// default parameters. If the second of them is omitted in the call, the rest
// of the text is updated with the new font.

void Paragraph::SetFont(Font newFont, int iFirstIndex /* = 0 */,
                        int iLastIndex /* = -1 */)
{
  if (iLastIndex == -1)
  {
    iLastIndex = m_stText.GetLength();
  }

  for (int iIndex = iFirstIndex; iIndex < iLastIndex; ++iIndex)
  {
    m_fontArray[iIndex] = newFont;
  }
}

// GetWord is called when the user double clicks on a word. It starts at the
// character that is edited (iEditChar) and traverses to the left and to the
// right until it finds a space character or the beginning or end of the
// paragraph. The parameters iFirstChar and iLastChar are reference
// parameters, which implies that there values can be obtained by the
// calling method. Finally, the method returns true if it finds a word to be
// marked; that is, if the index of the first character is less than the index
// of the last one.

BOOL Paragraph::GetWord(int iEditChar, int& iFirstChar,
                        int& iLastChar)
{
  int iChar;

  for (iChar = iEditChar; (iChar >= 0) &&
       isalnum(m_stText[iChar]); --iChar)
  {
    // Empty.
  }

  iFirstChar = (iChar + 1);
  int iLength = m_stText.GetLength();

  for (iChar = iEditChar; (iChar < iLength) &&
       isalnum(m_stText[iChar]); ++iChar)
  {
    // Empty.
  }

  iLastChar = iChar;
  return (iFirstChar < iLastChar);
}

// GetHomeChar is called when the user presses the home key. It first finds
// out which line the current index holds. Then it returns the index of the
// first key on that line. This method always finds the right line, of which
// reason an check watch is added to the end of the method.

int Paragraph::GetHomeChar(int iChar) const
{
  int iLines = (int) m_lineArray.GetSize();

  for (int iIndex = 0; iIndex < iLines; ++iIndex)
  {
    // We extract the last character of each line in the paragraph.

    Line line = m_lineArray[iIndex];
    int iFirstChar = line.GetFirstChar();
    int iLastChar = line.GetLastChar();

    // If the given character is on this line, we return the first index of
    // the line.

    if (iChar <= (iLastChar + 1))
    {
      return iFirstChar;
    }
  }

  // As the loop above always will find the correct index (every character
  // belong to a line of the paragraph), this point of the code will never be
  // reached. The check is for debugging purposes only.

  check(FALSE);
  return 0;
}

// GetEndChar is called when the user presses the end. Similar to GethomeKey
// above, it finds the line of the current index and returns the index of its
// last character.

int Paragraph::GetEndChar(int iChar) const
{
  int iLines = (int) m_lineArray.GetSize();

  for (int iIndex = 0; iIndex < iLines; ++iIndex)
  {
    // We extract the last character of each line in the paragraph.

    Line line = m_lineArray[iIndex];
    int iLastChar = line.GetLastChar();

    // If the given character is on this line, we return the first index of
    // the line.

    if (iChar <= (iLastChar + 1))
    {
      return (iLastChar + 1);
    }
  }

  // As the loop above always will find the correct index (every character
  // belong to a line of the paragraph), this point of the code will never be
  // reached. The check is for debugging purposes only.

  check(FALSE);
  return 0;
}

// ExtractText is called when the user marks one portion of the document’s
// text and then copies it. It creates a new paragraph and fills it with the
// text and fonts of the marked area. Like GetRepaintSet and SetFont above,
// its two indexes are default parameters. If the last index is -1, the rest
// of the text shall be extracted. The text is easy to extract with the
// CString Mid method. Unfortunately, there is no similar method for arrays,
// of which reason we must traverse the font array and add fonts one by one to
// the new paragraph. We also need to set the empty font of the paragraph. If
// the first index of the extracted text is less than the length of the text,
// we set the font of the first marked character. Otherwise, we use the font
// of the character preceding the first one. If the text is empty, we just
// set the empty font.

Paragraph* Paragraph::ExtractText(int iFirstIndex /* = 0 */,
                                  int iLastIndex /* = -1 */) const
{
  Paragraph* pNewParagraph;
  check_memory(pNewParagraph = new Paragraph(*this));

  if (!m_stText.IsEmpty())
  {
    int iLength = m_stText.GetLength();

    if (iLastIndex == -1)
    {
      iLastIndex = iLength;
    }

    pNewParagraph->m_stText = m_stText.Mid(iFirstIndex,
                                           iLastIndex - iFirstIndex);
    CRect rcEmpty(0, 0, 0, 0);
    for (int iChar = iFirstIndex; iChar < iLastIndex; ++iChar)
    {
      pNewParagraph->m_fontArray.Add(m_fontArray[iChar]);
      pNewParagraph->m_rectArray.Add(rcEmpty);
    }

    // The empty font is set to the one of the first index, unless the first
    // index is at the end of the text; in that case, it is set to the font
    // of the last character.

    if (iFirstIndex < iLength)
    {
      pNewParagraph->m_emptyFont = m_fontArray[iFirstIndex];
    }

    else
    {
      pNewParagraph->m_emptyFont = m_fontArray[iFirstIndex - 1];
    }
  }

  // If the text is empty, we just set the empty font. A succeeding call to
  // Recalculate will initialize the rest of the fields.

  else
  {
    pNewParagraph->m_emptyFont = m_emptyFont;
  }

  return pNewParagraph;
}

// Insert inserts a character in the paragraph. Unless the paragraph to insert
// in empty, we just insert its text and font array. Append adds a paragraph
// to the end of the paragraph simple by calling Insert.

void Paragraph::Insert(int iChar, Paragraph* pInsertParagraph)
{
  int iInsertLength = pInsertParagraph->GetLength();

  if (iInsertLength > 0)
  {
    m_stText.Insert(iChar, pInsertParagraph->m_stText);
    m_fontArray.InsertAt(iChar, &pInsertParagraph->m_fontArray);

    CRect rcEmpty(0, 0, 0, 0);
    m_rectArray.InsertAt(iChar, rcEmpty, iInsertLength);
  }
}

void Paragraph::Append(Paragraph* pAppendParagraph)
{
  Insert(GetLength(), pAppendParagraph);
}

// When the user presses the return key inside a paragraph, it is split into
// two parts. Split returns a new paragraph containing the second half of the
// paragraph and deletes it from this paragraph.

Paragraph* Paragraph::Split(int iChar)
{
  Paragraph* pNewParagraph;
  check_memory(pNewParagraph = new Paragraph());

  pNewParagraph->m_stText = m_stText.Mid(iChar);
  m_stText = m_stText.Left(iChar);

  pNewParagraph->m_fontArray.Copy(m_fontArray);
  pNewParagraph->m_fontArray.RemoveAt(0, iChar);
  m_fontArray.SetSize(iChar);

  pNewParagraph->m_eAlignment = m_eAlignment;
  pNewParagraph->m_emptyFont = GetFont(iChar);
  return pNewParagraph;
}

// When the users click the mouse, we have to decide which paragraph and
// character they clicked at. The mouse position in device units is caught by
// the view classes, converted to logical units, send to the document class,
// which first finds the paragraph in question and then finally calls
// PointToChar in order to find the position in the paragraph.

int Paragraph::PointToChar(CPoint ptMouse)
{
  // If the text is empty, we just return index 0. Otherwise, we traverse the
  // lines of the paragraph one by one in order to find the correct line. Then
  // we traverse the line in order to find the correct character. To start
  // with, we subtract the start position of the paragraph from the mouse
  // position, which is relative the beginning of the document.

  if (m_stText.IsEmpty())
  {
    return 0;
  }

  // If the document is large enough, it will be divided into several pages.
  // In that case, there will be a check that each page is beginning with a
  // whole paragraph. This might give the result that the user clicks at the
  // end of a page (or at the end of the whole document) where there is no
  // paragraph. If that happens, the correct character shall be the one above
  // the mouse click. That is why we have to make sure the position of the
  // mouse does not exceed the height of the paragraph.

  ptMouse.y -= m_yStartPos;
  ptMouse.y = min(ptMouse.y, m_iHeight - 1);

  int iLines = (int) m_lineArray.GetSize();

  int iParagraphHeight = 0;
  for (int iLine = 0; iLine < iLines; ++iLine)
  {
    Line line = m_lineArray[iLine];
    int iLineHeight = line.GetHeight();
    iParagraphHeight += iLineHeight;

    // When we find the right line, the search continues for the right
    // character. We cannot fail in finding the right line; therefore, there
    // is an check watch at the end of the method. When we look for the
    // correct character, we first check if the mouse position is to the left
    // of the first character of the line; in that case, we return the first
    // index of the first character of the line. If it instead is to the right
    // of the last character of the line, we return the index of the character
    // to the right of the last character.

    if (ptMouse.y < iParagraphHeight)
    {
      int iFirstChar = line.GetFirstChar();
      int iLastChar = line.GetLastChar();

      CRect rcFirstChar = m_rectArray[iFirstChar];
      CRect rcLastChar = m_rectArray[iLastChar];

      if (ptMouse.x <= rcFirstChar.left)
      {
        return iFirstChar;
      }

      else if (ptMouse.x >= rcLastChar.right)
      {
        return (iLastChar + 1);
      }

      // In none of the above cases applies, we traverses through the line and
      // when we find the correct character, we check whether the mouse
      // position if is mostly to the left or the right character, then we
      // return the index of the character to the left to the right,
      // respectively. We cannot fail in finding the correct character once we
      // have found the correct line. Therefore, we have an check watch at
      // the end of the character search.

      else
      {
        for (int iChar = iFirstChar; iChar <= iLastChar; ++iChar)
        {
          CRect rcChar = m_rectArray[iChar];

          if (ptMouse.x < rcChar.right)
          {
            int cxLeft = ptMouse.x - rcChar.left;
            int cxRight = rcChar.right - ptMouse.x;

            if (cxLeft < cxRight)
            {
              return iChar;
            }

            else
            {
              return iChar + 1;
            }
          }
        }

        check(FALSE);
        return 0;
      }
    }
  }

  check(FALSE);
  return 0;
}

// CharToRect returns the rectangle of the character at the given index in the
// paragraph. 

CRect Paragraph::CharToRect(int iChar)
{
  CSize szUpperLeft(0, m_yStartPos);

  // We have two special cases. First, the paragraph may be empty. In that
  // case, we use the average size of the paragraph’s empty font.

  if (m_stText.IsEmpty())
  {
    return szUpperLeft + CRect(0, 0, m_iEmptyAverageWidth, m_iHeight);
  }

  // Second, the given index may be outside the text. That is one step to the
  // right of the last character. In that case, we return a rectangle holding
  // the dimensions of the characters beyond the text using the size of the
  // last character.

  else if (iChar == m_stText.GetLength())
  {
    CRect rcChar = m_rectArray[iChar - 1];
    CRect rcCaret(rcChar.right, rcChar.top, rcChar.right + rcChar.Width(),
                  rcChar.bottom);
    return szUpperLeft + rcCaret;
  }

  // Otherwise, we just return the rectangle of the given character.

  else
  {
    return szUpperLeft + m_rectArray[iChar];
  }
}

// GetCaretRect return the rectangle of the given character.

CRect Paragraph::GetCaretRect(int iChar)
{
  CSize szUpperLeft(0, m_yStartPos);
  int iSize = m_stText.GetLength();

  // If the text is empty, we return a caret rectangle based on the empty
  // font.

  if (iSize == 0)
  {
    return szUpperLeft + CRect(0, 0, m_iEmptyAverageWidth, m_iHeight);
  }

  // If the text is non-empty but the given character is at index zero, 
  // we return the caret rectangle of the first character.

  else if (iChar == 0)
  {
    return szUpperLeft + m_rectArray[0];
  }

  // If the character is a "home character"; that is, if it is the first
  // on its line we return its rectangle.

  else if (isHomeChar(iChar))
  {
    CRect rcChar = m_rectArray[iChar];
    CRect rcCaret(rcChar.left, rcChar.top, rcChar.right, rcChar.bottom);
    return szUpperLeft + rcCaret;
  }

  // Otherwise, we find the preceding character and return a caret rectangle
  // based on its size.

  else
  {
    CRect rcChar = m_rectArray[iChar - 1];
    CRect rcCaret(rcChar.right, rcChar.top, rcChar.right + rcChar.Width(),
                  rcChar.bottom);
    return szUpperLeft + rcCaret;
  }
}

BOOL Paragraph::isHomeChar(int iChar)
{
  int iLines = (int) m_lineArray.GetSize();

  for (int iIndex = 0; iIndex < iLines; ++iIndex)
  {
    // We extract the last character of each line in the paragraph.

    Line line = m_lineArray[iIndex];
    int iFirstChar = line.GetFirstChar();

    // If the given character is on this line, we return the first index of
    // the line.

    if (iChar == iFirstChar)
    {
      return TRUE;
    }
  }

  return FALSE;
}

// When the user walks up and down through the document with the up and down
// arrows, we need to know the size of the current line. CharToLineRect
// returns a rectangle holding the dimensions of the line. Like CharToRect
// above, we cannot fail in finding the correct line, so we have an check
// watch at the end of the method.

CRect Paragraph::CharToLineRect(int iChar)
{
  int iParagraphHeight = 0;
  int iLines = (int) m_lineArray.GetSize();

  for (int iIndex = 0; iIndex < iLines; ++iIndex)
  {
    Line line = m_lineArray[iIndex];
    int iLastChar = line.GetLastChar();
    int iLineHeight = line.GetHeight();

    if (iChar <= (iLastChar + 1))
    {
      return CRect(0, m_yStartPos + iParagraphHeight, PAGE_WIDTH,
                   m_yStartPos + iParagraphHeight + iLineHeight);
    }

    iParagraphHeight += iLineHeight;
  }

  check(FALSE);
  return CRect();
}

// Recalculate is called in order to recalculate the rectangle (m_rectArray)
// and line (m_lineArray) arrays every time one or several character have be
// added or removed, or when the font or the alignment has been changed.

void Paragraph::Recalculate(CDC* pDC, RectSet* pRepaintSet /* = NULL */)
{
  RectArray oldRectArray;

  if (pRepaintSet != NULL)
  {
    oldRectArray.Copy(m_rectArray);
  }

  m_iHeight = 0;
  m_lineArray.RemoveAll();
  m_rectArray.RemoveAll();

  // If the paragraph is empty, we find the height and average width of a
  // character of the empty font. 

  if (m_stText.IsEmpty())
  {
    CFont cFont;
    cFont.CreateFontIndirect(m_emptyFont.PointsToMeters());
    CFont* pPrevFont = pDC->SelectObject(&cFont);

    TEXTMETRIC textMetric;
    pDC->GetTextMetrics(&textMetric);
    pDC->SelectObject(pPrevFont);

    m_iHeight = textMetric.tmHeight;
    m_iEmptyAverageWidth = textMetric.tmAveCharWidth;

    Line line(0, 0, 0);
    m_lineArray.Add(line);
  }

  // If the paragraph is not empty we generate arrays of size and ascent lines
  // for every character as well as the line and rectangle array by calling
  // GenerateSizeArray, GenerateAscentArray, GenerateLineArray, and
  // GenerateRectArray.

  else
  {
    SizeArray sizeArray;
    GenerateSizeArray(sizeArray, pDC);

    IntArray ascentArray;
    GenerateAscentArray(ascentArray, pDC);

    GenerateLineArray(sizeArray);
    GenerateRectArray(sizeArray, ascentArray);
  }

  // Finally, if the pointer to the repaint set is not null, we also call
  // GenerateRepaintSet.

  if (pRepaintSet != NULL)
  {
    GenerateRepaintSet(oldRectArray, pRepaintSet);
  }
}

// ClearRectArray sets the rectangle array of the paragraph to empty.

void Paragraph::ClearRectArray()
{
  CRect emptyRect(0, 0, 0, 0);
  int iSize = (int) m_rectArray.GetSize();
  
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    m_rectArray[iIndex] = emptyRect;
  }
}

// GenerateSizeArray fills the given array with the size (width and height) of
// every character in the paragraph (in logical units). For every character,
// we load the device context with the font. Note that we have to translate
// the font from typographical points at hundreds of millimeters by calling
// PointToMeters.

void Paragraph::GenerateSizeArray(SizeArray& sizeArray, CDC* pDC)
{
  int iLength = m_stText.GetLength();
  for (int iChar = 0; iChar < iLength; ++iChar)
  {
    CFont cFont;
    Font font = m_fontArray[iChar];
    cFont.CreateFontIndirect(font.PointsToMeters());
    CFont* pPrevFont = pDC->SelectObject(&cFont);

    CString stChar = m_stText.Mid(iChar, 1);
    CSize szChar = pDC->GetTextExtent(stChar);

    // Characters written in italic tend to request slightly more space, so we
    // increase the size with 20 percent. Also plain text tend to need a
    // little bit more space, why we increase the size with 10 percent.

    szChar.cx = (int) ((font.IsItalic() ? 1.2 : 1.1) * szChar.cx);
    szChar.cy = (int) ((font.IsItalic() ? 1.2 : 1.1) * szChar.cy);

    sizeArray.Add(szChar);
    pDC->SelectObject(pPrevFont);
  }
}

// GenerateAscentArray fills the given array with the ascent line of every
// character in the paragraph (in logical units). For every character, we load
// the device context with the font. Note that we have to translate the font
// from typographical points at hundreds of millimeters by calling
// PointToMeters.

void Paragraph::GenerateAscentArray(IntArray& ascentArray, CDC* pDC)
{
  int iSize = (int) m_fontArray.GetSize();
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    CFont cFont;
    Font font = m_fontArray[iIndex];
    cFont.CreateFontIndirect(font.PointsToMeters());
    CFont* pPrevFont = pDC->SelectObject(&cFont);

    TEXTMETRIC textMetric;
    pDC->GetTextMetrics(&textMetric);
    pDC->SelectObject(pPrevFont);

    ascentArray.Add(textMetric.tmAscent);
  }
}

// GenerateLineArray generates the line array. We have to decide how many
// words each line can hold. We traverse through the text, calculate the size
// of each word and when the next word does not fit on the line, we start a
// new line and save the index of the first and last character on the line as
// well as the height of the line (the height of the highest character).

void Paragraph::GenerateLineArray(SizeArray& sizeArray)
{
  BOOL bSpace = FALSE;
  int iSpaceIndex = 0, iStartIndex = 0, iLineWidth = 0, iLineHeight = 0,
      iSpaceLineHeight = 0;
  int iSize = m_stText.GetLength(), iIndex = 0;

  while (iIndex < iSize)
  {
    CSize szChar = sizeArray[iIndex];
    iLineHeight = max(iLineHeight, szChar.cy);

    // The latest space is a suitable point to break the line at.

    if (m_stText[iIndex] == TEXT(' '))
    {
      bSpace = TRUE;
      iSpaceIndex = iIndex;
      iSpaceLineHeight = iLineHeight;
    }

    iLineWidth += szChar.cx;

    // When no more characters fit on the line, we break it and look up the
    // latest space. 

    if (iLineWidth > PAGE_WIDTH)
    {
      if (bSpace)
      {
        Line line(iStartIndex, iSpaceIndex - 1,
                  iSpaceLineHeight);
        m_lineArray.Add(line);

        iStartIndex = iSpaceIndex + 1;
        iLineWidth = 0;
        iLineHeight = 0;

        bSpace = FALSE;
        iIndex = iStartIndex;
      }

      else if (iStartIndex < iIndex)
      {
        Line line(iStartIndex, iIndex - 1, iLineHeight);
        m_lineArray.Add(line);

        iStartIndex = iIndex;
        iLineWidth = 0;
        iLineHeight = 0;
      }

      else
      {
        Line line(iStartIndex, iIndex, iLineHeight);
        m_lineArray.Add(line);

        ++iIndex;
        iStartIndex = iIndex;
        iLineWidth = 0;
        iLineHeight = 0;
      }
    }

    else
    {
      ++iIndex;
    }
  }

  // If there are character left after the latest line break, we add them too.

  if (iStartIndex < iSize)
  {
    Line line(iStartIndex, iSize - 1, iLineHeight);
    m_lineArray.Add(line);
  }
}

// GenerateRectArray generates the rectangle array. With the size, ascent, and
// line arrays, we calculate the rectangle of each character.

void Paragraph::GenerateRectArray(SizeArray& sizeArray, IntArray& ascentArray)
{
  int iLines = (int) m_lineArray.GetSize();

  // First, we traverse the lines of the paragraph, one by one.
  // We find the first and last character of the first line,
  // the paragraphs has always at least one line.

  for (int iLineIndex = 0; iLineIndex < iLines; ++iLineIndex)
  {
    // First, we need to find the height, ascent line and width of the line.

    Line line = m_lineArray[iLineIndex];
    int iFirstChar = line.GetFirstChar();
    int iLastChar = line.GetLastChar();

    int iLineWidth = 0, iLineHeight = 0, iLineAscent = 0;
    for (int iIndex = iFirstChar; iIndex <= iLastChar; ++iIndex)
    {
      TCHAR cChar = m_stText[iIndex];
      CSize szChar = sizeArray[iIndex];

      // The width of the line is the sum of the width of all characters. If
      // the character is a space and the paragraph has justified alignment,
      // we do not include its width into the total width because later on we
      // need to compute the width of the line without the spaces.
 
      if (!((cChar == TEXT(' ')) && (m_eAlignment == ALIGN_JUSTIFIED)))
      {
        iLineWidth += szChar.cx;
      }

      // The height of the line is the height of the highest character.
      iLineHeight = max(iLineHeight, szChar.cy);

      // The accent line of the line is the ascent line of the character with
      // the highest ascent.
      iLineAscent = max(iLineAscent, ascentArray[iIndex]);
    }

    // We find the start position of the line by considering the alignment of
    // the paragraph and the width of the line.

    int xStartPos = 0, iSpaceWidth = 0;
    switch (m_eAlignment)
    {
      // If left alignment, the line start at the left side.

      case ALIGN_LEFT:
        xStartPos = 0;
        break;

      // If center and right alignment, we compute the start position by
      // comparing width of the line with the width of the page.

      case ALIGN_CENTER:
        xStartPos = (PAGE_WIDTH - iLineWidth) / 2;
        break;

      case ALIGN_RIGHT:
        xStartPos = PAGE_WIDTH - iLineWidth;
        break;

      // If justified alignment, we need to find the number of spaces on the
      // line and calculate the width of each space in order for the line to
      // completely fill the width of the page.

      case ALIGN_JUSTIFIED:
        xStartPos = 0;
        CString stTemp = m_stText.Mid(iFirstChar, iLastChar - iFirstChar + 1);
        int iSpaces = stTemp.Remove(TEXT(' '));

        if (iSpaces > 0)
        {
            iSpaceWidth = (PAGE_WIDTH - iLineWidth) / iSpaces;
        }
        break;
    }

    // Finally, we calculate the rectangle for each character. We traverse the
    // line and with the sizes of the characters and the height and ascent
    // line of the line we find each rectangle. We start by the start position
    // we found above and increase the position for each character on the
    // line.

    int xLeftPos = xStartPos, iWidth = 0, yTopPos = 0, iHeight = 0;
    for (int iIndex = iFirstChar; iIndex <= iLastChar; ++iIndex)
    {
      CSize szChar = sizeArray[iIndex];
      int iAscent = ascentArray[iIndex];         

      // If the paragraph has justified alignment and the character is a
      // space, we use the space width calculated above.

      if ((m_stText[iIndex] == TEXT(' ')) && (m_eAlignment == ALIGN_JUSTIFIED))
      {
        iWidth = iSpaceWidth;
      }
  
      else
      {
        iWidth = szChar.cx;
      }

      yTopPos = m_iHeight + iLineAscent - iAscent;
      iHeight = szChar.cy;

      CRect rcChar(xLeftPos, yTopPos, xLeftPos + iWidth, yTopPos + iHeight);
      m_rectArray.Add(rcChar);
      xLeftPos += iWidth;
    }

    // If we are not on the last line in of the paragraph and if there is a
    // blank character between this line and the next one, we add a rectangle
    // for it.

    if ((iLastChar < (m_stText.GetLength() - 1)) &&
        (m_stText[iLastChar + 1] == ' '))
    {
      CSize szChar = sizeArray[iLastChar + 1];
      CRect rcChar(xLeftPos, yTopPos, xLeftPos + szChar.cx,
                   yTopPos + iHeight);
      m_rectArray.Add(rcChar);
    }

    // The height of the paragraph is increased with the height of this line.
    m_iHeight += iLineHeight;
  }
}

// When a paragraph has been altered, we have to repaint the altered area of
// the client area. However, we do not want to repaint the whole paragraph,
// just the characters that need to be updated. GenerateRepaintSet
// compares the original rectangle array with the newly generated one and
// fills the repaint set with every rectangle that has been altered.

void Paragraph::GenerateRepaintSet(RectArray& oldRectArray,
                                      RectSet* pRepaintSet)
{
  // Rememember that the positio of each character is relative its own
  // paragraph, we start by defining the top left corner of the paragraph
  // relative the document.

  CSize szUpperLeft(0, m_yStartPos);

  // We traverse the characters and add those that has been given new
  // dimensions.

  int iSize = (int) m_rectArray.GetSize();
  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    CRect rcOldChar = oldRectArray[iIndex];
    CRect rcNewChar = m_rectArray[iIndex];

    if (rcOldChar != rcNewChar)
    {
      if (!rcOldChar.IsRectEmpty())
      {
        pRepaintSet->Add(rcOldChar + szUpperLeft);
      }

      if (!rcNewChar.IsRectEmpty())
      {
        pRepaintSet->Add(rcNewChar + szUpperLeft);
      }
    }
  }

  // If the paragraph in non-empty we add for each line the area to the left
  // and to the right of the line.

  if (!m_stText.IsEmpty())
  {
    int iTotalHeight = 0;
    int iLines = (int) m_lineArray.GetSize();

    // We traverse the lines and for each line find the before and after
    // areas.

    for (int iLineIndex = 0; iLineIndex < iLines; ++iLineIndex)
    {
      // We need the position of the first character of the line. We
      // create and add the left area. We also need the height of the line.

      Line line = m_lineArray[iLineIndex];
      int iFirstChar = line.GetFirstChar();
      int iHeight = line.GetHeight();

      CRect rcFirstChar = m_rectArray[iFirstChar];
      CRect rcLeftBlock(0, iTotalHeight, rcFirstChar.left,
                        iTotalHeight + iHeight);

      if (!rcLeftBlock.IsRectEmpty())
      {
        pRepaintSet->Add(rcLeftBlock + szUpperLeft);
      }

      // Finally, we look up the position of the first character of the line.
      // We create and add the right area.

      int iLastChar = line.GetLastChar();
      CRect rcLastChar = m_rectArray[iLastChar];
      CRect rcRightBlock(rcLastChar.right, iTotalHeight, PAGE_WIDTH,
                         iTotalHeight + iHeight);

      if (!rcRightBlock.IsRectEmpty())
      {
        pRepaintSet->Add(rcRightBlock + szUpperLeft);
      }

      iTotalHeight += iHeight;
    }
  }

  // If the paragraph is empty, we just create and add an area holding the
  // whole paragraph. Remember that even though the paragraph is empty, it
  // still holds a height.

  else
  {
    CRect rcTotalBlock(0, 0, PAGE_WIDTH, m_iHeight);
    pRepaintSet->Add(rcTotalBlock + szUpperLeft);
  }
}
