#include "StdAfx.h"

#include "List.h"
#include "Font.h"
#include "Color.h"
#include "Caret.h"
#include "Check.h"

#include "Figure.h"
#include "TwoDimensionalFigure.h"
#include "RectangleFigure.h"
#include "TextFigure.h"
#include "DrawDoc.h"

// The default constructor is used with serialization, as is the serial macro.
// The second constructor is used to initialize the text, and the copy
// constructor is called by Copy to create a copy of the text.

TextFigure::TextFigure()
 :m_eDragMode(CREATE_TEXT)
{
    // Empty.
}

TextFigure::TextFigure(const Color& color, const CPoint& ptMouse, const Font& font,
                       CDC* pDC)
 :Figure(color),
  m_ptText(ptMouse),
  m_font(font),
  m_iEditIndex(0),
  m_eDragMode(CREATE_TEXT)
{
  GenerateCaretArray(pDC);
}

TextFigure::TextFigure(const TextFigure& text)
 :Figure(text),
  m_ptText(text.m_ptText),
  m_szText(text.m_szText),
  m_stText(text.m_stText),
  m_font(text.m_font),
  m_iEditIndex(text.m_iEditIndex),
  m_eDragMode(text.m_eDragMode)
{
  m_caretArray.Copy(text.m_caretArray);
}

Figure* TextFigure::Copy() const
{
  check_memory(return (new TextFigure(*this)));
}

// Serialize loads and stores the position of the text and the text itself as
// well as the size of the text. Storing and saving the size of the text may
// seem as an unnecessary move because we can calculate the size given the 
// text and its font. However, that requires access to a device context, which
// Serialize does not have. So, instead of the rather complicate process of
// letting the document class create a device context without access to a view
// object, we just load and store the size of the text. Remember that the size
// if given in logical units, so the text will have the same size on screens
// with different resolutions.

void TextFigure::Serialize(CArchive& archive)
{
  Figure::Serialize(archive);
  m_font.Serialize(archive);
  m_caretArray.Serialize(archive);

  if (archive.IsStoring())
  {
    archive << m_ptText << m_stText << m_szText << m_iAverageWidth;
  }

  if (archive.IsLoading())
  {
    archive >> m_ptText >> m_stText >> m_szText >> m_iAverageWidth;
    m_iEditIndex = 0;
  }
}

// Click is rather straightforward as we do not have to check whether the
// user has clicked at any of the text’s squares or at the text itself. We
// just create a CRect object with the text’s coordinates and check if the
// mouse click is inside it.

BOOL TextFigure::Click(const CPoint& ptMouse)
{
  m_eDragMode = MOVE_TEXT;
  CRect rcText(m_ptText, m_szText);
  return rcText.PtInRect(ptMouse);
}

// DoubleClick, on the other hand, is more complicated. First, we decide if
// the user has clicked at the text at all. In that case, we have to find out
// where in the text the user has clicked in order to set the caret marker at
// the correct position. The vector m_caretArray has been initialized to the
// start positions of character in the text by a previous call to
// GenerateCaretArray. We traverse that vector and define the beginning and
// end position (iFirstPos and iLastPos) of every character. When we find the
// correct character (the character the user has clicked at), we have to
// decide if the users has clicked at its left or right side. If they have
// clicked at the left side, we return the characters position, if they have
// clicked at the right side, we return the position of the character on the
// right. As we know that the user has clicked at the text, we do not have
// to consider any case where the user has clicked at the left of the leftmost
// character or at the right of the rightmost character.

BOOL TextFigure::DoubleClick(const CPoint& ptMouse)
{
  CRect rcText(m_ptText, m_szText);

  if (rcText.PtInRect(ptMouse))
  {
    CPoint ptTextMouse = ptMouse - m_ptText;
    int iSize = m_stText.GetLength();

    for (int iIndex = 0; iIndex < iSize; ++iIndex)
    {
      int iFirstPos = m_caretArray[iIndex];
      int iLastPos = m_caretArray[iIndex + 1] - 1;

      if ((ptTextMouse.x >= iFirstPos) && (ptTextMouse.x <= iLastPos))
      {
        if ((ptTextMouse.x - iFirstPos) < (iLastPos - ptTextMouse.x))
        {
          m_iEditIndex = iIndex;
        }
        else
        {
          m_iEditIndex = iIndex + 1;
        }

        break;
      }
    }

    m_eDragMode = EDIT_TEXT;
    m_stPreviousText = m_stText;
    return TRUE;
  }

  // As we always find the character clicked at, this point of the method is
  // never reached. The assertion is for debugging purposes only.

  check(FALSE);
  return FALSE;
}

