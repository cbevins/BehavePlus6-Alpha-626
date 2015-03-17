//------------------------------------------------------------------------------
/*! \file apptranslator.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared translator access.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "platform.h"

#include <qtextcodec.h>

#include <stdio.h>

// Pointer to the shared translation dictionary.
static QDict<QString> *TrDict = 0;

// Current language
static QString Language( "en_US" );
static QString DefaultLanguage( "en_US" );

//------------------------------------------------------------------------------
/*! \brief Access to the application-wide, shared translation dictionary.
 */

QDict<QString> *appTranslator( void )
{
    return( TrDict );
}

//------------------------------------------------------------------------------
/*! \brief Creates an application-wide, shared translation dictionary.
 */

void appTranslatorCreate( int primeNumber )
{
    TrDict = new QDict<QString>( primeNumber, true );
    Q_CHECK_PTR( TrDict );
    TrDict->setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Deletes the application-wide, shared translation dictionary
 *  and its contents.
 */

void appTranslatorDelete( void )
{
    delete TrDict;  TrDict = 0;
    Language = "en_US";
    return;
}

//------------------------------------------------------------------------------
/*! \brief Finds the specified key in the application-wide, shared translation
 *  dictionary.
 *
 *  If the key is not found in the dictionary, a second search is performed
 *  for the default language.
 *
 *  \param key  Phrase look-up key.  This is usually a hierarchical key with
 *              sub-keys separated by colons (:) and whose last sub-key is the
 *              language label.
 *  \param defaultLang  Check this language if the \a key isn't found.
 *
 *  \retval Pointer to the key's text, on success.
 *  \retval 0 on failure.
 */

QString *appTranslatorFind( const QString &key  )
{
    QString *str = 0;
    if ( ! ( str = TrDict->find( key ) ) )
    {
        int pos = key.findRev( ":" );
        QString defKey = key.left( pos + 1 ) + DefaultLanguage;
        return( TrDict->find( defKey ) );
    }
    return( str );
}

//------------------------------------------------------------------------------
/*! \brief Adds some new \a text in the specified \a language under the \a key.
 *
 *  \param key  Look up key used within the program to locate the text.
 *  \param lang Language, which when combined with the \a key, provides a
 *              unique lookup key into the dictionary.
 *  \param text Reference to the text to be stored under the key:lang.
 *              Note that the translator makes its own deep copy of \a text
 *              to store.
 *
 *  The function makes sure the text doesn't already exists,
 *  and keeps a list of known languages.
 */

void appTranslatorInsert( const QString &key, const QString &language,
        const QString &text, bool ignoreDup )
{
    // The dictionary must have been created.
    if ( ! TrDict )
    // This code block should never be executed!
    {
        bomb( "Attempt to insert into Translation Dictionary before it is created." );
    }
    // Create the full key.
    QString fullKey( key + ":" + language );
    // Does this exist already?
    if ( TrDict->find( fullKey ) )
    {
        if ( ignoreDup )
        {
            return;
        }
        error( QString( "Translation Dictionary key \"%1\" previously defined." )
            .arg( fullKey ) );
    }
    // Add the text.
    QString *str = new QString( text );
    Q_CHECK_PTR( str );
    TrDict->insert( fullKey, str );

    // Add the Language itself to the dictionary
    // so we have a way of finding out what languages are known.
    fullKey.setLatin1( "Language:" + language );
    if ( ! TrDict->find( fullKey ) )
    {
        str = new QString( language );
        Q_CHECK_PTR( str );
        TrDict->insert( fullKey, str );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines if the \a language is known to the translator.
 *
 *  \retval TRUE if the language is known.
 *  \retval FALSE if the language is unknown.
 */

bool appTranslatorLanguageDefined( const QString &language )
{
    QString fullKey( "Language:" + language );
    return(  TrDict->find( fullKey ) ) ;
}

//------------------------------------------------------------------------------
/*! \brief Removes the specified key and its text if it exists.
 *
 *  \retval TRUE if the key:language was found and removed.
 *  \retval FALSE if the key:language was not found or not removed.
 */

bool appTranslatorRemove( const QString &key, const QString &language )
{
    if ( ! TrDict )
    // This code block should never be executed!
    {
         return( false );
    }
    // Create the full key.
    QString fullKey( key + ":" + language );
    return( TrDict->remove( fullKey ) );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current language for shared translation dictionary.
 */

void appTranslatorSetLanguage( const QString &language )
{
    // Make sure the language is known
    QString fullKey( "Language:" + language );
    if ( ! TrDict->find( fullKey ) )
    // This code block should never be executed!
    {
        bomb( QString( "translatorSetLanguage(%1) -- unknown language." )
            .arg( language ) );
    }
    // Store this as the current language
    Language = language;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Translates a message with zero to seven QString arguments embedded
 *  into the current \a Language.
 *
 *  \param dst  Destination QString for translated text.
 *  \param key  Dictionary key to which ":<lang>" is appended.
 *              If the key begins with a bang (!) then:
 *              translation is NOT performed, and the key after the bang is
 *              stored into the returned \a dst.
 *  \param arg1 Optional argument(s) applied to the recovered string
 *              which is assumed to be a QString format.
 */

void translate( QString &dst, const QString &key,
        const QString &arg1, const QString &arg2,
        const QString &arg3, const QString &arg4,
        const QString &arg5, const QString &arg6 )
{
    // If there is no translation dictionary
    if ( ! TrDict )
    // This code block should never be executed!
    {
        // Set the destination to the key and return it.
        dst = key;
        return;
    }
    // If key is NULL or empty, return an empty string.
    if ( key.isNull() || key.isEmpty() )
    // This code block should never be executed!
    {
        dst = "";
        return;
    }
    // Check for the bang (!), which means translate NOT.
    if ( key.left( 1 ) == "!" )
    {
        dst = key.mid( 1 );
        return;
    }
    // Attempt to find the key in the target language
    QString fullKey( key + ":" + Language );
    QString *fmt;
    if ( ! ( fmt = appTranslatorFind( fullKey ) ) )
    //if ( ! ( fmt = TrDict->find( fullKey ) ) )
    // This code block should never be executed!
    {
fprintf( stderr, "translate( key=\"%s\" ) unable to find full key \"%s\".\n",
key.latin1(), fullKey.latin1() );
platformExit(1);
        error( QString(
            "translate( key=\"%1\" ) unable to find full key \"%2\".\n"
            "Will now exit the program." ).arg( key ).arg( fullKey) );
        platformExit(1);
    }
    // Format translated text with embedded parameters.
    if ( arg1.isNull() )
    {
        dst = QString( *fmt );
    }
    else if ( arg2.isNull() )
    {
        dst = QString( *fmt ).arg( arg1 );
    }
    else if ( arg3.isNull() )
    {
        dst = QString( *fmt ).arg( arg1 ).arg( arg2 );
    }
    else if ( arg4.isNull() )
    {
        dst = QString( *fmt ).arg( arg1 ).arg( arg2 ).arg( arg3 );
    }
    else if ( arg5.isNull() )
    {
        dst = QString( *fmt ).arg( arg1 ).arg( arg2 ).arg( arg3 ).arg( arg4 );
    }
    else if ( arg6.isNull() )
    {
        dst = QString( *fmt ).arg( arg1 ).arg( arg2 ).arg( arg3 ).arg( arg4 )
             .arg( arg5 );
    }
    else
    {
        dst = QString( *fmt ).arg( arg1 ).arg( arg2 ).arg( arg3 ).arg( arg4 )
             .arg( arg5 ).arg( arg6 );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of apptranslator.cpp
//------------------------------------------------------------------------------

