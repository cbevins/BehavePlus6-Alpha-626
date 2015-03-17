//------------------------------------------------------------------------------
/*! \file standardwizards.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument standard wizards.
 *
 *  Includes methods for:
 *  - BarkThicknessWizard
 *  - CanopyBulkDensityWizard
 *  - CompassNorthWizard
 *  - CompassUpslopeWizard
 *  - CrownRatioWizard
 *  - FireLineIntWizard
 *  - LiveMoistureWizard
 *  - MapFractionWizard
 *  - SafetyZoneEquipmentAreaWizard
 *  - SafetyZonePersonnelAreaWizard
 *  - SlopeDegreesWizard
 *  - SlopeFractionWizard
 *  - WindAdjWizard
 */

// Custom include files
#include "appsiunits.h"
#include "apptranslator.h"
#include "fuelmodel.h"
#include "property.h"
#include "standardwizards.h"
#include "xeqapp.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qstring.h>

// Needed only to support WindAdjWizard
#include "bpdocument.h"
#include "parser.h"
#include "siunits.h"
#include <stdlib.h>
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief Tree bark thickness wizard constructor.
 */

static const double BarkThicknessValue[] =
{
    0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00, 1.50, 2.00
};

static const QString BarkThicknessKeys[] =
{ "", "", "", "", "", "", "", "", "", "", "", "" };

BarkThicknessWizard::BarkThicknessWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(BarkThicknessValue)/sizeof(double)),    // number of values
        BarkThicknessValue,         // array of suggested values
        BarkThicknessKeys,          // array of text keys for each value
        "",                         // value column text key
        "",                         // desc column text key
        "ZooCity1.png",             // picture file name
        "Zoo City",                 // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Canopy bulk density wizard.
 */

static const double CanopyBulkDensityValue[] =
{
    0.006,  0.012, 0.019, 0.025, 0.031 //, 0.037, 0.044, 0.050, 0.056, 0.062
};
static const QString CanopyBulkDensityKeys[] =
{
    "Wizard:CanopyBulkDensity:01",
    "Wizard:CanopyBulkDensity:02",
    "Wizard:CanopyBulkDensity:03",
    "Wizard:CanopyBulkDensity:04",
    "Wizard:CanopyBulkDensity:05"//,
    //"Wizard:CanopyBulkDensity:06",
    //"Wizard:CanopyBulkDensity:07",
    //"Wizard:CanopyBulkDensity:08",
    //"Wizard:CanopyBulkDensity:09",
    //"Wizard:CanopyBulkDensity:10"
};

CanopyBulkDensityWizard::CanopyBulkDensityWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(CanopyBulkDensityValue)/sizeof(double)), // number of values
        CanopyBulkDensityValue,             // array of suggested values
        CanopyBulkDensityKeys,              // array of text keys for each value
        "",                                 // value column text key
        "Wizard:CanopyBulkDensity:Col1",    // desc column text key
        "Ascension.png",                    // picture file name
        "Ascension",                        // picture title
        name )                              // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Compass Degrees from North wizard constructor.
 */

static const double CompassValue[] =
{
      0.,  15.,  30.,  45.,  60.,  75.,  90., 105., 120., 135., 150., 165.,
    180., 195., 210., 225., 240., 255., 270., 285., 300., 315., 330., 345., 360.
};

static const QString CompassNorthKeys[] =
{
    "Wizard:Compass:North", "", "",
    "Wizard:Compass:NE", "", "",
    "Wizard:Compas:East", "", "",
    "Wizard:Compass:SE", "", "",
    "Wizard:Compass:South", "", "",
    "Wizard:Compass:SW", "", "",
    "Wizard:Compass:West", "", "",
    "Wizard:Compass:NW", "", "",
    "Wizard:Compass:North"
};

