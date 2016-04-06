//------------------------------------------------------------------------------
/*! \file xeqcalc.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2016 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqTree calculator interface.
 *
 *  This file contains the implementation interface between the generic EqTree
 *  and the specific set of variables and functions.  The idea is to keep all
 *  function and variable details in just 2 files, xeqtree.xml and xeqcalc.cpp.
 *
 *  All variables must be
 *  -# specified in the xeqtree.xml file via
 *          <variable name="vVariableName" type=""... />
 *  -# declared in xeqcalc.h via
 *          EqVar *vVariableName;
 *  -# initialized in EqCalc::EqCalc() constructor via
 *          vVariableName = m_eqTree->getVarPtr( "vVariableName" );
 *
 *  All functions must be
 *  -# specified in xeqtree.xml file via
 *          <function name="fFunctionName" input="" output=""... />
 *  -# declared in xeqcalc.h via
 *          void FunctionName( void );
 *  -# defined as a function in this file via
 *          void EqCalc::FunctionName( void ) { }
 *  -# initialized in EqCalc::EqCalc() constructor via
 *      m_eqTree->setEqFunAddress( "FunctionName", &EqCalc::FunctioName );
 */

// Custom include files
#include "Bp6Globals.h"
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "cdtlib.h"
#include "contain.h"
#include "fuelmodel.h"
#include "moisscenario.h"
#include "parser.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqvar.h"
#include "xeqtree.h"
#include "xfblib.h"

#include "Algorithms/Fofem6BarkThickness.h"
#include "Algorithms/Fofem6Mortality.h"
#include "SemFireCrownFirebrandProcessor.h"

// Qt include files
#include <qstring.h>

// Standard include files
#include <stdlib.h>
#include <string.h>
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief EqCalc constructor.
 */

