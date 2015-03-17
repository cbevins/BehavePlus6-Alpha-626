/*
Faith Ann -

This is the method that determines which run option text gets displayed.

*/

//------------------------------------------------------------------------------
/*! \brief Draws the standard components of a new worksheet page.
 *
 *  \param runOpt   Pointer to an array of QString that on return contains
 *                  Run Option Notes text.
 *  \param nOptions On input, contains the size of runOpt array.
 *                  On return, contains number of runOpts.
 */

void BpDocument::runOptions( QString* runOpt, int& nOptions )
{
    // Initialize
    for ( int i=0; i<nOptions; i++ )
    {
        runOpt[i] = "";
    }
    nOptions = 0;
	bool directionOptions = false;

    // Surface module run options
    if ( property()->boolean( "surfaceModuleActive" ) )
    {
        if ( ! property()->boolean( "sizeModuleActive" )
          && ! property()->boolean( "containModuleActive" )
          && ! property()->boolean( "safetyModuleActive" )
          && ! property()->boolean( "spotModuleActive" )
          && ! property()->boolean( "mortalityModuleActive" )
          && ! property()->boolean( "scorchModuleActive" )
          && ! property()->boolean( "surfaceConfFuelPalmettoGallberry" )
          && ! property()->boolean( "surfaceCalcFireCharacteristicsDiagram" )
          && ! property()->boolean( "surfaceCalcFireDist" )
          && ! property()->boolean( "surfaceCalcFireEffWind" )
          && ! property()->boolean( "surfaceCalcFireFlameLeng" )
          && ! property()->boolean( "surfaceCalcFireHeatPerUnitArea" )
          && ! property()->boolean( "surfaceCalcFireLineInt" )
          && ! property()->boolean( "surfaceCalcFireMaxDirDiagram" )
          && ! property()->boolean( "surfaceCalcFireMaxDirFromUpslope" )
          && ! property()->boolean( "surfaceCalcFireReactionInt" )
          && ! property()->boolean( "surfaceCalcFireSpread" )
          && ! property()->boolean( "surfaceCalcFireWindSpeedFlag" )
          && ! property()->boolean( "surfaceCalcFireWindSpeedLimit" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDead1" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDead10" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDeadFoliage" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLitter" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLive1" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLive10" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLiveFoliage" )
          && ! property()->boolean( "surfaceCalcPalmettoBedDepth" )
          && ! property()->boolean( "surfaceCalcWindAdjFactor" )
          && ! property()->boolean( "surfaceCalcWindSpeedAtMidflame" ) )
        {
            // nothing
        }
        else
        {
            // Maximum reliable wind speed.
            if ( property()->boolean( "surfaceConfWindLimitApplied" )
            && ! property()->boolean( "surfaceConfFuelAspen" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:Wind:Limit:Applied" );
            }
            else
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:Wind:Limit:NotApplied" );
            }
			// 2 fuel model options
            if ( property()->boolean( "surfaceConfFuel2Dimensional" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:2Dimensional" );
            }
            else if ( property()->boolean( "surfaceConfFuelHarmonicMean" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:HarmonicMean" );
            }
            else if ( property()->boolean( "surfaceConfFuelAreaWeighted" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:AreaWeighted" );
            }
            else if ( property()->boolean( "surfaceConfFuelPalmettoGallberry" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:PG1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:PG2" );
            }
            else if ( property()->boolean( "surfaceConfFuelAspen" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Aspen1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Aspen2" );
            }
            // Directions Page
            if ( property()->boolean( "surfaceConfSpreadDirHead" ) )	// Option 1 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirHead" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirBack" ) ) // Option 2 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirBack" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirFlank" ) ) // Option 3 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirFlank" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirFireFront" ) ) // Option 4 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirFireFront" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirPointSourcePsi" ) ) // Option 5 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi2" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi3" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirPointSourceBeta" ) ) // Option 6 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta2" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta3" );
			}
			// Deprecated in V6
            else if ( property()->boolean( "surfaceConfSpreadDirMax" ) )
            {
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:MaxSpreadDir" );
                //translate( runOpt[nOptions++],
                //    "BpDocument:Worksheet:RunOptions:SpreadCalculations1" );
                //translate( runOpt[nOptions++],
                //  "BpDocument:Worksheet:RunOptions:SpreadCalculations2" );
            }
			// Deprecated in V6
            else // if ( property()->boolean( "surfaceConfSpreadDirInput" ) )
            {
				directionOptions = true;
                translate( runOpt[nOptions++],
                  "BpDocument:Worksheet:RunOptions:SpecifiedSpreadDir" );
                //translate( runOpt[nOptions++],
                //  "BpDocument:Worksheet:RunOptions:SpreadCalculations1" );
                //translate( runOpt[nOptions++],
				//  "BpDocument:Worksheet:RunOptions:SpreadCalculations2" );
			}
			if ( directionOptions )
			{
                if ( property()->boolean( "surfaceConfWindDirInput" ) )
                {
                    if ( property()->boolean( "surfaceConfDegreesWrtUpslope" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindTo" );
                        }
                    else // if ( property()->boolean( "surfaceConfDegreesWrtNorth" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromNorth" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindFrom" );
                    }
                }
                else // if ( property()->boolean( "surfaceConfWindDirUpslope" ) )
                {
                    if ( property()->boolean( "surfaceConfDegreesWrtUpslope" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindTo" );
                    }
                    else // if ( property()->boolean( "surfaceConfDegreesWrtNorth" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromNorth" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindFrom" );
                    }
                }
			}
            // Linked to Safety Module
            if ( property()->boolean( "safetyModuleActive" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SurfaceSafety1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SurfaceSafety2" );
            }   // End of safetyModuleActive
        }
    } // End surfaceModuleActive

    // Safety Module run options
    if ( property()->boolean( "safetyModuleActive" ) )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:SafetyFlameLeng" );
    }

    // Contain module run options
    if ( property()->boolean( "containModuleActive" ) )
    {
        // Single or Multiple resources option
        if ( property()->boolean( "containConfResourcesSingle" ) )
        {
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Single1" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Single2" );
        }
        else
        {
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple1" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple2" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple3" );
        }
    }

    // Module Dialog - Map distances run option
    if ( property()->boolean( "mapCalcDist" ) )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:CalcMapDist" );
    }

    // If there are no options to display, say so.
    if ( ! nOptions )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:None" );
    }
    return;
}


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
    return ( prop->boolean( "surfaceModuleActive" )
        && ! prop->boolean( "surfaceConfSpreadDirMax" )
          && prop->boolean( "spotModuleActive" )
          && prop->boolean( "spotCalcDistSurfaceFire" ) ) ;
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