CompassNorthWizard::CompassNorthWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(CompassValue)/sizeof(double)),  // number of values
        CompassValue,               // array of suggested values
        CompassNorthKeys,           // array of text keys for each value
        "Wizard:Compass:Col0",      // value column text key
        "Wizard:Compass:Col1",      // desc column text key
        "Ripple.png",               // picture file name
        "Ripple",                   // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Compass Degrees from Upslope wizard constructor.
 */

static const QString CompassUpslopeKeys[] =
{
    "Wizard:Upslope:UpSlope",   "", "", "", "", "",
    "Wizard:Upslope:CrossSlope", "", "", "", "", "",
    "Wizard:Upslope:DownSlope", "", "", "", "", "",
    "Wizard:Upslope:CrossSlope", "", "", "", "", "",
    "Wizard:Upslope:UpSlope"
};

CompassUpslopeWizard::CompassUpslopeWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(CompassValue)/sizeof(double)),  // number of values
        CompassValue,               // array of suggested values
        CompassUpslopeKeys,         // array of text keys for each value
        "Wizard:Upslope:Col0",      // value column text key
        "Wizard:Upslope:Col1",      // desc column text key
        "Ripple.png",               // picture file name
        "Ripple",                   // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Crown ratio wizard.
*/

static const double CrownRatioValue[] =
{ 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 };

static const QString CrownRatioKeys[] =
{ "", "", "", "", "", "", "", "", "", "" };

CrownRatioWizard::CrownRatioWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(CrownRatioValue)/sizeof(double)),   // number of values
        CrownRatioValue,            // array of suggested values
        CrownRatioKeys,             // array of text keys for each value
        "Wizard:CrownRatio:Col0",   // value column text key
        "",                         // desc column text key
        "HarvestBread.png",         // picture file name
        "Harvest Bread",            // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Fireline intensity wizard constructor.
 */

static const double FireLineIntValue[] =
{
  0.,    1.,    2.,     3.,    4.,    5.,    6.,    7.,    8.,
  10.,   20.,   30.,    40.,   50.,   60.,   70.,   80.,   90., 100.,
 125.,  150.,  200.,   300.,  400.,  500.,  600.,  750., 1000.,
1250., 1500., 1750.,  2000., 2500., 3000., 4000., 5000., 6000.,
7000., 8000., 9000., 10000.
};

static const QString FireLineIntKeys[] =
{
"!0.00", "!0.45", "!0.61", "!0.74", "!0.85", "!0.94", "! 1.0", "! 1.1", "! 1.2",
"! 1.3", "! 1.8", "! 2.2", "! 2.5", "! 2.7", "! 3.0", "! 3.2", "! 3.4", "! 3.6",
"! 3.7", "! 4.1", "! 4.5", "! 5.1", "! 6.2", "! 7.1", "! 7.8", "! 8.5", "! 9.5",
"!10.8", "!12.0", "!13.0", "!14.0", "!14.8", "!16.5", "!17.9", "!20.4", "!23.6",
"!24.6", "!26.4", "!28.1", "!29.6", "!31.0"
};

FireLineIntWizard::FireLineIntWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(FireLineIntValue)/sizeof(double)),  // number of values
        FireLineIntValue,           // array of suggested values
        FireLineIntKeys,            // array of text keys for each value
        "",                         // value column text key
        "Wizard:FireLineInt:Col0",  // desc column text key
        "SnowballInHell.png",       // picture file name
        "Snowball",                 // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Live Herbaceous Fuel Moisture Wizard constructor.
 */

static const double  LiveHerbMoisValue[] =
    { 3.0, 2.0, 1.0, 0.5, 1.20, 0.98, 0.90, 0.75, 0.60, 0.53, 0.30 };
static const QString LiveHerbMoisKeys[] = {
  "Wizard:LiveFuelMois:3.0",
  "Wizard:LiveFuelMois:2.0",
  "Wizard:LiveFuelMois:1.0",
  "Wizard:LiveFuelMois:0.5",
  "Wizard:LiveFuelMois:120",
  "Wizard:LiveFuelMois:098",
  "Wizard:LiveFuelMois:090",
  "Wizard:LiveFuelMois:075",
  "Wizard:LiveFuelMois:060",
  "Wizard:LiveFuelMois:053",
  "Wizard:LiveFuelMois:030"
};

