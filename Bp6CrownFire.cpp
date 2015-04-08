#include "Bp6CrownFire.h"
#include "Bp6FuelModel10.h"
#include <math.h>

//------------------------------------------------------------------------------
Bp6CrownFire::Bp6CrownFire() :
	Bp6SurfaceFire(),
	// Canopy inputs
	m_canopyBaseHeight( 0. ),
	m_canopyBulkDensity( 0. ),
	m_canopyFoliarMois( 5. ),
	m_canopyHeat( 8000. ),
	m_canopyHeight( 0. ),
	m_windSpeedAt20ft( 0. ),
	// Canopy-dependent outputs
	m_activeCrownFireRatio( 0. ),
	m_canopyFuelLoad( 0. ),
	m_canopyHpua( 0. ),
	m_criticalCrownFireRos( 999999999999. ),
	m_criticalSurfaceFireFlame( 999999999999. ),
	m_criticalSurfaceFireFli( 999999999999. ),
	m_crownFireLwRatio( 1. ),
	m_powerWind( 0. ),
	// Surface fire inputs
	m_surfaceFire( 0 ),
	m_surfaceFireFli( 0. ),
	m_surfaceFireHpua( 0. ),
	m_surfaceFireRos( 0. ),
	// Surface fire and canopy dependent outputs
	m_activeCrownFireFlame( 0. ),
	m_activeCrownFireFli( 0. ),
	m_activeCrownFireHpua( 0. ),
	m_criticalSurfaceFireRos( 999999999999. ),
	m_crownFractionBurned( 0. ),
	m_finalFireFlame( 0. ),
	m_finalFireFli( 0. ),
	m_finalFireHpua( 0. ),
	m_finalFireRos( 0. ),
	m_finalFireType( 0 ),
	m_isActiveCrownFire( false ),
	m_isCrownFire( false ),
	m_isPassiveCrownFire( false ),
	m_isPlumeDominated( false ),
	m_isSurfaceFire( true ),
	m_isWindDriven( true ),
	m_fullCrownFireU20( 0. ),
	m_fullCrownFireRos( 0. ),
	m_passiveCrownFireFlame( 0. ),
	m_passiveCrownFireFli( 0. ),
	m_passiveCrownFireHpua( 0. ),
	m_passiveCrownFireRos( 0. ),
	m_powerFire( 0. ),
	m_powerRatio( 0. ),
	m_transRatio( 0. ),
	// updateSiteExtension() outputs
	m_activeCrownFireRos( 0. ),
	// updateTimeExtension() outputs
	m_activeCrownFireArea( 0. ),
	m_activeCrownFireLength( 0. ),
	m_activeCrownFirePerimeter( 0. ),
	m_activeCrownFireWidth( 0. ),
	m_passiveCrownFireArea( 0. ),
	m_passiveCrownFireLength( 0. ),
	m_passiveCrownFirePerimeter( 0. ),
	m_passiveCrownFireWidth( 0. )
{
	resetCanopyInput();
	resetCanopyOutput();
	resetSurfaceFireInput();
	resetSurfaceFireOutput();
	init();
}

//------------------------------------------------------------------------------
Bp6CrownFire::~Bp6CrownFire()
{}

//------------------------------------------------------------------------------
/*!	\brief Returns the fire area (ft2) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireArea() const
{
	return m_activeCrownFireArea;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the flame length (ft) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireFlame() const
{
	return m_activeCrownFireFlame;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the fireline intensity (btu/ft/s) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireFli() const
{
	return m_activeCrownFireFli;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the heat per unit area (btu/ft2) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireHpua() const
{
	return m_activeCrownFireHpua;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the forward spread distance (ft) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireLength() const
{
	return m_activeCrownFireLength;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the fire perimeter (ft) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFirePerimeter() const
{
	return m_activeCrownFirePerimeter;
}
//------------------------------------------------------------------------------
/*!	\brief Returns Rothermel's active crowning ratio (dl).
 */
