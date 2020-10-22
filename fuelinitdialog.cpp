//------------------------------------------------------------------------------
/*! \file fuelinitdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument worksheet fuel initialization dialog.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "fuelinitdialog.h"
#include "fuelmodel.h"
#include "textview.h"           // For widget printing
#include "xeqapp.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qcursor.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpopupmenu.h>

//------------------------------------------------------------------------------
/*! \brief Displays a FuelInit dialog appropriate to the variable.
 *
 *  \param bp   Pointer to the parent BpDocument.
 *  \param name Widget's internal name.
 */

FuelInitDialog::FuelInitDialog( BpDocument *bp, const char *name ) :
    AppDialog( bp, "FuelInitDialog:Caption",
        "Oktoberfest.png", "Oktoberfest", "fuelInit.html", name ),
    m_bp(bp),
    m_listView(0),
    m_result(""),
    m_contextMenu(0),
    m_lvi(0)
{
    QString text("");
    // Discrete variable item selection list
    m_listView = new QListView( contentFrame(), "listView" );
    Q_CHECK_PTR( m_listView );
    translate( text, "FuelInitDialog:Col0" );
    m_listView->addColumn( text );
    translate( text, "FuelInitDialog:Col1" );
    m_listView->addColumn( text );
    translate( text, "FuelInitDialog:Col2" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    //m_listView->setColumnWidth( 0, 0 );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    m_listView->setColumnWidthMode( 2, QListView::Maximum );
    m_listView->setMultiSelection( false );
    m_listView->setRootIsDecorated( false );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );
    m_listView->setItemMargin( 2 );

    // Get the address of the "FuelBedModel" EqVarItemList
    EqVarItemList *itemList = bp->m_eqApp->m_itemListDict->find( "FuelBedModel" );
    if ( ! itemList )
    // This code block should never be executed!
    {
        translate( text, "FuelInitDialog:NoList" );
        bomb( text );
    }
    // Add each item choice
    QListViewItem *listItem;
    QString fmNumber, fmCode, fmDesc;
    for ( EqVarItem *varItem= itemList->first();
          varItem!=0;
          varItem= itemList->next() )
    {
		fmNumber = varItem->m_sort;
		fmCode   = varItem->m_name;
		if ( fmNumber != fmCode )
		{
			fmDesc = *(varItem->m_desc);
			listItem = new QListViewItem( m_listView, fmNumber, fmCode, fmDesc );
			Q_CHECK_PTR( listItem );
		}
    }
    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( itemDoubleClicked( QListViewItem * ) ) );

    // Allow right button to popup a context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );

    //m_listView->setMinimumWidth( m_listView->columnWidth( 0 )
    //    + m_listView->columnWidth( 1 ) );
    //m_listView->setMaximumWidth( m_listView->sizeHint().width() );
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );

    // Set the initial size for this widget so the entire picture,
    // a good chunk of both the listView and textBrowser are visible.
    int width = widthHint() + m_listView->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

FuelInitDialog::~FuelInitDialog( void )
{
    delete m_listView;      m_listView = 0;
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void FuelInitDialog::contextMenuActivated( int id )
{
    if ( id == ContextSelectFuelModel )
    {
        m_listView->setSelected( m_lvi, true );
        // Set m_lvi to 0 to let rightButtonClicked() know we're done with
        // the dialog
        m_lvi = 0;
    }
    else if ( id == ContextViewFuelParameters )
    {
        displayContents( m_lvi ) ;
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
/*! \brief Displays the contents of the fuel model or moisture scenario
 *  file in an info dialog containing an HTML table of parameters.
 *
 *  \return Returns TRUE on success or FALSE on failure
 */

bool FuelInitDialog::displayContents( QListViewItem *lvi )
{
    // Initialize display strings
    QString msg( "" );
    QString title( "" );
    QString name = lvi->text( 1 );

    // Locate the fuel model and load its values
    FuelModel *fm = m_bp->m_eqApp->m_fuelModelList->fuelModelByModelName( name );
    if ( ! fm )
    // This code block should never be executed!
    {
        translate( title, "FuelInitDialog:NoModel", name );
        error( title );
        return( false );
    }
    // Display the contents and return.
    translate( title, "FuelInitDialog:FuelModel", name );
    fm->formatHtmlTable( title, msg );
    info( msg );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Callback for mouse double-click on a discrete variable item,
 *  signaling that the user wants to select just this item.
 */

void FuelInitDialog::itemDoubleClicked( QListViewItem *item )
{
    // Must click an item
    if ( item )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief User access to the resulting entry value.
 *
 *  \return On return the \a qStr has the new entry string.
 */

void FuelInitDialog::resultString( QString &qStr )
{
    qStr = m_result;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a context menu to select a file or view file parameters.
 *  The menu actions are performed in contextMenuActivated().
 */

void FuelInitDialog::rightButtonClicked( QListViewItem *lvi,
    const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        translate( text, "FuelInitDialog:ContextMenu:Select" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextSelectFuelModel );

        translate( text, "FuelInitDialog:ContextMenu:ViewParameters" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextViewFuelParameters );
    }

    translate( text, "FuelInitDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "FuelInitDialog:ContextMenu:PrintEntire" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_lvi = lvi;
    m_contextMenu->exec( QCursor::pos() );

    // If m_lvi has been reset to 0 by contextMenuActivated(),
    // then the user made a selection and we're done with the dialog
    if ( lvi && ! m_lvi )
    {
        store();
    }

    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Accept button callback that validates and stores the input entries.
 */

void FuelInitDialog::store( void )
{
    QListViewItem *item = m_listView->selectedItem();
    if ( ! item )
    {
        QString caption("");
        translate( caption, "FuelInitDialog:SelectAFuelModel:Caption" );
        QString text("");
        translate( text, "FuelInitDialog:SelectAFuelModel:Text" );
        warn( caption, text );
    }
    else
    {
        m_result = item->text( 1 );
        m_lvi = item;
        accept();
    }
    return;
}

//------------------------------------------------------------------------------
//  End of fuelinitdialog.cpp
//------------------------------------------------------------------------------

