#include "StdAfx.h"

#include "Set.h"
#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"
#include "Check.h"

#include "Figure.h"
#include "LineFigure.h"
#include "ArrowFigure.h"
#include "TwoDimensionalFigure.h"
#include "RectangleFigure.h"
#include "EllipseFigure.h"
#include "TextFigure.h"
#include "FigureFileManager.h"

#include "DrawView.h"
#include "DrawDoc.h"
#include "Draw.h"

IMPLEMENT_DYNCREATE(CDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawDoc, CDocument)
  ON_UPDATE_COMMAND_UI(ID_ADD_LINE, OnUpdateAddLine)
  ON_COMMAND(ID_ADD_LINE, OnAddLine)

  ON_UPDATE_COMMAND_UI(ID_ADD_ARROW, OnUpdateAddArrow)
  ON_COMMAND(ID_ADD_ARROW, OnAddArrow)

  ON_UPDATE_COMMAND_UI(ID_ADD_RECTANGLE, OnUpdateAddRectangle)
  ON_COMMAND(ID_ADD_RECTANGLE, OnAddRectangle)

  ON_UPDATE_COMMAND_UI(ID_ADD_ELLIPSE, OnUpdateAddEllipse)
  ON_COMMAND(ID_ADD_ELLIPSE, OnAddEllipse)

  ON_UPDATE_COMMAND_UI(ID_ADD_TEXT, OnUpdateAddText)
  ON_COMMAND(ID_ADD_TEXT, OnAddText)

  ON_UPDATE_COMMAND_UI(ID_FORMAT_MODIFY, OnUpdateModifyFigure)
  ON_COMMAND(ID_FORMAT_MODIFY, OnModifyFigure)

  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCut)
  ON_COMMAND(ID_EDIT_CUT, OnCut)

  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
  ON_COMMAND(ID_EDIT_COPY, OnCopy)

  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
  ON_COMMAND(ID_EDIT_PASTE, OnPaste)

  ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)
  ON_COMMAND(ID_EDIT_DELETE, OnDelete)

  ON_UPDATE_COMMAND_UI(ID_FORMAT_COLOR, OnUpdateColor)
  ON_COMMAND(ID_FORMAT_COLOR, OnColor)

  ON_UPDATE_COMMAND_UI(ID_FORMAT_FONT, OnUpdateFont)
  ON_COMMAND(ID_FORMAT_FONT, OnFont)

  ON_UPDATE_COMMAND_UI(ID_FORMAT_FILL, OnUpdateFill)
  ON_COMMAND(ID_FORMAT_FILL, OnFill)
END_MESSAGE_MAP()

// This class has only one constructor, it initializes the fields to
// appropriate values.  The application is in idle mode, there is now single
// figure to handle, no text to edit, and no rectangle to modify. It also
// reads the values of the state fields from the registry. The last value of
// each call is the default value, in case the value is not registered.

CDrawDoc::CDrawDoc()
 :m_eApplicationState(IDLE),
  m_pSingleFigure(NULL),
  m_pEditText(NULL),
  m_pDragRectangle(NULL)
{
  m_eNextActionState = (NextActionState) AfxGetApp()->
                       GetProfileInt(TEXT("Draw"),
                       TEXT("ActionMode"), MODIFY_FIGURE);
  m_eKeyboardState = (KeyboardState) AfxGetApp()->
                     GetProfileInt(TEXT("Draw"),
                     TEXT("KeyboardState"), KS_INSERT);
  m_nextColor = (COLORREF) AfxGetApp()->GetProfileInt
                (TEXT("Draw"), TEXT("CurrentColor"), BLACK);
  m_bNextFill = (BOOL) AfxGetApp()->GetProfileInt
                (TEXT("Draw"),TEXT("CurrentFill"), TRUE);
}

// The destructor writes the values of the state fields to the registry. It
// does also de-allocate the memory allocated for the figures in m_figureList
// and m_copyList (by calling ClearCopyList).

CDrawDoc::~CDrawDoc()
{
  AfxGetApp()->WriteProfileInt(TEXT("Draw"),
               TEXT("ActionMode"), (int) m_eNextActionState);
  AfxGetApp()->WriteProfileInt(TEXT("Draw"),
               TEXT("KeyboardState"), (int) m_eKeyboardState);
  AfxGetApp()->WriteProfileInt(TEXT("Draw"),
               TEXT("CurrentColor"), (int) m_nextColor);
  AfxGetApp()->WriteProfileInt(TEXT("Draw"),
               TEXT("CurrentFill"), (int) m_bNextFill);

  for (POSITION position = m_figurePtrList.GetHeadPosition();
       position != NULL; m_figurePtrList.GetNext(position))
  {
    Figure* pFigure = m_figurePtrList.GetAt(position);
    delete pFigure;
  }

  ClearCopyList();
}

