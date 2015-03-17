//------------------------------------------------------------------------------
/*! \file module.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2010 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Module class methods
 */

// Custom include files
#include "module.h"

//------------------------------------------------------------------------------
/*! \brief Module constructor
 *
 *  \param name         Module name (lower case to match properties)
 *  \param textKey      Module Dialog checkbox text key.
 *  \param sort         Module Dialog checkbox order key.
 *  \param indent       Module Dialog indentation level.
 *  \param releaseFrom  Module available beginning with this release.
 *  \param releaseThru  Module available through with this release.
 */

Module::Module( const QString &name, const QString &textKey,
        const QString sort, int indent, int releaseFrom, int releaseThru,
		bool inputs, bool outputs) :
    m_name(name),
    m_textKey(textKey),
    m_sort(sort),
    m_output(),
    m_indent(indent),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru),
	m_inputs( inputs ),
	m_outputs( outputs )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines whether the Module is part of the specified release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the Module is part of the specified release.
 */

bool Module::isCurrent( int release ) const
{
    return( release >= m_releaseFrom && release <= m_releaseThru );
}

//------------------------------------------------------------------------------
/*! \brief Determines whether the Module is able to produce the specified
 *  variable as output.
 *
 *  \param varName  Output variable name.
 *
 *  \return TRUE if the Module is able to produce the variable as output.
 */

bool Module::producesVariable( const QString &varName ) const
{
    for ( QStringList::const_iterator it = m_output.begin();
          it != m_output.end();
          ++it )
    {
        if ( *it == varName )
        {
            return( true );
        }
    }
    return( false );
}


//------------------------------------------------------------------------------
//  End of module.cpp
//------------------------------------------------------------------------------

