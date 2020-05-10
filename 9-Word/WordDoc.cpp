#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "Font.h"
#include "Caret.h"
#include "Check.h"

#include "Line.h"
#include "Position.h"
#include "Paragraph.h"
#include "Page.h"

#include "WordView.h"
#include "WordDoc.h"
#include "Word.h"

IMPLEMENT_DYNCREATE(CWordDoc, CDocument)

BEGIN_MESSAGE_MAP(CWordDoc, CDocument)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, OnUpdateAlignLeft)
  ON_COMMAND(ID_ALIGN_LEFT, OnAlignLeft)

  ON_UPDATE_COMMAND_UI(ID_ALIGN_CENTER, OnUpdateAlignCenter)
  ON_COMMAND(ID_ALIGN_CENTER, OnAlignCenter)

  ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, OnUpdateAlignRight)
  ON_COMMAND(ID_ALIGN_RIGHT, OnAlignRight)

  ON_UPDATE_COMMAND_UI(ID_ALIGN_JUSTIFIED, OnUpdateAlignJustifed)
  ON_COMMAND(ID_ALIGN_JUSTIFIED, OnAlignJustified)

  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCut)
  ON_COMMAND(ID_EDIT_CUT, OnCut)

  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
  ON_COMMAND(ID_EDIT_COPY, OnCopy)

  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
  ON_COMMAND(ID_EDIT_PASTE, OnPaste)

  ON_COMMAND(ID_FORMAT_FONT, OnFont)
END_MESSAGE_MAP()

// This class must have a default constructor because it will be created
// dynamically by the Application Framework. The destructor de-allocates the
// memory associated with the paragraph and copy arrays. It does also de-
// allocate the memory associated with the next font. This operation is safe
// even if the pointer points at null, because the delete operator does
// nothing in that case.

CWordDoc::CWordDoc()
 :m_eKeyboardState(KM_INSERT),
  m_eWordState(WS_EDIT),
  m_psEdit(0, 0),
  m_psFirstMark(0, 0),
  m_psLastMark(0, 0),
  m_pNextFont(NULL)
{
  // Empty.
}

CWordDoc::~CWordDoc()
{
  int iParagraphs = (int) m_paragraphArray.GetSize();
  for (int iParagraph = 0; iParagraph < iParagraphs; ++iParagraph)
  {
    delete m_paragraphArray[iParagraph];
  }

  ClearCopyArray();
  delete m_pNextFont;
}
// OnNewDocument does the work of a constructor. It creates and initializes
// the first paragraph with the standard system font and left alignment. The
// paragraph is recalculated and added to the paragraph array. The first
// page is also defined, it holds the new paragraph as its first and last
// paragraph. Note that this method is only called when the users creates a
// new document, not when they open an existing document.

BOOL CWordDoc::OnNewDocument()
{
  Font defaultFont(TEXT("Times New Roman"), 48);
  Paragraph* pNewParagraph;
  check_memory(pNewParagraph =
               new Paragraph(defaultFont, ALIGN_LEFT));

  CClientDC dc(m_pView);
  m_pView->OnPrepareDC(&dc);

  pNewParagraph->Recalculate(&dc);
  m_paragraphArray.Add(pNewParagraph);

  Page page(0, 0);
  m_pageArray.Add(page);

  return CDocument::OnNewDocument();
}

// Serialize reads from and writes to the file connected to the parameter
// archive the paragraph array. As this method is called by the Application
// Framework every time the user loads or saves a document, we first of all
// have to call Serialize in the MFC base class CDocument.

// We cannot serialize the paragraph array itself, as it holds pointers to
// paragraph objects, not the object themselves. Instead, we first read or
// write the size of the array, and then we serialize the paragraphs
// one-by-one. When we read from the archive, we have to create the paragraph
// first, and then serialize it. Finally, we add it to the array.

void CWordDoc::Serialize(CArchive& archive)
{
  CDocument::Serialize(archive);

  if (archive.IsStoring())
  {
    int iSize = (int) m_paragraphArray.GetSize();
    archive << iSize;

    for (int iIndex = 0; iIndex < iSize; ++iIndex)
    {
      m_paragraphArray[iIndex]->Serialize(archive);
    }
  }

  if (archive.IsLoading())
  {
    int iSize;
    archive >> iSize;

    for (int iCount = 0; iCount < iSize; ++iCount)
    {
      // We must first create the new paragraph before we serialize it.
      Paragraph* pParagraph;
      check_memory(pParagraph = new Paragraph());

      pParagraph->Serialize(archive);
      m_paragraphArray.Add(pParagraph);
    }
  }

  m_pageArray.Serialize(archive);
}

// KeyDown and ShiftKeyDown is called by the view class when the user presses
// any of the special keys. They call the appropriate method to handle the
// key.

void CWordDoc::KeyDown(UINT uChar, CDC* pDC)
{
  switch (uChar)
  {
    case VK_LEFT:
      LeftArrowKey();
      break;

    case VK_RIGHT:
      RightArrowKey();
      break;

    case VK_UP:
      UpArrowKey();
      break;

    case VK_DOWN:
      DownArrowKey();
      break;

    case VK_PRIOR:
      PageUpKey(pDC);
      break;

    case VK_NEXT:
      PageDownKey(pDC);
      break;

    case VK_HOME:
      HomeKey();
      break;

    case VK_END:
      EndKey();
      break;

    case VK_DELETE:
      DeleteKey(pDC);
      break;

    case VK_BACK:
      BackspaceKey(pDC);
      break;

    case VK_RETURN:
      ReturnKey(pDC);
      break;

    case VK_INSERT:
      InsertKey();
      break;
  }

  delete m_pNextFont;
  m_pNextFont = NULL;

  MakeVisible();
  UpdateCaret();
}

void CWordDoc::ShiftKeyDown(UINT uChar, CDC* pDC)
{
  switch (uChar)
  {
    case VK_LEFT:
      ShiftLeftArrowKey();
      break;

    case VK_RIGHT:
      ShiftRightArrowKey();
      break;

    case VK_UP:
      ShiftUpArrowKey();
      break;

    case VK_DOWN:
      ShiftDownArrowKey();
      break;

    case VK_PRIOR:
      ShiftPageUpKey(pDC);
      break;

    case VK_NEXT:
      ShiftPageDownKey(pDC);
      break;

    case VK_HOME:
      ShiftHomeKey();
      break;

    case VK_END:
      ShiftEndKey();
      break;

    case VK_DELETE:
      DeleteKey(pDC);
      break;

    case VK_BACK:
      BackspaceKey(pDC);
      break;

    case VK_RETURN:
      ReturnKey(pDC);
      break;

    case VK_INSERT:
      InsertKey();
      break;
    }

  delete m_pNextFont;
  m_pNextFont = NULL;

  MakeVisible();
  UpdateCaret();
}

// When the user presses one of the arrow keys as well as the page up or page
// down key without pressing the shift key, we must make sure the application
// is in edit mode, EnsureEditStatus takes care of that. If the user, on the
// other hand, presses one of those keys together with the shift key we must
// make sure the application is in mark mode, EnsureMarkMode deals with that.

void CWordDoc::EnsureEditStatus()
{
  if (m_eWordState == WS_MARK)
  {
    RectSet repaintSet;
    GetRepaintSet(repaintSet, m_psFirstMark, m_psLastMark);

    m_eWordState = WS_EDIT;
    m_psEdit = m_psLastMark;

    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  }
}

