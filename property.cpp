//------------------------------------------------------------------------------
/*! \file property.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Generic property system used to define and access the plethora of
 *  properties used by BehavePlus.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "property.h"

// Qt include files
#include <qstringlist.h>

// Standard include files
#include <ctype.h>
#include <stdio.h>
#include <iostream>

//------------------------------------------------------------------------------
/*! \brief Property constructor.
 *
 *  \param type             Property type
 *  \param value            Text representation of the property value
 *  \param releaseFrom      Effective beginning this release
 *  \param m_releaseThru    Effective through this release
 */

Property::Property( PropertyType type, const QString &value,
        int releaseFrom, int releaseThru ) :
    m_type(type),
    m_value(value),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines whether or not the Property is part of the specified
 *  release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the Property is part of the specified release.
 */

bool Property::isCurrent( int release )
{
    return( release >= m_releaseFrom && release <= m_releaseThru );
}

//------------------------------------------------------------------------------
/*! \brief PropertyDict constructor.
 */

PropertyDict::PropertyDict( int size, bool caseSensitive ) :
    QDict<Property>( size, caseSensitive )
{
    setAutoDelete(true);
    return;
}

//------------------------------------------------------------------------------
/*! \brief Tests and inserts the property into the dictionary.
 *
 *   \return TRUE on success, FALSE on failure.
 */

