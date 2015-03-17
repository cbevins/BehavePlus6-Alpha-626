//------------------------------------------------------------------------------
/*! \file parser.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Qt text token parser.
 */

// Local include files
#include "appmessage.h"
#include "parser.h"

//------------------------------------------------------------------------------
/*! \brief Parser default constructor.
 */

Parser::Parser( void ) :
    m_whiteSpace(" \t\n"),
    m_quoteOpen("\"\'"),
    m_quoteClose("\"\'"),
    m_string(""),
    m_error(""),
    m_maxTokens(400),
    m_tokens(0),
    m_pos(0),
    m_len(0)
{
    // Allocate m_pos[] and m_len[]
    m_pos = new int[ m_maxTokens ];
    checkmem( __FILE__, __LINE__, m_pos, "int m_pos", m_maxTokens );
    m_len = new int[ m_maxTokens ];
    checkmem( __FILE__, __LINE__, m_len, "int m_len", m_maxTokens );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Parser constructor.
 *
 *  \param whiteSpace String containing all whitespace characters.
 *  \param quoteOpen  String containing all opening quote characters.
 *  \param quoteClose String containing all closing quote characters.
 */

Parser::Parser( const QString &whiteSpace, const QString &quoteOpen,
        const QString &quoteClose, int maxTokens ) :
    m_whiteSpace(whiteSpace),
    m_quoteOpen(quoteOpen),
    m_quoteClose(quoteClose),
    m_string(""),
    m_error(""),
    m_maxTokens(maxTokens),
    m_tokens(0),
    m_pos(0),
    m_len(0)
{
    // Allocate m_pos[] and m_len[]
    m_pos = new int[m_maxTokens];
    checkmem( __FILE__, __LINE__, m_pos, "int m_pos", m_maxTokens );
    m_len = new int[m_maxTokens];
    checkmem( __FILE__, __LINE__, m_len, "int m_len", m_maxTokens );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Parser destructor.
 *
 *  \return Copy of the error message.
 */

Parser::~Parser( void )
{
    if ( m_pos )
    {
        delete[] m_pos; m_pos = 0;
    }
    if( m_len )
    {
        delete[] m_len; m_len = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the error message.
 *
 *  \return Copy of the error message.
 */

QString Parser::error( void ) const
{
    return( m_error );
}

//------------------------------------------------------------------------------
/*! \brief Access to the specified token's length.
 *
 *  \param index Index number of the token (base 0).
 *
 *  \return Token's length.
 */

int Parser::length( int index ) const
{
    if ( index < 0 || index >= m_tokens )
    {
        return( -1 );
    }
    return( m_len[index] );
}

//------------------------------------------------------------------------------
/*! \brief Parses the string into tokens.
 *
 *  \param string String to be parsed.
 *
 *  \retval Number of parsed tokens on success.
 *  \retval -1 if errors are found; error message is in m_error.
 */

int Parser::parse( const QString &string )
{
    // Initialization
    m_string = string;
    m_error = "";
    m_tokens = 0;
    int pos = 0;
    int qpos = 0;
    for ( int i = 0;
          i < m_maxTokens;
          i++ )
    {
        m_pos[i] = -1;
        m_len[i] = 0;
    }
    int len = m_string.length();

    // Parse the entire string
    while ( pos < len )
    {
        // Skip leading whitespace.
        while ( m_whiteSpace.contains( m_string[pos], true ) && pos < len )
        {
            pos++;
        }
        // If at end of the string, then we're done.
        if ( pos >= len )
        {
            break;
        }
        // If this token starts with a quote
        if ( ( qpos = m_quoteOpen.find( m_string[pos] ) ) >= 0 )
        {
            // Token starts at the next character
            m_pos[m_tokens] = ++pos;
            // Find the closing quote
            if ( ( pos = m_string.find( m_quoteClose[qpos], pos ) ) < 0 )
            {
                m_error = QString( "Unclosed quoted token beginning with %1 at position %2." )
                    .arg( m_quoteOpen[qpos] ).arg( m_pos[m_tokens] );
                return( -1 );
            }
            // Store the length
            m_len[m_tokens] = pos - m_pos[m_tokens];
            // Done with this token
            pos++;
            m_tokens++;
            if ( m_tokens >= m_maxTokens )
            {
                break;
            }
            continue;
        }
        // Accumulate non-whitespace characters
        m_pos[m_tokens] = pos++;
        m_len[m_tokens]++;
        while ( ! m_whiteSpace.contains( m_string[pos], true ) && pos < len )
        {
            m_len[m_tokens]++;
            pos++;
        }
        m_tokens++;
        if ( m_tokens >= m_maxTokens )
        {
            break;
        }
    }
    return( m_tokens );
}

//------------------------------------------------------------------------------
/*! \brief Access to the specified token's position (base 0) in the string.
 *
 *  \param index Index number of the token (base 0).
 *
 *  \return First position (base 0) of the token in the parsed string.
 */

int Parser::position( int index ) const
{
    if ( index < 0 || index >= m_tokens )
    {
        return( -1 );
    }
    return( m_pos[index] );
}

//------------------------------------------------------------------------------
/*! \brief Access to the specified token.
 *
 *  \param index Index number of the token (base 0).
 *
 *  \return Copy of the specified token.
 */

QString Parser::token( int index ) const
{
    if ( index < 0 || index >= m_tokens )
    {
        return( "" );
    }
    return( m_string.mid( m_pos[index], m_len[index] ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to the number of parsed tokens.
 *
 *  \return Number of parsed tokens.
 */

int Parser::tokens( void ) const
{
    return( m_tokens );
}

//------------------------------------------------------------------------------
//  End of parser.cpp
//------------------------------------------------------------------------------

