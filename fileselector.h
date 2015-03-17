//------------------------------------------------------------------------------
/*! \file fileselector.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus custom file selection dialog API.
 *
 *  FileSelectorDialog handles the BehavePlus \b File->Open, \b File->New,
 *  and \b File->Delete menu options.
 *
 *  FileSaveAsDialog handles the \b File-saveAs menu option.
 *
 *  Works specifically with the BehavePlus prescribed file structure.
 */

#ifndef _FILESELECTOR_H_
/*! \def _FILESELECTOR_H_
    \brief Prevent redundant includes.
 */
#define _FILESELECTOR_H_ 1

// Class references
#include "appdialog.h"
#include <qcursor.h>
#include <qlistview.h>
#include <qmemarray.h>
class QComboBox;
class QGridLayout;
class QFrame;
class QLineEdit;
class QListView;
class QPopupMenu;
class QString;

//------------------------------------------------------------------------------
/*! \class FileItem fileselector.h
 *
 *  \brief Encapsulates information on a single item in the #FileSelectorDialog.
 *
 *  \sa #FileSelectorDialog, #FileSaveAsDialog.
 */

class FileItem : public QListViewItem
{
// Public methods
public:
    FileItem( QListView *parent, const QString &fileName, const QString &fileExt );
    FileItem( FileItem *parent, const QString &fileName, const QString &fileExt );
    void setOpen( bool );
    void setup( void );

// Public data
public:
    FileItem   *m_parent;       //!< Folders have 0 for m_parent
    QString     m_fileName;     //!< The file name
    QString     m_fileExt;      //!< The file extension
    bool        m_readable;     //!< TRUE if readable
};

//------------------------------------------------------------------------------
/*! \class FileSelectorDialog fileselector.h
 *
 *  \brief Handles the BehavePlus \b File->Open, \b File->New,
 *  and \b File->Delete menu options.
 *
 *  Works specifically with the BehavePlus prescribed file structure.
 *
 *  \sa #FileSaveAsDialog.
 */

class FileSelectorDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
    \brief Enumerates the types of Document context menu options available.
 */
enum ContextMenuOption
{
    ContextOpenFolder=0,
    ContextCloseFolder=1,
    ContextEditFolderDescription=2,
    ContextOpenFile=3,
    ContextPrintVisibleView=4,
    ContextPrintEntireView=5
};

// Public methods
public:
    FileSelectorDialog(
        QWidget *parent,                        //<! Parent widget
        const QString &topDirName,              //<! Directory absolute pathname
        const QString &fileType,                //<! Files are called this
        const QString &fileExt,                 //<! File extension filter.
        const char *name="fileSelectorDialog"   //<! Widget name
    );
    ~FileSelectorDialog( void ) ;
    void getFileSelection( QString &fileName ) ;

private:
    bool editFolderDescription( QListViewItem *lvi ) ;

// Private slots
private slots:
    void contextMenuActivated( int id ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void slotDoubleClicked( QListViewItem *lvi ) ;
    void store( void ) ;

// Private data members
private:
    QString         m_topDirName;   //!< Directory absolute path name.
    QString         m_fileType;     //!< Files are called this
    QString         m_fileExt;      //!< File extension.
    FileItem       *m_selected;     //!< Pointer to the selected item
    QListView      *m_listView;     //!< Pointer to the list view
    QPopupMenu     *m_contextMenu;  //!< Pointer to listview context menu
    QListViewItem  *m_lvi;          //!< Pointer to context menu lisview item
};

//------------------------------------------------------------------------------
/*! \class FileSaveAsDialog fileselector.h
 *
 *  \brief Handles the BehavePlus \b File-saveAs menu option.
 *
 *  Works specifically with the BehavePlus prescribed file structure.
 *
 *  \sa #FileSelectorDialog.
 */

class FileSaveAsDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
    \brief Enumerates the types of Document context menu options available.
 */
enum ContextMenuOption
{
    ContextOpenFolder=0,
    ContextCloseFolder=1,
    ContextSelectFolder=2,
    ContextEditFolderDescription=3,
    ContextSelectFile=4,
    ContextPrintVisibleView=5,
    ContextPrintEntireView=6
};

// Public methods
public:
    FileSaveAsDialog(
        QWidget *parent,                    //<! Parent widget
        const QString &topDirName,          //<! Directory absolute pathname
        const QString &fileType,            //<! Files are called this
        const QString &fileExt,             //<! File extension filter.
        const QString &defaultFolder,       //<! Default (starting) subdirectory
        const QString &defaultFile,         //<! Default file name.
        const QString &defaultDesc,         //<! Default file description text.
        const char *name="fileSaveAsDialog" //<! Widget name
    ) ;
    ~FileSaveAsDialog( void ) ;
    void getEntry( int id, QString &value ) ;
    void getFileDescription( QString &desc ) ;
    const char *getFileFormat( void ) ;
    void getFileSelection( QString &fileName ) ;

private:
    bool editFolderDescription( QListViewItem *lvi ) ;

// Private slots
private slots:
    void contextMenuActivated( int id ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void slotFolderSelected( QListViewItem *lvi ) ;
    void store( void ) ;

// Private data members
private:
    QString         m_topDirName;       //!< Directory absolute path name.
    QString         m_fileType;         //!< Files are called this
    QString         m_fileExt;          //!< File extension.
    QString         m_format;           //!< Capture file format (3 chars)
    QString         m_fileSelection;    //!< Pointer to the selected item
    QListView      *m_listView;         //!< Scrolling list view
    QFrame         *m_gridFrame;        //!< Pointer to the grid frame
    QGridLayout    *m_textGrid;         //!< Pointer to the text grid
    QLabel         *m_lbl[4];           //!< Field labels
    QString         m_label[3];         //!< Input entry labels
    QMemArray<QLineEdit *> m_entry;     //!< Input entry fields
    QComboBox      *m_formatComboBox;   //!< Capture file format selection box
    QPopupMenu     *m_contextMenu;      //!< Pointer to listview context menu
    QListViewItem  *m_lvi;              //!< Pointer to context menu lisview item
};

#endif

//------------------------------------------------------------------------------
//  End of fileselector.h
//------------------------------------------------------------------------------

