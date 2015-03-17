//------------------------------------------------------------------------------
/*! \file app.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus QApplication.
 */

#ifndef _APPLICATION_H_
/*! \def _APPLICATION_H_
 *  \brief Prevent redundant includes.
 */
#define _APPLICATION_H_ 1

// Qt include files
#include <qapplication.h>
#include <qpixmap.h>
class QLabel;
class QString;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class BehavePlusApp app.h
 *
 *  \brief QApplication class for BehavePlus3.
 *
 *  Allows the program to display a splash screen with initialization messages
 *  and immediately enables the gui so errors can be displayed on screen.
 */

class BehavePlusApp : public QApplication
{
// Public methods
public:
    BehavePlusApp( int &p_argc, char **p_argv ) ;
    void    closeSplashPage( void ) ;
    void    saveSplashPage( void ) ;
    void    showSplashPage( bool p_saveSplashPage ) ;
    void    updateSplashPage( const QString &message ) ;

private:
    void    drawSplashPage( void ) ;

// Public data
public:
    QString m_program;      //!< Program name    -- "BehavePlus"
    QString m_version;      //!< Program version -- "2.0.0"
    QString m_build;        //!< Program build   -- "Build 241"
    QString m_releaseNote;  //!< Program release -- "Fake" or "RC1" or "Beta"
    QPixmap m_pixmap;
    QLabel *m_splash;
    int     m_statusLine;
};

#endif

//------------------------------------------------------------------------------
//  End of app.h
//------------------------------------------------------------------------------

