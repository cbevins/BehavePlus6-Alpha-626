//------------------------------------------------------------------------------
/*! \file appfilesystem.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared FileSystem access.
 */

// Custom include files
#include "filesystem.h"

// Shared, application-wide FileSystem object.
static FileSystem *AppFileSystem = 0;

//------------------------------------------------------------------------------
/*! \brief Access to the shared, application-wide FileSystem.
 *
 *  \retunr Pointer to the shared, application-wide FileSystem.
 */

FileSystem *appFileSystem( void )
{
    return( AppFileSystem );
}

//------------------------------------------------------------------------------
/*! \brief Creates the shared, application-wide FileSystem.
 */

void appFileSystemCreate( void )
{
    AppFileSystem = new FileSystem();
    Q_CHECK_PTR( AppFileSystem );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Destroys the shared, application-wide FileSystem.
 */

void appFileSystemDelete( void )
{
    delete AppFileSystem;   AppFileSystem = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of appfilesystem.cpp
//------------------------------------------------------------------------------

