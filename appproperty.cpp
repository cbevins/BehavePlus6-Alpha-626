//------------------------------------------------------------------------------
/*! \file appproperty.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared Property access.
 */

// Custom include files
#include "appmessage.h"
#include "appproperty.h"

// Static, shared PropertyDict
static PropertyDict *AppProperty = 0;

//------------------------------------------------------------------------------
/*! \brief Access to the shared, application-wide PropertyDict.
 */

PropertyDict *appProperty( void )
{
    return( AppProperty );
}

//------------------------------------------------------------------------------
/*! \brief Creates the shared, application-wide PropertyDict.
 */

void appPropertyCreate( int primeNumber )
{
    AppProperty = new PropertyDict( primeNumber, true );
    checkmem( __FILE__, __LINE__, AppProperty, "PropertyDict AppProperty", 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Deletes the shared, application-wide PropertyDict.
 */

void appPropertyDelete( void )
{
    delete AppProperty;    AppProperty = 0;
    return;
}

//------------------------------------------------------------------------------
// End of appproperty.cpp
//------------------------------------------------------------------------------

