//------------------------------------------------------------------------------
/*! \file attachdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AttachItem and AttachDialog class methods.
 */

// Custom include files.
#include "appmessage.h"
#include "apptranslator.h"
#include "attachdialog.h"
#include "filesystem.h"
#include "fuelmodel.h"
#include "moisscenario.h"
#include "requestdialog.h"
#include "textview.h"           // For widget printing

// Qt include files.
#include <qcursor.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>
#include <qstringlist.h>

//! Closed folder icon xpm.
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

//! Open folder icon xpm.
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

//! Locked folder icon xpm.
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

//! File icon xpm.
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

//! Attached folder/file icon xpm.
static const char *attached_xpm[] = {
    /* width height num_colors chars_per_pixel */
    "    27    28        4            1",
    /* colors */
    ". c #000000",
    "# c #808080",
    "a c None",
    "b c #ffffff",
    /* pixels */
    "aaaaa.....aaaaaaaaaaaaaaaaa",
    "aaaa.#####.aaaaaaaaaaaaaaaa",
    "aaa.b#...b#.aaaaaaaaaaaaaaa",
    "aa.b#.aaa.b#.aaaaaaaaaaaaaa",
    "aa.b.aaa...b#.aaaaaaaaaaaaa",
    "aa.b.aa.bb#.b#.aaaaaaaaaaaa",
    "aa.b.a.bb.b#.b#.aaaaaaaaaaa",
    "aa.b.a.b.a.b#.b#.aaaaaaaaaa",
    "aa.b#..b.aa.b#.b#.aaaaaaaaa",
    "aaa.b#.b#.aa.b#.b#.aaaaaaaa",
    "aaaa.b#.b#.aa.b#.b#.aaaaaaa",
    "aaaaa.b#.b#.aa.b#.b#.aaaaaa",
    "aaaaaa.b#.b#.aa.b#.b#.aaaaa",
    "aaaaaaa.b#.b#.aa.b#.b#.aaaa",
    "aaaaaaaa.b#.b#.aa.b#.b#.aaa",
    "aaaaaaaaa.b#.b#.aa.b#..aaaa",
    "aaaaaaaaaa.b#.b#.aa.b#.aaaa",
    "aaaaaaaaaaa.b#.b#.aa.b#.aaa",
    "aaaaaaaaaaaa.b#.b#.aa.b#.aa",
    "aaaaaaaaaaaaa.b#..aaaa.b#.a",
    "aaaaaaaaaaaaaa.b#.aaaaa.b#.",
    "aaaaaaaaaaaaaaa.b#.aaaaa.#.",
    "aaaaaaaaaaaaaaaa.b#.aaaa.#.",
    "aaaaaaaaaaaaaaaaa.b#.aaa.#.",
    "aaaaaaaaaaaaaaaaaa.b#...##.",
    "aaaaaaaaaaaaaaaaaaa.bbbbb.a",
    "aaa#################.....aa",
    "a########################aa"
};

//------------------------------------------------------------------------------
/*! \brief AttachItem constructor for \b toplevel (NOT child) items.
 *
 *  Constructs a single AttachItem for the AttachDialog::m_listView.
 *  An AttachItem may be either a directory or a file, but in this case
 *  it \e must be a folder.
 *
 *  \param p_parent Pointer to the parent #m_listView.
 *  \param fileName Reference to the full path name of the file or directory.
 *  \param fileExt Reference to the extension of the file or directory.
 */

AttachItem::AttachItem( QListView *p_parent, const QString &fileName,
        const QString &fileExt, QStringList *attachedList ) :
    QListViewItem( p_parent ),
    m_parent(0),
    m_fileName(fileName),
    m_fileExt(fileExt),
    m_readable(false),
    m_attached(false),
    m_attachedList(attachedList)
{
    // Determine if the fileName is readable.
    m_readable = QDir( m_fileName ).isReadable();
    return;
}

//------------------------------------------------------------------------------
/*! \brief AttachItem constructor for \b child (NOT toplevel ListView) items.
 *
 *  Constructs a single AttachItem for the AttachDialog::m_listView.
 *  An AttachItem may be either a directory or a file, but in this case it
 *  \e must be a file.
 *
 *  \param p_parent Pointer to the parent AttachItem.
 *  \param fileName Reference to the full path name of the file or directory.
 *  \param fileExt Reference to the extension of the file or directory.
 */

