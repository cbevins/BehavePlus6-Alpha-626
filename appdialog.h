//------------------------------------------------------------------------------
/*! \file appdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AppPage, AppDialog, and AppTabDialog class declarations.
 */

#ifndef _APPDIALOG_H_
/*! \def _APPDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _APPDIALOG_H_ 1

// Class references.
#include <qobject.h>
#include <qhbox.h>
#include <qdialog.h>
#include <qsplitter.h>
#include <qtabdialog.h>
#include <qvbox.h>
class HelpBrowser;
class PropertyDict;
class QCheckBox;
class QFrame;
class QHBox;
class QLabel;
class QPopupMenu;
class QVBoxLayout;

class AppDialog;
class AppPage;
class AppTabDialog;

//------------------------------------------------------------------------------
/*! \class AppDialog appdialog.h
 *
 *  \brief Standard BehavePlus dialog base class inherited by all other
 *  non-tabbed dialog classes.
 *
 *  This is the base class for all modal dialogs used by BehavePlus.
 *  It knows about and uses the current data file structure and application
 *  appearance settings to create uniform (and hopefully, attractive)
 *  dialogs across the entire BehavePlus application.
 *
 *  The dialog has an upper section containing an AppPage with the
 *  main dialog contents, and a lower section containing one or more buttons.
 *
 *  The AppPage has three panes laid out horizontally:
 *  -#  the left pane contains an optional picture,
 *  -#  the middle pane contains the content frame which is populated
 *      by the derived class, and
 *  -#  the right pane contains an optional HelpBrowser.
 *
 *  The button box contains 1 or more buttons:
 *  -   The #m_acceptBtn calls the virtual store() slot member function.
 *      store() should be re-implemented by derived classes to do something
 *      useful like storing the dialog's setting.
 *      The button displays the constructor's \a acceptText which defaults to
 *      "Ok".  The button is not displayed if \a acceptText is NULL or empty.
 *
 *  -   The #m_rejectButton calls QDialog::reject() to close the dialog.
 *      The button displays the constructor's \a rejectText which defaults to
 *      "Cancel".  The button is not displayed if \a rejectText is NULL or empty.
 *
 *  -   The #m_clearButton calls the virtual clear() slot member function.
 *      clear() should be re-implemented by derived classes to do something
 *      useful like clearing the dialog's input entries or restoring defaults.
 *      The button only appears if the constructor's \a clearText is not NULL
 *      or empty, which it is by default.
 *
 *  -   The #m_wizardBtn calls the virtual wizard() slot member function.
 *      wizard() should be re-implemented by derived classes to do something
 *      useful like displaying a wizard or choices dialog.
 *      The button only appears if the constructor's \a wizardText is not NULL
 *      or empty, which it is by default.
 */

class AppDialog : public QDialog
{
/*! \var ContextMenuOption
 *  \brief Enumerates the available context menu options.
 */
enum ContextMenuOption
{
    ContextPrintDialog = 1      //!< Prints the entire dialog.
};

// Enable signals and slots on this class.
    Q_OBJECT

// Public methods
public:
    AppDialog(
        QWidget *p_parent,
        const QString &captionKey,
        const QString &pictureFile,
        const QString &pictureName,
        const QString &htmlFile,
        const char *p_name,
        const QString &acceptText="AppDialog:Button:Ok",
        const QString &rejectText="AppDialog:Button:Cancel",
        const QString &clearText="",
        const QString &wizardText="" ) ;
    ~AppDialog( void );
    QVBox  *contentFrame( void ) const ;
    HelpBrowser *helpBrowser( void ) const ;
    QVBox  *midFrame( void ) const;
    QHBox  *optionFrame( void ) const ;
    QLabel *pictureLabel( void ) const ;
    bool    setBrowser( const QString &htmlDir, const QString &htmlFile,
                const QString &indexFile, const QString &helpFile ) ;
    bool    setPicture( const QString &pictureFile, const QString &pictureTip ) ;
    bool    setSourceFile( const QString &topicFile ) ;
    int     widthHint( void ) ;

// Protected methods
protected:
    virtual void mousePressEvent( QMouseEvent *event );

protected slots:
    virtual void clear( void );
    virtual void store( void );
    virtual void wizard( void );

// Public data members
public:
    QVBoxLayout  *m_pageLayout;  //!< Pointer to page layout.
    AppPage      *m_page;        //!< Pointer to the single AppPage.
    QHBox        *m_buttonBox;   //!< Pointer to button box.
    QPushButton  *m_acceptBtn;   //!< Pointer to "Ok" button.
    QPushButton  *m_clearBtn;    //!< Pointer to "Clear" button.
    QPushButton  *m_wizardBtn;   //!< Pointer to "Choices" button.
    QPushButton  *m_rejectBtn;   //!< Pointer to "Cancel" button.
    QPopupMenu   *m_contextMenu; //!< Pointer to context menu.
};

