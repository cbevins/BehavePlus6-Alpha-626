//------------------------------------------------------------------------------
/*! \file sunview.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief SunView C++ API header.
 *
 *  Sun position, angle, and solar radiation class.  SunView inherits from
 *  both the GlobalSite and the DateTime classes.
 *
 *  \note This code is not yet part of the BehavePlus package.
 */

#ifndef _SUNVIEW_H_
/*! \def _SUNVIEW_H_
 *  \brief Prevents redundant incudes.
 */
#define _SUNVIEW_H_ 1

// Class references
#include "datetime.h"
#include "globalsite.h"

//------------------------------------------------------------------------------
/*! \class SunView sunview.h
 */

class SunView : public GlobalSite, public DateTime
{
public:
    SunView( void ) ;
    SunView( double latitude, double longitude, double gmtDiff,
                double elevation, double slope, double aspect,
                int year, int month, int day,
                int hour, int minute, int second, int millisecond ) ;
    SunView( const QString &locationName, const QString &zoneName,
                double latitude, double longitude, double gmtDiff,
                double elevation, double slope, double aspect,
                int year, int month, int day,
                int hour, int minute, int second, int millisecond ) ;
    SunView( GlobalSite &gs, DateTime &dt );
    SunView( SunView &sv ) ;
    SunView &operator=( const SunView &sv ) ;
    virtual ~SunView( void );

    void    print( FILE *fptr ) const ;
    double  radiationFraction( double atmTransparency,
                double cloudTransmittance, double canopyTransmittance ) ;
    double  solarAngle( void ) ;
    void    sunPosition( void ) ;

// Public data members
public:
    /*! \var double m_altitude
        \brief Sun altitude in decimal degrees above the horizon
        (negative if below the horizon).
    */
    double m_altitude;
    /*! \var double m_azimuth
        \brief Sun azimuth in decimal degrees clockwise from  north.
    */
    double m_azimuth;
    /*! \var double m_angle
        \brief Sun angle to the local slope in decimal degrees.
    */
    double m_angle;
    /*! \var m_radiation
        \brief Fraction of the solar constant that reaches the site floor.
    */
    double m_radiation;
};

#endif

//------------------------------------------------------------------------------
//  End of sunview.h
//------------------------------------------------------------------------------

