#include "Bp6ChaparralFuelChamise.h"
#include <math.h>

//------------------------------------------------------------------------------
Bp6ChaparralFuelChamise::Bp6ChaparralFuelChamise() :
	Bp6ChaparralFuel()
{}

//------------------------------------------------------------------------------
Bp6ChaparralFuelChamise::~Bp6ChaparralFuelChamise()
{}

//------------------------------------------------------------------------------
/*!	\brief Updates the fuel age from the current fuel bed depth.
 */
void Bp6ChaparralFuelChamise::updateAgeFromDepth()
{
	m_age = exp( 3.912023 * sqrt( m_fuelBedDepth / 7.5 ) );
}

//------------------------------------------------------------------------------
/*!	\brief Updates the fuel bed depth from the current age.
 */
void Bp6ChaparralFuelChamise::updateFuelBedDepthFromAge()
{
	double x = log( m_age ) / 3.912023;
	m_fuelBedDepth = 7.5 * x * x;
}

//------------------------------------------------------------------------------
/*!	\brief Updates the total fuel load from the current age.
 */
void Bp6ChaparralFuelChamise::updateTotalFuelLoadFromAge()
{
	double tpa = m_age / ( 1.4459 + 0.0315 * m_age );
	m_totalFuelLoad = tpa * 2000. / 43560.;
}
