#include "StdAfx.h"
#include <AfxTempl.h>

#include "Set.h"
#include "List.h"
#include "Color.h"
#include "Font.h"
#include "Caret.h"

#include "Reference.h"
#include "SyntaxTree.h"

#include "Cell.h"
#include "CellMatrix.h"
#include "TSetMatrix.h"

#include "CalcView.h"
#include "CalcDoc.h"

#include "Calc.h"

IMPLEMENT_DYNCREATE(CCalcDoc, CDocument)

BEGIN_MESSAGE_MAP(CCalcDoc, CDocument)
  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
  ON_COMMAND(ID_EDIT_COPY, OnCopy)

  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCut)
  ON_COMMAND(ID_EDIT_CUT, OnCut)

  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
  ON_COMMAND(ID_EDIT_PASTE, OnPaste)

  ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)
  ON_COMMAND(ID_EDIT_DELETE, OnDelete)

  ON_COMMAND(ID_COLOR_TEXT, OnTextColor)
  ON_COMMAND(ID_COLOR_BACKGROUND, OnBackgroundColor)

  ON_UPDATE_COMMAND_UI(ID_ALIGN_HORZ_LEFT, OnUpdateAlignmentHorizontalLeft)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_HORZ_CENTER, OnUpdateAlignmentHorizontalCenter)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_HORZ_RIGHT, OnUpdateAlignmentHorizontalRight)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_HORZ_JUSTIFIED,
                       OnUpdateAlignmentHorizontalJustified)

  ON_COMMAND(ID_ALIGN_HORZ_LEFT, OnAlignmentHorizontalLeft)
  ON_COMMAND(ID_ALIGN_HORZ_CENTER, OnAlignmentHorizontalCenter)
  ON_COMMAND(ID_ALIGN_HORZ_RIGHT, OnAlignmentHorizontalRight)
  ON_COMMAND(ID_ALIGN_HORZ_JUSTIFIED, OnAlignmentHorizontalJustified)

  ON_UPDATE_COMMAND_UI(ID_ALIGN_VERT_TOP, OnUpdateAlignmentVerticalTop)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_VERT_CENTER, OnUpdateAlignmentVerticalCenter)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_VERT_BOTTOM, OnUpdateAlignmentVerticalBottom)

  ON_COMMAND(ID_ALIGN_VERT_TOP, OnAlignmentVerticalTop)
  ON_COMMAND(ID_ALIGN_VERT_CENTER, OnAlignmentVerticalCenter)
  ON_COMMAND(ID_ALIGN_VERT_BOTTOM, OnAlignmentVerticalBottom)

  ON_COMMAND(ID_FORMAT_FONT, OnFont)
END_MESSAGE_MAP()

// When a new spreadsheet is created, it is in mark and insert mode, the
// upper left cell (row 0 and column 0) is marked. The cell matrix and the
// target set matrix are connected to each other.

CCalcDoc::CCalcDoc()
 :m_eCalcStatus(CS_MARK),
  m_eKeyboardState(KM_INSERT),
  m_rfMinCopy(-1, -1),
  m_rfMaxCopy(-1, -1)
{
  m_cellMatrix.SetTargetSetMatrix(&m_tSetMatrix);
  m_tSetMatrix.SetCellMatrix(&m_cellMatrix);
}

// Serialize is quite simple. As the document is created by the Application
// Framework, we have to call Serialize in the base class CDocument.

void CCalcDoc::Serialize(CArchive& archive)
{
  CDocument::Serialize(archive);

  m_cellMatrix.Serialize(archive);
  m_tSetMatrix.Serialize(archive);
}

// When the user has modified the text of a cell, the cell has to be
// updated. That is, the view must repaint the client area of the cell.

void CCalcDoc::RepaintEditArea()
{
  CPoint ptTopLeft(m_rfEdit.GetCol() * COL_WIDTH,m_rfEdit.GetRow() * ROW_HEIGHT);
  CSize szEditCell(COL_WIDTH, ROW_HEIGHT);

  CRect rcEditCell(ptTopLeft, szEditCell);
  UpdateAllViews(NULL, (LPARAM) &rcEditCell);
}

// Similar to RepaintEditArea above, we must update the client area of the
// marked cells when their mark status has changed.

void CCalcDoc::RepaintMarkedArea()
{
  int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iMaxMarkedRow = max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());

  int iMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());
  int iMaxMarkedCol = max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

  CPoint ptTopLeft(iMinMarkedCol * COL_WIDTH, iMinMarkedRow * ROW_HEIGHT);
  CPoint ptBottomRight((iMaxMarkedCol + 1) * COL_WIDTH,
                       (iMaxMarkedRow + 1) * ROW_HEIGHT);

  CRect rcMarkedBlock(ptTopLeft, ptBottomRight);
  UpdateAllViews(NULL, (LPARAM) &rcMarkedBlock);
}

// When the user modify the value of a cell, its targets need to be
// notified, re-evaluated, and updated. Event thought the set might hold
// many cells, they are not bound in a block. Therefore, we have to update
// the areas of the cells one by one.

void CCalcDoc::RepaintSet(const ReferenceSet& repaintSet)
{
  for (POSITION position = repaintSet.GetHeadPosition();
       position != NULL; repaintSet.GetNext(position))
  {
    Reference reference = repaintSet.GetAt(position);

    int iRow = reference.GetRow();
    int iCol = reference.GetCol();

    CPoint ptCell(iCol * COL_WIDTH, iRow * ROW_HEIGHT);
    CSize szCell(COL_WIDTH, ROW_HEIGHT);
    CRect rcCell(ptCell, szCell);

    UpdateAllViews(NULL, (LPARAM) &rcCell);
  }
}

