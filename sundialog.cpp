//------------------------------------------------------------------------------
/*! \file sundialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus Sun-Moon calendar dialog class.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "datetime.h"
#include "globalposition.h"
#include "property.h"
#include "sundialog.h"
#include "textview.h"           // For widget printing

// Qt include files
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtextstream.h>

// Standard include files
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
//  ComboBox entries.
//------------------------------------------------------------------------------

static const char *Month[] =
{
    "SunDialog:Jan", "SunDialog:Feb", "SunDialog:Mar",
    "SunDialog:Apr", "SunDialog:May", "SunDialog:Jun",
    "SunDialog:Jul", "SunDialog:Aug", "SunDialog:Sep",
    "SunDialog:Oct", "SunDialog:Nov", "SunDialog:Dec"
};

//------------------------------------------------------------------------------
/*! \brief SunDialog constructor.
 */

SunDialog::SunDialog( QWidget *parent, DateTime *dt, GlobalPosition *gp,
        PropertyDict *pd, const char *name ) :
    AppTabDialog( parent, "SunDialog:Caption", name ),
    m_page1(0),
    m_gridFrame1(0),
    m_gridLayout1(0),
    m_locationLabel(0),
    m_locationLineEdit(0),
    m_browseButton(0),
    m_zoneLabel(0),
    m_zoneSpinBox(0),
    m_zoneDescLabel(0),
    m_spacer1Label(0),
    m_degreesLabel(0),
    m_minutesLabel(0),
    m_secondsLabel(0),
    m_lonLabel(0),
    m_lonEWComboBox(0),
    m_lonDegSpinBox(0),
    m_lonMinSpinBox(0),
    m_lonSecSpinBox(0),
    m_latLabel(0),
    m_latNSComboBox(0),
    m_latDegSpinBox(0),
    m_latMinSpinBox(0),
    m_latSecSpinBox(0),
    m_spacer2Label(0),
    m_monthLabel(0),
    m_monthComboBox(0),
    m_yearLabel(0),
    m_yearSpinBox(0),
    m_page2(0),
    m_gridFrame2(0),
    m_gridLayout2(0),
    m_showCalendar(0),
    m_showChart(0),
    m_showSeasons(0),
    m_sunRise(0),
    m_sunSet(0),
    m_moonRise(0),
    m_moonSet(0),
    m_civilDawn(0),
    m_civilDusk(0),
    m_sunTimes(0),
    m_moonTimes(0),
    m_moonPhase(0),
    m_dt(dt),
    m_gp(gp),
    m_pd(pd)
{
    page1();
    page2();
    return;
}

//------------------------------------------------------------------------------
/*! \brief SunDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

SunDialog::~SunDialog( void )
{
    delete m_locationLabel; m_locationLabel = 0;
    delete m_locationLineEdit;  m_locationLineEdit = 0;
    delete m_browseButton;  m_browseButton = 0;
    delete m_zoneLabel;     m_zoneLabel = 0;
    delete m_zoneSpinBox;   m_zoneSpinBox = 0;
    delete m_zoneDescLabel; m_zoneDescLabel = 0;
    delete m_spacer1Label;  m_spacer1Label = 0;
    delete m_degreesLabel;  m_degreesLabel = 0;
    delete m_minutesLabel;  m_minutesLabel = 0;
    delete m_secondsLabel;  m_secondsLabel = 0;
    delete m_lonLabel;      m_lonLabel = 0;
    delete m_lonEWComboBox; m_lonEWComboBox = 0;
    delete m_lonDegSpinBox; m_lonDegSpinBox = 0;
    delete m_lonMinSpinBox; m_lonMinSpinBox = 0;
    delete m_lonSecSpinBox; m_lonSecSpinBox = 0;
    delete m_latLabel;      m_latLabel = 0;
    delete m_latNSComboBox; m_latNSComboBox = 0;
    delete m_latDegSpinBox; m_latDegSpinBox = 0;
    delete m_latMinSpinBox; m_latMinSpinBox = 0;
    delete m_latSecSpinBox; m_latSecSpinBox = 0;
    delete m_spacer2Label;  m_spacer2Label = 0;
    delete m_monthLabel;    m_monthLabel = 0;
    delete m_monthComboBox; m_monthComboBox = 0;
    delete m_yearLabel;     m_yearLabel = 0;
    delete m_yearSpinBox;   m_yearSpinBox = 0;
    delete m_gridLayout1;   m_gridLayout1 = 0;
    delete m_gridFrame1;    m_gridFrame1 = 0;
    delete m_page1;         m_page1 = 0;

    delete m_showCalendar;  m_showCalendar = 0;
    delete m_showChart;     m_showChart = 0;
    delete m_showSeasons;   m_showSeasons = 0;
    delete m_sunRise;       m_sunRise = 0;
    delete m_sunSet;        m_sunSet = 0;
    delete m_moonRise;      m_moonRise = 0;
    delete m_moonSet;       m_moonSet = 0;
    delete m_civilDawn;     m_civilDawn = 0;
    delete m_civilDusk;     m_civilDusk = 0;
    delete m_sunTimes;      m_sunTimes = 0;
    delete m_moonTimes;     m_moonTimes = 0;
    delete m_moonPhase;     m_moonPhase = 0;
    delete m_gridLayout2;   m_gridLayout2 = 0;
    delete m_gridFrame2;    m_gridFrame2 = 0;
    delete m_page2;         m_page2 = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the Location page.
 */