EqCalc::EqCalc( EqTree *eqTree ) :
    m_eqTree(eqTree),
    m_log(0)
{
	m_Bp6SurfaceFire		= new Bp6SurfaceFire();
	m_Bp6CrownFire			= new Bp6CrownFire();

    vContainAttackBack       = m_eqTree->getVarPtr( "vContainAttackBack" );
    vContainAttackDist       = m_eqTree->getVarPtr( "vContainAttackDist" );
    vContainAttackHead       = m_eqTree->getVarPtr( "vContainAttackHead" );
    vContainAttackPerimeter  = m_eqTree->getVarPtr( "vContainAttackPerimeter" );
    vContainAttackSize       = m_eqTree->getVarPtr( "vContainAttackSize" );
    vContainAttackTactic     = m_eqTree->getVarPtr( "vContainAttackTactic" );
    vContainCost             = m_eqTree->getVarPtr( "vContainCost" );
    vContainDiagram          = m_eqTree->getVarPtr( "vContainDiagram" );
    vContainLimitDist        = m_eqTree->getVarPtr( "vContainLimitDist" );
    vContainLine             = m_eqTree->getVarPtr( "vContainLine" );
    vContainPoints           = m_eqTree->getVarPtr( "vContainPoints" );
    vContainReportBack       = m_eqTree->getVarPtr( "vContainReportBack" );
    vContainReportHead       = m_eqTree->getVarPtr( "vContainReportHead" );
    vContainReportRatio      = m_eqTree->getVarPtr( "vContainReportRatio" );
    vContainReportSize       = m_eqTree->getVarPtr( "vContainReportSize" );
    vContainReportSpread     = m_eqTree->getVarPtr( "vContainReportSpread" );
    vContainResourceArrival  = m_eqTree->getVarPtr( "vContainResourceArrival" );
    vContainResourceBaseCost = m_eqTree->getVarPtr( "vContainResourceBaseCost" );
    vContainResourceDuration = m_eqTree->getVarPtr( "vContainResourceDuration" );
    vContainResourceHourCost = m_eqTree->getVarPtr( "vContainResourceHourCost" );
    vContainResourceName     = m_eqTree->getVarPtr( "vContainResourceName" );
    vContainResourceProd     = m_eqTree->getVarPtr( "vContainResourceProd" );
    vContainResourcesUsed    = m_eqTree->getVarPtr( "vContainResourcesUsed" );
    vContainSize             = m_eqTree->getVarPtr( "vContainSize" );
    vContainStatus           = m_eqTree->getVarPtr( "vContainStatus" );
    vContainTime             = m_eqTree->getVarPtr( "vContainTime" );
    vContainXMax             = m_eqTree->getVarPtr( "vContainXMax" );
    vContainXMin             = m_eqTree->getVarPtr( "vContainXMin" );
    vContainYMax             = m_eqTree->getVarPtr( "vContainYMax" );

	vCrownFireActiveCritOpenWindSpeed	= m_eqTree->getVarPtr( "vCrownFireActiveCritOpenWindSpeed" );
	vCrownFireActiveCritSurfSpreadRate	= m_eqTree->getVarPtr( "vCrownFireActiveCritSurfSpreadRate" );
    vCrownFireActiveCrown				= m_eqTree->getVarPtr( "vCrownFireActiveCrown" );
	vCrownFireActiveFireArea			= m_eqTree->getVarPtr( "vCrownFireActiveFireArea" );
	vCrownFireActiveFireLineInt			= m_eqTree->getVarPtr( "vCrownFireActiveFireLineInt" );
	vCrownFireActiveFirePerimeter		= m_eqTree->getVarPtr( "vCrownFireActiveFirePerimeter" );
	vCrownFireActiveFireWidth			= m_eqTree->getVarPtr( "vCrownFireActiveFireWidth" );
	vCrownFireActiveFlameLeng			= m_eqTree->getVarPtr( "vCrownFireActiveFlameLeng" );
	vCrownFireActiveHeatPerUnitArea		= m_eqTree->getVarPtr( "vCrownFireActiveHeatPerUnitArea" );
    vCrownFireActiveRatio				= m_eqTree->getVarPtr( "vCrownFireActiveRatio" );
	vCrownFireActiveSpreadDist			= m_eqTree->getVarPtr( "vCrownFireActiveSpreadDist" );
	vCrownFireActiveSpreadMapDist		= m_eqTree->getVarPtr( "vCrownFireActiveSpreadMapDist" );
	vCrownFireActiveSpreadRate			= m_eqTree->getVarPtr( "vCrownFireActiveSpreadRate" );
    //vCrownFireArea					= m_eqTree->getVarPtr( "vCrownFireArea" );
	vCrownFireCanopyFractionBurned		= m_eqTree->getVarPtr( "vCrownFireCanopyFractionBurned" );
    vCrownFireCritCrownSpreadRate		= m_eqTree->getVarPtr( "vCrownFireCritCrownSpreadRate" );
    vCrownFireCritSurfFireInt			= m_eqTree->getVarPtr( "vCrownFireCritSurfFireInt" );
    vCrownFireCritSurfFlameLeng			= m_eqTree->getVarPtr( "vCrownFireCritSurfFlameLeng" );
    vCrownFireCritSurfSpreadRate		= m_eqTree->getVarPtr( "vCrownFireCritSurfSpreadRate" );
    //vCrownFireFlameLeng				= m_eqTree->getVarPtr( "vCrownFireFlameLeng" );
    vCrownFireFuelLoad					= m_eqTree->getVarPtr( "vCrownFireFuelLoad" );
	//vCrownFireHeatPerUnitArea			= m_eqTree->getVarPtr( "vCrownFireHeatPerUnitArea" );
    vCrownFireHeatPerUnitAreaCanopy		= m_eqTree->getVarPtr( "vCrownFireHeatPerUnitAreaCanopy" );
	vCrownFireLengthToWidth				= m_eqTree->getVarPtr( "vCrownFireLengthToWidth" );
    //vCrownFireLineInt					= m_eqTree->getVarPtr( "vCrownFireLineInt" );
	vCrownFirePassiveFireArea			= m_eqTree->getVarPtr( "vCrownFirePassiveFireArea" );
	vCrownFirePassiveFireLineInt		= m_eqTree->getVarPtr( "vCrownFirePassiveFireLineInt" );
	vCrownFirePassiveFirePerimeter		= m_eqTree->getVarPtr( "vCrownFirePassiveFirePerimeter" );
	vCrownFirePassiveFireWidth			= m_eqTree->getVarPtr( "vCrownFirePassiveFireWidth" );
	vCrownFirePassiveFlameLeng			= m_eqTree->getVarPtr( "vCrownFirePassiveFlameLeng" );
	vCrownFirePassiveHeatPerUnitArea	= m_eqTree->getVarPtr( "vCrownFirePassiveHeatPerUnitArea" );
	vCrownFirePassiveSpreadDist			= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadDist" );
    vCrownFirePassiveSpreadMapDist		= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadMapDist" );
	vCrownFirePassiveSpreadRate			= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadRate" );
	//vCrownFirePerimeter				= m_eqTree->getVarPtr( "vCrownFirePerimeter" );
    vCrownFirePowerOfFire				= m_eqTree->getVarPtr( "vCrownFirePowerOfFire" );
    vCrownFirePowerOfWind				= m_eqTree->getVarPtr( "vCrownFirePowerOfWind" );
    vCrownFirePowerRatio				= m_eqTree->getVarPtr( "vCrownFirePowerRatio" );
    //vCrownFireSpreadDist				= m_eqTree->getVarPtr( "vCrownFireSpreadDist" );
    //vCrownFireSpreadMapDist			= m_eqTree->getVarPtr( "vCrownFireSpreadMapDist" );
    //vCrownFireSpreadRate				= m_eqTree->getVarPtr( "vCrownFireSpreadRate" );
    vCrownFireTransRatio				= m_eqTree->getVarPtr( "vCrownFireTransRatio" );
    vCrownFireTransToCrown				= m_eqTree->getVarPtr( "vCrownFireTransToCrown" );
    vCrownFireType						= m_eqTree->getVarPtr( "vCrownFireType" );
    vCrownFireWindDriven				= m_eqTree->getVarPtr( "vCrownFireWindDriven" );

    vDocDescription      = m_eqTree->getVarPtr( "vDocDescription" );
    vDocFireAnalyst      = m_eqTree->getVarPtr( "vDocFireAnalyst" );
    vDocFireName         = m_eqTree->getVarPtr( "vDocFireName" );
    vDocFirePeriod       = m_eqTree->getVarPtr( "vDocFirePeriod" );
    vDocRxAdminUnit      = m_eqTree->getVarPtr( "vDocRxAdminUnit" );
    vDocRxName           = m_eqTree->getVarPtr( "vDocRxName" );
    vDocRxPreparedBy     = m_eqTree->getVarPtr( "vDocRxPreparedBy" );
    vDocTrainingCourse   = m_eqTree->getVarPtr( "vDocTrainingCourse" );
    vDocTrainingExercise = m_eqTree->getVarPtr( "vDocTrainingExercise" );
    vDocTrainingTrainee  = m_eqTree->getVarPtr( "vDocTrainingTrainee" );

    vIgnitionFirebrandFuelMois  = m_eqTree->getVarPtr( "vIgnitionFirebrandFuelMois" );
    vIgnitionFirebrandProb      = m_eqTree->getVarPtr( "vIgnitionFirebrandProb" );
    vIgnitionLightningDuffDepth = m_eqTree->getVarPtr( "vIgnitionLightningDuffDepth" );
    vIgnitionLightningFuelMois  = m_eqTree->getVarPtr( "vIgnitionLightningFuelMois" );
    vIgnitionLightningFuelType  = m_eqTree->getVarPtr( "vIgnitionLightningFuelType" );
    vIgnitionLightningProb      = m_eqTree->getVarPtr( "vIgnitionLightningProb" );

    vMapContourCount			= m_eqTree->getVarPtr( "vMapContourCount" );
    vMapContourInterval			= m_eqTree->getVarPtr( "vMapContourInterval" );
    vMapDist					= m_eqTree->getVarPtr( "vMapDist" );
    vMapFraction				= m_eqTree->getVarPtr( "vMapFraction" );
    vMapScale					= m_eqTree->getVarPtr( "vMapScale" );

    vSafetyZoneEquipmentArea	= m_eqTree->getVarPtr( "vSafetyZoneEquipmentArea" );
    vSafetyZoneEquipmentNumber	= m_eqTree->getVarPtr( "vSafetyZoneEquipmentNumber" );
    vSafetyZonePersonnelArea	= m_eqTree->getVarPtr( "vSafetyZonePersonnelArea" );
    vSafetyZonePersonnelNumber	= m_eqTree->getVarPtr( "vSafetyZonePersonnelNumber" );
    vSafetyZoneLength			= m_eqTree->getVarPtr( "vSafetyZoneLength" );
    vSafetyZoneRadius			= m_eqTree->getVarPtr( "vSafetyZoneRadius" );
    vSafetyZoneSepDist			= m_eqTree->getVarPtr( "vSafetyZoneSepDist" );
    vSafetyZoneSize				= m_eqTree->getVarPtr( "vSafetyZoneSize" );
    vSafetyZoneSizeSquare		= m_eqTree->getVarPtr( "vSafetyZoneSizeSquare" );

    vSiteAspectDirFromCompass	= m_eqTree->getVarPtr( "vSiteAspectDirFromCompass" );
    vSiteAspectDirFromNorth		= m_eqTree->getVarPtr( "vSiteAspectDirFromNorth" );
    vSiteElevation				= m_eqTree->getVarPtr( "vSiteElevation" );
    vSiteLatitude				= m_eqTree->getVarPtr( "vSiteLatitude" );
    vSiteLongitude				= m_eqTree->getVarPtr( "vSiteLongitude" );
    vSiteRidgeToValleyDist		= m_eqTree->getVarPtr( "vSiteRidgeToValleyDist" );
    vSiteRidgeToValleyElev		= m_eqTree->getVarPtr( "vSiteRidgeToValleyElev" );
    vSiteRidgeToValleyMapDist	= m_eqTree->getVarPtr( "vSiteRidgeToValleyMapDist" );
    vSiteSlopeDegrees			= m_eqTree->getVarPtr( "vSiteSlopeDegrees" );
    vSiteSlopeFraction			= m_eqTree->getVarPtr( "vSiteSlopeFraction" );
    vSiteSlopeReach				= m_eqTree->getVarPtr( "vSiteSlopeReach" );
    vSiteSlopeRise				= m_eqTree->getVarPtr( "vSiteSlopeRise" );
    vSiteSunShading				= m_eqTree->getVarPtr( "vSiteSunShading" );
    vSiteTimeZone				= m_eqTree->getVarPtr( "vSiteTimeZone" );
    vSiteUpslopeDirFromNorth	= m_eqTree->getVarPtr( "vSiteUpslopeDirFromNorth" );

    vSpotCoverHtBurningPile        = m_eqTree->getVarPtr( "vSpotCoverHtBurningPile" );
    vSpotCoverHtSurfaceFire        = m_eqTree->getVarPtr( "vSpotCoverHtSurfaceFire" );
    vSpotCoverHtTorchingTrees      = m_eqTree->getVarPtr( "vSpotCoverHtTorchingTrees" );
    vSpotDistActiveCrown           = m_eqTree->getVarPtr( "vSpotDistActiveCrown" );
    vSpotDistBurningPile           = m_eqTree->getVarPtr( "vSpotDistBurningPile" );
    vSpotDistSurfaceFire           = m_eqTree->getVarPtr( "vSpotDistSurfaceFire" );
    vSpotDistTorchingTrees         = m_eqTree->getVarPtr( "vSpotDistTorchingTrees" );
    vSpotEmberDiamAtSurface        = m_eqTree->getVarPtr( "vSpotEmberDiamAtSurface" );
    vSpotFirebrandDriftSurfaceFire = m_eqTree->getVarPtr( "vSpotFirebrandDriftSurfaceFire" );
    vSpotFirebrandHtActiveCrown    = m_eqTree->getVarPtr( "vSpotFirebrandHtActiveCrown" );
    vSpotFirebrandHtBurningPile    = m_eqTree->getVarPtr( "vSpotFirebrandHtBurningPile" );
    vSpotFirebrandHtSurfaceFire    = m_eqTree->getVarPtr( "vSpotFirebrandHtSurfaceFire" );
    vSpotFirebrandHtTorchingTrees  = m_eqTree->getVarPtr( "vSpotFirebrandHtTorchingTrees" );
    vSpotFlameDurTorchingTrees     = m_eqTree->getVarPtr( "vSpotFlameDurTorchingTrees" );
    vSpotFlameHtActiveCrown        = m_eqTree->getVarPtr( "vSpotFlameHtActiveCrown" );
    vSpotFlameHtTorchingTrees      = m_eqTree->getVarPtr( "vSpotFlameHtTorchingTrees" );
    vSpotFlameRatioTorchingTrees   = m_eqTree->getVarPtr( "vSpotFlameRatioTorchingTrees" );
    vSpotFlatDistActiveCrown       = m_eqTree->getVarPtr( "vSpotFlatDistActiveCrown" );
    vSpotFlatDistBurningPile       = m_eqTree->getVarPtr( "vSpotFlatDistBurningPile" );
    vSpotFlatDistSurfaceFire       = m_eqTree->getVarPtr( "vSpotFlatDistSurfaceFire" );
    vSpotFlatDistTorchingTrees     = m_eqTree->getVarPtr( "vSpotFlatDistTorchingTrees" );
    vSpotFireSource                = m_eqTree->getVarPtr( "vSpotFireSource" );
    vSpotMapDistActiveCrown        = m_eqTree->getVarPtr( "vSpotMapDistActiveCrown" );
    vSpotMapDistBurningPile        = m_eqTree->getVarPtr( "vSpotMapDistBurningPile" );
    vSpotMapDistSurfaceFire        = m_eqTree->getVarPtr( "vSpotMapDistSurfaceFire" );
    vSpotMapDistTorchingTrees      = m_eqTree->getVarPtr( "vSpotMapDistTorchingTrees" );
    vSpotTorchingTrees             = m_eqTree->getVarPtr( "vSpotTorchingTrees" );

    vSurfaceFireArea                 = m_eqTree->getVarPtr( "vSurfaceFireArea" );
    vSurfaceFireCharacteristicsDiagram = m_eqTree->getVarPtr( "vSurfaceFireCharacteristicsDiagram" );
    vSurfaceFireDistAtBack           = m_eqTree->getVarPtr( "vSurfaceFireDistAtBack" );
    vSurfaceFireDistAtBeta           = m_eqTree->getVarPtr( "vSurfaceFireDistAtBeta" );
    vSurfaceFireDistAtFlank          = m_eqTree->getVarPtr( "vSurfaceFireDistAtFlank" );
    vSurfaceFireDistAtHead           = m_eqTree->getVarPtr( "vSurfaceFireDistAtHead" );
    vSurfaceFireDistAtPsi            = m_eqTree->getVarPtr( "vSurfaceFireDistAtPsi" );
    vSurfaceFireDistAtVector         = m_eqTree->getVarPtr( "vSurfaceFireDistAtVector" );
    vSurfaceFireEccentricity         = m_eqTree->getVarPtr( "vSurfaceFireEccentricity" );
    vSurfaceFireEllipseF             = m_eqTree->getVarPtr( "vSurfaceFireEllipseF" );
    vSurfaceFireEllipseG             = m_eqTree->getVarPtr( "vSurfaceFireEllipseG" );
    vSurfaceFireEllipseH             = m_eqTree->getVarPtr( "vSurfaceFireEllipseH" );
    vSurfaceFireEffWindAtHead        = m_eqTree->getVarPtr( "vSurfaceFireEffWindAtHead" );
    vSurfaceFireEffWindAtVector      = m_eqTree->getVarPtr( "vSurfaceFireEffWindAtVector" );
    vSurfaceFireElapsedTime          = m_eqTree->getVarPtr( "vSurfaceFireElapsedTime" );
    vSurfaceFireFlameAngleAtVector   = m_eqTree->getVarPtr( "vSurfaceFireFlameAngleAtVector" );
    vSurfaceFireFlameHtAtVector      = m_eqTree->getVarPtr( "vSurfaceFireFlameHtAtVector" );
    vSurfaceFireFlameHtPile          = m_eqTree->getVarPtr( "vSurfaceFireFlameHtPile" );
    vSurfaceFireFlameLengAtBeta      = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtBeta" );
    vSurfaceFireFlameLengAtHead      = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtHead" );
    vSurfaceFireFlameLengAtPsi       = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtPsi" );
    vSurfaceFireFlameLengAtVector    = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtVector" );
    vSurfaceFireHeatPerUnitArea      = m_eqTree->getVarPtr( "vSurfaceFireHeatPerUnitArea" );
    vSurfaceFireHeatSource           = m_eqTree->getVarPtr( "vSurfaceFireHeatSource" );
    vSurfaceFireLengDist             = m_eqTree->getVarPtr( "vSurfaceFireLengDist" );
    vSurfaceFireLengMapDist          = m_eqTree->getVarPtr( "vSurfaceFireLengMapDist" );
    vSurfaceFireLengthToWidth        = m_eqTree->getVarPtr( "vSurfaceFireLengthToWidth" );
    vSurfaceFireLineIntAtBeta        = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtBeta" );
    vSurfaceFireLineIntAtHead        = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtHead" );
    vSurfaceFireLineIntAtPsi         = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtPsi" );
    vSurfaceFireLineIntAtVector      = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtVector" );
    vSurfaceFireMapDistAtBack        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtBack" );
    vSurfaceFireMapDistAtBeta        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtBeta" );
    vSurfaceFireMapDistAtFlank       = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtFlank" );
    vSurfaceFireMapDistAtHead        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtHead" );
    vSurfaceFireMapDistAtPsi         = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtPsi" );
    vSurfaceFireMapDistAtVector      = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtVector" );
    vSurfaceFireMaxDirDiagram        = m_eqTree->getVarPtr( "vSurfaceFireMaxDirDiagram" );
    vSurfaceFireMaxDirFromNorth      = m_eqTree->getVarPtr( "vSurfaceFireMaxDirFromNorth" );
    vSurfaceFireMaxDirFromUpslope    = m_eqTree->getVarPtr( "vSurfaceFireMaxDirFromUpslope" );
    vSurfaceFireNoWindRate           = m_eqTree->getVarPtr( "vSurfaceFireNoWindRate" );
    vSurfaceFirePerimeter            = m_eqTree->getVarPtr( "vSurfaceFirePerimeter" );
    vSurfaceFirePropagatingFlux      = m_eqTree->getVarPtr( "vSurfaceFirePropagatingFlux" );
    vSurfaceFireReactionInt          = m_eqTree->getVarPtr( "vSurfaceFireReactionInt" );
    vSurfaceFireReactionIntDead      = m_eqTree->getVarPtr( "vSurfaceFireReactionIntDead" );
    vSurfaceFireReactionIntLive      = m_eqTree->getVarPtr( "vSurfaceFireReactionIntLive" );
    vSurfaceFireResidenceTime        = m_eqTree->getVarPtr( "vSurfaceFireResidenceTime" );
    vSurfaceFireScorchHtAtVector     = m_eqTree->getVarPtr( "vSurfaceFireScorchHtAtVector" );
    vSurfaceFireSeverityAspen        = m_eqTree->getVarPtr( "vSurfaceFireSeverityAspen" );
    vSurfaceFireShapeDiagram         = m_eqTree->getVarPtr( "vSurfaceFireShapeDiagram" );
    vSurfaceFireSlopeFactor          = m_eqTree->getVarPtr( "vSurfaceFireSlopeFactor" );
    vSurfaceFireSpreadAtBack         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtBack" );
    vSurfaceFireSpreadAtBeta         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtBeta" );
    vSurfaceFireSpreadAtFlank        = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtFlank" );
    vSurfaceFireSpreadAtHead         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtHead" );
    vSurfaceFireSpreadAtPsi          = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtPsi" );
    vSurfaceFireSpreadAtVector       = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtVector" );
    vSurfaceFireVectorBeta           = m_eqTree->getVarPtr( "vSurfaceFireVectorBeta" );
    vSurfaceFireVectorDirFromCompass = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromCompass" );
    vSurfaceFireVectorDirFromNorth   = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromNorth" );
    vSurfaceFireVectorDirFromUpslope = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromUpslope" );
    vSurfaceFireVectorPsi            = m_eqTree->getVarPtr( "vSurfaceFireVectorPsi" );
    vSurfaceFireVectorTheta          = m_eqTree->getVarPtr( "vSurfaceFireVectorTheta" );
    vSurfaceFireWidthDist            = m_eqTree->getVarPtr( "vSurfaceFireWidthDist" );
    vSurfaceFireWidthMapDist         = m_eqTree->getVarPtr( "vSurfaceFireWidthMapDist" );
    vSurfaceFireWindFactor           = m_eqTree->getVarPtr( "vSurfaceFireWindFactor" );
    vSurfaceFireWindFactorB          = m_eqTree->getVarPtr( "vSurfaceFireWindFactorB" );
    vSurfaceFireWindFactorK          = m_eqTree->getVarPtr( "vSurfaceFireWindFactorK" );
    vSurfaceFireWindSpeedFlag        = m_eqTree->getVarPtr( "vSurfaceFireWindSpeedFlag" );
    vSurfaceFireWindSpeedLimit       = m_eqTree->getVarPtr( "vSurfaceFireWindSpeedLimit" );

    vSurfaceFuelAspenCuring          = m_eqTree->getVarPtr( "vSurfaceFuelAspenCuring" );
    vSurfaceFuelAspenLoadDead1       = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadDead1" );
    vSurfaceFuelAspenLoadDead10      = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadDead10" );
    vSurfaceFuelAspenLoadLiveHerb    = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadLiveHerb" );
    vSurfaceFuelAspenLoadLiveWoody   = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadLiveWoody" );
    vSurfaceFuelAspenSavrDead1       = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrDead1" );
    vSurfaceFuelAspenSavrDead10      = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrDead10" );
    vSurfaceFuelAspenSavrLiveHerb    = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrLiveHerb" );
    vSurfaceFuelAspenSavrLiveWoody   = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrLiveWoody" );
    vSurfaceFuelAspenType            = m_eqTree->getVarPtr( "vSurfaceFuelAspenType" );

    vSurfaceFuelBedBetaRatio    = m_eqTree->getVarPtr( "vSurfaceFuelBedBetaRatio" );
    vSurfaceFuelBedBulkDensity  = m_eqTree->getVarPtr( "vSurfaceFuelBedBulkDensity" );
    vSurfaceFuelBedCoverage1    = m_eqTree->getVarPtr( "vSurfaceFuelBedCoverage1" );
    vSurfaceFuelBedDeadFraction = m_eqTree->getVarPtr( "vSurfaceFuelBedDeadFraction" );
    vSurfaceFuelBedLiveFraction = m_eqTree->getVarPtr( "vSurfaceFuelBedLiveFraction" );
    vSurfaceFuelBedDepth        = m_eqTree->getVarPtr( "vSurfaceFuelBedDepth" );
    vSurfaceFuelBedHeatSink     = m_eqTree->getVarPtr( "vSurfaceFuelBedHeatSink" );
    vSurfaceFuelBedMextDead     = m_eqTree->getVarPtr( "vSurfaceFuelBedMextDead" );
    vSurfaceFuelBedMextLive     = m_eqTree->getVarPtr( "vSurfaceFuelBedMextLive" );
    vSurfaceFuelBedModel        = m_eqTree->getVarPtr( "vSurfaceFuelBedModel" );
    vSurfaceFuelBedModel1       = m_eqTree->getVarPtr( "vSurfaceFuelBedModel1" );
    vSurfaceFuelBedModel2       = m_eqTree->getVarPtr( "vSurfaceFuelBedModel2" );
    vSurfaceFuelBedModelCode    = m_eqTree->getVarPtr( "vSurfaceFuelBedModelCode" );
    vSurfaceFuelBedModelNumber  = m_eqTree->getVarPtr( "vSurfaceFuelBedModelNumber" );
    vSurfaceFuelBedMoisDead     = m_eqTree->getVarPtr( "vSurfaceFuelBedMoisDead" );
    vSurfaceFuelBedMoisLive     = m_eqTree->getVarPtr( "vSurfaceFuelBedMoisLive" );
    vSurfaceFuelBedPackingRatio = m_eqTree->getVarPtr( "vSurfaceFuelBedPackingRatio" );
    vSurfaceFuelBedSigma        = m_eqTree->getVarPtr( "vSurfaceFuelBedSigma" );

    vSurfaceFuelChaparralAge			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralAge" );
    vSurfaceFuelChaparralDeadFuelFraction=m_eqTree->getVarPtr( "vSurfaceFuelChaparralDeadFuelFraction" );
    vSurfaceFuelChaparralDepth			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralDepth" );
    vSurfaceFuelChaparralLoadDead1		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead1" );
    vSurfaceFuelChaparralLoadDead2		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead2" );
    vSurfaceFuelChaparralLoadDead3		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead3" );
    vSurfaceFuelChaparralLoadDead4		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead4" );
    vSurfaceFuelChaparralLoadLiveLeaf	= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLiveLeaf" );
    vSurfaceFuelChaparralLoadLive1		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive1" );
    vSurfaceFuelChaparralLoadLive2		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive2" );
    vSurfaceFuelChaparralLoadLive3		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive3" );
    vSurfaceFuelChaparralLoadLive4		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive4" );
    vSurfaceFuelChaparralLoadTotal		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotal" );
    vSurfaceFuelChaparralLoadTotalDead  = m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotalDead" );
    vSurfaceFuelChaparralLoadTotalLive  = m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotalLive" );
    vSurfaceFuelChaparralType			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralType" );

    vSurfaceFuelDens[0] = m_eqTree->getVarPtr( "vSurfaceFuelDens0" );
    vSurfaceFuelDens[1] = m_eqTree->getVarPtr( "vSurfaceFuelDens1" );
    vSurfaceFuelDens[2] = m_eqTree->getVarPtr( "vSurfaceFuelDens2" );
    vSurfaceFuelDens[3] = m_eqTree->getVarPtr( "vSurfaceFuelDens3" );
    vSurfaceFuelDens[4] = m_eqTree->getVarPtr( "vSurfaceFuelDens4" );
    vSurfaceFuelDens[5] = m_eqTree->getVarPtr( "vSurfaceFuelDens5" );
    vSurfaceFuelDens[6] = m_eqTree->getVarPtr( "vSurfaceFuelDens6" );
    vSurfaceFuelDens[7] = m_eqTree->getVarPtr( "vSurfaceFuelDens7" );
    vSurfaceFuelDens[8] = m_eqTree->getVarPtr( "vSurfaceFuelDens8" );
    vSurfaceFuelDens[9] = m_eqTree->getVarPtr( "vSurfaceFuelDens9" );

    vSurfaceFuelHeat[0] = m_eqTree->getVarPtr( "vSurfaceFuelHeat0" );
    vSurfaceFuelHeat[1] = m_eqTree->getVarPtr( "vSurfaceFuelHeat1" );
    vSurfaceFuelHeat[2] = m_eqTree->getVarPtr( "vSurfaceFuelHeat2" );
    vSurfaceFuelHeat[3] = m_eqTree->getVarPtr( "vSurfaceFuelHeat3" );
    vSurfaceFuelHeat[4] = m_eqTree->getVarPtr( "vSurfaceFuelHeat4" );
    vSurfaceFuelHeat[5] = m_eqTree->getVarPtr( "vSurfaceFuelHeat5" );
    vSurfaceFuelHeat[6] = m_eqTree->getVarPtr( "vSurfaceFuelHeat6" );
    vSurfaceFuelHeat[7] = m_eqTree->getVarPtr( "vSurfaceFuelHeat7" );
    vSurfaceFuelHeat[8] = m_eqTree->getVarPtr( "vSurfaceFuelHeat8" );
    vSurfaceFuelHeat[9] = m_eqTree->getVarPtr( "vSurfaceFuelHeat9" );
    vSurfaceFuelHeatDead = m_eqTree->getVarPtr( "vSurfaceFuelHeatDead" );
    vSurfaceFuelHeatLive = m_eqTree->getVarPtr( "vSurfaceFuelHeatLive" );

    vSurfaceFuelLife[0] = m_eqTree->getVarPtr( "vSurfaceFuelLife0" );
    vSurfaceFuelLife[1] = m_eqTree->getVarPtr( "vSurfaceFuelLife1" );
    vSurfaceFuelLife[2] = m_eqTree->getVarPtr( "vSurfaceFuelLife2" );
    vSurfaceFuelLife[3] = m_eqTree->getVarPtr( "vSurfaceFuelLife3" );
    vSurfaceFuelLife[4] = m_eqTree->getVarPtr( "vSurfaceFuelLife4" );
    vSurfaceFuelLife[5] = m_eqTree->getVarPtr( "vSurfaceFuelLife5" );
    vSurfaceFuelLife[6] = m_eqTree->getVarPtr( "vSurfaceFuelLife6" );
    vSurfaceFuelLife[7] = m_eqTree->getVarPtr( "vSurfaceFuelLife7" );
    vSurfaceFuelLife[8] = m_eqTree->getVarPtr( "vSurfaceFuelLife8" );
    vSurfaceFuelLife[9] = m_eqTree->getVarPtr( "vSurfaceFuelLife9" );

    vSurfaceFuelLoad[0] = m_eqTree->getVarPtr( "vSurfaceFuelLoad0" );
    vSurfaceFuelLoad[1] = m_eqTree->getVarPtr( "vSurfaceFuelLoad1" );
    vSurfaceFuelLoad[2] = m_eqTree->getVarPtr( "vSurfaceFuelLoad2" );
    vSurfaceFuelLoad[3] = m_eqTree->getVarPtr( "vSurfaceFuelLoad3" );
    vSurfaceFuelLoad[4] = m_eqTree->getVarPtr( "vSurfaceFuelLoad4" );
    vSurfaceFuelLoad[5] = m_eqTree->getVarPtr( "vSurfaceFuelLoad5" );
    vSurfaceFuelLoad[6] = m_eqTree->getVarPtr( "vSurfaceFuelLoad6" );
    vSurfaceFuelLoad[7] = m_eqTree->getVarPtr( "vSurfaceFuelLoad7" );
    vSurfaceFuelLoad[8] = m_eqTree->getVarPtr( "vSurfaceFuelLoad8" );
    vSurfaceFuelLoad[9] = m_eqTree->getVarPtr( "vSurfaceFuelLoad9" );

    vSurfaceFuelLoadDead       = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead" );
    vSurfaceFuelLoadDead1      = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead1" );
    vSurfaceFuelLoadDead10     = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead10" );
    vSurfaceFuelLoadDead100    = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead100" );
    vSurfaceFuelLoadDeadHerb   = m_eqTree->getVarPtr( "vSurfaceFuelLoadDeadHerb" );
    vSurfaceFuelLoadLive       = m_eqTree->getVarPtr( "vSurfaceFuelLoadLive" );
    vSurfaceFuelLoadLiveHerb   = m_eqTree->getVarPtr( "vSurfaceFuelLoadLiveHerb" );
    vSurfaceFuelLoadLiveWood   = m_eqTree->getVarPtr( "vSurfaceFuelLoadLiveWood" );
    vSurfaceFuelLoadTransferEq = m_eqTree->getVarPtr( "vSurfaceFuelLoadTransferEq" );
    vSurfaceFuelLoadTransferFraction = m_eqTree->getVarPtr( "vSurfaceFuelLoadTransferFraction" );
    vSurfaceFuelLoadUndeadHerb = m_eqTree->getVarPtr( "vSurfaceFuelLoadUndeadHerb" );

    vSurfaceFuelMois[0] = m_eqTree->getVarPtr( "vSurfaceFuelMois0" );
    vSurfaceFuelMois[1] = m_eqTree->getVarPtr( "vSurfaceFuelMois1" );
    vSurfaceFuelMois[2] = m_eqTree->getVarPtr( "vSurfaceFuelMois2" );
    vSurfaceFuelMois[3] = m_eqTree->getVarPtr( "vSurfaceFuelMois3" );
    vSurfaceFuelMois[4] = m_eqTree->getVarPtr( "vSurfaceFuelMois4" );
    vSurfaceFuelMois[5] = m_eqTree->getVarPtr( "vSurfaceFuelMois5" );
    vSurfaceFuelMois[6] = m_eqTree->getVarPtr( "vSurfaceFuelMois6" );
    vSurfaceFuelMois[7] = m_eqTree->getVarPtr( "vSurfaceFuelMois7" );
    vSurfaceFuelMois[8] = m_eqTree->getVarPtr( "vSurfaceFuelMois8" );
    vSurfaceFuelMois[9] = m_eqTree->getVarPtr( "vSurfaceFuelMois9" );

    vSurfaceFuelMoisDead1    = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead1" );
    vSurfaceFuelMoisDead10   = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead10" );
    vSurfaceFuelMoisDead100  = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead100" );
    vSurfaceFuelMoisDead1000 = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead1000" );
    vSurfaceFuelMoisLifeDead = m_eqTree->getVarPtr( "vSurfaceFuelMoisLifeDead" );
    vSurfaceFuelMoisLifeLive = m_eqTree->getVarPtr( "vSurfaceFuelMoisLifeLive" );
    vSurfaceFuelMoisLiveHerb = m_eqTree->getVarPtr( "vSurfaceFuelMoisLiveHerb" );
    vSurfaceFuelMoisLiveWood = m_eqTree->getVarPtr( "vSurfaceFuelMoisLiveWood" );
    vSurfaceFuelMoisScenario = m_eqTree->getVarPtr( "vSurfaceFuelMoisScenario" );

    vSurfaceFuelPalmettoAge             = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoAge" );
    vSurfaceFuelPalmettoCover           = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoCover" );
    vSurfaceFuelPalmettoHeight          = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoHeight" );
    vSurfaceFuelPalmettoLoadDead1       = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDead1" );
    vSurfaceFuelPalmettoLoadDead10      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDead10" );
    vSurfaceFuelPalmettoLoadDeadFoliage = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDeadFoliage" );
    vSurfaceFuelPalmettoLoadLitter      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLitter" );
    vSurfaceFuelPalmettoLoadLive1       = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLive1" );
    vSurfaceFuelPalmettoLoadLive10      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLive10" );
    vSurfaceFuelPalmettoLoadLiveFoliage = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLiveFoliage" );
    vSurfaceFuelPalmettoOverstoryBasalArea = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoOverstoryBasalArea" );

    vSurfaceFuelSavr[0] = m_eqTree->getVarPtr( "vSurfaceFuelSavr0" );
    vSurfaceFuelSavr[1] = m_eqTree->getVarPtr( "vSurfaceFuelSavr1" );
    vSurfaceFuelSavr[2] = m_eqTree->getVarPtr( "vSurfaceFuelSavr2" );
    vSurfaceFuelSavr[3] = m_eqTree->getVarPtr( "vSurfaceFuelSavr3" );
    vSurfaceFuelSavr[4] = m_eqTree->getVarPtr( "vSurfaceFuelSavr4" );
    vSurfaceFuelSavr[5] = m_eqTree->getVarPtr( "vSurfaceFuelSavr5" );
    vSurfaceFuelSavr[6] = m_eqTree->getVarPtr( "vSurfaceFuelSavr6" );
    vSurfaceFuelSavr[7] = m_eqTree->getVarPtr( "vSurfaceFuelSavr7" );
    vSurfaceFuelSavr[8] = m_eqTree->getVarPtr( "vSurfaceFuelSavr8" );
    vSurfaceFuelSavr[9] = m_eqTree->getVarPtr( "vSurfaceFuelSavr9" );

    vSurfaceFuelSavrDead1    = m_eqTree->getVarPtr( "vSurfaceFuelSavrDead1" );
    vSurfaceFuelSavrLiveHerb = m_eqTree->getVarPtr( "vSurfaceFuelSavrLiveHerb" );
    vSurfaceFuelSavrLiveWood = m_eqTree->getVarPtr( "vSurfaceFuelSavrLiveWood" );

    vSurfaceFuelSeff[0] = m_eqTree->getVarPtr( "vSurfaceFuelSeff0" );
    vSurfaceFuelSeff[1] = m_eqTree->getVarPtr( "vSurfaceFuelSeff1" );
    vSurfaceFuelSeff[2] = m_eqTree->getVarPtr( "vSurfaceFuelSeff2" );
    vSurfaceFuelSeff[3] = m_eqTree->getVarPtr( "vSurfaceFuelSeff3" );
    vSurfaceFuelSeff[4] = m_eqTree->getVarPtr( "vSurfaceFuelSeff4" );
    vSurfaceFuelSeff[5] = m_eqTree->getVarPtr( "vSurfaceFuelSeff5" );
    vSurfaceFuelSeff[6] = m_eqTree->getVarPtr( "vSurfaceFuelSeff6" );
    vSurfaceFuelSeff[7] = m_eqTree->getVarPtr( "vSurfaceFuelSeff7" );
    vSurfaceFuelSeff[8] = m_eqTree->getVarPtr( "vSurfaceFuelSeff8" );
    vSurfaceFuelSeff[9] = m_eqTree->getVarPtr( "vSurfaceFuelSeff9" );

    vSurfaceFuelStot[0] = m_eqTree->getVarPtr( "vSurfaceFuelStot0" );
    vSurfaceFuelStot[1] = m_eqTree->getVarPtr( "vSurfaceFuelStot1" );
    vSurfaceFuelStot[2] = m_eqTree->getVarPtr( "vSurfaceFuelStot2" );
    vSurfaceFuelStot[3] = m_eqTree->getVarPtr( "vSurfaceFuelStot3" );
    vSurfaceFuelStot[4] = m_eqTree->getVarPtr( "vSurfaceFuelStot4" );
    vSurfaceFuelStot[5] = m_eqTree->getVarPtr( "vSurfaceFuelStot5" );
    vSurfaceFuelStot[6] = m_eqTree->getVarPtr( "vSurfaceFuelStot6" );
    vSurfaceFuelStot[7] = m_eqTree->getVarPtr( "vSurfaceFuelStot7" );
    vSurfaceFuelStot[8] = m_eqTree->getVarPtr( "vSurfaceFuelStot8" );
    vSurfaceFuelStot[9] = m_eqTree->getVarPtr( "vSurfaceFuelStot9" );

    vSurfaceFuelTemp    = m_eqTree->getVarPtr( "vSurfaceFuelTemp" );

    vTimeIntegerDate = m_eqTree->getVarPtr( "vTimeIntegerDate" );
    vTimeJulianDate  = m_eqTree->getVarPtr( "vTimeJulianDate" );

    vTreeBarkThickness            = m_eqTree->getVarPtr( "vTreeBarkThickness" );
    vTreeCanopyBulkDens           = m_eqTree->getVarPtr( "vTreeCanopyBulkDens" );
    vTreeCanopyCrownFraction      = m_eqTree->getVarPtr( "vTreeCanopyCrownFraction" );
    vTreeCanopyCover              = m_eqTree->getVarPtr( "vTreeCanopyCover" );
    vTreeCanopyCoverDownwind      = m_eqTree->getVarPtr( "vTreeCanopyCoverDownwind" );
    vTreeCount                    = m_eqTree->getVarPtr( "vTreeCount" );
    vTreeCoverHt                  = m_eqTree->getVarPtr( "vTreeCoverHt" );
    vTreeCoverHtDownwind          = m_eqTree->getVarPtr( "vTreeCoverHtDownwind" );
    vTreeCrownBaseHt              = m_eqTree->getVarPtr( "vTreeCrownBaseHt" );
    vTreeCrownLengFractionScorchedAtVector = m_eqTree->getVarPtr( "vTreeCrownLengFractionScorchedAtVector" );
    vTreeCrownLengScorchedAtVector= m_eqTree->getVarPtr( "vTreeCrownLengScorchedAtVector" );
    vTreeCrownRatio               = m_eqTree->getVarPtr( "vTreeCrownRatio" );
    vTreeCrownVolScorchedAtVector = m_eqTree->getVarPtr( "vTreeCrownVolScorchedAtVector" );
    vTreeDbh                      = m_eqTree->getVarPtr( "vTreeDbh" );
    vTreeFoliarMois               = m_eqTree->getVarPtr( "vTreeFoliarMois" );
    vTreeHt                       = m_eqTree->getVarPtr( "vTreeHt" );
    vTreeMortalityCountAtVector   = m_eqTree->getVarPtr( "vTreeMortalityCountAtVector" );
    vTreeMortalityRateAspenAtVector = m_eqTree->getVarPtr( "vTreeMortalityRateAspenAtVector" );
    vTreeMortalityRateAtVector    = m_eqTree->getVarPtr( "vTreeMortalityRateAtVector" );
    vTreeSpecies                  = m_eqTree->getVarPtr( "vTreeSpecies" );
    vTreeSpeciesMortality         = m_eqTree->getVarPtr( "vTreeSpeciesMortality" );
    vTreeSpeciesSpot              = m_eqTree->getVarPtr( "vTreeSpeciesSpot" );

    vWindAdjFactor       = m_eqTree->getVarPtr( "vWindAdjFactor" );
    vWindAdjMethod       = m_eqTree->getVarPtr( "vWindAdjMethod" );
    vWindDirFromCompass  = m_eqTree->getVarPtr( "vWindDirFromCompass" );
    vWindDirFromNorth    = m_eqTree->getVarPtr( "vWindDirFromNorth" );
    vWindDirFromUpslope  = m_eqTree->getVarPtr( "vWindDirFromUpslope" );
    vWindSpeedAt10M      = m_eqTree->getVarPtr( "vWindSpeedAt10M" );
    vWindSpeedAt20Ft     = m_eqTree->getVarPtr( "vWindSpeedAt20Ft" );
    vWindSpeedAtMidflame = m_eqTree->getVarPtr( "vWindSpeedAtMidflame" );

    vWthrAirTemp             = m_eqTree->getVarPtr( "vWthrAirTemp" );
    vWthrCumulusBaseHt       = m_eqTree->getVarPtr( "vWthrCumulusBaseHt" );
    vWthrDewPointTemp        = m_eqTree->getVarPtr( "vWthrDewPointTemp" );
    vWthrHeatIndex           = m_eqTree->getVarPtr( "vWthrHeatIndex" );
    vWthrLightningStrikeType = m_eqTree->getVarPtr( "vWthrLightningStrikeType" );
    vWthrRelativeHumidity    = m_eqTree->getVarPtr( "vWthrRelativeHumidity" );
    vWthrSummerSimmerIndex   = m_eqTree->getVarPtr( "vWthrSummerSimmerIndex" );
    vWthrWetBulbTemp         = m_eqTree->getVarPtr( "vWthrWetBulbTemp" );
    vWthrWindChillTemp       = m_eqTree->getVarPtr( "vWthrWindChillTemp" );

    fContainFF             = m_eqTree->setEqFunAddress( "fContainFF", &EqCalc::ContainFF );
    fContainFFReportRatio  = m_eqTree->setEqFunAddress( "fContainFFReportRatio", &EqCalc::ContainFFReportRatio );
    fContainFFReportSize   = m_eqTree->setEqFunAddress( "fContainFFReportSize", &EqCalc::ContainFFReportSize );
    fContainFFReportSpread = m_eqTree->setEqFunAddress( "fContainFFReportSpread", &EqCalc::ContainFFReportSpread );
    fContainFFSingle       = m_eqTree->setEqFunAddress( "fContainFFSingle", &EqCalc::ContainFFSingle );

	fCrownFireActiveCritOpenWindSpeed = m_eqTree->setEqFunAddress( "fCrownFireActiveCritOpenWindSpeed", &EqCalc::V6CrownFireActiveCriticalOpenWindSpeed );
	fCrownFireActiveCritSurfSpreadRate = m_eqTree->setEqFunAddress( "fCrownFireActiveCritSurfSpreadRate", &EqCalc::V6CrownFireActiveCriticalSurfaceSpreadRate );
	fCrownFireActiveCrown         = m_eqTree->setEqFunAddress( "fCrownFireActiveCrown", &EqCalc::V6CrownFireActiveCrown );
    fCrownFireActiveRatio         = m_eqTree->setEqFunAddress( "fCrownFireActiveRatio", &EqCalc::V6CrownFireActiveRatio );
    fCrownFireActiveFireArea      = m_eqTree->setEqFunAddress( "fCrownFireActiveFireArea", &EqCalc::V6CrownFireActiveFireArea );
    fCrownFireActiveFireLineInt   = m_eqTree->setEqFunAddress( "fCrownFireActiveFireLineInt", &EqCalc::V6CrownFireActiveFireLineIntensity );
    fCrownFireActiveFireLineIntFromFlameLeng = m_eqTree->setEqFunAddress( "fCrownFireActiveFireLineIntFromFlameLeng", &EqCalc::V6CrownFireActiveFireLineIntensityFromFlameLength );
    fCrownFireActiveFirePerimeter = m_eqTree->setEqFunAddress( "fCrownFireActiveFirePerimeter", &EqCalc::V6CrownFireActiveFirePerimeter );
    fCrownFireActiveFireWidth     = m_eqTree->setEqFunAddress( "fCrownFireActiveFireWidth", &EqCalc::V6CrownFireActiveFireWidth );
    fCrownFireActiveFlameLeng     = m_eqTree->setEqFunAddress( "fCrownFireActiveFlameLeng", &EqCalc::V6CrownFireActiveFlameLength );
    fCrownFireActiveHeatPerUnitArea = m_eqTree->setEqFunAddress( "fCrownFireActiveHeatPerUnitArea", &EqCalc::V6CrownFireActiveHeatPerUnitArea );
    fCrownFireActiveSpreadDist    = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadDist", &EqCalc::V6CrownFireActiveSpreadDist );
    fCrownFireActiveSpreadMapDist = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadMapDist", &EqCalc::V6CrownFireActiveSpreadMapDist );
    fCrownFireActiveSpreadRate    = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadRate", &EqCalc::V6CrownFireActiveSpreadRate );
	fCrownFireCanopyFractionBurned = m_eqTree->setEqFunAddress( "fCrownFireCanopyFractionBurned", &EqCalc::V6CrownFireCanopyFractionBurned );
	fCrownFireCritCrownSpreadRate = m_eqTree->setEqFunAddress( "fCrownFireCritCrownSpreadRate", &EqCalc::V6CrownFireCritCrownSpreadRate );
    fCrownFireCritSurfFireInt     = m_eqTree->setEqFunAddress( "fCrownFireCritSurfFireInt", &EqCalc::V6CrownFireCritSurfFireInt );
    fCrownFireCritSurfFlameLeng   = m_eqTree->setEqFunAddress( "fCrownFireCritSurfFlameLeng", &EqCalc::V6CrownFireCritSurfFlameLeng );
    fCrownFireCritSurfSpreadRate  = m_eqTree->setEqFunAddress( "fCrownFireCritSurfSpreadRate", &EqCalc::V6CrownFireCritSurfSpreadRate );
    fCrownFireFuelLoad            = m_eqTree->setEqFunAddress( "fCrownFireFuelLoad", &EqCalc::V6CrownFireFuelLoad );
    fCrownFireHeatPerUnitAreaCanopy = m_eqTree->setEqFunAddress( "fCrownFireHeatPerUnitAreaCanopy", &EqCalc::V6CrownFireHeatPerUnitAreaCanopy );
    fCrownFireLengthToWidth       = m_eqTree->setEqFunAddress( "fCrownFireLengthToWidth", &EqCalc::V6CrownFireLengthToWidth );
    fCrownFirePassiveFireArea     = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireArea", &EqCalc::V6CrownFirePassiveFireArea );
    fCrownFirePassiveFireLineInt  = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireLineInt", &EqCalc::V6CrownFirePassiveFireLineIntensity );
    fCrownFirePassiveFirePerimeter= m_eqTree->setEqFunAddress( "fCrownFirePassiveFirePerimeter", &EqCalc::V6CrownFirePassiveFirePerimeter );
    fCrownFirePassiveFireWidth    = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireWidth", &EqCalc::V6CrownFirePassiveFireWidth );
    fCrownFirePassiveFlameLeng    = m_eqTree->setEqFunAddress( "fCrownFirePassiveFlameLeng", &EqCalc::V6CrownFirePassiveFlameLength );
    fCrownFirePassiveHeatPerUnitArea = m_eqTree->setEqFunAddress( "fCrownFirePassiveHeatPerUnitArea", &EqCalc::V6CrownFirePassiveHeatPerUnitArea );
    fCrownFirePassiveSpreadDist   = m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadDist", &EqCalc::V6CrownFirePassiveSpreadDist );
    fCrownFirePassiveSpreadMapDist= m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadMapDist", &EqCalc::V6CrownFirePassiveSpreadMapDist );
    fCrownFirePassiveSpreadRate   = m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadRate", &EqCalc::V6CrownFirePassiveSpreadRate );
	fCrownFirePowerOfFire         = m_eqTree->setEqFunAddress( "fCrownFirePowerOfFire", &EqCalc::V6CrownFirePowerOfFire );
    fCrownFirePowerOfWind         = m_eqTree->setEqFunAddress( "fCrownFirePowerOfWind", &EqCalc::V6CrownFirePowerOfWind );
    fCrownFirePowerRatio          = m_eqTree->setEqFunAddress( "fCrownFirePowerRatio", &EqCalc::V6CrownFirePowerRatio );
    fCrownFireTransRatioFromFireIntAtVector = m_eqTree->setEqFunAddress( "fCrownFireTransRatioFromFireIntAtVector", &EqCalc::V6CrownFireTransRatioFromFireIntAtVector );
    fCrownFireTransRatioFromFlameLengAtVector = m_eqTree->setEqFunAddress( "fCrownFireTransRatioFromFlameLengAtVector", &EqCalc::V6CrownFireTransRatioFromFlameLengAtVector );
    fCrownFireTransToCrown        = m_eqTree->setEqFunAddress( "fCrownFireTransToCrown", &EqCalc::V6CrownFireTransToCrown );
    fCrownFireType                = m_eqTree->setEqFunAddress( "fCrownFireType", &EqCalc::V6CrownFireType );
    fCrownFireWindDriven          = m_eqTree->setEqFunAddress( "fCrownFireWindDriven", &EqCalc::V6CrownFireWindDriven );

    fIgnitionFirebrandFuelMoisFromDead1Hr   = m_eqTree->setEqFunAddress( "fIgnitionFirebrandFuelMoisFromDead1Hr", &EqCalc::IgnitionFirebrandFuelMoisFromDead1Hr );
    fIgnitionFirebrandProb                  = m_eqTree->setEqFunAddress( "fIgnitionFirebrandProb", &EqCalc::IgnitionFirebrandProb );
    fIgnitionLightningFuelMoisFromDead100Hr = m_eqTree->setEqFunAddress( "fIgnitionLightningFuelMoisFromDead100Hr", &EqCalc::IgnitionLightningFuelMoisFromDead100Hr );
    fIgnitionLightningProb                  = m_eqTree->setEqFunAddress( "fIgnitionLightningProb", &EqCalc::IgnitionLightningProb );

    fMapScale = m_eqTree->setEqFunAddress( "fMapScale", &EqCalc::MapScale );
    fMapSlope = m_eqTree->setEqFunAddress( "fMapSlope",  &EqCalc::MapSlope );

    fSafetyZoneRadius  = m_eqTree->setEqFunAddress( "fSafetyZoneRadius",  &EqCalc::SafetyZoneRadius );
    fSafetyZoneSepDist = m_eqTree->setEqFunAddress( "fSafetyZoneSepDist",  &EqCalc::SafetyZoneSepDist );

    fSiteAspectDirFromNorth  = m_eqTree->setEqFunAddress( "fSiteAspectDirFromNorth",  &EqCalc::SiteAspectDirFromNorth );
    fSiteRidgeToValleyDist   = m_eqTree->setEqFunAddress( "fSiteRidgeToValleyDist",  &EqCalc::SiteRidgeToValleyDist );
    fSiteSlopeFraction       = m_eqTree->setEqFunAddress( "fSiteSlopeFraction", &EqCalc::SiteSlopeFraction );
    fSiteUpslopeDirFromNorth = m_eqTree->setEqFunAddress( "fSiteUpslopeDirFromNorth", &EqCalc::SiteUpslopeDirFromNorth );

    fSpotDistActiveCrown      = m_eqTree->setEqFunAddress( "fSpotDistActiveCrown", &EqCalc::SpotDistActiveCrown );
    fSpotDistBurningPile      = m_eqTree->setEqFunAddress( "fSpotDistBurningPile", &EqCalc::SpotDistBurningPile );
    fSpotDistSurfaceFire      = m_eqTree->setEqFunAddress( "fSpotDistSurfaceFire", &EqCalc::SpotDistSurfaceFire );
    fSpotDistTorchingTrees    = m_eqTree->setEqFunAddress( "fSpotDistTorchingTrees", &EqCalc::SpotDistTorchingTrees );
    fSpotMapDistActiveCrown   = m_eqTree->setEqFunAddress( "fSpotMapDistActiveCrown", &EqCalc::SpotMapDistActiveCrown );
    fSpotMapDistBurningPile   = m_eqTree->setEqFunAddress( "fSpotMapDistBurningPile", &EqCalc::SpotMapDistBurningPile );
    fSpotMapDistSurfaceFire   = m_eqTree->setEqFunAddress( "fSpotMapDistSurfaceFire", &EqCalc::SpotMapDistSurfaceFire );
    fSpotMapDistTorchingTrees = m_eqTree->setEqFunAddress( "fSpotMapDistTorchingTrees", &EqCalc::SpotMapDistTorchingTrees );

    fSurfaceFireArea              = m_eqTree->setEqFunAddress( "fSurfaceFireArea", &EqCalc::FireArea );
    fSurfaceFireCharacteristicsDiagram = m_eqTree->setEqFunAddress( "fSurfaceFireCharacteristicsDiagram", &EqCalc::FireCharacteristicsDiagram );
    fSurfaceFireDistAtBack        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtBack", &EqCalc::FireDistAtBack );
    fSurfaceFireDistAtBeta        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtBeta", &EqCalc::FireDistAtBeta );
    fSurfaceFireDistAtFlank       = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtFlank", &EqCalc::FireDistAtFlank );
    fSurfaceFireDistAtHead        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtHead", &EqCalc::FireDistAtHead );
    fSurfaceFireDistAtPsi         = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtPsi", &EqCalc::FireDistAtPsi );
    fSurfaceFireDistAtVector      = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtVector", &EqCalc::FireDistAtVector );
    fSurfaceFireEccentricity      = m_eqTree->setEqFunAddress( "fSurfaceFireEccentricity", &EqCalc::FireEccentricity );
    fSurfaceFireEllipseF          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseF", &EqCalc::FireEllipseF );
    fSurfaceFireEllipseG          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseG", &EqCalc::FireEllipseG );
    fSurfaceFireEllipseH          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseH", &EqCalc::FireEllipseH );
    fSurfaceFireEffWindAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireEffWindAtVector", &EqCalc::FireEffWindAtVector );
    fSurfaceFireFlameHtAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameHtAtVector", &EqCalc::FireFlameHtAtVector );
    fSurfaceFireFlameLengAtBeta   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtBeta", &EqCalc::FireFlameLengAtBeta );
    fSurfaceFireFlameLengAtHead   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtHead", &EqCalc::FireFlameLengAtHead );
    fSurfaceFireFlameLengAtPsi    = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtPsi", &EqCalc::FireFlameLengAtPsi );
    fSurfaceFireFlameLengAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtVector", &EqCalc::FireFlameLengAtVector );
    fSurfaceFireHeatPerUnitArea   = m_eqTree->setEqFunAddress( "fSurfaceFireHeatPerUnitArea", &EqCalc::FireHeatPerUnitArea );
    fSurfaceFireHeatSource        = m_eqTree->setEqFunAddress( "fSurfaceFireHeatSource", &EqCalc::FireHeatSource );
    fSurfaceFireLengDist          = m_eqTree->setEqFunAddress( "fSurfaceFireLengDist", &EqCalc::FireLengDist );
    fSurfaceFireLengMapDist       = m_eqTree->setEqFunAddress( "fSurfaceFireLengMapDist", &EqCalc::FireLengMapDist );
    fSurfaceFireLengthToWidth     = m_eqTree->setEqFunAddress( "fSurfaceFireLengthToWidth", &EqCalc::FireLengthToWidth );
    fSurfaceFireLineIntAtBeta     = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtBeta", &EqCalc::FireLineIntAtBeta );
    fSurfaceFireLineIntAtHead     = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtHead", &EqCalc::FireLineIntAtHead );
    fSurfaceFireLineIntAtPsi      = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtPsi", &EqCalc::FireLineIntAtPsi );
    fSurfaceFireLineIntAtVectorFromBeta = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtVectorFromBeta", &EqCalc::FireLineIntAtVectorFromBeta );
    fSurfaceFireLineIntAtVectorFromPsi  = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtVectorFromPsi", &EqCalc::FireLineIntAtVectorFromPsi );
    fSurfaceFireMapDistAtBack     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtBack", &EqCalc::FireMapDistAtBack );
    fSurfaceFireMapDistAtBeta     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtBeta", &EqCalc::FireMapDistAtBeta );
    fSurfaceFireMapDistAtFlank    = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtFlank", &EqCalc::FireMapDistAtFlank );
    fSurfaceFireMapDistAtHead     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtHead", &EqCalc::FireMapDistAtHead );
    fSurfaceFireMapDistAtPsi      = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtPsi", &EqCalc::FireMapDistAtPsi );
    fSurfaceFireMapDistAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtVector", &EqCalc::FireMapDistAtVector );
    fSurfaceFireMaxDirFromNorth   = m_eqTree->setEqFunAddress( "fSurfaceFireMaxDirFromNorth", &EqCalc::FireMaxDirFromNorth );
    fSurfaceFireMaxDirDiagram     = m_eqTree->setEqFunAddress( "fSurfaceFireMaxDirDiagram", &EqCalc::FireMaxDirDiagram );
    fSurfaceFireNoWindRate        = m_eqTree->setEqFunAddress( "fSurfaceFireNoWindRate", &EqCalc::FireNoWindRate );
    fSurfaceFirePerimeter         = m_eqTree->setEqFunAddress( "fSurfaceFirePerimeter", &EqCalc::FirePerimeter );
    fSurfaceFirePropagatingFlux   = m_eqTree->setEqFunAddress( "fSurfaceFirePropagatingFlux", &EqCalc::FirePropagatingFlux );
    fSurfaceFireReactionInt       = m_eqTree->setEqFunAddress( "fSurfaceFireReactionInt", &EqCalc::FireReactionInt );
    fSurfaceFireResidenceTime     = m_eqTree->setEqFunAddress( "fSurfaceFireResidenceTime", &EqCalc::FireResidenceTime );
    fSurfaceFireScorchHtFromFliAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireScorchHtFromFliAtVector", &EqCalc::FireScorchHtFromFliAtVector );
    fSurfaceFireScorchHtFromFlameLengAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireScorchHtFromFlameLengAtVector", &EqCalc::FireScorchHtFromFlameLengAtVector );
    fSurfaceFireShapeDiagram      = m_eqTree->setEqFunAddress( "fSurfaceFireShapeDiagram", &EqCalc::FireShapeDiagram );
    fSurfaceFireSpreadAtBack      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtBack", &EqCalc::FireSpreadAtBack );
    fSurfaceFireSpreadAtBeta      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtBeta", &EqCalc::FireSpreadAtBeta );
    fSurfaceFireSpreadAtFlank     = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtFlank", &EqCalc::FireSpreadAtFlank );
    fSurfaceFireSpreadAtHead      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtHead", &EqCalc::FireSpreadAtHead );
    fSurfaceFireSpreadAtPsi       = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtPsi", &EqCalc::FireSpreadAtPsi );
    fSurfaceFireSpreadAtVectorFromBeta	= m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtVectorFromBeta", &EqCalc::FireSpreadAtVectorFromBeta );
    fSurfaceFireSpreadAtVectorFromPsi	= m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtVectorFromPsi", &EqCalc::FireSpreadAtVectorFromPsi );
    fSurfaceFireVectorBetaFromTheta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorBetaFromTheta", &EqCalc::FireVectorBetaFromTheta );
    fSurfaceFireVectorBetaFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorBetaFromUpslope", &EqCalc::FireVectorBetaFromUpslope );
    fSurfaceFireVectorDirFromNorth		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorDirFromNorth", &EqCalc::FireVectorDirFromNorth );
    fSurfaceFireVectorDirFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorDirFromUpslope", &EqCalc::FireVectorDirFromUpslope );
    fSurfaceFireVectorPsiFromTheta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorPsiFromTheta", &EqCalc::FireVectorPsiFromTheta );
    fSurfaceFireVectorPsiFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorPsiFromUpslope", &EqCalc::FireVectorPsiFromUpslope );
    fSurfaceFireVectorThetaFromBeta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorThetaFromBeta", &EqCalc::FireVectorThetaFromBeta );
    fSurfaceFireVectorThetaFromPsi		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorThetaFromPsi", &EqCalc::FireVectorThetaFromPsi );
    fSurfaceFireWidthDist		= m_eqTree->setEqFunAddress( "fSurfaceFireWidthDist", &EqCalc::FireWidthDist );
    fSurfaceFireWidthMapDist	= m_eqTree->setEqFunAddress( "fSurfaceFireWidthMapDist", &EqCalc::FireWidthMapDist );

    fSurfaceFuelAspenModel = m_eqTree->setEqFunAddress( "fSurfaceFuelAspenModel", &EqCalc::FuelAspenModel );
    fSurfaceFuelAspenParms = m_eqTree->setEqFunAddress( "fSurfaceFuelAspenParms", &EqCalc::FuelAspenParms );

	fSurfaceFuelBedHeatSink		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedHeatSink", &EqCalc::FuelBedHeatSink );
    fSurfaceFuelBedIntermediates = m_eqTree->setEqFunAddress( "fSurfaceFuelBedIntermediates", &EqCalc::FuelBedIntermediates );
    fSurfaceFuelBedModel		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedModel", &EqCalc::FuelBedModel );
    fSurfaceFuelBedParms		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedParms", &EqCalc::FuelBedParms );
    fSurfaceFuelBedWeighted		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedWeighted", &EqCalc::FuelBedWeighted );

	fSurfaceFuelChaparralAgeFromDepthType   = m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralAgeFromDepthType", &EqCalc::FuelChaparralAgeFromDepthType );
    fSurfaceFuelChaparralLoadTotalFromAgeType = m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralLoadTotalFromAgeType", &EqCalc::FuelChaparralLoadTotalFromAgeType );
	fSurfaceFuelChaparralModel				= m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralModel", &EqCalc::FuelChaparralModel );    
    fSurfaceFuelChaparralParms				= m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralParms", &EqCalc::FuelChaparralParms );

	fSurfaceFuelLoadTransferFraction= m_eqTree->setEqFunAddress( "fSurfaceFuelLoadTransferFraction", &EqCalc::FuelLoadTransferFraction );
    fSurfaceFuelMoisDeadHerbWood	= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisDeadHerbWood", &EqCalc::FuelMoisDeadHerbWood );
    fSurfaceFuelMoisLifeClass		= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisLifeClass", &EqCalc::FuelMoisLifeClass );
    fSurfaceFuelMoisScenarioModel	= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisScenarioModel", &EqCalc::FuelMoisScenarioModel );
    fSurfaceFuelMoisTimeLag			= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisTimeLag", &EqCalc::FuelMoisTimeLag );
    fSurfaceFuelPalmettoModel		= m_eqTree->setEqFunAddress( "fSurfaceFuelPalmettoModel", &EqCalc::FuelPalmettoModel );
    fSurfaceFuelPalmettoParms		= m_eqTree->setEqFunAddress( "fSurfaceFuelPalmettoParms", &EqCalc::FuelPalmettoParms );
    fSurfaceFuelTemp				= m_eqTree->setEqFunAddress( "fSurfaceFuelTemp", &EqCalc::FuelTemp );

    fTimeJulianDate = m_eqTree->setEqFunAddress( "fTimeJulianDate", &EqCalc::TimeJulianDate );

    fTreeBarkThicknessFofem = m_eqTree->setEqFunAddress( "fTreeBarkThicknessFofem", &EqCalc::TreeBarkThicknessFofem );
    fTreeBarkThicknessFofem6 = m_eqTree->setEqFunAddress( "fTreeBarkThicknessFofem6", &EqCalc::TreeBarkThicknessFofem6 );
    fTreeCrownBaseHt = m_eqTree->setEqFunAddress( "fTreeCrownBaseHt", &EqCalc::TreeCrownBaseHt );
    fTreeCrownRatio  = m_eqTree->setEqFunAddress( "fTreeCrownRatio", &EqCalc::TreeCrownRatio );
    fTreeCrownVolScorchedAtVector = m_eqTree->setEqFunAddress( "fTreeCrownVolScorchedAtVector", &EqCalc::TreeCrownVolScorchedAtVector );
    fTreeMortalityCountAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityCountAtVector", &EqCalc::TreeMortalityCountAtVector );
    fTreeMortalityRateAspenAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateAspenAtVector", &EqCalc::TreeMortalityRateAspenAtVector );
    fTreeMortalityRateFofemAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofemAtVector", &EqCalc::TreeMortalityRateFofemAtVector );
    fTreeMortalityRateFofem2AtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofem2AtVector", &EqCalc::TreeMortalityRateFofem2AtVector );
    fTreeMortalityRateFofem6AtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofem6AtVector", &EqCalc::TreeMortalityRateFofem6AtVector );
    fTreeMortalityRateFofemHoodAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofemHoodAtVector", &EqCalc::TreeMortalityRateFofemHoodAtVector );

    fWindAdjFactor = m_eqTree->setEqFunAddress( "fWindAdjFactor", &EqCalc::WindAdjFactor );
    fWindSpeedAt20Ft = m_eqTree->setEqFunAddress( "fWindSpeedAt20Ft", &EqCalc::WindSpeedAt20Ft );
    fWindSpeedAtMidflame = m_eqTree->setEqFunAddress( "fWindSpeedAtMidflame", &EqCalc::WindSpeedAtMidflame );
    fWindDirFromNorth = m_eqTree->setEqFunAddress( "fWindDirFromNorth", &EqCalc::WindDirFromNorth );
    fWindDirFromUpslope = m_eqTree->setEqFunAddress( "fWindDirFromUpslope", &EqCalc::WindDirFromUpslope );

    fWthrCumulusBaseHt = m_eqTree->setEqFunAddress( "fWthrCumulusBaseHt", &EqCalc::WthrCumulusBaseHt );
    fWthrDewPointTemp = m_eqTree->setEqFunAddress( "fWthrDewPointTemp", &EqCalc::WthrDewPointTemp );
    fWthrHeatIndex = m_eqTree->setEqFunAddress( "fWthrHeatIndex", &EqCalc::WthrHeatIndex );
    fWthrRelativeHumidity = m_eqTree->setEqFunAddress( "fWthrRelativeHumidity", &EqCalc::WthrRelativeHumidity );
    fWthrSummerSimmerIndex = m_eqTree->setEqFunAddress( "fWthrSummerSimmerIndex", &EqCalc::WthrSummerSimmerIndex );
    fWthrWindChillTemp = m_eqTree->setEqFunAddress( "fWthrWindChillTemp", &EqCalc::WthrWindChillTemp );
    return;
}

