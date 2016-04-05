//------------------------------------------------------------------------------
/*! \file unitseditdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2015 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Units editor dialog methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocument.h"
#include "unitseditdialog.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

//------------------------------------------------------------------------------
// Units of measure QComboBox choices.
//------------------------------------------------------------------------------

static const char *BasalAreaUnits[] =
    { "ft2/ac", "m2/ha", NULL };

static const char *DistLongUnits[] =
    { "mi", "km", "ch", "ft", "m", NULL };

static const char *DistMedUnits[] =
    { "ft", "m", "in", "cm", "ch", NULL };

static const char *DistShortUnits[] =
    { "in", "cm", "ft", "m", NULL };

static const char *DistMapUnits[] =
    { "in", "cm", NULL };

static const char *ElevationUnits[] =
    { "ft", "m", "ch", NULL };

static const char *FireAreaUnits[] =
    { "ac", "ha", "ft2", "m2", "mi2", "km2", NULL };

static const char *FireDistUnits[] =
    { "ch", "m", "ft", "mi", "km", NULL };

static const char *FireHpuaUnits[] =
    { "Btu/ft2", "kW-s/m2", "kJ/m2",NULL };

static const char *FireFlameUnits[] =
    { "ft", "m", "in", "cm", "ch", NULL };

static const char *FireLineIntUnits[] =
    { "Btu/ft/s", "kJ/m/s", "Btu/ft/min", "kJ/m/min", "kW/m", NULL };

static const char *FirePowerUnits[] =
    { "ft-lb/s/ft2", "m-kg/s/m2", NULL };

static const char *FireRxIntUnits[] =
    { "Btu/ft2/min", "kJ/m2/min", "Btu/ft2/s", "kJ/m2/s", "kW/m2", NULL };

static const char *FireSpreadUnits[] =
    { "ch/h", "m/min", "ft/min", "mi/h", "km/h", "m/h", "m/s",
      "furlongs/fortnight", NULL };

static const char *FuelBulkUnits[] =
    { "lb/ft3", "kg/m3", NULL };

static const char *FuelDepthUnits[] =
    { "ft", "m", "in", "cm", NULL };

static const char *FuelHeatUnits[] =
    { "Btu/lb", "kJ/kg", NULL };

static const char *FuelHeatSinkUnits[] =
    { "Btu/ft3", "kJ/m3", NULL };

static const char *FuelLoadUnits[] =
    { "ton/ac", "tonne/ha", "lb/ft2", "kg/m2", NULL };

static const char *FuelSavrUnits[] =
    { "ft2/ft3", "m2/m3", "in2/in3", "cm2/cm3", NULL } ;

static const char *PercentUnits[] =
    { "%", "fraction", NULL };

static const char *RatioUnits[] =
    { "ratio", NULL };

static const char *SafetyAreaUnits[] =
    { "ft2", "m2", "ac", "ha", "mi2", "km2", NULL };

static const char *SlopeUnits[] =
    { "%", "degrees", NULL };

static const char *TemperatureUnits[] =
    { "oF", "oC", "K", NULL };

static const char *TimeLongUnits[] =
    { "years", NULL };

static const char *TimeShortUnits[] =
    { "h", "min", "s", NULL };

static const char *TimeMedUnits[] =
    { "h", "min", NULL };

static const char *WafUnits[] =
    { "fraction", NULL };

static const char *WindSpeedUnits[] =
    { "mi/h", "m/min", "ft/min", "ch/h", "km/h", "m/s", "furlongs/fortnight", NULL };

//------------------------------------------------------------------------------
// Fuel & Vegetation Units (page 1)
//------------------------------------------------------------------------------

static const char *BasalAreaVars[] =
{
    "vSurfaceFuelPalmettoOverstoryBasalArea",
    NULL
};

static const char *FuelBulkVars[] =
{
    "vSurfaceFuelBedBulkDensity",
    "vSurfaceFuelDens0",
    "vSurfaceFuelDens1",
    "vSurfaceFuelDens2",
    "vSurfaceFuelDens3",
    "vSurfaceFuelDens4",
    "vSurfaceFuelDens5",
    "vSurfaceFuelDens6",
    "vSurfaceFuelDens7",
    NULL
};

static const char *FuelCoverVars[] =
{
    "vTreeCanopyCover",
    "vSurfaceFuelBedCoverage1",
    "vSurfaceFuelPalmettoCover",
    NULL
};

static const char *FuelCrownVars[] =
{
    "vTreeCrownRatio",
    NULL
};

static const char *FuelLoadVars[] =
{
    "vSurfaceFuelAspenLoadDead1",
    "vSurfaceFuelAspenLoadDead10",
    "vSurfaceFuelAspenLoadLiveHerb",
    "vSurfaceFuelAspenLoadLiveWoody",
	"vSurfaceFuelChaparralLoadDead1",
	"vSurfaceFuelChaparralLoadDead2",
	"vSurfaceFuelChaparralLoadDead3",
	"vSurfaceFuelChaparralLoadDead4",
	"vSurfaceFuelChaparralLoadLive1",
	"vSurfaceFuelChaparralLoadLive2",
	"vSurfaceFuelChaparralLoadLive3",
	"vSurfaceFuelChaparralLoadLive4",
	"vSurfaceFuelChaparralLoadLiveLeaf",
	"vSurfaceFuelChaparralLoadTotal",
	"vSurfaceFuelChaparralLoadTotalDead",
	"vSurfaceFuelChaparralLoadTotalLive",
    "vSurfaceFuelLoadDead",
    "vSurfaceFuelLoadDead1",
    "vSurfaceFuelLoadDead10",
    "vSurfaceFuelLoadDead100",
    "vSurfaceFuelLoadDeadHerb",
    "vSurfaceFuelLoadLive",
    "vSurfaceFuelLoadLiveHerb",
    "vSurfaceFuelLoadLiveWood",
    "vSurfaceFuelLoadUndeadHerb",
    "vSurfaceFuelLoad0",
    "vSurfaceFuelLoad1",
    "vSurfaceFuelLoad2",
    "vSurfaceFuelLoad3",
    "vSurfaceFuelLoad4",
    "vSurfaceFuelLoad5",
    "vSurfaceFuelLoad6",
    "vSurfaceFuelLoad7",
    "vSurfaceFuelPalmettoLoadDead1",
    "vSurfaceFuelPalmettoLoadDead10",
    "vSurfaceFuelPalmettoLoadDeadFoliage",
    "vSurfaceFuelPalmettoLoadLitter",
    "vSurfaceFuelPalmettoLoadLive1",
    "vSurfaceFuelPalmettoLoadLive10",
    "vSurfaceFuelPalmettoLoadLiveFoliage",
    "vCrownFireFuelLoad",
    NULL
};

static const char *FuelSavrVars[] =
{
    "vSurfaceFuelAspenSavrDead1",
    "vSurfaceFuelAspenSavrDead10",
    "vSurfaceFuelAspenSavrLiveHerb",
    "vSurfaceFuelAspenSavrLiveWoody",
    "vSurfaceFuelBedSigma",
    "vSurfaceFuelSavrDead1",
    "vSurfaceFuelSavrLiveHerb",
    "vSurfaceFuelSavrLiveWood",
    "vSurfaceFuelSavr0",
    "vSurfaceFuelSavr1",
    "vSurfaceFuelSavr2",
    "vSurfaceFuelSavr3",
    "vSurfaceFuelSavr4",
    "vSurfaceFuelSavr5",
    "vSurfaceFuelSavr6",
    "vSurfaceFuelSavr7",
    NULL
};

static const char *FuelUnderstoryDepthVars[] =
{
    "vSurfaceFuelBedDepth",
	"vSurfaceFuelChaparralDepth",
    "vSurfaceFuelPalmettoHeight",
    NULL
};

static const char *FuelGroundDepthVars[] =
{
    "vSurfaceFuelDuffDepth",
    "vIgnitionLightningDuffDepth",
    NULL
};

static const char *FuelMoisVars[] =
{
    "vSurfaceFuelMoisDead1",
    "vSurfaceFuelMoisDead10",
    "vSurfaceFuelMoisDead100",
    "vSurfaceFuelMoisDead1000",
    "vSurfaceFuelMoisLiveHerb",
    "vSurfaceFuelMoisLiveWood",
    "vSurfaceFuelMoisLifeDead",
    "vSurfaceFuelMoisLifeLive",
    "vSurfaceFuelBedMextDead",
    "vSurfaceFuelBedMextLive",
    "vSurfaceFuelBedMoisDead",
    "vSurfaceFuelBedMoisLive",
    "vSurfaceFuelMois0",
    "vSurfaceFuelMois1",
    "vSurfaceFuelMois2",
    "vSurfaceFuelMois3",
    "vSurfaceFuelMois4",
    "vSurfaceFuelMois5",
    "vSurfaceFuelMois6",
    "vSurfaceFuelMois7",
    "vTreeFoliarMois",
    "vIgnitionFirebrandFuelMois",
    "vIgnitionLightningFuelMois",
    NULL
};

static const char *FuelHeatVars[] =
{
    "vSurfaceFuelHeatDead",
    "vSurfaceFuelHeatLive",
    "vSurfaceFuelHeat0",
    "vSurfaceFuelHeat1",
    "vSurfaceFuelHeat2",
    "vSurfaceFuelHeat3",
    "vSurfaceFuelHeat4",
    "vSurfaceFuelHeat5",
    "vSurfaceFuelHeat6",
    "vSurfaceFuelHeat7",
    NULL
};

static const char *FuelHeatSinkVars[] =
{
    "vSurfaceFuelBedHeatSink",
    NULL
};

static const char *FuelFractionVars[] =
{
    "vSurfaceFuelAspenCuring",
	"vSurfaceFuelChaparralDeadFuelFraction",
    "vSurfaceFuelLoadTransferFraction",
    "vSurfaceFuelBedDeadFraction",
    "vTreeCanopyCrownFraction",
	"vSurfaceFuelBedLiveFraction",
	"vSurfaceFuelBedDeadFraction",
    NULL
};

static const char *FuelPackingRatioVars[] =
{
    "vSurfaceFuelBedBetaRatio",
    "vSurfaceFuelBedPackingRatio",
    NULL
};

static const char *TreeBulkVars[] =
{
    "vTreeCanopyBulkDens",
    NULL
};

static const char *TreeDbhVars[] =
{
    "vTreeDbh",
    "vTreeBarkThickness",
    NULL
};

static const char *TreeHtVars[] =
{
    "vTreeCoverHt",
    "vTreeCoverHtDownwind",
    "vTreeHt",
    "vTreeCrownBaseHt",
    "vTreeCrownLengScorchedAtVector",
    NULL
};

//------------------------------------------------------------------------------
// Weather Units (page 2)
//------------------------------------------------------------------------------

//static const char *HumidityVars[] =
//{                          `
//    "vWthrRelativeHumidity",
//    NULL
//};

static const char *TemperatureVars[] =
{
    "vWthrAirTemp",
    "vWthrWetBulbTemp",
    "vWthrDewPointTemp",
    "vWthrHeatIndex",
    "vWthrSummerSimmerIndex",
    "vWthrWindChillTemp",
    "vSurfaceFuelTemp",
    NULL
};

static const char *WindSpeedVars[] =
{
    "vWindSpeedAtMidflame",
    "vWindSpeedAt20Ft",
    "vWindSpeedAt10M",
    "vSurfaceFireEffWindAtHead",
    "vSurfaceFireEffWindAtVector",
    "vSurfaceFireWindSpeedLimit",
	"vCrownFireActiveCritOpenWindSpeed",
    NULL
};

static const char *WthrCoverVars[] =
{
    "vSiteSunShading",
    NULL
};

static const char *FuelWafVars[] =
{
    "vWindAdjFactor",
    NULL
};

//------------------------------------------------------------------------------
// Terrain & Spotting Units (page 3)
//------------------------------------------------------------------------------

static const char *SlopeSteepnessVars[] =
{
    "vSiteSlopeFraction",
    NULL
};

static const char *ElevationVars[] =
{
    "vSiteSlopeRise",
    "vSiteSlopeReach",
    "vSiteElevation",
    "vSiteRidgeToValleyElev",
    NULL
};

static const char *SiteDistanceVars[] =
{
    "vSiteRidgeToValleyDist",
    NULL
};

static const char *SpotDistanceVars[] =
{
    "vSpotDistActiveCrown",
    "vSpotDistBurningPile",
    "vSpotDistSurfaceFire",
    "vSpotDistTorchingTrees",
    "vSpotFlatDistActiveCrown",
    "vSpotFlatDistBurningPile",
    "vSpotFlatDistSurfaceFire",
    "vSpotFlatDistTorchingTrees",
    "vSpotFirebrandDriftSurfaceFire",
    NULL
};

static const char *SpotHeightVars[] =
{
    "vSpotCoverHtBurningPile",
    "vSpotCoverHtSurfaceFire",
    "vSpotCoverHtTorchingTrees",
    "vSpotFirebrandHtActiveCrown",
    "vSpotFirebrandHtBurningPile",
    "vSpotFirebrandHtSurfaceFire",
    "vSpotFirebrandHtTorchingTrees",
    NULL
};

static const char *SpotRatioVars[] =
{
    "vSpotFlameDurTorchingTrees",
    "vSpotFlameRatioTorchingTrees",
    NULL
};

//------------------------------------------------------------------------------
// Fire & Effects Units (page 4)
//------------------------------------------------------------------------------

static const char *FireSurfaceSpreadVars[] =
{
    "vSurfaceFireSpreadAtHead",
    "vSurfaceFireSpreadAtBack",
    "vSurfaceFireSpreadAtBeta",
    "vSurfaceFireSpreadAtFlank",
    "vSurfaceFireSpreadAtPsi",
    "vSurfaceFireSpreadAtVector",
    "vSurfaceFireNoWindRate",
    "vContainReportSpread",
    NULL
};

static const char *FireCrownSpreadVars[] =
{
	"vCrownFireActiveCritSurfSpreadRate",
	"vCrownFireActiveSpreadRate",
    "vCrownFireCritCrownSpreadRate",
	"vCrownFireCritSurfSpreadRate",
	"vCrownFirePassiveSpreadRate",
    //"vCrownFireSpreadRate",
	NULL
};

static const char *FireHpuaVars[] =
{
	"vCrownFireActiveHeatPerUnitArea",
    "vCrownFireActiveHeatPerUnitArea",
    "vCrownFireHeatPerUnitAreaCanopy",
	"vCrownFirePassiveHeatPerUnitArea",
    "vSurfaceFireHeatPerUnitArea",
NULL
};

static const char *FireLineIntVars[] =
{
    "vCrownFireActiveFireLineInt",
    "vCrownFireCritSurfFireInt",
    //"vCrownFireLineInt",
    "vCrownFirePassiveFireLineInt",
    "vSurfaceFireLineIntAtBeta",
    "vSurfaceFireLineIntAtHead",
    "vSurfaceFireLineIntAtPsi",
    "vSurfaceFireLineIntAtVector",
    NULL
};

static const char *FireFlameLengthVars[] =
{
    "vCrownFireActiveFlameLeng",
    "vCrownFireCritSurfFlameLeng",
    //"vCrownFireFlameLeng",
    "vCrownFirePassiveFlameLeng",
    "vSpotFlameHtActiveCrown",
    "vSpotFlameHtTorchingTrees",
    "vSurfaceFireFlameHtAtVector",
    "vSurfaceFireFlameHtPile",
    "vSurfaceFireFlameLengAtBeta",
    "vSurfaceFireFlameLengAtHead",
    "vSurfaceFireFlameLengAtPsi",
    "vSurfaceFireFlameLengAtVector",
    "vSurfaceFireScorchHtAtVector",
    NULL
};

static const char *FirePowerVars[] =
{
    "vCrownFirePowerOfFire",
    "vCrownFirePowerOfWind",
    NULL
};

//static const char *FireScorchHtVars[] =
//{
//    "vSurfaceFireScorchHtAtVector",
//    NULL
//};

static const char *FireRxIntVars[] =
{
    "vSurfaceFireHeatSource",
    "vSurfaceFireReactionInt",
    "vSurfaceFireReactionIntDead",
    "vSurfaceFireReactionIntLive",
    NULL
};

static const char *FireDistVars[] =
{
    "vCrownFireActiveFireWidth",
    "vCrownFireActiveSpreadDist",
    "vCrownFirePassiveFireWidth",
    "vCrownFirePassiveSpreadDist",
    //"vCrownFireSpreadDist",
    "vSurfaceFireDistAtHead",
    "vSurfaceFireDistAtBack",
    "vSurfaceFireDistAtBeta",
    "vSurfaceFireDistAtFlank",
    "vSurfaceFireDistAtPsi",
    "vSurfaceFireDistAtVector",
	"vSurfaceFireEllipseF",
	"vSurfaceFireEllipseG",
	"vSurfaceFireEllipseH",
    "vSurfaceFireWidthDist",
    "vSurfaceFireLengDist",
    NULL
};

static const char *FireAreaVars[] =
{
    "vContainAttackSize",
    "vContainReportSize",
    "vContainSize",
    "vCrownFireActiveFireArea",
    //"vCrownFireArea",
    "vCrownFirePassiveFireArea",
    "vSurfaceFireArea",
    NULL
};

static const char *FirePerimeterVars[] =
{
    "vContainAttackPerimeter",
    "vCrownFireActiveFirePerimeter",
    "vCrownFirePassiveFirePerimeter",
    //"vCrownFirePerimeter",
    "vSurfaceFirePerimeter",
    NULL
};

static const char *FireFactorVars[] =
{
    "vSurfaceFireSlopeFactor",
    "vSurfaceFireWindFactor",
    "vContainCost",
    "vContainResourceBaseCost",
    "vContainResourceHourCost",
    NULL
};

static const char *FireRatioVars[] =
{
    "vContainReportRatio",
    "vCrownFireActiveRatio",
    "vCrownFireLengthToWidth",
    "vCrownFirePowerRatio",
    "vCrownFireTransRatio",
    "vSurfaceFireEccentricity",
    "vSurfaceFireLengthToWidth",
    "vSurfaceFirePropagatingFlux",
    NULL
};

static const char *TreeScorchRatioVars[] =
{
	"vCrownFireCanopyFractionBurned",
    "vTreeCrownVolScorchedAtVector",
    "vTreeCrownLengFractionScorchedAtVector",
    "vTreeMortalityRateAtVector",
    "vTreeMortalityRateAspenAtVector",
    NULL
};

static const char *IgnitionProbVars[] =
{
    "vIgnitionFirebrandProb",
    "vIgnitionLightningProb",
    NULL
};

//------------------------------------------------------------------------------
// Time & Map Units (page 5)
//------------------------------------------------------------------------------

static const char *FireTimeVars[] =
{
    "vSurfaceFireElapsedTime",
    "vContainResourceArrival",
    "vContainResourceDuration",
    "vContainTime",
    NULL
};

static const char *FlameResidenceTimeVars[] =
{
    "vSurfaceFireResidenceTime",
    NULL
};

static const char *PalmettoAgeVars[] =
{
    "vSurfaceFuelChaparralAge",
    "vSurfaceFuelPalmettoAge",
    NULL
};

static const char *MapElevationVars[] =
{
    "vMapContourInterval",
    NULL
};

static const char *MapDistanceVars[] =
{
    "vCrownFireActiveSpreadMapDist",
    "vCrownFirePassiveSpreadMapDist",
    "vMapDist",
    "vSpotMapDistActiveCrown",
    "vSpotMapDistBurningPile",
    "vSpotMapDistSurfaceFire",
    "vSpotMapDistTorchingTrees",
    "vSurfaceFireMapDistAtHead",
    "vSurfaceFireMapDistAtBack",
    "vSurfaceFireMapDistAtBeta",
    "vSurfaceFireMapDistAtFlank",
    "vSurfaceFireMapDistAtPsi",
    "vSurfaceFireMapDistAtVector",
    "vSurfaceFireWidthMapDist",
    "vSurfaceFireLengMapDist",
    "vSiteRidgeToValleyMapDist",
    NULL
};

//------------------------------------------------------------------------------
// Contain & Safety Units (page 6)
//------------------------------------------------------------------------------

static const char *ContainDistVars[] =
{
    "vContainAttackBack",
    "vContainAttackDist",
    "vContainAttackHead",
    "vContainAttackPerimeter",
    "vContainLimitDist",
    "vContainLine",
    "vContainReportBack",
    "vContainReportHead",
    "vContainXMax",
    "vContainXMin",
    "vContainYMax",
    NULL
};

static const char *ContainRateVars[] =
{
    "vContainResourceProd",
    NULL
};

static const char *SafetyDistVars[] =
{
    "vSafetyZoneLength",
    "vSafetyZoneRadius",
    "vSafetyZoneSepDist",
    NULL
};

static const char *SafetyAreaVars[] =
{
    "vSafetyZoneSize",
    "vSafetyZoneSizeSquare",
    NULL
};

static const char *SafetyPEVars[] =
{
    "vSafetyZoneEquipmentArea",
    "vSafetyZonePersonnelArea",
    NULL
};


//------------------------------------------------------------------------------
/*! \brief UnitsEdit constructor.
 *
 *  \param varList  NULL-terminated list of names of variables affected by this
 *                  units editor QComboBox and QSpinBox.
 *  \param var      Pointer to the EqVar whose name is at varList[0].
 *  \param combo    Pointer to the QComboBox units editor.
 *  \param spin     Pointer to the QSpinBox decimal places editor
 *
 *  Called only by UnitsEditPage::addEdit().
 */

