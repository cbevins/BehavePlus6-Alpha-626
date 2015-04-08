#ifndef BEHAVEPLUS6_ALGORITHMS_FOFEM6BARKTHICKNESS
#define BEHAVEPLUS6_ALGORITHMS_FOFEM6BARKTHICKNESS
/*!	\file BehavePlus6/Algorithms/Fofem6Mortality.h
*	\brief Fofem6Mortality class declaration.
*/

/*!	\class Fofem6BarkThicness
*	\brief Implements the FOFEM v6 tree bark thickness equations.
*/
class Fofem6BarkThickness
{
public:
	static double barkThickness(
		int equationId,		//!> 0-40
		double dbh );		//!> in

	static double barkThickness(
		const char* fofem6SpeciesCode,
		double dbh );		//!> in

};

#endif	// BEHAVEPLUS6_ALGORITHMS_FOFEM6BARKTHICKNESS