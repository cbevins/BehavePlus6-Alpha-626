//------------------------------------------------------------------------------
/*! \file rundialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument run time dialog class methods.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "property.h"
#include "rundialog.h"
#include "textview.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qcheckbox.h>
#include <qfontmetrics.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qvbuttongroup.h>

//------------------------------------------------------------------------------
/*! \brief RunDialog constructor.
 *
 *  \return The function returns nothing.
 */

RunDialog::RunDialog( BpDocument *bp, const char *name ) :
    AppDialog( bp,
        "RunDialog:Caption",
        "StealingFire.png",
        "Stealing Fire",
        "",                         // No HelpBrowser
        name ),
    m_bp(bp),
    m_textView(0),
    m_graphButtonGroup(0),
    m_graphsCheckBox(0),
    m_graphsVar1(0),
    m_graphsVar2(0),
    m_tableButtonGroup(0),
    m_tablesCheckBox(0),
    m_tablesVar1(0),
    m_tablesVar2(0),
    m_graphLimitsCheckBox(0)
{
    // Add a text view to the main area
    m_textView = new TextView( m_page, "m_textView" );
    checkmem( __FILE__, __LINE__, m_textView, "TextView m_textView", 1 );
    m_textView->setTextFormat( Qt::RichText );

    // Create the tables checkbox
    QString text("");
    translate( text, "RunDialog:Tables:Checkbox" );
    m_tablesCheckBox = new QCheckBox( text, contentFrame(), "m_tablesCheckBox" );
    Q_CHECK_PTR( m_tablesCheckBox );
    m_tablesCheckBox->setChecked( m_bp->property()->boolean( "tableActive" ) );

    // If only ONE range variable, just display the text
    translate( text, "RunDialog:Tables:Caption" );
    QString html( "<H3><FONT COLOR=\"#ff5500\">" + text + "</FONT></H3><HR>" );
    if ( m_bp->m_eqTree->m_rangeCase <= 3 )
    {
        translate( text, "RunDialog:Tables:Range1",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label) ) ;
        html += text;
    }
    // If TWO range variables, ask user which one is the row variable.
    else
    {
        translate( text, "RunDialog:Tables:Range2",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label),
            *(m_bp->m_eqTree->m_rangeVar[1]->m_label) ) ;
        html += text;

        // Create the tables button group
        translate( text, "RunDialog:Tables:ButtonGroup" );
        m_tableButtonGroup = new QVButtonGroup( text, contentFrame(),
            "tableButtonGroup" );
        Q_CHECK_PTR( m_tableButtonGroup );
        m_tableButtonGroup->setAlignment( AlignHCenter );

        // If tableRowUseVar2 is NOT checked,
        // then tableRowUseVar1 MUST be checked.
        if ( ! m_bp->property()->boolean( "tableRowUseVar2" ) )
        {
            m_bp->property()->boolean( "tableRowUseVar1", true );
        }
        m_tablesVar1 = new QRadioButton(
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label),
            m_tableButtonGroup, "m_tablesVar1" ) ;
        Q_CHECK_PTR( m_tablesVar1 );
        m_tablesVar1->setChecked( m_bp->property()->boolean( "tableRowUseVar1" ) );

        m_tablesVar2 = new QRadioButton(
            *(m_bp->m_eqTree->m_rangeVar[1]->m_label),
            m_tableButtonGroup, "m_tablesVar2" ) ;
        Q_CHECK_PTR( m_tablesVar2 );
        m_tablesVar2->setChecked( m_bp->property()->boolean( "tableRowUseVar2" ) );

        m_tableButtonGroup->setFixedHeight(
            m_tableButtonGroup->sizeHint().height() );
    }

    // Create the graphs checkbox
    translate( text, "RunDialog:Graphs:Checkbox" );
    m_graphsCheckBox = new QCheckBox( text, contentFrame(), "m_graphsCheckBox" );
    Q_CHECK_PTR( m_graphsCheckBox );
    m_graphsCheckBox->setChecked( m_bp->property()->boolean( "graphActive" ) );

    translate( text, "RunDialog:Graphs:Caption" );
    html += "<P><H3><FONT COLOR=\"#ff5500\">" + text + "</FONT></H3><HR>";

    // Tell the user what will be drawn.
    // Case 1: 0 continuous and 0 discrete range vars
    // Case 2: 1 continuous and 0 discrete range vars
	if ( m_bp->m_eqTree->m_rangeCase == 2 )
    {
        translate( text, "RunDialog:Graphs:Case2",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label) ) ;
        html += text;
    }
    // Case 3: 0 continuous and 1 discrete range vars
    else if ( m_bp->m_eqTree->m_rangeCase == 3 )
    {
        translate( text, "RunDialog:Graphs:Case3",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label) ) ;
        html += text;
    }
    // Case 4: 2 continuous & 0 discrete range vars
    else if ( m_bp->m_eqTree->m_rangeCase == 4 )
    {
        translate( text, "RunDialog:Graphs:Case4",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label),
            *(m_bp->m_eqTree->m_rangeVar[1]->m_label) ) ;
        html += text;
    }
    // Case 5: 0 continuous & 2 discrete range vars
    else if ( m_bp->m_eqTree->m_rangeCase == 5 )
    {
        translate( text, "RunDialog:Graphs:Case5",
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label),
            *(m_bp->m_eqTree->m_rangeVar[1]->m_label) ) ;
        m_graphsCheckBox->setEnabled( false );
        html += text;
    }
    // Case 6: 1 continuous & 1 discrete range vars
    else if ( m_bp->m_eqTree->m_rangeCase == 6 )
    {
        if ( m_bp->m_eqTree->m_rangeVar[0]->isContinuous() )
		{
			translate( text, "RunDialog:Graphs:Case6",
                *(m_bp->m_eqTree->m_rangeVar[0]->m_label),
                *(m_bp->m_eqTree->m_rangeVar[1]->m_label) );
		}
		else
		{
			translate( text, "RunDialog:Graphs:Case6",
                *(m_bp->m_eqTree->m_rangeVar[1]->m_label),
                *(m_bp->m_eqTree->m_rangeVar[0]->m_label) );
		}
        html += text;
    }
    // Give the user a choice between the two continuous variables.
    if ( m_bp->m_eqTree->m_rangeCase == 4 )
    {
        // Create the graphs button group
        translate( text, "RunDialog:Graphs:ButtonGroup" );
        m_graphButtonGroup = new QVButtonGroup( text, contentFrame(),
            "graphButtonGroup" );
        Q_CHECK_PTR( m_graphButtonGroup );
        m_graphButtonGroup->setAlignment( AlignHCenter );

        // If graphXVar2 is not checked,
        // then graphXVar1 MUST be checked.
        if ( ! m_bp->property()->boolean( "graphXVar2" ) )
        {
            m_bp->property()->boolean( "graphXVar1", true );
        }
        m_graphsVar1 = new QRadioButton(
            *(m_bp->m_eqTree->m_rangeVar[0]->m_label), m_graphButtonGroup ) ;
        Q_CHECK_PTR( m_graphsVar1 );
        m_graphsVar1->setChecked( m_bp->property()->boolean( "graphXVar1" ) );

        m_graphsVar2 = new QRadioButton(
            *(m_bp->m_eqTree->m_rangeVar[1]->m_label), m_graphButtonGroup ) ;
        Q_CHECK_PTR( m_graphsVar2 );
        m_graphsVar2->setChecked( m_bp->property()->boolean( "graphXVar2" ) );

        m_graphButtonGroup->setFixedHeight(
            m_graphButtonGroup->sizeHint().height() );
    }

    // Create the graph limits checkbox
    if ( m_bp->m_eqTree->m_rangeCase >= 2
      && m_bp->m_eqTree->m_rangeCase <= 6 )
    {
        translate( text, "RunDialog:GraphLimits:Checkbox" );
        m_graphLimitsCheckBox = new QCheckBox( text, contentFrame(),
            "m_graphLimitsCheckBox" );
        Q_CHECK_PTR( m_graphLimitsCheckBox );
        m_graphLimitsCheckBox->setChecked(
            m_bp->property()->boolean( "graphYUserRange" ) );

        translate( text, "RunDialog:GraphLimits:Caption" );
        html += "<P><H3><FONT COLOR=\"#ff5500\">" + text + "</FONT></H3><HR>";

        translate( text, "RunDialog:GraphLimits:Text" );
        html += text;
    }
    contentFrame()->setMinimumWidth( contentFrame()->sizeHint().width() );

    // Set the text
    m_textView->setText( html );
    m_textView->setReadOnly( true );
    m_textView->setMinimumWidth( 350 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief RunDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

RunDialog::~RunDialog( void )
{
    delete m_textView;              m_textView = 0;

    delete m_graphLimitsCheckBox;   m_graphLimitsCheckBox = 0;
    delete m_graphsVar1;            m_graphsVar1 = 0;
    delete m_graphsVar2;            m_graphsVar2 = 0;
    delete m_graphButtonGroup;      m_graphButtonGroup = 0;
    delete m_graphsCheckBox;        m_graphsCheckBox = 0;

    delete m_tablesVar1;            m_tablesVar1 = 0;
    delete m_tablesVar2;            m_tablesVar2 = 0;
    delete m_tableButtonGroup;      m_tableButtonGroup = 0;
    delete m_tablesCheckBox;        m_tablesCheckBox = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stores the run dialog settings into the document state variables.
  * Usually called by the same function that created the RunDialog,
  * if it returns Accepted, and before it is destroyed.
 */

void RunDialog::store( void )
{
    m_bp->property()->boolean( "tableActive", m_tablesCheckBox->isChecked() );
    m_bp->property()->boolean( "graphActive", m_graphsCheckBox->isChecked() );
    m_bp->property()->boolean( "graphYUserRange",
        m_graphLimitsCheckBox->isChecked() );

    if ( m_bp->m_eqTree->m_rangeCase > 3 )
    {
        m_bp->property()->boolean( "tableRowUseVar1", m_tablesVar1->isChecked() );
        m_bp->property()->boolean( "tableRowUseVar2", m_tablesVar2->isChecked() );
        if ( m_bp->m_eqTree->m_rangeCase == 4 )
        {
            m_bp->property()->boolean( "graphXVar1", m_graphsVar1->isChecked() );
            m_bp->property()->boolean( "graphXVar2", m_graphsVar2->isChecked() );
        }
    }
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of rundialog.cpp
//------------------------------------------------------------------------------

