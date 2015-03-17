//------------------------------------------------------------------------------
/*! \file globalposition.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Global position C++ API header.
 *
 *  The DateTime and GlobalPosition classes provide a C++ wrapper for
 *  the Calender-Date-Time Library in cdtlib.c.
 */

#ifndef _GLOBALPOSITION_H_
/*! \def _GLOBALPOSITION_H_
 *  \brief Prevents redundant includes.
 */
#define _GLOBALPOSITION_H_ 1

#include <qstring.h>
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class GlobalPosition globalposition.h
 *
 *  \brief Defines a position on the globe.
 *  Used along with the DateTime class to get sun/moon times.
 */

class GlobalPosition
{
// Public methods
public:
    GlobalPosition( void ) ;
    GlobalPosition( GlobalPosition &gp ) ;
    GlobalPosition( double longitude, double latitude, double gmtDiff ) ;
    GlobalPosition( const QString &locationName, const QString &zoneName,
        double longitude, double latitude, double gmtDiff ) ;
    GlobalPosition &operator=( const GlobalPosition &dt ) ;
    virtual ~GlobalPosition( void ) ;

    double   gmtDiff( void ) const ;
    double   gmtDiff( double hours ) ;
    double   latitude( void ) const ;
    double   latitude( double degrees ) ;
    const QString &locationName( void ) const ;
    QString &locationName( const QString &name ) ;
    double   longitude( void ) const ;
    double   longitude( double degrees ) ;
    void     print( FILE *fptr ) const ;
    void     setPosition( double longitude, double latitude, double gmtDiff ) ;
    const QString &zoneName( void ) const ;
    QString &zoneName( const QString &name ) ;

// Protected member data
protected:
    /*! \var QString m_locationName
        \brief Optional geographic place name.
    */
    QString m_locationName;
    /*! \var QString m_zoneName
        \brief Optional time zone name.
    */
    QString m_zoneName;
    /*! \var double m_lat
        \brief Latitude in decimal degrees.

        Latitudes \e north of the equator have \e positive values, and
        latitudes \e south of the equator have \e negative values.
    */
    double  m_lat;
    /*! \var double m_lon
        \brief Longitude in decimal degrees.

        Longitudes \e west of Greenwich Meridian have \e positive values, and
        longitudes \e east of the Greenwich Meridian have \e negative values.
    */
    double  m_lon;
    /*! \var m_gmt
        \brief Local time difference from GMT in hours.

        This value is usually determined by the time zone.
        Some examples are:
        \arg EST -5
        \arg EDT -4
        \arg CST -6
        \arg CDT -5
        \arg MST -7
        \arg MDT -6
        \arg PST -8
        \arg PDT -7
 */
    double  m_gmt;
};

//------------------------------------------------------------------------------
//  Convenience routines
//------------------------------------------------------------------------------

double DmsToDeg( int degrees, int minutes, int seconds ) ;

void DegToDms( double decimal, int *degrees, int *minutes, int *seconds ) ;

#endif

//------------------------------------------------------------------------------
//  End of globalposition.h
//------------------------------------------------------------------------------