// Serialize loads and stores the figures in m_figurePtrList. We cannot
// serialize the list itself because it contains pointers the figure objects,
// not the objects itself. Therefore, we have to traverse the list and store
// the objects one by one. In order for the function to correctly read the
// objects, we also have to store information about the class, we know that
// the class is a sub class of Figure, but we do not know which one. However,
// as Figure is a sub class of CObject in MFC, there is a function
// GetRuntimeClass, which returns an object of the class CRuntimeClass. Before
// we store the figure object, we store information about it by calling
// WriteClass with the CRuntimeClass object as an argument. 

// When we load the figures in m_figurePtrList, we first call ReadClass to
// find out which class the object belong to, then we read the object itself
// by calling ReadObject and finally add its address (a pointer to the object)
// to m_figurePtrList.

void CDrawDoc::Serialize(CArchive& archive)
{
  CDocument::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << (int) m_figurePtrList.GetSize();

    for (POSITION position =m_figurePtrList.GetHeadPosition();
         position != NULL; m_figurePtrList.GetNext(position))
    {
      Figure* pFigure = m_figurePtrList.GetAt(position);
      FigureFileManager manager(pFigure);
      manager.Serialize(archive);
      pFigure->Serialize(archive);
    }
  }

  if (archive.IsLoading())
  {
    int iSize;
    archive >> iSize;

    for (int iIndex = 0; iIndex < iSize; ++iIndex)
    {
      FigureFileManager manager;
      manager.Serialize(archive);
      Figure* pFigure = manager.GetFigure();
      pFigure->Serialize(archive);
      m_figurePtrList.AddTail(pFigure);
    }
  }
}

// MouseDown is called by OnLButtonDown in the view class when it receives
// the WM_LBUTTONDOWN message.

void CDrawDoc::MouseDown(CPoint ptMouse, BOOL bControlKeyDown, CDC* pDC)
{
  // We start by storing the mouse position, because we will need to keep
  // track of mouse movements.

  m_ptPrevMouse = ptMouse;

  // In case the application is in the state of editing a text, we finish that
  // process by simulating a return key input.

  if (m_eApplicationState == EDIT_TEXT)
  {
    KeyDown(VK_RETURN, pDC);
  }
    
  // If m_eNextState is set to add a figure, we simple add the figure in
  // question; we create the figure object and add its address to the figure
  // list. In every case except text, we set m_eApplicationState to
  // DRAG_MOUSE, it will remain in that state as long as the user is holding
  // the mouse button down (MouseUp is called when the user releases the mouse
  // button). In these cases, we also set m_eDragState to SINGLE_DRAG, as we
  // only operate on one new figure.

  switch (m_eNextActionState)
  {
    case ADD_LINE:
      check_memory(m_pSingleFigure =
                   new LineFigure(m_nextColor, ptMouse));
      m_figurePtrList.AddTail(m_pSingleFigure);
      m_eApplicationState = SINGLE_DRAG;
      SetModifiedFlag();
      break;

    case ADD_ARROW:
      check_memory(m_pSingleFigure =
                   new ArrowFigure(m_nextColor, ptMouse));
      m_figurePtrList.AddTail(m_pSingleFigure);
      m_eApplicationState = SINGLE_DRAG;
      SetModifiedFlag();
      break;

    case ADD_RECTANGLE:
      check_memory(m_pSingleFigure =
                   new RectangleFigure(m_nextColor, ptMouse, m_bNextFill));
      m_figurePtrList.AddTail(m_pSingleFigure);
      m_eApplicationState = SINGLE_DRAG;
      SetModifiedFlag();
      break;

    case ADD_ELLIPSE:
      check_memory(m_pSingleFigure =
                   new EllipseFigure(m_nextColor, ptMouse, m_bNextFill));
      m_figurePtrList.AddTail(m_pSingleFigure);
      m_eApplicationState = SINGLE_DRAG;
      SetModifiedFlag();
      break;

    // In the case of adding a text, we set m_eApplicationState to EDIT_TEXT,
    // it will remain in that state until the user type the return or escape
    // key (or click the mouse, in which case we simulate a return key, see
    // the beginning of this method). In the case of creating a text, we also
    // set and show the caret marker to a appropriate size and position by
    // calling GetCaretArea on the text object.

    case ADD_TEXT:
      {
        check_memory(m_pEditText = new TextFigure(m_nextColor,
                                   ptMouse, m_nextFont, pDC));
        m_figurePtrList.AddTail(m_pEditText);
        m_eApplicationState = EDIT_TEXT;

        CRect rcCaret = m_pEditText->GetCaretArea
                                     (m_eKeyboardState);
        m_caret.SetAndShowCaret(rcCaret);
  
        SetModifiedFlag();
      }
      break;

    // In the case of modifying a figure: if the control key is not pressed,
    // we start by unmark all figures.

    case MODIFY_FIGURE:
      if (!bControlKeyDown)
      {
        UnmarkAllFigures();
      }

      // Then we traverse the figure list and break if we find a figure hit by
      // the click. We traverse the list from start to end, which correspond
      // to examine the foremost figure first. Note that we not need to
      // know what kind of figure we test, it up to the sub classes of Figure
      // to implement the pure virtual method Click.

      Figure* pClickedFigure = NULL;
      for (POSITION position =
           m_figurePtrList.GetTailPosition();
           position != NULL;m_figurePtrList.GetPrev(position))
      {
        Figure* pFigure = m_figurePtrList.GetAt(position);

        if (pFigure->Click(ptMouse))
        {
          pClickedFigure = pFigure;
          break;
        }
      }

      // If we find a figure we have two cases to consider depending on
      // whether the control key is pressed.

      if (pClickedFigure != NULL)
      {
        CRect rcOldFigure = pClickedFigure->GetArea();

        // If the control key is pressed, we mark of unmark the figure
        // depending on its current mark status.

        if (bControlKeyDown)
        {
          // If the figure is already marked, we unmark it and set the
          // application in idle mode.

          if (pClickedFigure->IsMarked())
          {
            pClickedFigure->Mark(FALSE);
            m_eApplicationState = IDLE;
          }

          // If the figure is not already marked, we mark it and move it to
          // the end of the figure list (which makes it appear on top of the
          // other figures) and set the application in multiple drag mode,
          // which means that one or several figure are marked and ready to be
          // moved, but not modified.

          else
          {
            pClickedFigure->Mark(TRUE);

            m_figurePtrList.Remove(pClickedFigure);
            m_figurePtrList.AddTail(pClickedFigure);

            m_eApplicationState = MULTIPLE_DRAG;
            SetModifiedFlag();
          }
        }

        // If the control key is not pressed, we mark the figure (all figures
        // has been unmarked if the control key is not pressed, see the
        // beginning of this case statement) and set the field m_pSingleFigure
        // to point at it, put it last in the figure list (which makes it
        // appear on top of the other figures), and set the application to
        // single drag mode.

        else
        {
          m_pSingleFigure = pClickedFigure;
          m_pSingleFigure->Mark(TRUE);

          m_figurePtrList.Remove(m_pSingleFigure);
          m_figurePtrList.AddTail(m_pSingleFigure);

          CRect rcFigure = m_pSingleFigure->GetArea();
          UpdateAllViews(NULL, (LPARAM) &rcFigure);

          m_eApplicationState = SINGLE_DRAG;
          SetModifiedFlag();
        }

        CRect rcNewFigure = pClickedFigure->GetArea();

        UpdateAllViews(NULL, (LPARAM) &rcOldFigure);
        UpdateAllViews(NULL, (LPARAM) &rcNewFigure);                        
      }

      // If we did not find a figure, we initialize the inside rectangle and
      // and set the application in rectangle drag mode.

      else
      {
        check_memory(m_pDragRectangle = new RectangleFigure(GRAY, ptMouse, FALSE));
        m_eApplicationState = RECTANGLE_DRAG;
      }
      break;
  }
}

