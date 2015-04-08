/*!	\file BehavePlus6/Algorithms/Fofem6Mortality.cpp
*	\brief Fofem6Mortality class definition.
*/

#include <cmath>
#include <cstdio>

#include <qstring.h>

#include "Fofem6Mortality.h"
#include "Fofem6Species.h"

//------------------------------------------------------------------------------
/*! \brief Calculates probability of tree mortality using the FOFEM 6.0
 *  equations for trees with dbh >= 1.
 *
 *  This is only a partial implementation of the FOFEM mortality algorithm.
 *  Specifically, it only implements those cases where the tree dbh >= 1".
 *  It also excludes the FOFEM special case of \e Populus \e tremuloides,
 *  which requires additional inputs (namely, flame height and fire severity).
 *
 *  \param equationId The FOFEM v6mortality equation number (1, 3, 5, 10-12, 14-20)
 *  \param dbh Diameter at breast height (in)
 *  \param barkThickness Tree bark thickness (in).
 *  \param scorchHt Scorch ht (ft)
 *  \param crownLengScorched Fraction of the crown length that is scorched (ft2/ft2).
 *  \param crownVolScorched Fraction of the crown volume that is scorched (ft3/ft3).
 *
 *  \return Tree mortality probability [0..1].
 */

double Fofem6Mortality::mortalityRate(
	int    equationId,
	double dbh,					// in
	double barkThickness,		// in
	double scorchHt,			// ft
	double crownLengScorched,	// fraction
    double crownVolScorched		// fraction
)
{
	double mr = 0.0;
	double bt = 2.54 * barkThickness;
    double cls = 100. * crownLengScorched;
    double cvs = 100. * crownVolScorched;

    // Pat requested that if scorch ht is zero, then mortality is zero
    if ( scorchHt < 0.0001 )
    {
        return( mr );
    }

	// Equation 1 is the default mortality equation for all species with dbh > 1"
	if ( equationId == 1 )
	{
		mr = -1.941
           + 6.316 * ( 1.0 - exp( -barkThickness ) )
           - 5.35 * crownVolScorched * crownVolScorched;
	    mr = 1.0 / ( 1.0 + exp( mr ) );
	}
	// Equation 3 is for spruce species;
	// its the same as Equation 1 but with a minimum value of 0.8
	else if ( equationId == 3 )
	{
		mr = -1.941
           + 6.316 * ( 1.0 - exp( -barkThickness ) )
           - 5.35 * crownVolScorched * crownVolScorched;
	    mr = 1.0 / ( 1.0 + exp( mr ) );
		mr = ( mr < 0.8 ) ? 0.8 : mr;
	}
	// Equation 5 is specifically for Pinus palustris (longleaf pine)
	// Note that bark thickness is in cm
	else if ( equationId == 5 )
	{
		// This equation uses CVS as a scale of 1-10
		cvs = cvs / 10.;
		mr = 0.169
		   + ( 5.136 * bt )
		   + ( 14.492 * bt * bt )
		   - ( 0.348 * cvs * cvs );
	    mr = 1.0 / ( 1.0 + exp( mr ) );
	}
	// Equation 10 is specifically for Abies concolor (white fir)
	else if ( equationId == 10 )
	{
        mr = -3.5083
           + ( 0.0956 * cls )
           - ( 0.00184 * cls * cls )
           + ( 0.000017 * cls * cls * cls );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 11 is specifically for Abies lasiocarpa (subalpine fir)
	// and Abies grandis (grad fir)
	else if ( equationId == 11 )
	{
        mr = -1.6950
           + ( 0.2071 * cvs )
           - ( 0.0047 * cvs * cvs )
           + ( 0.000035 * cvs * cvs * cvs );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 12 is specifically for Libocedrus decurrens (incense cedar)
	else if ( equationId == 12 )
	{
        mr = -4.2466 + 0.000007172 * cls * cls * cls;
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 14 is specifically for Larix occidentalis (western larch)
	// Note that this is from Hood, so dbh is in cm
	else if ( equationId == 14 )
	{
        mr = -1.6594
           + ( 0.0327 * cvs )
           - ( 0.0489 * (2.54 * dbh) );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 15 is specifically for Picea engelmannii (Englemann spruce)
	else if ( equationId == 15 )
	{
        mr = 0.0845 + ( 0.0445 * cvs );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 16 is specifically for Abies magnifica (red fir)
	else if ( equationId == 16 )
	{
        mr = -2.3085 + 0.000004059 * cls * cls * cls;
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 17 is specifically for Pinus albicaulis (whitebark pine)
	// and Pinus contorta (lodgepole pine)
	// Note that this is from Hood, so dbh is in cm
	else if ( equationId == 17 )
	{
        mr = -0.3268
           + ( 0.1387 * cvs )
           - ( 0.0033 * cvs * cvs )
           + ( 0.000025 * cvs * cvs * cvs )
           - ( 0.0266 * (2.54 * dbh) );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 18 is specifically for Pinus lambertiana (sugar pine)
	else if ( equationId == 18 )
	{
        mr = -2.0588 + ( 0.000814 * cls * cls );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 19 is specifically for Pinus ponderosa (ponderosa pine)
	// amd Pinus jeffreyi (Jeffry pine)
	else if ( equationId == 19 )
	{
        mr = -2.7103 + ( 0.000004093 * cvs * cvs * cvs );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
	// Equation 20 is specifically for Pseudotsuga menziesii (Douglas-fir)
	else if ( equationId == 20 )
	{
        mr = -2.0346
           + ( 0.0906 * cvs )
           - ( 0.0022 * cvs * cvs )
           + ( 0.000019 * cvs * cvs * cvs );
	    mr = 1.0 / ( 1.0 + exp( -mr ) );
	}
    // UNKNOWN EQUATION INDEX
    else
    {
		fprintf( stderr, "Fofem6Mortality::mortalityRate(): invalid mortality equation id %d.",
            equationId );
        return( 0.0 );
    }

    // Confine mortality results to range [0..1].
	mr = ( mr > 1.0 ) ? 1.0 : mr;
	mr = ( mr < 0.0001 ) ? 0.0 : mr;
    return( mr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates probability of tree mortality using the FOFEM 6.0
 *  equations for trees with dbh >= 1.
 *
 *	This is a convenience method that looks up the mortality equation number
 *	for the FOFEM v6 tree species code passed as the first argument.
 *
 *  This is only a partial implementation of the FOFEM mortality algorithm.
 *  Specifically, it only implements those cases where the tree dbh >= 1".
 *  It also excludes the FOFEM special case of \e Populus \e tremuloides,
 *  which requires additional inputs (namely, flame height and fire severity).
 *
 *  \param fofem6speciesCode Pointer to the FOFEM v6 tree species code.
 *  \param dbh Diameter at breast height (in)
 *  \param barkThickness Tree bark thickness (in).
 *  \param scorchHt Scorch ht (ft)
 *  \param crownLengScorched Fraction of the crown length that is scorched (ft2/ft2).
 *  \param crownVolScorched Fraction of the crown volume that is scorched (ft3/ft3).
 *
 *  \return Tree mortality probability [0..1].
 */

double Fofem6Mortality::mortalityRate(
	const char* fofem6SpeciesCode,
	double dbh,					// in
	double barkThickness,		// in
	double scorchHt,			// ft
	double crownLengScorched,	// fraction
    double crownVolScorched		// fraction
)
{
	QString spp = QString( fofem6SpeciesCode );
	int idx = Fofem6Species::getFofem6MortalityEquationIndex( spp.upper().latin1() );
	if ( idx >= 0 )
	{
		return mortalityRate( idx, dbh, barkThickness, scorchHt, crownLengScorched,
			crownVolScorched );
	}
	return 0.;
}
