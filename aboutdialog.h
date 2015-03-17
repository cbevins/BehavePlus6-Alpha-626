//------------------------------------------------------------------------------
/*! \file aboutdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus AboutDialog class declaration.
 */

#ifndef _ABOUTDIALOG_H_
//! Prevent redundant includes.
#define _ABOUTDIALOG_H_ 1

// Forward class references.
#include <QObject.h>
#include <qobject.h>
#include "appdialog.h"

//------------------------------------------------------------------------------
/*! \class AboutDialog aboutdialog.h
 *
 *  \brief The AboutDialog is a tabbed dialog invoked from the main menu by
 *  \b Help->About.
 *
 *  The dialog uses the standard AppTabDialog, and each page simply displays
 *  its own HTML file in the HelpBrowser and picture in the picture pane.
 */

class AboutDialog : public AppTabDialog
{
    Q_OBJECT
// Public methods
public:
    AboutDialog( QWidget *p_parent, const char *p_name="aboutDialog" ) ;
    ~AboutDialog( void ) ;

// Private methods
private:
    void behaveplus( void ) ;
    void andrews( void ) ;
    void bevins( void ) ;
    void carlton( void ) ;
    void dolack( void ) ;
    void funding( void );

// Private data members
private:
    //! Pointer to the dynamically-allocated BehavePlus AppPage.
    AppPage *m_page1;
    //! Pointer to the dynamically-allocated Andrews AppPage.
    AppPage *m_page2;
    //! Pointer to the dynamically-allocated Bevins AppPage.
    AppPage *m_page3;
    //! Pointer to the dynamically-allocated Carlton AppPage.
    AppPage *m_page4;
    //! Pointer to the dynamically-allocated Dolack AppPage.
    AppPage *m_page5;
    //! Pointer to the dynamically-allocated Acknowledgments AppPage.
    AppPage *m_page6;
};

#endif

//------------------------------------------------------------------------------
//  End of aboutdialog.h
//------------------------------------------------------------------------------

