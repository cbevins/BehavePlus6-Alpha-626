//------------------------------------------------------------------------------
/*! \file fuelwizards.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument fuel parameter wizards, including
 *  \arg FuelBedDepthWizard
 *  \arg FuelBedMextDeadWizard
 *  \arg FuelHeatWizard
 *  \arg FuelLoadDead1Wizard
 *  \arg FuelLoadDead10Wizard
 *  \arg FuelLoadDead100Wizard
 *  \arg FuelLoadLiveWizard
 *  \arg FuelSavrDead1Wizard
 *  \arg FuelSavrLiveWizard
 *  \arg PalmettoAgeWizard
 *  \arg PalmettoCoverWizard
 *  \arg PalmettoOverstoryBasalAreaWizard
 */

// Custom include files
#include "appsiunits.h"
#include "apptranslator.h"
#include "fuelwizards.h"
#include "xeqvar.h"

// Qt include files
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*  \brief FuelWizard constructor.
 *  Base class for fuel parameter wizards.
 *
 *  \param bp           Pointer to the parent BpDocument
 *  \param var          Pointer to the subject EqVar
 *  \param items        Number of values in the passed \a value[] array
 *  \param value        Array of suggested values
 *  \param textKey      Array of translator keys to descriptive text
 *                      that corresponds to the values
 *  \param pictureFile  Name of the picture file
 *  \param pictureName  Name (title) of the picture itself
 *  \param name         Widget's internal name
 */

