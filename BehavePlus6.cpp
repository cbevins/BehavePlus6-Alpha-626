//------------------------------------------------------------------------------
/*! \file BehavePlus6.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus main driver program.
 */

// Custom include files
#include "app.h"
#include "appmessage.h"
#include "appwindow.h"
#include "platform.h"

// Qt include files
#include <qapplication.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qframe.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qrect.h>

// Standard include files
#include <stdlib.h>
#include <time.h>

#if defined(Q_WS_WIN)
//extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
extern __declspec(dllimport) int qt_ntfs_permission_lookup;
#endif

// Program name and version
// NOTE: must recompile appwindow.cpp to update the displayed compile date & time
// NOTE: to save the splash page in a file, see line 275
static QString Program( "BehavePlus" );
static QString Version( "6.0.0" );
static QString Build( "Build 626 Beta 3" );
static QString ReleaseNote( "" );
static QString SplashFile( "BehavePlus6.bmp" );
//static QString SplashFile( "splash294.bmp" );
// Show a beta warning on the splash screen?
static bool ShowWarning = false;

static void startLog( int argc, char **argv ) ;
static void stopLog( void ) ;

// Splash page pixmap
#include "wildfire.xpm"

//------------------------------------------------------------------------------
/*! \brief BehavePlusApp constructor.
 */

