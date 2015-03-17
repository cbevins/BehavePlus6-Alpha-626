//------------------------------------------------------------------------------
/*! \file xeqtreeparser.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree parser class definitions.
 */

#ifndef _XEQTREEPARSER_H_
/*! \def _XEQTREEPARSER_H_
 *  \brief Prevent redundant includes.
 */
#define _XEQTREEPARSER_H_ 1

// Custom class references
class EqTree;
#include "xmlparser.h"

// Qt class references
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class EqTreeParser xeqtreeparser.h
 *
 *  \brief Parses an EqTree (BehavePlus) definition XML document.
 */

class EqTreeParser : public XmlParser
{
// Public methods
public:
    EqTreeParser( EqTree *eqTree, const QString &fileName,
        bool unitsOnly=false, bool validate=true, bool debug=false ) ;
    // Re-implemented virtual functions
    virtual bool    startElement( const QString &namespaceUri,
                        const QString &localName, const QString &elementName,
                        const QXmlAttributes &attribute );
    // Virtual element-handling functions
    virtual bool handleBehavePlus( const QString &elementName,
        const QXmlAttributes& attribute ) ;
    virtual bool handlePrescription( const QString &elementName,
        const QXmlAttributes& attribute ) ;
    virtual bool handleProperty( const QString &elementName,
        const QXmlAttributes& attribute ) ;
    virtual bool handleVariable( const QString &elementName,
        const QXmlAttributes& attribute ) ;

// Private data
private:
    EqTree  *m_eqTree;      //!< Ptr to parent EqTree
    bool     m_unitsOnly;   //!< If TRUE, only process <variable> units attributes
};

#endif

//------------------------------------------------------------------------------
//  End of xeqtreeparser.h
//------------------------------------------------------------------------------

