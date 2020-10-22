//------------------------------------------------------------------------------
/*! \file conflictdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument input conflict resolution dialog methods.
 */

// Custom include files
#include "apptranslator.h"
#include "bpdocument.h"
#include "conflictdialog.h"
#include "property.h"

// Qt include files
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

//------------------------------------------------------------------------------
/*! \brief Conflict1Dialog constructor.
 */

Conflict1Dialog::Conflict1Dialog( BpDocument *bp, const char *name ) :
    AppDialog( bp, "ConflictDialog:Caption", "EastMeetsWest.png",
        "East Meets West", "conflict1.html", name ),
    m_bp(bp),
    m_buttonGroup(0),
    m_button0(0),
    m_button1(0)
{
    QString text("");
    // Radio button group
    translate( text, "ConflictDialog:Group:Caption" );
    m_buttonGroup = new QButtonGroup( 1, Qt::Horizontal, text,
        contentFrame(), "m_buttonGroup" );
    Q_CHECK_PTR( m_buttonGroup );

    translate( text, "ConflictDialog1:Button0" );
    m_button0 = new QRadioButton( text, m_buttonGroup, "m_button0" ) ;
    Q_CHECK_PTR( m_button0 );

    translate( text, "ConflictDialog1:Button1" );
    m_button1 = new QRadioButton( text, m_buttonGroup, "m_button1" ) ;
    Q_CHECK_PTR( m_button1 );

    // Set the default
    m_button0->setChecked( true );

    // Fix the m_buttonGroup size so only the picture and text stretch
    m_buttonGroup->setFixedSize( m_buttonGroup->sizeHint() );

    // Set the initial size for this widget so the entire picture,
    // the entire button group, and a good chunk of the textView are visible.
    int width = widthHint() + m_buttonGroup->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Conflict1Dialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

Conflict1Dialog::~Conflict1Dialog( void )
{
    delete m_button0;       m_button0 = 0;
    delete m_button1;       m_button1 = 0;
    delete m_buttonGroup;   m_buttonGroup = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Changes the configuration according to the buttons,
 *  then reconfigures the worksheet and returns.
 */

void Conflict1Dialog::store( void )
{
    if ( m_button0->isChecked() )
    {
		// Through V5 ...
        // m_bp->property()->boolean( "surfaceConfSpreadDirInput", false );
        // m_bp->property()->boolean( "surfaceConfSpreadDirMax", true );
		// V6 version
        m_bp->property()->boolean( "surfaceConfSpreadDirHead", true );
        m_bp->property()->boolean( "surfaceConfSpreadDirFlank", false );
        m_bp->property()->boolean( "surfaceConfSpreadDirBack", false );
        m_bp->property()->boolean( "surfaceConfSpreadDirFireFront", false );
        m_bp->property()->boolean( "surfaceConfSpreadDirPointSourceBeta", false );
        m_bp->property()->boolean( "surfaceConfSpreadDirPointSourcePsi", false );
    }
    else if ( m_button1->isChecked() )
    {
        m_bp->property()->boolean( "spotModuleActive", false );
    }
    // Since we have changed the options, we must reconfigure.
    m_bp->configure();
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Conflict2Dialog constructor.
 *
 *  This conflict occurs when:
 *      1 Surface is active and uses wind speed at midflame height, and
 *      2 either CROWN or SPOT are active, which require wind speed at 20 ft.
 */

Conflict2Dialog::Conflict2Dialog( BpDocument *bp, const char *name ) :
    AppDialog( bp, "ConflictDialog:Caption", "Hatrack.png", "Hatrack",
        "conflict2.html", name ),
    m_bp(bp),
    m_buttonGroup(0),
    m_button0(0),
    m_button1(0),
    m_button2(0),
    m_button3(0),
    m_button4(0)
{
    QString text("");
    // Radio button group
    translate( text, "ConflictDialog:Group:Caption" );
    m_buttonGroup = new QButtonGroup( 1, Qt::Horizontal, text,
        contentFrame(), "m_buttonGroup" );
    Q_CHECK_PTR( m_buttonGroup );

    translate( text, "ConflictDialog2:Button0" );
    m_button0 = new QRadioButton( text, m_buttonGroup, "m_button0" );
    Q_CHECK_PTR( m_button0 );

    translate( text, "ConflictDialog2:Button1" );
    m_button1 = new QRadioButton( text, m_buttonGroup, "m_button1" );
    Q_CHECK_PTR( m_button1 );

    translate( text, "ConflictDialog2:Button2" );
    m_button2 = new QRadioButton( text, m_buttonGroup, "m_button2" );
    Q_CHECK_PTR( m_button2 );

    translate( text, "ConflictDialog2:Button3" );
    m_button3 = new QRadioButton( text, m_buttonGroup, "m_button3" );
    Q_CHECK_PTR( m_button3 );

    translate( text, "ConflictDialog2:Button4" );
    m_button4 = new QRadioButton( text, m_buttonGroup, "m_button4" );
    Q_CHECK_PTR( m_button4 );

    // Set the default
    m_button0->setChecked( true );

    // Fix the m_buttonGroup size so only the picture and text stretch
    m_buttonGroup->setFixedSize( m_buttonGroup->sizeHint() );

    // Set the initial size for this widget so the entire picture,
    // the entire button group, and a good chunk of the textView are visible.
    int width = widthHint() + m_buttonGroup->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Conflict2Dialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

Conflict2Dialog::~Conflict2Dialog( void )
{
    delete m_button0;       m_button0 = 0;
    delete m_button1;       m_button1 = 0;
    delete m_button2;       m_button2 = 0;
    delete m_button3;       m_button3 = 0;
    delete m_button4;       m_button4 = 0;
    delete m_buttonGroup;   m_buttonGroup = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Changes the configuration according to the buttons,
 *  then reconfigures the worksheet and returns.
 */

void Conflict2Dialog::store( void )
{
    if ( m_button0->isChecked() )
    {
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10M", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10MCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20Ft", true );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20FtCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAtMidflame", false );
        m_bp->configure();
        reject();
    }
    else if ( m_button1->isChecked() )
    {
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10M", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10MCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20Ft", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20FtCalc", true );
        m_bp->property()->boolean( "surfaceConfWindSpeedAtMidflame", false );
        m_bp->configure();
        reject();
    }
    else if ( m_button2->isChecked() )
    {
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10M", true );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10MCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20Ft", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20FtCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAtMidflame", false );
        m_bp->configure();
        reject();
    }
    else if ( m_button3->isChecked() )
    {
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10M", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt10MCalc", true );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20Ft", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAt20FtCalc", false );
        m_bp->property()->boolean( "surfaceConfWindSpeedAtMidflame", false );
        m_bp->configure();
        reject();
    }
    else if ( m_button4->isChecked() )
    {
        m_bp->property()->boolean( "crownModuleActive", false );
        m_bp->property()->boolean( "spotModuleActive", false );
        m_bp->configure();
        accept();
    }
    return;
}

//------------------------------------------------------------------------------
//  End of conflictdialog.cpp
//------------------------------------------------------------------------------

