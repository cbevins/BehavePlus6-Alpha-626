//------------------------------------------------------------------------------
/*! \file fileselector.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus custom file selection dialog methods.
 *
 *  FileSelectorDialog handles the BehavePlus \b File->Open, \b File->New,
 *  and \b File->Delete menu options.
 *
 *  FileSaveAsDialog handles the \b File->saveAs menu option.
 *
 *  Works specifically with the BehavePlus prescribed file structure.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "document.h"
#include "fileselector.h"
#include "requestdialog.h"
#include "textview.h"           // Supports dialog printing

// Qt include files
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>

// Closed folder xpm
static const char *folder_closed_xpm[] =
{
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

// Open folder xpm
static const char *folder_open_xpm[] =
{
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"
};

// Locked folder xpm
static const char *folder_locked_xpm[] =
{
    "16 16 10 1",
    "h c #808080",
    "b c #ffa858",
    "f c #c0c0c0",
    "e c #c05800",
    "# c #000000",
    "c c #ffdca8",
    ". c None",
    "a c #585858",
    "g c #a0a0a4",
    "d c #ffffff",
    "..#a#...........",
    ".#abc####.......",
    ".#daa#eee#......",
    ".#ddf#e##b#.....",
    ".#dfd#e#bcb##...",
    ".#fdccc#daaab#..",
    ".#dfbbbccgfg#ba.",
    ".#ffb#ebbfgg#ba.",
    ".#ffbbe#bggg#ba.",
    ".#fffbbebggg#ba.",
    ".##hf#ebbggg#ba.",
    "...###e#gggg#ba.",
    ".....#e#gggg#ba.",
    "......###ggg#b##",
    ".........##g#b##",
    "...........####."
};

static const char *file_xpm[] =
{
    "16 16 7 1",
    "# c #000000",
    "b c #ffffff",
    "e c #000000",
    "d c #404000",
    "c c #c0c000",
    "a c #ffffc0",
    ". c None",
    "................",
    ".........#......",
    "......#.#a##....",
    ".....#b#bbba##..",
    "....#b#bbbabbb#.",
    "...#b#bba##bb#..",
    "..#b#abb#bb##...",
    ".#a#aab#bbbab##.",
    "#a#aaa#bcbbbbbb#",
    "#ccdc#bcbbcbbb#.",
    ".##c#bcbbcabb#..",
    "...#acbacbbbe...",
    "..#aaaacaba#....",
    "...##aaaaa#.....",
    ".....##aa#......",
    ".......##......."
};

static void fileDialogFill( const QString &topDirName, const QString &fileExt,
    QListView *listView ) ;

//------------------------------------------------------------------------------
/*! \var  static const char *FormatName[]
    \brief Image file format name recognized by Qt.
 */
    static const char *FormatName[] = { "BMP", "JPEG", "PNG", NULL };

//------------------------------------------------------------------------------
/*! \var static const char *FormatExt[]
    \brief Image file name extension corresponding to the FormatName[].
 */
    static const char *FormatExt[] = { "bmp", "jpg", "png", NULL };

//------------------------------------------------------------------------------
/*! \brief FileItem constructor for toplevel (NOT child) items.
 *
 *  Constructs a single FileItem for the FileSelectorDialog and FileSaveAsDialog
 *  #m_listview.  A FileItem may be either a directory or a file.
 *
 *  \param parent Pointer to the parent #FileItem.
 *  \param fileName reference to the name of the file or directory.
 *  \param fileExt Reference to the extension of the file or directory.
 */

FileItem::FileItem( QListView *parent, const QString &fileName,
        const QString &fileExt ) :
    QListViewItem( parent ),
    m_parent(0),
    m_fileName(fileName),
    m_fileExt(fileExt),
    m_readable(false)
{
    // Determine if the fileName is readable
    m_readable = QDir( m_fileName ).isReadable();
    return;
}

//------------------------------------------------------------------------------
/*! \brief FileItem constructor for child (NOT toplevel ListView) items.
 *
 *  Constructs a single FileItem for the FileSelectorDialog and FileSaveAsDialog
 *  #m_listview.  A FileItem may be either a directory or a file.
 *
 *  \param parent Pointer to the parent #FileItem.
 *  \param fileName Reference to the name of the file or directory.
 *  \param fileExt Reference to the extension of the file or directory.
 */

