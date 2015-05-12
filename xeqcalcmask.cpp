//------------------------------------------------------------------------------
/*! \file xeqcalcmask.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2015 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Routines to mask/unmask EqCalc input variables.
 *
 *  This is one of the few EqCalc files that hold implementation-specific
 *  variable and function pointers.  The files are xeqcalc.cpp,
 *  xeqcalcmask.cpp, and xeqcalcreconfig.cpp.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "fuelmodel.h"
#include "parser.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Standard include files
#include <math.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/*! \brief Determines if there is a conflict between the Surface Module
 *  estimating spread at vectors other than the head, and the Spot Module
 *  which predicts spot distance from the head of a wind driven surface fire.
 *
 *  \return TRUE if the conflict exists, FALSE if no conflict.
 */

bool EqCalc::conflict1( void ) const
{
    PropertyDict *prop = m_eqTree->m_propDict;
    bool surfaceModuleActive = prop->boolean( "surfaceModuleActive" );
	bool surfaceSpreadNotAtHead = ! prop->boolean( "surfaceConfSpreadDirHead" );
    bool spotModuleActive = prop->boolean( "spotModuleActive" );
    bool surfaceSpotActive = prop->boolean( "spotCalcDistSurfaceFire" )
		|| prop->boolean( "spotCalcCoverHtSurfaceFire" )
		|| prop->boolean( "spotCalcFirebrandDriftSurfaceFire" )
		|| prop->boolean( "spotCalcFirebrandHtSurfaceFire" )
		|| prop->boolean( "spotCalcFlatDistSurfaceFire" );
	bool conflict = surfaceModuleActive && surfaceSpreadNotAtHead &&
		spotModuleActive && surfaceSpotActive;
	return conflict;
}

//------------------------------------------------------------------------------
/*! \brief Determines if there is a conflict between wind speeds.
 *
 *  This occurs if Surface Module is using midflame wind speed, but the Spot
 *  module uses above-canopy wind speed.
 *
 *  \return TRUE if the conflict exists, FALSE if no conflict.
 */

bool EqCalc::conflict2( void ) const
{
    return( vWindSpeedAtMidflame->m_isUserInput
    && ( vWindSpeedAt20Ft->m_isUserInput || vWindSpeedAt10M->m_isUserInput ) ) ;
}

//------------------------------------------------------------------------------
/*! \brief Read access to the current vDocDescription m_store from BpDocument.
 *
 *  \return Reference to the current vDocDescription->m_store.
 */

QString &EqCalc::docDescriptionStore( void ) const
{
    return( vDocDescription->m_store );
}

//------------------------------------------------------------------------------
/*! \brief Write access to the current vDocDescription m_store from BpDocument.
 *
 *  \param newStore New value for ther vDocDescription text.
 *
 *  \return Reference to the new vDocDescription->m_store.
 */

QString &EqCalc::docDescriptionStore( const QString &newStore )
{
    return( vDocDescription->m_store = newStore );
}

//------------------------------------------------------------------------------
/*! \brief Initializes the fuel parameter entries from a Fuel Module.
 *
 *  \param fmPtr Pointer to a FuelModel.
 *
 *  Called only by BpDocument::fuelClicked().
 */

