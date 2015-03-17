//------------------------------------------------------------------------------
/*! \file sunview.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief SunView C++ source code.
 *
 *  Sun position, angle, and solar radiation class.  SunView inherits from
 *  both the GlobalSite and the DateTime classes.
 *
 *  \note This code is not yet part of the BehavePlus package.
 */

// Custom include files
#include "cdtlib.h"
#include "sunview.h"

//------------------------------------------------------------------------------
/*! \brief SunView default constructor.
 *
 *  The assigned values include the current syste time and:
 *  \arg m_lat = 0 degrees (equator).
 *  \arg m_lon = 0 degrees (Greenwich Meridian).
 *  \arg m_gmt = 0 hours
 *  \arg m_elev = 0 feet (mean sea level).
 *  \arg m_slope = 0 degrees (flat).
 *  \arg m_aspect = 0 degrees (flat).
 */

SunView::SunView( void ) :
    GlobalSite(),
    DateTime(),
    m_altitude(0.),
    m_azimuth(0.),
    m_angle(0.),
    m_radiation(0.)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new SunView instance with the passed values.
 *
 *  \param gs Reference to an existing GlobalSite instance.
 *  \param dt Reference to an existing DateTime instance.
 *
 *  \return This function returns nothing.
 */

SunView::SunView( GlobalSite &gs, DateTime &dt ) :
    GlobalSite( gs ),
    DateTime( dt )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new SunView from the passed values.
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
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param month Month of the year (1-12).
 *  \param day Day of the month (1-31).
 *  \param hour Hours past midnight (0-23).
 *  \param minute Minutes past the hour (0-59).
 *  \param second Seconds past the minute (0-59).
 *  \param milliseconds Milliseconds past the second (0-999).
 */

SunView::SunView( double latitude, double longitude, double gmtDiff,
        double elevation, double slope, double aspect,
        int year, int month, int day, int hour, int minute, int second,
        int millisecond ) :
    GlobalSite( latitude, longitude, gmtDiff, elevation, slope, aspect ),
    DateTime( year, month, day, hour, minute, second, millisecond ),
    m_altitude(0.),
    m_azimuth(0.),
    m_angle(0.),
    m_radiation(0.)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new SunView from the passed values.
 *
 *  Latitude's \e north of the equator have \e positive values, and
 *  latitude's \e south of the equator have \e negative values.
 *
 *  Longitude's \e west of Greenwich Meridian have \e positive values, and
 *  longitude's \e east of the Greenwich Meridian have \e negative values.
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
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param month Month of the year (1-12).
 *  \param day Day of the month (1-31).
 *  \param hour Hours past midnight (0-23).
 *  \param minute Minutes past the hour (0-59).
 *  \param second Seconds past the minute (0-59).
 *  \param milliseconds Milliseconds past the second (0-999).
 */

SunView::SunView( const QString &locationName, const QString &zoneName,
        double latitude, double longitude, double gmtDiff,
        double elevation, double slope, double aspect,
        int year, int month, int day, int hour, int minute, int second,
        int millisecond ) :
    GlobalSite( locationName, zoneName, latitude, longitude, gmtDiff,
        elevation, slope, aspect ),
    DateTime( year, month, day, hour, minute, second, millisecond ),
    m_altitude(0.),
    m_azimuth(0.),
    m_angle(0.),
    m_radiation(0.)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief SunView copy constructor.
 *
 *  \param sv Reference to an existing SunView object.
 */

SunView::SunView( SunView &sv ) :
    GlobalSite( sv.m_locationName, sv.m_zoneName,
        sv.m_lon, sv.m_lat, sv.m_gmt,
        sv.m_elev, sv.m_slope, sv.m_aspect ),
    DateTime( sv.m_year, sv.m_month, sv.m_day,
        sv.m_hour, sv.m_minute, sv.m_second, sv.m_millisecond ),
        m_altitude(sv.m_altitude),
        m_azimuth(sv.m_azimuth),
        m_angle(sv.m_angle),
        m_radiation(sv.m_radiation)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief SunView destructor.
 */

