//------------------------------------------------------------------------------
/*! \file rxvar.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus RxVar class declaration.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "parser.h"
#include "rxvar.h"

// Qt include files
#include <qstringlist.h>

//------------------------------------------------------------------------------
/*! \brief Custom constructor that assigns the variable's prescription
 *  range to the passed values.
 *
 *  \param varPtr   Pointer to the associated continuous EqVar variable.
 *
 *  Called only by RxVarList::add().
 */

RxVar::RxVar( EqVar *varPtr ) :
    m_nativeMinimum(0.),
    m_nativeMaximum(0.),
    m_displayMinimum(0.),
    m_displayMaximum(0.),
    m_varPtr( varPtr ),
    m_isActive(false),
    m_storeMinimum("0"),
    m_storeMaximum("0"),
    m_firstItemBox(-1)
{
    m_itemChecked[0] = 1;
    m_itemChecked[1] = m_itemChecked[2] = m_itemChecked[3] = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Default destructor.
 */

RxVar::~RxVar( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the RxVar's displayMinimum and displayMaximum to the requested
 *  units and decimal places.
 *
 *  Called only by RxVar:applyEnglishUnits(), RxVar::applyMetricUnits(),
 *  or RxVar::applyNativeUnits().
 *
 *  \retval TRUE on success.
 *  \retval FALSE on error with results in appSiUnits()->htmlMsg().
 */

bool RxVar::applyDisplayUnits( const QString &units, int decimals )
{
    // Can we convert to the requested units?
    double factor, offset;
    if ( ! appSiUnits()->conversionFactorOffset(
        m_varPtr->m_nativeUnits.latin1(), units.latin1(), &factor, &offset ) )
    {
        return( false );
    }
    // Now set the display values to new units
    m_displayMinimum = offset + factor * m_nativeMinimum;
    m_displayMaximum = offset + factor * m_nativeMaximum;
    m_storeMinimum.sprintf( "%1.*f", decimals, m_displayMinimum );
    m_storeMaximum.sprintf( "%1.*f", decimals, m_displayMaximum );

    //dump( QString( "RxVar::applyDisplayUnits('%1', %2) - " )
    //  .arg( units ).arg( decimals) );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Resets the RxVar's display units and decimals to their default
 *  English units values.
 *
 *  Called via BpDocument::configureUnits() -> EqTree::applyUnitsSet()
 *  -> EqTree::applyEnglishUnits().
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVar::applyEnglishUnits( void )
{
    return( applyDisplayUnits( m_varPtr->m_englishUnits, m_varPtr->m_englishDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Resets the RxVar's display units and decimals to its default
 *  metric values.
 *
 *  Called via BpDocument::configureUnits() -> EqTree::applyUnitsSet()
 *  -> EqTree::applyMetricUnits().
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVar::applyMetricUnits( void )
{
    return( applyDisplayUnits( m_varPtr->m_metricUnits, m_varPtr->m_metricDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Resets RxVar's displayMinimum and displayMaximum to native values.
 *
 *  Called via BpDocument::configureUnits() -> EqTree::applyUnitsSet()
 *  -> EqTree::applyNativeUnits().
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVar::applyNativeUnits( void )
{
    return( applyDisplayUnits( m_varPtr->m_nativeUnits, m_varPtr->m_nativeDecimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Dumps RxVar values to stderr.
 */

void RxVar::dump( const QString &prefix )
{
    fprintf( stderr,
        "%s%s, %s, native = %f - %f %s (%d), display = %f - %f %s (%d), store = '%s' - '%s'\n",
        prefix.latin1(),
        m_varPtr->m_label->latin1(),
        m_isActive ? "ACTIVE" : "inactive",
        m_nativeMinimum,
        m_nativeMaximum,
        m_varPtr->m_nativeUnits.latin1(),
        m_varPtr->m_nativeDecimals,
        m_displayMinimum,
        m_displayMaximum,
        m_varPtr->m_displayUnits.latin1(),
        m_varPtr->m_displayDecimals,
        m_storeMinimum.latin1(),
        m_storeMaximum.latin1()
    );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the RxVar's current EqVar value is in range.
 *
 *  \retval TRUE if RxVar is not active
 *  \retval TRUE if RxVar's EqVar current value is within the prescription range.
 *  \retval FALSE if RxVar's EqVar current value is outside the prescription range.
 */

bool RxVar::inRange( void ) const
{
    if ( ! m_isActive )
    {
        return( true );
    }

    if ( isContinuous() )
    {
        bool inRange = m_varPtr->m_nativeValue >= m_nativeMinimum
                    && m_varPtr->m_nativeValue <= m_nativeMaximum;

        //fprintf( stderr,
        //  "%s current native value %f is %s range %f - %f %s\n",
        //  m_varPtr->m_label->latin1(),
        //  m_varPtr->m_nativeValue,
        //  inRange ? "WITHIN" : "OUTSIDE",
        //  m_nativeMinimum,
        //  m_nativeMaximum,
        //  m_varPtr->m_nativeUnits.latin1() );
        return( inRange );
    }
    else
    {
        int item = m_varPtr->activeItemDataIndex();
        return( m_itemChecked[item] );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns TRUE if RxVar's EqVar is continuous.
 */

bool RxVar::isContinuous( void ) const
{
    return( m_varPtr->isContinuous() );
}

//------------------------------------------------------------------------------
/*! \brief Returns TRUE if RxVar's EqVar is discrete.
 */

bool RxVar::isDiscrete( void ) const
{
    return( m_varPtr->isDiscrete() );
}

//------------------------------------------------------------------------------
/*! \brief Returns TRUE if RxVar's EqVar is selected as user output.
 */

bool RxVar::isUserOutput( void ) const
{
    return( m_varPtr->m_isUserOutput );
}

//------------------------------------------------------------------------------
/*! \brief Validates an RxVar entry string.
 *
 *  \retval Returns TRUE on success, FALSE on error.
 */

bool RxVar::isValid( const QString &str )
{
    // Parse tokens from the string
    Parser parser( " \t,\"", "", "" );
    int tokens = parser.parse( str );
    double d;
    QString token, caption(""), msg("");
    for ( int i = 0;
          i < tokens;
          i++ )
    {
        token = parser.token( i );
        if ( ! isValidDouble( token, &d ) )
        {
            translate( caption, "EqTree:InvalidRxInput:Caption" );
            translate( msg, "EqTree:InvalidRxInput:Double",
                *(m_varPtr->m_label) );
            error( caption, msg );
            return( false );
        }
    }
    return ( true );
}

//------------------------------------------------------------------------------
/*! \brief Returns TRUE if the discrete item checked flag is nonzero.
 */

bool RxVar::itemChecked( int id ) const
{
    return( m_itemChecked[id] > 0 );
}

//------------------------------------------------------------------------------
/*! \brief Returns number of possible items for a discrete variable.
 */

int RxVar::items( void ) const
{
    return( m_varPtr->isContinuous() ? 0 : m_varPtr->m_itemList->count() );
}

//------------------------------------------------------------------------------
/*! \brief Sets the index of the first discrete item's "acceptable" check box.
 */

void RxVar::setFirstItemBox( int id )
{
    m_firstItemBox = id;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the discrete item checked flag to 0 or 1.
 */

bool RxVar::setItemChecked( int id, bool checked )
{
    return( m_itemChecked[id] = ( checked ) ? 1 : 0 );
}

//------------------------------------------------------------------------------
/*! \brief Updates the RxVar values and stores from the passed strings.
 *
 *  \param strMinimum   String representation of lower prescription bound
 *                      in display units.
 *  \param strMaximum   String representation of upper prescription bound
 */

void RxVar::storeMinMax( const QString &strMinimum, const QString &strMaximum )
{
    // Convert passed strings to doubles
    if ( ! isValidDouble( strMinimum, &m_displayMinimum ) )
    {
        m_displayMinimum = 0.0;
    }
    if ( ! isValidDouble( strMaximum, &m_displayMaximum ) )
    {
        m_displayMaximum = 99999999.0;
    }
    if ( m_displayMinimum > m_displayMaximum )
    {
        double tmp = m_displayMaximum;
        m_displayMaximum = m_displayMinimum;
        m_displayMinimum = tmp;
    }

    // Reformat passed strings into the stores
    m_storeMinimum.sprintf( "%1.*f",
        m_varPtr->m_displayDecimals, m_displayMinimum );
    m_storeMaximum.sprintf( "%1.*f",
        m_varPtr->m_displayDecimals, m_displayMaximum );

    // Store in native units
    double factor, offset;
    appSiUnits()->conversionFactorOffset( m_varPtr->m_displayUnits.latin1(),
        m_varPtr->m_nativeUnits.latin1(), &factor, &offset );
    m_nativeMinimum = offset + factor * m_displayMinimum;
    m_nativeMaximum = offset + factor * m_displayMaximum;

    //dump( "RxVar::storeEntry() - " );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the RxVar's prescription toggle, minimum, and maximum.
 *
 *  \param isActive         If TRUE, prescription variable is active.
 *  \param nativeMinimum    Lower prescription bound in native units.
 *  \param nativeMaximum    Upper prescription bound in native units.
 *  \param displayMinimum   Lower prescription bound in display units.
 *  \param displayMaximum   Upper prescription bound in display units.
 *
 *  Called only by EqTreeParser::handlePrescription().
 */

void RxVar::update( bool isActive, double nativeMinimum, double nativeMaximum,
    double displayMinimum, double displayMaximum )
{
    m_isActive       = isActive;
    m_nativeMinimum  = nativeMinimum;
    m_nativeMaximum  = nativeMaximum;
    m_displayMinimum = displayMinimum;
    m_displayMaximum = displayMaximum;
    m_storeMinimum.sprintf( "%1.*f",
        m_varPtr->m_displayDecimals, m_displayMinimum );
    m_storeMaximum.sprintf( "%1.*f",
        m_varPtr->m_displayDecimals, m_displayMaximum );
    //dump( "RxVar::update() - " );
    return;
}

//------------------------------------------------------------------------------
/*! \brief RxVarList constructor.
 *
 *  Called only by EqTree::EqTree() constructor.
 */

RxVarList::RxVarList( void ) :
    m_dict( 37 )
{
    setAutoDelete( true );
    m_dict.setAutoDelete( false );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds a new RxVar to the RxVarList and its dictionary.
 *
 *  \param varPtr   Pointer to the continuous EqVar variable.
 */

void RxVarList::add( EqVar *varPtr )
{
    RxVar *rx = new RxVar( varPtr );
    checkmem( __FILE__, __LINE__, rx, "RxVar rx", 1 );
    append( rx );
    m_dict.insert( varPtr->m_name, rx );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Resets the RxVarList's display units and decimals to their default
 *  English units values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVarList::applyEnglishUnits( void )
{
    for ( RxVar *rxVar = first(); rxVar; rxVar = next() )
    {
        if ( ! rxVar->applyEnglishUnits() )
        {
            return( false );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Resets the RxVarList's display units and decimals to its default
 *  metric values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVarList::applyMetricUnits( void )
{
    for ( RxVar *rxVar = first(); rxVar; rxVar = next() )
    {
        if ( ! rxVar->applyMetricUnits() )
        {
            return( false );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Resets the RxVarList's display units and decimals to its default
 *  native values.
 *
 *  \return TRUE on success or FALSE on error.
 */

bool RxVarList::applyNativeUnits( void )
{
    for ( RxVar *rxVar = first(); rxVar; rxVar = next() )
    {
        if ( ! rxVar->applyNativeUnits() )
        {
            return( false );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Finds an EqVar in the RxVarList and returns its RxVar pointer.
 *
 *  \param varPtr   Pointer to the continuous EqVar variable.
 */

RxVar *RxVarList::find( EqVar *varPtr )
{
    return( m_dict.find( varPtr->m_name ) );
}

//------------------------------------------------------------------------------
/*! \brief Returns the total number of items of all discrete RxVars in the list.
 */

int RxVarList::items( void )
{
    int n = 0;
    for( RxVar *rxVar = first(); rxVar; rxVar=next() )
    {
        if ( rxVar->isDiscrete() )
        {
            n += rxVar->items();
        }
    }
    return( n );
}

//------------------------------------------------------------------------------
/*! \brief Returns the total number of discrete items of all OUTPUT discrete
 *  RxVars in the list.
 */

int RxVarList::outputItems( void )
{
    int n = 0;
    for( RxVar *rxVar = first(); rxVar; rxVar=next() )
    {
        if ( rxVar->isDiscrete() && rxVar->isUserOutput() )
        {
            n += rxVar->items();
        }
    }
    return( n );
}

//------------------------------------------------------------------------------
/*! \brief Find's the RxVar with the specified \a name.
 *
 *  \param name Name of the RxVar to locate.
 *
 *  \return Address of the RxVar with \a name.
 */

RxVar *RxVarList::rxVar( const QString &name ) const
{
    return( m_dict.find( name ) );
}

//------------------------------------------------------------------------------
/*! \brief Writes the current RxVarList contents in sorted order to the file
 *  stream. Assumes the XML header has already been written by the caller,
 *  and the XML footer will be added by the caller.
 *
 *  \param fptr     Output file stream pointer.
 *
 *  \return TRUE on success, FALSE if unable to open the file.
 */

bool RxVarList::writeXmlFile( FILE *fptr )
{
    // Write out all prescriptions to a string list for sorting
    QStringList rxList;
    for( RxVar *rxVar = first(); rxVar; rxVar=next() )
    {
        EqVar *eqVar = rxVar->m_varPtr;
        if ( eqVar->isContinuous() )
        {
            rxList.append( QString(
                "  <prescription name=\"%1\" active=\"%2\" minimum=\"%3\" maximum=\"%4\" units=\"%5\" />" )
                .arg( eqVar->m_name )
                .arg( rxVar->m_isActive ? "true" : "false" )
                .arg( rxVar->m_displayMinimum, 0, 'f', eqVar->m_displayDecimals )
                .arg( rxVar->m_displayMaximum, 0, 'f', eqVar->m_displayDecimals )
                .arg( eqVar->m_displayUnits ) );
        }
        else
        {
            QString toggles( "" );
            int nItems = rxVar->items();
            for ( int i=0; i<nItems; i++ )
            {
                toggles += QString( "%1%2" )
                    .arg( rxVar->m_itemChecked[i] )
                    .arg( ( i < nItems - 1 ) ? "," : "" );
            }
            rxList.append( QString(
                "  <prescription name=\"%1\" active=\"%2\" accept=\"%3\" />" )
                .arg( eqVar->m_name )
                .arg( rxVar->m_isActive ? "true" : "false" )
                .arg( toggles ) );
        }
    }
    // Sort the prescription variable list and write it to the file
    rxList.sort();
    for ( QStringList::Iterator sit=rxList.begin();
          sit != rxList.end();
          ++sit )
    {
        fprintf( fptr, "%s\n", (*sit).latin1() );
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of rxvar.cpp
//------------------------------------------------------------------------------