void EqCalc::initFuelInputs( FuelModel *fmPtr )
{
    vSurfaceFuelLoadDead1->nativeStore( fmPtr->m_load1 );
    vSurfaceFuelLoadDead10->nativeStore( fmPtr->m_load10 );
    vSurfaceFuelLoadDead100->nativeStore( fmPtr->m_load100 );
    vSurfaceFuelLoadLiveHerb->nativeStore( fmPtr->m_loadHerb );
    vSurfaceFuelLoadLiveWood->nativeStore( fmPtr->m_loadWood );
    vSurfaceFuelBedDepth->nativeStore( fmPtr->m_depth );
    vSurfaceFuelBedMextDead->nativeStore( fmPtr->m_mext );
    vSurfaceFuelSavrDead1->nativeStore( fmPtr->m_savr1 );
    vSurfaceFuelSavrLiveHerb->nativeStore( fmPtr->m_savrHerb );
    vSurfaceFuelSavrLiveWood->nativeStore( fmPtr->m_savrWood );
    vSurfaceFuelHeatDead->nativeStore( fmPtr->m_heatDead );
    vSurfaceFuelHeatLive->nativeStore( fmPtr->m_heatLive );
    vSurfaceFuelLoadTransferEq->setStore( fmPtr->m_transfer );
    vSurfaceFuelLoadTransferFraction->nativeStore( 0.0 );
    vSurfaceFuelLoadDeadHerb->nativeStore( 0.0 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines if the EqVar is a FuelModel variable.
 *
 *  \param varPtr Pointer to an EqVar.
 *
 *  Called only by BpDocument::saveFuelModel().
 */

bool EqCalc::isFuelModelVariable( EqVar *varPtr ) const
{
    if ( varPtr == vSurfaceFuelLoadDead1
      || varPtr == vSurfaceFuelLoadDead10
      || varPtr == vSurfaceFuelLoadDead100
      || varPtr == vSurfaceFuelLoadLiveHerb
      || varPtr == vSurfaceFuelLoadLiveWood
      || varPtr == vSurfaceFuelSavrDead1
      || varPtr == vSurfaceFuelSavrLiveHerb
      || varPtr == vSurfaceFuelSavrLiveWood
      || varPtr == vSurfaceFuelHeatDead
      || varPtr == vSurfaceFuelHeatLive
      || varPtr == vSurfaceFuelBedDepth
      || varPtr == vSurfaceFuelBedMextDead
      || varPtr == vSurfaceFuelLoadTransferEq
	  || varPtr == vSurfaceFuelLoadTransferFraction
	  || varPtr == vSurfaceFuelLoadDeadHerb
	  || varPtr == vSurfaceFuelBedModelCode
	  || varPtr == vSurfaceFuelBedModelNumber )
    {
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the EqVar is a MoisScenario variable.
 *
 *  \param varPtr Pointer to an EqVar.
 *
 *  Called only by BpDocument::saveMoisScenario().
 */

bool EqCalc::isMoisScenarioVariable( EqVar *varPtr ) const
{
    if ( varPtr == vSurfaceFuelMoisDead1
      || varPtr == vSurfaceFuelMoisDead10
      || varPtr == vSurfaceFuelMoisDead100
      || varPtr == vSurfaceFuelMoisLiveHerb
      || varPtr == vSurfaceFuelMoisLiveWood )
    {
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Marks unneeded fuel moistures, savr's, and heats as "masked"
 *  if there is no corresponding fuel loading.
 *
 *  This overloaded version is called only by
 *  BpDocument::validateWorksheetEntry() when validating a single input entry.
 *  It simply checks to see if this is an EqVar of interest (e.g., one that
 *  may affect maskable inputs), and if so, calls maskInputs() to do the
 *  real work.
 */

void EqCalc::maskInputs( EqVar *varPtr )
{
    PropertyDict *prop = m_eqTree->m_propDict;
    // Check if this EqVar triggers a mask test
    if ( varPtr == vSurfaceFuelBedModel
      || varPtr == vSurfaceFuelBedModel1
      || varPtr == vSurfaceFuelBedModel2
      || varPtr == vSurfaceFuelLoadDead1
      || varPtr == vSurfaceFuelLoadDead10
      || varPtr == vSurfaceFuelLoadDead100
      || varPtr == vSurfaceFuelLoadLiveHerb
      || varPtr == vSurfaceFuelLoadLiveWood
	  || varPtr == vSurfaceFuelLoadTransferEq
	  || varPtr == vSiteRidgeToValleyElev
      || varPtr == vIgnitionLightningFuelType
      || varPtr == vSafetyZonePersonnelNumber
      || varPtr == vSafetyZoneEquipmentNumber
      || varPtr == vTreeCanopyCover
      || prop->boolean( "mortalityModuleActive" )
      || prop->boolean( "surfaceConfFuelAspen" )
      || prop->boolean( "spotModuleActive" ) )
    {
        maskInputs();
    }
}

//------------------------------------------------------------------------------
/*! \brief Marks unneeded fuel moistures, savr's, and heats as "masked"
 *  if there is no corresponding fuel loading.
 *
 *  Called only by BpDocument::regenerateWorksheet(),
 *  BpDocument::fuelClicked(), BpDocument::runWorksheetValidation(),
 *  or by EqCalc::maskInputs( EqVar* ).
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::maskInputs( void )
{
    // Mask all the dependent moisture inputs.
    vSurfaceFuelMoisDead1000->m_isMasked = true;
    vSurfaceFuelMoisDead100->m_isMasked = true;
    vSurfaceFuelMoisDead10->m_isMasked = true;
    vSurfaceFuelMoisDead1->m_isMasked = true;
    vSurfaceFuelMoisLiveHerb->m_isMasked = true;
    vSurfaceFuelMoisLiveWood->m_isMasked = true;

    vSurfaceFuelMoisLifeDead->m_isMasked = true;
    vSurfaceFuelMoisLifeLive->m_isMasked = true;

    // Mask all the dependent fuel inputs.
    vSurfaceFuelSavrDead1->m_isMasked = true;
    vSurfaceFuelSavrLiveHerb->m_isMasked = true;
    vSurfaceFuelSavrLiveWood->m_isMasked = true;

    vSurfaceFuelHeatDead->m_isMasked = true;
    vSurfaceFuelHeatLive->m_isMasked = true;

    vSurfaceFuelLoadTransferFraction->m_isMasked = true;

    // Mask all the dependent spot inputs.
    vSiteRidgeToValleyDist->m_isMasked = true;
    vSpotFireSource->m_isMasked = true;
	vTreeCanopyCoverDownwind->m_isMasked = true;

    // Mask the lightning fire ignition probability inputs
    vIgnitionLightningDuffDepth->m_isMasked = true;
    vIgnitionLightningFuelMois->m_isMasked = true;

    // Mask the safety zone inputs
    vSafetyZoneEquipmentArea->m_isMasked = true;

    // Mask the WAF inputs
    vTreeCoverHt->m_isMasked = true;
    vTreeCrownRatio->m_isMasked = true;

    // Unmask needed Surface Module inputs.
    PropertyDict *prop = m_eqTree->m_propDict;
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
        if ( prop->boolean( "surfaceConfFuelPalmettoGallberry" ) )
        {
            vSurfaceFuelMoisDead100->m_isMasked = false;
            vSurfaceFuelMoisDead10->m_isMasked = false;
            vSurfaceFuelMoisDead1->m_isMasked = false;
            vSurfaceFuelMoisLiveHerb->m_isMasked = false;
            vSurfaceFuelMoisLiveWood->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            vSurfaceFuelMoisLifeLive->m_isMasked = false;
        }
        else if ( prop->boolean( "surfaceConfFuelAspen" ) )
        {
            vSurfaceFuelMoisDead10->m_isMasked = false;
            vSurfaceFuelMoisDead1->m_isMasked = false;
            vSurfaceFuelMoisLiveHerb->m_isMasked = false;
            vSurfaceFuelMoisLiveWood->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            vSurfaceFuelMoisLifeLive->m_isMasked = false;
        }
        else if ( prop->boolean( "surfaceConfFuelChaparral" ) )
        {
            vSurfaceFuelMoisDead100->m_isMasked = false;
            vSurfaceFuelMoisDead10->m_isMasked = false;
            vSurfaceFuelMoisDead1->m_isMasked = false;
            vSurfaceFuelMoisLiveHerb->m_isMasked = false;
            vSurfaceFuelMoisLiveWood->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            vSurfaceFuelMoisLifeLive->m_isMasked = false;
        }
        else if ( prop->boolean( "surfaceConfFuelModels" ) )
        {
            unmaskFuelModelInputs();
        }
        else if ( prop->boolean( "surfaceConfFuelParms" ) )
        {
            unmaskFuelParmInputs();
        }
        else if ( prop->boolean( "surfaceConfFuelAreaWeighted" )
               || prop->boolean( "surfaceConfFuelHarmonicMean" )
               || prop->boolean( "surfaceConfFuel2Dimensional" ) )
        {
            unmaskFuelModelInputs( true );
        }
        if ( prop->boolean( "surfaceConfWindSpeedAt10MCalc" )
          || prop->boolean( "surfaceConfWindSpeedAt20FtCalc" ) )
        {
            unmaskWafInputs();
        }
    }
    // Unmask needed Crown Module inputs.
    if ( prop->boolean( "crownModuleActive" ) )
    {
        unmaskCrownInputs();
    }
	// Unmask needed Spot Module inputs.
    if ( prop->boolean( "spotModuleActive" ) )
    {
        unmaskSpotInputs();
    }
    // Unmask needed Ignite Module inputs
    if ( prop->boolean( "ignitionModuleActive" )
      && prop->boolean( "ignitionCalcIgnitionLightningProb" ) )
    {
        unmaskIgniteInputs();
    }
    // Unmask needed Safety Module inputs
    if ( prop->boolean( "safetyModuleActive" ) )
    {
        unmaskSafetyInputs();
    }
    // Unmask needed Crown Module inputs
    if ( prop->boolean( "crownModuleActive" ) )
    {
        vSurfaceFuelMoisDead100->m_isMasked = false;
        vSurfaceFuelMoisDead10->m_isMasked = false;
        vSurfaceFuelMoisDead1->m_isMasked = false;
        vSurfaceFuelMoisLiveWood->m_isMasked = false;
        vSurfaceFuelMoisLifeDead->m_isMasked = false;
        vSurfaceFuelMoisLifeLive->m_isMasked = false;
    }
    // Unmask needed Mortality Module inputs.
    if ( prop->boolean( "mortalityModuleActive" ) )
    {
        unmaskMortalityInputs();
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief Unmasks Crown Module required inputs.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskCrownInputs( void )
{
    vTreeCoverHt->m_isMasked = false;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Unmasks required fuel moistures if they have a corresponding load.
 *
 *  \param twoFuelModels If FALSE, checks all fuel models mentioned in the
 *                          vSurfaceFuelBedModel entry field.
 *       If TRUE, checks all fuel models mentioned in both the
 *          vSurfaceFuelBedModel1 and vSurfaceFuelBedModel2 entry fields.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskFuelModelInputs( bool twoFuelModels )
{
	FuelModel *fm;
    QString token;
    Parser parser( " \t,\"", "", "" );
    for ( int model=0; model<2; model++ )
    {
        // If only checking a single fuel model...
        if ( ! twoFuelModels )
        {
            parser.parse( vSurfaceFuelBedModel->m_store );
            // Don't loop through here again.
            model++;
        }
        // Else if checking two fuel models...
        else if ( twoFuelModels )
        {
            if ( model == 0 )
            {
                parser.parse( vSurfaceFuelBedModel1->m_store );
            }
            else if ( model == 1 )
            {
                parser.parse( vSurfaceFuelBedModel2->m_store );
            }
        }
        // Must examine every fuel model in the input entry field.
        // Since this is called at the end of worksheetValidateEntryField(),
        // it ought to contain only valid fuel model codes.
        for ( int i = 0;
              i < parser.tokens();
              i++ )
        {
			fm = 0;
			const char *fmName = "UNKNOWN";		// debug purposes only
			const char *tokenAsc = "NOT SET";	// debug purposes only
            token = parser.token( i );
			tokenAsc = token.ascii();
            // Lookup this fuel model name to get a ptr to its data structure.
            fm = m_eqTree->m_fuelModelList->fuelModelByModelName( token );
			if ( fm )
            {
				fmName = fm->m_name.ascii();	// debug purposes only
                if ( fm->m_load1 > 0. )
                {
                    vSurfaceFuelMoisDead1->m_isMasked = false;
                    vSurfaceFuelMoisLifeDead->m_isMasked = false;
                }
                if ( fm->m_load10 > 0. )
                {
                    vSurfaceFuelMoisDead10->m_isMasked = false;
                    vSurfaceFuelMoisLifeDead->m_isMasked = false;
                }
                if ( fm->m_load100 > 0. )
                {
                    vSurfaceFuelMoisDead100->m_isMasked = false;
                    vSurfaceFuelMoisLifeDead->m_isMasked = false;
                }
                if ( fm->m_loadHerb > 0. )
                {
                    vSurfaceFuelMoisLiveHerb->m_isMasked = false;
                    vSurfaceFuelMoisLifeLive->m_isMasked = false;
                }
                if ( fm->m_loadWood > 0. )
                {
                    vSurfaceFuelMoisLiveWood->m_isMasked = false;
                    vSurfaceFuelMoisLifeLive->m_isMasked = false;
                }
                if ( fm->isDynamic() && fm->m_loadHerb > 0. )
                {
                    vSurfaceFuelLoadTransferFraction->m_isMasked = false;
                    vSurfaceFuelMoisDead1->m_isMasked = false;
                    vSurfaceFuelMoisLifeDead->m_isMasked = false;
                }
			}
			// Warn if loading an unattached custom fuel model.
			else
			{
                QString msg("");
                translate( msg, "EqCalc:UnmaskFuelModelInputs", token );
                warn( msg );
                return;
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Unmasks required fuel moistures, sa/vol ratios, and heats of
 *  combustion if they have a corresponding loading.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskFuelParmInputs( void )
{
    Parser parser( " \t,\"", "", "" );
    QString token;
    int i;

    // Check if there is any 1-h fuel load.
    parser.parse( vSurfaceFuelLoadDead1->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            vSurfaceFuelHeatDead->m_isMasked = false;
            vSurfaceFuelSavrDead1->m_isMasked = false;
            vSurfaceFuelMoisDead1->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            break;
        }
    }
    // Check if there is any 10-h fuel load.
    parser.parse( vSurfaceFuelLoadDead10->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            vSurfaceFuelHeatDead->m_isMasked = false;
            vSurfaceFuelMoisDead10->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            break;
        }
    }
    // Check if there is any 100-h fuel load.
    parser.parse( vSurfaceFuelLoadDead100->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            vSurfaceFuelHeatDead->m_isMasked = false;
            vSurfaceFuelMoisDead100->m_isMasked = false;
            vSurfaceFuelMoisLifeDead->m_isMasked = false;
            break;
        }
    }
    // Check if there is any live herbaceous fuel load.
	bool hasHerbLoad = false;
    parser.parse( vSurfaceFuelLoadLiveHerb->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
			hasHerbLoad = true;
            vSurfaceFuelHeatLive->m_isMasked = false;
            vSurfaceFuelSavrLiveHerb->m_isMasked = false;
            vSurfaceFuelMoisLiveHerb->m_isMasked = false;
            vSurfaceFuelMoisLifeLive->m_isMasked = false;
            break;
        }
    }
    // If there is any live woody fuel load.
    parser.parse( vSurfaceFuelLoadLiveWood->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            vSurfaceFuelHeatLive->m_isMasked = false;
            vSurfaceFuelMoisLiveWood->m_isMasked = false;
            vSurfaceFuelSavrLiveWood->m_isMasked = false;
            vSurfaceFuelMoisLifeLive->m_isMasked = false;
            break;
        }
    }

	// Determine if any of the Fuel Model Type inputs are dynamic,
	bool hasDynamicFuel = false;
    parser.parse( vSurfaceFuelLoadTransferEq->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token == QString( "D" ) || token == QString( "d" ) )
        {
			hasDynamicFuel = true;
            break;
        }
    }

	// If dynamic fuel model with herbaceous fuel,
	// require the 1-h dead fuel savr, heat, and moisture
	if ( hasDynamicFuel && hasHerbLoad )
	{
        //vSurfaceFuelHeatDead->m_isMasked = false;
        //vSurfaceFuelSavrDead1->m_isMasked = false;
        vSurfaceFuelMoisDead1->m_isMasked = false;
        vSurfaceFuelMoisLifeDead->m_isMasked = false;
	}
	
	// If there is a dynamic fuel model type AND an herb load present, unmask the transfer portion.
    vSurfaceFuelLoadTransferFraction->m_isMasked = ( ! hasDynamicFuel || ! hasHerbLoad );

	return;
}

//------------------------------------------------------------------------------
/*! \brief Unmasks vIgnitionLightningDuffDepth and/or
 *  vIgnitionLightningFuelMois if one or more of the vIgnitionLightningFuelType
 *  inputs require them.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskIgniteInputs( void )
{
    Parser parser( " \t,\"", "", "" );
    QString token;
    bool maskDepth = true;
    bool maskMoist = true;

    // Check if there is an ignition lightning fuel type
    parser.parse( vIgnitionLightningFuelType->m_store );
    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i ).upper();
        if ( token == "LPD"
          || token == "DFD" )
        {
            maskDepth = false;
        }
        else if ( token == "PPL"
          || token == "PWC"
          || token == "PWD"
          || token == "PWS"
          || token == "ESH"
          || token == "PMC" )
        {
            maskMoist = false;
        }
    }
    // Unmasked required inputs
    vIgnitionLightningDuffDepth->m_isMasked = maskDepth;
    vIgnitionLightningFuelMois->m_isMasked = maskMoist;
    if ( vSurfaceFuelMoisDead100->m_isMasked )
    {
        vSurfaceFuelMoisDead100->m_isMasked = maskMoist;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Unmasks vTreeCrownRatio if needed.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskMortalityInputs( void )
{
    if ( vTreeCrownVolScorchedAtVector->m_isUserOutput
      || vTreeCrownLengScorchedAtVector->m_isUserOutput
      || vTreeMortalityRateAtVector->m_isUserOutput )
    {
        vTreeCrownRatio->m_isMasked = false;
        vTreeCoverHt->m_isMasked = false;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief If there is a non-zero entry for personnel or equipment number,
 *  their corresponding area is unmasked.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskSafetyInputs( void )
{
    Parser parser( " \t,\"", "", "" );
    QString token;
    int i;

    // Check if there is a non-zero number of equipment
    parser.parse( vSafetyZoneEquipmentNumber->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            vSafetyZoneEquipmentArea->m_isMasked = false;
            break;
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief If EITHER ridge-to-valley elevational difference or
 *  ridge-to-valley horizontal distance are non-zero/non-blank,
 *  then all 3 are unmasked.
 *
 *  If BOTH ridge-to-valley elevational difference and
 *  ridge-to-valley horizontal distance are zero/blank,
 *  then all three are masked, since masking is only advisory.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskSpotInputs( void )
{
    Parser parser( " \t,\"", "", "" );
    QString token;
    int i;
    bool elev = false;
    parser.parse( vSiteRidgeToValleyElev->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            elev = true;
            break;
        }
    }
    // If elevation, then unmask the others
    if ( elev )
    {
        vSpotFireSource->m_isMasked = false;
        vSiteRidgeToValleyDist->m_isMasked = false;
    }
    // Canopy height is always required for SPOT
    vTreeCoverHt->m_isMasked = false;
	// Downwind canopy cover is not needed if downwind canopy ht is zero
    parser.parse( vTreeCoverHtDownwind->m_store );
    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.01 )
        {
            vTreeCanopyCoverDownwind->m_isMasked = false;
        }
	}
    return;
}

//------------------------------------------------------------------------------
/*! \brief Unmasks vTreeCoverHt and vTreeCrownRatio if vTreeCanopyCover > 0.
 *
 *  \note This function merely sets the \a m_isMasked data elements,
 *  and does not actually implement any GUI masks on the worksheet.
 */

void EqCalc::unmaskWafInputs( void )
{
    Parser parser( " \t,\"", "", "" );
    QString token;
    int i;
    bool cover = false;
    parser.parse( vTreeCanopyCover->m_store );
    for ( i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( token.toDouble() >= 0.000001 )
        {
            cover = true;
            break;
        }
    }
    // If canopy cover is > 0, unmask
    //PropertyDict *prop = m_eqTree->m_propDict;
    if ( cover )
    {
        vTreeCoverHt->m_isMasked    = false;
        vTreeCrownRatio->m_isMasked = false;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Checks for implementation-specific conflicts between variables.
 *
 *  This function is called only by EqTree::valiateInputs().
 *
 *  BpDocument::validateWorksheet() handles conflicts that require user
 *  intervention (and the GUI) after determining the conflict by calls to
 *  EqCalc::conflict1() and EqCalc::conflict2().
 *
 *  \return TRUE if valid, FALSE on failure.
 */

bool EqCalc::validateInputs( void ) const
{
    PropertyDict *prop = m_eqTree->m_propDict;

    // Spot Module warnings
    if ( prop->boolean( "spotModuleActive" )
      && prop->boolean( "spotCalcDistSurfaceFire" ) )
    {
        // Warning only
        if ( prop->boolean( "spotCalcDistTorchingTrees" ) )
        {
            QString text("");
            translate( text, "EqCalc:Validate:Conflict1" );
            warn( text );
        }
        // Warning only
        else if ( vTreeHt->m_isUserInput )
        {
            QString text("");
            translate( text, "EqCalc:Validate:Conflict2" );
            warn( text );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of xeqcalmask.cpp
//------------------------------------------------------------------------------

