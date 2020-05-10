#include "StdAfx.h"

#include <fstream>
using namespace std;

#include "Square.h"
#include "Figure.h"
#include "FigureInfo.h"
#include "ColorGrid.h"

#include "Tetris.h"
#include "TetrisView.h"
#include "TetrisDoc.h"

IMPLEMENT_DYNCREATE(CTetrisDoc, CDocument)

// As we do not add any menus or toolbar buttons to our applications, the
// message map is empty. The constructor initiates the fields, including the
// constant rectangles surrounding the next figure and the score. It also
// initiates the random generator and figure list as well as loading the high
// score list. The destructor de-allocates the dynamically memory allocated by
// this object and saves the high score list.

BEGIN_MESSAGE_MAP(CTetrisDoc, CDocument)
  // Empty.
END_MESSAGE_MAP()

Figure redFigure(NORTH, RED, RedInfo);
Figure brownFigure(EAST, BROWN, BrownInfo);
Figure turquoiseFigure(EAST, TURQUOISE, TurquoiseInfo);
Figure greenFigure(EAST, GREEN, GreenInfo);
Figure blueFigure(SOUTH, BLUE, BlueInfo);
Figure purpleFigure(SOUTH, PURPLE, PurpleInfo);
Figure yellowFigure(SOUTH, YELLOW, YellowInfo);

Figure CTetrisDoc::m_figureArray[] = {redFigure, brownFigure,
                   turquoiseFigure, greenFigure, yellowFigure,
                   blueFigure, purpleFigure};

CTetrisDoc::CTetrisDoc()
 :NEXT_AREA(COLS, 0, 2 * COLS, 3),
  SCORE_AREA(COLS, 3, 2 * COLS, 6),
  m_iScore(0)
{
  srand((unsigned int) time(NULL));

  for (int iIndex = 0; iIndex < FIGURE_ARRAY_SIZE; ++iIndex)
  {
    m_figureArray[iIndex].SetColorGrid(&m_colorGrid);
  }

  m_activeFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];
  m_nextFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];

  ifstream inFile("ScoreList.txt", ios::binary);

  int iScore;
  while (inFile >> iScore)
  {
    m_scoreList.AddTail(iScore);
  }
}

CTetrisDoc::~CTetrisDoc()
{
  SaveScoreList();
}

void CTetrisDoc::SaveScoreList()
{
  ofstream outFile("ScoreList.txt", ios::binary);

  for (POSITION pos = m_scoreList.GetHeadPosition();
       pos != NULL; m_scoreList.GetNext(pos))
  {
    int iScore = m_scoreList.GetAt(pos);
    outFile << iScore << endl;
  }
}

BOOL CTetrisDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  m_iScore = 0;
  m_colorGrid.Clear();
  srand((unsigned int) time(NULL));
  m_activeFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];
  m_nextFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];
  UpdateAllViews(NULL);

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}

// Serialize is called when the user wants to save or load a game. The
// framework associates the file with a path name and calls the method to
// perform the actually saving or loading of the object's data.

// As we cannot tell for sure what object m_activeFigure and m_nextFigure
// actually points at (it can be any one of the seven sub classes of Figure),
// first we have to write information about the object with WriteClass, and
// then save the object itself with WriteObject in order to read and create
// object of the correct classes with ReadClass and ReadObject.

void CTetrisDoc::Serialize(CArchive& archive)
{
  if (archive.IsStoring())
  {
    archive << m_iScore;
  }

  if (archive.IsLoading())
  {
    archive >> m_iScore;
  }

  m_colorGrid.Serialize(archive);
  m_nextFigure.Serialize(archive);
  m_activeFigure.Serialize(archive);
}

// LeftArrowKey, RightArrowKey, UpArrowKey, and DownArrowKey are called by the
// view class objects when the user presses the corresponding key. They call
// the corresponding method of the active figure and, if the movement was
// possible do perform, update the old and new area of the figure. In which
// case the modified flag is also set.

