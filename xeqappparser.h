//------------------------------------------------------------------------------
/*! \file xeqappparser.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqTree application parser class definition.
 */

#ifndef _XEQAPPPARSER_H_
/*! \def _XEQAPPPARSER_H_
    \brief Prevent redundant includes.
 */
#define _XEQAPPPARSER_H_ 1

// Custom class references
class EqApp;
class EqVarItemList;
#include "xmlparser.h"

// Qt class references
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class EqAppParser xeqappparser.h
 *
 *  \brief Parses an EqApp definition XML document.
 */

class EqAppParser : public XmlParser
{
// Public methods
public:
    EqAppParser( EqApp *eqApp, const QString &fileName,
        bool validate=true, bool debug=false ) ;
    // Re-implemented virtual functions
    virtual bool startElement( const QString &namespaceUri,
                    const QString &localName, const QString &elementName,
                    const QXmlAttributes &attribute );
    bool getRelease( const QString &name, int *value,
            const QString &elementName, const QXmlAttributes& attribute ) ;
    // Virtual element-handling functions
    virtual bool handleEqApp( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleFile( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleFunction( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleItem( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleItemList( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleModule( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleLanguage( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleRelease( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleProperty( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleTranslate( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool handleVariable( const QString &elementName,
                    const QXmlAttributes& attribute ) ;
    virtual bool isRelease( int release ) const ;

// Private data
private:
    EqApp         *m_eqApp;         //!< Ptr to EqApp to load
    EqVarItemList *m_itemList;      //!< Current <itemList> pointer for <item>
    int            m_release[100];  //!< Array of valid release numbers
    int            m_releaseCount;  //!< Number of releases in the array
};

#endif

//------------------------------------------------------------------------------
//  End of xeqappparser.h
//------------------------------------------------------------------------------