UnitsEdit::UnitsEdit( const char **varList, EqVar *var, QComboBox *combo,
        QSpinBox *spin ) :
    m_varList(varList),
    m_var(var),
    m_combo(combo),
    m_spin(spin)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief UnitsEditDialog constructor.
 *
 *  \param bp           Pointer to the parent BpDocument
 *  \param captionKey   Dialog caption translator key.
 *  \param name         Widget internal name.
 *  \param acceptKey    Translator key for the accept button
 *  \param rejectKey    Translator key for the reject button
 */

UnitsEditDialog::UnitsEditDialog( BpDocument *bp, const QString &captionKey,
        const char *name, const QString &acceptKey, const QString &rejectKey ) :
    AppTabDialog( appWindow(), captionKey, name, acceptKey, rejectKey ),
    m_bp(bp),
    m_pageList(0)
{
    // Create the UnitsEditPage list
    m_pageList = new QPtrList<UnitsEditPage>();
    Q_CHECK_PTR( m_pageList );
    m_pageList->setAutoDelete( false );

    // Fuel & Vegetation Units
    UnitsEditPage *page;
    int rows = 7;
    if ( appWindow()->m_release >= 20000 )
    {
        rows += 2;
    }
    int row = 1;
	bool nodecimals = false;
    page = addPage( "UnitsEditDialog:FuelVeg:Tab", rows,
                    "KitchenPreserve.png", "Kitchen Preserve",
                    "fuelVegetationUnits.html" );
    page->addEdit( "UnitsEditDialog:FuelVeg:FuelLoad",
                    FuelLoadVars, FuelLoadUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:FuelSavr",
                    FuelSavrVars, FuelSavrUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:UnderstoryFuelDepth",
                    FuelUnderstoryDepthVars, FuelDepthUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:GroundFuelDepth",
                    FuelGroundDepthVars, DistShortUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:FuelMois",
                    FuelMoisVars, PercentUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:FuelHeat",
                    FuelHeatVars, FuelHeatUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:FuelBulk",
                    FuelBulkVars, FuelBulkUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:PackingRatio",
                    FuelPackingRatioVars, RatioUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:TreeHt",
                    TreeHtVars, DistMedUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:TreeDbh",
                    TreeDbhVars, DistShortUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:TreeBulk",
                    TreeBulkVars, FuelBulkUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:Cover",
                    FuelCoverVars, PercentUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:BasalArea",
                    BasalAreaVars, BasalAreaUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:Fractions",
                    FuelFractionVars, PercentUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:CrownRatio",
                    FuelCrownVars, PercentUnits, row++ );

    // Weather Units
    page = addPage( "UnitsEditDialog:Weather:Tab", 4,
                    "TieOneOn.png", "Tie One On",
                    "weatherUnits.html" );
    row = 1;
    page->addEdit( "UnitsEditDialog:Weather:WindSpeed",
                    WindSpeedVars, WindSpeedUnits, row++ );
    page->addEdit( "UnitsEditDialog:Weather:Temperature",
                    TemperatureVars, TemperatureUnits, row++, nodecimals );
    //page->addEdit( "UnitsEditDialog:Weather:Humidity",
    //                HumidityVars, PercentUnits, row++ );
    page->addEdit( "UnitsEditDialog:Weather:SunShade",
                    WthrCoverVars, PercentUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:FuelVeg:Waf",
                    FuelWafVars, WafUnits, row++ );

    // Terrain & Spotting Units
    page = addPage( "UnitsEditDialog:TerrainSpot:Tab", 3,
                    "HomeOnTheReef.png", "Home on the Reef",
                    "terrainSpottingUnits.html" );
    row = 1;
    page->addEdit( "UnitsEditDialog:TerrainSpot:SlopeSteepness",
                    SlopeSteepnessVars, SlopeUnits, row++ );
    page->addEdit( "UnitsEditDialog:TerrainSpot:Elevation",
                    ElevationVars, ElevationUnits, row++ );
    page->addEdit( "UnitsEditDialog:TerrainSpot:SiteDistance",
                    SiteDistanceVars, DistLongUnits, row++, nodecimals );
    page->addEdit( "UnitsEditDialog:TerrainSpot:SpotDistance",
                    SpotDistanceVars, DistLongUnits, row++ );
    page->addEdit( "UnitsEditDialog:TerrainSpot:Height",
                    SpotHeightVars, DistMedUnits, row++ );
    page->addEdit( "UnitsEditDialog:TerrainSpot:Ratios",
                    SpotRatioVars, RatioUnits, row++ );

    // Fire & Effects Units
    page = addPage( "UnitsEditDialog:FireEffects:Tab", 10,
                    "LeaveItToBeavers1.png", "Leave It To Beavers",
                    "fireUnits.html" );
    row = 1;
    page->addEdit( "UnitsEditDialog:FireEffects:FireSurfaceSpread",
                    FireSurfaceSpreadVars, FireSpreadUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireCrownSpread",
                    FireCrownSpreadVars, FireSpreadUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireHpua",
                    FireHpuaVars, FireHpuaUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireLineInt",
                    FireLineIntVars, FireLineIntUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireFlameLength",
                    FireFlameLengthVars, FireFlameUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireRxInt",
                    FireRxIntVars, FireRxIntUnits, row++ );
    page->addEdit( "UnitsEditDialog:FuelVeg:HeatSink",
                    FuelHeatSinkVars, FuelHeatSinkUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireDistance",
                    FireDistVars, FireDistUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FireArea",
                    FireAreaVars, FireAreaUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:FirePerimeter",
                    FirePerimeterVars, FireDistUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:TreeScorch",
                    TreeScorchRatioVars, PercentUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:IgnitionProb",
                    IgnitionProbVars, PercentUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:Ratios",
                    FireRatioVars, RatioUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:Factors",
                    FireFactorVars, RatioUnits, row++ );
    page->addEdit( "UnitsEditDialog:FireEffects:Power",
                    FirePowerVars, FirePowerUnits, row++ );

    // Time & Map Units
    page = addPage( "UnitsEditDialog:TimeMap:Tab", 3,
                    "LeaveItToBeavers2.png", "Leave It To Beavers",
                    "timeMapUnits.html" );
    row = 1;
    page->addEdit("UnitsEditDialog:TimeMap:ElapsedTime",
                    FireTimeVars, TimeMedUnits, row++ );
    page->addEdit("UnitsEditDialog:TimeMap:FlameResidenceTime",
                    FlameResidenceTimeVars, TimeShortUnits, row++ );
    page->addEdit("UnitsEditDialog:TimeMap:PalmettoAge",
                    PalmettoAgeVars, TimeLongUnits, row++, nodecimals );
    page->addEdit("UnitsEditDialog:TimeMap:MapElevation",
                    MapElevationVars, DistMedUnits, row++, nodecimals );
    page->addEdit("UnitsEditDialog:TimeMap:MapDistance",
                    MapDistanceVars, DistMapUnits, row++ );

    // Contain & Safety Units
    page = addPage( "UnitsEditDialog:Contain:Tab", 3,
                    "BearsDen.png", "Bear's Den",
                    "containUnits.html" );
    row = 1;
    page->addEdit("UnitsEditDialog:Contain:Rate",
                    ContainRateVars, FireSpreadUnits, row++, nodecimals );
    page->addEdit("UnitsEditDialog:Contain:Dist",
                    ContainDistVars, FireDistUnits, row++ );
    page->addEdit("UnitsEditDialog:Safety:Dist",
                    SafetyDistVars,  DistMedUnits, row++ );
    page->addEdit("UnitsEditDialog:Safety:Area",
                    SafetyAreaVars,  FireAreaUnits, row++ );
    page->addEdit("UnitsEditDialog:Safety:PE",
                    SafetyPEVars,    SafetyAreaUnits, row++, nodecimals );
    return;
}

