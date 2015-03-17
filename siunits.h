//------------------------------------------------------------------------------
/*! \file siunits.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief A class to parse phrases describing source and destination
 *  units of measure and to convert between them.
 */

#ifndef _SI_UNITS_H_
/*! \def _SI_UNITS_H_
    \brief Prevent redundant includes.
 */
#define _SI_UNITS_H_ 1

// Qt include files
#include <qasciidict.h>
#include <qobject.h>
#include <qstrlist.h>

// Standard include files
#include <stdio.h>

// Array sizes
const int SIUnits_Exponents = 11;
const int SIUnits_MaxTerms  = 32;
const int SIUnits_MsgBuffer = 1024;

// "which" (source or destination) argument values
const int SIUnits_Src = 0;
const int SIUnits_Dst = 1;

// Compile status codes.
const int SIUnits_StatusOk = 0;
const int SIUnits_StatusBadSrcAlias = 1;
const int SIUnits_StatusBadDstAlias = 2;
const int SIUnits_StatusIncompatible = 3;

//------------------------------------------------------------------------------
/*! \class SIUnitData siunits.h
 *
 *  \brief Contains data on a single unit of measure.
 */

class SIUnitData
{
// Public constructor
public:
    SIUnitData( const char *desc, const char *baseUnits, int reference,
        int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
        int b8, int b9, int b10, double factor );

// Private data members
public:
    const char *desc;           // Full description
    const char *baseUnits;      // Compilable name of base units
    int         reference;      // SI table or other source record
    int         baseExp[SIUnits_Exponents];
    int         method;         // 0=multiplication
    double      factor;         // Conversion factor into base units
};

//------------------------------------------------------------------------------
/*! \class SIUnits siunits.h
 *
 *  \brief Contains all the known units of measure for English language.
 */

class SIUnits
{
// Member functions
public:
    SIUnits( void );
    ~SIUnits( void );
    bool conversionFactorOffset(
            const char *srcUnits, const char *dstUnits,
            double *factor, double *offset ) ;
    bool convert( double srcValue, const char *srcPhrase,
                  const char *dstPhrase, double *dstValue ) ;
    void dumpLongList( FILE *fptr ) ;
    void dumpShortList( FILE *fptr ) ;
    void dumpTableSize( FILE *fptr ) ;
    bool equivalent( const char *units1, const char *units2 ) ;
    SIUnitData *find( const char *alias ) ;
    QString htmlMsg( void ) const ;
    QString nameMsg( void ) const ;

private:
    QString &baseName( int which, bool type=true ) ;

    bool compatible( ) ;

    bool compile( int which, const char *unitPhrase, bool debug=false ) ;

    const char *derivedName( int which, bool typeName=true ) ;

    void define( const char *desc, const char *baseUnits, int reference,
        int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
        int b8, int b9, int b10, double factor, const char *a1,
        const char *a2=0, const char *a3=0, const char *a4=0, const char *a5=0,
        const char *a6=0, const char *a7=0, const char *a8=0, const char *a9=0,
        const char *a10=0, const char *a11=0, const char *a12=0 ) ;

    void defineAlias( const char *alias, const char *desc ) ;

    void defineUnits( void ) ;

    void defineData( const char *desc, const char *baseUnits, int reference,
            int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
            int b8, int b9, int b10, double factor ) ;

// Public data members
public:
    int       m_status;                     // Compilation result code
    QString   m_htmlMsg;                    // HTML error message
    QString   m_nameMsg;                    // baseName() phrase buffer
    QString   m_textMsg;                    // Standard text error message
    QStrList *m_shortList;                  // Sorted list of unit's FIRST alias

// Private data members
private:
    QAsciiDict<SIUnitData> *m_dataDict;     // Unit data dictionary
    QAsciiDict<char>       *m_aliasDict;    // Unit alias-to-dictionary link
    QStrList               *m_aliasList;    // Sorted unit alias list
    int                     m_definedUnits; // Number of units defined in m_dataDict
    // The following exist for the SIUnits_Src (0) and SIUnits_Dst (1)
    int         m_terms[2];                 // Unit terms parsed from source phrase
    int         m_expon[2][SIUnits_MaxTerms]; // Phrase accumulated power exponents
    const char *m_phrase[2];                // Pointer to original phrase
    SIUnitData *m_udPtr[2][SIUnits_MaxTerms]; // Ptrs to phrase term SIUnitData
    double      m_factor[2];                // Conversion multiplier into SI base unit
};

#endif

//------------------------------------------------------------------------------
//  End of siunits.h
//------------------------------------------------------------------------------