void CWordDoc::EnsureMarkStatus()
{
  if (m_eWordState == WS_EDIT)
  {
    m_eWordState = WS_MARK;

    m_psFirstMark = m_psEdit;
    m_psLastMark = m_psEdit;
  }
}

// LeftArrowKey is called when the user presses the left arrow key. If the
// caret position in not at the beginning of the paragraph, we just move it
// one step to the left. If it is at the beginning, we move the caret to the
// end of the preceding paragraph. If there is no preceding paragraph, nothing
// happens.

void CWordDoc::LeftArrowKey()
{
  EnsureEditStatus();

  if (m_psEdit.Character() > 0)
  {
    --m_psEdit.Character();
  }

  else if (m_psEdit.Paragraph() > 0)
  {
    Paragraph* pPreviousParagraph = m_paragraphArray[--m_psEdit.Paragraph()];
    m_psEdit.Character() = pPreviousParagraph->GetLength();
  }
}

// ShiftLeftArrowKey is a bit more complicated than LeftArrowKey above. First,
// we make sure the application is in mark mode and we get the set of marked
// characters. Then we move position of the last marked character one step to
// the left unless it already is at the beginning of the paragraph. If it is,
// we set the position to the last character of the preceding paragraph, if
// there is one. In both cases, we need to update the marked area, and we call
// SymmetricDifference in order to pick out only the characters that actually
// has been marked or unmarked. We do not want to update the characters that
// was marked before and after the operation. Finally, if this operation sets
// first and last marked character equal, we change to edit mode.

void CWordDoc::ShiftLeftArrowKey()
{
  EnsureMarkStatus();

  RectSet unmarkRepaintSet;
  GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

  if (m_psLastMark.Character() > 0)
  {
    --m_psLastMark.Character();
  }

  else if (m_psLastMark.Paragraph() > 0)
  {
    Paragraph* pPreviousParagraph =
    m_paragraphArray[--m_psLastMark.Paragraph()];

    m_psLastMark.Character() = pPreviousParagraph->GetLength();
  }

  RectSet markRepaintSet;
  GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

  RectSet resultRepaintSet =
  RectSet::SymmetricDifference(unmarkRepaintSet, markRepaintSet);
  UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

  if (m_psFirstMark == m_psLastMark)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = m_psFirstMark;
  }
}

// RightArrowKey is called when the user presses the right arrow key. If the
// caret position in not at the end of the paragraph, we just move it one
// step to the right. If it is at the end, we move the caret to the beginning
// of the following paragraph. If there is no following paragraph, nothing
// happens.

void CWordDoc::RightArrowKey()
{
  EnsureEditStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];

  if (m_psEdit.Character() < pParagraph->GetLength())
  {
    ++m_psEdit.Character();
  }

  else if (m_psEdit.Paragraph() < (m_paragraphArray.GetSize() - 1))
  {
    ++m_psEdit.Paragraph();
    m_psEdit.Character() = 0;
  }
}

// ShiftRightArrowKey moves the position of the last marked character in a way
// similar to ShiftLeftKey above. We move the position one step to the right
// or the beginning of the next paragraph, is there is one. We also use
// the symmetric difference between the new and old marked set.

void CWordDoc::ShiftRightArrowKey()
{
  EnsureMarkStatus();

  RectSet unmarkRepaintSet;
  GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];

  if (m_psLastMark.Character() < pParagraph->GetLength())
  {
    ++m_psLastMark.Character();
  }

  else if (m_psLastMark.Paragraph() < m_paragraphArray.GetSize() - 1)
  {
    Paragraph* pNextParagraph =
    m_paragraphArray[++m_psLastMark.Paragraph()];

    m_psLastMark.Character() = pNextParagraph->GetLength();
  }

  RectSet markRepaintSet;
  GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

  RectSet resultRepaintSet =
      RectSet::SymmetricDifference(unmarkRepaintSet, markRepaintSet);
  UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

  if (m_psFirstMark == m_psLastMark)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = m_psFirstMark;
  }
}

// When the user presses the up arrow key, we have to find the key above the
// current character. We do so by simulating a mouse click slightly above the
// character. As the characters may differ in height, we cannot be sure the
// correct character is the highest of the line, we have to look up the
// height of the line as well.

void CWordDoc::UpArrowKey()
{
  EnsureEditStatus();

  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];

  CRect rcLine = pParagraph->CharToLineRect(m_psEdit.Character());
  CRect rcChar = pParagraph->CharToRect(m_psEdit.Character());

  if (rcLine.top > 0)
  {
    m_psEdit = PointToChar(CPoint(rcChar.left, rcLine.top - 1));
    MakeVisible();
  }
}

// ShiftUpArrowKey is called when the user presses the up arrow key together
// with the shift key. Its task is to move the last marked position one line
// upwards. In order to do so, we first make sure the application is in mark
// mode, and then we simulate a mouse click one logical unit above the current
// line by calling PointToChar. We only want to update characters marked or
// unmarked by this operation. Characters not affected are not updated. With
// this in view, we call SymmetricDifference to sort out the characters to be
// updated. Finally, we check if the first and last marked positions are
// equal. If so, the application is set to edit mode.

void CWordDoc::ShiftUpArrowKey()
{
  // We make sure the application is in mark mode.
  EnsureMarkStatus();

  // We extract the set of unmarked character rectangles.
  RectSet unmarkRepaintSet;
  GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);
  
  // We find the dimensions of the current line and character.
  Paragraph* pParagraph = m_paragraphArray[m_psFirstMark.Paragraph()];
  CRect rcLine = pParagraph->CharToLineRect(m_psFirstMark.Character());
  CRect rcChar = pParagraph->CharToRect(m_psFirstMark.Character());

  // If the current character is not already at the top line, we simulate the
  // mouse click.

  if (rcLine.top > 0)
  {
    m_psFirstMark = PointToChar(CPoint(rcChar.left, rcLine.top - 1));

    // We extract the set of marked character rectangles.
    RectSet markRepaintSet;
    GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

    // We extract the affected character rectangles.
    RectSet resultRepaintSet =
        RectSet::SymmetricDifference(unmarkRepaintSet, markRepaintSet);
    UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

    // We set the application in edit mode if the first and last marked
    // positions are equal.

    if (m_psFirstMark == m_psLastMark)
    {
        m_eWordState = WS_EDIT;
        m_psEdit = m_psFirstMark;
    }
  }
}

// When the user presses the down arrow key, we simulate a mouse click
// slightly under the current character. Just as in the case of UpArrowKey
// above, we have to find the line of the current character. One additional
// problem is that it might be the last line of the paragraph and that there
// might be some space to the next one, if there is one. That gives that we
// have to look up the start position of the next paragraph to make sure our
// simulated mouse click really hits the next paragraph.

void CWordDoc::DownArrowKey()
{
  EnsureEditStatus();

  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  CRect rcLine = pParagraph->CharToLineRect(m_psEdit.Character());
  CRect rcChar = pParagraph->CharToRect(m_psEdit.Character());

  int iStartPos = pParagraph->GetStartPos();
  int iHeight = pParagraph->GetHeight();

  if (rcLine.bottom < (iStartPos + iHeight))
  {
    m_psEdit = PointToChar(CPoint(rcChar.left, rcLine.bottom + 1));
    MakeVisible();
  }

  else if (m_psEdit.Paragraph() < (m_paragraphArray.GetSize() - 1))
  {
    Paragraph* pNextParagraph = m_paragraphArray[m_psEdit.Paragraph() + 1];
    int iNextStartPos = pNextParagraph->GetStartPos();

    m_psEdit = PointToChar(CPoint(rcChar.left, iNextStartPos));
    MakeVisible();
  }
}

