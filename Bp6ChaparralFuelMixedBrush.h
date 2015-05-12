#ifndef BP6_CHAPARRAL_FUEL_MIXED_BRISH_H
#define BP6_CHAPARRAL_FUEL_MIXED_BRISH_H

#include "Bp6ChaparralFuel.h"

/*!	\class Bp6ChaparralFuelMixedBrush
 *	\brief Implements the Rothermel & Philpot (1973) "Predicting Changes in
 *	Chaparral Flammability" (Journal of Fotestry, Oct 1973) including
 *	modifications made by Jack Cohen in the FIRECAST program.
 *
 *	The model estimates live and dead fuel loads from a combination of the
 *	following input parameters:
 *	- fuel age
 *	- fuel bed depth
 *	- dead fuel load fraction
 *
 *	The live and dead fuel loads are then estimated by one of these methods:
 *	1 specify the age parameter, which calculates the fuel bed depth,
 *		and dead fuel load fraction parameters, or
 *	2 specify the fuel bed depth and dead fuel load fraction parameters,
 *		which estimates the fuel age parameter.
 *
 *	The model can estimate the live fuel moisture content and live fuel heat
 *	of combustion by specifying the seasonal date, either as month and day, or
 *	as number of days since May 1.  Otherwise, the client must specify the
 *	live leaf and live wood moistures and heat contents directly.
 *
 *	\par Usage
	// Step 1 - create the chaparral mixed brush fuel object
	Bp6ChaparralFuelMixedBrush* cf = new Bp6ChaparralFuelMixedBrush();

	// Step 2 - initialize the fuel loads using one of these methods:
	// Step 2a - input the brush age
	cf->setAge( ageInYears );
	// Step 2b - or input the fuel bed depth and dead fuel fraction
	cf->setDepthAndDeadFuelFraction( depthInFt, deadFuelFraction );

	// Step 3 - initialize the live fuel moistures and heats of combustion
	//	using one of these methods (or back-to-back to override previous estimates)
	cf->setDate( month, day );		// estimates the live fuel moisture and heat contens
	cf->setDate( daysSinceMay1 );	// estimates the live fuel moisture and heat contents
	cf->setLiveFuelHeat( liveLeafHeat, liveWoodHeat );
	cf->setLiveFuelMoisture( liveLeafMoisture, liveWoodMoisture );

	// Step 4 - access model variables
 */
class Bp6ChaparralFuelMixedBrush : public Bp6ChaparralFuel
{
public:
	Bp6ChaparralFuelMixedBrush();
	virtual ~Bp6ChaparralFuelMixedBrush();

	// Mutators
	virtual void updateAgeFromDepth();
	virtual void updateFuelBedDepthFromAge();
	virtual void updateTotalFuelLoadFromAge();

private:
};
#endif	// BP6_CHAPARRAL_FUEL_MIXED_BRISH_H
