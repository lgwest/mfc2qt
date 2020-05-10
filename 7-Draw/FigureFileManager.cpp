#include "StdAfx.h"

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

FigureFileManager::FigureFileManager(Figure* pFigure /* = NULL */)
{
  m_pFigure = pFigure;
}

int FigureFileManager::GetId() const
{
  if (dynamic_cast<ArrowFigure*>(m_pFigure) != NULL)
  {
    return ARROW;
  }

  else if (dynamic_cast<LineFigure*>(m_pFigure) != NULL)
  {
    return LINE;
  }

  else if (dynamic_cast<EllipseFigure*>(m_pFigure) != NULL)
  {
    return ELLIPSE;
  }

  else if (dynamic_cast<RectangleFigure*>(m_pFigure) != NULL)
  {
    return RECTANGLE;
  }

  else if (dynamic_cast<TextFigure*>(m_pFigure) != NULL)
  {
    return TEXT;
  }

  else
  {
    check(FALSE);
    return 0;
  }
}

void FigureFileManager::CreateFigure(int iId)
{
  switch (iId)
  {
    case LINE:
      check_memory(m_pFigure = new LineFigure());
      break;

    case ARROW:
      check_memory(m_pFigure = new ArrowFigure());
      break;

    case RECTANGLE:
      check_memory(m_pFigure = new RectangleFigure());
      break;

    case ELLIPSE:
      check_memory(m_pFigure = new EllipseFigure());
      break;

    case TEXT:
      check_memory(m_pFigure = new TextFigure());
      break;
    
    default:
      check(FALSE);
  }
}

void FigureFileManager::Serialize(CArchive& archive)
{
  CObject::Serialize(archive);

  if (archive.IsStoring())
  {
    archive << GetId();
  }

  if (archive.IsLoading())
  {
    int iId;
    archive >> iId;
    CreateFigure(iId);
  }
}