// ShiftDownArrowKey is called when the user presses the down arrow key
// together with the shift key. Its task is to move the last marked position
// one line downwards. Similar to ShiftUpArrowKey above, we first make sure
// the application is in mark mode, and then we simulate a mouse click one
// logical unit below the current line by calling PointToChar. We only want to
// update characters marked or unmarked by this operation. Characters not
// affected are not updated. With this in view, we call SymmetricDifference to
// sort out the characters to be updated. Finally, we check if the first and
// last marked positions are equal. If so, the application is set to edit
// mode.

// Unlike ShiftUpArrowKey above, we cannot simple simulate a mouse click below
// the current line as the line may be the last one of the paragraph and the
// next paragraph may be at the beginning of the next page, in which case
// there may be some space between the paragraphs. In that case, the simulated
// mouse click would only hit the original character. Instead, we click at the
// first line of the next paragraph, if there is one.

void CWordDoc::ShiftDownArrowKey()
{
  // We make sure the application is in mark mode.
  EnsureMarkStatus();

  // We extract the set of unmarked character rectangles.
  RectSet unmarkRepaintSet;
  GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

  // We find the dimensions of the current line and character.
  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];

  CRect rcLine = pParagraph->CharToLineRect(m_psLastMark.Character());
  CRect rcChar = pParagraph->CharToRect(m_psLastMark.Character());

  int iStartPos = pParagraph->GetStartPos();
  int iHeight = pParagraph->GetHeight();

  // If the next line in the same paragraph?

  if (rcLine.bottom < (iStartPos + iHeight))
  {
    m_psLastMark = PointToChar(CPoint(rcChar.left, rcLine.bottom + 1));
  }

  //If not, we simulate the mouse click at the beginning of the net paragraph.

  else if (m_psLastMark.Paragraph() < (m_paragraphArray.GetSize() - 1))
  {
    Paragraph* pNextParagraph = m_paragraphArray[m_psLastMark.Paragraph() + 1];
    int iNextStartPos = pNextParagraph->GetStartPos();

    m_psLastMark = PointToChar(CPoint(rcChar.left, iNextStartPos));
  }

  // We extract the set of marked character rectangles.
  RectSet markRepaintSet;
  GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

  // We extract the affected character rectangles.
  RectSet resultRepaintSet =
          RectSet::SymmetricDifference(unmarkRepaintSet,markRepaintSet);
  UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

  // We set the application in edit mode if the first and last marked
  // positions are equal.

  if (m_psFirstMark == m_psLastMark)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = m_psFirstMark;
  }
}

// PageUpKey finds the size of the view's client area in logical units, makes
// sure the application is in edit mode, and moves the edit position one page
// up by simulating a mouse click with PointToChar.

void CWordDoc::PageUpKey(CDC* pDC)
{
  CRect rcClient;
  m_pView->GetClientRect(rcClient);
  pDC->DPtoLP(rcClient);

  EnsureEditStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  CRect rcEdit = pParagraph->CharToRect(m_psEdit.Character());
  CPoint ptEdit((rcEdit.left + rcEdit.right) / 2,
                (rcEdit.top + rcEdit.bottom) / 2);
  m_psEdit = PointToChar(CPoint(ptEdit.x, ptEdit.y - rcClient.top));
}

// ShiftPageUpKey finds the size of the view's client area in logical units,
// makes sure the application is in edit mode, and moves the last marked
// position one page up by simulating a mouse click with PointToChar.

void CWordDoc::ShiftPageUpKey(CDC* pDC)
{
  CRect rcClient;
  m_pView->GetClientRect(rcClient);
  pDC->DPtoLP(rcClient);

  EnsureMarkStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];
  CRect rcLastMark = pParagraph->CharToRect(m_psLastMark.Character());
  CPoint ptLastMark((rcLastMark.left + rcLastMark.right) / 2,
                    (rcLastMark.top + rcLastMark.bottom) / 2);
  m_psLastMark = PointToChar(CPoint(ptLastMark.x,
                             ptLastMark.y - rcClient.top));
}

// Similar to PageUpKey above, PageDownKey finds the size of the view's client
// area in logical units, makes sure the application is in edit mode, and
// moves the edit position one page down by simulating a mouse click with
// PointToChar.

void CWordDoc::PageDownKey(CDC* pDC)
{
  CRect rcClient;
  m_pView->GetClientRect(rcClient);
  pDC->DPtoLP(rcClient);

  EnsureEditStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  CRect rcEdit = pParagraph->CharToRect(m_psEdit.Character());
  CPoint ptEdit((rcEdit.left + rcEdit.right) / 2,
                (rcEdit.top + rcEdit.bottom) / 2);
  m_psEdit = PointToChar(CPoint(ptEdit.x, ptEdit.y + rcClient.top));
}

// Similar to ShiftPageUpKey above, ShiftPageDownKey finds the size of the
// view's client area in logical units, makes sure the application is in edit
// mode, and moves the last marked position one page up by simulating a mouse
// click with PointToChar.

void CWordDoc::ShiftPageDownKey(CDC* pDC)
{
  CRect rcClient;
  m_pView->GetClientRect(rcClient);
  pDC->DPtoLP(rcClient);

  EnsureMarkStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];
  CRect rcLastMark = pParagraph->CharToRect(m_psLastMark.Character());
  CPoint ptLastMark((rcLastMark.left + rcLastMark.right) / 2,
                    (rcLastMark.top + rcLastMark.bottom) / 2);
  m_psLastMark = PointToChar(CPoint(ptLastMark.x,
                                    ptLastMark.y + rcClient.top));
}

// HomeKey is called when the user presses the home key. It makes sure the
// application is in edit mode and sets the current edit index to the first
// position of the current line. Finally, it makes sure the new position is
// visible.

void CWordDoc::HomeKey()
{
  EnsureEditStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  int iHomeChar = pParagraph->GetHomeChar(m_psEdit.Character());

  if (iHomeChar < m_psEdit.Character())
  {
    m_psEdit.Character() = iHomeChar;
    MakeVisible();
  }
}

// ShiftHomeKey is called when the user presses the home key together with the
// shift key. It makes sure the application is in mark mode and sets the last
// marked position to the first one of the current line unless it is already
// at the first position. It only extracts and updates the rectangles of those
// affected by the operation by calling SymmetricDifference.

void CWordDoc::ShiftHomeKey()
{
  EnsureMarkStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];
  int iHomeChar = pParagraph->GetHomeChar(m_psLastMark.Character());

  if (iHomeChar < m_psLastMark.Character())
  {
    RectSet unmarkRepaintSet;
    GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

    m_psLastMark.Character() = iHomeChar;

    RectSet markRepaintSet;
    GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

    RectSet resultRepaintSet =
          RectSet::SymmetricDifference(unmarkRepaintSet,markRepaintSet);
    UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

    if (m_psFirstMark == m_psLastMark)
    {
      m_eWordState = WS_EDIT;
      m_psEdit = m_psFirstMark;
    }
  }
}

// EndKey is called when the user presses the end key. It makes sure the
// application is in edit mode and sets the current edit index to the last
// position of the current line. Finally, it makes sure the new position is
// visible.

void CWordDoc::EndKey()
{
  EnsureEditStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  int iEndChar = pParagraph->GetEndChar(m_psEdit.Character());

  if (iEndChar > m_psEdit.Character())
  {
    m_psEdit.Character() = iEndChar;
  }
}

