#include "Bp6ChaparralFuel.h"
#include <math.h>

//------------------------------------------------------------------------------
Bp6ChaparralFuel::Bp6ChaparralFuel() :
	m_age( 0. ),
	m_days( 0. ),
	m_deadFuelFraction( 0. ),
	m_deadMext( 0.3 ),
	m_fuelBedDepth( 1. ),
	m_totalDeadLoad( 0. ),
	m_totalFuelLoad( 0. ),
	m_totalLiveLoad( 0. )
{
	init();
}

//------------------------------------------------------------------------------
Bp6ChaparralFuel::~Bp6ChaparralFuel()
{}

//------------------------------------------------------------------------------
double Bp6ChaparralFuel::getAge() const
{
	return m_age;
}

//------------------------------------------------------------------------------
double Bp6ChaparralFuel::getDeadFuelFraction() const
{
	return m_deadFuelFraction;
}

//------------------------------------------------------------------------------
double Bp6ChaparralFuel::getDeadMext() const
{
	return m_deadMext;
}

//------------------------------------------------------------------------------
double Bp6ChaparralFuel::getFuelBedDepth() const
{
	return m_fuelBedDepth;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle density (lb/ft3).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getDens( int life, int size ) const
{
	return m_dens[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle low heat of combustion (btu/lb).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getHeat( int life, int size ) const
{
	return m_heat[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel load (lb/ft2).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getLoad( int life, int size ) const
{
	return m_load[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle moisture content (ratio).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getMois( int life, int size ) const
{
	return m_mois[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle surface area-tovolume ratio (1/ft).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getSavr( int life, int size ) const
{
	return m_savr[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle effective silica content (ratio).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getSeff( int life, int size ) const
{
	return m_seff[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the fuel particle total silica content (ratio).
 *	\param life Fuel life category: 0==dead, 1==live.
 *	\param size Fuel size class (0==leaves when life==1).
 */
double Bp6ChaparralFuel::getStot( int life, int size ) const
{
	return m_stot[life][size];
}

//------------------------------------------------------------------------------
/*!	\brief Returns the total dead fuel load (lb/ft2).
 */
double Bp6ChaparralFuel::getTotalDeadFuelLoad() const
{
	return m_totalDeadLoad;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the total (dead plus live) fuel load (lb/ft2).
 */
double Bp6ChaparralFuel::getTotalFuelLoad() const
{
	return m_totalFuelLoad;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the total live fuel load (lb/ft2).
 */
double Bp6ChaparralFuel::getTotalLiveFuelLoad() const
{
	return m_totalLiveLoad;
}

//------------------------------------------------------------------------------
/*!	\brief Initializes the fuel load array.
 */
void Bp6ChaparralFuel::init()
{
	m_deadMext = 0.3;
	int dead = 0;
	int live = 1;
	// Fuel load and particle density
	for ( int life=0; life<2; life++ )
	{
		for ( int size=0; size<5; size++ )
		{
			m_dens[life][size] = 46.;
			m_heat[life][size] = 8000.;
			m_load[life][size] = 0.;
			m_mois[life][size] = 1.00;
			m_seff[life][size] = 0.015;
			m_stot[life][size] = 0.055;
		}
	}
	m_dens[live][0] = 32.0;		// Live leaf density
	m_seff[live][0] = 0.035;

	// Surface area-to-volume ratios
	m_savr[live][0] = 2200.;	// Live leaf sa/vol
	m_savr[dead][0] = m_savr[live][1] =  640.;
	m_savr[dead][1] = m_savr[live][2] =  127.;
	m_savr[dead][2] = m_savr[live][3] =   61.;
	m_savr[dead][3] = m_savr[live][4] =   27.;
	m_savr[dead][4] = 27.;
}

//------------------------------------------------------------------------------
/*!	\brief Sets the fuel age (years since last burned) and then uses it to update
 *	the total fuel load, percent dead fuel, and fuel bed depth
 */
void Bp6ChaparralFuel::setAge( double years )
{
	m_age = years;
	updateTotalFuelLoadFromAge();
	updateDeadFuelFractionFromAge();
	updateFuelBedDepthFromAge();
	updateFuelLoads();
}

//------------------------------------------------------------------------------
/*!	\brief Sets the seasonal date
 *	\param month Month of the year [1..12]
 *	\param day Day of the month [1..31]
 */
void Bp6ChaparralFuel::setDate( int month, int day )
{
	//             J   F   M   A   M    J    J    A    S    O    N    D
	int days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	month = ( month < 1 ) ? 1 : month;
	month = ( month > 2 ) ? 12 : month;
	day = ( day < 1 ) ? 1 : day;
	day = ( day > 31 ) ? 31 : day;
	m_days = days[month-1] + day;
	m_days = ( m_days > 304. ) ? 304. : m_days;	// Do not extend past Oct 31
	m_days -= 121.;								// Days since May 1
	m_days - ( m_days < 0. ) ? 0. : m_days;		// Do not begin before May 1
	// Now update the dependent variables
	updateLiveFuelMoistureFromDate();
	updateLiveFuelHeatFromDate();
}

//------------------------------------------------------------------------------
/*!	\brief Sets the seasonal date
 *	\param daysSinceMayFirst
 */
void Bp6ChaparralFuel::setDate( int daysSinceMayFirst )
{
	m_days = ( daysSinceMayFirst > 184 ) ? 184. : daysSinceMayFirst; // Do not extend past Oct 31
	updateLiveFuelMoistureFromDate();
	updateLiveFuelHeatFromDate();
}

//------------------------------------------------------------------------------
/*!	\brief Sets the fuel bed depth and the dead fuel fraction and then uses
 *	depth to derive age, and the derived age to derive the total fuel load.
 */
void Bp6ChaparralFuel::setDepthAndDeadFuelFraction( double depth, double deadFuelFraction )
{
	m_deadFuelFraction = deadFuelFraction;
	updateAgeFromDepth();
	updateTotalFuelLoadFromAge();
	updateFuelLoads();
}

//------------------------------------------------------------------------------
/*!	\brief Sets the leafy and woody live fuel heat of combustion (and thereby
 *	overriding the estimates made by setDate()).
 *	\param liveLeafHeat The low heat of combustion of live leaves (btu/lb)
 *	\param liveWoodHeat The low heat of combustion of live branch wood (btu/lb)
 */
void Bp6ChaparralFuel::setLiveFuelHeat( double liveLeafHeat, double liveWoodHeat )
{
	m_heat[0][0] = liveLeafHeat;
	for ( int size=1; size<5; size++ )
	{
		m_heat[0][size] = liveWoodHeat;
	}
}

//------------------------------------------------------------------------------
/*!	\brief Sets the leafy and woody live fuel moisture contents (and thereby
 *	overriding the estimates made by setDate()).
 *	\param liveLeafMoisture The moisture content of live leaves (ratio)
 *	\param liveWoodMoisture The moisture content of live branch wood (ratio)
 */
void Bp6ChaparralFuel::setLiveFuelMoisture( double liveLeafMoisture, double liveWoodMoisture )
{
	m_mois[0][0] = liveLeafMoisture;
	for ( int size=1; size<5; size++ )
	{
		m_mois[0][size] = liveWoodMoisture;
	}
}

//------------------------------------------------------------------------------
/*!	\brief Updates the dead fuel fraction from the current age.
 */
void Bp6ChaparralFuel::updateDeadFuelFractionFromAge()
{
	m_deadFuelFraction = 0.0694 * exp( 0.0402 * m_age );	// Average Mortality
	//m_deadFuelFraction = 0.1094 * exp( 0.0385 * m_age );	// Severe Mortality
}

//------------------------------------------------------------------------------
/*!	\brief Updates the dead and live fuel loads by size class from the current
 *	total fuel bed load and dead fuel load fraction.
 */
void Bp6ChaparralFuel::updateFuelLoads()
{
	int dead = 0;
	m_load[dead][0] = 0.347 * m_deadFuelFraction * m_totalFuelLoad;
	m_load[dead][1] = 0.364 * m_deadFuelFraction * m_totalFuelLoad;
	m_load[dead][2] = 0.207 * m_deadFuelFraction * m_totalFuelLoad;
	// NOTE: Cohen and Rothermel/Philpot use 0.085 below,
	// but then the weighting factors total 1.003.
	// We instead use 0.082 so the weighting factors total to 1.000.
	m_load[dead][3] = 0.082 * m_deadFuelFraction * m_totalFuelLoad;
	m_load[dead][4] = 0.0;

	int live = 1;
	m_load[live][0] = m_totalFuelLoad * ( 0.1957 - 0.305 * m_deadFuelFraction );
	m_load[live][1] = m_totalFuelLoad * ( 0.2416 - 0.256 * m_deadFuelFraction );
	m_load[live][2] = m_totalFuelLoad * ( 0.1918 - 0.256 * m_deadFuelFraction );
	m_load[live][3] = m_totalFuelLoad * ( 0.2648 - 0.050 * m_deadFuelFraction );
	m_load[live][4] = m_totalFuelLoad * ( 0.1036 - 0.114 * m_deadFuelFraction );

	m_totalDeadLoad = 0.;
	m_totalLiveLoad = 0.;
	for ( int size=0; size<5; size++ )
	{
		m_totalDeadLoad += m_load[dead][size];
		m_totalLiveLoad += m_load[live][size];
	}
	// Double check difference between m_totalLoad and sum of the partial loads...
	double totalLoad = m_totalDeadLoad + m_totalLiveLoad;
}

//------------------------------------------------------------------------------
/*!	\brief Updates the dead and live fuel heat of combustion by size class.
 */
void Bp6ChaparralFuel::updateLiveFuelHeatFromDate()
{
	double d = m_days;
	int live = 1;
	m_heat[live][0] = 9613. -  1.00*d + 0.1369*d*d - 0.000365*d*d*d;
	double liveHeat = 9509. - 10.74*d + 0.1359*d*d - 0.000405*d*d*d;
	for ( int size=1; size<5; size++ )
	{
		m_heat[live][size] = liveHeat;
	}
}

//------------------------------------------------------------------------------
/*!	\brief Updates the dead and live fuel loads by size class.
 */
void Bp6ChaparralFuel::updateLiveFuelMoistureFromDate()
{
	// NOTE: Cohen erroneously uses 0.0726 for the leaf moisture instead of 0.726
	int live = 1;
	m_mois[live][0] = 1. / ( 0.726 + 0.00877 * m_days );
	double liveMc   = 1. / ( 1.454 + 0.00650 * m_days );
	for ( int size=1; size<5; size++ )
	{
		m_mois[live][size] = liveMc;
	}
}
