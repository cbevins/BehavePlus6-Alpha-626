//------------------------------------------------------------------------------
/*! \file appwindow.cpp
 *		Build 614 2015-04-08
 *		Build 613 2014-07-14
 *		Build 612 2014-04-04
 *		Build 611 2014-03-13
 *		Build 610 2013-12-11
 *		Build 608 2013-11-13
 *		Build 606 2013-09-02
 *		Build 604 2012-07-07
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2015 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AppWindow class definition.  This class creates the main
 *  application window including its workspace, menu bar, menu system,
 *  status bar, icon sets, splash page, etc.
 *
 *  Note that the AppWindow is constructed in two parts.
 *  First this constructor does what it can BEFORE main executes the event loop
 *  (these functions start with "init").
 *  Once we have a gui running, slotAppInit() is called back by a timer
 *  to perform the remaining tasks in the "post" functions.
 */

// Custom include files
#include "aboutdialog.h"
#include "app.h"
#include "appfilesystem.h"
#include "appmessage.h"
#include "appproperty.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocument.h"
#include "calendardocument.h"
#include "cdtlib.h"
//#include "coverage.h"
#include "datetime.h"
#include "docscrollview.h"
#include "document.h"
#include "fdfmcdialog.h"
#include "fileselector.h"
#include "globalposition.h"
#include "horizontaldistancedialog.h"
#include "humiditydialog.h"
#include "platform.h"
#include "property.h"
#include "slopetooldialog.h"
#include "sundialog.h"
#include "textviewdocument.h"
#include "unitsconverterdialog.h"
#include "unitseditdialog.h"
#include "xeqapp.h"

// Qt include files
#include <qapplication.h>
#include <qdatetime.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvbox.h>
#include <qwindowsstyle.h>
#include <qworkspace.h>

// Standard include files
#include <stdlib.h>

// File containing all the application's icons and pixmaps
#include "appwindow.xpm"

//------------------------------------------------------------------------------
/*! \brief Access to this AppWindow (so we can create child dialogs).
 */

AppWindow *AppWindowPtr = 0;
AppWindow *appWindow( void )
{
    return( AppWindowPtr );
}

//------------------------------------------------------------------------------
/*! \brief AppWindow constructor.
 *
 *  The application is initialized in two parts.
 *  The first part, here in the constructor, shows the splash page,
 *  creates the application's EqApp and main widget, and sets a timer that
 *  starts the second part of the initialization.
 */

