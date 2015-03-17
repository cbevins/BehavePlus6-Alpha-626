//------------------------------------------------------------------------------
/*! \file apptranslator.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared translator access.
 *
 *  These functions offer a simple, application-wide text translation
 *  facility.
 *
 *  Normally, a single instance of the translation dictionary is maintained
 *  in translator.cpp as a static object.
 *
 *  The application creates a Translator via the global translatorCreate().
 *
 *  The application fills the Translator with look up keys and text using
 *  the global translatorInsert( key, language, text ).  Example calls are:
 *  \arg translatorInsert( "Yes", "en_US", "Yes" );
 *  \arg translatorInsert( "Yes", "es_MX", "Si" );
 *  \arg translatorInsert( "Yes", "es_Sp", "Si" );
 *  \arg translatorInsert( "Yes", "Germany_German", "Yah" );
 *
 *  The current language is set via translatorSetLanguage(),
 *  and a language can be tested via translatorLanguageDefined().
 *
 *  Translation is achieved via the global translate(), which uses the
 *  current language to build a key, retrieve the text, and perform any
 *  parameter substitution before returning the text.
 *
 *  Finally, the Translator is destroyed via translatorDelete().
 */

#ifndef _APPTRANSLATOR_H_
/*! \def _APPTRANSLATOR_H_
 *  \brief Prevent redundant includes.
 */
#define _APPTRANSLATOR_H_ 1

// Qt class references
#include <qdict.h>
#include <qstring.h>

// Function declarations

QDict<QString> *appTranslator( void ) ;

void appTranslatorCreate( int primeNumber ) ;

void appTranslatorDelete( void ) ;

QString *appTranslatorFind( const QString &key ) ;

void appTranslatorInsert( const QString &key, const QString &language,
        const QString &text, bool ignoreDup=false ) ;

bool appTranslatorLanguageDefined( const QString &language ) ;

bool appTranslatorRemove( const QString &key, const QString &language ) ;

void appTranslatorSetLanguage( const QString &language ) ;

void translate( QString &dst, const QString &key,
        const QString &arg1=0, const QString &arg2=0,
        const QString &arg3=0, const QString &arg4=0,
        const QString &arg5=0, const QString &arg6=0 ) ;

#endif

//------------------------------------------------------------------------------
//  End of apptranslator.h
//------------------------------------------------------------------------------

