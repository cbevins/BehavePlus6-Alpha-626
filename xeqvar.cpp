//------------------------------------------------------------------------------
/*! \file xeqvar.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree EqFun and EqVar class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "parser.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qdatetime.h>

// Standard include files
#include <math.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/*! \brief EqFun constructor.
 */

EqFun::EqFun( const QString &name, PFV func, int inputs, int outputs,
        int releaseFrom, int releaseThru, const QString &module ) :
    m_name(name),
    m_input(0),
    m_inputs(inputs),
    m_output(0),
    m_outputs(outputs),
    m_dirty(0),
    m_active(true),
    m_function(func),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru),
    m_module(module)
{
    // Create an array to hold pointers to all input EqVar's
    if ( m_inputs )
    {
        m_input = new EqVar *[ m_inputs ];
        checkmem( __FILE__, __LINE__, m_input, "EqVar *m_input", m_inputs );
    }
    // Create an array to hold pointers to all output EqVar's
    if ( m_outputs )
    {
        m_output = new EqVar *[ m_outputs ];
        checkmem( __FILE__, __LINE__, m_output, "EqVar *m_output", m_outputs );
    }
    // Create an array to hold dirty flags for all input EqVars's
    if ( m_inputs )
    {
        m_dirty = new bool[ m_inputs ];
        checkmem( __FILE__, __LINE__, m_dirty, "bool *m_dirty", m_inputs );
    }
    // Initialize all dirty flags to TRUE.
    int id;
    for ( id = 0;
          id < m_inputs;
          id++ )
    {
        m_dirty[id] = true;
        m_input[id] = 0;
    }
    for ( id = 0;
          id < m_outputs;
          id++ )
    {
        m_output[id] = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqFun destructor.
 */

EqFun::~EqFun( void )
{
    delete[] m_input;     m_input = 0;
    delete[] m_output;    m_output = 0;
    delete[] m_dirty;     m_dirty = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines whether or not the EqFun is part of the specified release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the EqFun is part of the specified release.
 */

bool EqFun::isCurrent( int release ) const
{
    return( release >= m_releaseFrom && release <= m_releaseThru );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqFun's input dirty flag for the specified EqVar.
 *  Uses brute force to locate the EqVar pointer in its input vardex[] list.
 *  Called only by EqVar::propagateDirty()
 *
 * \return EqFun's dirty flag's previous state.
 */

bool EqFun::setDirty( EqVar *varPtr )
{
    // Work through the EqFun's m_input[] array to find this EqVar
    for ( int id = 0;
          id < m_inputs;
          id++ )
    {
        if ( varPtr == m_input[id] )
        {
            bool previousState = m_dirty[id];
            m_dirty[id] = true;
            return( previousState );
        }
    }
    // This code block should never be executed!
    QString text("");
    translate( text, "EqFun:NotAnInput", m_name, varPtr->m_name );
    bomb( text );
    return( false );    // Just to keep the compiler from complaining.
}

//------------------------------------------------------------------------------
/*! \brief Sets all the EqFun's input dirty flags.
 */

void EqFun::setDirtyAll( void )
{
        int id;
    for ( id = 0;
          id < m_inputs;
          id++ )
    {
        m_dirty[id] = true;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqVar continuous variable constructor
 */

EqVar::EqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder, const QString &wizard,
        const QString &nativeUnits, int nativeDecimals,
        const QString &englishUnits, int englishDecimals,
        const QString &metricUnits, int metricDecimals,
        double nativeMinimum, double nativeMaximum, double defaultValue,
        int releaseFrom, int releaseThru ) :
    m_name(name),
    m_help(help),
    m_inpOrder(inpOrder),
    m_outOrder(outOrder),
    m_wizard(wizard),
    m_master(""),
    m_label(0),
    m_desc(0),
    m_hdr0(0),
    m_hdr1(0),
    m_varType(VarType_Continuous),
    m_consumer(0),
    m_consumers(0),
    m_producer(0),
    m_producers(0),
    m_tokens(0),
    m_store(""),
    m_isUserOutput(false),
    m_isUserInput(false),
    m_isConstant(false),
    m_isDiagram(false),
    m_isMasked(false),
    m_isWrap(false),
    m_isShaded(false),
    m_boundaries(0),
    m_boundary(0),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru),
    m_defaultValue(defaultValue),
    m_nativeMinimum(nativeMinimum),
    m_nativeMaximum(nativeMaximum),
    m_nativeValue(defaultValue),
    m_displayMinimum(nativeMinimum),
    m_displayMaximum(nativeMaximum),
    m_displayValue(defaultValue),
    m_nativeDecimals(nativeDecimals),
    m_englishDecimals(englishDecimals),
    m_metricDecimals(metricDecimals),
    m_displayDecimals(nativeDecimals),
    m_nativeUnits(nativeUnits),
    m_englishUnits(englishUnits),
    m_metricUnits(metricUnits),
    m_displayUnits(nativeUnits),
    m_factor(1.0),
    m_offset(0),
    m_convert(false),
    m_itemList(0),
    m_activeItemName("")
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqVar discrete variable constructor
 */

EqVar::EqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder,
        const QString &wizard, EqVarItemList *itemListPtr,
        int releaseFrom, int releaseThru ) :
    m_name(name),
    m_help(help),
    m_inpOrder(inpOrder),
    m_outOrder(outOrder),
    m_wizard(wizard),
    m_master(""),
    m_label(0),
    m_desc(0),
    m_hdr0(0),
    m_hdr1(0),
    m_varType(VarType_Discrete),
    m_eqTree(0),
    m_consumer(0),
    m_consumers(0),
    m_producer(0),
    m_producers(0),
    m_tokens(0),
    m_store(""),
    m_isUserOutput(false),
    m_isUserInput(false),
    m_isConstant(false),
    m_isDiagram(false),
    m_isMasked(false),
    m_isWrap(false),
    m_isShaded(false),
    m_boundaries(0),
    m_boundary(0),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru),
    m_defaultValue(0.),
    m_nativeMinimum(0.),
    m_nativeMaximum(0.),
    m_nativeValue(0.),
    m_displayMinimum(0.),
    m_displayMaximum(0.),
    m_displayValue(0.),
    m_nativeDecimals(0),
    m_englishDecimals(0),
    m_metricDecimals(0),
    m_displayDecimals(0),
    m_nativeUnits(""),
    m_englishUnits(""),
    m_metricUnits(""),
    m_displayUnits(""),
    m_factor(1.0),
    m_offset(0),
    m_convert(false),
    m_itemList(itemListPtr),
    m_activeItemName("")
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqVar text variable constructor
 */

EqVar::EqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder,
        const QString &wizard,
        int releaseFrom, int releaseThru ) :
    m_name(name),
    m_help(help),
    m_inpOrder(inpOrder),
    m_outOrder(outOrder),
    m_wizard(wizard),
    m_master(""),
    m_label(0),
    m_desc(0),
    m_hdr0(0),
    m_hdr1(0),
    m_varType(VarType_Text),
    m_eqTree(0),
    m_consumer(0),
    m_consumers(0),
    m_producer(0),
    m_producers(0),
    m_tokens(0),
    m_store(""),
    m_isUserOutput(false),
    m_isUserInput(false),
    m_isConstant(false),
    m_isDiagram(false),
    m_isMasked(false),
    m_isWrap(false),
    m_isShaded(false),
    m_boundaries(0),
    m_boundary(0),
    m_releaseFrom(releaseFrom),
    m_releaseThru(releaseThru),
    m_defaultValue(0.),
    m_nativeMinimum(0.),
    m_nativeMaximum(0.),
    m_nativeValue(0.),
    m_displayMinimum(0.),
    m_displayMaximum(0.),
    m_displayValue(0.),
    m_nativeDecimals(0),
    m_englishDecimals(0),
    m_metricDecimals(0),
    m_displayDecimals(0),
    m_nativeUnits(""),
    m_englishUnits(""),
    m_metricUnits(""),
    m_displayUnits(""),
    m_factor(1.0),
    m_offset(0),
    m_convert(false),
    m_itemList(0),
    m_activeItemName("")
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqVar destructor.
 */

EqVar::~EqVar( void )
{
    delete[] m_consumer;    m_consumer = 0;
    delete[] m_producer;    m_producer = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Finds the discrete EqVar's current active item
 *  and returns its data index.
 *
 *  \retval Active item's data index if an active item is found.
 *  \retval -1 if the active Item is not found.
 */

int EqVar::activeItemDataIndex( void ) const
{
    if ( m_itemList )
    {
        EqVarItem *itemPtr = m_itemList->itemWithName( m_activeItemName, false );
        if ( itemPtr )
        {
            return( itemPtr->m_index );
        }
    }
    return( -1 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the discrete EqVar's current active item
 *  and returns a pointer to its translated description string.
 *
 *  \retval Active item's data index if an active item is found.
 *  \retval 0 if the active Item is not found.
 */

QString *EqVar::activeItemDesc( void ) const
{
    if ( m_itemList )
    {
        EqVarItem *itemPtr = m_itemList->itemWithName( m_activeItemName, false );
        if ( itemPtr )
        {
            return( itemPtr->m_desc );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the discrete EqVar's current active item
 *  and returns its name.
 *
 *  \retval Active item's name if an active item is found.
 *  \retval Empty string if the active Item is not found.
 */

const QString EqVar::activeItemName( void ) const
{
    return( m_activeItemName );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first active producer function in the EqVar's
 *  m_producer[] array.
 *
 *  \retval Pointer to the FIRST active producer function.
 *  \retval 0 if there is no active producer function.
 */

EqFun *EqVar::activeProducerFunPtr( void ) const
{
    for ( int fid = 0;
          fid < m_producers;
          fid++ )
    {
        if ( m_producer[fid]->m_active )
        {
            return( m_producer[fid] );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Resets the EqVar's display units and decimals to their default
 *  English units values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool EqVar::applyEnglishUnits( void )
{
    return( setDisplayUnits( m_englishUnits, m_englishDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Resets the EqVar's display units and decimals to its default
 *  metric values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool EqVar::applyMetricUnits( void )
{
    return( setDisplayUnits( m_metricUnits, m_metricDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Resets the EqVar's display units and decimals to its default
 *  native values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool EqVar::applyNativeUnits( void )
{
    return( setDisplayUnits( m_nativeUnits, m_nativeDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Converts the EqVar m_store contents into #m_nativeUnits
 *  with full decimal precision ( if \a toNative is TRUE )
 *  or into the current #m_displayUnits with #m_displayDecimals
 *  if \a toNative is FALSE.
 *
 *  This function is called twice (and only) by EqVar::setDisplayUnits().
 *
 *  The first call, with toNative==TRUE, converts the store from its current
 *  #m_displayUnits into #m_nativeUnits with 6 decimal place precision.
 *
 *  EqVar::setDisplayUnits() then resets #m_displayUnits and #m_displayDecimals.
 *
 *  The second call, with toNative==FALSE, converts the store into the new
 *  #m_displayUnits with #m_displayDecimals.
 *
 *  \return TRUE on success, FALSE on error.
 */

bool EqVar::convertStoreUnits( bool toNative )
{
    // If converting to nativeUnits, derive the offset and factor
    int decimals;
    double factor, offset;
    if ( toNative )
    {
        if ( ! appSiUnits()->conversionFactorOffset(
                    m_displayUnits.latin1(), m_nativeUnits.latin1(),
                    &factor, &offset ) )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "EqVar:BadUnits", appSiUnits()->htmlMsg() );
            bomb( text );
        }
        decimals = 18;
    }
    // Otherwise we use current factor, offset, and decimals
    else
    {
        factor   = m_factor;
        offset   = m_offset;
        decimals = m_displayDecimals;
		decimals = 6;
    }
    // Convert each token in the entry field
    double  d0, d1;
    QString newStore = "";
    Parser parser( " \t,\"", "", "" );
    parser.parse( m_store );
    QString token;
    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        // If an invalid token is found, it ends up being 0.
        d0 = token.toDouble();
        d1 = offset + factor * d0;

        // Now format it and add it to the entry string
        QString fmt;
		// CDB DECIMALS MOD
        fmt.sprintf( "%1.*f", decimals, d1 );
        //fmt.sprintf( "%1.6f", d1 );
		while ( fmt.endsWith( "0" ) )
		{
			fmt = fmt.left( fmt.length()-1 );
		}
        newStore += fmt + ' ';
    }
    // Set the new m_store and return.
    m_store = newStore;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns the display units, which are the current display units
 *  text OR  an empty string if native units are "ratio" or blank.
 *
 *  \param enclosed If TRUE and the units string is not empty,
 *                  it is enclosed in parenthesis.
 *
 *  \retval Empty string if the eqVar has no units or "ratio" units.
 *  \retval EqVar's units if \a enclosed == FALSE.
 *  \retval EqVar's units enclosed in parenthesis if \a enclosed == TREU.
 */

QString EqVar::displayUnits( bool enclosed ) const
{
    QString str("");
    if ( m_nativeUnits != "ratio"
      && m_nativeUnits != "" )
    {
        if ( enclosed )
        {
            str = "(" + m_displayUnits + ")";
        }
        else
        {
            str = m_displayUnits;
        }
    }
    return( str );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem's name string
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's name string.
 *  \retval On failure returns an empty QString.
 */

const QString &EqVar::getItemName( int id ) const
{
    return( m_itemList->itemName( id ) );
}

//------------------------------------------------------------------------------
/*! \brief Allocates the EqVar's m_consumer[] and m_producer[] arrays.
 *
 *  The EqVar's m_consumers and m_producers must have already been set.
 *  In our case this is handled by EqTreeParser::startElement()
 *  when the <function> "input=" and "output=" attributes are process.
 */

void EqVar::init( void )
{
    // Create an array to hold pointers to this EqVar's consumers
    if ( m_consumers )
    {
        m_consumer = new EqFun *[ m_consumers ];
        checkmem( __FILE__, __LINE__, m_consumer, "EqFun *m_consumers",
            m_consumers );
    }
    // Create an array to hold pointers to this EqVar's producers
    if ( m_producers )
    {
        m_producer = new EqFun *[ m_producers ];
        checkmem( __FILE__, __LINE__, m_producer, "EqFun *m_producers",
            m_producers );
    }
    // Initialize the pointers to 0.
    int id;
    for ( id = 0;
          id < m_consumers;
          id++ )
    {
        m_consumer[id] = 0;
    }
    for ( id = 0;
          id < m_producers;
          id++ )
    {
        m_producer[id] = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines if EqVar is a continuous variable.
 *
 *  \return TRUE if a continuous variable, FALSE if not.
 */

bool EqVar::isContinuous( void ) const
{
    return( m_varType == VarType_Continuous );
}

//------------------------------------------------------------------------------
/*! \brief Determines whether or not the EqVar is part of the specified release.
 *
 *  \param release Release number (5 digits).
 *
 *  \return TRUE if the EqVar is part of the specified release.
 */

bool EqVar::isCurrent( int release ) const
{
    return( release >= m_releaseFrom && release <= m_releaseThru );
}

//------------------------------------------------------------------------------
/*! \brief Determines if EqVar is a diagram toggle variable.
 *
 *  \return TRUE if EqVar is a diagram toggle variable, FALSE if not.
 */

bool EqVar::isDiagram( void ) const
{
    return( m_isDiagram );
}

//------------------------------------------------------------------------------
/*! \brief Determines if EqVar is a discrete variable.
 *
 *  \return TRUE if EqVar is a discrete variable, FALSE if not.
 */

bool EqVar::isDiscrete( void ) const
{
    return( m_varType == VarType_Discrete );
}

//------------------------------------------------------------------------------
/*! \brief Determines if EqVar is a text variable.
 *
 *  \return TRUE if EqVar is a text variable, FALSE if not.
 */

bool EqVar::isText( void ) const
{
    return( m_varType == VarType_Text );
}

//------------------------------------------------------------------------------
/*! \brief Tests if the passed itemName is valid for this EqVar.
 *
 *  \returns TRUE if valid, FALSE if not valid.
 */

bool EqVar::isValidItemName( const QString &itemName, bool caseSensitive ) const
{
    EqVarItem *itemPtr = m_itemList->itemWithName( itemName, caseSensitive );
	if ( ! itemPtr  
	  && ( m_name == "vSurfaceFuelBedModel"
	    || m_name == "vSurfaceFuelBedModel1"
		|| m_name == "vSurfaceFuelBedModel2" ) )
	{
		itemPtr = m_itemList->itemWithSort( itemName, caseSensitive );
	}
    return( ( itemPtr ) ? true : false );
}

//------------------------------------------------------------------------------
/*! \brief Tests if the passed itemName is valid for this EqVar.
 *
 *  \returns TRUE if valid, FALSE if not valid.
 */

bool EqVar::isValidItemSort( const QString &itemSort, bool caseSensitive ) const
{
    EqVarItem *itemPtr = m_itemList->itemWithSort( itemSort, caseSensitive );
    return( ( itemPtr )
            ? true
            : false );
}

//------------------------------------------------------------------------------
/*! \brief Tests if the passed value is within this EqVar's range.
 *
 *  \return TRUE if within range, or FALSE if outside the range.
 */

bool EqVar::isValidRange( double value ) const
{
    //static double f[9] = { 1., 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001,
    //    0.0000001, 0.00000001 };
    static double f[9] = { 0.49, 0.049, 0.0049, 0.00049, 0.000049, 0.0000049,
        0.00000049, 0.000000049, 0.0000000049 };
    double slop = 0.;
    // If display units are different from native units, allow some leeway
    if ( m_displayUnits != m_nativeUnits )
    {
        slop = f[ m_displayDecimals ];
    }
    return( ( value < ( m_displayMinimum - slop )
           || value > ( m_displayMaximum + slop ) )
                ? false
                : true );
}

//------------------------------------------------------------------------------
/*! \brief Validates the EqVar's m_store contents AND updates its token count.
 *
 *  \retval TRUE on success with "tokens" containing the number of tokens.
 *  \retval Returns FALSE on error with:
 *          \a tokens containing the bad token index (base 1),
 *          \a position containing the position (base 0) of token's first char,
 *          and \a length containing the token length.
 */

bool EqVar::isValidStore( int *tokens, int *position, int *length )
{
    bool result = isValidString( m_store, tokens, position, length );
    if ( result )
    {
        m_tokens = *tokens;
    }
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Validates a string containing 0 or more input tokens for the EqVar.
 *  Discrete variables are validated against their item list.
 *  Continuous variables are validated against their ranges.
 *  Text variables are not validated.
 *
 *  \par Side Effects
 *  Calls setItemName() or setDisplayValue() for the first token validated.
 *
 *  \retval Returns TRUE on success with \a tokens containing the number of tokens.
 *  \retval Returns FALSE on error with:
 *          \a tokens containing the bad token index (base 1),
 *          \a position containing the position (base 0) of token's first char,
 *          and \a length containing the token length.
 */

bool EqVar::isValidString( const QString &str,
    int *tokens, int *position, int *length )
{
    // Get the current entry text and parse out each token.
    Parser parser( " \t,\"", "", "" );
    *tokens = parser.parse( str );
    *position = -1;
    *length = 0;
    QString token(""), caption(""), msg("");

	// Start with special test cases:
	if ( m_name == "vSurfaceFuelBedModelCode" )
	{
		QString minLen("1");
		QString maxLen("7");
		position = 0;
		if ( str.stripWhiteSpace() == "" )
		{
			translate( caption, "EqVar:InvalidInput:Caption" );
			translate( msg, "EqVar:InvalidInput:TextMissing", *m_label, minLen, maxLen );
			error( caption, msg );
			return( false );
		}
		if ( str.length() < 1 || str.length() > 7 )
		{
			translate( caption, "EqVar:InvalidInput:Caption" );
			translate( msg, "EqVar:InvalidInput:TextLength", *m_label, minLen, maxLen );
			error( caption, msg );
			return( false );
		}
	}

    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );

		// Start with special test cases:
		if ( m_name == "vSurfaceFuelBedModelNumber" )
		{
            double d;
            *position = parser.position(i);
            *length = parser.length(i);
            if ( ! isValidDouble( token, &d ) )
            {
                translate( caption, "EqVar:InvalidInput:Caption" );
                translate( msg, "EqVar:InvalidInput:Double", *m_label, token );
                error( caption, msg );
                return( false );
            }
			int n = d;
			if ( n <= 13						// standard fuel models
			 || ( n >=  90 && n<=  99 )			// NB series
			 || ( n >= 101 && n<= 109 )			// GR series
			 || ( n >= 121 && n<= 124 )			// GS series
			 || ( n >= 141 && n<= 149 )			// SH series
			 || ( n >= 161 && n<= 165 )			// TU series
			 || ( n >= 181 && n<= 189 )			// TL series
			 || ( n >= 201 && n<= 204 )			// SB series
			 || n > 256 )						// FARSITE upper limit
			{
                translate( caption, "EqVar:InvalidInput:Caption" );
                translate( msg, "EqVar:InvalidInput:FuelModelNumber", *m_label, token );
                error( caption, msg );
                return( false );
			}
		}

        if ( isDiscrete() )
        {
            if ( ! isValidItemName( token, false ) )
            {
                *position = parser.position(i);
                *length = parser.length(i);
                translate( caption, "EqVar:InvalidInput:Caption" );
                translate( msg, "EqVar:InvalidInput:Choice", *m_label, token );
                error( caption, msg );
                return( false );
            }
            // Store this only if its the first token
            if ( i == 0 )
            {
                setItemName( token, false );
            }
        }
        else if ( isContinuous() )
        {
            double d;
            if ( ! isValidDouble( token, &d ) )
            {
                *position = parser.position(i);
                *length = parser.length(i);
                translate( caption, "EqVar:InvalidInput:Caption" );
                translate( msg, "EqVar:InvalidInput:Double", *m_label, token );
                error( caption, msg );
                return( false );
            }
            if ( ! isValidRange( d ) )
            {
                *position = parser.position(i);
                *length = parser.length(i);
                translate( caption, "EqVar:InvalidInput:Caption" );
                translate( msg, "EqVar:InvalidInput:Range", *m_label, token,
                    QString( "%1 - %2 %3" )
                    .arg( m_displayMinimum, 0, 'f', m_displayDecimals )
                    .arg( m_displayMaximum, 0, 'f', m_displayDecimals )
                    .arg( m_displayUnits ) );
                error( caption, msg );
                return( false );
            }
            // Store this only if its the first token
            if ( i == 0 )
            {
                setDisplayValue( d );
            }
        }
        // Other text and other variable types are always ok.
        else
        {
            // Nothing
        }
    }
    return ( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqVar's nativeValue with the passed value,
 *  propagates the dirty flag up the EqTree,
 *  performs any necessary conversion for its displayValue,
 *  and writes the single value into the m_store.
 */

double EqVar::nativeStore( double value )
{
    nativeValue( value );
    m_store.sprintf( "%1.*f", m_displayDecimals, m_displayValue );
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqVar's m_nativeValue with the passed value,
 *  propagates the dirty flag up the EqTree,
 *  and performs any necessary conversion for its displayValue.
 */

double EqVar::nativeValue( double value )
{
    update( value );
    propagateDirty();
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Propagates an EqVar's dirty flags to EqVars further up the EqTree
 *  until another dirty EqVar is found.
 */

void EqVar::propagateDirty( int level )
{
//QString str;
//str.sprintf( "%*.*s %2d: propagateDirty(%s)\n",
//2*level, 2*level, "", level, m_name.latin1() );
//log( str );

    EqFun *funPtr;
    EqVar *varPtr;
    // Mark this EqVar as dirty input for all its consumer functions.
    for ( int cid = 0;
          cid < m_consumers;
          cid++ )
    {
        // Get consumer's EqFun pointer.
        funPtr = m_consumer[cid];
        // Set the function's dirty flag for this input variable.
        if ( ! funPtr->setDirty( this ) )
        {
            // Propagate dirty flag to all this function's output variables.
            for ( int oid = 0;
                  oid < funPtr->m_outputs;
                  oid++ )
            {
                varPtr = funPtr->m_output[oid];
                varPtr->propagateDirty( level+1 );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqVar's displayUnits with the passed value,
 *  validates the displayUnits,
 *  recalculates the displayValue, displayMinimum, and displayMaximum
 *  for the new units, and
 *  updates the m_store text.
 *
 *  \retval TRUE on success.
 *  \retval FALSE on error with results in appSiUnits()->htmlMsg().
 */

bool EqVar::setDisplayUnits( const QString &units, int decimals )
{
	// UnitsEditDialog::store() passes in decimals == -1 if decimals cannot be edited
	if ( decimals < 0 )
	{
		decimals = m_displayDecimals;
	}
    // Make sure this isn't a redundant call
    if ( units == m_displayUnits && decimals == m_displayDecimals )
    {
        return( true );
    }
    // Can we convert to the requested units?
    double factor, offset;
    if ( ! appSiUnits()->conversionFactorOffset(
        m_nativeUnits.latin1(), units.latin1(), &factor, &offset ) )
    {
        return( false );
    }
    // Ok to convert, so first convert the m_store to native values
    // (only needed if the m_store is not already in native units)
    if ( m_displayUnits != m_nativeUnits )
    {
        convertStoreUnits( true );
    }
    // Now set the new display units and recalculate display value, min, max
    m_displayUnits = units;
    m_displayDecimals = decimals;
    m_factor  = factor;
    m_offset  = offset;
    m_convert = ( fabs(1.-factor)<1.0e-07 && fabs(1.-offset)<1.0e-07 )
        ? 0
        : 1 ;
    m_displayValue   = offset + factor * m_nativeValue;
    m_displayMinimum = offset + factor * m_nativeMinimum;
    m_displayMaximum = offset + factor * m_nativeMaximum;

    // Finally convert the m_store from native values to the new display values
    // This will always be done, even if already in native units,
    // so we use the native units decimals instead of 6 decimals
    convertStoreUnits( false );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqVar's m_displayValue with the passed value,
 *  propagates the dirty flag up the EqTree,
 *  and performs any necessary conversion for its m_nativeValue.
 *
 *  \param value New value in display units.
 *
 *  \return Returns the passed \a value.
 */

double EqVar::setDisplayValue( double value )
{
    propagateDirty();
    m_displayValue = value;
    m_nativeValue  = ( m_convert==1 )
                     ? (value-m_offset)/m_factor
                     : value;
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Stores the text in the EqVar's m_help.
 *
 *  \return Reference to the new m_store value.
 */

QString &EqVar::setHelp( const QString &help )
{
    return( m_help = help );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current item name for a discrete EqVar
 *  and propagates the dirty flag up the EqTree.
 *
 *  \param doCheck If FALSE, then the caller already validated the name.
 */

void EqVar::setItemName( const QString &itemName, bool doCheck )
{
    if ( doCheck )
    {
        updateItem( itemName );
    }
    else
    {
        m_activeItemName = itemName;
    }
    propagateDirty();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the current item name for a discrete EqVar to the default.
 */

void EqVar::setItemNameToDefault( void )
{
    m_activeItemName = m_itemList->m_nameDefault;
    propagateDirty();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqVar's nativeValue with the passed value,
 *  propagates the dirty flag up the EqTree,
 *  and performs any necessary conversion for its displayValue.
 *
 *  \param value New value in native units.
 *
 *  \return Returns the passed \a value.
 */

double EqVar::setNativeValue( double value )
{
    update( value );
    propagateDirty();
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Stores the text in the EqVar's m_store.
 *
 *  \return Reference to the new m_store value.
 */

QString &EqVar::setStore( const QString &value )
{
    return( m_store = value );
}

//------------------------------------------------------------------------------
/*! \brief Updates continuous EqVar's nativeValue with the passed value
 *  WITHOUT setting or propagating any dirty flags.
 *
 *  Should be called only from within EqFun functions.
 *  All others should call EqVar::displayValue(), EqVar::nativeValue(),
 *  EqVar::itemname(), or EqVar::itemNameToDefault() to ensure
 *  dirty flag propagation.
 */

void EqVar::update( double value )
{
    m_nativeValue = value;
    m_displayValue = ( m_convert==1 )
                     ? (m_offset + m_factor * value)
                     : value;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the discrete EqVar's m_activeItemName with the passed value
 *  WITHOUT setting or propagating any dirty flags.
 *
 *  Should be called only from within EqFun functions.
 *  All others should call EqVar::setItem() to enforce dirty flag propagation.
 */

void EqVar::updateItem( const QString &itemName )
{
    if ( m_itemList )
    {
        EqVarItem *itemPtr = m_itemList->itemWithName( itemName, false );
        if ( ! itemPtr )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "EqVar:InvalidItemName", itemName );
            bomb( text );
        }
        m_activeItemName = itemName;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the discrete EqVar's m_activeItemName with the item with
 the matching data index WITHOUT setting or propagating any dirty flags.
 *
 *  Should be called only from within EqFun functions.
 *  All others should call EqVar::setItem() to enforce dirty flag propagation.
 */

void EqVar::updateItem( int itemDataIndex )
{
    if ( m_itemList )
    {
        EqVarItem *itemPtr = m_itemList->itemWithIndex( itemDataIndex );
        if ( ! itemPtr )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "EqVar:InvalidItemIndex",
                QString( "%1" ).arg( itemDataIndex ) );
            bomb( text );
        }
        m_activeItemName = itemPtr->m_name;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to convert a QString into a double and
 *   report the result.
 *
 *  \retval TRUE on success with *returnValue containing the double value.
 *  \retval FALSE on failure
 */

bool isValidDouble( const char *str, double *returnValue )
{
    char *endPtr;
    *returnValue = strtod( str, &endPtr );
    if ( (endPtr-str) != (int) strlen( str ) )
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of xeqvar.cpp
//------------------------------------------------------------------------------

