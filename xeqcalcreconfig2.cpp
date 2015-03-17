//------------------------------------------------------------------------------
/*! \file xeqcalcreconfig2.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2012 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Reconfigures the EqFun m_active and the EqVar m_isUserOutput flags
 *  to reflect the current configuration options, outputs, & properties.
 *
 *  This is one of the few EqCalc files that hold implementation-specific
 *  variable and function pointers.  The files are
 *	-- xeqcalc.cpp,
 *  -- xeqcalcmask.cpp, and
 *	-- xeqcalcreconfig.cpp.
 */

// Custom include files
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

//------------------------------------------------------------------------------
/*! \brief Dynamically reconfigures the each EqTree module
 *  based upon the current option and output configuration variables.
 *
 *  \param release Application's release number.
 *
 *  This should called whenever any of the *Option* or *Output* variables
 *  have been changed, thereby changing the EqTree configuration state.
 */


//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Surface Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureSurfaceModule2( PropertyDict *prop, int /* release */ )
{
	//--------------------------------------------------------------------------
	// Section 1: get configuration
	//--------------------------------------------------------------------------

	// All functions start out inactive, and all variables are neither input nor output.
	bool crown   = prop->boolean( "crownModuleActive" );
	bool scorch  = prop->boolean( "scorchModuleActive" );
	bool size    = prop->boolean( "sizeModuleActive" );
	bool spot    = prop->boolean( "spotModuleActive" );
    bool surface = prop->boolean( "surfaceModuleActive" );

	// SURFACE Tab 1 Fuel has 1 configurator:
	// The first configures fuel input method as 1 of 7 mutually exclusive options:
    bool opt_fuel_models       = prop->boolean( "surfaceConfFuelModels" );
    bool opt_fuel_parms        = prop->boolean( "surfaceConfFuelParms" );
    bool opt_fuel_wtd_area     = prop->boolean( "surfaceConfFuelAreaWeighted" );
    bool opt_fuel_wtd_harmonic = prop->boolean( "surfaceConfFuelHarmonicMean" );
    bool opt_fuel_wtd_2d       = prop->boolean( "surfaceConfFuel2Dimensional" );
	bool opt_fuel_pg           = prop->boolean( "surfaceConfFuelPalmettoGallberry" );
    bool opt_fuel_aspen        = prop->boolean( "surfaceConfFuelAspen" );
	// Convenience option for any fuel weighting method
	bool opt_fuel_wtd = opt_fuel_wtd_area || opt_fuel_wtd_harmonic || opt_fuel_wtd_2d;
   
	// SURFACE Tab 2 Moisture has 2 configurators:
	// The 1st configures the dynamic curing load transfer with 1 of 2 mutually exclusive options:
    bool opt_mois_calc_load_transfer  = prop->boolean( "surfaceConfLoadTransferCalc" );
    bool opt_mois_input_load_transfer = prop->boolean( "surfaceConfLoadTransferInput" );
	// The 2nd configures the type of fuel moisture input with 1 of 3 mutually exclusive options:
	bool opt_mois_time_lag = prop->boolean( "surfaceConfMoisTimeLag" );
	bool opt_mois_life_cat = prop->boolean( "surfaceConfMoisLifeCat" );
	bool opt_mois_scenario = prop->boolean( "surfaceConfMoisScenario" );

	// SURFACE Tab 3 Wind Speed has 2 configurators:
	// The 1st configures how wind speed is input with 1 of 5 mutually exclusive options:
	bool opt_wind_midflame  = prop->boolean( "surfaceConfWindSpeedAtMidflame" );
	bool opt_wind_20ft      = prop->boolean( "surfaceConfWindSpeedAt20Ft" );
	bool opt_wind_20ft_calc = prop->boolean( "surfaceConfWindSpeedAt20FtCalc" );
	bool opt_wind_10m       = prop->boolean( "surfaceConfWindSpeedAt10M" );
	bool opt_wind_10m_calc  = prop->boolean( "surfaceConfWindSpeedAt10MCalc" );
	// The 2nd configures whether the maximum reliable wind speed limit is imposed
	bool opt_wind_limit = prop->boolean( "surfaceConfWindLimitApplied" );
    // NOTE that EqCalc::FireSpreadAtHead() checks for the surfaceConfWindLimitApplied
    // property and passes it into FBL_SurfaceFireForwardSpreadRate(),
    // so no further setup is needed here

	// SURFACE Tab 4 Direction has 3 configurators:
	// The 1st configures surface fire spread direction with 1 of 6 mutually exclusive options:
	bool opt_spread_head       = prop->boolean( "surfaceConfSpreadDirHead" );
	bool opt_spread_back       = prop->boolean( "surfaceConfSpreadDirBack" );
	bool opt_spread_flank      = prop->boolean( "surfaceConfSpreadDirFlank" );
	bool opt_spread_ignpt_beta = prop->boolean( "surfaceConfSpreadDirPointSourceBeta" );
	bool opt_spread_ignpt_psi  = prop->boolean( "surfaceConfSpreadDirPointSourcePsi" );
	bool opt_spread_fire_front = prop->boolean( "surfaceConfSpreadDirFireFront" );
	// Convenience
	bool opt_spread_beta       = opt_spread_ignpt_beta || opt_spread_ignpt_psi;
	bool opt_spread_psi        = ! opt_spread_beta;
	// The 2nd configures wind direction with 1 of 2 mutually exclusive options:
	bool opt_wind_upslope = prop->boolean( "surfaceConfWindDirUpslope" ); 
	bool opt_wind_input   = prop->boolean( "surfaceConfWindDirInput" ); 

	// SURFACE Tab 5 Slope Options

	// SURFACE Tab 6 Basic Output Options

	// SURFACE Tab 7 Intermediate Direction Options

	// SURFACE Tab 8 Wind Output Options

	// SURFACE Tab 9 Slope Outputs Options

	// SURFACE Tab 10 Intermediate Outputs Options

	// SURFACE Tab 11 Fuels Output Options

	// SURFACE Tab 12 Aspen Outputs Options

	// SURFACE Tab 13 P-G Outputs Options

	//--------------------------------------------------------------------------
	// Section 2: set alternate functions based on configuration
	//--------------------------------------------------------------------------

    fSurfaceFireArea->m_active = surface;
    fSurfaceFireCharacteristicsDiagram->m_active = surface;
    fSurfaceFireDistAtBack->m_active =
		( surface && ! size );	// force Psi calculations
    fSurfaceFireDistAtBeta->m_active = surface;
    fSurfaceFireDistAtHead->m_active =
		( surface && ! size );	// force Psi calculations
    fSurfaceFireDistAtPsi->m_active = surface;
    fSurfaceFireDistAtVector->m_active = surface;
    fSurfaceFireEccentricity->m_active =
		( surface && ! size );	// force Psi calculations
    fSurfaceFireEffWindAtVector->m_active = surface;
    fSurfaceFireEllipseF->m_active = surface;
    fSurfaceFireEllipseG->m_active = surface;
    fSurfaceFireEllipseH->m_active = surface;
    fSurfaceFireFlameAngle->m_active = surface;
    fSurfaceFireFlameHtAtVector->m_active = surface;
    fSurfaceFireFlameLengAtBeta->m_active = surface;
    fSurfaceFireFlameLengAtHead->m_active = surface;
    fSurfaceFireFlameLengAtPsi->m_active = surface;
    fSurfaceFireFlameLengAtVector->m_active = surface;
    fSurfaceFireHeatPerUnitArea->m_active = surface;
    fSurfaceFireHeatSource->m_active = surface;
    fSurfaceFireLengDist->m_active =
		( surface && ! size );						// force Psi calculations
    fSurfaceFireLengMapDist->m_active = surface;
    fSurfaceFireLengthToWidth->m_active =
		( surface && ! size );						// force Psi calculations
    fSurfaceFireLineIntAtBeta->m_active = surface;
    fSurfaceFireLineIntAtHead->m_active = surface;
    fSurfaceFireLineIntAtPsi->m_active = surface;
    fSurfaceFireLineIntAtVectorFromBeta->m_active =
		( surface && opt_spread_ignpt_beta );		// only use this for ign pt with beta fli/fi
    fSurfaceFireLineIntAtVectorFromPsi->m_active = surface;
		( surface && ! opt_spread_ignpt_beta );		// always use this except for ign pt with beta fli/fl
    fSurfaceFireMaxDirFromNorth->m_active = surface;
    fSurfaceFireMaxDirDiagram->m_active = surface;
    fSurfaceFireMapDistAtBack->m_active = surface;
    fSurfaceFireMapDistAtBeta->m_active = surface;
    fSurfaceFireMapDistAtHead->m_active = surface;
    fSurfaceFireMapDistAtPsi->m_active = surface;
    fSurfaceFireMapDistAtVector->m_active = surface;
    fSurfaceFireNoWindRate->m_active = surface;
    fSurfaceFirePerimeter->m_active = surface;
    fSurfaceFirePropagatingFlux->m_active = surface;
    fSurfaceFireReactionInt->m_active = surface;
    fSurfaceFireResidenceTime->m_active = surface;
    fSurfaceFireScorchHtFromFliAtVector->m_active = surface;
    fSurfaceFireScorchHtFromFlameLengAtVector->m_active = surface;
    fSurfaceFireShapeDiagram->m_active = surface;
    fSurfaceFireSpreadAtBack->m_active =
		( surface && ! size );						// force Psi calculations
    fSurfaceFireSpreadAtBeta->m_active = surface;
    fSurfaceFireSpreadAtHead->m_active = surface;
    fSurfaceFireSpreadAtPsi->m_active = surface;
    fSurfaceFireSpreadAtVectorFromBeta->m_active =
		( surface && opt_spread_beta );
    fSurfaceFireSpreadAtVectorFromPsi->m_active =
		( surface && opt_spread_psi );
    fSurfaceFireVectorBetaFromTheta->m_active = surface;
		( surface && opt_spread_fire_front );
    fSurfaceFireVectorBetaFromUpslope->m_active =
		( surface && opt_spread_beta );
    fSurfaceFireVectorDirFromNorth->m_active = surface;
    fSurfaceFireVectorPsiFromTheta->m_active = surface;
		( surface && opt_spread_beta );
    fSurfaceFireVectorPsiFromUpslope->m_active =
		( surface && opt_spread_fire_front );
    fSurfaceFireVectorThetaFromBeta->m_active = surface;
		( surface && opt_spread_beta );
    fSurfaceFireVectorThetaFromPsi->m_active = surface;
		( surface && opt_spread_fire_front );
    fSurfaceFireVectorDirFromUpslope->m_active = surface;
    fSurfaceFireWidthDist->m_active =
		( surface && ! size );						// force Psi calculations
    fSurfaceFireWidthMapDist->m_active = surface;

    fSurfaceFuelAspenModel->m_active =
		( surface && opt_fuel_aspen );
    fSurfaceFuelAspenParms->m_active =
		( surface && opt_fuel_aspen );
    fSurfaceFuelBedHeatSink->m_active = surface;
    fSurfaceFuelBedIntermediates->m_active = surface;
    fSurfaceFuelBedModel->m_active =
		( surface && opt_fuel_models );
    fSurfaceFuelBedParms->m_active =
		( surface && ( opt_fuel_models || opt_fuel_parms ) );
    fSurfaceFuelBedWeighted->m_active =
		( surface && opt_fuel_wtd );
	// If Aspen or PG fuel, force dynamic fuel load transfers of 0% and don't allow transfer input
    fSurfaceFuelLoadTransferFraction->m_active =
		( surface && ( opt_mois_calc_load_transfer || opt_fuel_aspen || opt_fuel_pg ) );
	fSurfaceFuelMoisLifeClass->m_active =
		( surface && opt_mois_life_cat );
    fSurfaceFuelMoisScenarioModel->m_active =
		( surface && opt_mois_scenario );
    fSurfaceFuelMoisTimeLag->m_active =
		( surface && opt_mois_time_lag );
    fSurfaceFuelPalmettoModel->m_active =
		( surface && opt_fuel_pg );
    fSurfaceFuelPalmettoParms->m_active =
		( surface && opt_fuel_pg );
    // Don't calculate crown ratio unless both Surface and Crown are active and WAF is calculated
	fTreeCrownRatio->m_active =
		crown && ( surface && opt_wind_20ft_calc );
	fTreeMortalityRateAspenAtVector->m_active =
		( surface && opt_fuel_aspen );

    fWindAdjFactor->m_active =
		( surface && ( opt_wind_20ft_calc || opt_wind_10m_calc ) );
    fWindSpeedAt20Ft->m_active =
		( surface && ( opt_wind_10m || opt_wind_10m_calc ) );
    fWindSpeedAtMidflame->m_active =
		( surface && ! opt_wind_midflame );
    fWindDirFromNorth->m_active = surface;
    fWindDirFromUpslope->m_active = surface;

	//--------------------------------------------------------------------------
	// Section 3: set output variables based on configuration
	//--------------------------------------------------------------------------
	// 292 variables
    vContainAttackBack->m_isUserOutput = false;
    vContainAttackDist->m_isUserOutput = false;
    vContainAttackPerimeter->m_isUserOutput = false;
    vContainAttackSize->m_isUserOutput = false;
    vContainAttackHead->m_isUserOutput = false;
    vContainAttackTactic->m_isUserOutput = false;
    vContainCost->m_isUserOutput = false;
    vContainDiagram->m_isUserOutput = false;
    vContainLimitDist->m_isUserOutput = false;
    vContainLine->m_isUserOutput = false;
    vContainPoints->m_isUserOutput = false;
    vContainResourceArrival->m_isUserOutput = false;
    vContainResourceBaseCost->m_isUserOutput = false;
    vContainResourceDuration->m_isUserOutput = false;
    vContainResourceHourCost->m_isUserOutput = false;
    vContainResourceName->m_isUserOutput = false;
    vContainResourceProd->m_isUserOutput = false;
    vContainResourcesUsed->m_isUserOutput = false;
    vContainReportBack->m_isUserOutput = false;
    vContainReportHead->m_isUserOutput = false;
    vContainReportRatio->m_isUserOutput = false;
    vContainReportSize->m_isUserOutput = false;
    vContainReportSpread->m_isUserOutput = false;
    vContainSize->m_isUserOutput = false;
    vContainStatus->m_isUserOutput = false;
    vContainTime->m_isUserOutput = false;
    vContainXMax->m_isUserOutput = false;
    vContainXMin->m_isUserOutput = false;
    vContainYMax->m_isUserOutput = false;
    vCrownFireActiveCrown->m_isUserOutput = false;
    vCrownFireActiveRatio->m_isUserOutput = false;
    vCrownFireArea->m_isUserOutput = false;
    vCrownFireCritCrownSpreadRate->m_isUserOutput = false;
    vCrownFireCritSurfFireInt->m_isUserOutput = false;
    vCrownFireCritSurfFlameLeng->m_isUserOutput = false;
    vCrownFireFlameLeng->m_isUserOutput = false;
    vCrownFireFuelLoad->m_isUserOutput = false;
    vCrownFireHeatPerUnitArea->m_isUserOutput = false;
    vCrownFireHeatPerUnitAreaCanopy->m_isUserOutput = false;
    vCrownFireLengthToWidth->m_isUserOutput = false;
    vCrownFireLineInt->m_isUserOutput = false;
    vCrownFirePerimeter->m_isUserOutput = false;
    vCrownFirePowerOfFire->m_isUserOutput = false;
    vCrownFirePowerOfWind->m_isUserOutput = false;
    vCrownFirePowerRatio->m_isUserOutput = false;
    vCrownFireSpreadDist->m_isUserOutput = false;
    vCrownFireSpreadMapDist->m_isUserOutput = false;
    vCrownFireSpreadRate->m_isUserOutput = false;
    vCrownFireTransRatio->m_isUserOutput = false;
    vCrownFireTransToCrown->m_isUserOutput = false;
    vCrownFireType->m_isUserOutput = false;
    vCrownFireWindDriven->m_isUserOutput = false;
    vDocDescription->m_isUserOutput = false;
    vDocFireAnalyst->m_isUserOutput = false;
    vDocFireName->m_isUserOutput = false;
    vDocFirePeriod->m_isUserOutput = false;
    vDocRxAdminUnit->m_isUserOutput = false;
    vDocRxName->m_isUserOutput = false;
    vDocRxPreparedBy->m_isUserOutput = false;
    vDocTrainingCourse->m_isUserOutput = false;
    vDocTrainingExercise->m_isUserOutput = false;
    vDocTrainingTrainee->m_isUserOutput = false;
    vIgnitionFirebrandFuelMois->m_isUserOutput = false;
    vIgnitionFirebrandProb->m_isUserOutput = false;
    vIgnitionLightningDuffDepth->m_isUserOutput = false;
    vIgnitionLightningFuelMois->m_isUserOutput = false;
    vIgnitionLightningFuelType->m_isUserOutput = false;
    vIgnitionLightningProb->m_isUserOutput = false;
    vMapContourCount->m_isUserOutput = false;
    vMapContourInterval->m_isUserOutput = false;
    vMapDist->m_isUserOutput = false;
    vMapFraction->m_isUserOutput = false;
    vMapScale->m_isUserOutput = false;
    vSafetyZoneEquipmentArea->m_isUserOutput = false;
    vSafetyZoneEquipmentNumber->m_isUserOutput = false;
    vSafetyZonePersonnelArea->m_isUserOutput = false;
    vSafetyZonePersonnelNumber->m_isUserOutput = false;
    vSafetyZoneRadius->m_isUserOutput = false;
    vSafetyZoneSepDist->m_isUserOutput = false;
    vSafetyZoneSize->m_isUserOutput = false;
    vSiteAspectDirFromCompass->m_isUserOutput = false;
    vSiteAspectDirFromNorth->m_isUserOutput = false;
    vSiteElevation->m_isUserOutput = false;
    vSiteLatitude->m_isUserOutput = false;
    vSiteLongitude->m_isUserOutput = false;
    vSiteRidgeToValleyDist->m_isUserOutput = false;
    vSiteRidgeToValleyElev->m_isUserOutput = false;
    vSiteRidgeToValleyMapDist->m_isUserOutput = false;
    vSiteSlopeDegrees->m_isUserOutput = false;
    vSiteSlopeFraction->m_isUserOutput = false;
    vSiteSlopeReach->m_isUserOutput = false;
    vSiteSlopeRise->m_isUserOutput = false;
    vSiteSunShading->m_isUserOutput = false;
    vSiteTimeZone->m_isUserOutput = false;
    vSiteUpslopeDirFromNorth->m_isUserOutput = false;
    vSpotCoverHtBurningPile->m_isUserOutput = false;
    vSpotCoverHtSurfaceFire->m_isUserOutput = false;
    vSpotCoverHtTorchingTrees->m_isUserOutput = false;
    vSpotDistBurningPile->m_isUserOutput = false;
    vSpotDistSurfaceFire->m_isUserOutput = false;
    vSpotDistTorchingTrees->m_isUserOutput = false;
    vSpotFirebrandDriftSurfaceFire->m_isUserOutput = false;
    vSpotFirebrandHtBurningPile->m_isUserOutput = false;
    vSpotFirebrandHtSurfaceFire->m_isUserOutput = false;
    vSpotFirebrandHtTorchingTrees->m_isUserOutput = false;
    vSpotFlameDurTorchingTrees->m_isUserOutput = false;
    vSpotFlameHtTorchingTrees->m_isUserOutput = false;
    vSpotFlameRatioTorchingTrees->m_isUserOutput = false;
    vSpotFlatDistBurningPile->m_isUserOutput = false;
    vSpotFlatDistSurfaceFire->m_isUserOutput = false;
    vSpotFlatDistTorchingTrees->m_isUserOutput = false;
    vSpotFireSource->m_isUserOutput = false;
    vSpotMapDistBurningPile->m_isUserOutput = false;
    vSpotMapDistSurfaceFire->m_isUserOutput = false;
    vSpotMapDistTorchingTrees->m_isUserOutput = false;
    vSpotTorchingTrees->m_isUserOutput = false;
    vSurfaceFireArea->m_isUserOutput = false;
    vSurfaceFireCharacteristicsDiagram->m_isUserOutput = false;
    vSurfaceFireDistAtBack->m_isUserOutput = false;
    vSurfaceFireDistAtBeta->m_isUserOutput = false;
    vSurfaceFireDistAtHead->m_isUserOutput = false;
    vSurfaceFireDistAtPsi->m_isUserOutput = false;
    vSurfaceFireDistAtVector->m_isUserOutput = false;
    vSurfaceFireEccentricity->m_isUserOutput = false;
    vSurfaceFireEffWindAtHead->m_isUserOutput = false;
    vSurfaceFireEffWindAtVector->m_isUserOutput = false;
    vSurfaceFireElapsedTime->m_isUserOutput = false;
    vSurfaceFireEllipseF->m_isUserOutput = false;
    vSurfaceFireEllipseG->m_isUserOutput = false;
    vSurfaceFireEllipseH->m_isUserOutput = false;
    vSurfaceFireFlameAngleAtVector->m_isUserOutput = false;
    vSurfaceFireFlameHtAtVector->m_isUserOutput = false;
    vSurfaceFireFlameHtPile->m_isUserOutput = false;
    vSurfaceFireFlameLengAtBeta->m_isUserOutput = false;
    vSurfaceFireFlameLengAtHead->m_isUserOutput = false;
    vSurfaceFireFlameLengAtPsi->m_isUserOutput = false;
    vSurfaceFireFlameLengAtVector->m_isUserOutput = false;
    vSurfaceFireHeatPerUnitArea->m_isUserOutput = false;
    vSurfaceFireHeatSource->m_isUserOutput = false;
    vSurfaceFireLengDist->m_isUserOutput = false;
    vSurfaceFireLengMapDist->m_isUserOutput = false;
    vSurfaceFireLengthToWidth->m_isUserOutput = false;
    vSurfaceFireLineIntAtBeta->m_isUserOutput = false;
    vSurfaceFireLineIntAtHead->m_isUserOutput = false;
    vSurfaceFireLineIntAtPsi->m_isUserOutput = false;
    vSurfaceFireLineIntAtVector->m_isUserOutput = false;
    vSurfaceFireMapDistAtBack->m_isUserOutput = false;
    vSurfaceFireMapDistAtBeta->m_isUserOutput = false;
    vSurfaceFireMapDistAtHead->m_isUserOutput = false;
    vSurfaceFireMapDistAtPsi->m_isUserOutput = false;
    vSurfaceFireMapDistAtVector->m_isUserOutput = false;
    vSurfaceFireMaxDirDiagram->m_isUserOutput = false;
    vSurfaceFireMaxDirFromNorth->m_isUserOutput = false;
    vSurfaceFireMaxDirFromUpslope->m_isUserOutput = false;
    vSurfaceFireNoWindRate->m_isUserOutput = false;
    vSurfaceFirePerimeter->m_isUserOutput = false;
    vSurfaceFirePropagatingFlux->m_isUserOutput = false;
    vSurfaceFireReactionInt->m_isUserOutput = false;
    vSurfaceFireReactionIntDead->m_isUserOutput = false;
    vSurfaceFireReactionIntLive->m_isUserOutput = false;
    vSurfaceFireResidenceTime->m_isUserOutput = false;
    vSurfaceFireScorchHtAtVector->m_isUserOutput = false;
    vSurfaceFireSeverityAspen->m_isUserOutput = false;
    vSurfaceFireShapeDiagram->m_isUserOutput = false;
    vSurfaceFireSlopeFactor->m_isUserOutput = false;
    vSurfaceFireSpreadAtBack->m_isUserOutput = false;
    vSurfaceFireSpreadAtBeta->m_isUserOutput = false;
    vSurfaceFireSpreadAtHead->m_isUserOutput = false;
    vSurfaceFireSpreadAtPsi->m_isUserOutput = false;
    vSurfaceFireSpreadAtVector->m_isUserOutput = false;
    vSurfaceFireVectorBeta->m_isUserOutput = false;
    vSurfaceFireVectorDirFromCompass->m_isUserOutput = false;
    vSurfaceFireVectorDirFromNorth->m_isUserOutput = false;
    vSurfaceFireVectorDirFromUpslope->m_isUserOutput = false;
    vSurfaceFireVectorPsi->m_isUserOutput = false;
    vSurfaceFireVectorTheta->m_isUserOutput = false;
    vSurfaceFireWidthDist->m_isUserOutput = false;
    vSurfaceFireWidthMapDist->m_isUserOutput = false;
    vSurfaceFireWindFactor->m_isUserOutput = false;
    vSurfaceFireWindSpeedFlag->m_isUserOutput = false;
    vSurfaceFireWindSpeedLimit->m_isUserOutput = false;
    vSurfaceFuelAspenCuring->m_isUserOutput = false;
    vSurfaceFuelAspenLoadDead1->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenLoadDead1" ) );
    vSurfaceFuelAspenLoadDead10->m_isUserOutput = false;	// Pat says NO
    vSurfaceFuelAspenLoadLiveHerb->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenLoadLiveHerb" ) );
    vSurfaceFuelAspenLoadLiveWoody->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenLoadLiveWoody" ) );
    vSurfaceFuelAspenSavrDead1->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenSavrDead1" ) );
    vSurfaceFuelAspenSavrDead10->m_isUserOutput = false;	// Pat says NO
    vSurfaceFuelAspenSavrLiveHerb->m_isUserOutput = false;	// Pat says NO
    vSurfaceFuelAspenSavrLiveWoody->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenSavrLiveWoody" ) );
    vSurfaceFuelAspenType->m_isUserOutput = false;
    vSurfaceFuelBedBetaRatio->m_isUserOutput = false;
    vSurfaceFuelBedBulkDensity->m_isUserOutput = false;
    vSurfaceFuelBedCoverage1->m_isUserOutput = false;
    vSurfaceFuelBedDeadFraction->m_isUserOutput = false;
    vSurfaceFuelBedLiveFraction->m_isUserOutput = false;
    vSurfaceFuelBedDepth->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoBedDepth" ) );
    vSurfaceFuelBedHeatSink->m_isUserOutput = false;
    vSurfaceFuelBedMextDead->m_isUserOutput = false;
    vSurfaceFuelBedMextLive->m_isUserOutput = false;
    vSurfaceFuelBedModel->m_isUserOutput = false;
    vSurfaceFuelBedModel1->m_isUserOutput = false;
    vSurfaceFuelBedModel2->m_isUserOutput = false;
    vSurfaceFuelBedMoisDead->m_isUserOutput = false;
    vSurfaceFuelBedMoisLive->m_isUserOutput = false;
    vSurfaceFuelBedPackingRatio->m_isUserOutput = false;
    vSurfaceFuelBedSigma->m_isUserOutput = false;
    vSurfaceFuelDens[8]->m_isUserOutput = false;
    vSurfaceFuelHeat[8]->m_isUserOutput = false;
    vSurfaceFuelHeatDead->m_isUserOutput = false;
    vSurfaceFuelHeatLive->m_isUserOutput = false;
    vSurfaceFuelLife[8]->m_isUserOutput = false;
    vSurfaceFuelLoad[8]->m_isUserOutput = false;
    vSurfaceFuelLoadDead->m_isUserOutput = false;
    vSurfaceFuelLoadDead1->m_isUserOutput = false;
    vSurfaceFuelLoadDead10->m_isUserOutput = false;
    vSurfaceFuelLoadDead100->m_isUserOutput = false;
    vSurfaceFuelLoadDeadHerb->m_isUserOutput = false;
    vSurfaceFuelLoadDeadHerb->m_isConstant = surface;
    vSurfaceFuelLoadLive->m_isUserOutput = false;
    vSurfaceFuelLoadLiveHerb->m_isUserOutput = false;
    vSurfaceFuelLoadLiveWood->m_isUserOutput = false;
    vSurfaceFuelLoadTransferEq->m_isUserOutput = false;
    vSurfaceFuelLoadTransferFraction->m_isUserInput = ( surface && opt_mois_input_load_transfer );
    vSurfaceFuelLoadTransferFraction->m_isUserOutput = false;
    vSurfaceFuelLoadUndeadHerb->m_isUserOutput = false;
    vSurfaceFuelMois[8]->m_isUserOutput = false;
    vSurfaceFuelMoisDead1->m_isUserOutput = false;
    vSurfaceFuelMoisDead10->m_isUserOutput = false;
    vSurfaceFuelMoisDead100->m_isUserOutput = false;
    vSurfaceFuelMoisDead1000->m_isUserOutput = false;
    vSurfaceFuelMoisLifeDead->m_isUserOutput = false;
    vSurfaceFuelMoisLifeLive->m_isUserOutput = false;
    vSurfaceFuelMoisLiveHerb->m_isUserOutput = false;
    vSurfaceFuelMoisLiveWood->m_isUserOutput = false;
    vSurfaceFuelMoisScenario->m_isUserOutput = false;
    vSurfaceFuelPalmettoAge->m_isUserOutput = false;
    vSurfaceFuelPalmettoCover->m_isUserOutput = false;
    vSurfaceFuelPalmettoHeight->m_isUserOutput = false;
    vSurfaceFuelPalmettoLoadDead1->m_isUserOutput = 
		( surface && prop->boolean( "surfaceCalcPalmettoLoadDead1" ) );
    vSurfaceFuelPalmettoLoadDead10->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadDead10" ) );
    vSurfaceFuelPalmettoLoadDeadFoliage->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadDeadFoliage" ) );
    vSurfaceFuelPalmettoLoadLitter->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadLitter" ) );
    vSurfaceFuelPalmettoLoadLive1->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadLive1" ) );
    vSurfaceFuelPalmettoLoadLive10->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadLive10" ) );
    vSurfaceFuelPalmettoLoadLiveFoliage->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcPalmettoLoadLiveFoliage" ) );
    vSurfaceFuelPalmettoOverstoryBasalArea->m_isUserOutput = false;
    vSurfaceFuelSavr[8]->m_isUserOutput = false;
    vSurfaceFuelSavrDead1->m_isUserOutput = false;
    vSurfaceFuelSavrLiveHerb->m_isUserOutput = false;
    vSurfaceFuelSavrLiveWood->m_isUserOutput = false;
    vSurfaceFuelSeff[8]->m_isUserOutput = false;
    vSurfaceFuelStot[8]->m_isUserOutput = false;
    vSurfaceFuelTemp->m_isUserOutput = false;
    vTimeIntegerDate->m_isUserOutput = false;
    vTimeJulianDate->m_isUserOutput = false;
    vTreeBarkThickness->m_isUserOutput = false;
    vTreeCanopyBulkDens->m_isUserOutput = false;
    vTreeCanopyCover->m_isUserOutput = false;
    vTreeCanopyCrownFraction->m_isUserOutput = false;
    vTreeCount->m_isUserOutput = false;
    vTreeCoverHt->m_isUserOutput = false;
    vTreeCoverHtDownwind->m_isUserOutput = false;
    vTreeCrownBaseHt->m_isUserOutput = false;
    vTreeCrownLengFractionScorchedAtVector->m_isUserOutput = false;
    vTreeCrownLengScorchedAtVector->m_isUserOutput = false;
    vTreeCrownRatio->m_isUserOutput = false;
    vTreeCrownVolScorchedAtVector->m_isUserOutput = false;
    vTreeDbh->m_isUserOutput = false;
    vTreeFoliarMois->m_isUserOutput = false;
    vTreeHt->m_isUserOutput = false;
    vTreeMortalityCountAtVector->m_isUserOutput = false;
    vTreeMortalityRateAspenAtVector->m_isUserOutput =
        ( surface && prop->boolean( "surfaceCalcAspenMortality" ) );
    vTreeMortalityRateAtVector->m_isUserOutput = false;
    vTreeSpecies->m_isUserOutput = false;
    vTreeSpeciesMortality->m_isUserOutput = false;
    vTreeSpeciesSpot->m_isUserOutput = false;
    vWindAdjFactor->m_isUserOutput = false;
    vWindAdjMethod->m_isUserOutput = false;
    vWindDirFromCompass->m_isUserOutput = false;
    vWindDirFromNorth->m_isUserOutput = false;
    vWindDirFromUpslope->m_isUserOutput = false;
    vWindSpeedAt10M->m_isUserOutput = false;
    vWindSpeedAt20Ft->m_isUserOutput = false;
    vWindSpeedAtMidflame->m_isUserOutput =
        ( surface && ! opt_wind_midflame && prop->boolean( "surfaceCalcWindSpeedAtMidflame" ) );
    vWthrAirTemp->m_isUserOutput = false;
    vWthrCumulusBaseHt->m_isUserOutput = false;
    vWthrDewPointTemp->m_isUserOutput = false;
    vWthrHeatIndex->m_isUserOutput = false;
    vWthrLightningStrikeType->m_isUserOutput = false;
    vWthrRelativeHumidity->m_isUserOutput = false;
    vWthrSummerSimmerIndex->m_isUserOutput = false;
    vWthrWetBulbTemp->m_isUserOutput = false;
    vWthrWindChillTemp->m_isUserOutput = false;

	//--------------------------------------------------------------------------
	// Section 4: set variable attributes based on configuration
	//--------------------------------------------------------------------------

	if ( surface )
	{
		// Keep transfer equation off the worksheet when using weighted fuel models
		if ( surface && opt_fuel_wtd )
		{
			vSurfaceFuelLoadTransferEq->m_isUserInput = false;
			vSurfaceFuelLoadTransferEq->m_isConstant = true;
		}
		// Never do dynamic load transfer for Palmetto-Gallberry or Western Aspen
		if ( opt_fuel_aspen || opt_fuel_pg )
		{
			vSurfaceFuelLoadTransferEq->m_isConstant = true;
			vSurfaceFuelLoadTransferEq->updateItem( 0 );

			vSurfaceFuelLoadTransferFraction->m_isUserInput = false;
			fSurfaceFuelLoadTransferFraction->m_active = true;
			vSurfaceFuelLoadTransferFraction->m_isConstant = true;
			vSurfaceFuelLoadTransferFraction->update( 0.0 );
		}
		if ( opt_wind_midflame )
		{
			vTreeCanopyCrownFraction->m_isConstant = true;
			vTreeCanopyCrownFraction->update( 0.0 );
			vWindAdjFactor->m_isConstant = true;
			vWindAdjFactor->update( 1.0 );
			vWindAdjMethod->m_isConstant = true;
			vWindAdjMethod->updateItem( 2 );
		}
		else if ( opt_wind_20ft || opt_wind_10m )
		{
			// In case this is requested as output
			vTreeCanopyCrownFraction->m_isConstant = true;
			vTreeCanopyCrownFraction->update( 0.0 );
			vWindAdjMethod->m_isConstant = true;
			vWindAdjMethod->updateItem( 2 );
		}
		else if ( opt_wind_20ft_calc || opt_wind_10m_calc )
		{
			vSurfaceFuelBedDepth->m_isConstant = opt_fuel_wtd;	// prevent circular dependency
			vTreeCrownRatio->m_isUserOutput = crown && prop->boolean( "surfaceCalcCrownRatio" );
			// In case this is requested as output
			vTreeCanopyCrownFraction->m_isConstant = false; // Crown Fill Portion
			vTreeCanopyCrownFraction->update( 0.0 );
			vWindAdjMethod->m_isConstant = false;
			vWindAdjMethod->updateItem( 2 );
		}
		if ( opt_spread_head || opt_spread_back || opt_spread_flank )
		{
			vSurfaceFireVectorDirFromUpslope->m_isConstant = true;
			vSurfaceFireVectorDirFromUpslope->update( 0. );
			vSurfaceFireVectorPsi->m_isConstant = true;
			double dir = 0.;
			dir = ( opt_spread_back ) ? 180. : dir;
			dir = ( opt_spread_flank ) ? 90. : dir;
	 		vSurfaceFireVectorPsi->update( dir );
		}
	}
    return;
}