AttachItem::AttachItem( AttachItem *p_parent, const QString &fileName,
        const QString &fileExt, QStringList *attachedList ) :
    QListViewItem( p_parent ),
    m_parent(p_parent),
    m_fileName(fileName),
    m_fileExt(fileExt),
    m_readable(false),
    m_attached(false),
    m_attachedList(attachedList)
{
    // Determine if the fileName is readable and use the appropriate pixmap.
    QPixmap l_pixmap;
    if ( ! ( m_readable = QDir( m_fileName ).isReadable() ) )
    {
        if ( ! QPixmapCache::find( "FolderLocked", l_pixmap ) )
        {
            //qDebug( "FolderLocked pixmap loaded at AttachItem::AttachItem()." );
            l_pixmap = QPixmap( folder_locked_xpm );
            QPixmapCache::insert( "FolderLocked", l_pixmap );
        }
    }
    else
    {
        if ( ! QPixmapCache::find( "FolderClosed", l_pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at AttachItem::AttachItem()." );
            l_pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", l_pixmap );
        }
    }
    setPixmap( 0, l_pixmap );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens or closes the directory AttachItem based upon \a open argument.
 *
 *  This is called only for directory files.  The AttachItem's pixmap is set to
 *  either an open or a closed folder depending on \a open.
 *
 *  \param open TRUE opens the folder AttachItem, FALSE closes it.
 */

void AttachItem::setOpen( bool p_open )
{
    // Assign the appropriate pixmap.
    QPixmap l_pixmap;
    if ( m_attached )
    {
        if ( ! QPixmapCache::find( "Attached", l_pixmap ) )
        {
            //qDebug( "Attached pixmap loaded at AttachItem::setOpen()." );
            l_pixmap = QPixmap( attached_xpm );
            QPixmapCache::insert( "Attached", l_pixmap );
        }
    }
    else if ( p_open )
    {
        if ( ! QPixmapCache::find( "FolderOpen", l_pixmap ) )
        {
            //qDebug( "FolderOpen pixmap loaded at AttachItem::setOpen()." );
            l_pixmap = QPixmap( folder_open_xpm );
            QPixmapCache::insert( "FolderOpen", l_pixmap );
        }
    }
    else
    {
        if ( ! QPixmapCache::find( "FolderClosed", l_pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at AttachItem::setOpen()." );
            l_pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", l_pixmap );
        }
    }
    setPixmap( 0, l_pixmap );

    // If the item is open but has no children...
    QString nameFilter = "*." + m_fileExt;
    if ( p_open && ! childCount() )
    {
        QDir thisDir( m_fileName );
        // Presume this is locked and make it non-expandable.
        setExpandable( false );
        // If the directory is not readable, we're done.
        if ( ! thisDir.isReadable() )
        {
            m_readable = false;
            return;
        }
        // Get a list of all the files in this directory.
        thisDir.setFilter( QDir::Files );
        thisDir.setNameFilter( nameFilter );
        const QFileInfoList *files = thisDir.entryInfoList();

        // If files==0, then the directory is not readable or doesn't exist.
        if ( files && files->count() > 0 )
        {
            // Ok, we have files to show, so make this folder item expandable.
            setExpandable( true );
            // Display each file item in this folder item.
            QFileInfoListIterator it( *files );
            QFileInfo *fileInfo;
            QString desc;
            int result;
            while( ( fileInfo = it.current() ) != 0 )
            {
                ++it;
                // Try to read the file's embedded description.
                desc = "";
                result = fileDescription( fileInfo->absFilePath().latin1(),
                    desc );
                // If the file couldn't be opened, just skip it.
                if ( result == -1 )
                // This code block should never be executed!
                {
                    continue;
                }
                else if ( result == -2 )
                // This code block should never be executed!
                {
                    translate( desc, "FileSelector:NotABehavePlusFile" );
                }
                else if ( result == -3 || desc.isEmpty() || desc.isNull() )
                {
                    translate( desc, "FileSelector:NoDescription" );
                }
                // Make a new AttachItem for this file.
                AttachItem *attachItem = new AttachItem( this,
                    fileInfo->absFilePath(), m_fileExt, m_attachedList );
                checkmem( __FILE__, __LINE__, attachItem,
                    "AttachItem attachItem", 1 );
                // Make this item unselectable by the user.
                attachItem->setSelectable( false );
                // Display just its file name along with the file pixmap.
                attachItem->setText( 0, fileInfo->fileName() );
                attachItem->setPixmap( 0, QPixmap( file_xpm ) );
                attachItem->setText( 2, desc );
                // Display the modification date.
                attachItem->setText(3, fileInfo->lastModified().toString() );
                // If already in the attach list, display a paper clip.
                for ( QStringList::Iterator lit = m_attachedList->begin();
                      lit != m_attachedList->end();
                      ++lit )
                {
                    if ( fileInfo->absFilePath() == (*lit) )
                    {
                        if ( ! QPixmapCache::find( "Attached", l_pixmap ) )
                        {
                            //qDebug( "Attached pixmap loaded at AttachItem::setOpen()." );
                            l_pixmap = QPixmap( attached_xpm );
                            QPixmapCache::insert( "Attached", l_pixmap );
                        }
                        attachItem->setPixmap( 0, l_pixmap );
                        break;
                    }
                }
            }   // while( ( fileInfo = it.current() ) != 0 )
        }   // if ( files && files->count() > 0 )
    }   // if ( p_open && ! childCount() )

    // Now propagate onto the QListViewItem::setOpen().
    QListViewItem::setOpen( p_open );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Propagates the QListViewItem::setup() for this AttachItem.
 */

void AttachItem::setup()
{
    QListViewItem::setup();
    return;
}

//------------------------------------------------------------------------------
/*! \brief AttachDialog constructor.
 *
 *  \param p_parent Pointer to the parent widget (usually a BehavePlusDocument).
 *  \param topDirName reference to the top level directory for the file type.
 *      This must be one of:
 *      - "<workspaceDir>/FuelModelFolder" or
 *      - "<workspaceDir>MoistureScenarioFolder".
 *  \param fileType reference to the file type name.  This should be one of:
 *      - "Fuel Model" or
 *      - "Moisture Scenario".
 *  \param fileExt File type extension.  This should be one of:
 *      - "bpf" or
 *      - "bpm".
 *  \param attachedList Reference to a list of attached file names.
 *  \param p_name Internal widget name.
 */

AttachDialog::AttachDialog( QWidget *p_parent, const QString &topDirName,
        const QString &fileType,   const QString &fileExt,
        QStringList *attachedList, const char *p_name ) :
    AppDialog(
        p_parent,                                     // Parent widget
        QString( "!" + fileType ),                  // Caption
        "LongEaredOwl.png",                         // Picture file
        "Long-eared Owl",                           // Picture name
        "attachFiles.html",                         // Html file
        p_name),                                      // Widget name
    m_topDirName(topDirName),
    m_fileType(fileType),
    m_fileExt(fileExt),
    m_attachedList(attachedList),
    m_listView(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Add the listview to the mainFrame mainLayout.
    m_listView = new QListView( contentFrame(), "m_listView" );
    Q_CHECK_PTR( m_listView );

    QString text("");
    m_listView->addColumn( m_fileType );
    translate( text, "AttachDialog:Col1" );     // Files
    m_listView->addColumn( text );
    translate( text, "AttachDialog:Col2" );     // Description
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 1, QListView::Maximum ) ;
    m_listView->setColumnWidthMode( 2, QListView::Maximum ) ;
    m_listView->setRootIsDecorated( true );
    m_listView->setSelectionMode( QListView::Multi );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );    // Sort on column 0 ascending
    m_listView->setItemMargin( 3 );

    // Connect a right button click to the popup context menu.
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );
    // Fill the listview with the folders and return.
    fileDialogFill( topDirName, fileExt, m_listView );
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief AttachDialog destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
*/

AttachDialog::~AttachDialog( void )
{
    delete m_listView;      m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when an action has been selected by the dialog's
 *  popup context menu.
 *
 *  An appropriate function is called based on the association between the id
 *  of the selected context menu item and the ContextMenuOption values.
 *  These values are assigned when the context menu is created in method
 *  rightButtonClicked().
 *
 *  \param id   The parameter id assigned when the context menu was created,
 *              it has one of the predefined ContextMenuOption values.
 */

void AttachDialog::contextMenuActivated( int id )
{
    if ( id == ContextOpenFolder )
    {
        m_lvi->setOpen( true );
    }
    else if ( id == ContextCloseFolder )
    {
        m_lvi->setOpen( false );
    }
    else if ( id == ContextAttachFolder )
    {
        m_listView->setSelected( m_lvi, true );
    }
    else if ( id == ContextDetachFolder )
    {
        m_listView->setSelected( m_lvi, false );
    }
    else if ( id == ContextEditFolderDescription )
    {
        editFolderDescription( m_lvi );
    }
    else if ( id == ContextViewFileParameters )
    {
        displayContents( m_lvi ) ;
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( contentFrame() );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the contents of the fuel model or moisture scenario
 *  file in an info dialog containing an HTML table of parameters.
 *
 *  \param lvi Pointer to the list view item (file) to be displayed.
 *
 *  \return Returns TRUE on success or FALSE on failure.
*/

bool AttachDialog::displayContents( QListViewItem *lvi )
{
    // Initialization.
    QString msg("");
    QString title("");
    QFileInfo fi( lvi->text(0) );

    // If this is a fuel model...
    if ( fi.extension( false ) == "bpf" )
    {
        // The AttachItem has the full filename.
        AttachItem *item = (AttachItem *) lvi;
        // Load the file into a FuelModel instance.
        FuelModel fm;
        if ( ! fm.loadBpf( item->m_fileName ) )
        // This code block should never be executed!
        {
            return( false );
        }
        // Create the display table.
        translate( title, "AttachDialog:FuelModel", lvi->text(0) );
        fm.formatHtmlTable( title, msg );
    }
    // If this is a moisture scenario...
    else if ( fi.extension( false ) == "bpm" )
    {
        // The AttachItem has the full filename.
        AttachItem *item = (AttachItem *) lvi;
        // Load the file into a MoisScenario instance.
        MoisScenario ms;
        if ( ! ms.loadBpm( item->m_fileName ) )
        // This code block should never be executed!
        {
            return( false );
        }
        // Create the display table.
        translate( title, "AttachDialog:MoisScenario", lvi->text(0) );
        ms.formatHtmlTable( title, msg );
    }
    // Unknown file type.
    else
    // This code block should never be executed!
    {
        translate( title, "AttachDialog:UnknownType", lvi->text( 0 ) );
        error( title );
        return( false );
    }
    // Display the contents and return.
    info( msg );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Displays a RequestDialog to edit the folder description,
 *  then updates the description in the file and in the listview.
 *
 *  \param lvi Pointer to the list view item (folder) to be edited.
 *
 *  \return Returns TRUE on success or FALSE on failure
*/

bool AttachDialog::editFolderDescription( QListViewItem *lvi )
{
    // Request a new subdirectory description.
    QString prompt("");
    translate( prompt, "AttachDialog:FolderDesc:Prompt", lvi->text( 0 ) );
    QString desc = lvi->text( 2 );
    RequestDialog request( prompt, desc,
        "folderDescription.html", this, "requestDialog" );

    // Update the description.
    if ( request.exec() == QDialog::Accepted )
    {
        // Construct the file's full path name.
        AttachItem *item = (AttachItem *) lvi;
        QString fileName = item->m_fileName + QDir::separator() + lvi->text(0);

        // Write the new description to the file.
        FILE *fptr = 0;
        if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "AttachDialog:NoOpen", fileName );
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
/*! \brief Internal convenience function that initializes the file dialog's
 *  listview with all the file type folders.
 *
 *  \param topDirName reference to the top level directory for the file type.
 *      This must be one of:
 *      - "<workspaceDir>/FuelModelFolder" or
 *      - "<workspaceDir>MoistureScenarioFolder".
 *  \param fileExt File type extension.  This should be one of:
 *      - "bpf" or
 *      - "bpm".
 *  \param listView Pointer to the QListView to be filled (usually m_listView).
 */

void AttachDialog::fileDialogFill( const QString &topDirName,
    const QString &fileExt, QListView *listView )
{
    // Initially display all the subdirectories below the topDir.
    QDir topDir( topDirName );
    topDir.setFilter( QDir::Dirs );
    // Get a list of all the subdirectories.
    const QFileInfoList *subDirList = topDir.entryInfoList();
    QFileInfoListIterator subDirIt( *subDirList );
    QFileInfo *subDirInfo;
    AttachItem *attachItem;
    QString folderName, folderDesc;
    QPixmap pixmap;
    while( (subDirInfo = subDirIt.current()) )
    {
        ++subDirIt;
        // Skip this directory and the parent directory.
        if ( subDirInfo->fileName() == "." || subDirInfo->fileName() == ".." )
        {
            continue;
        }
        // Create the subdirectory item with a folder pixmap.
        attachItem = new AttachItem( listView, subDirInfo->absFilePath(),
            fileExt, m_attachedList );
        checkmem( __FILE__, __LINE__, attachItem, "AttachItem attachItem", 1 );

        // Presume this subdirectory has no readable files of interest.
        attachItem->setExpandable( false );
        if ( ! QPixmapCache::find( "FolderClosed", pixmap ) )
        {
            //qDebug( "FolderClosed pixmap loaded at AttachDialog::fileDialogFill()." );
            pixmap = QPixmap( folder_closed_xpm );
            QPixmapCache::insert( "FolderClosed", pixmap );
        }
        attachItem->setPixmap( 0, pixmap );
        attachItem->setText( 0, subDirInfo->fileName() );
        attachItem->setText( 1, "0" );
        attachItem->setSelectable( true );
        //attachItem->setText(3, subDirInfo->lastModified().toString() );

        // Get the folder description.
        folderName = subDirInfo->absFilePath() + QDir::separator()
                   + subDirInfo->fileName();
        folderDescription( folderName, folderDesc );
        attachItem->setText( 2, folderDesc );

        // Get a list of this subdirectory's readable files with the proper ext.
        QDir subDir( subDirInfo->absFilePath() );
        subDir.setFilter( QDir::Files | QDir::Readable );
        subDir.setNameFilter( QString( "*.%1" ).arg( fileExt ) );
        const QFileInfoList *files = subDir.entryInfoList();

        // If the subdirectory has any readable files, make it expandable.
        if ( files && files->count() > 0 )
        {
            attachItem->setExpandable( true );
            attachItem->setText( 1, QString( "%1" ).arg( files->count() ) );

            // See if any of these files are attached.
            QFileInfoListIterator it( *files );
            QFileInfo *fileInfo;
            while( ( fileInfo = it.current() ) != 0 )
            {
                ++it;
                // If folder has any attached files...
                attachItem->m_attached = false;
                for ( QStringList::Iterator lit = m_attachedList->begin();
                      lit != m_attachedList->end();
                      ++lit )
                {
                    if ( fileInfo->absFilePath() == (*lit) )
                    {
                        if ( ! QPixmapCache::find( "Attached", pixmap ) )
                        {
                            //qDebug( "Attached pixmap loaded at AttachDialog::fileDialogFill()." );
                            pixmap = QPixmap( attached_xpm );
                            QPixmapCache::insert( "Attached", pixmap );
                        }
                        // Display a paper clip.
                        attachItem->setPixmap( 0, pixmap );
                        attachItem->m_attached = true;
                        // Make it selected.
                        attachItem->setSelected( true );
                        break;
                    }
                }
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Returns a QStringList of all the selected file names.
 *
 *  \param list Reference to the QStringList to which selections are appended.
 *
 *  \return Number of items appended to QStringList list.
 */

int AttachDialog::getSelectionList( QStringList &list )
{
    // Traverse the list.
    int count = 0;
    QListViewItem *folder = m_listView->firstChild();
    while ( folder )
    {
        if ( folder->isSelected() )
        {
            // Open the folder so we can traverse it.
            folder->setOpen( true );
            QListViewItem *l_child = folder->firstChild();
            while ( l_child )
            {
                AttachItem *item = (AttachItem *) l_child;
                list.append( item->m_fileName );
                count++;
                l_child = l_child->nextSibling();
            }
        }
        folder = folder->nextSibling();
    }
    return( count );
}

//------------------------------------------------------------------------------
/*! \brief Displays the popup context menu when a list view item is right
 *  clicked.
 *
 *  The context menu allows the user to open, close, attach, or detach a
 *  folder, edit a folder description, or view file parameters.
 *  ContextMenuOptions are assigned to each menu item.
 *  The menu actions are performed in contextMenuActivated().
 *
 *  \param lvi Pointer to the list view item that was right clicked.
 *  \param p coordinates of the point that was clicked.
 *  \param column Id of the column that was clicked (base 0).
 */

void AttachDialog::rightButtonClicked( QListViewItem *lvi, const QPoint &,
    int )
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
        // If a folder was clicked...
        if ( ! lvi->parent() )
        {
            translate( text, "AttachDialog:ContextMenu:Open" );
            mid = m_contextMenu->insertItem( text,
                   this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextOpenFolder );

            translate( text, "AttachDialog:ContextMenu:Close" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextCloseFolder );

            translate( text, "AttachDialog:ContextMenu:Attach" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextAttachFolder );

            translate( text, "AttachDialog:ContextMenu:Detach" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextDetachFolder );

            translate( text, "AttachDialog:ContextMenu:Edit" );
            mid = m_contextMenu->insertItem( text,
                    this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextEditFolderDescription );
        }
        // if a file was clicked...
        else
        {
            translate( text, "AttachDialog:ContextMenu:ViewParameters" );
            mid = m_contextMenu->insertItem( text,
                   this, SLOT( contextMenuActivated( int ) ) );
            m_contextMenu->setItemParameter( mid, ContextViewFileParameters );
        }
    }
    // Options for both folders and files.
    translate( text, "AttachDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "AttachDialog:ContextMenu:PrintEntire" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu then delete it.
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of attachdialog.cpp
//------------------------------------------------------------------------------

