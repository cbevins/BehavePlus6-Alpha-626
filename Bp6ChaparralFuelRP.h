#ifndef BP6_CHAPARRAL_FUEL_RP_H
#define BP6_CHAPARRAL_FUEL_RP_H

/*!	\class Bp6ChaparralFuelRP
 *	\brief Implements the Rothermel & Philpot (1973) "Predicting Changes in
 *	Chaparral Flammability" (Journal of Forestry, Oct 1973).
  */
class Bp6ChaparralFuelRP
{
public:
	Bp6ChaparralFuelRP();
	virtual ~Bp6ChaparralFuelRP();

	// Accessors
	double getAge() const;
	double getDaysSinceMay1() const;
	double getDeadFuelFraction() const;
	double getDeadMext() const;
	double getDens( int life, int size ) const;
	double getFuelBedDepth() const;
	double getHeat( int life, int size ) const;
	double getLoad( int life, int size ) const;
	double getMois( int life, int size ) const;
	double getSavr( int life, int size ) const;
	double getSeff( int life, int size ) const;
	double getStot( int life, int size ) const;
	double getTotalDeadFuelLoad() const;
	double getTotalFuelLoad() const;
	double getTotalLiveFuelLoad() const;
	bool   isChamise() const;
	bool   isMixedBrush() const;

	// Mutators
	void setDeadFuelFraction( double deadFuelFraction );
	void setDeadFuelFractionFromAge( double age );
	void setDepth( double depth );
	void setDepthFromAge( double age );
	void setTotalFuelLoad( double totalFuelLoad );
	void setTotalFuelLoadFromAge( double age );
	void setTypeChamise();
	void setTypeMixedBrush();
	void update();

	// Updaters
	void updateDeadFuelFractionFromAge();
	void updateFuelLoads();
	void updateLiveFuelHeatFromDate();
	void updateLiveFuelMoistureFromDate();

	// Pure virtual updaters specific to the derived fuels
	virtual void updateAgeFromDepth()=0;
	virtual void updateFuelBedDepthFromAge()=0;
	virtual void updateTotalFuelLoadFromAge()=0;

private:
	void init();

protected:
	double m_age;					//!< Fuel age (years since last fire)
	double m_days;					//!< Days since May 1
	double m_deadFuelFraction;		//!< Fraction of total fuel load that is dead
	double m_deadMext;				//!< Dead fuel moisture of extinction content
	double m_fuelBedDepth;			//!< Fuel bed epth (ft)
	double m_dens[2][5];			//!< Fuel particle density (lb/ft3) by life category and size class
	double m_heat[2][5];			//!< Fuel heat of combustion (btu/lb) by life category and size class
	double m_load[2][5];			//!< Fuel load (lb/ft2) by life category and size class
	double m_mois[2][5];			//!< Fuel moisture content (ratio) by life category and size class
	double m_savr[2][5];			//!< Fuel surface area-to-volume ratios by life category and size class
	double m_seff[2][5];			//!< Fuel effective silica content (ratio) by life category and size class
	double m_stot[2][5];			//!< Fuel total silica content (ratio) by life category and size class
	double m_totalDeadLoad;			//!< Total dead fuel load
	double m_totalFuelLoad;			//!< Total fuel load
	double m_totalLiveLoad;			//!< Total live fuel load
};
#endif	// BP6_CHAPARRAL_FUEL_RP_H
