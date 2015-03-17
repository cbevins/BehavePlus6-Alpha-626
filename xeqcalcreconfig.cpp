//------------------------------------------------------------------------------
/*! \file xeqcalcreconfig.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Reconfigures the EqFun m_active and the EqVar m_isUserOutput flags
 *  to reflect the current configuration options, outputs, & properties.
 *
 *  This is one of the few EqCalc files that hold implementation-specific
 *  variable and function pointers.  The files are xeqcalc.cpp,
 *  xeqcalcmask.cpp, and xeqcalcreconfig.cpp.
 */

// Custom include files
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

//------------------------------------------------------------------------------
/*! \brief Returns the run subtitle, which indicates the current SURFACE
 *  Directions tab option.
 *
 *	Returns one of:
 *	- Head Fire
 *	- Flanking Fire
 *  - Backing Fire
 *  - Spread from Fire Front
 *	- Spread from Ignition Point
 *	- Spread from Ignition Point, V5 Directional FL Caclulation
 */

QString EqCalc::getSubtitle( void ) const
{
	PropertyDict *prop = m_eqTree->m_propDict;
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
		if ( prop->boolean( "surfaceConfSpreadDirHead" ) )
			return QString( "Head Fire" );
		else if ( prop->boolean( "surfaceConfSpreadDirBack" ) )
			return QString( "Backing Fire" );
		else if ( prop->boolean( "surfaceConfSpreadDirFlank" ) )
			return QString( "Flanking Fire" );
		else if ( prop->boolean( "surfaceConfSpreadDirPointSourceBeta" ) )
			return QString( "Spread from Ignition Point, V5 directional FL calculation" );
		else if ( prop->boolean( "surfaceConfSpreadDirPointSourcePsi" ) )
			return QString( "Spread from Ignition Point" );
		else if ( prop->boolean( "surfaceConfSpreadDirFireFront" ) )
			return QString( "Spread from Fire Front" );
	}
	return QString( "" );
}

//------------------------------------------------------------------------------
/*! \brief Dynamically reconfigures the each EqTree module
 *  based upon the current option and output configuration variables.
 *
 *  \param release Application's release number.
 *
 *  This should called whenever any of the *Option* or *Output* variables
 *  have been changed, thereby changing the EqTree configuration state.
 */