//------------------------------------------------------------------------------
/*! \brief UnitsEditDialog destructor.
 */

UnitsEditDialog::~UnitsEditDialog( void )
{
    delete m_pageList;  m_pageList = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds an UnitsEditPage to the UnitsEditDialog.
 *
 *  \param tabText      Tab translator key.
 *  \param rows         Number of rows
 *  \param pictureFile  Picture file name.
 *  \param pictureName  Picture name.
 *  \param htmlFile     Page's initial HTML file base name.
 *
 *  \return Pointer to the new UnitsEditPage.
 */

UnitsEditPage *UnitsEditDialog::addPage( const QString &tabKey, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile )
{
    // Create the new page
    UnitsEditPage *page = new UnitsEditPage( this, rows, pictureFile,
        pictureName, htmlFile, QString( "%1:%2" ).arg( name() ).arg( tabKey ) );
    checkmem( __FILE__, __LINE__, page, "UnitsEditPage page", 1 );
    // Add it to the dialog's page list
    m_pageList->append( page );
    // Add the tab
    if ( ! tabKey.isNull() && ! tabKey.isEmpty() )
    {
        QString tabText("");
        translate( tabText, tabKey );
        addTab( page, tabText );
    }
    return( page );
}

//------------------------------------------------------------------------------
/*! \brief UnitsEditPage constructor.
 *
 *  \param dialog       Pointer to parent UnitsEditDialog
 *  \param rows         Number of rows
 *  \param pictureFile  Picture file name
 *  \param pictureName  Picture name
 *  \param htmlFile     Page's initial HTML file base name
 *  \param name         Widget's internal name
 *
 *  /note The number of rows is fixed to the maximum for any page (12),
 *  as a work-around to the GridLayout tendency to give different row heights
 *  depending on the number of declared rows.
 */

UnitsEditPage::UnitsEditPage( UnitsEditDialog *dialog, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name ) :
    AppPage( dialog, pictureFile, pictureName, htmlFile, name ),
    m_dialog(dialog),
    m_frame(0),
    m_grid(0),
    m_editList(0)
{
    // Hidden frame to contain a grid layout.
    m_frame = new QFrame( m_contentFrame, QString( "%1:Frame" ).arg( name ) );
    Q_CHECK_PTR( m_frame );
    m_frame->setFrameStyle( QFrame::NoFrame );

    // Create grid layout with rows+1 rows, 3 columns, 0 border, and 2 spacing.
    //int rows = 20;
    m_grid = new QGridLayout( m_frame, rows, 3, 0, 2,
        QString( "%1:Grid" ).arg( name ) );
    Q_CHECK_PTR( m_grid );
    // Create the page's UnitsEdit list
    m_editList = new QPtrList<UnitsEdit>();
    Q_CHECK_PTR( m_editList );
    // We do want to delete the UnitsEdit's when this class is destroyed.
    m_editList->setAutoDelete( true );
    // Add captions
    addHeaders();
    return;
}

//------------------------------------------------------------------------------
/*! \brief UnitsEditPage destructor.
 */

UnitsEditPage::~UnitsEditPage( void )
{
    delete m_editList;     m_editList = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds a UnitsEdit to a UnitsEditPage.
 *
 *  \param labelKey     Translator key for the edit label.
 *  \param varList      NULL-terminated list of names of variables
 *                      to be affected by this PageEdit.
 *  \param unitsList    NULL-terminated list of units choices
 */

UnitsEdit *UnitsEditPage::addEdit( const QString &labelKey,
        const char **varList, const char **unitsList, int row, bool showDecimals )
{
    // Find the variable
    EqVar *varPtr = m_dialog->m_bp->m_eqTree->m_varDict->find( varList[0] );
    if ( ! varPtr )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "UnitsEditDialog:UnknownVar", varList[0] );
        bomb( text );
    }
    // Create the QLabel
    QString text("");
    translate( text, labelKey );
    QLabel *label = new QLabel( text, m_frame,
        QString( "%1:Label" ).arg( labelKey ) );
    Q_CHECK_PTR( label );
    m_grid->addMultiCellWidget( label, row, row, 0, 0, AlignLeft );

    // Create the combo box
    QComboBox *combo = new QComboBox( false, m_frame,
        QString( "%1:ComboBox" ).arg( labelKey ) );
    Q_CHECK_PTR( combo );
    // Insert items into the combo box while searching for the current item
    int n = 0;
    for ( int id = 0;
          unitsList[id];
          id++ )
    {
        combo->insertItem( unitsList[id] );
        if ( appSiUnits()->equivalent(
                unitsList[id], varPtr->m_displayUnits.latin1() ) )
        {
            n = id;
        }
    }
    combo->setCurrentItem( n );
    m_grid->addMultiCellWidget( combo, row, row, 1, 1 );

    // Create the spin box
	QSpinBox *spin = NULL;
	if ( showDecimals )
	{
		spin = new QSpinBox( 0, 6, 1, m_frame,
			QString( "%1:SpinBox" ).arg( labelKey ) );
		Q_CHECK_PTR( spin );
		spin->setValue( varPtr->m_displayDecimals );
		m_grid->addMultiCellWidget( spin, row, row, 2, 2 );
	}
    // All rows share the available space equally
    m_grid->setRowStretch( row, 10 );

    // Create the new UnitsEdit
    UnitsEdit *edit = new UnitsEdit( varList, varPtr, combo, spin );
    checkmem( __FILE__, __LINE__, edit, "UnitsEdit edit", 1 );
    // Add the UnitsEdit to the UnitEditPage's m_editList
    m_editList->append( edit );
    return( edit );
}

