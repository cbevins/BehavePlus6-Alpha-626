#ifndef BEHAVEPLUS6_ALGORITHMS_FOFEM6SPECIES
#define BEHAVEPLUS6_ALGORITHMS_FOFEM6SPECIES

/*!	\file BehavePlus6/Algorithms/Fofem6Species.h
*	\brief Fofem6SPecies class declaration.
*/

/*!	\class Fofem6Species
*	\brief Access to the FOFEM v6 species codes and their corresponding
*	scientific names, common names, bark thickness equations, and
*	mortality equations.
*
*	Note that all the species codes changed from FOFEM v5 to v6.
*/
class Fofem6Species
{
public:
	// Methods for accessing FOFEM species info by their FOFEM v5 species codes
	static int getFofem5BarkEquationIndex( const char* fofem5SpeciesCode );
	static int getFofem5MortalityEquationIndex( const char* fofem5SpeciesCode );
	static const char* getFofem6SpeciesCode( const char* fofem5SpeciesCode );
	static int getFofem5SpeciesIndex( const char* fofem5SpeciesCode );

	// Methods for accessing FOFEM species info by their FOFEM v6 species codes
	static int getFofem6BarkEquationIndex( const char* fofem6SpeciesCode );
	static int getFofem6MortalityEquationIndex( const char* fofem6SpeciesCode );
	static int getFofem6SpeciesIndex( const char* fofem6SpeciesCode );

	typedef struct _species
	{
		char *fofem6;		// FOFEM 6 genus-species abbreviation
		char *fofem5;       // FOFEM 5 genus-species abbreviation
		int   mortEq;       // Index to mortality equation (base 1): 1, 3, and 10-20
		int   barkEq;       // Index to single bark thickness equation (base 1)
		int   regions;      // Region list (any combination of 1, 2, 3, and/or 4)
		char *scientific;   // Scientific name
		char *common;       // Common name
	} SpeciesStruct;

private:
	static SpeciesStruct m_species[];
};

#endif	// BEHAVEPLUS6_ALGORITHMS_FOFEM6SPECIES