//------------------------------------------------------------------------------
/*! \file xeqappparser.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqTree application parser class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appproperty.h"
#include "apptranslator.h"
#include "xeqapp.h"
#include "xeqappparser.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qfileinfo.h>

// Standard include files
#include <iostream>

//------------------------------------------------------------------------------
/*! \brief EqAppParser constructor.
 *
 *  \param eqApp Ptr to parent EqApp
 *  \param fileName Name of the EqApp definition document to be read.
 *  \param validate If TRUE, extra validation is performed.
 *  \param debug If TRUE, input processing is echoed to stdout.
 */

EqAppParser::EqAppParser( EqApp *eqApp, const QString &fileName,
        bool validate, bool debug ) :
    XmlParser( fileName, validate, debug ),
    m_eqApp(eqApp),
    m_itemList(0),
    m_releaseCount(0)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Searches for the requested "releaseFrom" or "releaseThru" attribute,
 *  tests it against the list of known releases, and returns its value.
 *
 *  \param name         Name of the attribute to find
 *                      ("releaseFrom" or "releaseThru" )
 *  \param value        Returned text value of the attribute (if \a name found).
 *  \param elementName  Name of the element.
 *  \param attribute    List of attributes
 *
 *  \return TRUE if \a name found, FALSE if \a name not found.
 */

