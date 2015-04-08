/*!	\file BehavePlus6/Algorithms/Fofem6BarkThickness.cpp
*	\brief Fofem6BarkThickness class definition.
*/

#include <cmath>

#include <qstring.h>

#include "Fofem6BarkThickness.h"
#include "Fofem6Species.h"

//------------------------------------------------------------------------------
/*! \brief Calculates tree bark thickness using the FOFEM v6 equations.
 *
 *  \param equationId Bark thickness equation number.
 *  \param dbh Tree diameter at breast height (in).
 *
 *  \return Tree bark thickness (in).
 */
double Fofem6BarkThickness::barkThickness( int equationId, double dbh )
{
	// Fofem factors for determining Single Bark Thickness.
	// Each FOFEM species has a SBT equation index "barkEq" [1-39] into this array.
	static double Fofem_Sbt[] =
	{
		/* 00 */    0.000,      // Not used
		/* 01 */    0.019,      // Not used
		/* 02 */    0.022,
		/* 03 */    0.024,
		/* 04 */    0.025,
		/* 05 */    0.026,
		/* 06 */    0.027,
		/* 07 */    0.028,
		/* 08 */    0.029,
		/* 09 */    0.030,
		/* 10 */    0.031,
		/* 11 */    0.032,
		/* 12 */    0.033,
		/* 13 */    0.034,
		/* 14 */    0.035,
		/* 15 */    0.036,
		/* 16 */    0.037,
		/* 17 */    0.038,
		/* 18 */    0.039,
		/* 19 */    0.040,
		/* 20 */    0.041,
		/* 21 */    0.042,
		/* 22 */    0.043,
		/* 23 */    0.044,
		/* 24 */    0.045,
		/* 25 */    0.046,
		/* 26 */    0.047,
		/* 27 */    0.048,
		/* 28 */    0.049,
		/* 29 */    0.050,
		/* 30 */    0.052,
		/* 31 */    0.055,
		/* 32 */    0.057,      // Not used
		/* 33 */    0.059,
		/* 34 */    0.060,
		/* 35 */    0.062,
		/* 36 */    0.063,		// Changed from 0.065 to 0.063 in Build 606
		/* 37 */    0.068,
		/* 38 */    0.072,
		/* 39 */    0.081,
		/* 40 */    0.000,		// Reserved for Pinus palustrus (longleaf pine)
	};

	double bt = 0.;
	// In FOFEM 6, longleaf pine has its own bark thickness formula
	// and uses dbh in cm
	if ( equationId == 40 )
	{
		dbh = 2.54 * dbh;				// dbh in cm
		bt = 0.435 +  0.031 * dbh;		// bark thickness in cm
		bt = bt / 2.54;					// bark thickness in inches
	}
	else if ( equationId >= 0 && equationId < 40 )
	{
		bt = Fofem_Sbt[ equationId ] * dbh;
	}
	return bt;
}

//------------------------------------------------------------------------------
/*! \brief Calculates tree bark thickness using the FOFEM v6 equations.
 *
 *	This is a convenience method that looks up the bark thickness equation number
 *	for the FOFEM v6 tree species code passed as the first argument.
 *
 *  \param fofem6speciesCode Pointer to the FOFEM v6 tree species code.
 *  \param dbh Tree diameter at breast height (in).
 *
 *  \return Tree bark thickness (in).
 */
double Fofem6BarkThickness::barkThickness(
	const char* fofem6SpeciesCode,
	double dbh					// in
)
{
	QString spp = QString( fofem6SpeciesCode );
	int idx = Fofem6Species::getFofem6BarkEquationIndex( spp.upper().latin1() );
	if ( idx >= 0 )
	{
		return barkThickness( idx, dbh );
	}
	return 0.;
}
