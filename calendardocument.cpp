//------------------------------------------------------------------------------
/*! \file calendardocument.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Definition of the CalendarDocument class.
 *
 *  Important supporting methods are in the following files:
 *      - sundialog.cpp
 *      - sundialog.h
 *      - sunview.cpp
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "calendardocument.h"
#include "cdtlib.h"
#include "composer.h"
#include "datetime.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "doctabs.h"
#include "globalposition.h"
#include "property.h"

// Qt include files
#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qfontmetrics.h>
#include <qimage.h>
#include <qpaintdevicemetrics.h>
#include <qpen.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qtextstream.h>
#include <qworkspace.h>

// 36x36 logo
#include "logo036.xpm"

//------------------------------------------------------------------------------
/*! \brief CalendarDocument class constructor.

 *  \param workspace Pointer to the ApplicationWindow::m_workSpace parent.
 *  \param docId Unique serial number for the Document within this process.
 *  \param pd Pointer to a dictionary containing "cal*" properties.
 *  \param name Document's internal QWidget name.
 *  \param wflags Standard QWidget #wflags.
 */

CalendarDocument::CalendarDocument( QWorkspace *workspace, int docId,
        PropertyDict *pd, const char* name, int wflags ) :
    Document( workspace, "CalendarDocument", docId, name, wflags ),
    m_prop(pd)
{
    // Popup context menu must be created here because it pure virtual.
    contextMenuCreate();
    return;
}

//------------------------------------------------------------------------------
/*! \brief CalendarDocument destructor.
 */

