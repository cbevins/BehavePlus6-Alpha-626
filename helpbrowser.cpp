//------------------------------------------------------------------------------
/*! \file helpbrowser.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Help browser class methods.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "helpbrowser.h"
#include "platform.h"
#include "textview.h"

// Qt include files
#include <qdir.h>           // Moved from helpbrowser.h
#include <qfileinfo.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qevent.h>         // Moved from helpbrowser.h
#include <qmap.h>           // Moved from helpbrowser.h
#include <qpixmapcache.h>
#include <qpushbutton.h>
#include <qstringlist.h>

// Local XPMs
static const char *back_xpm[] =
{
    "16 16 5 1",
    "# c #000000",
    "a c #ffffff",
    "c c #808080",
    "b c #c0c0c0",
    ". c None",
    "................",
    ".......#........",
    "......##........",
    ".....#a#........",
    "....#aa########.",
    "...#aabaaaaaaa#.",
    "..#aabbbbbbbbb#.",
    "...#abbbbbbbbb#.",
    "...c#ab########.",
    "....c#a#ccccccc.",
    ".....c##c.......",
    "......c#c.......",
    ".......cc.......",
    "........c.......",
    "................",
    "................"
};

static const char *forward_xpm[] =
{
    "16 16 5 1",
    "# c #000000",
    "a c #ffffff",
    "c c #808080",
    "b c #c0c0c0",
    ". c None",
    "................",
    "................",
    ".........#......",
    ".........##.....",
    ".........#a#....",
    "..########aa#...",
    "..#aaaaaaabaa#..",
    "..#bbbbbbbbbaa#.",
    "..#bbbbbbbbba#..",
    "..########ba#c..",
    "..ccccccc#a#c...",
    "........c##c....",
    "........c#c.....",
    "........cc......",
    "........c.......",
    "................",
    "................"
};

static const char *home_xpm[] =
{
    "16 16 4 1",
    "# c #000000",
    "a c #ffffff",
    "b c #c0c0c0",
    ". c None",
    "........... ....",
    "   ....##.......",
    "..#...####......",
    "..#..#aabb#.....",
    "..#.#aaaabb#....",
    "..##aaaaaabb#...",
    "..#aaaaaaaabb#..",
    ".#aaaaaaaaabbb#.",
    "###aaaaaaaabb###",
    "..#aaaaaaaabb#..",
    "..#aaa###aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#####.######..",
    "................"
};

static const char *index_xpm[] =
{
    /* width height num_colors chars_per_pixel */
    "    22    16        7            1",
    /* colors */
    ". c #000000",
    "# c #808000",
    "a c #800080",
    "b c None",
    "c c #808080",
    "d c #ffff00",
    "e c #ffffff",
    /* pixels */
    "bbbbbbbbbba.bbbbbbbbbb",
    "bbbbbbbbbcaaa..bbbbbbb",
    "bbbbbbbacaaaaaaa.bbbbb",
    "bbbbbbaaaaadd#aaaaa.bb",
    "bbbbbcaaadd##ddaaaaaa.",
    "bbbacaaaaa#aaddaaaa..b",
    "bbaaaaaaa#ddd#aaaa.b.b",
    "bcaaaaaaaa#aaaaaacbb.b",
    "acc.aaaaddaaaaaacbbb..",
    "abebc..aaaaaaa.bbbba.b",
    "abeeeebc.aaaacbbb.abbb",
    "b.acceeeecc.cbbb..bbbb",
    "bbb..aceeeeebbba.bbbbb",
    "bbbbbb.aaceeb.abbbbbbb",
    "bbbbbbbbb.ac..bbbbbbbb",
    "bbbbbbbbbbb..bbbbbbbbb",
};

//------------------------------------------------------------------------------
/*! \brief HelpBrowser default constructor.
 *
 *  \param parent Pointer to the parent QWidget.
 *  \param Widget name.
 */