bool EqCalc::closeEnough( const char* what, double v5, double v6, double delta )
{
	if ( fabs( v5-v6 ) > delta )
	{
		printf( "\n\n*** %s v5=%g,  v6=%g\n", what, v5, v6 );
		return false;
	}
	return true;
}

void EqCalc::logMethod( char* methodName, int numInputs, int numOutputs )
{
    if( m_log )
    {
        fprintf( m_log, "        begin proc %s() %d %d\n",
			methodName, numInputs, numOutputs );
	}
}

void EqCalc::logInput( EqVar *var )
{
    if( m_log )
    {
        fprintf( m_log, "          i %s %g %s\n",
			var->m_name,
			var->m_nativeValue,
			var->m_nativeUnits.latin1() );
	}
}

void EqCalc::logOutput( EqVar *var )
{
    if( m_log )
    {
        fprintf( m_log, "          o %s %g %s\n",
			var->m_name,
			var->m_nativeValue,
			var->m_nativeUnits.latin1() );
	}
}

void EqCalc::logOutputItem( EqVar *var )
{
    if( m_log )
    {
        fprintf( m_log, "          o %s %d %s\n",
			var->m_name,
			var->m_nativeValue,
			var->m_nativeUnits.latin1() );
	}
}

double EqCalc::fetch( EqVar *var )
{
	double value = var->m_nativeValue;
	logInput( var );
	return value;
}


void EqCalc::store( EqVar *var, double value )
{
	var->update( value );
	logOutput( var );
}

//------------------------------------------------------------------------------
//  End of xeqcalcV6Main.cpp
//------------------------------------------------------------------------------