// DoubleClick is called by the view class when the user double clicks with
// the left mouse button. We start by setting the application in edit mode,
// and generate the input text of the cell in question. We also determine the
// index of the current character by subtracting the mouse position from
// the upper left corner of the cell. Finally, we generate the caret array
// of the cell and update the caret.

void CCalcDoc::DoubleClick(Reference rfCell, CPoint ptMouse, CDC* pDC)
{
  UnmarkAndMark(rfCell.GetRow(), rfCell.GetCol(), rfCell.GetRow(), rfCell.GetCol());

  m_eCalcStatus = CS_EDIT;
  m_rfEdit = rfCell;

  Cell* pEditCell = m_cellMatrix.Get(m_rfEdit.GetRow(), m_rfEdit.GetCol());
  pEditCell->GenerateInputText();

  CPoint ptTopLeft(m_rfEdit.GetCol() * COL_WIDTH,m_rfEdit.GetRow() * ROW_HEIGHT);
  m_iInputIndex = pEditCell->MouseToIndex(ptMouse - ptTopLeft);

  pEditCell->GenerateCaretArray(pDC);
  RepaintEditArea();
  UpdateCaret();
}

// When the user starts to edit a cell, the cell might be outside the
// visible part of the view of the spreadsheet due to scrolling or resizing
// of the window. MakeCellVisible takes care of that by notifying the
// current view about the cell's area.

void CCalcDoc::MakeCellVisible(Reference rfCell)
{
    MakeCellVisible(rfCell.GetRow(), rfCell.GetCol());
}

void CCalcDoc::MakeCellVisible(int iRow, int iCol)
{
  CPoint ptTopLeft(iCol * COL_WIDTH, iRow * ROW_HEIGHT);
  CRect rcCell(ptTopLeft, CSize(COL_WIDTH, ROW_HEIGHT));

  CCalcView* pCalcView = (CCalcView*) m_caret.GetView();
  pCalcView->MakeCellVisible(rcCell);
}

// When the application is in edit mode and the edited cell is visible in
// the view, the caret shall be visible too. If the keyboard is in
// overwrite mode, the caret is given the size of the current character, if
// it is in insert mode, the caret is a vertical line.

void CCalcDoc::UpdateCaret()
{
  switch (m_eCalcStatus)
  {
    // The caret marker is never visible when the application is in mark
    // mode.

    case CS_MARK:
      m_caret.HideCaret();
      break;

    // In edit mode, the caret is visible if the cell currently being
    // edited is visible in the view currently holding the input focus.

    // If it is visible, we need the rectangle of the caret relative its
    // top left corner.

    case CS_EDIT:
      CCalcView* pCalcView = (CCalcView*) m_caret.GetView();

      if (pCalcView->IsCellVisible(m_rfEdit.GetRow(), m_rfEdit.GetCol()))
      {
        Cell* pEditCell = m_cellMatrix.Get(m_rfEdit);
        CPoint ptTopLeft(m_rfEdit.GetCol() * COL_WIDTH,
                         m_rfEdit.GetRow() * ROW_HEIGHT);
        CRect rcCaret = ptTopLeft +
                        pEditCell->IndexToCaret(m_iInputIndex);

        // If the keyboard is in insert mode, we cut trim the caret to a
        // vertical line.

        if (m_eKeyboardState == KM_INSERT)
        {
          rcCaret.right = rcCaret.left + 1;
        }

        // We need to transform the coordinates of the caret to sheet point
        // coordinates in case the view has been scrolled. Finally, we show
        // the caret.

        pCalcView->SheetPointToLogicalPoint(rcCaret);
        m_caret.SetAndShowCaret(rcCaret);
      }

      // If the current cell is not visible in the view, we hide the caret.

      else
      {
        m_caret.HideCaret();
      }
      break;
  }
}

// UnmarkAndMark is a central and rather complex method. Its purpose is to
// unmark the marked cells and mark the new block given by the parameters
// without any unnecessary updating; that is, new cells already marked shall
// not be updated. Note that the first and last marked cells refers to when
// then were marked rather than their positions in the spreadsheet. The last
// row or column may be less than the first one. Therefore, we need to found
// the minimum and maximum value in order to loop through the block.

