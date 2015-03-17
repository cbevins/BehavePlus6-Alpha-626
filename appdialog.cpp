//------------------------------------------------------------------------------
/*! \file appdialog.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AppPage, AppDialog, and AppTabDialog class methods.
 */

// Custom include files
#include "appdialog.h"
#include "appfilesystem.h"
#include "appmessage.h"
#include "appproperty.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "helpbrowser.h"
#include "textview.h"           // For widget printing

// Qt include files
#include <qcheckbox.h>
#include <qcursor.h>
#include <qdir.h>
#include <qhbox.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtooltip.h>

//------------------------------------------------------------------------------
/*! \brief AppDialog constructor.
 *
 *  \param parent       Pointer to the parent widget.
 *  \param captionKey   Dialog caption text translator key.  If the key begins
 *                      with a bang (!), the text is displayed verbatim and
 *                      without translation.
 *  \param pictureFile  Base name of the picture file to display in the left pane.
 *  \param pictureName  Name of the picture that is displayed
 *                      when the mouse pauses over the picture.
 *  \param htmlFile     Base name of the HTML file to display in the HelpBrowser.
 *  \param p_name         Internal widget name.
 *  \param acceptKey    Translator key for text displayed on the #m_acceptBtn
 *                      (default is "AppDialog:Button:Ok").
 *                      The #m_acceptBtn is always displayed.
 *  \param rejectKey    Translator key for text displayed on the #m_rejectBtn.
 *                      (default is "AppDialog:Button:Cancel").
 *                      If NULL or empty, #m_rejectBtn is not displayed.
 *  \param clearKey     Translator key for text displayed on the #m_clearBtn.
 *                      (default is "").
 *                      If NULL or empty, #m_clearBtn is not displayed.
 *  \param wizardKey    Translator key for text displayed on the #m_wizardBtn.
 *                      (default is "").
 *                      If NULL or empty, #m_wizardBtn is not displayed.
 */