AppWindow::AppWindow( BehavePlusApp *bpApp, const QString &program,
        const QString &version, const QString &build,
        const QString &releaseNote ) :
    QMainWindow( 0, "applicationWindow", WDestructiveClose ),
    m_bpApp( bpApp ),
    m_program( program ),
    m_version( version ),
    m_build( build ),
    m_releaseNote( releaseNote ),
    m_xmlFile( "BehavePlus6.xml" ),
    m_startupFile( "BasicStart.bpw" ),
    m_startupWorkspace( "DefaultDataFolder" ),
    m_eqApp(0),
    m_release(0),
    m_docIdCount(0),
    m_saveSplash( false ),
    m_killArg( false ),
    m_openArg( false ),
    m_printArg( false ),
    m_runArg( false ),
    m_vb(0),
    m_workSpace(0),
    m_initTimer(0),
    m_propFont(0),
    m_fixedFont(0),
    m_calculateMenu(0),
    m_configMenu(0),
    m_configLangMenu(0),
    m_configUnitsMenu(0),
    m_fileMenu(0),
    m_fileExportMenu(0),
    m_fileExportFuelModelsMenu(0),
    m_fileSaveAsMenu(0),
    m_fileWorkspaceMenu(0),
    m_helpMenu(0),
    m_pagesMenu(0),
    m_toolsMenu(0),
    m_viewMenu(0),
    m_windowsMenu(0),
    m_toolBar(0),
    m_fileNewButton(0),
    m_fileOpenButton(0),
    //m_fileSaveButton(0),
    m_filePrintButton(0),
    m_docModulesButton(0),
    m_docRunButton(0),
    m_pageFirstButton(0),
    m_pageLastButton(0),
    m_pageNextButton(0),
    m_pagePrevButton(0),
    m_helpHelpButton(0),
    m_cameraIcon(0),
    m_checkedIcon(0),
    m_documentIcon(0),
    m_fileCopyIcon(0),
    m_fileDeleteIcon(0),
    m_fileNewIcon(0),
    m_fileOpenIcon(0),
    m_fileRunIcon(0),
    m_fileSaveIcon(0),
    m_filePrintIcon(0),
    m_helpIcon(0),
    m_modulesIcon(0),
    m_pageFirstIcon(0),
    m_pageLastIcon(0),
    m_pageNextIcon(0),
    m_pagePrevIcon(0),
    m_paperClipIcon(0),
    m_quitIcon(0)
{
    // Store this address in an accessible place.
    AppWindowPtr = this;
    hide();

    // Build the main window caption
    setCaption( m_program + " " + m_version );

    // Start a timer to call init() to perform startup sequence
    m_initTimer = new QTimer( this, "m_initTimer" );
    Q_CHECK_PTR( m_initTimer );
    connect( m_initTimer, SIGNAL( timeout() ),
             this,        SLOT( slotAppInit() ) );
    m_initTimer->start( 100, true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Application initialization callback function.
 *
 *  Invoked by a single-shot QTimer 1.0 second AFTER the main exec() loop
 *  to give the application a chance to perform startup chores.
 *  This lets us process command line options (such as opening documents)
 *  \e after the application GUI is ready to go.
 */

void AppWindow::slotAppInit( void )
{
    // Let the message handler know that we are GUI
    appGuiEnabled( true );

    // Create the application-wide, shared FileSystem names
    m_bpApp->updateSplashPage( "Locating installation directory ..." );
    log( "Beg Section: creating file system ...\n" );
    appFileSystemCreate();
    log( "End Section: creating file system completed.\n" );

    // Try to find the installation in the usual places
    // findInstallation() first checks if -home specified on the command line
    // otherwise it calls appFileSystem()->findInstallationDir()
    if ( ! findInstallation() )
    {
        // Notify user of any installation failure and quit.
        QMessageBox::critical( 0, QString( m_program + " " + m_version ),
            "A valid installation directory could not be found.<BR><BR>"
            "Locate the log file <B>BehavePlus.log</B> for details.",
            "Quit" );
        exit(1);
    }
    appFileSystem()->useDefaultWorkspace();

    // Create a new EqApp, which also reads the XML file and builds the
    // translation table, units converter, property dictionary, and file list.
    m_bpApp->updateSplashPage( "Reading definitions from XML file ..." );
    m_xmlFile = appFileSystem()->xmlFilePath();
    m_eqApp = new EqApp( m_xmlFile );
    checkmem( __FILE__, __LINE__, m_eqApp, "EqApp m_eqApp", 1 );

    // Get the release number
    m_release = m_eqApp->m_release;

    // Pass the file list to the FileSystem
    appFileSystem()->setFileList( m_eqApp->m_eqFileList );

    // Set the language for the translator
    appTranslatorSetLanguage( "en_US" );

    // Check and use any workspace mentioned on the command line.
    checkWorkspaceSwitch();

    // Test files if requested
    m_bpApp->updateSplashPage( "Checking installation files ..." );
    if ( ! testInstallation() )
    {
        exit(1);
    }

    // Try to read any existing application property file in the home directory
    QString propertyFile = appFileSystem()->propertyFilePath();
    QFileInfo fi( propertyFile );
    if ( fi.exists() && fi.isReadable() )
    {
        m_bpApp->updateSplashPage( "Loading property file ..." );
        appProperty()->readXmlFile( appFileSystem()->propertyFilePath() );
    }
    // If we want to force the page background color...
    // This is for Rob Seli User Guide preparation
    if ( false )
    {
        fprintf( stderr,
            "Forcing background color to 'gray90' at %s %d\n",
            __FILE__, __LINE__ );
        appProperty()->color( "pageBackgroundColor", "gray90" );
    }

    // Update these properties
    QPixmapCache::setCacheLimit( appProperty()->integer( "appPixmapCacheKSize" ) );

    // Does the user want big tool bar pixmaps and/or text?
    setUsesBigPixmaps( appProperty()->boolean( "appToolBarBigPixmaps" ) );
    setUsesTextLabel( appProperty()->boolean( "appToolBarTextLabels" ) );

    //---------------------------------------------------------------------
    // Create the user interface
    //---------------------------------------------------------------------

    m_bpApp->updateSplashPage( "Initializing main window ..." );

    // Set the application fonts to something we know we can scale smoothly
    m_propFont = new QFont( "Times New Roman", 12 );
    Q_CHECK_PTR( m_propFont );
    qApp->setFont( *m_propFont );
    m_fixedFont = new QFont( "Courier New", 12 );
    Q_CHECK_PTR( m_fixedFont );

    // Create a nice frame to hold the workspace
    m_vb = new QVBox( this, "m_vb" );
    Q_CHECK_PTR( m_vb );
    m_vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    setCentralWidget( m_vb );

    // Create the menu system
    initIconSets();
    initMenuFile();
    initMenuCalculate();
    initMenuView();
    initMenuConfigure();
    initMenuPages();
    initMenuWindows();
    initMenuTools();
    menuBar()->insertSeparator();
    initMenuHelp();

    // Workspace manager
    m_workSpace = new QWorkspace( m_vb, "m_workSpace" );
    Q_CHECK_PTR( m_workSpace );
    connect( m_workSpace, SIGNAL( windowActivated(QWidget*) ),
             this,        SLOT( slotStatusUpdate() ) );
    //m_workSpace->setBackgroundPixmap( *(m_splash->pixmap()) );
    QPixmap bgPixmap( canvas_xpm );
    //m_workSpace->setBackgroundPixmap( bgPixmap );
    m_workSpace->setPaletteBackgroundPixmap( bgPixmap );
    m_workSpace->setErasePixmap( bgPixmap );

    // Set the style
    qApp->setStyle( new QWindowsStyle );

    // Create the dockable tool bars
    initToolBar();

    // Determine starting size, display the status, and return.
    slotStatusUpdate();
    initResize();

    // Process all the command line arguments
    m_bpApp->updateSplashPage( "Processing command line options ..." );
    checkCommandLineSwitches();

    // Attach moisture scenarios
    m_bpApp->updateSplashPage( "Attaching moisture scenarios ..." );
    static const char *scenario[] =
    {
        "FuelModeling/d1l1.bpm",
        "FuelModeling/d1l2.bpm",
        "FuelModeling/d1l3.bpm",
        "FuelModeling/d1l4.bpm",
        "FuelModeling/d2l1.bpm",
        "FuelModeling/d2l2.bpm",
        "FuelModeling/d2l3.bpm",
        "FuelModeling/d2l4.bpm",
        "FuelModeling/d3l1.bpm",
        "FuelModeling/d3l2.bpm",
        "FuelModeling/d3l3.bpm",
        "FuelModeling/d3l4.bpm",
        "FuelModeling/d4l1.bpm",
        "FuelModeling/d4l2.bpm",
        "FuelModeling/d4l3.bpm",
        "FuelModeling/d4l4.bpm"
    };
    for ( unsigned i=0; i<16; i++ )
    {
        m_eqApp->attachMoisScenario(
            appFileSystem()->moisScenarioPath( scenario[i] ) );
    }

    // Show the main window
    m_bpApp->updateSplashPage( "Displaying BehavePlus main window ..." );
    show();

    // Close the splash page
    m_bpApp->updateSplashPage( "Initialization completed!" );
    m_bpApp->closeSplashPage();

    // Open the startup file
    openStartups();
    slotStatusUpdate();

    // Check for time expiration
//#define TEST_EXPIRATION 1
#undef TEST_EXPIRATION
#ifdef TEST_EXPIRATION
    int yr=2003, mon=9, day=1, hr=12, min=0, sec=0, ms=0;
    //fprintf( stderr, "JDate for %d/%02d/%02d %02d:%02d:%02d.%03d is %f\n",
    //  yr, mon, day, hr, min, sec, ms,
    //  CDT_JulianDate( yr, mon, day, hr, min, sec, ms ) );

    // Get the expiration date
    double julian = appProperty()->real( "appExpiration" );
    CDT_CalendarDate( julian, &yr, &mon, &day, &hr, &min, &sec, &ms );
    QDate expire( yr, mon, day );
    log( QString( "Expiration date is %1 (%2, %3, %4)." )
        .arg( julian, 0, 'f' )
        .arg( CDT_MonthAbbreviation( mon ) )
        .arg( day )
        .arg( yr ) );

    // Check expiration date against current date
    QDate today = QDate::currentDate();
    if ( today > expire )
    {
            QString errMsg("");
        translate( errMsg, "AppWindow:Expired",
            m_program,
            m_version,
            CDT_MonthAbbreviation( mon ),
            QString( "%1" ).arg( day ),
            QString( "%1" ).arg( yr ) );
        bomb( errMsg );
    }
#endif

    return;
}

//------------------------------------------------------------------------------
/*! \brief AppWindow destructor.
 */

AppWindow::~AppWindow( void )
{
    // Close all open documents
    Document *doc = 0;
    QWidgetList windows = m_workSpace->windowList();
    for ( int id = 0;
          id < int( windows.count() );
          ++id )
    {
        doc = (Document *) windows.at(id);
        doc->close();
        slotStatusUpdate();
    }
    // Store application's properties
    appProperty()->writeXmlFile( appFileSystem()->propertyFilePath(),
        "BehavePlus", m_release );

    // Cleanup
    delete m_cameraIcon;        m_cameraIcon = 0;
    delete m_checkedIcon;       m_checkedIcon = 0;
    delete m_documentIcon;      m_documentIcon = 0;
    delete m_fileCopyIcon;      m_fileCopyIcon = 0;
    delete m_fileDeleteIcon;    m_fileDeleteIcon = 0;
    delete m_fileNewIcon;       m_fileNewIcon = 0;
    delete m_fileOpenIcon;      m_fileOpenIcon = 0;
    delete m_fileRunIcon;       m_fileRunIcon = 0;
    delete m_fileSaveIcon;      m_fileSaveIcon = 0;
    delete m_filePrintIcon;     m_filePrintIcon = 0;
    delete m_helpIcon;          m_helpIcon = 0;
    delete m_modulesIcon;       m_modulesIcon = 0;
    delete m_pageFirstIcon;     m_pageFirstIcon = 0;
    delete m_pageLastIcon;      m_pageLastIcon = 0;
    delete m_pageNextIcon;      m_pageNextIcon = 0;
    delete m_pagePrevIcon;      m_pagePrevIcon = 0;
    delete m_paperClipIcon;     m_paperClipIcon = 0;
    delete m_quitIcon;          m_quitIcon = 0;

    delete m_fileNewButton;     m_fileNewButton = 0;
    delete m_fileOpenButton;    m_fileOpenButton = 0;
    //delete m_fileSaveButton;    m_fileSaveButton = 0;
    delete m_filePrintButton;   m_filePrintButton = 0;
    delete m_docModulesButton;  m_docModulesButton = 0;
    delete m_docRunButton;      m_docRunButton = 0;
    delete m_pageFirstButton;   m_pageFirstButton = 0;
    delete m_pageLastButton;    m_pageLastButton = 0;
    delete m_pageNextButton;    m_pageNextButton = 0;
    delete m_pagePrevButton;    m_pagePrevButton = 0;
    delete m_helpHelpButton;    m_helpHelpButton = 0;
    delete m_toolBar;           m_toolBar = 0;

    delete m_configMenu;        m_configMenu = 0;
    delete m_configLangMenu;    m_configLangMenu = 0;
    delete m_configUnitsMenu;   m_configUnitsMenu = 0;
    delete m_fileMenu;          m_fileMenu = 0;
    delete m_fileExportMenu;    m_fileExportMenu = 0;
    delete m_fileExportFuelModelsMenu;  m_fileExportFuelModelsMenu = 0;
    delete m_fileSaveAsMenu;    m_fileSaveAsMenu = 0;
    delete m_fileWorkspaceMenu; m_fileWorkspaceMenu = 0;
    delete m_helpMenu;          m_helpMenu = 0;
    delete m_pagesMenu;         m_pagesMenu = 0;
    delete m_toolsMenu;         m_toolsMenu = 0;
    delete m_viewMenu;          m_viewMenu = 0;
    delete m_windowsMenu;       m_windowsMenu = 0;

    delete m_propFont;          m_propFont = 0;
    delete m_fixedFont;         m_fixedFont = 0;
    delete m_initTimer;         m_initTimer = 0;

    delete m_workSpace;         m_workSpace = 0;
    delete m_vb;                m_vb = 0;
    delete m_eqApp;             m_eqApp = 0;
    return;
}

//------------------------------------------------------------------------------
/*  \brief Scans the command line for following switches:
 *  -   -run <runFile> opens and runs the specified file
 *  -   -open <runFile> opens the specified file
 *  -   -kill exists the program after completing this function
 *            (used for coverage testing)
 *  -   -splash causes Help-Splash to save the splash screen to a BMP file
 * -    -coverage performs coverage tests and exits.
 */

void AppWindow::checkCommandLineSwitches( void )
{
    log( "Beg Section: Checking command line switches\n" );
    QString l_caption(""), text("");
    for ( int i = 1;
          i < qApp->argc();
          i++ )
    {
        // -workspace <dirName> was already processed
        if ( strncmp( qApp->argv()[i], "-workspace", 2 ) == 0 )
        {
            log( "Found -workspace switch\n" );
            i++;        // Skip its value argument
        }
        // -home <dirName> was already processed
        else if ( strncmp( qApp->argv()[i], "-home", 4 ) == 0 )
        {
            log( "Found -home switch\n" );
            i++;        // Skip its value argument
        }
        // -html prints all HTML files for this release
        else if ( strncmp( qApp->argv()[i], "-html", 4 ) == 0 )
        {
            log( "Found -html switch\n" );
            i++;
            appFileSystem()->printHtmlFiles( 0 );
        }
        // -kill automatically kills the program at the end of this function
        else if ( strncmp( qApp->argv()[i], "-kill", 2 ) == 0 )
        {
            log( "Found -kill switch\n" );
            m_killArg = true;
        }
        // -splash causes Help-Splash to save the splash page to a BMP file
        else if ( strncmp( qApp->argv()[i], "-splash", 2 ) == 0 )
        {
            log( "Found -splash switch\n" );
            m_saveSplash = true;
        }
        // -print causes the current doc to be printed to PS file
        else if ( strncmp( qApp->argv()[i], "-print", 2 ) == 0 )
        {
            log( "Found -print switch\n" );
            m_printArg = true;
        }
        // -test was already processed
        else if ( strncmp( qApp->argv()[i], "-test", 2 ) == 0 )
        {
            log( "Found -test switch\n" );
        }
        // -coverage tests
        else if ( strncmp( qApp->argv()[i], "-coverage", 2 ) == 0 )
        {
            log( "Found -coverage switch\n" );
            // There must be a fileName argument
            if ( i == qApp->argc()-1 )
            {
                log( "-coverage switch is missing its argument\n" );
                translate( text, "AppWindow:MissingArg", qApp->argv()[i] );
                error( text );
                platformExit(1);
            }
            //int testId = atoi( qApp->argv()[ ++i ] );
            log( "Running coverage tests...\n" );
            log( "Whoops ... coverage tests not included in this version.\n" );
            //coverage( testId );
        }
        // "-run <fileName>"
        else if ( strncmp( qApp->argv()[i], "-run", 2 ) == 0 )
        {
            log( "Found -run switch\n" );
            // There must be a fileName argument
            if ( i == qApp->argc()-1 )
            {
                log( "-run switch is missing its argument.\n" );
                translate( text, "AppWindow:MissingArg", qApp->argv()[i] );
                error( text );
                platformExit(1);
            }
            m_runArg = true;
            m_startupFile = qApp->argv()[i+1];
            i++;        // Skip its value argument
        }
        // "-open <fileName>"
        else if ( strncmp( qApp->argv()[i], "-open", 2 ) == 0 )
        {
            log( "Found -open switch\n" );
            // There must be a fileName argument
            if ( i == qApp->argc()-1 )
            {
                log( "-open switch is missing its argument.\n" );
                translate( text, "AppWindow:MissingArg", qApp->argv()[i] );
                error( text );
                platformExit(1);
            }
            m_openArg = true;
            m_startupFile = qApp->argv()[i+1];
            i++;        // Skip its value argument
        }
        // All other arguments are unknown.
        else
        {
            log( QString( "Found unknown switch \"%1\"\n" )
                .arg( qApp->argv()[i] ) );
            translate( l_caption, "AppWindow:UnknownArg:Caption" );
            translate( text, "AppWindow:UnknownArg:Text", qApp->argv()[i] );
            warn( l_caption, text );
        }
    }
    log( "End Section: Checking command line switches\n" );
    return;
}

//------------------------------------------------------------------------------
/*  \brief Trys to open any -run, -open, or property appStartup files.
 *
 *  \param fileName Full path name to .bpr or .bpw file to try to open
 *  \param run      If TRUE, runs the file.
 */

void AppWindow::openStartups( void )
{
    bool status = false;
    if ( m_openArg )
    {
        status = openStartupFile( m_startupFile, false, m_printArg );
    }
    else if ( m_runArg )
    {
        status = openStartupFile( m_startupFile, true, m_printArg );
    }
    // Try to open the appStartup property document
    if ( ! status )
    {
        QString workspacePath = appFileSystem()->workspacePath();
        // If there is no property.xml, then appStartup comes from the
        // default definition in BehavePlus6.xml, so it needs a path
        m_startupFile = appProperty()->string( "appStartup" );
        if ( m_startupFile.startsWith( "%PATH%" ) )
        {
            m_startupFile.remove( 0, 6 );
            m_startupFile.prepend( workspacePath );
        }
        status = openStartupFile( m_startupFile, false, false );
        if ( ! status )
        {
warn( "Unable to Find Startup File", m_startupFile );
            // Try to open a known worksheet
            m_startupFile = "WorksheetFolder/ExampleWorksheets/BasicStart.bpw";
            m_startupFile.prepend( workspacePath );
            status = openStartupFile( m_startupFile, false, false );
        }
    }
    if ( ! status )
    {
warn( "Unable to Find Default Installation Startup File", m_startupFile );
        m_startupFile = "";
    }
    //fprintf( stderr, "Using startup worksheet '%s'\n", m_startupFile.latin1() );
    // Terminate the program if requested
    if ( m_killArg )
    {
        log( "Killing the run ...\n" );
        platformExit(0);
    }
    return;
}

//------------------------------------------------------------------------------
/*  \brief Attempts to open the specified .bpw or .bpr file.
 *
 *  \param fileName Full path name to .bpr or .bpw file to try to open
 *  \param run      If TRUE, runs the file.
 *  \param print    If TRUE, prints the file.
 */

bool AppWindow::openStartupFile( const QString &fileName, bool run, bool print )
{
    Document *doc = 0;
    log( "Beg Section: Opening startup file ...\n" );
    log( QString( "Opening startup file '%1' ...\n" ).arg( fileName ) );

    QString fileType = appFileSystem()->type( fileName );
    if ( ! ( doc = openDocument( fileName, fileType, false ) ) )
    {
        QString l_caption, text;
        log( "FAILED to open the startup file.\n" );
        translate( l_caption, "AppWindow:NoOpenDoc:Caption" );
        translate( text, "AppWindow:NoOpenDoc:Text", fileName );
        error( l_caption, text );
        log( "End Section: Opening startup file.\n" );
        return( false );
    }
    // Show the document
    // NOTE : only this particular sequence opens the doc maximized!
    doc->setFocus();
    qApp->processEvents();
    doc->showMaximized();
    doc->m_scrollView->setContentsPos( 0, 0 );

    // Run it if requested
    if ( run )
    {
        log( "-run running and printing the document ....\n" );
        doc->run( false );
        // NOTE : only this particular sequence opens the doc maximized!
        doc->setFocus();
        qApp->processEvents();
        doc->showMaximized();
        doc->m_scrollView->setContentsPos( 0, 0 );
    }
    // Print the run if requested
    if ( print )
    {
        doc->printPS( 1, 9999 );
    }
    log( "Opened.\n" );
    log( "End Section: Opening startup file.\n" );
    return( true );
}

//------------------------------------------------------------------------------
/*  \brief Scans the command line for any -workspace directory arguments and
 *  validates them.  If none are found, the installation default data
 *  directory is used.
 */

void AppWindow::checkWorkspaceSwitch( void )
{
    log( "Beg Section: Checking command line for -workspace argument\n" );
    QString end( "End Section: Checking command line for -workspace argument\n" );
    // Assume use of default data directory unless -workspace says otherwise
    QString dirName = appFileSystem()->defaultWorkspacePath();
    QString text(""), l_caption(""), errMsg("");
    // Check command line for any -workspace arguments
    bool found = false;
    for ( int i = 1;
          i < qApp->argc();
          i++ )
    {
        // "-workspace <dirName>"
        if ( strncmp( qApp->argv()[i], "-workspace", 2 ) == 0 )
        {
            // Can't be the last argument
            if ( i == qApp->argc()-1 )
            {
                translate( text, "AppWindow:MissingArg", "-workspace" );
                log( text );
                error( text );
                log( end );
                platformExit(1);
            }
            // Get the folder name and add a separator
            dirName = QString( qApp->argv()[i+1] ) + QDir::separator();
            found = true;
            break;
        }
    }
    if ( ! found )
    {
        log( "-workspace command line switch not found.\n" );
        log( end );
        return;
    }
    // Check the data directory structure and files
    if ( ! appFileSystem()->testWorkspaceFiles( dirName, errMsg ) )
    {
        translate( l_caption, "AppWindow:InvalidWorkspace:Caption" );
        if ( dirName == appFileSystem()->defaultWorkspacePath() )
        {
            translate( text, "AppWindow:InvalidDefaultWorkspace:Text",
                dirName, errMsg );
            log( text );
            error( l_caption, text );
            log( end );
            platformExit(1);
        }
        translate( text, "AppWindow:InvalidWorkspace:Text", dirName, errMsg );
        warn( l_caption, text );
        return;
    }
    // This is a good workspaceDir
    appFileSystem()->workspacePath( dirName );
    log( "Valid -workspace command line switch was found.\n" );
    log( end );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Checks if all the original distribution files exist.
 *  Displays a warning message showing any missing files.
 */

bool AppWindow::testInstallation( void )
{
    log( "Beg Section: Testing installation file structure ...\n" );
    QString errMsg(""), l_caption(""), text("");
    // Check all image files in <INSTALLDIR>/ImageFolder
    int ok = true;
    log( "Skipping image file checks ..." );
    // Disabled for 2.0.2
    if ( false && ! appFileSystem()->testImageFiles( errMsg ) )
    {
        translate( text, "AppWindow:MissingImageFiles",
            appFileSystem()->imagePath(), errMsg );
        QMessageBox::critical( 0, QString( m_program + " " + m_version ),
            text, "Quit" );
        warn( l_caption, text );
        ok = false;
        log( QString( "missing files: %1\n" ).arg( text ) );
    }
    else
    {
        log( "ok.\n" );
    }
    // Check all HTML files in <INSTALLDIR>/DocFolder/<LANGUAGE>/Html
    errMsg = "";
    log( "Skipping documentation file checks ..." );
    // Disabled for 2.0.2
    if ( false && ! appFileSystem()->testHtmlFiles( errMsg ) )
    {
        translate( l_caption, "AppWindow:MissingFiles" );
        translate( text, "AppWindow:MissingDocFiles",
            appFileSystem()->docHtmlPath(), errMsg );
        warn( l_caption, text );
        ok = false;
        log( QString( "missing files: %1\n" ).arg( text ) );
    }
    else
    {
        log( "ok.\n" );
    }
    // Check all geographic location files in <INSTALLDIR>/DocFolder/GeoFolder
    errMsg = "";
    log( "Skipping geographic location file checks ..." );
    // Disabled for 2.0.2
    if ( false && ! appFileSystem()->testGeoFiles( errMsg ) )
    {
        translate( l_caption, "AppWindow:MissingFiles" );
        translate( text, "AppWindow:MissingGeoFiles",
            appFileSystem()->docGeoPath(), errMsg );
        warn( l_caption, text );
        ok = false;
        log( QString( "missing files: %1\n" ).arg( text ) );
    }
    else
    {
        log( "ok.\n" );
    }
    // Check the default workspace files
    log( "Skipping whecking workspace file checks ..." );
    // Disabled for 2.0.2
    if ( false && ! appFileSystem()->testWorkspaceFiles(
            appFileSystem()->defaultWorkspacePath(), errMsg ) )
    {
        translate( l_caption, "AppWindow:MissingFiles" );
        translate( text, "AppWindow:MissingWorkspaceFiles",
            appFileSystem()->defaultWorkspacePath(), errMsg );
        warn( l_caption, text );
        ok = false;
        log( QString( "missing files: %1\n" ).arg( text ) );
    }
    else
    {
        log( "ok.\n" );
    }
    // Let us know if everything is ok
    if ( ok )
    {
        //translate( text, "AppWindow:FilesComplete" );
        //info( text );
        log( "Complete set of data files found.\n" );
    }
    log( "End Section: Testing installation file structure completed.\n" );
    return( ok );
}

//------------------------------------------------------------------------------
/*  \brief Checks if the application's installation directory was sepcified
 *  on the command line, and if so, validates it.
 *
 *      If -home wasn't specified, checks in the usual places
 *      (environment variables, resgistry).
 *
 *  \return On return FileSystem::m_installDir is set to the named or found
 *  installation directory, and FileSystem::m_workspaceDir is set to the default
 *  data directory located under the installation directory.  If no valid
 *  installation directory is found, an error message is displayed and
 *  the application terminates.
 */

bool AppWindow::findInstallation( void )
{
    log( "Beg Section: Checking command line for -home argument\n" );
    QString end( "End Section: Checking command line for -home argument\n" );
    QString text("");
    // Check command line for any -home arguments
    for ( int i = 1;
          i < qApp->argc();
          i++ )
    {
        // "-home <dirName>"
        if ( strncmp( qApp->argv()[i], "-home", 4 ) == 0 )
        {
            // Can't be the last argument
            if ( i == qApp->argc()-1 )
            {
                log( "-home is missing its argument.\n" );
                QMessageBox::critical( 0, QString( m_program + " " + m_version ),
                    "The <B>-home</B> command line argument is missing its value.",
                    "Quit" );
                log( end );
                return( false );
            }
            QString dirName = qApp->argv()[i+1];
            // Check if this is a valid installation directory
            // (since the splash page and XML file have already been read,
            // we don't need to call findInstallationStartupFiles().
            QString errMsg( "" );
            if ( ! appFileSystem()->isValidInstallation( dirName, errMsg ) )
            {
                log( QString( "-home %1 is an invalid installation:\n" )
                    .arg( dirName ) );
                log( QString( "    %1\n").arg( errMsg ) );
                QMessageBox::critical( 0, QString( m_program + " " + m_version ),
                    QString( "The <B>-home %1</B> command line argument "
                    "specifies an invalid installation directory.<BR><BR>"
                    "Locate the file <B>BehavePlus.log</B> for details.")
                    .arg( dirName ), "Quit" );
                log( end );
                return( false );
            }
            // This is a good installation directory
            appFileSystem()->installPath( dirName );
            appFileSystem()->useDefaultWorkspace();
            log( QString( "-home %1 is a valid installation.\n" )
                .arg( dirName ) );
            log( end );
            return( true );
            log( end );
        }
    }
    log( "-home command line switch not found.\n" );
    log( end );

    // Try to find the installation in the standard places
    if ( ! appFileSystem()->findInstallationDir( text ) )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Access to the current active Document.
 *
 *  While this will print an error message if there is no active Document,
 *  this should never happen if all menu items and buttons are properly
 *  inactivated when there are no Documents.
 *
 *  \param docType  If not empty, specifies that the current active window
 *                  must be of the specified document type.
 *
 *  \retval Pointer to the current active Document, if there is one.
 *  \retval Zero if there is no active Document.
 */

Document *AppWindow::getActiveWindow( const QString &docType )
{
    Document *doc = (Document *) m_workSpace->activeWindow();
    if ( ( ! doc )
      || ( ! docType.isEmpty() && doc->m_docType != docType ) )
    {
        QString msg("");
        translate( msg, "AppWindow:OpenBehavePlusFile" );
        error( msg );
        return( 0 );
    }
//fprintf( stderr, "AppWindow::getActiveWindow(%s) returns Doc %x id %d page %d of %d\n",
//docType.latin1(), doc, doc->m_docId, doc->m_page, doc->m_pages);
    return( doc );
}

//------------------------------------------------------------------------------
/*! \brief Converts all the internal, shared XPM's into QIconSets.
 */

void AppWindow::initIconSets( void )
{
    // Optimize for Windoze
    QPixmap::setDefaultOptimization( QPixmap::MemoryOptim );

    // Note that QIconSet makes its own deep copy of the passed QPixmap
    m_cameraIcon      = new QIconSet( QPixmap( camera_xpm ) );
    Q_CHECK_PTR( m_cameraIcon );
    m_checkedIcon     = new QIconSet( QPixmap( checked_xpm ) );
    Q_CHECK_PTR( m_checkedIcon );
    m_documentIcon    = new QIconSet( QPixmap( document_xpm ) );
    Q_CHECK_PTR( m_documentIcon );
    m_fileCopyIcon    = new QIconSet( QPixmap( filecopy_xpm ) );
    Q_CHECK_PTR( m_fileCopyIcon );
    m_fileDeleteIcon  = new QIconSet( QPixmap( filedelete_xpm ) );
    Q_CHECK_PTR( m_fileDeleteIcon );
    m_fileNewIcon     = new QIconSet( QPixmap( filenew_xpm ) );
    Q_CHECK_PTR( m_fileNewIcon );
    m_fileOpenIcon    = new QIconSet( QPixmap( fileopen_xpm ) );
    Q_CHECK_PTR( m_fileOpenIcon );
    m_fileRunIcon     = new QIconSet( QPixmap( filerun_xpm ) );
    Q_CHECK_PTR( m_fileRunIcon );
    m_fileSaveIcon    = new QIconSet( QPixmap( filesave_xpm ) );
    Q_CHECK_PTR( m_fileSaveIcon );
    m_filePrintIcon   = new QIconSet( QPixmap( fileprint_xpm ) );
    Q_CHECK_PTR( m_filePrintIcon );
    m_helpIcon        = new QIconSet( QPixmap( help_xpm ) );
    Q_CHECK_PTR( m_helpIcon );
    m_modulesIcon     = new QIconSet( QPixmap( modules_xpm ) );
    Q_CHECK_PTR( m_modulesIcon );
    m_pageFirstIcon   = new QIconSet( QPixmap( pagefirst_xpm ) );
    Q_CHECK_PTR( m_pageFirstIcon);
    m_pageLastIcon    = new QIconSet( QPixmap( pagelast_xpm ) );
    Q_CHECK_PTR( m_pageLastIcon );
    m_pagePrevIcon    = new QIconSet( QPixmap( pageprev_xpm ) );
    Q_CHECK_PTR( m_pagePrevIcon );
    m_pageNextIcon    = new QIconSet( QPixmap( pagenext_xpm ) );
    Q_CHECK_PTR( m_pageNextIcon    );
    m_paperClipIcon   = new QIconSet( QPixmap( paperclip_xpm ) );
    Q_CHECK_PTR( m_paperClipIcon );
    m_quitIcon        = new QIconSet( QPixmap( burnit_xpm ) );
    Q_CHECK_PTR( m_quitIcon );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Calculate submenu.
 */

void AppWindow::initMenuCalculate( void )
{
    // File menu
    m_calculateMenu = new QPopupMenu( this, "m_calculateMenu" );
    Q_CHECK_PTR( m_calculateMenu );
    QString text("");

    // Calculate
    translate( text, "Menu:Calculate:Calculate" );
    m_idFileCalculate = m_calculateMenu->insertItem( *m_fileRunIcon, text,
        this, SLOT( slotDocumentRun() ) );

    // Add Calculate menu to the menu bar
    translate( text, "Menu:Calculate" );
    m_idConfig = menuBar()->insertItem( text, m_calculateMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Configure submenu.
 */

void AppWindow::initMenuConfigure( void )
{
    // Configure menu
    m_configMenu = new QPopupMenu( this, "m_configMenu" );
    Q_CHECK_PTR( m_configMenu );
    QString text("");

    // Modules selection button
    translate( text, "Menu:Configure:Module" );
    m_idConfigModule = m_configMenu->insertItem( *m_modulesIcon, text,
        this, SLOT( slotConfigureModules() ) );

    // Separator
    m_configMenu->insertSeparator();

    // Attach fuel models button
    translate( text, "Menu:Configure:FuelModels" );
    m_idConfigFuel = m_configMenu->insertItem( text,
        this, SLOT( slotConfigureFuelModels() ) );

    // Attach moisture scenario button
    translate( text, "Menu:Configure:MoistureScenarios" );
    m_idConfigMois = m_configMenu->insertItem( text,
        this, SLOT( slotConfigureMoistureScenarios() ) );

    // Units submenu
    {
        m_configUnitsMenu = new QPopupMenu( this, "m_configUnitsMenu" );
        Q_CHECK_PTR( m_configUnitsMenu );

        translate( text, "Menu:Configure:Units:English" );
        m_idConfigUnitsEnglish = m_configUnitsMenu->insertItem( text,
            this, SLOT( slotConfigureUnitsEnglish() ) );

        translate( text, "Menu:Configure:Units:Metric" );
        m_idConfigUnitsMetric = m_configUnitsMenu->insertItem( text,
            this, SLOT( slotConfigureUnitsMetric() ) );

        translate( text, "Menu:Configure:Units:Custom" );
        m_idConfigUnitsCustom = m_configUnitsMenu->insertItem( text,
            this, SLOT( slotConfigureUnitsCustom() ) );

        // Add it to the Configure menu
        translate( text, "Menu:Configure:Units" );
        m_idConfigUnits = m_configMenu->insertItem( text, m_configUnitsMenu );
    }

    // Language submenu
#undef INCLUDE_LANGUAGES
#ifdef INCLUDE_LANGUAGES
    if ( m_release > 10000 )
    {
        m_configLangMenu = new QPopupMenu( this, "m_configLangMenu" );
        Q_CHECK_PTR( m_configLangMenu );

        QString *lang, langKey( "" );
        int id = 0;
        for ( lang = m_eqApp->m_languageList->first();
              lang != 0;
              lang = m_eqApp->m_languageList->next(), id++ )
        {
            langKey = "Menu:Configure:Language:" + *lang;
            translate( text, langKey );
            m_idConfigLanguage[id] = m_configLangMenu->insertItem(
                text, this, SLOT( slotConfigureLanguage(int) ) );
        }

        // Add it to the Configure menu
        translate( text, "Menu:Configure:Language" );
        m_idConfigLang = m_configMenu->insertItem( text, m_configLangMenu );
    }
#endif

    // Separator
    m_configMenu->insertSeparator();

    // Custom units definition moved here from the Tools menu
    translate( text, "Menu:Tools:UnitsEditor" );
    m_idToolsUnitsEditor = m_configMenu->insertItem( text,
        this, SLOT( slotToolsUnitsEditor() ) );

    // Appearance button
    translate( text, "Menu:Configure:Appearance" );
    m_idConfigAppearance = m_configMenu->insertItem( text,
        this, SLOT( slotConfigureAppearance() ) );

    // Add Configure menu to the menu bar
    translate( text, "Menu:Configure" );
    m_idConfig = menuBar()->insertItem( text, m_configMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b File submenu.
 */

void AppWindow::initMenuFile( void )
{
    // File menu
    m_fileMenu = new QPopupMenu( this, "m_fileMenu" );
    Q_CHECK_PTR( m_fileMenu );
    QString text("");

    // File->Open worksheet
    translate( text, "Menu:File:New" );
    m_idFileNew = m_fileMenu->insertItem( *m_fileNewIcon, text,
        this, SLOT( slotDocumentNew() ) );

    // File->Open run
    translate( text, "Menu:File:Open" );
    m_idFileOpen = m_fileMenu->insertItem( *m_fileOpenIcon, text,
        this, SLOT( slotDocumentOpen() ) );

    // File->Clone run
    //translate( text, "Menu:File:Clone" );
    //m_idFileClone = m_fileMenu->insertItem( text,
    //    this, SLOT( slotDocumentClone() ) );

    // Separator
    m_fileMenu ->insertSeparator();

    // File->Save as worksheet
    translate( text, "Menu:File:SaveAs:Worksheet" );
    m_idFileSaveAsWorksheet = m_fileMenu->insertItem( text,
        this, SLOT( slotDocumentSaveAsWorksheet() ) );

    // File->Save as run
    translate( text, "Menu:File:SaveAs:Run" );
    m_idFileSaveAsRun = m_fileMenu->insertItem( text,
        this, SLOT( slotDocumentSaveAsRun() ) );

    // Fuel models submenu
    {
        // File->Save as fuel model
        m_fileExportFuelModelsMenu = new QPopupMenu( this,
            "m_fileExportFuelModelsMenu" );
        Q_CHECK_PTR( m_fileExportFuelModelsMenu );

        // File->Save as fuel model->BehavePlus format
        translate( text, "Menu:File:SaveAs:FuelModel" );
        m_idFileSaveAsFuelModel = m_fileExportFuelModelsMenu->insertItem( text,
            this, SLOT( slotDocumentSaveAsFuelModel() ) );

        // File->Save as fuel model->FARSITE format, English
        translate( text, "Menu:File:Export:FuelModels:FarsiteEnglish" );
        m_idFileExportFuelModelsFarsiteEnglish = m_fileExportFuelModelsMenu->insertItem(
            text, this, SLOT( slotDocumentExportFuelModelsFarsiteEnglish() ) );

		// File->Save as fuel model->FARSITE format, metric
        translate( text, "Menu:File:Export:FuelModels:FarsiteMetric" );
        m_idFileExportFuelModelsFarsiteMetric = m_fileExportFuelModelsMenu->insertItem(
            text, this, SLOT( slotDocumentExportFuelModelsFarsiteMetric() ) );

        // Attach submenu to File menu
        translate( text, "Menu:File:Export:FuelModels" );
        m_idFileExportFuelModels = m_fileMenu->insertItem( text,
            m_fileExportFuelModelsMenu );
    }

    // File->Save as moisture scenario
    translate( text, "Menu:File:SaveAs:MoistureScenario" );
    m_idFileSaveAsMoistureScenario = m_fileMenu->insertItem( text,
        this, SLOT( slotDocumentSaveAsMoistureScenario() ) );

    // File->Image
    translate( text, "Menu:File:Capture" );
    m_idFileCapture = m_fileMenu->insertItem( *m_cameraIcon, text,
        this, SLOT( slotDocumentCapture() ) );

    // Separator
    m_fileMenu ->insertSeparator();

    // File->Export results
    translate( text, "Menu:File:Export:Results" );
    m_idFileExportResults = m_fileMenu->insertItem( text,
        this, SLOT( slotDocumentExportResults() ) );

    // Separator
    m_fileMenu ->insertSeparator();

    // Calculate
    //translate( text, "Menu:File:Calculate" );
    //m_idFileCalculate = m_fileMenu->insertItem( *m_fileRunIcon, text,
    //    this, SLOT( slotDocumentRun() ) );

    // File->Print
    translate( text, "Menu:File:Print" );
    m_idFilePrint = m_fileMenu->insertItem( *m_filePrintIcon, text,
        this, SLOT( slotDocumentPrint() ) );

    // Separator
    m_fileMenu ->insertSeparator();

    // File->Clear inputs
    //translate( text, "Menu:File:Clear" );
    //m_idFileClear = m_fileMenu->insertItem( text,
    //    this, SLOT( slotDocumentClear() ) );

    // File->Reset inputs
    //translate( text, "Menu:File:Reset" );
    //m_idFileReset = m_fileMenu->insertItem( text,
    //    this, SLOT( slotDocumentReset() ) );

    // File->Close
    translate( text, "Menu:File:Close" );
    m_idFileClose = m_fileMenu->insertItem( text,
        this, SLOT( slotDocumentClose() ) );

    // Separator
    m_fileMenu->insertSeparator();

    // Workspace
    // translate( text, "Menu:File:Workspace:Change" );
    // m_idFileWorkspaceOpen = m_fileMenu->insertItem( text,
    //    this, SLOT( slotDocumentWorkspaceOpen() ) );

    // File->Set startup worksheet
    translate( text, "Menu:File:SetStartupWorksheet" );
    m_idFileClone = m_fileMenu->insertItem( text,
        this, SLOT( slotSetStartupWorksheet() ) );

    // Workspace submenu
    m_fileWorkspaceMenu = new QPopupMenu( this,
        "m_fileWorkspaceMenu" );
    Q_CHECK_PTR( m_fileWorkspaceMenu );

    translate( text, "Menu:File:Workspace:Open" );
    m_idFileWorkspaceOpen = m_fileWorkspaceMenu->insertItem( text,
        this, SLOT( slotDocumentWorkspaceOpen() ) );

    translate( text, "Menu:File:Workspace:New" );
    m_idFileWorkspaceNew = m_fileWorkspaceMenu->insertItem( text,
        this, SLOT( slotDocumentWorkspaceNew() ) );

    translate( text, "Menu:File:Workspace:Clone" );
    m_idFileWorkspaceClone = m_fileWorkspaceMenu->insertItem( text,
        this, SLOT( slotDocumentWorkspaceClone() ) );

    translate( text, "Menu:File:Workspace" );
    m_idFileWorkspace = m_fileMenu->insertItem( text,
        m_fileWorkspaceMenu );

    // Quit
    translate( text, "Menu:File:Quit" );
    m_idFileQuit = m_fileMenu->insertItem( *m_quitIcon, text,
        this, SLOT( slotQuit() ) );
        //qApp, SLOT( closeAllWindows() ) );

    // Add File menu to the menu bar
    translate( text, "Menu:File" );
    m_idFile = menuBar()->insertItem( text, m_fileMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Help submenu.
 */

void AppWindow::initMenuHelp( void )
{
    // Help menu
    m_helpMenu = new QPopupMenu( this, "m_helpMenu" );
    Q_CHECK_PTR( m_helpMenu );

    // About BehavePlus (WAS About)
    QString text("");
    translate( text, "Menu:Help:AboutBehavePlus" );
    m_idHelpAboutBehavePlus = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpAboutBehavePlus() ) );

    // About help (NEW)
    translate( text, "Menu:Help:AboutHelp" );
    m_idHelpAboutHelp = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpAboutHelp() ) );

    // User Guide (WAS Program help)
    //translate( text, "Menu:Help:UsersGuide" );
    //m_idHelpUsersGuide = m_helpMenu->insertItem( text,
    //    this, SLOT( slotHelpUsersGuide() ) );
    translate( text, "Menu:Help:FeaturesPaper" );
    m_idHelpUsersGuide = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpFeaturesPaper() ) );

    // Variable Help (replaces Variable description / Variable index)
    translate( text, "Menu:Help:VariableHelp" );
    m_idHelpVariableHelp = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpVariableHelp() ) );

    // Master Index
    translate( text, "Menu:Help:MasterIndex" );
    m_idHelpMasterIndex = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpMasterIndex() ) );

    // Separator
    m_helpMenu->insertSeparator();

    // Splash page
    translate( text, "Menu:Help:SplashPage" );
    m_idHelpSplashPage = m_helpMenu->insertItem( text,
        this, SLOT( slotSplashPageShow() ) );

    // Installation info
    translate( text, "Menu:Help:InstallationInfo" );
    m_idHelpInstallationInfo = m_helpMenu->insertItem( text,
        this, SLOT( slotHelpInstallationInfo() ) );

    // Add Help menu to the menu bar
    translate( text, "Menu:Help" );
    m_idHelp = menuBar()->insertItem( text, m_helpMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Pages submenu.
 */

void AppWindow::initMenuPages( void )
{
    // Contents (Page) menu
    m_pagesMenu = new QPopupMenu( this, "pagesMenu" );
    Q_CHECK_PTR( m_pagesMenu );

    // Dynamically build this menu on every invocation
    m_pagesMenu->setCheckable( TRUE );
    connect( m_pagesMenu, SIGNAL( aboutToShow() ),
             this,           SLOT( slotPagesMenuAboutToShow() ) );
    connect( m_pagesMenu, SIGNAL( activated(int) ),
             this,           SLOT( slotPagesMenuActivated(int) ) );

    // Add Pages menu to the menu bar
    QString text("");
    translate( text, "Menu:Pages" );
    m_idPages = menuBar()->insertItem( text, m_pagesMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Tools submenu.
 */

void AppWindow::initMenuTools( void )
{
    // Tools menu
    m_toolsMenu = new QPopupMenu( this, "m_toolsMenu" );
    Q_CHECK_PTR( m_toolsMenu );

    QString text("");
    translate( text, "Menu:Tools:FdfmcDialog" );
    m_idToolsFdfmcDialog = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsFdfmcDialog() ) );

    translate( text, "Menu:Tools:HumidityDialog" );
    m_idToolsHumidityDialog = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsHumidityDialog() ) );

    translate( text, "Menu:Tools:UnitsConverter" );
    m_idToolsUnitsConverter = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsUnitsConverter() ) );

    translate( text, "Menu:Tools:SlopeFromMapMeasurements" );
    m_idToolsSlopeFromMapMeasurements = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsSlopeFromMapMeasurements() ) );

    translate( text, "Menu:Tools:HorizontalDistance" );
    m_idToolsHorizontalDistance = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsHorizontalDistance() ) );

    translate( text, "Menu:Tools:SunCalendar" );
    m_idToolsSunCal = m_toolsMenu->insertItem( text,
        this, SLOT( slotToolsSunMoonCalendar() ) );

    // Moved to Configure menu
    //translate( text, "Menu:Tools:UnitsEditor" );
    //m_idToolsUnitsEditor = m_toolsMenu->insertItem( text,
    //    this, SLOT( slotToolsUnitsEditor() ) );

    // Add Tools menu to the menu bar
    translate( text, "Menu:Tools" );
    m_idTools = menuBar()->insertItem( text, m_toolsMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b View submenu.
 */

void AppWindow::initMenuView( void )
{
    // View menu
    m_viewMenu = new QPopupMenu( this, "m_viewMenu" );
    Q_CHECK_PTR( m_viewMenu );

    // Dynamically built on invocation
    m_viewMenu->setCheckable( TRUE );
    connect( m_viewMenu, SIGNAL( aboutToShow() ),
             this,       SLOT( slotViewMenuAboutToShow() ) );

    // Add View menu to the menu bar
    QString text("");
    translate( text, "Menu:View" );
    m_idView = menuBar()->insertItem( text, m_viewMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's main menu \b Windows submenu.
 */

void AppWindow::initMenuWindows( void )
{
    // Windows menu
    m_windowsMenu = new QPopupMenu( this, "m_windowsMenu" );
    Q_CHECK_PTR( m_windowsMenu );

    // Dynamically built on invocation
    m_windowsMenu->setCheckable( true );
    connect( m_windowsMenu, SIGNAL( aboutToShow() ),
             this,          SLOT( slotWindowsMenuAboutToShow() ) );

    // Add Windows menu to the menu bar
    QString text("");
    translate( text, "Menu:Windows" );
    m_idWindows = menuBar()->insertItem( text, m_windowsMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the AppWindow's initial screen size.
 */

void AppWindow::initResize( void )
{
    // Get the screen size.
    int screenWd = QApplication::desktop()->width();
    int screenHt = QApplication::desktop()->height();

    // Create a ghost TextViewDocument just so we can determine its page size
    Document *doc = new TextViewDocument( 0, 0, "sizeTest", WDestructiveClose );
    checkmem( __FILE__, __LINE__, doc, "TextViewDocument doc", 1 );
    int pageWd = doc->pageWdPixels() + 20;
    int pageHt = doc->pageHtPixels() + 20;
    delete doc; doc = 0;

    // Adjust visible page width to fit the screen
    if ( pageWd > screenWd )
    // This code block should PROBABLY never be executed!
    {
        pageWd = screenWd - 72;
    }
    // Adjust visible page height to fit the screen
    if ( pageHt > screenHt )
    {
        pageHt = screenHt - 72;
    }
    // Set the size and return
    resize( pageWd, pageHt );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the AppWindow's tool bar.
 */

void AppWindow::initToolBar( void )
{
    // This may be called by the constructor, then again by slotAppInit()
    if ( m_toolBar )
    {
        return;
    }
    QString text("");

    // Create the tool bar
    //m_toolBar = new QToolBar(
    //    "Main Tool Bar",            // Label passed to QMainWindow::addToolBar()
    //    this,                       // Parent QMainWindow
    //    QMainWindow::Top,           // QMainWindow::ToolBarDock
    //    false,                      // Start tool bar on a new line?
    //    "m_toolBar" );              // widgetname
    m_toolBar = new QToolBar( this, "m_toolBar" );
    Q_CHECK_PTR( m_toolBar );

    translate( text, "Toolbar:File:New" );
    m_fileNewButton = new QToolButton(
        *m_fileNewIcon,                     // Pixmap
        text,                               // Tool tip text
        QString::null,                      // Status bar message
        this, SLOT( slotDocumentNew() ),    // Invokes this callback
        m_toolBar,                          // Parent
        "m_fileNewButton" );                // Widget name
    Q_CHECK_PTR( m_fileNewButton );

    translate( text, "Toolbar:File:Open" );
    m_fileOpenButton = new QToolButton(
        *m_fileOpenIcon,
        text,
        QString::null,
        this, SLOT( slotDocumentOpen() ),
        m_toolBar,
        "m_fileOpenButton" );
    Q_CHECK_PTR( m_fileOpenButton );

    //translate( text, "Toolbar:File:Save" );
    //m_fileSaveButton = new QToolButton(
    //    *m_fileSaveIcon,
    //    text,
    //    QString::null,
    //    this, SLOT( slotDocumentSave() ),
    //    m_toolBar,
    //    "m_fileSaveButton" );
    //Q_CHECK_PTR( m_fileSaveButton );

    translate( text, "Toolbar:File:Print" );
    m_filePrintButton = new QToolButton(
        *m_filePrintIcon,
        text,
        QString::null,
        this, SLOT( slotDocumentPrint() ),
        m_toolBar,
        "m_filePrintButton" );
    Q_CHECK_PTR( m_filePrintButton );

    m_toolBar->addSeparator();

    translate( text, "Toolbar:Configure:Module" );
    m_docModulesButton = new QToolButton(
        *m_modulesIcon,
        text,
        QString::null,
        this, SLOT( slotConfigureModules() ),
        m_toolBar,
        "m_docModulesButton" );
    Q_CHECK_PTR( m_docModulesButton );

    translate( text, "Toolbar:File:Calculate" );
    m_docRunButton = new QToolButton(
        *m_fileRunIcon,
        text,
        QString::null,
        this, SLOT( slotDocumentRun() ),
        m_toolBar,
        "m_docRunButton" );
    Q_CHECK_PTR( m_docRunButton );

    m_toolBar->addSeparator();

    translate( text, "Toolbar:Page:First" );
    m_pageFirstButton = new QToolButton(
        *m_pageFirstIcon,
        text,
        QString::null,
        this, SLOT( slotPageFirst() ),
        m_toolBar,
        "m_pageFirstButton" );
    Q_CHECK_PTR( m_pageFirstButton );

    translate( text, "Toolbar:Page:Last" );
    m_pageLastButton = new QToolButton(
        *m_pageLastIcon,
        text,
        QString::null,
        this, SLOT( slotPageLast() ),
        m_toolBar,
        "m_pageLastButton" );
    Q_CHECK_PTR( m_pageLastButton );

    translate( text, "Toolbar:Page:Previous" );
    m_pagePrevButton = new QToolButton(
        *m_pagePrevIcon,
        text,
        QString::null,
        this, SLOT( slotPagePrev() ),
        m_toolBar,
        "m_pagePrevButton" );
    Q_CHECK_PTR( m_pagePrevButton );

    translate( text, "Toolbar:Page:Next" );
    m_pageNextButton = new QToolButton(
        *m_pageNextIcon,
        text,
        QString::null,
        this, SLOT( slotPageNext() ),
        m_toolBar,
        "m_pageNextButton" );
    Q_CHECK_PTR( m_pageNextButton );

    m_toolBar->addSeparator();

    translate( text, "Toolbar:Help:Program" );
    m_helpHelpButton = new QToolButton(
        *m_helpIcon,
        text,
        QString::null,
        this, SLOT( slotHelpUsersGuide() ),
        m_toolBar,
        "m_helpHelpButton"
    );
    Q_CHECK_PTR( m_helpHelpButton );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Finds the Document with the full pathway \a fileName from the
 *  Workspace document list.
 *
 *  \param fileName Reference to a full file path name.
 *  \param focus If TRUE, focus is transferred to the Document if it is found.
 *
 *  \retval Pointer to the QWorkspace document with \a fileName.
 *  \retval Zero if no Document with \a fileName is found.
 */

Document *AppWindow::findDocument( const QString &fileName, bool focus )
{
    Document *doc = 0;
    if ( m_workSpace )
    {
        QWidgetList windows = m_workSpace->windowList();
        for ( int id = 0;
              id < int( windows.count() );
              ++id )
        {
            doc = (Document *) windows.at(id);
            if ( doc->m_absPathName == fileName )
            {
                if ( focus )
                {
                    doc->setFocus();
                }
                return( doc );
            }
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Opens a BpDocument file into a new Document instance.
 *
 *  Called by slotOpenDocument() and slotOpenDocument(const QString &fileName).
 *
 *  \param fileName Reference to the full file path name.
 *  If zero, a file selector will be displayed.
 *  \param thisFileType Reference to a file type:
 *  \arg Native
 *  \arg Fuel Model
 *  \arg Moisture Scenario
 *  \arg Run
 *  \arg Units Set
 *  \arg Worksheet
 *
 *  \retval TRUE if the Document is opened.
 *  \retval FALSE if the Document is not opened.
 */

Document *AppWindow::openDocument( const QString &fileName,
        const QString &thisFileType, bool openAnyway )
{
    log( "Beg Section: Open Document\n" );
    QString end( "End Section: Open Document\n" );
    // Request a file name if one wasn't provided
    QString fileType = thisFileType;
    QString openFileName = fileName;
    if ( openFileName.isNull() || openFileName.isEmpty() )
    {
        log( "Presenting Open Document dialog ...\n" );
        QString dirName, fileExt;

        // Determine the type of file to open
        if ( thisFileType == "Run"
          || thisFileType == "Native" )
        {
            fileType = "Run";
            dirName = appFileSystem()->runPath();
            fileExt = appFileSystem()->runExt();
        }
        else if ( thisFileType == "Worksheet" )
        {
            fileType = "Worksheet";
            dirName = appFileSystem()->worksheetPath();
            fileExt = appFileSystem()->worksheetExt();
        }
        else
        // This code block should never be executed!
        {
            QString msg("");
            translate( msg, "AppWindow:UnknownFileType",
                fileName, fileType );
            bomb( msg );
        }
        // Display the file selection dialog.
        FileSelectorDialog dialog(
            this,
            dirName,
            fileType,
            fileExt,
            "fileOpenDialog" );
        if ( dialog.exec() != QDialog::Accepted )
        {
            log( "Dialog cancelled.\n" );
            log( end );
            return( 0 );
        }
        // Get the user's file selection
        openFileName = "";
        dialog.getFileSelection( openFileName );
        if ( openFileName.isNull() || openFileName.isEmpty() )
        // This code block should never be executed!
        {
            log( "No document selected.\n" );
            log( end );
            return( 0 );
        }
    }
    log( QString("Document file is \"%1\" ... \n" ).arg( openFileName ) );
    // If the document is already open, focus it
    Document *doc = 0;
    if ( ( doc = findDocument( openFileName, true ) ) )
    {
        log( "Document is already open.\n" );
        log( end );
        return( doc );
    }
    // Create a new Document with the next document id for this process
    log( QString( "Creating a new BpDocument instance %1 ...\n" )
        .arg( m_docIdCount+1 ) );
    BpDocument *bdoc = new BpDocument(
        m_workSpace,
        m_docIdCount+1,
        m_eqApp,
        QString( "BpDocument%1" ).arg( m_docIdCount+1 ),
        WDestructiveClose );
    checkmem( __FILE__, __LINE__, bdoc, "BpDocument doc", 1 );
    m_docIdCount++;
    doc = bdoc;
    log( "BpDocument created ok.\n" );

    // Display any messages emitted by the Document in the status bar
    connect( doc,         SIGNAL( message(const QString&) ),
             statusBar(), SLOT(   message(const QString&) ) );

    // If the document is closed using the X button, update status.
    //connect( doc, SIGNAL( destroyed() ),
    //         this, SLOT( slotStatusUpdate() ) );

    // Open the BpDocument with data from the file
    doc->setIcon( m_documentIcon->pixmap() );
    log( QString("Opening file \"%1\" ... \n" ).arg( openFileName ) );
    if ( doc->open( openFileName, fileType ) )
    {
        doc->setFocus();
        doc->showMaximized();
        doc->m_scrollView->setContentsPos( 0, 0 );
        log( "Opened ok.\n" );
        log( end );
        return( doc );
    }
    log( "Open failed.\n" );
    // Open failed; undo effects of calling slotDocumentNew().
    delete doc;     doc = 0;
    m_docIdCount--;
    if ( ! openAnyway )
    {
        log( end );
        return( 0 );
    }
    // Ask if the user wants to view it anyway
    QString l_caption(""), msg("");
    translate( l_caption, "AppWindow:ViewAnyway:Caption" );
    translate( msg, "AppWindow:ViewAnyway:Text", openFileName );
    log( "Request whether to view file anyway ...\n" );
    if ( yesno( l_caption, msg ) == 0 )
    {
        log( "Don't want to view the file anyway.\n" );
        log( end );
        return( 0 );
    }
    // Create a new Document with the next document id for this process
    log( "Creating a new TextViewDocument instance ...\n" );
    doc = new TextViewDocument(
        m_workSpace,
        m_docIdCount+1,
        QString( "TvDocument%1" ).arg( m_docIdCount+1 ),
        WDestructiveClose );
    checkmem( __FILE__, __LINE__, doc, "TextViewDocument doc", 1 );
    m_docIdCount++;

    // Display any messages emitted by the Document in the status bar
    connect( doc,         SIGNAL( message(const QString&) ),
             statusBar(), SLOT(   message(const QString&) ) );

    // Set the TextViewocument's window icon and return
    translate( l_caption, "AppWindow:UnnamedCaption" );
    doc->setCaption( l_caption );
    doc->setIcon( m_documentIcon->pixmap() );

    // Open the document
    log( "Attempting to open TextViewDocument file ...\n" );
    if ( ! doc->open( openFileName, "Text" ) )
    // This code block should never be executed!
    {
        delete doc;     doc = 0;
        m_docIdCount--;
        log( "Open failed.\n" );
        log( end );
        return( 0 );
    }
    log( "Open as TextViewDocument succeeded.\n" );
    doc->setFocus();
    doc->showMaximized();
    doc->m_scrollView->setContentsPos( 0, 0 );
    log( end );
    return( doc );
}

//------------------------------------------------------------------------------
/*! \brief Changes all the displayed text to the requested language.
 *
 *  \param language New language to display
 */

void AppWindow::setLanguage( const QString &language )
{
    log( "Beg Section: AppWindow::setLanguage() ...\n" );
    QString end( "End Section: AppWindow::setLanguage() ...\n" );
    log( QString( "New language is \"%1\"." ).arg( language ) );
    // Do we really have to do this?
    if ( language == m_eqApp->m_language )
    {
        log( "This language is already in use.\n" );
        log( end );
        return;
    }
    // Update the translator language and all the EqTrees.
    log( "Setting the language in EqApp ...\n" );
    m_eqApp->setLanguage( language );

    // Translate all the currently displayed menu text
    log( "Changing all the menu text ...\n" );
    QString text("");
    translate( text, "Menu:Configure" );
    menuBar()->changeItem( m_idConfig, text );
    translate( text, "Menu:Configure:Appearance" );
    menuBar()->changeItem( m_idConfigAppearance, text );
    translate( text, "Menu:Configure:Language" );
    menuBar()->changeItem( m_idConfigLang, text );

    QString *lang, langKey( "" );
    int id = 0;
    for ( lang = m_eqApp->m_languageList->first();
          lang != 0;
          lang = m_eqApp->m_languageList->next(), id++ )
    {
        langKey = "Menu:Configure:Language:" + *lang;
        translate( text, langKey );
        menuBar()->changeItem( m_idConfigLanguage[id], text );
    }

    translate( text, "Menu:Configure:Module" );
    menuBar()->changeItem( m_idConfigModule, text );
    translate( text, "Menu:Configure:Units" );
    menuBar()->changeItem( m_idConfigUnits, text );
    translate( text, "Menu:Configure:Units:English" );
    menuBar()->changeItem( m_idConfigUnitsEnglish, text );
    translate( text, "Menu:Configure:Units:Custom" );
    menuBar()->changeItem( m_idConfigUnitsCustom, text );
    translate( text, "Menu:Configure:Units:Metric" );
    menuBar()->changeItem( m_idConfigUnitsMetric, text );
    translate( text, "Menu:Configure:FuelModels" );
    menuBar()->changeItem( m_idConfigFuel, text );
    translate( text, "Menu:Configure:MoistureScenarios" );
    menuBar()->changeItem( m_idConfigMois, text );
    // File menu
    translate( text, "Menu:File" );
    menuBar()->changeItem( m_idFile, text );
    translate( text, "Menu:File:New" );
    menuBar()->changeItem( m_idFileNew, text );
    translate( text, "Menu:File:Open" );
    menuBar()->changeItem( m_idFileOpen, text );
    //translate( text, "Menu:File:Save" );
    //menuBar()->changeItem( m_idFileSave, text );
    translate( text, "Menu:File:SaveAs" );
    menuBar()->changeItem( m_idFileSaveAs, text );
    translate( text, "Menu:File:SaveAs:Run" );
    menuBar()->changeItem( m_idFileSaveAsRun, text );
    translate( text, "Menu:File:SaveAs:Worksheet" );
    menuBar()->changeItem( m_idFileSaveAsWorksheet, text );
    translate( text, "Menu:File:SaveAs:FuelModel" );
    menuBar()->changeItem( m_idFileSaveAsFuelModel, text );
    translate( text, "Menu:File:SaveAs:MoistureScenario" );
    menuBar()->changeItem( m_idFileSaveAsMoistureScenario, text );
    translate( text, "Menu:File:Calculate" );
    menuBar()->changeItem( m_idFileCalculate, text );
    translate( text, "Menu:File:Print" );
    menuBar()->changeItem( m_idFilePrint, text );
    translate( text, "Menu:File:Export" );
    menuBar()->changeItem( m_idFileExport, text );
    translate( text, "Menu:File:Export:FuelModels" );
    menuBar()->changeItem( m_idFileExportFuelModels, text );
    translate( text, "Menu:File:Export:FuelModels:FarsiteEnglish" );
    menuBar()->changeItem( m_idFileExportFuelModelsFarsiteEnglish, text );
    translate( text, "Menu:File:Export:FuelModels:FarsiteMetric" );
    menuBar()->changeItem( m_idFileExportFuelModelsFarsiteMetric, text );
    //translate( text, "Menu:File:Export:FuelModels:Flammap" );
    //menuBar()->changeItem( m_idFileExportFuelModelsFlammap, text );
    translate( text, "Menu:File:Export:Results" );
    menuBar()->changeItem( m_idFileExportResults, text );
    translate( text, "Menu:File:Capture" );
    menuBar()->changeItem( m_idFileCapture, text );
    translate( text, "Menu:File:Close" );
    menuBar()->changeItem( m_idFileClose, text );
    translate( text, "Menu:File:Workspace" );
    menuBar()->changeItem( m_idFileWorkspace, text );
    translate( text, "Menu:File:Workspace:New" );
    menuBar()->changeItem( m_idFileWorkspaceNew, text );
    translate( text, "Menu:File:Workspace:Open" );
    menuBar()->changeItem( m_idFileWorkspaceOpen, text );
    translate( text, "Menu:File:Quit" );
    menuBar()->changeItem( m_idFileQuit, text );
    // Help menu
    translate( text, "Menu:Help" );
    menuBar()->changeItem( m_idHelp, text );
    translate( text, "Menu:Help:AboutBehavePlus" );
    menuBar()->changeItem( m_idHelpAboutBehavePlus, text );
    translate( text, "Menu:Help:AboutHelp" );
    menuBar()->changeItem( m_idHelpAboutHelp, text );
    translate( text, "Menu:Help:InstallationInfo" );
    menuBar()->changeItem( m_idHelpInstallationInfo, text );
    translate( text, "Menu:Help:MasterIndex" );
    menuBar()->changeItem( m_idHelpMasterIndex, text );
    translate( text, "Menu:Help:FeaturesPaper" );
    menuBar()->changeItem( m_idHelpUsersGuide, text );
    translate( text, "Menu:Help:SplashPage" );
    menuBar()->changeItem( m_idHelpSplashPage, text );
    translate( text, "Menu:Help:VariableHelp" );
    menuBar()->changeItem( m_idHelpVariableHelp, text );
    // Pages menu
    translate( text, "Menu:Pages" );
    menuBar()->changeItem( m_idPages, text );
    // Tools menu
    translate( text, "Menu:Tools" );
    menuBar()->changeItem( m_idTools, text );
    translate( text, "Menu:Tools:SunCalendar" );
    menuBar()->changeItem( m_idToolsSunCal, text );
    translate( text, "Menu:Tools:UnitsConverter" );
    menuBar()->changeItem( m_idToolsUnitsConverter, text );
    translate( text, "Menu:Tools:UnitsEditor" );
    menuBar()->changeItem( m_idToolsUnitsEditor, text );
    translate( text, "Menu:Tools:FdfmcDialog" );
    menuBar()->changeItem( m_idToolsFdfmcDialog, text );
    translate( text, "Menu:Tools:HumidityDialog" );
    menuBar()->changeItem( m_idToolsHumidityDialog, text );
    translate( text, "Menu:Tools:SlopeFromMapMeasurements" );
    menuBar()->changeItem( m_idToolsSlopeFromMapMeasurements, text );
    // View menu
    translate( text, "Menu:View" );
    menuBar()->changeItem( m_idView, text );
    // Windows menu
    translate( text, "Menu:Windows" );
    menuBar()->changeItem( m_idWindows, text );
    translate( text, "Menu:Windows:Cascade" );
    menuBar()->changeItem( m_idWindowsCascade, text );
    translate( text, "Menu:Windows:Tile" );
    menuBar()->changeItem( m_idWindowsTile, text );
    // Tool bar
    log( "Changing all the ToolBar text ...\n" );
    translate( text, "Toolbar:File:New" );
    m_fileNewButton->setTextLabel( text, true );
    translate( text, "Toolbar:File:Open" );
    m_fileOpenButton->setTextLabel( text, true );
    //translate( text, "Toolbar:File:Save" );
    //m_fileSaveButton->setTextLabel( text, true );
    translate( text, "Toolbar:File:Print" );
    m_filePrintButton->setTextLabel( text, true );
    translate( text, "Toolbar:Configure:Module" );
    m_docModulesButton->setTextLabel( text, true );
    translate( text, "Toolbar:File:Calculate" );
    m_docRunButton->setTextLabel( text, true );
    translate( text, "Toolbar:Page:First" );
    m_pageFirstButton->setTextLabel( text, true );
    translate( text, "Toolbar:Page:Last" );
    m_pageLastButton->setTextLabel( text, true );
    translate( text, "Toolbar:Page:Next" );
    m_pageNextButton->setTextLabel( text, true );
    translate( text, "Toolbar:Page:Previous" );
    m_pagePrevButton->setTextLabel( text, true );
    translate( text, "Toolbar:Help:Program" );
    m_helpHelpButton->setTextLabel( text, true );

    // Redraw all documents in the new language
    Document *activeDoc = (Document *) m_workSpace->activeWindow();
    QWidgetList windows = m_workSpace->windowList();
    Document *doc;
    for ( id = 0;
          id < int( windows.count() );
          ++id )
    {
        log( QString( "Reconfiguring document %1 ...\n" ).arg( id ) );
        doc = (Document *) windows.at(id);
        if ( doc && doc->m_docType == "BpDocument" )
        {
            doc->configure();
        }
    }
    // Restore the active document
    log( "Restoring the active document ...\n" );
    if ( activeDoc )
    {
        activeDoc->setFocus();
        activeDoc->showMaximized();
        doc->m_scrollView->setContentsPos( 0, 0 );
    }
    slotStatusUpdate();
    log( end );
    return;
}


//------------------------------------------------------------------------------
/*! \brief Sets a worksheet as the next startup worksheet.
 *
 *  Called by slotSetStartupWorksheet().
 *
 *  \retval TRUE if the next startup worksheet is set ok.
 *  \retval FALSE if not.
 */

bool AppWindow::setStartupWorksheet( void )
{
    // Display the file selection dialog.
    QString fileType = "Worksheet";
    QString dirName = appFileSystem()->worksheetPath();
    QString fileExt = appFileSystem()->worksheetExt();
    FileSelectorDialog dialog(
        this,
        dirName,
        fileType,
        fileExt,
        "fileOpenDialog" );
    if ( dialog.exec() != QDialog::Accepted )
    {
        return( false );
    }
    // Get the user's file selection
    QString openFileName("");
    dialog.getFileSelection( openFileName );
    if ( openFileName.isNull() || openFileName.isEmpty() )
    // This code block should never be executed!
    {
        return( false );
    }
    // Strip the workspace path name from the file
    //QString workspacePath = appFileSystem()->workspacePath();
    //openFileName = openFileName.remove( workspacePath );
    appProperty()->update( "appStartup", openFileName );
    // Confirm the change
    QString saved("");
    translate( saved, "BpDocument:SetStartupWorksheet:Changed", openFileName );
    info( saved );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Appearance Configuration Dialog.
 *
 *  Called only by the \b Configure->Appearance menu selection.
 */

void AppWindow::slotConfigureAppearance( void )
{
    log( "Beg Section: AppWindow::slotConfigureAppearance() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureAppearance();
    }
    log( "End Section: AppWindow::slotConfigureAppearance() invoked ...\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Fuel Models Configuration Dialog.
 *
 *  Called only by the \b Configure->FuelModels menu selection .
 */

void AppWindow::slotConfigureFuelModels( void )
{
    log( "Beg Section: AppWindow::slotConfigureFuelModels() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Menu Configue Fuel Models selected.\n" ) );
        doc->configureFuelModels();
    }
    log( "End Section: AppWindow::slotConfigureFuelModels() invoked ...\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Changes the language setting for the application and for the current
 *  document.
 *
 *  Called by Configure->Language-><choice> menu selection.
 */

void AppWindow::slotConfigureLanguage( int mid )
{
    log( "Beg Section: AppWindow::slotConfigureLanguage() invoked ...\n" );
    QString *lang;
    int lid = 0;
    for ( lang = m_eqApp->m_languageList->first();
          lang != 0;
          lang = m_eqApp->m_languageList->next(), lid++ )
    {
        if ( mid == m_idConfigLanguage[lid] )
        {
            log( QString( "Calling setLanguage( %1 ).\n" ).arg( *lang ) );
            setLanguage( *lang );
        }
    }
    log( "End Section: AppWindow::slotConfigureLanguage() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Module Configuration Dialog.
 *
 *  Called only by the \b Configure->Modules menu selection
 *  and the \b Modules tool bar button.
 */

void AppWindow::slotConfigureModules( void )
{
    log( "Beg Section: AppWindow::slotConfigureModules() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureModules();
    }
    log( "End Section: AppWindow::slotConfigureModules() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Moisture Scenario Configuration Dialog.
 *
 *  Called only by the \b Configure->MoistureScenarios menu selection .
 */

void AppWindow::slotConfigureMoistureScenarios( void )
{
    log( "Beg Section: AppWindow::slotConfigureMoistureScenarios() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureMoistureScenarios();
    }
    log( "End Section: AppWindow::slotConfigureMoistureScenarios() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Custom Units Configuration Dialog.
 *
 *  Called only by the \b Configure->Units->Custom menu selection.
 */

void AppWindow::slotConfigureUnitsCustom( void )
{
    log( "Beg Section: AppWindow::slotConfigureUnitsCustom() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureUnits( "Custom" );
    }
    log( "End Section: AppWindow::slotConfigureUnitsCustom() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Converts all the worksheet inputs and outputs to the standard
 *  English units set.
 *
 *  Called only by the \b Configure->Units->English menu selection.
 */

void AppWindow::slotConfigureUnitsEnglish( void )
{
    log( "Begins Section: AppWindow::slotConfiugreUnitsEnglish() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureUnits( "English" );
    }
    log( "End Section: AppWindow::slotConfigureUnitsEnglish() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Converts all the worksheet inputs and outputs to the standard
 *  Metric units set.
 *
 *  Called only by the \b Configure->Units->Metric menu selection.
 */

void AppWindow::slotConfigureUnitsMetric( void )
{
    log( "Beg Section: AppWindow::slotConfiugreUnitsMetric() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->configureUnits( "Metric" );
    }
    log( "End Section: AppWindow::slotConfigureUnitsMetric() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Captures the current Document page to an image file.
 *
 *  Called only by \b File->Capture menu selection.
 *
 *  Document::capture() is called to perform the operation.
 */

void AppWindow::slotDocumentCapture( void )
{
    log( "Beg Section: AppWindow::slotDocumentCapture() invoked ...\n" );
    // Get the current active document.
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->capture();
    }
    log( "End Section: AppWindow::slotDocumentCapture() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clears the input entry fields of the worksheet.
 *
 *  Called only by \b File->Clear worksheet inputs menu selection.
 *
 *  Document::clearEntries() is called to perform the operation.
 */

void AppWindow::slotDocumentClear( void )
{
    log( "Beg Section: AppWindow::slotDocumentClear() invoked ...\n" );
    // Get the current active document.
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->clear();
    }
    log( "End Section: AppWindow::slotDocumentClear() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clones the current worksheet into a new run
 *
 *  Called only by \b File->Clone worksheet menu selection.
 */

void AppWindow::slotDocumentClone( void )
{
    log( "Beg Section: AppWindow::slotDocumentClone() invoked ...\n" );
    // Get the current active document.
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        QString fileName = doc->m_absPathName;
        doc->save( 0, "RunClone" );
    }
    log( "End Section: AppWindow::slotDocumentClone() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Closes the current active Document.
 *
 *  Called only by the \b File->Close menu selection.
 *
 *  Calls Document::close(), which is actually QWidget::close().
 */

void AppWindow::slotDocumentClose( void )
{
    log( "Beg Section: AppWindow::slotDocumentClose() invoked ...\n" );
    Document *doc = (Document *) m_workSpace->activeWindow();
    if ( doc )
    {
        // Activate another window if one is available
        QWidgetList windows = m_workSpace->windowList();
        Document *nextDoc;
        for ( int id = 0;
              id < int( windows.count() );
              ++id )
        {
            nextDoc = (Document *) windows.at(id);
            if ( nextDoc && nextDoc != doc )
            {
                // Activate it and break out of loop
                log( QString( "Activating document %1 \"%2\" ...\n" )
                    .arg( id ).arg( nextDoc->m_absPathName ) );
                QWidget* w = m_workSpace->windowList().at( id );
                w->setFocus();
                break;
            }
        }
        // This actually calls QWidget::close(); there is no Document::close()
        log( QString( "Closing document \"%1\" ...\n" )
            .arg( nextDoc->m_absPathName ) );
        doc->close();
        // Update the status display
        slotStatusUpdate();
    }
    log( "End Section: AppWindow::slotDocumentClose() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Exports selected fuel models to a FARSITE formatted, English units file.
 *
 *  Called only by the \b File->Export->FuelModels->FARSITE English menu selection.
 */

void AppWindow::slotDocumentExportFuelModelsFarsiteEnglish( void )
{
    log( "Beg Section: AppWindow::slotDocumentExportFuelModelsFarsiteEnglish() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Exporting FARSITE fuel models in English units for document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Export Fuel Farsite English" );
    }
    log( "End Section: AppWindow::slotDocumentExportFuelModelsFarsiteEnglish() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Exports selected fuel models to a FARSITE formatted, metric units file.
 *
 *  Called only by the \b File->Export->FuelModels->FARSITE metric menu selection.
 */

void AppWindow::slotDocumentExportFuelModelsFarsiteMetric( void )
{
    log( "Beg Section: AppWindow::slotDocumentExportFuelModelsFarsiteMetric() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Exporting FARSITE fuel models in metric units for document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Export Fuel Farsite Metric" );
    }
    log( "End Section: AppWindow::slotDocumentExportFuelModelsFarsiteMetric() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Exports current run results to an HTML (or spreadsheet) file.
 *
 *  Called only by the \b File->Export menu selection.
 */

void AppWindow::slotDocumentExportResults( void )
{
    log( "Beg Section: AppWindow::slotDocumentExportResults() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Exporting results in for document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Export Results" );
    }
    log( "End Section: AppWindow::slotDocumentExportResults() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a BehavePlus Worksheet file into a new Document.
 *
 *  Called only by \b File->New menu selection .
 *
 *  \sa openDocument(), slotDocumentOpen()
 */

void AppWindow::slotDocumentNew( void )
{
    log( "Beg Section: AppWindow::slotDocumentNew() invoked ...\n" );
    openDocument( 0, "Worksheet" );
    log( "End Section: AppWindow::slotDocumentNew() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a BehavePlus document from a Run file into a new Document.
 *
 *  Called only by \b File->Open menu selection
 *  or by the \b Open tool bar button.
 *
 *  \sa openDocument(), slotDocumentNew().
 */

void AppWindow::slotDocumentOpen( void )
{
    log( "Beg Section: AppWindow::slotDocumentOpen() invoked ...\n" );
    openDocument( 0, "Run" );
    log( "End Section: AppWindow::slotDocumentOpen() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the current active Document to a printer.
 *
 *  Called only by the \b File->Print menu selection
 *  and the \b Print tool bar button.
 *
 *  Document::print() is called to perform the operation.
 */

void AppWindow::slotDocumentPrint( void )
{
    log( "Beg Section: AppWindow::slotDocumentPrint() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        log( QString( "Printing document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        doc->print();
    }
    log( "End Section: AppWindow::slotDocumentPrint() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Resets the input values of the current active Document.
 *
 *  Called only by the \b File->Reset worksheet inputs menu selection
 *
 *  Document::reset() is called to perform the operation.
 */

void AppWindow::slotDocumentReset( void )
{
    log( "Beg Section: AppWindow::slotDocumentReset() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        QString fileName = doc->m_absPathName;
        if ( doc->open( fileName, "Run" ) )
        {
            doc->setFocus();
            doc->showMaximized();
            doc->m_scrollView->setContentsPos( 0, 0 );
        }
    }
    log( "End Section: AppWindow::slotDocumentReset() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Runs the current active Document.
 *
 *  Called only by the \b File->Run menu selection
 *  and the \b Run tool bar button.
 *
 *  Document::run() is called to perform the operation.
 */

void AppWindow::slotDocumentRun( void )
{
    log( "Beg Section: AppWindow::slotDocumentRun() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Running document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        doc->run();
    }
    log( "End Section: AppWindow::slotDocumentRun() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current active Document to its current file name
 *  Document::m_absPathName.
 *
 *  Called only by the \b File->Save menu selection
 *  and the \b Save tool bar button.
 *
 *  Document::save() is called to perform the operation.
 */

void AppWindow::slotDocumentSave( void )
{
    log( "Beg Section: AppWindow::slotDocumentSave() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Savinging document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        doc->save( doc->m_absPathName, "Run" );
    }
    log( "End Section: AppWindow::slotDocumentSave() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current active Document to a fuel model file.
 *
 *  Called only by the \b File->saveAs->FuelModel menu selection.
 */

void AppWindow::slotDocumentSaveAsFuelModel( void )
{
    log( "Beg Section: AppWindow::slotDocumentSaveAsFuelModel() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Saving fuel model document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Fuel Model" );
    }
    log( "End Section: AppWindow::slotDocumentSaveAsFuelModel() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current active Document to a moisture scenario file.
 *
 *  Called only by the \b File->saveAs->MoistureScenario menu selection.
 */

void AppWindow::slotDocumentSaveAsMoistureScenario( void )
{
    log( "Beg Section: AppWindow::slotDocumentSaveAsMoistureScenario() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Saving moisture scenario document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Moisture Scenario" );
    }
    log( "End Section: AppWindow::slotDocumentSaveAsMoistureScenario() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current active Document to a Run file.
 *
 *  Called by the \b File->saveAs->Run menu selections.
 */

void AppWindow::slotDocumentSaveAsRun( void )
{
    log( "Beg Section: AppWindow::slotDocumentSaveAsRun() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Saving run document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Run" );
    }
    log( "End Section: AppWindow::slotDocumentSaveAsRun() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current active Document to a worksheet file.
 *
 *  Called only by the \b File->saveAs->Worksheet menu selection.
 */

void AppWindow::slotDocumentSaveAsWorksheet( void )
{
    log( "Beg Section: AppWindow::slotDocumentSaveAsWorksheet() invoked ...\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        log( QString( "Saving worksheet document \"%1\" ...\n" )
            .arg( doc->m_absPathName ) );
        // Passing fileName==0 forces a file dialog.
        doc->save( 0, "Worksheet" );
    }
    log( "End Section: AppWindow::slotDocumentSaveAsWorksheet() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a new data directory (workspace) and populates it with the
 *  required folders and files.
 *
 *  Called only by the \b File->change Workspace menu selection.
 */

void AppWindow::slotDocumentWorkspaceClone( void )
{
        log( "Beg Section: AppWindow::slotDocumentWorkspaceClone() invoked ...\n" );
        slotDocumentWorkspace( true );
        log( "End Section: AppWindow::slotDocumentWorkspaceClone() completed.\n" );
        return;
}

void AppWindow::slotDocumentWorkspaceNew( void )
{
        log( "Beg Section: AppWindow::slotDocumentWorkspaceNew() invoked ...\n" );
        slotDocumentWorkspace( false );
        log( "End Section: AppWindow::slotDocumentWorkspaceNew() completed.\n" );
        return;
}

void AppWindow::slotDocumentWorkspace( bool clone )
{
    // Request the new workspace directory.
    QString text(""), l_caption(""), errMsg("");
    translate( text, "AppWindow:SelectWorkspace" );
    QString dirName = QFileDialog::getExistingDirectory(
         0, this, "newWorkspace", text, true );

    // Make the new workspace directory.
    if ( ! dirName.isNull() && ! dirName.isEmpty() )
    {
        log( QString( "Creating new workspace \"%1\" ...\n" )
            .arg( dirName ) );
        translate( l_caption, "AppWindow:Workspace:Make:Error:Caption" );
        QFileInfo fi( dirName );
        if ( ! fi.exists() )
        {
            translate( text, "AppWindow:Workspace:Make:Error:DoesntExist",
                dirName );
            warn( l_caption, text );
            log( text );
        }
        else if ( ! fi.isDir() )
        {
            translate( text, "AppWindow:Workspace:Make:Error:NotDir",
                dirName );
            warn( l_caption, text );
            log( text );
        }
        else if ( ! fi.isWritable() )
        {
            translate( text, "AppWindow:Workspace:Make:Error:NotWritable",
                dirName );
            warn( l_caption, text );
            log( text );
        }
        else if ( appFileSystem()->testWorkspaceFiles( dirName, errMsg ) )
        {
            translate( text, "AppWindow:Workspace:Make:Error:IsWorkspace",
                dirName );
            warn( l_caption, text );
            log( text );
        }
        else if ( ! appFileSystem()->makeWorkspace( dirName,
                        appFileSystem()->workspacePath(), clone, errMsg ) )
        {
            translate( text, "AppWindow:Workspace:Make:Error:Create",
                dirName, errMsg );
            warn( l_caption, text );
            log( text );
        }
        else
        {
            log( "Created ok ... make its current? ... \n" );
            // Workspace was created ok; ask to make it the current workspace
            translate( l_caption, "AppWindow:Workspace:Make:Ok:Caption" );
            translate( text, "AppWindow:Workspace:Make:Ok:Text", dirName );
            if ( yesno( l_caption, text ) == 1 )
            {
                log( "Making it the current workspace ...\n" );
                appFileSystem()->workspacePath( dirName );
			    Document *doc = getActiveWindow( "BpDocument" );
				if ( doc )
				{
					doc->statusUpdate();
				}
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Changes the current data directory (workspace).
 *
 *  Called only by the \b File->change Workspace menu selection.
 */

void AppWindow::slotDocumentWorkspaceOpen( void )
{
    log( "Beg Section: AppWindow::slotDocumentWorkspaceOpen() invoked ...\n" );
    // Request the new workspace directory.
    QString text("");
    translate( text, "AppWindow:SelectWorkspace" );
    QString dirName = QFileDialog::getExistingDirectory(
         0, this, "openWorkspace", text, true );

    // Test the new workspace directory.
    if ( ! dirName.isNull() && ! dirName.isEmpty() )
    {
        log( QString( "Changing to workspace \"%1\" ...\n" )
            .arg( dirName ) );
        QString errMsg("");
        if ( ! appFileSystem()->testWorkspaceFiles( dirName, errMsg ) )
        {
            // This is an invalid workspaceDir
            QString l_caption("");
            translate( l_caption, "AppWindow:WorkspaceDirInvalid:Caption" );
            translate( text, "AppWindow:WorkspaceDirInvalid:Text",
                dirName, errMsg, appFileSystem()->workspacePath() );
            warn( l_caption, text );
            log( text );
        }
        else
        {
            // This is a good workspaceDir
            appFileSystem()->workspacePath( dirName );
            QString l_caption("");
            translate( text, "AppWindow:WorkspaceDirChanged",
                appFileSystem()->workspacePath() );
            info( text );
            log( text );
        }
    }
    log( "End Section: AppWindow::slotDocumentWorkspaceOpen() completed.\n" );
    Document *doc = getActiveWindow( "BpDocument" );
    if ( doc )
    {
        doc->statusUpdate();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the application's \b About BehavePlus dialog.
 *
 *  Called only by the \b Help->About BehavePlus menu selection.
 */

void AppWindow::slotHelpAboutBehavePlus( void )
{
    log( "BegSection: AppWindow::slotHelpAboutBehavePlus() invoked ...\n" );
    //AboutDialog dialog( this, "aboutDialog" );
    AppDialog dialog( this, "AboutBehavePlus:Caption",
        "Wildfire1.png", "Wildfire", "aboutBehavePlus.html", "aboutBehavePlus" );
    dialog.exec();
    log( "End Section: AppWindow::slotHelpAboutBehavePlus() invoked ...\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the application's \b About Help dialog.
 *
 *  Called only by the \b Help->About Help menu selection.
 */

void AppWindow::slotHelpAboutHelp( void )
{
    log( "BegSection: AppWindow::slotHelpAboutHelp() invoked ...\n" );
    AppDialog dialog( this, "AboutHelp:Caption",
        "Wildfire1.png", "Wildfire", "aboutHelp.html", "aboutHelp" );
    dialog.exec();
    log( "End Section: AppWindow::slotHelpAboutHelp() invoked ...\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the platform-specific program help.
 *
 *  Called only by the \b Help->Features Paper start menu selection.
 */

void AppWindow::slotHelpFeaturesPaper( void )
{
    log( "Beg Section: AppWindow::slotHelpFeaturesPaper() invoked ...\n" );
    QString featuresPaper = appFileSystem()->featuresPaperFilePath();
    platformShowHelp( featuresPaper );
    log( "End Section: AppWindow::slotHelpFeaturesPaper() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the application's \b Master Index dialog.
 *
 *  Called only by the \b Help->Master Index menu selection.
 */

void AppWindow::slotHelpMasterIndex( void )
{
    log( "BegSection: AppWindow::slotHelpMasterIndex() invoked ...\n" );
    AppDialog dialog( this, "MasterIndex:Caption",
        "Wildfire1.png", "Wildfire", "index.html", "masterIndex" );
    dialog.exec();
    log( "End Section: AppWindow::slotHelpMasterIndex() invoked ...\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the platform-specific program help.
 *
 *  Called only by the \b Help->User Guide start menu selection.
 */

void AppWindow::slotHelpUsersGuide( void )
{
    log( "Beg Section: AppWindow::slotHelpUsersGuide() invoked ...\n" );
    QString userGuide = appFileSystem()->userGuideFilePath();
    platformShowHelp( userGuide );
    log( "End Section: AppWindow::slotHelpUsersGuide() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the Variable Help index.
 *
 *  Called only by the \b Help->Variable Help menu selection.
 */

void AppWindow::slotHelpVariableHelp( void )
{
    log( "Beg Section: AppWindow::slotHelpVariableHelp() invoked ...\n" );
    QString variableRef = appFileSystem()->variableRefFilePath();
    platformShowHelp( variableRef );
    //helpDialog( "variableIndex.html" );
    log( "End Section: AppWindow::slotHelpVariableHelp() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays installation information dialog.
 *
 *  Called only by the \b Help->Installation menu selection.
 *
 *  This is mainly for user support and debugging purposes.  It displays:
 *  \arg version numbers and compilation dates
 *  \arg command line arguments
 *  \arg environment variables
 *  \arg client machine specifications
 *  \arg installation directories and files
 */

void AppWindow::slotHelpInstallationInfo( void )
{
    log( "Beg Section: AppWindow::slotHelpInstallionInfo() invoked ...\n" );
    // Construct the command line.
    QString cmdLine = "";
    for ( int i = 1;
          i < qApp->argc();
          i++ )
    {
        cmdLine.append( " " );
        cmdLine.append( qApp->argv()[i] );
    }
    // Display version information
    QString t1(""), t2(""), t3(""), t4(""), t5("");
    releaseString( m_release, t5 );
    translate( t1, "AppWindow:ProgramInfo:Title" );
    translate( t2, "AppWindow:ProgramInfo:Executable" );
    translate( t3, "AppWindow:ProgramInfo:Release" );
    QString release = QString( m_version + " (" + m_build + ")" );
    QString msg1a = QString(
        "<H2>%1</H2>"
        "<TABLE>"
        "    <TR>"
        "      <TD>%2</TD>"
        "      <TD>%3</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>%4</TD>"
        "      <TD>%5</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>&nbsp;</TD>"
        "      <TD>%6</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>&nbsp;</TD>"
        "      <TD>%7</TD>"
        "    </TR>"
        )
        .arg( t1 )
        .arg( t2 )
        .arg( qApp->argv()[0] )
        .arg( t3 )
        .arg( m_version )
        .arg( m_build )
        .arg( m_releaseNote );

    translate( t1, "AppWindow:ProgramInfo:Compiled" );
    QString msg1b = QString(
        "    <TR>"
        "      <TD>%1</TD>"
        "      <TD>%2 %3</TD>"
        "    </TR>"
        )
        .arg( t1 )
        .arg( __DATE__ )
        .arg( __TIME__ );

    translate( t1, "AppWindow:ProgramInfo:QtVersion" );
    translate( t2, "AppWindow:ProgramInfo:Args" );
    QString msg1c = QString(
        "    <TR>"
        "      <TD>%1</TD>"
        "      <TD>%2</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>%3 %4</TD>"
        "      <TD>%5</TD>"
        "    </TR>"
        "</TABLE>"
        )
        .arg( t1 )
        .arg( QT_VERSION_STR )
        .arg( qApp->argc()-1 )
        .arg( t2 )
        .arg( cmdLine );

    QString msg1 = msg1a + msg1b + msg1c;

    // Display directories
    translate( t1, "AppWindow:ProgramInfo:Directories" );
    translate( t2, "AppWindow:ProgramInfo:ExeDir" );
    translate( t3, "AppWindow:ProgramInfo:XmlDir" );
    translate( t4, "AppWindow:ProgramInfo:Installation" );
    translate( t5, "AppWindow:ProgramInfo:Workspace" );
    QString msg2a = QString(
        "<H2>%1</H2>"
        "<TABLE>"
        "    <TR>"
        "      <TD>%2</TD>"
        "      <TD>%3</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>%4</TD>"
        "      <TD>%5</TD>"
		"	</TR>"
        "    <TR>"
        "      <TD>%6</TD>"
        "      <TD>%7</TD>"
		"	</TR>"
        "    <TR>"
        "      <TD>%8</TD>"
        "      <TD>%9</TD>"
		"	</TR>"
        )
        .arg( t1 )
        .arg( t2 )
        .arg( qApp->applicationFilePath() )
        .arg( t3 )
        .arg( appFileSystem()->xmlFilePath() )
        .arg( t4 )
        .arg( appFileSystem()->installPath() )
        .arg( t5 )
        .arg( appFileSystem()->workspacePath() ) ;

	translate( t1, "AppWindow:ProgramInfo:StartupWorksheet" );
	QString msg2b = QString(
        "    <TR>"
        "      <TD>%1</TD>"
        "      <TD>%2</TD>"
		"    </TR>"
        "</TABLE>"
        )
        .arg( t1 )
        .arg( m_startupFile );
    QString msg2 = msg2a + msg2b;

    // Display environment variables
    translate( t1, "AppWindow:ProgramInfo:EnvironmentVariables" );
    QString msg3 = QString(
        "<H2>%1</H2>"
        "<TABLE>"
        "    <TR>"
        "      <TD>$HOME</TD>"
        "      <TD>%2</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>$PATH</TD>"
        "      <TD>%3</TD>"
        "    </TR>"
        "    <TR>"
        "      <TD>$BEHAVEPLUS</TD>"
        "      <TD>%4</TD>"
        "    </TR>"
        "</TABLE>"
        )
        .arg( t1 )
        .arg( getenv( "HOME" ) )
        .arg( getenv( "PATH" ) )
        .arg( getenv( "BEHAVEPLUS" ) );

    // Display monitor metrics
    QPaintDeviceMetrics dm( QApplication::desktop() );
    translate( t1, "AppWindow:ProgramInfo:DisplayMetrics" );
    translate( t2, "AppWindow:ProgramInfo:DesktopWidth" );
    translate( t3, "AppWindow:ProgramInfo:Pix" );
    translate( t4, "AppWindow:ProgramInfo:Mm" );
    translate( t5, "AppWindow:ProgramInfo:In" );
    QString msg4a = QString(
        "<H2>%1</H2>"
        "<TABLE>"
        "  <TR>"
        "    <TD>%2</TD>"
        "    <TD>%3 %4</TD>"
        "    <TD>%5 %6</TD>"
        "    <TD>%7 %8</TD>"
        "  </TR>"
        )
        .arg( t1 )
        .arg( t2 )
        .arg( dm.width() )
        .arg( t3 )
        .arg( dm.widthMM() )
        .arg( t4 )
        .arg( dm.widthMM()/25.4 )
        .arg( t5 );

    translate( t2, "AppWindow:ProgramInfo:DesktopHeight" );
    QString msg4b = QString(
        "  <TR>"
        "    <TD>%1</TD>"
        "    <TD>%2 %3</TD>"
        "    <TD>%4 %5</TD>"
        "    <TD>%6 %7</TD>"
        "  </TR>"
        )
        .arg( t2 )
        .arg( dm.height() )
        .arg( t3 )
        .arg( dm.heightMM() )
        .arg( t4 )
        .arg( dm.heightMM()/25.4 )
        .arg( t5 );

    translate( t2, "AppWindow:ProgramInfo:DesktopColors" );
    translate( t3, "AppWindow:ProgramInfo:Colors" );
    translate( t4, "AppWindow:ProgramInfo:Planes" );
    QString msg4c = QString(
        "  <TR>"
        "    <TD>%1</TD>"
        "    <TD>%2 %3</TD>"
        "    <TD>%4 %5</TD>"
        "    <TD></TD>"
        "  </TR>"
        "</TABLE>"
        )
        .arg( t2 )
        .arg( dm.numColors() )
        .arg( t3 )
        .arg( dm.depth() )
        .arg( t4 ) ;
    QString msg4 = msg4a + msg4b + msg4c;

    // Determine if all image files are present
    QString missing("");
    translate( t1, "AppWindow:ProgramInfo:ImageFiles" );
    QString msg5 = "<H2>" + t1 + "</H2>";
    if ( appFileSystem()->testImageFiles( missing ) )
    {
        translate( t2, "AppWindow:ProgramInfo:ImageFilesPresent" );
        msg5 += t2;
    }
    else
    {
        translate( t2, "AppWindow:ProgramInfo:ImageFilesMissing" );
        msg5 += t2 + missing;
    }
    // Determine if all doc files are present
    missing = "";
    translate( t1, "AppWindow:ProgramInfo:DocFiles" );
    QString msg6 = "<H2>" + t1 + "</H2>";
    if ( appFileSystem()->testHtmlFiles( missing ) )
    {
        translate( t2, "AppWindow:ProgramInfo:DocFilesPresent" );
        msg6 += t2;
    }
    else
    {
        translate( t2, "AppWindow:ProgramInfo:DocFilesMissing" );
        msg6 += t2 + missing;
    }
    // Determine if all geo files are present
    missing = "";
    translate( t1, "AppWindow:ProgramInfo:GeoFiles" );
    QString msg7 = "<H2>" + t1 + "</H2>";
    if ( appFileSystem()->testGeoFiles( missing ) )
    {
        translate( t2, "AppWindow:ProgramInfo:GeoFilesPresent" );
        msg7 += t2;
    }
    else
    {
        translate( t2, "AppWindow:ProgramInfo:GeoFilesMissing" );
        msg7 += t2 + missing;
    }
    // Display all custom fuel model files
    QString msg8("");
    QStringList fileList;
    appFileSystem()->findFuelModelFiles( "*", false, true, &fileList );
    translate( t1, "AppWindow:ProgramInfo:FuelModelFilesTable" );
    makeFileTable( fileList, t1, msg8 );

    // Display all fuel moisture scenario files
    QString msg9("");
    fileList.clear();
    appFileSystem()->findMoisScenarioFiles( "*", false, true, &fileList );
    translate( t1, "AppWindow:ProgramInfo:MoisScenarioFilesTable" );
    makeFileTable( fileList, t1, msg9 );

    // Display all units set files
    QString msg10("");
    fileList.clear();
    appFileSystem()->findUnitsSetFiles( "*", false, true, &fileList );
    translate( t1, "AppWindow:ProgramInfo:UnitsSetFilesTable" );
    makeFileTable( fileList, t1, msg10 );

    // Display all worksheet files
    QString msg11("");
    fileList.clear();
    appFileSystem()->findWorksheetFiles( "*", false, true, &fileList );
    translate( t1, "AppWindow:ProgramInfo:WorksheetFilesTable" );
    makeFileTable( fileList, t1, msg11 );

    // Display all run files
    QString msg12("");
    fileList.clear();
    appFileSystem()->findRunFiles( "*", false, true, &fileList );
    translate( t1, "AppWindow:ProgramInfo:RunFilesTable" );
    makeFileTable( fileList, "Run Files", msg12 );

    // Show it all in an info dialog.
    info( QString( msg1 + msg2 + msg3 + msg4 + msg5 + msg6 + msg7
        + msg8 + msg9 + msg10 + msg11 + msg12 ) );
    log( "End Section: AppWindow::slotHelpInstallationInfo() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the first page of the active Document.
 *
 *  Invoked by the \b FirstPage tool bar button
 *  or by the \b <Ctrl><Home> , \b <Alt><Home> , or \b <Shift><Home> keys.
 *
 *  Document->pageFirst() is called to perform the operation.
 */

void AppWindow::slotPageFirst( void )
{
    log( "Beg Section: AppWindow::slotPageFirst() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->pageFirst();
    }
    log( "\nEnd Section: AppWindow::slotPageFirst() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the last page of the active Document.
 *
 *  Invoked by the \b LastPage tool bar button
 *  or by the \b <Ctrl><End> , \b <Alt><End> , or \b <Shift><End> keys.
 *
 *  Document->pageLast() is called to perform the operation.
 */

void AppWindow::slotPageLast( void )
{
    log( "Beg Section: AppWindow::slotPageLast() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->pageLast();
    }
    log( "End Section: AppWindow::slotPageLast() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the next page of the active Document.
 *
 *  Invoked by the \b NextPage tool bar button
 *  or by the \b <PgDn> key.
 *
 *  Document->pageNext() is called to perform the operation.
 */

void AppWindow::slotPageNext( void )
{
    log( "Beg Section: AppWindow::slotPageNext() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->pageNext();
    }
    log( "End Section: AppWindow::slotPageNext() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the previous page of the active Document.
 *
 *  Invoked by the \b PreviousPage tool bar button
 *  or by the \b <PgUp> key.
 *
 *  Document->pagePrev() is called to perform the operation.
 */

void AppWindow::slotPagePrev( void )
{
    log( "Beg Section: AppWindow::slotPagePrev() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->pagePrev();
    }
    log( "End Section: AppWindow::slotPagePrev() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Rebuilds the AppWindow's main menu \b Pages submenu.
 *
 *  The \b Pages submenu is dynamic and must be rebuilt upon invocation
 *  since it displays a Table of Contents of all pages for the current Document
 *  (e.g., the Document in the active Window).  Not only can the active
 *  Document change, but its page contents can also change.
 *
 *  Called by the QPopupMenu #m_contentsMenu aboutToShow() signal.
 *
 *  Document::contentsMenuRebuild() is called to perform the operation.
 */

void AppWindow::slotPagesMenuAboutToShow( void )
{
    log( "Beg Section: AppWindow::slotPagesMenuAboutToShow() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->contentsMenuRebuild( m_pagesMenu );
    }
    log( "End Section: AppWindow::slotPagesMenuAboutToShow() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Called by the \b Pages submenu activated(int id) signal
 *  indicating the user has selected a menu item (e.g., a page to view).
 *
 *  Document::contentsMenuActivated() is called to perform the operation.
 */

void AppWindow::slotPagesMenuActivated( int id )
{
    log( "Beg Section: AppWindow::slotPagesMenuActivated() invoked ...\n" );
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        doc->contentsMenuActivated( id );
    }
    log( "End Section: AppWindow::slotPagesMenuActivated() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays quit confirmation dialog before quitting.
 */

void AppWindow::slotQuit( void )
{
    QString caption, message;
    translate( caption, "QuitDialog:Caption" );
    translate( message, "QuitDialog:Message" );
    if ( yesno( caption, message ) )
    {
        qApp->closeAllWindows();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets a WorksheetFolder worksheet to the next startup worksheet.
 *
 *  Called only by \b File->Set Startup Worksheet menu selection.
 */

void AppWindow::slotSetStartupWorksheet( void )
{
    log( "Beg Section: AppWindow::slotSetStartupWorksheet() invoked ...\n" );
    setStartupWorksheet();
    log( "End Section: AppWindow::slotSetStartupWorksheet() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Help->Splash Page callback function.
 */

void AppWindow::slotSplashPageShow( void )
{
    log( "Beg Section: AppWindow::slotSplashPageShow() invoked ...\n" );
    m_bpApp->showSplashPage( m_saveSplash );
    log( "End Section: AppWindow::slotSplashPageShow() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the current Document name
 *  in the main application window's status bar.
 *
 *  Normally each Document puts its name in the status bar as it receives focus.
 *  But when all Documents have been closed(), then a different, more
 *  appropriate message needs to be displayed.  This SLOT function is connected
 *  to each Document::destroyed() SIGNAL so that the message is displayed
 *  regardless if invoked from File->Close or the Window manager.
 */

void AppWindow::slotStatusUpdate( void )
{
    log( "Beg Section: AppWindow::slotStatusUpdate() invoked ...\n" );
    // If there is exactly 1 window left in the list, but its caption is null,
    // then the last window has been closed but not yet removed from the list.
    QWidgetList windows = m_workSpace->windowList();
    if ( m_workSpace->windowList().isEmpty()
      || (windows.count() == 1 && windows.at(0)->caption().isNull() ) )
    {
        log( "No more documents, disabling a bunch of controls ...\n" );
        QString text("");
        translate( text, "StatusBar:OpenDocument" );
        statusBar()->message( text );
        // Toggle tool button states
        //m_fileSaveButton->setEnabled( false );
        m_filePrintButton->setEnabled( false );
        m_docModulesButton->setEnabled( false );
        m_docRunButton->setEnabled( false );
        m_pageFirstButton->setEnabled( false );
        m_pageLastButton->setEnabled( false );
        m_pageNextButton->setEnabled( false );
        m_pagePrevButton->setEnabled( false );
        // Toggle menu states
        menuBar()->setItemEnabled( m_idConfigAppearance, false );
        menuBar()->setItemEnabled( m_idConfigFuel, false );
        menuBar()->setItemEnabled( m_idConfigModule, false );
        menuBar()->setItemEnabled( m_idConfigMois, false );
        menuBar()->setItemEnabled( m_idConfigUnits, false );
        menuBar()->setItemEnabled( m_idPages, false );
        //m_fileMenu->setItemEnabled( m_idFileSave, false );
        m_fileMenu->setItemEnabled( m_idFileSaveAs, false );
        m_fileMenu->setItemEnabled( m_idFileCalculate, false );
        m_fileMenu->setItemEnabled( m_idFilePrint, false );
        m_fileMenu->setItemEnabled( m_idFileExport, false );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModels, false );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFarsiteEnglish, false );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFarsiteMetric, false );
        //m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFlammap, false );
        m_fileMenu->setItemEnabled( m_idFileExportResults, false );
        m_fileMenu->setItemEnabled( m_idFileCapture, false );
        m_fileMenu->setItemEnabled( m_idFileClose, false );
        menuBar()->setItemEnabled( m_idView, false );
        menuBar()->setItemEnabled( m_idWindows, false );
    }
    else
    {
        log( "More documents, enabling a bunch of controls ...\n" );
        // Toggle tool button states
        //m_fileSaveButton->setEnabled( true );
        m_filePrintButton->setEnabled( true );
        m_docModulesButton->setEnabled( true );
        m_docRunButton->setEnabled( true );
        m_pageFirstButton->setEnabled( true );
        m_pageLastButton->setEnabled( true );
        m_pageNextButton->setEnabled( true );
        m_pagePrevButton->setEnabled( true );
        // Toggle menu states
        menuBar()->setItemEnabled( m_idConfigAppearance, true );
        menuBar()->setItemEnabled( m_idConfigFuel, true );
        menuBar()->setItemEnabled( m_idConfigModule, true );
        menuBar()->setItemEnabled( m_idConfigMois, true );
        menuBar()->setItemEnabled( m_idConfigUnits, true );
        menuBar()->setItemEnabled( m_idPages, true );
        //m_fileMenu->setItemEnabled( m_idFileSave, true );
        m_fileMenu->setItemEnabled( m_idFileSaveAs, true );
        m_fileMenu->setItemEnabled( m_idFileCalculate, true );
        m_fileMenu->setItemEnabled( m_idFilePrint, true );
        m_fileMenu->setItemEnabled( m_idFileExport, true );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModels, true );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFarsiteEnglish, true );
        m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFarsiteMetric, true );
        //m_fileMenu->setItemEnabled( m_idFileExportFuelModelsFlammap, true );
        m_fileMenu->setItemEnabled( m_idFileExportResults, true );
        m_fileMenu->setItemEnabled( m_idFileCapture, true );
        m_fileMenu->setItemEnabled( m_idFileClose, true );
        menuBar()->setItemEnabled( m_idView, true );
        menuBar()->setItemEnabled( m_idWindows, true );
    }
    log( "End Section: AppWindow::slotStatusUpdate() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the fine dead fuel moisture tool.
 *
 *  Called only by the \b Tools->Fine dead fuel moisture menu selection.
 */

void AppWindow::slotToolsFdfmcDialog( void )
{
    log( "Beg Section: AppWindow::slotToolsFdfmcDialog() invoked ...\n" );
    FdfmcDialog dialog( this, "fdfmcDialog",
        m_bpApp->m_program, m_bpApp->m_version );
    dialog.exec();
    log( "End Section: AppWindow::slotToolsFdfmcd() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Slope vs horizontal map distance tool.
 *
 *  Called only by the \b Tools->'Slope vs horizontal map distance' menu selection.
 */

void AppWindow::slotToolsHorizontalDistance( void )
{
    log( "Beg Section: AppWindow::slotToolsHorizontalDistance() invoked ...\n" );
    HorizontalDistanceDialog dialog( this );
    dialog.exec();
    log( "End Section: AppWindow::slotToolsHorizontalDistance() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the relative humidity tool.
 *
 *  Called only by the \b Tools->Relative humidity menu selection.
 */

void AppWindow::slotToolsHumidityDialog( void )
{
    log( "Beg Section: AppWindow::slotToolsHumidityDialog() invoked ...\n" );
    HumidityDialog dialog( this );
    dialog.exec();
    log( "End Section: AppWindow::slotToolsHumidity() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the 'slope from map measurements' tool.
 *
 *  Called only by the \b Tools->Slope from map measurements menu selection.
 */

void AppWindow::slotToolsSlopeFromMapMeasurements( void )
{
    log( "Beg Section: AppWindow::slotToolsSlopeFromMapMeasurements() invoked ...\n" );
    SlopeToolDialog dialog( this, "slopeToolDialog",
        m_bpApp->m_program, m_bpApp->m_version );
    dialog.exec();
    log( "End Section: AppWindow::slotToolsSlopeFromMapMeasurements() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Sun-Moon Calendar Dialog.
 *
 *  Called only by the \b Tools->Sun-Moon Calendar menu selection.
 */

void AppWindow::slotToolsSunMoonCalendar( void )
{
    log( "Beg Section: AppWindow::slotSunMoonCalendar() invoked ...\n" );
    // Create a local GlobalPosition and DateTime to pass to sunDialog.
    DateTime dt;
    GlobalPosition gp(
        appProperty()->string( "calLocationName" ),
        appProperty()->string( "calZoneName" ),
        appProperty()->real( "calLongitude" ),
        appProperty()->real( "calLatitude" ),
        appProperty()->real( "calGmtDiff" ) );
    // Display the dialog
    SunDialog dialog( this, &dt, &gp, appProperty() );
    log( "Display location dialog ...\n" );
    if ( dialog.exec() != QDialog::Accepted )
    {
        log( "Dialog cancelled.\n" );
        log( "End Section: AppWindow::slotSunMoonCalendar() completed.\n" );
        return;
    }
    // Store the returned properties
    log( QString( "Location is \"%1\" in zone \"%2\" at lat %3 lon %4.\n" )
        .arg( gp.locationName() ).arg( gp.zoneName() )
        .arg( gp.latitude() ).arg( gp.longitude() ) );
    appProperty()->string( "calLocationName", gp.locationName() );
    appProperty()->string( "calZoneName", gp.zoneName() );
    appProperty()->real( "calLatitude", gp.latitude() );
    appProperty()->real( "calLongitude", gp.longitude() );
    appProperty()->real( "calGmtDiff", gp.gmtDiff() );

    // Create a new Document with the next document id for this process
    log( "Creating a new CalendarDocument instance ...\n" );
    CalendarDocument *doc = new CalendarDocument(
        m_workSpace,
        m_docIdCount+1,
        appProperty(),
        QString( "calendarDocument%1" ).arg( m_docIdCount+1 ),
        WDestructiveClose );
    checkmem( __FILE__, __LINE__, doc, "CalendarDocument doc", 1 );
    m_docIdCount++;

    // Display any messages emitted by the Document in the status bar
    connect( doc,         SIGNAL( message(const QString&) ),
             statusBar(), SLOT(   message(const QString&) ) );

    // Set the CalendarDocument's caption and window icon.
    doc->setIcon( m_documentIcon->pixmap() );
    doc->setCaption( appProperty()->string( "calLocationName" ) );

    // Display the Seasons table if requested.
    if ( appProperty()->boolean( "calSeasonsActive" ) )
    {
        log( "Composing the Seasons page ...\n" );
        doc->composeSeasons( &dt, &gp );
    }
    // Display the Sun-Moon Chart if requested.
    if ( appProperty()->boolean( "calChartActive" ) )
    {
        log( "Composing the Sun-Moon Chart ...\n" );
        doc->composeChart( &dt, &gp );
    }
    // Display the Calendar if requested.
    if ( appProperty()->boolean( "calCalendarActive" ) )
    {
        log( "Composing the Calendar page ...\n" );
        doc->composeCalendar( &dt, &gp );
    }
    // Show the first page and return
    doc->showPage( 1 );
    doc->showMaximized();
    doc->m_scrollView->setContentsPos( 0, 0 );
    log( "End Section: AppWindow::slotSunMoonCalendar() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the units conversion tool.
 *
 *  Called only by the \b Tools->Units converter menu selection.
 *  Moved to the Configure->Custom units definition menu selection.
 */

void AppWindow::slotToolsUnitsConverter( void )
{
    log( "Beg Section: AppWindow::slotToolsUnitsConverter() invoked ...\n" );
    UnitsConverterDialog dialog( this, "unitsConverterDialog" );
    dialog.exec();
    log( "End Section: AppWindow::slotToolsUnitsConverter() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the units conversion tool.
 *
 *  Called only by the \b Tools->Units converter menu selection.
 */

void AppWindow::slotToolsUnitsEditor( void )
{
    log( "Beg Section: AppWindow::slotToolsUnitsEditor() invoked ...\n" );
    // Store the current document (if any)
    Document *activeDoc = (Document *) m_workSpace->activeWindow();
    Document *doc = activeDoc;
    // If there is no active BpDocument, create one
    if ( ( ! doc )
      || ( doc->m_docType != "BpDocument" ) )
    {
        log( "Creating a dummy BpDocument instance ...\n" );
        doc = new BpDocument(
            m_workSpace,
            m_docIdCount+1,
            m_eqApp,
            QString( "BpDocument%1" ).arg( m_docIdCount+1 ),
            WDestructiveClose );
        checkmem( __FILE__, __LINE__, doc, "BpDocument doc", 1 );

        // Display any messages emitted by the Document in the status bar
        connect( doc,         SIGNAL( message(const QString&) ),
                 statusBar(), SLOT(   message(const QString&) ) );

        // Must do this to notify QWorkspace of a new document
        QString text("");
        doc->setIcon( m_documentIcon->pixmap() );
        QString fileName = appFileSystem()->worksheetPath(
            "ExampleWorksheets/0Default.bpw" );
        log( "Trying to open 0Default.bpw ...\n" );
        if ( ! doc->open( fileName, "Worksheet" ) )
        // This code block should never be executed!
        {
            translate( text, "AppWindow:0Default.bpw:Missing" );
            error( text );
            delete doc;     doc = 0;
            m_docIdCount--;
            return;
        }
        translate( text, "UnitsEditDialog:Caption" );
        doc->setCaption( text );
        doc->showMaximized();
        doc->m_scrollView->setContentsPos( 0, 0 );
    }
    // Display the editor
    log( "Displaying the UnitsEditDialog ...\n" );
    UnitsEditDialog dialog( (BpDocument *) doc );
    if ( dialog.exec() == QDialog::Accepted )
    {
        log( "Reconfiguring ...\n" );
        ((BpDocument *) doc)->configure();
    }
    // Close the editor and redisplay the previous document
    log( "Redisplaying the active document ...\n" );
    if ( doc != activeDoc )
    {
        doc->close();
    }
    if ( activeDoc )
    {
        activeDoc->setFocus();
        activeDoc->showMaximized();
        doc->m_scrollView->setContentsPos( 0, 0 );
    }
    slotStatusUpdate();
    log( "End Section: AppWindow::slotToolsUnitsEditor() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the \b View submenu.
 *
 *  The \b View submenu allows the user to scale the view.  Text documents
 *  probably use a font size as a scale metric.  Graphic Documents may use
 *  a scaling factor.  This is a dynamic submenu because each Document may
 *  have a different current scale value, a different method of presenting
 *  the scale, and a different number of scales.
 *
 *  The Document will also connect the activation signal to its own rescale().
 *
 *  Called only by the \b View submenu aboutToShow() signal.
 */

void AppWindow::slotViewMenuAboutToShow( void )
{
    log( "Beg Section: AppWindow::slotViewMenuAboutToShow() invoked ...\n" );
    // Get the current scaled font size for the active document.
    Document *doc = getActiveWindow( "" );
    if ( doc )
    {
        m_viewMenu->clear();
        doc->viewMenuAboutToShow( m_viewMenu );
    }
    log( "End Section: AppWindow::slotViewMenuAboutToShow() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Rebuilds the AppWindow's main menu \b Windows submenu.
 *
 *  The \b Windows submenu is dynamic and must be rebuilt upon invocation
 *  since it displays a list of all available Documents in the QWorkspace.
 *
 *  Called by the QPopupMenu #m_windowsMenu aboutToShow() signal.
 */

void AppWindow::slotWindowsMenuAboutToShow( void )
{
    log( "Beg Section: AppWindow::slotWindowsMenuAboutToShow() invoked ...\n" );
    // Clear the windows menu
    m_windowsMenu->clear();
    // Cascade windows
    QString text("");
    translate( text, "Menu:Windows:Cascade" );
    m_idWindowsCascade = m_windowsMenu->insertItem( text,
        m_workSpace,
        SLOT( cascade() ) );
    // Tile windows
    translate( text, "Menu:Windows:Tile" );
    m_idWindowsTile = m_windowsMenu->insertItem( text,
        m_workSpace,
        SLOT( tile() ) );

    // The main menu Windows item should be disabled if there are no windows
    if ( m_workSpace->windowList().isEmpty() )
    // This code block should never be executed!
    {
        m_windowsMenu->setItemEnabled( m_idWindowsCascade, false );
        m_windowsMenu->setItemEnabled( m_idWindowsTile, false );
    }
    // Add a separator
    m_windowsMenu->insertSeparator();
    // Insert menu item for each open window
    QWidgetList windows = m_workSpace->windowList();
    for ( int i = 0;
          i < int( windows.count() );
          ++i )
    {
        int id = m_windowsMenu->insertItem(
            windows.at(i)->caption(),
            this,
            SLOT( slotWindowsMenuActivated( int ) ) );
        m_windowsMenu->setItemParameter( id, i );
        m_windowsMenu->setItemChecked( id,
            m_workSpace->activeWindow() == windows.at(i) );
    }
    log( "End Section: AppWindow::slotWindowsMenuAboutToShow() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Applies focus to the Document selected from the \b Windows submenu.
 *
 *  Called only by the \b Windows submenu activated(int id) SIGNAL.
 *
 *  \param id Index of the selected Document.
 */

void AppWindow::slotWindowsMenuActivated( int id )
{
    log( "Beg Section: AppWindow::slotWindowsMenuActivated() invoked ...\n" );
    Document *doc = (Document *) m_workSpace->windowList().at( id );
//fprintf( stderr, "AppWindow::slotWindowsMenuActivated(%d) activates Doc %x id %d page %d of %d\n",
//id, doc, doc->m_docId, doc->m_page, doc->m_pages);
    if ( doc )
    {
        doc->setFocus();
        //qApp->processEvents();
        //doc->showMaximized();
        //doc->m_scrollView->setContentsPos( 0, 0 );
    }
    log( "End Section: AppWindow::slotWindowsMenuActivated() completed.\n" );
    return;
}

//------------------------------------------------------------------------------
//  End of appwindow.cpp
//------------------------------------------------------------------------------
