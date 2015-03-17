//------------------------------------------------------------------------------
/*! \file module.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Module application class definition.
 */

#ifndef _MODULE_H_
/*! \def _MODULE_H_
    \brief Prevent redundant includes.
 */
#define _MODULE_H_ 1

// Qt class references
#include <qstring.h>
#include <qstringlist.h>

//------------------------------------------------------------------------------
/*! \class Module module.h
 *
 *  \brief Class that holds the name, textKey, sort key, indentation level,
 *   and release information for a single <module> entry in BehavePlus.xml.
 */

class Module
{
// Public methods
public:
    Module( const QString &name, const QString &textKey, const QString sort,
        int indent, int releaseFrom, int releaseThru ) ;
    bool isCurrent( int release ) const ;
	bool producesVariable( const QString &varName ) const ;

// Public data
public:
    QString m_name;         //!< Module name (lower case to match properties)
    QString m_textKey;      //!< Module Dialog checkbox text key
    QString m_sort;         //!< Module Dialog checkbox sort order
	QStringList m_output;	//!< Module output variable names
    int     m_indent;       //!< Module Dialog checkbox indentation level
    int     m_releaseFrom;  //!< Module available beginning this release
    int     m_releaseThru;  //!< Module available through this release
};

#endif

//------------------------------------------------------------------------------
//  End of module.h
//------------------------------------------------------------------------------

