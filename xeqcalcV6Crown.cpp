//------------------------------------------------------------------------------
/*! \file xeqcalcV6Crown.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Rothermel (1991) and Scott & Reinhardt (2001) crown fire implementation.
 *
 *  This file implements the interface between the generic EqTree, the specific
 *  set of variables and functions, and the static FBL calculator methods.
 */

// Custom include files
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

//------------------------------------------------------------------------------
/*! \brief V6CrownFireActiveCriticalOpenWIndSpeed [O'active]
 *
 *  Dependent Variables (Outputs)
 *		vCrownFireActiveCritSurfRate (ft/min)
 *
 *  Independent Variables (Inputs)
 *		vTreeCanopyBulkDens (lb/ft3)
 *		vCrownFireActiveCritSurfSpreadRate (ft/min)
 *			as a proxy which derives the following:
 *		m_canopyRxInt
 *		m_canopyRbQig
 *      m_canopySlopeFactor
 */
void EqCalc::V6CrownFireActiveCriticalOpenWindSpeed( void )
{
	logMethod( "V6CrownFireActiveCriticalOpenWindSpeed", 2, 1 );
	// Access current input values
	double cbd = fetch( vTreeCanopyBulkDens );
    // Calculate results
	double oActive = FBL_CrownFireActiveWindSpeed(
		cbd,
		m_canopyRxInt,
		m_canopyRbQig,
		m_canopySlopeFactor );
    // Store results
    store( vCrownFireActiveCritOpenWindSpeed, oActive );
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
	logMethod( "V6CrownFireActiveCriticalSurfaceSpreadRate", 5, 1 );
	// Access current input values
	double ros0 = fetch( vSurfaceFireNoWindRate );
	double phiS = fetch( vSurfaceFireSlopeFactor );
	double windB = fetch( vSurfaceFireWindFactorB );
	double windK = fetch( vSurfaceFireWindFactorK );
	double oActive = fetch( vCrownFireActiveCritOpenWindSpeed );
	double waf = fetch( vWindAdjFactor );
	// Calculate results
	double midflame = waf * oActive;
    double phiW = ( midflame <= 0. ) ? 0.0 : ( windK * pow( midflame, windB ) );
	double criticalRos = ros0 * ( 1. + phiS + phiW );
	// Store results
    store( vCrownFireActiveCritSurfSpreadRate, criticalRos );
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
	logMethod( "V6CrownFireActiveCrown", 1, 1 );
    // Access current input values
    double ratio = fetch( vCrownFireActiveRatio );
    // Calculate results
    int status = ( ratio < 1.0 ) ? 0 : 1;
    // Store results
	vCrownFireActiveCrown->updateItem( status );
    logOutputItem( vCrownFireActiveCrown );
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
	logMethod( "V6CrownFireActiveFireArea", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFireActiveSpreadDist );
    double ratio = fetch( vCrownFireLengthToWidth );
    // Calculate results
    double area = FBL_CrownFireArea( dist, ratio );
    // Store results
    store( vCrownFireActiveFireArea, area );
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
	logMethod( "V6CrownFireActiveLineIntensity", 2, 1 );
    // Access current input values
    double chpua = fetch( vCrownFireActiveHeatPerUnitArea );
    double cros = fetch( vCrownFireActiveSpreadRate );
    // Calculate results
    double cfli = FBL_CrownFireFirelineIntensity( chpua, cros );
    // Store results
    store( vCrownFireActiveFireLineInt, cfli );
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
	logMethod( "V6CrownFireActiveFireLineIntensityFromFlameLength", 1, 1 );
    // Access current input values
    double cfl = fetch( vCrownFireActiveFlameLeng );
    // Calculate results
    double cfli = FBL_CrownFireFirelineIntensityFromFlameLength( cfl );
    // Store results
    store( vCrownFireActiveFireLineInt, cfli );
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
	logMethod( "V6CrownFireActiveFirePerimeter", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFireActiveSpreadDist );
    double ratio = fetch( vCrownFireLengthToWidth );
    // Calculate results
    double perim = FBL_CrownFirePerimeter( dist, ratio );
    // Store results
    store( vCrownFireActiveFirePerimeter, perim );
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
	logMethod( "V6CrownFireActiveFireWidth", 2, 1 );
    // Access current input values
    double length = fetch( vCrownFireActiveSpreadDist );
	double ratio = fetch( vCrownFireLengthToWidth );
	// Calculate results
	double width = FBL_CrownFireWidth( length, ratio );
	// Store results
	store( vCrownFireActiveFireWidth, width );
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
	logMethod( "V6CrownFireActiveFlameLength", 1, 1 );
    // Access current input values
    double cfli = fetch( vCrownFireActiveFireLineInt );
    // Calculate results
    double cfl = FBL_CrownFireFlameLength( cfli );
    // Store results
    store( vCrownFireActiveFlameLeng, cfl );
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
	logMethod( "V6CrownFireActiveHeatPerUnitArea", 2, 1 );
    // Access current input values
    double surfaceHpua = fetch( vSurfaceFireHeatPerUnitArea );
    double canopyHpua = fetch( vCrownFireHeatPerUnitAreaCanopy );
    // Calculate results
    double crownHpua = FBL_CrownFireHeatPerUnitArea( surfaceHpua, canopyHpua );
    // Store results
    store( vCrownFireActiveHeatPerUnitArea, crownHpua );
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
	logMethod( "V6CrownFireActiveRatio", 2, 1 );
	// Access current input values
    double ros  = fetch( vCrownFireActiveSpreadRate );
    double cros = fetch( vCrownFireCritCrownSpreadRate );
    // Calculate results
    double ratio = FBL_CrownFireActiveRatio( ros, cros );
    // Store results
    store( vCrownFireActiveRatio, ratio );
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
	logMethod( "V6CrownFireActiveSpreadMapDist", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFireActiveSpreadDist );
    double scale = fetch( vMapScale );
    // Calculate results
    double map = scale * dist / 5280.;
    // Store results
    store( vCrownFireActiveSpreadMapDist, map );
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
	logMethod( "V6CrownFireActiveSpreadDist", 2, 1 );
    // Access current input values
    double elapsed = fetch( vSurfaceFireElapsedTime );
	double rate = fetch( vCrownFireActiveSpreadRate );
	// Calculate results
	double distance = elapsed * rate;
	// Store results
	store( vCrownFireActiveSpreadDist, distance );
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
	logMethod( "V6CrownFireActiveSpreadRate", 5, 1 );
    // Access current input values
    double mc1      = fetch( vSurfaceFuelMoisDead1 );
    double mc10     = fetch( vSurfaceFuelMoisDead10 );
    double mc100    = fetch( vSurfaceFuelMoisDead100 );
    double mcWood   = fetch( vSurfaceFuelMoisLiveWood );
    double wind20Ft = fetch( vWindSpeedAt20Ft );
    // Calculate results
    //double cros = FBL_CrownFireSpreadRate(
	//	wind20Ft, mc1, mc10, mc100, mcWood, &oActive );
    double mois[4];
    mois[0] = mc1;
    mois[1] = mc10;
    mois[2] = mc100;
    mois[3] = mcWood;
	// Calculate locally so we can get the crown fuel RbQig and RxInt
	Bp6CrownFire cf;
	cf.setMoisture( mois );
	cf.setWindSpeedAt20FtFpm( 88. * wind20Ft );
	double Ractive = cf.getActiveCrownFireRos();
    // Store results
    store( vCrownFireActiveSpreadRate, Ractive );

	// ALSO store these for use by V6CrownFireActiveCriticalOpenWindSpeed( void )
	m_canopyRbQig = cf.getRbQig();
	m_canopyRxInt = cf.getTotalRxInt();
	m_canopySlopeFactor = cf.getSlopeFactor();
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
	logMethod( "V6CrownFireCritCrownSpreadRate", 3, 1 );
    // Access current input values
	// Rsurface, the actual surface fire spread rate (ft/min).
    double Rsurface = fetch( vSurfaceFireSpreadAtHead );
	// R'intiation, the surface fire spread rate required to initiate torching/crowning (ft/min).
	double Rinitiation = fetch( vCrownFireCritSurfSpreadRate );
	// R'sa, the surface fire spread rate at CI and O'active (at which the active crown fire
	// spread rate is fully achieved and the crown fraction burned becomes 1.
    double Rsa = fetch( vCrownFireActiveCritSurfSpreadRate );
   // Calculate results
    double cfb = FBL_CrownFireCanopyFractionBurned(
		Rsurface,
		Rinitiation,
		Rsa );
    // Store results
    store( vCrownFireCanopyFractionBurned, cfb );
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
	logMethod( "V6CrownFireCritCrownSpreadRate", 1, 1 );
    // Access current input values
    double rhob = fetch( vTreeCanopyBulkDens );
    // Calculate results
    double RprimeActive = FBL_CrownFireCriticalCrownFireSpreadRate( rhob );
    // Store results
    store( vCrownFireCritCrownSpreadRate, RprimeActive );
}

