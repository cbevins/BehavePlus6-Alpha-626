//------------------------------------------------------------------------------
/*! \file appsiunits.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared SIUnits access.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"

// Static SIUnits instance.
static SIUnits *AppSiUnits = 0;

//------------------------------------------------------------------------------
/*! \brief Access to the shared, application-wide SIUnits.
 *
 *  \retunr Pointer to the shared, application-wide SIUnits.
 */

SIUnits *appSiUnits( void )
{
    return( AppSiUnits );
}

//------------------------------------------------------------------------------
/*! \brief Creates the SIUnits converter.
 */

void appSiUnitsCreate( void )
{
    AppSiUnits = new SIUnits();
    checkmem( __FILE__, __LINE__, AppSiUnits, "SIUnits AppSiUnits", 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Deletes the SIUnits converter.
 */

void appSiUnitsDelete( void )
{
    delete AppSiUnits;   AppSiUnits = 0;
    return;
}

//------------------------------------------------------------------------------
// End of appsiunits.cpp
//------------------------------------------------------------------------------

