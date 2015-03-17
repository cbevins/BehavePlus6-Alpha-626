//------------------------------------------------------------------------------
/*! \file unitsconverterdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus units converter dialog class declarations.
 */

#ifndef _UNITSCONVERTERDIALOG_H_
/*! \def _UNITSCONVERTERDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _UNITSCONVERTERDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class QFrame;
class QGridLayout;
class QLineEdit;
//class QListView;
//class QListViewItem;
class QPopupMenu;
class QPushButton;

//------------------------------------------------------------------------------
/*! \class UnitsConverterDialog unitsconverterdialog.h
 *
 *  \brief BehavePlus units converter dialog.
 */

class UnitsConverterDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of Document context menu options available.
 */

enum ContextMenuOption
{
    ContextPrintVisibleView=0,
    ContextPrintEntireView=1
};

// Public methods
public:
    UnitsConverterDialog( QWidget *parent,
        const char *name="unitsConverterDialog" ) ;
    ~UnitsConverterDialog( void ) ;

// Protected slots
protected slots:
    bool convert( void );
    void keyPressEvent( QKeyEvent *e ) ;

// Public data members
public:
    QFrame         *m_gridFrame;        //!< Pointer to grid frame
    QGridLayout    *m_gridLayout;       //!< Pointer to the layout widget
    QLabel         *m_lbl[3];           //!< Pointer to array of entry labels
    QLineEdit      *m_entry[4];         //!< Pointer to array of entry fields
    QPushButton    *m_convertButton;    //!< Pointer to the Convert button
};

#endif

//------------------------------------------------------------------------------
//  End of unitsconverterdialog.h
//------------------------------------------------------------------------------