void CCalcDoc::UnmarkAndMark(int iNewFirstMarkedRow,int iNewFirstMarkedCol,
                             int iNewLastMarkedRow, int iNewLastMarkedCol)
{
  // Notice that the first and last rows and columns refer to the time the
  // user marked them, not their location in the spreadsheet. The first row or
  // column may be greater than the last one. So, we to find the minimum and
  // maximum of the current rows and column.

  int iOldMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iOldMaxMarkedRow = max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iOldMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());
  int iOldMaxMarkedCol = max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

  int iNewMinMarkedRow = min(iNewFirstMarkedRow, iNewLastMarkedRow);
  int iNewMaxMarkedRow = max(iNewFirstMarkedRow, iNewLastMarkedRow);
  int iNewMinMarkedCol = min(iNewFirstMarkedCol, iNewLastMarkedCol);
  int iNewMaxMarkedCol = max(iNewFirstMarkedCol, iNewLastMarkedCol);

  m_rfFirstMark.SetRow(iNewFirstMarkedRow);
  m_rfLastMark.SetRow(iNewLastMarkedRow);
  m_rfFirstMark.SetCol(iNewFirstMarkedCol);
  m_rfLastMark.SetCol(iNewLastMarkedCol);

  switch (m_eCalcStatus)
  {
    // In case of edit mode, we need to finish the editing and evaluate the
    // value of the cell.

    case CS_EDIT:
      {
        Cell* pCell = m_cellMatrix.Get(m_rfEdit);
        m_eCalcStatus = CS_MARK;

        try
        {
          pCell->EndEdit(m_rfEdit);
          pCell->EvaluateValue(FALSE);

          // We need to evaluate and update all targets of this cell.

          ReferenceSet repaintSet =
                       m_tSetMatrix.EvaluateTargets(m_rfEdit);
          RepaintSet(repaintSet);

          SetModifiedFlag();
        }

        // In case of a parse error, we display the message in a message
        // box.

        catch (const CString stMessage)
        {
          AfxGetApp()->GetMainWnd()->MessageBox(stMessage, TEXT("Parse Error."));
          RepaintEditArea();
        }

        UpdateCaret();
      }
      break;

    // In case of mark mode, we need to unmark the cells not included in
    // the new marked cell block.

    case CS_MARK:
      for (int iRow = iOldMinMarkedRow; iRow <= iOldMaxMarkedRow; ++iRow)
      {
        for (int iCol = iOldMinMarkedCol; iCol <= iOldMaxMarkedCol; ++iCol)
        {
          if ((iRow < iNewMinMarkedRow) || (iRow > iNewMaxMarkedRow) ||
              (iCol < iNewMinMarkedCol) || (iCol > iNewMaxMarkedCol))
          {
            CPoint ptTopLeft(iCol * COL_WIDTH, iRow * ROW_HEIGHT);
            CRect rcCell(ptTopLeft, CSize(COL_WIDTH, ROW_HEIGHT));
            UpdateAllViews(NULL, (LPARAM) &rcCell);
          }
        }
      }
      break;
  }

  // Finally, we traverse the new marked cell block and update all cells not
  // already marked in the previous marked cell block.

  for (int iRow = iNewMinMarkedRow; iRow <= iNewMaxMarkedRow; ++iRow)
  {
    for (int iCol = iNewMinMarkedCol; iCol <= iNewMaxMarkedCol; ++iCol)
    {
      if ((iRow < iOldMinMarkedRow) || (iRow > iOldMaxMarkedRow) ||
          (iCol < iOldMinMarkedCol) || (iCol > iOldMaxMarkedCol))
      {
        CPoint ptTopLeft(iCol * COL_WIDTH, iRow * ROW_HEIGHT);
        CRect rcCell(ptTopLeft,  CSize(COL_WIDTH, ROW_HEIGHT));
        UpdateAllViews(NULL, (LPARAM) &rcCell);
      }
    }
  }
}

// KeyDown is called when the user presses a special character, regular
// characters are handled by CharDown below.

void CCalcDoc::KeyDown(UINT uChar, CDC* pDC, BOOL bShiftKeyDown)
{
  switch (uChar)
  {
    case VK_LEFT:
        LeftArrowKey(bShiftKeyDown);
        break;

    case VK_RIGHT:
        RightArrowKey(bShiftKeyDown);
        break;

    case VK_UP:
        UpArrowKey(bShiftKeyDown);
        break;

    case VK_DOWN:
        DownArrowKey(bShiftKeyDown);
        break;

    case VK_HOME:
        HomeKey(bShiftKeyDown);
        break;

    case VK_END:
        EndKey(bShiftKeyDown);
        break;

    // InsertKey simply changes the mode of the keyboard.

    case VK_INSERT:
        m_eKeyboardState = (m_eKeyboardState == KM_INSERT) ?
                           KM_OVERWRITE : KM_INSERT;
        break;

    // The return key finish the editing session. The user can also finish
    // by pressing the tabulator key or pressing the mouse; in either case,
    // MarkAndUnmark above takes care of finishing the editing process.

    case VK_RETURN:
        {
          // When the editing is finished, we try to mark the cell to the
          // right.

          int iNewFirstMarkedRow = min(m_rfFirstMark.GetRow() + 1, ROWS - 1);
          UnmarkAndMark(iNewFirstMarkedRow, m_rfFirstMark.GetCol(),
                        iNewFirstMarkedRow, m_rfFirstMark.GetCol());
          MakeCellVisible(iNewFirstMarkedRow, m_rfFirstMark.GetCol());
        }
        break;

    // The tabulator key does almost the same thing as the return key. The
    // difference is that the next marked cell is, if possible, the cell to
    // right, or the cell to the left if the user pressed the shift key.

    case VK_TAB:
        // If the user has pressed the shift key, we try to mark the cell to
        // the right.

        if (bShiftKeyDown)
        {
          int iNewFirstMarkedCol = max(0, m_rfFirstMark.GetCol() - 1);
          UnmarkAndMark(m_rfFirstMark.GetRow(), iNewFirstMarkedCol,
                        m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
          MakeCellVisible(m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
        }

        // If the user has not pressed the shift key, we try to mark the t
        // the left.

        else
        {
          int iNewFirstMarkedCol = min(m_rfFirstMark.GetCol() + 1, COLS - 1);
          UnmarkAndMark(m_rfFirstMark.GetRow(), iNewFirstMarkedCol,
                        m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
          MakeCellVisible(m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
        }

        break;

    case VK_ESCAPE:
        switch (m_eCalcStatus)
        {
          // When the user presses the escape key in edit mode, the editing
          // is aborted and no interpretation of the text is made.

          case CS_EDIT:
            m_eCalcStatus = CS_MARK;
            m_rfFirstMark = m_rfEdit;
            m_rfLastMark = m_rfEdit;

            RepaintMarkedArea();
            MakeCellVisible(m_rfFirstMark);
            break;

          // When the user presses the escape key in mark mode, only the first
          // marked cell shall remain marked.

          case CS_MARK:
            UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                          m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol());
            break;
        }
        break;

    case VK_DELETE:
        DeleteKey(pDC);
        break;

    case VK_BACK:
        BackspaceKey(pDC);
        break;
  }

  UpdateCaret();
}

// CharDown is called when the user presses a regular key (ascii number
// between 32 and 122).

void CCalcDoc::CharDown(UINT uChar, CDC* pDC)
{
  // If the application is in mark mode, we mark the first marked cell, change
  // to edit mode, and clear the input text before adding the character.

  if (m_eCalcStatus == CS_MARK)
  {
      UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                    m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol());

      m_eCalcStatus = CS_EDIT;
      m_rfEdit = m_rfFirstMark;
      m_iInputIndex = 0;

      Cell* pCell = m_cellMatrix.Get(m_rfEdit);
      pCell->SetInputText(TEXT(""));
  }

  // We make sure the edited cell is visible.
  MakeCellVisible(m_rfEdit);

  // We add the character and generate a new caret array.
  Cell* pCell = m_cellMatrix.Get(m_rfEdit);
  pCell->CharDown(uChar, m_iInputIndex++, m_eKeyboardState);
  pCell->GenerateCaretArray(pDC);

  // Finally, we update the edit area (the cell being edited) and the caret.
  RepaintEditArea();
  UpdateCaret();
}

// LeftArrowKey is called when the user presses the left arrow key. We have
// three different cases to consider, depending on whether the application
// is in edit or mark mode and on whether the user pressed the shift key.

void CCalcDoc::LeftArrowKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // If the application is in edit mode, we make the current cell
    // visible, move the current index one step to the left if it is not
    // already at the leftmost index, and update the caret.

    case CS_EDIT:
      MakeCellVisible(m_rfEdit);
      m_iInputIndex = max(0, m_iInputIndex - 1);
      UpdateCaret();
      break;

    // If the application is in mark mode, we have to take into
    // consideration whether the shift key was pressed at the same time.
    // If it was not, we place the marked block one step to the left of the
    // first marked cell if it is not already at the leftmost column. In
    // that case, the place the marked block at the first marked cell.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        int iNewFirstMarkedCol = max(0, m_rfFirstMark.GetCol() - 1);
        MakeCellVisible(m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
        UnmarkAndMark(m_rfFirstMark.GetRow(), iNewFirstMarkedCol, 
                      m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
      }

      // If the shift key was pressed, we move the last marked cell one step
      // to the left unless it is already at the leftmost position. The first
      // marked cell is not affected.

      else
      {
        int iNewLastMarkedCol = max(0, m_rfLastMark.GetCol() - 1);
        MakeCellVisible(m_rfLastMark.GetRow(), iNewLastMarkedCol);
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      m_rfLastMark.GetRow(), iNewLastMarkedCol);
      }
      break;
  }
}
// RightArrowKey is called when the user presses the left arrow key. It
// works similar to LeftArrowKey above.