// ShiftEndKey is called when the user presses the end key together with the
// shift key. It makes sure the application is in mark mode and sets the last
// marked position to the last one of the current line unless it is already
// at the last position. It only extracts and updates the rectangles of those
// affected by the operation by calling SymmetricDifference.

void CWordDoc::ShiftEndKey()
{
  EnsureMarkStatus();
  Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];
  int iEndChar = pParagraph->GetEndChar(m_psLastMark.Character());

  if (iEndChar > m_psLastMark.Character())
  {
    RectSet unmarkRepaintSet;
    GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

    m_psLastMark.Character() = iEndChar;

    RectSet markRepaintSet;
    GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

    RectSet resultRepaintSet =
          RectSet::SymmetricDifference(unmarkRepaintSet,markRepaintSet);
    UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);
  }
}

// ReturnKey is called when the user presses the return key. If the
// application is in mark mode, we first have to remove the marked area and
// set the application in edit mode. Then we split the paragraph and insert
// the new paragraph into the paragraph array. Finally, we call
// UpdateParagraphAndPageArray that updates the page and paragraph arrays
// because the operation may affect the following paragraph and maybe moves
// one or several paragraphs to another page.

void CWordDoc::ReturnKey(CDC* pDC)
{
  if (m_eWordState == WS_MARK)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = min(m_psFirstMark, m_psLastMark);

    RectSet repaintSet;
    DeleteText(repaintSet, pDC, m_psFirstMark, m_psLastMark);
    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  }

  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  Paragraph* pNewParagraph = pParagraph->Split(m_psEdit.Character());

  pParagraph->Recalculate(pDC);
  pNewParagraph->Recalculate(pDC);

  m_paragraphArray.InsertAt(++m_psEdit.Paragraph(), pNewParagraph);
  m_psEdit.Character() = 0;

  UpdateParagraphAndPageArray();
  SetModifiedFlag();
}

// InsertKey is quite simple, it just switches the keyboard mode between
// insert and overwrite. The keyboard mode plays a part when the user adds a
// character to a paragraph, whether the new character shall be inserted
// before or if it shall overwrite the current one. It does also affect the
// shape of the carat, in insert mode, the caret is a vertical line; in
// overwrite mode, it is a block.

void CWordDoc::InsertKey()
{
  switch (m_eKeyboardState)
  {
    case KM_INSERT:
      m_eKeyboardState = KM_OVERWRITE;
      break;

    case KM_OVERWRITE:
      m_eKeyboardState = KM_INSERT;
      break;
  }
}

// In edit mode, DeleteKey deletes the key of the current position if it is
// not at the end of the paragraph. In that case, it instead merges the
// current paragraph with the next one, if there is one. In mark mode, it
// deletes the marked text, which may cover several paragraphs.

void CWordDoc::DeleteKey(CDC* pDC)
{
  switch (m_eWordState)
  {
    case WS_EDIT:
      {
        // In edit mode, we delete the current character unless it is at the
        // end of the paragraph.

        Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];

        if (m_psEdit.Character() < pParagraph->GetLength())
        {
          pParagraph->DeleteText(m_psEdit.Character(), m_psEdit.Character() + 1);

          RectSet repaintSet;
          pParagraph->Recalculate(pDC, &repaintSet);
          UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

          SetModifiedFlag();
        }

        // If it is at the end of the paragraph, we append the next paragraph
        // (if there is one) to this one.

        else if (m_psEdit.Paragraph() < (m_paragraphArray.GetSize() - 1))
        {
          Paragraph* pNextParagraph =
                     m_paragraphArray[m_psEdit.Paragraph() +1 ];
          pParagraph->Append(pNextParagraph);

          RectSet repaintSet;
          pParagraph->Recalculate(pDC, &repaintSet);
          UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

          m_paragraphArray.RemoveAt(m_psEdit.Paragraph()+1);
          delete pNextParagraph;

          SetModifiedFlag();
        }
      }
      break;

    // In mark mode, we delete the marked portion of the text and set the
    // application in edit mode.

    case WS_MARK:
      m_eWordState = WS_EDIT;
      m_psEdit = min(m_psFirstMark, m_psLastMark);

      RectSet repaintSet;
      DeleteText(repaintSet, pDC, m_psFirstMark, m_psLastMark);
      UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

      SetModifiedFlag();
      break;
  }

  UpdateParagraphAndPageArray();
}

// BackSpace is quite simple, it just calls DeleteKey. In edit mode, it first
// moves one step at the left unless the edit position is not already at the
// beginning of the document. If it is, nothing happens. When a portion of the
// text is marked, the delete key and the backspace key has the same effect.

void CWordDoc::BackspaceKey(CDC* pDC)
{
  switch (m_eWordState)
  {
    case WS_EDIT:
      if (!((m_psEdit.Paragraph() == 0) && (m_psEdit.Character() == 0)))
      {
        LeftArrowKey();
        DeleteKey(pDC);
      }
      break;

    case WS_MARK:
      DeleteKey(pDC);
      break;
  }
}

// CharDown is called every time the user presses a regular character. If the
// character is not printable, nothing happens. Otherwise, if the text is
// marked, that text is first removed. Thereafter, the character is added to
// there current paragraph, the paragraph array is updated, and the next font
// is set to null.

void CWordDoc::CharDown(UINT uChar, CDC* pDC)
{
  if (isprint(uChar))
  {
    RectSet repaintSet;

    if (m_eWordState == WS_MARK)
    {
      DeleteText(repaintSet, pDC, m_psFirstMark, m_psLastMark);
      m_eWordState = WS_EDIT;
      m_psEdit = min(m_psFirstMark, m_psLastMark);
    }

    Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
    pParagraph->AddChar(m_psEdit.Character(), uChar, m_pNextFont, m_eKeyboardState);
    pParagraph->Recalculate(pDC, &repaintSet);

    ++m_psEdit.Character();

    delete m_pNextFont;
    m_pNextFont = NULL;

    SetModifiedFlag();
    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

    UpdateParagraphAndPageArray();
    MakeVisible();
    UpdateCaret();
  }
}

// When the users click with the mouse, we first have to decide which
// paragraph they hit. PointToParagraph traverses the paragraph array to find
// the correct paragraph. If the user clicks beyond the last paragraph, the
// last one is returned. Likewise, if the user clicks at the end of a page,
// beyond the last paragraph of the page, the last paragraph of that page is
// returned.

int CWordDoc::PointToParagraph(const CPoint& ptMouse) const
{
  int iParagraphs = (int) m_paragraphArray.GetSize();
  for (int iParagraph = 0; iParagraph < iParagraphs; ++iParagraph)
  {
    Paragraph* pParagraph = m_paragraphArray[iParagraph];

    if (ptMouse.y < pParagraph->GetStartPos())
    {
      return iParagraph - 1;
    }
  }

  return iParagraphs - 1;
}

// PointToChar returns the position the right paragraph and character index by
// calling PointToParagraph.

Position CWordDoc::PointToChar(const CPoint& ptMouse) const
{
  int iParagraph = PointToParagraph(ptMouse);
  Paragraph* pParagraph = m_paragraphArray[iParagraph];
  int iChar = pParagraph->PointToChar(ptMouse);
  return Position(iParagraph, iChar);
}

// When the user presses the left button of the mouse, MouseDown is called.
// First, we have to unmark any marked portion of the text. Then we set the
// application to mark mode.

