#include "Bp6ChaparralFuelMixedBrush.h"
#include <math.h>

//------------------------------------------------------------------------------
Bp6ChaparralFuelMixedBrush::Bp6ChaparralFuelMixedBrush() :
	Bp6ChaparralFuel()
{}

//------------------------------------------------------------------------------
Bp6ChaparralFuelMixedBrush::~Bp6ChaparralFuelMixedBrush()
{}

//------------------------------------------------------------------------------
/*!	\brief Updates the fuel age from the current fuel bed depth.
 */
void Bp6ChaparralFuelMixedBrush::updateAgeFromDepth()
{
	m_age = exp( 3.912023 * sqrt( m_fuelBedDepth / 10. ) );
}

//------------------------------------------------------------------------------
/*!	\brief Updates the fuel bed depth from the current age.
 */
void Bp6ChaparralFuelMixedBrush::updateFuelBedDepthFromAge()
{
	double x = log( m_age ) / 3.912023;
	m_fuelBedDepth = 10. * x * x;
}

//------------------------------------------------------------------------------
/*!	\brief Updates the total fuel load from the current age.
 */
void Bp6ChaparralFuelMixedBrush::updateTotalFuelLoadFromAge()
{
	double tpa = m_age / ( 0.4849 + 0.0170 * m_age );
	m_totalFuelLoad = tpa * 2000. / 43560.;
}
