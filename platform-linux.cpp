//------------------------------------------------------------------------------
/*! \file platform-linux.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Linux-specific implementations.
 */

// Custom include files
#include "appmessage.h"
#include "appfilesystem.h"
#include "appproperty.h"
#include "apptranslator.h"
#include "platform.h"

// Qt include files
#include <qapplication.h>
#include <qdir.h>
#include <qprocess.h>
#include <qstring.h>

// *nix include files
#include <stdlib.h>
#include <unistd.h>

//------------------------------------------------------------------------------
/*! \brief Special hack to accommodate the stupid Microsoft non-Posix names.
 */

void platformAbort( void )
{
    abort();
}

//------------------------------------------------------------------------------
/*! \brief Special hack to accommodate the stupid Microsoft non-Posix names.
 */

void platformExit( int status )
{
    exit( status );
}

//------------------------------------------------------------------------------
/*! \brief Special hack to accommodate the stupid Microsoft non-Posix names.
 *
 *  \param buffer Location in which the current working directory is returned.
 *  \param size Length of \a buffer.
 *
 *  \return Pointer the \a buffer.
 */

char *platformGetCwd( char *buffer, size_t size)
{
    return( getcwd( buffer, size ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the full path name of the executable program.
 *
 *  \param withProgramName If TRUE, the program name is included in the path.
 *                         If FALSE, the program name is omitted from the path
 *                         and the path does NOT end in a separator.
 *
 *  \return Pointer to static storage containing the full path name
 *          of the executable program.
 */

char *platformGetExePath( bool withProgramName )
{
    static char storage[1024];
    memset( storage, '\0', sizeof(storage) );
    QFileInfo prog( qApp->argv()[0] );
    QString path = prog.dir( true ).path();
    if ( withProgramName )
    {
        sprintf( storage, "%s/%s", path.latin1(), qApp->argv()[0] );
    }
    else
    {
        sprintf( storage, "%s", path.latin1() );
    }
    // Return pointer to static storage.
    return( storage );
}

//------------------------------------------------------------------------------
/*! \brief Returns the operating system name.
 *
 *  \return Operating system name.
 */

QString platformGetOs( void )
{
    return( "Linux" );
}

//------------------------------------------------------------------------------
/*! \brief Special hack to accommodate the stupid Microsoft non-Posix names.
 *
 *  \return Process id.
 */

int platformGetPid( void )
{
    return( getpid() );
}

//------------------------------------------------------------------------------
/*! \brief Returns the full installation path name (without a trailing
 *  separator) under Windows systems.
 *
 *  Never gets called under Linux.
 *
 *  \retval Under Windows flavor operating systems, returns the full BehavePlus
 *  installation path name (without a trailing separator)
 *  or returns an empty string if the Registry entry is not found.
 *  \retval Under Linux flavor operating systems, returns "Linux".
 */

QString platformGetWindowsInstallPath( void )
{
    return( "Linux" );
}

//------------------------------------------------------------------------------
/*! \brief Displays the Linux version of Program WinHelp
 *
 *  Called only by the AppWindow::slotHelpProgram().
 */

void platformShowHelp( const QString &helpFile )
{
    QProcess *proc = new QProcess( qApp );
    QString program = appProperty()->string( "appHelpReaderLinux" );
    proc->addArgument( program );
    proc->addArgument( helpFile );
    QString stdIn( "" );
    if ( ! proc->launch( stdIn ) )
    {
        error( QString( "Couldn't lauch %1 %2" )
            .arg( program ).arg( helpFile ) );
    }

    //QString caption(""), text("");
    //translate( caption, "AppWindow:NoProgramHelp:Caption" );
    //translate( text, "AppWindow:NoProgramHelp:Text" );
    //if ( yesno( caption, text ) == QDialog::Accepted )
    //{
    //    helpDialog( "index.html" );
    //}
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the HTML index file.
 *
 *  Never gets called under Linux.
 *
 *  Called only by HelpBrowser::showIndex( void ).
 */

void platformShowHelpBrowserIndex( const QString & )
{
    return;
}

//------------------------------------------------------------------------------
//  End of platform-linux.cpp
//------------------------------------------------------------------------------