// Inside is quite straightforward, we just check if the top left and bottom
// right corner of the text is enclosed by the given rectangle.

BOOL TextFigure::Inside(const CRect& rcInside) const
{
  CRect rcText(m_ptText, m_szText);
  rcText.NormalizeRect();

  return rcInside.PtInRect(rcText.TopLeft()) &&
         rcInside.PtInRect(rcText.BottomRight());
}

// MoveOrModify and Move are also quite easy, the user cannot modify the text
// by dragging the mouse, so the only option is to move it.

void TextFigure::MoveOrModify(const CSize& szDistance)
{
  Move(szDistance);
}

void TextFigure::Move(const CSize& szDistance)
{
  m_ptText += szDistance;
}

// KeyDown and CharDown are called by one of the view objects as a response to
// the WM_KEYDOWN and WM_CHAR message. As the name implies, WM_CHAR is called
// when the user presses a non-system character (writable character with the
// ASCII number 32 – 122) while WM_KEYDOWN is sent for every key at the
// keyboard. There is also a message WM_KEYUP that is sent when the user
// releases the key; we have, however, no need for that message.

// KeyDown catches the home and end key, the left and right arrow key. These
// keys all set the carat index to an appropriate value. Furthermore, KeyDown
// catches the delete key; which, unless the caret index is already at the end
// of the text, erases the current character (the character after the caret
// index) and recalculates the size of the text and the start position of
// every character in the text by calling GenerateCaretArray. KeyDown also
// catches the backspace key in a similar manner; unless the caret index is
// already at the beginning of the text, it erases the character at the left
// of the caret index and recalculates the text calling GenerateCaretArray.

BOOL TextFigure::KeyDown(UINT uChar, CDC* pDC)
{
  int iLength = m_stText.GetLength();

  switch (uChar)
  {
    case VK_HOME:
      if (m_iEditIndex > 0)
      {
        m_iEditIndex = 0;
      }
      break;

    case VK_END:
      if (m_iEditIndex < iLength)
      {
        m_iEditIndex = iLength;
      }
      break;

    case VK_LEFT:
      if (m_iEditIndex > 0)
      {
        --m_iEditIndex;
      }
      break;

    case VK_RIGHT:
      if (m_iEditIndex < m_stText.GetLength())
      {
        ++m_iEditIndex;
      }
      break;

    case VK_DELETE:
      if (m_iEditIndex < m_stText.GetLength())
      {
        m_stText.Delete(m_iEditIndex);
        GenerateCaretArray(pDC);
        return TRUE;
      }
      break;

    case VK_BACK:
      if (m_iEditIndex > 0)
      {
        --m_iEditIndex;
        m_stText.Delete(m_iEditIndex);
        GenerateCaretArray(pDC);
        return TRUE;
      }
      break;
  }

  return FALSE;
}

// CharDown is called by one of the view objects when the user presses a
// regular key. We restrict the acceptable set of keys to the printable
// characters. If the caret index is located at the end of the text, we just
// add the character to the text regardless of the keyboard input mode.
// Otherwise, we insert or overwrite the character at the caret position. In
// either case, we increments the caret index by one and recalculates the text
// by calling GenerateCaretArray.

void TextFigure::CharDown(UINT uChar, CDC* pDC, KeyboardState eKeyboardState)
{
  if (m_iEditIndex == m_stText.GetLength())
  {
    m_stText.AppendChar((TCHAR) uChar);
  }

  else
  {
    switch (eKeyboardState)
    {
      case KS_INSERT:
        m_stText.Insert(m_iEditIndex, (TCHAR) uChar);
        break;

      case KS_OVERWRITE:
        m_stText.SetAt(m_iEditIndex, (TCHAR) uChar);
        break;
    }
  }

  ++m_iEditIndex;
  GenerateCaretArray(pDC);
}

