//------------------------------------------------------------------------------
/*  \file document.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2010 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief MDI document base class methods.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "docscrollview.h"
#include "doctabs.h"
#include "document.h"
#include "filesystem.h"
#include "pagemenudialog.h"
#include "printer.h"

// Qt include files
#include <qapplication.h>
#include <qcursor.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qfontmetrics.h>
#include <qimage.h>
#include <qobject.h>
#include <qpaintdevicemetrics.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qprogressdialog.h>
#include <qtextstream.h>
#include <qworkspace.h>

// Standard include files
#include <time.h>

//------------------------------------------------------------------------------
/*! \brief Document abstract base class constructor.
 *
 *  \param workspace Pointer to the #ApplicationWindow::m_workSpace.
 *  \param docId Unique serial number for the Document in this process.
 *  \param name Document's QWidget name.
 *  \param wflags QWidget #wflags.
 */

Document::Document( QWorkspace *workspace, const QString &docType, int docId,
        const char* name, int wflags ) :
    QMainWindow( workspace, name, wflags ),
    m_scrollView(0),
    m_pageSize(0),
    m_screenSize(0),
    m_tocList(0),
    m_contextMenu(0),
    m_maintenanceMenu(0),
    m_composer(0),
    m_tabs(0),
    m_docType(docType),
    m_absPathName(""),
    m_baseName(""),
    m_runTime(""),
    m_fixedFont("Courier New",12),
    m_propFont("Times New Roman",12),
    m_fontBaseSize(12),
    m_fontScaleSize(12),
    m_docId(docId),
    m_pages(0),
    m_page(0)
{
    // DocPageSize defines the page's physical dimensions (inches)
    m_pageSize = new DocPageSize();
    Q_CHECK_PTR( m_pageSize );

    // DocDeviceSize defines the physical device's dimensions (pixels)
    m_screenSize = new DocDeviceSize( QApplication::desktop(),
        m_pageSize, 1.0 );
    Q_CHECK_PTR( m_screenSize );

    // DocScrollView display a single, scrollable page of the Document.
    m_scrollView = new DocScrollView( this, m_screenSize, name );
    Q_CHECK_PTR( m_scrollView );

    // The DocScrollView is the central widget of the Document.
    setFocusProxy( m_scrollView );
    setCentralWidget( m_scrollView );

    // Create the Composer
    m_composer = new Composer();
    Q_CHECK_PTR( m_composer );

    // Table of contents
    m_tocList = new TocList();
    Q_CHECK_PTR( m_tocList );

    // Create the Document tabs
    m_tabs = new DocTabs( 0, 4, "Tab 1" );
    Q_CHECK_PTR( m_tabs );

    // Note that the contextMenu must be created in the derived class's
    // constructor since it is a virtual function call.
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document destructor.
 *
 *  Removes all dynamically allocated sub-components
 *  and all temporary Composer disk files.
 */

Document::~Document()
{
    // Remove all the temporary composer files used by this Document.
    removeComposerFiles();
    // Delete the dynamically-allocated protected data members.
    delete m_tocList;       m_tocList = 0;
    delete m_pageSize;      m_pageSize = 0;
    delete m_screenSize;    m_screenSize = 0;
    delete m_scrollView;    m_scrollView = 0;
    delete m_composer;      m_composer = 0;
    delete m_contextMenu;   m_contextMenu = 0;
    delete m_tabs;          m_tabs = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Captures the current Document page to an image file.
 *
 *  Called by ApplicationWindow::slotDocumentCapture() via \b File->Capture
 *  and by Document::contextMenuActivated().
 *
 *  Supports the following output formats:
 *  \arg BMP
 *  \arg JPEG
 *  \arg PNG
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool Document::capture( void )
{
    /*! \var  static const char *format[]
        \brief Image file format name recognized by Qt.
    */
    static const char *format[] = { "BMP", "JPEG", "PNG", NULL };
    /*! \var static const char *extension[]
        \brief Image file name extension corresponding to the Format[].
    */
    static const char *extension[] = { "bmp", "jpg",  "png", NULL };

    // Display the file dialog to request the output file name
    QFileDialog fd( this, "captureFileDialog", true );
    fd.setMode( QFileDialog::AnyFile );
    fd.setFilters( QString( "BMP (*.bmp);;JPEG (*.jpg);;PNG (*.png);;" ) );
    if ( fd.exec() != QDialog::Accepted )
    {
        return( false );
    }
    // Check that a file name was selected
    QString fileName = fd.selectedFile();
    if ( fileName.isEmpty() )
    {
        return( false );
    }
    // Check if the user already specified a known file extension
    QFileInfo fi( fileName );
    QString qext = fi.extension( false );
    const char *ext = (const char *) qext;
    int id;
    for ( id = 0;
          extension[id];
          id++ )
    {
        if ( strcmp( ext, extension[id] ) == 0 )
        {
            break;
        }
    }
    // If the file has a non-standard extension; use the filter extension
    if ( ! extension[id] )
    {
        QString filter = fd.selectedFilter().left(3);
        for ( id = 0;
              format[id];
              id++ )
        {
            if ( strncmp( filter, format[id], 3 ) == 0 )
            {
                break;
            }
        }
        fileName.append( "." );
        fileName.append( extension[id] );
    }
    // Save the file.
    QImageIO iio;
    QImage   image;
    image = m_scrollView->m_backingPixmap;
    iio.setImage( image );
    iio.setFileName( fileName );
    iio.setFormat( format[id] );
    if ( ! iio.write() )
    {
        QString msg("");
        translate( msg, "Document:Capture:Error", fileName, format[id] );
        error( msg );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Default virtual method to handle non-BehavePlusDocuments.
 *
 *  \param showRunDialog UNUSED.
 */

void Document::clear( bool /* showRunDialog */ )
{
    QString msg("");
    translate( msg, "Document:Run:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the logo at the specified offset with the specified size.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which does nothing.
 *  It probably needs to be re-implemented in every derived Document class.
 *
 *  \param x0 Horizontal distance from the left edge of the page in inches.
 *  \param y0 Vertical distance from top edge of the page in inches.
 *  \param wd Width in inches.
 *  \param ht Height in inches.
 *  \param penWd Pen width in pixels.
 */

void Document::composeLogo( double x0, double y0, double wd, double ht,
    int penWd )
{
    // Prevent compiler warnings by using the args
    x0 = y0 = wd = ht = 0.0;
    penWd = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the page background and decoration.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which simply draws a white page background, page number, and any page tabs.
 *  It probably needs to be re-implemented in every derived Document class.
 *
 *  Should \b only be called by Document::startNewPage().
 */

void Document::composeNewPage( )
{
    // Add the new page with a white background.
    m_composer->fill( 0., 0., m_pageSize->m_pageWd, m_pageSize->m_pageHt,
        QBrush( "white", Qt::SolidPattern ) );

    // Add all standard / optional page decoration here.
    double pad = 0.1;

    // Page number at right header
    QString text("");
    translate( text, "Document:Page" );
    m_composer->text(
        m_pageSize->m_marginLeft,
        0.5 * m_pageSize->m_marginTop - pad,
        m_pageSize->m_bodyWd,
        0.5 * m_pageSize->m_marginTop,
        Qt::AlignVCenter|Qt::AlignRight,
        QString( "%1 %2" ).arg( text ).arg( m_pages ) );

    // Add a tab, if requested, and return
    if ( m_tabs->active() )
    {
        composeTab();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the page tab.
 *
 *  Called only by composeNewPage().
 *
 *  This is a virtual function that may be re-implemented in derived classes.
 */

void Document::composeTab( void )
{
    // Determine tab locations in the right page margin
    double tabRight  = m_pageSize->m_pageWd - 0.1;
    double tabLeft   = m_pageSize->m_bodyRight + 0.1;
    double tabLength = m_pageSize->m_bodyHt / (double) m_tabs->m_tabs;
    double tabTop    = m_pageSize->m_marginTop + (m_tabs->m_tab-1) * tabLength;
    double tabBottom = tabTop + tabLength;
    double tabCut    = 0.1;

    // Draw the tab outline
    QPen pen( black );
    m_composer->pen( pen );
    m_composer->line( tabLeft,  tabTop,           tabRight, tabTop+tabCut );
    m_composer->line( tabRight, tabTop+tabCut,    tabRight, tabBottom-tabCut );
    m_composer->line( tabRight, tabBottom-tabCut, tabLeft,  tabBottom );
    m_composer->line( tabLeft,  tabBottom,        tabLeft,  tabTop );

    // Print the tab text rotated and centered within the tab outline
    //composer->rotateText( tabLeft, tabBottom-tabCut,
    //    tabLength-2.*tabCut, tabRight-tabLeft, 270., text );
    QPen tabPen( m_tabs->m_fontColor );
    m_composer->pen( tabPen );
    //m_tabs->tabFont().setPointSize( 20 - m_tabs->tabs() );
    QFont tabFont( m_tabs->m_fontFamily, m_tabs->m_fontSize );
    m_composer->font( tabFont );
    m_composer->rotateText(
        tabRight,
        tabTop + tabCut,
        tabLength -2. * tabCut,
        tabRight - tabLeft,
        90.,
        m_tabs->m_tabText );
    m_composer->pen( pen );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Document's reconfiguration to do such things as
 *  change languages.
 */

void Document::configure( void )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Worksheet Configuration Dialog.
 *
 *  Called only by ApplicationWindow::slotConfigureAppearance().
 */

void Document::configureAppearance( void )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Fuel Models Configuration Dialog.
 *
 *  Called only by ApplicationWindow::slotConfigureFuelModels().
 */

void Document::configureFuelModels( void )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Moisture Scenarios Configuration Dialog.
 *
 *  Called only by ApplicationWindow::slotConfigureMoistureScenarios().
 */

void Document::configureMoistureScenarios( void )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Modules Configuration Dialog.
 *
 *  Called only by ApplicationWindow::slotConfigureModules().
 */

void Document::configureModules( void )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes the Units Configuration Dialog.
 *
 *  \param unitsSet UNUSED
 *  Called only by ApplicationWindow::slotConfigureUnits().
 */

void Document::configureUnits( const QString & /*unitsSet */ )
{
    QString msg("");
    translate( msg, "Document:Configure:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the page number, title, and page/icon type #TocType to the
 *  Document's table of contents.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \param page Page number.
 *  \param pageTitle Page title text.
 *  \param tocType Page #TocType (so the appropriate page icon is displayed).
 */

void Document::contentsAddItem( int page, const QString &pageTitle,
        TocType tocType )
{
    m_tocList->addItem( page, pageTitle, tocType );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clears the ToC of all items.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::contentsClear( void )
{
    m_tocList->clear();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback to display the requested table of contents page.
 *
 *  Called by ApplicationWindow::slotContentsMenuActivated() whenever a
 *  \b File->Content submenu selection is made.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \param pageNumber Number of page to be displayed.
 */

void Document::contentsMenuActivated( int pageNumber )
{
    showPage( m_tocList->itemPage( pageNumber ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Rebuilds the \b File->Cotents submenu from scratch so it displays
 *  the Document's current table of contents.
 *
 *  Called by ApplicationWindow::slotContentsAboutToShow() indicating the
 *  \b File->Contents menu is QPopupMenu::aboutToShow().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \param contentsMenu Pointer to the #ApplicationWindow::m_contentsMenu.
 */

void Document::contentsMenuRebuild( QPopupMenu *contentsMenu )
{
    contentsMenu->clear();
    // If not oo many pages, show them in a standard popup menu
    if ( m_tocList->count() < 20 )
    {
        m_tocList->rebuildMenu( contentsMenu, m_page );
    }
    // If a lot of pages, show them in a list view
    else
    {
        PageMenuDialog *dialog = new PageMenuDialog( this, m_page,
            contentsMenu );
        if ( dialog->exec() == QDialog::Accepted )
        {
            if ( dialog->selectedPage() >= 0 )
            {
                showPage( m_tocList->itemPage( dialog->selectedPage() ) );
            }
        }
        contentsMenu->close();
        delete dialog;  dialog = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the Document's popup context menu has been
 *  activated.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which calls back a save(), saveAs(), print(), or capture() method.
 *  It probably needs to be re-implemented in every derived Document class.
 *
 *  \param id #ContextMenuOption of the selected option.
 */

void Document::contextMenuActivated( int id )
{
    if ( id == ContextSave )
    {
        save( m_absPathName, "Native" );
    }
    else if ( id == ContextSaveAs )
    {
        // Setting fileName to 0 forces a file dialog
        save( 0, "Native" );
    }
    else if ( id == ContextPrint )
    {
        print();
    }
    else if ( id == ContextCapture )
    {
        capture();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the Document's popup context menu.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which creates a menu with Save, saveAs, Print, and Capture options.
 *  It probably needs to be re-implemented in every derived Document class.
 *
 *  Called only by the Document() constructor.
 */

void Document::contextMenuCreate( void )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    // Add the menu items.
    int mid;
    QString text("");
    // Save
    translate( text, "Menu:File:Save" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextSave );
    // SaveAs
    translate( text, "Menu:File:SaveAs" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextSaveAs );
    // Separator
    m_contextMenu->insertSeparator();
    // Print
    translate( text, "Menu:File:Print" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrint );
    // Capture
    translate( text, "Menu:File:Capture" );
    mid = m_contextMenu->insertItem( text,
             this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextCapture );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the Document's context-sensitive popup menu.
 *
 *  May need to override this for derived Document classes.
 *
 *  Called only by the Document's DocScrollView::viewportMouseReleaseEvent().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::contextMenuShow( void )
{
    m_contextMenu->exec( QCursor::pos() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current scale font size.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

int Document::fontScaleSize( void ) const
{
    return( m_fontScaleSize );
}

//------------------------------------------------------------------------------
/*! \brief Invokes a possible maintenance menu.
 *
 *  Should be re-implemented by derived classes.
 *
 *  Invoked by <Ctl>-<alt>-<m>.
 *
 *  Called only by DocScrollView::keyPressEvent().
 */

void Document::maintenance( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens the selected file into the Document's DocScrollView.
 *
 *  Called only by ApplicationWindow::slotOpen().
 *
 *  This is the default virtual method for the Document abstract class,
 *  which opens a presumed text file and displays it.
 *  It probably needs to be re-implemented in every derived Document class.
 *
 *  \param fileName Name of the Document file to open.
 *  \param fileType A one-word string describing the file type.  If this
 *  method does not recognize the string, an error message is displayed.
 *
 *  \retval TRUE if the file is successfully opened.
 *  \retval FALSE if the file couldn't be opened.
 */

bool Document::open( const QString &fileName, const QString &fileType )
{
    // Check if we can open this type of output file
    if ( fileType != "Text" && fileType != "text"
      && fileType != "Native" && fileType != "native" )
    {
        QString msg("");
        translate( msg, "Document:Open:WrongType", fileType );
        warn( msg );
        return( false );
    }
    // Attempt to open the file.
    QFile file( fileName );
    if ( ! file.open( IO_ReadOnly ) )
    {
        QString msg("");
        translate( msg, "Document:Open:NoOpen", fileType, fileName );
        warn( msg );
        return( false );
    }
    // Save the Document file path and base names
    m_absPathName  = fileName;
    QFileInfo fileInfo( file );
    m_baseName = fileInfo.baseName();

    // Open a new composer file for the first page
    startNewPage( "", TocDocument );
    m_composer->font( m_fixedFont );
    QFontMetrics fontMetrics( m_fixedFont );
    double yStep = (double) fontMetrics.lineSpacing()
                 / m_screenSize->m_yppi;
    double yPos = m_pageSize->m_marginTop + yStep;

    // Open the source file
    QTextStream textStream( &file );
    while ( ! textStream.eof() )
    {
        // Check if a new page is needed.
        if ( yPos > m_pageSize->m_bodyEnd )
        {
            startNewPage( "", TocDocument );
            m_composer->font( m_fixedFont );
            yPos = m_pageSize->m_marginTop + yStep;
        }
        // Draw this line of text
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,
            m_pageSize->m_bodyWd, yStep,
            Qt::AlignLeft|Qt::AlignVCenter,
            textStream.readLine() );
        // Increment to the next line
        yPos += yStep;
    }
    // Close the composer and source file
    m_composer->end();
    file.close();

    // Draw the first page in the Document's DocScrollView.
    setCaption( m_baseName );
    showPage( 1 );
    // Force full page display with scrollbars!!!
    showMaximized();
    // Display a message and return TRUTH.
    statusUpdate();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Displays the first page of the Document.
 *
 *  Called only by ApplicationWindow::slotFirstPage().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::pageFirst( void )
{
    showPage( 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the last page of the Document.
 *
 *  Called only by ApplicationWindow::slotLastPage().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::pageLast( void )
{
    showPage( m_pages );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the next page of the Document.
 *  Does nothing if the last page is already on display.
 *
 *  Called only by ApplicationWindow::slotNextPage().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::pageNext( void )
{
    if ( m_page < m_pages )
    {
        showPage( m_page + 1 );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the previous page of the Document.
 *  Does nothing if the first page is already on display.
 *
 *  Called only by ApplicationWindow::slotPrevPage().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::pagePrev( void )
{
    if ( m_page > 1 )
    {
        showPage( m_page - 1 );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current display page height in pixels.
 *
 *  Called only by ApplicationWindow constructor on a dummy Document
 *  to estimate an initial window size.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \return Display page height in pixels.
 */

int Document::pageHtPixels( void ) const
{
    return( m_screenSize->m_pageHt );
}

//------------------------------------------------------------------------------
/*! \brief Gets the current display page width in pixels.
 *
 *  Called only by ApplicationWindow constructor on a dummy Document
 *  to estimate an initial window size.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \return Display page width in pixels.
 */

int Document::pageWdPixels( void ) const
{
    return( m_screenSize->m_pageWd );
}

//------------------------------------------------------------------------------
/*! \brief Prints the Document file to a printer device.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which calls the Composer to draw the file on the print device.
 *  It probably works for most text-based derived Document class.
 *
 *  Called only by ApplicationWindow::print().
 *
 *  \return TRUE on success, FALSE on failure or cancel.
 */

bool Document::print( void )
{
    // Display the print dialog and return if OK not pressed.
    Printer printer;
    printer.setCreator( "BehavePlus6" );
    printer.setDocName( "BehavePlus6" );
    printer.setColorMode( QPrinter::Color );
    printer.setPrintRange( QPrinter::PageRange );
    printer.setFromTo( 1, m_pages );
    printer.setMinMax( 1, m_pages );
    printer.setNumCopies( 1 );
    printer.setOrientation( QPrinter::Portrait );
    printer.setPageOrder( QPrinter::FirstPageFirst );
    printer.setPageSize( QPrinter::Letter );
    if ( ! printer.setup() )
    {
        return( false );
    }

    // Get the printer resolution
    QPaintDeviceMetrics pdm( &printer );
    double xppi = (double) pdm.logicalDpiX();
    double yppi = (double) pdm.logicalDpiY();

    // Compose each page onto the printer.
    QString composerFile;

    // According to Qt docs: "If fromPage() and toPage() both return 0 this
    // signifies 'print the whole document'."
    int fromPage = printer.fromPage();
    int toPage = printer.toPage();
    if ( ! fromPage && ! toPage )
    {
        fromPage = 1;
        toPage = m_pages;
    }
    // First page first
    if ( printer.pageOrder() == QPrinter::FirstPageFirst )
    {
        for ( int page = fromPage;
              page <= toPage;
              page++ )
        {
            if ( printer.aborted() )
            {
                return( false );
            }
            composerFile = appFileSystem()->composerFilePath( m_docId, page );
            m_composer->paint( composerFile, &printer, xppi, yppi,
                m_screenSize->m_scale, true );
            printer.newPage();
        }
    }
    // Last page first
    else
    {
        for ( int page = toPage;
              page >= fromPage;
              page-- )
        {
            if ( printer.aborted() )
            {
                return( false );
            }
            composerFile = appFileSystem()->composerFilePath( m_docId, page );
            m_composer->paint( composerFile, &printer, xppi, yppi,
                m_screenSize->m_scale, true );
            printer.newPage();
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Prints a range of Document pages to series of PostScript files.
 *
 *  The PostScript files have the Document file's base name plus a 3-digit
 *  page number as its file name; e.g. "Test-001.ps".
 *
 *  \param fromPage First page of the Document to print.
 *  \param thruPage Last page of the Document to print.
 *
 *  \return TRUE on success, FALSE on failure or cancel.
 */

bool Document::printPS( int fromPage, int thruPage )
{
    // Constrain number of pages.
    if ( thruPage > m_pages )
    {
        thruPage = m_pages;
    }
    // Create the printer
    Printer printer;
    // Get the printer resolution
    QPaintDeviceMetrics pdm( &printer );
    double xppi = (double) pdm.logicalDpiX();
    double yppi = (double) pdm.logicalDpiY();

    // Set up the progress dialog.
    int step = 0;
    int steps = thruPage - fromPage + 1;
    QString text("");
    QString button("");
    translate( text, "Document:PrintPS:Text", m_baseName );
    translate( button, "Document:PrintPS:Abort" );
    QProgressDialog *progress = new QProgressDialog( text, button, steps );
    Q_CHECK_PTR( progress );
    progress->setMinimumDuration( 0 );
    progress->setProgress( 0 );

    // Print each page.
    QString fileName("");
    QString composerFile;
    for ( int page = fromPage;
          page <= thruPage;
          page++ )
    {
        fileName.sprintf( "%s-%03d.ps", m_baseName.latin1(), page );
        printer.setOutputFileName( fileName );
        composerFile = appFileSystem()->composerFilePath( m_docId, page );
        m_composer->paint( composerFile, &printer, xppi, yppi,
            m_screenSize->m_scale, true );
        printer.newPage();
        // Update progress dialog.
        progress->setProgress( ++step );
        qApp->processEvents();
        if ( progress->wasCancelled() )
        {
            delete progress;    progress = 0;
            return( false );
        }
    }
    // Clean up and return.
    delete progress;    progress = 0;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Removes all the Composer files created for this Document
 *  starting at \a fromPageNumber.
 *
 *  Calling removeComposerFiles( 1 ) is the same as calling the overloaded
 *  function removeComposerFiles().
 *
 *  \param fromPageNumber Number of the first page to be removed (the first
 *  page is page 1, NOT PAGE 0).
 */

void Document::removeComposerFiles( int fromPageNumber )
{
    for ( int i = fromPageNumber;
          i <= m_pages;
          i++ )
    {
        QFile::remove( appFileSystem()->composerFilePath( m_docId, i ) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Re-scales the Document's fonts and screen display using the ratio
 *  of the new \a points to the default font point size as a scaling factor.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which scales based upon a user-selected font size from the \b View menu.
 *  It probably works for most text-based derived Document classes.
 *
 *  For graphics documents we will probably have to move the \b View menu
 *  building routine into the derived class so it can display some other
 *  sort of scaling mechanism.
 *
 *  \param points Font point size used for a scale factor.
 */

void Document::rescale( int points )
{
    // Rescale the screen device logical pixel-to-inch sizes
    double scale = (double) points / (double) m_fontBaseSize;
    m_screenSize->reset( QApplication::desktop(), m_pageSize, scale );

    // Re-scale the font sizes
    m_fontScaleSize = points;
    m_fixedFont.setPointSize( m_fontScaleSize );
    m_propFont.setPointSize( m_fontScaleSize );

    // Rescale the backing QPixmap in the scrollView
    m_scrollView->rescale();

    // Re-display the current page in the new scale
    showPage( m_page );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Default virtual method to handle non-BehavePlusDocuments.
 *
 *  \param showRunDialog UNUSED.
 */

void Document::reset( bool /* showRunDialog */ )
{
    QString msg("");
    translate( msg, "Document:Run:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Default virtual method to handle non-BehavePlusDocuments.
 *
 *  \param showRunDialog UNUSED.
 */

void Document::run( bool /* showRunDialog */ )
{
    QString msg("");
    translate( msg, "Document:Run:Error" );
    warn( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Writes the current Document to a (possibly) new file location
 *  and updates the Document's file name.
 *
 *  Called only by ApplicationWindow::save().
 *
 *  \param fileName Name of the file to be (over)written.
 *  If empty or NULL, a file selection dialog is displayed using the current
 *  #m_absPathName as the default.
 *  \param fileType A one-word string describing the file type.  If this
 *  method does not recognize the string, an error message is displayed.
 */

void Document::save( const QString &fileName, const QString &fileType )
{
    // Check if we can generate this type of output file
    if ( fileType != "Text" && fileType != "text"
      && fileType != "Native" && fileType != "native" )
    {
        QString msg("");
        translate( msg, "Document:Save:WrongType", fileType );
        warn( msg );
        return;
    }
    // Request a file name if none was given
    QString newFileName("");
    if ( fileName == 0 )
    {
        fprintf( stderr, "Document::save() -- request fileName here.\n" );
    }
    // Return if no file name was selected
    if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        return;
    }
    // Store the new file name in m_absPathName and m_basename
    QFileInfo fi( newFileName );
    m_absPathName = fi.absFilePath();
    m_baseName = fi.baseName();

    // Save the file
    fprintf( stderr, "Document::save() -- write file here.\n" );

    // Update the status bar
    statusUpdate();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Document's run time to the current system date and time.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which sets the current date and time string.
 *  This needs to be re-implemented in the derived Document class.
 *
 *  \return On return #m_runTime contains the formatted date and time.
 */

void Document::setRunTime( void )
{
    time_t now = time((time_t *) NULL);
    struct tm *t = localtime( &now );
    //QString temp = asctime( t );
    //runTime = temp.left( 24 );
    char temp[256];
    memset(temp, '\0', sizeof(temp));
    strftime( temp, sizeof(temp), "%a, %b %d, %Y at %H:%M:%S", t );
    m_runTime = temp;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the requested pageNumber in the Document's DocScrollView.
 *
 *  Usually called by the ApplicationWindow navigation slots.
 *
 *  This is a virtual function that may be re-implemented in derived classes.
 *
 *  \param pageNumber Number of the page to be displayed.
 */

void Document::showPage( int pageNumber )
{
    // Hide while drawing
    //hide();

    // Test and store the new page number.
    m_page = pageNumber;
    if ( m_page < 1 )
    {
        m_page = 1;
    }
    else if ( m_page > m_pages )
    {
        m_page = m_pages;
    }
    // Find the name of the file for this page.
    QString composerFile = appFileSystem()->composerFilePath( m_docId, m_page );

    // Display the page file in the DocScrollView
    if ( ! m_scrollView->showPage( m_composer, composerFile ) )
    {
        QString msg("");
        translate( msg, "Document:ShowPage:NoComposerFile", composerFile );
        bomb( msg );
    }
    // Now display the page
    //show();

    // MUST setFocus() so the focus is not passed to the next Document!!
    setFocus();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Increments the Document's page counter, composes a new page with
 *  decorations, and adds the page to the table of contents.
 *
 *  Called by Document composition functions whenever they need to draw to a
 *  new, clean page.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \param tocName Table-of-contents text for the page.
 *  \param tocType #TocType enumeration value indicating the page type
 *  (and the icon displayed with the page).
 */

void Document::startNewPage( const QString &tocName, TocType tocType )
{
    // Increment the page counter
    m_pages++;

    // Get the new page's composer file name
    QString composerFile = appFileSystem()->composerFilePath( m_docId, m_pages );

    // Open the new composer file
    if ( ! m_composer->begin( composerFile ) )
    {
        QString msg("");
        translate( msg, "Document:ShowPage:NoComposerFile", composerFile );
        bomb( msg );
    }
    // Move into another method
    composeNewPage();

    // Add this page to the table of contents
    QString tocString;
    if ( tocName.isNull() || tocName.isEmpty() )
    {
        tocString = QString( "Page %1" ).arg( m_pages );
    }
    else
    {
        tocString = tocName;
    }
    contentsAddItem( m_pages, tocString, tocType );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sends the passed message to the ApplicationWindow's status bar.
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 *
 *  \param msg Message to be displayed in the status bar.
 */

void Document::status( const QString &msg )
{
    emit message( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the Document's absolute path name
 *  on the ApplicationWindow's status bar.
 *
 *  Called by DocScrollView::focusInEvent() and DocScrollView::showPage().
 *
 *  This is a non-virtual function that should NOT be re-implemented in any
 *  derived class.
 */

void Document::statusUpdate( void )
{
    QString msg("");
    QString workspacePath = appFileSystem()->workspacePath();
    translate( msg, "Document:PageOf", workspacePath,
    //translate( msg, "Document:PageOf", m_absPathName,
        QString( "%1" ).arg( m_page ), QString( "%1" ).arg( m_pages ) );
    emit message( msg );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the ApplicationWindow's \b View submenu options just
 *  before it displays itself.
 *
 *  This is the default virtual method for the Document abstract class,
 *  which offers 12 scales based on font point sizes.
 *  This probably works well enough for most text-based documents.
 *
 *  \param viewMenu Pointer to the \b View menu.
 */

void Document::viewMenuAboutToShow( QPopupMenu *viewMenu )
{
    // Rebuild the View menu with the current font scale checked
    static int point[] = { 6, 8, 9, 10, 12, 14, 16, 18, 20, 24, 36, 48 };
    QString str;
    int id;
    for ( int i = 0;
          i < 10;
          ++i )
    {
        str.sprintf( "%3d%%", (int) (0.5 + 100. * (double) point[i] / 12. ) );
        id = viewMenu->insertItem( str,
            this, SLOT( rescale( int ) ) );
        viewMenu->setItemParameter( id, point[i] );
        viewMenu->setItemChecked( id, ( point[i] == m_fontScaleSize ) );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of document.cpp
//------------------------------------------------------------------------------

