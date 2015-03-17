//------------------------------------------------------------------------------
/*! \file globalposition.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Global position C++ source code.
 *
 *  The GlobalPosition class, along with the DateTime class,  provide a C++
 *  wrapper for the Calender-Date-Time Library in cdtlib.c.
 */

// Custom include files
#include "globalposition.h"

// Qt include files
#include <qstring.h>

// Standard include files
#include <math.h>
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalPosition instance with default values.
 *
 *  \arg m_lat = 0 degrees (equator).
 *  \arg m_lon = 0 degrees (Greenwich Meridian).
 *  \arg m_gmt = 0 hours
 */

GlobalPosition::GlobalPosition( void ) :
    m_locationName(""),
    m_zoneName(""),
    m_lat(0.),
    m_lon(0.),
    m_gmt(0.)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GlobalPosition copy constructor.
 *
 *  \param gp Reference to an existing GlobalPosition.
 */

GlobalPosition::GlobalPosition( GlobalPosition &gp ) :
    m_locationName(gp.m_locationName),
    m_zoneName(gp.m_zoneName),
    m_lat(gp.m_lat),
    m_lon(gp.m_lon),
    m_gmt(gp.m_gmt)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalPosition instance with the passed values.
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
 *  \param longitude Longitude in degrees.
 *  \param latitude Latitude in degrees.
 *  \param gmtDiff Local time difference from GMT in hours.
 */

GlobalPosition::GlobalPosition( double longitude, double latitude,
        double gmtDiff ) :
    m_locationName(""),
    m_zoneName(""),
    m_lat(latitude),
    m_lon(longitude),
    m_gmt(gmtDiff)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new GlobalPosition instance with the passed values.
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
 *  \param locationName Geographic place name.
 *  \param zoneName Time zone name.
 *  \param longitude Longitude in degrees.
 *  \param latitude Latitude in degrees.
 *  \param gmtDiff Local time difference from GMT in hours.
 */

GlobalPosition::GlobalPosition(
        const QString &locationName, const QString &zoneName,
        double longitude, double latitude, double gmtDiff ) :
    m_locationName(locationName),
    m_zoneName(zoneName),
    m_lat(latitude),
    m_lon(longitude),
    m_gmt(gmtDiff)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GlobalPosition destructor.
 */

GlobalPosition::~GlobalPosition( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the local time difference from GMT.
 *
 *  \return Local time difference from GMT in hours.
 */

double GlobalPosition::gmtDiff( void ) const
{
    return( m_gmt );
}

//------------------------------------------------------------------------------
/*! \brief Sets the local time difference from GMT.
 *
 *  \param Local time difference from GMT in hours.
 *
 *  \return New local time difference from GMT in hours.
 */

double GlobalPosition::gmtDiff( double hours )
{
    return( m_gmt = hours );
}

//------------------------------------------------------------------------------
/*! \brief Gets the position latitude.
 *
 *  Latitudes \e north of the equator have \e positive values, and
 *  latitudes \e south of the equator have \e negative values.
 *
 *  \return Position latitude in decimal degrees.
 */

double GlobalPosition::latitude( void ) const
{
    return( m_lat );
}

//------------------------------------------------------------------------------
/*! \brief Sets the position latitude.
 *
 *  Latitudes \e north of the equator have \e positive values, and
 *  latitudes \e south of the equator have \e negative values.
 *
 *  \param degrees New latitude in decimal degrees.
 *
 *  \return New position latitude in decimal degrees.
 */

double GlobalPosition::latitude( double degrees )
{
    return( m_lat = degrees );
}

//------------------------------------------------------------------------------
/*! \brief Gets the current location name.
 *
 *  \return Reference to the current location name.
 */

const QString &GlobalPosition::locationName( void ) const
{
    return( m_locationName );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current location name.
 *
 *  \return Reference to the new location name.
 */

QString &GlobalPosition::locationName( const QString &name )
{
    return( m_locationName = name );
}

//------------------------------------------------------------------------------
/*! \brief Gets the position longitude.
 *
 *  Longitudes \e west of Greenwich Meridian have \e positive values, and
 *  longitudes \e east of the Greenwich Meridian have \e negative values.
 *
 *  \return Position longitude in decimal degrees.
 */

double GlobalPosition::longitude( void ) const
{
    return( m_lon );
}

//------------------------------------------------------------------------------
/*! \brief Sets the position longtiude.
 *
 *  Longitudes \e west of Greenwich Meridian have \e positive values, and
 *  longitudes \e east of the Greenwich Meridian have \e negative values.
 *
 *  \param degrees The new longitude in decimal degrees.
 *
 *  \return New position longitude in decimal degrees.
 */

double GlobalPosition::longitude( double degrees )
{
    return( m_lon = degrees );
}

//------------------------------------------------------------------------------
/*! \brief Prints the GlobalPosition member data to the FILE stream.
 *
 *  \param fptr Pointer to an open FILE stream.
 */

void GlobalPosition::print( FILE *fptr ) const
{
    fprintf( fptr,
        "Global position is %s%3.2f, %s%3.2f (GMT + %1.2f)\n",
        ( m_lon >= 0. )
            ? "W"
            : "E",
        fabs( m_lon ),
        ( m_lat >= 0. )
            ? "N"
            : "S",
        fabs( m_lat ),
        m_gmt );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets all the protected data members.
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
 *  \param longitude The new longitude in decimal degrees.
 *  \param latitude The new latitude in decimal degrees.
 *  \param gmtDiff The new local time difference from GMT in  hours.
 */

void GlobalPosition::setPosition( double longitude, double latitude,
    double gmtDiff )
{
    m_lon = longitude;
    m_lat = latitude;
    m_gmt = gmtDiff;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current time zone name.
 *
 *  \return Reference to the current time zone name.
 */

const QString &GlobalPosition::zoneName( void ) const
{
    return( m_zoneName );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current time zone name.
 *
 *  \return Reference to the new zone name.
 */

QString &GlobalPosition::zoneName( const QString &name )
{
    return( m_zoneName = name );
}

//------------------------------------------------------------------------------
/*! \brief GlobalPosition::operator=() assignment opertor.
 *
 *  \brief Assignment operator.
 *
 *  \param gp Reference to an existing GlobalPosition.
 *
 *  \return A reference to \a this object.
 */

GlobalPosition &GlobalPosition::operator=( const GlobalPosition &gp )
{
    m_lon = gp.m_lon;
    m_lat = gp.m_lat;
    m_gmt = gp.m_gmt;
    return( *this );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to convert integral degrees, minutes, and
 *  seconds into decimal degrees.  No sign adjustment is made for east/west or
 *  north/south.
 *
 *  \return Decimal degrees.
 */

double DmsToDeg( int degrees, int minutes, int seconds )
{
    return( (double) degrees + (double) minutes/60. + (double) seconds/3600. );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to convert decimal degrees to integral degrees,
 *  minutes, and seconds.  No sign conversion is made for north/south or
 *  east/west.
 *
 *  \return Decimal degrees.
 */

void DegToDms( double decimal, int *degrees, int *minutes, int *seconds )
{
    decimal  = fabs( decimal );
    *degrees = (int) decimal;
    decimal  = 60. * ( decimal - *degrees );
    *minutes = (int) decimal;
    decimal  = 60. * ( decimal - *minutes );
    *seconds = (int) ( decimal + 0.5 );     // Round to nearest second
    return;
}

//------------------------------------------------------------------------------
//  End of globalposition.cpp
//------------------------------------------------------------------------------