CalendarDocument::~CalendarDocument( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  CalendarDocuments cannot be "Cleared".
 */

void CalendarDocument::clear( bool )
{
    QString text("");
    translate( text, "CalendarDoc:NoRun" );
    warn( text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the monthly calendar to a Composer file.  The calendar includes
 *  sunrise, sunset, moonrise, moonset times for each day as well as diagrams
 *  of moon quarter phases.
 *
 *  \param dt Pointer to the DateTime class specifying the calendar month.
 *  \param gp Pointer to the GlocalPosition class specifying the location.
 */

void CalendarDocument::composeCalendar( DateTime *dt, GlobalPosition *gp )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
    QFont fineFont( m_prop->string( "calFineFontFamily" ),
                    m_prop->integer( "calFineFontSize" ) );
    QPen moonPen( m_prop->color( "calMoonTimeFontColor" ) );
    QPen sunPen( m_prop->color( "calSunTimeFontColor" ) );
    QFontMetrics fineMetrics( fineFont );

    QFont numberFont( m_prop->string( "calNumberFontFamily" ),
                    m_prop->integer( "calNumberFontSize" ) );
    QPen numberPen( m_prop->color( "calNumberFontColor" ) );
    QFontMetrics numberMetrics( numberFont );

    QFont textFont( m_prop->string( "calTextFontFamily" ),
                    m_prop->integer( "calTextFontSize" ) );
    QPen textPen( m_prop->color( "calTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    QFont titleFont( m_prop->string( "calTitleFontFamily" ),
                    m_prop->integer( "calTitleFontSize" ) );
    QPen titlePen( m_prop->color( "calTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    QPen boxPen( m_prop->color( "calBoxColor" ),
                 m_prop->integer( "calBoxWidth" ) );

    QPen lightPen( yellow );
    QPen darkPen( gray );

    // Store pixel resolution into local variables.
    double yppi = m_screenSize->m_yppi;
    double textHt, titleHt, numberHt, fineHt;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt  = ( titleMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    numberHt = ( numberMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    fineHt = ( fineMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Open the composer and start with a new page.
    QString text(""), text2(""), text3("");
    translate( text, "CalendarDoc:Calendar:ToC" );
    startNewPage( text, TocCalendar );
    double yPos = m_pageSize->m_marginTop;

    // Print the table header.
    m_composer->font( numberFont );         // use calNumberFont (BIG)
    m_composer->pen( titlePen );            // but use calTitleFontColor
    int align = Qt::AlignVCenter|Qt::AlignCenter;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,     // start at UL corner
        m_pageSize->m_bodyWd, numberHt,     // width and height
        align, gp->locationName() );        // center location name
    yPos += numberHt;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,     // start at UL corner
        m_pageSize->m_bodyWd, numberHt,     // width and height
        align,                              // center
        QString( tr( "%1 %2" ) )
            .arg( dt->monthName() ).arg( dt->year() ) );
    yPos += numberHt;

    m_composer->font( titleFont );          // use calTitleFont
    QString qStr(""), qStr2("");
    translate( text, "CalendarDoc:Loc:Lon" );
    translate( text2, "CalendarDoc:Loc:Lat" );
    translate( text3, "CalendarDoc:Loc:GMT" );

    qStr.sprintf( "( %s %1.3f, %s %1.3f, %s %1.1f )",
        text.latin1(), gp->longitude(),
        text2.latin1(), gp->latitude(),
        text3.latin1(), gp->gmtDiff() );
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,     // start at UL corner
        m_pageSize->m_bodyWd, titleHt,      // width and height
        align, qStr );                      // Center lat-lon
    yPos += 2*numberHt;

    // Determine the requested display elements.
    bool doSunTimes  = m_prop->boolean( "calCalendarSunTimes" );
    bool doMoonPhase = m_prop->boolean( "calCalendarMoonPhase" );
    bool doMoonTimes = m_prop->boolean( "calCalendarMoonTimes" );
    bool above = false;
    bool below = false;

    // Determine the calendar matrix sizes.
    double calHt = m_pageSize->m_pageHt - yPos - m_pageSize->m_marginBottom
                 - 2 * numberHt - 3 * fineHt;
    double boxHt = calHt / 6.;
    double boxWd = ( m_pageSize->m_bodyWd - 2 * m_pageSize->m_padWd ) / 7.;
    int id;
    double x[8], y[8];
    x[0] = m_pageSize->m_marginLeft + m_pageSize->m_padWd;
    y[0] = yPos;
    for ( id = 1;
          id < 8;
          id++ )
    {
        x[id] = x[id-1] + boxWd;
        y[id] = y[id-1] + boxHt;
    }

    // Determine moon phases for each day.
    int moon[32];
    QString phase[5] = { "", "New", "1st", "Full", "3rd" };
    translate( phase[1], "CalendarDoc:Moon:New" );
    translate( phase[2], "CalendarDoc:Moon:1st" );
    translate( phase[3], "CalendarDoc:Moon:Full" );
    translate( phase[4], "CalendarDoc:Moon:3rd" );
    if ( doMoonPhase )
    {
        // Initialize each calendar day with no interesting moon phase.
        for ( id = 0;
              id < 32;
              id++ )
        {
            moon[id] = 0;
        }
        // Find each new moon for this year.
        DateTime moonDate[5];
        double newMoon0, newMoon1, fullMoon, qtr1Moon, qtr3Moon;
        for ( int period = 0;
              period <= 14;
              period++ )
        {
            int l_year = dt->year();
            // First new moon.
            moonDate[0].newMoon( dt->year(), period, *gp );
            newMoon0 = moonDate[0].julianDate();
            // We're done if we got into the next year.
            if ( moonDate[0].year() > dt->year() )
            {
                break;
            }
            // We're done if we got past this month.
            if ( moonDate[0].year() == dt->year()
              && moonDate[0].month() > dt->month() )
            {
                break;
            }
            // Next new moon.
            moonDate[4].newMoon( l_year, period+1, *gp );
            newMoon1 = moonDate[4].julianDate();
            // Interpolate and store phases between the two new moons.
            qtr1Moon = newMoon0 + 0.25 * ( newMoon1 - newMoon0 );
            moonDate[1].set( qtr1Moon );
            fullMoon = newMoon0 + 0.50 * ( newMoon1 - newMoon0 );
            moonDate[2].set( fullMoon );
            qtr3Moon = newMoon0 + 0.75 * ( newMoon1 - newMoon0 );
            moonDate[3].set( qtr3Moon );
            if ( moonDate[3].year() < dt->year() )
            {
                continue;
            }
            // Set moon[day] flag for significant moon days of this month.
            for ( id = 0;
                  id < 4;
                  id++ )
            {
                if ( moonDate[id].month() == dt->month()
                  && moonDate[id].year()  == dt->year() )
                {
                    moon[moonDate[id].day()] = id + 1;
                }
            }
        }
    }

    // Draw each day's box.
    DateTime date( dt->year(), dt->month(), 1 );
    int dow = date.dayOfWeek();
    int week = 0;
    QString Dow[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    translate( Dow[0], "CalendarDoc:Dow:Sun" );
    translate( Dow[1], "CalendarDoc:Dow:Mon" );
    translate( Dow[2], "CalendarDoc:Dow:Tue" );
    translate( Dow[3], "CalendarDoc:Dow:Wed" );
    translate( Dow[4], "CalendarDoc:Dow:Thu" );
    translate( Dow[5], "CalendarDoc:Dow:Fri" );
    translate( Dow[6], "CalendarDoc:Dow:Sat" );
    for ( int day = 1;
          day <= dt->daysInMonth();
          day++ )
    {
        // Draw the box.
        m_composer->pen( boxPen );
        m_composer->line( x[dow],   y[week],   x[dow+1], y[week] );
        m_composer->line( x[dow+1], y[week],   x[dow+1], y[week+1] );
        m_composer->line( x[dow+1], y[week+1], x[dow],   y[week+1] );
        m_composer->line( x[dow],   y[week+1], x[dow],   y[week] );

        // If this is one of first seven days, draw day-of-the-week name.
        m_composer->pen( numberPen );               // use calNumberFontColor
        m_composer->font( numberFont );             // use calNumberFont (BIG)
        if ( day <= 7 )
        {
            m_composer->text(
                x[dow], y[week]-boxHt,              // start at UL
                boxWd, boxHt-0.1,                   // width and height
                Qt::AlignBottom|Qt::AlignHCenter,   // center justify
                Dow[dow] );                         // draw day name text
        }

        // Draw the date number
        m_composer->text(
            x[dow], y[week]+0.1,                    // start at UL corner
            boxWd-0.1, boxHt,                       // width and height
            Qt::AlignTop|Qt::AlignRight,            // upper-right corner
            QString( "%1" ).arg( day ) );           // draw date text

        // Draw the sunrise-set times for this date.
        if ( doSunTimes )
        {
            // Sunrise time.
            date.set( dt->year(), dt->month(), day );
            date.sunRise( *gp );
            if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below-" );
                below = true;
            }
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above-" );
                above = true;
            }
            else
            {
                qStr.sprintf( "%02d:%02d - ", date.hour(), date.minute() );
            }

            // Sunset time.
            date.set( dt->year(), dt->month(), day );
            date.sunSet( *gp );
            if ( date.flag() == CDT_Invisible )
            {
                translate( qStr2, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr2, "CalendarDoc:RiseSet:Above" );
                above = true;
            }
            else
            {
                qStr2.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }

            m_composer->pen( sunPen );              // use calSunTimeFontColor
            m_composer->font( fineFont );           // use calFineFont
            m_composer->text(
                x[dow], y[week],                    // start at UL corner
                boxWd, boxHt,                       // width and height
                Qt::AlignBottom|Qt::AlignHCenter,   // center alignment
                (qStr + qStr2) );                   // draw sunrise-set times
        }

        // Draw the moonrise moonset times for this date.
        if ( doMoonTimes )
        {
            // Moonrise
            date.set( dt->year(), dt->month(), day );
            date.moonRise( *gp );
            if ( date.flag() == CDT_NeverRises )
            {
                translate( qStr, "CalendarDoc:RiseSet:Never-" );
            }
            else if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below-" );
                below = true;
            }
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above-" );
                above = true;
            }
            else
            {
                qStr.sprintf( "%02d:%02d - ", date.hour(), date.minute() );
            }

            // Moonset
            date.set( dt->year(), dt->month(), day );
            date.moonSet( *gp );
            if ( date.flag() == CDT_NeverSets )
            {
                translate( qStr, "CalendarDoc:RiseSet:Never-" );
            }
            else if ( date.flag() == CDT_Invisible )
            {
                translate( qStr2, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr2, "CalendarDoc:RiseSet:Above" );
                qStr2 = tr( "Above" );
                above = true;
            }
            else
            {
                qStr2.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }

            m_composer->pen( moonPen );             // use calMoonTimeFontColor
            m_composer->font( fineFont );           // use calFineFont
            m_composer->text(
                x[dow], y[week],                    // start at UL corner
                boxWd, boxHt-fineHt,                // width and height
                Qt::AlignBottom|Qt::AlignHCenter,   // center alignment
                (qStr + qStr2) );                   // draw moonrise moonset times
        }

        // Draw the moon phase (if any) for the day.
        if ( doMoonPhase && moon[day] )
        {
            // New moon?
            if ( moon[day] == 1 )
            {
                m_composer->pen( darkPen );         // use gray pen
                m_composer->ellipse(                // draw full circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt );           // width and height
            }
            // First quarter?
            else if ( moon[day] == 2 )
            {
                m_composer->pen( darkPen );         // use gray pen
                m_composer->ellipse(                // draw full circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt );           // width and height
                // Draw clockwise (negative) half circle starting at 12 o'clock.
                m_composer->pen( lightPen );        // use yellow pen
                m_composer->pie(                    // draw half circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt,             // width and height
                    -1440, 2880 );                  // cw 12-6 o'clock
            }
            // Full moon?
            else if ( moon[day] == 3 )
            {
                m_composer->pen( lightPen );        // use yellow pen
                m_composer->ellipse(                // draw full circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt );           // width and height
            }
            // Third quarter?
            else if ( moon[day] == 4 )
            {
                m_composer->pen( darkPen );         // use gray pen
                m_composer->ellipse(                // draw a circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt );           // width and height
                // Draw counter-clockwise (negative) half circle
                // starting at 12 o'clock.
                m_composer->pen( lightPen );        // use yellow pen
                m_composer->pie(                    // draw half circle
                    x[dow]+0.1, y[week]+0.1,        // start at UL corner
                    numberHt, numberHt,             // width and height
                    -1440, -2880 );                 // ccw 12-to-6 o'clock
            }

            // Draw text in center of the moon diagram.
            m_composer->pen( numberPen );           // use calNumberFontColor
            m_composer->font( fineFont );           // use calFineFont
            m_composer->text(
                x[dow]+0.1, y[week]+0.1,            // start at UL corner
                numberHt, numberHt,                 // width and height
                Qt::AlignVCenter|Qt::AlignHCenter,  // center alignment
                phase[moon[day]] );                 // draw moon phase text
        }

        // Set the next box position.
        if ( ++dow > 6 )
        {
            dow = 0;
            week++;
        }
    }

    // Draw the legend for moonrise-moonset and sunrise-sunset text.
    yPos = y[7] - 2 * fineHt;
    if ( doSunTimes )
    {
        m_composer->pen( sunPen );                  // use calSunTimeFontColor
        m_composer->font( fineFont );               // use calFineFont
        translate( text, "CalendarDoc:SunRiseSunSet" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,         // start at UL corner
            m_pageSize->m_bodyWd, fineHt,           // width and height
            Qt::AlignBottom|Qt::AlignHCenter,       // center alignment
            text );                                 // draw text
        yPos -= fineHt;
    }

    if ( doMoonTimes )
    {
        m_composer->pen( moonPen );                 // use calMoonTimeFontColor
        m_composer->font( fineFont );               // use calFineFont
        translate( text, "CalendarDoc:MoonRiseMoonSet" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,         // start at UL corner
            m_pageSize->m_bodyWd, fineHt,           // width and height
            Qt::AlignBottom|Qt::AlignHCenter,       // center alignment
            text );                                 // draw text
    }

    // Be polite and stop the composer.
    m_composer->end();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the chart of daily times for the month to a Composer file.
 *  The chart includes sunrise, sunset, moonrise, moonset, and civil
 *  dawn and dusk.
 *
 *  \param dt Pointer to the DateTime class specifying the calendar month.
 *  \param gp Pointer to the GlocalPosition class specifying the location.
 */

void CalendarDocument::composeChart( DateTime *dt, GlobalPosition *gp )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
    QFont textFont( m_prop->string( "calTextFontFamily" ),
                    m_prop->integer( "calTextFontSize" ) );
    QPen textPen( m_prop->color( "calTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    QFont titleFont( m_prop->string( "calTitleFontFamily" ),
                    m_prop->integer( "calTitleFontSize" ) );
    QPen titlePen( m_prop->color( "calTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    QFont valueFont( m_prop->string( "calValueFontFamily" ),
                    m_prop->integer( "calValueFontSize" ) );
    QPen valuePen( m_prop->color( "calValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    bool doRowBg = m_prop->boolean( "calRowBackgroundColorActive" );
    QBrush rowBrush( m_prop->color( "calRowBackgroundColor" ),
        Qt::SolidPattern );

    // Store pixel resolution into local variables.
    double yppi = m_screenSize->m_yppi;
    double textHt, titleHt, valueHt;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Determine number of columns needed.
    bool doSunRise   = m_prop->boolean( "calChartSunRise" );
    bool doSunSet    = m_prop->boolean( "calChartSunSet" );
    bool doMoonRise  = m_prop->boolean( "calChartMoonRise" );
    bool doMoonSet   = m_prop->boolean( "calChartMoonSet" );
    bool doCivilDawn = m_prop->boolean( "calChartCivilDawn" );
    bool doCivilDusk = m_prop->boolean( "calChartCivilDusk" );
    int columns = 1;
    if ( doSunRise ) columns++;
    if ( doSunSet ) columns++;
    if ( doMoonRise ) columns++;
    if ( doMoonSet ) columns++;
    if ( doCivilDawn ) columns++;
    if ( doCivilDusk ) columns++;

    // Divide page into vertical columns with edge padding.
    double width = ( m_pageSize->m_bodyWd - 2 * m_pageSize->m_padWd ) / columns;
    double x[7];
    x[0] = m_pageSize->m_marginLeft + m_pageSize->m_padWd;
    int id;
    for ( id = 1;
          id < 7;
          id++ )
    {
        x[id] = x[id-1] + width;
    }

    // Open the composer and start with a new page
    QString text(""), text2(""), text3("");
    translate( text, "CalendarDoc:Chart:ToC" );
    startNewPage( text, TocListOut );
    double yPos = m_pageSize->m_marginTop;

    // Print the table header.
    m_composer->font( titleFont );                  // use calTitleFont
    m_composer->pen( titlePen );                    // use calTitleFontColor
    int align = Qt::AlignVCenter|Qt::AlignCenter;

    translate( text, "CalendarDoc:Chart:Caption" );
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align, text );                              // center alignment
    yPos += titleHt;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align, gp->locationName() );                // center alignment
    yPos += titleHt;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align,                                      // center alignment
        QString( tr( "%1 %2" ) )                    // draw month and year text
            .arg( dt->monthName() ).arg( dt->year() ) );
    yPos += titleHt;

    QString qStr;
    translate( text, "CalendarDoc:Loc:Lon" );
    translate( text2, "CalendarDoc:Loc:Lat" );
    translate( text3, "CalendarDoc:Loc:GMT" );
    qStr.sprintf( "( %s %1.3f, %s %1.3f, %s %1.1f )",
        text.latin1(), gp->longitude(),
        text2.latin1(), gp->latitude(),
        text3.latin1(), gp->gmtDiff() );
    m_composer->font( textFont );                   // use calTextFont
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align, qStr );                              // center lat-lon text
    yPos += titleHt;

    // Display column headers.
    bool doThisRowBg = false;
    if ( doRowBg )
    {
        m_composer->fill( x[0], yPos, columns*width, textHt, rowBrush );
    }
    align = Qt::AlignVCenter|Qt::AlignCenter;
    m_composer->font( textFont );                   // use calTextFont
    m_composer->pen( textPen );                     // use calTextFontColor
    id = 0;
    // Day-of-the-month column header.
    translate( text, "CalendarDoc:Chart:Day" );
    m_composer->text(
        x[id], yPos,
        width, textHt,
        align, text );
    // Sunrise column header.
    if ( doSunRise )
    {
        translate( text, "CalendarDoc:Chart:SunRise" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    // Sunset column header.
    if ( doSunSet )
    {
        translate( text, "CalendarDoc:Chart:SunSet" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    // Moonrise column header.
    if ( doMoonRise )
    {
        translate( text, "CalendarDoc:Chart:MoonRise" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    // Moonset column header.
    if ( doMoonSet )
    {
        translate( text, "CalendarDoc:Chart:MoonSet" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    // Civil dawn column header.
    if ( doCivilDawn )
    {
        translate( text, "CalendarDoc:Chart:CivilDawn" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    // Civil dusk column header.
    if ( doCivilDusk )
    {
        translate( text, "CalendarDoc:Chart:CivilDusk" );
        m_composer->text(
            x[++id], yPos,
            width, textHt,
            align, text );
    }
    yPos += textHt;

    // Write a row for each day of the month.
    m_composer->font( valueFont );                  // use calValueFont
    m_composer->pen( valuePen );                    // use calValueFontColor
    DateTime date( dt->year(), dt->month() );
    bool above = false;
    bool below = false;
    bool dark  = false;
    bool light = false;
    for ( int day = 1;
          day <= dt->daysInMonth();
          day++ )
    {
        // Display a colored row background?
        if ( doRowBg && doThisRowBg )
        {
            m_composer->fill(
                x[0], yPos,
                columns*width, textHt,
                rowBrush );
        }
        doThisRowBg = ! doThisRowBg;

        // Write the day-of-the-month.
        id = 0;
        date.set( dt->year(), dt->month(), day );
        m_composer->text(
            x[id], yPos,
            width, valueHt,
            align,
            QString( "%1 %2" )
                .arg( date.dayOfWeekAbbreviation() ).arg( day ) );

        // Write the sunrise time.
        if ( doSunRise )
        {
            date.set( dt->year(), dt->month(), day );
            date.sunRise( *gp );
            // If the sun is always below the horizon...
            if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            // If the sun is always above the horizon...
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above" );
                above = true;
            }
            // If the sunrises...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // Write the sunrise time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Write the sunset time.
        if ( doSunSet )
        {
            date.set( dt->year(), dt->month(), day );
            date.sunSet( *gp );
            // If the sun stays below the horizon...
            if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            // If the sun stays above the horizon...
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above" );
                above = true;
            }
            // If the suns sets...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // Write the sunset time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Write the moonrise time.
        if ( doMoonRise )
        {
            date.set( dt->year(), dt->month(), day );
            date.moonRise( *gp );
            // If the moon never rises...
            if ( date.flag() == CDT_NeverRises )
            {
                translate( qStr, "CalendarDoc:RiseSet:Dash" );
            }
            // If the moon stays below the horizon...
            else if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            // If th moon stays above the horizon...
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above" );
                above = true;
            }
            // If there is a moonrise...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // Write the moonrise time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Write the moonset time.
        if ( doMoonSet )
        {
            date.set( dt->year(), dt->month(), day );
            date.moonSet( *gp );
            // If the moon never sets...
            if ( date.flag() == CDT_NeverSets )
            {
                translate( qStr, "CalendarDoc:RiseSet:Dash" );
            }
            // If the moon stays below the horizon...
            else if ( date.flag() == CDT_Invisible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Below" );
                below = true;
            }
            // If the moon stays above the horizon...
            else if ( date.flag() == CDT_Visible )
            {
                translate( qStr, "CalendarDoc:RiseSet:Above" );
                above = true;
            }
            // If there is a moonset...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // Write the moonset time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Write the civil dawn time.
        if ( doCivilDawn )
        {
            date.set( dt->year(), dt->month(), day );
            date.civilDawn( *gp );
            // If it stays light all day...
            if ( date.flag() == CDT_Light )
            {
                translate( qStr, "CalendarDoc:RiseSet:Light" );
                light = true;
            }
            // If it stays dark all day...
            else if ( date.flag() == CDT_Dark )
            {
                translate( qStr, "CalendarDoc:RiseSet:Dark" );
                dark = true;
            }
            // If there is a dawn...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // Write the civl dawn time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Write the civil dusk.
        if ( doCivilDusk )
        {
            date.set( dt->year(), dt->month(), day );
            date.civilDusk( *gp );
            // If it is light all day...
            if ( date.flag() == CDT_Light )
            {
                translate( qStr, "CalendarDoc:RiseSet:Light" );
                light = true;
            }
            // If it is dark all day...
            else if ( date.flag() == CDT_Dark )
            {
                translate( qStr, "CalendarDoc:RiseSet:Dark" );
                dark = true;
            }
            // If there is a dusk...
            else
            {
                qStr.sprintf( "%02d:%02d", date.hour(), date.minute() );
            }
            // write the civl dusk time.
            m_composer->text(
                x[++id], yPos,
                width, valueHt,
                align, qStr );
        }
        // Move to the next row position on the age.
        yPos += valueHt;
    }

    // Add any special notation.
    align = Qt::AlignVCenter|Qt::AlignCenter;
    m_composer->font( textFont );
    m_composer->pen( textPen );
    yPos += valueHt;
    if ( above )
    {
        translate( qStr, "CalendarDoc:RiseSet:Above:Note" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,
            m_pageSize->m_bodyWd, textHt,
            align, qStr );
        yPos += textHt;
    }
    if ( below )
    {
        translate( qStr, "CalendarDoc:RiseSet:Above:Note" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,
            m_pageSize->m_bodyWd, textHt,
            align, qStr );
        yPos += textHt;
    }
    if ( dark )
    {
        translate( qStr, "CalendarDoc:RiseSet:Dark:Note" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,
            m_pageSize->m_bodyWd, textHt,
            align, qStr );
        yPos += textHt;
    }
    if ( light )
    {
        translate( qStr, "CalendarDoc:RiseSet:Light:Note" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,
            m_pageSize->m_bodyWd, textHt,
            align, qStr );
        yPos += textHt;
    }
    // Be polite and stop the composer.
    m_composer->end();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the logo at the specified offset with the specified size.
 *
 *  This is a forced re-implementation of the Document::composeLogo() pure
 *  virtual method.  CalendarDocuments can use the default
 *  Document::composeLogo() method which draws nothing.
 *
 *  \param x0 Horizontal distance from the left edge of the page in inches.
 *  \param y0 Vertical distance from top edge of the page in inches.
 *  \param wd Width in inches.
 *  \param ht Height in inches.
 *  \param penWd Pen width in pixels.
 */

void CalendarDocument::composeLogo( double x0, double y0, double wd, double ht,
    int penWd )
{
    Document::composeLogo( x0, y0, wd, ht, penWd );
}

//------------------------------------------------------------------------------
/*! \brief Composes the CalendarDocument page background and decoration.
 *
 *  This is a forced re-implementation of the Document::composeLogo() pure
 *  virtual method.
 *
 *  CalendarDocuments create a white page with a rounded-rectangle border,
 *  page number on the upper right header,
 *  version number on the lower right footer, and
 *  logo on the lower left footer.
 *
 *  Should \b only be called by Document::startNewPage().
 */

void CalendarDocument::composeNewPage( void )
{
    static QPixmap logoPixmap( logo036_xpm );

    // Here are some styles to try.
    enum PageLocation
    {
        None,
        RightHeader, LeftHeader, CenterHeader,
        RightFooter, LeftFooter, CenterFooter
    };
    PageLocation pageNumber = RightHeader;
    PageLocation runTime    = LeftHeader;
    PageLocation version    = RightFooter;
    PageLocation logo       = LeftFooter;
    bool bottomLine         = false;

    // Convenience locals.
    Composer *cPtr    = m_composer;
    DocPageSize *pPtr = m_pageSize;

    // Fill the blank page with the background property color.
    QString colorName = m_prop->color( "calBackgroundColor" );
    cPtr->fill(
        0., 0.,
        pPtr->m_pageWd, pPtr->m_pageHt,
        QBrush( colorName, Qt::SolidPattern ) );

    // Draw a rounded rectangle around the page border (use xRnd = yRnd = 7).
    double pad = 0.1;
    if ( m_prop->boolean( "calBorderActive" ) )
    {
        QPen pen( m_prop->color( "calBorderColor" ),
                  m_prop->integer( "calBorderWidth" ) );
        //int round = m_prop->integer( "calBorderRound" );
        cPtr->pen( pen );
        cPtr->rect(
            pPtr->m_bodyLeft - pad,
            pPtr->m_bodyStart - pad,
            pPtr->m_bodyWd + 2*pad,
            pPtr->m_bodyHt + 2*pad,
            0.25 );
    }

    // A shaded header block.
    colorName = m_prop->color( "calHeaderColor" );
    cPtr->fill(
        pPtr->m_bodyLeft - pad,
        0.5 * pPtr->m_marginTop - pad,
        pPtr->m_bodyWd + 2*pad,
        0.5 * pPtr->m_marginTop,
        QBrush( colorName, Qt::SolidPattern ) );

    // A shaded footer block.
    colorName = m_prop->color( "calFooterColor" );
    cPtr->fill(
        pPtr->m_bodyLeft - pad,
        pPtr->m_bodyEnd + pad,
        pPtr->m_bodyWd + 2*pad,
        0.5 * pPtr->m_marginBottom,
        QBrush( colorName, Qt::SolidPattern ) );

    // Use the page decoration font.
    QFont font( m_prop->string( "calDecorationFontFamily" ),
                m_prop->integer( "calDecorationFontSize" ) );
    QPen pen( m_prop->color( "calDecorationFontColor" ) );
    cPtr->font( font );
    cPtr->pen( pen );

    // Page number at right header.
    QString qStr("");
    translate( qStr, "CalendarDoc:Page", QString( "%1" ).arg( m_pages ) );
    if ( pageNumber == RightHeader )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            0.5 * pPtr->m_marginTop - pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginTop,
            Qt::AlignVCenter|Qt::AlignRight,
            qStr.latin1() );
    }
    // Date stamp at left header.
    if ( runTime == LeftHeader )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            0.5 * pPtr->m_marginTop - pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginTop,
            Qt::AlignVCenter|Qt::AlignLeft,
            m_runTime.latin1() );
    }
    // Version info at right footer.
    if ( version == RightFooter )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            pPtr->m_bodyEnd + pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginBottom,
            Qt::AlignVCenter|Qt::AlignRight,
            QString( appWindow()->m_program + " " + appWindow()->m_version ) );
    }
    // Draw program logo at left footer?
    if ( logo == LeftFooter )
    {
        double x0 = pPtr->m_bodyLeft;
        double y0 = pPtr->m_bodyEnd + 2*pad;
        double ht = pPtr->m_marginBottom - 4*pad;
        //double wd = ht * 13./4.;
        //int penWd = 1;
        //composeLogo( x0, y0, wd, ht, penWd );
        cPtr->pixmap( logoPixmap, x0, y0, ht, ht );
    }
    // Draw a line at the bottom of the page?
    if ( bottomLine )
    {
        cPtr->line(
            0.,             pPtr->m_pageHt-.1,
            pPtr->m_pageWd, pPtr->m_pageHt-.1 );
    }
    // Add a page tab?
    if ( m_tabs->active() )
    {
        composeTab();
    }
    // Reset the font before returning.
    cPtr->font( m_propFont );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the table of the year's equinox, solstice, and moon phase
 *  times.
 *
 *  \param dt Pointer to the DateTime class specifying the year.
 *  \param gp Pointer to the GlocalPosition class specifying the location.
 */

void CalendarDocument::composeSeasons( DateTime *dt, GlobalPosition *gp )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
    QFont textFont( m_prop->string( "calTextFontFamily" ),
                    m_prop->integer( "calTextFontSize" ) );
    QPen textPen( m_prop->color( "calTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    QFont titleFont( m_prop->string( "calTitleFontFamily" ),
                    m_prop->integer( "calTitleFontSize" ) );
    QPen titlePen( m_prop->color( "calTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    QFont valueFont( m_prop->string( "calValueFontFamily" ),
                    m_prop->integer( "calValueFontSize" ) );
    QPen valuePen( m_prop->color( "calValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    bool doRowBg = m_prop->boolean( "calRowBackgroundColorActive" );
    QBrush rowBrush( m_prop->color( "calRowBackgroundColor" ),
        Qt::SolidPattern );

    // Store pixel resolution into local variables.
    double yppi = m_screenSize->m_yppi;
    double textHt, titleHt, valueHt;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Divide page into four vertical columns with edge padding.
    double width = ( m_pageSize->m_bodyWd - 2 * m_pageSize->m_padWd ) / 4;
    double x[4];
    x[0] = m_pageSize->m_marginLeft + m_pageSize->m_padWd;
    x[1] = x[0] + width;
    x[2] = x[1] + width;
    x[3] = x[2] + width;

    // Open the composer and start with a new page.
    QString text(""), text2(""), text3("");
    translate( text, "CalendarDoc:Seasons:ToC" );
    startNewPage( text, TocListOut );
    double yPos = m_pageSize->m_marginTop + titleHt;

    // Print the table header.
    m_composer->font( titleFont );                  // use calTitleFont
    m_composer->pen( titlePen );                    // use calTitleFontColor
    int align = Qt::AlignVCenter|Qt::AlignCenter;

    translate( text, "CalendarDoc:Seasons:Caption" );
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align,                                      // center alignment
        text );                                     // draw title
    yPos += titleHt;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align,                                      // center alignment
        gp->locationName() );                       // draw location name
    yPos += titleHt;

    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align,                                      // center alignment
        QString( tr( "%1" ) ).arg( dt->year() ) );  // draw year
    yPos += titleHt;

    QString qStr;
    translate( text, "CalendarDoc:Loc:Lon" );
    translate( text2, "CalendarDoc:Loc:Lat" );
    translate( text3, "CalendarDoc:Loc:GMT" );
    qStr.sprintf( "( %s %1.3f, %s %1.3f, %s %1.1f )",
        text.latin1(), gp->longitude(),
        text2.latin1(), gp->latitude(),
        text3.latin1(), gp->gmtDiff() );
    m_composer->font( textFont );                   // use calTextFont
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        align, qStr );                              // draw lat-lon
    yPos += 2 * titleHt;

    // Determine the equinox & solstice dates.
    int year  = dt->year();
    DateTime date[5];
    date[0].springEquinox( year, *gp );
    date[1].summerSolstice( year, *gp );
    date[2].fallEquinox( year, *gp );
    date[3].winterSolstice( year, *gp );
    date[4].easter( year );
    QString Season[5];
    translate( Season[0], "CalendarDoc:Season:SpringEquinox" );
    translate( Season[1], "CalendarDoc:Season:SummerSolstice" );
    translate( Season[2], "CalendarDoc:Season:FallEquinox" );
    translate( Season[3], "CalendarDoc:Season:WinterSolstice" );
    translate( Season[4], "CalendarDoc:Season:EasterSunday" );

    // Draw the equinox and solstice table.
    int id;
    for ( id = 0;
          id < 5;
          id++ )
    {
        m_composer->font( textFont );               // use calTextFont
        m_composer->pen( textPen );                 // use calTextFontColor
        m_composer->text(
            x[0], yPos,                             // start at UL corner
            2.*width-0.1, textHt,                   // width and height
            Qt::AlignVCenter|Qt::AlignRight,        // right justify
            Season[id] );                           // draw event name

        m_composer->font( valueFont );              // use calValueFont
        m_composer->pen( valuePen );                // use calValueFontColor
        // If equinox or solstice...
        if ( id < 4 )
        {
            qStr.sprintf( "%s, %s %02d  %02d:%02d",
                date[id].dayOfWeekAbbreviation(),
                date[id].monthAbbreviation(),
                date[id].day(),
                date[id].hour(),
                date[id].minute() );
        }
        // If Easter...
        else
        {
            qStr.sprintf( "%s, %s %02d",
                date[id].dayOfWeekAbbreviation(),
                date[id].monthAbbreviation(),
                date[id].day() );
        }
        m_composer->text(
            x[2]+0.1, yPos,                         // start at UL corner
            2.*width, valueHt,                      // width and height
            Qt::AlignVCenter|Qt::AlignLeft,         // left justify
            qStr );                                 // draw the date and time
        yPos += textHt;
    }
    yPos += 2 * textHt;

    // Draw the moon phase table.
    bool doThisRowBg = false;
    if ( doRowBg )
    {
        m_composer->fill(
            x[0], yPos,                             // start at UL corner
            4*width, textHt,                        // width and height
            rowBrush );                             // fill color and pattern
    }
    // Draw column header text for moon phases.
    align = Qt::AlignVCenter|Qt::AlignCenter;
    m_composer->font( textFont );
    m_composer->pen( textPen );
    QString MoonPhase[4];
    translate( MoonPhase[0], "CalendarDoc:Season:Moon:New" );
    translate( MoonPhase[1], "CalendarDoc:Season:Moon:1st" );
    translate( MoonPhase[2], "CalendarDoc:Season:Moon:Full" );
    translate( MoonPhase[3], "CalendarDoc:Season:Moon:3rd" );
    for ( id = 0;
          id < 4;
          id++ )
    {
        m_composer->text( x[id], yPos, width, textHt, align, MoonPhase[id] );
    }
    yPos += textHt;
    m_composer->font( valueFont );
    m_composer->pen( valuePen );

    // Draw one new, 1st quarter, full, and 3rd quarter moon cycle per row.
    double newMoon0, newMoon1, fullMoon, qtr1Moon, qtr3Moon;
    for ( int period = 0;
          period <= 14;
          period++ )
    {
        int l_year = year;
        // First new moon.
        date[0].newMoon( year, period, *gp );
        newMoon0 = date[0].julianDate();
        // Are we done with this year?
        if ( date[0].year() > year )
        {
            break;
        }
        // Next new moon.
        date[4].newMoon( l_year, period+1, *gp );
        newMoon1 = date[4].julianDate();
        // Interpolate and store the moon phases.
        qtr1Moon = newMoon0 + 0.25 * ( newMoon1 - newMoon0 );
        date[1].set( qtr1Moon );
        fullMoon = newMoon0 + 0.50 * ( newMoon1 - newMoon0 );
        date[2].set( fullMoon );
        qtr3Moon = newMoon0 + 0.75 * ( newMoon1 - newMoon0 );
        date[3].set( qtr3Moon );
        // Make sure all dates are this year.
        if ( date[3].year() < year )
        {
            continue;
        }
        // Display a colored row background?
        if ( doRowBg && doThisRowBg )
        {
            m_composer->fill(
                x[0], yPos,                         // start at UL corner
                4*width, textHt,                    // width and height
                rowBrush );                         // fill color and pattern
        }
        doThisRowBg = ! doThisRowBg;

        // Draw the new, 1st, full, and 3rd quarter moon dates and times.
        for ( id = 0;
              id<4;
              id++ )
        {
            qStr.sprintf( "%s %02d  %02d:%02d",
                date[id].monthAbbreviation(),
                date[id].day(),
                date[id].hour(),
                date[id].minute() );
            m_composer->text(
                x[id], yPos,                        // start at UL corner
                width, valueHt,                     // width and height
                align, qStr );                      // center the date and time
        }
        yPos += valueHt;
    }
    // Be polite and stop the composer.
    m_composer->end();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void CalendarDocument::configure( void )
{
    Document::configure();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-
 *  BehavePlusDocuments.
 */

void CalendarDocument::configureAppearance( void )
{
    Document::configureAppearance();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 */

void CalendarDocument::configureFuelModels( void )
{
    Document::configureFuelModels();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 */

void CalendarDocument::configureMoistureScenarios( void )
{
    Document::configureMoistureScenarios();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 */

void CalendarDocument::configureModules( void )
{
    Document::configureModules();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 */

void CalendarDocument::configureUnits( const QString &unitsSet )
{
    Document::configureUnits( unitsSet );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *
 *  Slot called when the Document's popup context menu has been selected.
 *  CalendarDocuments can use the default Document::contextMenuActivated()
 *  method that provides Save, saveAs, Print, and Capture callbacks.
 */

void CalendarDocument::contextMenuActivated( int id )
{
    Document::contextMenuActivated( id );
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *
 *  Slot called when the Document's popup context menu has been selected.
 *  CalendarDocuments can use the default Document::contextMenuCreate()
 *  method that provides Save, saveAs, Print, and Capture callbacks.
 *
 *  Called only by the Document() constructor.
 */

void CalendarDocument::contextMenuCreate( void )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    // Add the menu items.
    int mid;
    QString text("");
    // Print
    translate( text, "Menu:File:Print" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrint );
    // Capture
    translate( text, "Menu:File:Capture" );
    mid = m_contextMenu->insertItem( text,
             this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextCapture );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *
 *  CalendarDocument files use the default Document::open() method, since we
 *  never "open" a calendar file and this method is never called.
 *
 *  \param fileName Name of the Document file to open.
 *  \param fileType A one-word string describing the file type.  If this
 *  method does not recognize the string, an error message is displayed.
 *
 *  \retval TRUE if the file is successfully opened,.
 *  \retval FALSE if the file couldn't be opened.
 */

bool CalendarDocument::open(
    const QString &fileName,
    const QString &fileType )
{
    return( Document::open( fileName, fileType ) );
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  Prints the CalendarDocument file to a printer device.
 *
 *  CalendarDocument files use the default Document::print() method
 *  which uses the Composer to paint composer files onto the print device.
 *
 *  Called only by ApplicationWindow::print().
 *
 *  \return TRUE on success, FALSE on failure or cancel.
 */

bool CalendarDocument::print( void )
{
    return( Document::print() );
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  Re-scales the CalendarDocument's fonts and screen display using the ratio
 *  of the new \a points to the default font point size as a scaling factor.
 *
 *  CalendarDocuments use the default Document::rescale() method
 *  which offers 12 scales based on font point sizes.
 *
 *  \param points Font point size used for a scale factor.
 */

void CalendarDocument::rescale( int points )
{
    Document::rescale( points );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  CalendarDocuments cannot be "Reset".
 */

void CalendarDocument::reset( bool )
{
    QString text("");
    translate( text, "CalendarDoc:NoRun" );
    warn( text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  CalendarDocuments cannot be "Run".
 */

void CalendarDocument::run( bool )
{
    QString text("");
    translate( text, "CalendarDoc:NoRun" );
    warn( text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  CalendarDocuments cannot be "Saved".
 */

void CalendarDocument::save( const QString &, const QString & )
{
    QString text("");
    translate( text, "CalendarDoc:NoSave" );
    warn( text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Document pure virtual function re-implementation for non-BpDocuments.
 *  Creates the ApplicationWindow's \b View submenu options just before it
 *  displays itself.
 *
 *  CalendarDocuments use the default Document::viewMenuAboutToShow() method .
 *
 *  \param viewMenu Pointer to the \b View menu.
 */

void CalendarDocument::viewMenuAboutToShow( QPopupMenu *viewMenu )
{
    Document::viewMenuAboutToShow( viewMenu );
    return;
}

//------------------------------------------------------------------------------
//  End of calendardocument.cpp
//------------------------------------------------------------------------------

