//------------------------------------------------------------------------------
/*! \file appproperty.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared Property access.
 */

#ifndef _APPPROPERTY_H_
/*! \def _APPPROPERTY_H_
 *  \brief Prevent redundant includes.
 */
#define _APPPROPERTY_H_ 1

// Qt class references
#include "property.h"

// Property dictionary construction and destruction.
void appPropertyCreate( int primeNumber ) ;
void appPropertyDelete( void ) ;
PropertyDict *appProperty( void ) ;

#endif

//------------------------------------------------------------------------------
// End of appproperty.h
//------------------------------------------------------------------------------

