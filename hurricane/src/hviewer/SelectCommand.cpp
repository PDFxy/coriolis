
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2008, All Rights Reserved
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./SelectCommand.cpp"                      |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


# include <QMouseEvent>
# include <QKeyEvent>
# include <QAction>

# include "hurricane/Cell.h"

# include "hurricane/viewer/CellWidget.h"
# include "hurricane/viewer/SelectCommand.h"
# include "hurricane/viewer/HSelectionPopup.h"


namespace Hurricane {


// -------------------------------------------------------------------
// Class  :  "SelectCommand".


  SelectCommand::SelectCommand ()
    : AreaCommand()
    , _selectAction(NULL)
    , _selectionPopup(NULL)
  {
    _selectionPopup = new HSelectionPopup ();

    connect ( _selectionPopup, SIGNAL(occurrenceSelected(Occurrence,bool))
            , this           , SIGNAL(selectionToggled  (Occurrence,bool)) );
  }


  SelectCommand::~SelectCommand ()
  {
    delete _selectionPopup;
  }


  void  SelectCommand::bindToAction ( QAction* action )
  {
    _selectAction = action;
  }


  bool  SelectCommand::mousePressEvent ( CellWidget* widget, QMouseEvent* event )
  {
    if ( isActive() ) return true;

    if ( event->button() == Qt::RightButton ) {
      if ( !event->modifiers() ) {
        setActive         ( true );
        setStartPoint     ( event->pos() );
        setDrawingEnabled ( true );
      } else if ( event->modifiers() == Qt::ControlModifier ) {
        QRect selectArea ( event->pos() - QPoint(2,2), QSize(4,4) );
        forEach ( Occurrence, ioccurrence
                , widget->getCell()->getOccurrencesUnder(widget->screenToDbuBox(selectArea)) ) {
          _selectionPopup->add ( *ioccurrence );
        }
        _selectionPopup->updateLayout ();
        _selectionPopup->move ( event->globalPos() );
        _selectionPopup->popup ();
      }
    }

    return isActive();
  }


  bool  SelectCommand::mouseReleaseEvent ( CellWidget* widget, QMouseEvent* event )
  {
    if ( !isActive() ) return false;

    setActive ( false );
    setDrawingEnabled ( false );

    QRect selectArea;
    if ( _startPoint == _stopPoint )
      selectArea = QRect ( _startPoint - QPoint(2,2), QSize(4,4) );
    else
      selectArea = QRect ( _startPoint, _stopPoint );
      
  //widget->unselectAll ();
    widget->selectOccurrencesUnder ( widget->screenToDbuBox(selectArea) );

    if ( _selectAction ) {
      if ( !_selectAction->isChecked() )
        _selectAction->setChecked ( true );
      else
        widget->redraw ();
    } else {
      widget->setShowSelection ( true );
      widget->redraw ();
    }
    
    return false;
  }



} // End of Hurricane namespace.