FuelWizard::FuelWizard( BpDocument *bp, EqVar *var, unsigned int items,
        const double value[], const QString modelKey[],
        const QString &pictureFile, const QString &pictureName,
        const char *name, const QString &headerKey ) :
    WizardDialog( bp, var, pictureFile, pictureName, name )
{
    // Determine if the current display units are english, metric, or neither
    bool showEnglishUnits = ! appSiUnits()->equivalent(
        m_var->m_displayUnits.latin1(), m_var->m_englishUnits.latin1() );

    bool showMetric = ! appSiUnits()->equivalent(
        m_var->m_displayUnits.latin1(), m_var->m_metricUnits.latin1() );

    // Override the default columns
    QString text( "" );
    translate( text, headerKey );  // "Fuel Models"
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    int col = 1;

    // Show English units value only if its not already in first column
    if ( showEnglishUnits )
    {
        col++;
        m_listView->addColumn( m_var->m_englishUnits );
        m_listView->setColumnWidthMode( col, QListView::Maximum );
    }
    // Show Metric value only if its not already in first column
    if ( showMetric )
    {
        col++;
        m_listView->addColumn( m_var->m_metricUnits );
        m_listView->setColumnWidthMode( col, QListView::Maximum );
    }
    // Add the list items
    QListViewItem *item;
    QString str;
    double val;
    for ( unsigned int id = 0;
          id < items;
          id++ )
    {
        m_var->nativeValue( value[id] );
        str.sprintf( "%8.*f ", m_var->m_displayDecimals, m_var->m_displayValue );

        translate( text, modelKey[id] );
        item = new QListViewItem( m_listView, str, text );
        Q_CHECK_PTR( item );
        col = 2;

        if ( showEnglishUnits )
        {
            appSiUnits()->convert( value[id], m_var->m_nativeUnits.latin1(),
                m_var->m_englishUnits.latin1(), &val );
            str.sprintf( "%8.*f ", m_var->m_englishDecimals, val );
            item->setText( col++, str );
        }
        if ( showMetric )
        {
            appSiUnits()->convert( value[id], m_var->m_nativeUnits.latin1(),
                m_var->m_metricUnits.latin1(), &val );
            str.sprintf( "%8.*f ", m_var->m_metricDecimals, val );
            item->setText( col++, str );
        }
    }
    // Make the column at least as wide as its header
    QFontMetrics fm( m_listView->header()->font() );
    for ( int c = 0;
          c < col;
          c++ )
    {
        int colWidth = m_listView->columnWidth( c );
        int hdrWidth = fm.width( m_listView->columnText( c ) ) + 10;
        if ( colWidth < hdrWidth )
        {
            m_listView->setColumnWidthMode( c, QListView::Manual );
            m_listView->setColumnWidth( c, hdrWidth );
        }
    }
    // Override the default listview min and max widths
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );

    // Set the initial size for this widget so the entire picture and
    // a good chunk of both the listView and textBrowser are visible.
    int width = widthHint() + m_listView->sizeHint().width();
    resize( width, sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Fuel bed depth Wizard constructor.
 */

static const double BedDepthValues[] =
{   0.2,    1.0,            2.0, 2.3,  2.5,       3.0,  6.0 };

static const QString BedDepthModels[] =
{   "!8, 9", "!1, 2, 10, 11", "!5", "!12", "!3, 6, 7", "!13", "!4" };

FuelBedDepthWizard::FuelBedDepthWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(BedDepthValues)/sizeof(double),
        BedDepthValues,
        BedDepthModels,
        "Mirage1.png",
        "Mirage",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Fuel bed dead extinction moisture content Wizard constructor.
 */

static const double BedMextDeadValues[] =
{   0.12,   0.15,     0.20,        0.25,             0.30, 0.40 };

static const QString BedMextDeadModels[] =
{   "!1", "!2, 11", "!4, 5, 12", "!3, 6, 9, 10, 13", "!8", "!7" };

FuelBedMextDeadWizard::FuelBedMextDeadWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(BedMextDeadValues)/sizeof(double),
        BedMextDeadValues,
        BedMextDeadModels,
        "Mirage2.png",
        "Mirage",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Fuel heat of combustion Wizard constructor.
 */

static const double HeatValues[] =
{   6000., 7000., 8000.0, 9000., 10000., 11000., 12000. };

static const QString HeatModels[] =
{
    "Wizard:Fuel:Heat:6000", "",
    "Wizard:Fuel:Heat:8000", "",
    "Wizard:Fuel:Heat:10000", "",
    "Wizard:Fuel:Heat:12000"
};

FuelHeatWizard::FuelHeatWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(HeatValues)/sizeof(double),
        HeatValues,
        HeatModels,
        "SunsetOnNinepipes.png",
        "Sunset On Ninepipes",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dead 1-h fuel load Wizard constructor.
 */

static const double LoadDead1Values[] =
{ 0.034, 0.046, 0.052, 0.069, 0.092, 0.134, 0.138, 0.184, 0.230, 0.322 };

static const QString LoadDead1Models[] =
{ "!1",  "!5",  "!7",  "!6, 8","!2", "!9", "!3, 10","!12", "!4", "!13" };

FuelLoadDead1Wizard::FuelLoadDead1Wizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(LoadDead1Values)/sizeof(double),
        LoadDead1Values,
        LoadDead1Models,
        "RestoringAmericasForests5.png",
        "Restoring America's Forests",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dead 10-h fuel load Wizard constructor.
 */

static const double LoadDead10Values[] =
{ 0.000, 0.019, 0.023, 0.046, 0.086, 0.092, 0.115, 0.184, 0.207, 0.644, 1.058 };

static const QString LoadDead10Models[] =
{"!1, 3", "!9", "!5", "!2, 8", "!7", "!10",  "!6", "!4",  "!11", "!12", "!13" };

FuelLoadDead10Wizard::FuelLoadDead10Wizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(LoadDead10Values)/sizeof(double),
        LoadDead10Values,
        LoadDead10Models,
        "RestoringOurPrairieGrasslands1.png",
        "Restoring Our Prairie Grasslands",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dead 100-h fuel load Wizard constructor.
 */

static const double LoadDead100Values[] =
{ 0.000,   0.007, 0.023, 0.069, 0.092, 0.115, 0.230, 0.253, 0.759, 1.288 };

static const QString LoadDead100Models[] =
{"!1, 3, 5", "!9", "!2", "!7", "!4, 6", "!8", "!10", "!11", "!12", "!13" };

FuelLoadDead100Wizard::FuelLoadDead100Wizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(LoadDead100Values)/sizeof(double),
        LoadDead100Values,
        LoadDead100Models,
        "RestoringOurPrairieGrasslands2.png",
        "Restoring Our Prairie Grasslands",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Live herbaceous and woody fuel load Wizard constructor.
 */

static const double LoadLiveValues[] = { 0.000, 0.017, 0.023, 0.092, 0.230 };

static const QString LoadLiveModels[] =
{   "!1, 3, 6, 8, 9, 11, 12, 13",
    "Wizard:Fuel:LiveLoad:017",
    "Wizard:Fuel:LiveLoad:023",
    "Wizard:Fuel:LiveLoad:092",
    "Wizard:Fuel:LiveLoad:230"
};

FuelLoadLiveWizard::FuelLoadLiveWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(LoadLiveValues)/sizeof(double),
        LoadLiveValues,
        LoadLiveModels,
        "RestoringAmericasForests4.png",
        "Restoring America's Forests",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dead 1-h fuel surface area-to-volume ratio wizard constructor.
 */

static const double SavrDead1Values[] =
{ 1500.,             1750.,   2000.,        2500., 3000., 3500. };

static const QString SavrDead1Models[] =
{ "!3, 11, 12, 13", "!6, 7", "!4, 5, 8, 10", "!9", "!2", "!1" };

FuelSavrDead1Wizard::FuelSavrDead1Wizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(SavrDead1Values)/sizeof(double),
        SavrDead1Values,
        SavrDead1Models,
        "MontanasPineButteSwamp1.png",
        "Montana's Pine Butte Swamp Preserve",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Live herbaceous and woody fuel surface area-to-volume ratio wizard.
 */

static const double SavrLiveValues[] = { 1500. };

static const QString SavrLiveModels[] = { "!2, 4, 5, 7, 10" };

FuelSavrLiveWizard::FuelSavrLiveWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(SavrLiveValues)/sizeof(double),
        SavrLiveValues,
        SavrLiveModels,
        "MontanasPineButteSwamp2.png",
        "Montana's Pine Butte Swamp Preserve",
        name )
{
    return;
}
//------------------------------------------------------------------------------
/*! \brief Heat per unit area Wizard constructor.
 */

