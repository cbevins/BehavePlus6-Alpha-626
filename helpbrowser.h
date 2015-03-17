//------------------------------------------------------------------------------
/*! \file helpbrowser.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Help browser class declarations.
 */

#ifndef _HELPBROWSER_H_
/*! \def _HELPBROWSER_H_
    \brief Prevent redundant includes.
 */
#define _HELPBROWSER_H_ 1

// Class references
#include <qvbox.h>
class TextBrowser;
class QHBox;
class QPushButton;

//------------------------------------------------------------------------------
/*! \class HelpBrowser helpbrowser.h
 *
 *  \brief Embeddable help HTML browser with Back, Forward, Home, and Index
 *  buttons.
 */

class HelpBrowser : public QVBox
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    HelpBrowser( QWidget *parent, const char *name ) ;
    HelpBrowser( QWidget *parent, const QString &htmlDir,
        const QString &topicFile, const QString &indexFile,
        const QString &helpFile,  const char *name ) ;
    ~HelpBrowser( void ) ;

    bool setFiles( const QString &htmlDir, const QString &topicFile,
        const QString &indexFile, const QString &helpFile ) ;

    bool setSourceFile( const QString &topicFile ) ;

// Private methods
private:
    void init( void ) ;

// Protected slots
protected slots:
    void showIndex( void ) ;

// Protect data elements
protected:
    QHBox        *m_navFrame;   //!< Frame for navigation buttons
    QPushButton  *m_btn[4];     //!< Navigation buttons
    TextBrowser  *m_browser;    //!< Pointer to help's TextBrowser
    QString       m_htmlDir;    //!< HTML directory full path name
    QString       m_topicFile;  //!< Name of current topic file in m_htmlDir
    QString       m_indexFile;  //!< Name of the main index file in m_htmlDir
    QString       m_helpFile;   //!< Name of the Windows "hlp" file in m_htmlDir
};

#endif

//------------------------------------------------------------------------------
//  End of helpbrowser.h
//------------------------------------------------------------------------------

