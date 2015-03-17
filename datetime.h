//------------------------------------------------------------------------------
/*! \file datetime.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief C++ API wrapper for calendar, date, and time functions.
 *
 *  The DateTime and GlobalPosition classes provide a C++ wrapper for
 *  the Calender-Date-Time Library in cdtlib.c.
 */

#ifndef _DATETIME_H_
/*! \def _DATETIME_H_
    \brief Prevents redundant incudes.
*/
#define _DATETIME_H_ 1

// Forward class references
class GlobalPosition;
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class DateTime datetime.h
 *
 *  \brief Calendar, date, and time routines for the Western (Julian-Gregorian)
 *  calendar.
 *
 *  The DateTime class is a C++ wrapper for the calendar, date, and time
 *  functions in the CDT library (cdtlib.c).
 *
 *  \sa cdtlib.c
 */

class DateTime
{
// Public constructor methods
public:
    DateTime( void ) ;
    DateTime( int year, int month=1, int day=1, int hour=0, int minute=0,
                int second=0, int millisecond=0 ) ;
    DateTime( DateTime &dt ) ;
    DateTime& operator=( const DateTime &dt ) ;

// Public methods to access or update data members
    int         day( void ) const ;
    int         day( int newDay ) ;
    int         dayOfWeek( void ) const ;
    const char *dayOfWeekAbbreviation( void ) const ;
    const char *dayOfWeekName( void ) const ;
    int         dayOfYear( void ) const ;
    int         daysInMonth( void ) const ;
    int         daysInYear( void ) const ;
    double      decimalDay( void ) const ;
    double      decimalHour( void ) const ;
    int         event( void ) const ;
    const char *eventName( void ) const ;
    int         flag( void ) const ;
    const char *flagName ( void ) const ;
    int         hour( void ) const ;
    int         hour( int newHour ) ;
    int         isLeapYear( void ) const ;
    bool        isValid( void ) ;
    bool        isValidDate( void ) ;
    bool        isValidTime( void ) ;
    double      julianDate( void ) const ;
    int         millisecond( void ) const ;
    int         millisecond( int newMillisecond ) ;
    int         millisecondOfDay( void ) const ;
    int         minute( void ) const ;
    int         minute( int newMinute ) ;
    double      modifiedJulianDate( void ) const ;
    int         month( void ) const ;
    int         month( int newMonth) ;
    const char *monthAbbreviation( void ) const ;
    const char *monthName( void ) const ;
    void        print( FILE *fptr ) const ;
    int         second( void ) const ;
    int         second( int newSecond ) ;
    bool        set( double julianDate ) ;
    bool        set( int year, int month=1, int day=1, int hour=0, int minute=0,
                    int second=0, int millisecond=0 ) ;
    bool        setSystem( void ) ;
    bool        setTime( int hour=0, int minute=0, int second=0,
                    int millisecond=0 ) ;
    int         year( void ) const ;
    int         year( int newYear ) ;

// Public methods that perform date and time arithmetic
    bool        addDays( double days ) ;
    bool        addHours( double hours ) ;
    bool        addMinutes( double minutes ) ;
    bool        addSeconds( double seconds ) ;
    bool        addMilliseconds( int milliseconds ) ;
    double      daysSince( const DateTime &dt ) const ;
    double      daysUntil( const DateTime &dt ) const ;
    double      hoursSince( const DateTime &dt ) const ;
    double      hoursUntil( const DateTime &dt ) const ;

// Public methods to determine times of daily events
    bool        astronomicalDawn( const GlobalPosition &gp ) ;
    bool        astronomicalDusk( const GlobalPosition &gp ) ;
    bool        civilDawn( const GlobalPosition &gp ) ;
    bool        civilDusk( const GlobalPosition &gp ) ;
    bool        moonRise( const GlobalPosition &gp ) ;
    bool        moonSet( const GlobalPosition &gp ) ;
    bool        nauticalDawn( const GlobalPosition &gp ) ;
    bool        nauticalDusk( const GlobalPosition &gp ) ;
    bool        sunRise( const GlobalPosition &gp ) ;
    bool        sunSet( const GlobalPosition &gp ) ;

// Public methods to determine seasonal or annual events
    bool        easter( void ) ;
    bool        easter( int year ) ;
    bool        fallEquinox( int year, const GlobalPosition &gp ) ;
    bool        fallEquinox( const GlobalPosition &gp ) ;
    bool        fullMoon( int lunation, const GlobalPosition &gp ) ;
    bool        newMoon( int lunation, const GlobalPosition &gp ) ;
    bool        newMoon( int year, int period, const GlobalPosition &gp ) ;
    bool        springEquinox( int year, const GlobalPosition &gp ) ;
    bool        springEquinox( const GlobalPosition &gp ) ;
    bool        summerSolstice( int year, const GlobalPosition &gp) ;
    bool        summerSolstice( const GlobalPosition &gp ) ;
    bool        winterSolstice( int year, const GlobalPosition &gp ) ;
    bool        winterSolstice( const GlobalPosition &gp ) ;

    bool        checkCalendarDate( void ) { return( calculateCalendarDate() ); }

// Private methods
private:
    bool        calculateCalendarDate( void ) ;
    double      calculateJulianDate( void ) ;
    bool        calculateSolstice( int i, const GlobalPosition &gp ) ;
    bool        calculateSunTime( int event, const GlobalPosition &gp ) ;

//  Protected data members
protected:
    /*! \var double m_jdate
        \brief Julian date (decimal days since noon of Jan 1, -4712).
    */
    double  m_jdate;
    /*! \var m_year
        \brief Julian-Gregorian calendar year (-4712 or later).
    */
    int     m_year;
    /*! \var int m_month
        \brief Month of the year (1=Jan, 12=Dec).
    */
    int     m_month;
    /*! \var int m_day
        \brief Day of the month (1-31).
    */
    int     m_day;
    /*! \var int m_hour
        \brief Hour of the day, e.g. elapsed hours since midnight (0-23).
    */
    int     m_hour;
    /*! \var int m_minute
        \brief Minute of the hour, e.g. elapsed minutes since the hour (0-59).
    */
    int     m_minute;
    /*! \var int m_second
        \brief Second of the minute, e.g. elapsed seconds since the minute (0-59).
    */
    int     m_second;
    /*! \var m_millisecond
        \brief Millisecond of the second (0-999).
    */
    int     m_millisecond;
    /*! \var int m_event
        \brief #CDT_Event enumeration value of the last DateTime operation.
    */
    int     m_event;
    /*! \var int m_flag
        \brief #CDT_Flag enumeration value of the result of the last DateTime
        operation.
    */
    int     m_flag;
};

#endif

//------------------------------------------------------------------------------
//  End of datetime.h
//------------------------------------------------------------------------------

