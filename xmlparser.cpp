//------------------------------------------------------------------------------
/*! \file xmlparser.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus XML definition document parser base class methods.
 *
 *  Base class for parsing EqApp, EqTree, and Property XML documents.
 */

// Custom include files
#include "stdafx.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "xmlparser.h"

// Standard include files
#include <iostream>

//------------------------------------------------------------------------------
/*! \brief XmlParser constructor.
 *
 *  \param fileName Name of the XML document to be read.
 *  \param validate If TRUE, extra validation is performed.
 *  \param debug    If TRUE, input processing is echoed to stdout.
 */

XmlParser::XmlParser( const QString &fileName, bool validate, bool debug ) :
    QXmlDefaultHandler(),
    m_fileName(fileName),
    m_error(""),
    m_indent(""),
    m_elements(0),
    m_errors(0),
    m_validate(validate),
    m_debug(debug)
{
    // Initialize the element name stack.
    for ( int i = 0;
          i < 20;
          i++ )
    {
        m_element[i] = "";
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Called whenever startElement() returns FALSE.
 */

void XmlParser::displayError( const QString &type, const QXmlParseException& e )
{
    // If first fatal error, display error dialog.
    if ( ! m_errors )
    {
        QString msg = QString( "XmlParser() %1 %2 at %3:%4 - %5" )
            .arg( type )
            .arg( m_fileName )
            .arg( e.lineNumber() )
            .arg( e.columnNumber() )
            .arg( e.message() );
        // Make sure we call the global ::error() function,
        // rather than the local EqTreeParser::error() function,
        // or we get infinite recursion!
        ::error( msg );
    }
    m_errors++;
    return;
}

//------------------------------------------------------------------------------
/*! \brief End-of-element callback.  It simply pops the stack if the element
 *  name matches the one at the end of the stack.
 */

bool XmlParser::endElement( const QString & /* namespaceUrl */,
    const QString & /* localName */, const QString& elementName )
{
    if ( m_debug )
    {
        std::cout << m_indent << "</" << elementName << ">" << std::endl;
    }
    m_indent.remove( 0, 4 );
    if ( m_elements && elementName == m_element[m_elements-1] )
    {
        m_elements--;
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief SAX error callback handler.
 */

bool XmlParser::error( const QXmlParseException& e )
{
    displayError( "Error", e );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the current error string.
 *
 *  Called internally by QXmlParseEception::message().
 */

QString XmlParser::errorString( void )
{
    return( m_error );
}

//------------------------------------------------------------------------------
/*! \brief Called whenever startElement() returns FALSE.
 */

bool XmlParser::fatalError( const QXmlParseException& e )
{
    displayError( "Fatal Error", e );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Searches for the requested attribute and returns its text.
 *
 *  \param name         Name of the attribute to find.
 *  \param value        Returned text value of the attribute (if \a name found).
 *  \param required     If TRUE, error message displayed if \a name not found.
 *  \param elementName  Name of the element.
 *  \param attribute    List of attributes
 *
 *  \return TRUE if \a name found, FALSE if \a name not found.
 */

bool XmlParser::getAtt( const QString &name, QString &value, bool required,
        const QString &elementName, const QXmlAttributes& attribute )
{
    // Find the attribute name
    int id;
    if ( ( id = attribute.index( name ) ) < 0 )
    {
        if ( required )
        {
            m_error = QString( "<%1> element is missing the required "
                "\"%2=\" attribute." ).arg( elementName ).arg( name ) ;
        }
        return( false );
    }
    value = attribute.value( id );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Searches for the requested attribute and returns its integer value.
 *
 *  \param name         Name of the attribute to find.
 *  \param value        Returned integer value of the attribute
 *                      (if \a name found).
 *  \param required     If TRUE, error message displayed if \a name not found.
 *  \param elementName  Name of the element.
 *  \param attribute    List of attributes
 *
 *  \return TRUE if \a name found, FALSE if \a name not found.
 */

bool XmlParser::getAtt( const QString &name, int *value, bool required,
        const QString &elementName, const QXmlAttributes& attribute )
{
    // Find the attribute name
    int id;
    if ( ( id = attribute.index( name ) ) < 0 )
    {
        if ( required )
        {
            m_error = QString( "<%1> element is missing the required "
                "\"%2=\" attribute." ).arg( elementName ).arg( name ) ;
        }
        return( false );
    }
    // Get and return its integer value
    bool ok;
    *value = attribute.value( id ).toInt( &ok, 10 );
    if ( ! ok )
    {
        m_error = QString( "<%1 %2=\"%3\" > element must be an integer." )
            .arg( elementName ).arg( name ).arg( attribute.value( id ) );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Searches for the requested attribute and returns its real value.
 *
 *  \param name         Name of the attribute to find.
 *  \param value        Returned real (double) value of the attribute
 *                      (if \a name found).
 *  \param required     If TRUE, error message displayed if \a name not found.
 *  \param elementName  Name of the element.
 *  \param attribute    List of attributes
 *
 *  \return TRUE if \a name found, FALSE if \a name not found.
 */

bool XmlParser::getAtt( const QString &name, double *value, bool required,
        const QString &elementName, const QXmlAttributes& attribute )
{
    // Find the attribute name
    int id;
    if ( ( id = attribute.index( name ) ) < 0 )
    {
        if ( required )
        {
            m_error = QString( "<%1> element is missing the required "
                "\"%2=\" attribute." ).arg( elementName ).arg( name ) ;
        }
        return( false );
    }
    // Get and return its integer value
    bool ok;
    *value = attribute.value( id ).toDouble( &ok );
    if ( ! ok )
    {
        m_error = QString( "<%1 %2=\"%3\" > element must be a real number." )
            .arg( elementName ).arg( name ).arg( attribute.value( id ) );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Searches for the requested attribute and returns its boolean value.
 *
 *  \param name         Name of the attribute to find.
 *  \param value        Returned boolean value of the attribute
 *                      (if \a name found).
 *  \param required     If TRUE, error message displayed if \a name not found.
 *  \param elementName  Name of the element.
 *  \param attribute    List of attributes
 *
 *  \return TRUE if \a name found, FALSE if \a name not found.
 */

bool XmlParser::getAtt( const QString &name, bool *value, bool required,
        const QString &elementName, const QXmlAttributes& attribute )
{
    // Find the attribute name
    int id;
    if ( ( id = attribute.index( name ) ) < 0 )
    {
        if ( required )
        {
            m_error = QString( "<%1> element is missing the required "
                "\"%2=\" attribute." ).arg( elementName ).arg( name ) ;
        }
        return( false );
    }
    // Get and return its boolean value
    QString perm = attribute.value( id );
    if ( perm.lower() == "true" )
    {
        *value = true;
    }
    else if ( perm.lower() == "false" )
    {
        *value = false;
    }
    else
    {
        m_error = QString( "<%1 %2=\"%3\" > element must be \"true\" or \"false\"." )
            .arg( elementName ).arg( name ).arg( perm );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Pushes the element onto the end of the stack.
 *
 *  \param element Name of the element to be pushed.
 */

void XmlParser::push( const QString &element )
{
    if ( m_elements < 19 )
    {
        m_element[m_elements] = element;
        m_elements++;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Start-of-element callback.
 *  This should be re-implemented by derived classes.
 */

bool XmlParser::startElement( const QString & /* namespaceUri */,
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
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Translates a message with zero to four QString arguments embedded.
 */

void XmlParser::trError( const QString &key, const QString &arg1,
        const QString &arg2, const QString &arg3, const QString &arg4 )
{
    translate( m_error, key, arg1, arg2, arg3, arg4 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief, Yep, it displays a warning message, but I'm not sure who calls it,
 *  when, or why, but we'll catch 'em here.
 */

bool XmlParser::warning( const QXmlParseException& e )
{
    displayError( "Warning", e );
    return( true );
}


//------------------------------------------------------------------------------
/*! \brief Performs rotate13 operation on the \a src string and copies it
 *  into a newly allocated string.
 *
 *  \return Ptr to the newly allocated QString.
 *          It is the caller's responsiblity to delete the returned string.
 */

QString *rot13( const QString &src )
{
    QString *rot = new QString( "" );
    Q_CHECK_PTR( rot );
    const char *str = src.latin1();
    int len = strlen( str );
    for ( int i = 0;
          i < len;
          i++ )
    {
        if ( ( str[i] >= 'A' && str[i] <= 'M' )
          || ( str[i] >= 'a' && str[i] <= 'm' ) )
        {
            rot->append( str[i]+13 );
        }
        else if ( ( str[i] >= 'N' && str[i] <= 'Z' )
               || ( str[i] >= 'n' && str[i] <= 'z' ) )
        {
            rot->append( str[i]-13 );
        }
        else
        {
            rot->append( str[i] );
        }
    }
    return( rot );
}

//------------------------------------------------------------------------------
/*! \brief Convenience function to convert XML reserved characters to their
 *  escape sequences.  The five XML entity references are:
 *      -# "&" is converted to "&amp;"
 *      -# """ is converted to "&quot;"
 *      -# "'" is converted to "&apos;"
 *      -# "<" is converted to "&lt;"
 *      -# ">" is converted to "&gt;"
 *
 *  \param text Refernce to the text to be escaped.
 */

void xmlEscape( QString &text )
{
    int pos = 0;
    while ( ( pos = text.find( "&", pos ) ) >= 0 )
    {
        text = text.insert( pos+1, "amp;" );
        pos += 4;
    }
    pos = 0;
    while ( ( pos = text.find( "\"", pos ) ) >= 0 )
    {
        text = text.replace( pos, 1, "&quot;" );
        pos += 4;
    }
    pos = 0;
    while ( ( pos = text.find( "'", pos ) ) >= 0 )
    {
        text = text.replace( pos, 1, "&apos;" );
        pos += 4;
    }
    pos = 0;
    while ( ( pos = text.find( "<", pos ) ) >= 0 )
    {
        text = text.replace( pos, 1, "&lt;" );
        pos += 4;
    }
    pos = 0;
    while ( ( pos = text.find( ">", pos ) ) >= 0 )
    {
        text = text.replace( pos, 1, "&gt;" );
        pos += 4;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience function to write an XML file footer.
 *
 *  \param fptr Output file stream.
 *  \param name Document tag used to start the file.
 *
 *  Writes the last line which is "</name>".
 */

void xmlWriteFooter( FILE *fptr, const QString &name )
{
    fprintf( fptr, "</%s>\n", name.latin1() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience function to write a standard XML file header.
 *
 *  \param fptr Output file stream.
 *  \param name Document tag used to start the file.
 *  \param type Document tag type.
 *
 *  The \a name and \a type go together to form the document tag with the format
 *  <name type="type">
 */

void xmlWriteHeader( FILE *fptr, const QString &name, const QString &type,
        int release )
{
    fprintf( fptr, "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"yes\" ?>\n\n" );
    fprintf( fptr, "<!-- Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved. -->\n\n" );
    fprintf( fptr, "<%s type=\"%s\" release=\"%d\">\n",
        name.latin1(), type.latin1(), release );
}
