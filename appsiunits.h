//------------------------------------------------------------------------------
/*! \file appsiunits.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared SIUnits access.
 */

#ifndef _APPSIUNITS_H_
/*! \def _APPSIUNITS_H_
 *  \brief Prevent redundant includes.
 */
#define _APPSIUNITS_H_ 1

// Class references
#include "siunits.h"

// Function declarations
SIUnits *appSiUnits( void ) ;
void appSiUnitsCreate( void ) ;
void appSiUnitsDelete( void ) ;

#endif

//------------------------------------------------------------------------------
// End of appsiunits.h
//------------------------------------------------------------------------------

