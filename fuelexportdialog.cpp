//------------------------------------------------------------------------------
/*! \file fuelexportdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument fuel model export dialog.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "fuelmodel.h"
#include "fuelexportdialog.h"
#include "textview.h"           // For widget printing
#include "xeqapp.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qcursor.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief Displays a fuel model selection dialog.
 *
 *  \param bp   Pointer to the parent BpDocument.
 */

FuelExportDialog::FuelExportDialog( BpDocument *bp, QString htmlFile,
        const char *name ) :
    AppDialog( bp,
        "FuelExportDialog:Caption",
        "WildflowersWildSkies.png",
        "Wild Flowers, Wild Skies",
        htmlFile,
        name,
        "GuideDialog:Ok",
        "GuideDialog:Cancel",
        "",
        "" ),
    m_bp(bp),
    m_gridFrame(0),
    m_gridLayout(0),
    m_listView(0),
    m_result(""),
    m_results(0),
    m_contextMenu(0),
    m_lvi(0)
{
    EqVar *varPtr = m_bp->m_eqTree->m_varDict->find( "vSurfaceFuelBedModel" );
    //FuelModelList *fuelModelList = m_bp->m_eqApp->m_fuelModelList;
    // Determine if the m_sort and m_name are the same or not
    bool showSortCol = false;
    for ( int iid = 0;
          iid < (int) varPtr->m_itemList->count();
          iid++ )
    {
       if ( varPtr->m_itemList->itemSort( iid )
        !=  varPtr->m_itemList->itemName( iid ) )
        {
            showSortCol = true;
            break;
        }
    }
    QString text("");
    m_listView = new QListView( contentFrame(), "listView" );
    Q_CHECK_PTR( m_listView );
    translate( text, "FuelInitDialog:Col0" );
    m_listView->addColumn( text );
    translate( text, "FuelInitDialog:Col1" );
    m_listView->addColumn( text );
    translate( text, "FuelInitDialog:Col2" );
    m_listView->addColumn( text );
    if ( showSortCol )
    {
        m_listView->setColumnWidthMode( 0, QListView::Maximum );
    }
    else
    {
        m_listView->setColumnWidthMode( 0, QListView::Manual );
        m_listView->setColumnWidth( 0, 0 );
    }
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    m_listView->setColumnWidthMode( 2, QListView::Maximum );
    m_listView->setMultiSelection( true );
    m_listView->setRootIsDecorated( false );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );
    m_listView->setItemMargin( 3 );

    // Add each item choice
    QListViewItem *item;
    QString fmNumber, fmCode, fmDesc;
    for ( int iid = 0;
          iid < (int) varPtr->m_itemList->count();
          iid++ )
    {
		fmNumber = varPtr->m_itemList->itemSort( iid );
		fmCode   = varPtr->m_itemList->itemName( iid );
		if ( fmNumber != fmCode )
		{
	        fmDesc = *( varPtr->m_itemList->itemDesc( iid ) );
		    item = new QListViewItem( m_listView, fmNumber, fmCode, fmDesc );
		    Q_CHECK_PTR( item );
		}
    }
    //m_listView->setMinimumWidth( m_listView->columnWidth( 0 ) );
    //m_listView->setMaximumWidth( m_listView->sizeHint().width() );
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );
    m_listView->setMaximumHeight( 1600 );

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

FuelExportDialog::~FuelExportDialog( void )
{
    delete m_listView;      m_listView = 0;
    delete m_gridLayout;    m_gridLayout = 0;
    delete m_gridFrame;     m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void FuelExportDialog::contextMenuActivated( int id )
{
    // Restore button to its previous selection state
    m_listView->setSelected( m_lvi, ! m_listView->isSelected( m_lvi ) );

    if ( id == ContextOk )
    {
        // Set m_lvi to 0 to let rightButtonClicked() know we're done with
        // the dialog
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
    else if ( id == ContextViewParameters )
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
/*! \brief Displays the contents of the fuel model
 *  in an info dialog containing an HTML table of parameters.
 *
 *  \return Returns TRUE on success or FALSE on failure
 */

bool FuelExportDialog::displayContents( QListViewItem *lvi )
{
    // If a folder was clicked, then edit its contents.
    QString msg( "" );
    QString title( "" );
    QString name( lvi->text(1) );

    // Get the FuelModel pointer.
    FuelModel *fm =
        m_bp->m_eqApp->m_fuelModelList->fuelModelByModelName( name );
    if ( ! fm )
    // This code block should never be executed!
    {
        translate( title, "GuideDialog:FuelModel:NotFound", name );
        error( title );
        return( false );
    }
    translate( title, "GuideDialog:FuelModel", name );
    fm->formatHtmlTable( title, msg );
    // Display the contents and return.
    info( msg );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Callback for mouse double-click on a discrete variable item,
 *  signaling that the user wants to select just this item.
 */

void FuelExportDialog::itemDoubleClicked( QListViewItem *item )
{
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
 *  Returns the number of entry tokens in the string.
 */

int FuelExportDialog::resultString( QString &qStr )
{
    qStr = m_result;
    return( m_results );
}

//------------------------------------------------------------------------------
/*! \brief Displays the parameters of the clicked fuel model or
 *  moisture scenario.
 */

void FuelExportDialog::rightButtonClicked( QListViewItem *lvi,
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
        translate( text, "GuideDialog:ContextMenu:Ok" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextOk );

        translate( text, "GuideDialog:ContextMenu:Select" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextSelect );

        translate( text, "GuideDialog:ContextMenu:Deselect" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextDeselect );

        translate( text, "GuideDialog:ContextMenu:ViewParameters" );
        mid = m_contextMenu->insertItem( text,
               this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextViewParameters );
    }

    translate( text, "GuideDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "GuideDialog:ContextMenu:PrintEntire" );
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
/*! \brief Accept button callback that validates and stores the input entries.
 */

void FuelExportDialog::store( void )
{
    // Initialize results
    m_result = "";
    m_results = 0;
    QListViewItem *item = m_listView->firstChild();
    if ( item )
    {
        do {
            if ( item->isSelected() )
            {
                if ( m_results )
                {
                    m_result.append( ", " );
                }
                m_result.append( item->text( 1 ) );
                m_results++;
            }
        } while ( ( item = item->itemBelow() ) != 0 );
    }
    // Everything is just OK!
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of fuelexportdialog.cpp
//------------------------------------------------------------------------------