static const double HpuaValues[] = { 580., 760., 1050., 1325., 1325., 1570., 3430. };

static const QString HpuaText[] =
{   "!8",
    "!2",
	"!9",
	"!9 + 30 ton/ac",
	"!10",
	"!10 + 30 ton/ac",
	"!12"
};

HeatPerUnitAreaWizard::HeatPerUnitAreaWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(HpuaValues)/sizeof(double),
        HpuaValues,
        HpuaText,
        "RestoringAmericasForests4.png",
        "Restoring America's Forests",
        name )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Palmetto age wizard constructor.
 */

static const double PalmettoAgeValues[] =
{   1., 2., 3., 5., 8., 15., 25. };

static const QString PalmettoAgeModels[] =
{
    "Wizard:Fuel:PalmettoAge:1",
    "Wizard:Fuel:PalmettoAge:2",
    "Wizard:Fuel:PalmettoAge:3",
    "Wizard:Fuel:PalmettoAge:5",
    "Wizard:Fuel:PalmettoAge:8",
    "Wizard:Fuel:PalmettoAge:15",
    "Wizard:Fuel:PalmettoAge:25"
};

PalmettoAgeWizard::PalmettoAgeWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(PalmettoAgeValues)/sizeof(double),
        PalmettoAgeValues,
        PalmettoAgeModels,
        "WhiteCliffsOfTheMissouri1.png",
        "White Cliffs Of The Missouri",
        name, "Wizard:Fuel:Description" )
{
    return;
}


//------------------------------------------------------------------------------
/*! \brief Palmetto cover wizard constructor.
 */

static const double PalmettoCoverValues[] =
{   15., 25., 50., 75., 85. };

static const QString PalmettoCoverModels[] =
{
    "", "Wizard:Fuel:PalmettoCover:15",
    "", "Wizard:Fuel:PalmettoCover:50",
    "", "Wizard:Fuel:PalmettoCover:85", ""
};

PalmettoCoverWizard::PalmettoCoverWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(PalmettoCoverValues)/sizeof(double),
        PalmettoCoverValues,
        PalmettoCoverModels,
        "ReturnOfLakeMissoula1.png",
        "Return of Lake Missoula",
        name, "Wizard:Fuel:Description" )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Palmetto overstory basal area wizard constructor.
 */

static const double PalmettoOverstoryBasalAreaValues[] =
{  30., 50., 70., 90., 110. };

static const QString PalmettoOverstoryBasalAreaModels[] =
{
    "", "Wizard:Fuel:PalmettoOverstoryBasalArea:30",
    "", "Wizard:Fuel:PalmettoOverstoryBasalArea:70",
    "", "Wizard:Fuel:PalmettoOverstoryBasalArea:110", ""
};

PalmettoOverstoryBasalAreaWizard::PalmettoOverstoryBasalAreaWizard(
        BpDocument *bp, EqVar *var, const char *name ) :
    FuelWizard( bp, var,
        sizeof(PalmettoOverstoryBasalAreaValues)/sizeof(double),
        PalmettoOverstoryBasalAreaValues,
        PalmettoOverstoryBasalAreaModels,
        "WhiteCliffsOfTheMissouri2.png",
        "White Cliffs Of The Missouri",
        name, "Wizard:Fuel:Description" )
{
    return;
}

//------------------------------------------------------------------------------
//  End of fuelwizards.cpp
//------------------------------------------------------------------------------

