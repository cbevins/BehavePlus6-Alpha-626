//------------------------------------------------------------------------------
/*! \file platform.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Platform-neutral declarations of platform-specific functions.
 */

#ifndef _PLATFORM_H_
/*! \def _PLATFORM_H_
 *  \brief Prevent redundant includes.
 */
#define _PLATFORM_H_ 1

// Qt include files
#include <qstring.h>

void    platformAbort( void ) ;
void    platformExit( int status ) ;
char   *platformGetCwd( char *buffer, size_t size ) ;
char   *platformGetExePath( bool withProgramName=true ) ;
QString platformGetOs( void ) ;
int     platformGetPid( void ) ;
QString platformGetWindowsInstallPath( void ) ;
void    platformShowHelp( const QString &helpFile ) ;
void    platformShowHelpBrowserIndex( const QString &helpFile ) ;

#endif

//------------------------------------------------------------------------------
// End of platform.h
//------------------------------------------------------------------------------