//------------------------------------------------------------------------------
/*! \brief Adds column headers to a UnitsEditPage.
 */

void UnitsEditPage::addHeaders( void )
{
    // Add column headers
    QLabel *label;
    QString text("");

    translate( text, "UnitsEditDialog:Col0" );
    label = new QLabel( text, m_frame, QString( "%1:Header0" ).arg( name() ) );
    Q_CHECK_PTR( label );
    m_grid->addMultiCellWidget( label, 0, 0, 0, 0 );

    translate( text, "UnitsEditDialog:Col1" );
    label = new QLabel( text, m_frame, QString( "%1:Header1" ).arg( name() ) );
    Q_CHECK_PTR( label );
    m_grid->addMultiCellWidget( label, 0, 0, 1, 1 );

    translate( text, "UnitsEditDialog:Col2" );
    label = new QLabel( text, m_frame, QString( "%1:Header2" ).arg( name() ) );
    Q_CHECK_PTR( label );
    m_grid->addMultiCellWidget( label, 0, 0, 2, 2 );

    // All rows share the available space equally
    //m_grid->setRowStretch( 0, 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stores the dialog's units and decimal settings into their EqVars.
 *
 *  This is the UnitsEditDialog's Accept button callback.
 */

void UnitsEditDialog::store( void )
{
    UnitsEditPage *page;
    UnitsEdit *edit;
    EqVar *varPtr;
    QString newUnits;
    int newDecimals;
    // Loop for each UnitsEditPage in this UnitsEditDialog.
    for ( page = m_pageList->first();
          page != 0;
          page = m_pageList->next() )
    {
        // Loop for each UnitsEdit item on this UnitsEditPage
        for ( edit = page->m_editList->first();
              edit != 0;
              edit = page->m_editList->next() )
        {
            // Get the current units and decimals
            newUnits = edit->m_combo->currentText();
			newDecimals = ( edit->m_spin == NULL ) ? -1 : edit->m_spin->value();
            varPtr = edit->m_var;
            // Reset units of each EqVar in the UnitEdit item's m_varList[]
            varPtr->setDisplayUnits( newUnits, newDecimals );
            for ( int id = 1;
                  edit->m_varList[id];
                  id++ )
            {
                // Find the variable
                EqVar *varPtr = m_bp->m_eqTree->m_varDict->find(
                    edit->m_varList[id] );
                if ( ! varPtr )
                // This code block should never be executed!
                {
                    QString text("");
                    translate( text, "UnitsEditDialog:UnknownVar",
                        edit->m_varList[id] );
                    bomb( text );
                }
                varPtr->setDisplayUnits( newUnits, newDecimals );
            }
        }
    }
    // Ok, all the edited units have been stored to their variables.
    // Now save it to a file
    m_bp->save( "", "Units Set" );
    return;
}

//------------------------------------------------------------------------------
//  End of unitseditdialog.cpp
//------------------------------------------------------------------------------

