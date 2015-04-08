#ifndef BP6_CROWN_FIRE_H
#define BP6_CROWN_FIRE_H

#include "Bp6SurfaceFire.h"

/*!	\class Bp6CrownFire
 *	\brief Implements the Rothermel (1991) and the Scott & Reinhardt
 *	crown fire spread model.
 *
 *	\par Usage
	// Step 1 - create the crown fire object
	Bp6CrownFire* cf = new Bp6CrownFire();

	// Step 2 - set the 1-h, 10-h, 100-h, and live woody fuel moisture contents
	double mois[] = { 0.05, 0.05, 0.05, 1.0 );
	cf->setMoisture( mois );

	// Step 3 - set the wind conditions
	cf->setWind( windSpeedAt20Ft );

	// At this point we can request the following crown fire output variables:
	cf->getActiveCrownFireRos();	// spread rate (ft/min) of a potential active crown fire

	// Step 4 - set the crown canopy conditions
	cf->setCanopy(
		double canopyHeight,		// distance from surface fuel to top of the canopy (ft)
		double canopyBaseHeight,	// distance from surface fuel to base of the canopy (ft)
		double canopyBulkDensity,	// canopy bulk density (lb fuel / ft3 canopy)
		double canopyFoliarMoisture,// canopy foliar moisture content (lb water/lb ovendry foliage)
		double canopyHeat );		// canopy fuel low heat of combustion, defaults to 7732.64 btu/lb (18,000 kJ/kg)

	// From the above, we can obtain the following crown fire characteristcs:
	cf->getActiveCrownFireRatio();	// Rothermel's active crowning ratio (dl)
	cf->getCanopyFuelLoad();		// crown canopy total fuel load (lb/ft2)
	cf->getCanopyHpua();			// crown canopy heat per unit area (btu/lb)
	cf->getCriticalCrownFireRos();	// crown fire spread rate required to sustain active crowning (ft/min)
	cf->getCriticalSurfaceFireFlame();// critical surface fire flame length to initiate crowning (ft)
	cf->getCriticalSurfaceFireFli();// critical surface fireline intensity to initiate crowning (btu/ft/s)
	cf->getCrownFireLwRatio();		// crown fire ellipse length-to-width ratio (dl)
	cf->getPowerWind();				// Rothermel's measure of the power of the wind (ft-lb/ft2/s)

	// Step 5 - set the surface fire behavior characteristics
	cf->setSurfaceFire(
		spreadRate,					// surface fire spread rate (ft/min)
		firelineIntensity,			// surface fireline intensity (btu/ft/s)
		heatPerUnitArea );			// surface fire heat per unit area (btu/ft2)

	// Alternatively, could pass ina reference to a Bp6SurfaceFire object instance:
	cf->setSurfaceFire( bp6SurfaceFirePtr );

	// At this point, we can request the following additional crown fire characteristics:
	cf->getActiveCrownFireFlame();	// flame length (ft) of a potential active crown fire
	cf->getActiveCrownFireFli();	// fireline intensity (btu/ft/s) of a potential active crown fire
	cf->getActiveCrownFireHpua();	// heat per unit area (btu/ft2) of a potential active crown fire
	cf->getCriticalSurfaceFireRos();// critical surface fire spread rate to initiate crown fire (ft/min)
	cf->getCrownFractionBurned();	// fraction of available canopy fuels consumed by crown fire(ratio)
	cf->getFinalFireFlame();		// flame length (ft) of the final fire type
	cf->getFinalFireFli();			// fireline intensity (btu/ft/s) of the final fire type
	cf->getFinalFireHpua();			// heat per unit area (btu/ft2) of the final fire type
	cf->getFinalFireRos();			// spread rate (ft/min) of the final fire type
	cf->getFinalFireType();			// 0=surface, 1=passive (torching), 2=conditional active, 3=active crown
	cf->getFullCrownFireU20();		// wind speed at 20 ft (ft/min) required to sustain a fully active crown fire
	cf->getFullCrownFireRos();		// surface fire spread rate (ft/min) required to sustain a fully active crown fire
	cf->getPassiveCrownFireFlame();	// flame length (ft) of a potential passive/torching crown fire
	cf->getPassiveCrownFireFli();	// fireline intensity (btu/ft/s) of a potential passive/torching crown fire
	cf->getPassiveCrownFireHpua();	// heat per unit area (btu/ft2) of a potential passive/torching crown fire
	cf->getPassiveCrownFireRos();	// spread rate (ft/min) of a potential passive (torching) crown fire
	cf->getPowerFire();				// power of the fire (ft-lb/ft2/s)
	cf->getPowerRatio();			// ratio of power-of-the-fire to the power-of-the-wind
	cf->getTransRatio();			// ratio of actual-to-critical surface fireline intensity (dl)
	cf->isActiveCrownFire();		// TRUE if final fire type =3 (active crown)
	cf->isCrownFire();				// TRUE if final fire type = 1 (passive/torching) or 3 (active crown)
	cf->isPassiveCrownFire();		// TRUE if final fire type = 1 (passive/torching)
	cf->isPlumeDominated();			// TRUE if power of the wind < power of the fire
	cf->isSurfaceFire();			// TRUE if final fire type is 0 (surface fire) or 2
	cf->isWindDriven();				// TRUE if power of the fire > power of the wind

	// Step 6 - set the crown fire elasped time (min) since ignition:
	cf->setTime(
		elapedMinutes );			// elapsed minutes since fire ignition

	// At this point, we can request the following additional crown fire characteristics:
	cf->getActiveCrownFireArea();	// fire area (ft2) of a potential active crown fire
	cf->getActiveCrownFireLength();	// forward spread distance (ft) of a potential active crown fire
	cf->getActiveCrownFirePerimeter(); // fire perimeter (ft) of a potential active crown fire
	cf->getActiveCrownFireWidth();	// fire width (ft) of a potential active crown fire
	cf->getPassiveCrownFireArea();	// fire area (ft2) of a potential passive (torching) crown fire
	cf->getPassiveCrownFireLength();// forward spread distance (ft) of a potential passive (torching) crown fire
	cf->getPassiveCrownFirePerimeter();// fire perimeter (ft) of a potential passive (torching) crown fire
	cf->getPassiveCrownFireWidth();	// fire width (ft) of a potential passive (torching) crown fire
 */
