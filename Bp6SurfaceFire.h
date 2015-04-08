#ifndef BP6_SURFACE_FIRE_H
#define BP6_SURFACE_FIRE_H

class Bp6SurfaceFire
{
public:
	static const int DeadCat = 0;
	static const int LiveCat = 1;
	static const int MaxCats = 2;
	static const int MaxParts = 20;
	static const int MaxSizes = 6;

	Bp6SurfaceFire();
	virtual ~Bp6SurfaceFire();

	// Fuel particle member variable accessors
	double getDeadMext() const;
	double getDepth() const;
	int    getParticles() const;
	double getParticleArea( int particleIndex ) const;
	double getParticleAwtg( int particleIndex ) const;
	double getParticleDens( int particleIndex ) const;
	double getParticleHeat( int particleIndex ) const;
	int    getParticleLife( int particleIndex ) const;
	double getParticleLoad( int particleIndex ) const;
	double getParticleSavr( int particleIndex ) const;
	double getParticleSeff( int particleIndex ) const;
	double getParticleSigK( int particleIndex ) const;
	int    getParticleSize( int particleIndex ) const;
	double getParticleStot( int particleIndex ) const;
	double getParticleSwtg( int particleIndex ) const;

	// Fuel life category member variable accessors
	double getLifeArea( int lifeCat ) const;
	double getLifeAwtg( int lifeCat ) const;
	double getLifeEtas( int lifeCat ) const;
	double getLifeFine( int lifeCat ) const;
	double getLifeHeat( int lifeCat ) const;
	double getLifeLoad( int lifeCat ) const;
	double getLifeRxK( int lifeCat ) const;
	double getLifeSavr( int lifeCat ) const;
	double getLifeSeff( int lifeCat ) const;
	double getLifeStot( int lifeCat ) const;
	double getLifeSwtg( int lifeCat, int size ) const;

	// Fuel bed member variable accessors
	double getAa() const;
	double getBetaRatio() const;
	double getBulkDensity() const;
	double getC() const;
	double getE() const;
	double getEffectiveHeatingNumber() const;
	double getGammaMax() const;
	double getGammaOpt() const;
	double getLiveMextK() const;
	double getOptimumPackingRatio() const;
	double getPackingRatio() const;
	double getPropagatingFlux() const;
	double getResidenceTime() const;
	double getSigma() const;
	double getSigma15() const;
	double getSlopeK() const;
	double getTotalArea() const;
	double getTotalLoad() const;
	double getWindB() const;
	double getWindE() const;
	double getWindK() const;

	// Fuel moisture input accessors
	double getMois( int particleIndex ) const;

	// Fuel moisture dependent member variable accessors
	double getDeadMois() const;
	double getDeadRxK() const;
	double getFdMois() const;
	double getHeatSink() const;
	double getLifeMext( int lifeCat ) const;
	double getLifeMois( int lifeCat ) const;
	double getLiveMext() const;
	double getLiveMois() const;
	double getLiveRxK() const;
	double getRbQig() const;
	double getWfmd() const;
	double getDeadEtaM() const;
	double getDeadRxInt() const;
	double getLiveEtaM() const;
	double getLiveRxInt() const;
	double getTotalRxInt() const;
	double getNoWindNoSlopeSpreadRate() const;

	// Fire site input member variable accessors
	double getAspect() const ;
	bool   getApplyWindSpeedLimit() const;
	double getMidflameWindSpeed() const ;
	double getSlopeFraction() const ;
	double getWindDirFromUpslope() const;

	// Fire site dependent member variable accessors
	double getEccentricity() const;
	double getEffectiveWindSpeed() const;
	double getEffectiveWindSpeedAtVector( double vectorRos ) const;
	double getFirelineIntensityAtBack() const;
	double getFirelineIntensityAtFlank() const;
	double getFirelineIntensityAtHead() const;
	double getFlameLengthAtBack() const;
	double getFlameLengthAtFlank() const;
	double getFlameLengthAtHead() const;
	double getHeadDirFromUpslope() const;
	double getHeatPerUnitArea() const;
	double getLengthToWidthRatio() const;
	int    getSituation() const;
	double getSlopeFactor() const;
	bool   getSpreadRateExceedsWindSpeed() const;
	double getSpreadRateAtBack() const;
	double getSpreadRateAtBeta( double beta ) const;
	double getSpreadRateAtHead() const;
	double getSpreadRateAtFlank() const;
	double getSpreadRateAtMajorAxis() const;
	double getWindFactor() const;
	bool   getWindLimitExceeded() const;
	double getWindSlopeFactor() const;
	double getWindSpeedLimit() const;

	// Fire time dependent member variable accessors
	double getFireAcres() const;
	double getFireArea() const;
	double getFireLength() const;
	double getFirePerimeter() const;
	double getFireWidth() const;

	double getRsa( double midflameWindSpeed );

	// Mutators
	virtual void setFuel(
		double depth,		// ft
		double deadMext,	// lb/lb
		int    particles,	// count
		int    *life,
		double *load,
		double *savr,
		double *heat,
		double *dens,
		double *stot,
		double *seff );