void SunDialog::page1( void )
{
    QString text("");
    // Create the page
    m_page1 = new AppPage( this,
        "ReturnOfLakeMissoula2.png",
        "Return Of Lake Missoula",
        "sunDialogLocation.html",
        "m_page1" );
    checkmem( __FILE__, __LINE__, m_page1, "AppPage m_page1", 1 );

    // Hidden frame to contain a grid layout
    m_gridFrame1 = new QFrame( m_page1->m_contentFrame, "m_gridFrame1" );
    Q_CHECK_PTR( m_gridFrame1 );
    m_gridFrame1->setFrameStyle( QFrame::NoFrame );

    // Create the label-entry grid layout
    m_gridLayout1 = new QGridLayout( m_gridFrame1, 9, 5, 0, 5,
        "m_gridLayout1" ) ;
    Q_CHECK_PTR( m_gridLayout1 );

    // Location
    translate( text, "SunDialog:Location" );
    m_locationLabel = new QLabel( text, m_gridFrame1, "m_locationLabel" );
    Q_CHECK_PTR( m_locationLabel );
    m_locationLabel->setFixedSize( m_locationLabel->sizeHint() );
    m_gridLayout1->addWidget( m_locationLabel, 0, 0, AlignLeft );

    m_locationLineEdit = new QLineEdit( m_gp->locationName(),
        m_gridFrame1, "m_locationLineEdit" );
    Q_CHECK_PTR( m_locationLineEdit );
    m_gridLayout1->addMultiCellWidget( m_locationLineEdit, 0, 0, 1, 3 );

    // Browse Button
    translate( text, "SunDialog:Browse" );
    m_browseButton = new QPushButton( text, m_gridFrame1, "m_browseButton" );
    Q_CHECK_PTR( m_browseButton );
    m_gridLayout1->addWidget( m_browseButton, 0, 4 );
    connect( m_browseButton, SIGNAL( clicked() ),
             this,           SLOT( browse() ) );

    // Time zone
    translate( text, "SunDialog:GmtDiff" );
    m_zoneLabel = new QLabel( text, m_gridFrame1, "m_zoneLabel" );
    Q_CHECK_PTR( m_zoneLabel );
    m_zoneLabel->setFixedSize( m_zoneLabel->sizeHint() );
    m_gridLayout1->addWidget( m_zoneLabel, 1, 0, AlignLeft );

    m_zoneSpinBox = new QSpinBox( -12, 12, 1, m_gridFrame1, "m_zoneSpinBox" );
    Q_CHECK_PTR( m_zoneSpinBox );
    m_zoneSpinBox->setValue( (int) m_gp->gmtDiff() );
    m_gridLayout1->addWidget( m_zoneSpinBox, 1, 1 );

    translate( text, "SunDialog:GmtDiff:Note" );
    m_zoneDescLabel = new QLabel( text, m_gridFrame1, "m_zoneDescLabel" );
    Q_CHECK_PTR( m_zoneDescLabel );
    m_zoneDescLabel->setFixedSize( m_zoneDescLabel->sizeHint() );
    m_gridLayout1->addMultiCellWidget( m_zoneDescLabel, 1, 1, 2, 4 );

    // Spacer
    m_spacer1Label = new QLabel( "", m_gridFrame1, "m_spacer1Label" );
    Q_CHECK_PTR( m_spacer1Label );
    m_gridLayout1->addWidget( m_spacer1Label, 2, 0, AlignLeft );

    translate( text, "SunDialog:Degrees" );
    m_degreesLabel = new QLabel( text, m_gridFrame1, "degreesLabel" );
    Q_CHECK_PTR( m_degreesLabel );
    m_gridLayout1->addWidget( m_degreesLabel, 3, 2, AlignLeft );

    translate( text, "SunDialog:Minutes" );
    m_minutesLabel = new QLabel( text, m_gridFrame1, "minutesLabel" );
    Q_CHECK_PTR( m_minutesLabel );
    m_gridLayout1->addWidget( m_minutesLabel, 3, 3, AlignLeft );

    translate( text, "SunDialog:Seconds" );
    m_secondsLabel = new QLabel( text, m_gridFrame1, "secondsLabel" );
    Q_CHECK_PTR( m_secondsLabel );
    m_gridLayout1->addWidget( m_secondsLabel, 3, 4, AlignLeft );

    // Longitude
    int degrees, minutes, seconds;
    DegToDms( m_gp->longitude(), &degrees, &minutes, &seconds );

    translate( text, "SunDialog:Longitude" );
    m_lonLabel = new QLabel( text, m_gridFrame1, "m_lonLabel" );
    Q_CHECK_PTR( m_lonLabel );
    m_lonLabel->setFixedSize( m_lonLabel->sizeHint() );
    m_gridLayout1->addWidget( m_lonLabel, 4, 0, AlignLeft );

    m_lonEWComboBox = new QComboBox( false, m_gridFrame1, "m_lonEWComboBox" );
    Q_CHECK_PTR( m_lonEWComboBox );
    translate( text, "SunDialog:West" );
    m_lonEWComboBox->insertItem( text );
    translate( text, "SunDialog:East" );
    m_lonEWComboBox->insertItem( text );
    m_lonEWComboBox->setCurrentItem( ( m_gp->longitude() >= 0. )
        ? 0
        : 1 );
    m_gridLayout1->addWidget( m_lonEWComboBox, 4, 1 );

    m_lonDegSpinBox = new QSpinBox( 0, 180, 1, m_gridFrame1, "m_lonDegSpinBox" );
    Q_CHECK_PTR( m_lonDegSpinBox );
    m_lonDegSpinBox->setValue( degrees );
    m_gridLayout1->addWidget( m_lonDegSpinBox, 4, 2 );

    m_lonMinSpinBox = new QSpinBox( 0, 59, 1, m_gridFrame1, "m_lonMinSpinBox" );
    Q_CHECK_PTR( m_lonMinSpinBox );
    m_lonMinSpinBox->setValue( minutes );
    m_gridLayout1->addWidget( m_lonMinSpinBox, 4, 3 );

    m_lonSecSpinBox = new QSpinBox( 0, 59, 1, m_gridFrame1, "m_lonSecSpinBox" );
    Q_CHECK_PTR( m_lonSecSpinBox );
    m_lonSecSpinBox->setValue( seconds );
    m_gridLayout1->addWidget( m_lonSecSpinBox, 4, 4 );

    // Latitude
    DegToDms( m_gp->latitude(), &degrees, &minutes, &seconds );

    translate( text, "SunDialog:Latitude" );
    m_latLabel = new QLabel( text, m_gridFrame1, "m_latLabel" );
    Q_CHECK_PTR( m_latLabel );
    m_latLabel->setFixedSize( m_latLabel->sizeHint() );
    m_gridLayout1->addWidget( m_latLabel, 5, 0, AlignLeft );

    m_latNSComboBox = new QComboBox( false, m_gridFrame1, "m_latNSComboBox" );
    Q_CHECK_PTR( m_latNSComboBox );
    translate( text, "SunDialog:North" );
    m_latNSComboBox->insertItem( text );
    translate( text, "SunDialog:South" );
    m_latNSComboBox->insertItem( text );
    m_latNSComboBox->setCurrentItem( ( m_gp->latitude() >= 0. )
        ? 0
        : 1);
    m_gridLayout1->addWidget( m_latNSComboBox, 5, 1 );

    m_latDegSpinBox = new QSpinBox( 0, 90, 1, m_gridFrame1, "m_latDegSpinBox" );
    Q_CHECK_PTR( m_latDegSpinBox );
    m_latDegSpinBox->setValue( degrees );
    m_gridLayout1->addWidget( m_latDegSpinBox, 5, 2 );

    m_latMinSpinBox = new QSpinBox( 0, 59, 1, m_gridFrame1, "m_latMinSpinBox" );
    Q_CHECK_PTR( m_latMinSpinBox );
    m_latMinSpinBox->setValue( minutes );
    m_gridLayout1->addWidget( m_latMinSpinBox, 5, 3 );

    m_latSecSpinBox = new QSpinBox( 0, 59, 1, m_gridFrame1, "m_latSecSpinBox" );
    Q_CHECK_PTR( m_latSecSpinBox );
    m_latSecSpinBox->setValue( seconds );
    m_gridLayout1->addWidget( m_latSecSpinBox, 5, 4 );

    // Spacer
    m_spacer2Label = new QLabel( tr( "" ), m_gridFrame1, "m_spacer2Label" );
    Q_CHECK_PTR( m_spacer2Label );
    m_gridLayout1->addWidget( m_spacer2Label, 6, 0, AlignLeft );

    // Month
    translate( text, "SunDialog:Month" );
    m_monthLabel = new QLabel( text, m_gridFrame1, "m_monthLabel" );
    Q_CHECK_PTR( m_monthLabel );
    m_monthLabel->setFixedSize( m_monthLabel->sizeHint() );
    m_gridLayout1->addWidget( m_monthLabel, 7, 0, AlignLeft );

    m_monthComboBox = new QComboBox( false, m_gridFrame1, "m_monthComboBox" );
    Q_CHECK_PTR( m_monthComboBox );
    for ( int m = 0;
          m < 12;
          m++ )
    {
        translate( text, Month[m] );
        m_monthComboBox->insertItem( text );
    }
    m_monthComboBox->setCurrentItem( m_dt->month() - 1 );
    m_gridLayout1->addWidget( m_monthComboBox, 7, 1 );

    // Year
    translate( text, "SunDialog:Year" );
    m_yearLabel = new QLabel( text, m_gridFrame1, "m_yearLabel" );
    Q_CHECK_PTR( m_yearLabel );
    m_yearLabel->setFixedSize( m_yearLabel->sizeHint() );
    m_gridLayout1->addWidget( m_yearLabel, 8, 0, AlignLeft );

    m_yearSpinBox = new QSpinBox( -4712, 4000, 1,
        m_gridFrame1, "m_yearSpinBox" );
    Q_CHECK_PTR( m_yearSpinBox );
    m_yearSpinBox->setValue( m_dt->year() );
    m_gridLayout1->addWidget( m_yearSpinBox, 8, 1 );

    // Add the page tab and return
    translate( text, "SunDialog:Location:Tab" );
    addTab( m_page1, text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the Contents page.
 */

void SunDialog::page2( void )
{
    QString text("");
    // Create the page
    m_page2 = new AppPage( this, "ZooCity2.png", "Zoo City",
        "sunDialogContents.html", "m_page2" );
    checkmem( __FILE__, __LINE__, m_page2, "AppPage m_page2", 1 );

    // Hidden frame to contain a grid layout
    m_gridFrame2 = new QFrame( m_page2->m_contentFrame, "m_gridFrame2" );
    Q_CHECK_PTR( m_gridFrame2 );
    m_gridFrame2->setFrameStyle( QFrame::NoFrame );

    // Create the label-entry grid layout
    m_gridLayout2 = new QGridLayout( m_gridFrame2, 12, 4, 10, 2,
        "m_gridLayout2" ) ;
    Q_CHECK_PTR( m_gridLayout2 );

    // New/Full Moon and Seasons
    translate( text, "CalendarDoc:Seasons:Caption" );
    m_showSeasons = new QCheckBox( text, m_gridFrame2, "m_showSeasons" );
    Q_CHECK_PTR( m_showSeasons );
    m_showSeasons->setChecked( m_pd->boolean( "calSeasonsActive" ) );
    m_gridLayout2->addMultiCellWidget( m_showSeasons, 0, 0, 0, 3 );

    // Sun-Moon Chart
    translate( text, "CalendarDoc:Chart:Caption" );
    m_showChart = new QCheckBox( text, m_gridFrame2, "m_showChart" );
    Q_CHECK_PTR( m_showChart );
    m_showChart->setChecked( m_pd->boolean( "calChartActive" ) );
    m_gridLayout2->addMultiCellWidget( m_showChart, 1, 1, 0, 3 );

    // Sunrise
    translate( text, "CalendarDoc:Chart:SunRise" );
    m_sunRise = new QCheckBox( text, m_gridFrame2, "m_sunRise" );
    Q_CHECK_PTR( m_sunRise );
    m_sunRise->setChecked( m_pd->boolean( "calChartSunRise" ) );
    m_gridLayout2->addMultiCellWidget( m_sunRise, 2, 2, 1, 3 );

    // Sunset
    translate( text, "CalendarDoc:Chart:SunSet" );
    m_sunSet = new QCheckBox( text, m_gridFrame2, "m_sunSet" );
    Q_CHECK_PTR( m_sunSet );
    m_sunSet->setChecked( m_pd->boolean( "calChartSunSet" ) );
    m_gridLayout2->addMultiCellWidget( m_sunSet, 3, 3, 1, 3 );

    // Moonrise
    translate( text, "CalendarDoc:Chart:MoonRise" );
    m_moonRise = new QCheckBox( text, m_gridFrame2, "m_moonRise" );
    Q_CHECK_PTR( m_moonRise );
    m_moonRise->setChecked( m_pd->boolean( "calChartMoonRise" ) );
    m_gridLayout2->addMultiCellWidget( m_moonRise, 4, 4, 1, 3 );

    // Moonset
    translate( text, "CalendarDoc:Chart:MoonSet" );
    m_moonSet = new QCheckBox( text, m_gridFrame2, "m_moonSet" );
    Q_CHECK_PTR( m_moonSet );
    m_moonSet->setChecked( m_pd->boolean( "calChartMoonSet" ) );
    m_gridLayout2->addMultiCellWidget( m_moonSet, 5, 5, 1, 3 );

    // Civil Dawn
    translate( text, "CalendarDoc:Chart:CivilDawn" );
    m_civilDawn = new QCheckBox( text, m_gridFrame2, "m_civilDawn" );
    Q_CHECK_PTR( m_civilDawn );
    m_civilDawn->setChecked( m_pd->boolean( "calChartCivilDawn" ) );
    m_gridLayout2->addMultiCellWidget( m_civilDawn, 6, 6, 1, 3 );

    // Civil Dusk
    translate( text, "CalendarDoc:Chart:CivilDusk" );
    m_civilDusk = new QCheckBox( text, m_gridFrame2, "m_civilDusk" );
    Q_CHECK_PTR( m_civilDusk );
    m_civilDusk->setChecked( m_pd->boolean( "calChartCivilDusk" ) );
    m_gridLayout2->addMultiCellWidget( m_civilDusk, 7, 7, 1, 3 );

    // Calendar
    translate( text, "SunDialog:SunMoonCalendar" );
    m_showCalendar = new QCheckBox( text, m_gridFrame2, "m_showCalendar" );
    Q_CHECK_PTR( m_showCalendar );
    m_showCalendar->setChecked( m_pd->boolean( "calCalendarActive" ) );
    m_gridLayout2->addMultiCellWidget( m_showCalendar, 8, 8, 0, 3 );

    // Sun Times
    translate( text, "SunDialog:SunTimes" );
    m_sunTimes = new QCheckBox( text, m_gridFrame2, "m_sunTimes" );
    Q_CHECK_PTR( m_sunTimes );
    m_sunTimes->setChecked( m_pd->boolean( "calCalendarSunTimes" ) );
    m_gridLayout2->addMultiCellWidget( m_sunTimes, 9, 9, 1, 3 );

    // Moon Times
    translate( text, "SunDialog:MoonTimes" );
    m_moonTimes = new QCheckBox( text, m_gridFrame2, "m_moonTimes" );
    Q_CHECK_PTR( m_moonTimes );
    m_moonTimes->setChecked( m_pd->boolean( "calCalendarMoonTimes" ) );
    m_gridLayout2->addMultiCellWidget( m_moonTimes, 10, 10, 1, 3 );

    // Moon Phase
    translate( text, "SunDialog:MoonPhase" );
    m_moonPhase = new QCheckBox( text, m_gridFrame2, "m_moonPhase" );
    Q_CHECK_PTR( m_moonPhase );
    m_moonPhase->setChecked( m_pd->boolean( "calCalendarMoonPhase" ) );
    m_gridLayout2->addMultiCellWidget( m_moonPhase, 11, 11, 1, 3 );

    // Display the page tab and return.
    translate( text, "SunDialog:Contents:Tab" );
    addTab( m_page2, text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Broswe button callback.  Displays the state/provence/country list.
 */

void SunDialog::browse( void )
{
    // Display the Geographic Catalog Browser
    GeoCatalogDialog catalogDialog( this );
    if ( catalogDialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Make sure a catalog was really selected.
    QString fileName = catalogDialog.fileSelection();
    if ( fileName.isNull() || fileName.isEmpty() )
    {
        QString text("");
        translate( text, "SunDialog:NoCatalog" );
        warn( text );
        return;
    }
    // Store the results and hide the dialog
    QString catalogName = catalogDialog.catalogSelection();
    catalogDialog.hide();

    // Display the Geographic Place Browser
    GeoPlaceDialog placeDialog( catalogName, fileName, this );
    if ( placeDialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Make sure a place was really selected
    QString placeName = placeDialog.m_place;
    if ( placeName.isNull() || placeName.isEmpty() )
    {
        QString text("");
        translate( text, "SunDialog:NoPlace" );
        warn( text );
        return;
    }
    // Do something with the results
    QString state   = placeDialog.m_state;
    //QString county  = placeDialog.m_county;
    //QString elev    = placeDialog.m_elev;
    QString lat     = placeDialog.m_lat;
    QString lon     = placeDialog.m_lon;
    //QString type    = placeDialog.m_type;
    QString gmtDiff = placeDialog.m_gmt;

    // Decode and update the location name
    m_locationLineEdit->setText( placeName + ", " + state );

    // Update the GMT difference and time zone from the file name and county
    m_zoneSpinBox->setValue( gmtDiff.toInt( 0, 10 ) );

    // Decode and update the longtiude
    bool ok;
    int deg, min, sec, dir;
    deg = lon.mid( 0, 3 ).toInt( &ok, 10 );
    min = lon.mid( 3, 2 ).toInt( &ok, 10 );
    sec = lon.mid( 5, 2 ).toInt( &ok, 10 );
    dir = ( lon.mid( 7, 1 ) == "W" )
          ? 0
          : 1;
    m_lonEWComboBox->setCurrentItem( dir );
    m_lonDegSpinBox->setValue( deg );
    m_lonMinSpinBox->setValue( min );
    m_lonSecSpinBox->setValue( sec );

    // Decode and update the latitude
    deg = lat.mid( 0, 2 ).toInt( &ok, 10 );
    min = lat.mid( 2, 2 ).toInt( &ok, 10 );
    sec = lat.mid( 4, 2 ).toInt( &ok, 10 );
    dir = ( lat.mid( 6, 1 ) == "N" )
          ? 0
          : 1;
    m_latNSComboBox->setCurrentItem( dir );
    m_latDegSpinBox->setValue( deg );
    m_latMinSpinBox->setValue( min );
    m_latSecSpinBox->setValue( sec );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Accept button callback.
 *  Stores the dialog settings into the document state variables.
 */

void SunDialog::store( void )
{
    // Store the DateTime
    m_dt->set( m_yearSpinBox->value(), m_monthComboBox->currentItem() + 1 );

    // Store the location and GMT difference
    m_gp->locationName( m_locationLineEdit->text() );
    m_gp->gmtDiff( (double) m_zoneSpinBox->value() );

    // Store the latitude decimal degrees
    double lat = DmsToDeg( m_latDegSpinBox->value(),
        m_latMinSpinBox->value(), m_latSecSpinBox->value() );
    if ( m_latNSComboBox->currentItem() == 1 )
    {
        lat *= -1;
    }
    m_gp->latitude( lat );

    // Store the longitude decimal degrees
    double lon = DmsToDeg( m_lonDegSpinBox->value(),
        m_lonMinSpinBox->value(), m_lonSecSpinBox->value() );
    if ( m_lonEWComboBox->currentItem() == 1 )
    {
        lon *= -1;
    }
    m_gp->longitude( lon );

    // Store the contents
    m_pd->boolean( "calCalendarActive",     m_showCalendar->isChecked() );
    m_pd->boolean( "calCalendarMoonPhase",  m_moonPhase->isChecked() );
    m_pd->boolean( "calCalendarMoonTimes",  m_moonTimes->isChecked() );
    m_pd->boolean( "calCalendarSunTimes",   m_sunTimes->isChecked() );
    m_pd->boolean( "calChartActive",        m_showChart->isChecked() );
    m_pd->boolean( "calChartSunRise",       m_sunRise->isChecked() );
    m_pd->boolean( "calChartSunSet",        m_sunSet->isChecked() );
    m_pd->boolean( "calChartMoonRise",      m_moonRise->isChecked() );
    m_pd->boolean( "calChartMoonSet",       m_moonSet->isChecked() );
    m_pd->boolean( "calChartCivilDawn",     m_civilDawn->isChecked() );
    m_pd->boolean( "calChartCivilDusk",     m_civilDusk->isChecked() );
    m_pd->boolean( "calSeasonsActive",      m_showSeasons->isChecked() );

    // Return QDialog::Accepted
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief GeoCatalogDialog constructor.
 *  Displays all the geographic catalog files in the folder
 *  and allows the user to select one.
 */

GeoCatalogDialog::GeoCatalogDialog( QWidget *parent, const char *name ) :
    AppDialog( parent, "GeoCatalogDialog:Caption",
        "DancingCranes.png", "Dancing Cranes", "geoCatalog.html", name ),
    m_catalog(""),
    m_file(""),
    m_geoDir(""),
    m_listView(0),
    m_contextMenu(0),
    m_lvi(0)
{
    // Add the listview to the mainFrame mainLayout
    m_listView = new QListView( m_page->m_contentFrame, "m_listView" );
    Q_CHECK_PTR( m_listView );
    QString text("");
    translate( text, "GeoCatalogDialog:StateProvinceCountry" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 0, QListView::Maximum ) ;
    m_listView->setRootIsDecorated( false );
    m_listView->setSelectionMode( QListView::Single );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );    // Sort on column 0 ascending
    m_listView->setItemMargin( 1 );

    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT(   doubleClicked( QListViewItem * ) ) );

    // Get a list of all the files in this directory
    m_geoDir = appFileSystem()->docGeoPath();
    QDir geoDir( m_geoDir );
    geoDir.setFilter( QDir::Files );
    geoDir.setNameFilter( "*.geo" );
    const QFileInfoList *files = geoDir.entryInfoList();

    // If files==0, then directory not readable or doesn't exist
    if ( files && files->count() > 0 )
    {
        // Ok, we have files to show, so make this item expandable
        QFileInfoListIterator it( *files );
        QFileInfo *fileInfo;
        while( ( fileInfo = it.current() ) != 0 )
        {
            ++it;
            // Make a new list item for this file
            QListViewItem *fileItem = new QListViewItem(
                m_listView, fileInfo->baseName() );
            Q_CHECK_PTR( fileItem );
            // Make this item selectable by the user
            fileItem->setSelectable( true );
        }
    }
    // Allow right click to invoke the context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );
    m_listView->setMinimumWidth( m_listView->columnWidth( 0 ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief GeoCatalogDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

GeoCatalogDialog::~GeoCatalogDialog( void )
{
    delete m_listView;      m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the current (highlighted, selected)catalog selection.
 */

const QString &GeoCatalogDialog::catalogSelection( void ) const
{
    return( m_catalog );
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void GeoCatalogDialog::contextMenuActivated( int id )
{
    if ( id == ContextSelect )
    {
        // Set m_lvi to 0 to let rightButtonClicked() know we're done with
        // the dialog
        m_listView->setSelected( m_lvi, true );
        m_lvi = 0;
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( m_page->m_contentFrame );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Double-click callback slot to store the clicked item,
 *  drop the dialog, and return Accepted.
 *
 *  \return Reference to a QString containing the current catalog file name.
 */

void GeoCatalogDialog::doubleClicked( QListViewItem *lvi )
{
    // Must click an item
    if ( lvi )
    {
        m_listView->setSelected( lvi, true );
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the current (highlighted, selected) catalog file.
 *
 *  \return Reference to a QString containing the current catalog file name.
 */

const QString &GeoCatalogDialog::fileSelection( void ) const
{
    return( m_file );
}

//------------------------------------------------------------------------------
/*! \brief Lists or selects the item.
 */

void GeoCatalogDialog::rightButtonClicked( QListViewItem *lvi,
    const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    m_lvi = lvi;

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        translate( text, "GeoCatalogDialog:ContextMenu:Select" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextSelect );
    }

    translate( text, "GeoCatalogDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "GeoCatalogDialog:ContextMenu:PrintEntire" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;

    // If m_lvi has been reset to 0 by contextMenuActivated(),
    // then the user made a selection and we're done with the dialog
    if ( lvi && ! m_lvi )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Ok button callback slot to store the current settings,
 *  drop the dialog, and return Accepted.
 */

void GeoCatalogDialog::store( void )
{
    QListViewItem *lvi = m_listView->selectedItem();
    if ( lvi )
    {
        m_catalog = lvi->text( 0 );
        m_file = m_geoDir + m_catalog + ".geo";
    }
    else
    {
        m_catalog = "";
        m_file    = "";
    }
    // Consume the event and return Accepted.
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief GeoPlaceDialog constructor.
 *  Displays all the named geographic places within a GeoPlaceCatalog
 *  and allows the user to select one.
 */

GeoPlaceDialog::GeoPlaceDialog( const QString &catalogName,
        const QString &fileName, QWidget *parent, const char *name ) :
    AppDialog( parent, "GeoPlaceDialog:Caption",
        "ArmchairSafari.png", "Armchair Safari", "geoPlace.html", name ),
    m_catalog(catalogName),
    m_county(""),
    m_elev(""),
    m_gmt(""),
    m_lat(""),
    m_lon(""),
    m_place(""),
    m_state(""),
    m_type(""),
    m_listView(0),
    m_contextMenu(0),
    m_lvi(0)
{
    QString text("");
    // Open the file
    QFile file( fileName );
    if ( ! file.open( IO_ReadOnly ) )
    {
        translate( text, "GeoPlaceDialog:NoOpen", fileName );
        error( text );
        return;
    }
    // Add the listview to the mainFrame mainLayout
    m_listView = new QListView( m_page->m_contentFrame, "m_listView" );
    Q_CHECK_PTR( m_listView );
    const QString headerKey[] =
    {
        "GeoPlaceDialog:State",
        "GeoPlaceDialog:County",
        "GeoPlaceDialog:PlaceName",
        "GeoPlaceDialog:Type",
        "GeoPlaceDialog:Longitude",
        "GeoPlaceDialog:Latitude",
        "GeoPlaceDialog:Elev",
        "GeoPlaceDialog:Gmt"
    };
    for ( int col = 0;
          col < 8;
          col++ )
    {
        translate( text, headerKey[col] );
        m_listView->addColumn( text );
        m_listView->setColumnWidthMode( col, QListView::Maximum ) ;
    }
    m_listView->setRootIsDecorated( false );
    m_listView->setSelectionMode( QListView::Single );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setSorting( 0, true );    // Sort on column 0 ascending
    m_listView->setItemMargin( 1 );
    m_listView->setMinimumWidth( m_listView->columnWidth( 0 ) );

    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT(   doubleClicked( QListViewItem * ) ) );

    QTextStream textStream( &file );
    QString qstr, place;
    while ( ! textStream.eof() )
    {
        qstr = textStream.readLine();
        // Make a new list item for this file
        QListViewItem *item = new QListViewItem( m_listView );
        Q_CHECK_PTR( item );
        item->setText( 0, qstr.mid(  93, 16 ).stripWhiteSpace() ); // state
        item->setText( 1, qstr.mid(  61, 31 ).stripWhiteSpace() ); // county
        item->setText( 2, qstr.mid(   0, 50 ).stripWhiteSpace() ); // place
        item->setText( 3, qstr.mid(  51,  9 ).stripWhiteSpace() ); // type
        item->setText( 4, qstr.mid( 117,  8 ).stripWhiteSpace() ); // lon
        item->setText( 5, qstr.mid( 110,  7 ).stripWhiteSpace() ); // lat
        item->setText( 6, qstr.mid( 126,  5 ).stripWhiteSpace() ); // elev
        item->setText( 7, qstr.mid( 132,  4 ).stripWhiteSpace() ); // GMT diff
        item->setSelectable( true );
    }
    file.close();

    // Allow right click to invoke the context menu
    connect(
        m_listView,
        SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
        this,
        SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int ) )
    );
    return;
}

//------------------------------------------------------------------------------
/*! \brief GeoPlaceDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

GeoPlaceDialog::~GeoPlaceDialog( void )
{
    delete m_listView;      m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the dialog's popup context menu has been selected.
 */

void GeoPlaceDialog::contextMenuActivated( int id )
{
    if ( id == ContextSelect )
    {
        // Set m_lvi to 0 to let rightButtonClicked() know we're done with
        // the dialog
        m_listView->setSelected( m_lvi, true );
        m_lvi = 0;
    }
    else if ( id == ContextPrintVisibleView )
    {
        printWidget( m_page->m_contentFrame );
    }
    else if ( id == ContextPrintEntireView )
    {
        printListView( m_listView );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Double-click callback slot to store the clicked item,
 *  drop the dialog, and return Accepted.
 *
 *  \return Reference to a QString containing the current catalog file name.
 */

void GeoPlaceDialog::doubleClicked( QListViewItem *lvi )
{
    // Must click an item
    if ( lvi )
    {
        m_listView->setSelected( lvi, true );
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Lists or selects the item.
 */

void GeoPlaceDialog::rightButtonClicked( QListViewItem *lvi,
    const QPoint &, int )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );
    m_lvi = lvi;

    // If cursor is over an item...
    int mid;
    QString text("");
    if ( lvi )
    {
        translate( text, "GeoCatalogDialog:ContextMenu:Select" );
        mid = m_contextMenu->insertItem( text,
                this, SLOT( contextMenuActivated( int ) ) );
        m_contextMenu->setItemParameter( mid, ContextSelect );
    }

    translate( text, "GeoCatalogDialog:ContextMenu:PrintVisible" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintVisibleView );

    translate( text, "GeoCatalogDialog:ContextMenu:PrintEntire" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrintEntireView );

    // Show the context menu
    m_contextMenu->exec( QCursor::pos() );
    delete m_contextMenu;   m_contextMenu = 0;

    // If m_lvi has been reset to 0 by contextMenuActivated(),
    // then the user made a selection and we're done with the dialog
    if ( lvi && ! m_lvi )
    {
        store();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Ok button callback slot to store the clicked item,
 *  drop the dialog, and return Accepted.
 */

void GeoPlaceDialog::store( void )
{
    QListViewItem *lvi = m_listView->selectedItem();
    if ( lvi )
    {
        m_state  = lvi->text( 0 );
        m_county = lvi->text( 1 );
        m_place  = lvi->text( 2 );
        m_type   = lvi->text( 3 );
        m_lon    = lvi->text( 4 );
        m_lat    = lvi->text( 5 );
        m_elev   = lvi->text( 6 );
        m_gmt    = lvi->text( 7 );
    }
    else
    {
        m_state  = "";
        m_county = "";
        m_place  = "";
        m_type   = "";
        m_lon    = "";
        m_lat    = "";
        m_elev   = "";
        m_gmt    = "";
    }
    // Consume the click and return
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of sundialog.cpp
//------------------------------------------------------------------------------

