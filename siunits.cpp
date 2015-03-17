//------------------------------------------------------------------------------
/*! \file siunits.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief A class to parse phrases describing source and destination
 *  units of measure and to convert between them.
 *
 *  SI base units, derived units, symbols, and conversion factors follow
 *  the 20th CGPM as described by:
 *  Taylor, Barry N. 1995.  Guide for the use of the International System
 *      of Units (SI).  1995 Edition.  NIST Special Publication 811.
 *       Physics Laboratory, National Institute of Standards and Technology,
 *      Gaithersburg, MD 20899-0001.  86p.
 *
 *  A units "phrase" consists of 0 or more "terms".
 *    Each term is either a "units term" or a "divisor term".
 *      A "divisor term" is either:
 *        - a forward slash with or without leading or trailing
 *          whitespace, or
 *        - the word "per" with leading and trailing whitespace.
 *      A "units term" is a single word (no embedded whitespace) containing
 *        - an optional SI prefix (e.g., "kilo")
 *        - an SI units name, symbol, or abbreviation
 *        - an optional carat ("^") preceeding an exponent
 *        - an optional signed or unsigned exponent.
 *
 *  All non-alphabetic, non-digit, and non-slash characters are treated
 *  as whitespace and may be used to separate terms.
 *
 *  All terms appearing before the first divisor term are considered
 *  part of the numerator, and their exponents are applied "as is".
 *  All terms appearing after the first (and any subsequent) divisor terms
 *  are considered part of the denominator, and their exponents are assigned
 *  the opposite sense.
 *  Thus, "kg/m2", "kg m-2", and "m-2/kg-1" are all equivalent.
 */

// Custom include files
#include "appmessage.h"
#include "siunits.h"

// Standard include files
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char  *prefix;
    char  *abbrev;
    double factor;
} UnitsPrefixData, *UnitsPrefixPtr;

static UnitsPrefixData UnitsPrefix[] =
{
    {"yotta", "Y",  1.0e+24},
    {"zetta", "Z",  1.0e+21},
    {"exa",   "E",  1.0e+18},
    {"peta",  "P",  1.0e+15},
    {"tera",  "T",  1.0e+12},
    {"giga",  "G",  1.0e+09},
    {"mega",  "M",  1.0e+06},
    {"kilo",  "k",  1.0e+03},
    {"hecto", "h",  1.0e+02},
    {"deka",  "da", 1.0e+01},
    {"deci",  "d",  1.0e-01},
    {"centi", "c",  1.0e-02},
    {"milli", "m",  1.0e-03},
    {"micro", "mu", 1.0e-06},
    {"nano",  "n",  1.0e-09},
    {"pico",  "p",  1.0e-12},
    {"femto", "f",  1.0e-15},
    {"atto",  "a",  1.0e-18},
    {"zepto", "z",  1.0e-21},
    {"yocto", "y",  1.0e-24},
    {NULL,    NULL, 1.0e+00}
};

static const char *BaseTypeName[] =
{
    "class",
    "distance",
    "mass",
    "time",
    "electric current",
    "temperature",
    "luminous intensity",
    "substance",
    "plane angle",
    "solid angle",
    "ratio"
};

static const char *BaseUnitsName[] =
{ "", "m", "kg", "s", "A", "K", "cd", "mol", "rad", "sr", "dl", NULL };

//------------------------------------------------------------------------------
//  SI Derived Units.
//------------------------------------------------------------------------------

typedef struct
{
    char  *typeName;                    // SI units name.
    int   baseExp[SIUnits_Exponents];   // Base unit exponents.
    const char *derivedName;            // SI dderived name/
} DerivedUnitData;

