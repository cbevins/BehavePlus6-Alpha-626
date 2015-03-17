//------------------------------------------------------------------------------
/*! \file xeqfile.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqFile class methods.
 */

// Custom include files
#include "xeqfile.h"

//------------------------------------------------------------------------------
/*! \brief EqFile constructor
 *
 *  \param name         File path name relative to INSTALLDIR
 *  \param type         EqFileType
 *  \param releaseFrom  File is required beginning with this release
 *  \param releaseThru  File is required through with this release
 */

EqFile::EqFile( const QString &name, EqFileType type, const QString& permission,
        int releaseFrom, int releaseThru ) :
    m_name(name),
        m_permission( permission ),
    m_type(type),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines whether the EqFile is part of the specified release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the EqFile is part of the specified release.
 */

bool EqFile::isCurrent( int release ) const
{
    return( release >= m_releaseFrom && release <= m_releaseThru );
}

//------------------------------------------------------------------------------
//  End of xeqfile.cpp
//------------------------------------------------------------------------------

