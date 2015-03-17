//------------------------------------------------------------------------------
/*! \file pagemenudialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QListView for selecting a document page from a large ToC.
 */

#ifndef _PAGEMENUDIALOG_H_
/*! \def _PAGEMENUDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _PAGEMENUDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class Document;
class QListView;
class QListViewItem;
class QPopupMenu;

//------------------------------------------------------------------------------
/*! \class PageMenuDialog pagemenudialog.h
 *
 *  \brief QListView for selecting a document page from a large ToC.
 */

class PageMenuDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    PageMenuDialog( Document *doc, int currentPage, QPopupMenu *popupMenu,
        const char *name="pageMenuDialog" ) ;
    ~PageMenuDialog( void ) ;
    int selectedPage( void ) ;

// Protected slots
protected slots:
    void itemDoubleClicked( QListViewItem *lvi ) ;
    void store( void ) ;

// Public data
public:
    QListView  *m_listView;     //!< Pointer to the QListView
    int         m_currentPage;  //!< Current display page number
    int         m_selectedPage; //!< Selected display page number
};

#endif

//------------------------------------------------------------------------------
//  End of pagemenu.h
//------------------------------------------------------------------------------

