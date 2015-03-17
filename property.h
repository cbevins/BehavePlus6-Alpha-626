//------------------------------------------------------------------------------
/*! \file property.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Generic property system used to define and access the plethora of
 *  properties used by BehavePlus.
 */

#ifndef _PROPERTY_H_
/*! \def _PROPERTY_H_
    \brief Prevent redundant includes.
 */
#define _PROPERTY_H_ 1

// Qt class references
#include <qdict.h>
#include <qstring.h>
#include "xmlparser.h"

//------------------------------------------------------------------------------
/*! \class Property property.h
 *
 *  \brief A single property consisting of a type and its textual value.
 */

class Property
{
// Public enumerations
public:
    /*! \var PropertyType
     *  \brief Defines the valid property types.
     */
    enum PropertyType { None, Boolean, Color, Integer, Real, String };

// Public methods
public:
    Property( PropertyType type, const QString &value,
        int releaseFrom, int releaseThru );
    bool isCurrent( int release ) ;

// Public data
public:
    PropertyType m_type;        //!< Property type
    QString      m_value;       //!< Text representation of the property value
    int          m_releaseFrom; //!< Effective beginning this release
    int          m_releaseThru; //!< Effective through this release
};

//------------------------------------------------------------------------------
/*! \class PropertyDict property.h
 *
 *  \brief Dictionary of all named properties.
 */

class PropertyDict : public QDict<Property>
{
// Public methods
public:
    PropertyDict( int size, bool caseSensitive=true ) ;
    bool add( const QString &name, Property::PropertyType type,
            const QString &value, int releaseFrom, int releaseThru );
    bool exists( const QString &name ) const ;
    bool update( const QString &name, const QString &value ) ;

    bool readXmlFile( const QString &fileName ) ;
    bool writeXmlFile( const QString &fileName, const QString &elementName,
            int release ) ;
    bool writeXmlFile( FILE *fptr, int release ) ;

    // Access to individual property's member data
    Property::PropertyType type( const QString &name ) const ;
    QString value( const QString &name ) const ;
    bool isCurrent( const QString &name, int release ) const ;

    // Getting property values (strongly asserted() )
    bool    boolean( const QString &name ) const ;
    QString color( const QString &name ) const ;
    int     integer( const QString &name ) const ;
    double  real( const QString &name ) const ;
    QString string( const QString &name ) const ;

    // Setting property values (strongly asserted() )
    bool    boolean( const QString &name, bool value );
    bool    color( const QString &name, const QString &value );
    bool    integer( const QString &name, int value );
    bool    real( const QString &name, double value );
    bool    string( const QString &name, const QString &value );

private:
    Property *get( const QString &name, Property::PropertyType type ) const ;
};

//------------------------------------------------------------------------------
/*! \class PropertyParser property.h
 *
 *  \brief Parses an XML file containing property definitions.
 */

class PropertyParser : public XmlParser
{
// Public methods
public:
    PropertyParser( PropertyDict *propDict, const QString &fileName,
        bool validate=true, bool debug=false ) ;
    // Re-implemented virtual functions
    virtual bool    startElement( const QString &namespaceUri,
                        const QString &localName, const QString &elementName,
                        const QXmlAttributes &attribute );
    // Virtual element-handling functions
    virtual bool handleProperty( const QString &elementName,
        const QXmlAttributes& attribute ) ;

// Private data
private:
    PropertyDict *m_propDict;
};

#endif

//------------------------------------------------------------------------------
//  End of property.h
//------------------------------------------------------------------------------

