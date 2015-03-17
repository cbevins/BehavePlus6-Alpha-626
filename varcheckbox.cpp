//------------------------------------------------------------------------------
/*! \file varcheckbox.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QCheckBox that displays its informational text in a QTextView
 *  whenever the mouse enters the QCheckBox.
 */

// CUstom include files
#include "helpbrowser.h"
#include "varcheckbox.h"

// Qt include files
#include <qevent.h>
#include <qtimer.h>

//------------------------------------------------------------------------------
/*! \brief VarCheckBox constructor.
 *
 *  The VarCheckBox is an extended QCheckBox that displays an HTML page
 *  about itself inside an associated HelpBrowser whenever the cursor pauses
 *  over it.
 *
 *  \param buttonText Text to display with the button
 *  \param htmlFile   Name of the HTML file to display
 *  \param browser    Pointer to the shared HelpBrowser
 *  \param parent     Pointer to the parent widget
 *  \param name       Widget name
 */

VarCheckBox::VarCheckBox( const QString &buttonText,
        const QString &htmlFile, HelpBrowser *browser,
        QWidget *parent, const char *name ) :
    QCheckBox( buttonText, parent, name ),
    m_html(htmlFile),
    m_browser(browser),
    m_timer(0)
{
    // Note that QTimer objects are destroyed when their parent is destroyed.
    if ( ! m_html.isNull() && ! m_html.isEmpty() )
    {
        m_timer = new QTimer( this );
        Q_CHECK_PTR( m_timer );
        connect( m_timer, SIGNAL( timeout() ), this, SLOT( timerDone() ) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

VarCheckBox::~VarCheckBox( void )
{
    delete m_timer; m_timer = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Catches the VarCheckBox Enter and Leave events
 *  to start/stop the timer for delayed display of HTML page.
 */

bool VarCheckBox::event( QEvent *e )
{
    if ( m_timer )
    {
        // If Enter, start the timer with a half-second delay
        if ( e->type() == QEvent::Enter )
        {
            m_timer->start( 500, true );
        }
        // If Leave, stop the time if it is active.
        else if ( e->type() == QEvent::Leave )
        {
            if ( m_timer->isActive() )
            {
                m_timer->stop();
            }
        }
    }
    // Continue event processing
    return( QWidget::event( e ) );
}

//------------------------------------------------------------------------------
/*! \brief Timer callback function that displays the HTML page.
 */

void VarCheckBox::timerDone( void )
{
    if ( m_timer )
    {
        m_browser->setSourceFile( m_html );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of varcheckbox.cpp
//------------------------------------------------------------------------------