HelpBrowser::HelpBrowser( QWidget *parent, const char *name ) :
    QVBox( parent, name ),
    m_navFrame(0),
    m_browser(0),
    m_htmlDir(""),
    m_topicFile(""),
    m_indexFile(""),
    m_helpFile("")
{
    m_btn[0] = m_btn[1] = m_btn[2] = m_btn[3] = 0;
    init();
    return;
}

//------------------------------------------------------------------------------
/*! \brief HelpBrowser custom constructor.
 *
 *  \param parent   Pointer to the parent QWidget.
 *  \param htmlDir  Full path name of the HTML file source directory
 *                  (with terminating separator).
 *  \param topic    Topic's home HTML document file name.
 *  \param index    Main index HTML document file name.
 *  \param help     Windows "hlp" help file name.
 *  \param name     Widget name.
*/

HelpBrowser::HelpBrowser( QWidget *parent, const QString &htmlDir,
        const QString &topicFile, const QString &indexFile,
        const QString &helpFile,  const char *name ) :
    QVBox( parent, name ),
    m_navFrame(0),
    m_browser(0),
    m_htmlDir(""),
    m_topicFile(""),
    m_indexFile(""),
    m_helpFile("")
{
    m_btn[0] = m_btn[1] = m_btn[2] = m_btn[3] = 0;
    init();
    setFiles( htmlDir, topicFile, indexFile, helpFile );
    return;
}

//------------------------------------------------------------------------------
/*! \brief HelpBrowser destructor
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

HelpBrowser::~HelpBrowser( void )
{
    delete m_browser;       m_browser = 0;
    for ( int btn = 0;
          btn < 4;
          btn++ )
    {
        delete m_btn[btn];  m_btn[btn] = 0;
    }
    delete m_navFrame;      m_navFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Private function that assembles the HelpBrowser dialog.
 *
 *  Called only by the constructors.
 */

