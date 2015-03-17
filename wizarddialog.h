//------------------------------------------------------------------------------
/*! \file wizarddialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlusDocument continuous variable wizard dialog base class.
 */

#ifndef _WIZARDDIALOG_H_
/*! \def _WIZARDDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _WIZARDDIALOG_H_ 1

// Class references
#include "appdialog.h"
class BpDocument;
class EqVar;
class QListView;
class QListViewItem;
class QPopupMenu;

//------------------------------------------------------------------------------
/*! \class WizardDialog  wizarddialog.h
 *
 *  \brief BpDocument continuous variable wizard dialog base class.
 *
 *  It primarily provides the BehavePlus look and feel.
 *  It is subclassed by StandardWizard and FuelWizard, which provide more
 *  mechanics and functionality for fuel parameter wizards and all other
 *  continuous variable wizards.
 */

class WizardDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
    \brief Enumerates the types of Document context menu options available.
 */
enum ContextMenuOption
{
    ContextOk=0,
    ContextSelect=1,
    ContextDeselect=2,
    ContextPrintVisibleView=3,
    ContextPrintEntireView=4
};

// Public methods
public:
    WizardDialog(
        BpDocument    *bp,
        EqVar         *var,
        const QString &pictureFile,
        const QString &pictureTip,
        const char    *name,
        const QString &acceptKey="WizardDialog:Ok",
        const QString &rejectKey="WizardDialog:Cancel" ) ;
    ~WizardDialog( void ) ;
    int  resultString( QString &qStr ) ;

// Protected slots
protected slots:
    void contextMenuActivated( int id ) ;
    virtual void itemDoubleClicked( QListViewItem *item ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    virtual void store( void ) ;

// Protect data
protected:
    BpDocument     *m_bp;           //!< Pointer to the parent BpDocument
    EqVar          *m_var;          //!< Pointer to subject EqVar
    QListView      *m_listView;     //!< Pointer to QListView
    QString         m_result;       //!< String of all selected values
    int             m_results;      //!< Number of selected values
    QPopupMenu     *m_contextMenu;  //!< Pointer to listview context menu
    QListViewItem  *m_lvi;          //!< Pointer to context menu listview item
};

#endif

//------------------------------------------------------------------------------
//  End of wizarddialog.h
//------------------------------------------------------------------------------