AppDialog::AppDialog( QWidget *p_parent, const QString &captionKey,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile,    const char *p_name,
        const QString &acceptKey,   const QString &rejectKey,
        const QString &clearKey,    const QString &wizardKey ) :
    QDialog( p_parent, p_name, true,
        WStyle_Customize |          // Use the following flags:
        WStyle_SysMenu |            // System menu in upper left
        WStyle_Title |              // Window title
        WStyle_MinMax |             // Min/max in system menu
        WStyle_Dialog |             // Modal toplevel window
        WStyle_NormalBorder ),      // Resizeable corners
    m_pageLayout(0),
    m_page(0),
    m_buttonBox(0),
    m_acceptBtn(0),
    m_clearBtn(0),
    m_wizardBtn(0),
    m_rejectBtn(0),
    m_contextMenu(0)
{
    // Set the dialog caption
    QString l_caption("");
    translate( l_caption, captionKey );
    setCaption( appWindow()->m_program + " "
              + appWindow()->m_version + " " + l_caption );

    // Divide the dialog window into top and bottom sections.
    m_pageLayout = new QVBoxLayout( this, 10, 10, "m_pageLayout" );
    Q_CHECK_PTR( m_pageLayout);

    // Top section contains the AppPage.
    m_page = new AppPage( this, pictureFile, pictureName, htmlFile, p_name );
    checkmem( __FILE__, __LINE__, m_page, "AppPage appPage", 1 );
    m_pageLayout->addWidget( m_page );

    // The bottom section contains a row of buttons.
    m_buttonBox = new QHBox( this, "m_buttonBox" );
    Q_CHECK_PTR( m_buttonBox );
    m_pageLayout->addWidget( m_buttonBox );

    // Accept button
    QString text("");
    translate( text, acceptKey );
    m_acceptBtn = new QPushButton( text, m_buttonBox, "m_acceptBtn" );
    Q_CHECK_PTR( m_acceptBtn );
    connect( m_acceptBtn, SIGNAL( clicked() ), SLOT( store() ) );
    // Clear button
    if ( ! clearKey.isNull() && ! clearKey.isEmpty() )
    {
        translate( text, clearKey );
        m_clearBtn = new QPushButton( text, m_buttonBox, "m_clearBtn" );
        Q_CHECK_PTR( m_clearBtn );
        connect( m_clearBtn, SIGNAL( clicked() ), SLOT( clear() ) );
    }
    // Wizard button
    if ( ! wizardKey.isNull() && ! wizardKey.isEmpty() )
    {
        translate( text, wizardKey );
        m_wizardBtn = new QPushButton( text, m_buttonBox, "m_wizardBtn" );
        Q_CHECK_PTR( m_wizardBtn );
        connect( m_wizardBtn, SIGNAL( clicked() ), SLOT( wizard() ) );
    }
    // Cancel button
    if ( ! rejectKey.isNull() && ! rejectKey.isEmpty() )
    {
        translate( text, rejectKey );
        m_rejectBtn = new QPushButton( text, m_buttonBox, "m_rejectBtn" );
        Q_CHECK_PTR( m_rejectBtn );
        connect( m_rejectBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    }
    // Find the widest button
    int l_width = m_acceptBtn->sizeHint().width();
    QWidget *widest = m_acceptBtn;
    if ( m_rejectBtn && m_rejectBtn->sizeHint().width() > l_width )
    {
        l_width = m_rejectBtn->sizeHint().width();
        widest = m_rejectBtn;
    }
    if ( m_clearBtn && m_clearBtn->sizeHint().width() > l_width )
    {
        l_width = m_clearBtn->sizeHint().width();
        widest = m_clearBtn;
    }
    if ( m_wizardBtn && m_wizardBtn->sizeHint().width() > l_width )
    {
        l_width = m_wizardBtn->sizeHint().width();
        widest = m_wizardBtn;
    }
    // Make all buttons the same size as the widest button.
    m_acceptBtn->setFixedSize( widest->sizeHint() );
    if ( m_rejectBtn )
    {
        m_rejectBtn->setFixedSize( widest->sizeHint() );
    }
    if ( m_clearBtn )
    {
        m_clearBtn->setFixedSize( widest->sizeHint() );
    }
    if ( m_wizardBtn )
    {
        m_wizardBtn->setFixedSize( widest->sizeHint() );
    }
    m_buttonBox->setFixedHeight( m_buttonBox->sizeHint().height() );

    // Create the context menu.
    m_contextMenu = new QPopupMenu( this, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    translate( text, "AppDialog:ContextMenu:Print" );
    int mid = m_contextMenu->insertItem( text );
    m_contextMenu->setItemParameter( mid, ContextPrintDialog );
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppDialog destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
 */

AppDialog::~AppDialog( void )
{
    delete m_rejectBtn;         m_rejectBtn = 0;
    delete m_wizardBtn;         m_wizardBtn = 0;
    delete m_clearBtn;          m_clearBtn = 0;
    delete m_acceptBtn;         m_acceptBtn = 0;
    delete m_buttonBox;         m_buttonBox = 0;
    delete m_page;              m_page = 0;
    delete m_pageLayout;        m_pageLayout = 0;
    delete m_contextMenu;       m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual callback slot for the AppDialog's Clear button.
 *
 *  \attention This function should be reimplemented by all derived classes to
 *  do something useful like clearing the dialog contents.
 */

void AppDialog::clear( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the AppPage::m_contentFrame widget.
 *
 *  Called by derived dialogs so they know where to place all their content.
 *
 *  \return Pointer to the AppPage::m_contentFrame (QVBox) widget.
 */

QVBox *AppDialog::contentFrame( void ) const
{
    return( m_page->m_contentFrame );
}

//------------------------------------------------------------------------------
/*! \brief Access to the AppDialog's AppPage::m_helpBrowser widget.
 *
 *  \return Pointer to the AppPage::m_helpBrowser (HelpBrowser) widget.
 */

HelpBrowser *AppDialog::helpBrowser( void ) const
{
    return( m_page->m_helpBrowser );
}

//------------------------------------------------------------------------------
/*! \brief Access to the AppPage::m_midFrame widget.
 *
 *  Called by HelpBrowser and other dialogs that need to hide the midFrame.
 *
 *  \return Pointer to the AppPage::m_midFrame (QVBox) widget.
 */

QVBox *AppDialog::midFrame( void ) const
{
    return( m_page->m_midFrame );
}

//------------------------------------------------------------------------------
/*! \brief AppDialog's mouse event filter.
 *
 *  Reimplemented QDialog virtual function that traps right-click mouse
 *  events to invoke the context menu.  All other mouse events are passed up
 *  the inheritance chain so that links can be clicked, etc.
 *
 *  \param event Pointer to QMouseEvent event.
 */

void AppDialog::mousePressEvent( QMouseEvent *p_event )
{
    // If RightButton, display context menu at cursor position
    if ( p_event->button() == QMouseEvent::RightButton )
    {
        int l_result = m_contextMenu->exec( QCursor::pos() );
        int choice = m_contextMenu->itemParameter( l_result );
        if ( choice == ContextPrintDialog )
        {
            printWidget( this );
        }
    }
    // Otherwise pass the mouse event on to handle links, etc.
    else
    {
        QDialog::mousePressEvent( p_event );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the AppDialog's AppPage's m_optionFrame widget.
 *
 *  Called by the HelpDialog constructor.
 *
 *  \return Pointer to the AppPage::m_optionFrame (QHBox) pointer.
 */

QHBox *AppDialog::optionFrame( void ) const
{
    return( m_page->m_contentFrame );
}

//------------------------------------------------------------------------------
/*! \brief Access to the AppDialog's AppPage::m_pictureLabel widget.
 *
 *  \return Pointer to the AppPage::m_pictureLabel (QVBox) widget.
 */

QLabel *AppDialog::pictureLabel( void ) const
{
    return( m_page->m_pictureLabel );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppDialog's HelpBrowser HTML directory,
 *  topic file, and index file.
 *
 *  \param htmlDir  Full path name of the HTML file source directory
 *                  (with terminating separator).
 *  \param topic    Topic's home HTML document file name.
 *  \param index    Main index HTML document file name.
 *  \param help     Windows "hlp" help file name.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppDialog::setBrowser( const QString &htmlDir, const QString &htmlFile,
        const QString &indexFile, const QString &helpFile )
{
    return( m_page->setBrowser( htmlDir, htmlFile, indexFile, helpFile ) );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppDialog's picture pixmap and tool tip.
 *
 *  \param picturePath Base name of the picture image file.
 *  \param pictureTip Tool tip text to be displayed.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppDialog::setPicture(
        const QString &pictureFile,
        const QString &pictureTip )
{
    return( m_page->setPicture( pictureFile, pictureTip ) );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppDialog's HelpBrowser HTML source file.
 *
 *  \param topicFile Topic's home HTML document file name,
 *  which must be in the current htmlDir.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppDialog::setSourceFile( const QString &topicFile )
{
    return( m_page->setSourceFile( topicFile ) );
}

//------------------------------------------------------------------------------
/*! \brief Virtual callback slot for the AppDialog's Ok button.
 *
 *  \attention This function should be reimplemented by all derived classes to
 *  do something useful like storing the dialog settings before it is destroyed.
 */

void AppDialog::store( void )
{
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Returns the suggested width of the AppPage portion
 *  of the dialog minus the central pane.
 *
 *  Used by derived dialogs to adjust their total width.
 *
 *  \return Suggested width of the base dialog's AppPage minus its
 *          central pane.
 */

int AppDialog::widthHint( void )
{
    return( m_page->widthHint() );
}

//------------------------------------------------------------------------------
/*! \brief Virtual callback slot for the AppDialog's "Wizard" button.
 *
 *  \attention This function should be reimplemented by all derived classes to
 *  do something useful like displaying a wizard dialog.
 */

void AppDialog::wizard( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppPage constructor.
 *
 *  \param parent      Pointer to the parent widget, usually an
 *                     AppDialog or an AppTabDialog.
 *  \param pictureFile Base name of the picture file to display in the left
 *                     pane, If NULL or empty, no picture is displayed
 *                     regardless of settings.
 *  \param pictureName Text that is displayed when the mouse pauses over the
 *                     picture.  If the first character is a bang (!), then
 *                     the text is displayed as is, without translation.
 *                     Otherwise it is presumed to be the picture title,
 *                     and is translated to read "Detail from <pictureName>.".
 *  \param htmlFile    Base name of the HTML file to display in the HelpBrowser.
 *                     If NULL or empty, no helpBrowser is displayed regardless
 *                     of settings.
 *  \param p_name        Internal widget name.
 */

AppPage::AppPage( QWidget *p_parent, const QString &pictureFile,
        const QString &pictureName, const QString &htmlFile, const char *p_name ) :
    QSplitter( p_parent, p_name ),
    m_pictureLabel(0),
    m_midFrame(0),
    m_contentFrame(0),
    m_optionFrame(0),
    m_helpBrowser(0),
    m_showBrowser(0),
    m_showPicture(0)
{
    // Add some visual space around the borders.
    setMargin( 10 );

    // The optional left pane contains a picture.
    if ( ! pictureFile.isNull() && ! pictureFile.isEmpty() )
    {
        // Create a label containing the picture.
        m_pictureLabel = new QLabel( this,
            QString("m_pictureLabel%1").arg( p_name ) );
        Q_CHECK_PTR( m_pictureLabel );

        // Set the picture file and tool tip.
        // HACK: if pictureName starts with a bang (!), it is displayed as is.
        // Otherwise, the phrase "Detail from " is prefixed to it.
        QString pictureTip( pictureName );
        if ( pictureTip.left( 1 )  == "!" )
        {
            pictureTip = pictureName.mid( 1 );
        }
        else
        {
            translate( pictureTip, "Picture:Detail", pictureName );
        }
        setPicture( pictureFile, pictureTip );

        // Make the picture appear inset into the frame.
        m_pictureLabel->setFrameStyle( QFrame::Box | QFrame::Sunken );

        // Does the user even want to see the picture?
        if ( appProperty()->boolean( "appShowPicture" ) )
        {
            m_pictureLabel->show();
        }
        else
        {
            m_pictureLabel->hide();
        }
    }
    // The middle pane contains the dialog content
    // and picture/browser toggle box.
    m_midFrame = new QVBox( this, "m_midFrame" );
    Q_CHECK_PTR( m_midFrame );
    m_midFrame->setFrameStyle( QFrame::NoFrame );
    m_midFrame->setMargin( 1 );

    // The upper middle pane contains dialog content to be filled in by
    // derived classes.
    m_contentFrame = new QVBox( m_midFrame, "m_contentFrame" );
    Q_CHECK_PTR( m_contentFrame );
    m_contentFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    m_contentFrame->setMargin( 10 );

    // The optional right pane contains a HelpBrowser.
    if ( ! htmlFile.isNull() && ! htmlFile.isEmpty() )
    {
        QString htmlDir = appFileSystem()->docHtmlPath();
        QString indexFile = appFileSystem()->docHtmlIndexFile();
        m_helpBrowser = new HelpBrowser( this, htmlDir, htmlFile, indexFile,
            appFileSystem()->helpFile(), "m_helpBrowser" );
        checkmem( __FILE__, __LINE__, m_helpBrowser,
            "HelpBrowser m_helpBrowser", 1 );

        // Does the user even want to see it?
        if ( appProperty()->boolean( "appShowBrowser" ) )
        {
            m_helpBrowser->show();
        }
        else
        {
            m_helpBrowser->hide();
        }
    }
    // The lower middle pane contains buttons to show/hide picture/browser.
    if ( ! m_pictureLabel && ! m_helpBrowser )
    {
        return;
    }
    m_optionFrame = new QHBox( m_midFrame, "m_optionFrame" );
    Q_CHECK_PTR( m_optionFrame );
    m_optionFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
    m_optionFrame->setMargin( 10 );

    // Picture checkbox.
    QString text("");
    if ( m_pictureLabel )
    {
        translate( text, "AppPage:Picture" );
        m_showPicture = new QCheckBox( text, m_optionFrame, "m_showPicture" );
        Q_CHECK_PTR( m_showPicture );
        m_showPicture->setChecked( appProperty()->boolean( "appShowPicture" ) ) ;
        connect( m_showPicture, SIGNAL( toggled( bool ) ),
                 this,          SLOT( pictureToggled( bool ) ) );
    }
    // HelpBrowser checkbox.
    if ( m_helpBrowser )
    {
        translate( text, "AppPage:Help" );
        m_showBrowser = new QCheckBox( text, m_optionFrame, "m_showBrowser" );
        Q_CHECK_PTR( m_showBrowser );
        m_showBrowser->setChecked( appProperty()->boolean( "appShowBrowser" ) );
        connect( m_showBrowser, SIGNAL( toggled( bool ) ),
                 this,          SLOT( browserToggled( bool ) ) );
    }

    // Set the picture/browser frame size.
    m_optionFrame->setMinimumWidth( m_optionFrame->sizeHint().width() );
    m_optionFrame->setMaximumHeight( m_optionFrame->sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppPage destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
 */

AppPage::~AppPage( void )
{
    delete m_showBrowser;       m_showBrowser = 0;
    delete m_showPicture;       m_showPicture = 0;
    delete m_optionFrame;       m_optionFrame = 0;
    delete m_helpBrowser;       m_helpBrowser = 0;
    delete m_contentFrame;      m_contentFrame = 0;
    delete m_midFrame;          m_midFrame = 0;
    delete m_pictureLabel;      m_pictureLabel = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppPage "Help" checkbox callback slot.
 *
 *  Shows or hides the HelpBrowser depending upon the "Help" checkbox state.
 *
 *  \param on If TRUE, HelpBrowser is toggled on.
 *            If FALSE, HelpBrowser is toggled off.
 */

void AppPage::browserToggled( bool on )
{
    if ( on )
    {
        m_helpBrowser->show();
    }
    else
    {
        m_helpBrowser->hide();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppPage "Picture" checkbox callback slot.
 *
 *  Shows or hides the picture depending upon the "Picture" checkbox state.
 *
 *  \param on If TRUE, picture is toggled on.  If FALSE, picture is toggled off.
*/

void AppPage::pictureToggled( bool on )
{
    if ( on )
    {
        m_pictureLabel->show();
    }
    else
    {
        m_pictureLabel->hide();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppPage's HelpBrowser HTML directory,
 *  topic file, and index file.
 *
 *  Called by AppDialog::setBrowser().
 *
 *  \param htmlDir  Full path name of the HTML file source directory
 *                  (with terminating separator).
 *  \param topic    Topic's home HTML document file name.
 *  \param index    Main index HTML document file name.
 *  \param help     Windows "hlp" help file name.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppPage::setBrowser(const QString &htmlDir, const QString &htmlFile,
        const QString &indexFile, const QString &helpFile )
{
    if ( m_helpBrowser )
    {
        return( m_helpBrowser->setFiles(
            htmlDir, htmlFile, indexFile, helpFile ) );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppPage's picture pixmap and tool tip.
 *
 *  \param picturePath Base name of the picture image file.
 *  \param pictureTip Tool tip text to be displayed.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppPage::setPicture( const QString &pictureFile,
        const QString &pictureTip )
{
    // If no picture label in the AppPage, simply return.
    if ( ! m_pictureLabel )
    {
        return( false );
    }
    // Check if the picture is in the pixmap cache.
    QPixmap picturePixmap;
    if ( ! QPixmapCache::find( pictureFile, picturePixmap ) )
    {
        //qDebug( QString( "%1 pixmap loaded by AppPage::setPicture()." )
        //    .arg( pictureFile ) );
        picturePixmap = QPixmap( appFileSystem()->imagePath( pictureFile ) );
        QPixmapCache::insert( pictureFile, picturePixmap );
    }
    // Display a white space if the picture can't be created.
    if ( picturePixmap.isNull() )
    // This code block should never be executed!
    {
        int wd = 185;
        int ht = 300;
        picturePixmap.resize( wd, ht );
        QPainter paint( &picturePixmap );
        paint.fillRect( 0, 0, wd, ht, QBrush( "white", SolidPattern ) );
        paint.end();
        return( false );
    }
    // Apply the pixmap to the picture label.
    m_pictureLabel->setPixmap( picturePixmap );

    // Don't let the dialog get any shorter than the picture.
    m_pictureLabel->setMinimumHeight( m_pictureLabel->sizeHint().height() );

    // Don't let the picture get any wider than necessary.
    m_pictureLabel->setMaximumWidth( m_pictureLabel->sizeHint().width() );

    // Add a tool tip to the picture.
    if ( ! pictureTip.isNull() && ! pictureTip.isEmpty() )
    {
        QToolTip::add( m_pictureLabel, pictureTip );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets/resets the AppPage's HelpBrowser's HTML source file.
 *
 *  \param topicFile Topic's home HTML document file name,
 *  which must be in the current htmlDir.
 *
 *  \return TRUE on success, FALSE if the files cannot be found or read.
 */

bool AppPage::setSourceFile( const QString &topicFile )
{
    if ( m_helpBrowser )
    {
        return( m_helpBrowser->setSourceFile( topicFile ) );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Access to the combined width of the AppPage's picture pane
 *  (if active) plus HelpBrowser (if active).
 *
 *  \return Suggested combined width of the base dialog's active picture
 *  and/or HelpBrowser components.
 */

int AppPage::widthHint( void )
{
    int l_width = 20;
    if ( m_pictureLabel )
    {
        l_width += m_pictureLabel->sizeHint().width();
    }
    if ( m_helpBrowser )
    {
        l_width += m_helpBrowser->sizeHint().width();
    }
    return( l_width );
}

//------------------------------------------------------------------------------
/*! \brief AppTabDialog constructor.
 *
 *  \param parent     Pointer to the parent widget.
 *  \param captionKey Dialog caption text translator key.  If the key begins
 *                    with a bang (!), the text is displayed verbatim and
 *                    without translation.
 *  \param p_name       Internal widget name.
 *  \param acceptKey  Translator key for text displayed on the tab okButton
 *                    (default is "AppDialog:Button:Ok").
 *  \param rejectKey  Translator key fo text displayed on the cancelButton
 *                    (default is "AppDialog:Button:Cancel").
 *                    If NULL or empty, the Cancel button is not displayed.
 */

AppTabDialog::AppTabDialog( QWidget *p_parent, const QString &captionKey,
        const char *p_name, const QString &acceptKey, const QString &rejectKey ) :
    QTabDialog( p_parent, p_name, true,
        WStyle_Customize |          // Use following flags
        WStyle_SysMenu |            // System menu in upper left
        WStyle_Title |              // Window title
        WStyle_MinMax |             // Min/max in system menu
        WStyle_Dialog |             // Modal toplevel window
        WStyle_NormalBorder ),      // Resizeable corners
    m_contextMenu(0)
{
    // Set the dialog caption
    QString l_caption("");
    translate( l_caption, captionKey );
    setCaption( appWindow()->m_program + " "
              + appWindow()->m_version + " " + l_caption );

    // Add the standard QTabDialog Ok button.
    QString text("");
    translate( text, acceptKey );
    setOkButton( text );
    connect( this, SIGNAL( applyButtonPressed() ),
             this, SLOT( store() ) );

    // Optionally add the QTabDialog cancel button.
    if ( ! rejectKey.isNull() && ! rejectKey.isEmpty() )
    {
        translate( text, rejectKey );
        setCancelButton( text );
        connect( this, SIGNAL( cancelButtonPressed() ),
                 this, SLOT( reject() ) );
    }
    // Create the context menu.
    m_contextMenu = new QPopupMenu( this, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    translate( text, "AppDialog:ContextMenu:Print" );
    int mid = m_contextMenu->insertItem( text );
    m_contextMenu->setItemParameter( mid, ContextPrintDialog );
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppTabDialog destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
 */

AppTabDialog::~AppTabDialog( void )
{
    delete m_contextMenu;   m_contextMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief AppTabDialog mouse event filter.
 *
 *  Reimplemented QDialog virtual function that traps right-click mouse
 *  events to invoke the context menu.  All other mouse events are passed up
 *  the inheritance chain so that links can be clicked, etc.
 */

void AppTabDialog::mousePressEvent( QMouseEvent *p_event )
{
    // If RightButton, display context menu at cursor position
    if ( p_event->button() == QMouseEvent::RightButton )
    {
        int l_result = m_contextMenu->exec( QCursor::pos() );
        int choice = m_contextMenu->itemParameter( l_result );
        if ( choice == ContextPrintDialog )
        {
            printWidget( this );
        }
    }
    // Otherwise pass the mouse event on to handle links, etc.
    else
    {
        QTabDialog::mousePressEvent( p_event );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual callback slot for the AppTabDialog's "Ok" button
 *
 *  \attention This function should be reimplemented by all derived classes to
 *  do something useful like storing the dialog settings before it is destroyed.
 */

void AppTabDialog::store( void )
{
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of appdialog.cpp
//------------------------------------------------------------------------------