LiveHerbMoistureWizard::LiveHerbMoistureWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(LiveHerbMoisValue)/sizeof(double)), // number of values
        LiveHerbMoisValue,                  // array of suggested values
        LiveHerbMoisKeys,                   // array of text keys for each value
        "",                             // value column text key
        "Wizard:LiveFuelMois:Col1",     // desc column text key
        "RestoringOurPrairieGrasslands3.png",
        "Restoring Our Prairie Grasslands",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Live Woody Fuel Moisture Wizard constructor.
 */

static const double  LiveWoodMoisValue[] = { 3.0, 2.0, 1.0, 0.5 };
static const QString LiveWoodMoisKeys[] = {
  "Wizard:LiveFuelMois:3.0",
  "Wizard:LiveFuelMois:2.0",
  "Wizard:LiveFuelMois:1.0",
  "Wizard:LiveFuelMois:0.5"
};

LiveWoodMoistureWizard::LiveWoodMoistureWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(LiveWoodMoisValue)/sizeof(double)), // number of values
        LiveWoodMoisValue,                  // array of suggested values
        LiveWoodMoisKeys,                   // array of text keys for each value
        "",                             // value column text key
        "Wizard:LiveFuelMois:Col1",     // desc column text key
        "Resurrection.png",             // picture file name
        "Resurrection",                 // picture title
        name )
{
    return;
}

//------------------------------------------------------------------------------
/* \brief MapFractionWizard constructor.
 *
 *  While this does not inherit StandardWizard, it uses many of the same
 *  mechanisms to produce a listview with 5 columns instead of 2.
 */

static const double MapRf[] =
{
    1980., 3960., 7920., 10000., 15840., 21120., 24000., 31680.,
    50000., 62500., 63360., 100000., 126720., 250000.,
	253440., 506880., 1000000., 1013760.
};

MapFractionWizard::MapFractionWizard( BpDocument *bp, EqVar *var,
        const char *name ) :
    WizardDialog( bp, var, "CliffSwallows.png", "Cliff Swallows", name )
{
    // Create 5 columns
    QString text("");
    m_listView->setColumnText( 0, "1:x" );
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    translate( text, "Wizard:MapFraction:In/Mi" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Mi/In" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 2, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Cm/Km" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 3, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Km/Cm" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 4, QListView::Maximum );
    // Sort on the mi/in
    m_listView->setSorting( 2, true );

    // Add the list items
    unsigned int items = sizeof(MapRf) / sizeof(double);
    QListViewItem *item;
    QString str0, str1, str2, str3, str4;
    for ( unsigned int id = 0;
          id < items;
          id++ )
    {
        str0.sprintf( "%8.0f", MapRf[id] );             // representative fraction
        str1.sprintf( "%7.4f", 63360. / MapRf[id] );    // in/mi
        str2.sprintf( "%8.5f", MapRf[id] / 63360. );    // mi/in
        str3.sprintf( "%7.4f", 100000. / MapRf[id] );   // cm/km
        str4.sprintf( "%7.4f", MapRf[id] / 100000. );   // km/cm
        item = new QListViewItem( m_listView, str0, str1, str2, str3, str4 );
        Q_CHECK_PTR( item );
    }
    // Override the default listview min and max widths
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );

    // Set the initial size for this widget so the entire picture,
    // a good chunk of both the listView and textBrowser are visible.
    int width = widthHint() + m_listView->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

MapFractionWizard::~MapFractionWizard( void )
{
    delete m_listView;  m_listView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Safety Zone Equipment Area Wizard constructor.
 */

static const double  EquipAreaValue[] = { 200., 280., 360. };
static const QString EquipAreaKeys[] = {
  "Wizard:Safety:EquipArea:CrewCab",
  "Wizard:Safety:EquipArea:D6",
  "Wizard:Safety:EquipArea:D8"
};

SafetyZoneEquipmentAreaWizard::SafetyZoneEquipmentAreaWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(EquipAreaValue)/sizeof(double)),// number of values
        EquipAreaValue,                         // array of suggested values
        EquipAreaKeys,                          // array of text keys for each value
        "",                                     // value column text key
        "Wizard:Safety:EquipArea:Col1",         // desc column text key
        "RockCreek2000.png",                    // picture file name
        "Rock Creek (2000)",                    // picture title
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Safety Zone Personnel Area Wizard constructor.
 */

static const double  PersonAreaValue[] = { 50. };
static const QString PersonAreaKeys[] = {
  "Wizard:Safety:PersonArea:Recommended"
};

SafetyZonePersonnelAreaWizard::SafetyZonePersonnelAreaWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(PersonAreaValue)/sizeof(double)),// number of values
        PersonAreaValue,                        // array of suggested values
        PersonAreaKeys,                         // array of text keys for each value
        "",                                     // value column text key
        "Wizard:Safety:PersonArea:Col1",        // desc column text key
        "Garnet.png",                           // picture file name
        "Garnet - A Montana Ghost Town",        // picture title
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slope steepness (degrees) wizard.
 */