bool PropertyDict::add( const QString &name, Property::PropertyType type,
    const QString &value, int releaseFrom, int releaseThru )
{
    // Create the property property
    Property *property = new Property( type, "", releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, property, "Property property", 1 );
    // Add it to the dictionary
    insert( name, property );
    // Test and store its value
    update( name, value ) ;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current Boolean value of property named \a name.
 */

bool PropertyDict::boolean( const QString &name ) const
{
    Property *property = get( name, Property::Boolean );
    if ( ! property )
    {
        return( false );
    }
    else if ( property->m_value == "true" )
    {
        return( true );
    }
    else if ( property->m_value == "false" )
    {
        return( false );
    }
    // This code block should never be executed!
    // (unless the value string is internally corrupted)
    QString msg("");
    translate( msg, "Property:badBooleanStore", name, property->m_value );
    error( msg );
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Sets the Boolean property named \a name to \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::boolean( const QString &name, bool value )
{
    Property *property = get( name, Property::Boolean );
    if ( ! property )
    {
        return( false );
    }
    property->m_value = ( value )
                      ? "true"
                      : "false" ;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current Color value of property named \a name.
 */

QString PropertyDict::color( const QString &name ) const
{
    Property *property = get( name, Property::Color );
    if ( ! property )
    {
        return( QString( "yellow" ) );
    }
    return( property->m_value );
}

//------------------------------------------------------------------------------
/*! \brief Sets the Color property named \a name to \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::color( const QString &name, const QString &value )
{
    Property *property = get( name, Property::Color );
    if ( ! property )
    {
        return( false );
    }
    property->m_value = value;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the property named \a name exists int he dictionary.
 *
 *  \return TRUE if \a name exists, FALSE if \a name not found.
 */

bool PropertyDict::exists( const QString &name ) const
{
    Property *property = find( name );
    return( ( property != 0 )
            ? true
            : false );
}

//------------------------------------------------------------------------------
/*! \brief Finds the property with \a name, ensures its of the requested type,
 *  and returns a pointer to it.
 *
 *  \retval TRUE if \a name exists and is os \a type.
 *  \retval FALSE if \a name not found or is not of \a type.
 */

Property *PropertyDict::get( const QString &name, Property::PropertyType type )
        const
{
    Property *property = find( name );
    if ( ! property )
    {
        QString msg("");
        translate( msg, "Property:notFound", name );
        error( msg );
        return( 0 );
    }
    else if ( property->m_type != type )
    {
        QString msg("");
        translate( msg, "Property:wrongType", name,
            QString( "%1" ).arg( property->m_type ),
            QString( "%1" ).arg( type ) );
        error( msg );
        return( 0 );
    }
    return( property );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current integer value of property named \a name.
 */

int PropertyDict::integer( const QString &name ) const
{
    Property *property = get( name, Property::Integer );
    if ( ! property )
    {
        return( 0 );
    }
    bool ok;
    int i = property->m_value.toInt( &ok, 10 );
    if ( ok )
    {
        return( i );
    }
    // This code block should never be executed!
    // (unless the value string is internally corrupted)
    QString msg("");
    translate( msg, "Property:badIntegerStore", name, property->m_value );
    error( msg );
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Sets the Integer property named \a name to \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::integer( const QString &name, int value )
{
    Property *property = get( name, Property::Integer );
    if ( ! property )
    {
        return( 0 );
    }
    property->m_value = QString( "%1" ).arg( value );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines whether or not the Property is part of the specified
 *  release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the Property is part of the specified release.
 */

bool PropertyDict::isCurrent( const QString &name, int release ) const
{
    Property *property = find( name );
    if ( property )
    {
        return( release >= property->m_releaseFrom
             && release <= property->m_releaseThru );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current real value of property named \a name.
 */

double PropertyDict::real( const QString &name ) const
{
    Property *property = get( name, Property::Real );
    if ( ! property )
    {
        return( 0. );
    }
    bool ok;
    double d = property->m_value.toDouble( &ok );
    if ( ok )
    {
        return( d );
    }
    // This code block should never be executed!
    // (unless the value string is internally corrupted)
    QString msg("");
    translate( msg, "Property:badRealStore", name, property->m_value );
    error( msg );
    return( 0. );
}

//------------------------------------------------------------------------------
/*! \brief Sets the Real property named \a name to \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::real( const QString &name, double value )
{
    Property *property = get( name, Property::Real );
    if ( ! property )
    {
        return( false );
    }
    property->m_value = QString( "%1" ).arg( value );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current String value of property named \a name.
 */

QString PropertyDict::string( const QString &name ) const
{
    Property *property = get( name, Property::String );
    if ( ! property )
    {
        return( QString( "" ) );
    }
    return( property->m_value );
}

//------------------------------------------------------------------------------
/*! \brief Sets the String property named \a name to \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::string( const QString &name, const QString &value )
{
    Property *property = get( name, Property::String );
    if (  ! property )
    {
        return( false );
    }
    property->m_value = value;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the PropertyType of property \a name.
 */

Property::PropertyType PropertyDict::type( const QString &name ) const
{
    Property *property = find( name );
    if ( ! property )
    {
        QString msg("");
        translate( msg, "Property:notFound", name );
        error( msg );
        return( Property::None );
    }
    return( property->m_type );
}

//------------------------------------------------------------------------------
/*! \brief Returns the QString value of property \a name.
 */

QString PropertyDict::value( const QString &name ) const
{
    Property *property = find( name );
    if ( ! property )
    {
        QString msg("");
        translate( msg, "Property:notFound", name );
        error( msg );
        return( QString( "" ) );
    }
    return( property->m_value );
}

//------------------------------------------------------------------------------
/*! \brief Updates an existing Property \a name with \a value.
 *
 *  \retval TRUE if property exists and is of correct type.
 *  \retval FALSE if property doesn't exist or is of the wrong type.
 */

bool PropertyDict::update( const QString &name, const QString &value )
{
    Property *property = find( name );
    if ( ! property )
    {
        QString msg("");
        translate( msg, "Property:notFound", name );
        error( msg );
        return( false );
    }

    if ( property->m_type == Property::Boolean )
    {
        if ( value != "true" && value != "false" )
        {
            QString msg("");
            translate( msg, "Property:badBooleanAssign", name, value );
            error( msg );
            return( false );
        }
    }
    else if ( property->m_type == Property::Integer )
    {
        bool ok;
        value.toInt( &ok, 10 );
        if ( ! ok )
        {
            QString msg("");
            translate( msg, "Property:badIntegerAssign", name, value );
            error( msg );
            return( false );
        }
    }
    else if ( property->m_type == Property::Real )
    {
        bool ok;
        value.toDouble( &ok );
        if ( ! ok )
        {
            QString msg("");
            translate( msg, "Property:badRealAssign", name, value );
            error( msg );
            return( false );
        }
    }
    property->m_value = value;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Reads properties from the file into the property dictionary.
 *  The file must be in the same XML format as produced by
 *  PropertyDict::write().
 *
 *  \return TRUE on success, FALSE if unable to open the file.
 */

bool PropertyDict::readXmlFile( const QString &fileName )
{
    PropertyParser *handler = new PropertyParser( this, fileName );
    checkmem( __FILE__, __LINE__, handler, "PropertyParser handler", 1 );
    QFile xmlFile( fileName );
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( handler );
    reader.setErrorHandler( handler );
    bool result = reader.parse( &source );
    delete handler;
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Writes the current property dictionary contents in sorted order
 *  to the \a fileName in XML format.
 *
 *  \param fileName Name of the file to be (over)written.
 *  \param release  Only write properties with this release or lower.
 *
 *  \return TRUE on success, FALSE if unable to open the file.
 */

bool PropertyDict::writeXmlFile( const QString &fileName,
        const QString &elementName, int release )
{
    // Atempt to open the property file
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
    {
        QString text("");
        translate( text, "Property:WriteXmlFile:NoOpen", fileName );
        error( text );
        return( false );
    }
    // Write the header, body, and footer.
    xmlWriteHeader( fptr, elementName, "property", release );
    writeXmlFile( fptr, release );
    xmlWriteFooter( fptr, elementName );
    fclose( fptr );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Writes the current property dictionary contents in sorted order
 *  to the file stream. Assumes the XML header has already been written by the
 *  caller, and the XML footer will be added by the caller.
 *
 *  \param fptr     Output file stream pointer.
 *  \param release  Only write properties with this release or lower.
 *
 *  \return TRUE on success, FALSE if unable to open the file.
 */

bool PropertyDict::writeXmlFile( FILE *fptr, int release )
{
    // Write out all properties to a string list for sorting
    QStringList propList;
    QDictIterator<Property> it( *this );
    Property *p;
    QString xml("");
    while( it.current() )
    {
        p = (Property *) it.current();
        if ( p->isCurrent( release ) )
        {
            if ( p->m_value.isNull()
              || p->m_value.isEmpty() )
            {
                xml = "";
            }
            else
            {
                xml = p->m_value;
                xmlEscape( xml );
            }
            propList.append(
                QString( "  <property name=\"%1\" value=\"%2\" />" )
                    .arg( it.currentKey() ).arg( xml ) );
        }
        ++it;
    }
    // Sort the list and write it to the file
    propList.sort();
    for ( QStringList::Iterator sit=propList.begin();
          sit != propList.end();
          ++sit )
    {
        fprintf( fptr, "%s\n", (*sit).latin1() );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief PropertyParser constructor.
 *
 *  \param propDict Ptr to parent PropertyDict.
 *  \param fileName Name of the Property definition document to be read.
 *  \param validate If TRUE, extra validation is performed.
 *  \param debug    If TRUE, input procesing is echoed to stdout.
 */

PropertyParser::PropertyParser( PropertyDict *propDict, const QString &fileName,
        bool validate, bool debug ) :
    XmlParser( fileName, validate, debug ),
    m_propDict(propDict)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Handles the <property> element.
 *
 *  This uses error message translation since a translation dictionary should
 *  be available.
 *
 *  \return TRUE on success, FALSE one error.
 */

bool PropertyParser::handleProperty( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int id;
    QString name, value;

    // "name" attribute is required
    if ( ( id = attribute.index( "name" ) ) < 0 )
    {
        trError( "PropertyParser:missingName", elementName, "name" );
        return( false );
    }
    name = attribute.value( id );
    // "value" attribute is required
    if ( ( id = attribute.index( "value" ) ) < 0 )
    {
        trError( "PropertyParser:missingAttribute", elementName, name, "value" );
        return( false );
    }
    if ( ( value = attribute.value( id ) ) == "(null)" )
    {
        value = "";
    }
    // Find this property in the local Property directory and update it
    Property *property;
    if ( ( property = m_propDict->find( name ) ) > 0 )
    {
        if ( ! m_propDict->update( name, value ) )
        {
            trError( "PropertyParser:badValue",
                elementName, name, "value", value );
            return( false );
        }
    }
    // Report unknown <property> names here.
    else
    {
        trError( "PropertyParser:badValue", elementName, name, "name", name );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Start-of-element callback.  This does most the work.
 *
 *  \param namespaceUrl Not used.
 *  \param localName Not used.
 *  \param elementName
 *  \param attribute
 */

bool PropertyParser::startElement( const QString & /* namespaceUrl */,
    const QString & /* localName */, const QString& elementName,
    const QXmlAttributes& attribute )
{
    // If debugging, print each element and its attributes as encountered.
    m_indent += "    ";
    if ( m_debug )
    {
        std::cout << m_indent << "<" << elementName;
        for ( int id = 0;
              id < attribute.length();
              id++ )
        {
            std::cout << " " << attribute.localName(id) << "=\""
              << attribute.value(id) << "\"";
        }
        std::cout << " >" << std::endl;
    }
    // Skip all elements until <BehavePlus> is found.
    if ( ! m_elements )
    {
        if ( elementName == "BehavePlus" )
        {
            push( elementName );
            return( true );
        }
        trError( "PropertyParser:unknownDocument" );
        return( false );
    }
    // <property> elements
    if ( elementName == "property" )
    {
        push( elementName );
        if ( ! handleProperty( elementName, attribute ) )
        {
            return( false );
        }
    }
    // Ignore all other tags
    else
    {
        trError( "PropertyParser:unknownElement", elementName );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of property.cpp
//------------------------------------------------------------------------------

