//------------------------------------------------------------------------------
/*! \file xmlparser.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus XML definition document parser base class declarations.
 *
 *  Base class for parsing EqApp, EqTree, and Property XML documents.
 */

#ifndef _XMLPARSER_H_
/*! \def _XMLPARSER_H_
    \brief Prevent redundant includes.
 */
#define _XMLPARSER_H_ 1

// Qt class references
#include <qstring.h>
#include <qxml.h>

//------------------------------------------------------------------------------
/*! \class XmlParser xmlparser.h
 *
 *  \brief Abstract base class from which EqAppParser, EqTreeParser, and
 *  PropertyParser derived.
 */

class XmlParser : public QXmlDefaultHandler
{
// Public methods
public:
    XmlParser( const QString &fileName, bool validate, bool debug ) ;
    // Reimplemented virtual functions
    virtual bool    endElement( const QString &namespaceUri,
                        const QString &localName, const QString &elementName ) ;
    virtual bool    error( const QXmlParseException& e ) ;
    virtual QString errorString( void ) ;
    virtual bool    fatalError( const QXmlParseException& e ) ;
    virtual bool    getAtt( const QString &attName, QString &attText,
                        bool required,
                        const QString &elementName,
                        const QXmlAttributes& attribute ) ;
    virtual bool    getAtt( const QString &attName, int *attValue,
                        bool required,
                        const QString &elementName,
                        const QXmlAttributes& attribute ) ;
    virtual bool    getAtt( const QString &attName, double *attValue,
                        bool required,
                        const QString &elementName,
                        const QXmlAttributes& attribute ) ;
    virtual bool    getAtt( const QString &name, bool *value, bool required,
                        const QString &elementName,
                        const QXmlAttributes& attribute ) ;
    virtual bool    startElement( const QString &namespaceUri,
                        const QString &localName, const QString &elementName,
                        const QXmlAttributes &attribute );
    virtual bool    warning( const QXmlParseException& e ) ;
    // New virtual functions
    virtual void    displayError( const QString &type,
                        const QXmlParseException& e ) ;
    virtual void    push( const QString &element ) ;
    virtual void    trError( const QString &key,
                        const QString &arg1=0, const QString &arg2=0,
                        const QString &arg3=0, const QString &arg4=0 ) ;

// Public data
public:
    QString   m_fileName;       //!< EqApp XML document file name
    QString   m_error;          //!< Error message text
    QString   m_indent;         //!< Indentation level
    QString   m_element[20];    //!< Stack of elements
    int       m_elements;       //!< Stack pointer
    int       m_errors;         //!< Number of errors
    bool      m_validate;       //!< Performs extensive EqApp validation
    bool      m_debug;          //!< Reports XML to cerr
    bool      m_useTranslator;  //!< If TRUE, uses translation dictionary
};

// Convenience routines

QString *rot13( const QString &src ) ;

void xmlEscape( QString &text ) ;

void xmlWriteFooter( FILE *fptr, const QString &name ) ;

void xmlWriteHeader( FILE *fptr, const QString &name, const QString &type,
        int release ) ;

#endif

//------------------------------------------------------------------------------
//  End of xmlparser.h
//------------------------------------------------------------------------------