void CCalcDoc::RightArrowKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // If the application is in edit mode, we make the current cell
    // visible, move the current index one step to the right if it is not
    // already at the rightmost index, and update the caret.

    case CS_EDIT:
      {
        MakeCellVisible(m_rfEdit);
        Cell* pCell = m_cellMatrix.Get(m_rfEdit.GetRow(), m_rfEdit.GetCol());
        m_iInputIndex = min(m_iInputIndex + 1,
                            pCell->GetInputText().GetLength());
        UpdateCaret();
      }
      break;

    // If the application is in mark mode, we have to take into
    // consideration whether the shift key was pressed at the same time.
    // If it was not, we place the marked block one step to the left of the
    // first marked cell if it is not already at the rightmost column. In
    // that case, the place the marked block at the first marked cell.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        int iNewFirstMarkedCol = min(m_rfFirstMark.GetCol() + 1, COLS - 1);
        MakeCellVisible(m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
        UnmarkAndMark(m_rfFirstMark.GetRow(), iNewFirstMarkedCol, 
                      m_rfFirstMark.GetRow(), iNewFirstMarkedCol);
      }

      // If the shift key was pressed, we move the last marked cell one step
      // to the right unless it is already at the rightmost position. The
      // first marked cell is not affected.

      else
      {
        int iNewLastMarkedCol = min(m_rfLastMark.GetCol() + 1, COLS - 1);
        MakeCellVisible(m_rfLastMark.GetRow(), iNewLastMarkedCol);
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      m_rfLastMark.GetRow(), iNewLastMarkedCol);
      }
      break;
  }
}

// UpArrowKey is called when the user presses the up arrow key.

void CCalcDoc::UpArrowKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // In edit mode, nothing happens.

    case CS_EDIT:
      break;

    // If the application is in mark mode, we have to take into
    // consideration whether the shift key was pressed at the same time.
    // If it was not, we place the marked block one step up of the first
    // marked cell if it is not already at the top row. In that case, we
    // place the marked block at the first marked cell.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        int iNewFirstMarkedRow = max(0, m_rfFirstMark.GetRow() - 1);
        UnmarkAndMark(iNewFirstMarkedRow, m_rfFirstMark.GetCol(),
                      iNewFirstMarkedRow, m_rfFirstMark.GetCol());
        MakeCellVisible(iNewFirstMarkedRow, m_rfFirstMark.GetCol());
      }

      // If the shift key was pressed, we move the last marked cell one step
      // up unless it is already at the top row. The first marked cell is not
      // affected.

      else
      {
        int iNewLastMarkedRow = max(0, m_rfLastMark.GetRow() - 1);
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      iNewLastMarkedRow, m_rfLastMark.GetCol());
        MakeCellVisible(iNewLastMarkedRow, m_rfLastMark.GetCol());
      }
      break;
  }
}

// UpArrowKey is called when the user presses the up arrow key. It works in
// a way similar to UpArrowKey above.

