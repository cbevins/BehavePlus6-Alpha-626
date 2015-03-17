//------------------------------------------------------------------------------
/*! \file xeqfile.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqFile application class definition.
 */

#ifndef _XEQFILE_H_
/*! \def _XEQFILE_H_
    \brief Prevent redundant includes.
 */
#define _XEQFILE_H_ 1

// Qt class references
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class EqFile xeqfile.h
 *
 *  \brief Class that holds the name, type, and release information
 *  for a single directory or file required by the EqApp's FileSystem.
 */

enum EqFileType
{
    EqFileType_Dir=0,
    EqFileType_BpData=1,
    EqFileType_BpDesc=2,
    EqFileType_GeoData=3,
    EqFileType_DocHlp=4,
    EqFileType_DocHtml=5,
    EqFileType_DocImage=6,
    EqFileType_DocPdf=7,
    EqFileType_DialogImage=8
};

class EqFile
{
// Public methods
public:
    EqFile( const QString &name, EqFileType type, const QString &permission, int releaseFrom,
        int releaseThru );
    bool isCurrent( int release ) const ;

// Public data
public:
    QString    m_name;          //!< Dir or file full path name from INSTALLDIR
    QString    m_permission;    //!< Combination of "r" (readable), "w" (writable), "e" (must exist)
    EqFileType m_type;          //!< File type
    int        m_releaseFrom;   //!< File require beginning this release
    int        m_releaseThru;   //!< File require through this release
};

#endif

//------------------------------------------------------------------------------
//  End of xeqfile.h
//------------------------------------------------------------------------------