void CWordDoc::MouseDown(const CPoint& ptMouse)
{
  if (m_eWordState == WS_MARK)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = m_psFirstMark;

    RectSet repaintSet;
    GetRepaintSet(repaintSet, m_psFirstMark, m_psLastMark);
    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  }

  m_eWordState = WS_MARK;
  m_psFirstMark = PointToChar(ptMouse);
  m_psLastMark = m_psFirstMark;

  delete m_pNextFont;
  m_pNextFont = NULL;

  MakeVisible();
  UpdateCaret();
}

// When the user releases the left button of the mouse, we just have to check
// the last position. If it is the same as the first one (the user presses and
// releases the mouse button on the same character), we change to edit mode.

void CWordDoc::MouseDrag(const CPoint& ptMouse)
{
  Position psNewLastMark = PointToChar(ptMouse);

  if (m_psLastMark != psNewLastMark)
  {
    RectSet unmarkRepaintSet;
    GetRepaintSet(unmarkRepaintSet, m_psFirstMark, m_psLastMark);

    m_psLastMark = psNewLastMark;

    RectSet markRepaintSet;
    GetRepaintSet(markRepaintSet, m_psFirstMark, m_psLastMark);

    RectSet resultRepaintSet =
        RectSet::SymmetricDifference(unmarkRepaintSet, markRepaintSet);
    UpdateAllViews(NULL, 0, (CObject*) &resultRepaintSet);

    MakeVisible();
  }
}

// When the user releases the left button of the mouse, we just have to check
// the last position. If it is the same as the first one (the user presses and
// releases the mouse button on the same character), we change to mark mode.

void CWordDoc::MouseUp()
{
  if (m_psFirstMark == m_psLastMark)
  {
    m_eWordState = WS_EDIT;
    m_psEdit = m_psLastMark;
  }

  else
  {
    m_eWordState =  WS_MARK;
  }

  MakeVisible();
  UpdateCaret();
}

// When the user double clicks with the left button of the mouse, the word hit
// by the mouse shall be marked. The application is is edit mode and the
// correct character is noted, because a double click always is preceded by
// call to MouseDown and MouseUp. If the mouse is on a word, we mark and
// update the word.

void CWordDoc::DoubleClick()
{
  RectSet repaintSet;
  Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];

  int iFirstChar, iLastChar;
  if (pParagraph->GetWord(m_psEdit.Character(), iFirstChar, iLastChar))
  {
    m_eWordState = WS_MARK;

    m_psFirstMark.Paragraph() = m_psEdit.Paragraph();
    m_psFirstMark.Character() = iFirstChar;

    m_psLastMark.Paragraph() = m_psEdit.Paragraph();
    m_psLastMark.Character() = iLastChar;

    RectSet repaintSet;
    GetRepaintSet(repaintSet, m_psFirstMark, m_psLastMark);
    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

    MakeVisible();
  }

  UpdateCaret();
}

// MakeVisible makes sure the caret (in edit mode) or the last marked position
// (in mark mode) is visible in the view window.

void CWordDoc::MakeVisible()
{
  switch (m_eWordState)
  {
    // In edit mode, we call MakeVisible in the view class with the edit
    // position.

    case WS_EDIT:
      {
        Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
        CRect rcChar = pParagraph->CharToRect(m_psEdit.Character());
        rcChar.right = min(rcChar.right, PAGE_WIDTH);
        m_pView->MakeVisible(rcChar);
      }
      break;

    // In mark mode, we call MakeVisible with the first marked position.

    case WS_MARK:
      Paragraph* pParagraph = m_paragraphArray[m_psLastMark.Paragraph()];
      CRect rcChar = pParagraph->CharToRect(m_psLastMark.Character());
      rcChar.right = min(rcChar.right, PAGE_WIDTH);
      m_pView->MakeVisible(rcChar);
      break;
    }
}

// UpdateCaret updates the visibility and position of the caret.

void CWordDoc::UpdateCaret()
{
  switch (m_eWordState)
  {
    // In edit mode, we extract the caret block from the current paragraph.
  
    case WS_EDIT:
      {
        Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
        CRect rcCaret = pParagraph->GetCaretRect(m_psEdit.Character());

        // If the keyboard is in insert mode, the caret is set to a vertical
        // bar. It is later on set to a width of at least one pixel by
        // OnUpdate in the view class.

        if (m_eKeyboardState == KM_INSERT)
        {
          rcCaret.right = rcCaret.left + 1;
        }

        // If the is (partly or completely) located outside the page, we
        // move it to the left.

        if (rcCaret.right >= PAGE_WIDTH)
        {
          rcCaret.left -= (rcCaret.right - PAGE_WIDTH);
          rcCaret.right = PAGE_WIDTH;
        }

        m_pView->MakeVisible(rcCaret);
        m_caret.SetAndShowCaret(rcCaret);
      }
      break;

    // In mark mode, we just hide the caret.

    case WS_MARK:
      m_caret.HideCaret();
  }
}

// When a portion of the area becomes marked or unmarked, we need the areas of
// the characters in question in order to repaint them.

void CWordDoc::GetRepaintSet(RectSet& repaintSet, Position psFirst,
                                Position psLast)
{
  Position psMin = min(psFirst, psLast);
  Position psMax = max(psFirst, psLast);

  if (psMin.Paragraph() == psMax.Paragraph())
  {
    Paragraph* pParagraph = m_paragraphArray[psMin.Paragraph()];
    pParagraph->GetRepaintSet(repaintSet, psMin.Character(), psMax.Character());
  }

  else
  {
    Paragraph* pMinParagraph = m_paragraphArray[psMin.Paragraph()];
    pMinParagraph->GetRepaintSet(repaintSet, psMin.Character());

    for (int iParagraph = psMin.Paragraph() + 1;
         iParagraph < psMax.Paragraph(); ++iParagraph)
    {
      Paragraph* pParagraph = m_paragraphArray[iParagraph];
      pParagraph->GetRepaintSet(repaintSet);
    }

    Paragraph* pMaxParagraph = m_paragraphArray[psMax.Paragraph()];
    pMaxParagraph->GetRepaintSet(repaintSet, 0, psMax.Character());
  }
}
// DeleteText removes the text between the two positions. It is quite
// complicated as we have several different special cases.