void HelpBrowser::init( void )
{
    // Text translation and pixmap cache keys.
    static const char *Key[] = { "HelpBrowser:Back",
        "HelpBrowser:Frwd", "HelpBrowser:Home", "HelpBrowser:Index" };
    // Pointers to each buttons' pixmap
    static const char **xpm[] = { back_xpm, forward_xpm, home_xpm, index_xpm };

    // Optionally set the frame style
    //setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    // Add the navigation button box and optional frame style
    m_navFrame = new QHBox( this, "m_navFrame" );
    Q_CHECK_PTR( m_navFrame );
    //m_navFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    // Add all the buttons
    QPixmap pixmap;
    QString text("");
    for ( int btn = 0;
          btn < 4;
          btn++ )
    {
        // Check if the pixmap is in the cache
        if ( ! QPixmapCache::find( Key[btn], pixmap ) )
        {
            //qDebug( QString( "%1 pixmap loaded at HelpBrowser::init()." ).arg( Key[btn] ) );
            pixmap = QPixmap( xpm[btn] );
            QPixmapCache::insert( Key[btn], pixmap );
        }
        // Get translated button text
        translate( text, Key[btn] );
        // Create the button
        m_btn[btn] = new QPushButton( pixmap, text, m_navFrame, Key[btn] );
        Q_CHECK_PTR( m_btn[btn] );
        m_btn[btn]->setMinimumSize( m_btn[btn]->sizeHint() );
    }

    // Browser
    m_browser = new TextBrowser( this, "m_browser" );
    Q_CHECK_PTR( m_browser );

    // Connect the Back button
    connect( m_btn[0],  SIGNAL( clicked() ),
             m_browser, SLOT( backward() ) );
    connect( m_browser, SIGNAL( backwardAvailable(bool) ),
             m_btn[0],  SLOT( setEnabled(bool) ) );
    // Connect the Frwd button
    connect( m_btn[1],  SIGNAL( clicked() ),
             m_browser, SLOT( forward() ) );
    connect( m_browser, SIGNAL( forwardAvailable(bool) ),
             m_btn[1],  SLOT( setEnabled(bool) ) );
    // Connect the Home button
    connect( m_btn[2],  SIGNAL( clicked() ),
             m_browser, SLOT( home() ) );
    // Connect the Index button
    connect( m_btn[3],  SIGNAL( clicked() ),
             this,      SLOT( showIndex() ) );

    // Disable the Back and Frwd buttons to start with and set the minimum size
    m_btn[0]->setEnabled( false );
    m_btn[1]->setEnabled( false );
    int minWidth = // 40 +
        m_btn[0]->sizeHint().width() +
        m_btn[1]->sizeHint().width() +
        m_btn[2]->sizeHint().width() +
        m_btn[3]->sizeHint().width();
    m_navFrame->setMinimumWidth( minWidth );
    setMinimumWidth( minWidth );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the HTML directory, topic file, and index file.
 *
 *  \param htmlDir  Full path name of the HTML file source directory
 *                  (with terminating separator).
 *  \param topic    Topic's home HTML document file name.
 *  \param index    Main index HTML document file name.
 *  \param help     Windows "hlp" help file name.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool HelpBrowser::setFiles( const QString &htmlDir, const QString &topicFile,
    const QString &indexFile, const QString &helpFile )
{
    // Make sure there is a drive letter
    if ( platformGetOs() == "Windows" )
    {
        if ( htmlDir.mid( 1, 1 ) != ":" )
        {
            QString msg("");
            translate( msg, "HelpBrowser:MissingDrive", htmlDir );
            error( msg );
            return( false );
        }
    }

    // Check if the HTML directory exists and is readable
    QFileInfo fi( htmlDir );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isDir() )
    {
        QString msg("");
        translate( msg, "HelpBrowser:MissingDir", htmlDir );
        error( msg );
        return( false );
    }
    m_htmlDir = htmlDir;

    // Check if the topic file exists and is readable
    QString topicPath( m_htmlDir + topicFile );
    fi.setFile( topicPath );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        QString msg("");
        translate( msg, "HelpBrowser:MissingFile", topicPath );
        error( msg );
        m_topicFile = indexFile;
        return( false );
    }
    m_topicFile = topicFile;

    // Check if the index file exists and is readable
    QString indexPath( m_htmlDir + indexFile );
    fi.setFile( indexPath );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        QString msg("");
        translate( msg, "HelpBrowser:MissingFile", indexPath );
        error( msg );
        return( false );
    }
    m_indexFile = indexFile;

    // Check if the help file exists and is readable
    if ( platformGetOs() == "Windows" )
    {
        QString helpPath( m_htmlDir + helpFile );
        fi.setFile( helpPath );
        if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
        {
            QString msg("");
            translate( msg, "HelpBrowser:MissingFile", helpPath );
            error( msg );
            return( false );
        }
        m_helpFile = helpFile;
    }
    // Set the values and return
    m_browser->mimeSourceFactory()->setFilePath( QStringList( htmlDir ) );
    m_browser->setSourceFile( m_topicFile );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the HTMl source file.
 *
 *  \param topicFile Topic's home HTML document file name,
 *                      which must be in the current htmlDir.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool HelpBrowser::setSourceFile( const QString &topicFile )
{
    // Check if the topic file exists and is readable
    QString topicPath( m_htmlDir + topicFile );
    QFileInfo fi( topicPath );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        QString msg("");
        translate( msg, "HelpBrowser:MissingFile", topicPath );
        error( msg );
        m_topicFile = m_indexFile;
        return( false );
    }
    m_topicFile = topicFile;

    // Set the source file and return.
    m_browser->setSourceFile( m_topicFile );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Displays the HTML index file.
 */

void HelpBrowser::showIndex( void )
{
    //if ( platformGetOs() == "Windows" )
    //{
    //    QString helpFile( m_htmlDir + m_helpFile );
    //    platformShowHelpBrowserIndex( helpFile );
    //}
    //else if ( platformGetOs() == "Linux" )
    //{
        m_browser->setSourceFile( m_indexFile );
    //}
    return;
}

//------------------------------------------------------------------------------
//  End of helpbrowser.cpp
//------------------------------------------------------------------------------