// SetPreviousText is called when the user aborts the text input by pressing
// the escape key. It restores the text saved before the user input and
// recalculates its size and positions.

void TextFigure::SetPreviousText(CDC* pDC)
{
  m_stText = m_stPreviousText;
  GenerateCaretArray(pDC);
}

// Draw writes the text by calling TextOut. It writes the text with its top
// left corner at the given position. When it comes to writing text, we do
// not have to select a pen; we just set the color by calling SetTextColor.
// However, we need to select a font; the font is stored as points (1 point =
// 1/72 inch, 1 inch = 25.4 millimeters), so we convert the size of the font
// to hundreds of millimeters by callinf PointToMeters in the Font class.
// Finally, if the text is marked, we need to calculate, create, and paint the
// squares marking the text.

void TextFigure::Draw(CDC* pDC) const
{
  CFont cFont;
  cFont.CreateFontIndirect(m_font.PointsToMeters());
  CFont* pPrevFont = pDC->SelectObject(&cFont);

  pDC->SetTextColor((COLORREF) GetColor());
  pDC->TextOut(m_ptText.x, m_ptText.y + m_szText.cy,
               m_stText);

  pDC->SelectObject(pPrevFont);

  if (IsMarked())
  {
    CPen pen(PS_SOLID, 0, BLACK);
    CPen* pOldPen = pDC->SelectObject(&pen);

    CBrush brush(BLACK);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    int xLeft = m_ptText.x;
    int xRight = m_ptText.x + m_szText.cx;

    int yTop = m_ptText.y;
    int yBottom = m_ptText.y + m_szText.cy;

    int xCenter = m_ptText.x + m_szText.cx / 2;
    int yCenter = m_ptText.y + m_szText.cy / 2;

    CRect rcLeft(xLeft - (SQUARE_SIDE / 2),
                 yCenter - (SQUARE_SIDE / 2),
                 xLeft + (SQUARE_SIDE / 2),
                 yCenter + (SQUARE_SIDE / 2));

    CRect rcRight(xRight - (SQUARE_SIDE / 2),
                  yCenter - (SQUARE_SIDE / 2),
                  xRight + (SQUARE_SIDE / 2),
                  yCenter + (SQUARE_SIDE / 2));

    CRect rcTop(xCenter - (SQUARE_SIDE / 2),
                yTop - (SQUARE_SIDE / 2),
                xCenter + (SQUARE_SIDE / 2),
                yTop + (SQUARE_SIDE / 2));

    CRect rcBottom(xCenter - (SQUARE_SIDE / 2),
                   yBottom - (SQUARE_SIDE / 2),
                   xCenter + (SQUARE_SIDE / 2),
                   yBottom + (SQUARE_SIDE / 2));

    pDC->Rectangle(rcLeft);
    pDC->Rectangle(rcRight);
    pDC->Rectangle(rcTop);
    pDC->Rectangle(rcBottom);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
  }
}

// GetArea starts by calculating the unmarked size. It is an easy task to
// create a CRect object because we already have to top left position
// (m_ptText) and its size (m_szText). In case the text is marked, we just add
// some margins for the squares.

CRect TextFigure::GetArea() const
{
  CRect rcText(m_ptText, m_szText);
  rcText.NormalizeRect();

  if (IsMarked())
  {
    rcText.left  -= (SQUARE_SIDE / 2);
    rcText.right += (SQUARE_SIDE / 2);

    rcText.top    -= (SQUARE_SIDE / 2);
    rcText.bottom += (SQUARE_SIDE / 2);
  }

  return rcText;
}

// SetFont sets the font of the text. We need to recalculate the
// characters positions by calling GenerateCaretArray.

void TextFigure::SetFont(const Font& font, CDC* pDC)
{
  m_font = font;
  GenerateCaretArray(pDC);
}