void CTetrisDoc::LeftArrowKey()
{
  CRect rcOldArea = m_activeFigure.GetArea();

  if (m_activeFigure.MoveLeft())
  {
    CRect rcNewArea = m_activeFigure.GetArea();

    UpdateAllViews(NULL, COLOR, (CObject*) &rcOldArea);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcNewArea);

    SetModifiedFlag();
  }
}

void CTetrisDoc::RightArrowKey()
{
  CRect rcOldArea = m_activeFigure.GetArea();

  if (m_activeFigure.MoveRight())
  {
    CRect rcNewArea = m_activeFigure.GetArea();

    UpdateAllViews(NULL, COLOR, (CObject*) &rcOldArea);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcNewArea);

    SetModifiedFlag();
  }
}

void CTetrisDoc::UpArrowKey()
{
  CRect rcOldArea = m_activeFigure.GetArea();

  if (m_activeFigure.RotateClockwise())
  {
    CRect rcNewArea = m_activeFigure.GetArea();

    UpdateAllViews(NULL, COLOR, (CObject*) &rcOldArea);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcNewArea);

    SetModifiedFlag();
  }
}

void CTetrisDoc::DownArrowKey()
{
  CRect rcOldArea = m_activeFigure.GetArea();

  if (m_activeFigure.RotateAnticlockwise())
  {
    CRect rcNewArea = m_activeFigure.GetArea();

    UpdateAllViews(NULL, COLOR, (CObject*) &rcOldArea);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcNewArea);

    SetModifiedFlag();
  }
}

// Timer is called by the view class when it receives the timer message, which
// occurs every second the view has focus. This method is a little more
// complicated than those above because we have to take action regardless of
// whether the movement down is possible or not. If the movement is possible,
// we update the figure's old and new area. However, if it is not possible,
// the figure is finished, and we add its squares to the grid. We then let the
// next figure become the active figure and we create a new next figure. We
// also delete the full rows, if any. The new active figure must be tested for
// validity. That means that the initial positions for the figure are not
// already occupied. If they are, the game is over and we ask the player for
// another game.

BOOL CTetrisDoc::Timer()
{
  SetModifiedFlag();
  CRect rcOldArea = m_activeFigure.GetArea();

  if (m_activeFigure.MoveDown())
  {
    CRect rcNewArea = m_activeFigure.GetArea();

    UpdateAllViews(NULL, COLOR, (CObject*) &rcOldArea);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcNewArea);

    return TRUE;
  }

  else
  {
    m_activeFigure.AddToGrid();

    m_activeFigure = m_nextFigure;
    CRect rcActiveArea = m_activeFigure.GetArea();
    UpdateAllViews(NULL, COLOR, (CObject*) &rcActiveArea);

    m_nextFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];
    UpdateAllViews(NULL, COLOR, (CObject*) &NEXT_AREA);

    DeleteFullRows();

    if (!m_activeFigure.IsFigureValid())
    {
      GameOver();
    }

    return FALSE;
  }
}

// SpaceKey just calls Timer as many times as possible at intervals on twenty
// milliseconds to make the active figure fall down the game grid.

void CTetrisDoc::SpaceKey()
{
  while (Timer())
  {
    Sleep(20);
  }
}

// GameOver first update the client area in gray colors. Then it asks the
// player for another game. If the player says yes, we reset the game plan
// and then redraw the game plan in color. If the player says no, we
// just abort the application.

void CTetrisDoc::GameOver()
{
  UpdateAllViews(NULL, GRAY);

  if (NewGame())
  {
    m_colorGrid.Clear();

    m_activeFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];
    m_nextFigure = m_figureArray[rand() % FIGURE_ARRAY_SIZE];

    UpdateAllViews(NULL, COLOR);
  }

  else
  {
    SaveScoreList();
    exit(0);
  }
}

// NewGame adds the score to the high score list and asks the player for a new
// game. If the player made the top ten list, the message says so.

