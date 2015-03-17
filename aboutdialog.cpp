//------------------------------------------------------------------------------
/*! \file aboutdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Definition of the BehavePlus AboutDialog class.
 */

// Custom include files.
#include "aboutdialog.h"
#include "appmessage.h"
#include "apptranslator.h"

//------------------------------------------------------------------------------
/*! \brief AboutDialog constructor.
 *
 *  \param parent Pointer to the parent widget.
 *  \param name Internal widget name.
 */

AboutDialog::AboutDialog( QWidget *p_parent, const char *p_name ) :
    AppTabDialog( p_parent, "", p_name, "AboutDialog:Ok", "" ),
    m_page1(0),
    m_page2(0),
    m_page3(0),
    m_page4(0),
    m_page5(0),
    m_page6(0)
{
    QString text("");
    // The bang (!) preceding the picture name prevents the phrase
    // "Detail from" from being prefixed.
    // BehavePlus
    m_page1 = new AppPage( this,
        "Wildfire1.png",
        "!BehavePlus logo by Monte Dolack",
        "aboutBehavePlus.html",
        "aboutBehavePlus" );
    checkmem( __FILE__, __LINE__, m_page1, "AppPage m_page1", 1 );
    m_page1->m_contentFrame->hide();
    addTab( m_page1, "BehavePlus" );

    // Andrews
    m_page2 = new AppPage( this,
        "PatAndMocha.png",
        "!Pat Andrews and Mocha",
        "aboutAndrews.html",
        "aboutAndrews" );
    checkmem( __FILE__, __LINE__, m_page2, "AppPage m_page2", 1 );
    m_page2->m_contentFrame->hide();
    addTab( m_page2, "Andrews" );

    // Bevins
    m_page3 = new AppPage( this,
        "BevinsAtFence.png",
        "!Annual pilgrimage",
        "aboutBevins.html",
        "aboutBevins" );
    checkmem( __FILE__, __LINE__, m_page3, "AppPage m_page3", 1 );
    m_page3->m_contentFrame->hide();
    addTab( m_page3, "Bevins" );

    // Seli
    m_page4 = new AppPage( this,
        "Seli.png",
        "!Robert C. Seli",
        "aboutSeli.html",
        "aboutSeli" );
    checkmem( __FILE__, __LINE__, m_page4, "AppPage m_page4", 1 );
    m_page4->m_contentFrame->hide();
    addTab( m_page4, "Seli" );

    // Dolack
    m_page5 = new AppPage( this,
        "Dolack.png",
        "!Monte Dolack at work & play",
        "aboutDolack.html",
        "aboutDolack" );
    checkmem( __FILE__, __LINE__, m_page5, "AppPage m_page5", 1 );
    m_page5->m_contentFrame->hide();
    addTab( m_page5, "Dolack" );

    // Acknowledgments
    m_page6 = new AppPage( this,
        "GhostRiders.png",
        "Ghost Riders",
        "aboutFunding.html",
        "aboutFunding" );
    checkmem( __FILE__, __LINE__, m_page6, "AppPage m_page6", 1 );
    m_page6->m_contentFrame->hide();
    translate( text, "AboutDialog:Acknowledgments" );
    addTab( m_page6, text );

    // Resize and return.
    resize( 800, 600 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief AboutDialog destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
*/

AboutDialog::~AboutDialog( void )
{
    delete m_page1; m_page1 = 0;
    delete m_page2; m_page2 = 0;
    delete m_page3; m_page3 = 0;
    //delete m_page4; m_page4 = 0;
    delete m_page5; m_page5 = 0;
    delete m_page6; m_page6 = 0;
    delete m_page6; m_page6 = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of aboutdialog.cpp
//------------------------------------------------------------------------------

