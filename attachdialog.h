//------------------------------------------------------------------------------
/*! \file attachdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AttachItem and AttachDialog class declarations.
 */

#ifndef _ATTACHDIALOG_H_
/*! \def _ATTACHDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _ATTACHDIALOG_H_ 1

// Forward class references.
#include "appdialog.h"
#include "textview.h"
#include <qlistview.h>
class QPopupMenu;
class QStringList;

//------------------------------------------------------------------------------
/*! \class AttachItem attachdialog.h
 *
 *  \brief Encapsulates information on a single item in the AttachDialog.
 *
 *   \sa #FileSelectorDialog, #FileSaveAsDialog.
 */

class AttachItem : public QListViewItem
{
// Friends classes
    friend class AttachDialog;

// Public methods
public:
    AttachItem( QListView *parent, const QString &fileName,
            const QString &fileExt, QStringList *attachedList );

    AttachItem( AttachItem *parent, const QString &fileName,
            const QString &fileExt, QStringList *attachedList );

// Private methods
private:
    void setOpen( bool );
    void setup( void );

// Private data members
private:
    AttachItem  *m_parent;      //!< Folders have 0 for m_parent
    QString      m_fileName;
    QString      m_fileExt;
    bool         m_readable;
    bool         m_attached;
    QStringList *m_attachedList;
};

//------------------------------------------------------------------------------
/*! \class AttachDialog attachdialog.h
 *
 *  \brief AttachDialog allows the user to attach/detach custom fuel model
 *  and/or moisture scenario files to the application.  The dialog is invoked
 *  from the main menu by \b Configure->Fuel \b model \b set \b selection or
 *  \b Configure->moisture \b Scenario \b set \b selection.
 *
 *  The AttachDialog has special knowledge of the BehavePlus folder and file
 *  structure.
 */

class AttachDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

//! Enumerates the available context menu options.
enum ContextMenuOption
{
    ContextOpenFolder=0,            //!< Open the current folder.
    ContextCloseFolder=1,           //!< Close the current folder.
    ContextAttachFolder=2,          //!< Attach all the current folder's files.
    ContextDetachFolder=3,          //!< Detach all the current folder's files.
    ContextEditFolderDescription=4, //!< Edit the current folder's description.
    ContextViewFileParameters=5,    //!< View the current file's parameters.
    ContextPrintVisibleView=6,      //!< Print the folders/files currently in view.
    ContextPrintEntireView=7        //!< Print all the folders/files.
};

// Public methods
public:
    AttachDialog(
        QWidget *parent,                //<! Parent widget
        const QString &topDirName,      //<! Directory absolute pathname
        const QString &fileType,        //<! Files are called this
        const QString &fileExt,         //<! File extension filter
        QStringList *attachedList,      //<! List of currently attached files
        const char *name="attachDialog" //<! Widget name
    );
    ~AttachDialog( void ) ;
    int  getSelectionList( QStringList &list ) ;

// Private methods
private:
    bool displayContents( QListViewItem *lvi ) ;
    bool editFolderDescription( QListViewItem *lvi ) ;
    void fileDialogFill( const QString &topDirName,
        const QString &fileExt, QListView *listView ) ;

private slots:
    void contextMenuActivated( int id ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;

// Private data members
private:
    //! Full path name of the fuel model or moisture scenario folder.
    QString         m_topDirName;
    //! File type name ("Fuel Model" or "Moisture Scenario").
    QString         m_fileType;
    //! File type extension ("bpf" or "bpm").
    QString         m_fileExt;
    //! Stored pointer to the list of attached files.
    QStringList    *m_attachedList;
    //! Pointer to the dynamically-allocated file/folder list view widget.
    QListView      *m_listView;
    //! Pointer to the dynamically-allocated context menu.
    QPopupMenu     *m_contextMenu;
    //! Stored pointer to the list view item invoked for the context menu.
    QListViewItem  *m_lvi;
};

#endif

//------------------------------------------------------------------------------
//  End of attachdialog.h
//------------------------------------------------------------------------------

