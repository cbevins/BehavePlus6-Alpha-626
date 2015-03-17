//------------------------------------------------------------------------------
/*! \file textview.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QTextView and a QTextBrowser derived classes that support printing
 *  of their rich text contents via a right-click context menu.
 */

#ifndef _TEXTVIEW_H_
/*! \def _TEXTVIEW_H_
    \brief Prevent redundant includes.
 */
#define _TEXTVIEW_H_ 1

// Qt class references
#include <qlistview.h>
#include <qtextbrowser.h>
#include <qtextedit.h>

//------------------------------------------------------------------------------
/*! \class TextBrowser textview.h
 *
 *  \brief A QTextBrowser derived class that supports printing of its rich text
 *  contents via a right-click context menu.
 */

class TextBrowser : public QTextBrowser
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of context menu options available.
 */
enum ContextMenuOption
{
    ContextPrintVisible   = 0,
    ContextPrintEntire    = 1,
    ContextPrintFormatted = 2
};

// Public methods
public:
    TextBrowser( QWidget *parent, const char *name ) ;
    ~TextBrowser( void );
    void setSourceFile( const QString &fileName ) ;

// Protected and private methods
protected:
    virtual void viewportMousePressEvent( QMouseEvent *event );
protected slots:
    void contextMenuActivated( int id ) ;

// Public data members
public:
    QString     m_sourceFile;   //!< File name of text to display.
    QPopupMenu *m_contextMenu;  //!< Ptr to popup context menu.
};

//------------------------------------------------------------------------------
/*! \class TextView textview.h
 *
 *  \brief A QTextEdit derived class that supports printing of its rich text
 *  contents via a right-click context menu.
 */

class TextView : public QTextEdit
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of context menu options available.
 */
enum ContextMenuOption
{
    ContextPrintVisible   = 0,
    ContextPrintEntire    = 1,
    ContextPrintFormatted = 2
};

// Public methods
public:
    TextView( QWidget *parent, const char *name ) ;
    ~TextView( void );

// Protected and private methods
protected:
    virtual void viewportMousePressEvent( QMouseEvent *event );
protected slots:
    void contextMenuActivated( int id ) ;

// Public data members
public:
    QPopupMenu *m_contextMenu;  //!< Ptr to popup context menu.
};

//------------------------------------------------------------------------------
/*! \brief Non-member functions that actually perform the printing
 * for both classes.
 */

bool printListView( QScrollView *scrollView ) ;
bool printRichText( QTextEdit *textEdit ) ;
bool printWidget( QWidget *widget ) ;

#endif

//------------------------------------------------------------------------------
//  End of textview.h
//------------------------------------------------------------------------------

