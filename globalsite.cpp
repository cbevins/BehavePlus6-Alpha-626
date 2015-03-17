//------------------------------------------------------------------------------
/*! \file globalsite.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Global site C++ source code.
 *
 *  Combines the data elements of a GlobalPosition and local site attributes
 *  of elevation, slope, and aspect.
 */

// Custom include files
#include "globalsite.h"

// Standard include files
#include <math.h>
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalSite instance with default values.
 *
 *  \arg m_elev = 0 feet (mean sea level).
 *  \arg m_slope = 0 degrees (flat).
 *  \arg m_aspect = 0 degrees (flat).
 */

GlobalSite::GlobalSite( void ) :
    GlobalPosition( 0., 0., 0. ),
    m_elev(0.),
    m_slope(0.),
    m_aspect(0.)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalSite instance with the passed values.
 *
 *  Latitudes \e north of the equator have \e positive values, and
 *  latitudes \e south of the equator have \e negative values.
 *
 *  Longitudes \e west of Greenwich Meridian have \e positive values, and
 *  longitudes \e east of the Greenwich Meridian have \e negative values.
 *
 *  The \a gmtDiff is the number of hours added to GMT to obtain the local
 *  time ( LocalTime = gmtDiff + GMT ).
 *
 *  The site's terrain aspect is the down-slope direction in decimal degrees
 *  clockwise from north (0-360).
 *
 *  \param longitude Longitude in degrees.
 *  \param latitude Latitude in degrees.
 *  \param gmtDiff Local time difference from GMT in hours.
 *  \param elevation Local site elevation in feet.
 *  \param slope Local site slope steepness in decimal degrees.
 *  \param aspect Local site aspect in decimal degrees clockwise from north.
 */

GlobalSite::GlobalSite( double longitude, double latitude, double gmtDiff,
        double elevation, double slope, double aspect ) :
    GlobalPosition( longitude, latitude, gmtDiff ),
    m_elev(elevation),
    m_slope(slope),
    m_aspect(aspect)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalSite instance with the passed values.
 *
 *  Latitudes \e north of the equator have \e positive values, and
 *  latitudes \e south of the equator have \e negative values.
 *
 *  Longitudes \e west of Greenwich Meridian have \e positive values, and
 *  longitudes \e east of the Greenwich Meridian have \e negative values.
 *
 *  The \a gmtDiff is the number of hours added to GMT to obtain the local
 *  time ( LocalTime = gmtDiff + GMT ).
 *
 *  The site's terrain aspect is the down-slope direction in decimal degrees
 *  clockwise from north (0-360).
 *
 *  \param locationName Geographic place name.
 *  \param zoneName Time zone name.
 *  \param longitude Longitude in degrees.
 *  \param latitude Latitude in degrees.
 *  \param gmtDiff Local time difference from GMT in hours.
 *  \param elevation Local site elevation in feet.
 *  \param slope Local site slope steepness in decimal degrees.
 *  \param aspect Local site aspect in decimal degrees clockwise from north.
 */

GlobalSite::GlobalSite( const QString &locationName, const QString &zoneName,
        double longitude, double latitude, double gmtDiff,
        double elevation, double slope, double aspect ) :
    GlobalPosition( locationName, zoneName, longitude, latitude, gmtDiff ),
    m_elev(elevation),
    m_slope(slope),
    m_aspect(aspect)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GlobalSIte copy constructor.
 *
 *  \param gs Reference to an existing GlobalSite object.
 */

GlobalSite::GlobalSite( GlobalSite &gs ) :
    GlobalPosition( gs.m_lon, gs.m_lat, gs.m_gmt ),
    m_elev(gs.m_elev),
    m_slope(gs.m_slope),
    m_aspect(gs.m_aspect)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GlobalSite destructor.
 */

GlobalSite::~GlobalSite( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the local site aspect.
 *
 *  \return Local site aspect in decimal degrees clockwise from north.
 */

double GlobalSite::aspect( void ) const
{
    return( m_aspect );
}

//------------------------------------------------------------------------------
/*! \brief Sets the local site aspect.
 *
 *  \param degrees Local site aspect in decimal degrees clockwise from north.
 *
 *  \return New local site aspect in decimal degrees clockwise from north.
 */

double GlobalSite::aspect( double degrees )
{
    return( m_aspect = degrees );
}

//------------------------------------------------------------------------------
/*! \brief Gets the local site elevation.
 *
 *  \return Local site elevation in feet above mean sea level.
 */

double GlobalSite::elevation( void ) const
{
    return( m_elev );
}

//------------------------------------------------------------------------------
/*! \brief Sets the local site mean elevation.
 *
 *  \param feet Local site elevation in feet above mean sea level.
 *
 *  \return New local site elevation in feet above mean sea level.
 */

double GlobalSite::elevation ( double feet )
{
    return( m_elev = feet );
}

//------------------------------------------------------------------------------
/*! \brief Gets the local site slope steepness.
 *
 *  \return Local site slope steepness in decimal degrees.
 */

double GlobalSite::slope( void ) const
{
    return( m_slope );
}

//------------------------------------------------------------------------------
/*! \brief Sets the local site slope steepness.
 *
 *  \param degrees New local site slope steepness in decimal degrees.
 *
 *  \return New locla site slope steepness in decimal degrees.
 */

double GlobalSite::slope ( double degrees )
{
    return( m_slope = degrees );
}

//------------------------------------------------------------------------------
/*! \brief Prints the GlobalSite member data to the FILE stream.
 *
 *  \param fptr Pointer to an open FILE stream.
 */

void GlobalSite::print( FILE *fptr ) const
{
    fprintf( fptr,
        "Location is %s%3.2f, %s%3.2f (GMT + %1.2f hours)\n"
        "Site is at %2.1f feet with %2.1f degree slope and %1.0f degree aspect.\n",
        ( m_lon >= 0. )
            ? "West "
            : "East ",
        fabs( m_lon ),
        ( m_lat >= 0. )
            ? "North "
            : "South ",
        fabs( m_lat ),
        m_gmt, m_elev,
        m_slope,
        m_aspect );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets all the member data to the passed values.
 *
 *  \param feet Local site elevation in feet above mean sea level.
 *  \param slope Local site slope steepness in decimal degrees.
 *  \param aspect Local site aspect in decimal degrees clockwise from north.
 */

void GlobalSite::setSite( double elevation, double slope, double aspect )
{
    m_elev = elevation;
    m_slope = slope;
    m_aspect = aspect;
    return;
}

//------------------------------------------------------------------------------
/*! \brief GlobalSite operator=() assignment operator.
 *
 *  \param gp Reference to an existing GlobalSite.
 *
 *  \return A reference to \a this object.
 */

GlobalSite &GlobalSite::operator=( const GlobalSite &gs )
{
    m_lon    = gs.m_lon;
    m_lat    = gs.m_lat;
    m_gmt    = gs.m_gmt;
    m_elev   = gs.m_elev;
    m_slope  = gs.m_slope;
    m_aspect = gs.m_aspect;
    return( *this );
}

//------------------------------------------------------------------------------
//  End of globalsite.cpp
//------------------------------------------------------------------------------