BOOL CTetrisDoc::NewGame()
{
  CString stMessage;
  int iScoreNumber = AddScoreToList();

  if (iScoreNumber > 0)
  {
    stMessage.Format(TEXT("Game Over.\nYou scored %d points, and made\n")
                     TEXT("number %d at the high score list.\nAnother game?"),
                     m_iScore, iScoreNumber);
  }

  else
  {
    stMessage.Format(TEXT("Game Over.\nYou scored %d points.\n")
                     TEXT("Another game?"), m_iScore);
  }

  return (MessageBox(NULL, stMessage, TEXT("Tetris"), MB_YESNO) == IDYES);
}

// AddScoreToList adds the latest score to the high score list if it makes the
// ten top list. If it does, the high score number is returned. If not, zero
// is returned.

int CTetrisDoc::AddScoreToList()
{
  int iScoreNumber = 0;

  for (POSITION pos = m_scoreList.GetHeadPosition();
       pos != NULL; m_scoreList.GetNext(pos))
  {
    ++iScoreNumber;
    int iScore = m_scoreList.GetAt(pos);

    if (m_iScore > iScore)
    {
      m_scoreList.InsertBefore(pos, m_iScore);

      if (m_scoreList.GetSize() > 10)
      {
        m_scoreList.RemoveTail();
      }

      return iScoreNumber;
    }
  }

  int iSize = (int) m_scoreList.GetSize();
  if (iSize < 10)
  {
    m_scoreList.AddTail(m_iScore);
    return (iSize + 1);
  }

  return 0;
}

// When a figure has reached its final position, it may have filled one or
// several (at most four) rows, those rows shall be removed from the game
// grid. DeleteFullRows takes care of that, for each full row it found, it
// flashes it, deletes it, and updates the current score.

void CTetrisDoc::DeleteFullRows()
{
  int iRow = ROWS - 1;

  while (iRow >= 0)
  {
    if (IsRowFull(iRow))
    {
      FlashRow(iRow);
      DeleteRow(iRow);

      ++m_iScore;
      UpdateAllViews(NULL, COLOR, (CObject*) &SCORE_AREA);
    }

    else
    {
      --iRow;
    }
  }
}

// IsRowFull decides whether a given row if filled or not.

BOOL CTetrisDoc::IsRowFull(int iRow)
{
  for (int iCol = 0; iCol < COLS; ++iCol)
  {
    if (m_colorGrid.Index(iRow, iCol) == DEFAULT_COLOR)
    {
      return FALSE;
    }
  }

  return TRUE;
}

// The flash effect is executed by redrawing the row in color and in gray
// scale three times with at an interval of 50 milliseconds.

void CTetrisDoc::FlashRow(int iRow)
{
  for (int iCount = 0; iCount < 3; ++iCount)
  {
    CRect rcRowArea(0, iRow, COLS, iRow + 1);

    UpdateAllViews(NULL, GRAY, (CObject*) &rcRowArea);
    Sleep(50);

    CRect rcRowArea2(0, iRow, COLS, iRow + 1);
    UpdateAllViews(NULL, COLOR, (CObject*) &rcRowArea2);
    Sleep(50);
  }
}

// When a row is removed, we do not really remove it. If we did, the game grid
// should shrink. Instead, we copy the squares above it and clear the top row.

void CTetrisDoc::DeleteRow(int iMarkedRow)
{
  for (int iRow = iMarkedRow; iRow > 0; --iRow)
  {
    for (int iCol = 0; iCol < COLS; ++iCol)
    {
      m_colorGrid.Index(iRow, iCol) =
      m_colorGrid.Index(iRow - 1, iCol);
    }
  }

  for (int iCol = 0; iCol < COLS; ++iCol)
  {
    m_colorGrid.Index(0, iCol) = WHITE;
  }

  CRect rcArea(0, 0, COLS, iMarkedRow + 1);
  UpdateAllViews(NULL, COLOR, (CObject*) &rcArea);
}
