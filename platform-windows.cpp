//------------------------------------------------------------------------------
/*! \file platform-windows.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Windoze-specific implementations.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "platform.h"

// Qt include files
#include <qapplication.h>
#include <qfileinfo.h>
#include <qprocess.h>
#include <qstring.h>

// Windows include files
#include <direct.h>
#include <process.h>
#include <windows.h>
#include <winuser.h>
#include <shlwapi.h>

// Local function declarations
QString platformGetWindowsRegString( HKEY key, const char *subKey ) ;

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
 *  \return Current working directory.
 */

char *platformGetCwd( char *buffer, size_t size)
{
    return( _getcwd( buffer, size ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the full pathname of the executable program.
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
    // Get the full path name of this process.
    static char storage[1024];
    TCHAR buffer[1024];
    LPTSTR buff = buffer;
    int n = GetModuleFileName( GetModuleHandle( NULL ), buff, sizeof(buffer) );
    // Convert it from TCHAR to ordinary ASCII char
    memset( storage, '\0', sizeof(storage) );
    int i;
    for ( i=0; i<n; i++ )
    {
        storage[i] = (char) buffer[i];
    }
    // If no program name is requested, strip it off the tail
    if ( ! withProgramName )
    {
        for( i=n-1; i>=0; i-- )
        {
            if ( storage[i] == '\\' )
            {
                storage[i] = '\0';
                break;
            }
        }
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
    return( "Windows" );
}

//------------------------------------------------------------------------------
/*! \brief Special hack to accommodate the stupid Microsoft non-Posix names.
 *
 *  \return Process id.
 */

int platformGetPid( void )
{
    return( _getpid() );
}

//------------------------------------------------------------------------------
/*! \brief Returns the full installation path name (without a trailing
 *  separator) under Windows systems.
 *
 *  Requests the HKEY_CLASSES_ROOT\bpf.BehavePlus\shell\open\command Registry
 *  key, which has the entire executable path name.
 *
 *  \retval Under Windows flavor operating systems, returns the full BehavePlus
 *  installation path name (without a trailing separator)
 *  or returns an empty string if the Registry entry is not found.
 *  \retval Under Linux flavor operating systems, returns "Linux".
 */

QString platformGetWindowsInstallPath( void )
{
    QString path( "" );
    HKEY key;
    if ( RegOpenKeyExA( HKEY_CLASSES_ROOT,
        "SEM.BehavePlus.2\\shell\\open\\command",
        0, KEY_READ, &key ) == ERROR_SUCCESS )
    {
        path = platformGetWindowsRegString( key, 0 );
        // Strip off the leading quote and the trailing exe program name.
        int pos = path.findRev( "BehavePlus.exe" );
        path = path.mid( 1, pos-2 );
    }
    RegCloseKey( key );
    return( path );
}

//------------------------------------------------------------------------------
/*! \brief Gets the contents of \a subKey for the \a key from the Windoze
 *  Registry.
 *
 *  The caller must first get HKEY key by passing the key string to
 *  RegOpenKeyExA().  See getWindowsInstallPath() for an example.
 *  This is taken directly from \src\dialogs\qfiledialog.cpp lines 4235-4335.
 *
 *  \return Contents of \a subKey on success, empty string on failure.
 */

QString platformGetWindowsRegString( HKEY key, const char *subKey )
{
    QString s( "" );
    char  buf[512];
    DWORD bsz = sizeof( buf );
    int r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)buf, &bsz );

    if ( r == ERROR_SUCCESS )
    {
        s = buf;
    }
    else if ( r == ERROR_MORE_DATA )
    {
        char *ptr = new char[ bsz + 1 ];
        checkmem( __FILE__, __LINE__, ptr, "char ptr", bsz + 1 );
        r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)ptr, &bsz );
        if ( r == ERROR_SUCCESS )
        {
            s = ptr;
        }
        delete [] ptr;
    }
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief Displays the Program WinHelp
 *
 *  Called only by the AppWindow::slotHelpProgram().
 */

void platformShowHelp( const QString &helpFile )
{
    // Original code using WinHelp system with BehavePlus.hlp file
    // QString helpFile = appFileSystem()->helpFilePath();
    // WinHelpA( NULL, helpFile.latin1(), HELP_CONTENTS, 0 );

    // Registry keys that may contain the Adobe Acrobat Reader exe
    typedef struct
    {
        HKEY        hkey;
        const char *key;
    } RegKey;

    RegKey regKey[] = {
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\AcroExch.Document\\protocol\\StdFileEditing\\server" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\Software\\Adobe\\Acrobat\\Exe" },
        { HKEY_CLASSES_ROOT,  "AcroExch.Document\\protocol\\StdFileEditing\\server" },
        { HKEY_CLASSES_ROOT,  "Software\\Adobe\\Acrobat\\Exe" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\AcroRd32.exe" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Applications\\AcroRd32.exe\\shell\\print\\command" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Applications\\AcroRd32.exe\\shell\\printto\\command" },
        { HKEY_CLASSES_ROOT,  "applications\\AcroRd32.exe\\shell\\print\\command" },
        { HKEY_CLASSES_ROOT,  "applications\\AcroRd32.exe\\shell\\printto\\command" },
    };

    // Try each regKey[] until we find one that has a PDF viewer
    HKEY key;
    bool ok = false;
    for ( int i=0; i<9; i++ )
    {
        // Try to find this registry key...
        if ( RegOpenKeyExA( regKey[i].hkey, regKey[i].key, 0, KEY_READ, &key )
          == ERROR_SUCCESS )
        {
            // Get the value (executable file pathname) for this key ...
            QString prgPath = platformGetWindowsRegString( key, 0 );
            // Start a process for this executable file
            QProcess *proc = new QProcess( qApp );
            proc->addArgument( prgPath );
            proc->addArgument( helpFile );
            if ( proc->launch( "" ) )
            {
                ok = true;
                break;
            }
            delete proc;
        }
        RegCloseKey( key );
    }

    // If we didn't get launch a viewer, let the user know why
    if ( ! ok )
    {
        error( QString( "Couldn't find a PDF viewer for \"%1\"." )
            .arg( helpFile ) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the HTML index file.
 *
 *  Called only by HelpBrowser::showIndex( void ).
 */

void platformShowHelpBrowserIndex( const QString &helpFile )
{
    WinHelpA( NULL, helpFile.latin1(), HELP_CONTENTS, 0 );
    return;
}

//------------------------------------------------------------------------------
//  End of platform-windows.cpp
//------------------------------------------------------------------------------