// MouseDrag is called when the user moves the mouse with the left button
// pressed.

void CDrawDoc::MouseDrag(const CPoint& ptMouse)
{
  // The distance since the last call to MouseDown or MouseDrag is stored in
  // szDistance.

  CSize szDistance = ptMouse - m_ptPrevMouse;
  m_ptPrevMouse = ptMouse;

  switch (m_eApplicationState)
  {
    // If the application is in single drag mode, we call MoveAndModify on the
    // single figure. Whether the figure will be moved of modified depends on
    // the setting in the previous call the Click. We do not really need to
    // know as the information is stored in the figure object.

    case SINGLE_DRAG:
      {
        CRect rcOldFigure = m_pSingleFigure->GetArea();
        m_pSingleFigure->MoveOrModify(szDistance);
        CRect rcNewFigure = m_pSingleFigure->GetArea();

        UpdateAllViews(NULL, (LPARAM) &rcOldFigure);
        UpdateAllViews(NULL, (LPARAM) &rcNewFigure);
      }
      break;

    // If the application is in multiple drag mode, we traverse the figure
    // list and for each marked figure, we call move the figure. Note that
    // we do not modify the figures in this case.

    case MULTIPLE_DRAG:
      {
        for (POSITION position = m_figurePtrList.
             GetHeadPosition(); position != NULL;
             m_figurePtrList.GetNext(position))
        {
          Figure* pFigure = m_figurePtrList.GetAt(position);

          if (pFigure->IsMarked())
          {
            CRect rcOldFigure = pFigure->GetArea();
            pFigure->Move(szDistance);
            CRect rcNewFigure = pFigure->GetArea();
          
            UpdateAllViews(NULL, (LPARAM) &rcOldFigure);
            UpdateAllViews(NULL, (LPARAM) &rcNewFigure);
         }
        }
      }
      break;

    // If the application is in rectangle drag mode, we modify the rectangle.
    // Even thought we call MoveOrModify, the rectangle will be modified
    // because it was created by the call to MouseDown above.

    case RECTANGLE_DRAG:
      {
        CRect rcOldInside = m_pDragRectangle->GetArea();
        m_pDragRectangle->MoveOrModify(szDistance);
        CRect rcNewInside = m_pDragRectangle->GetArea();

        UpdateAllViews(NULL, (LPARAM) &rcOldInside);
        UpdateAllViews(NULL, (LPARAM) &rcNewInside);
      }
      break;

    // If the application is in edit text or idle mode, we do nothing.

    case EDIT_TEXT:
    case IDLE:
        break;
  }
}