void CCalcDoc::DownArrowKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // In edit mode, nothing happens.

    case CS_EDIT:
      break;

    // If the application is in mark mode, we have to take into
    // consideration whether the shift key was pressed at the same time.
    // If it was not, we place the marked block one step up of the first
    // marked cell if it is not already at the bottom row. In that case, we
    // place the marked block at the first marked cell.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        int iNewFirstMarkedRow = min(m_rfFirstMark.GetRow() + 1, ROWS - 1);
        UnmarkAndMark(iNewFirstMarkedRow, m_rfFirstMark.GetCol(),
                      iNewFirstMarkedRow, m_rfFirstMark.GetCol());
        MakeCellVisible(iNewFirstMarkedRow, m_rfFirstMark.GetCol());
      }

      // If the shift key was pressed, we move the last marked cell one step
      // down unless it is already at the bottom row. The first marked cell is
      // not affected.

      else
      {
        int iNewLastMarkedRow = min(m_rfLastMark.GetRow() + 1, ROWS - 1);
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      iNewLastMarkedRow, m_rfLastMark.GetCol());
        MakeCellVisible(iNewLastMarkedRow, m_rfLastMark.GetCol());
      }
      break;
  }
}

// HomeKey is called when the user presses the home key.

void CCalcDoc::HomeKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // If the application is in edit mode, we make the current cell
    // visible, move the current index to the leftmost index, and update
    // the caret.

    case CS_EDIT:
      m_iInputIndex = 0;
      MakeCellVisible(m_rfEdit);
      UpdateCaret();
      break;

    // If mark mode, we have to take into consideration whether the shift
    // key was pressed at the same time. If it was not, we move the marked
    // block to the leftmost column of the first marked row.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        UnmarkAndMark(m_rfFirstMark.GetRow(), 0, m_rfFirstMark.GetRow(), 0);
        MakeCellVisible(m_rfFirstMark);
      }

      // If the shift key was pressed, we move the last marked cell to the
      // leftmost column of the lasted marked row. The first marked cell is
      // not affected.

      else
      {
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      m_rfFirstMark.GetRow(), 0);
        MakeCellVisible(m_rfLastMark);
      }
      break;
  }
}

void CCalcDoc::EndKey(BOOL bShiftKeyDown)
{
  switch (m_eCalcStatus)
  {
    // If the application is in edit mode, we make the current cell
    // visible, move the current index to the rightmost index, and update
    // the caret.

    case CS_EDIT:
      {
        Cell* pCell = m_cellMatrix.Get(m_rfEdit);
        m_iInputIndex = pCell->GetInputText().GetLength();
        MakeCellVisible(m_rfEdit);
      }
      break;

    // If mark mode, we have to take into consideration whether the shift
    // key was pressed at the same time. If it was not, we move the marked
    // block to the rightmost column of the lasted marked row.

    case CS_MARK:
      if (!bShiftKeyDown)
      {
        UnmarkAndMark(m_rfFirstMark.GetRow(), COLS - 1,
                      m_rfFirstMark.GetRow(), COLS - 1);
        MakeCellVisible(m_rfFirstMark);
      }

      // If the shift key was pressed, we move the last marked cell to the
      // rightmost column of the lasted marked row. The first marked cell is
      // not affected.

      else
      {
        UnmarkAndMark(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol(),
                      m_rfFirstMark.GetRow(), COLS - 1);
        MakeCellVisible(m_rfLastMark);
      }
      break;
  }
}

// DeleteKey is called when the user presses the delete key to delete a
// character in edit mode or, in mark mode, the contents of a block of one or
// more cells in the marked block.

void CCalcDoc::DeleteKey(CDC* pDC)
{
  switch (m_eCalcStatus)
  {
    // In edit mode, we delete the character on the edit index unless it is
    // at the end of the text.

    case CS_EDIT:
      {
        Cell* pCell = m_cellMatrix.Get(m_rfEdit);
        CString stInput = pCell->GetInputText();

        if (m_iInputIndex < stInput.GetLength())
        {
          stInput.Delete(m_iInputIndex);
          pCell->SetInputText(stInput);
          pCell->GenerateCaretArray(pDC);
          RepaintEditArea();
          SetModifiedFlag();
        }
      }
      break;

    // In mark mode, we just call OnDelete to remove the marked cells.

    case CS_MARK:
      OnDelete();
      break;
  }
}

// BackspaceKey is called when the user presses the backspace key to delete a
// character in a cell in edit mode or, in mark mode, the contents of a block
// of one or more cells. It is quite simple, we just call DeleteKey above.

void CCalcDoc::BackspaceKey(CDC* pDC)
{
  switch (m_eCalcStatus)
  {
    // In edit mode, we decrement the edit index and remove the character
    // by calling DeleteKey at the new index unless it is already at the
    // beginning of text.

    case CS_EDIT:
      if (m_iInputIndex > 0)
      {
        --m_iInputIndex;
        DeleteKey(pDC);
      }
      break;

    // In mark mode, we just call OnDelete to remove the marked cells.

    case CS_MARK:
      OnDelete();
      break;
  }
}

// The copy menu item, toolbar button, and accelerator are enabled when the
// application is in mark mode, and disabled in edit mode.

void CCalcDoc::OnUpdateCopy(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

// OnCopy is called when the user choose the Copy menu item, or Copy button
// on the toolbar.
 
void CCalcDoc::OnCopy()
{
  m_rfMinCopy.SetRow(min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow()));
  m_rfMinCopy.SetCol(min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol()));

  m_rfMaxCopy.SetRow(max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow()));
  m_rfMaxCopy.SetCol(max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol()));

  for (int iRow = m_rfMinCopy.GetRow(); iRow <= m_rfMaxCopy.GetRow(); ++iRow)
  {
    for (int iCol = m_rfMinCopy.GetCol(); iCol <= m_rfMaxCopy.GetCol();++iCol)
    {
      *m_copyMatrix.Get(iRow, iCol) = *m_cellMatrix.Get(iRow, iCol);
    }
  }
}