void CWordDoc::DeleteText(RectSet& repaintSet, CDC* pDC, Position psFirst,
                          Position psLast)
{
  Position psMin = min(psFirst, psLast);
  Position psMax = max(psFirst, psLast);

  // If both the character positions are at the beginning of the their
  // paragraphs (the positions still have to refers to different paragraphs),
  // we simple remove the paragraphs in between.

  if ((psMin.Character() == 0) && (psMax.Character() == 0))
  {
    for (int iParagraph = psMin.Paragraph();
         iParagraph < psMin.Paragraph(); ++iParagraph)
    {
      delete m_paragraphArray[iParagraph];
    }

    m_paragraphArray.RemoveAt(psMin.Paragraph(),
                              psMax.Paragraph() - psMin.Paragraph());
  }

  else if (psMax.Character() == 0)
  {
    // If the last character position of the last paragraph is zero, and the
    // last paragraph is the next one, we remove the first up till the last
    // position in the next paragraph.

    if ((psMin.Paragraph() + 1) == psMax.Paragraph())
    {
      Paragraph* pMinParagraph = m_paragraphArray[psMin.Paragraph()];
      pMinParagraph->DeleteText(psMin.Character());

      Paragraph* pMaxParagraph = m_paragraphArray[psMax.Paragraph()];
      pMinParagraph->Append(pMaxParagraph);
      m_paragraphArray.RemoveAt(psMax.Paragraph());
      pMinParagraph->Recalculate(pDC, &repaintSet);
    }

    // If the last character position of the last paragraph is zero, and the
    // last paragraph is not the next one, we remove from the first position
    // in the first paragraph and up to the last position in the last
    // paragraph as well as the whole paragraphs in between.

    else
    {
      Paragraph* pMinParagraph = m_paragraphArray[psMin.Paragraph()];

      pMinParagraph->DeleteText(psMax.Character());
      pMinParagraph->Recalculate(pDC, &repaintSet);

      for (int iParagraph = psMin.Paragraph() + 1;
           iParagraph <= psMin.Paragraph(); ++iParagraph)
      {
        delete m_paragraphArray[iParagraph];
      }

      m_paragraphArray.RemoveAt(psMin.Paragraph() + 1,
                                psMax.Paragraph() - psMin.Paragraph() + 1);
    }

    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  }

  else
  {
    // If the marked area does not start at the beginning of the paragraph,
    // and the marked area is restricted to the same paragraph, we just delete
    // the text in that paragraph.

    if (psMin.Paragraph() == psMax.Paragraph())
    {
      Paragraph* pParagraph = m_paragraphArray[psMin.Paragraph()];

      pParagraph->DeleteText(psMin.Character(), psMax.Character());
      pParagraph->Recalculate(pDC, &repaintSet);
    }

    // If the marked area does not start at the beginning of the paragraph,
    // and the marked area is not restricted to the same paragraph, we delete 
    // the text in the first and last paragraph as well as the paragraphs in
    // between.

    else
    {
      Paragraph* pMinParagraph = m_paragraphArray[psMin.Paragraph()];
      Paragraph* pMaxParagraph = m_paragraphArray[psMax.Paragraph()];
      pMinParagraph->DeleteText(psMin.Character());

      if (psMax.Character() == pMaxParagraph->GetLength())
      {
        pMaxParagraph->DeleteText(0, psMax.Character());
      }

      else
      {
        pMaxParagraph->DeleteText(0, psMax.Character() - 1);
      }

      pMaxParagraph->ClearRectArray();

      pMinParagraph->Append(pMaxParagraph);
      pMinParagraph->Recalculate(pDC, &repaintSet);

      for (int iParagraph = psMin.Paragraph() + 1;
           iParagraph < psMin.Paragraph(); ++iParagraph)
      {
        delete m_paragraphArray[iParagraph];
      }

      m_paragraphArray.RemoveAt(psMin.Paragraph() + 1,
                                psMax.Paragraph() - psMin.Paragraph());
    }

    UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  }
}

// When a paragraph has been altered in some way, we need to recalculate and
// repaint the altered part of the paragraph. However, we need also check the
// rest of the paragraphs and repaint the ones that have been shifted on the
// page. Moreover, we need to examine the pages and update the first and last
// paragraph on each page.

void CWordDoc::UpdateParagraphAndPageArray()
{
  int iOldPages = (int) m_pageArray.GetSize();
  m_pageArray.RemoveAll();

  int iPageHeight = 0, iStartParagraph = 0;
  int iParagraphes = (int) m_paragraphArray.GetSize();

  // We traverse the paragraphs and divide them into pages of the document by
  // examine their height.

  for (int iParagraph = 0; iParagraph < iParagraphes; ++iParagraph)
  {
    Paragraph* pParagraph = m_paragraphArray[iParagraph];
    int iHeight = pParagraph->GetHeight();

    if ((iPageHeight + iHeight) <= PAGE_HEIGHT)
    {
      iPageHeight += iHeight;
    }

    // When the current height exceeds the height of the page, we start a new
    // page. If this page holds at least one paragraph, we adds them to the page. 

    else if (iStartParagraph < iParagraph)
    {
      Page page(iStartParagraph, iParagraph - 1);
      m_pageArray.Add(page);

      iStartParagraph = iParagraph;
      iPageHeight = iHeight;
    }

    // If a single paragraph is higher than the page, we include it
    // on the page and start the new page with the next paragraph.

    else
    {
      Page page(iStartParagraph, iStartParagraph);
      m_pageArray.Add(page);

      iStartParagraph = iParagraph + 1;
      iPageHeight = 0;
    }
  }

  Page page(iStartParagraph, iParagraphes - 1);
  m_pageArray.Add(page);

  // The repaint set is used to collect the parts of the documents area
  // that need to be repainted. 

  RectSet repaintSet;
  int iNewPages = (int) m_pageArray.GetSize();

  // For each page, we traverse the paragraphs and set their start position.

  for (int iPage = 0; iPage < iNewPages; ++iPage)
  {
    int iPageHeight = iPage * PAGE_HEIGHT;

    Page page = m_pageArray[iPage];
    int iFirstParagraph = page.GetFirstParagraph();
    int iLastParagraph = page.GetLastParagraph();

    for (int iParagraph = iFirstParagraph;
         iParagraph <= iLastParagraph; ++iParagraph)
    {
      Paragraph* pParagraph = m_paragraphArray[iParagraph];
      int iHeight = pParagraph->GetHeight();
      int yPos = pParagraph->GetStartPos();

      // If the previous start position of the paragraphs is being updated, we
      // set the new start position and add the paragraphs' area to the
      // repaint set.

      if (iPageHeight != yPos)
      {
        CRect rcOldParagraph(0, yPos, PAGE_WIDTH, yPos + iHeight);
        repaintSet.Add(rcOldParagraph);

        CRect rcNewParagraph(0, iPageHeight, PAGE_WIDTH,
                             iPageHeight + iHeight);
        repaintSet.Add(rcNewParagraph);

        pParagraph->SetStartPos(iPageHeight);
      }

      iPageHeight += iHeight;
    }

    // For each page, we add the rest of the page to the repaint set.

    CRect rcPageRest(0, iPageHeight, PAGE_WIDTH, (iPage + 1) * PAGE_HEIGHT);
    repaintSet.Add(rcPageRest);
  }

  // If the number of pages has decreased, we need to repaint the rest of
  // document.

  if (iNewPages < iOldPages)
  {
    CRect rcRestDocument(0, iNewPages * PAGE_HEIGHT, PAGE_WIDTH,
                         iOldPages * PAGE_HEIGHT);
    repaintSet.Add(rcRestDocument);
  }

  // If the number of pages has been changed, we need to notify OnUpdate in
  // the view class to reset the vertical scroll bars. In that case, the whole
  // document is repainted.

  if (iNewPages != iOldPages)
  {
    UpdateAllViews(NULL, (LPARAM) iNewPages);
  }

  // If the number of pages are unchanged, we only update the areas of the
  // repaint set.

  else if (!repaintSet.IsEmpty())
  {
    UpdateAllViews(NULL, 0, &repaintSet);
  }
}

void CWordDoc::SetDocumentHome()
{
  EnsureEditStatus();
  m_psEdit.Paragraph() = 0;
  m_psEdit.Character() = 0;
  MakeVisible();
  UpdateCaret();
}

void CWordDoc::SetDocumentEnd()
{
  EnsureEditStatus();
  int iSize = m_paragraphArray.GetSize();
  Paragraph* pLastParagraph = m_paragraphArray[iSize - 1];
  m_psEdit.Paragraph() = iSize - 1;
  m_psEdit.Character() = pLastParagraph->GetLength();
  MakeVisible();
  UpdateCaret();
}

