//------------------------------------------------------------------------------
/*! \file xeqtree.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appproperty.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "filesystem.h"
#include "fuelmodel.h"
#include "moisscenario.h"
#include "parser.h"
#include "property.h"
#include "rxvar.h"
#include "xeqapp.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqtreeparser.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qapplication.h>
#include <qprogressdialog.h>
#include <qdatetime.h>

// Standard include files
#include <stdlib.h>
#include <iostream>

//------------------------------------------------------------------------------
/*! \brief EqTree constructor.
 *
 *  This simply allocates the pointer arrays.  EqTree::init() must be called
 *  after parsing the EqTree definition XML document to fill the array
 *  cross references and create the implementation-specific EqCalc data.
 *
 *  The language must then be set and the property dictionary filled.
 *
 *  The above are all performed by EqApp::newEqTree(), which should probably
 *  be the only caller of this constructor.
 */

EqTree::EqTree( EqApp *eqApp, const QString &name, int funCount, int funPrime,
        int varCount, int varPrime, int propPrime,
        EqVarItemList **itemList, int itemListCount,
        QDict<EqVarItemList> *itemListDict,
        FuelModelList *fuelModelList, MoisScenarioList *moisScenarioList ) :
    m_eqApp(eqApp),
    m_name(name),
    m_lang("en_US"),
    m_type(""),
    m_release(0),
    m_fun(0),
    m_funCount(funCount),
    m_var(0),
    m_varCount(varCount),
    m_leaf(0),
    m_leafCount(0),
    m_root(0),
    m_rootCount(0),
    m_itemList(itemList),
    m_itemListCount(itemListCount),
    m_funDict(0),
    m_varDict(0),
    m_propDict(0),
    m_itemListDict(itemListDict),
    m_fuelModelList(fuelModelList),
    m_moisScenarioList(moisScenarioList),
    m_rxVarList(0),
    m_debug(false),
    m_eqCalc(0),
    m_maxRangeVars(16),
    m_rangeVar(0),
    m_rangeVars(0),
    m_rangeCase(0),
    m_tableRows(0),
    m_tableCols(0),
    m_tableVars(0),
    m_tableCells(0),
    m_tableCol(0),
    m_tableRow(0),
    m_tableVal(0),
    m_tableInRx(0),
    m_tableVar(0),
    m_resultFile(""),
    m_traceFile(""),
    m_resultFptr(0),
    m_traceFptr(0)
{
    // Allocate all dynamic storage
    QString text("");
    m_fun = new EqFun *[ m_funCount ];
    checkmem( __FILE__, __LINE__, m_fun, "EqFun *m_fun", m_funCount );

    m_var = new EqVar *[ m_varCount ];
    checkmem( __FILE__, __LINE__, m_var, "EqVar *m_var", m_varCount );

    m_leaf = new EqVar *[ m_varCount ];
    checkmem( __FILE__, __LINE__, m_leaf, "EqVar *m_leaf", m_varCount );

    m_root = new EqVar *[ m_varCount ];
    checkmem( __FILE__, __LINE__, m_root, "EqVar *m_root", m_varCount );

    m_rangeVar = new EqVar *[ m_maxRangeVars ];
    checkmem( __FILE__, __LINE__, m_rangeVar, "EqVar *m_rangeVar",
        m_maxRangeVars );

    // Create local dictionaries
    m_funDict = new QDict<EqFun>( funPrime, true );
    Q_CHECK_PTR( m_funDict );
    m_funDict->setAutoDelete( true );

    m_varDict = new QDict<EqVar>( varPrime, true );
    Q_CHECK_PTR( m_varDict );
    m_varDict->setAutoDelete( true );

    m_propDict = new PropertyDict( propPrime, true );
    checkmem( __FILE__, __LINE__, m_propDict, "PropertyDict m_propDict", 1 );
    m_propDict->setAutoDelete( true );

    // Fill the m_fun[] array with new EqFuns using EqApp attributes
    int id;
    EqFun *f;
    for ( id = 0;
          id < m_funCount;
          id++ )
    {
        // Get the EqApp's version of the EqFun
        f = m_eqApp->m_fun[id];
        // Create a new EqFun
		const char* name = f->m_name;
        EqFun *funPtr = new EqFun( f->m_name, (PFV) 0, f->m_inputs,
            f->m_outputs, f->m_releaseFrom, f->m_releaseThru, f->m_module );
        checkmem( __FILE__, __LINE__, funPtr, "EqFun funPtr", 1 );
        // Add EqFun ptr to the local EqTree name lookup dictionary
        m_funDict->insert( f->m_name, funPtr );
        // Add the EqFun ptr to the local EqTree m_fun[] array
        m_fun[id] = funPtr;
    }

    // Fill the m_var[] array with new EqVars from EqApp defaults
    EqVar *v, *varPtr;
    for ( id = 0;
          id < m_varCount;
          id++ )
    {
        // Initialize these arrays while we're at it
        m_leaf[id] = 0;
        m_root[id] = 0;
        // Get the EqApp's version of this EqVar
        v = m_eqApp->m_var[id];
        // Create a new continuous EqVar using the EqApp attributes
        if ( v->isContinuous() )
        {
            varPtr = new EqVar( v->m_name, v->m_help,
                v->m_inpOrder,      v->m_outOrder,      v->m_wizard,
                v->m_nativeUnits,   v->m_nativeDecimals,
                v->m_englishUnits,  v->m_englishDecimals,
                v->m_metricUnits,   v->m_metricDecimals,
                v->m_nativeMinimum, v->m_nativeMaximum,
                v->m_defaultValue,
                v->m_releaseFrom,   v->m_releaseThru );
        }
        // Create a new discrete EqVar using the EqApp attributes
        else if ( v->isDiscrete() )
        {
            varPtr = new EqVar( v->m_name, v->m_help,
                v->m_inpOrder, v->m_outOrder, v->m_wizard, v->m_itemList,
                v->m_releaseFrom,   v->m_releaseThru );
        }
        // Create a new text EqVar using the EqApp attributes
        else if ( v->isText() )
        {
            varPtr = new EqVar( v->m_name, v->m_help,
                v->m_inpOrder, v->m_outOrder, v->m_wizard,
                v->m_releaseFrom,   v->m_releaseThru );
        }
        checkmem( __FILE__, __LINE__, varPtr,
            QString( "%1 EqVar varPtr" ).arg( v->m_name ), 1 );
        // Assign all non-constructor attributes
        varPtr->m_boundary   = v->m_boundary;
        varPtr->m_boundaries = v->m_boundaries;
        varPtr->m_isConstant = v->m_isConstant;
        varPtr->m_isDiagram  = v->m_isDiagram;
        varPtr->m_isShaded   = v->m_isShaded;
        varPtr->m_isWrap     = v->m_isWrap;
        varPtr->m_master     = v->m_master;
        // Add EqVar ptr to the local EqTree's name lookup dictionary
        m_varDict->insert( v->m_name, varPtr );
        // Add the EqVar ptr to the local EqTree's m_var[] array
        m_var[id] = varPtr;
        // Set the EqVar's producers and consumers from the EqApp values
        varPtr->m_consumers = v->m_consumers;
        varPtr->m_producers = v->m_producers;
        // Create the empty m_consumer[] and m_producer[] arrays
        varPtr->init();
    }

    // Fill EqTree's property dictionary with the application-wide
    // default properties
    QDictIterator<Property> it( *appProperty() );
    Property *prop;
    while( it.current() )
    {
        prop = (Property *) it.current();
        m_propDict->add( it.currentKey(), prop->m_type, prop->m_value,
            prop->m_releaseFrom, prop->m_releaseThru );
        ++it;
    }

    // Initialize the EqVar consumer/producer and EqFun input/output addresses
    init();

    // Create the implementation specific EqCalc.
    m_eqCalc = new EqCalc( this );
    checkmem( __FILE__, __LINE__, m_eqCalc, "EqCalc m_eqCalc", 1 );

    // Create the RxVarList
    m_rxVarList = new RxVarList();
    checkmem( __FILE__, __LINE__, m_rxVarList, "RxVarList *m_rxVarList", 1 );

    // SURFACE Module
    m_rxVarList->add( m_eqCalc->vSurfaceFireSpreadAtHead ) ;        // 1:010
    m_rxVarList->add( m_eqCalc->vSurfaceFireSpreadAtVector ) ;      // 1:010
    m_rxVarList->add( m_eqCalc->vSurfaceFireHeatPerUnitArea ) ;     // 1:020
    m_rxVarList->add( m_eqCalc->vSurfaceFireLineIntAtHead ) ;       // 1:030
    m_rxVarList->add( m_eqCalc->vSurfaceFireLineIntAtVector ) ;     // 1:030
    m_rxVarList->add( m_eqCalc->vSurfaceFireFlameLengAtHead ) ;     // 1:040
    m_rxVarList->add( m_eqCalc->vSurfaceFireFlameLengAtVector ) ;   // 1:040

    // CROWN Module
    m_rxVarList->add( m_eqCalc->vCrownFireTransToCrown) ;           // 2:040
    m_rxVarList->add( m_eqCalc->vCrownFireSpreadRate ) ;            // 2:050
    m_rxVarList->add( m_eqCalc->vCrownFireActiveCrown) ;            // 2:080
    m_rxVarList->add( m_eqCalc->vCrownFireType) ;                   // 2:090

    // SAFETY Module
    m_rxVarList->add( m_eqCalc->vSafetyZoneSepDist ) ;              // 3:010
    m_rxVarList->add( m_eqCalc->vSafetyZoneSize ) ;                 // 3:020
    m_rxVarList->add( m_eqCalc->vSafetyZoneRadius ) ;               // 3:030

    // SIZE Module
    m_rxVarList->add( m_eqCalc->vSurfaceFireArea ) ;                // 4:010

    // CONTAIN Module
    m_rxVarList->add( m_eqCalc->vContainStatus ) ;                  // 5:030
    m_rxVarList->add( m_eqCalc->vContainTime ) ;                    // 5:040
    m_rxVarList->add( m_eqCalc->vContainSize ) ;                    // 5:050

    // SPOT Module
    m_rxVarList->add( m_eqCalc->vSpotDistTorchingTrees ) ;          // 6:010
    m_rxVarList->add( m_eqCalc->vSpotDistBurningPile ) ;            // 6:020
    m_rxVarList->add( m_eqCalc->vSpotDistSurfaceFire ) ;            // 6:030

    // SCORCH Module
    //m_rxVarList->add( m_eqCalc->vSurfaceFireScorchHtAtHead ) ;      // 7:010
    m_rxVarList->add( m_eqCalc->vSurfaceFireScorchHtAtVector ) ;    // 7:010

    // MORTALITY Module
    //m_rxVarList->add( m_eqCalc->vTreeCrownVolScorchedAtHead ) ;     // 8:030
    m_rxVarList->add( m_eqCalc->vTreeCrownVolScorchedAtVector ) ;   // 8:030
    //m_rxVarList->add( m_eqCalc->vTreeMortalityRateAtHead ) ;        // 8:040
    m_rxVarList->add( m_eqCalc->vTreeMortalityRateAtVector ) ;      // 8:040

    // IGNITE Module
    m_rxVarList->add( m_eqCalc->vIgnitionFirebrandProb ) ;          // 9:010
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqTree destructor.
 */

EqTree::~EqTree( void )
{
    //runClean();
    delete   m_rxVarList;   m_rxVarList = 0;
    delete   m_eqCalc;      m_eqCalc = 0;
    delete[] m_fun;         m_fun = 0;
    delete[] m_leaf;        m_leaf = 0;
    delete[] m_root;        m_root = 0;
    delete[] m_var;         m_var = 0;
    delete[] m_rangeVar;    m_rangeVar = 0;
    delete   m_funDict;     m_funDict = 0;
    delete   m_varDict;     m_varDict = 0;
    delete   m_propDict;    m_propDict = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Activates/deactivates all the functions in the m_fun[] array.
 *
 *  Called just before reconfiguration to put EqTree into a known state.
 */

void EqTree::activateFunctions( bool toggle )
{
    for ( int fid = 0;
          fid < m_funCount;
          fid++ )
    {
        m_fun[fid]->m_active = toggle;
                m_fun[fid]->setDirtyAll();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets all the EqTree's EqVar's displayUnits, displayDecimals, and
 *   m_stores to their default English units values.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::applyEnglishUnits( void )
{
    for ( int vid = 0;
          vid < m_varCount;
          vid++ )
    {
        if ( ! m_var[vid]->applyEnglishUnits() )
        // This code block should never be executed!
        {
            return( false );
        }
    }
    m_rxVarList->applyEnglishUnits();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets all the EqTree's EqVar's displayUnits, displayDecimals, and
 *   m_stores to their default metric values.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::applyMetricUnits( void )
{
    for ( int vid = 0;
          vid < m_varCount;
          vid++ )
    {
        if ( ! m_var[vid]->applyMetricUnits() )
        // This code block should never be executed!
        {
            return( false );
        }
    }
    m_rxVarList->applyMetricUnits();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets all the EqTree's EqVar's displayUnits, displayDecimals, and
 *   m_stores to their default native values.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::applyNativeUnits( void )
{
    for ( int vid = 0;
          vid < m_varCount;
          vid++ )
    {
        if ( ! m_var[vid]->applyNativeUnits() )
        // This code block should never be executed!
        {
            return( false );
        }
    }
    m_rxVarList->applyNativeUnits();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Applies the units used in the BehavePlus \a fileName to the
 *  current BehavePlusDocument.
 *
 *  If the units fileName is "English", "Metric", or "Native",
 *  then the appropriate built-in units set is applied.
 *
 *  Otherwise the passed UnitsSet fileName is opened and read,
 *  and each continuous variable record is validated and applied.
 *
 *  Note that FIRST all the EqTree variable's display units are reset to their
 *  Native units, THEN the UnitsSet file is applied.
 *  Therefore, any variables whose units are NOT specified in the UnitsSet
 *  file will use their native units by default.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::applyUnitsSet( const QString &fileName )
{
    // If English, Metric, or Native units, apply the built-in and return.
    if ( fileName == "English" )
    {
        return( applyEnglishUnits() );
    }
    else if ( fileName == "Metric" )
    {
        return( applyMetricUnits() );
    }
    else if ( fileName == "Native" )
    {
        return( applyNativeUnits() );
    }
    // Test the specified UnitsSet file.
    if ( ! isBehavePlusXmlDocument( fileName ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqTree:ApplyUnits:UnknownDocument", fileName );
        error( text );
        return( false );
    }
    // File is ok, so apply the native units before applying file's units
    applyNativeUnits();

    // Create a "units only" parser.
    if ( ! readXmlFile( fileName, true ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqTree:ApplyUnits:FileError", fileName );
        error( text );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Updates the specified root EqVar's current value by recursively
 *  calling its producer functions.
 *
 *  This is the "fast" version. See also EqTree::calculateVariableDebug().
 *
 *  This method uses upward propagation of the EqVar dirty flag so that
 *  calculation recursion can stop at the first clean variable.
 *  Dirty flags are cleaned as the recursion unwinds.
 */

void EqTree::calculateVariable( EqVar *varPtr, int level )
{
    // Get the current producer for this variable.
    // If NULL, then this is user input (leaf), so simply return.
    EqFun *funPtr;
    EqVar *subVarPtr;
    if ( ! ( funPtr = varPtr->activeProducerFunPtr() ) )
    {
        return;
    }
    // Otherwise this is a derived variable.
    // Check if any of the EqFun's inputs are dirty.
    int doCalc = 0;
    for ( int inputId = 0;
          inputId < funPtr->m_inputs;
          inputId++ )
    {
        if ( funPtr->m_dirty[inputId] )
        {
            doCalc++;
            subVarPtr = funPtr->m_input[inputId];
            calculateVariable( subVarPtr, level+1 );
            funPtr->m_dirty[inputId] = false;
        }
    }
	// In two fuel model mode, several variables are calculated inline by
	// weighting or selecting between two values,
	// so we must prevent them from being recalculated here
	if ( false )	// DISABLED to allow two-fuel processing when ROS not selected!
	{
	PropertyDict *prop = m_eqCalc->m_eqTree->m_propDict;
    if ( prop->boolean( "surfaceModuleActive" ) )
    {
		if ( prop->boolean( "surfaceConfFuelAreaWeighted" )
          || prop->boolean( "surfaceConfFuelHarmonicMean" )
          || prop->boolean( "surfaceConfFuel2Dimensional" ) )
		{
			if (   varPtr->m_name == "vSurfaceFireReactionInt"			// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireMaxDirFromUpslope"	// before 5.0.5
				|| varPtr->m_name == "vWindAdjFactor"					// before 5.0.5
				|| varPtr->m_name == "vWindSpeedAtMidflame"				// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireEffWindAtHead"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireEffWindAtVector"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireWindSpeedLimit"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireWindSpeedFlag"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireRosWindFlag"			// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireLengthToWidth"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireHeatPerUnitArea"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireLineIntAtHead"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireLineIntAtVector"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireFlameLengAtHead"		// before 5.0.5
				|| varPtr->m_name == "vSurfaceFireFlameLengAtVector"	// before 5.0.5
				|| varPtr->m_name == "vSurfaceFuelBedDepth"				// before 5.0.5
				//|| varPtr->m_name == "vSurfaceFireSpreadAtHead"			// added 5.0.5
				//|| varPtr->m_name == "vSurfaceFireSpreadAtVector"		// added 5.0.5
  			  )
			{
				doCalc = 0;
			}
		}
	}
	}
    // If doCalc, we have to recalculate this variable.
    if ( doCalc )
    {
        ( m_eqCalc->*funPtr->m_function )();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clears the isUserInput flag of every EqVar in the EqTree.
 *  Recall that the m_leaf[] list is generated from EqVar isUserInput flags.
 */

void EqTree::clearUserInput( void )
{
    for ( int vid = 0;
          vid < m_varCount;
          vid++ )
    {
        m_var[vid]->m_isMasked = false ;
        m_var[vid]->m_isUserInput = false;
        m_var[vid]->m_isConstant = false;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clears the isUserOutput flag of every EqVar in the EqTree.
 *  Recall that the root[] list is generated from EqVar isUserOutput flags.
 */

void EqTree::clearUserOutput( void )
{
    for ( int vid = 0;
          vid < m_varCount;
          vid++)
    {
        m_var[vid]->m_isUserOutput = false;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Generates a fresh m_leaf[] (required inputs) array
 *  containing pointers to all the EqVar's required by an input worksheet
 *  given the current membership of the m_root[] (requested) EqVar list.
 *
 *  \param release Application's release number.
 *
 *  \return Number of required inputs (including hidden).
 */

int EqTree::generateLeafList( int release )
{
    // First clear the leaf list.
    for ( int vid = 0;
          vid < m_varCount;
          vid++)
    {
        m_leaf[vid] = 0;
    }
    m_leafCount = 0;

    // Add each root (output) EqVar's inputs to the leaf list.
    for ( int rid = 0;
          rid < m_rootCount;
          rid++ )
    {
        generateLeafListNext( m_root[rid], release );
    }
    // Sort the m_leaf[] list by the desired input order.
    qsort( m_leaf, m_leafCount, sizeof(EqVar *), EqTree_InpOrderCompare );
    return( m_leafCount );
}

//------------------------------------------------------------------------------
/*! \brief Recursively descends from this EqVar until it finds a leaf
 *  (input variable), adds it to the m_leaf[] array (if it isn't already there),
 *  and then unwinds.
 *
 *  \param release Application's release number.
 *
 *  1999/10/12 If the variable isConstant, it is not included in the list.
 *  2000/04/16 Text variables ARE included in the list
 */

void EqTree::generateLeafListNext( EqVar *varPtr, int release )
{
    // Get the current producer EqFun for this variable.
    // If this is a user input (leaf), add it to the list.
    EqFun *funPtr;
    if ( ! ( funPtr = varPtr->activeProducerFunPtr() ) )
    {
        // Make sure this isn't a constant variable.
        if ( varPtr->m_isConstant )
        {
            return;
        }
        // Make sure this input isn't already in the m_leaf[] array
        for ( int lid = 0;
              lid < m_leafCount;
              lid++ )
        {
            if ( m_leaf[lid] == varPtr )
            {
                return;
            }
        }
        // Add this EqVar to the m_leaf[] array
        m_leaf[m_leafCount++] = varPtr;
        varPtr->m_isUserInput = true;
    }
    // Otherwise this is a produced variable,
    // so recurse into all the producer's input variables.
    else
    {
        for ( int iid = 0;
              iid < funPtr->m_inputs;
              iid++ )
        {
            if ( funPtr->m_input[iid]->isCurrent( release ) )
            {
                generateLeafListNext( funPtr->m_input[iid], release );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Generates a fresh m_root[] array for the EqTree
 *  from the current EqVar m_isUserOutput flag settings.
 *  Only called by EqTree::reconfigure().
 *
 *  \param release  Application's release number.
 *
 *  2000/04/16 Text variables ARE included in the list
 *
 *  \return Number of EqVars in the m_root[] array.
 */

int EqTree::generateRootList( int release )
{
    // First clear the m_root[] array.
    int vid;
    for ( vid = 0;
          vid < m_varCount;
          vid++)
    {
        m_root[vid] = 0;
    }
    m_rootCount = 0;

    // Check every variable in the list to see if it is an output.
    for ( vid = 0;
          vid < m_varCount;
          vid++ )
    {
        if ( m_var[vid]->m_isUserOutput
          && m_var[vid]->isCurrent( release ) )
        {
            m_root[m_rootCount++] = m_var[vid];
        }
    }
    // Sort the output m_root[] list by the desired output order.
    qsort( m_root, m_rootCount, sizeof(EqVar *), EqTree_OutOrderCompare );
    return( m_rootCount );
}

//------------------------------------------------------------------------------
/*! \brief Gets the result for the specified row, column, and output variable.
 *
 *  Functions that access or display results should call this function,
 *  since future optimizations may store results in unexpected order.
 *
 *  \param row Row index (base 0).
 *  \param col Column index (base 0).
 *  \param var Variable index (base 0).
 *
 *  \return Value from the m_tableVal[] array.
 */

double EqTree::getResult( int row, int col, int var ) const
{
    double value = 0.;
    if ( m_tableVal && m_tableCells
      && row >= 0 && row < m_tableRows
      && col >= 0 && col < m_tableCols
      && var >= 0 && var < m_tableVars )
    {
        int id = var + col * m_tableVars + row * m_tableCols * m_tableVars;
        value = m_tableVal[ id ];
    }
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Gets the address of the EqVar with \a name.
 *
 *  \param name EqFun name.
 *
 *  \return Address of the EqVar.
 */

EqVar *EqTree::getVarPtr( const QString &name ) const
{
    EqVar *varPtr = m_varDict->find( name );
    if ( ! varPtr )
    // This code block should never be executed!
    {
		const char* aname = name.ascii();
        QString text("");
        translate( text, "EqTree:VariableNameUnknown", name );
        bomb( text );
    }
    return( varPtr );
}

//------------------------------------------------------------------------------
/*! \brief Initializes the individual EqVar consumer/producer and EqFun
 *  input/output cross-reference pointer arrays.
 */

void EqTree::init( void )
{
    int fun, var, con, pro;
    EqFun *funPtr, *f;
    EqVar *varPtr, *v;
    QString text("");

    // Initialize each function's variables
    for ( fun = 0;
          fun < m_funCount;
          fun++ )
    {
        f = m_eqApp->m_fun[fun];
        funPtr = m_fun[fun];
        // Initialize all the function's input variables
        for ( var = 0;
              var < funPtr->m_inputs;
              var++ )
        {
            // Get the name of this input variable
            v = f->m_input[var];
            // Find its local EqTree address
            if ( ! ( varPtr = m_varDict->find( v->m_name ) ) )
            // This code block should never be executed!
            {
                translate( text, "EqTree:VariableNotFound",
                    funPtr->m_name,
                    "input",
                    QString( "%1" ).arg( var ),
                    v->m_name );
                bomb( text );
            }
            // Set the input EqVar's address in the EqFun's m_input[] array
            funPtr->m_input[var] = varPtr;
            // Now add the EqFun's address to the end of this EqVar's
            // m_consumer[] array
            for ( con = 0;
                  con < varPtr->m_consumers;
                  con++ )
            {
                if ( ! varPtr->m_consumer[con] )
                {
                    varPtr->m_consumer[con] = funPtr;
                    break;
                }
            }
            // Check for overflow!
            if ( con >= varPtr->m_consumers )
            // This code block should never be executed!
            {
                translate( text, "EqTree:VariableOverflow",
                    funPtr->m_name,
                    "input",
                    QString( "%1" ).arg( var ),
                    varPtr->m_name,
                    "m_consumer",
                    QString( "%1" ).arg( varPtr->m_consumers ) );
                bomb( text );
            }
        }
        // Initialize all the function's output variables
        for ( var = 0;
              var < funPtr->m_outputs;
              var++ )
        {
            // Get the name of this output variable
            v = f->m_output[var];
            // Find its local EqTree address
            if ( ! ( varPtr = m_varDict->find( v->m_name ) ) )
            // This code block should never be executed!
            {
                translate( text, "EqTree:VariableNotFound",
                    funPtr->m_name,
                    "output",
                    QString( "%1" ).arg( var ),
                    v->m_name );
                bomb( text );
            }
            // Set the output EqVar's address in the EqFun's m_output[] array
            funPtr->m_output[var] = varPtr;
            // Now add the EqFun's address to the end of this EqVar's
            // m_producer[] array
            for ( pro = 0;
                  pro < varPtr->m_producers;
                  pro++ )
            {
                if ( ! varPtr->m_producer[pro] )
                {
                    varPtr->m_producer[pro] = funPtr;
                    break;
                }
            }
            // Check for overflow!
            if ( pro >= varPtr->m_producers )
            // This code block should never be executed!
            {
                translate( text, "EqTree:VariableOverflow",
                    funPtr->m_name,
                    "output",
                    QString( "%1" ).arg( var ),
                    varPtr->m_name,
                    "m_producer",
                    QString( "%1" ).arg( varPtr->m_producers ) );
                bomb( text );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief qsort() comparison function used to sort variables by their
 *  input order.  Called only by qsort() on behalf of generateLeafList()
 *  and by printPatsTable().
 *
 *  \return  -1, 0, or 1 as required by qsort().
 */

int EqTree_InpOrderCompare( const void *s1, const void *s2 )
{
    EqVar **v1 = (EqVar **) s1;
    EqVar **v2 = (EqVar **) s2;
    return ( strcmp( (*v1)->m_inpOrder.latin1(), (*v2)->m_inpOrder.latin1() ) );
}

//------------------------------------------------------------------------------
/*! \brief qsort() comparison function used to sort variables by their
 *  output order.  Called only by qsort() on behalf of generateRootList()
 *  and by printPatsTable().
 *
 *  \return  -1, 0, or 1 as required by qsort().
 */

int EqTree_OutOrderCompare( const void *s1, const void *s2 )
{
    EqVar **v1 = (EqVar **) s1;
    EqVar **v2 = (EqVar **) s2;
    return ( strcmp( (*v1)->m_outOrder.latin1(), (*v2)->m_outOrder.latin1() ) );
}

//------------------------------------------------------------------------------
/*! \brief Closes the temporary EqTree trace file if it is open.
 *
 *  Usually called only by EqTree::runTable().
 *  Note that the m_traceFile member is not cleared since other functions
 *  need to get its name to open the file.
 */

void EqTree::traceFileClose( void )
{
    // Close the file if it is open.
    if ( m_traceFptr )
    {
        fclose( m_traceFptr );
        m_traceFptr = m_eqCalc->m_log = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a new temporary EqTree log file.
 *
 *  Usually called only by EqTree::runTable().
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::traceFileInit( const QString &fileName )
{
    // Close the file if it is open.
    traceFileClose();

    // Get the file name and try to open it.
    m_traceFile = fileName;
    if ( ! ( m_traceFptr = fopen( m_traceFile.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        QString msg("");
        translate( msg, "EqTree:UnopenedTraceFile", m_traceFile );
        error( msg );
        m_traceFile = "";
        return( false );
    }
    // Let EqCalc know about the file stream
    m_eqCalc->m_log = m_traceFptr;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Deletes the temporary run log file.
 */

void EqTree::traceFileRemove( void )
{
    traceFileClose();
    remove( m_traceFile.latin1() );
    m_traceFile = "";
    return;
}

//------------------------------------------------------------------------------
/*! \brief Reads a BehavePlus XML document file into the EqTree.
 *
 *  Once the file is read, the EqTree should be reconfigured
 *  so we know the number and order of inputs.
 *
 *  \param fileName Name of the BehavePlus XML document file to read.
 *  \param unitsOnly If TRUE, only the units and decimal values of continuous
 *                      variables are read and updated in the EqTree.
 *                      If FALSE, then the entire file (properties and all
 *                      variables) are read and stored in the EqTree.
 *
 *  Normally only EqTree::applyUnitsSet() calls this with unitsOnly==TRUE.
 *
 *  \b NOTE that "value", "units", and "decimals" in the file refer to the
 *  \b display values, units, and decimals, not \b native values, units and
 *  decimals!
 *
 *  \retval TRUE if the file is opened, read, and stored without error.
 *  \retval FALSE otherwise.
 */

bool EqTree::readXmlFile( const QString &fileName, bool unitsOnly )
{
    EqTreeParser *handler = new EqTreeParser( this, fileName, unitsOnly );
    checkmem( __FILE__, __LINE__, handler, "EqTreeParser handler", 1 );
    QFile xmlFile( fileName );
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( handler );
    reader.setErrorHandler( handler );
    bool result = reader.parse( &source );
    delete handler;
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Dynamically reconfigures the EqTree based upon the current
 *  option and output configuration variables.
 *
 *  \param release Application's release number.
 *
 *  This should called whenever any of the *Option* or *Output* variables
 *  have been changed, thereby changing the EqTree configuration state.
 */

void EqTree::reconfigure( int release )
{
    // Clear all the variable input/output flags and de-activate all functions
    clearUserInput();
    clearUserOutput();
    activateFunctions( false );

    // Reconfigure the tree
    m_eqCalc->reconfigure( release );

    // Generate the root (output) and leaf (input) lists.
    generateRootList( release );
    generateLeafList( release );

    // If I ever need to figure out the tree dependencies, here's the spot!
    // FILE *fptr = fopen( "junk", "w" );
    // printSubtree( vIgnitionFirebrandProb, stdout );
    // fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the run's "range case", as defined below.
 *
 *  Should be called only after EqTree::validateInputs(),
 *  which sets up the m_rangeVars and m_rangeVar[] data elements.
 *  If m_rangeVars is:
 *      \arg 0, neither m_rangeVra[0] nor m_rangeVar[1] are multi-valued,
 *      \arg 1, only m_rangeVar[0] is multi-valued, or
 *      \arg 2, both m_rangeVar[0] and m_rangeVar[1] are multi-valued.
 *
 *  \return The range variable case number:
 *  \retval 1 ZERO continuous and ZERO discrete range variables.
 *  \retval 2 ONE  continuous and ZERO discrete range variable.
 *  \retval 3 ZERO continuous and ONE  discrete range variable.
 *  \retval 4 TWO  continuous and ZERO discrete range variables.
 *  \retval 5 ZERO continuous and TWO  discrete range variables.
 *  \retval 6 ONE  continuous and ONE  discrete range variable.
 */

int EqTree::rangeCase( void )
{
    QString text("");
    // Block 1: 0 range variables
    if ( m_rangeVars == 0 )
    {
        // Case 1: 0 continuous and 0 discrete range vars
        return( m_rangeCase = 1 );
    }

    // Block 2: 1 range variable
    if ( m_rangeVars == 1 )
    {
        // Case 2: 1 continuous and 0 discrete range vars
        if ( m_rangeVar[0]->isContinuous() )
        {
            return( m_rangeCase = 2 );
        }
        // Case 3: 0 continuous and 1 discrete range vars
        else if ( m_rangeVar[0]->isDiscrete() )
        {
            return( m_rangeCase = 3 );
        }
        // This code block should never be executed!
        translate( text, "EqTree:RangeCaseCheckpoint", "1" );
        bomb( text );
    }

    // Block 3: 2 range variables
    if ( m_rangeVars == 2 )
    {
        if ( m_rangeVar[0]->isDiscrete() )
        {
            // Case 5: 0 continuous & 2 discrete range vars
            if ( m_rangeVar[1]->isDiscrete() )
            {
                return( m_rangeCase = 5 );
            }
            // Case 6: 1 continuous & 1 discrete range vars
            else if ( m_rangeVar[1]->isContinuous() )
            {
                return( m_rangeCase = 6 );
            }
            // This code block should never be executed!
            translate( text, "EqTree:RangeCaseCheckpoint", "2" );
            bomb( text );
        }
        else if ( m_rangeVar[0]->isContinuous() )
        {
            // Case 6: 1 continuous and 1 discrete range variables.
            if ( m_rangeVar[1]->isDiscrete() )
            {
                return( m_rangeCase = 6 );
            }
            // Case 4: 2 continuous & 0 discrete range vars
            else if ( m_rangeVar[1]->isContinuous() )
            {
                return( m_rangeCase = 4);
            }
            // This code block should never be executed!
            translate( text, "EqTree:RangeCaseCheckpoint", "3" );
            bomb( text );
        }
        // This code block should never be executed!
        translate( text, "EqTree:RangeCaseCheckpoint", "4" );
        bomb( text );
    }
    // This code block should never be executed!
    translate( text, "EqTree:RangeCaseCheckpoint", "5" );
    bomb( text );
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Closes the temporary EqTree result file if it is open.
 *
 *  Usually called only by EqTree::runTable().
 *  Note that the m_resultFile member is not cleared since other functions
 *  need to get its name to open the file.
 */

void EqTree::resultFileClose( void )
{
    // Close the file if it is open.
    if ( m_resultFptr )
    {
        fclose( m_resultFptr );
        m_resultFptr = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a new temporary EqTree result file.
 *
 *  Usually called only by EqTree::runTable().
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::resultFileInit( const QString &fileName )
{
    // Close the file if it is open.
    resultFileClose();

    // Get the file name and try to open it.
    m_resultFile = fileName;
    if ( ! ( m_resultFptr = fopen( m_resultFile.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        QString msg("");
        translate( msg, "EqTree:UnopenedResultFile", m_resultFile );
        error( msg );
        m_resultFile = "";
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Deletes the temporary run result file.
 */

void EqTree::resultFileRemove( void )
{
    resultFileClose();
    remove( m_resultFile.latin1() );
    m_resultFile = "";
    return;
}

//------------------------------------------------------------------------------
/*! \brief Validates the EqTree values and runs the current configuration,
 *  storing values in the m_tableVal[] array.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::run( const QString &traceFile, const QString &resultFile )
{
    // Inputs must be complete and valid.
    int badLeaf, badPos, badLen, badRx;
    if ( validateInputs( &badLeaf, &badPos, &badLen ) < 0 )
    {
        return( false );
    }
    if ( validateRxInputs( &badRx ) < 0 )
    {
        return( false );
    }
    // Determine the range case.
    rangeCase();

    // Calculate a table
    runTable( traceFile, resultFile );
    printResults( stdout );

    // Cleanup
    runClean();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Frees all the memory allocated for a specific run.
 *
 *  The deleted memory is set in
 *  \arg EqTree::runInitColsFromStore()
 *  \arg EqTree::runInitRowsFromStore()
 */

void EqTree::runClean( void )
{
    delete[] m_tableRow;    m_tableRow = 0;
    delete[] m_tableCol;    m_tableCol = 0;
    delete[] m_tableVal;    m_tableVal = 0;
    delete[] m_tableInRx;   m_tableInRx = 0;
    delete[] m_tableVar;    m_tableVar = 0;
    m_tableVars = m_tableCols = m_tableRows = m_tableCells = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Performs the necessary initialization determine all the outputs for
 *  the current set of tables.
 *
 *  \param graphTable   If FALSE, only results for the requested row and
 *                      column values are produced.
 *                      If TRUE, then the row or column range is used to
 *                      select n equi-distant computation points suitable
 *                      for generating graph results.
 *
 *  The current set of tables is determined by the last EqTree::rangeCase()
 *  call since it sets the range variables.
 *  Sets up the m_tableRow[], m_tableCol[], and m_tableVar[] arrays.
 *
 *  \return TRUE on success, FALSE if allocation failure.
 */

bool EqTree::runInit( bool graphTable )
{
    // Determine and store all the table's row values
    runClean();
    if ( graphTable )
    {
        runInitRowsFromRange();
    }
    else
    {
        runInitRowsFromStore();
    }
    // Determine and store all the table's column values
    runInitColsFromStore();
    // Initialize output variable info
    if ( ! runInitTableVars() )
    {
        runClean();
        return( false );
    }
    // Create a single array to hold all the table's result values
    m_tableCells = m_tableRows * m_tableCols * m_tableVars;
    m_tableVal = new double[ m_tableCells ];
    checkmem( __FILE__, __LINE__, m_tableVal, "double m_tableVal", m_tableCells );

    // Create a single array to hold all the table's shading results
    m_tableInRx = new bool[ m_tableCells ];
    checkmem( __FILE__, __LINE__, m_tableInRx, "bool m_tableInRx", m_tableCells );
    for ( int i=0; i<m_tableCells; i++ )
    {
        m_tableInRx[i] = false;
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets up the m_tableCol[] array with all the column values parsed
 *  from the column variable's store.
 *
 *  The column variable is pointed to by m_rangeVar[1],
 *  which is set by the most recent call to EqTree::rangeCase()
 *  and must always point to a valid EqVar, even if it is not multi-valued.
 *
 *  Called only by EqTree::runInit().
 */

void EqTree::runInitColsFromStore( void )
{
    // Remove any existing row value array
    delete[] m_tableCol;    m_tableCol = 0;
    m_tableCols = 1;

    // If there is no column variable then we're done here
    if ( ! m_rangeVar[1] )
    {
        return;
    }
    // Create an array to hold the table's output column values
    // or the graph's z-axis values
    EqVar *colVar = m_rangeVar[1];
    m_tableCols = colVar->m_tokens;
    m_tableCol = new double[ m_tableCols ];
    checkmem( __FILE__, __LINE__, m_tableCol, "double m_tableCol", m_tableCols );

    // Determine and store all the table's column values from its store
    Parser parser( " \t,\"", "", "" );
    parser.parse( colVar->m_store );
    QString token;
    int col = 0;
    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( colVar->isDiscrete() )
        {
			// Check if this token is a fuel model sort number
 			token = tokenConverter( colVar, token );
			m_tableCol[col++] = 0.5 +
                (double) colVar->m_itemList->itemIdWithName( token );
        }
        else if ( colVar->isContinuous() )
        {
            m_tableCol[col++] = token.toDouble();
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets up the m_tableRow[] array with all the row values calculated
 *  from the row variable's m_store minimum and maximum value and from the
 *  current "graphLinepoints" property.
 *
 *  The row variable is pointed to by m_rangeVar[0],
 *  which is set by the most recent call to EqTree::rangeCase()
 *  and must always point to a valid EqVar, even if it is not multi-valued.
 *
 *  Called only by EqTree::runInit().
 */

void EqTree::runInitRowsFromRange( void )
{
    // Remove any existing row value array
    delete[] m_tableRow;    m_tableRow = 0;

    // Get a pointer to the row variable and number of graph line points
    EqVar *rowVar = m_rangeVar[0];
    m_tableRows = m_propDict->integer( "graphLinePoints" );

    // Create an array to hold the graph's x-axis values
    m_tableRow = new double[ m_tableRows ];
    checkmem( __FILE__, __LINE__, m_tableRow, "double m_tableRow", m_tableRows );

    // Determine the x-axis value step size
    double xMin, xMax, xStep;
    strMinMax( rowVar->m_store, &xMin, &xMax );
    xStep = (xMax - xMin) / ( m_tableRows - 1 );

    // Fill the row array
    for ( int row = 0;
          row < m_tableRows;
          row++ )
    {
        m_tableRow[row] = xMin + row * xStep;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets up the m_tableRow[] array with all the row values parsed
 *  from the row variable's store.
 *
 *  The row variable is pointed to by m_rangeVar[0],
 *  which is set by the most recent call to EqTree::rangeCase()
 *  and must always point to a valid EqVar, even if it is not multi-valued.
 *
 *  Called only by EqTree::runInit().
 */

void EqTree::runInitRowsFromStore( void )
{
    // Remove any existing row value array
    delete[] m_tableRow;  m_tableRow = 0;
    m_tableRows = 1;

    // If there is no row variable, then we're done here
    if ( ! m_rangeVar[0] )
    {
        return;
    }
    // Create an array to hold the table's output row values
    // or the graph's x-axis values
    EqVar *rowVar = m_rangeVar[0];
    m_tableRows = rowVar->m_tokens;
    m_tableRow = new double[ m_tableRows ];
    checkmem( __FILE__, __LINE__, m_tableRow, "double m_tableRow", m_tableRows );

    // Determine and store all the table's row values from its store
    Parser parser( " \t,\"", "", "" );
    parser.parse( rowVar->m_store );
    QString token;
    int row = 0;
    for ( int i = 0;
          i < parser.tokens();
          i++ )
    {
        token = parser.token( i );
        if ( rowVar->isDiscrete() )
        {
			// Check if this is a fuel model sort number
			token = tokenConverter( rowVar, token );
			const char* tokenAsc = token.ascii();	// for debug purposes only
            m_tableRow[row++] = 0.5 +
                (double) rowVar->m_itemList->itemIdWithName( token );
        }
        else if ( rowVar->isContinuous() )
        {
            m_tableRow[row++] = token.toDouble();
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets up the m_tableVar[] array with pointers to all the output
 *  variables.
 *
 *  The number of output variables may be equal to or less than the number
 *  of root variables, since text root and diagram variables are not output
 *  variables.
 *
 *  Called only by EqTree::runInit().
 *
 *   \return TRUE on success, FALSE if no input variables.
 */

bool EqTree::runInitTableVars( void )
{
    // Remove any existing row value array
    delete[] m_tableVar;    m_tableVar = 0;
    m_tableVars = 0;

    // Determine number of root variables to be displayed
    int rid;
    for ( rid = 0;
          rid < m_rootCount;
          rid++ )
    {
        if ( ! m_root[rid]->isText() )
          //&& ! m_root[rid]->isDiagram() )
        {
            m_tableVars++;
        }
    }
    // Let user know if there are no outputs to calculate
    if ( ! m_tableVars )
    {
        QString msg("");
        translate( msg, "EqTree:NoOutputs" );
        error( msg );
        return( false );
    }
    // Create an array to hold the table's output variable pointers
    m_tableVar = new EqVar *[ m_tableVars ];
    checkmem( __FILE__, __LINE__, m_tableVar, "EqVar *m_tableVars", m_tableVars );

    // Determine and store all the output variable pointers
    int vid = 0;
    for ( rid = 0;
          rid < m_rootCount;
          rid++ )
    {
        if ( ! m_root[rid]->isText() )
          //&& ! m_root[rid]->isDiagram() )
        {
            m_tableVar[vid++] = m_root[rid];
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Creates a table of results from the current input values and range
 *  variables.
 *
 *  \param traceFile    Name of the EqCalc processing log file.
 *                      If NULL or empty, no log file is written.
 *  \param graphTable   If FALSE, only results for the requested row and
 *                      column values are produced.
 *                      If TRUE, then the row or column range is used to
 *                      select n equi-distant computation points suitable
 *                      for generating graph results.
 *
 *  Called only by EqTree::run() or BpDocument::runWorksheet().
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::runTable( const QString &traceFile, const QString &resultFile,
        bool graphTable )
{
    // Set up the supporting dynamic memory
    if ( ! runInit( graphTable ) )
    {
        return( false );
    }
    // We're gonna need these!
    EqVar *rowVar = m_rangeVar[0];
    EqVar *colVar = m_rangeVar[1];
    EqVar *outVar = 0;
    int row, col, cell, vid, iid, var, step;

    // Attempt to open a new copy of the trace file.
    if ( ! traceFile.isNull()
     && ! traceFile.isEmpty() )
    {
        if ( ! traceFileInit( traceFile ) )
        {
            runClean();
            return( false );
        }
        // Log the table header
        fprintf( m_traceFptr, "begin table %d %s %d %s %d\n",
            m_tableRows,
            ( rowVar ? rowVar->m_name.latin1() : "none" ),
            m_tableCols,
            ( colVar ? colVar->m_name.latin1() : "none" ),
            m_tableVars );
        // Log the output variable names
        for ( vid = 0;
              vid < m_tableVars;
              vid++ )
        {
            outVar = m_tableVar[ vid ];
            fprintf( m_traceFptr, "  output %d %s\n",
                vid, outVar->m_name.latin1() );
        }
    }

    // Attempt to open a new copy of the result file
    if ( ! graphTable
      && ! resultFile.isNull()
      && ! resultFile.isEmpty() )
    {
        if ( ! resultFileInit( resultFile ) )
        {
            traceFileClose();
            return( false );
        }
        fprintf( m_resultFptr,
            "ROWS %d COLS %d VARS %d\n",
            m_tableRows,
            m_tableCols,
            m_varCount );
    }

    // Set up the progress dialog.
    QString caption(""), button("");
    translate( caption, "EqTree:RunTable:Progress:Caption",
        QString( "%1" ).arg( m_tableCells ),
        QString( "%1" ).arg( m_tableRows ),
        QString( "%1" ).arg( m_tableCols ),
        QString( "%1" ).arg( m_tableVars ) );
    translate( button, "EqTree:RunTable:Progress:Button" );
    QProgressDialog *progress = new QProgressDialog( caption, button,
        m_tableCells );
    Q_CHECK_PTR( progress );
    progress->setMinimumDuration( 0 );
    progress->setProgress( 0 );

    // Make an Equation Tree run for every table cell
    // Loop for each table row or graph x-axis variable.
    RxVar *rxVar;
    for ( step = 0, var = 0, row = 0, cell = 0;
          row < m_tableRows;
          row++ )
    {
        // Set this row's input value.
        if ( rowVar )
        {
            if ( rowVar->isDiscrete() )
            {
                iid = (int) m_tableRow[ row ];
                // Get the item name for this id and make it the current item.
                rowVar->setItemName( rowVar->getItemName( iid ) );
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr,
                        "  begin row %d discrete %s \"%s\" %d \"%s\"\n",
                        row,
                        rowVar->m_name.latin1(),
                        rowVar->m_label->latin1(),
                        iid,
                        rowVar->getItemName( iid ).latin1() );
                }
                if ( ! graphTable
                  && m_resultFptr )
                {
                    fprintf( m_resultFptr,
                        "ROW %d %s disc %s\n",
                        row+1,
                        rowVar->m_name.latin1(),
                        rowVar->getItemName( iid ).latin1() );
                }
            }
            else if ( rowVar->isContinuous() )
            {
                rowVar->setDisplayValue( m_tableRow[ row ] );
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr,
                        "  begin row %d continuous %s \"%s\" %g \"%s\"\n",
                        row,
                        rowVar->m_name.latin1(),
                        rowVar->m_label->latin1(),
                        rowVar->m_nativeValue,
                        rowVar->m_nativeUnits.latin1() );
                }
                if ( ! graphTable
                  && m_resultFptr )
                {
                    fprintf( m_resultFptr,
                        "ROW %d %s cont %g %s\n",
                        row+1,
                        rowVar->m_name.latin1(),
                        rowVar->m_displayValue,
                        rowVar->m_displayUnits.latin1() );
                }
            }
            else
            {
                if ( ! graphTable
                  && m_resultFptr )
                {
                    fprintf( m_resultFptr,
                        "ROW %d %s othr\n",
                        row+1,
                        rowVar->m_name.latin1() );
                }
            }
        }
        else // if ( ! rowVar )
        {
            if ( m_traceFptr )
            {
                fprintf( m_traceFptr, "  begin row %d none\n", row );
            }
        }
        // Loop for each table column or graph z-axis variable.
        for ( col = 0;
              col < m_tableCols;
              col++, cell++ )
        {
            if ( colVar )
            {
                // Set this column's input value.
                if ( colVar->isDiscrete() )
                {
                    iid = (int) m_tableCol[ col ];
                    // Get the item name for this id and make it the current item.
                    colVar->setItemName( colVar->getItemName( iid ) );
                    if ( m_traceFptr )
                    {
                        fprintf( m_traceFptr,
                            "    begin column %d discrete %s \"%s\" %d \"%s\"\n",
                            col,
                            colVar->m_name.latin1(),
                            colVar->m_label->latin1(),
                            iid,
                            colVar->getItemName( iid ).latin1() );
                    }
                    if ( ! graphTable
                      && m_resultFptr )
                    {
                        fprintf( m_resultFptr,
                            "COL %d %s disc %s\n",
                            col+1,
                            colVar->m_name.latin1(),
                            colVar->getItemName( iid ).latin1() );
                    }
                }
                else if ( colVar->isContinuous() )
                {
                    colVar->setDisplayValue( m_tableCol[ col ] );
                    if ( m_traceFptr )
                    {
                        fprintf( m_traceFptr,
                            "    begin column %d continuous %s \"%s\" %g \"%s\"\n",
                            col,
                            colVar->m_name.latin1(),
                            colVar->m_label->latin1(),
                            colVar->m_nativeValue,
                            colVar->m_nativeUnits.latin1() );
                    }
                    if ( ! graphTable
                      && m_resultFptr )
                    {
                        fprintf( m_resultFptr,
                            "COL %d %s cont %g %s\n",
                            col+1,
                            colVar->m_name.latin1(),
                            colVar->m_displayValue,
                            colVar->m_displayUnits.latin1() );
                    }
                }
                else
                {
                    if ( ! graphTable
                      && m_resultFptr )
                    {
                        fprintf( m_resultFptr,
                            "COL %d %s othr\n",
                            col+1,
                            colVar->m_name.latin1() );
                    }
                }
            }
            else // if ( ! colVar )
            {
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr, "    begin column %d none\n", col );
                }
            }
            // Loop for each table output or graph y-axis variable.
            for ( vid = 0;
                  vid < m_tableVars;
                  vid++ )
            {
                // Set the output variable pointer.
                outVar = m_tableVar[ vid ];

                // Log start of this loop.
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr,
                        "      begin output %d %s \"%s\"\n",
                        vid,
                        outVar->m_name.latin1(),
                        outVar->m_label->latin1() );
                }
                // Calculate the output for this row/col combination.
                calculateVariable( outVar, 0 );
                //calculateVariableDebug( outVar, 0 );

                // Store the output value.
                if ( outVar->isDiscrete() )
                {
                    m_tableVal[ var++ ] = 0.5 + (double)
                        outVar->m_itemList->itemIdWithName(
                            outVar->activeItemName() );
                }
                else if ( outVar->isContinuous() )
                {
                    m_tableVal[ var++ ] = outVar->m_displayValue;
                }

                // Log end of this loop.
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr, "      end output %d %s\n",
                        vid, outVar->m_name.latin1() );
                }
                // Update progress dialog.
                progress->setProgress( ++step );
                qApp->processEvents();
                if ( progress->wasCancelled() )
                {
                    delete progress;    progress = 0;
                    resultFileClose();
                    traceFileClose();
                    return( false );
                }
            } // Next table output or graph y-axis variable.

            // Determine if results are within prescription
            m_tableInRx[cell] = true;
            for ( rxVar = m_rxVarList->first();
                  rxVar;
                  rxVar = m_rxVarList->next() )
            {
                if ( rxVar->m_isActive
                  && rxVar->m_varPtr->m_isUserOutput )
                {
                    if ( ! rxVar->inRange() )
                    {
                        m_tableInRx[ cell ] = false;
                        break;
                    }
                }
            }
//fprintf( stderr, "Cell %d is %s\n",
//cell, m_tableInRx[cell] ? "INSIDE" : "OUTSIDE" );

            // Dump all variables
            if ( ! graphTable
              && m_resultFptr )
            {
                for ( vid = 0;
                      vid < m_varCount;
                      vid++ )
                {
                    // Set the output variable pointer.
                    outVar = m_var[ vid ];
                    // Dump the variable's current value
                    if ( outVar->isDiscrete() )
                    {
                        iid = outVar->m_itemList->itemIdWithName(
                            outVar->activeItemName() );
                        fprintf( m_resultFptr,
                            "CELL %d %d %s disc %s\n",
                            row+1,
                            col+1,
                            outVar->m_name.latin1(),
                            outVar->getItemName( iid ).latin1() );
                    }
                    else if ( outVar->isContinuous() )
                    {
                        fprintf( m_resultFptr,
                            "CELL %d %d %s cont %g %s\n",
                            row+1,
                            col+1,
                            outVar->m_name.latin1(),
                            outVar->m_displayValue,
                            outVar->m_displayUnits.latin1() );
                    }
                    else if ( outVar->isText() )
                    {
                        fprintf( m_resultFptr,
                            "CELL %d %d %s text %s\n",
                            row+1,
                            col+1,
                            outVar->m_name.latin1(),
                            outVar->m_store.latin1() );
                    }
                    else
                    {
                        fprintf( m_resultFptr,
                            "CELL %d %d %s othr\n",
                            row+1,
                            col+1,
                            outVar->m_name.latin1() );
                    }
                }
            }

            // Log end of this loop.
            if ( colVar )
            {
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr, "    end column %d %s\n",
                        col, colVar->m_name.latin1() );
                }
            }
            else // if ( ! colVar )
            {
                if ( m_traceFptr )
                {
                    fprintf( m_traceFptr, "    end column %d none\n", col );
                }
            }
        } // Next table column or graph z-axis variable.
        // Log end of this loop.
        if ( rowVar )
        {
            if ( m_traceFptr )
            {
                fprintf( m_traceFptr, "    end row %d %s\n",
                    row, rowVar->m_name.latin1() );
            }
        }
        else // if ( ! rowVar )
        {
            if ( m_traceFptr )
            {
                fprintf( m_traceFptr, "    end row %d none\n", row );
            }
        }
    } // Next table row or graph x-axis variable.
    // Log the table footer
    if ( m_traceFptr )
    {
        fprintf( m_traceFptr, "end table %d %d %d\n",
            m_tableRows, m_tableCols, m_tableVars );
    }
    // Clean up and return.
    resultFileClose();
    traceFileClose();

    // Clean up and return.
    delete progress;    progress = 0;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqFun function address.
 *  Called only by EqCalc::EqCalc() constructor.
 *
 *  \param name EqFun name.
 *  \param address EqFun calculation function address.
 *
 *  \return Pointer to the EqFun.
 */

EqFun *EqTree::setEqFunAddress( const QString &name, PFV address )
{
    EqFun *funPtr = m_funDict->find( name );
    if ( ! funPtr )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqTree:UnknownFunctionName", name );
        bomb( text );
    }
    funPtr->m_function = address;
    return( funPtr );
}

//------------------------------------------------------------------------------
/*! \brief Resets all the EqTree's output text to the requested language.
 *
 *  The following are affected:
 *      EqVar::m_label
 *      EqVar::m_desc
 *      EqVar::m_hdr0
 *      EqVar::m_hdr1
 *
 *  Also prints error messages if there is no language defined for any of
 *  the above elements.
 */

void EqTree::setLanguage( const QString &lang )
{
    int id;
    QString key(""), text("");
    m_lang = lang;

    // Update all the variable labels, descriptions, and headers
    for ( id = 0;
          id < m_varCount;
          id++ )
    {
        // Build and locate the label key
        key = QString( m_var[id]->m_name + ":Label:" + lang );
        if ( ! ( m_var[id]->m_label = appTranslatorFind( key ) ) )
        // This code should never be executed!
        {
            translate( text, "EqTree:SetLanguage:NoKey", "label", key );
            error( text );
        }
        // Build and locate the desc key
        key = QString( m_var[id]->m_name + ":Desc:" + lang );
        if ( ! ( m_var[id]->m_desc = appTranslatorFind( key ) ) )
        // This code should never be executed!
        {
            translate( text, "EqTree:SetLanguage:NoKey", "description", key );
            error( text );
        }
        if ( ! m_var[id]->isText() )
        {
            // Build and locate the hdr0 key
            key = QString( m_var[id]->m_name + ":Hdr0:" + lang );
            if ( ! ( m_var[id]->m_hdr0 = appTranslatorFind( key ) ) )
            // This code should never be executed!
            {
                translate( text, "EqTree:SetLanguage:NoKey", "header 0", key );
                error( text );
            }
            // Build and locate the hdr1 key
            key = QString( m_var[id]->m_name + ":Hdr1:" + lang );
            if ( ! ( m_var[id]->m_hdr1 = appTranslatorFind( key ) ) )
            // This code should never be executed!
            {
                translate( text, "EqTree:SetLanguage:NoKey", "header 1", key );
                error( text );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Special hack called only by EqCalc::reconfigure() to swap the
 *  vWindSpeedAt10M, vWindSpeedAt20Ft, anf vWindSpeedAtMidflame variable
 *  labels so they optionally have "(upslope)" appended to them.
 *
 *  \param stuff The garbage to append to the variable name to find the
 *                  alternate label.  In our case, this should be either
 *                  blank ("") or "Upslope".
 */

void EqTree::setLabel( EqVar *varPtr, const QString &stuff )
{
    // Build and locate the label key
    QString key = QString( varPtr->m_name + stuff + ":Label:" + m_lang );
    if ( ! ( varPtr->m_label = appTranslatorFind( key ) ) )
    // This code should never be executed!
    {
        QString msg("");
        translate( msg, "EqTree:SetLabel:NoKey", "label", key );
        error( msg );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the result for the specified row, column, and output variable.
 *
 *  Functions that update results should call this function,
 *  since future optimizations may store results in unexpected order.
 *
 *  \param row      Row index (base 0).
 *  \param col      Column index (base 0).
 *  \param var      Variable index (base 0).
 *  \param value    Value to insert into the table.
 *
 *  \return Value from the m_tableVal[] array.
 */

double EqTree::setResult( int row, int col, int var, double value )
{
    if ( m_tableVal && m_tableCells
      && row >= 0 && row < m_tableRows
      && col >= 0 && col < m_tableCols
      && var >= 0 && var < m_tableVars )
    {
        int id = var + col * m_tableVars + row * m_tableCols * m_tableVars;
        m_tableVal[ id ] = value;
    }
    return( value );
}

//------------------------------------------------------------------------------
/*!	\brief If this is a fuel model variable and the token is its sort number,
	returns the fuel model name as the token.

	Called by runInitRowsFromStore() and runInitColsFromStore().
*/
QString EqTree::tokenConverter( EqVar* varPtr, const QString &token )
{
	const char* tokenAsc = token.ascii();	// for debug purposes only
	if ( varPtr->m_name == "vSurfaceFuelBedModel"
	  || varPtr->m_name == "vSurfaceFuelBedModel1"
	  || varPtr->m_name == "vSurfaceFuelBedModel2" )
	{
		// Reset the token to the fuel model name if this is a fuel model number
        FuelModel *fm = m_fuelModelList->fuelModelByModelName( token );
		if ( fm )
		{
			return fm->m_name;
		}
	}
	return token;
}

//------------------------------------------------------------------------------
/*! \brief Validates all worksheet entry values and checks the number of
 *  range variables.
 *
 *  All worksheet text should be moved into the EqVars' m_store and validated
 *  before calling this!
 *
 *  All error messages are displayed here and the caller, EqTree::run(),
 *  can just test for the result code and continue.
 *
 *  The caller must have stored all variable values in the m_stores!
 *
 *  \return A negative return code indicates an error condition,
 *  and the offending m_leaf[] index is returned in the \a badLid argument.
 *  On return the m_rangeVar[] array contains pointers to (possibly)
 *  the first zero, one, or two multi-valued variables in the leaf list,
 *  and all remaining elements are NULL.
 *
 *  \retval  0 All variables values are present and correct.
 *  \retval -1 An invalid EqVar entry was detected.
 *  \retval -2 A required EqVar entry field was left empty.
 *  \retval -3 More than 2 range variables were detected.
 *  \retval -4 The EqCalc::validateInputs() detected an error.
 *  \retval -5 Mismatched number of values between master and slave input fields.
 */

int EqTree::validateInputs( int *badLid, int *badPosition, int *badLength )
{
    // Initialize the rangeVar[] pointer array.
    m_rangeVars = 0;
    for ( int i = 0;
          i < m_maxRangeVars;
          i++ )
    {
        m_rangeVar[i] = 0;
    }
    // Validate each m_store.
    EqVar *varPtr;
    int lid, tokens, position, length;
    QString caption(""), msg("");
    for ( lid = 0;
          lid < m_leafCount;
          lid++ )
    {
        varPtr = m_leaf[lid];
        if ( ! varPtr->isValidStore( &tokens, &position, &length ) )
        {
            *badLid = lid;
            *badPosition = position;
            *badLength = length;
            return( -1 );
        }
    }
    // Now set input masks and check for missing inputs.
    m_eqCalc->maskInputs();
    for ( lid = 0;
          lid < m_leafCount;
          lid++ )
    {
        varPtr = m_leaf[lid];
        // Masked inputs are allowed to have any number of tokens.
        if ( varPtr->m_isMasked )
        {
            continue;
        }
        // Only check continuous and discrete output variables.
        if ( varPtr->isDiscrete() || varPtr->isContinuous() )
        {
            // Must be at least 1 input token.
            if ( varPtr->m_tokens < 1 )
            {
                // Missing contents; reposition the focus and return.
                translate( caption, "EqTree:MissingInput:Caption" );
                translate( msg, "EqTree:MissingInput:Text",
                    *varPtr->m_label );
                error( caption, msg );
                *badLid = lid;
                *badPosition = 0;
                *badLength = 0;
                return( -2 );
            }
            // Input variables without masters ...
            if ( varPtr->m_master == ""
              || m_propDict->boolean( "containConfResourcesSingle" ) )
            {
                // Check for multiple tokens
                if ( varPtr->m_tokens > 1 )
                {
                    m_rangeVar[m_rangeVars++] = varPtr;
                    if ( m_rangeVars == m_maxRangeVars-1 )
                    {
                        break;
                    }
                }
            }
            // Input variables with masters
            else
            {
                // Get pointer to its master input variable
                EqVar *master = getVarPtr( varPtr->m_master );
                // If this isn't the master, check its token count
                if ( ( master != varPtr )
                  && ( varPtr->m_tokens != master->m_tokens ) )
                {
                    translate( caption, "EqTree:MasterSlaveMismatch:Caption" );
                    translate( msg, "EqTree:MasterSlaveMismatch:Text",
                        varPtr->m_label->latin1(),
                        QString( "%1" ).arg( varPtr->m_tokens),
                        QString( "%1" ).arg( master->m_tokens),
                        master->m_label->latin1() );
                    error( caption, msg );
                    return( -5 );
                }
            }
        }   //if ( varPtr->isDiscrete() || varPtr->isContinuous() )
    }   // for

    // Ensure there are no more than 2 range variables.
    if ( m_rangeVars > 2 )
    {
        QString text("");
        for ( int mid = 0;
              mid < m_rangeVars;
              mid++ )
        {
            text += QString( "\n        %1" )
                .arg( m_rangeVar[mid]->m_label->latin1() );
        }
        translate( caption, "EqTree:TooManyRangeVariables:Caption" );
        translate( msg, "EqTree:TooManyRangeVariables:Text", text );
        error( caption, msg );
        return( -3 );
    }
    // Perform any implementation-specific value cross-checks
    if ( ! m_eqCalc->validateInputs() )
    {
        return( -4 );
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Validates all worksheet RxVar entry values.
 *
 *  All worksheet text should be moved into the EqVars' m_store and validated
 *  before calling this!
 *
 *  All error messages are displayed here and the caller, EqTree::run(),
 *  can just test for the result code and continue.
 *
 *  The caller must have stored all variable values in the m_stores!
 *
 *  \return A negative return code indicates an error condition,
 *  and the offending m_leaf[] index is returned in the \a badLid argument.
 *  On return the m_rangeVar[] array contains pointers to (possibly)
 *  the first zero, one, or two multi-valued variables in the leaf list,
 *  and all remaining elements are NULL.
 *
 *  \retval  0 All variables values are present and correct.
 *  \retval -1 A checked RxVar minimum entry was left blank.
 *  \retval -2 A checked RxVar maximum entry was left blank.
 *  \retval -3 A RxVar minimum entry was invalid.
 *  \retval -4 A RxVar maximum entry was invalid.
 */

int EqTree::validateRxInputs( int *badRx )
{
    // Validate each m_store.
    QString caption, msg;
    int rx = 0;
    for ( RxVar *rxVar = m_rxVarList->first();
          rxVar;
          rxVar = m_rxVarList->next(), rx++ )
    {
        // Must be a user output
        EqVar *rxPtr = rxVar->m_varPtr;
        *badRx = rx;
        if ( rxPtr->m_isUserOutput
          && rxVar->m_isActive )
        {
            // Must be some text
            if ( rxVar->m_storeMinimum == "" )
            {
                // Missing contents; reposition the focus and return.
                translate( caption, "EqTree:MissingRxInput:Caption" );
                translate( msg, "EqTree:MissingRxMinInput:Text", *(rxPtr->m_label) );
                error( caption, msg );
                return( -1 );
            }
            // Must be some text
            if ( rxVar->m_storeMaximum == "" )
            {
                // Missing contents; reposition the focus and return.
                translate( caption, "EqTree:MissingRxInput:Caption" );
                translate( msg, "EqTree:MissingRxMaxInput:Text", *(rxPtr->m_label) );
                error( caption, msg );
                return( -2 );
            }

            if ( ! rxVar->isValid( rxVar->m_storeMinimum ) )
            {
                return( -3 );
            }
            if ( ! rxVar->isValid( rxVar->m_storeMaximum ) )
            {
                return( -4 );
            }
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Writes the current EqTree properties and variables to an XML
 *  file in a format that can be read by EqTree:readXmlFile().
 *
 *  \param fileName     Full path name of the file to write.
 *                      If the file already exists, it is overwritten.
 *  \param elementName  Name of the enclosing element.
 *  \param elementType  Name of the enclosing element's type attribute.
 *  \param release      Only write variables and properties with this release
 *                      or lower.
 *  \param writeValues  If TRUE, variables values are written.
 *                      If FALSE, no values are written, e.g., for worksheets.
 *
 *  The property and variable elements are enclosed as below:
 *  <?xml version="1.0" encoding="iso-8859-1" standalone="yes" ?>
 *  <elementName type="elementType">
 *    <property name="appDescription" value="This is a description property" />
 *    <variable name="vDocDescription" />
 *  </elementName>
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqTree::writeXmlFile( const QString &fileName,
        const QString &elementName, const QString &elementType,
        int release, bool writeValues )
{
    // Attempt to open the file
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqTree:WriteXmlFile:NoOpen",
            elementName, elementType, fileName );
        warn( text );
        return( false );
    }
    // Write the header, properties, variables, and footer.
    xmlWriteHeader( fptr, elementName, elementType, appWindow()->m_release );
    m_propDict->writeXmlFile( fptr, release );
    m_rxVarList->writeXmlFile( fptr );
    writeXmlFile( fptr, release, writeValues );
    xmlWriteFooter( fptr, elementName );
    fclose( fptr );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Writes the current variable contents in sorted order to the file
 *  stream. Assumes the XML header has already been written by the caller,
 *  and the XML footer will be added by the caller.
 *
 *  \param fptr         File stream to be written
 *  \param release      Only write variables and properties with this release
 *                      or lower.
 *  \param writeValues  If TRUE, variables values are written.
 *                      If FALSE, no values are written, e.g., for worksheets.
 *
 *  \return TRUE on success, FALSE if unable to open the file.
 */

bool EqTree::writeXmlFile( FILE *fptr, int release, bool writeValues )
{
    // Write out all variables to a string list for sorting
    QStringList varList;
    EqVar *varPtr;
    QString xml("");
    for ( int id = 0;
          id < m_varCount;
          id++ )
    {
        varPtr = m_var[id];
        // Only write variables for this and previous releases.
        if ( release >= varPtr->m_releaseFrom
          && release <= varPtr->m_releaseThru )
        {
            // Escape the XML entity references (&, ", ', <, and > )
            if ( writeValues )
            {
                xml = varPtr->m_store;
                xmlEscape( xml );
            }
            else
            {
                xml = "";
            }
            // Write record appropriate for the variable type
            if ( varPtr->isContinuous() )
            {
                varList.append( QString(
                    "  <variable name=\"%1\" decimals=\"%2\" units=\"%3\" value=\"%4\" />" )
                    .arg( varPtr->m_name )
                    .arg( varPtr->m_displayDecimals )
                    .arg( varPtr->m_displayUnits )
                    .arg( xml ) );
            }
            else if ( varPtr->isDiscrete() )
            {
                varList.append(
                    QString( "  <variable name=\"%1\" code=\"%2\" />" )
                    .arg( varPtr->m_name )
                    .arg( xml ) );
            }
            else if ( varPtr->isText() )
            {
                varList.append(
                    QString( "  <variable name=\"%1\" text=\"%2\" />" )
                    .arg( varPtr->m_name )
                    .arg( xml ) );
            }
        }
    }
    // Sort the list and write it to the file
    varList.sort();
    for ( QStringList::Iterator sit=varList.begin();
          sit != varList.end();
          ++sit )
    {
        fprintf( fptr, "%s\n", (*sit).latin1() );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Parses a string of values (usually from an entry field)
 *  and returns the min and max values.
 *
 *  \param qStr The string to be parsed.
 *  \param minval  Where to store the minimum parsed value.
 *  \param maxval  Where to store the maximum parsed value.
 *
 *  \retval The min and max values are returned in the passed arguments.
 *  \retval The number of tokens.
 */

int strMinMax( const QString &qStr, double *minval, double *maxval )
{
    double val;
    Parser parser( " \t,\"", "", "" );
    QString token;
    int n = parser.parse( qStr );
    if ( n > 0 )
    {
        token = parser.token( 0 );
        *minval = *maxval = token.toDouble();
        for ( int i = 1;
              i < n;
              i++ )
        {
            token = parser.token( i );
            val = token.toDouble();
            if ( val < *minval )
            {
                *minval = val;
            }
            if ( val > *maxval )
            {
                *maxval = val;
            }
        }
    }
    return( n );
}

//------------------------------------------------------------------------------
//  End of xeqtree.cpp
//------------------------------------------------------------------------------