// GenerateCaretArray determines the size and position of each character in the
// text. The positions relative the beginning of the text (the x-position) is
// for each character stored in m_posVector. The size of the text is stored in
// m_szText and the average character width with the given font is stored in
// m_iAverageWidth.

// First, we need to select a font with coordinates given in logical units.
// The translation from typographical points to hundreds of millimeters is
// done by PointsToMetrics in the Font class.

// To find the size of the text we have two cases. If the text is non-empty,
// we call GetTextExtent to receive the size of the text. If the text is
// empty, we use the field tmHeight of the TEXTMETRIC structure above and we
// set the width of the text to zero. A call to GetTextExtent in this case
// would return a zero size. The size is given in logical units because the
// function calling GenerateCaretArray has initialized the device context
// with the logical coordinate system.

// After the size of the whole text is set, we need to find the horizontal
// starting position (the x-position) of each character in the text. We
// traverse through the text and find the width of each character by calling
// GetTextExtent. We set the size of the vector m_caretArray to one more than
// the size of the text, in order to store also the rightmost position of the
// text.

void TextFigure::GenerateCaretArray(CDC* pDC)
{
  CFont cFont;
  cFont.CreateFontIndirect(m_font.PointsToMeters());
  CFont* pPrevFont = pDC->SelectObject(&cFont);

  TEXTMETRIC textMetric;
  pDC->GetTextMetrics(&textMetric);
  m_iAverageWidth = textMetric.tmAveCharWidth;

  if (!m_stText.IsEmpty())
  {
    m_szText = pDC->GetTextExtent(m_stText);
  }

  else
  {
    m_szText.SetSize(0, textMetric.tmHeight);
  }

  int iWidth = 0, iSize = m_stText.GetLength();
  m_caretArray.SetSize(iSize + 1);

  for (int iIndex = 0; iIndex < iSize; ++iIndex)
  {
    CSize szChar = pDC->GetTextExtent
                        (m_stText.Mid(iIndex, 1));
    m_caretArray[iIndex] = iWidth;
    iWidth += szChar.cx;
  }

  m_caretArray[iSize] = m_szText.cx;
  pDC->SelectObject(pPrevFont);
}

// GetCaretArea calculates and returns the size and position of the caret.
// First we define the top left position of the character the caret marker is
// set to. Then we have two cases to considering depending on the input mode
// of the keyboard. In case of insert mode, the caret shall be a thin vertical
// blinking line. We set the width to one and the height to the height of the
// text. Remember that we are dealing with logical coordinates (hundreds of
// millimeters), so the width will most likely be rounded down to zero when
// translated to device coordinates. However, the function OnUpdate in the
// view class will take this into consideration and set the width to at least
// one device unit. If the keyboard is set to overwrite input mode, the caret
// marker shall be a small blinking rectangle which width shall be the one of
// an average character in the current font. We use the value of
// m_iAverageWidth, which was assessed by a previous call to
// GenerateCaretArray.

CRect TextFigure::GetCaretArea(KeyboardState eKeyboardState)
{
  CPoint ptCaret(m_ptText.x + m_caretArray[m_iEditIndex],
                 m_ptText.y);

  switch (eKeyboardState)
  {
    case KS_INSERT:
      {
        CSize szCaret(1, m_szText.cy);
        return CRect(ptCaret, ptCaret + szCaret);
      }
      break;

      case KS_OVERWRITE:
      {
        CSize szCaret(m_iAverageWidth, m_szText.cy);
        return CRect(ptCaret, ptCaret + szCaret);
      }
      break;
  }

  return CRect();
}

// GetCursor returns a handle to a cursor. There are only two cases to
// consider. If the user creates or edits the text, we return a thin vertical
// bar. If the user is moving the text, we return a cursor with four arrows
// representing movement in all directions.

HCURSOR TextFigure::GetCursor() const
{
  switch (m_eDragMode)
  {
    case CREATE_TEXT:
    case EDIT_TEXT:
      return AfxGetApp()->LoadStandardCursor(IDC_IBEAM);

    case MOVE_TEXT:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
  }

  return NULL;
}