// The following four update methods set a radio button on the current
// alignment menu item. They simple call IsAlignment to check the current
// alignment.

void CWordDoc::OnUpdateAlignLeft(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(IsAlignment(ALIGN_LEFT));
}

void CWordDoc::OnUpdateAlignCenter(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(IsAlignment(ALIGN_CENTER));
}

void CWordDoc::OnUpdateAlignRight(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(IsAlignment(ALIGN_RIGHT));
}

void CWordDoc::OnUpdateAlignJustifed(CCmdUI *pCmdUI)
{
  pCmdUI->SetRadio(IsAlignment(ALIGN_JUSTIFIED));
}

// In edit mode, IsAlignment checks whether the current paragraph has the
// given alignment. In mark mode, it check that all partly or completely
// marked paragraph have the given alignment. This implies that if several
// paragraph are marked and not all of them have the same alignment, no menu
// item is checked.

BOOL CWordDoc::IsAlignment(Alignment eAlignment) const
{
  switch (m_eWordState)
  {
    case WS_EDIT:
      {
        Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
        return (pParagraph->GetAlignment() == eAlignment);
      }

    case WS_MARK:
      for (int iParagraph = m_psFirstMark.Paragraph();
           iParagraph <= m_psLastMark.Paragraph(); ++iParagraph)
      {
        Paragraph* pParagraph = m_paragraphArray[iParagraph];

        if (pParagraph->GetAlignment() != eAlignment)
        {
          return FALSE;
        }
      }

      return TRUE;
  }

  return TRUE;    
}

// The following methods are called when the user choose an alignment, they
// all call SetAlignment.

void CWordDoc::OnAlignLeft()
{
  SetAlignment(ALIGN_LEFT);
}

void CWordDoc::OnAlignCenter()
{
  SetAlignment(ALIGN_CENTER);
}

void CWordDoc::OnAlignRight()
{
  SetAlignment(ALIGN_RIGHT);
}

void CWordDoc::OnAlignJustified()
{
  SetAlignment(ALIGN_JUSTIFIED);
}

// In edit mode, SetAlignment sets the given alignment to the current
// paragraph. In mark mode, it traverses through the paragraphs and sets the
// given alignment to them one by one.

void CWordDoc::SetAlignment(Alignment eAlignment)
{
  CClientDC dc(m_pView);
  m_pView->OnPrepareDC(&dc);

  switch (m_eWordState)
  {
    // In edit mode, we just set the alignment of the current paragraph.
    // Remember that this method can only be called when the paragraph has
    // another alignment due to a previous call to one of the update methods
    // above.

    case WS_EDIT:
      {
        Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];

        pParagraph->SetAlignment(eAlignment);
        pParagraph->Recalculate(&dc);

        int iHeight = pParagraph->GetHeight();
        int yPos = pParagraph->GetStartPos();

        CRect rcParagraph(0, yPos, PAGE_WIDTH, yPos + iHeight);
        RectSet repaintSet;
        repaintSet.Add(rcParagraph);
        UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

        MakeVisible();
        UpdateCaret();
      }
      break;

    // In mark mode, we traverse the marked paragraphs and set the alignment
    // on those who has not already been set to the alignment in question.
    // Remember that this method can only be called if at least one paragraph
    // is not already set to the alignment in question.

    case WS_MARK:
      RectSet repaintSet;
      for (int iParagraph = m_psFirstMark.Paragraph();
           iParagraph <= m_psLastMark.Paragraph(); ++iParagraph)
      {
        Paragraph* pParagraph = m_paragraphArray[iParagraph];

        if (pParagraph->GetAlignment() != eAlignment)
        {
          pParagraph->SetAlignment(eAlignment);
          pParagraph->Recalculate(&dc);

          int iHeight = pParagraph->GetHeight();
          int yPos = pParagraph->GetStartPos();

          CRect rcParagraph(0, yPos, PAGE_WIDTH, yPos + iHeight);
          repaintSet.Add(rcParagraph);
        }
      }

      UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
      UpdateCaret();
      break;
  }

  SetModifiedFlag();
}

// The cut menu item shall be enabled when the application in in edit mode;
// that is, when the user has marked a portion o the text. OnCut is quite
// simple, it just copies the marked area into the copy buffer, and then
// deletes it. Note that we do not have to check whether the application is in
// edit or mark mode. The cut menu item can only be chosen when it is
// enabled, and the condition is that the application is in mark mode.

void CWordDoc::OnUpdateCut(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eWordState == WS_MARK);
}

void CWordDoc::OnCut()
{
  OnCopy();

  CClientDC dc(m_pView);
  m_pView->OnPrepareDC(&dc);
  DeleteKey(&dc);
}

// OnUpdateCopy works in the same way as OnUpdateCut, it enables the copy menu
// item when the application is in mark mode. ClearCopyArray de-allocates the
// paragraphs in the copy buffer and clears the copy array.

void CWordDoc::OnUpdateCopy(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eWordState == WS_MARK);
}

void CWordDoc::ClearCopyArray()
{
  int iParagraphs = (int) m_copyArray.GetSize();

  for (int iParagraph = 0; iParagraph < iParagraphs; ++iParagraph)
  {
      delete m_copyArray[iParagraph];
  }

  m_copyArray.RemoveAll();
}

// Similar to OnCut, OnCopy is called only when the application is in mark
// mode. First, we clear the copy buffer array and determine the minimum and
// maximum of the first and last marked character. Remember that the first and
// last marked character refers to the chronological order they were set, and
// not necessarily their order in the document. Instead, psMin and psMax
// refers to their positions in the document.

// Then we have two cases to consider. With only one paragraph marked, we
// simple extract the marked text from that paragraph and add it to the copy
// buffer array. If at least two paragraphs are marked, we extract the marked
// text from the first and last one. The paragraphs in between, if any, are
// to be completely marked, so we just copies them into the copy buffer array.

void CWordDoc::OnCopy()
{
  // We clear the copy array holding the previously copied paragraphs.
  ClearCopyArray();

  // psMin and psMax are set to the first and last positions in of the marked
  // area.
  Position psMin = min(m_psFirstMark, m_psLastMark);
  Position psMax = max(m_psFirstMark, m_psLastMark);

  CClientDC dc(m_pView);
  m_pView->OnPrepareDC(&dc);

  // In only one paragraph is marked, we simple extract its marked portion.

  if (psMin.Paragraph() == psMax.Paragraph())
  {
    Paragraph* pParagraph = m_paragraphArray[psMin.Paragraph()];
    Paragraph* pCopyParagraph =
               pParagraph->ExtractText(psMin.Character(), psMax.Character());
    m_copyArray.Add(pCopyParagraph);    
  }

  // If at least two paragraphs are marked, we extract the marked portions of
  // the first and last ones. We also add the paragraphs in between, in any.

  else
  {
    Paragraph* pMinParagraph = m_paragraphArray[psMin.Paragraph()];
    Paragraph* pCopyMinParagraph = pMinParagraph->ExtractText(psMin.Character());
    m_copyArray.Add(pCopyMinParagraph);

    for (int iParagraph = psMin.Paragraph() + 1;
         iParagraph < psMax.Paragraph(); ++iParagraph)
    {
      Paragraph* pParagraph = m_paragraphArray[iParagraph];
      Paragraph* pCopyParagraph;
      check_memory(pCopyParagraph = new Paragraph(*pParagraph));
      m_copyArray.Add(pCopyParagraph);
    }

    Paragraph* pMaxParagraph = m_paragraphArray[psMax.Paragraph()];
    Paragraph* pCopyMaxParagraph =
               pMaxParagraph->ExtractText(0, psMax.Character());
    m_copyArray.Add(pCopyMaxParagraph);
  }
}

