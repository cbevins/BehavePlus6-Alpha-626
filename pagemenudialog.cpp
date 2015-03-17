//------------------------------------------------------------------------------
/*! \file pagemenudialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QListView for selecting a document page from a large ToC.
 */

// Custom include files
#include "apptranslator.h"
#include "document.h"
#include "pagemenudialog.h"
#include "toc.h"

// Qt include files
#include <qlistview.h>
#include <qpopupmenu.h>

//------------------------------------------------------------------------------
/*! \brief Rebuilds a list view containing the current table of contents (ToC).
 */

PageMenuDialog::PageMenuDialog( Document *doc, int currentPage,
        QPopupMenu *popupMenu, const char *name ) :
    AppDialog( popupMenu, "PageMenuDialog:Caption",
        "BrightIdea.png", "Bright Idea", "", name,
        "PageMenuDialog:Button:Ok", "PageMenuDialog:Button:Cancel" ),
    m_listView(0),
    m_currentPage(currentPage),
    m_selectedPage(-1)
{
    // Create a list view
    m_listView = new QListView( contentFrame(), "m_listView" );
    Q_CHECK_PTR( m_listView );
    QString page( "" ), text( "" );
    translate( text, "PageMenuDialog:ListView:Col0" );
    m_listView->addColumn( text );
    m_listView->addColumn( "" );
    translate( text, "PageMenuDialog:ListView:Col1" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    m_listView->setColumnWidthMode( 2, QListView::Maximum );
    m_listView->setMultiSelection( false );
    m_listView->setRootIsDecorated( false );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );
    m_listView->setItemMargin( 3 );

    // Add table of contents entries
    TocItem *tocItem;
    QListViewItem *lvi;
    QPixmap pm;
    int ypos = 0;
    for ( tocItem = doc->m_tocList->first();
          tocItem != 0;
          tocItem = doc->m_tocList->next() )
    {
        lvi = new QListViewItem( m_listView );
        Q_CHECK_PTR( lvi );
        lvi->setText( 0, QString( "%1" ).arg( tocItem->m_page, 4 ) );
        lvi->setPixmap( 1, doc->m_tocList->pixmap( tocItem->m_type, pm ) );
        lvi->setText( 2, tocItem->m_text );
        // Show the current page
        if ( tocItem->m_page == m_currentPage )
        {
            m_listView->setSelected( lvi, true );
            ypos = m_listView->itemPos( lvi );
        }
    }
    // Show the current selection
    m_listView->setContentsPos( 0, ypos );
    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( itemDoubleClicked( QListViewItem * ) ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

PageMenuDialog::~PageMenuDialog( void )
{
    delete m_listView;  m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback for mouse double-click on a page in the ToC.
 */

void PageMenuDialog::itemDoubleClicked( QListViewItem *lvi )
{
    if ( lvi )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the selected page.
 *
 *  \return The selected page number, or 0 if no selected page.
 */

int PageMenuDialog::selectedPage( void )
{
    return( m_selectedPage );
}

//------------------------------------------------------------------------------
/*! \brief Stores the selected page and closes the dialog.
 */

void PageMenuDialog::store( void )
{
    QListViewItem *lvi = m_listView->selectedItem();
    if ( lvi )
    {
        m_selectedPage = lvi->text( 0 ).toInt() - 1;
    }
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of pagemenudialog.cpp
//------------------------------------------------------------------------------