	virtual void setMoisture( double* mois );

	virtual void setSite(
        double slopeFraction,			// rise/reach
		double aspect,					// degrees clockwise from north
        double midflameWindSpeed,		// ft/min
        double windDirFromUpslope,		// degrees clockwise from North
		bool   applyWindSpeedLimit );

	virtual void setTime( double elapsed );

	// Possibly static methods
	double constrainCompassDegrees( double degrees ) const;
	double calcDegrees( double radians ) const;
	double calcEllipseArea( double length, double lwRatio ) const;
	double calcEllipseEccentricity( double lwRatio ) const;
	double calcEllipseLengthToWidth( double effectiveWindSpeed ) const;
	double calcEllipsePerimeter(
		double length,
		double width ) const;
	double calcEllipsePerimeterRothermel(
		double length,
		double width ) const;
	double calcFirelineIntensity(
		double spreadRate,
		double reactionIntensity,
		double residenceTime ) const;
	double calcFirelineIntensityByram( double flameLength ) const;
	double calcFirelineIntensityThomas( double flameLength ) const;
	double calcFlameLengthByram( double firelineIntensity ) const;
	double calcFlameLengthThomas( double firelineIntensity ) const;
	double calcHeatPerUnitArea(
		double reactionIntensity,
		double residenceTime ) const;
	double calcHerbaceousFuelLoadCuredFraction( double moistureContent ) const;
	double calcRadians( double degrees ) const;
	double calcSpreadRateAtBack(
		double rosHead,
		double eccent ) const;
	double calcSpreadRateAtBeta(
		double rosHead,
		double lwRatio,
        double beta ) const;
	double calcSpreadRateAtFlank(
		double rosHead,
		double lwRatio ) const;

protected:
	int  fuelLife( int lifeCode );
	virtual void resetFuelInput();
	virtual void resetFuelOutput();
	virtual void resetMoistureInput();
	virtual void resetMoistureOutput();
	virtual void resetSiteInput();
	virtual void resetSiteOutput();
	virtual void resetSiteOutputExtension();
	virtual void resetTimeInput();
	virtual void resetTimeOutput();
	virtual void resetTimeOutputExtension();
	virtual void updateFuel();
	virtual void updateMoisture();
	virtual void updateSite( bool applyExtension=true );
	virtual void updateSiteExtension();	// Hook for crown fire to update m_activeCrownFireRos
	virtual void updateTime( bool applyExtension=true );
	virtual void updateTimeExtension();	// Hook for crown fire to update its lwRatio, area, perimeter

protected:
	// constants
	double m_pi;					// close to diameter / radius
	double m_smidgen;				// close to zero
	double m_infinity;				// close to infinity
	// fuel inputs
	double  m_depth;				// fuel bed depth (ft)
	double  m_deadMext;				// fuel bed dead fuel moisture of extinction (lb water / lb ovendry fuel)
	int		m_particles;			// fuel bed number of fuel particle types
	int     m_life[MaxParts];		// fuel particle life category (DeadCat or LiveCat)
	double  m_load[MaxParts];		// fuel particle ovendry load (lb/ft2)
	double  m_savr[MaxParts];		// fuel particle surface area-to-volume ratio (ft3/ft2)
	double  m_heat[MaxParts];		// fuel particle low heat of combustion (btu/lb)
	double  m_dens[MaxParts];		// fuel particle density (lb/ft3)
	double  m_stot[MaxParts];		// fuel particle total Silica content (lb si / lb fuel)
	double  m_seff[MaxParts];		// fuel particle effective silica content (lb si / lb fuel)
    // fuel particle intermediates
    double m_area[MaxParts];		// fuel particle surface area (ft2)
	double m_aWtg[MaxParts];		// fuel particle surface area weighting factor (dl)
	double m_sigK[MaxParts];		// fuel particle effective heating number (dl)
    int    m_size[MaxParts];		// fuel particle sie class (index)
    double m_sWtg[MaxParts];		// fuel particle fuel load weighting factor for its size class (dl)
    // fuel life category intermediates.
    double m_lifeArea[MaxCats];		// life category surface area (ft2)
	double m_lifeAwtg[MaxCats];		// life category surface area weighting factor (dl)
    double m_lifeEtaS[MaxCats];		// life category Silica damping coefficient (dl)
	double m_lifeFine[MaxCats];		//
    double m_lifeHeat[MaxCats];		// life category weighted heat of combustion (btu/lb)
    double m_lifeLoad[MaxCats];		// life category fuel load (lb/ft2)
	double m_lifeRxK[MaxCats];		// life category reaction intensity WITHOUT moisture damping (btu/ft/min)
    double m_lifeSavr[MaxCats];		// life category weighted surface area-to-volume ration (ft3/ft2)
    double m_lifeSeff[MaxCats];		// life category weighted effective SIlica content (lb Si / lb fuel)
    double m_lifeStot[MaxCats];		// life category weighted total Silica content (lb Si / lb fuel )
    double m_lifeSwtg[MaxCats][MaxSizes];	// life category fuel load weighting factors by size class
    // fuel bed intermediates
	double m_aa;					// arbitrary variable 'A' used to derive gammaOpt (dl) [eq 39, p 19]
	double m_betaOpt;				// optium packing ratio (dl) [eq 37]
    double m_betaRatio;				// packing ratio / optimum packing ratio
    double m_bulkDensity;			// fuel bed bulk density (lb/ft3) [eq 40]
	double m_c;						// wind coefficient correlation parameter 'C' (dl) [eq 48, p23]
	double m_e;						// wind coefficient correlation parameter 'E' (dl) [eq 50, p23]
	double m_epsilon;				// fuel bed effective heating number (dl) [eq 14]
	double m_gammaMax;				// maximum reaction velocity (1/min) [eq 36]
	double m_gammaOpt;				// optimum reaction velocity (1/min) [eq 38]
	double m_liveMextK;
    double m_packingRatio;			// fuel bed packing ratio (dl)
	double m_propFlux;				// propagating flux ratio (dl) [eq 42]
	double m_resTime;				// flame residence time (min)
    double m_sigma;					// fuel bed characteristic surface area-to-volume ratio (ft3/ft2)
	double m_sigma15;				// pow( sigma, 1.5)
	double m_slopeK;				// factor used to derive the slope coefficient (dl) [from eq 51]
    double m_totalArea;				// fuel bed total surface area (ft2)
    double m_totalLoad;				// fuel bed total load (lb/ft2)
	double m_windB;					// wind coefficient correlation parameter 'C' [eq 49, p23]
	double m_windE;					// inverse factor of m_windK (dl) [from eq 47]
	double m_windK;					// factor used to derive the wind coefficient (dl) [from eq 47]
	// fuel moisture inputs
	double m_mois[MaxParts];		// fuel particle moisture content (lb water / lb ovendry fuel)
	// fuel moisture outputs
    double m_rbQig;					// fuel bed heat sink (btu/ft3)
    double m_fdmois;
    double m_wfmd;
	double m_deadMois;				// fuel bed weighted dead fuel moisture content (lb water / lb dead fuel)
	double m_liveMois;				// fuel bed weighted live fuel moisture content (lb water / lb live fuel)
	double m_liveMext;				// live fuel moisture of extinction (lb/lb)
    double m_deadEtaM;				// dead fuel moisture damping coefficient (dl) [eq 29]
	double m_deadRxInt;				// dead fuel reaction intensity (btu/ft2/min) [eq 27]
	double m_liveEtaM;				// live fuel moisture damping coefficient (dl) [eq 29]
	double m_liveRxInt;				// live fuel reaction intensity [btu/ft2/min] [eq 27]
	double m_totalRxInt;			// total reaction intensity (btu/ft2/min)
	double m_ros0;					// no-wind, no-slope fire spread rate (ft/min)
	// site (terrain and weather) inputs
    double m_slopeFraction;			// terrain slope (rise/reach)
	double m_aspect;				// terrain aspect (down slope direction) (degrees clockwise from north)
    double m_midflameWindSpeed;		// midflame wind speed (ft/min)
    double m_windDirFromUpslope;	// compass degrees clockwise from north
	bool   m_applyWindSpeedLimit;	//
	// site (terrain and weather) outputs
	double m_rosHead;				// surface fire rate of spread at fire head (ft/min)
    double m_headDirFromUpslope;	// direction of maximum spread (head direction, degrees clowise from north)
    double m_effectiveWindSpeed;	// effective wind speed (ft/min)
    double m_windSpeedLimit;		// upper wind speed limit (ft/min)
    bool   m_windLimitExceeded;		// TRUE if m_rosHead > m_windSpeedLimit
    double m_windFactor;			// wind coefficient (dl)
    double m_slopeFactor;			// slope coefficient (dl)
	bool   m_spreadExceedsWind;		// TRUE if m_rosHead > m_midflameWindSpeed
	int    m_situation;
	double m_lwRatio;				// surface fire perimeter length-to-width ratio (dl)
	double m_eccent;				// surface fire perimeter eccentricity (dl)
	double m_hpua;					// surface fire heat per uniut area (btu/lb)
	double m_rosBack;				// surface fire rate of spread at perimeter back (ft/min)
	double m_rosMajor;				// surface fire rate of spread at perimeter head and back (ft/min)
	double m_rosFlank;				// surface fire rate of spread at peimeret flank (ft/min)
	double m_fliHead;
	double m_fliBack;
	double m_fliFlank;
	double m_flameHead;
	double m_flameBack;
	double m_flameFlank;
	double m_ellipseF;
	double m_ellipseG;
	double m_ellipseH;
	// time inputs and outputs
	double m_elapsed;
	double m_fireLength;
	double m_fireWidth;
	double m_fireArea;
	double m_firePerimeter;
};

#endif // BP6_SURFACE_FIRE_H