// MouseUp is called by OnLButtonUp in the view class when it receives the
// WM_LBUTTONUP message.

void CDrawDoc::MouseUp()
{
  switch (m_eApplicationState)
  {
    // If the application is in single drag or multiple drag mode we just set
    // the application in idle mode. Otherwise, we are done because the
    // movement or modification of the figures has been made by call to
    // MouseMove.

    case SINGLE_DRAG:
    case MULTIPLE_DRAG:
      m_eApplicationState = IDLE;
      break;

    // If the application is in edit text of idle mode, we do nothing.

    case EDIT_TEXT:
    case IDLE:
      break;

    // If the application is in rectangle drag mode, we have to find the
    // figures enclosed in the rectangle. We do that by traversing the figure
    // list and calling Inside on every figure. The figures found are marked,
    // put at the end of the figure list (on top of the non-marked figures),
    // the surrounding rectangle is deleted and the application is set in idle
    // mode. Note that all figures were unmarked when MouseDown was called
    // (the control was not pressed).

    case RECTANGLE_DRAG:
      CRect rcArea = m_pDragRectangle->GetArea();
      rcArea.NormalizeRect();

      POSITION position = m_figurePtrList.GetTailPosition();
      while (position != NULL)
      {
        Figure* pFigure = m_figurePtrList.GetPrev(position);

        if (pFigure->Inside(rcArea))
        {
          pFigure->Mark(TRUE);

          m_figurePtrList.Remove(pFigure);
          m_figurePtrList.AddTail(pFigure);
        }
      }

      delete m_pDragRectangle;
      m_pDragRectangle = NULL;

      UpdateAllViews(NULL, (LPARAM) &rcArea);
      m_eApplicationState = IDLE;
      break;
  }
}

// DoubleClick is called by OnLDblClick in the view class when it receives the
// WM_LDBLCLICK message.

void CDrawDoc::DoubleClick(const CPoint& ptMouse)
{
  switch (m_eNextActionState)
  {
    // This call is only interesting when then application state is in
    // modifying figure mode. Otherwise, we do nothing.

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
    case ADD_TEXT:
      break;

    // If the application is in modifying mode, we first unmark all figures,
    // then we traverse the figure list backwards trying to find a figure that
    // is hit by the mouse by calling DoubleClick on each figure.

    case MODIFY_FIGURE:
      UnmarkAllFigures();
      m_eApplicationState = IDLE;

      CRect rcOldArea;
      Figure* pClickedFigure = NULL;
      for (POSITION position = m_figurePtrList.
           GetTailPosition(); position != NULL;
           m_figurePtrList.GetPrev(position))
      {
        Figure* pFigure = m_figurePtrList.GetAt(position);
        rcOldArea = pFigure->GetArea();

        if (pFigure->DoubleClick(ptMouse))
        {
          pClickedFigure = pFigure;
          break;
        }
      }

      // If we find a figure, we have two cases depending on whether it is a
      // text or not.

      if (pClickedFigure != NULL)
      {
        // We place the figure on top of the other figures by placing it at
        // the end of the figure list.

        m_figurePtrList.Remove(pClickedFigure);
        m_figurePtrList.AddTail(pClickedFigure);

        // If it is a text, we set edit text pointer to the figure and set the
        // caret. Finally, we set the application mode to edit text.

        m_pEditText = dynamic_cast<TextFigure*>
                                  (pClickedFigure);
        if (m_pEditText != NULL)
        {
          CRect rcCaret = m_pEditText->GetCaretArea
                                       (m_eKeyboardState);
          m_caret.SetAndShowCaret(rcCaret);
          m_eApplicationState = EDIT_TEXT;
        }

        // If it is not a text, we settle with putting it on top as the
        // modification of the figure has taken place in the call to
        // DoubleClick.

        else
        {
          CRect rcNewArea = pClickedFigure->GetArea();
          UpdateAllViews(NULL, (LPARAM) &rcOldArea);
          UpdateAllViews(NULL, (LPARAM) &rcNewArea);
        }
      }
      break;
  }
}

// When the user presses and releases a key, the two messages WM_KEYDOWN and
// WM_KEYUP are sent. They are sent for every key, including Insert, Delete,
// Home, End, PageUp, PageDown, and the arrow keys. In addition to that,
// WM_CHAR is sent between them for every key with ASCII table value 32 - 122.

