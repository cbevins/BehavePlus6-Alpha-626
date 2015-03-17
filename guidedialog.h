//------------------------------------------------------------------------------
/*! \file guidedialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument worksheet guide button dialog.
 */

#ifndef _GUIDEDIALOG_H_
/*! \def _GUIDEDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _GUIDEDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class BpDocument;
class EqVar;
class QFrame;
class QGridLayout;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPopupMenu;

//------------------------------------------------------------------------------
/*! \class GuideDialog guidedialog.h
 *
 *  \brief Guide button dialog for entering worksheet data.
 */

class GuideDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of context menu options available.
 */

enum ContextMenuOption
{
    ContextOk=0,
    ContextSelect=1,
    ContextDeselect=2,
    ContextViewParameters=3,
    ContextPrintVisibleView=4,
    ContextPrintEntireView=5
};

// Public methods
public:
    GuideDialog( BpDocument *parent, int lid, const char *name=0 ) ;
    ~GuideDialog( void ) ;
    int resultString( QString &qStr ) ;

// Protected slots
protected slots:
    void clear( void );
    void contextMenuActivated( int id ) ;
    void itemDoubleClicked( QListViewItem *item ) ;
    void keyPressEvent( QKeyEvent *e ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void store( void ) ;
    void wizard( void );

// Private methods
private:
    bool displayContents( QListViewItem *lvi ) ;
    void storeContinuous( double d ) ;
    bool validRange( void );

// Private data
private:
    BpDocument    *m_bp;            //!< Pointer to the parent BpDocument
    EqVar         *m_var;           //!< Pointer to subject EqVar
    QFrame        *m_gridFrame;     //!< Pointer to dialog's grid frame
    QGridLayout   *m_gridLayout;    //!< Pointer to dialog's grid layout
    QLabel        *m_rangeLabel;    //!< Pointer to the valid range label
    QListView     *m_listView;      //!< Pointer to the choices list
    QLabel        *m_lbl[3];        //!< From, Thru, Step entry labels
    QLineEdit     *m_entry[3];      //!< From, Thru, Step entry widgets
    bool           m_blank[3];      //!< From, Thru, Step entry flags
    double         m_value[3];      //!< From, Thru, Step entry values
    QString        m_result;        //!< Number of implied values
    int            m_results;       //!< Fully expanded value string
    QPopupMenu    *m_contextMenu;   //!< Pointer to listview context menu
    QListViewItem *m_lvi;           //!< Pointer to context menu listview item
};

#endif

//------------------------------------------------------------------------------
//  End of guidedialog.h
//------------------------------------------------------------------------------

