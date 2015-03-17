//------------------------------------------------------------------------------
/*! \file datetime.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief C++ wrapper for calendar, date, and time functions.
 *
 *  The DateTime and GlobalPosition classes provide a C++ wrapper for
 *  the Calender-Date-Time Library in cdtlib.c.
 */

// Custom include files
#include "cdtlib.h"
#include "datetime.h"
#include "globalposition.h"

// Standard include files
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/*! \brief Constructs a new DateTime using the current system date and time.
 *
 *  \sa DateTime(int year,int month,int day,int hour,int minute,int second,int millisecond)
 */

DateTime::DateTime()
{
    // Set to the current system date and time
    setSystem();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Constructs a new DateTime using the passed values.
 *
 *  \param year Julian-Gregorian year (-4712 or later).
 *  \param month Month of the year (1=Jan, 12=Dec).
 *  \param day Day of the month (1-31).
 *  \param hour Hour of the day (0-23).
 *  \param minute Minute of the hour (0-59).
 *  \param second Second of the minute (0-59).
 *  \param millisecond Millisecond of the second (0-999).
 *
 *  \sa DateTime()
 */

DateTime::DateTime( int year, int month, int day, int hour, int minute,
            int second, int millisecond ) :
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(hour),
    m_minute(minute),
    m_second(second),
    m_millisecond(millisecond),
    m_event(CDT_User),
    m_flag(CDT_None)
{
    // Update
    calculateJulianDate();
    isValid();      // At least set the flag even tho we can't return it
    return;
}

//------------------------------------------------------------------------------
/*! \brief DateTime copy constructor.
 *
 *  \param dt Reference to an existing DateTime object.
 */

DateTime::DateTime( DateTime &dt ) :
    m_jdate(dt.m_jdate),
    m_year(dt.m_year),
    m_month(dt.m_month),
    m_day(dt.m_day),
    m_hour(dt.m_hour),
    m_minute(dt.m_minute),
    m_second(dt.m_second),
    m_millisecond(dt.m_millisecond),
    m_event(dt.m_event),
    m_flag(dt.m_flag)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds some number of decimal days to the current DateTime value.
 *
 *  \param days Number of decimal days to add;
 *   may be positive, zero, or negative.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa addHours(), addMinutes(), addSeconds(), addMilliseconds().
 */

bool DateTime::addDays( double days )
{
    m_jdate += days;
    // Update
    m_event = CDT_User;
    calculateJulianDate();
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Adds some number of decimal hours to the current DateTime value.
 *
 *  \param hours Number of decimal hours to add;
 *   may be positive, zero, or negative.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa addDays(), addMinutes(), addSeconds(), addMilliseconds().
 */

bool DateTime::addHours( double hours )
{
    return( addDays( hours / 24. ) );
}

//------------------------------------------------------------------------------
/*! \brief Adds some number of decimal hours to the current DateTime value.
 *
 *  \param minutes Number of decimal minutes to add;
 *   may be positive, zero, or negative.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa addDays(), addHours(), addSeconds(), addMilliseconds().
 */

bool DateTime::addMinutes( double minutes )
{
    return( addDays( minutes / 1440. ) );
}

//------------------------------------------------------------------------------
/*! \brief Adds some number of decimal seconds to the current DateTime value.
 *
 *  \param secondsa Number of decimal seconds to add;
 *   may be positive, zero, or negative.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa addDays(), addHours(), addMinutes(), addMilliseconds().
 */

bool DateTime::addSeconds( double minutes )
{
    return( addDays( minutes / 86400. ) );
}

//------------------------------------------------------------------------------
/*! \brief Adds some number of whole milliseconds to the current DateTime value.
 *
 *  \param milliseconds Number of whole milliseconds to add;
 *   may be positive, zero, or negative.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa addDays(), addHours(), addMinutes(), addSeconds(), MillisecondsOfDay().
 */

bool DateTime::addMilliseconds( int milliseconds )
{
    return( addDays( (double) milliseconds / 86400000. ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of astronomical dawn for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The astronomical dawn occurs when the rising sun reaches 18 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() is #CDT_Rises,
 *  then astronomical dawn occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dawn doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dawn doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_AstronomicalDawn.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDusk(), civilDawn(), civilDusk(), nauticalDawn(),
 *  nauticalDusk().
 */

bool DateTime::astronomicalDawn( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_AstronomicalDawn, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of astronomical dusk for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The astronomical dusk occurs when the setting sun reaches 18 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() is #CDT_Sets,
 *  then astronomical dusk occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dusk doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dusk doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_AstronomicalDusk.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDawn(), civilDawn(), civilDusk(), nauticalDawn(),
 *  nauticalDusk().
 */

bool DateTime::astronomicalDusk( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_AstronomicalDusk, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the calendar date and time from the current DateTime
 *  \a m_jdate Julian date.
 *
 *  The algorithms of Duffett-Smith and Meeus are used as described for
 *  CDT_CalendarDate().
 *
 *  \warning No date or time validation is performed before the calendar
 *  date and time is calculated.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  \sa CDT_CalendarDate().
 */

bool DateTime::calculateCalendarDate( void )
{
    CDT_CalendarDate( m_jdate, &m_year, &m_month, &m_day,
        &m_hour, &m_minute, &m_second, &m_millisecond );
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the Julian date from the DateTime current date and
 *  time data members.
 *
 *  Calls CDT_JulianDate() to perform the actual computations.
 *
 *  \return The Julian date in decimal days since 1 Jan -4712.  On return the
 *  #m_jdate data member is updated.
 *
 *  \sa CDT_JulianDate().
 */

double DateTime::calculateJulianDate( void )
{
    m_jdate = CDT_JulianDate( m_year, m_month, m_day,
        m_hour, m_minute, m_second, m_millisecond );
    return( m_jdate );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the calendar date-time of an equinox or solstice
 *  #CDT_Event for the current DateTime year.
 *
 *  Calls #CDT_SolsticeGMT() to perform the computation.
 *
 *  \param event One of the #CDT_Event enumeration values:
 *  \arg #CDT_Spring for the spring (March) equinox date and time,
 *  \arg #CDT_Summer for the summer (June) solstice date and time,
 *  \arg #CDT_Fall for the fall (September) equinox date and time, or
 *  \arg #CDT_Winter for the winter (December) solstice date and time.
 *  \param gp Reference to an existing GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::calculateSolstice( int event, const GlobalPosition &gp )
{
    // Determine GMT Julian date of the event
    m_jdate = CDT_SolsticeGMT( event, m_year );

    // Add time zone difference from GMT
    m_jdate += (gp.gmtDiff() / 24.);

    // Update (m_flag must be set by the caller)
    calculateCalendarDate();
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief calculates the rise or set times for a GlobalPosition for the
 *  current DateTime #m_year, #m_mont, and #m_day values.
 *
 *  Called by one of the family of rise/set functions to derive the time of
 *  the event.
 *
 *  \param event One of the #CDT_Event enumeration values:
 *  \arg #CDT_SunRise
 *  \arg #CDT_SunSet
 *  \arg #CDT_MoonRise
 *  \arg #CDT_MoonSet
 *  \arg #CDT_AstronomicalDawn
 *  \arg #CDT_AstronomicalDusk
 *  \arg #CDT_CivilDawn
 *  \arg #CDT_CivilDusk
 *  \arg #CDT_NauticalDawn
 *  \arg #CDT_NauticalDusk
 *  \param gp Reference to an existing GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::calculateSunTime( int event, const GlobalPosition &gp )
{
    // Get the local rise/set time of this event and save the flag
    double hours = 0.0;
    int flag = CDT_RiseSet( (int) event, m_jdate, gp.longitude(),
        gp.latitude(), gp.gmtDiff(), &hours );

    // Add the event time to the Julian date
    m_jdate += ( hours / 24. );

    // Update the calendar
    m_event = event;
    calculateCalendarDate();
    if ( ! isValid() )
    {
        return( false );
    }

    // Since the validation routine updates m_flag, set it back again.
    m_flag = flag;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of civil dawn for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The civil dawn occurs when the rising sun reaches 6 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Rises,
 *  then civil dawn occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dawn doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dawn doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_CivilDawn.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDawn(), astronicalDusk(), civilDusk(), nauticalDawn(),
 *  nauticalDusk().
 */

bool DateTime::civilDawn( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_CivilDawn, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of civil dusk for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The civil dusk occurs when the setting sun reaches 6 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Sets,
 *  then civil dusk occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dusk doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time is undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dusk doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time is undefined.
 *
 *  After calling this function, event() returns #CDT_CivilDusk.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDawn(), astronicalDusk(), civilDawn(), nauticalDawn(),
 *  nauticalDusk().
 */

bool DateTime::civilDusk( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_CivilDusk, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime day of the month.
 *
 *  \return Current value of the DateTime day of the month (1-31).
 */

int DateTime::day( void ) const
{
    return( m_day );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime day to newDay.
 *
 *  \param newDay New day of the month (1-31).
 *
 *  \return New day of the month value (1-31).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::day( int newDay )
{
    m_day = newDay;
    calculateJulianDate();
    isValid();
    return( m_day );
}

//------------------------------------------------------------------------------
/*! \brief detrmines the day-of-the-week index for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  Calls #CDT_DayOfWeek() to perform the calculation.
 *
 *  \retval 0 = Sun
 *  \retval 1 = Mon
 *  \retval 2 = Tue
 *  \retval 3 = Wed
 *  \retval 4 = Thu
 *  \retval 5 = Fri
 *  \retval 6 = Sat
 */

int DateTime::dayOfWeek( void ) const
{
    return( CDT_DayOfWeek( m_jdate ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the 3-letter English abbreviation for the current
 *  DateTime #m_year, #m_month, and #m_day.
 *
 *  Calls #CDT_DayOfWeekAbbreviation() to perform the calculation.
 *
 *  \return Pointer to a static string containing the 3-letter English
 *  abbreviation for the current DateTime day-of-the-week,
 *  or a pointer to the static string "Bad Day-of-Week Index".
 */

const char *DateTime::dayOfWeekAbbreviation( void ) const
{
    return( CDT_DayOfWeekAbbreviation( dayOfWeek() ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the English name for the current DateTime #m_year,
 *  #m_month, and #m_day.
 *
 *  Calls #CDT_DayOfWeekName() to perform the calculation.
 *
 *  \return Pointer to a static string containing the English name for the
 *  current DateTime day-of-the-week,
 *  or a pointer to the static string "Bad Day-of-Week Index".
 */

const char *DateTime::dayOfWeekName( void ) const
{
    return( CDT_DayOfWeekName( dayOfWeek() ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the day of the year for the current DateTime #m_year,
 *  #m_month, and #m_day.
 *
 *  Julian leap years, Gregorian leap years, and the Gregorian calendar
 *  reform of 1582 is taken into account.
 *
 *  Calls #CDT_DayOfYear() to perform the calculation.
 *
 *  \return Current DateTime day-of-the-year (1-366).
 */

int DateTime::dayOfYear( void ) const
{
    return( CDT_DayOfYear( m_year, m_month, m_day ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the days in the month for the current DateTime #m_year
 *  and #m_month.
 *
 *  Julian leap years, Gregorian leap years, and the Gregorian calendar
 *  reform of 1582 is taken into account.
 *
 *  Calls CDT_DaysInMonth() to perform the calculation.
 *
 *  \return Days in the month for the current DateTime (21-31).
 */

int DateTime::daysInMonth( void ) const
{
    return( CDT_DaysInMonth( m_year, m_month ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the days in the month for the current DateTime #m_year.
 *
 *  Julian leap years, Gregorian leap years, and the Gregorian calendar
 *  reform of 1582 is taken into account.
 *
 *  Calls CDT_DaysInYear() to perform the calculation.
 *
 *  \return Days in the year for the current DateTime (355, 365, or 366).
 */

int DateTime::daysInYear( void ) const
{
    return( CDT_DaysInYear( m_year ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the number of days since another DateTime.

 *  The result is positive if \a dt is earlier than \a this DateTime.
 *  The result is negative if \a dt is later than \a this DateTime.
 *
 *  \param dt Reference to an existing DateTime instance.
 *
 *  \return Decimal days since \a dt DateTime and \a this DateTime.
 *
 *  \sa daysUntil(), hoursUntil(), hoursSince().
 */

double DateTime::daysSince( const DateTime &dt ) const
{
    return( m_jdate - dt.m_jdate );
}

//------------------------------------------------------------------------------
/*! \brief Determines the number of days until another DateTime.
 *
 *  The result is negative if \a dt is earlier than \a this DateTime.
 *  The result is positive if \a dt is later than \a this DateTime.
 *
 *  \param dt Reference to an existing DateTime instance.
 *
 *  \return Decimal days from \a this DateTime until \a dt DateTime.
 *
 *  \sa daysSince(), hoursSince(), hoursUntil().
 */

double DateTime::daysUntil( const DateTime &dt ) const
{
    return( dt.m_jdate - m_jdate );
}

//------------------------------------------------------------------------------
/*! \brief Determines the elapsed portion of the DateTime #m_day since midnight.
 *
 *  \return The elapsed portion of the DateTime #m_day since midnight in
 *  decimal days.
 *
 *  \sa millisecondOfDay(), decimalHour()
 */

double DateTime::decimalDay( void ) const
{
    return( CDT_DecimalDay( m_hour, m_minute, m_second, m_millisecond ) ) ;
}

//------------------------------------------------------------------------------
/*! \brief Determines the DateTime elapsed hours since midnight.
 *
 *  \return The elapsed portion of the DateTime day since midnight in
 *  decimal hours.
 *
 *  \sa millisecondOfDay(), decimalDay()
 */

double DateTime::decimalHour( void ) const
{
    return( CDT_DecimalHour( m_hour, m_minute, m_second, m_millisecond ) ) ;
}

//------------------------------------------------------------------------------
/*! \brief Determines the date of Easter for the specified \a year.
 *
 *  Calls CDT_EasterDay() to perform the computations.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *
 *  \warning Valid \e only for the Gregorian calendar (1583 and later).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return #m_year is set to \a year, #m_month is set to the month of
 *  Easter (3=March, 4=April) and #m_day is set to the day of easter
 *  for the \a year.
 *
 *  \sa easter().
 */

bool DateTime::easter( int year )
{
    m_year = year;
    return( easter() );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date of Easter for the current DateTime #m_year.
 *
 *  Calls CDT_EasterDay() to perform the computations.
 *
 *  \warning Valid \e only for the Gregorian calendar (1583 and later).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return #m_month is set to the month of Easter (3=March, 4=April)
 *  and #m_day is set to the day of easter for the #m_year.
 *
 *  \sa easter(int year).
 */

bool DateTime::easter( void )
{
    CDT_EasterDay( m_year, &m_month, &m_day );
    m_hour = 12;
    m_minute = m_second = m_millisecond = 0;
    // Update
    calculateJulianDate();
    m_event = CDT_Easter;
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Gets the #CDT_Event enumeration value indicating the last operation
 *  on performed on this DateTime.
 *
 *  \return One of the #CDT_Event enumeration values.
 */

int DateTime::event( void ) const
{
    return( m_event );
}

//------------------------------------------------------------------------------
/*! \brief Gets the brief name of the #CDT_Event enumeration value indicating
 *  the last operation on performed on this DateTime.
 *
 *  \return Pointer to a static string containing the English name of the
 *  #CDT_Event enumeration value.
 */

const char *DateTime::eventName( void ) const
{
    return( CDT_EventName( m_event ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the fall equinox for \a year.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the fall
 *  equinox for \a year, and event() returns #CDT_Fall.
 */

bool DateTime::fallEquinox( int year, const GlobalPosition &gp )
{
    m_year = year;
    return( fallEquinox( gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the fall equinox for the current
 *  DateTime #m_year.
 *
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the fall
 *  equinox for \a year, and event() returns #CDT_Fall.
 */

bool DateTime::fallEquinox( const GlobalPosition &gp )
{
    calculateSolstice( (m_event = CDT_Fall), gp );
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Gets the #CDT_Flag enumeration value indicating the result condition
 *  of the last operation on performed on this DateTime.
 *
 *  \return One of the #CDT_Flag enumeration values.
 */

int DateTime::flag( void ) const
{
    return( m_flag );
}

//------------------------------------------------------------------------------
/*! \brief Gets the brief name of the #CDT_Flag enumeration value indicating
 *  the result condition of the last operation on performed on this DateTime.
 *
 *  \return Pointer to a static string containing the English name of the
 *  #CDT_Flag enumeration value.
 */

const char *DateTime::flagName( void ) const
{
    return( CDT_FlagName( m_flag ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the full moon following the
 *  \a period new moon for the current DateTime #m_year at the GlobalPosition
 *  and stores the result in the DateTime.
 *
 *  \param period Lunation period; 1 == first new moon of \a year, 2 == second
 *  new moon of \a year, etc.  Use period==0 to get the last new moon prior to
 *  \a year.
 *  \param gp Reference to an existing GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return #m_event is set to #CDT_NewMoon and
 *  the remaining DateTime fields contains the date and time of the new moon.
 */

bool DateTime::fullMoon( int period, const GlobalPosition &gp )
{
    int l_year = m_year;
    newMoon( period, gp );
    double date0 = m_jdate;
    newMoon( l_year, period+1, gp );
    double date1 = m_jdate;
    m_jdate = 0.5 * (date0 + date1);

    // Update
    calculateCalendarDate();
    m_event = CDT_FullMoon;
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime hour of the day.
 *
 *  \return Current value of the DateTime hour of the day (0-23).
 */

int DateTime::hour( void ) const
{
    return( m_hour );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime hour to newHour.
 *
 *  \param newHour New hour of the day; e.g. elapsed hours since midnight (0-23).
 *
 *  \return New hour of the day value (0-23).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::hour( int newHour )
{
    m_hour = newHour;
    calculateJulianDate();
    isValid();
    return( m_hour );
}

//------------------------------------------------------------------------------
/*! \brief Determines the number of hours since another DateTime.
 *
 *  The result is positive if \a dt is earlier than \a this DateTime.
 *  The result is negative if \a dt is later than \a this DateTime.
 *
 *  \param dt Reference to an existing DateTime instance.
 *
 *  \return Decimal hours since \a dt DateTime and \a this DateTime.
 *
 *  \sa daysSince(), daysUntil(), hoursUntil().
 */

double DateTime::hoursSince( const DateTime &dt ) const
{
    return( 24. * (m_jdate - dt.m_jdate) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the number of hours until another DateTime.
 *
 *  The result is negative if \a dt is earlier than \a this DateTime.
 *  The result is positive if \a dt is later than \a this DateTime.
 *
 *  \param dt Reference to an existing DateTime instance.
 *
 *  \return Decimal hours from \a this DateTime until \a dt DateTime.
 *
 *  \sa daysSince(), daysUntil(), hoursSince().
 */
double DateTime::hoursUntil( const DateTime &dt ) const
{
    return( 24. * (dt.m_jdate - m_jdate) );
}

//------------------------------------------------------------------------------
/*! \brief determines the number of leap days for the current DateTime #m_year.
 *
 *  Accounts for both Julian and Gregorian leap years.
 *
 *  \return Number of leap days for the current DateTime #m_year (0 or 1).
 */

int DateTime::isLeapYear( void ) const
{
    return( CDT_LeapYear( m_year ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines if all the current DateTime date and time data member
 *  values are valid.
 *
 *  On return #m_flag has one of the following #CDT_Flag enumeration values
 *  (which may be queried via flag()):
 *  \arg #CDT_HasValidDateTime if all the date and time fields are valid.
 *  \arg #CDT_HasInvalidYear
 *  \arg #CDT_HasInvalidMonth
 *  \arg #CDT_HasInvalidDay
 *  \arg #CDT_HasInvalidHour
 *  \arg #CDT_HasInvalidMinute
 *  \arg #CDT_HasInvalidSecond
 *  \arg #CDT_HasInvalidMillisecond
 *
 *  \return Sets the #m_flag and returns TRUE if valid, FALSE if invalid.
 *
 *  \sa isValidDate(), isValidTime().
 */

bool DateTime::isValid( void )
{
    if ( ( m_flag =
            CDT_ValidDateTime( m_year, m_month, m_day,
                m_hour, m_minute, m_second, m_millisecond ) )
        != CDT_HasValidDateTime )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the current DateTime #m_year, #m_month, and #m_day
 *  values are valid.
 *
 *  On return #m_flag has one of the following #CDT_Flag enumeration values
 *  (which may be queried via flag()):
 *  \arg #CDT_HasValidDate if all the date fields are valid.
 *  \arg #CDT_HasInvalidYear
 *  \arg #CDT_HasInvalidMonth
 *  \arg #CDT_HasInvalidDay
 *
 *  \return Sets the #m_flag and returns TRUE if valid, FALSE if invalid.
 *
 *  \sa isValidDate(), isValidTime().
 */

bool DateTime::isValidDate( void )
{
    if ( ( m_flag =
            CDT_ValidDate( m_year, m_month, m_day ) )
        != CDT_HasValidDate )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the current DateTime #m_hour, #m_minute,
 *  #m-second, and #m_millisecond values are valid.
 *
 *  On return #m_flag has one of the following #CDT_Flag enumeration values
 *  (which may be queried via flag()):
 *  \arg #CDT_HasValidTime if all the time fields are valid.
 *  \arg #CDT_HasInvalidHour
 *  \arg #CDT_HasInvalidMinute
 *  \arg #CDT_HasInvalidSecond
 *  \arg #CDT_HasInvalidMillisecond
 *
 *  \return Sets the #m_flag and returns TRUE if valid, FALSE if invalid.
 *
 *  \sa isValidDate(), isValidDateTime().
 */

bool DateTime::isValidTime( void )
{
    if ( ( m_flag =
            CDT_ValidTime( m_hour, m_minute, m_second, m_millisecond ) )
        != CDT_HasValidTime )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime Julian date.
 *
 *  \return Current value of the DateTime Julian date in days since noon of
 *  January 1, 4713 B.C.
 */

double DateTime::julianDate( void ) const
{
    return( m_jdate );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime millisecond of the second.
 *
 *  \return Current value of the DateTime millisecond of the second (0-999).
 */

int DateTime::millisecond( void ) const
{
    return( m_millisecond );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime millisecond to newMillisecond.
 *
 *  \param newMillisecond New millisecond of the second (0-999).
 *
 *  \return New millisecond of the second value (0-999).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::millisecond( int newMillisecond )
{
    m_millisecond = newMillisecond;
    calculateJulianDate();
    isValid();
    return( m_millisecond );
}

//------------------------------------------------------------------------------
/*! \brief Determines the number of milliseconds elapsed since midnight for
 *  the current DateTime.
 *
 *  \return Milliseconds past midnight for the current DateTime (0 - 86399999).
 */

int DateTime::millisecondOfDay( void ) const
{
    return( CDT_MillisecondOfDay( m_hour, m_minute, m_second, m_millisecond ) );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime minute of the hour.
 *
 *  \return Current value of the DateTime minute of the hour (0-59).
 */

int DateTime::minute( void ) const
{
    return( m_minute );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime minute to newMinute.
 *
 *  \param newMinute New minute of the hour (0-59).
 *
 *  \return New minute of the hour value (0-59).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::minute( int newMinute )
{
    m_minute = newMinute;
    calculateJulianDate();
    isValid();
    return( m_minute );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime \e modified Julian date.
 *
 *  \return Current value of the DateTime \e modified Julian date in days
 *  since midnight of 1858 Nov 17.
 */

double DateTime::modifiedJulianDate( void ) const
{
    return( CDT_ModifiedJulianDate( m_jdate ) );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime month of the year (1-12).
 *
 *  \return Current value of the DateTime month of the year (1-12).
 */

int DateTime::month( void ) const
{
    return( m_month );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime month to newMonth.
 *
 *  \param newMonth New month of the year (1-12).
 *
 *  \return New month of the year value (1-12).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::month( int newMonth )
{
    m_month = newMonth;
    calculateJulianDate();
    isValid();
    return( m_month );
}

//------------------------------------------------------------------------------
/*! \brief Returns the 3-letter English abbreviation for the current
 *  DateTime #m_month.
 *
 *  Calls #CDT_MonthAbbreviation() to perform the calculation.
 *
 *  \return Pointer to a static string containing the 3-letter English
 *  abbreviation for the current DateTime month.
 */

const char *DateTime::monthAbbreviation( void ) const
{
    return( CDT_MonthAbbreviation( m_month ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the English name for the current  DateTime #m_month.
 *
 *  Calls #CDT_MonthName() to perform the calculation.
 *
 *  \return Pointer to a static string containing the English  name for the
 *  current DateTime month.
 */

const char *DateTime::monthName( void ) const
{
    return( CDT_MonthName( m_month ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of moonrise for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Rises,
 *  then a moonrise occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_NeverRises,
 *  then a moonrise does not occur on the date
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Light,
 *  then it is polar day and the moon is always visible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then it is polar night and the moon is always invisible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_MoonRise.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of moonrise.
 *
 *  \sa moonSet(), sunRise(), sunSet().
 */

bool DateTime::moonRise( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_MoonRise, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of moonset for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Sets,
 *  then a moonset occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_NeverSets,
 *  then a moonrise does not occur on the date
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Light,
 *  then it is polar day and the moon is always visible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then it is polar night and the moon is always invisible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_MoonSet.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of moonset.
 *
 *  \sa moonRise(), sunRise(), sunSet().
 */

bool DateTime::moonSet( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_MoonSet, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of nautical dawn for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The nautical dawn occurs when the rising sun reaches 12 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() is #CDT_Rises,
 *  then nautical dawn occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dawn doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dawn doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_NauticalDawn.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDawn(), astronicalDusk(), civilDawn(), civilDusk(),
 *  nauticalDusk().
 */

bool DateTime::nauticalDawn( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_NauticalDawn, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of nautical dusk for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  The nautical dusk occurs when the setting sun reaches 12 degrees
 *  below the horizon.
 *
 *  If the #CDT_Flag returned by flag() is #CDT_Sets,
 *  then nautical dusk occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then dusk doesn't occur on the date because it is polar day,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then dusk doesn't occur on the date because it is polar night,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_NauticalDusk.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of dawn.
 *
 *  \sa astronimicalDawn(), astronicalDusk(), civilDawn(), civilDusk(),
 *  nauticalDawn().
 */

bool DateTime::nauticalDusk( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_NauticalDusk, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the \a year's \a period new moon at
 *  the GlobalPosition and stores the result in the DateTime.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later) for the new moon.
 *  \param period Lunation period; 1 == first new moon of \a year, 2 == second
 *  new moon of \a year, etc.  Use period==0 to get the last new moon prior to
 *  \a year.
 *  \param gp Reference to an existing GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return #m_event is set to #CDT_NewMoon, #m_year is set to \a year, and
 *  the remaining DateTime fields contains the date and time of the new moon.
 */

bool DateTime::newMoon( int year, int period, const GlobalPosition &gp )
{
    m_year = year;
    return( newMoon( period, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the \a period new moon for the
 *  current DateTime #m_year at the GlobalPosition
 *  and stores the result in the DateTime.
 *
 *  \param period Lunation period; 1 == first new moon of \a year, 2 == second
 *  new moon of \a year, etc.  Use period==0 to get the last new moon prior to
 *  \a year.
 *  \param gp Reference to an existing GlobalPosition instance.
 *
 *  \par Important Note
 *  Calling this with period==0 will get the last new moon before this year,
 *  \b BUT it will also reset the #m_year to the previous year!
 *  Any subsequent call will then be getting lunations from the \e PREVIOUS
 *  year.  Subsequent calls should use the 3-parameter form ::newMoon(
 *  int year, int period, const GlobalPosition &gp ).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return #m_event is set to #CDT_NewMoon and
 *  the remaining DateTime fields contains the date and time of the new moon.
 */

bool DateTime::newMoon( int period, const GlobalPosition &gp )
{
    // Get new moon GMT and adjust to local time
    m_jdate = CDT_NewMoonGMT( m_year, period) + ( gp.gmtDiff() / 24. );

    // Update
    calculateCalendarDate();
    m_event = CDT_NewMoon;
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Prints the current DateTime member data values to the file stream.
 *
 *  \param fptr Pointer to an open file stream.
 *
 *  \return The function returns nothing.
 */

void DateTime::print( FILE *fptr ) const
{
    fprintf( fptr,
        "%s is %s %s %02d, %04d (%03d) at %02d:%02d:%02d.%03d %s [jd %1.9f]\n",
        eventName(), dayOfWeekAbbreviation(), monthAbbreviation(), m_day,
        m_year, dayOfYear(), m_hour, m_minute, m_second, m_millisecond,
        flagName(), m_jdate );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime second of the hour.
 *
 *  \return Current value of the DateTime second of the hour (0-59).
 */

int DateTime::second( void ) const
{
    return( m_second );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime second to newSecond.
 *
 *  \param newSecond New second of the minute (0-59).
 *
 *  \return New second of the minute value (0-59).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::second( int newSecond )
{
    m_second = newSecond;
    calculateJulianDate();
    isValid();
    return( m_second );
}

//------------------------------------------------------------------------------
/*! \brief Sets all the DateTime data members from the passed Julian date.
 *
 *  \param julianDate Julian date (-4712 or later).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::set( double julianDate )
{
    m_jdate = julianDate;
    return( calculateCalendarDate() );
}

//------------------------------------------------------------------------------
/*! \brief Sets all the DateTime data members from the passed arguments.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param month Month of the year (1=Jan, 12=Dec).
 *  \param day Day of the month (1-31).
 *  \param hour Hour of the day (0-23).
 *  \param minute Minute of the hour (0-59).
 *  \param second Second of the minute (0-59).
 *  \param millisecond Millisecond of the second (0-999).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::set( int year, int month, int day, int hour, int minute,
        int second, int millisecond )
{
    // Store the passed values
    m_year          = year;
    m_month         = month;
    m_day           = day;
    m_hour          = hour;
    m_minute        = minute;
    m_second        = second;
    m_millisecond   = millisecond;

    // Update
    m_event = CDT_User;
    calculateJulianDate();
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Sets all the data members of the DateTime to the current system
 *  clock values.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::setSystem( void )
{
    // Get the current system date and time
    time_t now = time( (time_t *) NULL );
    struct tm *t = localtime( &now );

    // Store in the DateTime
    m_year          = t->tm_year + 1900;
    m_month         = t->tm_mon + 1;
    m_day           = t->tm_mday;
    m_hour          = t->tm_hour;
    m_minute        = t->tm_min;
    m_second        = t->tm_sec;
    m_millisecond   = 0;

    // Update
    m_event = CDT_System;
    calculateJulianDate();
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Sets the DateTime time data members from the passed arguments.
 *
 *  \param hour Hour of the day (0-23).
 *  \param minute Minute of the hour (0-59).
 *  \param second Second of the minute (0-59).
 *  \param millisecond Millisecond of the second (0-999).
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 */

bool DateTime::setTime( int hour, int minute, int second, int millisecond )
{
    if ( hour >= 0 )
    {
        m_hour = hour;
    }
    if ( minute >= 0 )
    {
        m_minute = minute;
    }
    if ( second >= 0 )
    {
        m_second = second;
    }
    if ( millisecond >= 0 )
    {
        m_millisecond = millisecond;
    }

    // Update
    m_event = CDT_User;
    calculateJulianDate();
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the spring equinox for \a year.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the spring
 *  equinox for \a year, and event() returns #CDT_Spring.
 */

bool DateTime::springEquinox( int year, const GlobalPosition &gp )
{
    m_year = year;
    return( springEquinox( gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the spring equinox for the current
 *  DateTime #m_year.
 *
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the spring
 *  equinox for \a year, and event() returns #CDT_Spring.
 */

bool DateTime::springEquinox( const GlobalPosition &gp)
{
    calculateSolstice( (m_event = CDT_Spring), gp );
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the summer solstice for \a year.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the summer
 *  solstice for \a year, and event() returns #CDT_Summer.
 */

bool DateTime::summerSolstice( int year, const GlobalPosition &gp )
{
    m_year = year;
    return( summerSolstice( gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the summer solstice for the current
 *  DateTime #m_year.
 *
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the summer
 *  solstice for \a year, and event() returns #CDT_Summer.
 */

bool DateTime::summerSolstice( const GlobalPosition &gp )
{
    calculateSolstice( (m_event = CDT_Summer), gp );
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of sunrise for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Rises,
 *  then a sunrise occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then it is polar day and the sun is always visible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then it is polar night and the sun is always invisible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_SunRise.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of sunrise.
 *
 *  \sa moonRise(), moonSet(), sunSet(), #CDT_Event, #CDT_Flag.
 */

bool DateTime::sunRise( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_SunRise, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the time of sunset for the current DateTime
 *  #m_year, #m_month, and #m_day.
 *
 *  If the #CDT_Flag returned by flag() returns #CDT_Sets,
 *  then a sunrise occurs on the DateTime day at the new DateTime time.
 *
 *  If flag() returns #CDT_Light,
 *  then it is polar day and the sun is always visible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  If flag() returns #CDT_Dark,
 *  then it is polar night and the sun is always invisible,
 *  and the resulting DateTime time fields are undefined.
 *
 *  After calling this function, event() returns #CDT_SunSet.
 *
 *  Calls #calculateSunTime() to perform the calculation.
 *
 *  \param gp Reference to a GlobalPosition or GlobalSite object.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return, the #m_hour, #m_minute, #m_second, and #m_millisecond
 *  data members are set to the time of sunset.
 *
 *  \sa moonRise(), moonSet(), sunRise(), #CDT_Event, #CDT_Flag.
 */

bool DateTime::sunSet( const GlobalPosition &gp )
{
    return( calculateSunTime( CDT_SunSet, gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the winter solstice for \a year.
 *
 *  \param year Julian-Gregorian calendar year (-4712 or later).
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the winter
 *  solstice for \a year, and event() returns #CDT_Winter.
 */

bool DateTime::winterSolstice( int year, const GlobalPosition &gp )
{
    m_year = year;
    return( winterSolstice( gp ) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the date and time of the winter solstice for the current
 *  DateTime #m_year.
 *
 *  \param gp Reference to a GlobalPosition instance.
 *
 *  \return TRUE if the resulting DateTime is valid,
 *  FALSE if the resulting DateTime is invalid.
 *  The invalid data member can be determined from the flag() return code.
 *
 *  On return the DateTime member data contain the date and time of the winter
 *  solstice for \a year, and event() returns #CDT_Winter.
 */

bool DateTime::winterSolstice( const GlobalPosition &gp)
{
    calculateSolstice( (m_event = CDT_Winter), gp );
    return( isValid() );
}

//------------------------------------------------------------------------------
/*! \brief Gets the value of the DateTime year.
 *
 *  \return Current value of the DateTime year (-4712 or later).
 */

int DateTime::year( void ) const
{
    return( m_year );
}

//------------------------------------------------------------------------------
/*! \brief Sets the value of the DateTime year to newYear.
 *
 *  \param newYear New Julian-Gregorian calendar year (-4712 or later).
 *
 *  \return New Julian-Gregorian calendar year value (-4712 or later).
 *  The Julian date #m_jdate and #CDT_Flag #m_flag are updated.
 */

int DateTime::year( int newYear )
{
    m_year = newYear;
    calculateJulianDate();
    isValid();
    return( m_year );
}

//------------------------------------------------------------------------------
/*! \brief Assignment operator.
 *
 *  \param dt Reference to an existing DateTime object.
 *  \return Reference to \a this object.
 */

DateTime &DateTime::operator=( const DateTime &dt )
{
    m_jdate         = dt.m_jdate;
    m_year          = dt.m_year;
    m_month         = dt.m_month;
    m_day           = dt.m_day;
    m_hour          = dt.m_hour;
    m_minute        = dt.m_minute;
    m_second        = dt.m_second;
    m_millisecond   = dt.m_millisecond;
    m_event         = dt.m_event;
    m_flag          = dt.m_flag;
    return( *this );
}

//------------------------------------------------------------------------------
//  End of datetime.cpp
//------------------------------------------------------------------------------