FileItem::FileItem( FileItem *parent, const QString &fileName,
        const QString &fileExt ) :
    QListViewItem( parent ),
    m_parent(parent),
    m_fileName(fileName),
    m_fileExt(fileExt),
    m_readable(false)
{
    // Determine if the fileName is readable and use the appropriate pixmap
    QPixmap pixmap;
    if ( ! ( m_readable = QDir( m_fileName ).isReadable() ) )
    {
        if ( ! QPixmapCache::find( "FolderLocked", pixmap ) )
        {
            //qDebug( "FolderLocked pixmap loaded at FileItem::FileItem()." );
            pixmap = QPixmap( folder_locked_xpm );
            QPixmapCache::insert( "FolderLocked", pixmap );
        }
    }
    else
    {
        if ( ! QPixmapCache::find( "FolderClosed", pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at FileItem::FileItem()." );
            pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", pixmap );
        }
    }
    setPixmap( 0, pixmap );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens or closes the directory FileItem based upon \a open argument.
 *
 *  This is called only for directory files.  The FileItem's pixmap is set to
 *  either an open or a closed folder depending on \a open.
 *
 *  \param open TRUE opens the item, FALSE closes it.
 */

void FileItem::setOpen( bool open )
{
    // Assign the appropriate pixmap
    QPixmap pixmap;
    if ( open )
    {
        if ( ! QPixmapCache::find( "FolderOpen", pixmap ) )
        {
            //qDebug( "FolderOpen pixmap loaded at FileItem::setOpen()." );
            pixmap = QPixmap( folder_open_xpm );
            QPixmapCache::insert( "FolderOpen", pixmap );
        }
    }
    else
    {
        if ( ! QPixmapCache::find( "FolderClosed", pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at FileItem::setOpen()." );
            pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", pixmap );
        }
    }
    setPixmap( 0, pixmap );

    // If the item is open but has no children...
    QString nameFilter = "*." + m_fileExt;
    if ( open && ! childCount() )
    {
        QDir thisDir( m_fileName );
        // Presume this is locked and make it non-expandable
        setExpandable( false );
        // If the directory is not readable, we're done.
        if ( ! thisDir.isReadable() )
        {
            m_readable = false;
            return;
        }
        // Get a list of all the files in this directory
        thisDir.setFilter( QDir::Files );
        thisDir.setNameFilter( nameFilter );
        const QFileInfoList *files = thisDir.entryInfoList();

        // If files==0, then directory not readable or doesn't exist
        if ( files && files->count() > 0 )
        {
            // Ok, we have files to show, so make this item expandable
            setExpandable( true );
            QFileInfoListIterator it( *files );
            QFileInfo *fileInfo;
            QString desc;
            int result;
            while( ( fileInfo = it.current() ) != 0 )
            {
                ++it;
                // Try to read the file's embedded description
                desc = "";
                result = fileDescription(
                    fileInfo->absFilePath().latin1(), desc );
                // If the file couldn't be opened, just skip it
                if ( result == -1 )
                {
                    continue;
                }
                else if ( result == -2 )
                {
                    translate( desc, "FileSelector:NotABehavePlusFile" );
                }
                else if ( result == -3 || desc.isEmpty() || desc.isNull() )
                {
                    translate( desc, "FileSelector:NoDescription" );
                }
                // Make a new list item for this file
                FileItem *fileItem = new FileItem( this, fileInfo->absFilePath(),
                    m_fileExt );
                Q_CHECK_PTR( fileItem );
                // Make this item selectable by the user
                fileItem->setSelectable( true );
                // Display just its file name along with the file pixmap
                fileItem->setText( 0, fileInfo->fileName() );
                if ( ! QPixmapCache::find( "File", pixmap ) )
                {
                    //qDebug( "File pixmap loaded at FileItem::setOpen()." );
                    pixmap = QPixmap( file_xpm );
                    QPixmapCache::insert( "File", pixmap );
                }
                fileItem->setPixmap( 0, QPixmap( file_xpm ) );
                fileItem->setText( 2, desc );
                // Display the modification date
                fileItem->setText(3, fileInfo->lastModified().toString() );
            }
        }
    }
    // Now propagate onto the QListViewItem::setOpen()
    QListViewItem::setOpen( open );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Calls the QListViewItem::setup() for this FileItem.
 */

void FileItem::setup( void )
{
    QListViewItem::setup();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs the BehavePlus file selection dialog for the
 *  \b File->New, \b File->Open, and \b File->Delete menu options.
 *
 *  \param parent       Pointer to this dialog's parent.
 *  \param topDirName   Toplevel directory for the file type; RunFolder,
 *                      FuelModelFolder, UnitsSetFolder, WorksheetFolder, etc.
 *  \param fileType     File type such as "Run", "Worksheet", "Fuel Model",
 *                      "Units Set", "Moisture Set"
 *  \param fileExt      File extension such as "bpr", "bpf", "bpw", "bpu", "bpm"
 *  \param name         #QWidget name.
 */

FileSelectorDialog::FileSelectorDialog( QWidget *parent,
        const QString &topDirName, const QString &fileType,
        const QString &fileExt, const char *name ) :
    AppDialog(
        parent,                                     // Parent
        QString( "FileSelector:FileSelectorDialog:Caption:" + fileType ),  // Caption key
        "RockCreek.png",                            // Picture file
        "Rock Creek",                               // Picture name
        "",                                         // No HelpBrowser
        name ),                                     // Widget name
    m_topDirName(topDirName),
    m_fileType(fileType),
    m_fileExt(fileExt),
    m_selected(0),
    m_listView(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Add the listview to the mainFrame mainLayout
    QString text("");
    m_listView = new QListView( m_page->m_contentFrame, "m_listView" );
    Q_CHECK_PTR( m_listView );
    m_listView->addColumn( m_fileType );
    translate( text, "FileSelector:Header:Files" );
    m_listView->addColumn( text );
    translate( text, "FileSelector:Header:Description" );
    m_listView->addColumn( text );
    translate( text, "FileSelector:Header:LastModified" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 1, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 2, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 3, QListView::Maximum ) ;
    m_listView->setRootIsDecorated( true );
    m_listView->setSelectionMode( QListView::Single );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );    // Sort on column 0 ascending
    m_listView->setItemMargin( 3 );
    m_listView->setMinimumWidth( m_listView->columnWidth( 0 )
     + m_listView->columnWidth(1) );

    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( slotDoubleClicked( QListViewItem * ) ) );

    // Allow right button to popup a context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );

    // Fill the listview with the folders and return.
    fileDialogFill( topDirName, fileExt, m_listView );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

FileSelectorDialog::~FileSelectorDialog( void )
{
    delete m_listView;      m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void FileSelectorDialog::contextMenuActivated( int id )
{
    if ( id == ContextOpenFolder )
    {
        m_lvi->setOpen( true );
    }
    else if ( id == ContextCloseFolder )
    {
        m_lvi->setOpen( false );
    }
    else if ( id == ContextEditFolderDescription )
    {
        editFolderDescription( m_lvi );
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( m_page->m_contentFrame );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    else if ( id == ContextOpenFile )
    {
        m_listView->setSelected( m_lvi, true );
        m_selected = (FileItem*) m_lvi;
        // Set m_lvi to 0 to let rightButtonClicked() know we're done with
        // the dialog
        m_lvi = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays a RequestDialog to edit the folder description,
 *  and update the description in the file and in the listview.
 *
 *  \return Returns TRUE on success or FALSE on failure
 */

bool FileSelectorDialog::editFolderDescription( QListViewItem *lvi )
{
    // Request a new subdirectory description.
    QString prompt("");
    translate( prompt, "FileSelector:Prompt:Description", lvi->text( 0 ) );
    QString desc = lvi->text( 2 );
    RequestDialog request( prompt, desc, "folderDescription.html",
        this, "requestDialog" );

    // Update the description
    if ( request.exec() == QDialog::Accepted )
    {
        // Construct the file's full path name
        FileItem *item = (FileItem *) lvi;
        QString fileName = item->m_fileName + QDir::separator() + lvi->text(0);

        // Write the new description
        FILE *fptr = 0;
        if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
        {
            translate( prompt, "FileSelector:NoOpen", fileName );
            info( prompt );
            return( false );
        }
        request.text( desc );
        fprintf( fptr, "%s", desc.latin1() );
        fclose( fptr );
        lvi->setText( 2, desc );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Gets the current file selection and returns it in \a fileName.
 *
 *  \param fileName Reference to a QString to contain the current selection.
 *
 *  \return The function returns nothing.
 */

void FileSelectorDialog::getFileSelection( QString &fileName )
{
    fileName = m_selected->m_fileName;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the popup context menu.
 */

void FileSelectorDialog::rightButtonClicked( QListViewItem *lvi,
        const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    m_lvi = lvi;

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        // Folder clicked
        if ( ! lvi->parent() )
        {
            translate( text, "FileSelector:ContextMenu:OpenFolder" );
            mid = m_contextMenu->insertItem( text,
                   this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextOpenFolder );

            translate( text, "FileSelector:ContextMenu:CloseFolder" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextCloseFolder );

            translate( text, "FileSelector:ContextMenu:EditFolderDescription" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextEditFolderDescription );
        }
        // File clicked
        else
        {
            translate( text, "FileSelector:ContextMenu:OpenFile" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextOpenFile );
        }
    }

    translate( text, "FileSelector:ContextMenu:PrintVisibleText" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "FileSelector:ContextMenu:PrintEntireText" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;

    // If m_lvi has been reset to 0 by contextMenuActivated(),
    // then the user made a selection and we're done with the dialog
    if ( lvi && ! m_lvi )
    {
        m_selected = (FileItem*) lvi;
        accept();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Called whenever user double clicks on a \b file item (not on a
 *  \b directory item).
 *
 *  Simply stores a pointer to the selected item, closes the dialog,
 *  and returns \c ACCEPTED.
 *
 *  \param lvi Pointer to the selected list view item.
 */

void FileSelectorDialog::slotDoubleClicked( QListViewItem *lvi )
{
    // Must click a file item, not a folder item
    if ( ! lvi )
    {
        return;
    }
    // Only allow doubleclick selection of directory children.
    if ( lvi->parent() )
    {
        m_selected = (FileItem*) lvi;
        // Close the dialog and return QDialog::Accepted.
        accept();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Called whenever the user presses the \b Ok button.
 */

void FileSelectorDialog::store( void )
{
    // Must be a selected item before its "Ok".
    if ( ! m_selected )
    {
        QListViewItem *lvi;
        // Must be a selected item with a parent.
        if ( ! ( lvi = m_listView->selectedItem() ) || ! lvi->parent() )
        {
            QString caption("");
            translate( caption, "FileSelector:SelectAFile:Caption" );
            QString text("");
            translate( text, "FileSelector:SelectAFile:Text", m_fileType );
            warn( caption, text );
            return;
        }
        m_selected = (FileItem*) lvi;
    }
    // Consume the event
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief A file selection dialog to handle the \b File->saveAs and
 *  \a File->Capture menu options.
 *
 *  \param topDirName   Absolute pathname of the toplevel directory,
 *                      below which are one or more "folders".
 *  \param fileType     Files are called this.
 *                      Note that special handling is given to fileType
 *                      "Capture".  Other file types are "Run", "Worksheet",
 *                      "Fuel Model", "Moisture Scenario", and "Units Set".
 *  \param fileExt      File extension filter.
 *  \param defaultFolder Default (starting) folder (subdir of topDirName).
 *  \param defaultFile  Default file name.
 *  \param defaultDesc  Default file description text.
 *
 *  \return The function returns nothing.
 */

FileSaveAsDialog::FileSaveAsDialog( QWidget *parent,
        const QString &topDirName,  const QString &fileType,
        const QString &fileExt,     const QString &defaultFolder,
        const QString &defaultFile, const QString &defaultDesc,
        const char *name ) :
    AppDialog(
        parent,                                 // Parent
        "FileSelector:FileSaveAsDialog:Caption",// Caption key
        "FishingTheYellowstone.png",            // Picture file
        "Fishing The Yellowstone",              // Picture name
        "",                                     // No HelpBrowser
        name ),                                 // Widget name
    m_topDirName(topDirName),
    m_fileType(fileType),
    m_fileExt(fileExt),
    m_format(""),
    m_fileSelection(""),
    m_listView(0),
    m_gridFrame(0),
    m_textGrid(0),
    m_entry(3),
    m_formatComboBox(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Compose labels for the input fields.
    QString text("");
    translate( text, "FileSelector:FieldLabel:Folder" );
    m_label[0] = m_fileType + " " + text;
    translate( text, "FileSelector:FieldLabel:File" );
    m_label[1] = m_fileType + " " + text;
    if ( m_fileType == "Capture" )
    {
        translate( text, "FileSelector:FieldLabel:Type" );
    }
    else
    {
        translate( text, "FileSelector:FieldLabel:Description" );
    }
    m_label[2] = m_fileType + " " + text;
    m_lbl[0] = m_lbl[1] = m_lbl[2] = m_lbl[3] = 0;

    // Add the main scrolling file list to the top right side
    m_listView = new QListView( m_page->m_contentFrame, "m_listView" );
    Q_CHECK_PTR( m_listView );
    translate( text, "FileSelector:Header:Folder/File" );
    m_listView->addColumn( QString( m_fileType + text ) );
    translate( text, "FileSelector:Header:Files" );
    m_listView->addColumn( text );
    translate( text, "FileSelector:Header:Description" );
    m_listView->addColumn( text );
    translate( text, "FileSelector:Header:LastModified" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 1, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 2, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 3, QListView::Maximum ) ;
    m_listView->setRootIsDecorated( true );
    m_listView->setSelectionMode( QListView::Single );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );    // Sort on column 0 ascending
    m_listView->setItemMargin( 3 );
    m_listView->setMinimumWidth( m_listView->columnWidth( 0 )
        + m_listView->columnWidth( 1 ) );

    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( slotFolderSelected( QListViewItem * ) ) );

    // Allow right button to popup a context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );

    // Fill the listview with the folders
    fileDialogFill( topDirName, fileExt, m_listView );

    // Hidden frame to contain a grid layout
    m_gridFrame = new QFrame( m_page->m_contentFrame, "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );

    // The middle panel contains another grid to manage file info entry
    m_textGrid = new QGridLayout( m_gridFrame, 4, 2, 5 );
    Q_CHECK_PTR( m_textGrid );
    for ( int row = 0;
          row < 3;
          row++ )
    {
        m_lbl[row] = new QLabel( m_label[row], m_gridFrame );
        Q_CHECK_PTR( m_lbl[row] );
        m_textGrid->addWidget( m_lbl[row], row, 0, AlignLeft );

        if ( m_fileType == "Capture" && row == 2 )
        {
            m_formatComboBox = new QComboBox( false, m_gridFrame,
                "m_formatComboBox" );
            Q_CHECK_PTR( m_formatComboBox );
            m_formatComboBox->insertStrList( FormatExt );
            m_formatComboBox->setCurrentItem( 0 );
            m_textGrid->addWidget( m_formatComboBox, row, 1 );
        }
        else
        {
            m_entry[row] = new QLineEdit( "", m_gridFrame );
            Q_CHECK_PTR( m_entry[row] );
            m_textGrid->addWidget( m_entry[row], row, 1 );
        }
    }

    translate( text, "FileSelector:NewFolder", m_label[0] );
    m_lbl[3] = new QLabel( text, m_gridFrame, "newFolderText" );
    Q_CHECK_PTR( m_lbl[3] );
    m_textGrid->addMultiCellWidget( m_lbl[3], 3, 3, 0, 1 );

    m_entry[0]->setText( defaultFolder );
    m_entry[1]->setText( defaultFile );
    if ( m_fileType != "Capture" )
    {
        m_entry[2]->setText( defaultDesc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

FileSaveAsDialog::~FileSaveAsDialog( void )
{
    delete m_formatComboBox;    m_formatComboBox = 0;
    delete m_entry[0];          m_entry[0] = 0;
    delete m_entry[1];          m_entry[1] = 0;
    delete m_lbl[0];            m_lbl[0] = 0;
    delete m_lbl[1];            m_lbl[1] = 0;
    delete m_lbl[2];            m_lbl[2] = 0;
    delete m_lbl[3];            m_lbl[3] = 0;
    delete m_listView;          m_listView = 0;
    delete m_textGrid;          m_textGrid = 0;
    delete m_gridFrame;         m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void FileSaveAsDialog::contextMenuActivated( int id )
{
    if ( id == ContextOpenFolder )
    {
        m_lvi->setOpen( true );
    }
    else if ( id == ContextCloseFolder )
    {
        m_lvi->setOpen( false );
    }
    else if ( id == ContextSelectFolder )
    {
        slotFolderSelected( m_lvi );
    }
    else if ( id == ContextEditFolderDescription )
    {
        editFolderDescription( m_lvi );
    }
    else if ( id == ContextSelectFile )
    {
        slotFolderSelected( m_lvi );
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( m_page->m_contentFrame );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays a RequestDialog to edit the folder description,
 *  and update the description in the file and in the listview.
 *
 *  \return Returns TRUE on success or FALSE on failure
 */

bool FileSaveAsDialog::editFolderDescription( QListViewItem *lvi )
{
    // Request a new subdirectory description.
    QString prompt("");
    translate( prompt, "FileSelector:Prompt:Description", lvi->text( 0 ) );
    QString desc = lvi->text( 2 );
    RequestDialog request( prompt, desc, "folderDescription.html",
        this, "requestDialog" );

    // Update the description
    if ( request.exec() == QDialog::Accepted )
    {
        // Construct the file's full path name
        FileItem *item = (FileItem *) lvi;
        QString fileName = item->m_fileName + QDir::separator() + lvi->text(0);

        // Write the new description
        FILE *fptr = 0;
        if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
        {
            translate( prompt, "FileSelector:NoOpen", fileName );
            info( prompt );
            return( false );
        }
        request.text( desc );
        fprintf( fptr, "%s", desc.latin1() );
        fclose( fptr );
        lvi->setText( 2, desc );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current entry field text for \a id in \a value.
 *
 *  \param id Index of the text to retrieve:
 *  \arg 0 is the directory name,
 *  \arg 1 is the file name, and
 *  \arg 2 is the file description.
 *  \param value Reference to the string to contain the returned text.
 */

void FileSaveAsDialog::getEntry( int id, QString &value )
{
    value = m_entry[id]->text();
    return;
}

//------------------------------------------------------------------------------
/*! \brief gets the current file description text from the entry field.
 *
 *  \param desc Reference to the string to contain the returned text.
 */

void FileSaveAsDialog::getFileDescription( QString &desc )
{
    desc = m_entry[2]->text();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current file format name.
 *
 *  \return Pointer to the file format name.
 */

const char *FileSaveAsDialog::getFileFormat( void )
{
    const char *fmt  = (const char *) m_format;
    for ( int id = 0;
          FormatExt[id];
          id++ )
    {
        if ( strcmp( fmt, FormatExt[id] ) == 0 )
        {
            return( FormatName[id] );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Gets the current file selection.
 *
 *  \param fileName Reference to the string to contain the returned file name.
 */

void FileSaveAsDialog::getFileSelection( QString &fileName )
{
    fileName = m_fileSelection;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Distplays the context menu.
 */

void FileSaveAsDialog::rightButtonClicked( QListViewItem *lvi,
    const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    m_lvi = lvi;

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        // Folder clicked
        if ( ! lvi->parent() )
        {
            translate( text, "FileSelector:ContextMenu:OpenFolder" );
            mid = m_contextMenu->insertItem( text,
                   this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextOpenFolder );

            translate( text, "FileSelector:ContextMenu:CloseFolder" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextCloseFolder );

            translate( text, "FileSelector:ContextMenu:SelectFolder" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextSelectFolder );

            translate( text, "FileSelector:ContextMenu:EditFolderDescription" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextEditFolderDescription );
        }
        // File clicked
        else
        {
            translate( text, "FileSelector:ContextMenu:SelectFile" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextSelectFile );
        }
    }

    translate( text, "FileSelector:ContextMenu:PrintVisibleText" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "FileSelector:ContextMenu:PrintEntireText" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Called whenever use double clicks on an item.  Fills out the
 *  subdirectory and/or file name fields with the selected values.
 *
 *  \param lvi Pointer to the selected item.
 */

void FileSaveAsDialog::slotFolderSelected( QListViewItem *lvi )
{
    // Must click an item
    if ( ! lvi )
    {
        return;
    }
    // If this is a toplevel item, use it as the directory name
    if ( ! lvi->parent() )
    {
        m_entry[0]->setText( lvi->text(0) );
    }
    // Otherwise this is a subitem, so set both the directory & filename
    else
    {
        m_entry[0]->setText( lvi->parent()->text(0) );
        m_entry[1]->setText( lvi->text(0) );
        if ( m_fileType != "Capture" )
        {
            m_entry[2]->setText( lvi->text(2) );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Performs entry field validation.  Called whenever the user presses
 *  the "Ok" button.
 */

void FileSaveAsDialog::store( void )
{
    // All fields must have text.
    QString caption("");
    QString text("");
    int lastRow = ( m_fileType == "Capture" )
                ? ( 1 )
                : ( 2 );
    for ( int row = 0;
          row <= lastRow;
          row++ )
    {
        text = m_entry[row]->text().stripWhiteSpace();
        if ( text.isEmpty() )
        {
            translate( caption, "FileSelector:SelectAFile:Caption" );
            translate( text, "FileSelector:SelectASaveAsFile:Text",
                m_label[row] );
            warn( caption, text );
            return;
        }
    }
    // The file name must be just one word.
    QString fileName = m_entry[1]->text().stripWhiteSpace();
    if ( fileName.contains( " " ) )
    {
        translate( caption, "FileSelector:SelectASaveAsFile:Error:Caption" );
        translate( text, "FileSelector:SelectASaveAsFile:Error:Text" );
        warn( caption, text );
        return;
    }
    // The file name must not contain a subdirectory
    if ( fileName.contains( "/" )
      || fileName.contains( "\\" ) )
    {
        translate( caption, "BpDocument:FileIsDir:Caption" );
        translate( text, "BpDocument:FileIsDir:Message", fileName );
        warn( caption, text );
        return;
    }
    // If the directory name doesn't exist, ask if we should create it.
    // Note: m_topDirName already has a slash as its last character!
    QString dirName = m_topDirName + m_entry[0]->text().stripWhiteSpace();
    QDir dir( dirName );
    bool newDir = false;
    QString desc("");
    if ( ! dir.exists() )
    {
        // Ask if we are to create the new subdirectory.
        translate( caption, "FileSelector:SaveAs:NewDirectory:Caption" );
        translate( text, "FileSelector:SaveAs:NewDirectory:Text",
            m_label[0], dirName );
        if ( yesno( caption, text ) == 0 )
        {
            return;
        }
        // Request a new subdirectory description.
        QString prompt("");
        translate( prompt, "FileSelector:Prompt:Description", m_label[0] );
        RequestDialog request( prompt, desc, "folderDescription.html",
            this, "requestDialog" );
        if ( request.exec() !=  QDialog::Accepted )
        {
            return;
        }
        request.text( desc );
        if ( desc.isNull() || desc.isEmpty() )
        {
            return;
        }
        // Make the new directory.
        if ( ! dir.mkdir( dirName ) )
        {
            translate( caption, "FileSelector:NewDirectory:Error:Caption" );
            translate( text, "FileSelector:NewDirectory:Error:Text",
                m_label[0], dirName );
            warn( caption, text );
            return;
        }
        // Set flag that this is a new directory.
        newDir = true;
    }
    // Append the file extension if one wasn't provided.
    fileName = dirName + QDir::separator()
        + m_entry[1]->text().stripWhiteSpace();
    QFileInfo fi( fileName );
    if ( m_fileType != "Capture" )
    {
        if ( fi.extension( false ) != m_fileExt )
        {
            fileName.append( "." + m_fileExt );
        }
    }
    // If a Capture file already has an extension, it must match the file type
    else
    {
        m_format = m_formatComboBox->currentText();
        QString fileExt = fi.extension( false );
        if ( ! fileExt.isNull() && ! fileExt.isEmpty() )
        {
            const char *cext = (const char *) fileExt;
            const char *fmt  = (const char *) m_format;
            for ( int id = 0;
                  FormatExt[id];
                  id++ )
            {
                if ( strcmp( cext, FormatExt[id] ) == 0 )
                {
                    if ( strcmp( cext, fmt ) != 0 )
                    {
                        translate( caption, "FileSelector:CaptureFormat:Error:Caption" );
                        translate( text, "FileSelector:CaptureFormat:Error:Text",
                            fileExt, m_format );
                        warn( caption, text );
                        return;
                    }
                    break;
                }
            }
        }
        // Otherwise add the appropriate extension
        else
        {
            if ( fileName.right(1) != "." )
            {
                fileName.append( "." );
            }
            fileName.append( m_format );
            fi.setFile( fileName );
        }
    }
    // If the file exists, get permission to overwrite it.
    if ( fi.exists() )
    {
        translate( caption, "FileSelector:OverwriteFile:Caption" );
        translate( text, "FileSelector:OverwriteFile:Text",
                m_label[0], fileName );
        if ( yesno( caption, text ) == 0 )
        {
            if ( newDir )
            {
                if ( ! dir.rmdir( dirName ) )
                {
                    translate( caption, "FileSelector:RemoveDirectory:Error:Caption" );
                    translate( text, "FileSelector:RemoveDirectory:Error:Text",
                        m_label[0], dirName );
                    warn( caption, text );
                }
            }
            return;
        }
    }
    // If a new subdirectory was created, write its description file.
    if ( newDir )
    {
        // The description file has the same name as the folder it describes
        QString descFileName = dirName + QDir::separator()
            + m_entry[0]->text().stripWhiteSpace();
        FILE *fptr;
        if ( ! ( fptr = fopen( descFileName.latin1(), "w" ) ) )
        {
            translate( caption, "FileSelector:FileCreateError:Caption" );
            translate( text, "FileSelector:FileCreateError:Text",
                m_label[0], descFileName );
            warn( caption, text );
        }
        else
        {
            fprintf( fptr, "%s\n", desc.latin1() );
            fclose( fptr );
        }
    }
    m_fileSelection = fileName;
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Internal convenience function that initializes the file dialog's
 *  listview with all the file type folders.
 */

void fileDialogFill( const QString &topDirName, const QString &fileExt,
    QListView *listView )
{
    // Initially display all the subdirectories below the topDir
    QDir topDir( topDirName );
    topDir.setFilter( QDir::Dirs );
    const QFileInfoList *subDirList = topDir.entryInfoList();
    if ( ! subDirList )
    {
        return;
    }
    // Check each subdirectory
    QFileInfoListIterator subDirIt( *subDirList );
    QFileInfo *subDirInfo;
    FileItem *fileItem;
    QString folderName, folderDesc;
    QPixmap pixmap;
    while( (subDirInfo = subDirIt.current()) )
    {
        ++subDirIt;
        // Skip this directory and the parent directory
        if ( subDirInfo->fileName() == "." || subDirInfo->fileName() == ".." )
        {
            continue;
        }
        // Create the subdirectory item with a folder pixmap
        fileItem = new FileItem( listView, subDirInfo->absFilePath(), fileExt );
        Q_CHECK_PTR( fileItem );

        // Presume this subdirectory has no readable files of interest
        if ( ! QPixmapCache::find( "FolderClosed", pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at fileDialogFill()." );
            pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", pixmap );
        }
        fileItem->setPixmap( 0, pixmap );
        fileItem->setExpandable( false );
        fileItem->setText( 0, subDirInfo->fileName() );
        fileItem->setText( 1, "0" );
        fileItem->setSelectable( false );
        fileItem->setText(3, subDirInfo->lastModified().toString() );

        // Get the folder description (separator is required here!)
        folderName = subDirInfo->absFilePath() + QDir::separator()
                   + subDirInfo->fileName();
        folderDescription( folderName, folderDesc );
        fileItem->setText( 2, folderDesc );

        // Get a list of this subdirectory's readable files with the proper ext
        QDir subDir( subDirInfo->absFilePath() );
        subDir.setFilter( QDir::Files | QDir::Readable );
        subDir.setNameFilter( QString( "*.%1" ).arg( fileExt ) );
        const QFileInfoList *files = subDir.entryInfoList();

        // If the subdirectory has any readable files, make it expandable.
        if ( files && files->count() > 0 )
        {
            fileItem->setExpandable( true );
            fileItem->setText( 1, QString( "%1" ).arg( files->count() ) );
        }
    }
    return;
}

//------------------------------------------------------------------------------
//  End of fileselector.cpp
//------------------------------------------------------------------------------