void EqCalc::reconfigure( int release )
{
    reconfigureDocumentation( m_eqTree->m_propDict, release );
    reconfigureSurfaceModule( m_eqTree->m_propDict, release );
    reconfigureCrownModule( m_eqTree->m_propDict, release );
    reconfigureSizeModule( m_eqTree->m_propDict, release );
    reconfigureContainModule( m_eqTree->m_propDict, release );
    reconfigureSpotModule( m_eqTree->m_propDict, release );
    reconfigureScorchModule( m_eqTree->m_propDict, release );
    reconfigureMortalityModule( m_eqTree->m_propDict, release );
    reconfigureIgnitionModule( m_eqTree->m_propDict, release );
    reconfigureWeatherModule( m_eqTree->m_propDict, release );
    reconfigureSafetyModule( m_eqTree->m_propDict, release );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Contain Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureContainModule( PropertyDict *prop, int /* release */ )
{
    // If the Contain Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "containModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fContainFF->m_active = prop->boolean( "containConfResourcesMultiple" );
    fContainFFSingle->m_active = prop->boolean( "containConfResourcesSingle" );

    // Input variables
    if ( prop->boolean( "containConfResourcesSingle" ) )
    {
        vContainResourceName->m_isConstant = true;
    }
    else
    {
        vContainResourceName->m_isConstant = false;
    }

    // Contain spread distance limit
    if ( prop->boolean( "containConfLimitDistOn" ) )
    {
        vContainLimitDist->m_isConstant = false;
        vContainLimitDist->m_isUserInput = true;
    }
    else
    {
        vContainLimitDist->m_isConstant = true;
        vContainLimitDist->m_isUserInput = false;
    }

    // Output variables
    vContainLine->m_isUserOutput =
        prop->boolean( "containCalcLine" );
    vContainResourcesUsed->m_isUserOutput =
        prop->boolean( "containCalcResourcesUsed" );
    vContainSize->m_isUserOutput =
        prop->boolean( "containCalcSize" );
    vContainStatus->m_isUserOutput =
        prop->boolean( "containCalcStatus" );
    vContainTime->m_isUserOutput =
        prop->boolean( "containCalcTime" );
    vContainCost->m_isUserOutput =
        prop->boolean( "containCalcCost" ) ;
    vContainDiagram->m_isUserOutput =
        prop->boolean( "containCalcDiagram" );
    vContainAttackPerimeter->m_isUserOutput =
        prop->boolean( "containCalcAttackPerimeter" );
    vContainAttackSize->m_isUserOutput =
        prop->boolean( "containCalcAttackSize" );

    // If cost is an output, then we need rates for input
    vContainResourceBaseCost->m_isConstant = false;
    vContainResourceHourCost->m_isConstant = false;
    if ( ! vContainCost->m_isUserOutput )
    {
        vContainResourceBaseCost->m_isConstant = true;
        vContainResourceHourCost->m_isConstant = true;
    }

    // If linked to the Surface Module ...
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        // Use the Surface fire spread rate as the report spread rate
        fContainFFReportSpread->m_active = true;
        // Use the Surface fire length-to-width ratio as the report l:w
        fContainFFReportRatio->m_active = true;
    }
    // If linked to the Size module ...
    if ( prop->boolean( "sizeModuleActive" ) )
    {
		// Use the Surface fire spread rate as the report spread rate
		fContainFFReportSpread->m_active = true;
		// Use the Surface fire length-to-width ratio as the report l:w
		fContainFFReportRatio->m_active = true;
        // Use the Size fire area as the Contain report size
        fContainFFReportSize->m_active = true;
    }
	// ALWAYS use the Surface fire spread rate at head as the report spread rate
	fContainFFReportSpread->m_active = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Crown Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureCrownModule( PropertyDict *prop, int /* release */ )
{
    // If the Crown Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "crownModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fCrownFireActiveCrown->m_active = true;
    fCrownFireActiveRatio->m_active = true;
    fCrownFireArea->m_active = true;
    fCrownFireCritCrownSpreadRate->m_active = true;
    fCrownFireCritSurfFireInt->m_active = true;
    fCrownFireCritSurfFlameLeng->m_active = true;
    fCrownFireLengthToWidth->m_active = true;
    fCrownFirePerimeter->m_active = true;
    fCrownFireSpreadDist->m_active = true;
    fCrownFireSpreadMapDist->m_active = true;
    fCrownFireSpreadRate->m_active = true;
    fCrownFireTransToCrown->m_active = true;
    fCrownFireTransRatioFromFireIntAtVector->m_active = false;
    fCrownFireTransRatioFromFireIntAtVector->m_active = false;
    fCrownFireType->m_active = true;

    fCrownFireFlameLeng->m_active = true;
    fCrownFireFuelLoad->m_active = true;
    fCrownFireHeatPerUnitArea->m_active = true;
    fCrownFireHeatPerUnitAreaCanopy->m_active = true;
    fCrownFireLineInt->m_active = true;
    fCrownFirePowerOfFire->m_active = true;
    fCrownFirePowerOfWind->m_active = true;
    fCrownFirePowerRatio->m_active = true;
    fCrownFireWindDriven->m_active = true;

	// Build 611 uses SURFACE Module fuel moisture settings
    fSurfaceFuelMoisTimeLag->m_active = true;

    // If linked to the Surface Module...
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        // If user wants behavior at a specified fire vector:
        if ( prop->boolean( "surfaceConfSpreadDirInput" ) )
        {
            // Output scorch height at VECTOR using default FLI input
            fCrownFireTransRatioFromFireIntAtVector->m_active = true;
        }
        // else user wants behavior at the fire head
        else //if ( surfaceConfSpreadDirMax )
        {
            // Output scorch height at HEAD using default FLI input
            fCrownFireTransRatioFromFireIntAtVector->m_active = true;
        }
    }
    // ... else if not linked to the Surface Module...
    else // if ( ! surfaceModuleActive )
    {
        // Option 1: Surface fire intensity is entered as
        // Choice 1: flame length.
        if ( prop->boolean( "crownConfUseFlameLeng" ) )
        {
            fCrownFireTransRatioFromFlameLengAtVector->m_active = true;
        }
        // Choice 2: fireline intensity.
        else // if ( prop->boolean( "crownConfUseFireLineInt" ) )
        {
            fCrownFireTransRatioFromFireIntAtVector->m_active = true;
        }
        // If Surface Module is configured to use 10-m wind ...
        if ( prop->boolean( "surfaceConfWindSpeedAt10M" )
          || prop->boolean( "surfaceConfWindSpeedAt10MCalc" ) )
        {
            // Must derive wind speed at 20-ft from wind speed at 10-m
            fWindSpeedAt20Ft->m_active = true;
        }
		//--------------------------------------------------------------------------
		// Build 611 - Use SURFACE Module settings for fuel moisture input
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		// Choice 3.1: Moisture is entered by individual size class.
		//--------------------------------------------------------------------------
		if ( prop->boolean( "surfaceConfMoisTimeLag" ) )
		{
			// Already set up for this, but what the heck...
			fSurfaceFuelMoisTimeLag->m_active = true;
		}
		//--------------------------------------------------------------------------
		// Choice 3.2: Moisture is entered by dead and live category.
		//--------------------------------------------------------------------------
		else if ( prop->boolean( "surfaceConfMoisLifeCat" ) )
		{
			// Must derive time lag fuel moisture from life category moistures
			fSurfaceFuelMoisLifeClass->m_active = true;
		}
		//--------------------------------------------------------------------------
		// Choice 3.3: Moisture is entered by dead, herb, and wood categories.
		//--------------------------------------------------------------------------
		else if ( prop->boolean( "surfaceConfMoisDeadHerbWood" ) )
		{
			// Must derive time lag fuel moisture from dead category and herb and woody classes
			fSurfaceFuelMoisDeadHerbWood->m_active = true;
		}
		//--------------------------------------------------------------------------
		// Choice 3.4: Moisture is entered by moisture scenario.
		//--------------------------------------------------------------------------
		else if ( prop->boolean( "surfaceConfMoisScenario" ) )
		{
			// Must derive time lag fuel moisture from moistures scenario
			fSurfaceFuelMoisScenarioModel->m_active = true;
		}
    }

    // Output variables
    vCrownFireActiveCrown->m_isUserOutput =
        prop->boolean( "crownCalcActiveCrown" );
    vCrownFireActiveRatio->m_isUserOutput =
        prop->boolean( "crownCalcActiveRatio" );
    vCrownFireArea->m_isUserOutput =
        prop->boolean( "crownCalcFireArea" );
    vCrownFireCritCrownSpreadRate->m_isUserOutput =
        prop->boolean( "crownCalcCriticalCrownSpreadRate" );
    vCrownFireCritSurfFireInt->m_isUserOutput =
        prop->boolean( "crownCalcCriticalSurfaceIntensity" );
    vCrownFireCritSurfFlameLeng->m_isUserOutput =
        prop->boolean( "crownCalcCriticalSurfaceFlameLeng" );
    vCrownFireFlameLeng->m_isUserOutput =
        prop->boolean( "crownCalcFlameLeng" );
    vCrownFireFuelLoad->m_isUserOutput =
        prop->boolean( "crownCalcFuelLoad" );
    vCrownFireHeatPerUnitArea->m_isUserOutput =
        prop->boolean( "crownCalcHeatPerUnitArea" );
    vCrownFireHeatPerUnitAreaCanopy->m_isUserOutput =
        prop->boolean( "crownCalcHeatPerUnitAreaCanopy" );
    vCrownFireLengthToWidth->m_isUserOutput =
        prop->boolean( "crownCalcFireLengthToWidth" );
    vCrownFireLineInt->m_isUserOutput =
        prop->boolean( "crownCalcFireLineInt" );
	vCrownFirePerimeter->m_isUserOutput =
        prop->boolean( "crownCalcFirePerimeter" );
    vCrownFirePowerOfFire->m_isUserOutput =
        prop->boolean( "crownCalcPowerOfFire" );
    vCrownFirePowerOfWind->m_isUserOutput =
        prop->boolean( "crownCalcPowerOfWind" );
    vCrownFirePowerRatio->m_isUserOutput =
        prop->boolean( "crownCalcPowerRatio" );
    vCrownFireSpreadDist->m_isUserOutput =
        prop->boolean( "crownCalcCrownSpreadDist" );
    vCrownFireSpreadRate->m_isUserOutput =
        prop->boolean( "crownCalcCrownSpreadRate" );
    vCrownFireTransRatio->m_isUserOutput =
        prop->boolean( "crownCalcTransitionRatio" );
    vCrownFireTransToCrown->m_isUserOutput =
        prop->boolean( "crownCalcTransitionToCrown" );
    vCrownFireType->m_isUserOutput =
        prop->boolean( "crownCalcCrownFireType" );
    vCrownFireWindDriven->m_isUserOutput =
        prop->boolean( "crownCalcWindDriven" );
    if ( prop->boolean( "mapCalcDist" ) )
    {
        fMapScale->m_active = true;
        vCrownFireSpreadMapDist->m_isUserOutput =
            prop->boolean( "crownCalcCrownSpreadDist" );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree documentation variables
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureDocumentation( PropertyDict *prop, int /* release */ )
{
    if ( prop->boolean( "docDescriptionActive" ) )
    {
        vDocDescription->m_isUserInput = true;
        vDocDescription->m_isUserOutput = true;
    }
    if ( prop->boolean( "docRxActive" ) )
    {
        vDocRxAdminUnit->m_isUserInput = true;
        vDocRxAdminUnit->m_isUserOutput = true;
        vDocRxName->m_isUserInput = true;
        vDocRxName->m_isUserOutput = true;
        vDocRxPreparedBy->m_isUserInput = true;
        vDocRxPreparedBy->m_isUserOutput = true;
    }
    if ( prop->boolean( "docFireActive" ) )
    {
        vDocFireAnalyst->m_isUserInput = true;
        vDocFireAnalyst->m_isUserOutput = true;
        vDocFireName->m_isUserInput = true;
        vDocFireName->m_isUserOutput = true;
        vDocFirePeriod->m_isUserInput = true;
        vDocFirePeriod->m_isUserOutput = true;
    }
    if ( prop->boolean( "docTrainingActive" ) )
    {
        vDocTrainingCourse->m_isUserInput = true;
        vDocTrainingCourse->m_isUserOutput = true;
        vDocTrainingExercise->m_isUserInput = true;
        vDocTrainingExercise->m_isUserOutput = true;
        vDocTrainingTrainee->m_isUserInput = true;
        vDocTrainingTrainee->m_isUserOutput = true;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Ignition Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureIgnitionModule( PropertyDict *prop, int /* release */ )
{
    // If the Ignition Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "ignitionModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fIgnitionFirebrandProb->m_active = true;
    fIgnitionLightningProb->m_active = true;
    fSurfaceFuelTemp->m_active = true;

    // Output variables
    vIgnitionFirebrandProb->m_isUserOutput =
        prop->boolean( "ignitionCalcIgnitionFirebrandProb" );
    vIgnitionLightningProb->m_isUserOutput =
        prop->boolean( "ignitionCalcIgnitionLightningProb" );
    vSurfaceFuelTemp->m_isUserOutput =
        prop->boolean( "ignitionCalcFuelTemp");

    // If linked to the Surface Module...
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        // Use the 1-h and 100-h surface fuel moistures
        fIgnitionFirebrandFuelMoisFromDead1Hr->m_active = true;
        fIgnitionLightningFuelMoisFromDead100Hr->m_active = true;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Mortality Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureMortalityModule( PropertyDict *prop, int /* release */ )
{
    // If the Mortality Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "mortalityModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fTreeCrownBaseHt->m_active = false;		// CDB - prevent circular dependency with fTreeCrownRatio -> vTreeCrownRatio
    fTreeCrownVolScorchedAtVector->m_active = true;
    fTreeMortalityCountAtVector->m_active = true;
    fTreeMortalityRateFofemAtVector->m_active = false;      // deprecated
    fTreeMortalityRateFofem2AtVector->m_active = false;     // deprecated
    fTreeMortalityRateFofemHoodAtVector->m_active = false;	// deprecated
    fTreeMortalityRateFofem6AtVector->m_active = true;

	bool a = prop->boolean( "crownModuleActive" );
	//bool b = vTreeCoverHt->m_isUserInput;
	if ( a )
    {
        fTreeCrownRatio->m_active = true;
    }

    // Note: Version > 4.0.0 do not allow bark input; it's always calculated
    // Option 1: Bark thickness is
    // Choice 2: estimated from species and d.b.h.
    //if ( prop->boolean( "mortalityConfBarkDerived" ) )
    //{
        // Must derive bark thickness from tree species and d.b.h.
        fTreeMortalityRateFofem6AtVector->m_active = true;
        fTreeBarkThicknessFofem6->m_active = true;
        // User may request bark thickness as an output variable
        vTreeBarkThickness->m_isUserOutput =
            prop->boolean( "mortalityCalcBarkThickness" );
    //}
    // Choice 1: specified on the worksheet.
    //else
    //{
    //    fTreeMortalityRateFofem6AtVector->m_active = true;
    //    fTreeBarkThicknessFofem6->m_active = false;
    //}

    // Use fire behavior at the specified VECTOR
    vTreeCrownLengScorchedAtVector->m_isUserOutput =
        prop->boolean( "mortalityCalcTreeCrownLengScorched" );
    vTreeCrownVolScorchedAtVector->m_isUserOutput =
        prop->boolean( "mortalityCalcTreeCrownVolScorched" );
    vTreeMortalityCountAtVector->m_isUserOutput =
        prop->boolean( "mortalityCalcTreeMortalityCount" );
    vTreeMortalityRateAtVector->m_isUserOutput =
        prop->boolean( "mortalityCalcTreeMortalityRate" );
    // If linked to the Surface Module but NOT to the Scorch Module ...
    if ( prop->boolean( "surfaceModuleActive" )
      && ! prop->boolean( "scorchModuleActive" ) )
    {
        fSurfaceFireScorchHtFromFliAtVector->m_active = true;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Safety Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureSafetyModule( PropertyDict *prop, int /* release */ )
{
    // If the Safety Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "safetyModuleActive" ) )
    {
        return;
    }

    // Activate functions required by the output variables
    if ( ( vSafetyZoneRadius->m_isUserOutput = prop->boolean( "safetyCalcRadius" ) ) )
    {
        fSafetyZoneSepDist->m_active = true;
        fSafetyZoneRadius->m_active = true;
    }

	if ( ( vSafetyZoneLength->m_isUserOutput = prop->boolean( "safetyCalcLength" ) ) )
    {
        fSafetyZoneSepDist->m_active = true;
        fSafetyZoneRadius->m_active = true;
    }

    if ( ( vSafetyZoneSepDist->m_isUserOutput = prop->boolean( "safetyCalcSepDist" ) ) )
    {
        fSafetyZoneSepDist->m_active = true;
    }

    if ( ( vSafetyZoneSize->m_isUserOutput = prop->boolean( "safetyCalcSize" ) ) )
    {
        fSafetyZoneSepDist->m_active = true;
        fSafetyZoneRadius->m_active = true;
    }

	if ( ( vSafetyZoneSizeSquare->m_isUserOutput = prop->boolean( "safetyCalcSizeSquare" ) ) )
    {
        fSafetyZoneSepDist->m_active = true;
        fSafetyZoneRadius->m_active = true;
    }

    // If linked to the Surface Module...
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        fSurfaceFireFlameLengAtHead->m_active = true;
        vSurfaceFireFlameLengAtHead->m_isUserInput = false;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Scorch Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureScorchModule( PropertyDict *prop, int /* release */ )
{
    // If the Scorch Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "scorchModuleActive" )
      || ! prop->boolean( "scorchCalcScorchHt" ) )
    {
        return;
    }

    // If linked to the Surface Module...
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        // Output scorch height at VECTOR using default FLI input
        fSurfaceFireScorchHtFromFliAtVector->m_active = true;
        vSurfaceFireScorchHtAtVector->m_isUserOutput = true;
    }
    // ... else if not linked to the Surface Module...
    else // if ( ! surfaceModuleActive )
    {
		fWindSpeedAt20Ft->m_active = false;
		fWindSpeedAtMidflame->m_active = false;
		fWindAdjFactor->m_active = false;
        vSurfaceFireScorchHtAtVector->m_isUserOutput = true;
        // Option 1: Fire intensity is entered as
        // Choice 1: flame length.
        if ( prop->boolean( "scorchConfUseFlameLeng" ) )
        {
            fSurfaceFireScorchHtFromFlameLengAtVector->m_active = true;
        }
        // Choice 2: fireline intensity.
        else // if ( prop->boolean( "scorchConfUseFireLineInt" ) )
        {
            fSurfaceFireScorchHtFromFliAtVector->m_active = true;
        }
		// Apply SURFACE wind rules even tho SURFACE is not active
		// Surface Choice 4.1: Wind speed is entered as midflame height
		if ( prop->boolean( "surfaceConfWindSpeedAtMidflame" ) )
		{
	        vWindAdjFactor->m_isConstant = true;
		    vWindAdjFactor->update( 1.0 );
			vWindAdjMethod->m_isConstant = true;
			vWindAdjMethod->updateItem( 2 );
		}
		// Surface Choice 4.2: Wind speed is entered as 20-ft wind and INPUT wind adj factor.
		else if ( prop->boolean( "surfaceConfWindSpeedAt20Ft" ) )
		{
			// Must derive wind speed at midflame height from wind speed at 20 ft
			fWindSpeedAtMidflame->m_active = true;
			// User wants to enter WAF
			fWindAdjFactor->m_active = false;
			// And user may request derived midflame wind speed as output
			vWindSpeedAtMidflame->m_isUserOutput = prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
	        vWindAdjMethod->m_isConstant = true;
		    vWindAdjMethod->updateItem( 2 );
		}
		// Surface Choice 4.3: Wind speed is entered as 20-ft wind and CALCULATED wind adj factor.
		else if ( prop->boolean( "surfaceConfWindSpeedAt20FtCalc" ) )
		{
			// Must derive wind speed at midflame height from wind speed at 20 ft
			fWindSpeedAtMidflame->m_active = true;
			// User wants WAF to be calculated (and maybe output)
			fWindAdjFactor->m_active = true;
			// And user may request derived midflame wind speed as output
			vWindSpeedAtMidflame->m_isUserOutput = prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
	        vWindAdjMethod->m_isConstant = false;
		    vWindAdjMethod->updateItem( 2 );
		}
		// Surface Choice 4.4: Wind speed is entered as 10-m wind and INPUT wind adj factor.
		else if ( prop->boolean( "surfaceConfWindSpeedAt10M" ) )
		{
			// Must derive wind speed at 20-ft from wind speed at 10-m
			fWindSpeedAt20Ft->m_active = true;
			// Must derive wind speed at midflame height from wind speed at 20-ft
			fWindSpeedAtMidflame->m_active = true;
			// User wants to enter WAF
			fWindAdjFactor->m_active = false;
	        // And user may request derived midflame wind speed as output
		    vWindSpeedAtMidflame->m_isUserOutput = prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
	        vWindAdjMethod->m_isConstant = true;
		    vWindAdjMethod->updateItem( 2 );
		}
		// Surface Choice 4.5: Wind speed is entered as 10-m wind and CALCULATED wind adj factor.
		else if ( prop->boolean( "surfaceConfWindSpeedAt10MCalc" ) )
		{
			// Must derive wind speed at 20-ft from wind speed at 10-m
			fWindSpeedAt20Ft->m_active = true;
			// Must derive wind speed at midflame height from wind speed at 20-ft
			fWindSpeedAtMidflame->m_active = true;
			// User wants WAF to be calculated (and maybe output)
			fWindAdjFactor->m_active = true;
			// And user is may request derived midflame wind speed as output
			vWindSpeedAtMidflame->m_isUserOutput = prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
	        vWindAdjMethod->m_isConstant = false;
		    vWindAdjMethod->updateItem( 2 );
		}
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Size Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureSizeModule( PropertyDict *prop, int /* release */ )
{
    // If the Size Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "sizeModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fSurfaceFireArea->m_active = true;
    fSurfaceFireDistAtBack->m_active = true;
    fSurfaceFireDistAtFlank->m_active = true;
    fSurfaceFireDistAtHead->m_active = true;
    fSurfaceFireEccentricity->m_active = true;
    fSurfaceFireLengDist->m_active = true;
    fSurfaceFireLengMapDist->m_active = true;
    fSurfaceFireLengthToWidth->m_active = true;
    fSurfaceFireMapDistAtBack->m_active = true;
    fSurfaceFireMapDistAtFlank->m_active = true;
    fSurfaceFireMapDistAtHead->m_active = true;
    fSurfaceFirePerimeter->m_active = true;
    fSurfaceFireShapeDiagram->m_active = true;
    fSurfaceFireSpreadAtBack->m_active = true;
    fSurfaceFireSpreadAtFlank->m_active = true;
    fSurfaceFireWidthDist->m_active = true;
    fSurfaceFireWidthMapDist->m_active = true;

    // Output variables
    vSurfaceFireArea->m_isUserOutput =
        prop->boolean( "sizeCalcFireArea" );
    vSurfaceFireDistAtBack->m_isUserOutput =
        prop->boolean( "sizeCalcFireDistAtBack" );
    vSurfaceFireDistAtFlank->m_isUserOutput =
        prop->boolean( "sizeCalcFireDistAtFlank" );
    vSurfaceFireDistAtHead->m_isUserOutput =
        prop->boolean( "sizeCalcFireDistAtFront" );
    vSurfaceFireLengthToWidth->m_isUserOutput =
        prop->boolean( "sizeCalcFireLengToWidth" );
    vSurfaceFireLengDist->m_isUserOutput =
        prop->boolean( "sizeCalcFireLengDist" );
    vSurfaceFirePerimeter->m_isUserOutput =
        prop->boolean( "sizeCalcFirePerimeter" );
    vSurfaceFireWidthDist->m_isUserOutput =
        prop->boolean( "sizeCalcFireWidthDist" );
    vSurfaceFireShapeDiagram->m_isUserOutput =
        prop->boolean( "sizeCalcFireShapeDiagram" );
    if ( prop->boolean( "mapCalcDist" ) )
    {
        fMapScale->m_active = true;
        vSurfaceFireMapDistAtBack->m_isUserOutput =
            prop->boolean( "sizeCalcFireDistAtBack" );
        vSurfaceFireMapDistAtFlank->m_isUserOutput =
            prop->boolean( "sizeCalcFireDistAtFlank" );
        vSurfaceFireMapDistAtHead->m_isUserOutput =
            prop->boolean( "sizeCalcFireDistAtFront" );
        vSurfaceFireLengMapDist->m_isUserOutput =
            prop->boolean( "sizeCalcFireLengDist" );
        vSurfaceFireWidthMapDist->m_isUserOutput =
            prop->boolean( "sizeCalcFireWidthDist" );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Spot Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureSpotModule( PropertyDict *prop, int /* release */ )
{
    // If the Spot Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "spotModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fSpotDistActiveCrown->m_active = true;
    fSpotDistBurningPile->m_active = true;
    fSpotDistSurfaceFire->m_active = true;
    fSpotDistTorchingTrees->m_active = true;

    // If the SURFACE Module is NOT active ...
    if ( ! prop->boolean( "surfaceModuleActive" ) )
    {
        // If Surface Module is configured to use 10-m wind ...
        if ( prop->boolean( "surfaceConfWindSpeedAt10M" )
          || prop->boolean( "surfaceConfWindSpeedAt10MCalc" ) )
        {
            // Must derive wind speed at 20-ft from wind speed at 10-m
            fWindSpeedAt20Ft->m_active = true;
        }
    }

    // If the CROWN Module is NOT active ...
    if ( ! prop->boolean( "crownModuleActive" ) )
    {
		// And the user wants some active cxrown fire spotting results
        if ( prop->boolean( "spotCalcDistActiveCrown" )
		 || prop->boolean( "spotCalcFirebrandHtActiveCrown" )
		 || prop->boolean( "spotCalcFlameHtActiveCrown" )
         || prop->boolean( "spotCalcFlatDistActiveCrown" ) )
		{
			// Ask for crown fire flame length as surrogate for fireline intensity
			fCrownFireLineIntFromFlameLeng->m_active = true;
		}
	}

    // Output variables
    vSpotDistActiveCrown->m_isUserOutput =
        prop->boolean( "spotCalcDistActiveCrown" );
    vSpotFirebrandHtActiveCrown->m_isUserOutput =
        prop->boolean( "spotCalcFirebrandHtActiveCrown" );
    vSpotFlameHtActiveCrown->m_isUserOutput =
        prop->boolean( "spotCalcFlameHtActiveCrown" );
    vSpotFlatDistActiveCrown->m_isUserOutput =
        prop->boolean( "spotCalcFlatDistActiveCrown" );

    vSpotDistBurningPile->m_isUserOutput =
        prop->boolean( "spotCalcDistBurningPile" );
    vSpotCoverHtBurningPile->m_isUserOutput =
        prop->boolean( "spotCalcCoverHtBurningPile" );
    vSpotFirebrandHtBurningPile->m_isUserOutput =
        prop->boolean( "spotCalcFirebrandHtBurningPile" );
    vSpotFlatDistBurningPile->m_isUserOutput =
        prop->boolean( "spotCalcFlatDistBurningPile" );

    vSpotDistSurfaceFire->m_isUserOutput =
        prop->boolean( "spotCalcDistSurfaceFire" );
    vSpotCoverHtSurfaceFire->m_isUserOutput =
        prop->boolean( "spotCalcCoverHtSurfaceFire" );
    vSpotFirebrandDriftSurfaceFire->m_isUserOutput =
        prop->boolean( "spotCalcFirebrandDriftSurfaceFire" );
    vSpotFirebrandHtSurfaceFire->m_isUserOutput =
        prop->boolean( "spotCalcFirebrandHtSurfaceFire" );
    vSpotFlatDistSurfaceFire->m_isUserOutput =
        prop->boolean( "spotCalcFlatDistSurfaceFire" );

    vSpotDistTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcDistTorchingTrees" );
    vSpotCoverHtTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcCoverHtTorchingTrees" );
    vSpotFlameDurTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcFlameDurTorchingTrees" );
    vSpotFlameHtTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcFlameHtTorchingTrees" );
    vSpotFlameRatioTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcFlameRatioTorchingTrees" );
    vSpotFirebrandHtTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcFirebrandHtTorchingTrees" );
    vSpotFlatDistTorchingTrees->m_isUserOutput =
        prop->boolean( "spotCalcFlatDistTorchingTrees" );

    // If map distances are requested
    if ( prop->boolean( "mapCalcDist" ) )
    {
        fMapScale->m_active = true;
        //fSiteRidgeToValleyDist->m_active = true;
        fSpotMapDistActiveCrown->m_active = true;
        fSpotMapDistBurningPile->m_active = true;
        fSpotMapDistSurfaceFire->m_active = true;
        fSpotMapDistTorchingTrees->m_active = true;

        vSpotMapDistActiveCrown->m_isUserOutput =
            prop->boolean( "spotCalcDistActiveCrown" );
        vSpotMapDistBurningPile->m_isUserOutput =
            prop->boolean( "spotCalcDistBurningPile" );
        vSpotMapDistSurfaceFire->m_isUserOutput =
            prop->boolean( "spotCalcDistSurfaceFire" );
        vSpotMapDistTorchingTrees->m_isUserOutput =
            prop->boolean( "spotCalcDistTorchingTrees" );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Surface Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureSurfaceModule( PropertyDict *prop, int /* release */ )
{
    // If the Surface Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "surfaceModuleActive" ) )
    {
        return;
    }
    // First activate all module functions that are not user configurable
    fSurfaceFireCharacteristicsDiagram->m_active = true;
	fSurfaceFireDistAtBeta->m_active = true;
    fSurfaceFireDistAtHead->m_active = true;
	fSurfaceFireDistAtPsi->m_active = true;
    fSurfaceFireDistAtVector->m_active = true;
    fSurfaceFireEccentricity->m_active = true;
    fSurfaceFireEffWindAtVector->m_active = true;
	fSurfaceFireEllipseF->m_active = true;
	fSurfaceFireEllipseG->m_active = true;
	fSurfaceFireEllipseH->m_active = true;
	fSurfaceFireFlameLengAtBeta->m_active = true;
    fSurfaceFireFlameLengAtHead->m_active = true;
	fSurfaceFireFlameLengAtPsi->m_active = true;
    fSurfaceFireFlameLengAtVector->m_active = true;
    fSurfaceFireHeatPerUnitArea->m_active = true;
    fSurfaceFireHeatSource->m_active = true;
    fSurfaceFireLengthToWidth->m_active = true;
 	fSurfaceFireLineIntAtBeta->m_active = true;
    fSurfaceFireLineIntAtHead->m_active = true;
	fSurfaceFireLineIntAtPsi->m_active = true;
    fSurfaceFireLineIntAtVectorFromBeta->m_active = true;
    fSurfaceFireNoWindRate->m_active = true;
    fSurfaceFireMaxDirFromNorth->m_active = true;
    fSurfaceFireMaxDirDiagram->m_active = true;
	fSurfaceFireMapDistAtBeta->m_active = true;
    fSurfaceFireMapDistAtHead->m_active = true;
	fSurfaceFireMapDistAtPsi->m_active = true;
    fSurfaceFireMapDistAtVector->m_active = true;
    fSurfaceFirePropagatingFlux->m_active = true;
    fSurfaceFireReactionInt->m_active = true;
    fSurfaceFireResidenceTime->m_active = true;
    fSurfaceFireSpreadAtBack->m_active = true;
    fSurfaceFireSpreadAtBeta->m_active = true;
    fSurfaceFireSpreadAtHead->m_active = true;
	fSurfaceFireSpreadAtPsi->m_active = true;
    fSurfaceFireVectorBetaFromUpslope->m_active = true;
    fSurfaceFuelBedIntermediates->m_active = true;
    fSurfaceFuelBedHeatSink->m_active = true;
    fSurfaceFuelMoisTimeLag->m_active = true;

	bool weighted = prop->boolean( "surfaceConfFuelAreaWeighted" )
           || prop->boolean( "surfaceConfFuelHarmonicMean" )
           || prop->boolean( "surfaceConfFuel2Dimensional" );

	// If SIZE module is NOT requested, we also need these for Psi calculations
	if ( ! prop->boolean( "sizeModuleActive" ) )
	{
		fSurfaceFireDistAtBack->m_active = true;
		fSurfaceFireDistAtHead->m_active = true;
		fSurfaceFireEccentricity->m_active = true;
		fSurfaceFireLengDist->m_active = true;
		fSurfaceFireLengthToWidth->m_active = true;
		fSurfaceFireSpreadAtBack->m_active = true;
		fSurfaceFireWidthDist->m_active = true;
	}

    // Don't calculate crown ratio unless both Surface and Crown are active
    // and WAF is calculated
    fTreeCrownRatio->m_active = false;

    // Dead herb fuel load is constant "input"
    vSurfaceFuelLoadDeadHerb->m_isConstant = true;

    // Next apply user configuration options to functions and outputs

    //--------------------------------------------------------------------------
    // Option 1.0: Fuel is entered as
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 1.1: Fuel is entered as fuel models (standard or custom)
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfFuelModels" ) )
    {
        // Need this if no dynamic fuel modeling.
        fSurfaceFuelBedParms->m_active = true;
        // Must derive fuel parameters from fuel model input
        fSurfaceFuelBedModel->m_active = true;
    }
    //--------------------------------------------------------------------------
    // Choice 1.2: Fuel is entered as fuel parameters (for custom fuel modeling)
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfFuelParms" ) )
    {
        // Need this if no dynamic fuel modeling.
        fSurfaceFuelBedParms->m_active = true;
    }
    //--------------------------------------------------------------------------
    // Choice 1.3: Fuel is entered as two fuel models - 2d expected rate (recommended)
    // Choice 1.4: Fuel is entered as two fuel models - harmonic mean
    // Choice 1.5: Fuel is entered as two fuel models - area weighted (like old BEHAVE)
    //--------------------------------------------------------------------------
    else if ( weighted )
    {
        // Use the weighted fuel-fire function
        fSurfaceFuelBedWeighted->m_active = true;
 		//vSurfaceFireSpreadAtHead->m_isUserOutput = true;

 		// Keep this off the worksheet
		vSurfaceFuelLoadTransferEq->m_isUserInput = false;
        vSurfaceFuelLoadTransferEq->m_isConstant = true;
		vSurfaceFuelBedModelCode->m_isUserInput = false;	// added in Build 607
		vSurfaceFuelBedModelNumber->m_isUserInput = false;	// added in Build 607
	
		// Functions that are deactivated if doing weighted fuel modeling
		// since they are called directly
		fSurfaceFuelBedModel->m_active = false;					// loaded directly
		fSurfaceFuelBedParms->m_active = false;					// called directly
		fSurfaceFuelLoadTransferFraction->m_active = false;		// called directly
		fSurfaceFuelBedIntermediates->m_active = false;			// called directly
		fSurfaceFireResidenceTime->m_active = false;			// called directly
		fSurfaceFuelMoisDeadHerbWood->m_active = false;			// called directly
		fSurfaceFuelMoisLifeClass->m_active = false;			// called directly
		fSurfaceFuelMoisScenarioModel->m_active = false;		// called directly
		fSurfaceFuelMoisTimeLag->m_active = false;				// called directly
		fSurfaceFuelBedHeatSink->m_active = false;				// called directly
		fSurfaceFirePropagatingFlux->m_active = false;			// called directly
		fSurfaceFireReactionInt->m_active = false;				// called directly
		fSurfaceFireNoWindRate->m_active = false;				// called directly
		fWindAdjFactor->m_active = false;						// called directly
		fWindSpeedAt20Ft->m_active = false;						// called directly
		fWindSpeedAtMidflame->m_active = false;					// called directly
		fSurfaceFireSpreadAtHead->m_active = false;				// called directly
		fSurfaceFireLineIntAtHead->m_active = false;			// called directly
		fSurfaceFireFlameLengAtHead->m_active = false;			// called directly
		fSurfaceFireLengthToWidth->m_active = false;			// called directly
		fSurfaceFireEccentricity->m_active = false;				// called directly
		fSurfaceFireVectorBetaFromUpslope->m_active = false;	// called directly
		fSurfaceFireVectorThetaFromBeta->m_active = false;		// called directly
		fSurfaceFireVectorPsiFromTheta->m_active = false;		// called directly
		fSurfaceFireVectorPsiFromUpslope->m_active = false;		// called directly
		fSurfaceFireVectorThetaFromPsi->m_active = false;		// called directly
		fSurfaceFireVectorBetaFromTheta->m_active = false;		// called directly
		//fSurfaceFireSpreadAtBeta->m_active = false;				// called directly
		//fSurfaceFireLineIntAtBeta->m_active = false;			// called directly
		//fSurfaceFireFlameLengAtBeta->m_active = false;			// called directly
		//fSurfaceFireLengDist->m_active = false;					// called directly
		//fSurfaceFireWidthDist->m_active = false;				// called directly
		//fSurfaceFireDistAtBack->m_active = false;				// called directly
		//fSurfaceFireEllipseF->m_active = false;					// called directly
		//fSurfaceFireEllipseG->m_active = false;					// called directly
		//fSurfaceFireEllipseH->m_active = false;					// called directly
		//fSurfaceFireSpreadAtPsi->m_active = false;				// called directly
		//fSurfaceFireLineIntAtPsi->m_active = false;				// called directly
		//fSurfaceFireFlameLengAtPsi->m_active = false;			// called directly
		fSurfaceFireSpreadAtVectorFromBeta->m_active = false;	// called directly
		fSurfaceFireSpreadAtVectorFromPsi->m_active = false;	// called directly
		fSurfaceFireLineIntAtVectorFromBeta->m_active = false;	// called directly
		fSurfaceFireLineIntAtVectorFromPsi->m_active = false;	// called directly
		fSurfaceFireFlameLengAtVector->m_active = false;		// called directly
		fSurfaceFireEffWindAtVector->m_active = false;			// called directly
		fSurfaceFireHeatPerUnitArea->m_active = false;			// called directly

        // If wind adjustment factor is being calculated,
        // we must prevent fuel bed depth from becoming an input
        // as it is also calculated (but not declared as an output)

        // fSurfaceFireMaxDirFromNorth->m_active = false;
    }
    //--------------------------------------------------------------------------
    // Choice 1.6: dynamic Palmetto-Gallberry fuel model
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfFuelPalmettoGallberry" ) )
    {
        // Use PG versions of these functions:
        fSurfaceFuelPalmettoModel->m_active = true;
        fSurfaceFuelPalmettoParms->m_active = true;

		// Keep this off the worksheet
		vSurfaceFuelBedModelCode->m_isUserInput   = false;	// added in Build 610
		vSurfaceFuelBedModelCode->m_isConstant    = true;	// added in Build 610
		vSurfaceFuelBedModelNumber->m_isUserInput = false;	// added in Build 610
		vSurfaceFuelBedModelNumber->m_isConstant  = true;	// added in Build 610

        // Output variables
        vSurfaceFuelPalmettoLoadDead1->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadDead1" );
        vSurfaceFuelPalmettoLoadDead10->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadDead10" );
        vSurfaceFuelPalmettoLoadDeadFoliage->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadDeadFoliage" );
        vSurfaceFuelPalmettoLoadLive1->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadLive1" );
        vSurfaceFuelPalmettoLoadLive10->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadLive10" );
        vSurfaceFuelPalmettoLoadLiveFoliage->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadLiveFoliage" );
        vSurfaceFuelPalmettoLoadLitter->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoLoadLitter" );
        vSurfaceFuelBedDepth->m_isUserOutput =
            prop->boolean( "surfaceCalcPalmettoBedDepth" );
    }
    //--------------------------------------------------------------------------
    // Choice 1.7: dynamic Western Aspen fuel models
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfFuelAspen" ) )
    {
        // Use Aspen versions of these functions:
        fSurfaceFuelAspenModel->m_active = true;
        fSurfaceFuelAspenParms->m_active = true;
        fTreeMortalityRateAspenAtVector->m_active = true;

		// Keep this off the worksheet
		vSurfaceFuelBedModelCode->m_isUserInput   = false;	// added in Build 610
		vSurfaceFuelBedModelCode->m_isConstant    = true;	// added in Build 610
		vSurfaceFuelBedModelNumber->m_isUserInput = false;	// added in Build 610
		vSurfaceFuelBedModelNumber->m_isConstant  = true;	// added in Build 610

        // Output variables
        vSurfaceFuelAspenLoadDead1->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenLoadDead1" );
        vSurfaceFuelAspenLoadDead10->m_isUserOutput = false;
            //prop->boolean( "surfaceCalcAspenLoadDead10" ); Pat says no
        vSurfaceFuelAspenLoadLiveHerb->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenLoadLiveHerb" );
        vSurfaceFuelAspenLoadLiveWoody->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenLoadLiveWoody" );
        vSurfaceFuelAspenSavrDead1->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenSavrDead1" );
        vSurfaceFuelAspenSavrDead10->m_isUserOutput = false;
            //prop->boolean( "surfaceCalcAspenSavrDead10" ); Pat says no
        vSurfaceFuelAspenSavrLiveHerb->m_isUserOutput = false;
            //prop->boolean( "surfaceCalcAspenSavrLiveHerb" );  Pat says no
        vSurfaceFuelAspenSavrLiveWoody->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenSavrLiveWoody" );
        vSurfaceFuelBedDepth->m_isUserOutput = false;
            //prop->boolean( "surfaceCalcAspenBedDepth" ); Pat says no
        vTreeMortalityRateAspenAtVector->m_isUserOutput =
            prop->boolean( "surfaceCalcAspenMortality" );
    }

    //--------------------------------------------------------------------------
    // Option 2: Dynamic curing load transfer is:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 2.1: Dynamic fuel load transfer is calculated
    //             from live herbaceous fuel moisture
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfLoadTransferCalc" ) )
    {
        vSurfaceFuelLoadTransferFraction->m_isUserInput = false;
        fSurfaceFuelLoadTransferFraction->m_active = true;
    }
    //--------------------------------------------------------------------------
    // Choice 2.2: Dynamic fuel load transfer is input directly.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfLoadTransferInput" ) )
    {
        vSurfaceFuelLoadTransferFraction->m_isUserInput = true;
        fSurfaceFuelLoadTransferFraction->m_active = false;
    }

    // Never do load transfer for Palmetto-Gallberry or Western Aspen
    if ( prop->boolean( "surfaceConfFuelPalmettoGallberry" )
      || prop->boolean( "surfaceConfFuelAspen" ) )
    {
        vSurfaceFuelLoadTransferEq->m_isConstant = true;
        vSurfaceFuelLoadTransferEq->updateItem( 0 );
        vSurfaceFuelLoadTransferFraction->m_isUserInput = false;
        fSurfaceFuelLoadTransferFraction->m_active = true;
        vSurfaceFuelLoadTransferFraction->m_isConstant = true;
        vSurfaceFuelLoadTransferFraction->update( 0.0 );
    }

    //--------------------------------------------------------------------------
    // Choice 3: Moisture is entered by:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 3.1: Moisture is entered by individual size class.
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfMoisTimeLag" ) )
    {
        // Already set up for this
    }
    //--------------------------------------------------------------------------
    // Choice 3.2: Moisture is entered by dead and live category.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfMoisLifeCat" ) )
    {
        // Must derive time lag fuel moisture from life category moistures
        fSurfaceFuelMoisLifeClass->m_active = true;
    }
    //--------------------------------------------------------------------------
    // Choice 3.3: Moisture is entered by dead, herb, and wood categories.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfMoisDeadHerbWood" ) )
    {
        // Must derive time lag fuel moisture from dead category and herb and woody classes
        fSurfaceFuelMoisDeadHerbWood->m_active = true;
    }
    //--------------------------------------------------------------------------
    // Choice 3.4: Moisture is entered by moisture scenario.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfMoisScenario" ) )
    {
        // Must derive time lag fuel moisture from moistures scenario
        fSurfaceFuelMoisScenarioModel->m_active = true;
    }

    //--------------------------------------------------------------------------
    // Option 4: Wind speed is entered as:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 4.1: Wind speed is entered as midflame height
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfWindSpeedAtMidflame" ) )
    {
        vTreeCanopyCrownFraction->m_isConstant = true;
        vTreeCanopyCrownFraction->update( 0.0 );
        vWindAdjFactor->m_isConstant = true;
        vWindAdjFactor->update( 1.0 );
        vWindAdjMethod->m_isConstant = true;
        vWindAdjMethod->updateItem( 2 );
    }
    //--------------------------------------------------------------------------
    // Choice 4.2: Wind speed is entered as 20-ft wind
    //           and INPUT wind adj factor.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfWindSpeedAt20Ft" ) )
    {
        // Must derive wind speed at midflame height from wind speed at 20 ft
        fWindSpeedAtMidflame->m_active = true;
        // User wants to enter WAF
        fWindAdjFactor->m_active = false;
        // And user may request derived midflame wind speed as output
        vWindSpeedAtMidflame->m_isUserOutput =
            prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
        // In case this is requested as output
        vTreeCanopyCrownFraction->m_isConstant = true;
        vTreeCanopyCrownFraction->update( 0.0 );
        vWindAdjMethod->m_isConstant = true;
        vWindAdjMethod->updateItem( 2 );
    }
    //--------------------------------------------------------------------------
    // Choice 4.3: Wind speed is entered as 20-ft wind
    //           and CALCULATED wind adj factor.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfWindSpeedAt20FtCalc" ) )
    {
        // Must derive wind speed at midflame height from wind speed at 20 ft
        fWindSpeedAtMidflame->m_active = true;
        // User wants WAF to be calculated (and maybe output)
        fWindAdjFactor->m_active = true;
        // And user is may request derived midflame wind speed as output
        vWindSpeedAtMidflame->m_isUserOutput =
            prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
        // Prevent circular dependency
        if ( weighted )
        {
            vSurfaceFuelBedDepth->m_isConstant = true;
        }
        if ( prop->boolean( "crownModuleActive" ) )
        {
            fTreeCrownRatio->m_active = true;
            vTreeCrownRatio->m_isUserOutput = prop->boolean( "surfaceCalcCrownRatio" );
        }
        // In case this is requested as output
        vTreeCanopyCrownFraction->m_isConstant = false; // Crown Fill Portion
        vTreeCanopyCrownFraction->update( 0.0 );
        vWindAdjMethod->m_isConstant = false;
        vWindAdjMethod->updateItem( 2 );
    }
    //--------------------------------------------------------------------------
    // Choice 4.4: Wind speed is entered as 10-m wind
    //             and INPUT wind adj factor.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfWindSpeedAt10M" ) )
    {
        // Must derive wind speed at 20-ft from wind speed at 10-m
        fWindSpeedAt20Ft->m_active = true;
        // Must derive wind speed at midflame height from wind speed at 20-ft
        fWindSpeedAtMidflame->m_active = true;
        // User wants to enter WAF
        fWindAdjFactor->m_active = false;
        // And user is may request derived midflame wind speed as output
        vWindSpeedAtMidflame->m_isUserOutput =
            prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
        // In case this is requested as output
        vTreeCanopyCrownFraction->m_isConstant = true;
        vTreeCanopyCrownFraction->update( 0.0 );
        vWindAdjMethod->m_isConstant = true;
        vWindAdjMethod->updateItem( 2 );
    }
    //--------------------------------------------------------------------------
    // Choice 4.5: Wind speed is entered as 10-m wind
    //             and CALCULATED wind adj factor.
    //--------------------------------------------------------------------------
    else if ( prop->boolean( "surfaceConfWindSpeedAt10MCalc" ) )
    {
        // Must derive wind speed at 20-ft from wind speed at 10-m
        fWindSpeedAt20Ft->m_active = true;
        // Must derive wind speed at midflame height from wind speed at 20-ft
        fWindSpeedAtMidflame->m_active = true;
        // User wants WAF to be calculated (and maybe output)
        fWindAdjFactor->m_active = true;
        // And user is may request derived midflame wind speed as output
        vWindSpeedAtMidflame->m_isUserOutput =
            prop->boolean( "surfaceCalcWindSpeedAtMidflame" );
        // Prevent circular dependency
        if ( weighted )
        {
            vSurfaceFuelBedDepth->m_isConstant = true;
        }
        if ( prop->boolean( "crownModuleActive" ) )
        {
            fTreeCrownRatio->m_active = true;
            vTreeCrownRatio->m_isUserOutput = prop->boolean( "surfaceCalcCrownRatio" );
        }
        // In case this is requested as output
        vTreeCanopyCrownFraction->m_isConstant = false; // Crown Fill Portion
        vTreeCanopyCrownFraction->update( 0.0 );
        vWindAdjMethod->m_isConstant = false;
        vWindAdjMethod->updateItem( 2 );
    }
    //--------------------------------------------------------------------------
    // Option 5: Impose maximum reliable effective wind speed limit?
    //--------------------------------------------------------------------------
    // EqCalc::FireSpreadAtHead() checks for the surfaceConfWindLimitApplied
    // property and passes it into FBL_SurfaceFireForwardSpreadRate(),
    // so no further setup is needed here

    //--------------------------------------------------------------------------
    // Option 6: Surface fire spread direction is:
    //	Choice 6.1: Surface fire spread direction is heading only (Version 6).
    //	Choice 6.2: Surface fire spread direction is backing only (Version 6).
    //	Choice 6.3: Surface fire spread direction is flanking only (Version 6).
    //	Choice 6.4: Surface fire spread direction is in specified directions from
	//				Fire Front (Version 6).
    //	Choice 6.5: Surface fire spread direction is in specified directions from
	//				Ignition Point (fireline intensity and flame length are based
	//				on flaming segment in direction normal to the fire perimeter:
	//				recommended). (Version 6).
    //	Choice 6.6: Surface fire spread direction is in specified directions from
	//				Ignition Point (fireline intensity and flame length are based
	//				on flaming segment in direction from ignition point: method
	//				used through version 5). (Version 6).
    //--------------------------------------------------------------------------
	bool opt_spread_head       = prop->boolean( "surfaceConfSpreadDirHead" );
	bool opt_spread_back       = prop->boolean( "surfaceConfSpreadDirBack" );
	bool opt_spread_flank      = prop->boolean( "surfaceConfSpreadDirFlank" );
	bool opt_spread_beta_beta  = prop->boolean( "surfaceConfSpreadDirPointSourceBeta" );
	bool opt_spread_beta_psi   = prop->boolean( "surfaceConfSpreadDirPointSourcePsi" );
	bool opt_spread_fire_front = prop->boolean( "surfaceConfSpreadDirFireFront" );
	// Convenience
	bool opt_spread_beta       = opt_spread_beta_beta || opt_spread_beta_psi;
	bool opt_spread_psi        = ! opt_spread_beta;

	// Psi-only functions
	fSurfaceFireVectorPsiFromUpslope->m_active   = opt_spread_psi;			// Get Psi from user input?
	fSurfaceFireVectorThetaFromPsi->m_active     = opt_spread_psi;			// Derive Theta from input Psi
	fSurfaceFireVectorBetaFromTheta->m_active    = opt_spread_psi;			// Derive Beta from Theta
	fSurfaceFireSpreadAtVectorFromPsi->m_active  = opt_spread_psi;			// Store Psi ROS into AtVector
	fSurfaceFireLineIntAtVectorFromPsi->m_active = opt_spread_psi || opt_spread_beta_psi; // Store Psi FLI into AtVector
	// Beta-only functions
	fSurfaceFireVectorBetaFromUpslope->m_active   = opt_spread_beta;		// Get Beta from user input?
	fSurfaceFireVectorThetaFromBeta->m_active     = opt_spread_beta;		// Derive Theta from Beta
	fSurfaceFireVectorPsiFromTheta->m_active      = opt_spread_beta;		// Derive Psi from Theta
	fSurfaceFireSpreadAtVectorFromBeta->m_active  = opt_spread_beta;		// Store Beta ROS into AtVector
	fSurfaceFireLineIntAtVectorFromBeta->m_active = opt_spread_beta_beta;	// Store Beta FLI into AtVector

    // Need to derive spread direction wrt upslope from the input spread direction wrt north?
    fSurfaceFireVectorDirFromUpslope->m_active = prop->boolean( "surfaceConfDegreesWrtNorth" );

	// If user wants fixed input directions...
    vSurfaceFireVectorDirFromUpslope->m_isConstant = false;
	if ( opt_spread_head || opt_spread_back || opt_spread_flank )
    {
		// Assign direction based only on the input option
		fSurfaceFireVectorBetaFromUpslope->m_active = false;
		fSurfaceFireVectorPsiFromUpslope->m_active = false;
        vSurfaceFireVectorDirFromUpslope->m_isConstant = true;
		vSurfaceFireVectorDirFromUpslope->update( 0. );
        vSurfaceFireVectorPsi->m_isConstant = true;
		double dir = 0.;
		dir = opt_spread_back ? 180. : dir;
		dir = opt_spread_flank ? 90. : dir;
 		vSurfaceFireVectorPsi->update( dir );
	}

    // User only gets outputs in the direction of the fire HEAD.
    // NOTE -- SURFACE only produces spread AtVector
    vSurfaceFireDistAtHead->m_isUserOutput = false;
    vSurfaceFireEffWindAtHead->m_isUserOutput = prop->boolean( "surfaceCalcFireEffWind" );

	// Either the beta-based or psi-based ROS, FLI, FL, DIST variables are in the AtVector variables
	vSurfaceFireSpreadAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireSpread" );
	vSurfaceFireLineIntAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireLineInt" );
	vSurfaceFireFlameLengAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireFlameLeng" );
	vSurfaceFireDistAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireDist" );
    if ( prop->boolean( "mapCalcDist" ) )
    {
        fMapScale->m_active = true;
        vSurfaceFireMapDistAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireDist" );
    }

	// Does user also want Intermediate Beta-based outputs?
	vSurfaceFireVectorBeta->m_isUserOutput = prop->boolean( "surfaceCalcVectorOther" );
	vSurfaceFireVectorPsi->m_isUserOutput = prop->boolean( "surfaceCalcVectorOther" );
	vSurfaceFireSpreadAtBeta->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireSpreadAtOther" );
	vSurfaceFireSpreadAtPsi->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireSpreadAtOther" );
	vSurfaceFireLineIntAtBeta->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireLineIntAtOther" );
	vSurfaceFireLineIntAtPsi->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireLineIntAtOther" );
	vSurfaceFireFlameLengAtBeta->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireFlameLengAtOther" );
	vSurfaceFireFlameLengAtPsi->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireFlameLengAtOther" );
	vSurfaceFireDistAtBeta->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireDistAtOther" );
	vSurfaceFireDistAtPsi->m_isUserOutput = ! weighted && prop->boolean( "surfaceCalcFireDistAtOther" );
    if ( prop->boolean( "mapCalcDist" ) )
    {
	    fMapScale->m_active = true;
		vSurfaceFireMapDistAtBeta->m_isUserOutput = ! weighted && opt_spread_psi && prop->boolean( "surfaceCalcFireDistAtOther" );
		vSurfaceFireMapDistAtPsi->m_isUserOutput = ! weighted && opt_spread_beta && prop->boolean( "surfaceCalcFireDistAtOther" );
	}
    vSurfaceFireEllipseF->m_isUserOutput = prop->boolean( "surfaceCalcEllipseF" ) ;
    vSurfaceFireEllipseG->m_isUserOutput = prop->boolean( "surfaceCalcEllipseG" ) ;
    vSurfaceFireEllipseH->m_isUserOutput = prop->boolean( "surfaceCalcEllipseH" ) ;
    vSurfaceFireVectorTheta->m_isUserOutput = prop->boolean( "surfaceCalcVectorTheta" ) ;

    //--------------------------------------------------------------------------
    //	Choice 6.1: Rate of spread is calculated only in the direction of maximum spread.
	//	NOTE: This is no longer used as of Version 6.
    //--------------------------------------------------------------------------
    if ( false ) // prop->boolean( "surfaceConfSpreadDirMax" ) )
    {
        vSurfaceFireVectorDirFromUpslope->m_isConstant = true;
        vSurfaceFireVectorDirFromUpslope->update( 0. );
        vSurfaceFireVectorBeta->m_isConstant = true;
        vSurfaceFireVectorBeta->update( 0. );
        // User only gets outputs in the direction of the fire HEAD.
        // NOTE -- SURFACE only produces spread AtVector
        vSurfaceFireDistAtHead->m_isUserOutput = false;
    }
    //--------------------------------------------------------------------------
    // Choice 6.2: Rate of spread is calculated in directions specified on the worksheet
	//	NOTE: This is no longer used as of Version 6.
    //--------------------------------------------------------------------------
    else if ( false ) // prop->boolean( "surfaceConfSpreadDirInput" ) )
    {
        // User gets outputs in the direction of the fire VECTOR.
        vSurfaceFireDistAtVector->m_isUserOutput = prop->boolean( "surfaceCalcFireDist" );
    }

	// Elapsed time is a constant when just Psi-related variables are required
	// But it must be user input if any of the distance outputs are requested
	vSurfaceFireElapsedTime->m_isConstant = true;
	vSurfaceFireElapsedTime->update( 1.0 );
	if ( prop->boolean( "surfaceCalcFireDist" )
      || prop->boolean( "surfaceCalcFireDistAtOther" )
	  || ( prop->boolean( "crownModuleActive" ) && prop->boolean( "crownCalcCrownSpreadDist" ) )
	  || ( prop->boolean( "sizeModuleActive" ) ) )
	  //|| ( prop->boolean( "sizeModuleActive" ) && prop->boolean( "sizeCalcFireDistAtBack" ) )
	  //|| ( prop->boolean( "sizeModuleActive" ) && prop->boolean( "sizeCalcFireDistAtFront" ) )
	  //|| ( prop->boolean( "sizeModuleActive" ) && prop->boolean( "sizeCalcFireLengDist" ) )
	  //|| ( prop->boolean( "sizeModuleActive" ) && prop->boolean( "sizeCalcFireWidthDist" ) )
	  //|| ( prop->boolean( "sizeModuleActive" ) && prop->boolean( "sizeCalcFireShapeDiagram" ) ) )
	{
		vSurfaceFireElapsedTime->m_isConstant = false;
	}

	// HACK to force label "Fire Spread from Fire Front (from Upslope/North)"
	if ( prop->boolean( "surfaceConfSpreadDirFireFront" ) )
	{
		const char* label = prop->boolean( "surfaceConfDegreesWrtUpslope" ) ? "Upslope" : "North";
		m_eqTree->setLabel( vSurfaceFireVectorPsi, label );
	}

    // Initialize use of standard labels for windspeeds
    // e.g., without "(upslope)" appended
    m_eqTree->setLabel( vWindSpeedAtMidflame, "" );
    m_eqTree->setLabel( vWindSpeedAt20Ft, "" );
    m_eqTree->setLabel( vWindSpeedAt10M, "" );

    //--------------------------------------------------------------------------
    // Option 7: Wind direction is:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 7.1: Wind direction is specified on the worksheet.
    //--------------------------------------------------------------------------
    vWindDirFromUpslope->m_isConstant = false;
    if ( prop->boolean( "surfaceConfWindDirInput" ) )
    {
        // Option 8: Wind and spread directions are
        // Choice 8.2: degrees clockwise from north (direction from which the wind is blowing)
        if ( prop->boolean( "surfaceConfDegreesWrtNorth" ) )
        {
            // Must derive wind vector from wind source
            fWindDirFromUpslope->m_active = true;
        }
    }
    //--------------------------------------------------------------------------
    // Choice 7.2: Wind direction is upslope.
    //--------------------------------------------------------------------------
    else // if ( prop->boolean( "surfaceConfWindDirUpslope" ) )
    {
        // No aspect or wind direction is needed,
        // so make wind direction from upslope an input CONSTANT of 0.
        vWindDirFromUpslope->m_isConstant = true;
        vWindDirFromUpslope->update( 0. );
        // Use special labels for upslope windspeeds
        m_eqTree->setLabel( vWindSpeedAtMidflame, "Upslope" );
        m_eqTree->setLabel( vWindSpeedAt20Ft, "Upslope" );
        m_eqTree->setLabel( vWindSpeedAt10M, "Upslope" );
    }

    //--------------------------------------------------------------------------
    // Option 8: Wind & spread directions are:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 8.1: Wind and spread directions are degrees clockwise from upslope.
    //             (the direction the wind is pushing the fire).
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfDegreesWrtUpslope" ) )
    {
        // Make sure the output spread direction is also wrt upslope
        vSurfaceFireMaxDirFromUpslope->m_isUserOutput =
            (  prop->boolean( "surfaceCalcFireMaxDirFromUpslope" )
            // (also output if the fire/wind/slope diagram is requested)
            || prop->boolean( "surfaceCalcFireMaxDirDiagram" )
            // (also output if the Size Module fire shape diagram is requested)
            || ( prop->boolean( "sizeModuleActive" )
              && prop->boolean( "sizeCalcFireShapeDiagram" ) ) );
    }
    //--------------------------------------------------------------------------
    // Choice 8.2: Wind and spread directions are degrees clockwise from north.
    //             (direction from which the wind is blowing).
    //--------------------------------------------------------------------------
    else // if ( prop->boolean( "surfaceConfDegreesWrtNorth" ) )
    {
        // Must derive upslope direction from aspect
        fSiteUpslopeDirFromNorth->m_active = true;
        // Make sure the output spread direction is also wrt north
        vSurfaceFireMaxDirFromNorth->m_isUserOutput =
            (  prop->boolean( "surfaceCalcFireMaxDirFromUpslope" )
            // (also output if the fire/wind/slope diagram is requested)
            || prop->boolean( "surfaceCalcFireMaxDirDiagram" )
            // (also output if the Size Module fire shape diagram is requested)
            || ( prop->boolean( "sizeModuleActive" )
              && prop->boolean( "sizeCalcFireShapeDiagram" ) ) );
    }

    //--------------------------------------------------------------------------
    // Option 10: Slope steepness is:
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Choice 10.1: Slope steepness is specified on the worksheet.
    //--------------------------------------------------------------------------
    if ( prop->boolean( "surfaceConfSlopeInput" ) )
    {
        //----------------------------------------------------------------------
        // Option 9: Slope is specified as
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        // Choice 9.1: Slope is specified as percent.
        //----------------------------------------------------------------------
        if ( prop->boolean( "surfaceConfSlopeFraction" ) )
        {
            // Already set up for this
        }
        //----------------------------------------------------------------------
        // Choice 9.2: Slope is specified in degrees.
        //----------------------------------------------------------------------
        else if ( prop->boolean( "surfaceConfSlopeDegrees" ) )
        {
            // Must derive the slope fraction from slope degrees
            fSiteSlopeFraction->m_active = true;
        }
        // No vSiteSlopeSteepness, vSiteSlopeRise, or vSiteSlopeReach output
    }
    //--------------------------------------------------------------------------
    // Choice 10.2: Slope steepness is calculated from map measurements.
    //--------------------------------------------------------------------------
    else // if ( prop->boolean( "surfaceConfSlopeDerived" ) )
    {
        // Must derive slope degrees from map measurements
        fMapSlope->m_active = true;
        fMapScale->m_active = true;
        // Must derive slope fraction from slope degrees
        fSiteSlopeFraction->m_active = true;
        // The user may request results of slope calculations
        vSiteSlopeReach->m_isUserOutput =
            prop->boolean( "surfaceCalcSlopeReach" );
        vSiteSlopeRise->m_isUserOutput =
            prop->boolean( "surfaceCalcSlopeRise" );
        if ( prop->boolean( "surfaceCalcSlopeSteepness" ) )
        {
            //----------------------------------------------------------------------
            // Option 9: Slope is specified as
            //----------------------------------------------------------------------

            //----------------------------------------------------------------------
            // Choice 9.1: Slope is specified as percent.
            //----------------------------------------------------------------------
            vSiteSlopeFraction->m_isUserOutput =
                prop->boolean( "surfaceConfSlopeFraction" );
            //----------------------------------------------------------------------
            // Choice 9.2: Slope is specified as degrees.
            //----------------------------------------------------------------------
            vSiteSlopeDegrees->m_isUserOutput =
                prop->boolean( "surfaceConfSlopeDegrees" );
        }
    }

    // Finally apply user output selections that are not affected by any
    // configuration options
    vSurfaceFireHeatPerUnitArea->m_isUserOutput =
        prop->boolean( "surfaceCalcFireHeatPerUnitArea" );
    vSurfaceFireReactionInt->m_isUserOutput =
        prop->boolean( "surfaceCalcFireReactionInt" );
    vSurfaceFireWindSpeedFlag->m_isUserOutput =
        prop->boolean( "surfaceCalcFireWindSpeedFlag" );
    vSurfaceFireWindSpeedLimit->m_isUserOutput =
        prop->boolean( "surfaceCalcFireWindSpeedLimit" );
    vTreeCanopyCrownFraction->m_isUserOutput =
        prop->boolean( "surfaceCalcCrownFillPortion" );
    vWindAdjFactor->m_isUserOutput =
        prop->boolean( "surfaceCalcWindAdjFactor" );
    vWindAdjMethod->m_isUserOutput =
        prop->boolean( "surfaceCalcWindAdjMethod" );

    // These should never be output if 2 fuel models used
    bool twofuels = prop->boolean( "surfaceConfFuelAreaWeighted" )
           || prop->boolean( "surfaceConfFuelHarmonicMean" )
           || prop->boolean( "surfaceConfFuel2Dimensional" );
    vSurfaceFireHeatSource->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireHeatSource" );
    vSurfaceFireReactionIntDead->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireReactionIntDead" );
    vSurfaceFireReactionIntLive->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireReactionIntLive" );
    vSurfaceFireMaxDirDiagram->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireMaxDirDiagram" );
    vSurfaceFireCharacteristicsDiagram->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireCharacteristicsDiagram" );
    vSurfaceFuelLoadTransferFraction->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelLoadTransferFraction" );
    vSurfaceFuelLoadDead->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelLoadDead" );
    vSurfaceFuelLoadDeadHerb->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelLoadDeadHerb" );
    vSurfaceFuelLoadLive->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelLoadLive" );
    vSurfaceFuelLoadUndeadHerb->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelLoadUndeadHerb" );
    vSurfaceFuelBedSigma->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedSigma" );
    vSurfaceFuelBedPackingRatio->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedPackingRatio" );
    vSurfaceFuelBedBulkDensity->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedBulkDensity" );
    vSurfaceFuelBedBetaRatio->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedBetaRatio" );
    vSurfaceFuelBedDeadFraction->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedDeadFraction" );
    vSurfaceFuelBedLiveFraction->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedLiveFraction" );
    vSurfaceFuelBedHeatSink->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedHeatSink" );
    vSurfaceFuelBedMoisDead->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedMoisDead" );
    vSurfaceFuelBedMoisLive->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedMoisLive" );
    vSurfaceFuelBedMextLive->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFuelBedMextLive" );
    vSurfaceFireResidenceTime->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireResidenceTime" );
    vSurfaceFireWindFactor->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireWindFactor" );
    vSurfaceFireSlopeFactor->m_isUserOutput = twofuels ? false :
        prop->boolean( "surfaceCalcFireSlopeFactor" );

    // Until we add BURNUP, 1000-hr moisture is never used.
    vSurfaceFuelMoisDead1000->update( 0.20 );
    vSurfaceFuelMoisDead1000->m_isConstant = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reconfigures the EqTree Weather Module variables & functions
 *  based upon current user option settings.
 *
 *  \param release Application's release number.
 */

void EqCalc::reconfigureWeatherModule( PropertyDict *prop, int /* release */ )
{
    // If the Weather Module is not active, leave all its functions inactive
    // and all its input and output variables cleared.
    if ( ! prop->boolean( "weatherModuleActive" ) )
    {
        return;
    }

    // First activate all module functions that are not user configurable
    fWthrRelativeHumidity->m_active = true;
    fWthrCumulusBaseHt->m_active = true;
    fWthrHeatIndex->m_active = true;
    fWthrSummerSimmerIndex->m_active = true;
    fWthrWindChillTemp->m_active = true;

    // Option 1: Relative humidity is derived from
    // Choice 2: dry and wet bulb temperatures and elevation.
    if ( prop->boolean( "weatherConfHumidityFromWetBulbElev" ) )
    {
        // Must derive dew point from dry bulb and wet bulb temperatures
        fWthrDewPointTemp->m_active = true;
        // User may request dew point temperature as an output
        vWthrDewPointTemp->m_isUserOutput =
            prop->boolean( "weatherCalcWthrDewPointTemp" );
    }
    // Choice 1: dew point and dry bulb temperatures
    else // if ( prop->boolean( "weatherConfHumidityFromDewPoint" ) )
    {
        // Already set up for this
    }

    // Output variables
    vWthrRelativeHumidity->m_isUserOutput =
        prop->boolean( "weatherCalcWthrRelativeHumidity" );

    // These are not yet implemented at the user interface
    vWthrCumulusBaseHt->m_isUserOutput =
        prop->boolean( "weatherCalcWthrCumulusBaseHt" );
    vWthrHeatIndex->m_isUserOutput =
        prop->boolean( "weatherCalcWthrHeatIndex" );
    vWthrSummerSimmerIndex->m_isUserOutput =
        prop->boolean( "weatherCalcWthrSummerSimmerIndex" );
    vWthrWindChillTemp->m_isUserOutput =
        prop->boolean( "weatherCalcWthrWindChillTemp" );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines whether or not the GUI should display the "Initialize
 *  from Fuel Model" button when the user is doing "fuel modeling".
 *
 *  Called only by BpDocument::composeWorksheet().  The button should only be
 *  shown if fuel parameters rather than fuel models are input parameters.
 *
 *  \return TRUE if the "Init from Fuel Model" button should be displayed.
 */

bool EqCalc::showInitFromFuelModelButton( void ) const
{
    return( fSurfaceFuelBedParms->m_active
       && ! fSurfaceFuelBedModel->m_active );
}

//------------------------------------------------------------------------------
//  End of xeqcalcreconfig.cpp
//------------------------------------------------------------------------------

