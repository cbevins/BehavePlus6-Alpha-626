//------------------------------------------------------------------------------
/*  \file docscrollview.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief DocScrollView scrolling document class methods.
 */

// Custom include files
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "docscrollview.h"
#include "document.h"

// Qt include files
#include <qapplication.h>
#include <qpaintdevice.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \brief DocScrollView constructor.
 */

DocScrollView::DocScrollView( QWidget *qMainWindow,
        DocDeviceSize *docDeviceSize, const char *name ) :
    QScrollView( qMainWindow, name, WNorthWestGravity | WRepaintNoErase ),
    m_screenSize(docDeviceSize),
    m_backingPixmap(1,1)
{
    // Make the backing pixmap the same size as a page on the screen device
    m_backingPixmap.resize( m_screenSize->m_pageWd, m_screenSize->m_pageHt );

    // Set the QScrollView and viewport attributes
    setKeyCompression( TRUE );
    setVScrollBarMode( QScrollView::AlwaysOn );
    setHScrollBarMode( QScrollView::AlwaysOn );
    viewport()->setBackgroundMode( PaletteBackground );
    //viewport()->setBackgroundMode( NoBackground );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the DocScrollView's parent document.
 */

Document *DocScrollView::document( void )
{
    return( (Document *) parent() );
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation to draw the contents of the backing pixmap
 *  onto the scroll view.
 */

void DocScrollView::drawContents( QPainter* painter,
    int cx, int cy, int cw, int ch)
{
    painter->drawPixmap( cx, cy, m_backingPixmap, cx, cy, cw, ch );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation of the QScrollView::focusInEvent()
 *  that adds status bar updating to the Document.
 */

void DocScrollView::focusInEvent( QFocusEvent * )
{
    // Display the Document name in the application's scroll bar
    document()->statusUpdate();
    setMicroFocusHint( width()/2, 0, 1, height(), FALSE );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation of the QScrollView::focusOutEvent()
 *  in case we want to print debug information.
 */

void DocScrollView::focusOutEvent( QFocusEvent * )
{
    //fprintf( stderr, "focusOutEvent()\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Implements cursor movement within the DocScrollView.
 *  This will probably have to be reworked for each application.
 */

void DocScrollView::keyPressEvent( QKeyEvent *e )
{
    int unknown = 0;
    // No Shift, Ctrl, or Alt button
    if ( e->state() == ( Qt::NoButton | Qt::ShiftButton ) )
    {
        switch ( e->key() )
        {
            case Key_Right:
                scrollBy( 10, 0 );
                break;
            case Key_Left:
                scrollBy( -10, 0 );
                break;
            case Key_Up:
                scrollBy( 0, -10 );
                break;
            case Key_Down:
                scrollBy( 0, 10 );
                break;
            case Key_Home:
                setContentsPos( 0, 0 );
                break;
            case Key_End:
                setContentsPos( 0, contentsHeight()-visibleHeight() );
                break;
            case Key_PageUp:
                scrollBy( 0, -visibleHeight() );
                break;
            case Key_PageDown:
                scrollBy( 0, visibleHeight() );
                break;
    #if defined (_ENABLE_COPY_)
            case Key_Insert:
                if ( e->state() & ControlButton )
                {
                    //copy();
                }
                break;
    #endif
            default:
                unknown++;
        }
    }
    // Maintenance door
    else if ( e->state() == ( Qt::ControlButton | Qt::AltButton ) )
    {
        if ( e->key() == Key_M )
        {
            document()->maintenance();
        }
    }
    // Else there is a Shift, Ctrl, or Alt button
    else
    {
        switch ( e->key() )
        {
            case Key_Left:
            case Key_Up:
            case Key_PageUp:
                document()->pagePrev();
                break;
            case Key_Right:
            case Key_Down:
            case Key_PageDown:
                document()->pageNext();
                break;
            case Key_Home:
                document()->pageFirst();
                break;
            case Key_End:
                document()->pageLast();
                break;
            default:
                unknown++;
        }
    }
    // Unknown key
    if ( unknown )
    {
        e->ignore();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Rescales the backing pixmap to the view scale.
 *
 *  Called only by Document::rescale() via DocScrollView::rescale().
 */

void DocScrollView::rescale( void )
{
    // Reset viewport to background color in case the pixmap is shrinking,
    m_backingPixmap.fill( backgroundColor() );
    viewport()->update();
    qApp->processEvents();
    // The screenDevice has already been scaled before this was called,
    // so simply reset the pixmap size to match it.
    m_backingPixmap.resize( m_screenSize->m_pageWd, m_screenSize->m_pageHt );
    // Repaint the viewport's background
    viewport()->repaint();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation in case we want to catch it.
 */

void DocScrollView::resizeEvent( QResizeEvent* e )
{
    // Catch it here
    //fprintf( stderr, "resizeEvent() from %d x %d to %d x %d\n",
    //  e->oldSize().width(), e->oldSize().height(),
    //  e->size().width(), e->size().height() );

    // Now pass it along
    QScrollView::resizeEvent( e );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens the composer file,
 *  draws its instructions onto the backing pixmap, and
 *  blasts the pixmap onto the screen via resizeContents() and setContentsPos().
 *
 *  \return TRUE on success, FALSE if unable to open the composer file.
 */

bool DocScrollView::showPage( Composer *composer, const QString &composerFile )
{
    // Paint the composer file on the backing pixmap
    if ( ! composer->paint( composerFile,
        &m_backingPixmap,
        m_screenSize->m_xppi,
        m_screenSize->m_yppi,
        m_screenSize->m_scale,
        false ) )
    {
        return( false );
    }
    // Must call this to get rid of previous contents
    viewport()->update();

    // Force a scrollbar update
    document()->show();
    resizeContents( m_backingPixmap.width(), m_backingPixmap.height() );

    // Start at the top of the page
    setContentsPos( 0, 0 );

    // Update the status bar
    document()->statusUpdate();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation in case we want to catch it.
 */

void DocScrollView::viewportMouseReleaseEvent( QMouseEvent *e )
{
    // Which button was pressed?
    int btn = 0;
    if ( e->button() == QMouseEvent::LeftButton )
    {
        btn = 1;
    }
    else if ( e->button() == QMouseEvent::RightButton )
    {
        btn = 2;
    }
    else if ( e->button() == QMouseEvent::MidButton )
    {
        btn = 3;
    }

    //static const char *Btn[] = { "Other", "Left", "Right", "Mid" };
    //fprintf( stderr, "%s button pressed at %d, %d (%d, %d)\n",
    //  Btn[btn], e->x(), e->y(), e->globalX(), e->globalY() );

    // Catch the context menu button
    if ( btn == 2 )
    {
        document()->contextMenuShow();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation in case we want to catch it.
 */

void DocScrollView::viewportResizeEvent(QResizeEvent* )
{
    //fprintf( stderr, "viewportResizeEvent()\n" );
    return;
}

//------------------------------------------------------------------------------
//  End of docscrollview.cpp
//------------------------------------------------------------------------------