//------------------------------------------------------------------------------
/*! \class AppPage appdialog.h
 *
 *  \brief AppPage defines the common appearance of all dialogs used by
 *  BehavePlus.  AppDialogs and AppTabDialogs contain one or more AppPages
 *  to display their content.
 *
 *  The AppPage has three panes laid out horizontally:
 *  -#  the left pane contains a picture #m_pictureLabel,
 *  -#  the middle pane contains the #m_contentFrame which is populated
 *      by the derived class, and
 *  -#  the right pane contains a HelpBrowser #m_helpBrowser.
 *
 *  AppPage knows about application-wide resources such as the file
 *  system structure and application properties.
*/

class AppPage : public QSplitter
{
// Enable signals and slots on this class.
    Q_OBJECT

// Friend classes
    friend class AppDialog;
    friend class AppTabDialog;

// Public methods
public:
    AppPage( QWidget *p_parent, const QString &pictureFile,
            const QString &pictureName, const QString &htmlFile,
            const char *p_name );
    ~AppPage( void );
    void addButtons( const QString &acceptText, const QString &rejectText ) ;
    bool setBrowser( const QString &htmlDir, const QString &htmlFile,
            const QString &indexFile, const QString &helpFile ) ;
    bool setPicture( const QString &pictureFile, const QString &pictureTip ) ;
    bool setSourceFile( const QString &topicFile ) ;
    int  widthHint( void ) ;

// Protected methods
protected slots:
    virtual void browserToggled( bool on ) ;
    virtual void pictureToggled( bool on ) ;

// Public data members
public:
    QLabel       *m_pictureLabel;   //!< Pointer to label containing the picture.
    QVBox        *m_midFrame;       //!< Pointer to central frame.
    QVBox        *m_contentFrame;   //!< Pointer to dialog content frame.
    QHBox        *m_optionFrame;    //!< Pointer to picture & help option frame.
    HelpBrowser  *m_helpBrowser;    //!< Pointer to HelpBrowser.
    QCheckBox    *m_showBrowser;    //!< Pointer to HelpBrowser show/hide checkbox.
    QCheckBox    *m_showPicture;    //!< Pointer to picture show/hide checkbox.
    QPushButton  *m_printButton;    //!< Pointer to dialog print button
};

//------------------------------------------------------------------------------
/*! \class AppTabDialog appdialog.h
 *
 *  \brief Standard BehavePlus dialog base class inherited by all other
 *  tabbed dialog classes.
 *
 *  This is the base class for all modal tabbed dialogs used by BehavePlus.
 *  It knows about and uses the current data file structure and application
 *  appearance settings to create uniform (and hopefully, attractive)
 *  dialogs across the entire BehavePlus application.
 *
 *  The tab page of the dialog has an upper section containing an AppPage with
 *  the main page contents, and a lower section containing one or two buttons.
 *
 *  Each tab page has an AppPage with three panes laid out horizontally:
 *  -#  the left pane containing a picture,
 *  -#  the middle pane containing the content frame which is populated
 *      by the derived class, and
 *  -#  the right pane containing a HelpBrowser.
 *
 *  The button box contains 1 or 2 buttons:
 *  -   The okButton calls the virtual store() slot member function.
 *      store() should be reimplemented by derived classes to do something
 *      useful like storing the dialog's setting.
 *      The button displays the constructor's \a okText which defaults to "Ok".
 *      The button is not displayed if \a okText is NULL or empty.
 *
 *  -   The cancelButton calls QDialog::reject() to close the dialog.
 *      The button displays the constructor's \a cancelText which defaults to
 *      "Cancel". The button is not displayed if \a cancelText is NULL or empty.
 */

class AppTabDialog : public QTabDialog
{
/*! \var ContextMenuOption
 *  \brief Enumerates the available context menu options.
 */
enum ContextMenuOption
{
    ContextPrintDialog = 1  //!< Print the entire dialog.
};

// Enable signals and slots on this class.
    Q_OBJECT

// Public methods
public:
    AppTabDialog(
        QWidget *p_parent,
        const QString &captionKey,
        const char *p_name,
        const QString &acceptText="AppDialog:Button:Ok",
        const QString &rejectText="AppDialog:Button:Cancel" ) ;
    ~AppTabDialog( void ) ;

// Protected methods
protected:
    virtual void mousePressEvent( QMouseEvent *event );

// Protected slots
protected slots:
    virtual void store( void );

// Public data members
public:
    QPopupMenu *m_contextMenu;  //!< Pointer to the popup context menu.
};

#endif

//------------------------------------------------------------------------------
//  End of appdialog.h
//------------------------------------------------------------------------------

