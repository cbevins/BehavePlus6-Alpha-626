//------------------------------------------------------------------------------
/*! \file horizontaldistancedialog.h
 *  \version BehavePlus4
 *  \author Copyright (C) 2002-2006 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus ground vs horizontal map distance tool.
 */

#ifndef _HORIZONTALDISTANCEDIALOG_H_
/*! \def _HORIZONTALDISTANCEDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _HORIZONTALDISTANCEDIALOG_H_ 1

/*! \def M_PI
 *  \brief Some compilers don't define this.
 */
#ifndef M_PI
#define M_PI 3.141592654
#endif

// Forward class references
#include "appdialog.h"
class QFrame;
class QGridLayout;
class QLineEdit;
class QPopupMenu;
class QPushButton;

//------------------------------------------------------------------------------
/*! \class HorizontalDistanceDialog horizontaldistancedialog.h
 *
 *  \brief BehavePlus ground vs horizontal map distance tool.
 */

class HorizontalDistanceDialog : public AppDialog
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
    HorizontalDistanceDialog( QWidget *parent,
        const char *name="horizontalDistanceDialog" ) ;
    ~HorizontalDistanceDialog( void ) ;

// Protected slots
protected slots:
    bool convert( void );
    void keyPressEvent( QKeyEvent *e ) ;

// Public data members
public:
    QFrame         *m_gridFrame;        //!< Pointer to grid frame
    QGridLayout    *m_gridLayout;       //!< Pointer to the layout widget
    QLabel         *m_lbl[10];          //!< Pointer to array of entry labels
    QLineEdit      *m_entry[10];        //!< Pointer to array of entry fields
    QPushButton    *m_convertButton;    //!< Pointer to the Convert button
};

#endif

//------------------------------------------------------------------------------
//  End of horizontaldistancedialog.h
//------------------------------------------------------------------------------

