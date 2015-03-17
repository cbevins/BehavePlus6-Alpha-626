//------------------------------------------------------------------------------
/*! \file horizontaldistancedialog.cpp
 *  \version BehavePlus4
 *  \author Copyright (C) 2002-2006 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus ground vs horizontal map distance tool.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "horizontaldistancedialog.h"

// Qt include files
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

// Standard include files
#include <cmath>

/*! \var FieldNameKey
 *  \brief Array of translator keys for the 2 input entry field labels,
 *  Calculate button, and 8 initial results text.  The translated results are
 *  stored in the FieldName[] array.
 */
static QString FieldName[11] =  { "", "", "", "", "", "", "", "", "", "", "" };
static const QString FieldNameKey[11] =
{
    "HorizontalDistanceDialog:Field:MapDistance",
    "HorizontalDistanceDialog:Field:SlopeSteepness",
    "HorizontalDistanceDialog:Field:SlopeDegrees",
    "HorizontalDistanceDialog:Field:Result00",
    "HorizontalDistanceDialog:Field:Result15",
    "HorizontalDistanceDialog:Field:Result30",
    "HorizontalDistanceDialog:Field:Result45",
    "HorizontalDistanceDialog:Field:Result60",
    "HorizontalDistanceDialog:Field:Result75",
    "HorizontalDistanceDialog:Field:Result90",
    "HorizontalDistanceDialog:Field:Calculate"
};

//------------------------------------------------------------------------------
/*! \brief HorizontalDistanceDialog constructor.
 */

HorizontalDistanceDialog::HorizontalDistanceDialog( QWidget *parent, const char *name ) :
    AppDialog( parent, "HorizontalDistanceDialog:Caption",
        "WatchableWildlife.png", "Watchable Wildlife",
        "horizontalDistance.html", name, "HorizontalDistanceDialog:Button:Dismiss", 0 ),
    m_gridFrame(0),
    m_gridLayout(0),
    m_convertButton(0)
{
    // Initialization
    int row;
    for ( row = 0;
          row < 10;
          row++ )
    {
        translate( FieldName[row], FieldNameKey[row] );
        m_entry[row] = 0;
        m_lbl[row]   = 0;
    }
    translate( FieldName[10], FieldNameKey[10] );

    // Hidden frame to contain a grid layout
    m_gridFrame = new QFrame( m_page->m_contentFrame, "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::NoFrame );

    // Create the label-entry grid layout
    m_gridLayout = new QGridLayout( m_gridFrame, 11, 2, 0, 2, "m_gridLayout" ) ;
    Q_CHECK_PTR( m_gridLayout );

    // Top portion contains entry fields and labels
    int id = 0;
    for ( row = 0;
          row < 11;
          row++ )
    {
        // Label/entry row
        if ( row != 2 )
        {
            // Label goes in first column
            m_lbl[id] = new QLabel( FieldName[id], m_gridFrame );
            Q_CHECK_PTR( m_lbl[id] );
            m_lbl[id]->setFixedSize( m_lbl[id]->sizeHint() );
            m_gridLayout->addWidget( m_lbl[id], row, 0, AlignLeft );
            // Entry field goes in second column
            m_entry[id] = new QLineEdit( m_gridFrame );
            Q_CHECK_PTR( m_entry[id] );
            m_gridLayout->addWidget( m_entry[id], row, 1 );
            m_entry[id]->setMinimumWidth( 60 );
            if ( row > 2 )
            {
                m_entry[id]->setReadOnly( true );
            }
            // Next entry
            id++;
        }
        // Button row
        else
        {
            m_convertButton = new QPushButton( FieldName[10],
                m_gridFrame, "m_convertButton" );
            Q_CHECK_PTR( m_convertButton );
            m_gridLayout->addWidget( m_convertButton, row, 1 );
            connect( m_convertButton, SIGNAL( clicked() ),
                     this,            SLOT( convert() ) );
        }
    }
    m_entry[0]->setFocus();

    // Do not allow the middle pane to change size, so that if the
    // dialog is maximized the help pane gets all the new space.
    m_page->m_midFrame->setFixedWidth(
        m_page->m_midFrame->sizeHint().width() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief HorizontalDistanceDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

HorizontalDistanceDialog::~HorizontalDistanceDialog( void )
{
    int row;
    for ( row = 0;
          row < 10;
          row++ )
    {
        delete m_entry[row];    m_entry[row] = 0;
        delete m_lbl[row];      m_lbl[row] = 0;
    }
    delete m_convertButton; m_convertButton = 0;
    delete m_gridLayout;    m_gridLayout = 0;
    delete m_gridFrame;     m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Validates the entry fields and fills in the result.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HorizontalDistanceDialog::convert( void )
{
    bool ok;
    double input[2];
    double minVal[2] = { 0.0, 0.0 };
    double maxVal[2] = { 1000.0, 604.0 };

    // Get and validate the two input fields
    QString qStr(""), text("");
    for ( int row = 0;
          row < 2;
          row++ )
    {
        qStr = m_entry[row]->text().stripWhiteSpace();
        // Must not be empty
        if ( qStr.isNull() || qStr.isEmpty() )
        {
            translate( text, "HorizontalDistanceDialog:NoEntry", FieldName[row] );
            warn( text );
            return( false );
        }
        // Must be a real value
        input[row] = qStr.toDouble( &ok );
        if ( ! ok )
        {
            translate( text, "HorizontalDistanceDialog:EntryInvalid",
                FieldName[0], qStr );
            warn( text );
            return( false );
        }
        // Must be in range
        if ( input[row] < minVal[row] || input[row] > maxVal[row] )
        {
            QString minStr(""), maxStr("");
            minStr.setNum( minVal[row] );
            maxStr.setNum( maxVal[row] );
            translate( text, "HorizontalDistanceDialog:RangeInvalid",
                FieldName[0], qStr, minStr, maxStr );
            warn( text );
            return( false );
        }
    }
    // Calculate
    double groundDistance = input[0];
    double percentSlope   = input[1];
    double degreesSlope   = atan( 0.01 * percentSlope ) * 180. / M_PI;
    qStr.setNum( degreesSlope, 'f', 1 );
    m_entry[2]->setText( qStr );
    for ( int i=0; i<7; i++ )
    {
        double direction = 15. * (double) i;
        double a = groundDistance * cos( direction * M_PI / 180. );
        double b = groundDistance * sin( direction * M_PI / 180. );
        double c = a * cos( degreesSlope * M_PI / 180. );
        double d = sqrt( c * c + b * b );
        qStr.setNum( d, 'f', 1 );
        m_entry[i+3]->setText( qStr );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles navigation and focus between the dialog fields.
 */

void HorizontalDistanceDialog::keyPressEvent( QKeyEvent *e )
{
    const int fieldNext[3] = { 1, 2, 0 };
    const int fieldPrev[3] = { 2, 0, 1 };
    // Find out who has the focus
    int focus = 0;
    for ( int row = 0;
          row < 3;
          row++ )
    {
        if ( m_entry[row]->hasFocus() )
        {
            focus = row;
            break;
        }
    }
    // Process navigation keys.
    if ( e->key() == Key_Up )
    {
        focus = fieldPrev[focus];
        m_entry[focus]->setFocus();
        e->accept();
    }
    else if ( e->key() == Key_Down || e->key() == Key_Return )
    {
        focus = fieldNext[focus];
        m_entry[focus]->setFocus();
        e->accept();
    }
    else
    {
        e->ignore();
    }
    return;
}

//------------------------------------------------------------------------------
//  End of horizontaldistancedialog.cpp
//------------------------------------------------------------------------------