//------------------------------------------------------------------------------
/*! \brief V6CrownFireCritSurfFireInt [I'initiation]
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
	logMethod( "V6CrownFireCritSurfFireInt", 2, 1 );
    // Access current input values
    double fmc = fetch( vTreeFoliarMois );
    double cbh = fetch( vTreeCrownBaseHt );
    // Calculate results
    double IprimeInit = FBL_CrownFireCriticalSurfaceFireIntensity( fmc, cbh );
    // Store results
    store( vCrownFireCritSurfFireInt, IprimeInit );
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
	logMethod( "V6CrownFireCritSurfFlameLeng", 1, 1 );
    // Access current input values
    double cfli = fetch( vCrownFireCritSurfFireInt );
    // Calculate results
    double cfl = FBL_CrownFireCriticalSurfaceFlameLength( cfli );
    // Store results
    store( vCrownFireCritSurfFlameLeng, cfl );
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
	logMethod( "V6CrownFireCritSurfSpreadRate", 2, 1 );
    // Access current input values
    double cfli = fetch( vCrownFireCritSurfFireInt );
	double hpua = fetch( vSurfaceFireHeatPerUnitArea );
    // Calculate results
	double RprimeInit = FBL_CrownFireCriticalSurfaceFireSpreadRate( cfli, hpua );
    // Store results
    store( vCrownFireCritSurfSpreadRate, RprimeInit );
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
	logMethod( "V6CrownFireFuelLoad", 3, 1 );
    // Access current input values
    double bulkDensity = fetch( vTreeCanopyBulkDens );
    double canopyHt = fetch( vTreeCoverHt );
	double baseHt = fetch( vTreeCrownBaseHt );
    // Calculate results
    double load  = FBL_CrownFuelLoad( bulkDensity, canopyHt, baseHt );
    // Store results
    store( vCrownFireFuelLoad, load );
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
	logMethod( "V6CrownFireHeatPerUnitAreaCanopy", 1, 1 );
    // Access current input values
    double load = fetch( vCrownFireFuelLoad );
    // Calculate results
	double heat = 8000.;	// OR 18,000 kJ/kg = 7732.64 Btu/lb
    double hpua = FBL_CrownFireHeatPerUnitAreaCanopy( load, 8000. );
    // Store results
    store( vCrownFireHeatPerUnitAreaCanopy, hpua );
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
	logMethod( "V6CrownFireLengthToWidth", 1, 1 );
    // Access current input values
    double wind = fetch( vWindSpeedAt20Ft );
    // Calculate results
    double ratio = FBL_CrownFireLengthToWidthRatio( wind );
    // Store results
    store( vCrownFireLengthToWidth, ratio );
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
	logMethod( "V6CrownFirePassiveFireArea", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFirePassiveSpreadDist );
    double ratio = fetch( vCrownFireLengthToWidth );
    // Calculate results
    double area = FBL_CrownFireArea( dist, ratio );
    // Store results
    store( vCrownFirePassiveFireArea, area );
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
	logMethod( "V6CrownFirePassiveLineIntensity", 2, 1 );
    // Access current input values
    double passiveHpua = fetch( vCrownFirePassiveHeatPerUnitArea );
    double passiveRos = fetch( vCrownFirePassiveSpreadRate );
    // Calculate results
    double passiveFli = FBL_CrownFireFirelineIntensity( passiveHpua, passiveRos );
    // Store results
    store( vCrownFirePassiveFireLineInt, passiveFli );
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
	logMethod( "V6CrownFirePassiveFirePerimeter", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFirePassiveSpreadDist );
    double ratio = fetch( vCrownFireLengthToWidth );
    // Calculate results
    double perim = FBL_CrownFirePerimeter( dist, ratio );
    // Store results
    store( vCrownFirePassiveFirePerimeter, perim );
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
	logMethod( "V6CrownFirePassiveFireWidth", 2, 1 );
    // Access current input values
    double length = fetch( vCrownFirePassiveSpreadDist );
	double ratio  = fetch( vCrownFireLengthToWidth );
	// Calculate results
	double width  = FBL_CrownFireWidth( length, ratio );
	// Store results
	store( vCrownFirePassiveFireWidth, width );
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
	logMethod( "V6CrownFirePassiveFlameLength", 1, 1 );
    // Access current input values
    double cfli = fetch( vCrownFirePassiveFireLineInt );
    // Calculate results
    double cfl  = FBL_CrownFireFlameLength( cfli );
    // Store results
    store( vCrownFirePassiveFlameLeng, cfl );
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
	logMethod( "V6CrownFirePassiveHeatPerUnitArea", 2, 1 );
    // Access current input values
    double surfaceHpua = fetch( vSurfaceFireHeatPerUnitArea );
    double canopyHpua  = fetch( vCrownFireHeatPerUnitAreaCanopy );
	double cfb = fetch( vCrownFireCanopyFractionBurned );
    // Calculate results
    double crownHpua = FBL_CrownFireHeatPerUnitArea( surfaceHpua, cfb*canopyHpua );
    // Store results
    store( vCrownFirePassiveHeatPerUnitArea, crownHpua );
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
	logMethod( "V6CrownFirePassiveSpreadDist", 2, 1 );
    // Access current input values
    double elapsed = fetch( vSurfaceFireElapsedTime );
	double rate = fetch( vCrownFirePassiveSpreadRate );
	// Calculate results
	double distance = elapsed * rate;
	// Store results
	store( vCrownFirePassiveSpreadDist, distance );
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
	logMethod( "V6CrownFirePassiveSpreadMapDist", 2, 1 );
    // Access current input values
    double dist  = fetch( vCrownFirePassiveSpreadDist );
    double scale = fetch( vMapScale );
    // Calculate results
    double map = scale * dist / 5280.;
    // Store results
    store( vCrownFirePassiveSpreadMapDist, map );
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
	logMethod( "V6CrownFirePassiveSpreadRate", 3, 1 );
    // Access current input values
    double Rsurface = fetch( vSurfaceFireSpreadAtHead );
	double Ractive = fetch( vCrownFireActiveSpreadRate );
	double cfb = fetch( vCrownFireCanopyFractionBurned );
	// Calculate results
	double Rfinal = FBL_CrownFirePassiveSpreadRate( Rsurface, Ractive, cfb );
	// Store results
	store( vCrownFirePassiveSpreadRate, Rfinal );
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
	logMethod( "V6CrownFirePowerFire", 1, 1 );
    // Access current input values
    double cfli = fetch( vCrownFireActiveFireLineInt );
    // Calculate results
    double power = FBL_CrownFirePowerOfFire( cfli );
    // Store results
    store( vCrownFirePowerOfFire, power );
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
	logMethod( "V6CrownFirePowerWind", 2, 1 );
    // Access current input values
    double wind = fetch( vWindSpeedAt20Ft );
    double cros = fetch( vCrownFireActiveSpreadRate );
    // Calculate results
	wind = wind * 88.;	// convert from miles/hour to ft/min
    double power = FBL_CrownFirePowerOfWind( wind, cros );
    // Store results
    store( vCrownFirePowerOfWind, power );
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
	logMethod( "V6CrownFirePowerRatio", 2, 1 );
    // Access current input values
    double fire = fetch( vCrownFirePowerOfFire );
    double wind = fetch( vCrownFirePowerOfWind );
    // Calculate results
    double ratio = FBL_CrownFirePowerRatio( fire, wind );
    // Store results
    store( vCrownFirePowerRatio, ratio );
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
	logMethod( "V6CrownFireTransRatioFromFireIntAtVector", 2, 1 );
    // Access current input values
    double cfli  = fetch( vCrownFireCritSurfFireInt );
    double fli   = fetch( vSurfaceFireLineIntAtVector );
    // Calculate results
    double ratio = FBL_CrownFireTransitionRatio( fli, cfli );
    // Store results
    store( vCrownFireTransRatio, ratio );
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
	logMethod( "V6CrownFireTransRatioFromFlameLengAtVector", 2, 1 );
    // Access current input values
    double cfli  = fetch( vCrownFireCritSurfFireInt );
    double fl    = fetch( vSurfaceFireFlameLengAtVector );
    // Calculate results
    double fli   = FBL_SurfaceFireFirelineIntensity( fl );
    double ratio = FBL_CrownFireTransitionRatio( fli, cfli );
    // Store results
    store( vCrownFireTransRatio, ratio );
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
	logMethod( "V6CrownFireTransToCrown", 1, 1 );
    // Access current input values
    double ratio = fetch( vCrownFireTransRatio );
    // Calculate results
    int   status = ( ratio < 1.0 ) ? 0 : 1;
    // Store results
    vCrownFireTransToCrown->updateItem( status );
	logOutputItem( vCrownFireTransToCrown );
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
	logMethod( "V6CrownFireType", 2, 1 );
    // Access current input values
    double activeRatio = fetch( vCrownFireActiveRatio );
    double transRatio  = fetch( vCrownFireTransRatio );
    // Calculate results
    int status = FBL_FireType( transRatio, activeRatio ) ;
    // Store results
    vCrownFireType->updateItem( status );
	logOutputItem( vCrownFireType );
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
	logMethod( "V6CrownFireWindDriven", 1, 1 );
    // Access current input values
    double ratio = fetch( vCrownFirePowerRatio );
    // Calculate results
    int wind_driven = ( ratio > 0.00001 && ratio < 1.0 ) ? 1 : 0;
    // Store results
    vCrownFireWindDriven->updateItem( wind_driven );
	logOutputItem( vCrownFireWindDriven );
}

//------------------------------------------------------------------------------
//  End of xeqcalcV6Crown.cpp
//------------------------------------------------------------------------------