class Bp6CrownFire : public Bp6SurfaceFire
{
public:
	Bp6CrownFire();
	virtual ~Bp6CrownFire();

	// Accessors
	double getActiveCrownFireArea() const;
	double getActiveCrownFireFlame() const;
	double getActiveCrownFireFli() const;
	double getActiveCrownFireHpua() const;
	double getActiveCrownFireLength() const;
	double getActiveCrownFirePerimeter() const;
	double getActiveCrownFireRatio() const;
	double getActiveCrownFireRos() const;
	double getActiveCrownFireWidth() const;
	double getCanopyFuelLoad() const;
	double getCanopyHpua() const;
	double getCriticalCrownFireRos() const;
	double getCriticalSurfaceFireFlame() const;
	double getCriticalSurfaceFireFli() const;
	double getCriticalSurfaceFireRos() const;
	double getCrownFireLwRatio() const;
	double getCrownFractionBurned() const;
	double getFinalFireFlame() const;
	double getFinalFireFli() const;
	double getFinalFireHpua() const;
	double getFinalFireRos() const;
	int	   getFinalFireType() const;
	double getFullCrownFireU20() const;
	double getFullCrownFireRos() const;
	double getPassiveCrownFireArea() const;
	double getPassiveCrownFireFlame() const;
	double getPassiveCrownFireFli() const;
	double getPassiveCrownFireHpua() const;
	double getPassiveCrownFireLength() const;
	double getPassiveCrownFirePerimeter() const;
	double getPassiveCrownFireRos() const;
	double getPassiveCrownFireWidth() const;
	double getPowerFire() const;
	double getPowerWind() const;
	double getPowerRatio() const;
	Bp6SurfaceFire* getSurfaceFire() const;
	double getTransRatio() const;
	bool   isActiveCrownFire() const;
	bool   isCrownFire() const;
	bool   isPassiveCrownFire() const;
	bool   isPlumeDominated() const;
	bool   isSurfaceFire() const;
	bool   isWindDriven() const;

	// Extensions
	virtual void updateSiteExtension();
	virtual void resetSiteOutputExtension();
	virtual void updateTimeExtension();
	virtual void resetTimeOutputExtension();

	// New class methods
	void resetCanopyInput();
	void resetCanopyOutput();
	void resetSurfaceFireInput();
	void resetSurfaceFireOutput();

