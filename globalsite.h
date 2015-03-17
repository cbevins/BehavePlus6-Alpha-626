//------------------------------------------------------------------------------
/*! \file globalsite.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Global site C++ API header.
 *
 *  Combines the data elements of a GlobalPosition and local site attributes
 *  of elevation, slope, and aspect.
 */

#ifndef _GLOBALSITE_H_
/*! \def _GLOBALSITE_H_
 *  \brief Prevents redundant incudes.
 */
#define _GLOBALSITE_H_ 1

#include "globalposition.h"

//------------------------------------------------------------------------------
/*! \class GlobalSite globalsite.h
 *
 *  \brief Defines a position on the globe with site parameters.
 *  Used along with the DateTime class to get sun/moon times.
 */

class GlobalSite : public GlobalPosition
{
public:
    GlobalSite( void );
    GlobalSite( GlobalSite &gs );
    GlobalSite( double longitude, double latitude, double gmtDiff,
        double elevation, double slope, double aspect );
    GlobalSite( const QString &locationName, const QString &zoneName,
        double longitude, double latitude, double gmtDiff,
        double elevation, double slope, double aspect ) ;
    GlobalSite &operator=( const GlobalSite &gs ) ;
    virtual ~GlobalSite( void ) ;

    double  aspect( void ) const ;
    double  aspect( double degrees ) ;
    double  elevation( void ) const ;
    double  elevation( double feet ) ;
    double  slope( void ) const ;
    double  slope( double degrees ) ;

    void    print( FILE *fptr ) const ;
    void    setSite( double elevation, double slope, double aspect ) ;

// Private member data
protected:
    /*! \var double m_elev
        \brief Site's elevation above mean sea level in feet.
    */
    double m_elev;
    /*! \var double m_slope
        \brief Site's terrain slope steepness in decimal degrees.
        */
    double m_slope;
    /*! \var double m_aspect
        \brief Site's terrain aspect (down-slope direction) in decimal degrees
        clockwise from north (0-360).
    */
    double m_aspect;
};

#endif

//------------------------------------------------------------------------------
//  End of globalsite.h
//------------------------------------------------------------------------------