bool EqAppParser::getRelease( const QString &name, int *value,
        const QString &elementName, const QXmlAttributes& attribute )
{
    getAtt( name, value, true, elementName, attribute );
    // Test it against the list of known releases
    if ( ! isRelease( *value ) )
    {
        m_error = QString( "<%1 %2=\"%3\"> is an unknown release." )
            .arg( elementName ).arg( name ).arg( *value );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <EqApp> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleEqApp( const QString &elementName,
        const QXmlAttributes& attribute )
{
    QString name;
    int release;
    // "name" and "release" attributes are required
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "release", &release, true, elementName, attribute ) )
    {
        return( false );
    }
    m_eqApp->m_release = release;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <file> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE on error.
 */

bool EqAppParser::handleFile( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int     releaseFrom, releaseThru;
    QString name, type, perm;
    EqFileType fileType;

    // "name", "type", "permission", "releaseFrom", and "releaseThru" attributes are required
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "permission", perm, true, elementName, attribute )
      || ! getRelease( "releaseFrom", &releaseFrom, elementName, attribute )
      || ! getRelease( "releaseThru", &releaseThru, elementName, attribute )
      || ! getAtt( "type", type, true, elementName, attribute ) )
    {
        return( false );
    }
    // Validate "type" attribute value
    if ( type == "Dir" )
    {
        fileType = EqFileType_Dir;
    }
    else if ( type == "BpData" )
    {
        fileType = EqFileType_BpData;
    }
    else if ( type == "BpDesc" )
    {
        fileType = EqFileType_BpDesc;
    }
    else if ( type == "GeoData" )
    {
        fileType = EqFileType_GeoData;
    }
    else if ( type == "DocHlp" )
    {
        fileType = EqFileType_DocHlp;
    }
    else if ( type == "DocHtml" )
    {
        fileType = EqFileType_DocHtml;
    }
    else if ( type == "DocPdf" )
    {
        fileType = EqFileType_DocPdf;
    }
    else if ( type == "DocImage" )
    {
        fileType = EqFileType_DocImage;
    }
    else if ( type == "DialogImage" )
    {
        fileType = EqFileType_DialogImage;
    }
    else
    {
        m_error = QString( "<%1> element has unknown "
            "\"type=\"%2\" attribute." ).arg( elementName ).arg( type ) ;
        return( false );
    }
    // Create the EqFile and add it to the m_eqFileList
    m_eqApp->addEqFile( name, fileType, perm, releaseFrom, releaseThru );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <function> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleFunction( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "name" attribute is required
    QString name, type, varName;
    int     releaseFrom, releaseThru;
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getRelease( "releaseFrom", &releaseFrom, elementName, attribute )
      || ! getRelease( "releaseThru", &releaseThru, elementName, attribute ) )
    {
        return( false );
    }
    // Should be the only function with this name
    if ( m_validate && m_eqApp->m_funDict->find( name ) )
    {
        m_error = QString( "<%1 name=\"%2\"> element was previously defined." )
            .arg( elementName ).arg( name );
        return( false );
    }
    // "module" attribute is optional
    QString module("");
    getAtt( "module", module, false, elementName, attribute );
    // Pass 1: count and validate all inputs and outputs
    int inputs = 0;
    int outputs = 0;
    int id;
    for ( id = 0;
          id < attribute.length();
          id++ )
    {
        // Count input & output variables
        type = attribute.localName( id );
        if ( type == "input" )
        {
            inputs++;
        }
        else if ( type == "output" )
        {
            outputs++;
        }
        else if ( type == "name"
			   || type == "note"
               || type == "releaseFrom"
               || type == "releaseThru"
               || type == "module" )
        {
            continue;
        }
        else
        {
            m_error = QString( "<%1 name=\"%2\"> element may only have "
            "\"input=\" and \"output=\" attributes; found \"%3\"." )
            .arg( elementName ).arg( name ).arg( type );
            return( false );
        }
        // Validate input/output variable name
        varName = attribute.value( id );
        if ( m_validate && ! m_eqApp->m_varDict->find( varName ) )
        {
            m_error = QString( "<%1 name=\"%2\" %3=\"%4\" is undefined." )
                .arg( elementName).arg( name ).arg( type ).arg( varName );
            return( false );
        }
    }
    // Pass 2: create the EqFun and add the inputs and outputs
    EqFun *funPtr = m_eqApp->addEqFun( name, inputs, outputs, releaseFrom,
        releaseThru, module );
    EqVar  *varPtr;
    inputs = outputs = 0;
    for ( id = 0;
          id < attribute.length();
          id++ )
    {
        type = attribute.localName( id );
        varName = attribute.value( id );
        if ( type == "input" )
        {
            varPtr = m_eqApp->m_varDict->find( varName );
            funPtr->m_input[inputs++] = varPtr;
            varPtr->m_consumers++;
        }
        else if ( type == "output" )
        {
            varPtr = m_eqApp->m_varDict->find( varName );
            funPtr->m_output[outputs++] = varPtr;
            varPtr->m_producers++;
            if ( ! module.isEmpty() )
            {
                m_eqApp->addModuleOutputVariable( module, varName );
            }
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <item> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleItem( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // Must appear inside <itemList>
    if ( m_validate
      && ( m_elements < 2 || m_element[m_elements-2] != "itemList" ) )
    {
        m_error = QString( "<%1> element is outside an enclosing "
            "<itemList> element." ).arg( elementName );
        return( false );
    }
    // "name", "sort", "index", "perm" attributes are required
    QString name, sort;
    int     index;
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "sort", sort, true, elementName, attribute )
      || ! getAtt( "index", &index, true, elementName, attribute ) )
    {
        return( false );
    }
    // "perm" and "default" attributes are optional
    bool isPerm = true;
    bool isDflt = false;
    getAtt( "perm",    &isPerm, false, elementName, attribute );
    getAtt( "default", &isDflt, false, elementName, attribute );
    // \todo Name must be unique for this itemList
    if ( m_validate )
    {
        m_validate = true; /* NOTHING */
    }
    // Insert a new EqVarItem in the most recent <itemList>
    m_itemList->addItem( name, sort, index, isPerm, isDflt );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <itemList> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleItemList( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "name" attribute is required
    QString name;
    if ( ! getAtt( "name", name, true, elementName, attribute ) )
    {
        return( false );
    }
    // Make sure this EqVarItemList name is unique
    if ( m_validate
      && m_eqApp->m_itemListDict->find( name ) )
    {
        m_error = QString( "<%1 name=\"%2\"> element previously defined." )
            .arg( elementName).arg( name );
        return( false );
    }
    // Make sure there is room in the pointer array.
    if ( m_validate
      && m_eqApp->m_itemCount >= m_eqApp->m_itemListCount )
    {
        m_error = QString( "<%1 name=\"%2\"> element causes an EqApp "
            " m_itemList[%3] overflow." )
            .arg( elementName).arg( name ).arg( m_eqApp->m_itemListCount );
        return( false );
    }
    // Create the new EqVarItemList and add its pointer to the array.
    EqVarItemList *ptr = new EqVarItemList( name );
    checkmem( __FILE__, __LINE__, ptr,
        QString( "EqVarItemList %1 ptr" ).arg( name ), 1 );
    m_eqApp->m_itemList[ m_eqApp->m_itemCount++ ] = ptr;

    // Add the EqVarItemList name to the dictionary.
    m_eqApp->m_itemListDict->insert( name, ptr );

    // Set this as the most recent EqVarItemList.
    m_itemList = ptr;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <language> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleLanguage( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "name" attribute is required
    QString name;
    if ( ! getAtt( "name", name, true, elementName, attribute ) )
    {
        return( false );
    }
    // Store this in the language list
    QString *str = new QString( name );
    Q_CHECK_PTR( str );
    //m_eqApp->m_languageList->inSort( str );
    m_eqApp->m_languageList->append( str );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <module> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleModule( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int     releaseFrom, releaseThru, indent;
    QString name, sort, textKey;

    // "name", "sort", "textKey", "indent", "releaseFrom", and "releaseThru"
    // attributes are required
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "sort", sort, true, elementName, attribute )
      || ! getAtt( "textKey", textKey, true, elementName, attribute )
      || ! getAtt( "indent", &indent, true, elementName, attribute )
      || ! getRelease( "releaseFrom", &releaseFrom, elementName, attribute )
      || ! getRelease( "releaseThru", &releaseThru, elementName, attribute ) )
    {
        return( false );
    }
    // Create the EqFile and add it to the m_eqFileList
    m_eqApp->addModule( name, textKey, sort, indent, releaseFrom, releaseThru );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <property> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleProperty( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "name", "type", "value", "releaseFrom", and "releaseThru"
    // attributes are required
    QString name, type, value;
    int releaseFrom, releaseThru;
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "type", type, true, elementName, attribute )
      || ! getAtt( "value", value, true, elementName, attribute )
      || ! getRelease( "releaseFrom", &releaseFrom, elementName, attribute )
      || ! getRelease( "releaseThru", &releaseThru, elementName, attribute ) )
    {
        return( false );
    }
    // Validate the "type" attribute text
    Property::PropertyType propType;
    if ( type == "Boolean" )
    {
        propType = Property::Boolean;
    }
    else if ( type == "Color" )
    {
        propType = Property::Color;
    }
    else if ( type == "Integer" )
    {
        propType = Property::Integer;
    }
    else if ( type == "Real" )
    {
        propType = Property::Real;
    }
    else if ( type == "String" )
    {
        propType = Property::String;
    }
    else
    {
        m_error = QString( "<%1 name=\"%2\"> element has a bad "
            "type=\"%3\" attribute." )
            .arg( elementName).arg( name ).arg( type ) ;
        return( false );
    }
    // Add the property to the application-wide default property dictionary
    appProperty()->add( name, propType, value, releaseFrom, releaseThru );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <release> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleRelease( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "name" and "desc" attributes are required
    QString desc;
    int     release;
    if ( ! getAtt( "name", &release, true, elementName, attribute )
      || ! getAtt( "desc", desc, true, elementName, attribute ) )
    {
        return( false );
    }
    m_release[ m_releaseCount++ ] = release;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <translate> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleTranslate( const QString &elementName,
        const QXmlAttributes& attribute )
{
    // "key" attribute is required
    QString key, lang, text;
    if ( ! getAtt( "key", key, true, elementName, attribute ) )
    {
        return( false );
    }
    // Get each language attribute
    for ( int id = 0;
          id < attribute.length();
          id++ )
    {
        lang = attribute.localName( id );
        text = attribute.value( id );
        // Skip the "key" and "used" attributes
        if ( lang == "key"
          || lang == "used" )
        {
            continue;
        }
        // Add the key and text to the dictionary.
        appTranslatorInsert( key, lang, text );
        // If en_US, create a US_Rot13 version
        //if ( lang == "en_US" )
        //{
        //    appTranslatorInsert( key, "US_Rot13", *rot13( text ) );
        //}
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <variable> element.
 *
 *  Does not use error message translation since we are boot strapping the
 *  translation dictionary!
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqAppParser::handleVariable( const QString &elementName,
        const QXmlAttributes& attribute )
{
    EqVar  *varPtr;
    // "name", "type", "help", "sortIn", "sortOut", "releaseFrom",
    // "releaseThru" attributes are required
    QString name, type, help, sortIn, sortOut;
    int     releaseFrom, releaseThru;
    if ( ! getAtt( "name", name, true, elementName, attribute )
      || ! getAtt( "type", type, true, elementName, attribute )
      || ! getAtt( "help", help, true, elementName, attribute )
      || ! getAtt( "sortIn", sortIn, true, elementName, attribute )
      || ! getAtt( "sortOut", sortOut, true, elementName, attribute )
      || ! getRelease( "releaseFrom", &releaseFrom, elementName, attribute )
      || ! getRelease( "releaseThru", &releaseThru, elementName, attribute ) )
    {
        return( false );
    }
    // "calc" attribute is optional
    QString calc( "standard" );
    if ( getAtt( "calc", calc, false, elementName, attribute ) )
    {
        if ( calc != "standard"
          && calc != "diagram"
          && calc != "weighted" )
        {
            m_error = QString( "<%1 name=\"%2\" > element "
                "calc=\"%3\"> must be \"standard\", \"diagram\", or \"weighted\"." )
                .arg( elementName ).arg( name ).arg( calc );
            return( false );
        }
    }
    // "master" attribute is optional
    QString master("");
    getAtt( "master", master, false, elementName, attribute );
    // "wizard" attribute is optional
    QString wizard("");
    getAtt( "wizard", wizard, false, elementName, attribute );

    // Continuous variables
    if ( type == "continuous" )
    {
        int     nativeDecimals, englishDecimals, metricDecimals;
        double  minimum, maximum, value;
        QString nativeUnits, englishUnits, metricUnits;
        if ( ! getAtt( "nativeUnits", nativeUnits, true, elementName, attribute )
          || ! getAtt( "nativeDecimals", &nativeDecimals, true, elementName, attribute )
          || ! getAtt( "englishUnits", englishUnits, true, elementName, attribute )
          || ! getAtt( "englishDecimals", &englishDecimals, true, elementName, attribute )
          || ! getAtt( "metricUnits", metricUnits, true, elementName, attribute )
          || ! getAtt( "metricDecimals", &metricDecimals, true, elementName, attribute )
          || ! getAtt( "minimum", &minimum, true, elementName, attribute )
          || ! getAtt( "maximum", &maximum, true, elementName, attribute )
          || ! getAtt( "default", &value, true, elementName, attribute ) )
        {
            return( false );
        }
        // "wrap" attribute is optional
        bool isWrap = false;
        getAtt( "wrap", &isWrap, false, elementName, attribute );
        // Store the variable
        varPtr = m_eqApp->addEqVar( name, help, sortIn, sortOut, wizard,
            nativeUnits, nativeDecimals, englishUnits, englishDecimals,
            metricUnits, metricDecimals, minimum, maximum, value,
            releaseFrom, releaseThru );
        varPtr->m_isWrap = isWrap;
        varPtr->m_master = master;
    }

    // Discrete and diagram variables
    else if ( type == "discrete" )
    {
        QString itemList("");
        // "itemList" attribute is required
        if ( ! getAtt( "itemList", itemList, true, elementName, attribute ) )
        {
            return( false );
        }
        // Make sure the itemList, if specified, exists
        EqVarItemList *list = 0;
        if ( m_validate && itemList != "" )
        {
            if ( ! ( list = m_eqApp->m_itemListDict->find( itemList ) ) )
            {
                m_error = QString( "<%1 name=\"%2\"> references an unknown "
                    "itemlist=\"%3\"." )
                    .arg( elementName ).arg( name ).arg( itemList );
                return( false );
            }
        }
        // Add the discrete variable
        varPtr = m_eqApp->addEqVar( name, help, sortIn, sortOut, wizard,
            list, releaseFrom, releaseThru );
        varPtr->m_master = master;
        if ( calc == "diagram" )
        {
            varPtr->m_isDiagram = true;
        }
    }

    // Text variables
    else if ( type == "text" )
    {
        // No special attributes to process.
        // Add the text variable
        varPtr = m_eqApp->addEqVar( name, help, sortIn, sortOut, wizard,
            releaseFrom, releaseThru );
    }

    // Unknown variable type
    else
    {
        m_error = QString( "<%1 name=\"%2\"> element has an unknown or bad "
                "\"type=\" attribute." ).arg( elementName ).arg( name );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the specified release is in the known release list.
 *
 *  \return TRUE if the release is in the list, FALSE otherwise.
 */

bool EqAppParser::isRelease( int release ) const
{
    for ( int i = 0;
          i < m_releaseCount;
          i++ )
    {
        if ( m_release[i] == release )
        {
            return( true );
        }
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Start-of-element callback.  This does most the work.
 */

bool EqAppParser::startElement( const QString & /* namespaceUrl */,
    const QString & /* localName */ , const QString& elementName,
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
    // Skip all elements until <EqApp> is found.
    if ( ! m_elements )
    {
        if ( elementName == "EqApp" )
        {
            push( elementName );
            if ( ! handleEqApp( elementName, attribute ) )
            {
                return( false );
            }
        }
        return( true );
    }
    // <file> element
    if ( elementName == "file" )
    {
        push( elementName );
        if ( ! handleFile( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <function> elements
    else if ( elementName == "function" )
    {
        push( elementName );
        if ( ! handleFunction( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <itemList> elements
    else if ( elementName == "itemList" )
    {
        push( elementName );
        if ( ! handleItemList( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <item> element
    else if ( elementName == "item" )
    {
        push( elementName );
        if ( ! handleItem( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <language> element
    else if ( elementName == "language" )
    {
        push( elementName );
        if ( ! handleLanguage( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <module> element
    else if ( elementName == "module" )
    {
        push( elementName );
        if ( ! handleModule( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <property> elements
    else if ( elementName == "property" )
    {
        push( elementName );
        if ( ! handleProperty( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <release> elements
    else if ( elementName == "release" )
    {
        push( elementName );
        if ( ! handleRelease( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <translate> elements
    else if ( elementName == "translate" )
    {
        push( elementName );
        if ( ! handleTranslate( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <variable> elements
    else if ( elementName == "variable" )
    {
        push( elementName );
        if ( ! handleVariable( elementName, attribute ) )
        {
            return( false );
        }
    }
    // Ignore all other tags
    else
    {
        m_error = QString( "<%1> element is unknown." ).arg( elementName );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of xeqapp.cpp
//------------------------------------------------------------------------------

