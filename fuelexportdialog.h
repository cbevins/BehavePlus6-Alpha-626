//------------------------------------------------------------------------------
/*! \file fuelexportdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument fuel model export dialog.
 */

#ifndef _FUELEXPORTDIALOG_H_
/*! \def _FUELEXPORTDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _FUELEXPORTDIALOG_H_ 1

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
/*! \class FuelExportDialog fuelexportdialog.h
 *
 *  \brief Dialog for selecting fuel models to export.
 */

class FuelExportDialog : public AppDialog
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
    FuelExportDialog( BpDocument *parent, QString htmlFile, const char *name=0 ) ;
    ~FuelExportDialog( void ) ;
    int resultString( QString &qStr ) ;

// Protected slots
protected slots:
    void contextMenuActivated( int id ) ;
    void itemDoubleClicked( QListViewItem *item ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void store( void ) ;

// Private methods
private:
    bool displayContents( QListViewItem *lvi ) ;

// Private data
private:
    BpDocument    *m_bp;            //!< Pointer to the parent BpDocument
    QFrame        *m_gridFrame;     //!< Pointer to dialog's grid frame
    QGridLayout   *m_gridLayout;    //!< Pointer to dialog's grid layout
    QListView     *m_listView;      //!< Pointer to the choices list
    QString        m_result;        //!< Number of implied values
    int            m_results;       //!< Fully expanded value string
    QPopupMenu    *m_contextMenu;   //!< Pointer to listview context menu
    QListViewItem *m_lvi;           //!< Pointer to context menu listview item
};

#endif

//------------------------------------------------------------------------------
//  End of fuelexportdialog.h
//------------------------------------------------------------------------------