// The Cut menu item, toolbar button, and accelerator are enabled when the
// application is in mark mode, similar to OnUpdateCopy above. OnCut simply
// calls OnCopy and OnDelete.

void CCalcDoc::OnUpdateCut(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

void CCalcDoc::OnCut()
{
  OnCopy();
  OnDelete();
}

// The Paste menu item, toolbar button, and accelerator are disabled when
// the application is in edit mode. In mark mode, it is enabled if there is
// a block of cells to start with (m_rfMinCopy.GetRow() != -1) and if
// exactly one cell is marked or if a block of the same size at the copied
// block is marked.

void CCalcDoc::OnUpdatePaste(CCmdUI *pCmdUI)
{
  switch (m_eCalcStatus)
  {
    case CS_EDIT:
      pCmdUI->Enable(FALSE);
      break;

    case CS_MARK:
      if (m_rfMinCopy.GetRow() != -1)
      {
        int iCopiedRows = abs(m_rfMaxCopy.GetRow() - m_rfMinCopy.GetRow()) +1;
        int iCopiedCols = abs(m_rfMaxCopy.GetCol() - m_rfMinCopy.GetCol()) +1;

        if ((m_rfFirstMark.GetRow() == m_rfLastMark.GetRow()) &&
            (m_rfFirstMark.GetCol() == m_rfLastMark.GetCol()))
        {
          int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
          int iMinMarkedCol = min(m_rfFirstMark.GetCol(),m_rfLastMark.GetCol());

          pCmdUI->Enable(((iMinMarkedRow + iCopiedRows) <= ROWS) &&
                         ((iMinMarkedCol + iCopiedCols) <= COLS));
        }

        else
        {
          int iMarkedRows = abs(m_rfLastMark.GetRow()-m_rfFirstMark.GetRow()) +1;
          int iMarkedCols = abs(m_rfLastMark.GetCol()-m_rfFirstMark.GetCol())+1;

          pCmdUI->Enable((iMarkedRows == iCopiedRows) &&
                         (iMarkedCols == iCopiedCols));
        }
      }

      else
      {
        pCmdUI->Enable(FALSE);
      }
      break;
  }
}

// When we paste a cell block into the spreadsheet, we have to check that
// it does not introduce a cycle into the cell matrix. Then we paste and
// parse the cells one by one. We start by defining a test cell matrix and a
// test target set matrix, which are copies of the document field m_cellMatrix
// and m_tSetMatrix.

// Then we paste the cells one by one. Before we paste a cell, we have to
// remove it as a target for each of its sources. For each pasted cell, we
// adjust its references, check for cycles, and evaluates its value
// recursively; that is, each time we find a reference in a formula, we
// evaluate that reference and if it is a formula itself, its references
// are evaluated, and so on. As we do not have any cyclic references, the
// recursive evaluation have to terminate. This is necessary in order for the
// cells in the pasted block to receive their correct values. Otherwise, we
// cannot be sure that the value of a reference is the correct one or if it is
// the one of the previous cell, before the paste.

// In case of any problem, an exception is thrown, a message box reports the
// error to the user, and the method returns. The cell and target set
// matrices are only updated at the end of the method if every cell has been
// pasted without any problems.

void CCalcDoc::OnPaste()
{
  // We need to find the difference between the copy and paste location of the
  // block in order to update the references of the block.

  int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

  int iRowDiff = iMinMarkedRow - m_rfMinCopy.GetRow();
  int iColDiff = iMinMarkedCol - m_rfMinCopy.GetCol();

  // We introduce test matrices to protect the original ones in case of
  // cyclic references.

  TSetMatrix testTSetMatrix(m_tSetMatrix);
  CellMatrix testCellMatrix(m_cellMatrix);

  testTSetMatrix.SetCellMatrix(&testCellMatrix);
  testCellMatrix.SetTargetSetMatrix(&testTSetMatrix);

  ReferenceSet totalRepaintSet;
  BOOL bModified = FALSE;
  for (int iSourceRow = m_rfMinCopy.GetRow(); iSourceRow <= m_rfMaxCopy.GetRow();
       ++iSourceRow)
  {
    for (int iSourceCol = m_rfMinCopy.GetCol();
         iSourceCol <= m_rfMaxCopy.GetCol();++iSourceCol)
    {
      int iTargetRow = iSourceRow + iRowDiff;
      int iTargetCol = iSourceCol + iColDiff;

      Reference mark(iTargetRow, iTargetCol);
      testTSetMatrix.RemoveTargets(mark);

      Cell* pSourceCell = m_copyMatrix.Get(iSourceRow, iSourceCol);
      Cell* pTargetCell = testCellMatrix.Get(iTargetRow, iTargetCol);

      // We paste the cell.
      *pTargetCell = *pSourceCell;
    
      if (!pSourceCell->IsEmpty() && !pTargetCell->IsEmpty())
      {
        bModified = TRUE;
      }

      // We update the references of the cell's formula, if it has one. Then
      // we check for cyclic references. If it goes well, we add the cell as
      // a target for each cell in its source set by calling AddTargets and
      // add its area to the total set of cell client areas to be updated.

      try
      {
        pTargetCell->UpdateSyntaxTree(iRowDiff, iColDiff);
        testTSetMatrix.CheckCircular(mark, pTargetCell->GetSourceSet());
        testTSetMatrix.AddTargets(mark);

        pTargetCell->EvaluateValue(TRUE);
        ReferenceSet repaintSet =  testTSetMatrix.EvaluateTargets(mark);
        totalRepaintSet.AddAll(repaintSet);
      }

      // If we find a cyclic reference, an exception is thrown. We report the
      // the error and return the method. Note that as we have been working on
      // copies of the original cell and target set matrix, nothing has
      // actually been pasted.

      catch (const CString stMessage)
      {
        AfxGetApp()->GetMainWnd()->MessageBox(stMessage, TEXT("Parse Error."));
        return;
      }
   }
  }

  // If everything worked and at least one cell has been changed, we set the
  // modified flag. Note that we could not set the flag immediate as we did
  // not know if the block really was to be pasted, of if a circular reference
  // was to be found, in case nothing had been modified.

  if (bModified)
  {
    SetModifiedFlag();
  }

  // If we make this far without finding any cyclic references, we replace the
  // original cell and target set matrices and update the client areas of the
  // pasted cells.

  m_cellMatrix = testCellMatrix;
  m_tSetMatrix = testTSetMatrix;
  RepaintSet(totalRepaintSet);
}

// The delete menu item and accelerator is enabled when the application is
// in mark mode.

void CCalcDoc::OnUpdateDelete(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

// OnDelete traverses the marked block and clears the contents of all non-
// empty cells.

void CCalcDoc::OnDelete()
{
  int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iMaxMarkedRow = max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
  int iMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());
  int iMaxMarkedCol = max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

  ReferenceSet totalRepaintSet;

  for (int iRow = iMinMarkedRow; iRow <= iMaxMarkedRow; ++iRow)
  {
    for (int iCol = iMinMarkedCol; iCol <= iMaxMarkedCol; ++iCol)
    {
      Cell* pCell = m_cellMatrix.Get(iRow, iCol);

      // If the cell is non-empty, we clears it and set the modified flag.

      if (!pCell->IsEmpty())
      {
        Reference mark(iRow, iCol);
        pCell->Clear(mark);

        // We evaluate the targets of this cell and saves the client areas of
        // the affected cells.

        ReferenceSet repaintSet = m_tSetMatrix.EvaluateTargets(mark);
        totalRepaintSet.AddAll(repaintSet);

        SetModifiedFlag();
      }
    }
  }

  //Finally, we update the client areas of the removed cells.
  RepaintSet(totalRepaintSet);
}

// The update alignment methods are called during the process idle time. They
// simple call UpdateAlignment below.

void CCalcDoc::OnUpdateAlignmentHorizontalLeft(CCmdUI *pCmdUI)
{
  UpdateAlignment(HORIZONTAL, HALIGN_LEFT, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentHorizontalCenter(CCmdUI *pCmdUI)
{
  UpdateAlignment(HORIZONTAL, HALIGN_CENTER, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentHorizontalRight(CCmdUI *pCmdUI)
{
  UpdateAlignment(HORIZONTAL, HALIGN_RIGHT, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentHorizontalJustified(CCmdUI *pCmdUI)
{
  UpdateAlignment(HORIZONTAL, HALIGN_JUSTIFIED, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentVerticalTop(CCmdUI *pCmdUI)
{
  UpdateAlignment(VERTICAL, VALIGN_TOP, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentVerticalCenter(CCmdUI *pCmdUI)
{
  UpdateAlignment(VERTICAL, VALIGN_CENTER, pCmdUI);
}

void CCalcDoc::OnUpdateAlignmentVerticalBottom(CCmdUI *pCmdUI)
{
  UpdateAlignment(VERTICAL, VALIGN_BOTTOM, pCmdUI);
}

// The alignments are enabled if the application is in mark mode and if not
// all the marked cells already have the alignment in question. If all
// cells have the alignment, the menu item is also marked with a radio dot.

void CCalcDoc::UpdateAlignment(Direction eDirection, Alignment eAlignment, CCmdUI *pCmdUI)
{
  switch (m_eCalcStatus)
  {
    case CS_MARK:
      pCmdUI->Enable(!IsAlignment(eDirection, eAlignment));
      pCmdUI->SetRadio(IsAlignment(eDirection, eAlignment));
      break;

    case CS_EDIT:
      pCmdUI->Enable(FALSE);
      pCmdUI->SetRadio(FALSE);
      break;
  }
}

// IsAlignment goes through all the marked cells and return false if at
// least one of them does not have the given alignment. It returns true
// only if all cells in the marked block have the alignment.

BOOL CCalcDoc::IsAlignment(Direction eDirection,
                           Alignment eAlignment)
{
  int iMinMarkedRow = min(m_rfFirstMark.GetRow(),
                          m_rfLastMark.GetRow());
  int iMaxMarkedRow = max(m_rfFirstMark.GetRow(),
                          m_rfLastMark.GetRow());
  int iMinMarkedCol = min(m_rfFirstMark.GetCol(),
                          m_rfLastMark.GetCol());
  int iMaxMarkedCol = max(m_rfFirstMark.GetCol(),
                          m_rfLastMark.GetCol());

  for (int iRow = iMinMarkedRow; iRow <= iMaxMarkedRow;
       ++iRow)
  {
    for (int iCol = iMinMarkedCol; iCol <= iMaxMarkedCol;
         ++iCol)
    {
      Cell* pCell = m_cellMatrix.Get(iRow, iCol);

      if (eAlignment != pCell->GetAlignment(eDirection))
      {
        return FALSE;
      }
    }
  }

  // If no cell did not have the given alignment, we return true. That is, we
  // return true only if all cells have the given alignment.

  return TRUE;
}

// The alignment methods simple call SetAlignment below.

void CCalcDoc::OnAlignmentHorizontalLeft()
{
  SetAlignment(HORIZONTAL, HALIGN_LEFT);
}

void CCalcDoc::OnAlignmentHorizontalCenter()
{
  SetAlignment(HORIZONTAL, HALIGN_CENTER);
}

void CCalcDoc::OnAlignmentHorizontalRight()
{
  SetAlignment(HORIZONTAL, HALIGN_RIGHT);
}

void CCalcDoc::OnAlignmentHorizontalJustified()
{
  SetAlignment(HORIZONTAL, HALIGN_JUSTIFIED);
}

void CCalcDoc::OnAlignmentVerticalTop()
{
  SetAlignment(VERTICAL, VALIGN_TOP);
}

void CCalcDoc::OnAlignmentVerticalCenter()
{
  SetAlignment(VERTICAL, VALIGN_CENTER);
}

void CCalcDoc::OnAlignmentVerticalBottom()
{
  SetAlignment(VERTICAL, VALIGN_BOTTOM);
}

// SetAlignment sets the given alignment for all cells in the marked block.
// Remember that this method is called only if the application is in mark
// mode and at least one cell does not already have the alignment in
// question.

void CCalcDoc::SetAlignment(Direction eDirection,
                            Alignment eAlignment)
{
  int iMinMarkedRow = min(m_rfFirstMark.GetRow(),
                          m_rfLastMark.GetRow());
  int iMaxMarkedRow = max(m_rfFirstMark.GetRow(),
                          m_rfLastMark.GetRow());
  int iMinMarkedCol = min(m_rfFirstMark.GetCol(),
                          m_rfLastMark.GetCol());
  int iMaxMarkedCol = max(m_rfFirstMark.GetCol(),
                          m_rfLastMark.GetCol());

  for (int iRow = iMinMarkedRow; iRow <= iMaxMarkedRow;
       ++iRow)
  {
    for (int iCol = iMinMarkedCol; iCol <= iMaxMarkedCol;
         ++iCol)
    {
      Cell* pCell = m_cellMatrix.Get(iRow, iCol);
      pCell->SetAlignment(eDirection, eAlignment);
    }
  }

  RepaintMarkedArea();
  SetModifiedFlag();
}

// The user can update the text or background color when the application is
// in mark mode.

void CCalcDoc::OnUpdateColorText(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

void CCalcDoc::OnUpdateColorBackground(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

// The color update methods both call OnColor.

void CCalcDoc::OnTextColor()
{
    OnColor(TEXT);
}

void CCalcDoc::OnBackgroundColor()
{
    OnColor(BACKGROUND);
}

// OnColor displays the MFC Color Dialog, and if the user presses ok, the
// chosen color is set on all marked cells without that color.

void CCalcDoc::OnColor(int iColorType)
{
  // The default color is picked from the latest marked cell.

  Cell* pCell = m_cellMatrix.Get(m_rfLastMark.GetRow(), m_rfLastMark.GetCol());
  CColorDialog colorDialog((COLORREF) pCell->GetColor(iColorType));

  if (colorDialog.DoModal() == IDOK)
  {
    Color color = colorDialog.GetColor();

    int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
    int iMaxMarkedRow = max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
    int iMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());
    int iMaxMarkedCol = max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

    for (int iRow = iMinMarkedRow; iRow <= iMaxMarkedRow; ++iRow)
    {
      for (int iCol = iMinMarkedCol; iCol <= iMaxMarkedCol; ++iCol)
      {
        Cell* pCell = m_cellMatrix.Get(iRow, iCol);

        if (pCell->GetColor(iColorType) != color)
        {
          pCell->SetColor(iColorType, color);
          SetModifiedFlag();
        }
      }
    }

    // Finally, the whole marked block is updated.
    RepaintMarkedArea();
  }
}

// The font of the marked cells can be updated when the application is in mark
// mode.

void CCalcDoc::OnUpdateFont(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(m_eCalcStatus == CS_MARK);
}

// OnFont displays the MFC Font Dialog similar to the Color Dialog in
// OnColor above.

void CCalcDoc::OnFont()
{
  // The default font and text color is picked from the latest marked cell.

  Cell* pCell = m_cellMatrix.Get(m_rfFirstMark.GetRow(), m_rfFirstMark.GetCol());

  // We load the dialog with the default font.

  Font firstFont = pCell->GetFont();
  LOGFONT logFont = (LOGFONT) firstFont;
  CFontDialog fontDialog(&logFont);

  // We load the dialog with the default text color.

  Color firstColor = pCell->GetColor(TEXT);
  fontDialog.m_cf.rgbColors = (COLORREF) firstColor;

  if (fontDialog.DoModal() == IDOK)
  {
    fontDialog.GetCurrentFont(&logFont);
    Font newFont = (Font) logFont;
    Color newColor = (Color) fontDialog.GetColor();

    int iMinMarkedRow = min(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
    int iMaxMarkedRow = max(m_rfFirstMark.GetRow(), m_rfLastMark.GetRow());
    int iMinMarkedCol = min(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());
    int iMaxMarkedCol = max(m_rfFirstMark.GetCol(), m_rfLastMark.GetCol());

    // All marked cell not already having the chosen font and color are
    // updated.

    for (int iRow = iMinMarkedRow; iRow <= iMaxMarkedRow; ++iRow)
    {
      for (int iCol = iMinMarkedCol; iCol <= iMaxMarkedCol; ++iCol)
      {
        Cell* pCell = m_cellMatrix.Get(iRow, iCol);

        if ((pCell->GetFont() != newFont) ||
            (pCell->GetColor(TEXT) != newColor))
        {
          pCell->SetFont(newFont);
          pCell->SetColor(TEXT, newColor);

          // The Modified flag is set if we actually find a cell to update.
          SetModifiedFlag();
        }
      }
    }

    // Finally, we update the marked block.
    RepaintMarkedArea();
  }
}
