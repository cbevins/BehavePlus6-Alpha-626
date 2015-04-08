/*!	\file Behaveplus6/Algorithms/Fofem6Species.cpp
*	\brief Fodem6Species class definition

*/

#include <string>
#include "Fofem6Species.h"

//------------------------------------------------------------------------------
/*!	\brief Returns the species bark equation index given the FOFEM5 species code.
	\param fofem5SpeciesCode The FOFEM v5 species alphabetic code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem5BarkEquationIndex( const char* fofem5SpeciesCode )
{
	int idx = getFofem5SpeciesIndex( fofem5SpeciesCode );
	return ( idx < 0 ) ? -1 : m_species[idx].barkEq;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the species mortality equation index given the FOFEM5 species code.
	\param fofem5SpeciesCode The FOFEM v5 species alphabetic code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem5MortalityEquationIndex( const char* fofem5SpeciesCode )
{
	int idx = getFofem5SpeciesIndex( fofem5SpeciesCode );
	return ( idx < 0 ) ? -1 : m_species[idx].mortEq;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the FOFEM6 species alphanumeric code given the FOFEM5 species code.
	\param fofem5SpeciesCode The FOFEM v5 species alphabetic code.
	\return Returns pointer to the FOFEM6 species code on success, NULL on failure.
*/
const char* Fofem6Species::getFofem6SpeciesCode( const char* fofem5SpeciesCode )
{
	int idx = getFofem5SpeciesIndex( fofem5SpeciesCode );
	return ( idx < 0 ) ? 0 : m_species[idx].fofem6;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the FOFEM6 Species[] index given the FOFEM6 species code.
	\param fofem5SpeciesCode The FOFEM v5 species alphabetic code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem5SpeciesIndex( const char* fofem5SpeciesCode )
{
	for( int idx=0; m_species[idx].fofem5 != 0; idx++ )
	{
		if ( strcmp( fofem5SpeciesCode, m_species[idx].fofem5 ) == 0 )
		{
			return idx;
		}
	}
	return -1;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the species bark equation index given the FOFEM6 species code.
	\param fofem6SpeciesCode The FOFEM v6 species alphanumeric code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem6BarkEquationIndex( const char* fofem6SpeciesCode )
{
	int idx = getFofem6SpeciesIndex( fofem6SpeciesCode );
	return ( idx < 0 ) ? -1 : m_species[idx].barkEq;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the species mortality equation index given the FOFEM6 species code.
	\param fofem6SpeciesCode The FOFEM v6 species alphanumeric code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem6MortalityEquationIndex( const char* fofem6SpeciesCode )
{
	int idx = getFofem6SpeciesIndex( fofem6SpeciesCode );
	return ( idx < 0 ) ? -1 : m_species[idx].mortEq;
}

//------------------------------------------------------------------------------
/*!	\brief Returns the FOFEM6 Species[] index given the FOFEM6 species code.
	\param fofem6SpeciesCode The FOFEM v6 species alphanumeric code.
	\return Returns a non-negative index on success, -1 on failure.
*/
int Fofem6Species::getFofem6SpeciesIndex( const char* fofem6SpeciesCode )
{
	for( int idx=0; m_species[idx].fofem6 != 0; idx++ )
	{
		if ( strcmp( fofem6SpeciesCode, m_species[idx].fofem6 ) == 0 )
		{
			return idx;
		}
	}
	return -1;
}