BOOL CDrawDoc::KeyDown(UINT uChar, CDC* pDC)
{
  // First, if the insert key is pressed, we set m_eKeyboardState to its
  // reverse value. If the application is editing a text, we update the caret
  // marker.

  if (uChar == VK_INSERT)
  {
    if (m_eKeyboardState == KS_INSERT)
    {
      m_eKeyboardState = KS_OVERWRITE;
    }
    else
    {
      m_eKeyboardState = KS_INSERT;
    }

    if (m_eApplicationState == EDIT_TEXT)
    {
      CRect rcCaret = m_pEditText->GetCaretArea(m_eKeyboardState);
      m_caret.SetAndShowCaret(rcCaret);
    }

    return TRUE;
  }

  // Otherwise, if the application is in edit text mode we examine the
  // inputted character.

  if (m_eApplicationState ==  EDIT_TEXT)
  {
    switch (uChar)
    {
      case VK_LEFT:
      case VK_RIGHT:
      case VK_HOME:
      case VK_END:
      case VK_DELETE:
      case VK_BACK:
        {
          // If there is an editing character, we edit the text and set the
          // modified flag if the KeyDown method returns true.

          CRect rcOldText = m_pEditText->GetArea();
          BOOL bModified = m_pEditText->KeyDown(uChar, pDC);

          if (bModified)
          {
              SetModifiedFlag();
          }

          // We update the text and the caret.

          CRect rcNewText = m_pEditText->GetArea();
          UpdateAllViews(NULL, (LPARAM) &rcOldText);
          UpdateAllViews(NULL, (LPARAM) &rcNewText);

          CRect rcCaret = m_pEditText->GetCaretArea(m_eKeyboardState);
          m_caret.SetAndShowCaret(rcCaret);
        }
        break;

      // In case or the return key, we finish the editing process by
      // unmarking and updating the text. Finally, we set the application to
      // idle mode.

      case VK_RETURN:
        {
          CRect rcText = m_pEditText->GetArea();
          m_pEditText->Mark(FALSE);

          UpdateAllViews(NULL, (LPARAM) &rcText);
          m_caret.HideCaret();

          m_pEditText = NULL;
          m_eApplicationState = IDLE;
        }
        break;

      // In case of the escape key, we resume the original text and simulate
      // a return key.

      case VK_ESCAPE:
        {
          CRect rcOldText = m_pEditText->GetArea();
          m_pEditText->SetPreviousText(pDC);
          CRect rcNewText = m_pEditText->GetArea();

          UpdateAllViews(NULL, (LPARAM) &rcOldText);
          UpdateAllViews(NULL, (LPARAM) &rcNewText);

          KeyDown(VK_RETURN, pDC);
        }
        break;
      }

      return TRUE;
    
  }
  
  // If the application is in modifying mode and at least one figure is
  // marked, we simulate the menu option Delete.

  int iMarked = m_figurePtrList.CountIf(IsMarked);
  if ((uChar == VK_DELETE) && (iMarked > 0))
  {
    OnDelete();
    return TRUE;
  }

  // In case of a key different from those handled so far, we just return
  // false.

  return FALSE;
}

// CharDown checks that the state of the application is in edit mode and that
// the character is printable. If it is, it adds it the text currently being
// edited and set the modified flag.

void CDrawDoc::CharDown(UINT uChar, CDC* pDC)
{
  if ((m_eApplicationState == EDIT_TEXT) && isprint(uChar))
  {
    m_pEditText->CharDown(uChar, pDC, m_eKeyboardState);
    CRect rcText = m_pEditText->GetArea();
    UpdateAllViews(NULL, (LPARAM) &rcText);

    CRect rcCaret = m_pEditText->GetCaretArea(m_eKeyboardState);
    m_caret.SetAndShowCaret(rcCaret);

    SetModifiedFlag();
  }
}

const HCURSOR CDrawDoc::GetCursor() const
{
  switch (m_eApplicationState)
  {
    // If one single figure is being dragged, we let it pick the cursor.

    case SINGLE_DRAG:
      return m_pSingleFigure->GetCursor();

    // If there are several figure being dragged, we pick the size all
    // (four arrows) cursor.

    case MULTIPLE_DRAG:
      return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

    // If the surrounding rectangle is being dragged, we pick the hair cross.

    case RECTANGLE_DRAG:
      return AfxGetApp()->LoadStandardCursor(IDC_CROSS);

    // If a text is being edited, we pick the hair vertical line.

    case EDIT_TEXT:
      return AfxGetApp()->LoadStandardCursor(IDC_IBEAM);

    // If the application is in idle mode, we pick the regular arrow cursor.

    case IDLE:
      return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
  }

  // As all possible cases have been covered above, this point of the code
  // will never be reached. The assertion is for debugging purposes only.

  check(FALSE);
  return NULL;
}

// It is possible for the user to choose one of the options if the application
// is in idle mode. The menu item is radio checked for the figure chosen.

void CDrawDoc::OnUpdateAddLine(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == ADD_LINE);
}

