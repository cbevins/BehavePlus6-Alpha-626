//------------------------------------------------------------------------------
/*! \file xeqtreeparser.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree parser class methods.
 *  This reads all BehavePlus runs, worksheet, fuel model, moisture scenario,
 *  and units set files.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "property.h"
#include "rxvar.h"
#include "xeqtree.h"
#include "xeqtreeparser.h"
#include "xeqvar.h"

#include "Algorithms/Fofem6Species.h"

// Qt include files
#include <qapplication.h>
#include <qprogressdialog.h>

// Standard include files
#include <stdlib.h>
#include <iostream>

//------------------------------------------------------------------------------
/*! \brief EqTreeParser constructor.
 *
 *  \param eqTree    Ptr to parent EqTree
 *  \param fileName  Name of the EqTree definition document to be read.
 *  \param unitsOnly If TRUE, only continuous variable units and decimals
 *                   are applied to the EqTree by the handler functions.
 *  \param validate  If TRUE, extra validation is performed.
 *  \param debug     If TRUE, input processing is echoed to stdout.
 */

EqTreeParser::EqTreeParser( EqTree *eqTree, const QString &fileName,
        bool unitsOnly, bool validate, bool debug ) :
    XmlParser( fileName, validate, debug ),
    m_eqTree(eqTree),
    m_unitsOnly(unitsOnly)
{
    //printf( "Parsing '%s' ...\n", fileName.latin1() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Handles the <BehavePlus> element.
 *
 *  This uses error message translation since a translation dictionary should
 *  be available.
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqTreeParser::handleBehavePlus( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int id;
    // "type" attribute is required
    if ( ( id = attribute.index( "type" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingName", elementName, "type" );
        return( false );
    }
    m_eqTree->m_type = attribute.value( id );
    // "release" attribute is optional
    m_eqTree->m_release = 10000;
    if ( ( id = attribute.index( "release" ) ) >= 0 )
    {
        bool ok;
        m_eqTree->m_release = attribute.value( id ).toInt( &ok, 10 );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <prescription> element.
 *
 *  This uses error message translation since a translation dictionary should
 *  be available.
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqTreeParser::handlePrescription( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int id;
    bool ok;
    QString name, active, minimum, maximum, units, accept;

    // If we're only loading units, we're done!
    if ( m_unitsOnly )
    {
        return( true );
    }

    // "name" attribute is required
    if ( ( id = attribute.index( "name" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingName", elementName, "name" );
        return( false );
    }
    name = attribute.value( id );
    // Handles V2 to V3 changes as no-ops
    if ( name == "vSurfaceFireEffWindAtHead"
      || name == "vSurfaceFuelMoisDead1"
      || name == "vSurfaceFuelMoisDead10"
      || name == "vSurfaceFuelMoisDead100"
      || name == "vSurfaceFuelMoisLifeDead"
      || name == "vSurfaceFuelMoisLifeLive"
      || name == "vSurfaceFuelMoisLiveHerb"
      || name == "vSurfaceFuelMoisLiveWood"
      || name == "vWindSpeedAt20Ft"
      || name == "vWindSpeedAtMidflame"
      || name == "vSurfaceFireScorchHtAtHead"
      || name == "vTreeCrownVolScorchedAtHead"
      || name == "vTreeMortalityRateAtHead"
    )
    {
        return( true );
    }
    // Find this prescription variable.
    RxVar *rxVar = m_eqTree->m_rxVarList->rxVar( name );
    if ( ! rxVar )
    {
        trError( "EqTreeParser:UnknownProperty",
            elementName, name, "name", name );
        return( false );
    }

    // "active" attribute is required
    if ( ( id = attribute.index( "active" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingAttribute", elementName, name, "active" );
        return( false );
    }
    active = attribute.value( id );
    // "active" must be true or false
    bool isActive = false;
    if ( active == "true" )
    {
        isActive = true;
    }
    else if ( active == "false" )
    {
        isActive = false;
    }
    else
    {
        trError( "EqTreeParser::NotABoolean",
            elementName, "active", active );
        return( false );
    }

    // If this is a discrete variable, it has an "accept" attribute
    int acceptId  = attribute.index( "accept" );
    if ( acceptId >= 0 )
    {
        rxVar->m_isActive = isActive;
        accept = attribute.value( acceptId );
        int items = rxVar->items();
        for ( int i=0; i<items; i++ )
        {
            rxVar->m_itemChecked[i] =
                ( accept.section( ",", i, i ) == "0" ) ? 0 : 1;
        }
        return( true );
    }

    // "minimum" attribute is required
    int minimumId = attribute.index( "minimum" );
    if ( minimumId < 0 )
    {
        trError( "EqTreeParser:MissingAttribute", elementName, name, "minimum" );
        return( false );
    }
    minimum = attribute.value( minimumId );
    // Validate minimum value as a double
    double displayMinimum = minimum.toDouble( &ok );
    if ( ! ok )
    {
        trError( "EqTreeParser::NotADouble",
            elementName, "minimum", minimum );
        return( false );
    }

    // "maximum" attribute is required
    int maximumId = attribute.index( "maximum" );
    if ( maximumId < 0 )
    {
        trError( "EqTreeParser:MissingAttribute", elementName, name, "maximum" );
        return( false );
    }
    maximum = attribute.value( maximumId );
    // Validate maximum value as a double
    double displayMaximum = maximum.toDouble( &ok );
    if ( ! ok )
    {
        trError( "EqTreeParser::NotADouble",
            elementName, "maximum", maximum );
        return( false );
    }

    // "units" attribute is required
    if ( ( id = attribute.index( "units" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingAttribute", elementName, name, "units" );
        return( false );
    }
    if ( ( units = attribute.value( id ) ) == "(null)" )
    {
        units = "";
    }

    double nativeMinimum = displayMinimum;
    double nativeMaximum = displayMaximum;

    // Units must be compatible with native units.
    EqVar *varPtr = rxVar->m_varPtr;
    // No units case...
    if ( units == "none" )
    {
        // Check if the native units are empty
        if ( varPtr->m_nativeUnits != "" )
        {
            trError( "EqTreeParser:BadUnits",
                elementName, name, units, varPtr->m_nativeUnits );
            return( false );
        }
        // Reset the units to empty and return
        units = "";
    }
    else
    {
        double factor, offset;
        if ( ! appSiUnits()->conversionFactorOffset(
            varPtr->m_nativeUnits.latin1(), units, &factor, &offset) )
        {
            trError( "EqTreeParser:BadUnits",
                elementName, name, units, varPtr->m_nativeUnits );
            return( false );
        }
        // Convert displayMinimum and displayMaximum values
        nativeMinimum = offset + factor * displayMinimum;
        nativeMaximum = offset + factor * displayMaximum;
    }
    // Update the prescription range
    rxVar->update( isActive, nativeMinimum, nativeMaximum,
        displayMinimum, displayMaximum );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <property> element.
 *
 *  This uses error message translation since a translation dictionary should
 *  be available.
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqTreeParser::handleProperty( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int id;
    QString name, value;

    // If we're only loading units, we're done!
    if ( m_unitsOnly )
    {
        return( true );
    }
    // "name" attribute is required
    if ( ( id = attribute.index( "name" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingName", elementName, "name" );
        return( false );
    }
    name = attribute.value( id );
    // "value" attribute is required
    if ( ( id = attribute.index( "value" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingAttribute", elementName, name, "value" );
        return( false );
    }
    if ( ( value = attribute.value( id ) ) == "(null)" )
    {
        value = "";
    }
    // Find this property in the local EqTree directory and update it
    Property *property;
    if ( ( property = m_eqTree->m_propDict->find( name ) ) > 0 )
    {
        if ( ! m_eqTree->m_propDict->update( name, value ) )
        {
            trError( "EqTreeParser:BadValue",
                elementName, name, "value", value );
            return( false );
        }
    }
    // Report unknown <property> names here.
    else
    {
        trError( "EqTreeParser:UnknownProperty",
            elementName, name, "name", name );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Handles the <variable> element.
 *
 *  This uses error message translation since a translation dictionary should
 *  be available.
 *
 *  \return TRUE on success, FALSE one error.
 */

bool EqTreeParser::handleVariable( const QString &elementName,
        const QXmlAttributes& attribute )
{
    int     id, decimals;
    QString name, units, value, text, code;
    EqVar  *varPtr;
    bool    ok;
    // "name" attribute is required
    if ( ( id = attribute.index( "name" ) ) < 0 )
    {
        trError( "EqTreeParser:MissingName", elementName, "name" );
        return( false );
    }
    name = attribute.value( id );
    // Check if this is a known variable name.
    if ( ! ( varPtr = m_eqTree->m_varDict->find( name ) ) )
    {
        // HACK -- these names were introduced in V1 but dropped for V2
        // They are kept here to keep old V1 run and worksheet files working
        if ( name == "vSurfaceFireFlameHt"
          || name == "vSurfaceFireFlameAngle"
          || name == "vSurfaceFireSafetyZoneHuman"
          || name == "vSurfaceFuelBedCoverage"
          || name == "vWthrCumulusBaseHt"
          || name == "vWthrHeatIndex"
          || name == "vWthrSummerSimmerIndex"
          || name == "vWthrWindChillTemp"
          // These were eliminated in the Great Purge of 2007
          || name == "vSurfaceFireFlameAngleAtHead"
          || name == "vSurfaceFireFlameHtAtHead"
          || name == "vSurfaceFireScorchHtAtHead"
          || name == "vTreeCrownLengFractionScorchedAtHead"
          || name == "vTreeCrownLengScorchedAtHead"
          || name == "vTreeCrownVolScorchedAtHead"
          || name == "vTreeMortalityCountAtHead" 
          || name == "vTreeMortalityRateAtHead" 
          )
        {
            return( true );
        }
        trError( "EqTreeParser:BadValue", elementName, name, "name", name );
        return( false );
    }
    // Continuous variables
    if ( varPtr->isContinuous() )
    {
        // "decimals" attribute is required
        if ( ( id = attribute.index( "decimals" ) ) < 0 )
        {
            trError( "EqTreeParser:MissingAttribute",
                elementName, name, "decimals" );
            return( false );
        }
        decimals = attribute.value( id ).toInt( &ok, 10 );
        if ( ! ok )
        {
            trError( "EqTreeParser::NotAnInteger",
                elementName, "decimals", attribute.value( id ) );
            return( false );
        }
        // "units" attribute is required
        if ( ( id = attribute.index( "units" ) ) < 0 )
        {
            trError( "EqTreeParser:MissingAttribute",
                elementName, name, "units" );
            return( false );
        }
        units = attribute.value( id );
        // "value" attribute is required
        if ( ( id = attribute.index( "value" ) ) < 0 )
        {
            trError( "EqTreeParser:MissingAttribute",
                elementName, name, "value" );
            return( false );
        }
        value = attribute.value( id );
        // Units must be compatible with native units.
        // No units case...
        if ( units == "none" )
        {
            // Check if the native units are empty
            if ( varPtr->m_nativeUnits != "" )
            {
                trError( "EqTreeParser:BadUnits",
                    elementName, name, units, varPtr->m_nativeUnits );
                return( false );
            }
            // Reset the unit the empty and return
            units = "";
        }
        // Units must be compatible with native units.
        else
        {
            double factor, offset;
            if ( ! appSiUnits()->conversionFactorOffset(
                varPtr->m_nativeUnits.latin1(), units, &factor, &offset) )
            {
                trError( "EqTreeParser:BadUnits",
                    elementName, name, units, varPtr->m_nativeUnits );
                return( false );
            }
        }
        // Set the variable's display units and decimals.
        if ( ! varPtr->setDisplayUnits( units, decimals ) )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "EqTreeParser:UnitsError",
                elementName, name, units, appSiUnits()->htmlMsg() );
            error( text );
            return( false );
        }
        // Store any specified values AFTER displayUnits and displayDecimals are set!!
        if ( ! m_unitsOnly )
        {
            varPtr->setStore( value );
        }
    }
    // Discrete variables
    else if ( varPtr->isDiscrete() )
    {
        // Skip if we're only loading units
        if ( m_unitsOnly )
        {
            return( true );
        }
        // "code" attribute is required
        if ( ( id = attribute.index( "code" ) ) < 0 )
        {
            trError( "EqTreeParser:MissingAttribute",
                elementName, name, "code" );
            return( false );
        }
        code = attribute.value( id );
		// Convert BehavePlus5/FOFEM5 species codes to BehavePlus6/FOFEM6 species codes
		if ( ( name == "vTreeSpecies"
	 	    || name == "vTreeSpeciesMortality"
		    || name == "vTreeSpeciesSpot" )
		  && ! code.isEmpty() )
		{
			// Codes may be sparated by commas, blanks, or both
			code.replace( QChar( ',' ), " " );
			const char* codeLatin1 = code.latin1();
			QStringList oldList = QStringList::split( " ", code.simplifyWhiteSpace() );
			QStringList newList;
		    for ( QStringList::Iterator it = oldList.begin(); it != oldList.end(); ++it )
			{
				QString spp = (*it).upper();
				const char* oldSpp = spp.latin1();
				// First check if this is a FOFEM v6 species code
				int idx = Fofem6Species::getFofem6SpeciesIndex( oldSpp );
				if ( idx >= 0 )
				{
					newList << QString( oldSpp );
					continue;
				}
				// Otherwise check if this is a FOFEM v5 species code
				const char* newSpp = Fofem6Species::getFofem6SpeciesCode( oldSpp );
				if ( newSpp )
				{
					newList << QString( newSpp );
					continue;
				}
				// Otherwise this is an unknown species code, which will cause an error
				// dialog when the user leaves the worksheet entry field
			}
			code = newList.join( ", " );
		}
        // Store the text variable
        varPtr->setStore( code );
    }
    // Text variables
    else if ( varPtr->isText() )
    {
        // Skip if we're only loading units
        if ( m_unitsOnly )
        {
            return( true );
        }
        // "text" attribute is required
        if ( ( id = attribute.index( "text" ) ) < 0 )
        {
            trError( "EqTreeParser:MissingAttribute",
                elementName, name, "text" );
            return( false );
        }
        text = attribute.value( id );
        // Store the text variable
        varPtr->setStore( text );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Start-of-element callback.  This does most the work.
 */

bool EqTreeParser::startElement( const QString & /*namespaceUrl */,
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
    // Skip all elements until <BehavePlus> is found.
    if ( ! m_elements )
    {
        if ( elementName == "BehavePlus" )
        {
            push( elementName );
            if ( ! handleBehavePlus( elementName, attribute ) )
            {
                return( false );
            }
            return( true );
        }
        trError( "EqTreeParser:UnknownDocument" );
        return( false );
    }
    // <property> elements
    if ( elementName == "property" )
    {
        push( elementName );
        if ( ! handleProperty( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <variable> elements
    else if ( elementName == "variable" )
    {
        push( elementName );
        if ( ! handleVariable( elementName, attribute ) )
        {
            return( false );
        }
    }
    // <prescription> elements
    else if ( elementName == "prescription" )
    {
        push( elementName );
        if ( ! handlePrescription( elementName, attribute ) )
        {
            return( false );
        }
    }
    // Ignore all other tags
    else
    {
        trError( "EqTreeParser:UnknownElement", elementName );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
//  End of xeqtreeparser.cpp
//------------------------------------------------------------------------------

