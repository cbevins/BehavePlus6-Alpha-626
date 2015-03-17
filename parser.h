//------------------------------------------------------------------------------
/*! \file parser.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Qt-based text token parser.
 */

// Prevent redundant includes
#ifndef _PARSER_H_
/*! \def _PARSER_H_
    \brief Prevent redundant includes.
 */
#define _PARSER_H_ 1

// Qt class references
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class parser parser.h
 *
 *  \brief Qt text token parser.
 */

class Parser
{
// Public methods
public:
    Parser( void ) ;
    Parser( const QString &whiteSpace,
            const QString &quoteOpen="\"\'",
            const QString &quoteClose="\"\'",
            int maxTokens=400 ) ;
    ~Parser( void ) ;
    QString error( void ) const ;
    int     length( int index ) const ;
    int     parse( const QString &string ) ;
    int     position( int index ) const ;
    QString token( int index ) const ;
    int     tokens( void ) const ;

// Public data
public:
    QString m_whiteSpace;   //!< String containing all white space
    QString m_quoteOpen;    //!< String containing all beginning quotes
    QString m_quoteClose;   //!< String containing all ending quotes
    QString m_string;       //!< The string to be parsed
    QString m_error;        //!< Parser error string
    int     m_maxTokens;    //!< Size of m_pos[] and m_len[]
    int     m_tokens;       //!< Number of parsed tokens
    int    *m_pos;          //!< Array of m_string token starting positions
    int    *m_len;          //!< Array of m_string token lengths
};

#endif

//------------------------------------------------------------------------------
//  End of parser.h
//------------------------------------------------------------------------------