// Unlike the cut and copy menu item, the paste menu item can be called when
// the application is in edit as well as mark mode. The only condition is that
// the copy buffer array is non-empty. If the application is in mark mode, we
// first delete the marked text and set the application in edit mode.

void CWordDoc::OnUpdatePaste(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(!m_copyArray.IsEmpty());
}

// Just as when we copied the text, we have two cases to consider when we
// paste it. If the copy buffer array consists of only one paragraph, we just
// insert it and update the current carat position. Otherwise, split the
// current paragraph in two halves and insert the copied paragraphs between
// the halves.

void CWordDoc::OnPaste()
{
  CClientDC dc(m_pView);
  m_pView->OnPrepareDC(&dc);

  RectSet repaintSet;

  // If the application is in mark mode, we delete the marked text and set the
  // application in edit mode.

  if (m_eWordState == WS_MARK)
  {
    DeleteText(repaintSet, &dc, m_psFirstMark, m_psLastMark);

    m_eWordState = WS_EDIT;
    m_psEdit = min(m_psFirstMark, m_psFirstMark);
  }

  Paragraph* pEditParagraph = m_paragraphArray[m_psEdit.Paragraph()];
  int iSize = (int) m_copyArray.GetSize();

  // If the copy buffer holds only one paragraph, we insert in at the current
  // edit position.

  if (iSize == 1)
  {
    Paragraph* pCopyParagraph = m_copyArray[0];

    pEditParagraph->Insert(m_psEdit.Character(), pCopyParagraph);
    pEditParagraph->Recalculate(&dc, &repaintSet);

    m_psEdit.Character() += pCopyParagraph->GetLength();
  }

  // If the copy buffer holds more than one paragraph, we split the current
  // paragraph in two halves. The first copied paragraph is appended to the
  // first half, and the last one is inserted to the beginning of the second
  // half. We also add the copied paragraphs in between, in any, to the
  // paragraph array.

  else
  {
    Paragraph* pLastParagraph = pEditParagraph->Split(m_psEdit.Character());
    Paragraph* pCopyParagraph = m_copyArray[0];

    pEditParagraph->Append(pCopyParagraph);
    pEditParagraph->Recalculate(&dc, &repaintSet);
  
    for (int iParagraph = iSize - 2; iParagraph > 0; --iParagraph)
    {
      Paragraph* pCopyParagraph = m_copyArray[iParagraph];
      Paragraph* pInsertParagraph;
      check_memory(pInsertParagraph =
                   new Paragraph(*pCopyParagraph));

      m_paragraphArray.InsertAt(m_psEdit.Paragraph() + 1,
                                pInsertParagraph);
    }

    pCopyParagraph = m_copyArray[iSize - 1];
    Paragraph* pInsertParagraph;
    check_memory(pInsertParagraph = new Paragraph(*pCopyParagraph));

    m_psEdit.Character() = pInsertParagraph->GetLength();
    pInsertParagraph->Append(pLastParagraph);
    pInsertParagraph->Recalculate(&dc);

    delete pLastParagraph;

    m_psEdit.Paragraph() += iSize - 1;
    m_paragraphArray.InsertAt(m_psEdit.Paragraph(),
                                pInsertParagraph);
  }

  // Finally, we update the affected characters and the paragraph array. We
  // also make the current position visible and update the caret.

  UpdateAllViews(NULL, 0, (CObject*) &repaintSet);
  UpdateParagraphAndPageArray();
  
  MakeVisible();
  UpdateCaret();

  SetModifiedFlag();
}

// OnFont has no update method as the user always can change the font. We
// initialize the font dialog with a default font. In case of edit mode, we
// first check next font pointer and then current position. In mark mode, we
// choose the font of the first marked character.

void CWordDoc::OnFont()
{
  switch (m_eWordState)
  {
    case WS_EDIT:
      {
        Font defaultFont;
        // If the next font is set, we let it be the default font.

        if (m_pNextFont != NULL)
        {
          defaultFont = *m_pNextFont;
        }

        // Otherwise, we let the font of the current position be the default
        // font. Note that the font is set also for the position to the right
        // of the text.

        else
        {
          Paragraph* pParagraph = m_paragraphArray[m_psEdit.Paragraph()];
          defaultFont = pParagraph->GetFont(m_psEdit.Character());
        }

        // We initialize the font dialog with the font found above.

        LOGFONT oldLogFont = (LOGFONT) defaultFont;
        CFontDialog fontDialog(&oldLogFont);

        // If the user chooses the Ok button, we set the new font.
 
        if (fontDialog.DoModal() == IDOK)
        {
          LOGFONT newLogFont;
          fontDialog.GetCurrentFont(&newLogFont);

          delete m_pNextFont;
          Font newFont = (Font) newLogFont;
          check_memory(m_pNextFont = new Font(newFont));

          SetModifiedFlag();
        }
      }
      break;

    // In mark mode, we initialize the font dialog with the font of the first
    // marked character.

    case WS_MARK:
      Paragraph* pParagraph = m_paragraphArray[m_psFirstMark.Paragraph()];
      Font defaultFont = pParagraph->GetFont(m_psFirstMark.Character());
      LOGFONT oldLogFont = (LOGFONT) defaultFont;
      CFontDialog fontDialog(&oldLogFont);

      // If the user chooses the Ok button, we set the new font on every
      // marked character.

      if (fontDialog.DoModal() == IDOK)
      {
        LOGFONT newLogFont;
        fontDialog.GetCurrentFont(&newLogFont);
        Font newFont = (Font) newLogFont;

        Position psMin = min(m_psFirstMark, m_psLastMark);
        Position psMax = max(m_psFirstMark, m_psLastMark);

        CClientDC dc(m_pView);
        m_pView->OnPrepareDC(&dc);

        RectSet repaintSet;

        // If only one paragraph is marked, we set the new font on its marked
        // portion.

        if (psMin.Paragraph() == psMax.Paragraph())
        {
          Paragraph* pParagraph = m_paragraphArray[psMin.Paragraph()];
          pParagraph->SetFont(newFont, psMin.Character(), psMax.Character());
          pParagraph->Recalculate(&dc, &repaintSet);
        }

        // If at least two paragraphs are marked, we set the new font on the
        // marked part of the first and last paragraphs and on the whole
        // paragraphs in between, if any.

        else
        {
          Paragraph* pFirstParagraph = m_paragraphArray[psMin.Paragraph()];
          pFirstParagraph->SetFont(newFont, psMin.Character());
          pFirstParagraph->Recalculate(&dc, &repaintSet);

          for (int iParagraph = psMin.Paragraph() + 1;
               iParagraph < psMax.Paragraph() - 1; ++iParagraph)
          {
            Paragraph* pParagraph = m_paragraphArray[iParagraph];
            pParagraph->SetFont(newFont);
            pParagraph->Recalculate(&dc, &repaintSet);
          }

          Paragraph* pLastParagraph = m_paragraphArray[psMax.Paragraph()];
          pLastParagraph->SetFont(newFont, 0, psMax.Character());
          pLastParagraph->Recalculate(&dc, &repaintSet);
        }

        UpdateAllViews(NULL, 0, (CObject*) &repaintSet);

        UpdateParagraphAndPageArray();
        MakeVisible();
        UpdateCaret();

        SetModifiedFlag();
      }
      break;
  }
}
