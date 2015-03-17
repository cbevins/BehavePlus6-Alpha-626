//------------------------------------------------------------------------------
/*! \file fuelinitdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument worksheet fuel initialization dialog.
 */

#ifndef _FUELINITDIALOG_H_
/*! \def _FUELINITDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _FUELINITDIALOG_H_ 1

// Class references
#include "appdialog.h"
class BpDocument;
class QListView;
class QListViewItem;
class QPopupMenu;

//------------------------------------------------------------------------------
/*! \class FuelInitDialog fuelinitdialog.h
 *
 *  \brief Worksheet Fuel Initialization button dialog.
 */

class FuelInitDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
    \brief Enumerates the types of context menu options available.
 */
enum ContextMenuOption
{
    ContextSelectFuelModel=1,
    ContextViewFuelParameters=2,
    ContextPrintVisibleView=3,
    ContextPrintEntireView=4
};

// Public methods
public:
    FuelInitDialog( BpDocument *parent, const char *name=0 ) ;
    ~FuelInitDialog( void ) ;
    void resultString( QString &qStr ) ;

// Protected slots
protected slots:
    void contextMenuActivated( int id ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    virtual void itemDoubleClicked( QListViewItem *item ) ;
    virtual void store( void );

// Private methods
private:
    bool displayContents( QListViewItem *lvi ) ;

private:
    BpDocument     *m_bp;           //!< Pointer to parent BpDocument
    QListView      *m_listView;     //!< Pointer to the list of model
    QString         m_result;       //!< Selected string
    QPopupMenu     *m_contextMenu;  //!< Pointer to listview context menu
    QListViewItem  *m_lvi;          //!< Pointer to context menu listview item
};

#endif

//------------------------------------------------------------------------------
//  End of fuelinitdialog.h
//------------------------------------------------------------------------------