void CDrawDoc::OnUpdateAddArrow(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == ADD_ARROW);
}

void CDrawDoc::OnUpdateAddRectangle(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == ADD_RECTANGLE);
}

void CDrawDoc::OnUpdateAddEllipse(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == ADD_ELLIPSE);
}

void CDrawDoc::OnUpdateAddText(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == ADD_TEXT);
}

void CDrawDoc::OnUpdateModifyFigure(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eApplicationState == IDLE);
  pCmdUI->SetRadio(m_eNextActionState == MODIFY_FIGURE);
}

// When the user choose one of the add figure option, all figures are
// unmarked.

void CDrawDoc::OnAddLine()
{
  UnmarkAllFigures();
  m_eNextActionState = ADD_LINE;
}

void CDrawDoc::OnAddArrow()
{
  UnmarkAllFigures();
  m_eNextActionState = ADD_ARROW;
}

void CDrawDoc::OnAddRectangle()
{
  UnmarkAllFigures();
  m_eNextActionState = ADD_RECTANGLE;
}

void CDrawDoc::OnAddEllipse()
{
  UnmarkAllFigures();
  m_eNextActionState = ADD_ELLIPSE;
}

void CDrawDoc::OnAddText()
{
  UnmarkAllFigures();
  m_eNextActionState = ADD_TEXT;
}

void CDrawDoc::OnModifyFigure()
{
  m_eNextActionState = MODIFY_FIGURE;
}

// It is possible for the user to choose the Cut menu item if there is at
// least one marked figure and the application is in idle mode.

void CDrawDoc::OnUpdateCut(CCmdUI *pCmdUI)
{
  int iMarked = m_figurePtrList.CountIf(IsMarked);
  pCmdUI->Enable((m_eApplicationState == IDLE) &&
                 (iMarked > 0));
}

// It is possible for the user to choose the Copy menu item if there is at
// least one marked figure and the application is in idle mode.

void CDrawDoc::OnUpdateCopy(CCmdUI *pCmdUI)
{
  int iMarked = m_figurePtrList.CountIf(IsMarked);
  pCmdUI->Enable((m_eApplicationState == IDLE) && (iMarked > 0));
}

// The OnCut method just call OnCopy and OnDelete.

void CDrawDoc::OnCut()
{
  OnCopy();
  OnDelete();
}

// OnCopy copies the marked figures in the figure list to the copy list.

void CDrawDoc::OnCopy()
{
  ClearCopyList();

  for (POSITION position = m_figurePtrList.GetHeadPosition();
       position != NULL; m_figurePtrList.GetNext(position))
  {
    Figure* pFigure = m_figurePtrList.GetAt(position);

    if (pFigure->IsMarked())
    {
        Figure* pCopiedFigure = pFigure->Copy();
        m_copyPtrList.AddTail(pCopiedFigure);
    }
  }
}

// It is possible for the user to choose the Paste menu item if the copy
// list is non-empty and the application is in idle mode.

void CDrawDoc::OnUpdatePaste(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(!m_copyPtrList.IsEmpty() &&
                 (m_eApplicationState == IDLE));
}

// OnPaste add the figures in the copy list to the figure list and moves them
// ten millimeter (1000 hundreds of millimeters).

void CDrawDoc::OnPaste()
{
  CSize szDistance(1000, -1000);

  for (POSITION position = m_copyPtrList.GetHeadPosition();
       position != NULL; m_copyPtrList.GetNext(position))
  {
    Figure* pCopiedFigure = m_copyPtrList.GetAt(position);
    pCopiedFigure->Move(szDistance);

    Figure* pPastedFigure = pCopiedFigure->Copy();
    m_figurePtrList.AddTail(pPastedFigure);

    CRect rcFigure = pPastedFigure->GetArea();
    UpdateAllViews(NULL, (LPARAM) &rcFigure);
  }

  SetModifiedFlag();
}

// It is possible for the user to choose the Delete menu item if the copy list
// is non-empty and the application is in idle mode.

void CDrawDoc::OnUpdateDelete(CCmdUI *pCmdUI)
{
  int iMarked = m_figurePtrList.CountIf(IsMarked);
  pCmdUI->Enable((m_eApplicationState == IDLE) && (iMarked > 0));
}

// OnDelete traverses the figure list and delete all marked figures and set
// the modified flag if there was at least one marked figure. We need to check
// that there is at least one figure to delete as this method can be called by
// KeyDown if the user presses the delete key.

void CDrawDoc::OnDelete()
{
  FigurePointerList markedPtrList = m_figurePtrList.FilterIf(IsMarked);
  POSITION position = markedPtrList.GetHeadPosition();

  while (position != NULL)
  {
    Figure* pFigure = markedPtrList.GetNext(position);
    CRect rcArea = pFigure->GetArea();

    m_figurePtrList.Remove(pFigure);
    UpdateAllViews(NULL, (LPARAM) &rcArea);

    delete pFigure;
  }

  SetModifiedFlag(markedPtrList.GetSize() > 0);
}