BehavePlusApp::BehavePlusApp( int &p_argc, char **p_argv ) :
    QApplication( p_argc, p_argv ),
    m_program( Program ),
    m_version( Version ),
    m_build( Build ),
    m_releaseNote( ReleaseNote ),
    m_pixmap( Wildfire_xpm ),
    m_splash( 0 )
{
    // We only need to draw the pixmap once.
    drawSplashPage();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Closes the splash page.
 */

void BehavePlusApp::closeSplashPage( void )
{
    // Note that close(true) and/or WDestructiveClose deletes the widget when closed.
    m_splash->close( true );
    m_splash = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the program info on the splash pixmap.
 *
 *  Private method called only by showSplashPage().
 */

void BehavePlusApp::drawSplashPage( void )
{
    // If unable to load the picture, make a plain cyan backdrop instead
    if ( m_pixmap.isNull() )
    // This code block should never be executed!
    {
        m_pixmap.resize( 700, 500 );
        QPainter paint( &m_pixmap );
        paint.fillRect( 0, 0, 700, 500, QBrush( "cyan", SolidPattern ) );
        paint.end();
    }

    // Painter attributes
    int align = Qt::AlignHCenter | Qt::AlignTop ;
    //static QString fontName( "Times New Roman" );
    static QString fontName( "Arial" );
    QFont fnt1( fontName, 48, QFont::Bold, false );
    QFont fnt2( fontName, 24, QFont::Bold, false );
    QFont fnt3( fontName, 16, QFont::Bold, false );
    QFontMetrics fm1( fnt1 );
    QFontMetrics fm2( fnt2 );
    QFontMetrics fm3( fnt3 );
    QPen blackPen( black );
    QPen grayPen( gray );
    QPen redPen( red );
    QPen whitePen( white );

    // Draw shadowed name
    QPainter paint( &m_pixmap );
    int wd = m_pixmap.width();
    int ht = m_pixmap.height() + 3 * fm3.lineSpacing();
    paint.setFont( fnt1 );
    paint.setPen( grayPen );
    int y0 = fm1.lineSpacing()/8;
    paint.drawText( 4, y0+4, wd, ht, align, m_program );
    paint.setPen( blackPen );
    paint.drawText( 2, y0+2, wd, ht, align, m_program );
    paint.setPen( whitePen );
    paint.drawText( 0, y0,   wd, ht, align, m_program );

    // Draw sub name and version
    y0 += 3*fm1.lineSpacing()/4;
    paint.setFont( fnt2 );
    paint.drawText( 0, y0, wd, ht-y0, align, "fire modeling system" );
    y0 += 3*fm2.lineSpacing()/4;
    paint.drawText( 0, y0, wd, ht-y0, align, "Version " + m_version );

    // Warning
    if ( ShowWarning )
    {
        y0 += 3 * fm2.lineSpacing();
        paint.setFont( fnt2 );
        paint.setPen( redPen );
        paint.drawText( 0, y0, wd, ht-y0, align,
            "This is pre-release software" );
        y0 += fm2.lineSpacing();
        paint.drawText( 0, y0, wd, ht-y0, align,
            "for testing purposes only!" );
        y0 += fm2.lineSpacing();
        paint.drawText( 0, y0, wd, ht-y0, align,
            "Use at Your Own Risk!" );
    }

    // Authors
    y0 = ht - 6 * fm3.lineSpacing();
    paint.setFont( fnt3 );
    paint.setPen( whitePen );
    paint.drawText( 0, y0, wd, ht-y0, align,
        "US Forest Service, Rocky Mountain Research Station" );
    y0 += fm3.lineSpacing();
    paint.drawText( 0, y0, wd, ht-y0, align,
        "& Systems for Environmental Management" );
    paint.end();

    // Status message display area
    m_statusLine = ht - 2 * fm3.lineSpacing();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves Splash Page to a BMP file using the current.
 */

void BehavePlusApp::saveSplashPage( void )
{
    log( QString( "    Saving the splash page to file \"%1\"...\n" )
        .arg( SplashFile ) );

    QImageIO iio;
    QImage   image;
    image =  m_pixmap;
    iio.setImage( image );
    iio.setFileName( SplashFile );
    iio.setFormat( "BMP" );
    if ( ! iio.write() )
    // This code block should never be executed!
    {
        QString caption(""), text("");
        translate( caption, "AppWindow:SplashPage:WriteError:Caption" );
        translate( text, "AppWindow:SplashPage:WriteError:Text", SplashFile );
        warn( caption, text );
        log( QString( "    FAILED.\n" ) );
    }
    else
    {
        log( QString( "    OK.\n" ) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Shows the splash page.
 *
 *  \param p_saveSplashPage If TRUE, saves the splash page to a BMP file.
 *
 *  Called only by the main() and AppWindow::slotSplashPageShow().
 */

void BehavePlusApp::showSplashPage( bool p_saveSplashPage )
{
    // Note that WDestructiveClose deletes the widget when closed.
    m_splash = new QLabel( 0, "m_splash", WDestructiveClose );
    // | WStyle_Customize | WStyle_NoBorder | WX11BypassWM | WStyle_StaysOnTop );
    m_splash->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
    m_splash->setPixmap( m_pixmap );
    m_splash->adjustSize();
    m_splash->setCaption( m_program );

    // Center it on the screen
    QRect screen = QApplication::desktop()->geometry();
    m_splash->move( screen.center() - QPoint( m_splash->width() / 2,
        m_splash->height() / 2 ) );

    // Show the splash page
    m_splash->show();
    m_splash->repaint( FALSE );
    QApplication::flushX();

    // Save the splash page?
    if ( p_saveSplashPage )
    {
        saveSplashPage();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the splash page status message.
 */

void BehavePlusApp::updateSplashPage( const QString &message )
{
    log( QString( "Update splash message : " + message ), true );
    if ( m_splash )
    {
        m_splash->repaint( false );
        QPainter p( m_splash );
        p.setFont( QFont( "Times New Roman", 16, QFont::Bold, false ) );
        p.setPen( Qt::black );
        p.drawText( 5, 20, message );
        p.end();
        QApplication::flushX();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief BehavePlus main function.
 */

int main( int argc, char **argv )
{
#if defined(Q_WS_WIN)
     qt_ntfs_permission_lookup = 0;
#endif

    // Initialize the BehavePlus QApplication.
    BehavePlusApp *app = new BehavePlusApp( argc, argv );
    Q_CHECK_PTR( app );
    app->connect( app, SIGNAL( lastWindowClosed() ), app, SLOT( quit() ) );

    // Start the logger
    startLog( argc, argv );

    // Show the splash page ( if TRUE, then splash page is saved as a bmp file)
    app->showSplashPage( false );
    app->updateSplashPage( "Initializing..." );

    // Create the application main window
    AppWindow *appWin = new AppWindow( app, app->m_program, app->m_version,
        app->m_build, app->m_releaseNote );
    Q_CHECK_PTR( appWin );
    app->setMainWidget( appWin );

#if defined( _WS_X11 )
    qt_wait_for_window_manager( appWin );
#endif

    // Run the application.
    int result = app->exec();

    // Cleanup
    stopLog();
    delete app;         app = 0;
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Starts the BehavePlus log file.
 */

static void startLog( int argc, char **argv )
{
    // Open a log file
    time_t now = time((time_t *) NULL);
    logOpen( QString( Program + ".log" ).latin1() );
    log( QString( Program + " " + Version + " started on " + ctime(&now) ),
        true );
    log( "Beg Section: Command Line and Environment\n" );

    // Log command line options
    QString str( argv[0] );
    int i;
    for ( i = 1;
          i < argc;
          i++ )
    {
        str.append( " " );
        str.append( argv[i] );
    }
    log( QString( "    Command Line               = \"%1\"\n").arg( str ) );
    log( QString( "    Program Name               = \"%1\"\n").arg( argv[0] ) );
    log( QString( "    Version Number             = \"%1\"\n").arg( Version ) );
    log( QString( "    Build Number               = \"%1\"\n").arg( Build ) );
    log( QString( "    Release Notes              = \"%1\"\n")
        .arg( ReleaseNote ) );
    log( QString( "    Compiled                   = \"%1 %2\"\n")
        .arg( __DATE__ ).arg( __TIME__ ) );

    char buffer[1024];
    platformGetCwd( buffer, sizeof(buffer) );
    log( QString( "    Current Working Directory  = \"%1\"\n" ).arg( buffer ) );
    log( QString( "    qApp->applicationFilePath()= \"%1\"\n" )
        .arg( qApp->applicationFilePath() ) );
    log( QString( "    qApp->applicationDirPath() = \"%1\"\n" )
        .arg( qApp->applicationDirPath() ) );

    // Environment Variables
    str = getenv( "PATH" );
    log( QString( "    PATH                       = \"%1\"\n" ).arg( str ) );
    str = getenv( "BEHAVEPLUS" );
    log( QString( "    BEHAVEPLUS                 = \"%1\"\n" ).arg( str ) );
    str = getenv( "BEHAVEPLUSHOME" );
    log( QString( "    BEHAVEPLUSHOME             = \"%1\"\n" ).arg( str ) );
    log( "End Section: Command Line and Environment\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stops the BehavePlus log file.
 */

static void stopLog( void )
{
    time_t now = time((time_t *) NULL);
    log( "\n" );
    log( QString( Program + " " + Version + " stopped on " + ctime(&now) ),
        true );
    logClose();
    return;
}

//------------------------------------------------------------------------------
//  End of BehavePlus3.cpp
//------------------------------------------------------------------------------

