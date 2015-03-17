//------------------------------------------------------------------------------
/*! \file unitsconverterdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus units converter dialog class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "unitsconverterdialog.h"

// Qt include files
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

/*! \var FieldNameKey
 *  \brief Array of translator keys for the 3 entry field labels,
 *  Convert button, and initial results text.  The translated results are
 *  stored in the FieldName[] array.
 */
static QString FieldName[5] =  { "", "", "", "", "" };
static const QString FieldNameKey[5] =
{
    "UnitsConverterDialog:Field:FromAmount",
    "UnitsConverterDialog:Field:FromUnits",
    "UnitsConverterDialog:Field:IntoUnits",
    "UnitsConverterDialog:Convert",
    "UnitsConverterDialog:Entry"
};

//------------------------------------------------------------------------------
/*! \brief UnitsConverterDialog constructor.
 */

UnitsConverterDialog::UnitsConverterDialog( QWidget *parent, const char *name ) :
    AppDialog( parent, "UnitsConverterDialog:Caption",
        "WatchableWildlife.png", "Watchable Wildlife",
        "unitsConverter.html", name, "UnitsConverterDialog:Button:Dismiss", 0 ),
    m_gridFrame(0),
    m_gridLayout(0),
    m_convertButton(0)
{
    // Initialization
    int row;
    for ( row = 0;
          row < 5;
          row++ )
    {
        translate( FieldName[row], FieldNameKey[row] );
    }
    for ( row = 0;
          row < 4;
          row++ )
    {
        m_entry[row] = 0;
    }
    for ( row = 0;
          row < 3;
          row++ )
    {
        m_lbl[row] = 0;
    }

    // Hidden frame to contain a grid layout
    m_gridFrame = new QFrame( m_page->m_contentFrame, "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::NoFrame );

    // Create the label-entry grid layout
    m_gridLayout = new QGridLayout( m_gridFrame, 4, 2, 0, 2, "m_gridLayout" ) ;
    Q_CHECK_PTR( m_gridLayout );

    // Top portion contains entry fields and labels
    for ( row = 0;
          row < 4;
          row++ )
    {
        // Label (or button)
        if ( row < 3 )
        {
            m_lbl[row] = new QLabel( FieldName[row], m_gridFrame );
            Q_CHECK_PTR( m_lbl[row] );
            m_lbl[row]->setFixedSize( m_lbl[row]->sizeHint() );
            m_gridLayout->addWidget( m_lbl[row], row, 0, AlignLeft );
        }
        else
        {
            m_convertButton = new QPushButton( FieldName[row],
                m_gridFrame, "m_convertButton" );
            Q_CHECK_PTR( m_convertButton );
            m_gridLayout->addWidget( m_convertButton, row, 0 );
            connect( m_convertButton, SIGNAL( clicked() ),
                     this,            SLOT( convert() ) );
        }
        // Entry field
        m_entry[row] = new QLineEdit( m_gridFrame );
        Q_CHECK_PTR( m_entry[row] );
        m_gridLayout->addWidget( m_entry[row], row, 1 );
    }
    m_entry[3]->setText( FieldName[4] );
    m_entry[3]->setReadOnly( true );
    m_entry[0]->setFocus();

    // Make sure the entry fields are big enough.
    QFontMetrics fm( m_entry[3]->font() );
    int w = fm.width( FieldName[4] );
    for ( row = 0;
          row < 4;
          row++ )
    {
        m_entry[row]->setMinimumWidth( w );
    }
    // Do not allow the middle pane to change size, so that if the
    // dialog is maximized the help pane gets all the new space.
    m_page->m_midFrame->setFixedWidth(
        m_page->m_midFrame->sizeHint().width() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief UnitsConverterDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

UnitsConverterDialog::~UnitsConverterDialog( void )
{
    int row;
    for ( row = 0;
          row < 4;
          row++ )
    {
        delete m_entry[row];    m_entry[row] = 0;
    }
    for ( row = 0;
          row < 3;
          row++ )
    {
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

bool UnitsConverterDialog::convert( void )
{
    bool ok;
    double srcValue = 1.;
    double dstValue = 0.;

    // Make sure there is an entry in each field.
    QString qStr(""), text("");
    for ( int row = 0;
          row < 3;
          row++ )
    {
        qStr = m_entry[row]->text().stripWhiteSpace();
        if ( qStr.isNull() || qStr.isEmpty() )
        {
            translate( text, "UnitsConverterDialog:NoEntry", FieldName[row] );
            warn( text );
            return( false );
        }
    }
    // Make sure the amount field is a real value
    qStr = m_entry[0]->text().stripWhiteSpace();
    srcValue = qStr.toDouble( &ok );
    if ( ! ok )
    {
        translate( text, "UnitsConverterDialog:EntryInvalid",
            FieldName[0], qStr );
        warn( text );
        return( false );
    }
    // Try to convert
    if ( ! appSiUnits()->convert( srcValue, m_entry[1]->text(),
        m_entry[2]->text(), &dstValue ) )
    {
        translate( text, "UnitsConverterDialog:Error" );
        //m_entry[3]->setEnabled( true );
        m_entry[3]->setText( text );
        //m_entry[3]->setEnabled( false );
        warn( appSiUnits()->htmlMsg() );
        return( false );
    }
    qStr.setNum( dstValue );
    //m_entry[3]->setEnabled( true );
    m_entry[3]->setText( qStr );
    //m_entry[3]->setEnabled( false );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles navigation and focus between the dialog fields.
 */

void UnitsConverterDialog::keyPressEvent( QKeyEvent *e )
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
//  End of unitsconverterdialog.cpp
//------------------------------------------------------------------------------

