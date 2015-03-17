//------------------------------------------------------------------------------
/*! \file wizarddialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument continuous variable wizard dialog base class.
 */

// Custom include files
#include "apptranslator.h"
#include "bpdocument.h"
#include "textview.h"           // For widget printing
#include "wizarddialog.h"
#include "xeqvar.h"

// Qt include files
#include <qcursor.h>
#include <qlistview.h>
#include <qpopupmenu.h>

//------------------------------------------------------------------------------
/*! \brief BehavePlus wizard dialog base class for continuous variables.
 *
 *  This is the base class for all BehavePlus continuous variable wizards.
 *  It is subclassed by StandardWizard and FuelWizard, which provide more
 *  mechanics and functionality for fuel parameter wizards and all other
 *  continuous variable wizards.
 *
 *  \param bp           Pointer to the parent BpDocument
 *  \param var          Pointer to the subject EqVar
 *  \param pictureFile  Name of the picture file
 *  \param pictureName  Name (title) of the picture itself
 *  \param name         Widget's internal name
 *  \param acceptText   Accept button text translator key
 *                      (default is "WizardDialog:Ok")
 *  \param rejectText   Reject button text translator key
 *                      (default is "WizardDialog:Cancel")
 */

WizardDialog::WizardDialog( BpDocument *bp, EqVar *var,
        const QString &pictureFile, const QString &pictureName,
        const char *name, const QString &acceptKey, const QString &rejectKey ) :
    AppDialog( bp,
        QString( "!" + *(var->m_label) ),   // Variable label is the caption
        pictureFile, pictureName,
        //QString( var->m_name + ".html" ),
        var->m_help,
        name, acceptKey, rejectKey ),
    m_bp(bp),
    m_var(var),
    m_listView(0),
    m_result(""),
    m_results(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Create the scrolling listview to display choices
    m_listView = new QListView( contentFrame(), "m_listView" );
    Q_CHECK_PTR( m_listView );

    // Default columns
    m_listView->addColumn( m_var->m_displayUnits );
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    m_listView->setMultiSelection( true );
    m_listView->setRootIsDecorated( false );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setItemMargin( 2 );

    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( itemDoubleClicked( QListViewItem * ) ) );

    // Allow right click to invoke the context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

WizardDialog::~WizardDialog( void )
{
    delete m_listView;      m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void WizardDialog::contextMenuActivated( int id )
{
    // Restore button to its previous selection state
    m_listView->setSelected( m_lvi, ! m_listView->isSelected( m_lvi ) );

    if ( id == ContextOk )
    {
        // Set m_lvi to 0 so rightButtonClicked() knows we're done with dialog
        m_lvi = 0;
    }
    else if ( id == ContextSelect )
    {
        m_listView->setSelected( m_lvi, true );
    }
    else if ( id == ContextDeselect )
    {
        m_listView->setSelected( m_lvi, false );
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( contentFrame() );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback for mouse double-click on a discrete variable item,
 *  signaling that the user wants to select just this item.
 */

void WizardDialog::itemDoubleClicked( QListViewItem *item )
{
    if ( item )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Builds the fully expanded value set into the passed \a qStr.
 *  The QString is usually then inserted by the caller
 *  into the appropriate entry field text.
 *
 *  \param qStr reference to a QString to hold the resulting value string.
 *
 *  \return Number of items in the value string.
 */

int WizardDialog::resultString( QString &qStr )
{
    qStr = m_result;
    return( m_results );
}

//------------------------------------------------------------------------------
/*! \brief Displays the context menu.
 */

void WizardDialog::rightButtonClicked( QListViewItem *lvi,
    const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    m_lvi = lvi;

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        translate( text, "WizardDialog:ContextMenu:Ok" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextOk );

        translate( text, "WizardDialog:ContextMenu:Select" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextSelect );

        translate( text, "WizardDialog:ContextMenu:Deselect" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextDeselect );
    }

    translate( text, "WizardDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "WizardDialog:ContextMenu:PrintEntire" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;

    // If m_lvi has been reset to 0 by contextMenuActivated(),
    // then the user made a selection and we're done with the dialog
    if ( lvi && ! m_lvi )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stores the selected values into data member \a result.
*/

void WizardDialog::store( void )
{
    // Initialize the result string
    m_results = 0;
    m_result = "";

    // Add each checked item to the result string
    QListViewItem *item = m_listView->firstChild();
    for ( int id = 0;
          item != 0;
          item = item->itemBelow(), id++ )
    {
        if ( item->isSelected() )
        {
            if ( m_results )
            {
                m_result.append( ", " );
            }
            m_result.append( item->text( 0 ).stripWhiteSpace() );
            m_results++;
        }
    }
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of wizarddialog.cpp
//------------------------------------------------------------------------------