// If application is in modify mode, the Color menu item is enabled if the
// the application is in idle mode and there is at least one marked figure.
// Otherwise, the menu item is enabled if the application is in idle mode.

void CDrawDoc::OnUpdateColor(CCmdUI *pCmdUI)
{
  switch (m_eNextActionState)
  {
    case MODIFY_FIGURE:
      {
        int iMarked = m_figurePtrList.CountIf(IsMarked);
        pCmdUI->Enable((m_eApplicationState == IDLE) && (iMarked > 0));
      }
      break;

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
    case ADD_TEXT:
      pCmdUI->Enable(m_eApplicationState == IDLE);
      break;
  }
}

// If the next action state is modify figure, we let the user set the color of
// all marked figures. Otherwise, we let them set the next color field.

void CDrawDoc::OnColor()
{
  switch (m_eNextActionState)
  {
    case MODIFY_FIGURE:
      {
        // We select the marked figures, and let the top figure give the
        // default color of the color dialog.

        FigurePointerList markedPtrList = m_figurePtrList.FilterIf(IsMarked);
        Figure* pLastMarkedFigure = markedPtrList.GetTail();
        Color lastColor = pLastMarkedFigure->GetColor();
        CColorDialog colorDialog((COLORREF) lastColor);

        if (colorDialog.DoModal() == IDOK)
        {
          Color newColor = (Color) colorDialog.GetColor();

          // We traverse the figure list and set the new color to all marked
          // figures not already having that color.

          for (POSITION position = m_figurePtrList.GetHeadPosition();
               position != NULL; m_figurePtrList.GetNext(position))
          {
            Figure* pFigure = m_figurePtrList.GetAt(position);

            if (pFigure->IsMarked() && (pFigure->GetColor() != newColor))
            {
              pFigure->SetColor(newColor);
              CRect rcFigure = pFigure->GetArea();
              UpdateAllViews(NULL, (LPARAM) &rcFigure);

              // If at least one figure has changed color, we set the modified
              // flag.
              SetModifiedFlag();
            }
          }
        }
      }
      break;
  }
}

void CDrawDoc::OnUpdateFont(CCmdUI *pCmdUI)
{
  switch (m_eNextActionState)
  {
    // In add text mode, the menu item is enabled if the application is in
    // idle mode.

    case ADD_TEXT:
      pCmdUI->Enable(m_eApplicationState == IDLE);
      break;

    // The menu item is enabled if at least one text is marked.

    case MODIFY_FIGURE:
      {
        int iMarkedText = m_figurePtrList.CountIf(IsMarkedText);
        pCmdUI->Enable((m_eApplicationState == IDLE) && (iMarkedText > 0));
      }
      break;

    // We cannot set the font of the other kinds of figures.

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
      pCmdUI->Enable(FALSE);
      break;
  }
}

void CDrawDoc::OnFont()
{
  switch (m_eNextActionState)
  {
    case MODIFY_FIGURE:
      {
        // We let the font and color of the last marked text give the default
        // values of the font dialog.

        FigurePointerList textPtrList =
                          m_figurePtrList.FilterIf(IsMarkedText);

        TextFigure* pLastText = (TextFigure*) textPtrList.GetTail();
        Font* pLastFont = pLastText->GetFont();
        LOGFONT logFont = (LOGFONT) (*pLastFont);
        CFontDialog fontDialog(&logFont);

        Color oldColor = pLastText->GetColor();
        fontDialog.m_cf.rgbColors = (COLORREF) oldColor;

        if (fontDialog.DoModal() == IDOK)
        {
          fontDialog.GetCurrentFont(&logFont);
          Font newFont = (Font) logFont;
          Color newColor = (Color) fontDialog.GetColor();

          // We traverse the list of marked texts and set the new font and
          // color to each figure unless it already has the those font and
          // color.

          POSITION position = GetFirstViewPosition();
          CView* pView = GetNextView(position);
          CClientDC dc(pView);
          pView->OnPrepareDC(&dc);

          for (POSITION position = textPtrList.GetHeadPosition();
               position != NULL; textPtrList.GetNext(position))
          {
            TextFigure* pText = (TextFigure*) textPtrList.GetAt(position);

            Color textColor = pText->GetColor();
            Font* pTextFont = pText->GetFont();

            if ((*pTextFont != newFont) || (textColor != newColor))
            {
              CRect rcOldText = pText->GetArea();
              pText->SetFont(newFont, &dc);
              pText->SetColor(newColor);

              CRect rcNewText = pText->GetArea();
              UpdateAllViews(NULL, (LPARAM) &rcOldText);
              UpdateAllViews(NULL, (LPARAM) &rcNewText);

              // If at least one text has got new font or color, the modified
              // flag is set.

              SetModifiedFlag();
            }
          }
        }
      }
      break;

    // Otherwise, we just set the next font and next color field.

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
    case ADD_TEXT:
      {
        LOGFONT logFont = (LOGFONT) m_nextFont;
        CFontDialog fontDialog(&logFont);

        if (fontDialog.DoModal() == IDOK)
        {
            fontDialog.GetCurrentFont(&logFont);
            m_nextFont = (Font) logFont;
            m_nextColor = (Color) fontDialog.GetColor();
        }
      }
      break;

    }
}

