#ifndef BP6_CHAPARRAL_FUEL_H
#define BP6_CHAPARRAL_FUEL_H

/*!	\class Bp6ChaparralFuel
 *	\brief Implements the Rothermel & Philpot (1973) "Predicting Changes in
 *	Chaparral Flammability" (Journal of Fotestry, Oct 1973) including
 *	modifications made by Jack Cohen in the FIRECAST program.
*
 *	The purpose of the R&P paper was to assess seasonal and year-to-year
 *	changes in chaparral flammability.  This was accomplished by providing
 *	chaparral fuel descriptors to the Rothermel fire spread model.
 *
 *	The Rothermel fire spread model requires as input the (1) fuel bed depth,
 *	(2) fuel bed dead extinction moisture, and (3) fuel particle size, load,
 *	density, heat content, moisture content, and silica content by life
 *	category and size class.
 *
 *	A major portion of the R&P paper is devoted to the derivation of the
 *	chaparral fuel descriptors over time (with the remainder devoted to the
 *	actual assessment of flammability over time).  Towards this end, R&P:
 *	- applied a constant dead fuel extinction of 0.4 (Cohen uses 0.3);
 *	- applied constant fuel particle surface area-to-volume ratio, density,
 *		and silica content by life category and size class; and
 *	- applied constant dead fuel particle heat of combustion of 8000 btu/lb.
 *
 *	The remaining parameters are either age-dependent (fuel depth and load)
 *	or season-dependent (live fuel moisture and live heat of combustion).
 *
 *	First R&P developed a set of equations to apportion fuel load amongst the
 *	life and size classes based upon the total fuel load and a dead fuel fraction.
 *	They then provide a relationship between total fuel load and age,
 *	and between dead fuel fraction and age.  Finally, they developed a relationship
 *	between fuel bed depth and age.
 *
 *	R&P wanted to include seasonal effects on flammability, so they also
 *	developed a relationship between live fuel heat content and seasonal date,
 *	and between live fuel moisture content and seasonal date.
 *
 *	We can summarize the capabilities of the R&P chaparral fuel models with
 *	respect to fire behavior modeling as follows:
 *	- fuel bed depth can be (1) specified or (2) estimated from age;
 *	- total fuel load can be (1) specified or (2) estimated from age;
 *	- dead fuel fraction can be (1) specified or (2) estimated from age;
 *	- live fuel heat content can be (1) specified or (2) estimated from seasonal date;
 *	- live fuel moisture content can be (1) specified or (2) estimated from seasonal date;
 *	- dead fuel moisture content must be specified;
 *	- all other fuel model parameters are constant over time, including:
 *		- dead fuel moisture of extinction
 *		- fuel particle surface area-to-volume ratio by life category and size class;
 *		- fuel particle density by life category and size class;
 *		- fuel particle total and effective silica content by life category and size class;
 *		- dead fuel particle heat content by size class;
 */
class Bp6ChaparralFuel
{
public:
	Bp6ChaparralFuel();
	virtual ~Bp6ChaparralFuel();

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

	// Mutators that automatically update other parameters
	void setAge( double years );
	void setDate( int daysSinceMayFirst );
	void setDate( int month, int day );
	void setDepthAndDeadFuelFraction( double depth, double deadFuelFraction );

	// Mutators that override previously estimated parameters
	void setLiveFuelHeat( double liveLeafHeat, double liveWoodHeat );
	void setLiveFuelMoisture( double liveLeafMoisture, double liveWoodMositure );

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
#endif	// BP6_CHAPARRAL_FUEL_H