static const double SlopeDegreesValue[] =
{
         0.,     3.,     6.,      9.,      11.,    17.,     22.,      27.,
        31.,    35.,    39.,     42.,      45.,    48.,     50.,      56.,
        63.,    68.,    72.,     74.,      76.,    77.,     79.,      80.,
        81.
};

static const QString SlopeDegreesKeys[] =
{
      "!0%",   "!5%",  "!10%",  "!15%",  "!20%",  "!30%",  "!40%",  "!50%",
     "!60%",  "!70%",  "!80%",  "!90%", "!100%", "!110%", "!120%", "!150%",
    "!200%", "!250%", "!300%", "!350%", "!400%", "!450%", "!500%", "!550%",
    "!600%"
};

SlopeDegreesWizard::SlopeDegreesWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(SlopeDegreesValue)/sizeof(double)), // number of values
        SlopeDegreesValue,          // array of suggested values
        SlopeDegreesKeys,           // array of text keys for each value
        "Wizard:Slope:Degrees",     // value column text key
        "Wizard:Slope:Percent",     // desc column text key
        "WesternTanagers.png",      // picture file name
        "Western Tanagers",         // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/* \brief Slope steepness (rise/reach) wizard.
 */

static const double SlopeFractionValue[] =
{
      0.00,  0.05,  0.10,  0.15,  0.20,  0.30,  0.40,  0.50,  0.60,  0.70,
      0.80,  0.90,  1.00,  1.10,  1.20, 1.50,   2.00,  2.50,  3.00,  3.50,
      4.00,  4.50,  5.00,  5.50,  6.00
};

static const QString SlopeFractionKeys[] =
{
      "!0",  "!3",  "!6",  "!9", "!11", "!17", "!22", "!27", "!31", "!35",
     "!39", "!42", "!45", "!48", "!50", "!56", "!63", "!68", "!72", "!74",
     "!76", "!77", "!79", "!80", "!81"
};

SlopeFractionWizard::SlopeFractionWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(SlopeFractionValue)/sizeof(double)),    // number of values
        SlopeFractionValue,         // array of suggested values
        SlopeFractionKeys,          // array of text keys for each value
        "Wizard:Slope:Percent",     // value column text key
        "Wizard:Slope:Degrees",     // desc column text key
        "Yellowbells.png",          // picture file name
        "Yellow Bells",             // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