double Bp6CrownFire::getActiveCrownFireRatio() const
{
	return m_activeCrownFireRatio;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the spread rate (ft/min) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireRos() const
{
	return m_activeCrownFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief Returnsd the fire width (ft) of a potential active crown fire.
 */
double Bp6CrownFire::getActiveCrownFireWidth() const
{
	return m_activeCrownFireWidth;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the crown canopy total fuel load (lb/ft2).
 */
double Bp6CrownFire::getCanopyFuelLoad() const
{
	return m_canopyFuelLoad;
}
//------------------------------------------------------------------------------
/*!	\brief returns the crown canopy heat per unit area (btu/lb).
 */
double Bp6CrownFire::getCanopyHpua() const
{
	return m_canopyHpua;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the crown fire spread rate required to sustain active crowning (ft/min).
 */
double Bp6CrownFire::getCriticalCrownFireRos() const
{
	return m_criticalCrownFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the critical surface fire flame length to initiate crowning (ft).
 */
double Bp6CrownFire::getCriticalSurfaceFireFlame() const
{
	return m_criticalSurfaceFireFlame;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the critical surface fireline intensity to initiate crowning (btu/ft/s).
 */
double Bp6CrownFire::getCriticalSurfaceFireFli() const
{
	return m_criticalSurfaceFireFli;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the critical surface fire spread rate to initiate crown fire (ft/min).
 */
double Bp6CrownFire::getCriticalSurfaceFireRos() const
{
	return m_criticalSurfaceFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the canopy fraction burned by the crown fire (ratio).
*/
double Bp6CrownFire::getCrownFractionBurned() const
{
	return m_crownFractionBurned;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the crown fire ellipse length-to-width ratio (dl).
 */
double Bp6CrownFire::getCrownFireLwRatio() const
{
	return m_crownFireLwRatio;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the final flame length (ft) based upon the final fire type.
 */
double Bp6CrownFire::getFinalFireFlame() const
{
	return m_finalFireFlame;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the final fireline intensity (btu/ft/s) based upon the final fire type.
 */
double Bp6CrownFire::getFinalFireFli() const
{
	return m_finalFireFli;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the final heat per unit area (btu/ft2) based upon the final fire type.
 */
double Bp6CrownFire::getFinalFireHpua() const
{
	return m_finalFireHpua;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the final spread rate (ft/min) based upon the final fire type.
 */
double Bp6CrownFire::getFinalFireRos() const
{
	return m_finalFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the final fire type.
 *	\retval 0 if the final fire type is a surface fire
 *	\retval 1 if the final fire type is a passive (torching) crown fire
 *	\retval 2 if the final fire type is a surface fire that would be an active crown fire if it transitions
 *	\retval 3 if the final fire type is an active crown fire
 */
int Bp6CrownFire::getFinalFireType() const
{
	return m_finalFireType;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the wind speed at 20 ft (ft/min) required to sustain a fully active crown fire.
 */
double Bp6CrownFire::getFullCrownFireU20() const
{
	return m_fullCrownFireU20;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the surface fire spread rate (ft/min) required to sustain a fully active crown fire.
 */
double Bp6CrownFire::getFullCrownFireRos() const
{
	return m_fullCrownFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief returns the fire area (ft2) of a potential passive (torching) crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireArea() const
{
	return m_passiveCrownFireArea;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the flame length (ft) of a potential passive/torching crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireFlame() const
{
	return m_passiveCrownFireFlame;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the fireline intensity (btu/ft/s) of a potential passive/torching crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireFli() const
{
	return m_passiveCrownFireFli;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the heat per unit area (btu/ft2) of a potential passive/torching crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireHpua() const
{
	return m_passiveCrownFireHpua;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the forward spread distance (ft) of a potential passive (torching) crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireLength() const
{
	return m_passiveCrownFireLength;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the fire perimeter (ft) of a potential passive (torching) crown fire.
 */
double Bp6CrownFire::getPassiveCrownFirePerimeter() const
{
	return m_passiveCrownFirePerimeter;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the spread rate (ft/min) of a potential passive (torching) crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireRos() const
{
	return m_passiveCrownFireRos;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the fire width (ft) of a potential passive (torching) crown fire.
 */
double Bp6CrownFire::getPassiveCrownFireWidth() const
{
	return m_passiveCrownFireWidth;
}
//------------------------------------------------------------------------------
/*!	\brief Returns Rothermel's measure of the power of the fire (ft-lb/ft2/s).
 */
double Bp6CrownFire::getPowerFire() const
{
	return m_powerFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns Rothermel's measure of the power of the wind (ft-lb/ft2/s).
 */
double Bp6CrownFire::getPowerWind() const
{
	return m_powerWind;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the ratio of the power-of-the-fire to the power-of-the-wind (dl).
 */
double Bp6CrownFire::getPowerRatio() const
{
	return m_powerRatio;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the ratio of the power-of-the-fire to the power-of-the-wind (dl).
 */
Bp6SurfaceFire* Bp6CrownFire::getSurfaceFire() const
{
	return m_surfaceFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the ratio of actual-to-critical surface fireline intensity (dl).
 */
double Bp6CrownFire::getTransRatio() const
{
	return m_transRatio;
}

//------------------------------------------------------------------------------
/*!	\brief Initializes the base Bp6SurfaceFire instance with Fuel Model 10.
 */
void Bp6CrownFire::init()
{
	// Crown fire model always uses standard fire behavior fuel model 10:
    static double depth     = 1.0;
    static double deadFuelMext = 0.25;
    static int    particles = 4;
    static int    life[4] = {     0,     0,     0,     2 };
    static double load[4] = { 0.138, 0.092, 0.230, 0.092 };
    static double savr[4] = { 2000.,  109.,   30., 1500. };
    static double heat[4] = { 8000., 8000., 8000., 8000. };
    static double dens[4] = {  32.0,  32.0,  32.0,  32.0 };
    static double stot[4] = { .0555, .0555, .0555, .0555 };
    static double seff[4] = { .0100, .0100, .0100, .0100 };
	setFuel( depth, deadFuelMext, particles, life, load, savr, heat, dens, stot, seff );
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the final fire type is an active crown fire.
*/
bool Bp6CrownFire::isActiveCrownFire() const
{
	return m_isActiveCrownFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the final fire type is either a passive (torching)
 *	or an active crown fire (i.e., if it is NOT a surface fire).
*/
bool Bp6CrownFire::isCrownFire() const
{
	return m_isCrownFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the final fire type is a passive (torching) fire.
*/
bool Bp6CrownFire::isPassiveCrownFire() const
{
	return m_isPassiveCrownFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the power of the fire exceeds the power of the wind.
*/
bool Bp6CrownFire::isPlumeDominated() const
{
	return m_isPlumeDominated;
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the final fire type if a surface fire.
*/
bool Bp6CrownFire::isSurfaceFire() const
{
	return m_isSurfaceFire;
}
//------------------------------------------------------------------------------
/*!	\brief Returns TRUE if the power of the wind exceeds the power of the fire.
*/
bool Bp6CrownFire::isWindDriven() const
{
	return m_isWindDriven;
}

//------------------------------------------------------------------------------
/*!	\brief Resets the canopy input member variables to their values.
 */
void Bp6CrownFire::resetCanopyInput()
{
	m_canopyBaseHeight	= 0.;
	m_canopyBulkDensity = 0.;
	m_canopyFoliarMois	= 5.;
	m_canopyHeat		= 8000.;	// 7732.64 = equals 18,000 kJ/kg
	m_canopyHeight		= 0.;
	m_windSpeedAt20ft	= 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Resets all canopy output member variables tp default values.
*/
void Bp6CrownFire::resetCanopyOutput()
{
	m_activeCrownFireRatio		= 0.;
	m_canopyFuelLoad			= 0.;
	m_canopyHpua				= 0.;
	m_criticalCrownFireRos		= m_infinity;
	m_criticalSurfaceFireFlame	= m_infinity;
	m_criticalSurfaceFireFli	= m_infinity;
	m_crownFireLwRatio			= 1.;
	m_powerWind					= 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Hook to derived class extensions called by the base resetSiteOutput() method.
*/
void Bp6CrownFire::resetSiteOutputExtension()
{
	m_activeCrownFireRos = 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Resets the surface fire input member variables to their default values.
 */
void Bp6CrownFire::resetSurfaceFireInput()
{
	m_surfaceFire    = 0;
	m_surfaceFireFli = 0;
	m_surfaceFireHpua = 0.;
	m_surfaceFireRos = 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Resets the surface fire output member variables to their default values.
 */
void Bp6CrownFire::resetSurfaceFireOutput()
{
	m_activeCrownFireFlame	= 0.;
	m_activeCrownFireFli	= 0.;
	m_activeCrownFireHpua	= 0.;

	m_criticalSurfaceFireRos= m_infinity;
	m_crownFractionBurned	= 0.;

	m_finalFireFlame		= 0.;
	m_finalFireFli			= 0.;
	m_finalFireHpua			= 0.;
	m_finalFireRos			= 0.;
	m_finalFireType			= 0;	// 0=surface, 1=passive, 2=conditional crown, 3=crown

	m_fullCrownFireU20		= m_infinity;
	m_fullCrownFireRos		= m_infinity;

	m_isActiveCrownFire		= false;
	m_isCrownFire			= false;
	m_isPassiveCrownFire	= false;
	m_isWindDriven			= false;

	m_passiveCrownFireFlame = 0.;
	m_passiveCrownFireFli	= 0.;
	m_passiveCrownFireHpua	= 0.;
	m_passiveCrownFireRos	= 0.;

	m_powerFire				= 0.;
	m_powerRatio			= 0.;
	m_transRatio			= 0.;
}
//------------------------------------------------------------------------------
/*!	\brief Hook to derived class extensions called by the base resetTimeOutput() method.
*/
void Bp6CrownFire::resetTimeOutputExtension()
{
	m_activeCrownFireArea		= 0.;
	m_activeCrownFireLength		= 0.;
	m_activeCrownFirePerimeter	= 0.;
	m_activeCrownFireWidth		= 0.;

	m_passiveCrownFireArea		= 0.;
	m_passiveCrownFireLength	= 0.;
	m_passiveCrownFirePerimeter = 0.;
	m_passiveCrownFireWidth		= 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Sets all the canopy input variables and then updates all the canopy
 *	dependent output variables.
 */
void Bp6CrownFire::setCanopy(
	double canopyHeight,
	double canopyBaseHeight,
	double canopyBulkDensity,
	double canopyFoliarMoisture,
	double canopyHeat )
{
	resetCanopyInput();
	resetCanopyOutput();
	m_canopyHeight		= canopyHeight;
	m_canopyBaseHeight	= canopyBaseHeight;
	m_canopyBulkDensity = canopyBulkDensity;
	m_canopyFoliarMois  = canopyFoliarMoisture;
	m_canopyHeat		= canopyHeat;
	updateCanopy();
}
//------------------------------------------------------------------------------
/*!	\brief Reimplemented to prevent use from within Bp6CrownFire. Client should
 *	use setWind() instead.
 */
void Bp6CrownFire::setSite(
        double slopeFraction,			// terrain slope (rise/reach)
		double aspect,					// degrees clockwise from north
        double midflameWindSpeed,		// mid-flame wind speed (ft/min)
        double windDirFromUpslope,		// wind heading direction (degrees clockwise from upslope)
		bool   applyWindSpeedLimit )	// if TRUE, apply Rothermel's upper wind speed limit
{
	setWind( midflameWindSpeed / 0.4 );
}

//------------------------------------------------------------------------------
/*!	\brief Sets all the surface fire input variables and then updates all the
 *	surface fire dependent output variables.
 */
void Bp6CrownFire::setSurfaceFire( Bp6SurfaceFire *surfaceFire )
{
	resetSurfaceFireInput();
	resetSurfaceFireOutput();
	m_surfaceFire		= surfaceFire;
	m_surfaceFireFli	= surfaceFire->getFirelineIntensityAtHead();
	m_surfaceFireHpua	= surfaceFire->getHeatPerUnitArea();
	m_surfaceFireRos	= surfaceFire->getSpreadRateAtHead();
	updateSurfaceFire();
}

//------------------------------------------------------------------------------
/*!	\brief Sets all the surface fire input variables and then updates all the
 *	surface fire dependent output variables.
 */
void Bp6CrownFire::setSurfaceFire( double ros, double fli, double hpua )
{
	m_surfaceFire		= 0;
	m_surfaceFireFli	= fli;
	m_surfaceFireHpua	= hpua;
	m_surfaceFireRos	= ros;
	updateSurfaceFire();
}

//------------------------------------------------------------------------------
/*!	\brief Sets the site input variables and then updates all the
 *	site dependent output variables.
 *
 *	This overrides the Bp6SurfaceFire::setSite(), since the crown fire models
 *	treats many of the arguments as constants.
 */
void Bp6CrownFire::setWind(
        double windSpeedAt20Ft )		// mid-flame wind speed (ft/min)
{
	resetSiteOutput();
	m_slopeFraction			= 0.0;
	m_aspect				= 180.;
	m_midflameWindSpeed		= 0.4 * windSpeedAt20Ft;
	m_windDirFromUpslope	= 0.;
	m_applyWindSpeedLimit	= true;
	updateSite();
}

//------------------------------------------------------------------------------
/*!	\brief Updates all canopy-dependent output member data variables.
*/
void Bp6CrownFire::updateCanopy()
{
	// Crown fire length-to-width ratio (dl)
    m_crownFireLwRatio = calcCrownFireLengthToWidth( m_windSpeedAt20ft );
	
	// Canopy fuel load (lb/ft2)
    m_canopyFuelLoad = calcCrownFuelLoad( m_canopyBulkDensity, m_canopyHeight, m_canopyBaseHeight );

	// Canopy heat per unit area (Btu/ft2)
	m_canopyHpua = ( m_activeCrownFireRos > m_smidgen ) ? m_canopyFuelLoad * m_canopyHeat : 0.;

	// Rothermel's critical surface fire intensity (I'initiation) (btu/ft/s)
	m_criticalSurfaceFireFli = calcCriticalSurfaceFireIntensity( m_canopyFoliarMois, m_canopyBaseHeight );

	// Rothermel's critical surface fire flame length (ft) using Byram
    m_criticalSurfaceFireFlame = calcFlameLengthByram( m_criticalSurfaceFireFli );

	// Rothermel's critical crown fire spread rate (R'active) (ft/min)
	m_criticalCrownFireRos = calcCriticalCrownFireSpreadRate( m_canopyBulkDensity );

	// Rothermel's active crowning ratio
	m_activeCrownFireRatio = calcCrownFireActiveRatio( m_activeCrownFireRos, m_criticalCrownFireRos );

	// Rothermel's power of the wind (ft-lb/ft2/s)
    m_powerWind = calcPowerOfWind( m_windSpeedAt20ft, m_activeCrownFireRos );
}

//------------------------------------------------------------------------------
/*!	\brief Called by the base updateSite() method to perform additional
 *	member variable updates for this derived class.
 */
void Bp6CrownFire::updateSiteExtension()
{
	// Rothermel's crown fire ros (no foliar moisture effect)
	m_activeCrownFireRos = 3.34 * m_rosHead;
}

//------------------------------------------------------------------------------
/*!	\brief Updates all surface fire-dependent output member data variables.
*/
void Bp6CrownFire::updateSurfaceFire()
{
	// Active crown fire heat per unit area (combines surface and canopy heat) (Btu/ft2)
	m_activeCrownFireHpua = m_canopyHpua + m_surfaceFireHpua;

	// Active crown fireline intensity (btu/ft/s)
	m_activeCrownFireFli = ( m_activeCrownFireRos / 60. ) * m_activeCrownFireHpua;

	// Active crown fire flame length (ft) - uses Thomas (1963) instead of Byram (1959)
	m_activeCrownFireFlame = calcFlameLengthThomas( m_activeCrownFireFli );

	// Rothermel/s crown fire transition ratio
	m_transRatio = calcTransitionRatio( m_surfaceFireFli, m_criticalSurfaceFireFli );

	// Final fire type
	m_finalFireType = calcFireType( m_transRatio, m_activeCrownFireRatio );
	m_isSurfaceFire = m_finalFireType == 0 || m_finalFireType == 2;
	m_isPassiveCrownFire = m_finalFireType == 1;
	m_isActiveCrownFire = m_finalFireType == 3;
	m_isCrownFire = m_isActiveCrownFire || m_isPassiveCrownFire;

	// Rothermel's power of the fire (ft-lb/ft2/s)
    m_powerFire = calcPowerOfFire( m_activeCrownFireFli );

	// Rothermel's power ratio
	m_powerRatio = ( m_powerWind <= 0. ) ? 0. : ( m_powerFire / m_powerWind );
	m_isWindDriven = ( m_powerRatio < 1. );
	m_isPlumeDominated = ! m_isWindDriven;

	// Scott & Reinhardt's critical surface fire spread rate (R'initiation) (ft/min)
	m_criticalSurfaceFireRos = 
		calcCriticalSurfaceFireSpreadRate( m_criticalSurfaceFireFli, m_surfaceFireHpua );

	// Scott & Reinhardt crown fraction burned
	// This can only be determined if a Bp6SurfaceFire has been provided
	if ( ! m_surfaceFire || m_canopyBulkDensity < m_smidgen )
	{
		m_fullCrownFireU20 = 0.;
		m_fullCrownFireRos = 0.;
		m_crownFractionBurned = 0.;
		m_passiveCrownFireRos = m_surfaceFireRos;
		m_passiveCrownFireHpua = m_surfaceFireHpua;
		m_passiveCrownFireFli =  m_surfaceFireFli;
	}
	else
	{
		m_fullCrownFireU20 =
			calcCrowningIndex( m_canopyBulkDensity, m_totalRxInt, m_rbQig, m_slopeFactor );
		m_fullCrownFireRos =
			m_surfaceFire->getRsa( 0.4 * m_fullCrownFireU20 );
		m_crownFractionBurned =
			calcCrownFractionBurned( m_surfaceFireRos, m_criticalSurfaceFireRos, m_fullCrownFireRos );
		// Scott & Reinhardt torching (passive crown) spread rate, hpua, fireline intensity
		m_passiveCrownFireRos = m_surfaceFireRos
			+ m_crownFractionBurned * ( m_activeCrownFireRos - m_surfaceFireRos );
		m_passiveCrownFireHpua = m_surfaceFireHpua + m_canopyHpua * m_crownFractionBurned;
		m_passiveCrownFireFli =  m_passiveCrownFireHpua * m_passiveCrownFireRos / 60.;
	}

	// Scott & Reinhardt torching (passive) flame length
	m_passiveCrownFireFlame = calcFlameLengthThomas( m_passiveCrownFireFli );

	// Determine final fire behavior
	if ( m_isSurfaceFire )
	{
		m_finalFireRos   = m_surfaceFireRos;
		m_finalFireHpua  = m_surfaceFireHpua;
		m_finalFireFli   = m_surfaceFireFli;
		m_finalFireFlame = calcFlameLengthByram( m_surfaceFireFli );
	}
	else if ( m_isPassiveCrownFire )
	{
		m_finalFireRos   = m_passiveCrownFireRos;
		m_finalFireHpua  = m_passiveCrownFireHpua;
		m_finalFireFli   = m_passiveCrownFireFli;
		m_finalFireFlame = m_passiveCrownFireFlame;
	}
	else if ( m_isActiveCrownFire )
	{
		m_finalFireRos   = m_activeCrownFireRos;
		m_finalFireHpua  = m_activeCrownFireHpua;
		m_finalFireFli   = m_activeCrownFireFli;
		m_finalFireFlame = m_activeCrownFireFlame;
	}
}

//------------------------------------------------------------------------------
/*!	\brief Updates all time-dependent output member variables.
 */
void Bp6CrownFire::updateTimeExtension()
{
	// Rothermel (1991) ignores backing spread rate and distance
	m_activeCrownFireLength = m_elapsed * m_activeCrownFireRos;
	m_passiveCrownFireLength = m_elapsed * m_passiveCrownFireRos;

	m_activeCrownFireWidth  = m_activeCrownFireLength / m_crownFireLwRatio;
	m_passiveCrownFireWidth = m_passiveCrownFireLength / m_crownFireLwRatio;

	// Calculate the crown fire area from Rothermel (1991) equation 11 on page 16
	// (which ignores backing distance).
    m_activeCrownFireArea = calcEllipseArea( m_activeCrownFireLength, m_crownFireLwRatio );
    m_passiveCrownFireArea = calcEllipseArea( m_passiveCrownFireLength, m_crownFireLwRatio );

	// Estimates crown fire perimeter from forward spread distance and
	// as per Rothermel (1991) equation 13 on page 16.
    m_activeCrownFirePerimeter =
		calcEllipsePerimeterRothermel( m_activeCrownFireLength, m_crownFireLwRatio );
    m_passiveCrownFirePerimeter =
		calcEllipsePerimeterRothermel( m_passiveCrownFireLength, m_crownFireLwRatio );

	// Method used for surface fires:	
    m_activeCrownFirePerimeter =
		calcEllipsePerimeter( m_activeCrownFireLength, m_activeCrownFireWidth );
    m_passiveCrownFirePerimeter =
		calcEllipsePerimeter( m_passiveCrownFireLength, m_passiveCrownFireWidth );
}

//------------------------------------------------------------------------------
// Crown fire static library
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*! \brief Calculates the critical crown fire spread rate required to achieve
 *  an active crown fire.
 *	\param canopyBulkDensity Crown canopy bulk density (lb/ft3).
 *	\return Critical crown fire spread rate (ft/min).
 */
double Bp6CrownFire::calcCriticalCrownFireSpreadRate( double canopyBulkDensity ) const
{
    double cbd = 16.0185 * canopyBulkDensity;			// convert to Kg/m3
    double ros = ( cbd <= 0. ) ? 0. : ( 3.0 / cbd );	// m/min
    return 3.28084 * ros;								// return as ft/min
}

//------------------------------------------------------------------------------
/*! \brief Calculates the critical surface fire intensity (I'initiation)
 *	sufficient to initiate a passive or active crown fire.
 *  \param foliarMoisture Canopy foliar moisture content (ratio)
 *	\param crownBaseHeight Crown canopy base height (ft)
 *	\return Critical surface fireline intensity (btu/ft/s) required to initiate
 *	a passive or active crown fire.
 */
double Bp6CrownFire::calcCriticalSurfaceFireIntensity(
	double foliarMoisture,
	double canopyBaseHeight ) const
{
    double fmc = 100. * foliarMoisture;				// convert to percent
    fmc = ( fmc < 30.0 ) ? 30. : fmc;				// constrain to lower limit
    double cbh = 0.3048 * canopyBaseHeight;			// convert to meters
    cbh = ( cbh < 0.1 ) ? 0.1 : cbh;				// contrain to lower limt
    double csfi = pow( ( 0.010 * cbh * ( 460. + 25.9 * fmc ) ), 1.5 ); // (kW/m)
    csfi *= 0.288672;								// convert to Btu/ft/s
	return csfi;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the Scott & Reinhardt critical surface fire spread rate
 *	(R'initiation) sufficient to initiate a passive or active crown fire.
 *	\param criticalSurfaceFireIntensity Critical surface fireline intensity (btu/ft/s) 
 *  \param surfaceFireHpua Surface fire heat per unit area (Btu/ft2)
 *	\return Critical surface fire spread rate (ft/min)
 */
double Bp6CrownFire::calcCriticalSurfaceFireSpreadRate(
		double criticalSurfaceFireIntensity,
		double surfaceFireHpua ) const
{
	// Scott & Reinhardt's critical surface fire spread rate (R'initiation) (ft/min)
	double ros = ( surfaceFireHpua <= 0. ) ? m_infinity
		: ( 60. * criticalSurfaceFireIntensity ) / surfaceFireHpua;
	return ros;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire active ratio (dl).
 *  \param crownSpreadRate      Crown fire spread rate (ft/min).
 *  \param criticalSpreadRate   Critical crown fire spread rate (ft/min).
 *  \return Crown fire active ratio (dl).
 */
double Bp6CrownFire::calcCrownFireActiveRatio(
		double crownSpreadRate,
        double criticalSpreadRate ) const
{
    double ratio = ( criticalSpreadRate < 0. ) ? 0. : ( crownSpreadRate / criticalSpreadRate );
	return ratio;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire length-to-width ratio given the 20-ft
 *  wind speed (Rothermel 1991, Equation 10, p16).
 *  \param windSpeedAt20ft Wind speed at 20-ft (ft/min).
 *  \return Crown fire length-to-width ratio (dl).
 */
double Bp6CrownFire::calcCrownFireLengthToWidth( double windSpeedAt20Ft ) const
{
	double mph = windSpeedAt20Ft / 88.;		// Wind speed must be in mi/h
	double lw = 1. + 0.125 * mph;
	return lw;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fraction burned as per SCott & Reinhardt.
 *  \param surfaceFireRos Actual surface fire spread rate (ft/min).
 *	\param criticalSurfaceFireRos Surface fire spread rate required to
 *		initiate torching/crowning (ft/min).
 *	\param crowningSurfaceFireRos Surface fire spread rate at which the
 *	active crown fire spread rate is fully achieved and the crown fraction
 *	burned is 1.
 *  \return Crown fration burned (dl).
 */
double Bp6CrownFire::calcCrownFractionBurned(
		double surfaceFireRos,
		double criticalSurfaceFireRos,
		double crowningSurfaceFireRos ) const
{
	double num = surfaceFireRos - criticalSurfaceFireRos;
	double den = crowningSurfaceFireRos - criticalSurfaceFireRos;
	double cfb = ( den > m_smidgen ) ? ( num / den ) : 0.;
	cfb = ( cfb > 1. ) ? 1. : cfb;
	cfb = ( cfb < 0. ) ? 0. : cfb;
	return cfb;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire fuel load given the canopy bulk density and canopy height.
 *  \param canopyBulkDensity Canopy bulk density (lb/ft3).
 *  \param canopyHt Canopy height (ft)
 *	\param baseHt Canopy base height (ft)
 *  \return Crown canopy fuel load (lb/ft2).
 */
double Bp6CrownFire::calcCrownFuelLoad(
		double canopyBulkDensity,
		double canopyHt,
		double baseHt ) const
{
    double load = canopyBulkDensity * ( canopyHt - baseHt );
	return load;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the Scott & Reinhardt 'crowning index' (O'active),
 *	the 20-ft wind speed at which the crown canopy becomes fully available
 *	for active fire spread (and the crown fraction burned approaches 1).
 *	\param canopyBulkDensity Crown canopy bulk density (btu/ft3)
 *	\param reactionIntensity Crown fire (fuel model 10) reaction intensity (btu/ft2/min)
 *	\param heatSink Crown fire (fuel model 10) heat sink (btu/ft3)
 *	\param slopeFactor
 *  \return Crowing index, aka O'active (ft/min).
*/
double Bp6CrownFire::calcCrowningIndex(
		double canopyBulkDensity,
		double reactionIntensity,
		double heatSink,
		double slopeFactor ) const
{
	double rhob = 0.5520;							// Fuel model 10 bulk density (lb/ft3)
	double cbd = 16.0185 * canopyBulkDensity;		// Convert from lb/ft3 to kg/m3
	double rxInt = 0.189422 * reactionIntensity;	// Convert from Btu/ft2/min to kW/m2
	// Determine the epsilon * Qig product from the fuel bed rbQig heat sink
	double epsQig = heatSink / rhob;				// Product of eff htg num and heat of preignition
	epsQig *= 2.32779;								// Convert from Btu/lb to kJ/kg
	double numerator = ( 164.8 * epsQig / ( rxInt * cbd ) ) - slopeFactor - 1.;
	double term = numerator / 0.001612;
	// Scott & Reinhardt Eq 20 to derive wind speed at 20-ft that sustains a fully active crown fire
	double oActive = 0.0457 * pow( term, 0.7 );		// m/min
	double fpm = 3.2808 * oActive;					// Convert from m/min to ft/min
	return fpm;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the final fire type.
 *  \param transRatio The ratio of the surface fireline intensity to the
 *		critical surface fireline intensity.
 *	\param activeRatio The ratio of the active crown fire spread rate to the
 *		critical crown fire spread rate
 *  \retval 0 indicates a surface fire with  nor torching or crowning
 *	\retval 1 indicates a passive or torching crown fire
 *	\retval 2 indicates a surface fire that could conditionally transition to ana ctive crown fire
 *	\retval 3 indicates an active crown fire
 */
int Bp6CrownFire::calcFireType(
		double transRatio,
		double activeRatio ) const
{
	int fireType = 0;			// surface fire
    if ( transRatio < 1.0 )
    {
		if ( activeRatio < 1.0 )
			fireType = 0;		// surface fire
		else
			fireType = 2;		// conditiona surface fire
    }
	else // ( transRatio >= 1.0 )
    {
        if ( activeRatio < 1.0 )
        {
            fireType = 1;     // Torching (passive)
        }
        else // if ( activeRatio >= 1.0 )
        {
            fireType = 3;     // Crowning
        }
    }
	return fireType;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire power-of-the-fire.
 *  \param crownFirelineintensity Crown fire fireline intensity (Btu/ft/s).
 *  \return Rothermel's power of the fire (ft-lb/ft2/s).
 */
double Bp6CrownFire::calcPowerOfFire( double crownFirelineIntensity ) const
{
    double powerFire = crownFirelineIntensity / 129.;
	return powerFire;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire power-of-the-wind.
 *  \param windSpeedAt20ft Wind speed at 20-ft (mi/h).
 *  \return Rothermel's power of the wind (ft-lb/ft2/s).
 */
double Bp6CrownFire::calcPowerOfWind(
		double windSpeedAt20ft,
		double spreadRate ) const
{
    double diff = ( windSpeedAt20ft - spreadRate ) / 60.;
    diff = ( diff < 0. ) ? 0. : diff;
    double powerWind = 0.00106 * diff * diff * diff;
	return powerWind;
}
//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire transition ratio.
 *  \param surfaceFireIntensity Actual surface fire fireline intensity (Btu/ft/s).
 *	\param criticalSurfaceFireIntensity Critical surface fire fireline intensity
 *		required to initiate active or passive crowning (Btu/ft/s).
 *  \return Rothermel's power of the wind (ft-lb/ft2/s).
 */
double Bp6CrownFire::calcTransitionRatio(
		double surfaceFireIntensity,
		double criticalSurfaceFireIntensity ) const
{
	double ratio = ( criticalSurfaceFireIntensity <= 0. ) ? 0.
		: ( surfaceFireIntensity / criticalSurfaceFireIntensity );
	return ratio;
}