static DerivedUnitData DerivedUnit[] = {
// Base units                   c  l  m  t  e  T  i  s  a  s  r
//                              c  m kg  s  A  K cd ml rad str dl
    {"class",                  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "class" },
    {"distance",               {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "meter" },
    {"mass",                   {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, "kilogram" },
    {"time",                   {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, "second" },
    {"electric current",       {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, "ampere" },
    {"temperature",            {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, "kelvin" },
    {"luminous intensity",     {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, "candela" },
    {"substance",              {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, "mole" },
    {"plane angle",            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, "radian (m/m)" },
    {"solid angle",            {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, "steradian (m2/m2)" },
    {"ratio",                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, "ratio" },
// Derived units                c  l  m  t  e  T  i  s  a s r
//                              c  m kg  s  A  K cd ml rad str dl
    {"absorbed dose",          {0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0}, "gray (J/kg)" },
    {"ambient dose equivalent",{0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0}, "sievert (J/kg)" },
    {"acceleration",           {0, 1, 0,-2, 0, 0, 0, 0, 0, 0, 0}, "(m/s2)" },
    {"radionuclide activity",  {0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0}, "becquerel" },
    {"radiology",              {0, 0,-1, 1, 1, 0, 0, 0, 0, 0, 0}, "C/kg" },
    {"angular acceleration",   {0, 0, 0,-2, 0, 0, 0, 0, 1, 0, 0}, "(rad/m2)" },
    {"angular velocity",       {0, 0, 0,-1, 0, 0, 0, 0, 1, 0, 0}, "(rad/s)" },
    {"area",                   {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "(m2)" },
    {"capacitance",            {0,-2,-1, 4, 2, 0, 0, 0, 0, 0, 0}, "farad (C/V)" },
    {"density",                {0,-3, 1, 0, 0, 0, 0, 0, 0, 0, 0}, "(kg/m3)" },
    {"dynamic viscosity",      {0,-1, 1,-1, 0, 0, 0, 0, 0, 0, 0}, "Pa s" },
    {"electric charge",        {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, "coulomb (s A)" },
    {"electric conductance",   {0,-2,-1, 3, 2, 0, 0, 0, 0, 0, 0}, "siemens (A/V)" },
    {"electric field strength",{0, 1, 1,-3,-1, 0, 0, 0, 0, 0, 0}, "V/m" },
    {"electric resistance",    {0, 2, 1,-3,-2, 0, 0, 0, 0, 0, 0}, "ohm (V/A)" },
    {"entropy",                {0, 2, 1,-2, 0,-1, 0, 0, 0, 0, 0}, "J/K" },
    {"force",                  {0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0}, "newton" },
    {"frequency",              {0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0}, "Hz" },
    {"fuel consumption",       {0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "m/m3" },
    {"heat density",           {0, 0, 1,-2, 0, 0, 0, 0, 0, 0, 0}, "J/m2" },
    {"illuminance",            {0,-2, 0, 0, 0, 0, 1, 0, 0, 1, 0}, "lux (lm/m2)" },
    {"inductance",             {0, 2, 1,-2,-2, 0, 0, 0, 0, 0, 0}, "henry" },
    {"kinematic viscosity",    {0, 2, 0,-1, 0, 0, 0, 0, 0, 0, 0}, "m2/s" },
    {"thermal diffusivity",    {0, 2, 0,-1, 0, 0, 0, 0, 0, 0, 0}, "m2/s" },
    {"luminance",              {0,-2, 0, 0, 0, 0, 1, 0, 0, 0, 0}, "cd/m2" },
    {"luminous flux",          {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0}, "lumen (cd sr)" },
    {"magnetic field strength",{0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 0}, "A/m" },
    {"magnetic flux density",  {0, 0, 1,-2,-1, 0, 0, 0, 0, 0, 0}, "tesla (Wb/m2)" },
    {"magnetic flux",          {0, 2, 1,-2,-1, 0, 0, 0, 0, 0, 0}, "weber (V s)" },
    {"permeability",           {0,-1, 0, 1, 0, 0, 0, 0, 0, 0, 0}, "s/m" },
    {"potential difference",   {0, 2, 1,-3,-1, 0, 0, 0, 0, 0, 0}, "volt (W/A)" },
    {"power, radiant flux",    {0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0}, "watt (J/s)" },
    {"pressure",               {0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0}, "pascal (N/m2)" },
    {"radiant intensity",      {0, 2, 1,-3, 0, 0, 0, 0, 0,-1, 0}, "W/sr" },
    {"revolutions",            {0, 0, 0,-1, 0, 0, 0, 0, 1, 0, 0}, "rad/s" },
    {"specific heat capacity", {0, 2, 0,-2, 0,-1, 0, 0, 0, 0, 0}, "J/kg-K" },
    {"thermal conductivity",   {0, 1, 1,-3, 0,-1, 0, 0, 0, 0, 0}, "W/m-K" },
    {"thermal insulance",      {0, 0,-1, 3, 0, 1, 0, 0, 0, 0, 0}, "m2 K/W" },
    {"velocity",               {0, 1, 0,-1, 0, 0, 0, 0, 0, 0, 0}, "m/s" },
    {"volume",                 {0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "m3" },
    {"work, energy, or heat",  {0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0}, "joule" },
// The following were added.
    {"loading",                {0,-2, 1, 0, 0, 0, 0, 0, 0, 0, 0}, "kg/m2" },
    {"surface area/volume",    {0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "m2/m3" },
// These duplicate other entries and have a lower precedence.
    {"wave number",            {0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "1/m" },
    {NULL,                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, NULL }
};

//------------------------------------------------------------------------------
/*! \brief SIUnitData constructor
 *  Usually called only by SIUnit::defineData() from SIUnit::SIUnit().
 */

SIUnitData::SIUnitData( const char *desc, const char *baseUnits, int reference,
        int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
        int b8, int b9, int b10, double factor ) :
    desc(desc),
    baseUnits(baseUnits),
    reference(reference),
    method(0),
    factor(factor)
{
    baseExp[0] = b0;
    baseExp[1] = b1;
    baseExp[2] = b2;
    baseExp[3] = b3;
    baseExp[4] = b4;
    baseExp[5] = b5;
    baseExp[6] = b6;
    baseExp[7] = b7;
    baseExp[8] = b8;
    baseExp[9] = b9;
    baseExp[10] = b10;
    return;
}

//------------------------------------------------------------------------------
/*! \brief SIUnits constructor.
 *  Initializes variables AND defines all the units.
 */

SIUnits::SIUnits( ) :
    m_status(0),
    m_htmlMsg(""),
    m_nameMsg(""),
    m_textMsg(""),
    m_shortList(0),
    m_dataDict(0),
    m_aliasDict(0),
    m_aliasList(0),
    m_definedUnits(0)
{
    // Initialize the ptr arrays
    for ( int which = 0;
          which < 2;
          which++ )
    {
        m_terms[which] = 0;
        m_phrase[which] = 0;
        m_factor[which] = 1.0;
        for ( int term = 0;
              term < SIUnits_MaxTerms;
              term++ )
        {
            m_udPtr[which][term] = 0;
            m_expon[which][term] = 0;
        }
    }
    // Create the hash tables and lists
    m_dataDict  = new QAsciiDict<SIUnitData>( 277, true, false );
    Q_CHECK_PTR( m_dataDict );
    m_dataDict->setAutoDelete( true );
    m_aliasDict = new QAsciiDict<char>( 919, true, false );
    Q_CHECK_PTR( m_aliasDict );
    m_aliasList = new QStrList( false );
    Q_CHECK_PTR( m_aliasList );
    m_shortList = new QStrList( false );
    Q_CHECK_PTR( m_shortList );
    // Define all the units
    defineUnits();
    return;
}

//------------------------------------------------------------------------------
/*! \brief SIUnits destructor.
 */

SIUnits::~SIUnits( void )
{
    delete m_dataDict;      m_dataDict = 0;
    delete m_aliasDict;     m_aliasDict = 0;
    delete m_aliasList;     m_aliasList = 0;
    delete m_shortList;     m_shortList = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates an English units phrase consisting of either
 *      (1) if type==true, the SI base type names (distance, mass, etc)
 *      (2) if type==false, the SI base unit names (m, kg, etc)
 *          for the specified compiled SIUnits term
 *          (which == SIUnits_Src or SIUnits_Dst)
 *
 *      Example SI base name phrases for some compiled phrases include:
 *        Phrase        BaseTypeName        BaseUnitName
 *          m/s         distance / time
 *          lb/ft2
 *          volt        distance^2 mass / time^3 current
 *
 *  \return Reference to the m_nameMsg data member,
 *  WHICH IS OVERWRITTEN WITH EACH CALL!
 */

QString &SIUnits::baseName( int which, bool type )
{
    QString num("");
    QString den("");
    long term, j, cumexp[SIUnits_Exponents];

    // Initialize exponents of each SI base type.
    for ( j = 0;
          j < SIUnits_Exponents;
          j++ )
    {
        cumexp[j] = 0;
    }

    // Determine exponent of each base unit type for this units phrase.
    for ( term = 0;
          term < m_terms[which];
          term++ )
    {
        for ( j = 0;
              j < SIUnits_Exponents;
              j++ )
        {
            cumexp[j] +=
                ( m_expon[which][term] * m_udPtr[which][term]->baseExp[j] );
        }
    }

    // Compose the units phrase separately for numerator and denominator.
    for ( j = 0;
          j < SIUnits_Exponents;
          j++ )
    {
        if ( cumexp[j] > 1 )
        {
            num += QString( "%1^%2 " )
                .arg( type
                      ? BaseTypeName[j]
                      : BaseUnitsName[j] )
                .arg( cumexp[j] );
        }
        else if ( cumexp[j] > 0 )
        {
            num += QString( "%1 " )
                .arg( type
                      ? BaseTypeName[j]
                      : BaseUnitsName[j] );
        }
        else if ( cumexp[j] < -1 )
        {
            den += QString( "%1^%2 " )
                .arg( type
                      ? BaseTypeName[j]
                      : BaseUnitsName[j] )
                .arg( -(cumexp[j]) );
        }
        else if ( cumexp[j] < 0 )
        {
            den += QString( "%1 " )
                .arg( type
                      ? BaseTypeName[j]
                      : BaseUnitsName[j] );
        }
    }

    // Now paste the numerator and denominator together.
    if ( strlen( num ) < 1 )
    {
        m_nameMsg = "1 ";
    }
    else
    {
        m_nameMsg = num;
    }

    if ( strlen( den ) > 1 )
    {
        m_nameMsg += QString( "/ %1" ).arg( den );
    }
    m_nameMsg = m_nameMsg.stripWhiteSpace();
    return( m_nameMsg );
}

//------------------------------------------------------------------------------
/*! \brief Determines whether the two compiled SIUnitData structures
 *  are compatible for conversion.
 *
 *  \return TRUE if compatible, FALSE if not compatible.
 */

bool SIUnits::compatible( )
{
    long i, j, srcExpon[SIUnits_Exponents], dstExpon[SIUnits_Exponents];
    QString base;
    const char *dPtr;

    for ( j = 0;
          j < SIUnits_Exponents;
          j++ )
    {
        srcExpon[j] = 0;
        dstExpon[j] = 0;
    }

    for ( i = 0;
          i < m_terms[SIUnits_Src];
          i++ )
    {
        for ( j = 0;
              j < SIUnits_Exponents;
              j++ )
        {
            srcExpon[j] +=
                (m_expon[SIUnits_Src][i] * m_udPtr[SIUnits_Src][i]->baseExp[j]);
        }
    }

    for ( i = 0;
          i < m_terms[SIUnits_Dst];
          i++ )
    {
        for ( j = 0;
              j < SIUnits_Exponents;
              j++ )
        {
            dstExpon[j] +=
                (m_expon[SIUnits_Dst][i] * m_udPtr[SIUnits_Dst][i]->baseExp[j]);
        }
    }

    for ( j = 0;
          j < SIUnits_Exponents;
          j++ )
    {
        if ( srcExpon[j] != dstExpon[j] )
        {
            m_status = SIUnits_StatusIncompatible;

            m_textMsg = QString(
                "Source units \"%1\" are incompatible with "
                "destination units \"%2\".\n" )
                .arg( m_phrase[SIUnits_Src] ).arg( m_phrase[SIUnits_Dst] );

            m_htmlMsg = QString(
                "Source units <b>%1</b> are incompatible with "
                "destination units <b>%2</b>.<br><br>" )
                .arg( m_phrase[SIUnits_Src] ).arg( m_phrase[SIUnits_Dst] );

            base = baseName( SIUnits_Src, true );
            dPtr = derivedName( SIUnits_Src, true );
            if ( dPtr != NULL && base != QString( dPtr ) )
            {
                m_textMsg += QString(
                    "Source units describe \"%1\" (%2) " )
                    .arg( dPtr ).arg( base );
                m_htmlMsg += QString(
                    "Source units describe <b>%1</b> (%2) " )
                    .arg( dPtr ).arg( base );
            }
            else
            {
                m_textMsg += QString(
                    "Source units describe \"%1\" " ).arg( base );
                m_htmlMsg += QString(
                    "Source units describe <b>%1</b> " ).arg( base );
            }

            base = baseName( SIUnits_Src, false );
            m_textMsg += QString(
                 "which reduces to SI base units \"%2\".\n" )
                 .arg( base );
            m_htmlMsg += QString(
                 "which reduces to SI base units <b>%2</b>.<br><br>" )
                 .arg( base );

            base = baseName( SIUnits_Dst, true );
            dPtr = derivedName( SIUnits_Dst, true ) ;
            if ( dPtr != NULL && base != QString( dPtr ) )
            {
                m_textMsg += QString(
                    "Destination units describe \"%1\" (%2) " )
                    .arg( dPtr ).arg( base );
                m_htmlMsg += QString(
                    "Destination units describe <b>%1</b> (%2) " )
                    .arg( dPtr ).arg( base );
            }
            else
            {
                m_textMsg += QString(
                    "Destination units describe \"%1\" " ).arg( base );
                m_htmlMsg += QString(
                    "Destination units describe <b>%1</b> " ).arg( base );
            }

            base = baseName( SIUnits_Dst, false );
            m_textMsg += QString(
                "which reduces to SI base units \"%1\".\n" ).arg( base );
            m_htmlMsg += QString(
                "which reduces to SI base units <b>%1</b>.<br>" ).arg( base );

            return( false );
        }
    }

    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Parses an English phrase describing units of measure
 *  and compiles it into SIUnit member variables for later use.
 *
 *  \retval TRUE on success.
 *  \retval FALSE if unknown or improperly formed units term;
 *  check m_status and m_textMsg or m_htmlMsg for error mesages.
 */

bool SIUnits::compile( int which, const char *unitsPhrase, bool debug )
{
    // The following non-alphanumeric chars [%_"'] are allowed in term names
    static const char *allow = {"%_\'\""};
    char const *kPtr;               // Pointer into unitsPhrase
    char        buffer[256], *bPtr; // Local transformed copy of unitsPhrase
    char       *ePtr, *fPtr;        // End-of-term and front-of-term pointers
    char       *pPtr;               // UnitsPrefix[] pointer.
    int         denom = 1;
    int         i;
    SIUnitData *uPtr;
    double      prefix;

    // Initialize the UnitsData.
    m_status = SIUnits_StatusOk;
    m_phrase[which] = unitsPhrase;
    m_factor[which] = 1.0;
    m_terms[which] = 0;
    for ( i = 0;
          i < SIUnits_MaxTerms;
          i++ )
    {
        m_udPtr[which][i] = 0;
        m_expon[which][i] = 0;
    }

    // Special handling for empty unitsPhrase
    if ( strcmp( unitsPhrase, "" ) == 0 )
    {
        m_udPtr[which][0] = find( unitsPhrase );
        m_expon[which][0] = 1;
        m_terms[which] = 1;
        return( true );
    }

    // The first pass copies unitsPhrase into the local buffer[]
    // while converting non-term name chars to blanks.
    memset( buffer, '\0', sizeof(buffer) );
    for ( bPtr = buffer, kPtr=unitsPhrase;
          *kPtr;
          kPtr++, bPtr++ )
    {
        if ( isalnum( *kPtr )
          || strchr( allow, *kPtr )
          || (*kPtr == '^' &&
             (isdigit( *(kPtr+1) ) || *(kPtr+1)=='+' || *(kPtr+1)=='-' ) )
          || (*kPtr == '-' && isdigit( *(kPtr+1) ) )
          || (*kPtr == '+' && isdigit( *(kPtr+1) ) ) )
        {
            *bPtr = *kPtr;
        }
        else if ( *kPtr == '/' )
        {
            *bPtr++ = ' ';
            *bPtr++ = '/';
            *bPtr   = ' ';
        }
        else
        {
            *bPtr = ' ';
        }
    }
    // Print buffer after replacement
    if ( debug )
    // This code block should never be executed!
    {
        fprintf( stderr, "m_phrase[%d] = [%s]\n", which, unitsPhrase );
        fprintf( stderr, "buffer[%d] = [%s]\n", which, buffer );
    }

    // Throw away any leading white space or numbers such as "1/seconds".
    strtod( buffer, &bPtr );

    // Now parse each space-separated token
    bPtr = strtok( bPtr, " ");
    int bPtrLen;
    while ( bPtr )
    {
        // Must store the strlen() since it may get truncated later.
        bPtrLen = strlen( bPtr );
        // Check for slash "/" or "per".
        if ( strcmp( bPtr, "/" ) == 0 || strcmp( bPtr, "per" ) == 0 )
        {
            denom = -1;
            bPtr = strtok( (bPtr+bPtrLen+1), " " );
            continue;
        }
        // Print the unparsed term and current NUM/DEN flag.
        if ( debug )
        // This code block should never be executed!
        {
            fprintf( stderr, "    term[%d][%d] [%s] %s : ",
                which,
                m_terms[which],
                bPtr,
                ( denom==1 )
                     ? "NUM"
                     : "DEN"
            );
        }
        // Initialize the exponent to the current denom value.
        m_expon[which][m_terms[which]] = denom;

        // Find any trailing exponent
        ePtr = bPtr + strlen( bPtr ) -1 ;       // end-of-term pointer
        if ( isdigit( *ePtr ) )
        {
            while( isdigit( *ePtr ) )
            {
                ePtr--;
            }
            // Accept a single preceeding plus or minus sign
            if ( *ePtr == '-' || *ePtr == '+' )
            {
                ePtr--;
            }
            // Convert to a power
            m_expon[which][m_terms[which]] *= atoi( ePtr+1 );
            *(ePtr+1) = '\0';
            // Consume any preceeding carat
            if ( *ePtr == '^' )
            {
                *ePtr-- = '\0';
            }
        }

        // Check for SI prefix only if NOT kilogram.
        prefix = 1.0;
        fPtr = bPtr;                            // front-of-term pointer
        if ( strncmp( fPtr, "kilog", 5 ) != 0 )
        {
            for ( i = 0;
                  UnitsPrefix[i].prefix != NULL;
                  i++ )
            {
                pPtr = UnitsPrefix[i].prefix;
                // If this is an SI prefix
                if ( strncmp( fPtr, pPtr, strlen(pPtr) ) == 0 )
                {
                    // Set the multiplication factor to the prefix factor
                    prefix = UnitsPrefix[i].factor;
                    // Skip the term pointer past the prefix
                    fPtr += strlen( pPtr );
                }
            }
        }

        // Consume any trailing plural-form
        if ( *ePtr == 's' && strlen(fPtr) > 1 )
        {
            *ePtr-- = '\0';
        }

        // Print prefix, name, and exponent for this term.
        if ( debug )
        // This code block should never be executed!
        {
            fprintf( stderr, " %f [%s] %d +=>",
                prefix, fPtr, m_expon[which][m_terms[which]] );
        }

        // Look up the term
        if ( ! ( uPtr = find( fPtr ) ) )
        {
            // If not found ...
            if ( which == SIUnits_Src )
            {
                m_status = SIUnits_StatusBadSrcAlias;
                m_textMsg += QString(
                    "Source units term \"%1\" is unknown.\n" )
                    .arg( fPtr );
                m_htmlMsg += QString(
                    "Source units term <b>%1</b> is unknown.<br>" )
                    .arg( fPtr );
            }
            else
            {
                m_status = SIUnits_StatusBadDstAlias;
                m_textMsg += QString(
                    "Destination units term \"%1\" is unknown.\n" )
                    .arg( fPtr );
                m_htmlMsg += QString(
                    "Destination units term <b>%1</b> is unknown.<br>" )
                    .arg( fPtr );
            }
            return( false );
        }

        // Accumulate base units conversion factor.
        if ( m_expon[which][m_terms[which]] > 0 )
        {
            for ( i = 0;
                  i < m_expon[which][m_terms[which]];
                  i++ )
            {
                m_factor[which] *= (prefix * uPtr->factor);
            }
        }
        else
        {
            for ( i = m_expon[which][m_terms[which]];
                  i < 0;
                  i++ )
            {
                m_factor[which] /= (prefix * uPtr->factor);
            }
        }

        // Print accumulated factor so far.
        if ( debug )
        // This code block should never be executed!
        {
            fprintf( stderr, " %f\n", m_factor[which] );
        }

        // Store the SIUnitData pointer and increment the term count.
        m_udPtr[which][m_terms[which]] = uPtr;
        m_terms[which]++;

        // Get next token.
        bPtr = strtok( (bPtr+bPtrLen+1), " " );
    }

    // Debug printing
    if ( debug )
    // This code block should never be executed!
    {
        fprintf( stderr, "Result: %s times %f --> %s",
            unitsPhrase, m_factor[which],
            baseName( which, false ).latin1() );
        fprintf( stderr, "  (%s)", baseName( which, true ).latin1() ) ;
        if ( ( kPtr = derivedName( which ) ) )
        {
            fprintf( stderr, " Dervied term is \"%s\"", kPtr );
        }
        fprintf( stderr, "\n\n" );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines conversion factor and offset to convert from srcPhrase
 *  units into dstPhrase units.
 *
 *  Side Effects
 *      On return, "factor" contains multiplication factor.
 *      On return, "offset" contains the offset value.
 *      Such that, dstValue = offset + factor * srcValue.
 *
 *  \retval TRUE on success.
 *  \retval FALSE on error.  The following status codes are set.
 *       0 on success.
 *      -1 if srcPhrase is unknown.
 *      -2 if dstPhrase is unknown.
 *      -3 if srcPhrase and dstPhrase are incompatible.
 */

bool SIUnits::conversionFactorOffset(
    const char *srcUnits, const char *dstUnits,
    double *finalFactor, double *offset )
{
    SIUnitData *cPtr, *fPtr, *kPtr;

    // Initialize returned values.
    *finalFactor = 1.;
    *offset = 0.;

    // Compile the srcUnits; return FALSE on error.
    m_textMsg = m_htmlMsg = "";
    if ( ! compile( SIUnits_Src, srcUnits )
      || ! compile( SIUnits_Dst, dstUnits )
      || ! compatible() )
    {
        return( false );
    }
    // Calculate the return destination value.
    *finalFactor = m_factor[SIUnits_Src] / m_factor[SIUnits_Dst] ;

    // If this is a temperature only conversion, perform offset.
    if ( m_terms[SIUnits_Src] == 1 && m_terms[SIUnits_Dst] == 1 )
    {
        cPtr = find( "oC" );
        fPtr = find( "oF" );
        kPtr = find( "K" );
        if ( m_udPtr[SIUnits_Src][0] == cPtr )
        {
            if ( m_udPtr[SIUnits_Dst][0] == fPtr )
            {
                *offset = 32.0;
            }
            else if ( m_udPtr[SIUnits_Dst][0] == kPtr )
            {
                *offset = 273.15;
            }
        }
        else if ( m_udPtr[SIUnits_Src][0] == fPtr )
        {
            if ( m_udPtr[SIUnits_Dst][0] == cPtr )
            {
                *offset = - (32.0 * 5. / 9.);
            }
            else if ( m_udPtr[SIUnits_Dst][0] == kPtr )
            {
                *offset = 255.372222222;
            }
        }
        else if ( m_udPtr[SIUnits_Src][0] == kPtr )
        {
            if ( m_udPtr[SIUnits_Dst][0] == cPtr )
            {
                *offset = -273.15;
            }
            else if ( m_udPtr[SIUnits_Dst][0] == fPtr )
            {
                *offset = -459.67;
            }
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Performs a complete units conversion.
 *
 *  \return TRUE on success or FALSE on error.
 *  On TRUE, returns the result in the "dstValue" address.
 *  On FALSE, check "m_status" or call statusMessage() or statusHtml().
 */

bool SIUnits::convert( double srcValue, const char *srcPhrase,
                       const char *dstPhrase, double *dstValue )
{
    // Get the conversion factor and offset.
    double factor, offset;
    if ( ! ( conversionFactorOffset( srcPhrase, dstPhrase, &factor, &offset ) ) )
    {
        return( false );
    }
    // Calculate the return destination value.
    *dstValue = offset + srcValue * factor;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Defines a single unit.
 *  Called only by SIUnits::defineUnits();
 */

void SIUnits::define( const char *desc, const char *baseUnits, int reference,
        int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
        int b8, int b9, int b10, double factor, const char *a1,
        const char *a2, const char *a3, const char *a4, const char *a5,
        const char *a6, const char *a7, const char *a8, const char *a9,
        const char *a10, const char *a11, const char *a12 )
{
    defineData( desc, baseUnits, reference,
        b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, factor );
    m_shortList->inSort( a1 );
    defineAlias( a1, desc );
    if ( a2 ) defineAlias( a2, desc );
    if ( a3 ) defineAlias( a3, desc );
    if ( a4 ) defineAlias( a4, desc );
    if ( a5 ) defineAlias( a5, desc );
    if ( a6 ) defineAlias( a6, desc );
    if ( a7 ) defineAlias( a7, desc );
    if ( a8 ) defineAlias( a8, desc );
    if ( a9 ) defineAlias( a9, desc );
    if ( a10) defineAlias( a10, desc );
    if ( a11) defineAlias( a11, desc );
    if ( a12) defineAlias( a12, desc );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the unit's alias to the alias dictionary.
 *  Called only by SIUnits::define();
 */

void SIUnits::defineAlias( const char *alias, const char *desc )
{
    // Make sure this alias doesn't already exist in the dictionary.
    SIUnitData *udPtr;
    if ( ( udPtr = find( alias ) ) != 0 )
    // This code block should never be executed!
    {
        fprintf( stderr, "Alias \"%s\" for \"%s\" already exists for \"%s\"\n",
            alias, desc, udPtr->desc );
    }
    // The alias must be unique
    if ( m_aliasDict->find( alias ) )
    {
        fprintf( stderr, "Units alias \"%s\" already defined.\n", alias );
        abort();
    }
    // Add the alias to the hash table with its description as data.
    // This provides the link from an alias to the m_dataDict.
    m_aliasDict->insert( alias, desc );
    // also add it to the sorted Ascii list for ordered display purposes.
    m_aliasList->inSort( alias );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the SIUnitData record to the units dictionary.
 *  Called only by SIUnits::define();
 */

void SIUnits::defineData( const char *desc, const char *baseUnits, int reference,
            int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
            int b8, int b9, int b10, double factor )
{
    // Create a new SIUnitData.
    SIUnitData *udPtr = new SIUnitData( desc, baseUnits, reference,
        b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, factor );
    checkmem( __FILE__, __LINE__, udPtr, "SIUnitData udPtr", 1 );
    // It must be unique
    if ( m_dataDict->find( desc ) )
    {
        fprintf( stderr, "Units description \"%s\" already defined.\n", desc );
        abort();
    }
    // Add it to the hash table using the description as a key.
    m_dataDict->insert( desc, udPtr );
    m_definedUnits++;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Returns a pointer to the SI derived units names (if one exists)
 *  for the compiled UnitData.
 *      (1) if typeName==true, the generic type name (area, density. etc).
 *      (2) if typeName==false, the SI derived names (coulomb, etc)
 *          for the specified compiled SIUnits term
 *          (which == SIUnits_Src or SIUnits_Dst)
 *
 *      Example SI derived names for some compiled phrases include:
 *        Phrase        Type Name               Derived Name
 *          m2          area                    (m2)
 *          C           electric charge         coulomb (s A)
 *          lb/ft2      density
 *          volt        potential difference
 *
 *  \retval If reducable, returns a pointer to the static derived name.
 *  \retval If not reducable, returns NULL.
 */

const char *SIUnits::derivedName( int which , bool typeName )
{
    int term, i, j, cumexp[SIUnits_Exponents];

    // Initialize exponents of each SI base type.
    for ( j = 0;
          j < SIUnits_Exponents;
          j++ )
    {
        cumexp[j] = 0;
    }
    // Determine exponent of each base unit type for this units phrase.
    for ( term = 0;
          term < m_terms[which];
          term++ )
    {
        for ( j = 0;
              j < SIUnits_Exponents;
              j++ )
        {
            cumexp[j] +=
                ( m_expon[which][term] * m_udPtr[which][term]->baseExp[j] );
        }
    }
    // Check if this has a derived name.
    for ( i = 0;
          DerivedUnit[i].typeName != NULL;
          i++ )
    {
        for ( j = 1;
              j < SIUnits_Exponents;
              j++ )
        {
            if ( cumexp[j] != DerivedUnit[i].baseExp[j] )
            {
                break;
            }
        }
        if ( j == SIUnits_Exponents )
        {
            return( typeName
                    ? DerivedUnit[i].typeName
                    : DerivedUnit[i].derivedName );
        }
    }
    return( NULL );
}

//------------------------------------------------------------------------------
/*! \brief Dumps the long list to the specified stream.
 */

#undef SIUNITS_DUMP
#ifdef SIUNITS_DUMP

void SIUnits::dumpLongList( FILE *fptr )
{
    SIUnitData *uPtr;
    for ( char *alias = m_aliasList->first();
          alias != 0;
          alias = m_aliasList->next() )
    {
        uPtr = find( alias );
        fprintf( fptr, "%-32.32s %s\n", alias, uPtr->desc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dumps the short list to the specified stream.
 */

void SIUnits::dumpShortList( FILE *fptr )
{
    SIUnitData *uPtr;
    for ( char *alias = m_shortList->first();
          alias != 0;
          alias = m_shortList->next() )
    {
        uPtr = find( alias );
        fprintf( fptr, "%-28.28s  %-12.12s  %s\n",
            alias, uPtr->baseUnits, uPtr->desc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Dumps the table size info to the specified stream.
 */

void SIUnits::dumpTableSize( FILE *fptr )
{
    fprintf( fptr, "There are %d units stored in the %d hash table\n",
        m_definedUnits, m_dataDict->size() );
    fprintf( fptr, "There are %d aliases defined in the %d hash table.\n",
        m_aliasList->count(), m_aliasDict->size() );
    fprintf( fptr, "There are %d aliases defined in the short list and %d in the long list.\n",
        m_shortList->count(), m_aliasList->count() );
    return;
}

#endif

//------------------------------------------------------------------------------
/*! \brief Public function to test if two units of measure phrases are
 *  equivalent.  For example, "lbs/ft2" is equivalent to "pounds per ft per ft".
 *
 *  \retval TRUE if the two phrases are equivalent.
 *  \retval FALSE if the two phrases are not equivalent.
 */

bool SIUnits::equivalent( const char *units1, const char *units2 )
{
    // First, the conversion must succeed.
    double factor, offset;
    if ( ! conversionFactorOffset( units1, units2, &factor, &offset ) )
    {
        return( false );
    }
    // Second, the factor must approximate 1 and the offset approximate 0.
    if ( fabs( 1. - factor ) > 0.00001
      || fabs( offset ) > 0.00001 )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Public function to look up a units alias and return a pointer to its
 *  SIUnitData record.
 *
 *  \retval A pointer to the alias' SIUnitData record if the alias is found.
 *  \retval 0 if the alias is not found.
 */

SIUnitData *SIUnits::find( const char *alias )
{
    SIUnitData *udPtr = 0;
    char *desc;
    if ( ( desc  = m_aliasDict->find( alias ) ) != NULL
      && ( udPtr = m_dataDict->find(  desc  ) ) != NULL )
    {
        return( udPtr );
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief  Defines all the SIUnitData members and aliases (English version).
 *  Called only bu the SIUnits::SIUnits() constructor.
 *
 *  The "reference" data member indicates the name-symbol-conversion source.
 *  Most references the NIST Special Publication 811:
 *      Table 1. SI base units
 *      Table 3a. SI derived units with special names and symbols,
 *                including the radian and steradian.
 *      Table 3b. SI derived units with special names and symbols
 *                admitted for reasons of safeguarding human health.
 *      Table 6. Units accepted for use with the SI
 *      Table 7. Units accepted for use with the SI whose values in SI
 *               are obtained experimentally
 *      Table 9. Units temporarily accepted for use with the SI
 *      Table 10. Examples of CGS units with special names (not accepted
 *                  for use with the SI)
 *      Table 11. Examples of other unacceptable units
 *
 *  Reference to NASA SP-7012 are for additional units not covered
 *      by NIST SP 811.
 */

#define SI_REF_1        (0)
#define SI_REF_3A       (1)
#define SI_REF_3B       (2)
#define SI_REF_6        (3)
#define SI_REF_7        (4)
#define SI_REF_9        (5)
#define SI_REF_10       (6)
#define SI_REF_11       (7)
#define SI_REF_B        (8)
#define SI_REF_NASA     (9)
#define SI_REF_CDB      (10)

void SIUnits::defineUnits( void )
{
    define( "EMU of current [abampere]", "A", SI_REF_B,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    1.000000000000e+01,
        "abampere", "abA", "abamp", "aba", "abAmp", "abAmpere");

    define( "EMU of electric charge [abcoulomb]", "C", SI_REF_B,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    1.000000000000e+01,
        "abcoulomb", "abC", "abc", "abCoulomb" );

    define( "EMU of capacitance [abfarad]", "F", SI_REF_B,
        0,-2,-1, 4, 2, 0, 0, 0, 0, 0, 0,    1.000000000000e+09,
        "abfarad", "abF", "abf", "abFarad" );

    define( "EMU of inductance [abhenry]", "H", SI_REF_B,
        0, 2, 1,-2,-2, 0, 0, 0, 0, 0, 0,    1.000000000000e-09,
        "abhenry", "abH", "abh", "abHenry" );

    define( "EMU of conductance [abmho]", "S", SI_REF_B,
        0,-2,-1, 3, 2, 0, 0, 0, 0, 0, 0,    1.000000000000e+09,
        "abmho", "abM", "abm", "abMho", "absiemens", "abSiemens",
        "abS", "abs", "ab" );

    define( "EMU of resistance [abohm]", "ohm", SI_REF_B,
        0, 2, 1,-3,-2, 0, 0, 0, 0, 0, 0,    1.000000000000e-09,
        "abohm", "abO", "abo", "abOmega" );

    define( "EMU of electric potential [abvolt]", "V", SI_REF_B,
        0, 2, 1,-3,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e-08,
        "abvolt", "abV", "abv", "abVolt" );

    define( "standard acceleration of free fall [ga]", "m/s2", SI_REF_B,
        0, 1, 0,-2, 0, 0, 0, 0, 0, 0, 0,    9.806650000000e+00,
        "freefall", "ga", "aff" );

    define( "acre (based on international foot)", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.046856422400e+03,
        "acre", "ac" );

    define( "acre foot (based on international foot)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.233481837000e+03,
        "acrefoot", "acft", "acrefeet", "acreft", "acFt", "acreFt" );

    define( "acre (based on U.S. survey foot)", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.046873000000e+03,
        "acreSurvey", "acSurvey" );

    define( "acre foot (based on U.S. survey foot)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.233486890000e+03,
        "acrefootSurvey", "acrefeetSurvey", "acreftSurvey", "acftSurvey",
        "acreFootSurvey", "acFtSurvey" );

    define( "electric current [ampere]", "A", SI_REF_1,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "ampere", "A", "amp" );

    define( "ampere hour [A h]", "C", SI_REF_B,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    3.600000000000e+03,
        "ampereHour", "Ah", "ampH", "amph" );

    define( "angstrom", "m", SI_REF_9,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-10,
        "angstrom", "ang" );

    define( "are [a] (agrarian area)", "m2", SI_REF_9,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+02,
        "are", "a" );

    define( "astronomical unit [AU]", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.495979000000e+11,
        "astronomicalUnit", "AU", "au", "astro", "astronimical" );

    define( "standard atmosphere [atm]", "Pa", SI_REF_11,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.013250000000e+05,
        "standardAtmosphere", "atm", "atmosphere" );

    define( "technical atmosphere [at]", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    9.806650000000e+04,
        "technicalAtmosphere", "at" );

    define( "bar [bar]", "Pa", SI_REF_9,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+05,
        "bar" );

    define( "barn [b]", "m2", SI_REF_9,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-28,
        "barn", "b" );

    define( "barrel [bbl] (for petroleum, 42 U.S. gallons)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.589873000000e-01,
        "barrel", "bbl" );

    define( "barye", "N/m2", SI_REF_NASA,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-01,
        "barye" );

    define( "activity of a radionuclide [Bq]", "1/s", SI_REF_3B,
        0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "becquerel", "Bq", "Becquerel" );

    define( "biot [Bi]", "A", SI_REF_B,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "biot", "Bi", "bi", "Biot" );

    define( "board foot (1'x1'x1\")", "m3", SI_REF_NASA,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.359737216000e-03,
        "boardFoot", "bdft", "boardfeet", "bdf", "bdfeet", "bdfoot",
        "boardfoot", "boardFeet", "bdF", "bdFt", "bdFeet", "bdFoot" );

    define( "International Table British thermal unit (after 1956)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.055056000000e+03,
        "internationalTableBtu", "btuIT", "btuit", "itBtu", "itbtu" );

    define( "thermochemical British thermal unit", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.054350000000e+03,
        "thermochemicalBtu", "btuth", "btuTh", "thbtu", "thBtu" );

    define( "mean British thermal unit [Btu]", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.055870000000e+03,
        "meanBtu", "Btu", "btu", "meanbtu", "btuMean", "btumean" );

    define( "British thermal unit (39 F)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.059670000000e+03,
        "btu39F", "btu_39F", "btu_39f", "btu39f" );

    define( "British thermal unit (59 F)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.054800000000e+03,
        "btu59F", "btu_59F", "btu_59f", "btu59f" );

    define( "British thermal unit (60 F)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.054680000000e+03,
        "btu60F", "btu_60F", "btu_60f", "btu60f" );

    define( "bushel (U.S.) [bu]", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.523907016688e-02,
        "bushel", "bu", "Bu", "Bushel" );

    define( "cable", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.194560000000e+02,
        "cable", "Cable" );

    define( "caliber", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.540000000000e-04,
        "caliber", "Caliber" );

    define( "International Table calorie [calIT] (after 1956)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.186800000000e+00,
        "internationalTableCalorie", "calIT", "calit", "itCal", "itcal" );

    define( "thermochemical calorie [calth]", "J", SI_REF_11,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.184000000000e+00,
        "thermochemicalCalorie", "calTh", "calth", "thCal", "thcal" );

    define( "mean calorie", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.190020000000e+00,
        "calorie", "cal", "calMean", "calmean", "meancal", "meancalorie",
        "Calorie", "Cal" );

    define( "calorie (15 C)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.185800000000e+00,
        "cal15C", "cal_15C", "cal_15c", "cal15c" );

    define( "calorie (20 C)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.181900000000e+00,
        "cal20C", "cal_20C", "cal_20c", "cal20c" );

    define( "luminous intensity [candela]", "cd", SI_REF_1,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,    1.000000000000e+00,
        "candela", "cd", "Candela", "Cd" );

    define( "metric carat [metric carat]", "kg", SI_REF_11,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    2.000000000000e-04,
        "carat", "metricCarat", "metriccarat", "Carat" );

    // Must define centimeter so we also get "cm".
    define( "centimeter", "m", SI_REF_CDB,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-02,
        "centimeter", "cm", "Centimeter", "Cm" );

    define( "centimeter of mercury (0 oC)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.333220000000e+03,
        "cmHg0oC" );

    define( "centimeter of mercury, conventional", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.333224000000e+03,
        "cmHg" );

    define( "centimeter of water (4 oC)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    9.806380000000e+01,
        "cmH2O4oC", "cmWater4oC" );

    define( "centimeter of water, conventional", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    9.806650000000e+01,
        "cmH2O", "cmWater" );

    define( "centipoise [cP]", "Pa s", SI_REF_10,
        0,-1, 1,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-03,
        "centipoise", "cP" );

    define( "centistokes [cSt]", "m2/s", SI_REF_10,
        0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-06,
        "centistokes", "centistoke", "cSt" );

    define( "chain (engineer or Ramden)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.048000000000e+01,
        "chainEnginer", "chainRamden", "RamdenChain", "engineerChain" );

    define( "chain [ch] (based on U.S. survey foot, or Gunter)", "m", SI_REF_NASA,
        //0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.011684000000e+01,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.011680000000e+01,
        "chain", "ch", "Chain", "Ch", "Gunter", "surveyorChain",
        "chainSurveyor", "chainGunter" );

    define( "circular mil", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.067074800000e-10,
        "circularMil", "cmil", "circularmil" );

    define( "class or other cardinal entity", "class", SI_REF_CDB,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "class", "", "cardinal", "category", "clas", "index", "none",
        "ordinal", "rank" );

    define( "clo", "m2 K/W", SI_REF_B,
        0, 0, 1,-3, 0, 1, 0, 0, 0, 0, 0,    1.550000000000e-01,
        "clo", "Clo" );

    define( "cord (128 ft3)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.624556300000e+00,
        "cord", "Cord" );

    define( "electric charge or quantitiy of electricity [C]", "A s", SI_REF_3A,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "coulomb", "C", "coul", "Coulomb" );

    define( "cubit", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.572000000000e-01,
        "cubit", "Cubit" );

    define( "cup (U.S.)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.365882365000e-04,
        "cup", "cp", "Cup" );

    define( "curie [Ci]", "Bq", SI_REF_9,
        0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0,    3.700000000000e+10,
        "curie", "Ci", "Curie" );

    define( "permeability of porous solids (not area)", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.869233e-13,
        "darcy", "Darcy" );

    define( "day (86,400 s)", "s", SI_REF_6,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    8.640000000000e+04,
        "day", "d", "Day" );

    define( "day (sidereal)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    8.616409000000e+04,
        "daySidereal", "siderealDay" );

    define( "decimeter", "m", SI_REF_CDB,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-01,
        "decimeter", "dm", "Decimeter", "Dm" );

    define( "debye [D]", "C m", SI_REF_B,
        0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0,    3.335641000000e-30,
        "debye", "D", "Debye" );

    define( "degree [o] (plane angle)", "rad", SI_REF_6,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    1.745329251994e-02,
        "degree", "o", "deg", "degreeAngle", "angleDegree", "angleDeg",
        "Degree",      "Deg" );

    define( "Celsius temperature [oC]", "K", SI_REF_3A,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "oC", "celcius", "c", "cel", "celciu",
        "oc", "Celcius",      "Cel", "Celciu" );

    define( "degree Fahrenheit", "K", SI_REF_B,
        0, 0, 0, 0, 0,1, 0, 0, 0, 0, 0,     5.555555555556e-01,
        "oF", "f", "of", "fahrenheit", "Fahrenheit" );

    define( "denier", "kg/m", SI_REF_B,
        0,-1, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.111111111111e-07,
        "denier", "Denier" );

    define( "dram (avoirdupois)", "kg", SI_REF_NASA,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.7718451953125e-03,
        "dram", "avDram", "Dram" );

    define( "dram (troy or apothecary)", "kg", SI_REF_NASA,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    3.887934600000e-03,
        "apDram", "apothecaryDram", "troyDram", "TroyDram" );

    define( "dram (U.S. fluid)", "kg", SI_REF_NASA,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.6966911953125e-06,
        "fluidDram" );

    define( "dyne [dyn]", "N", SI_REF_10,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-05,
        "dyne", "dyn", "Dyne" );

    define( "electron volt [eV]", "J", SI_REF_7,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.602177220000e-19,
        "eV", "electronVolt", "electronV", "evolt", "eVolt" );

    define( "erg [erg]", "J", SI_REF_10,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-07,
        "erg", "Erg" );

    define( "capacitance [F]", "C/V", SI_REF_3A,
        0,-2,-1, 4, 2, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "farad", "F", "Farad" );

    define( "faraday (based on carbon 12)", "C", SI_REF_B,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    9.648531000000e+04,
        "faraday", "Faraday" );

    define( "faraday (chemical)", "C", SI_REF_NASA,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    9.649570000000e+04,
        "faradayChemical" );

    define( "faraday (physical)", "C", SI_REF_NASA,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    9.652190000000e+04,
        "faradayPhysical" );

    define( "fathom (based on international foot)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.828800000000e+00,
        "fathom", "fath", "Fathom" );

    define( "fathom (based on U.S. survey foot)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.828804000000e+00,
        "fathomUS", "fathUS" );

    define( "fermi [fermi]", "m", SI_REF_11,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-15,
        "fermi", "femtometer", "fm", "Fermi" );

    define( "foot [ft] (international)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.04800000000000e-01,
        "ft", "foot", "feet", "Ft", "Foot", "Feet" );

    define( "square foot [sqft] (international)", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.290304000000e-02,
        "sqft", "sqFoot", "sqFeet", "squareFoot", "squareFeet", "squareFt",
        "sqFt" );

    define( "cubic foot [cuft] (international)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.831684700000e-02,
        "cuft", "cuFoot", "cuFeet", "cubicFoot", "cubicFeet", "cubicFt",
        "cuFt" );

    define( "foot (U.S. survey)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.048006096000e-01,
        "footSurvey", "ftSurvey", "surveyFoot", "surveyFt" );

    define( "foot of mercury (conventional)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.063666000000e+04,
        "ftHg" );

    define( "foot of water (39.2 oF)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.988980000000e+03,
        "ftH2O39oF", "ftH2O39f" );

    define( "foot of water (conventional)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.989067000000e+03,
        "ftH2O" );

    define( "footcandle", "lx", SI_REF_B,
        0,-2, 0, 0, 0, 0, 1, 0, 0, 1, 0,    1.076391000000e+01,
        "footcandle", "ftcandle", "ftc", "ftCandle" );

    define( "footlambert", "cd/m2", SI_REF_B,
        0,-2, 0, 0, 0, 0, 1, 0, 0, 0, 0,    3.426259000000e+00,
        "footlambert", "footLambert", "ftlambert", "ftLambert", "ftl", "ftL" );

    define( "fortnight (14 days)", "s", SI_REF_CDB,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    1.209600000000e+06,
        "fortnight", "fn" );

    define( "franklin [Fr]", "C", SI_REF_B,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    3.335641000000e-10,
        "franklin", "Fr", "Franklin" );

    define( "furlong", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.011680000000e+02,
        "furlong", "fur", "Furlong" );

    define( "gal [Gal] (galileo)", "m/s2", SI_REF_9,
        0, 1, 0,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-02,
        "gal", "Gal", "galileo", "Galileo" );

    define( "gallon, Canadian and U.K. (Imperial)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.546087000000e-03,
        "gallonUK", "galUK", "galImp", "galImperial", "gallonImp",
        "gallonImperial" );

    define( "gallon (U.S. dry)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.404883770860e-03,
        "gallonDry", "galDry", "galUSDry" );

    define( "gallon (U.S. liquid)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.785411784000e-03,
        "gallon", "galUS", "Gallon" );

    define( "gamma (magnetic flux density)", "T", SI_REF_11,
        0, 0, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e-09,
        "gamma", "Gamma" );

    define( "gamma (mass)", "kg", SI_REF_11,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-09,
        "gammaMass", "gammaMas" );

    define( "gauss [Gs] [G]", "T", SI_REF_10,
        0, 0, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e-04,
        "gauss", "Gs", "G", "Gauss", "gaus", "Gaus" );

    define( "gilbert [Gi]", "A", SI_REF_B,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    7.957747e-01,
        "gilbert", "Gi", "Gilbert" );

    define( "gill, Canadian and U.K. (Imperial)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.420653000000e-04,
        "gillUK", "gillImperial", "gillImp", "giUK", "giImperial", "giImp" );

    define( "gill [gi] (U.S.)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.182941200000e-04,
        "gill", "gi", "gillUS", "giUS", "Gill" );

    define( "grade [gon]", "rad", SI_REF_B,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    1.570796300000e-02,
        "gon", "grade", "grad", "Gon", "Grad", "Grade" );

    define( "absorbed dose, specific energy (imparted), kerma [Gy]", "J/kg", SI_REF_3B,
        0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-02,
        "gray", "Gy", "Gray" );

    define( "grain [gr]", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    6.479891000000e-05,
        "grain", "gr", "Grain" );

    define( "gram", "kg", SI_REF_NASA,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-03,
        "gram", "g", "gm", "Gram" );

    define( "hand", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.016000000000e-01,
        "hand", "hd", "hnd" );

    define( "hectare [ha] (agrarian area)", "m2", SI_REF_9,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+04,
        "hectare", "ha", "hec", "Ha", "Hectare" );

    define( "inductance [H]", "Wb/A", SI_REF_3A,
        0, 2, 1,-2,-2, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "henry", "H", "Henry" );

    define( "frequency [Hz]", "1/s", SI_REF_3A,
        0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "hertz", "hz", "Hertz", "Hz" );

    define( "hogshead (U.S.)", "m3", SI_REF_NASA,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.384809423920e-01,
        "hogshead", "hogshd", "hh" );

    define( "horsepower (550 foot-lbf/s)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    7.456998700000e+02,
        "horsepower550", "hp550)" );

    define( "horsepower (boiler)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    9.809500000000e+03,
        "horsepowerBoiler", "hpBoiler" );

    define( "horsepower (electric)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    7.460000000000e+02,
        "horsepower", "hp", "horse", "hpElectric", "horsepowerElectric", "Hp" );

    define( "horsepower (metric)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    7.354988000000e+02,
        "horsepowerMetric", "hpMetric" );

    define( "horsepower (U.K.)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    7.457000000000e+02,
        "horsepowerUK", "hpUK" );

    define( "horsepower (water)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    7.460430000000e+02,
        "horsepowerWater", "hpWater" );

    define( "hour [h]", "s", SI_REF_6,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    3.600000000000e+03,
        "hour", "h", "hr", "Hour", "Hr" );

    define( "hour (sidereal)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    3.590170400000e+03,
        "hourSidereal", "hSidereal", "hrSidereal" );

    define( "hundredweight (long, 112 lb)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    5.080234544000e+01,
        "hundredweightLong" );

    define( "hundredweight (short, 100 lb)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    4.535923700000e+01,
        "hundredweightShort", "hwt", "Hwt" );

    define( "inch [in]", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.540000000000e-02,
        "inch", "in", "inche", "In", "Inch", "Inche" );

    define( "inch of mercury (32 oF)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    3.386389000000e+03,
        "inHg32oF", "inHg32f" );

    define( "inch of mercury (60 oF)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    3.376850000000e+03,
        "inHg60oF", "inHg60f" );

    define( "inch of mercury (conventional)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    3.386389000000e+03,
        "inHg" );

    define( "inch of water (39.2 oF)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.490820000000e+02,
        "inH2O39oF", "inWater39oF", "inH2O39f", "inWater39f" );

    define( "inch of water (60 oF)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.488400000000e+02,
        "inH2O60oF", "inWater60oF", "inH2O60f", "inWater60f" );

    define( "inch of water (conventional)", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.490889000000e+02,
        "inH2O", "inWater" );

    define( "energy, work, or quantity of heat [J]", "N m", SI_REF_3A,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "joule", "J", "Joule" );

    define( "energy, work, or quantity of heat [kJ]", "N m", SI_REF_CDB,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+03,
        "kJ", "kj" );

    define( "kayser", "1/m", SI_REF_NASA,
        0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+02,
        "kayser", "kay", "Kayser" );

    define( "thermodynamic temperature [Kelvin]", "K", SI_REF_1,
        0, 0, 0, 0, 0,1, 0, 0, 0, 0, 0,     1.000000000000e+00,
        "kelvin", "K", "oK", "k", "ok", "Kelvin" );

    define( "kilocalorie, International Table (after 1956)", "W", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.186800000000e+03,
        "kcalIT", "kcalit" );

    define( "kilocalorie (thermochemical)", "W", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.184000000000e+03,
        "kcalTh", "kcalth" );

    define( "kilocalorie (mean)", "W", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.190020000000e+03,
        "kilocalorie", "kcalorie", "kcal", "kilocal" );

    define( "mass [kilogram]", "kg", SI_REF_1,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "kilogram", "kg", "kgm", "kilog", "kilogm",
        "Kilogram", "Kg", "Kgm" );

    define( "kilogram-force [kgf]", "N", SI_REF_11,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    9.806650000000e+00,
        "kilogramforce", "kgf", "kilopond", "kp" );

    // Must define kilometer so we also get "km".
    define( "kilometer", "m", SI_REF_CDB,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+03,
        "kilometer", "km", "Kilometer", "Km" );

    define( "kip (1 kip = 1000 lbf)", "N", SI_REF_B,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.448221615260e+03,
        "kip", "Kip" );

    define( "knot", "m/s", SI_REF_9,
        0, 1, 0,-1, 0, 0, 0, 0, 0, 0, 0,    5.144444444444e-01,
        "knot", "kn", "Knot" );

    define( "lambda (volume)", "m3", SI_REF_11,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-09,
        "lambda", "lambdaVolume" );

    define( "lambert", "cd/m2", SI_REF_B,
        0,-2, 0, 0, 0, 0,1, 0, 0, 0, 0,     3.183098860000e+03,
        "lambert", "lam", "Lambert" );

    define( "langley", "J/m2", SI_REF_B,
        0, 0, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.184000000000e+04,
        "langley", "lan", "lang", "Langley" );

    define( "league (international nautical)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.556000000000e+03,
        "league", "lg", "League" );

    define( "league (statute)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.828032000000e+03,
        "leagueStatute" );

    define( "league (U.K nautical)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.559552000000e+03,
        "leagueUK" );

    define( "light year", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.460730000000e+15,
        "lightYear", "ly", "lightyear", "LY" );

    define( "link (engineer or ramden)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.048000000000e-01,
        "linkEngineer", "linkRamdan" );

    define( "link (surveyor or gunter)", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.011680000000e-01,
        "link", "lnk", "linkSurveyor", "linkGunter", "Link" );

    define( "liter", "m3", SI_REF_6,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-03,
        "liter", "l", "L", "Liter" );

    define( "luminous flux [lm]", "cd sr", SI_REF_3A,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,    1.000000000000e+00,
        "lumen", "lm", "lum", "Lumen" );

    define( "illuminance [lx]", "lm/m2", SI_REF_3A,
        0,-2, 0, 0, 0, 0, 1, 0, 0, 1, 0,    1.000000000000e+00,
        "lux", "lx", "Lux" );

    define( "maxwell [Mx]", "Wb", SI_REF_10,
        0, 2, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e-08,
        "maxwell", "Mx", "mx", "Maxwell" );

    define( "distance or length [meter]", "m", SI_REF_1,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "meter", "m", "Meter" );

    define( "area [square meters]", "m2", SI_REF_1,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "sqmeter", "sqm", "sqMeter", "sqM" );

    define( "volume [cubic meters]", "m3", SI_REF_1,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "cumeter", "cum", "cuMeter", "cuM" );

    define( "milligram [mg]", "kg", SI_REF_CDB,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-06,
        "milligram", "mg" );

    define( "microinch", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.540000000000e-08,
        "microinch", "microin" );

    define( "micron [mu]", "m", SI_REF_11,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-06,
        "micron", "mu" );

    define( "mil (0.001 in)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.540000000000e-05,
        "mil" );

    define( "mil (plane angle)", "rad", SI_REF_B,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    9.817477e-04,
        "milAngle" );

    define( "nautical mile", "m", SI_REF_9,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.852000000000e+03,
        "nauticalMile", "nmi" );

    define( "mile (U.K. nautical)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.853184000000e+03,
        "nauticalMileUK" );

    define( "mile (U.S. statute)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.609344000000e+03,
        "mile", "mi", "Mile", "Mi", "mileStatute", "miStatute" );

    define( "square mile (U.S. statute)", "m2", SI_REF_B,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.589988110340e+06,
        "sqmile", "sqmi", "sqMile", "sqMi" );

    define( "cubic mile (U.S. stature)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.168181825440e+09,
        "cumile", "cumi", "cuMile", "cuMi" );

    define( "mile (based U.S. survey foot)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.609347825440e+03,
        "miSurvey", "mileSurvey" );

    define( "mile per gallon [mpg] (U.S.)", "m/m3", SI_REF_B,
        0, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0,   4.251435e+05,
        "mpg", "MPG" );

    define( "mile per hour [mph]", "m/s", SI_REF_B,
        0, 1, 0,-1, 0, 0, 0, 0, 0, 0, 0,    4.4704e-01,
        "mph", "MPH" );

    define( "millibar [mbar]", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+02,
        "millibar", "mbar" );

    define( "millimeter of mercury", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.333224000000e+02,
        "mmHg" );

    define( "millimeter of water", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    9.806650000000e+00,
        "mmH2O" );

    define( "minute", "s", SI_REF_6,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    6.000000000000e+01,
        "minute", "min", "Min", "Minute" );

    define( "minute (plane angle)", "rad", SI_REF_6,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    2.908882086660e-04,
        "'", "minuteAngle", "minAngle", "degreeMinute", "degreeMin" );

    define( "minute (sidereal)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    5.983617400000e+01,
        "minuteSidereal" );

    // Must define millimeter so we also get "mm".
    define( "millimeter", "m", SI_REF_CDB,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-03,
        "millimeter", "mm", "Millimeter", "MM" );

    define( "amount of substance [mole]", "mol", SI_REF_1,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,     1.000000000000e+00,
        "mole", "mol" );

    // Based on a Julian century of 36,525 days (30.4375 days/month)
    // and 86,400 seconds per day
    define( "month (mean calendar, 36,525 day Julian century)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    2.629800000000e+06,
        "month", "Month" );

    define( "force [N]", "m kg/s", SI_REF_3A,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "newton", "N", "Newton" );

    define( "oersted [Oe]", "A/m", SI_REF_10,
        0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 0,    7.957747200000e+01,
        "oersted", "Oe", "Oerstad" );

    define( "electric resistance [omega]", "V/A", SI_REF_3A,
        0, 2, 1,-3,-2, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "ohm", "omega", "Ohm" );

    define( "ounce mass [oz] (avoirdupois)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    2.834952312500e-02,
        "oz", "ounce", "ozMass", "ozMas", "ozAv", "ounceAv", "Ounce", "Oz" );

    define( "ounce mass (troy or apothecary)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    3.110347680000e-02,
        "ounceTroy", "ozTroy", "troyOz", "troyOunce" );

    define( "ounce (Canadian and U.K. fluid, Imperial)", "kg", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.841306000000e-05,
        "flozUK", "flOzUK", "flozImp", "flOzImp" );

    define( "ounce [fl oz] (U.S. fluid)", "kg", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.957352956250e-05,
        "floz", "flOz" );

    define( "ounce-force (avoirdupois)", "N", SI_REF_NASA,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    2.780138500000e-01,
        "ounceforce", "ounceForce", "ozf" );

    define( "pace", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    7.620000000000e-01,
        "pace", "Pace" );

    define( "parsec", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.085678000000e+16,
        "parsec", "pc", "Parsec" );

    define( "pressure or stress [Pa]", "N/m2", SI_REF_3A,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "pascal", "Pa", "pa", "Pascal" );

    define( "peck (U.S.)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    8.809767541720e-03,
        "peck", "pk", "Peck" );

    define( "pennyweight", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.555173840000e-03,
        "pennyweight", "dwt", "Dwt" );

    define( "percent", "dl", SI_REF_CDB,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,    1.000000000000e-02,
        "percent", "%", "pct" );

    define( "perch", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.029200000000e+00,
        "perch", "Perch" );

    define( "perm (0 oC)", "kg/(Pa s m2)", SI_REF_B,
        0,-1, 1, 1, 0, 0, 0, 0, 0, 0, 0,    5.72135e-11,
        "perm", "Perm" );

    define( "phot [ph]", "lx", SI_REF_10,
        0,-2, 0, 0, 0, 0, 1, 0, 0, 1, 0,    1.000000000000e+04,
        "phot", "ph", "Phot" );

    define( "pica (computer, 1/6 in)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.233333333333e-03,
        "computerPica" );

    define( "pica (printers)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.217517600000e-03,
        "pica", "printersPica", "Pica" );

    define( "pint (U.S. dry)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.506104713575e-04,
        "dryPint", "dryPt", "drypt" );

    define( "pint (U.S. liquid)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.731764730000e-04,
        "pt", "pint", "liquidPint", "liquidPt", "liqpt", "liqPt", "Pint" );

    define( "point (computer, 1/72 in)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.527777777778e-04,
        "computerPoint" );

    define( "point (printers)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3.514598000000e-04,
        "point", "printersPoint" );

    define( "poise [P]", "Pa s", SI_REF_10,
        0,-1, 1,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-01,
        "poise", "P" );

    define( "pole", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.029200000000e+00,
        "pole", "Pole" );

    define( "pound-force", "N", SI_REF_B,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.448221615260e+00,
        "lbf", "poundforce", "poundForce" );

    define( "pound mass (avoirdupois)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    4.535923700000e-01,
        "lb", "pound", "lbm", "poundAv", "lbAv", "Lb", "Pound" );

    define( "pound mass (troy or apothecary)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    3.732417216000e-01,
        "lbTroy", "poundTroy", "troyLb", "troyPound" );

    define( "poundal", "N", SI_REF_B,
        0, 1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.382549543760e-01,
        "poundal", "Poundal" );

    define( "pound-force per square inch", "Pa", SI_REF_B,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    6.894757200000e+03,
        "psi" );

    define( "quad (10^15 Btu IT)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.055056e+18,
        "quad", "Quad" );

    define( "quart (U.S. dry)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.101220942715e-03,
        "dryQt", "qtDry" );

    define( "quart (U.S. liquid)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.463529500000e-04,
        "qt", "quart", "liquidQt", "liquidquart", "Qt", "Quart" );

    define( "rad (absorbed dose)", "Gy", SI_REF_9,
        0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-02,
        "rd", "radDose" );

    define( "plane angle [rad]", "m/m", SI_REF_3A,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    1.000000000000e+00,
        "radian", "rad" );

    define( "Rankine temperature", "K", SI_REF_NASA,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,    5.555555555555e-01,
        "rankine", "Rankine" );

    define( "ratio", "dl", SI_REF_CDB,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,    1.000000000000e+00,
        "ratio", ".", "dl", "fraction", "frac" );

    define( "rayleigh (rate of photon emission)", "1/(m2 s)", SI_REF_NASA,
        0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+10,
        "rayleigh", "Rayleigh" );

    define( "rem", "Sv", SI_REF_9,
        0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-02,
        "rem", "Rem" );

    define( "revolution [r]", "rad", SI_REF_B,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    6.2831853e+00,
        "revolution", "r" );

    define( "revolution per minute [rpm]", "rad/s", SI_REF_B,
        0, 0, 0,-1, 0, 0, 0, 0, 1, 0, 0,    1.047198e+00,
        "rpm", "RPM" );

    define( "reciprocal Pascal second [rhe]", "1/(Pa s)", SI_REF_B,
        0, 1,-1, 1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+01,
        "rhe", "Rhe" );

    define( "rod (based on U.S. survey foot)", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    5.029210000000e+00,
        "rod", "Rod" );

    define( "roentgen [R]", "C/kg", SI_REF_9,
        0, 0,-1, 1, 1, 0, 0, 0, 0, 0, 0,    2.579760000000e-04,
        "roentgen", "R", "Roentgen" );

    define( "scruple (apothecary)", "kg", SI_REF_NASA,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.295978200000e-03,
        "scruple", "sc", "Scruple" );

    define( "time [second]", "s", SI_REF_1,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "s", "second", "sec", "Second", "Sec" );

    define( "second (plane angle)", "rad", SI_REF_6,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    4.848136811000e-06,
        "\"", "secondAngle", "secAngle", "angleSecond", "angleSec" );

    define( "second (sidereal)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    9.972695700000e-01,
        "sSidereal", "secSidereal", "secondSidereal" );

    define( "section (U.S. surveyor)", "m2", SI_REF_NASA,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.589988110336e+06,
        "section", "sect", "Section" );

    define( "shake", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-08,
        "shake", "sh", "Shake" );

    define( "electric conductance [S]", "A/V", SI_REF_3A,
        0,-2,-1, 3, 2, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "siemens", "S", "siemen", "mho", "Siemens", "Siemen", "Mho" );

    define( "ambient dose equivalent [Sv]", "J/kg", SI_REF_3B,
        0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "sievert", "Sv", "Sievert"  );

    define( "skein", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.097280000000e+02,
        "skein", "sk", "Skein" );

    define( "slug", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.459390290000e+01,
        "slug", "sl", "Slug" );

    define( "span", "m", SI_REF_NASA,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.286000000000e-01,
        "span", "sp", "Span" );

    define( "ESU of current [statampere]", "A", SI_REF_B,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    3.335641000000e-10,
        "statampere", "statamp", "stata", "statAmpere", "statAmp", "statA" );

    define( "ESU of electric charge [statcoulomb]", "C", SI_REF_B,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,    3.335641000000e-10,
        "statcoulomb", "statCoulomb", "statC", "statc" );

    define( "ESU of capacitance [statfarad]", "F", SI_REF_B,
        0,-2,-1, 4, 2, 0, 0, 0, 0, 0, 0,    1.112650000000e-12,
        "statfarad", "statFarad", "statF", "statf" );

    define( "ESU of inductance [stathenry]", "H", SI_REF_B,
        0, 2, 1,-2,-2, 0, 0, 0, 0, 0, 0,    8.987552000000e+11,
        "stathenry", "statHenry", "statH", "stath" );

    define( "ESU of conductance [statmho]", "S", SI_REF_B,
        0,-2,-1, 3, 2, 0, 0, 0, 0, 0, 0,    1.112650000000e-12,
        "statmho", "statMho", "statM", "statm", "statS", "statSiemen",
        "statsiemen", "stats", "stat" );

    define( "ESU of resistance [statohm]", "ohm", SI_REF_B,
        0, 2, 1,-3,-2, 0, 0, 0, 0, 0, 0,    8.987552000000e+11,
        "statohm", "statOhm", "stato", "statO" );

    define( "ESU of electric potential [statvolt]", "V", SI_REF_B,
        0, 2, 1,-3,-1, 0, 0, 0, 0, 0, 0,    2.997925000000e+02,
        "statvolt", "statVolt", "statV", "statv" );

    define( "solid angle [sr]", "m2/m2", SI_REF_3A,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,    1.000000000000e+00,
        "steradian", "sr", "Steradian" );

    define( "stere [st]", "m3", SI_REF_11,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "stere", "st", "Stere" );

    define( "stilb [sb]", "cd/m2", SI_REF_10,
        0,-2, 0, 0, 0, 0,1, 0, 0, 0, 0,     1.000000000000e+04,
        "stilb", "sb", "Stilb" );

    define( "stoke [St]", "m2/s", SI_REF_10,
        0, 2, 0,-1, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-04,
        "stokes", "stoke", "St", "Stokes", "Stoke" );

    define( "tablespoon", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.478676478125e-05,
        "tablepoon", "tb", "Tb" );

    define( "teaspoon", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    4.928921593750e-06,
        "teaspoon", "tsp", "Tsp" );

    define( "magnetic flux density [T]", "Wb/m2", SI_REF_3A,
        0, 0, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "tesla", "T", "Tesla" );

    define( "tex", "kg/m", SI_REF_B,
        0,-1, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e-06,
        "tex", "Text" );

    define( "therm (EC)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.055060000000e+08,
        "thermEC" );

    define( "therm (U.S.)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.054804000000e+08,
        "thermUS" );

    define( "ton (assay)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    2.916666666667e-02,
        "tonAssay", "assayTon", "AT", "aTon" );

    define( "ton (long, 2240 lb)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.016046908800e+03,
        "tonLong", "longTon", "lTon" );

    define( "ton (metric)", "kg", SI_REF_6,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+03,
        "t", "tonne", "tonMetric", "metricTon", "mTon", "Tonne" );

    define( "ton (register)", "m3", SI_REF_B,
        0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,    2.831684659200e+00,
        "tonRegister", "registerTon", "rTon" );

    define( "ton (refrigeration, 12,000 BTU IT/h)", "W", SI_REF_B,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    3.516853659200e+03,
        "tonRefrigeration", "refrigerationTon", "referTon" );

    define( "ton (short, 2000 lb)", "kg", SI_REF_B,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    9.071847400000e+02,
        "ton", "tonShort", "shortTon", "sTon", "Ton" );

    define( "ton (TNT)", "J", SI_REF_B,
        0, 2, 1,-2, 0, 0, 0, 0, 0, 0, 0,    4.184000000000e+09,
        "tonTnt", "tntTon" );

    define( "torr", "Pa", SI_REF_11,
        0,-1, 1,-2, 0, 0, 0, 0, 0, 0, 0,    1.333224000000e+02,
        "torr", "Torr" );

    define( "township (U.S. surveyor)", "m2", SI_REF_NASA,
        0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.323957200000e+07,
        "township", "tnship", "twn" );

    define( "unit pole", "Wb", SI_REF_B,
        0, 2, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.256637000000e-07,
        "unitpole", "up", "unitPole" );

    define( "unified atomic mass unit", "kg", SI_REF_7,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,    1.6605402e-27,
        "uamu", "atomicMass", "atomicMas", "atomicmass", "atomicmas" );

    define( "electric potential, potential difference, electromotive force [V]",
        "W/A", SI_REF_3A,
        0, 2, 1,-3,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "volt", "V", "v", "Volt" );

    define( "power or radiant flux [W]", "J/s", SI_REF_3A,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "watt", "W", "w", "Watt" );

    define( "power or radiant flux [kW]", "J/s", SI_REF_CDB,
        0, 2, 1,-3, 0, 0, 0, 0, 0, 0, 0,    1.000000000000e+03,
        "kW", "kilowatt", "kw", "kiloWatt" );

    define( "magnetic flux [Wb]", "V s", SI_REF_3A,
        0, 2, 1,-2,-1, 0, 0, 0, 0, 0, 0,    1.000000000000e+00,
        "weber", "Wb", "wb", "Weber" );

    define( "x unit", "m", SI_REF_11,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    1.002e-13,
        "xu", "xUnit", "Xunit" );

    define( "yard [yd]", "m", SI_REF_B,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,    9.144000000000e-01,
        "yard", "yd", "Yard" );

    define( "year (calendar, 365 days)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    3.153600000000e+07,
        "year", "yr", "calendarYear", "Year", "Yr" );

    define( "year (sidereal)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    3.155815000000e+07,
        "siderealYear", "siderealYr" );

    define( "year (tropical)", "s", SI_REF_B,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,    3.155692597470e+07,
        "tropicalYear", "tropicalYr" );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Public access to the current error message with HTML markup.
 */

QString SIUnits::htmlMsg( void ) const
{
    return( m_htmlMsg );
}

//------------------------------------------------------------------------------
/*! \brief Public access to the current error message without HTML markup.
 */

QString SIUnits::nameMsg( void ) const
{
    return( m_nameMsg );
}

//------------------------------------------------------------------------------
//  End of siunits.cpp
//------------------------------------------------------------------------------