void CDrawDoc::OnUpdateFill(CCmdUI *pCmdUI)
{
  switch (m_eNextActionState)
  {
    // We cannot fill a one-dimensional figure.

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_TEXT:
      pCmdUI->Enable(FALSE);
      pCmdUI->SetCheck(FALSE);
      break;

    // We can, however, fill a two-dimensional figure.

    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
      pCmdUI->Enable(TRUE); 
      pCmdUI->SetCheck(m_bNextFill);
      break;

    // If the application is in modify color mode, at least one two-
    // dimensional figure must be marked.

    case MODIFY_FIGURE:
      int iFilled = m_figurePtrList.CountIf(IsMarkedAndFilled);
      int iNotFilled = m_figurePtrList.CountIf(IsMarkedAndNotFilled);      
      BOOL bAtLeastOne = ((iFilled > 0) || (iNotFilled > 0));
      pCmdUI->Enable(bAtLeastOne);

      // The radio check is set if at least half the figures are filled.

      pCmdUI->SetCheck(bAtLeastOne && (iFilled >= iNotFilled));
      break;
  }
}

void CDrawDoc::OnFill()
{
  switch (m_eNextActionState)
  {
    // In modify figure mode, we traverse the figure list and modify all two-
    // dimensional marked figures.

    case MODIFY_FIGURE:
      {
        for (POSITION position = m_figurePtrList.GetHeadPosition();
              position != NULL; m_figurePtrList.GetNext(position))
        {
          Figure* pFigure = m_figurePtrList.GetAt(position);
          TwoDimensionalFigure* pTwoDim = dynamic_cast<TwoDimensionalFigure*>(pFigure);

          if (pFigure->IsMarked() && (pTwoDim != NULL))
          {
            // Filled figures become unfilled and vice versa.

            BOOL bFilled = pTwoDim->IsFilled();
            pTwoDim->Fill(!bFilled);

            CRect rcFigure = pFigure->GetArea();
            UpdateAllViews(NULL, (LPARAM) &rcFigure);
          }
        }

        SetModifiedFlag();
      }
      break;

    // Otherwise, we switch the next fill field.

    case ADD_LINE:
    case ADD_ARROW:
    case ADD_RECTANGLE:
    case ADD_ELLIPSE:
    case ADD_TEXT:
      m_bNextFill = !m_bNextFill;
      break;

  }
}

// UnmarkAllFigures unmarks all figures (I suppose you might have guessed
// that).

void CDrawDoc::UnmarkAllFigures()
{
  for (POSITION position = m_figurePtrList.GetHeadPosition();
       position != NULL; m_figurePtrList.GetNext(position))
  {
    Figure* pFigure = m_figurePtrList.GetAt(position);

    if (pFigure->IsMarked())
    {
        CRect rcFigure = pFigure->GetArea();
        pFigure->Mark(FALSE);
        UpdateAllViews(NULL, (LPARAM) &rcFigure);
    }
  }
}

// ClearCopyList clears the copy list. It also de-allocate the copied figures
// in the list.

void CDrawDoc::ClearCopyList()
{
  for (POSITION position = m_copyPtrList.GetHeadPosition();
       position != NULL; m_copyPtrList.GetNext(position))
  {
    Figure* pFigure = m_copyPtrList.GetAt(position);
    delete pFigure;
  }

  m_copyPtrList.RemoveAll();
}

// Filter function: takes a figure and return true if it marked.

BOOL CDrawDoc::IsMarked(Figure* pFigure)
{
  return pFigure->IsMarked();
}

// Filter function: takes a figure and return true if it marked and if it is a
// text.

BOOL CDrawDoc::IsMarkedText(Figure* pFigure)
{
  return pFigure->IsMarked() && (dynamic_cast<TextFigure*>(pFigure) != NULL);
}

// Filter function: takes a figure and return true if it marked, two-
// dimensional, and filled.

BOOL CDrawDoc::IsMarkedAndFilled(Figure* pFigure)
{
  return pFigure->IsMarked() && (dynamic_cast<TwoDimensionalFigure*>(pFigure) != NULL) &&
         ((TwoDimensionalFigure*) pFigure)->IsFilled();
}

// Filter function: takes a figure and return true if it marked, two-
// dimensional and not filled.

BOOL CDrawDoc::IsMarkedAndNotFilled(Figure* pFigure)
{
  return pFigure->IsMarked() && (dynamic_cast<TwoDimensionalFigure*>(pFigure) != NULL) &&
         !((TwoDimensionalFigure*) pFigure)->IsFilled();
}
