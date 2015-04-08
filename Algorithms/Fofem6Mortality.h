#ifndef BEHAVEPLUS6_ALGORITHMS_FOFEM6MORTALITY
#define BEHAVEPLUS6_ALGORITHMS_FOFEM6MORTALITY

/*!	\file BehavePlus6/Algorithms/Fofem6Mortality.h
*	\brief Fofem6Mortality class declaration.
*/

/*!	\class Fofem6Mortality
*	\brief Implements the FOFEM v6 tree mortality equations.
*/
class Fofem6Mortality
{
public:
	static double mortalityRate(
		int    equationId,			//!> 1,3,5,10-12,14-20
		double dbh,					//!> in
		double barkThickness,		//!> in
		double scorchHt,			//!> ft
		double crownLengScorched,	//!> fraction
		double crownVolScorched );	//!> fraction

	static double mortalityRate(
		const char* fofem6SpeciesCode,
		double dbh,					//!> in
		double barkThickness,		//!> in
		double scorchHt,			//!> ft
		double crownLengScorched,	//!> fraction
		double crownVolScorched );	//!> fraction
};

#endif	// BEHAVEPLUS6_ALGORITHMS_FOFEM6MORTALITY
