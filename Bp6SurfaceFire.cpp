#include "Bp6SurfaceFire.h"
#include <math.h>

Bp6SurfaceFire::Bp6SurfaceFire() :
	m_pi( 3.141592654 ),
	m_smidgen( 1.0e-7 ),
	m_infinity( 999999999999. )
{
	resetFuelInput();
	resetFuelOutput();
	resetMoistureInput();
	resetMoistureOutput();
	resetSiteInput();
	resetSiteOutput();
	resetTimeInput();
	resetTimeOutput();
}

//------------------------------------------------------------------------------
Bp6SurfaceFire::~Bp6SurfaceFire()
{}

//------------------------------------------------------------------------------
int Bp6SurfaceFire::fuelLife( int lifeCode )
{
    static int Life[4] =
    {
        // FuelLifeType_DeadTimeLag=0, Dead category, dead time lag size class moisture
        DeadCat,
        // FuelLifeType_LiveHerb=1, Live category, live herbaceous moisture
        LiveCat,
        // FuelLifeType_LiveWood=2, Live category, live woody moisture
        LiveCat,
        // FuelLifeType_DeadLitter=3, Dead category, 100-h time lag moisture
        DeadCat
    };
    return( Life[ lifeCode ] );
}

// Fuel particle member variable accessors
double Bp6SurfaceFire::getDeadMext() const
{
	return m_deadMext;
}
double Bp6SurfaceFire::getDepth() const
{
	return m_depth;
}
int    Bp6SurfaceFire::getParticles() const
{
	return m_particles;
}
double Bp6SurfaceFire::getParticleArea( int particleIndex ) const
{
	return m_area[particleIndex];
}
double Bp6SurfaceFire::getParticleAwtg( int particleIndex ) const
{
	return m_aWtg[particleIndex];
}
double Bp6SurfaceFire::getParticleDens( int particleIndex ) const
{
	return m_dens[particleIndex];
}
double Bp6SurfaceFire::getParticleHeat( int particleIndex ) const
{
	return m_heat[particleIndex];
}
int    Bp6SurfaceFire::getParticleLife( int particleIndex ) const
{
	return m_life[particleIndex];
}
double Bp6SurfaceFire::getParticleLoad( int particleIndex ) const
{
	return m_load[particleIndex];
}
double Bp6SurfaceFire::getParticleSavr( int particleIndex ) const
{
	return m_savr[particleIndex];
}
double Bp6SurfaceFire::getParticleSeff( int particleIndex ) const
{
	return m_seff[particleIndex];
}
double Bp6SurfaceFire::getParticleSigK( int particleIndex ) const
{
	return m_sigK[particleIndex];
}
int Bp6SurfaceFire::getParticleSize( int particleIndex ) const
{
	return m_size[particleIndex];
}
double Bp6SurfaceFire::getParticleStot( int particleIndex ) const
{
	return m_stot[particleIndex];
}
double Bp6SurfaceFire::getParticleSwtg( int particleIndex ) const
{
	return m_sWtg[particleIndex];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) total surface area (ft2).
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeArea( int lifeCat ) const
{
	return m_lifeArea[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) surface area weighting factor (fraction).
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeAwtg( int lifeCat ) const
{
	return m_lifeAwtg[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) mineral damping coefficient (fraction).
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeEtaS( int lifeCat ) const
{
	return m_lifeEtaS[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) mass of fuel that must be
 *	heated to ignition (i.e., load * effective heating number).
 *
 *  Used to calculate the live fuel moisture content of extinction.
 *
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeFine( int lifeCat ) const
{
	return m_lifeFine[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) weighted heat of combustion (btu/lb).
  *	\param lifeCat 0=dead, 1=live
*/
double Bp6SurfaceFire::getLifeHeat( int lifeCat ) const
{
	return m_lifeHeat[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) size-class weighted fuel load (lb/ft2).
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeLoad( int lifeCat ) const
{
	return m_lifeLoad[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) dry fuel reaction intensity
 *		(btu/ft2/min); i.e., the reaction intensity \a before the moisture damping
 *		coefficient is applied.
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeRxDry( int lifeCat ) const
{
	return m_lifeRxDry[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) weighted surface area-to-volume ratio (1/ft).
 *	\param lifeCat 0=dead, 1=live
*/
double Bp6SurfaceFire::getLifeSavr( int lifeCat ) const
{
	return m_lifeSavr[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) weighted effective (silica-free)
 *	mineral content (fraction).
 *	\param lifeCat 0=dead, 1=live
 */
double Bp6SurfaceFire::getLifeSeff( int lifeCat ) const
{
	return m_lifeSeff[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) weighted total mineral content (fraction).
 *	\param lifeCat 0=dead, 1=live
*/
double Bp6SurfaceFire::getLifeStot( int lifeCat ) const
{
	return m_lifeStot[lifeCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) size class weighting factor
 *	by size class (fraction).
 *	\param lifeCat 0=dead, 1=live
 *	\param size Size class index [0..5]
*/
double Bp6SurfaceFire::getLifeSwtg( int lifeCat, int size ) const
{
	return m_lifeSwtg[lifeCat][size];
}

// Fuel bed member variable accessors
double Bp6SurfaceFire::getAa() const
{
	return m_aa;
}
double Bp6SurfaceFire::getBetaRatio() const
{
	return m_betaRatio;
}
double Bp6SurfaceFire::getBulkDensity() const
{
	return m_bulkDensity;
}
double Bp6SurfaceFire::getC() const
{
	return m_c;
}
double Bp6SurfaceFire::getE() const
{
	return m_e;
}
double Bp6SurfaceFire::getEffectiveHeatingNumber() const
{
	return m_epsilon;
}
double Bp6SurfaceFire::getGammaMax() const
{
	return m_gammaMax;
}
double Bp6SurfaceFire::getGammaOpt() const
{
	return m_gammaOpt;
}
double Bp6SurfaceFire::getLiveMextK() const
{
	return m_liveMextK;
}
double Bp6SurfaceFire::getOptimumPackingRatio() const
{
	return m_betaOpt;
}
double Bp6SurfaceFire::getPackingRatio() const
{
	return m_packingRatio;
}
double Bp6SurfaceFire::getPropagatingFlux() const
{
	return m_propFlux;
}
double Bp6SurfaceFire::getResidenceTime() const
{
	return m_resTime;
}
double Bp6SurfaceFire::getSigma() const
{
	return m_sigma;
}
double Bp6SurfaceFire::getSigma15() const
{
	return m_sigma15;
}
double Bp6SurfaceFire::getSlopeK() const
{
	return m_slopeK;
}
double Bp6SurfaceFire::getTotalArea() const
{
	return m_totalArea;
}
double Bp6SurfaceFire::getTotalLoad() const
{
	return m_totalLoad;
}
double Bp6SurfaceFire::getWindB() const
{
	return m_windB;
}
double Bp6SurfaceFire::getWindE() const
{
	return m_windE;
}
double Bp6SurfaceFire::getWindK() const
{
	return m_windK;
}
//------------------------------------------------------------------------------
// Fuel moisture input accessors
double Bp6SurfaceFire::getMois( int particleIndex ) const
{
	return m_mois[ particleIndex ];
}
//------------------------------------------------------------------------------
// Fuel moisture dependent output member data accessors
double  Bp6SurfaceFire::getDeadMois() const
{
	return m_deadMois;
}
double  Bp6SurfaceFire::getDeadRxDry() const
{
	return m_lifeRxDry[DeadCat];
}
//------------------------------------------------------------------------------
/*!	\brief Returns the ratio of the water mass / fuel mass that must be heated
 *	to ignition (used to derive the live fuel moisture content of extinction).
 */
double  Bp6SurfaceFire::getFdMois() const
{
	return m_fdmois;
}
double  Bp6SurfaceFire::getHeatSink() const
{
	return m_rbQig;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) fuel moisture content of extinction (ratio).
 *	\param lifeCat 0=dead, 1=live
*/
double Bp6SurfaceFire::getLifeMext( int lifeCat ) const
{
	return ( lifeCat == DeadCat ) ? m_deadMext : m_liveMextApplied;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the life category (dead or live) weighted fuel moisture content (ratio).
 *	\param lifeCat 0=dead, 1=live
*/
double Bp6SurfaceFire::getLifeMois( int lifeCat ) const
{
	return ( lifeCat == DeadCat ) ? m_deadMois : m_liveMois;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the live fuel moisture content of extinction (ratio) that was
 *	applied in the derivation of the live fuel moisture damping coefficient.
 *
 *	If m_liveMextChaparral > 0.5, then it is applied.  This should only occur if
 *	dynamic chaparral fuel models (Rothermel & Philpot 1973) are being used,
 *	in which case Weise et al recommend 0.65 for chamise and 0.74 for mixed brush.
 *
 *	If m_liveMextChaparral < 0.5, then m_liveMextCalculated is applied.
*/
double  Bp6SurfaceFire::getLiveMextApplied() const
{
	return m_liveMextApplied;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the \a calculated live fuel moisture content of extinction (ratio).
*/
double  Bp6SurfaceFire::getLiveMextCalculated() const
{
	return m_liveMextCalculated;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the \a chaparral live fuel moisture content of extinction (ratio).
 *
 *	If m_liveMextChaparral > 0.5, then it is applied.  This should only occur if
 *	dynamic chaparral fuel models (Rothermel & Philpot 1973) are being used,
 *	in which case Weise et al recommend 0.65 for chamise and 0.74 for mixed brush.
 *
 *	If m_liveMextChaparral < 0.5, then m_liveMextCalculated is applied.
*/
double  Bp6SurfaceFire::getLiveMextChaparral() const
{
	return m_liveMextChaparral;
}
double  Bp6SurfaceFire::getLiveMois() const
{
	return m_liveMois;
}
double  Bp6SurfaceFire::getLiveRxDry() const
{
	return m_lifeRxDry[LiveCat];
}
double  Bp6SurfaceFire::getRbQig() const
{
	return m_rbQig;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the ratio of the mass of water within the mass of fuel that
 *	must be heated to ignition (used to derive the live fuel moisture content of extinction).
 */
double  Bp6SurfaceFire::getWfmd() const
{
	return m_wfmd;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the dead fuel moisture damping coefficient (dl).
 */
 double Bp6SurfaceFire::getDeadEtaM() const
 {
	 return m_deadEtaM;
 }
//------------------------------------------------------------------------------
/*!	\brief Returns the dead fuel fire reaction intensity (Btu/ft2/min).
 */
 double Bp6SurfaceFire::getDeadRxInt() const
 {
	 return m_deadRxInt;
 }
//------------------------------------------------------------------------------
/*!	\brief Returns the live fuel moisture damping coefficient (dl).
 */
 double Bp6SurfaceFire::getLiveEtaM() const
 {
	 return m_liveEtaM;
 }
//------------------------------------------------------------------------------
/*!	\brief Returns the live fuel fire reaction intensity (Btu/ft2/min).
 */
 double Bp6SurfaceFire::getLiveRxInt() const
 {
	 return m_liveRxInt;
 }
//------------------------------------------------------------------------------
/*!	\brief Returns the total (dead plus live) fire reaction intensity (Btu/ft2/min).
 */
 double Bp6SurfaceFire::getTotalRxInt() const
 {
	 return m_totalRxInt;
 }
//------------------------------------------------------------------------------
/*! \brief Returns the current no-wind, no-slope spread rate (ft/min).
*/
 double Bp6SurfaceFire::getNoWindNoSlopeSpreadRate() const
{
	return m_ros0;
}
//------------------------------------------------------------------------------
/*! \brief Returns the current input applyWindSpeedLimit toggle.
*/
bool Bp6SurfaceFire::getApplyWindSpeedLimit() const
{
	return m_applyWindSpeedLimit;
}
//------------------------------------------------------------------------------
/*! \brief Returns the current input terrain aspect (degrees clockwie from north).
*/
double Bp6SurfaceFire::getAspect() const
{
	return m_aspect;
}
//------------------------------------------------------------------------------
/*! \brief Returns the fire perimeter eccentricity (fl).
*/
double Bp6SurfaceFire::getEccentricity() const
{
	return m_eccent;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated effective wind speed (ft/min)
*/
double Bp6SurfaceFire::getEffectiveWindSpeed() const
{
	return m_effectiveWindSpeed;
}
//------------------------------------------------------------------------------
/*!	\brief Returns the effective wind speed (ft/min) for the given vector spread rate.
 *
 *	Replaces the xfblib function FBL_EffectWindSpeedAtVector
 *  which requires use of the global m_* variables.
 */
double Bp6SurfaceFire::getEffectiveWindSpeedAtVector( double vectorRos ) const
{
	double phiEw   = ( m_ros0 < m_smidgen ) ? 0. : ( vectorRos / m_ros0 - 1. );
    double effWind = ( ( phiEw * m_windE ) < m_smidgen || m_windB < m_smidgen )
                   ? 0. : pow( ( phiEw * m_windE ), ( 1. / m_windB ) );
    return effWind;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated direction of maximum spread (degrees clockwise from upslope).
*/
double Bp6SurfaceFire::getHeadDirFromUpslope() const
{
	return m_headDirFromUpslope;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated heat per unit area (btu/ft2).
*/
double Bp6SurfaceFire::getHeatPerUnitArea() const
{
	return m_hpua;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fireline intensity at the fire back (btu/ft/s).
*/
double Bp6SurfaceFire::getFirelineIntensityAtBack() const
{
	return m_fliBack;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fireline intensity at the fire flank (btu/ft/s).
*/
double Bp6SurfaceFire::getFirelineIntensityAtFlank() const
{
	return m_fliFlank;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fireline intensity at the fire head (btu/ft/s).
*/
double Bp6SurfaceFire::getFirelineIntensityAtHead() const
{
	return m_fliHead;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated flame length at the fire back (btu/ft/s).
*/
double Bp6SurfaceFire::getFlameLengthAtBack() const
{
	return m_flameBack;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated flame length at the fire flank (btu/ft/s).
*/
double Bp6SurfaceFire::getFlameLengthAtFlank() const
{
	return m_flameFlank;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated flame length at the fire head (btu/ft/s).
*/
double Bp6SurfaceFire::getFlameLengthAtHead() const
{
	return m_flameHead;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire length-to-width ratio
*/
double Bp6SurfaceFire::getLengthToWidthRatio() const
{
	return m_lwRatio;
}
//------------------------------------------------------------------------------
/*! \brief Returns the current input midflame wind speed (mi/h).
*/
double Bp6SurfaceFire::getMidflameWindSpeed() const
{
	return m_midflameWindSpeed;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire spread rate situation code (1..6).
*/
int Bp6SurfaceFire::getSituation() const
{
	return m_situation;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire spread rate slope factor phiS (dl).
*/
double Bp6SurfaceFire::getSlopeFactor() const
{
	return m_slopeFactor;
}
//------------------------------------------------------------------------------
/*! \brief Returns the current input terrain slope steepnes (rise/reach).
*/
double Bp6SurfaceFire::getSlopeFraction() const
{
	return m_slopeFraction;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated spread rate at the fire back (ft/min).
*/
double Bp6SurfaceFire::getSpreadRateAtBack() const
{
	return m_rosBack;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated spread rate at the specified beta vector (ft/min).
 *	\param beta Vector of interest (degrees clockwise from heading direction).
*/
double Bp6SurfaceFire::getSpreadRateAtBeta( double beta ) const
{
	return calcSpreadRateAtBeta( m_rosHead, m_lwRatio, beta );
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated spread rate at the fire flank (ft/min).
*/
double Bp6SurfaceFire::getSpreadRateAtFlank() const
{
	return m_rosFlank;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated spread rate at the fire head (ft/min).
*/
double Bp6SurfaceFire::getSpreadRateAtHead() const
{
	return m_rosHead;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated spread rate-exceeds-wind speed flag.
*/
bool Bp6SurfaceFire::getSpreadRateExceedsWindSpeed() const
{
	return m_spreadExceedsWind;
}
//------------------------------------------------------------------------------
/*! \brief Returns the current input no-wind, no-slope fire spread rate (ft/min).
*/
double Bp6SurfaceFire::getWindDirFromUpslope() const
{
	return m_windDirFromUpslope;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire spread rate wind factor phiW (dl).
*/
double Bp6SurfaceFire::getWindFactor() const
{
	return m_windFactor;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated wind limit exceeded flag.
*/
bool Bp6SurfaceFire::getWindLimitExceeded() const
{
	return m_windLimitExceeded;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire spread rate combined wind-slope factor phiEw (dl).
*/
double Bp6SurfaceFire::getWindSlopeFactor() const
{
	return m_windFactor + m_slopeFactor;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire spread wind speed limit (ft/min).
*/
double Bp6SurfaceFire::getWindSpeedLimit() const
{
	return m_windSpeedLimit;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire area in acres.
*/
double Bp6SurfaceFire::getFireAcres() const
{
	return m_fireArea / ( 66. * 660.);
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire area in ft2.
*/
double Bp6SurfaceFire::getFireArea() const
{
	return m_fireArea;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire length (ft).
*/
double Bp6SurfaceFire::getFireLength() const
{
	return m_fireLength;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire perimeter (ft).
*/
double Bp6SurfaceFire::getFirePerimeter() const
{
	return m_firePerimeter;
}
//------------------------------------------------------------------------------
/*! \brief Returns the updated fire width (ft).
*/
double Bp6SurfaceFire::getFireWidth() const
{
	return m_fireWidth;
}
//------------------------------------------------------------------------------
/*!	\brief This is a convenience routine used only by the derived Bp6CrownFire
 *	class to easily derive the R'sa of the surface fuel below the canopy.
 */
double Bp6SurfaceFire::getRsa(
        double midflameWindSpeed )		// mid-flame wind speed (ft/min)
{
	double saved = m_midflameWindSpeed;
	m_midflameWindSpeed	= midflameWindSpeed;
	updateSite();
	double rsa = m_rosHead;
	m_midflameWindSpeed = saved;
	updateSite();
	return rsa;
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetFuelInput()
{
	m_depth = 1.;
	m_deadMext = 0.;
	m_particles = 0;
	for( int p=0; p<MaxParts; p++ )
	{
		m_life[p] = DeadCat;
		m_load[p] = 0.;
		m_savr[p] = 1.;
		m_heat[p] = 8000.;
		m_dens[p] = 32.;
		m_stot[p] = 0.0555;
		m_seff[p] = 0.0100;
	}
}
//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetFuelOutput()
{
    // Fuel bed intermediates output by this function
	m_aa           = 0.;
	m_betaOpt      = 0.;
    m_betaRatio    = 0.;
    m_bulkDensity  = 0.;
	m_c            = 0.;
	m_e            = 0.;
	m_epsilon      = 0.;
	m_gammaMax     = 0.;
	m_gammaOpt     = 0.;
	m_liveMextK    = 0.;
    m_packingRatio = 0.;
	m_propFlux     = 0.;
	m_resTime      = 0.;
    m_sigma        = 0.;
	m_sigma15      = 0.;
	m_slopeK       = 0.;
    m_totalArea    = 0.;
    m_totalLoad    = 0.;
	m_windB        = 0.;
	m_windE        = 0.;
	m_windK        = 0.;

    // Initialize particle intermediates
    for ( int p=0; p<MaxParts; p++ )
    {
		m_aWtg[MaxParts] = 0.;
		m_sigK[MaxParts] = 0.;
		m_area[MaxParts] = 0.;
		m_size[MaxParts] = 0;
		m_sWtg[MaxParts] = 0.;
    }
    // Initialize all fuel bed intermediates
    for ( int l=0; l<MaxCats; l++)
    {
        m_lifeArea[l] = 0.;
        m_lifeAwtg[l] = 0.;
        m_lifeEtaS[l] = 0.;
        m_lifeFine[l] = 0.;
        m_lifeHeat[l] = 0.;
        m_lifeLoad[l] = 0.;
        m_lifeRxDry[l]  = 0.;
        m_lifeSavr[l] = 0.;
        m_lifeSeff[l] = 0.;
        m_lifeStot[l] = 0.;
        for ( int s=0; s<MaxSizes; s++ )
        {
            m_lifeSwtg[l][s] = 0.;
        }
    }
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetMoistureInput()
{
	for( int p=0; p<MaxParts; p++ )
	{
	    m_mois[p] = 5.0;
	}
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetMoistureOutput()
{
    m_rbQig    = 0.;
    m_fdmois   = 0.;
    m_wfmd     = 0.;
    m_deadMois = 0.;
    m_liveMois = 0.;
    m_deadEtaM = 0.;
	m_deadRxInt = 0.;
	m_liveEtaM   = 0.;
	m_liveRxInt  = 0.;
	m_totalRxInt = 0.;
	m_ros0       = 0.;
	m_liveMextApplied = 0.;
	m_liveMextCalculated = 0.;
	m_liveMextChaparral = 0.;
}
//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetSiteInput()
{
	m_aspect				= 180.;
	m_slopeFraction			= 0.;
	m_midflameWindSpeed		= 0.;
	m_windDirFromUpslope	= 0.;
	m_applyWindSpeedLimit	= true;
}
//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetSiteOutput()
{
	m_rosHead = 0.;
    m_headDirFromUpslope = 0.;
    m_effectiveWindSpeed = 0.;
    m_windSpeedLimit = 0.;
    m_windLimitExceeded = false;
    m_windFactor = 0.;
    m_slopeFactor = 0.;
	m_spreadExceedsWind = false;
	m_situation = 0;
	m_lwRatio = 1.;
	m_eccent = 0.;
	m_hpua = 0.;
	m_rosBack =0.;
	m_rosMajor =0.;
	m_rosFlank =0.;
	m_fliHead =0.;
	m_fliBack = 0.;
	m_fliFlank = 0.;
	m_flameHead = 0.;
	m_flameBack = 0.;
	m_flameFlank = 0.;
	m_ellipseF = 0.;
	m_ellipseG = 0.;
	m_ellipseH = 0.;
	resetSiteOutputExtension();
}
//------------------------------------------------------------------------------
/*!	\brief Hook to derived class extensions.
*/
void Bp6SurfaceFire::resetSiteOutputExtension()
{
}
//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetTimeInput()
{
	m_elapsed = 0.;
}
//------------------------------------------------------------------------------
void Bp6SurfaceFire::resetTimeOutput()
{
	m_fireLength = 0.;
	m_fireWidth = 0.;
	m_fireArea = 0.;
	m_firePerimeter = 0.;
	resetTimeOutputExtension();
}
//------------------------------------------------------------------------------
/*!	\brief Hook to derived class extensions.
*/
void Bp6SurfaceFire::resetTimeOutputExtension()
{
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::setFuel(
	double depth,		// fuel bed depth (ft)
	double deadMext,	// fuel dead dead fuel moisture of extinction
	int    particles,	// number of particles in following arrays
	int    *life,		// array of fuel particle life codes
	double *load,		// array of fuel particle loads (lb/ft2)
	double *savr,		// array of fuel aprticle surface area-to-volume rations (ft3/ft2)
	double *heat,		// array of fuel particle low heat of combustions (btu/lb)
	double *dens,		// array of fuel particle densities (lb/ft3)
	double *stot,		// array of fuel particle total Silica contents (lb/lb)
	double *seff )		// array of fuel particle effective Silica contents (lb/lb)
{
	resetFuelOutput();
	resetMoistureOutput();
	resetSiteOutput();
	m_depth     = depth;
	m_deadMext  = deadMext;
	m_particles = particles;
	for( int p=0; p<m_particles; p++ )
	{
		m_life[p] = life[p];
		m_load[p] = load[p];
		m_savr[p] = savr[p];
		m_heat[p] = heat[p];
		m_dens[p] = dens[p];
		m_stot[p] = stot[p];
		m_seff[p] = seff[p];
	}
	updateFuel();
}

//------------------------------------------------------------------------------
/*!
 *	\param chaparralLiveMext Weise, et al. uses 0.65 for chamise and 0.74 for mixed brush.
 *	If liveMext is 0., then live fuel extinction moisture is calculated internally.
*/
void Bp6SurfaceFire::setMoisture(
		double* mois,// array of fuel particle moisture contents (lb/lb)
		double chaparralLiveMext	// Rothermel & Philpot chaparral live mext
	)
{
	resetMoistureOutput();
	resetSiteOutput();
	for( int p=0; p<m_particles; p++ )
	{
		m_mois[p] = mois[p];
	}
	m_liveMextChaparral = chaparralLiveMext;
	updateMoisture();
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::setSite(
        double slopeFraction,			// terrain slope (rise/reach)
		double aspect,					// degrees clockwise from north
        double midflameWindSpeed,		// mid-flame wind speed (ft/min)
        double windDirFromUpslope,		// wind heading direction (degrees clockwise from upslope)
		bool   applyWindSpeedLimit )	// if TRUE, apply Rothermel's upper wind speed limit
{
	resetSiteOutput();
	m_slopeFraction			= slopeFraction;
	m_aspect				= aspect;
	m_midflameWindSpeed		= midflameWindSpeed;
	m_windDirFromUpslope	= windDirFromUpslope;
	m_applyWindSpeedLimit	= applyWindSpeedLimit;
	updateSite();
}

//------------------------------------------------------------------------------
void Bp6SurfaceFire::setTime( double elapsed )	// elapsed time since ignition (min)
{
	resetTimeOutput();
	m_elapsed = elapsed;
	updateTime();
}

//------------------------------------------------------------------------------
/*!	\brief Updates all the variables that depend only upon fuel parameters.
 *
 *	Replaces the xfblib functions:
 *	- FBL_SurfaceFuelBedIntermediates
 *	and adds calculation of:
 *	- propagating flux
 *	- fire residence time.
 */
void Bp6SurfaceFire::updateFuel()
{
	// If no fuel bed depth, we're done
	// If no fuel, or missing pointers, or no fuel bed depth, them we're done
    if ( m_depth < m_smidgen  || m_particles < 1
		|| ! m_life || ! m_load || ! m_savr || ! m_heat || ! m_dens || ! m_stot || ! m_stot )
	{
		return;
	}

	// Savr by size class                 .04"  .25"  0.5"  1.0"  3.0"
    static double Size_bdy[MaxSizes] = { 1200., 192., 96.0, 48.0, 16.0, 0. };
    // Determine particle area, savr exponent factor, and size class.
    for ( int p=0; p<m_particles; p++ )
    {
        // Accumulate fuel bed total load
        m_totalLoad += m_load[p];
        // Particle surface area
        m_area[p] = ( m_dens[p] < m_smidgen ) ? ( 0.0 ) : ( m_load[p] * m_savr[p] / m_dens[p] );
        // Accumulate fuel bed and life category surface areas
        int l = fuelLife( m_life[p] );
        m_lifeArea[l] += m_area[p];
        m_totalArea += m_area[p];
        // Fuel bed packing ratio
        if ( m_dens[p] >= m_smidgen )
        {
            m_packingRatio += m_load[p] / m_dens[p];
        }

        m_sigK[p] = ( m_savr[p] < m_smidgen ) ? ( 0.0 ) : ( exp( -138. / m_savr[p] ) );
        // Determine size class for this particle
		int s;
        for ( s=0; m_savr[p] < Size_bdy[s]; s++ )
        {
            ; // NOTHING
        }
        m_size[p] = s;
    }
    // If there is no fuel area, then we're done.
    if ( m_totalArea < m_smidgen )
    {
        return;
    }
    // Complete the bulkDensity, packingRatio, and slopeK calculations.
    if ( m_depth > m_smidgen )
    {
        m_bulkDensity = m_totalLoad / m_depth;
        m_packingRatio /= m_depth;
    }
    m_slopeK = ( m_packingRatio < m_smidgen ) ? ( 0.0 ) : ( 5.275 * pow( m_packingRatio, -0.3 ) );
    // Surface area wtg factor for each particle within its life category
    // and within its size class category (used to weight loading).
    for ( int p=0; p<m_particles; p++ )
    {
        int l = fuelLife( m_life[p] );
        m_aWtg[p] = ( m_lifeArea[l] < m_smidgen ) ? ( 0.0 ) : ( m_area[p] / m_lifeArea[l] );
        m_lifeSwtg[l][m_size[p]] += m_aWtg[p];
    }
    // Assign size class surface area weights to each particle.
    for ( int p=0; p<m_particles; p++ )
    {
        int l = fuelLife( m_life[p] );
        m_sWtg[p] = m_lifeSwtg[l][m_size[p]];
    }
    // Derive life category surface area weighting factors.
    for ( int l=0; l<MaxCats; l++ )
    {
        m_lifeAwtg[l] = ( m_totalArea < m_smidgen ) ? ( 0.0 ) : ( m_lifeArea[l] / m_totalArea );
    }
    // Accumulate life category weighted load, savr, heat, seff, and stot,
    // and the particle's contribution to bulk density and packing ratio.
    for ( int p=0; p<m_particles; p++ )
    {
        int l = fuelLife( m_life[p] );
        m_lifeLoad[l] += m_sWtg[p] * m_load[p];
        m_lifeSavr[l] += m_aWtg[p] * m_savr[p];
        m_lifeHeat[l] += m_aWtg[p] * m_heat[p];
        m_lifeSeff[l] += m_aWtg[p] * m_seff[p];
        m_lifeStot[l] += m_aWtg[p] * m_stot[p];
    }
    // Fuel bed characteristics surface area-to-volume (sigma).
    for ( int l=0; l<MaxCats; l++ )
    {
        m_sigma += m_lifeAwtg[l] * m_lifeSavr[l];
    }
    // Optimum reaction velocity computations.
    m_betaOpt   = 3.348 / ( pow( m_sigma, 0.8189 ) );
    m_aa        = 133. / ( pow( m_sigma, 0.7913 ) );
    m_sigma15   = pow( m_sigma, 1.5 );
    m_gammaMax  = m_sigma15 / ( 495. + 0.0594 * m_sigma15 );
    m_betaRatio = ( m_betaOpt < m_smidgen ) ? ( 0.0 ) : ( m_packingRatio / m_betaOpt );
    if ( m_betaRatio > m_smidgen && m_betaRatio != 1. )
    {
        m_gammaOpt = m_gammaMax * pow( m_betaRatio, m_aa ) * exp( m_aa * ( 1. - m_betaRatio ) );
    }
    // Slope and wind fuel bed intermediates.
    m_windB  = 0.02526 * pow( m_sigma, 0.54 );
    m_c      = 7.47 * exp( -0.133 * pow( m_sigma, 0.55 ) );
    m_e      = 0.715 * exp( -0.000359 * m_sigma );
    m_windK  = ( m_betaRatio < m_smidgen ) ? ( 0. ) : ( m_c * pow( m_betaRatio, -m_e ) );
    m_windE  = ( m_betaRatio < m_smidgen || m_c < m_smidgen ) ? ( 0. )
             : ( pow( m_betaRatio, m_e ) / m_c );
    // Life category mineral damping coefficient and contribution to reaction intensity.
    for ( int l=0; l<MaxCats; l++ )
    {
        // Mineral damping coefficient.
        if ( ( m_lifeEtaS[l] = ( m_lifeSeff[l] < m_smidgen )
                    ? ( 1.0 ) : ( 0.174 / pow( m_lifeSeff[l], 0.19 ) )
             ) > 1.0 )
        {
            m_lifeEtaS[l] = 1.0;
        }
        m_lifeRxDry[l] = m_gammaOpt
			* m_lifeLoad[l] * ( 1. - m_lifeStot[l] )	// net fuel loading 
			* m_lifeHeat[l]
			* m_lifeEtaS[l];
    }
    //  Mass of dead and live fuel that must be heated to ignition
    for ( int p=0; p<m_particles; p++ )
    {
        int l = fuelLife( m_life[p] );
        if ( l == DeadCat )
        {
            m_lifeFine[l] += m_load[p] * m_sigK[p];
        }
        else if ( m_savr[p] > m_smidgen )
        {
            m_lifeFine[l] += m_load[p] * exp( -500. / m_savr[p] );
        }
    }
    // Live fuel extinction moisture factor.
    m_liveMextK = ( m_lifeFine[LiveCat] < m_smidgen )
                ? ( 0.0 ) : ( 2.9 * m_lifeFine[DeadCat] / m_lifeFine[LiveCat] );
	// Propagating flux
    m_propFlux = ( m_sigma < m_smidgen ) ? ( 0. )
          : ( exp( ( 0.792 + 0.681 * sqrt( m_sigma ) )
            * ( m_packingRatio + 0.1 ) )
            / ( 192. + 0.2595 * m_sigma ) );
	// Residence time
    m_resTime = ( m_sigma < m_smidgen ) ? ( 0.0 ) : ( 384. / m_sigma );
	// Effective heating number
	m_epsilon = ( m_sigma > m_smidgen ) ? exp( -138. / m_sigma ) : 0.;
}

//------------------------------------------------------------------------------
/*!	\brief Updates all variables that depend upon fuel and moisture parameters.
 *
 *	Replaces the xfblib functions:
 *	- FBL_SurfaceFuelBedHeatSink
 *	- FBL_SurfaceFireReactionIntensity
 *	And adds calculation of:
 *	- no-wind, no-slope fire spread rate.
 */
void Bp6SurfaceFire::updateMoisture()
{
    int nLive = 0;
    // Compute category weighted moisture and accumulate the rbQig.
	for ( int p=0; p<m_particles; p++ )
    {
        double qig = 250. + 1116. * m_mois[p];
        int lifeCat = fuelLife( m_life[p] );
        if ( lifeCat == DeadCat )
        {
			// Mass of water within the mass of fuel that must be heated to ignition
			// Used to derive the live fuel moisture content of extinction
            m_wfmd     += m_mois[p] * m_sigK[p] * m_load[p];
            m_deadMois += m_aWtg[p] * m_mois[p];
        }
        else
        {
            nLive++;
            m_liveMois += m_aWtg[p] * m_mois[p];
        }
		// Note that m_sigK[p] is the effective heating number of particle p
        m_rbQig += qig * m_aWtg[p] * m_lifeAwtg[lifeCat] * m_sigK[p];
    }

    // Complete the rbQig calculation.
	m_rbQig *= m_bulkDensity;

    // Compute live fuel extinction moisture.
	m_liveMextCalculated = m_deadMext;
    if ( nLive )
    {
		double deadFine = m_lifeFine[DeadCat];
		// Ratio of water mass to fuel mass that must be heated to ignition
		// Used to derive the live fuel moisture content of extinction
        m_fdmois = ( deadFine < m_smidgen ) ? ( 0.0 ) : ( m_wfmd / deadFine );

        m_liveMextCalculated = ( m_deadMext < m_smidgen ) ? ( 0.0 )
                 : ( ( m_liveMextK * ( 1.0 - m_fdmois / m_deadMext ) ) - 0.226 );
    }
    m_liveMextCalculated = ( m_liveMextCalculated < m_deadMext )
		                 ? ( m_deadMext ) : ( m_liveMextCalculated );
	m_liveMextApplied = ( m_liveMextChaparral > 0.5 )
		              ? m_liveMextChaparral : m_liveMextCalculated;

	// Hack requested by Pat Andrews
    //if ( m_liveMext > 4.00 )
    //{
    //    m_liveMext = 4.00;
    //}
    
	//  Moisture damping coefficient by life class.
    double r;
    if ( m_deadMext >= m_smidgen && ( ( r = m_deadMois / m_deadMext ) ) < 1. )
	{
        m_deadEtaM = 1.0 - 2.59*r + 5.11*r*r - 3.52*r*r*r;
	}

    if ( m_liveMextApplied >= m_smidgen && ( ( r = m_liveMois / m_liveMextApplied ) ) < 1. )
	{
		m_liveEtaM = 1.0 - 2.59*r + 5.11*r*r - 3.52*r*r*r;
	}

    // Combine moisture damping with rx factor to get total reaction int.
    m_deadRxInt  = m_lifeRxDry[DeadCat] * m_deadEtaM;
    m_liveRxInt  = m_lifeRxDry[LiveCat] * m_liveEtaM;
    m_totalRxInt = m_deadRxInt + m_liveRxInt;

	// No-wind, no-slope spread rate
    m_ros0 = ( m_rbQig < m_smidgen ) ? ( 0.0 ) : ( m_totalRxInt * m_propFlux / m_rbQig );
}
//------------------------------------------------------------------------------
/*!	\brief Updates all the variables that depend upon fuel, moisture, slope, and
 *	wind variables.
 *
 *	Replaces the xfblib functions:
 *	- FBL_SurfaceFireForwardSpreadRate
 *  and adds the variables:
 *	- length-to-width ratio
 *
 *	\par UNITS CHANGE!
 *	Effective wind speed and the wind speed limit are now in ft/min, NOT mi/h!
 */
void Bp6SurfaceFire::updateSite( bool applyExtension )
{
    // Slope factor
    double phiS  = m_slopeK * m_slopeFraction * m_slopeFraction;

    // Wind factor
    double windFpm = m_midflameWindSpeed;
    double phiW  = ( windFpm < m_smidgen )
                 ? ( 0.0 )
                 : ( m_windK * pow( windFpm, m_windB ) );

    // Combined wind-slope factor
    double phiEw = phiS + phiW;

    // Wind direction relative to upslope
    double windDir = m_windDirFromUpslope;

    // No-wind no-slope spread rate and parameters
    double ros0      = m_ros0;
    double rosMax    = ros0;
    double dirMax    = 0.;
    double effWind   = 0.;
    bool   doEffWind = false;
    bool   windLimitExceeded = false;
    int    situation = 0;
    // Situation 1: no fire spread.
    if ( ros0 < m_smidgen )
    {
        rosMax    = ros0;
        dirMax    = 0;
        effWind   = 0.;
        // There IS an effective wind even if there is no fire.
        doEffWind = true;
        // But since BEHAVE doesn't calculate effective wind when no spread,
        // we wont either.
        doEffWind = false;
        situation = 1;
    }
    // Situation 2: no wind and no slope.
    else if ( phiEw < m_smidgen )
    {
        rosMax    = ros0;
        dirMax    = 0;
        effWind   = 0.;
        doEffWind = false;
        situation = 2;
    }
    // Situation 3: wind with no slope.
    else if ( phiS < m_smidgen )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = windDir;
        effWind   = windFpm;
        doEffWind = false;
        situation = 3;
    }
    // Situation 4: slope with no wind.
    else if ( phiW < m_smidgen )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = 0.;
        doEffWind = true;
        situation = 4;
    }
    // Situation 5: wind blows upslope.
    else if ( windDir < m_smidgen )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = 0.;
        doEffWind = true;
        situation = 5;
    }
    // Situation 6: wind blows cross slope.
    else
    {
        // Calculate spread rate in optimal direction.
        double splitRad  = windDir * m_pi / 180.;
        double slpRate   = ros0 * phiS;
        double wndRate   = ros0 * phiW;
        double x         = slpRate + wndRate * cos(splitRad);
        double y         = wndRate * sin(splitRad);
        double rv        = sqrt(x*x + y*y);
        rosMax = ros0 + rv;

        // Recalculate phiEw in the optimal direction.
        phiEw     = ( ros0 < m_smidgen ) ? ( 0.0 ) : ( rosMax / ros0 - 1.0 );
        doEffWind = ( phiEw >= m_smidgen );
        // Recalculate direction of maximum spread in azimuth degrees.
        double al = ( rv < m_smidgen ) ? ( 0.0 ) : ( asin( fabs( y ) / rv ) );
        double a;
        if ( x >= 0. )
        {
            a = ( y >= 0. ) ? ( al ) : ( m_pi + m_pi - al );
        }
        else
        {
            a = ( y >= 0. ) ? ( m_pi - al ) : ( m_pi + al );
        }
        dirMax = a * 180. / m_pi;
        if ( fabs( dirMax ) < 0.5 )
        {
            dirMax = 0.0;
        }
        situation = 6;
    }
    // Recalculate effective wind speed based upon phiEw.
    if ( doEffWind )
    {
        effWind = ( ( phiEw * m_windE ) < m_smidgen || m_windB < m_smidgen )
                ? ( 0.0 )
                : ( pow( ( phiEw * m_windE ), ( 1. / m_windB ) ) );
    }
    // If effective wind exceeds maximum wind, scale back spread & phiEw.
    double maxWind = 0.9 * m_totalRxInt;
    if ( effWind > maxWind )
    {
        windLimitExceeded = true;
        if ( m_applyWindSpeedLimit )
        {
            phiEw     = ( maxWind < m_smidgen )
                      ? ( 0.0 )
                      : ( m_windK * pow( maxWind, m_windB ) );
            rosMax    = ros0 * ( 1. + phiEw );
            effWind   = maxWind;
        }
    }
	// Added to BehavePlus6 by PLA
	m_spreadExceedsWind = false;
	if ( rosMax > effWind && effWind > 88. )
	{
		m_spreadExceedsWind = true;
		rosMax = effWind;
	}

    // Store results so far
	m_rosHead            = rosMax;
    m_headDirFromUpslope = dirMax;
    m_effectiveWindSpeed = effWind;
    m_windSpeedLimit     = maxWind;
    m_windLimitExceeded  = windLimitExceeded;
    m_windFactor         = phiW;
    m_slopeFactor        = phiS;
	m_situation          = situation;

	// Heat per unit area
	m_hpua = calcHeatPerUnitArea( m_totalRxInt, m_resTime );

	// Length-to-width ratio
	m_lwRatio = calcEllipseLengthToWidth( m_effectiveWindSpeed );

	// Eccentricity
    m_eccent = calcEllipseEccentricity( m_lwRatio );

	// Spread rate at ellipse back
    m_rosBack = calcSpreadRateAtBack( m_rosHead, m_lwRatio );

	// Spread rate along ellipse major axis
	m_rosMajor = m_rosHead + m_rosBack;

	// Spread rate along the ellipse minor axis (flank)
	m_rosFlank = calcSpreadRateAtFlank( m_rosHead, m_lwRatio );
	
	// Fireline intensity at head
	m_fliHead = calcFirelineIntensity( m_rosHead, m_totalRxInt, m_resTime );
	m_fliBack = calcFirelineIntensity( m_rosBack, m_totalRxInt, m_resTime );
	m_fliFlank = calcFirelineIntensity( m_rosFlank, m_totalRxInt, m_resTime );
	
	// Flame length at head
	m_flameHead = calcFlameLengthByram( m_fliHead );
	m_flameBack = calcFlameLengthByram( m_fliBack );
	m_flameFlank = calcFlameLengthByram( m_fliFlank );

	// Ellipse rate factors (must be multiplied by elapsed time to get distance)
	m_ellipseF = 0.5 * m_rosMajor;
	m_ellipseG = 0.5 * m_rosMajor - m_rosBack;
	m_ellipseH = m_rosFlank;

	// Hook into derived class extensions
	if ( applyExtension )
	{
		updateSiteExtension();
	}
}
//------------------------------------------------------------------------------
/*!	\brief Hook into derived class extensions
 */
void Bp6SurfaceFire::updateSiteExtension()
{}
//------------------------------------------------------------------------------
/*!	\brief Updates all the variables that depend upon elapsed time.
 */
void Bp6SurfaceFire::updateTime( bool applyExtension )
{
	m_fireLength = m_rosMajor * m_elapsed;
	m_fireWidth = 2. * m_rosFlank * m_elapsed;
    m_fireArea = calcEllipseArea( m_fireLength, m_lwRatio );
    m_firePerimeter = calcEllipsePerimeter( m_fireLength, m_fireWidth );

	// Hook into derived class extensions
	if ( applyExtension )
	{
		updateTimeExtension();
	}
}
//------------------------------------------------------------------------------
/*!	\brief Hook into derived class extensions
 */
void Bp6SurfaceFire::updateTimeExtension()
{}

//------------------------------------------------------------------------------
//	Everything below here could be made static
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*! \brief Constrains compass degrees to the range [0-360].
 */
double Bp6SurfaceFire::constrainCompassDegrees( double degrees ) const
{
    while ( degrees > 360 )
    {
        degrees -= 360;
    }
    return( degrees );
}

//------------------------------------------------------------------------------
/*! \brief Returns compass degrees (clockwise from north) from radians.
 */
double Bp6SurfaceFire::calcDegrees( double radians ) const
{
    double degrees = radians * 180. / m_pi;
	return degrees;
}

//------------------------------------------------------------------------------
/*! \brief Returns area of an ellipse given its major axis length and
 *	length-to-width ratio as per Rothermel (1991) equation 11 on page 16
 * (which ignores backing distance).
 */
double Bp6SurfaceFire::calcEllipseArea(
		double length,
		double lwRatio ) const
{
    double area = m_pi * length * length / ( 4. * lwRatio );
	return area;
}

//------------------------------------------------------------------------------
/*! \brief Returns ellipse eccentricity given its length-to-width ratio.
 *	\param lwRatio Ellipse length-to-width ratio.
 *	\return Ellipse eccentricity (dl).
 */
double Bp6SurfaceFire::calcEllipseEccentricity( double lwRatio ) const
{
    double x = m_lwRatio * m_lwRatio - 1.0;
	double eccent = ( x <= 0. ) ? 0. : ( sqrt( x ) / lwRatio );
	return eccent;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fire ellipse length-to-width ratio from the
 *	effective wind speed (ft/min).
 *	\param Effective wind speed (ft/min)
 *	\return Fire ellipse length-to-width ratio (dl).
 */
double Bp6SurfaceFire::calcEllipseLengthToWidth( double effectiveWindSpeed ) const
{
	double mph = effectiveWindSpeed / 88.;	// must be in mi/h
	double ratio = 1. + 0.25 * mph;
	return ratio;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the ellipse perimeter from its length and width.
 *	\param length Ellipse length (arbitrary units)
 *	\param lwRatio Ellipse length-to-width ratio (dl).
 *	\returns Ellipse perimeter (same units as \a length).
 */
double Bp6SurfaceFire::calcEllipsePerimeter(
		double length,
		double width ) const
{
    double a = 0.5 * length;
    double b = 0.5 * width;
	double xm = ( ( a + b ) <= 0. ) ? 0. : ( ( a - b ) / ( a + b ) );
    double xk = 1. + xm * xm / 4. + xm * xm * xm * xm / 64.;
    double perim = m_pi * ( a + b ) * xk;
    return perim;
}

//------------------------------------------------------------------------------
/*! \brief Approximates the ellipse perimeter from its length and length-to-width
 *	ratio using Rothermel's (1991) equation 13 on page 16.
 *	\param length Ellipse length (arbitrary units)
 *	\param lwRatio Ellipse length-to-width ratio (dl).
 *	\returns Ellipse perimeter (same units as \a length).
 */
double Bp6SurfaceFire::calcEllipsePerimeterRothermel(
		double length,
		double lwRatio ) const
{
    double perim = 0.5 * m_pi * length * ( 1. + 1. / lwRatio );
	return perim;
}

//------------------------------------------------------------------------------
/*!	\brief Calculates the fireline intensity from the fire spread rate,
 *	reaction intensity, and residence time.
 *	\param spreadRate Fire spread rate (ft/min)
 *	\param reactionIntensity Fire reaction intensity (btu/ft2/min)
 *	\param residenceTime Fire flaming front residence time (min)
 *	\return Byram's fireline intensity (btu/ft/s)
 */
double Bp6SurfaceFire::calcFirelineIntensity(
	double spreadRate,
	double reactionIntensity,
	double residenceTime ) const
{
	double fli = spreadRate * residenceTime * reactionIntensity / 60.;
	return fli;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fireline intensity (btu/ft/s) given the flame length (ft)
 *	using Byram's (1959) equation.
 */
double Bp6SurfaceFire::calcFirelineIntensityByram( double flameLength ) const
{
	double fli = ( flameLength <= 0.) ? 0. : pow( ( flameLength / 0.45 ), ( 1. / 0.46 ) );
	return fli;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fireline intensity (btu/ft/s) given the flame length (ft)
 *	using Thomas's (1963) equation.
 */
double Bp6SurfaceFire::calcFirelineIntensityThomas( double flameLength ) const
{
	double fli = ( flameLength <= 0.) ? 0. : pow( ( 5. * flameLength ), 1.5 );
	return fli;
}

//------------------------------------------------------------------------------
/*!	\brief Returns Byram's (1959) flame length (ft) given a fireline intensity (Btu/ft/s).
 */
double Bp6SurfaceFire::calcFlameLengthByram( double firelineIntensity ) const
{
	double fl = ( firelineIntensity <= 0.) ? 0.0 : ( 0.45 * pow( firelineIntensity, 0.46 ) );
	return fl;
}

//------------------------------------------------------------------------------
/*!	\brief Returns Thomas's (1963) flame length (ft) given a fireline intensity (Btu/ft/s).
 */
double Bp6SurfaceFire::calcFlameLengthThomas( double firelineIntensity ) const
{
	double fl = ( firelineIntensity <= 0.) ? 0.	: ( 0.2 * pow( firelineIntensity, (2./3.) ) );
	return fl;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's heat per unit area.
 *  \param reactionIntensity Fire reaction intensity (Btu/ft2/min).
 *  \param residenceTime     Fire residence time (min).
 *  \return Fire's heat per unit area (Btu/ft2).
 */
double Bp6SurfaceFire::calcHeatPerUnitArea(
		double reactionIntensity,
		double residenceTime ) const
{
    return( reactionIntensity * residenceTime );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fraction of live herbaceous fuel that is cured.
 *  \param moistureContent Live herbaceous fuel moisture content (fraction).
 *  \return Fraction [0..1] of live herbaceous fuel that is cured.
 */
double Bp6SurfaceFire::calcHerbaceousFuelLoadCuredFraction( double moistureContent ) const
{
    double fraction = 1.333 - 1.11 * moistureContent;
    fraction = ( fraction < 0.0 ) ? 0.0 : fraction;
    fraction = ( fraction > 1.0 ) ? 1.0 : fraction;
    return( fraction );
}

//------------------------------------------------------------------------------
/*! \brief Returns radians of the compass degrees (clockwise from north).
 */
double Bp6SurfaceFire::calcRadians( double degrees ) const
{
    double radians = degrees * m_pi / 180.;
	return radians;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire backing spread rate given its length-to-width ratio.
 *  \param rosHead Fire spread rate at the head of the ellipse (ft/min).
 *	\param Fire ellipse length-to-width ratio (dl).
 *  \return Fire spread rate at the back of the ellipse (ft/min).
 */
double Bp6SurfaceFire::calcSpreadRateAtBack(
		double rosHead,
		double lwRatio ) const
{
	double eccent = calcEllipseEccentricity( lwRatio );
    double rosBack = rosHead * ( 1. - eccent ) / ( 1. + eccent );
	return rosBack;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire spread rate at 'beta' degrees from the
 *  direction of maximum spread.
 *  \param rosHead Fire spread rate at the head (ft/min).
 *  \param lwRatio Fire ellipse length-to-width ratio (dl)
 *  \param beta Fire spread vector of interest (degrees clockwise from heading direction).
 *  \return Fire spread rate along the specified vector (ft/min).
 */
double Bp6SurfaceFire::calcSpreadRateAtBeta(
		double rosHead,
		double lwRatio,
        double beta ) const
{
    double rosBeta = rosHead;
    // Calculate the fire spread rate in this azimuth
    // if it deviates more than a tenth degree from the maximum azimuth
    if ( fabs( beta ) > 0.1 )
    {
        double radians = calcRadians( beta );
		double eccent = calcEllipseEccentricity( lwRatio );
        rosBeta = rosHead * ( 1. - eccent ) / ( 1. - eccent * cos( radians ) );
    }
    return rosBeta;
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire flanking spread rate given its length-to-width ratio.
 *  \param rosHead Fire spread rate at the head of the ellipse (ft/min).
 *	\param Fire ellipse length-to-width ratio (dl).
 *  \return Fire spread rate at the the widest point of the ellipse (ft/min).
 *	Double this result for the elliptical width expansion rate.
 */
double Bp6SurfaceFire::calcSpreadRateAtFlank(
		double rosHead,
		double lwRatio ) const
{
	// Spread rate at ellipse back
    double rosBack = calcSpreadRateAtBack( rosHead, lwRatio );
	// Spread rate along ellipse major axis
	double rosMajor = rosHead + rosBack;
	// Spread rate along the ellipse minor axis (flank)
	double rosFlank = 0.5 * rosMajor / lwRatio;
	return rosFlank;
}
