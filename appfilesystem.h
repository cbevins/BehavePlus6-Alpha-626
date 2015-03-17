//------------------------------------------------------------------------------
/*! \file appfilesystem.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared FileSystem access.
 */

#ifndef _APPFILESYSTEM_H_
/*! \def _APPFILESYSTEM_H_
    \brief Prevent redundant includes.
 */
#define _APPFILESYSTEM_H_ 1

// Class references
#include "filesystem.h"

// Public functions
void appFileSystemCreate( void ) ;
void appFileSystemDelete( void ) ;
FileSystem *appFileSystem( void ) ;

#endif

//------------------------------------------------------------------------------
//  End of appfilesystem.h
//------------------------------------------------------------------------------