/*  \brief StandardWizard constructor.
 *  This is the base class to the following wizards:
 *  \arg BarkThicknessWizard
 *  \arg CompassNorthWizard
 *  \arg CompassUpslopeWizard
 *  \arg CrownRatioWizard
 *  \arg FireLineIntWizard
 *  \arg LiveMoistureWizard
 *  \arg SlopeDegreesWizard
 *  \arg SlopeFractionWizard
 *
 *  \param bp           Pointer to the parent BpDocument
 *  \param var          Pointer to the subject EqVar
 *  \param items        Number of values in the passed \a value[] array
 *  \param value        Array of suggested values
 *  \param textKey      Array of translator keys to descriptive text
 *                      that corresponds to the values
 *  \param col0Key      First column label translator key.
 *                      If empty or NULL, \a var's m_displayUnits are used.
 *  \param col1Key      Second column label translator key
 *  \param pictureFile  Name of the picture file
 *  \param pictureName  Name (title) of the picture itself
 *  \param name         Widget's internal name
 */

StandardWizard::StandardWizard( BpDocument *bp, EqVar *var,
        unsigned int items,
        const double value[],
        const QString textKey[],
        const QString &col0Key,
        const QString &col1Key,
        const QString &pictureFile,
        const QString &pictureName,
        const char *name ) :
    WizardDialog( bp, var, pictureFile, pictureName, name )
{
    QString text("");
    // If no value column header, display the units
    if ( col0Key.isNull() || col0Key.isEmpty() )
    {
        m_listView->setColumnText( 0, var->m_displayUnits );
    }
    // Otherwise display the text key.
    else
    {
        translate( text, col0Key );
        m_listView->setColumnText( 0, text );
    }
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    translate( text, col1Key );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    m_listView->setSorting( -1 );

    // Add the list items
    QListViewItem *item;
    QString str;
    for ( int id = items-1;
          id >= 0;
          id-- )
    {
        var->nativeValue( value[id] );
        str = QString( "%1" ).arg( var->m_displayValue, 10, 'f', var->m_displayDecimals ),
        translate( text, textKey[id] );
        item = new QListViewItem( m_listView, str, text );
        Q_CHECK_PTR( item );
    }
    // Make the column at least as wide as its header
    QFontMetrics fm( m_listView->header()->font() );
    for ( int col = 0;
          col < 2;
          col++ )
    {
        int colWidth = m_listView->columnWidth( col );
        int hdrWidth = fm.width( m_listView->columnText( col ) ) + 10;
        if ( colWidth < hdrWidth )
        {
            m_listView->setColumnWidthMode( col, QListView::Manual );
            m_listView->setColumnWidth( col, hdrWidth );
        }
    }
    // Override the default listview min and max widths
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );

    // Set the initial size for this widget so the entire picture,
    // a good chunk of both the listView and textBrowser are visible.
    int width = widthHint() + m_listView->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*  \brief WindAdjWizard constructor.
 *
 *  While this doesn't inherit StandardWizard, it uses many of the same
 *  mechanisms.
 */

// Must be in reverse order from appearance order desired in the QListView
static const double AdjValues[] =  { 0.1, 0.2, 0.3, 0.3, 0.4, 0.5 };
static const QString AdjKeys[] =
{
    "Wizard:WindAdj:0",
    "Wizard:WindAdj:1",
    "Wizard:WindAdj:2",
    "Wizard:WindAdj:3",
    "Wizard:WindAdj:4",
    "Wizard:WindAdj:5"
};

WindAdjWizard::WindAdjWizard( BpDocument *bp, EqVar *var, const char *name ) :
        StandardWizard( bp, var,
        (sizeof(AdjValues)/sizeof(double)),  // number of values
        AdjValues,                  // array of suggested values
        AdjKeys,                    // array of text keys for each value
        "Wizard:WindAdj:Col0",      // value column text key
        "Wizard:WindAdj:Col1",      // desc column text key
        "WatershedPreservation.png",// picture file name
        "Watershed Preservation",   // picture title
        name )                      // internal widget name
{
    return;
}

//------------------------------------------------------------------------------
//  End of standardwizards.cpp
//------------------------------------------------------------------------------

