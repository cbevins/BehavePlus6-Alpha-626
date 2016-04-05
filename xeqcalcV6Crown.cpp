// Custom include files
#include "Bp6Globals.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqvar.h"
#include "xeqtree.h"
#include "xfblib.h"

// Qt include files
#include <qstring.h>

// Standard include files
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char Margin[] = { "        " };

void EqCalc::V6CrownFireActiveCriticalOpenWindSpeed( void )
{
	double cbd = vTreeCanopyBulkDens->m_nativeValue;
    // Calculate results
	double oActive = FBL_CrownFireActiveWindSpeed(
		cbd, m_canopyRxInt, m_canopyRbQig, m_canopySlopeFactor );
    // Store results
    vCrownFireActiveCritOpenWindSpeed->update( oActive );
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveCriticalSurfaceSpreadRate [R'sa]
 *
 *  Dependent Variables (Outputs)
 *		vCrownFireActiveCritSurfRate (ft/min)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireNoWindRate (ft/min)
 *      vSurfaceFireSlopeFactor (dl)
 *      vSurfaceFireWindFactorB (dl)
 *      vSurfaceFireWindFactorK (dl)
 *		vCrownFireActiveCritOpenWindSpeed (ft/min)
 */
void EqCalc::V6CrownFireActiveCriticalSurfaceSpreadRate( void )
{
	// Access current input values
	double ros0 = vSurfaceFireNoWindRate->m_nativeValue;
	double phiS = vSurfaceFireSlopeFactor->m_nativeValue;
	double windB = vSurfaceFireWindFactorB->m_nativeValue;
	double windK = vSurfaceFireWindFactorK->m_nativeValue;
	double oActive = vCrownFireActiveCritOpenWindSpeed->m_nativeValue;
	double midflame = 0.4 * oActive;
	// Calculate results
    double phiW = ( midflame <= 0. ) ? 0.0 : ( windK * pow( midflame, windB ) );
	double criticalRos = ros0 * ( 1. + phiS + phiW );
	// Store results
    vCrownFireActiveCritSurfSpreadRate->update( criticalRos );
    // Log results
	logMethod( "V6CrownFireActiveCriticalSurfaceSpreadRate", 5, 1 );
	logInput( vSurfaceFireNoWindRate );
	logInput( vSurfaceFireSlopeFactor );
	logInput( vSurfaceFireWindFactorB );
	logInput( vSurfaceFireWindFactorK );
	logInput( vCrownFireActiveCritOpenWindSpeed );
	logOutput( vCrownFireActiveCritSurfSpreadRate );
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveCrown
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveCrown
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveRatio (dl)
 */
void EqCalc::V6CrownFireActiveCrown( void )
{
    // Access current input values
    double ratio = vCrownFireActiveRatio->m_nativeValue;
    // Calculate results
    int   status = ( ratio < 1.0 ) ? 0 : 1;
    // Store results
    vCrownFireActiveCrown->updateItem( status );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveCrown() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveRatio %g %s\n", Margin,
            vCrownFireActiveRatio->m_nativeValue,
            vCrownFireActiveRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveCrown %g %s\n", Margin,
            vCrownFireActiveCrown->m_nativeValue,
            vCrownFireActiveCrown->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFireArea
 *
 *  Rothermel (1991) equation 11 on page 16 (which ignores backing distance).
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFireArea (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ft/ft)
 */
void EqCalc::V6CrownFireActiveFireArea( void )
{
    // Access current input values
    double dist  = vCrownFireActiveSpreadDist->m_nativeValue;
    double ratio = vCrownFireLengthToWidth->m_nativeValue;
    // Calculate results
    double area = FBL_CrownFireArea( dist, ratio );
    // Store results
    vCrownFireActiveFireArea->update( area );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveFireArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireSpreadDist %g %s\n", Margin,
            vCrownFireActiveSpreadDist->m_nativeValue,
            vCrownFireActiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireArea %g %s\n", Margin,
            vCrownFireActiveFireArea->m_nativeValue,
            vCrownFireActiveFireArea->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFireLineIntensity
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFireLineInt (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveHeatPerUnitArea (Btu/ft2)
 *      vCrownFireActiveSpreadRate (ft/min)
 */
void EqCalc::V6CrownFireActiveFireLineIntensity( void )
{
    // Access current input values
    double chpua = vCrownFireActiveHeatPerUnitArea->m_nativeValue;
    double cros = vCrownFireActiveSpreadRate->m_nativeValue;
    // Calculate results
    double cfli = FBL_CrownFireFirelineIntensity( chpua, cros );
    // Store results
    vCrownFireActiveFireLineInt->update( cfli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveLineIntensity() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveHeatPerUnitArea %g %s\n", Margin,
            vCrownFireActiveHeatPerUnitArea->m_nativeValue,
            vCrownFireActiveHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveFireLineInt %g %s\n", Margin,
            vCrownFireActiveFireLineInt->m_nativeValue,
            vCrownFireActiveFireLineInt->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFireLineIntensityFromFlameLength
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFireLineInt (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveFlameLeng (ft)
 */
void EqCalc::V6CrownFireActiveFireLineIntensityFromFlameLength( void )
{
    // Access current input values
    double cfl = vCrownFireActiveFlameLeng->m_nativeValue;
    // Calculate results
    double cfli = FBL_CrownFireFirelineIntensityFromFlameLength( cfl );
    // Store results
    vCrownFireActiveFireLineInt->update( cfli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveFireLineIntensityFromFlameLength() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveFlameLeng %g %s\n", Margin,
            vCrownFireActiveFlameLeng->m_nativeValue,
            vCrownFireActiveFlameLeng->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveFireLineInt %g %s\n", Margin,
            vCrownFireActiveFireLineInt->m_nativeValue,
            vCrownFireActiveFireLineInt->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFirePerimeter
 *
 *  Rothermel (1991) equation 13 on page 16.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFirePerimeter (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ft/ft)
 */
void EqCalc::V6CrownFireActiveFirePerimeter( void )
{
    // Access current input values
    double dist  = vCrownFireActiveSpreadDist->m_nativeValue;
    double ratio = vCrownFireLengthToWidth->m_nativeValue;
    // Calculate results
    double perim = FBL_CrownFirePerimeter( dist, ratio );
    // Store results
    vCrownFireActiveFirePerimeter->update( perim );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveFirePerimeter() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadDist %g %s\n", Margin,
            vCrownFireActiveSpreadDist->m_nativeValue,
            vCrownFireActiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveFirePerimeter %g %s\n", Margin,
            vCrownFireActiveFirePerimeter->m_nativeValue,
            vCrownFireActiveFirePerimeter->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFireWidth
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFireWidth (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ratio)
 */
void EqCalc::V6CrownFireActiveFireWidth( void )
{
    // Access current input values
    double length = vCrownFireActiveSpreadDist->m_nativeValue;
	double ratio = vCrownFireLengthToWidth->m_nativeValue;
	// Calculate results
	double width = FBL_CrownFireWidth( length, ratio );
	// Store results
	vCrownFireActiveFireWidth->update( width );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveFireWidth() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadDist %g %s\n", Margin,
            vCrownFireActiveSpreadDist->m_nativeValue,
            vCrownFireActiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vCrownFireLengthToWidth->m_nativeValue,
            vCrownFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveFireWidth %g %s\n", Margin,
            vCrownFireActiveFireWidth->m_nativeValue,
            vCrownFireActiveFireWidth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveFlameLength
 *
 *	Rothermel (1991) equation 3 page 11, using Thomas (1963).
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveFlameLeng (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveFireLineInt (Btu/ft/s)
 */
void EqCalc::V6CrownFireActiveFlameLength( void )
{
    // Access current input values
    double cfli = vCrownFireActiveFireLineInt->m_nativeValue;
    // Calculate results
    double cfl  = FBL_CrownFireFlameLength( cfli );
    // Store results
    vCrownFireActiveFlameLeng->update( cfl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveFlameLength() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveFireLineInt %g %s\n", Margin,
            vCrownFireActiveFireLineInt->m_nativeValue,
            vCrownFireActiveFireLineInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveFlameLeng %g %s\n", Margin,
            vCrownFireActiveFlameLeng->m_nativeValue,
            vCrownFireActiveFlameLeng->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveHeatPerUnitArea
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveHeatPerUnitArea (Btu/ft2)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireHeatPerUnitAreaCanopy (Btu/ft2)
 *		vSurfaceFireHeatPerUnitArea (Btu/ft2)
 */
void EqCalc::V6CrownFireActiveHeatPerUnitArea( void )
{
    // Access current input values
    double surfaceHpua = vSurfaceFireHeatPerUnitArea->m_nativeValue;
    double canopyHpua  = vCrownFireHeatPerUnitAreaCanopy->m_nativeValue;
    // Calculate results
    double crownHpua = FBL_CrownFireHeatPerUnitArea( surfaceHpua, canopyHpua );
    // Store results
    vCrownFireActiveHeatPerUnitArea->update( crownHpua );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveHeatPerUnitArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireHeatPerUnitAreaCanopy %g %s\n", Margin,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeValue,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveHeatPerUnitArea %g %s\n", Margin,
            vCrownFireActiveHeatPerUnitArea->m_nativeValue,
            vCrownFireActiveHeatPerUnitArea->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveRatio
 *
 * The ratio of the actual crown fire spread rate [Ractive] to the critical
 * crown fire spread rate [R'active] for sustaining a fully active crown fire.
 *
 * The Active Ratio == 1 when Ractive==R'active, which occurs when the open
 * wind speed O'active (the Crowning Index) is achieved, resulting in a
 * theoretical surface fire spread rate of R'sa
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveRatio (dl)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireSpreadRate (ft/min) [Ractive]
 *      vCrownFireCritCrownSpreadRate (ft/min) [R'active]
 */
void EqCalc::V6CrownFireActiveRatio( void )
{
    // Access current input values
    double ros  = vCrownFireActiveSpreadRate->m_nativeValue;
    double cros = vCrownFireCritCrownSpreadRate->m_nativeValue;
    // Calculate results
    double ratio = FBL_CrownFireActiveRatio( ros, cros );
    // Store results
    vCrownFireActiveRatio->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveRatio() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireCritCrownSpreadRate %g %s\n", Margin,
            vCrownFireCritCrownSpreadRate->m_nativeValue,
            vCrownFireCritCrownSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveRatio %g %s\n", Margin,
            vCrownFireActiveRatio->m_nativeValue,
            vCrownFireActiveRatio->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveSpreadMapDist
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveSpreadMapDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadDist (ft)
 *      vMapScale (ratio)
 */
void EqCalc::V6CrownFireActiveSpreadMapDist( void )
{
    // Access current input values
    double dist  = vCrownFireActiveSpreadDist->m_nativeValue;
    double scale = vMapScale->m_nativeValue;
    // Calculate results
    double md    = scale * dist / 5280.;
    // Store results
    vCrownFireActiveSpreadMapDist->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveSpreadMapDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadDist %g %s\n", Margin,
            vCrownFireActiveSpreadDist->m_nativeValue,
            vCrownFireActiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveSpreadMapDist %g %s\n", Margin,
            vCrownFireActiveSpreadMapDist->m_nativeValue,
            vCrownFireActiveSpreadMapDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveSpreadDist
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveSpreadDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadRate (ft/min)
 *      vSurfaceFireElapsedTime ( min )
 */

void EqCalc::V6CrownFireActiveSpreadDist( void )
{
    // Access current input values
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
	double rate = vCrownFireActiveSpreadRate->m_nativeValue;
	// Calculate results
	double distance = elapsed * rate;
	// Store results
	vCrownFireActiveSpreadDist->update( distance );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveSpreadDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveSpreadDist %g %s\n", Margin,
            vCrownFireActiveSpreadDist->m_nativeValue,
            vCrownFireActiveSpreadDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveSpreadRate
 *
 *	Rothermel (1991) factor of 3.34 times faster than predicted for the surface
 *	fire using Fuel Model 10.
 *
 *	Also calculates the Crowning Index [O'active], the open wind speed for
 *  sustaining a fully active crown fire, since it requires knowledge of the
 *  heat sink and crown bulk density parameters for Fuel Model 10.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireActiveSpreadRate (ft/min)	[Ractive]
 *		vCrownFireActiveWindSpeed (ft/min) [O'active]
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisDead1 (lb/lb)
 *      vSurfaceFuelMoisDead10 (lb/lb)
 *      vSurfaceFuelMoisDead100 (lb/lb)
 *      vSurfaceFuelMoisLiveWood (lb/lb)
 *      vWindSpeedAt20Ft (ft/min)
 */
void EqCalc::V6CrownFireActiveSpreadRate( void )
{
    // Access current input values
    double mc1      = vSurfaceFuelMoisDead1->m_nativeValue;
    double mc10     = vSurfaceFuelMoisDead10->m_nativeValue;
    double mc100    = vSurfaceFuelMoisDead100->m_nativeValue;
    double mcWood   = vSurfaceFuelMoisLiveWood->m_nativeValue;
    double wind20Ft = vWindSpeedAt20Ft->m_nativeValue;

#ifdef INCLUDE_V5_CODE
    // Calculate results
    //double cros = FBL_CrownFireSpreadRate(
	//	wind20Ft, mc1, mc10, mc100, mcWood, &oActive );
    double mois[4];
    mois[0] = mc1;
    mois[1] = mc10;
    mois[2] = mc100;
    mois[3] = mcWood;
	Bp6CrownFire cf;
	cf.setMoisture( mois );
	cf.setWindSpeedAt20FtFpm( 88. * wind20Ft );
	double Ractive = cf.getActiveCrownFireRos();

    // Store results
    vCrownFireActiveSpreadRate->update( Ractive );

	// Store these for use by V6CrownFireActiveCriticalOpenWindSpeed( void )
	m_canopyRbQig = cf.getRbQig();
	m_canopyRxInt = cf.getTotalRxInt();
	m_canopySlopeFactor = cf.getSlopeFactor();
#elif INCLUDE_V6_CODE

	// V6 Refactor
	//--------------------------------------------------------------------------
	// Bp6CrownFire::setMoisture()
	//--------------------------------------------------------------------------
	// collect the moisture inputs
    double mois[4];
    mois[0] = mc1;
    mois[1] = mc10;
    mois[2] = mc100;
    mois[3] = mcWood;
	// submit the moisture inputs
	m_Bp6CrownFire->setMoisture( mois );

	//--------------------------------------------------------------------------
	// Bp6CrownFire::setWind()
	//--------------------------------------------------------------------------
	// submit the wind inputs
	m_Bp6CrownFire->setWindSpeedAt20FtFpm( 88. * wind20Ft );
	// collect the outputs
	double activeRos = m_Bp6CrownFire->getActiveCrownFireRos();
	double oActive = m_Bp6CrownFire->getFullCrownFireU20();
	// Store the results
    vCrownFireActiveSpreadRate->update( activeRos );
    vCrownFireActiveCritOpenWindSpeed->update( oActive );

	//--------------------------------------------------------------------------
	// Bp6CrownFire::setCanopy()
	//--------------------------------------------------------------------------
	// collect the canopy inputs
    double canopyBulk = vTreeCanopyBulkDens->m_nativeValue;
    double canopyHt   = vTreeCoverHt->m_nativeValue;
	double canopyBase = vTreeCrownBaseHt->m_nativeValue;
    double foliarMc   = vTreeFoliarMois->m_nativeValue;
	// submit the canopy inputs
	m_Bp6CrownFire->setCanopy( canopyHt, canopyBase, canopyBulk, foliarMc );
	// collect the outputs
	double activeRatio		= m_Bp6CrownFire->getActiveCrownFireRatio();
	double canopyFuelLoad	= m_Bp6CrownFire->getCanopyFuelLoad();
	double canopyHpua		= m_Bp6CrownFire->getCanopyHpua();
	double critCrownRos		= m_Bp6CrownFire->getCriticalCrownFireRos();
	double critSurfFlame	= m_Bp6CrownFire->getCriticalSurfaceFireFlame();
	double critSurfFli		= m_Bp6CrownFire->getCriticalSurfaceFireFli();
	double crownLwRatio		= m_Bp6CrownFire->getCrownFireLwRatio();
	double powerWind		= m_Bp6CrownFire->getPowerWind();
	// Store the results
    vCrownFireActiveRatio->update( activeRatio );
    vCrownFireFuelLoad->update( canopyFuelLoad );
    vCrownFireHeatPerUnitAreaCanopy->update( canopyHpua );
    vCrownFireCritCrownSpreadRate->update( critCrownRos );
    vCrownFireCritSurfFlameLeng->update( critSurfFlame );
    vCrownFireCritSurfFireInt->update( critSurfFli );
	vCrownFireLengthToWidth->update( crownLwRatio );
    vCrownFirePowerOfWind->update( powerWind );

	//--------------------------------------------------------------------------
	// Bp6CrownFire::setSurfaceFire()
	//--------------------------------------------------------------------------
	// If SURFACE module is active, provide its spread and heat
    PropertyDict *prop = m_eqTree->m_propDict;
	if ( prop->boolean( "surfaceModuleActive" ) )
	{
		// submit the Bp6SurfaceFire object
		m_Bp6CrownFire->setSurfaceFire( m_Bp6SurfaceFire );
	}
	// Otherwise get the user input values for ros, fli, hpua
	else
	{
		// collect the surface fire inputs
        double surfaceRos  = vSurfaceFireSpreadAtHead->m_nativeValue;
		double surfaceHpua = vSurfaceFireHeatPerUnitArea->m_nativeValue;
		double surfaceFli = 0.;
		if ( prop->boolean( "crownConfUseFlameLeng" ) )
		{
			double surfaceFlame = vSurfaceFireFlameLengAtVector->m_nativeValue;
			surfaceFli = FBL_SurfaceFireFirelineIntensity( surfaceFlame );
		}
		else // if ( prop->boolean( "crownConfUseFlameLeng" )
		{
			surfaceFli = vSurfaceFireLineIntAtVector->m_nativeValue;
		}
		// submit the surface fire inputs
		m_Bp6CrownFire->setSurfaceFire( surfaceRos, surfaceFli, surfaceHpua );
	}
	// collect outputs
	double activeFlame	= m_Bp6CrownFire->getActiveCrownFireFlame();
	double activeFli	= m_Bp6CrownFire->getActiveCrownFireFli();
	double activeHpua	= m_Bp6CrownFire->getActiveCrownFireHpua();
	double critSurfRos	= m_Bp6CrownFire->getCriticalSurfaceFireRos();
	double cfb			= m_Bp6CrownFire->getCrownFractionBurned();
	double finalFlame	= m_Bp6CrownFire->getFinalFireFlame();
	double finalFli		= m_Bp6CrownFire->getFinalFireFli();
	double finalHpua	= m_Bp6CrownFire->getFinalFireHpua();
	double finalRos		= m_Bp6CrownFire->getFinalFireRos();
	int    finalFireType= m_Bp6CrownFire->getFinalFireType();
	bool   isActiveFire = m_Bp6CrownFire->isActiveCrownFire();
	bool   isCrownFire	= m_Bp6CrownFire->isCrownFire();
	bool   isPassiveFire= m_Bp6CrownFire->isPassiveCrownFire();
	bool   isSurfaceFire= m_Bp6CrownFire->isSurfaceFire();
	bool   isPlumeDom	= m_Bp6CrownFire->isPlumeDominated();
	bool   isWindDriven = m_Bp6CrownFire->isWindDriven();
	double fullCrownU20 = m_Bp6CrownFire->getFullCrownFireU20();
	double fullCrownRos = m_Bp6CrownFire->getFullCrownFireRos();
	double passiveFlame = m_Bp6CrownFire->getPassiveCrownFireFlame();
	double passiveFli	= m_Bp6CrownFire->getPassiveCrownFireFli();
	double passiveHpua	= m_Bp6CrownFire->getPassiveCrownFireHpua();
	double passiveRos	= m_Bp6CrownFire->getPassiveCrownFireRos();
	double powerFire	= m_Bp6CrownFire->getPowerFire();
	double powerRatio	= m_Bp6CrownFire->getPowerRatio();
	double transRatio	= m_Bp6CrownFire->getTransRatio();
	// NOTE - when not linked to Surface Module, we cannot calculate
	// many of the Scott & Reinhardt variables, which are within
	// Bp6CrownFire to:
	//  - vCrownFireActiveCritOpenWindSpeed = 0
	//  - vCrownFireActiveCritSurfSpreadRate = 0
	//	- vCrownFireCanopyFractionBurned = 0
	// and all passive crown fire values default to surface fire values
	//	- vCrownFirePassiveFlameLeng = vSurfaceFireFlameLeng
	//	- vCrownFirePassiveHeatPerUnitArea = vSurfaceFireHeatPerUnitArea
	//	- vCrownFirePassiveFireLineInt = vSurfaceFireLineInt
	//	- vCrownFirePassiveSpreadRate = vSurfaceFireSpreadAtHead

	// Store the results
	vCrownFireCanopyFractionBurned->update( cfb );
    vCrownFireCritSurfSpreadRate->update( critSurfRos );

	vCrownFireActiveCritSurfSpreadRate->update( fullCrownRos );
	vCrownFireActiveCritOpenWindSpeed->update( fullCrownU20 );

	vCrownFireTransRatio->update( transRatio );
	vCrownFireTransToCrown->updateItem( isCrownFire ? 1 : 0 );
	vCrownFireActiveCrown->updateItem( isActiveFire ? 1 : 0 );
    vCrownFireType->updateItem( finalFireType );
	vCrownFireWindDriven->updateItem( isWindDriven ? 1 : 0 );

	vCrownFirePowerOfFire->update( powerFire );
    vCrownFirePowerRatio->update( powerRatio );

    vCrownFireActiveFlameLeng->update( activeFlame );
    vCrownFireActiveHeatPerUnitArea->update( activeHpua );
    vCrownFireActiveFireLineInt->update( activeFli );

	vCrownFirePassiveFlameLeng->update( passiveFlame );
	vCrownFirePassiveHeatPerUnitArea->update( passiveHpua );
	vCrownFirePassiveFireLineInt->update( passiveFli );
	vCrownFirePassiveSpreadRate->update( passiveRos );
#endif

	// Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireActiveSpreadRate() 5 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead10 %g %s\n", Margin,
            vSurfaceFuelMoisDead10->m_nativeValue,
            vSurfaceFuelMoisDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLiveWood %g %s\n", Margin,
            vSurfaceFuelMoisLiveWood->m_nativeValue,
            vSurfaceFuelMoisLiveWood->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireActiveCritOpenWindSpeed %g %s\n", Margin,
            vCrownFireActiveCritOpenWindSpeed->m_nativeValue,
            vCrownFireActiveCritOpenWindSpeed->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCanopyFractionBurned
 *
 *  Calculates the crown fraction burned as per Scott & Reinhardt (2001)
 *	equation 28 on page 41.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireCanopyFractionBurned (fraction)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireSpreadAtHead (ft/min) [Rsurface]
 *		vCrownFireCritSurfSpreadRate (ft/min) [R'initiation]
 *		vCrownFireActiveCritSurfSpreadRate (ft/min) [R'sa]
 */
void EqCalc::V6CrownFireCanopyFractionBurned( void )
{
    // Access current input values
	// Rsurface, the actual surface fire spread rate (ft/min).
    double ros = vSurfaceFireSpreadAtHead->m_nativeValue;
	// R'intiation, the surface fire spread rate required to initiate torching/crowning (ft/min).
	double rInitiation = vCrownFireCritSurfSpreadRate->m_nativeValue;
	// R'sa, the surface fire spread rate at CI and O'active (at which the active crown fire
	// spread rate is fully achieved and the crown fraction burned becomes 1.
    double rsa = vCrownFireActiveCritSurfSpreadRate->m_nativeValue;
   // Calculate results
    double cfb = FBL_CrownFireCanopyFractionBurned( ros, rInitiation, rsa );
    // Store results
    vCrownFireCanopyFractionBurned->update( cfb );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireCritCrownSpreadRate() 1 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCanopyBulkDens %g %s\n", Margin,
            vTreeCanopyBulkDens->m_nativeValue,
            vTreeCanopyBulkDens->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireCritCrownSpreadRate %g %s\n", Margin,
            vCrownFireCritCrownSpreadRate->m_nativeValue,
            vCrownFireCritCrownSpreadRate->m_nativeUnits.latin1() );
    }
    return;

}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCritCrownSpreadRate [R'active]
 *
 *	Calculates the critical crown fire spread rate [R'active] for
 *	sustaining an active crown fire.
 *
 *	See Scott & Reinhardt (2001) equation 14 on page 14.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireCritCrownSpreadRate (ft/min) [R'active]
 *
 *  Independent Variables (Inputs)
 *      vTreeCanopyBulkDens (lb/ft3)
 */
void EqCalc::V6CrownFireCritCrownSpreadRate( void )
{
    // Access current input values
    double rhob = vTreeCanopyBulkDens->m_nativeValue;
    // Calculate results
    double cros = FBL_CrownFireCriticalCrownFireSpreadRate( rhob );
    // Store results
    vCrownFireCritCrownSpreadRate->update( cros );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireCritCrownSpreadRate() 1 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCanopyBulkDens %g %s\n", Margin,
            vTreeCanopyBulkDens->m_nativeValue,
            vTreeCanopyBulkDens->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireCritCrownSpreadRate %g %s\n", Margin,
            vCrownFireCritCrownSpreadRate->m_nativeValue,
            vCrownFireCritCrownSpreadRate->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCritSurfFireInt [R'initiation]
 *
 *	Calculates the critical surface fire intensity [I'initiation] for a
 *	surface fire to transition to a passsive or active crown fire.
 *
 *	See Scott & Reinhardt (2001) equation 11 on page 12.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireCritSurfFireInt (Btu/ft/s) [R'initation]
 *
 *  Independent Variables (Inputs)
 *      vTreeCrownBaseHt (ft)
 *      vTreeFoliarMois (lb water/lb fuel)
 */
void EqCalc::V6CrownFireCritSurfFireInt( void )
{
    // Access current input values
    double fmc = vTreeFoliarMois->m_nativeValue;
    double cbh = vTreeCrownBaseHt->m_nativeValue;
    // Calculate results
    double cfli = FBL_CrownFireCriticalSurfaceFireIntensity( fmc, cbh );
    // Store results
    vCrownFireCritSurfFireInt->update( cfli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireCritSurfFireInt() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCrownBaseHt %g %s\n", Margin,
            vTreeCrownBaseHt->m_nativeValue,
            vTreeCrownBaseHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeFoliarMois %g %s\n", Margin,
            vTreeFoliarMois->m_nativeValue,
            vTreeFoliarMois->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireCritSurfFireInt %g %s\n", Margin,
            vCrownFireCritSurfFireInt->m_nativeValue,
            vCrownFireCritSurfFireInt->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCritSurfFlameLeng (for Scott & Reinhardt R'initiation)
 *
 *	Uses Byram's flame length (Rothermel 1991 equation 2 page 11) since we're
 *  estimating the surface fire flame length and not the crown fire flame length.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireCritSurfFlameLeng (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireCritSurfFireInt (Btu/ft/s)
 */
void EqCalc::V6CrownFireCritSurfFlameLeng( void )
{
    // Access current input values
    double cfli = vCrownFireCritSurfFireInt->m_nativeValue;
    // Calculate results
    double cfl  = FBL_CrownFireCriticalSurfaceFlameLength( cfli );
    // Store results
    vCrownFireCritSurfFlameLeng->update( cfl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireCritSurfFlameLeng() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireCritSurfFireInt %g %s\n", Margin,
            vCrownFireCritSurfFireInt->m_nativeValue,
            vCrownFireCritSurfFireInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireCritSurfFlameLeng %g %s\n", Margin,
            vCrownFireCritSurfFlameLeng->m_nativeValue,
            vCrownFireCritSurfFlameLeng->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCritSurfSpreadRate [R'initiation]
 *
 *  Calculates the Scott & Reinhardt (2001) critical surface fire spread rate
 *	[R'initiation] sufficient to initiate a passive or active crown fire.
 *
 *	See Scott & Reinhardt (2001) equation 12 on page 13.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireCritSurfSpreadRate (ft/min) [R'initiation]
 *
 *  Independent Variables (Inputs)
 *      vCrownFireCritSurfFireInt (Btu/ft/s) [I'inititation]
 *		vSurfaceFireHeatPerUnitArea (Btu/ft2)
 */
void EqCalc::V6CrownFireCritSurfSpreadRate( void )
{
    // Access current input values
    double cfli = vCrownFireCritSurfFireInt->m_nativeValue;
	double hpua = vSurfaceFireHeatPerUnitArea->m_nativeValue;
    // Calculate results
	double cros = FBL_CrownFireCriticalSurfaceFireSpreadRate( cfli, hpua );
    // Store results
    vCrownFireCritSurfSpreadRate->update( cros );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireCritSurfSpreadRate() 2 1\n", Margin );
       fprintf( m_log, "%s  i vCrownFireCritSurfFireInt %g %s\n", Margin,
            vCrownFireCritSurfFireInt->m_nativeValue,
            vCrownFireCritSurfFireInt->m_nativeUnits.latin1() );
       fprintf( m_log, "%s  i vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireCritSurfSpreadRate %g %s\n", Margin,
            vCrownFireCritSurfSpreadRate->m_nativeValue,
            vCrownFireCritSurfSpreadRate->m_nativeUnits.latin1() );
     }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireFuelLoad
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireFuelLoad (lb/ft2)
 *
 *  Independent Variables (Inputs)
 *      vTreeCanopyBulkDens (lb/ft3)
 *      vTreeCoverHt (ft)
 *		vTreeCrownBaseHt (ft)
 */
void EqCalc::V6CrownFireFuelLoad( void )
{
    // Access current input values
    double bulkDensity = vTreeCanopyBulkDens->m_nativeValue;
    double canopyHt = vTreeCoverHt->m_nativeValue;
	double baseHt = vTreeCrownBaseHt->m_nativeValue;
    // Calculate results
    double load  = FBL_CrownFuelLoad( bulkDensity, canopyHt, baseHt );
    // Store results
    vCrownFireFuelLoad->update( load );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireFuelLoad() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCanopyBulkDens %g %s\n", Margin,
            vTreeCanopyBulkDens->m_nativeValue,
            vTreeCanopyBulkDens->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHt %g %s\n", Margin,
            vTreeCoverHt->m_nativeValue,
            vTreeCoverHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireFuelLoad %g %s\n", Margin,
            vCrownFireFuelLoad->m_nativeValue,
            vCrownFireFuelLoad->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireHeatPerUnitAreaCanopy
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireHeatPerUnitAreaCanopy (Btu/ft2)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireFuelLoad (lb/ft2)
 */
void EqCalc::V6CrownFireHeatPerUnitAreaCanopy( void )
{
    // Access current input values
    double load = vCrownFireFuelLoad->m_nativeValue;
    // Calculate results
	double heat = 8000.;	// OR 18,000 kJ/kg = 7732.64 Btu/lb
    double hpua = FBL_CrownFireHeatPerUnitAreaCanopy( load, 8000. );
    // Store results
    vCrownFireHeatPerUnitAreaCanopy->update( hpua );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireHeatPerUnitAreaCanopy() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireFuelLoad %g %s\n", Margin,
            vCrownFireFuelLoad->m_nativeValue,
            vCrownFireFuelLoad->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireHeatPerUnitAreaCanopy %g %s\n", Margin,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeValue,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireLengthToWidth
 *
 *  Rothermel (1991) equation 10 on page 16.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireLengthToWidth (ft)
 *
 *  Independent Variables (Inputs)
 *      vWindSpeedAt20Ft (mi/h)
 */
void EqCalc::V6CrownFireLengthToWidth( void )
{
    // Access current input values
    double wind = vWindSpeedAt20Ft->m_nativeValue;
    // Calculate results
    double ratio = FBL_CrownFireLengthToWidthRatio( wind );
    // Store results
    vCrownFireLengthToWidth->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireLengthToWidth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireLengthToWidth %g %s\n", Margin,
            vCrownFireLengthToWidth->m_nativeValue,
            vCrownFireLengthToWidth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrowFirePassiveFireArea
 *
 *  Rothermel (1991) equation 11 on page 16 (which ignores backing distance).
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveFireArea (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ft/ft)
 */
void EqCalc::V6CrownFirePassiveFireArea( void )
{
    // Access current input values
    double dist  = vCrownFirePassiveSpreadDist->m_nativeValue;
    double ratio = vCrownFireLengthToWidth->m_nativeValue;
    // Calculate results
    double area = FBL_CrownFireArea( dist, ratio );
    // Store results
    vCrownFirePassiveFireArea->update( area );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveFireArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireSpreadDist %g %s\n", Margin,
            vCrownFirePassiveSpreadDist->m_nativeValue,
            vCrownFirePassiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireArea %g %s\n", Margin,
            vCrownFirePassiveFireArea->m_nativeValue,
            vCrownFirePassiveFireArea->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveFireLineIntensity
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveFireLineInt (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveHeatPerUnitArea (Btu/ft2)
 *      vCrownFirePassiveSpreadRate (ft/min)
 */
void EqCalc::V6CrownFirePassiveFireLineIntensity( void )
{
    // Access current input values
    double chpua = vCrownFirePassiveHeatPerUnitArea->m_nativeValue;
    double cros = vCrownFirePassiveSpreadRate->m_nativeValue;
    // Calculate results
    double cfli = FBL_CrownFireFirelineIntensity( chpua, cros );
    // Store results
    vCrownFirePassiveFireLineInt->update( cfli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveLineIntensity() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveHeatPerUnitArea %g %s\n", Margin,
            vCrownFirePassiveHeatPerUnitArea->m_nativeValue,
            vCrownFirePassiveHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFirePassiveSpreadRate %g %s\n", Margin,
            vCrownFirePassiveSpreadRate->m_nativeValue,
            vCrownFirePassiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveFireLineInt %g %s\n", Margin,
            vCrownFirePassiveFireLineInt->m_nativeValue,
            vCrownFirePassiveFireLineInt->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveFirePerimeter
 *
 *  Rothermel (1991) equation 13 on page 16 (which ignores backing distance).
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveFirePerimeter (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ft/ft)
 */
void EqCalc::V6CrownFirePassiveFirePerimeter( void )
{
    // Access current input values
    double dist  = vCrownFirePassiveSpreadDist->m_nativeValue;
    double ratio = vCrownFireLengthToWidth->m_nativeValue;
    // Calculate results
    double perim = FBL_CrownFirePerimeter( dist, ratio );
    // Store results
    vCrownFirePassiveFirePerimeter->update( perim );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveFirePerimeter() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveSpreadDist %g %s\n", Margin,
            vCrownFirePassiveSpreadDist->m_nativeValue,
            vCrownFirePassiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveFirePerimeter %g %s\n", Margin,
            vCrownFirePassiveFirePerimeter->m_nativeValue,
            vCrownFirePassiveFirePerimeter->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveFireWidth
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveFireWidth (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *      vCrownFireLengthToWidth (ratio)
 */
void EqCalc::V6CrownFirePassiveFireWidth( void )
{
    // Access current input values
    double length = vCrownFirePassiveSpreadDist->m_nativeValue;
	double ratio  = vCrownFireLengthToWidth->m_nativeValue;
	// Calculate results
	double width  = FBL_CrownFireWidth( length, ratio );
	// Store results
	vCrownFirePassiveFireWidth->update( width );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveFireWidth() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveSpreadDist %g %s\n", Margin,
            vCrownFirePassiveSpreadDist->m_nativeValue,
            vCrownFirePassiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireLengthToWidth %g %s\n", Margin,
            vCrownFireLengthToWidth->m_nativeValue,
            vCrownFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveFireWidth %g %s\n", Margin,
            vCrownFirePassiveFireWidth->m_nativeValue,
            vCrownFirePassiveFireWidth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveFlameLength
 *
 *	Rothermel (1991) equation 3 page 11, using Thomas (1963).
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveFlameLeng (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveFireLineInt (Btu/ft/s)
 */
void EqCalc::V6CrownFirePassiveFlameLength( void )
{
    // Access current input values
    double cfli = vCrownFirePassiveFireLineInt->m_nativeValue;
    // Calculate results
    double cfl  = FBL_CrownFireFlameLength( cfli );
    // Store results
    vCrownFirePassiveFlameLeng->update( cfl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveFlameLength() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveFireLineInt %g %s\n", Margin,
            vCrownFirePassiveFireLineInt->m_nativeValue,
            vCrownFirePassiveFireLineInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveFlameLeng %g %s\n", Margin,
            vCrownFirePassiveFlameLeng->m_nativeValue,
            vCrownFirePassiveFlameLeng->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveHeatPerUnitArea
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveHeatPerUnitArea (Btu/ft2)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireHeatPerUnitAreaCanopy (Btu/ft2)
 *		vSurfaceFireHeatPerUnitArea (Btu/ft2)
 */
void EqCalc::V6CrownFirePassiveHeatPerUnitArea( void )
{
    // Access current input values
    double surfaceHpua = vSurfaceFireHeatPerUnitArea->m_nativeValue;
    double canopyHpua  = vCrownFireHeatPerUnitAreaCanopy->m_nativeValue;
    // Calculate results
    double crownHpua = FBL_CrownFireHeatPerUnitArea( surfaceHpua, canopyHpua );
    // Store results
    vCrownFirePassiveHeatPerUnitArea->update( crownHpua );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveHeatPerUnitArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireHeatPerUnitAreaCanopy %g %s\n", Margin,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeValue,
            vCrownFireHeatPerUnitAreaCanopy->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveHeatPerUnitArea %g %s\n", Margin,
            vCrownFirePassiveHeatPerUnitArea->m_nativeValue,
            vCrownFirePassiveHeatPerUnitArea->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveSpreadDist
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveSpreadRate (ft/min)
 *      vSurfaceFireElapsedTime ( min )
 */
void EqCalc::V6CrownFirePassiveSpreadDist( void )
{
    // Access current input values
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
	double rate = vCrownFirePassiveSpreadRate->m_nativeValue;
	// Calculate results
	double distance = elapsed * rate;
	// Store results
	vCrownFirePassiveSpreadDist->update( distance );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveSpreadDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveSpreadRate %g %s\n", Margin,
            vCrownFirePassiveSpreadRate->m_nativeValue,
            vCrownFirePassiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveSpreadDist %g %s\n", Margin,
            vCrownFirePassiveSpreadDist->m_nativeValue,
            vCrownFirePassiveSpreadDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveSpreadMapDist
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveSpreadMapDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *      vMapScale (ratio)
 */
void EqCalc::V6CrownFirePassiveSpreadMapDist( void )
{
    // Access current input values
    double dist  = vCrownFirePassiveSpreadDist->m_nativeValue;
    double scale = vMapScale->m_nativeValue;
    // Calculate results
    double md    = scale * dist / 5280.;
    // Store results
    vCrownFirePassiveSpreadMapDist->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveSpreadMapDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePassiveSpreadDist %g %s\n", Margin,
            vCrownFirePassiveSpreadDist->m_nativeValue,
            vCrownFirePassiveSpreadDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveSpreadMapDist %g %s\n", Margin,
            vCrownFirePassiveSpreadMapDist->m_nativeValue,
            vCrownFirePassiveSpreadMapDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePassiveSpreadRate
 *
 *  Calculates the Scott & Reinhardt 'final' crown fire spread rate (ft/min).
 *
 *	Scott & Reinhardt (2001) equation 21 on page 19.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePassiveSpreadDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vCrownFireActiveSpreadRate (ft/min)
 *      vCrownFireCanopyFractionBurned (fraction)
 */
void EqCalc::V6CrownFirePassiveSpreadRate( void )
{
    // Access current input values
    double Rsurface = vSurfaceFireSpreadAtHead->m_nativeValue;
	double Ractive = vCrownFireActiveSpreadRate->m_nativeValue;
	double cfb = vCrownFireCanopyFractionBurned->m_nativeValue;
	// Calculate results
	double Rfinal = FBL_CrownFirePassiveSpreadRate( Rsurface, Ractive, cfb );
	// Store results
	vCrownFirePassiveSpreadRate->update( Rfinal );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePassiveSpreadDist() 3 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireCanopyFractionBurned %g %s\n", Margin,
            vCrownFireCanopyFractionBurned->m_nativeValue,
            vCrownFireCanopyFractionBurned->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePassiveSpreadRate %g %s\n", Margin,
            vCrownFirePassiveSpreadRate->m_nativeValue,
            vCrownFirePassiveSpreadRate->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePowerOfFire
 *
 *	Rothermel (1991) equation 6 on page 14.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePowerOfFire (ft-lb/s/ft2)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveFireLineInt (Btu/ft/s)
 */
void EqCalc::V6CrownFirePowerOfFire( void )
{
    // Access current input values
    double cfli = vCrownFireActiveFireLineInt->m_nativeValue;
    // Calculate results
    double power = FBL_CrownFirePowerOfFire( cfli );
    // Store results
    vCrownFirePowerOfFire->update( power );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePowerFire() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveFireLineInt %g %s\n", Margin,
            vCrownFireActiveFireLineInt->m_nativeValue,
            vCrownFireActiveFireLineInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePowerOfFire %g %s\n", Margin,
            vCrownFirePowerOfFire->m_nativeValue,
            vCrownFirePowerOfFire->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePowerOfWind
 *
 *	Rothermel (1991) equation 7 on page 14.
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePowerOfWind (ft-lb/s/ft2)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveSpreadRate (ft/min)
 *      vWindSpeedAt20Ft (ft/min)
 */
void EqCalc::V6CrownFirePowerOfWind( void )
{
    // Access current input values
    double wind = vWindSpeedAt20Ft->m_nativeValue;
    double cros = vCrownFireActiveSpreadRate->m_nativeValue;
    // Calculate results
	wind = wind * 5280. / 60.;	// convert from miles/hour to ft/min
    double power = FBL_CrownFirePowerOfWind( wind, cros );
    // Store results
    vCrownFirePowerOfWind->update( power );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePowerWind() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireActiveSpreadRate %g %s\n", Margin,
            vCrownFireActiveSpreadRate->m_nativeValue,
            vCrownFireActiveSpreadRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePowerOfWind %g %s\n", Margin,
            vCrownFirePowerOfWind->m_nativeValue,
            vCrownFirePowerOfWind->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFirePowerRatio
 *
 *  Dependent Variables (Outputs)
 *      vCrownFirePowerRatio (ratio)
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePowerOfFire (ft-lb/s/ft2)
 *      vCrownFirePowerOfWind (ft-lb/s/ft2)
 */
void EqCalc::V6CrownFirePowerRatio( void )
{
    // Access current input values
    double fire = vCrownFirePowerOfFire->m_nativeValue;
    double wind = vCrownFirePowerOfWind->m_nativeValue;
    // Calculate results
    double ratio = FBL_CrownFirePowerRatio( fire, wind );
    // Store results
    vCrownFirePowerRatio->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFirePowerRatio() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePowerOfFire %g %s\n", Margin,
            vCrownFirePowerOfFire->m_nativeValue,
            vCrownFirePowerOfFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFirePowerOfWind %g %s\n", Margin,
            vCrownFirePowerOfWind->m_nativeValue,
            vCrownFirePowerOfWind->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFirePowerRatio %g %s\n", Margin,
            vCrownFirePowerRatio->m_nativeValue,
            vCrownFirePowerRatio->m_nativeUnits.latin1() );
    }
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireTransRatioFromFireIntAtVector
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireTransRatio (dl)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 *      vCrownFireCritSurfFireInt (Btu/ft/s)
 */
void EqCalc::V6CrownFireTransRatioFromFireIntAtVector( void )
{
    // Access current input values
    double cfli  = vCrownFireCritSurfFireInt->m_nativeValue;
    double fli   = vSurfaceFireLineIntAtVector->m_nativeValue;
    // Calculate results
    double ratio = FBL_CrownFireTransitionRatio( fli, cfli );
    // Store results
    vCrownFireTransRatio->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireTransRatioFromFireIntAtVector() 2 1\n", Margin,
            vCrownFireCritSurfFireInt->m_nativeValue,
            vCrownFireCritSurfFireInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireTransRatio %g %s\n", Margin,
            vCrownFireTransRatio->m_nativeValue,
            vCrownFireTransRatio->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireTransRatioFromFlameLengAtVector
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireTransRatio (dl)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireFlameLengAtVector (ft)
 *      vCrownFireCritSurfFireInt (Btu/ft/s)
 */
void EqCalc::V6CrownFireTransRatioFromFlameLengAtVector( void )
{
    // Access current input values
    double cfli  = vCrownFireCritSurfFireInt->m_nativeValue;
    double fl    = vSurfaceFireFlameLengAtVector->m_nativeValue;
    // Calculate results
    double fli   = FBL_SurfaceFireFirelineIntensity( fl );
    double ratio = FBL_CrownFireTransitionRatio( fli, cfli );
    // Store results
    vCrownFireTransRatio->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireTransRatioFromFlameLengAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireCritSurfFireInt %g %s\n", Margin,
            vCrownFireCritSurfFireInt->m_nativeValue,
            vCrownFireCritSurfFireInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireTransRatio %g %s\n", Margin,
            vCrownFireTransRatio->m_nativeValue,
            vCrownFireTransRatio->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireTransToCrown
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireArea (ac)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 */
void EqCalc::V6CrownFireTransToCrown( void )
{
    // Access current input values
    double ratio = vCrownFireTransRatio->m_nativeValue;
    // Calculate results
    int   status = ( ratio < 1.0 ) ? 0 : 1;
    // Store results
    vCrownFireTransToCrown->updateItem( status );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireTransToCrown() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireTransRatio %g %s\n", Margin,
            vCrownFireTransRatio->m_nativeValue,
            vCrownFireTransRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireTransToCrown %g %s\n", Margin,
            vCrownFireTransToCrown->m_nativeValue,
            vCrownFireTransToCrown->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireType
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireType (class)
 *
 *  Independent Variables (Inputs)
 *      vCrownFireActiveRatio (dl)
 *      vCrownFireTransRatio (dl)
 */
void EqCalc::V6CrownFireType( void )
{
    // Access current input values
    double activeRatio = vCrownFireActiveRatio->m_nativeValue;
    double transRatio  = vCrownFireTransRatio->m_nativeValue;
    // Calculate results
    int status = FBL_FireType( transRatio, activeRatio ) ;
    // Store results
    vCrownFireType->updateItem( status );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireType() 2 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFireActiveRatio %g %s\n", Margin,
            vCrownFireActiveRatio->m_nativeValue,
            vCrownFireActiveRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireTransRatio %g %s\n", Margin,
            vCrownFireTransRatio->m_nativeValue,
            vCrownFireTransRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireType %g %s\n", Margin,
            vCrownFireType->m_nativeValue,
            vCrownFireType->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireWindDriven
 *
 *  Dependent Variables (Outputs)
 *      vCrownFireWindDriven
 *
 *  Independent Variables (Inputs)
 *      vCrownFirePowerRatio (ratio)
 */
void EqCalc::V6CrownFireWindDriven( void )
{
    // Access current input values
    double ratio = vCrownFirePowerRatio->m_nativeValue;
    // Calculate results
    int wind_driven = ( ratio > 0.00001 && ratio < 1.0 ) ? 1 : 0;
    // Store results
    vCrownFireWindDriven->updateItem( wind_driven );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc CrownFireWindDriven() 1 1\n", Margin );
        fprintf( m_log, "%s  i vCrownFirePowerRatio %g %s\n", Margin,
            vCrownFirePowerRatio->m_nativeValue,
            vCrownFirePowerRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vCrownFireWindDriven %g %s\n", Margin,
            vCrownFireWindDriven->m_nativeValue,
            vCrownFireWindDriven->m_nativeUnits.latin1() );
    }
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