	void setCanopy(
		double canopyHeight,
		double canopyBaseHeight,
		double canopyBulkDensity,
		double canopyFoliarMoisture,
		double canopyHeat=8000. );	// 7732.64 btu/lb = 18,000 kJ/kg
	// This masks the Bp6SurfaceFire version of setSite() since its not implemented!
	virtual void setSite(
        double slopeFraction,			// rise/reach
		double aspect,					// degrees clockwise from north
        double midflameWindSpeed,		// ft/min
        double windDirFromUpslope,		// degrees clockwise from North
		bool   applyWindSpeedLimit );
	void setSurfaceFire( Bp6SurfaceFire* surfaceFire );
	void setSurfaceFire(
		double ros,
		double fli,
		double hpua );
	// Use this instead of setSite()
	void setWind( double windSpeedAt20ft );	// wind speed at 20 ft (ft/min)
	virtual void updateCanopy();
	virtual void updateSurfaceFire();

	// Static methods
	double calcCriticalCrownFireSpreadRate( double canopyBulkDensity ) const;

	double calcCriticalSurfaceFireIntensity(
		double foliarMoisture,
		double canopyBaseHeight ) const;

	double calcCriticalSurfaceFireSpreadRate(
			double criticalSurfaceFireIntensity,
			double surfaceFireHpua ) const;

	double calcCrownFireActiveRatio(
		double crownSpreadRate,
		double criticalSpreadRate ) const;

	double calcCrownFireLengthToWidth( double windSpeedAt20ft ) const;

	double calcCrownFractionBurned(
		double surfaceFireRos,
		double criticalSurfaceFireRos,
		double crowningSurfaceFireRos ) const;

	double calcCrownFuelLoad(
		double canopyBulkDensity,
		double canopyHt,
		double baseHt ) const;
	
	double calcCrowningIndex(
		double canopyBulkDensity,
		double reactionIntensity,
		double heatSink,
		double slopeFactor ) const;

	int calcFireType(
		double tranRatio,
		double activeRatio ) const;

	double calcPowerOfFire( double crownFirelineIntensity ) const;

	double calcPowerOfWind(
		double windSpeedAt20ft,
		double spreadRate ) const;

	double calcTransitionRatio(
		double surfaceFireIntensity,
		double critcalSurfaceFireIntensity ) const;

private:
	void init();

private:
	// Canopy inputs
	double m_canopyBaseHeight;
	double m_canopyBulkDensity;
	double m_canopyFoliarMois;
	double m_canopyHeat;
	double m_canopyHeight;
	double m_windSpeedAt20ft;
	// Canopy-dependent outputs
	double m_activeCrownFireRatio;
	double m_canopyFuelLoad;
	double m_canopyHpua;
	double m_criticalCrownFireRos;
	double m_criticalSurfaceFireFlame;
	double m_criticalSurfaceFireFli;
	double m_crownFireLwRatio;
	double m_powerWind;
	// Surface fire inputs
	Bp6SurfaceFire* m_surfaceFire;
	double m_surfaceFireFli;
	double m_surfaceFireHpua;
	double m_surfaceFireRos;
	// Surface fire and canopy dependent outputs
	double m_activeCrownFireFlame;
	double m_activeCrownFireFli;
	double m_activeCrownFireHpua;
	double m_criticalSurfaceFireRos;
	double m_crownFractionBurned;
	double m_finalFireFlame;
	double m_finalFireFli;
	double m_finalFireHpua;
	double m_finalFireRos;
	int	   m_finalFireType;				// 0=surface, 1=passive, 2=conditional crown, 3=crown
	bool   m_isActiveCrownFire;
	bool   m_isCrownFire;
	bool   m_isPassiveCrownFire;
	bool   m_isPlumeDominated;
	bool   m_isSurfaceFire;
	bool   m_isWindDriven;
	double m_fullCrownFireU20;
	double m_fullCrownFireRos;
	double m_passiveCrownFireFlame;
	double m_passiveCrownFireFli;
	double m_passiveCrownFireHpua;
	double m_passiveCrownFireRos;
	double m_powerFire;
	double m_powerRatio;
	double m_transRatio;
	// updateSiteExtension() outputs
	double m_activeCrownFireRos;
	// updateTimeExtension() outputs
	double m_activeCrownFireArea;
	double m_activeCrownFireLength;
	double m_activeCrownFirePerimeter;
	double m_activeCrownFireWidth;
	double m_passiveCrownFireArea;
	double m_passiveCrownFireLength;
	double m_passiveCrownFirePerimeter;
	double m_passiveCrownFireWidth;
};
#endif	// BP6_CROWN_FIRE_H
