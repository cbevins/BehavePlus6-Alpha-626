//------------------------------------------------------------------------------
/*! \file guidedialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument worksheet guide button dialog.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "fuelmodel.h"
#include "fuelwizards.h"
#include "guidedialog.h"
#include "moisscenario.h"
#include "standardwizards.h"
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

/*! \var FieldKey[]
 *  \brief Array of translator keys to the Guide Dialog From, Thru, and Step
 *  entry field labels.
 */

static const QString FieldKey[] =
{
    "GuideDialog:From",
    "GuideDialog:Thru",
    "GuideDialog:Step"
};
static QString FieldName[] = { "From", "Thru", "Step" };

//------------------------------------------------------------------------------
/*! \brief Displays a Guide dialog appropriate to the variable.
 *
 *  \param bp   Pointer to the parent BpDocument.
 *  \param lid  Leaf variable index.  A pointer to the leaf variable is
 *              returned by a call to leaf( lid ).
 */

GuideDialog::GuideDialog( BpDocument *bp, int lid, const char *name ) :
    AppDialog( bp,
        "GuideDialog:Caption",
        "NorthernLights.png",
        "NorthernLights",
        //QString( (bp->leaf( lid ))->m_name + ".html" ),
        (bp->leaf( lid ))->m_help,
        name,
        "GuideDialog:Ok",
        "GuideDialog:Cancel",
        "GuideDialog:Clear",
        (bp->leaf( lid ))->m_wizard.isEmpty()
            ? ""
            : "GuideDialog:Choices" ),
    m_bp(bp),
    m_var(bp->leaf( lid )),
    m_gridFrame(0),
    m_gridLayout(0),
    m_rangeLabel(0),
    m_listView(0),
    m_result(""),
    m_results(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Initialize member data
    int id, iid;
    for ( id = 0;
          id < 3;
          id++ )
    {
        m_blank[id] = true;
        m_lbl[id] = 0;
        m_entry[id] = 0;
        m_value[id] = 0.0;
        translate( FieldName[id], FieldKey[id] );
    }

    //--------------------------------------------------------------------------
    // Continuous variables get From, Thru, and Step input fields
    //--------------------------------------------------------------------------
    if ( m_var->isContinuous() )
    {
        // Hidden frame to contain a grid layout
        m_gridFrame = new QFrame( contentFrame(), "m_gridFrame" );
        Q_CHECK_PTR( m_gridFrame );
        m_gridFrame->setFrameStyle( QFrame::NoFrame );

        // Create the label-entry grid layout
        m_gridLayout = new QGridLayout( m_gridFrame, 10, 2, 1, 5,
            "m_gridLayout" ) ;
        Q_CHECK_PTR( m_gridLayout );

        // Display the variable's valid range
        QString maxString;
        QString units =  m_var->displayUnits();
        maxString.sprintf( "(%1.*f - %1.*f %s)",
            m_var->m_displayDecimals, m_var->m_displayMinimum,
            m_var->m_displayDecimals, m_var->m_displayMaximum,
            units.latin1() );

        // Display the valid range in a label
        m_rangeLabel = new QLabel( maxString, m_gridFrame, "m_rangeLabel" );
        Q_CHECK_PTR( m_rangeLabel );
        m_gridLayout->addWidget( m_rangeLabel, 3, 1, AlignLeft );

        // Display the From, Thru, and Step labels and entries
        for ( int i = 0;
              i < 3;
              i++ )
        {
            m_lbl[i] = new QLabel( FieldName[i], m_gridFrame );
            Q_CHECK_PTR( m_lbl[i] );
            m_gridLayout->addWidget( m_lbl[i], i+4, 0, AlignLeft );

            m_entry[i] = new QLineEdit( m_gridFrame );
            Q_CHECK_PTR( m_entry[i] );
            m_entry[i]->setFixedWidth( m_rangeLabel->sizeHint().width() );
            m_gridLayout->addWidget( m_entry[i], i+4, 1, AlignLeft );
        }
        m_entry[0]->setFocus();
        // Set tab order
        setTabOrder( m_entry[2], m_acceptBtn );
    }
    //--------------------------------------------------------------------------
    // Discrete variable get an item selection list
    //--------------------------------------------------------------------------
    else if ( m_var->isDiscrete() )
    {
        // Determine if the m_sort and m_name are the same or not
        bool showSortCol = false;
        for ( iid = 0;
              iid < (int) m_var->m_itemList->count();
              iid++ )
        {
           if ( m_var->m_itemList->itemSort( iid )
            !=  m_var->m_itemList->itemName( iid ) )
            {
                showSortCol = true;
                break;
            }
        }
        QString text("");
        m_listView = new QListView( contentFrame(), "listView" );
        Q_CHECK_PTR( m_listView );
        translate( text, "GuideDialog:Col0" );
        m_listView->addColumn( text );
        translate( text, "GuideDialog:Col1" );
        m_listView->addColumn( text );
        translate( text, "GuideDialog:Col2" );
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
        QString desc;
        for ( iid = 0;
              iid < (int) m_var->m_itemList->count();
              iid++ )
        {
            desc = *( m_var->m_itemList->itemDesc( iid ) );
            item = new QListViewItem( m_listView,
                m_var->m_itemList->itemSort( iid ),
                m_var->m_itemList->itemName( iid ),
                desc );
            Q_CHECK_PTR( item );
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
        // Set tab order
        m_listView->setFocus();
        setTabOrder( m_listView, m_acceptBtn );
    }
    //--------------------------------------------------------------------------
    // Text variable input guide
    //--------------------------------------------------------------------------
    else if ( m_var->isText() )
    {
        /* NOTHING */ ;
    }
    // Set remaining tab order
    setTabOrder( m_acceptBtn, m_clearBtn );
    if ( m_wizardBtn )
    {
        setTabOrder( m_clearBtn, m_wizardBtn );
        setTabOrder( m_wizardBtn, m_rejectBtn );
    }
    else
    {
        setTabOrder( m_clearBtn, m_rejectBtn );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

GuideDialog::~GuideDialog( void )
{
    for ( int i = 0;
          i < 3;
          i++ )
    {
        delete m_lbl[i];    m_lbl[i] = 0;
        delete m_entry[i];  m_entry[i] = 0;
    }
    delete m_rangeLabel;    m_rangeLabel = 0;
    delete m_listView;      m_listView = 0;
    delete m_gridLayout;    m_gridLayout = 0;
    delete m_gridFrame;     m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback for the Clear button.
 *  Merely clears the \a m_results string and \a m_result code.
 */

void GuideDialog::clear( void )
{
    m_result = "";
    m_results = 0;
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void GuideDialog::contextMenuActivated( int id )
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
/*! \brief Displays the contents of the fuel model or moisture scenario
 *  file in an info dialog containing an HTML table of parameters.
 *
 *  \return Returns TRUE on success or FALSE on failure
 */

bool GuideDialog::displayContents( QListViewItem *lvi )
{
    // If a folder was clicked, then edit its contents.
    QString msg( "" );
    QString title( "" );
    QFileInfo fi( lvi->text(0) );
    QString name( lvi->text(1) );

    // Fuel model contents
    if ( m_var->m_name == "vSurfaceFuelBedModel" )
    {
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
    }
    // Moisture scenario contents
    else if ( m_var->m_name == "vSurfaceFuelMoisScenario" )
    {
        // Get the FuelMoisScenario pointer
        MoisScenario *ms =
            m_bp->m_eqApp->m_moisScenarioList->moisScenarioByScenarioName( name );
        if ( ! ms )
        // This code block should never be executed!
        {
            translate( title, "GuideDialog:MoisScenario:NotFound", name );
            error( title );
            return( false );
        }
        translate( title, "GuideDialog:MoisScenario", name );
        ms->formatHtmlTable( title, msg );
    }
    // Unknown file type
    // This code block should never be executed!
    else
    {
        translate( title, "GuideDialog:UnknownType",
            lvi->text( 1 ), *(m_var->m_label) );
        error( title );
        return( false );
    }
    // Display the contents and return.
    info( msg );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Callback for mouse double-click on a discrete variable item,
 *  signaling that the user wants to select just this item.
 */

void GuideDialog::itemDoubleClicked( QListViewItem *item )
{
    if ( item )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \bried Provides keyboard traversal for the range inputs.
 */

void GuideDialog::keyPressEvent( QKeyEvent *e )
{
    const int rangeNext[3] = { 1, 2, 0 };
    const int rangePrev[3] = { 2, 0, 1 };

    // This only applies to the continuous variable range entries
    if ( ! m_var->isContinuous() )
    {
        return;
    }
    // Find out who has the focus
    int focus = 0;
    for ( int i = 0;
          i < 3;
          i++ )
    {
        if ( m_entry[i]->hasFocus() )
        {
            focus = i;
            break;
        }
    }
    // Process navigation keys.
    if ( e->key() == Key_Up )
    {
        focus = rangePrev[focus];
        m_entry[focus]->setFocus();
        m_entry[focus]->home( false );
        e->accept();
    }
    else if ( e->key() == Key_Down || e->key() == Key_Return )
    {
        focus = rangeNext[focus];
        m_entry[focus]->setFocus();
        m_entry[focus]->home( false );
        e->accept();
    }
    else
    {
        e->ignore();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief User access to the resulting entry value.
 *
 *  \return On return the \a qStr has the new entry string.
 *  Returns the number of entry tokens in the string.
 */

int GuideDialog::resultString( QString &qStr )
{
    qStr = m_result;
    return( m_results );
}

//------------------------------------------------------------------------------
/*! \brief Displays the parameters of the clicked fuel model or
 *  moisture scenario.
 */

void GuideDialog::rightButtonClicked( QListViewItem *lvi,
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

        if ( m_var->m_name == "vSurfaceFuelBedModel"
          || m_var->m_name == "vSurfaceFuelMoisScenario" )
        {
            translate( text, "GuideDialog:ContextMenu:ViewParameters" );
            mid = m_contextMenu->insertItem( text,
                   this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextViewParameters );
        }
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

void GuideDialog::store( void )
{
    // Initialize results
    m_result = "";
    m_results = 0;
    // Construct the discrete variable item list
    if ( m_var->isDiscrete() )
    {
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
    }
    // Construct the continuous variable value list
    else if ( m_var->isContinuous() )
    {
        // First validate the range
        if ( ! validRange() )
        {
            return;
        }
        // If all entires are blank, then nothing is stored
        if ( m_blank[0] && m_blank[1] && m_blank[2] )
        {
            reject();
            return;
        }
        // Adjust boundaries for wrap-around variables
        if ( m_var->m_isWrap )
        {
            // From == Thru
            if ( ( m_value[0] == m_value[1] && ! m_blank[1] )
              || ( m_value[0] == m_var->m_displayMinimum
                && m_value[1] == m_var->m_displayMaximum )
              || ( m_value[0] == m_var->m_displayMaximum
                && m_value[1] == m_var->m_displayMinimum ) )
            {
                m_value[1] = m_value[0];
                if ( m_value[2] > 0. )
                {
                    m_value[1] += m_var->m_displayMaximum;
                }
                else if ( m_value[2] < 0. )
                {
                    m_value[0] += m_var->m_displayMaximum;
                }
            }
            // From < Thru
            else if ( m_value[0] < m_value[1] && m_value[2] < 0. )
            {
                m_value[0] += m_var->m_displayMaximum;
            }
            // From > Thru
            else if ( m_value[0] > m_value[1] && m_value[2] > 0. )
            {
                m_value[1] += m_var->m_displayMaximum;
            }
        }
        // Build the value list string
        double d;
        // If the FROM field is less than or equal to the THRU field
        if ( m_value[0] <= m_value[1] )
        {
            while ( ( d = m_value[0] + m_results * m_value[2] ) <= m_value[1] )
            {
                storeContinuous( d );
            }
        }
        // If the FROM field is greater than the THRU field
        else if ( m_value[0] > m_value[1] )
        {
            while ( ( d = m_value[0] + m_results * m_value[2] ) >= m_value[1] )
            {
                storeContinuous( d );
            }
        }
    }
    // Everything is just OK!
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the continuous variable value \a d to the \a m_result string.
 *
 *  Called only by store().
 */

void GuideDialog::storeContinuous( double d )
{
    // Reset wrap-around variables to original range
    double d2 = d;
    if ( m_var->m_isWrap )
    {
        while ( d2 > m_var->m_displayMaximum )
        {
            d2 -= m_var->m_displayMaximum;
        }
    }
    // Add this to the result string
    QString qStr("");

	// CDB - DECIMALS MOD
	if ( false )
	{
		qStr.sprintf( "%s%1.*f",
			( m_results )
				? ", "
	            : "",
		    m_var->m_displayDecimals,
			d2 );
	}
	else
	{
		// First add a decimal point and 6 decimal places
		qStr.sprintf( "%s%1.6f", ( m_results ) ? ", " : "", d2 );
		// Second, remove trailing zeros
		while ( qStr.endsWith( "0" ) )
		{
			qStr = qStr.left( qStr.length()-1 );
		}
		// Third, remove any trailing decimal point
		if ( qStr.endsWith( "." ) )
		{
			qStr = qStr.left( qStr.length()-1 );
		}
	}
    m_result.append( qStr );
    m_results++;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Performs validation of the range inputs.
 *
 *  \return TRUE if valid, FALSE if invalid.
 */

bool GuideDialog::validRange( void )
{
    // We're gonna need these
    bool ok;
    QString caption("");
    QString text("");
    // Make sure entries are either blank or a valid in-range floating point
    for ( int i = 0;
          i < 3;
          i++ )
    {
        // Check if the entry is empty
        QString qStr = m_entry[i]->text().stripWhiteSpace();
        m_blank[i] = qStr.isEmpty();
        if ( m_blank[i] )
        {
            m_value[i] = 0.;
            continue;
        }
        // Check if this is a valid double.
        m_value[i] = qStr.toDouble( &ok );
        if ( ! ok )
        {
            translate( caption, "GuideDialog:InvalidInput:Caption" );
            translate( text, "GuideDialog:InvalidInput:FloatingPoint",
                FieldName[i], qStr );
            error( caption, text );
            m_entry[i]->setFocus();
            m_entry[i]->home( false );
            return( false );
        }
        // Check min and max.
        if ( i < 2 )
        {
            if ( ! m_var->isValidRange( m_value[i] ) )
            {
                QString qRange("");
                qRange.sprintf( "%1.*f - %1.*f %s",
                    m_var->m_displayDecimals, m_var->m_displayMinimum,
                    m_var->m_displayDecimals, m_var->m_displayMaximum,
                    m_var->m_displayUnits.latin1() );
                translate( caption, "GuideDialog:InvalidInput:Caption" );
                translate( text, "GuideDialog:InvalidInput:OutsideRange",
                    FieldName[i], qStr, qRange );
                error( caption, text );
                m_entry[i]->setFocus();
                m_entry[i]->home( false );
                return( false );
            }
        }
    }
    // Assign default values depending upon which fields are blank.
    if ( m_blank[0] )
    {
        if ( m_blank[1] )
        {
            // Blank FROM, blank THRU, blank STEP
            if ( m_blank[2] )
            {
                return( true );
            }
            // Blank FROM, blank THRU, text STEP
            else if ( ! m_blank[2] )
            {
                translate( caption, "GuideDialog:SuperfluousInput:Caption" );
                translate( text, "GuideDialog:SuperfluousInput:Text",
                    FieldName[2], FieldName[1] );
                warn( caption, text );
                m_entry[1]->setFocus();
                m_entry[1]->home( false );
                return( false );
            }
        }
        else if ( ! m_blank[1] )
        {
            // Blank FROM, text THRU, blank STEP
            if ( m_blank[2] )
            {
                translate( caption, "GuideDialog:SuperfluousInput:Caption" );
                translate( text, "GuideDialog:SuperfluousInput:Text",
                    FieldName[1], FieldName[0] );
                warn( caption, text );
                m_entry[0]->setFocus();
                m_entry[0]->home( false );
                return( false );
            }
            // Blank FROM, text THRU, text STEP
            else if ( ! m_blank[2] )
            {
                translate( caption, "GuideDialog:SuperfluousInput:Caption" );
                translate( text, "GuideDialog:SuperfluousInput:Text",
                    FieldName[1], FieldName[0] );
                warn( caption, text );
                m_entry[0]->setFocus();
                m_entry[0]->home( false );
                return( false );
            }
        }
    }
    else if ( ! m_blank[0] )
    {
        if ( m_blank[1] )
        {
            // Text FROM, blank THRU, blank STEP
            if ( m_blank[2] )
            {
                m_value[1] = m_value[0];
                m_value[2] = 1.;
                return( true );     // One value only
            }
            // Text FROM, blank THRU, text STEP
            else if ( ! m_blank[2] )
            {
                translate( caption, "GuideDialog:SuperfluousInput:Caption" );
                translate( text, "GuideDialog:SuperfluousInput:Text",
                    FieldName[2], FieldName[1] );
                warn( caption, text );
                m_entry[1]->setFocus();
                m_entry[1]->home( false );
                return( false );
            }
        }
        else if ( ! m_blank[1] )
        {
            // Text FROM, text THRU, blank STEP
            if ( m_blank[2] )
            {
                m_value[2] = m_value[1] - m_value[0];
                // Prevent a step size of zero which leads to an infinite loop
                if ( fabs( m_value[2] ) < 0.000001 )
                {
                    m_value[2] = m_value[1];
                }
                return( true );     // Either one or two values
            }
            // Text FROM, text THRU, text STEP
            else if ( ! m_blank[2] )
            {
                // Prevent infinite steps
                if ( fabs( m_value[2] ) < 0.000001 )
                {
                    translate( caption, "GuideDialog:ZeroStepSize:Caption" );
                    translate( text, "GuideDialog:ZeroStepSize:Text" );
                    warn( caption, text );
                    return( false );
                }
                // If not degrees (which allows for wrap-around)...
                if ( fabs( m_var->m_displayMinimum ) > 0.00001
                 || fabs( 360. - m_var->m_displayMaximum ) > 0.00001 )
                {
                    // Make sure from and thru can meet via step
                    if ( ( m_value[0] < m_value[1] && m_value[2] < 0.000001 )
                      || ( m_value[0] > m_value[1] && m_value[2] > -0.000001 ) )
                    {
                        translate( caption, "GuideDialog:BadStepSizeDir:Caption" );
                        translate( text, "GuideDialog:BadStepSizeDir:Text" );
                        warn( caption, text );
                        return( false );
                    }
                }
                // Check steps limit
                double steps = ( m_value[1] - m_value[0] ) / m_value[2];
                if ( fabs( steps ) > 1000. )
                {
                    translate( caption, "GuideDialog:TooManyValues:Caption" );
                    translate( text, "GuideDialog:TooManyValues:Text",
                        QString( "%1" ).arg( (int) steps ), "1000" );
                    warn( caption, text );
                    return( false );
                }
                return( true );
            }
        }
    }
    // This code block should never be executed!
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Wizard button callback.
 */

void GuideDialog::wizard( void )
{
    // The the wizard name and make sure its valid
    QString wizardId = m_var->m_wizard;
    if ( wizardId.isNull() || wizardId.isEmpty() )
    // This code block should never be executed!
    {
        reject();
        return;
    }
    // Hide the GuideDialog
    hide();
    m_result = "";
    m_results = 0;

    // Construct the appropriate wizard
    WizardDialog *wizard = 0;
    if ( wizardId == "barkThicknessWizard" )
    {
        wizard = new BarkThicknessWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "canopyBulkDensityWizard" )
    {
        wizard = new CanopyBulkDensityWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "compassNorthWizard" )
    {
        wizard = new CompassNorthWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "compassUpslopeWizard" )
    {
        wizard = new CompassUpslopeWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "crownRatioWizard" )
    {
        wizard = new CrownRatioWizard( m_bp, m_var, wizardId );
    }
    //else if ( wizardId == "fireLineIntWizard" )
    //{
    //    wizard = new FireLineIntWizard( m_bp, m_var, wizardId );
    //}
    else if ( wizardId == "fuelBedDepthWizard" )
    {
        wizard = new FuelBedDepthWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelBedMextDeadWizard" )
    {
        wizard = new FuelBedMextDeadWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelHeatWizard" )
    {
        wizard = new FuelHeatWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelLoadDead1Wizard" )
    {
        wizard = new FuelLoadDead1Wizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelLoadDead10Wizard" )
    {
        wizard = new FuelLoadDead10Wizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelLoadDead100Wizard" )
    {
        wizard = new FuelLoadDead100Wizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelLoadLiveWizard" )
    {
        wizard = new FuelLoadLiveWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelSavrDead1Wizard" )
    {
        wizard = new FuelSavrDead1Wizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "fuelSavrLiveWizard" )
    {
        wizard = new FuelSavrLiveWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "heatPerUnitAreaWizard" )
    {
        wizard = new HeatPerUnitAreaWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "liveHerbMoistureWizard" )
    {
        wizard = new LiveHerbMoistureWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "liveWoodMoistureWizard" )
    {
        wizard = new LiveWoodMoistureWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "palmettoAgeWizard" )
    {
        wizard = new PalmettoAgeWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "palmettoCoverWizard" )
    {
        wizard = new PalmettoCoverWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "palmettoOverstoryBasalAreaWizard" )
    {
        wizard = new PalmettoOverstoryBasalAreaWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "mapFractionWizard" )
    {
        wizard = new MapFractionWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "safetyZoneEquipmentAreaWizard" )
    {
        wizard = new SafetyZoneEquipmentAreaWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "safetyZonePersonnelAreaWizard" )
    {
        wizard = new SafetyZonePersonnelAreaWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "slopeDegreesWizard" )
    {
        wizard = new SlopeDegreesWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "slopeFractionWizard" )
    {
        wizard = new SlopeFractionWizard( m_bp, m_var, wizardId );
    }
    else if ( wizardId == "windAdjWizard" )
    {
        wizard = new WindAdjWizard( m_bp, m_var, wizardId );
    }
    // Check the allocation
    checkmem( __FILE__, __LINE__, wizard, "WizardDialog wizard", 1 );
    // Show the wizard
    if ( wizard->exec() != QDialog::Accepted )
    {
        // Rejected, so redisplay the Guide dialog
        reject();
        show();
    }
    else
    {
        m_results = wizard->resultString( m_result );
        accept();
    }
    // Free resources and return
    delete wizard;  wizard = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of guidedialog.cpp
//------------------------------------------------------------------------------