SunView::~SunView( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the SunView member data to the FILE stream.
 *
 *  \param fptr Pointer to an open FILE stream.
 */

void SunView::print( FILE *fptr ) const
{
    DateTime::print( fptr );
    GlobalSite::print( fptr );
    fprintf( fptr,
        "Sun is at %3.1f deg altitude and %3.1f deg azimuth with a "
        "%3.1f deg angle to the terrain.\n",
        m_altitude, m_azimuth, m_angle );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the proportion [0..1] of the solar radiation constant
 *  arriving at the forest floor given:
 *
 *  Uses the algorithm from MTCLIM.
 *
 *  \param canopyTransmittance The canopy transmittance factor [0..1].
 *  \param canopyTransmittance The cloud transmittance factor [0..1].
 *  \param atmTransparency The atmospheric transparency coefficient ([0.6-0.8])
 *  \arg 0.80 Exceptionally clear atmosphere
 *  \arg 0.75 Average clear forest atmosphere
 *  \arg 0.70 Moderate forest (blue) haze
 *  \arg 0.60 Dense haze
 *
 *  \bug Does not account for reflected or diffuse radiation.  Therefore, a
 *  site will have zero radiation if any of the following are true:
 *  \arg the sun is below the horizon,
 *  \arg the slope is self-shaded,
 *  \arg the cloud transmittance is zero, or
 *  \arg the canopy transmittance is zero.
 *
 *  \return The data member m_radiation si updated on return.  The function
 *  itself returns nothing.
 *
 *  \sa CDT_SolarRadiation(), CDT_SunPosition(), CDT_SolarAngle().
 */

double SunView::radiationFraction( double atmTransparency,
    double cloudTransmittance, double canopyTransmittance )
{
    return( m_radiation =
        CDT_SolarRadiation( m_jdate, m_lon, m_lat, m_gmt,
            m_slope, m_aspect, m_elev,
            atmTransparency, cloudTransmittance, canopyTransmittance ) ) ;
}

//------------------------------------------------------------------------------
/*! \brief Determines the solar angle to the terrain slope given the current
 *  state of the SunView object's GlobalPosition and DateTime.
 *
 *  \return The data member m_angle is updated on return.  The function itself
 *  returns nothing.
 *
 *  \sa CDT_SolarAngle(), CDT_SunPosition().
 */

double SunView::solarAngle( void )
{
    return( ( m_angle = CDT_SolarAngle( m_slope, m_aspect, m_altitude,
        m_azimuth ) ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the position of the sun in the sky given the current
 *  state of the SunView GlobalPosition and DateTime.
 *
 *  \return The m_altitude and m_azimuth data members are updated on return.
 *  The function itself returns nothing.
 *
 *  \sa CDT_SunPosition(), CDT_SolarAngle().
 */

void SunView::sunPosition( void )
{
    CDT_SunPosition( m_jdate, m_lon, m_lat, m_gmt, &m_altitude, &m_azimuth );
    return;
}

//------------------------------------------------------------------------------
/*! \brief SunView assignment operator=( const SunView &sv )
 *
 *  \param sv Reference to an existing SunView.
 *
 *  \return A reference to \a this object.
 */

SunView &SunView::operator=( const SunView &sv )
{
    m_lon           = sv.m_lon;
    m_lat           = sv.m_lat;
    m_gmt           = sv.m_gmt;
    m_elev          = sv.m_elev;
    m_slope         = sv.m_slope;
    m_aspect        = sv.m_aspect;
    m_jdate         = sv.m_jdate;
    m_year          = sv.m_year;
    m_month         = sv.m_month;
    m_day           = sv.m_day;
    m_hour          = sv.m_hour;
    m_minute        = sv.m_minute;
    m_second        = sv.m_second;
    m_millisecond   = sv.m_millisecond;
    m_event         = sv.m_event;
    m_flag          = sv.m_flag;
    return( *this );
}

//------------------------------------------------------------------------------
//  End of sunview.cpp
//------------------------------------------------------------------------------

