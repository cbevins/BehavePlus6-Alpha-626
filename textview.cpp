//------------------------------------------------------------------------------
/*! \file textview.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QTextView and a QTextBrowser derived classes that support printing
 *  of their rich text contents via a right-click context menu.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"      // Access to program name and version
#include "printer.h"
#include "textview.h"

// Qt include files
#include <qapplication.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpalette.h>       // for QSimpleRichText::draw()
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qsimplerichtext.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \brief TextBrowser constructor.
 */

TextBrowser::TextBrowser( QWidget *parent, const char *name ) :
    QTextBrowser( parent, name ),
    m_sourceFile(""),
    m_contextMenu( 0 )
{
    // Display rich text
    setTextFormat( Qt::RichText );

    // Create the context menu
    m_contextMenu = new QPopupMenu( this, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    int mid;
    QString text("");

    // Print visible text (quick and dirty)
    translate( text, "TextBrowser:ContextMenu:Visible" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisible );

    // Print entire text (quick and dirty)
    translate( text, "TextBrowser:ContextMenu:Entire" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntire );

    // Print formatted text (slow but pretty)
    translate( text, "TextBrowser:ContextMenu:Formatted" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintFormatted );

    return;
}

//------------------------------------------------------------------------------
/*! \brief TextBrowser destructor.
 */

TextBrowser::~TextBrowser( void )
{
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Simple front end to QTextBrowser::setSource().
 */

void TextBrowser::setSourceFile( const QString &fileName )
{
    m_sourceFile = fileName;
    setSource( m_sourceFile );
    // HACK for Qt 3 to force the logo to be painted
    viewport()->repaint();
    qApp->processEvents();
    append( "." );
    qApp->processEvents();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback function when TextBrowser is right-clicked().
 */

void TextBrowser::contextMenuActivated( int id )
{
    if ( id == ContextPrintVisible )
    {
        printWidget( this );
    }
    else if ( id == ContextPrintEntire )
    {
        printListView( this );
    }
    else if ( id == ContextPrintFormatted )
    {
        printRichText( this );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplemented virtual function that traps right-click mouse events
 *  to invoke the context menu.  All other mouse events are passed up the
 *  chain so that links can be clicked, etc.
 */

void TextBrowser::viewportMousePressEvent( QMouseEvent *event )
{
    // If RightButton, display context menu at cursor position
    if ( event->button() == QMouseEvent::RightButton )
    {
        m_contextMenu->exec( QCursor::pos() );
    }
    // Otherwise pass the mouse event on to handle links, etc.
    else
    {
        QTextBrowser::viewportMousePressEvent( event );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TextView constructor.
 */

TextView::TextView( QWidget *parent, const char *name ) :
    QTextEdit( parent, name ),
    m_contextMenu( 0 )
{
    // Display rich text
    setTextFormat( Qt::RichText );

    // Create the context menu
    m_contextMenu = new QPopupMenu( this, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    int mid;
    QString text("");
    // Print visible text ( quick and dirty )
    translate( text, "TextBrowser:ContextMenu:Visible" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisible );

    // Print entire text (quick and dirty)
    translate( text, "TextBrowser:ContextMenu:Entire" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntire );

    // Print formatted text (slow but pretty)
    translate( text, "TextBrowser:ContextMenu:Formatted" );
    mid = m_contextMenu->insertItem( text,
           this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintFormatted );

    return;
}

//------------------------------------------------------------------------------
/*! \brief TextView destructor.
 */

TextView::~TextView( void )
{
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback function when TextView is right-clicked().
 */

void TextView::contextMenuActivated( int id )
{
    if ( id == ContextPrintVisible )
    {
        printWidget( this );
    }
    else if ( id == ContextPrintEntire )
    {
        printListView( this );
    }
    else if ( id == ContextPrintFormatted )
    {
        printRichText( this );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reimplemented virtual function that traps right-click mouse events
 *  to invoke the context menu.  All other mouse events are passed up the
 *  chain so that links can be clicked, etc.
 */

void TextView::viewportMousePressEvent( QMouseEvent *event )
{
    // If RightButton, display context menu at cursor position
    if ( event->button() == QMouseEvent::RightButton )
    {
        m_contextMenu->exec( QCursor::pos() );
    }
    // Otherwise pass the mouse event on to handle links, etc.
    else
    {
        QTextEdit::viewportMousePressEvent( event );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the contents of a QScrollview.
 *
 *  Used by dialogs that contain a QListViews and QScrollViews (or even a
 *  QTextEdit or QTextBrowser, since they are all QScrollViews).
 *
 *  This works by actually paging the scrollview contents through its viewport,
 *  capturing the viewport widget to a pixmap, then scaling the pixmap to the
 *  printer's paint device.  This makes it more specific than
 *  printWidget(), which could actually print any widget.
 *
 *  This is also much faster for printers to handle than printRichText(),
 *  but the quality is significantly lower since output is in screen
 *  resolution scaled up to printer resolution.
 *
 *  \return TRUE if printed, FALSE if not printed.
 */

bool printListView( QScrollView *scrollView )
{
    // Set up the printer
    Printer printer;
    printer.setFullPage( true );
    printer.setColorMode( QPrinter::Color );
    if ( ! printer.setup() )
    {
        return( false );
    }
    // Get the printer dialog out of the way and
    // TRY to get the widget repainted before it gets grabbed
    qApp->processEvents();
    scrollView->repaint();
    scrollView->raise();

    // Determine the painter scale used to project from screen onto the printer
    QPaintDeviceMetrics dmp( &printer );
    QPaintDeviceMetrics dms( qApp->desktop() );
    double xscale = (double) dmp.logicalDpiX() / (double) dms.logicalDpiX();
    double yscale = (double) dmp.logicalDpiY() / (double) dms.logicalDpiY();

    // Determine the printer's page height, width, and margin in screen pixels
    int pageTop  = (int) ( 0.75 * dmp.logicalDpiY() / yscale );
    int pageLeft = (int) ( 0.75 * dmp.logicalDpiX() / xscale );
    int pageHt  = (int) ( (double) dmp.height() / yscale);
    int pageWd  = (int) ( (double) dmp.width() / xscale);
    int pageEOT = pageHt - pageTop;
    int pageY   = pageTop;

    // Store scrollview's starting position so we can return there
    int x0 = scrollView->contentsX();
    int y0 = scrollView->contentsY();

    // Create the painter with the required scale
    QPainter painter( &printer );
    painter.scale( xscale, yscale );
    QFont font( "Times New Roman", 10 );
    painter.setFont( font );

    // Page the text onto the pixmap
    int blockBeg, blockEnd, blockHt;
    int page       = 1;
    int textY      = 0;
    int viewportWd = scrollView->viewport()->width();
    int viewportHt = scrollView->viewport()->height();
    bool eject = false;
    while( textY < scrollView->contentsHeight() )
    {
        // Attempt to scroll the viewport to the next full page of text
        scrollView->setContentsPos( 0, textY );
        scrollView->viewport()->repaint();
        qApp->processEvents();

        // Viewport will not scroll as far as requested if the end-of-text
        // is reached first
        blockBeg = textY - scrollView->contentsY();

        // Check for the case where the total amount of contents text
        // is less than one viewport page
        blockEnd = ( viewportHt < scrollView->contentsHeight() )
                   ? viewportHt
                   : scrollView->contentsHeight();

        // How many pixel rows are to be fetched from the text view?
        blockHt = blockEnd - blockBeg;

        // Can the page hold this many pixel rows?
        eject = false;
        if ( ( pageY + blockHt ) > pageEOT )
        {
            blockHt = pageEOT - pageY;
            blockEnd = blockBeg + blockHt;
            eject = true;
        }

        // Grab the required part of the widget into a pixmap
        QPixmap pixmap = QPixmap::grabWidget( scrollView->viewport(),
            0, blockBeg, viewportWd, blockHt );

        // Draw the pixmap onto the printer page
        painter.drawPixmap( pageLeft, pageY, pixmap );


        // Increment position pointers
        pageY += blockHt;
        textY += blockHt;

        // Printer page eject?
        if ( eject )
        {
//fprintf( stderr, "Printing page %d\n", page );
            // Draw page number
            painter.drawText(
                pageWd - pageLeft - painter.fontMetrics().width( QString::number(page) ),
                pageHt - pageTop + painter.fontMetrics().ascent() + 5,
                QString::number(page) );
            // Draw program and version
            painter.drawText(
                pageLeft,
                pageHt - pageTop + painter.fontMetrics().ascent() + 5,
                ( appWindow()->m_program + " " + appWindow()->m_version ) );
            // Eject the page and start a new page
            printer.newPage();
            page++;
            pageY = pageTop;
        }
    }
    // Print the last page
    if ( pageY > pageTop )
    {
            // Draw page number
            painter.drawText(
                pageWd - pageLeft - painter.fontMetrics().width( QString::number(page) ),
                pageHt - pageTop + painter.fontMetrics().ascent() + 5,
                QString::number(page) );
            // Draw program and version
            painter.drawText(
                pageLeft,
                pageHt - pageTop + painter.fontMetrics().ascent() + 5,
                ( appWindow()->m_program + " " + appWindow()->m_version ) );
    }
    painter.end();

    // Restore original viewport and return
    scrollView->setContentsPos( x0, y0 );
    scrollView->viewport()->repaint();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Prints rich text. Used by dialogs that contain a TextView
 *  or TextBrowser.
 *
 *  This is much slower than printListView() since the rich text is drawn at
 *  the printer resolution rather than being drawn at the screen resolution
 *  and scaling up.  For 600 dpi printers and 75 dpi screens, this increases
 *  printer data volume by a factor of 64.
 *
 *  This code is taken from src/examples/helpviewer/helpwindow.cpp
 */

bool printRichText( QTextEdit *textEdit )
{
    // Set up the printer
    QPrinter printer;
    printer.setFullPage( true );
    printer.setColorMode( QPrinter::Color );
    if ( ! printer.setup( textEdit ) )
    {
        return( false );
    }

    // Set up the painter and device resolution and size
    QPainter p( &printer );
    QPaintDeviceMetrics metrics(p.device());
    int dpix = metrics.logicalDpiX();
    int dpiy = metrics.logicalDpiY();
    const int margin = 72;      // pt
    QRect body(
        margin * dpix / 72,
        margin * dpiy / 72,
        metrics.width()  - margin * dpix / 72 * 2,
        metrics.height() - margin * dpiy / 72 * 2 );

    // Get the rich text content to display
    QFont font( "Times New Roman", 10 );
    QSimpleRichText richText(
        textEdit->text(),
        font,
        textEdit->context(),
        textEdit->styleSheet(),
        textEdit->mimeSourceFactory(),
        body.height() );
    richText.setWidth( &p, body.width() );
    QRect view( body );

    // Print the rich text page by page
    int page = 1;
    do {
        richText.draw(
            &p,
            body.left(),
            body.top(),
            view,
            textEdit->colorGroup() );
        view.moveBy( 0,  body.height() );
        p.translate( 0, -body.height() );
        p.setFont( font );
        p.drawText(
            view.right() - p.fontMetrics().width( QString::number(page) ),
            view.bottom() + p.fontMetrics().ascent() + 5,
            QString::number(page) );
        p.drawText(
            view.left(),
            view.bottom() + p.fontMetrics().ascent() + 5,
            ( appWindow()->m_program + " " + appWindow()->m_version ) );
        if ( view.top() >= body.top() + richText.height() )
        {
            break;
        }
        printer.newPage();
        page++;
    } while ( true );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Prints the widget to a user-selectable printer.
 *  If necessary the widget is down-scaled to fit the printed page.
 *
 *  Convenience function used by TextView and TextBrowser and all the other
 *  dialogs that want to print themselves or some subwidget.
 *
 *  This uses grabWidget(), that is, it calls repaint() on the widget.
 *  It gets its name because it was originally developed for TextView and
 *  TextBrowser, and would only print the currently visible text.
 *
 *  This is also much faster for printers to handle than printRichText(),
 *  but the quality is significantly lower since output is in screen
 *  resolution scaled up to printer resolution.
 *
 *  \return TRUE if printed, FALSE if not printed.
 */

bool printWidget( QWidget *widget )
{
    // Set up the printer.
    Printer printer;
    printer.setFullPage( true );
    printer.setColorMode( QPrinter::Color );
    if ( ! printer.setup() )
    {
        return( false );
    }
    // Get the printer dialog out of the way and
    // TRY to get the widget repainted before it gets grabbed.
    qApp->processEvents();
    widget->repaint();
    widget->raise();
    qApp->processEvents();

    // Determine the painter scale used to project from screen onto the printer.
    QPaintDeviceMetrics dmp( &printer );
    QPaintDeviceMetrics dms( qApp->desktop() );
    double xscale = (double) dmp.logicalDpiX() / (double) dms.logicalDpiX();
    double yscale = (double) dmp.logicalDpiY() / (double) dms.logicalDpiY();

    // Paint the widget onto a pixmap.
    QPixmap pixmap = QPixmap::grabWidget( widget );

    // Determine the printer's page width (less 1.5" margins) in screen pixels.
    int pageWd  = (int) ( ( (double) dmp.width()
        - 1.5 * (double) dmp.logicalDpiX() ) / xscale );

    // Rescale if the pixmap is too wide for the printer.
    if ( pixmap.width() > pageWd )
    {
        xscale *= ( (double) pageWd / (double) pixmap.width() );
        yscale *= ( (double) pageWd / (double) pixmap.width() );
    }
    // Determine the printer's page height (less 1.5" margins) in screen pixels.
    int pageHt  = (int) ( ( (double) dmp.height()
        - 1.5 * (double) dmp.logicalDpiY() ) / yscale );

    // Rescale if the pixmap is too tall for the printer.
    if ( pixmap.height() > pageHt )
    {
        xscale *= ( (double) pageHt / (double) pixmap.height() );
        yscale *= ( (double) pageHt / (double) pixmap.height() );
    }
    // Determine scaled top and left margins.
    int top  = (int) ( 0.75 * dmp.logicalDpiY() / yscale );
    int left = (int) ( 0.75 * dmp.logicalDpiX() / xscale );

    // Create the painter with the required scale.
    QPainter painter( &printer );
    painter.scale( xscale, yscale );
    painter.drawPixmap( left, top, pixmap );
    painter.end();
    return( true );
}

//------------------------------------------------------------------------------
//  End of textview.cpp
//------------------------------------------------------------------------------

