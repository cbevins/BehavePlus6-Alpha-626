//------------------------------------------------------------------------------
/*! \file xeqcalc.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2016 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqTree calculator interface.
 *
 *  This file contains the implementation interface between the generic EqTree
 *  and the specific set of variables and functions.  The idea is to keep all
 *  function and variable details in just 2 files, xeqtree.xml and xeqcalc.cpp.
 *
 *  All variables must be
 *  -# specified in the xeqtree.xml file via
 *          <variable name="vVariableName" type=""... />
 *  -# declared in xeqcalc.h via
 *          EqVar *vVariableName;
 *  -# initialized in EqCalc::EqCalc() constructor via
 *          vVariableName = m_eqTree->getVarPtr( "vVariableName" );
 *
 *  All functions must be
 *  -# specified in xeqtree.xml file via
 *          <function name="fFunctionName" input="" output=""... />
 *  -# declared in xeqcalc.h via
 *          void FunctionName( void );
 *  -# defined as a function in this file via
 *          void EqCalc::FunctionName( void ) { }
 *  -# initialized in EqCalc::EqCalc() constructor via
 *      m_eqTree->setEqFunAddress( "FunctionName", &EqCalc::FunctioName );
 */

// Custom include files
#include "Bp6Globals.h"
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "cdtlib.h"
#include "contain.h"
#include "fuelmodel.h"
#include "moisscenario.h"
#include "parser.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqvar.h"
#include "xeqtree.h"
#include "xfblib.h"

#include "Algorithms/Fofem6BarkThickness.h"
#include "Algorithms/Fofem6Mortality.h"
#include "SemFireCrownFirebrandProcessor.h"

// Qt include files
#include <qstring.h>

// Standard include files
#include <stdlib.h>
#include <string.h>
#include <math.h>

static const int MaxParts = 10;
static char Margin[] = { "        " };

bool EqCalc::closeEnough( const char* what, double v5, double v6, double delta )
{
	if ( fabs( v5-v6 ) > delta )
	{
		printf( "\n\n*** %s v5=%g,  v6=%g\n", what, v5, v6 );
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------
/*! \brief Convenience routine to get a pointer to the FuelModel
 *  of the current vSurfaceFuelBedModel (if not doing two fuel model weighting)
 *  or of the current vSurfaceFuelBedModel1 or vSurfaceFuelBedModel2
 *  (if doing two fuel model weighting).
 *
 *  \param id Indicates which fuel model to get:
 *      - 0 fetches a pointer to the current vSurfaceFuelBedModel FuelModel
 *      - 1 fetches a pointer to the current vSurfaceFuelBedModel1 FuelModel
 *      - 2 fetches a pointer to the current vSurfaceFuelBedModel2 FuelModel
 *
 *  \return Pointer to the current items FuelModel.
 */

FuelModel *EqCalc::currentFuelModel( int id )
{
    // Get the requested variable's address
    EqVar *varPtr;
    if ( id == 0 )
    {
        varPtr = vSurfaceFuelBedModel;
    }
    else if ( id == 1 )
    {
        varPtr = vSurfaceFuelBedModel1;
    }
    else if ( id == 2 )
    {
        varPtr = vSurfaceFuelBedModel2;
    }
    // Get the FuelModel class pointer for the current fuel model name
    QString name = varPtr->activeItemName();
	const char* nameAsc = name.ascii();		// for debug pruposes only
    FuelModel *fm = m_eqTree->m_fuelModelList->fuelModelByModelName( name );
    if ( ! fm )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqCalc:FuelModelNotFound", varPtr->activeItemName() );
        bomb( text );
    }
    return( fm );
}

//------------------------------------------------------------------------------
/*! \brief ContainFF is Contain a'la Fried & Fried for multiple resources.
 *
 *  Dependent Variables (Outputs)
 *      vContainAttackBack (ch)
 *      vContainAttackHead (ch)
 *      vContainAttackPerimeter (ch)
 *      vContainAttackSize (ac)
 *      vContainCost (none)
 *      vContainLine (ch)
 *      vContainPoints
 *      vContainReportBack (ch)
 *      vContainReportHead (ch)
 *      vContainResourcesUsed (count)
 *      vContainSize (ac)
 *      vContainStatus
 *      vContainTime (min since report)
 *      vContainXMax (ch)
 *      vContainXMin (ch)
 *      vContainYMax (ch)
 *
 *  Independent Variables (Inputs)
 *      vContainAttackDist  (ch)
 *      vContainAttackTactic (Head or Rear)
 *      vContainReportRatio (ch/ch)
 *      vContainReportSize (ac)
 *      vContainReportSpread (ch/h)
 *      vContainResourceArrival (min since fire report)
 *      vContainResourceBaseCost
 *      vContainResourceDuration (min)
 *      vContainResourceHourCost
 *      vContainResourceName
 *      vContainResourceProd (ch/h)
 */

void EqCalc::ContainFF( void )
{
    // Crosswalk from Contain status codes to BehavePlus status codes.
    // 0 == Contained
    // 1 == Withdrawn
    // 2 == Escaped
    static int Status[8] =
    {
        2, // 0=Unreported  Fire started but not yet reported (init() not called)
        2, // 1=Reported    Fire reported but not yet attacked (init() called)
        2, // 2=Attacked    Fire attacked but not yet resolved
        0, // 3=Contained   Fire contained by attacking forces
        1, // 4=Overrun     Attacking resources are overrun
        2, // 5=Exhausted   Attacking resources are exhausted
        2, // 6=Overflow    Simulation max step overflow
        2, // 7=DistLimit   Fire spread distance limit exceeded
    };

    // We'll need to get some properties
    PropertyDict *prop = m_eqTree->m_propDict;

    // Access current input values
    double attackDist = vContainAttackDist->m_nativeValue;
    int    tactic     = vContainAttackTactic->activeItemDataIndex();
    double lwRatio    = vContainReportRatio->m_nativeValue;
    double reportSize = vContainReportSize->m_nativeValue;
    double reportRate = vContainReportSpread->m_nativeValue;
    double distLimit  = vContainLimitDist->m_nativeValue;
    if ( prop->boolean( "containConfLimitDistOff" ) )
    {
        distLimit = 1000000.;
    }

    // Build the containment resources array
    ContainForce *force = new ContainForce();
    checkmem( __FILE__, __LINE__, force, "ContainForce force", 1 );
    Parser parserArr( " \t,\"", "", "" );
    parserArr.parse( vContainResourceArrival->m_store );
    Parser parserDur( " \t,\"", "", "" );
    parserDur.parse( vContainResourceDuration->m_store );
    Parser parserName( " \t,\"", "", "" );
    parserName.parse( vContainResourceName->m_store );
    Parser parserProd( " \t,\"", "", "" );
    parserProd.parse( vContainResourceProd->m_store );

    Parser parserBase( " \t,\"", "", "" );
    Parser parserHour( " \t,\"", "", "" );
    bool doCost = vContainCost->m_isUserOutput;
    if ( doCost )
    {
        parserBase.parse( vContainResourceBaseCost->m_store );
        parserHour.parse( vContainResourceHourCost->m_store );
    }
    double arr, dur, prod, tmp;
    double base = 0.;
    double hour = 0.;
    QString name;
    // Loop for each resource
    for ( int i=0; i<vContainResourceName->m_tokens; i++ )
    {
        // Resource arrival time
        tmp = parserArr.token( i ).toDouble();
        appSiUnits()->convert( tmp,
            vContainResourceArrival->m_displayUnits.latin1(),
            vContainResourceArrival->m_nativeUnits.latin1(), &arr );
        //fprintf( stderr, "Resource Arrival %f %s --> %f %s\n",
        //  tmp, vContainResourceArrival->m_displayUnits.latin1(),
        //    arr, vContainResourceArrival->m_nativeUnits.latin1() );

        // Resource duration
        tmp = parserDur.token( i ).toDouble();
        appSiUnits()->convert( tmp,
            vContainResourceDuration->m_displayUnits.latin1(),
            vContainResourceDuration->m_nativeUnits.latin1(), &dur );
        //fprintf( stderr, "Resource Duration %f %s --> %f %s\n",
        //  tmp, vContainResourceDuration->m_displayUnits.latin1(),
        //    dur, vContainResourceDuration->m_nativeUnits.latin1() );

        // Resource name
        name = parserName.token( i );

        // Resource productivity
        tmp = parserProd.token( i ).toDouble();
        appSiUnits()->convert( tmp,
            vContainResourceProd->m_displayUnits.latin1(),
            vContainResourceProd->m_nativeUnits.latin1(), &prod );
        //fprintf( stderr, "Resource Productivity %f %s --> %f %s\n",
        //  tmp, vContainResourceProd->m_displayUnits.latin1(),
        //    prod, vContainResourceProd->m_nativeUnits.latin1() );

        // Resource cost
        if ( doCost )
        {
            base = parserBase.token( i ).toDouble();
            hour = parserHour.token( i ).toDouble();
        }
        // Add the resource to the resource array
        force->addResource( arr, prod, dur, LeftFlank, name.latin1(),
            base, hour );
    }

    // Calculate results
    int maxSteps = prop->integer( "containConfMaxSteps" );
    int minSteps = prop->integer( "containConfMinSteps" );
    bool retry   = prop->boolean( "containConfRetry" );
    ContainSim *sim = new ContainSim( reportSize, reportRate, lwRatio,
        force, (ContainTactic) tactic, attackDist, distLimit,
        retry, minSteps, maxSteps );
    checkmem( __FILE__, __LINE__, sim, "ContainSim sim", 1 );
    sim->run();
    int    status = Status[ sim->m_left->m_status ];
    double finalSize = sim->m_finalSize;
    // If no contained or limit exceeded...
    if ( status != 0 && status != 3 )
    {
        // Reset final size to -1 acres
        finalSize = -1;
        // If fire line was built, then Withdrawn, otherwise Escaped
        status = ( sim->m_finalLine > 0.0 ) ? 1 : 2;
    }

    // Determine perimeter and size at initial attack
    bool useFirstResourceArrivalTimeAsInitialAttack = true;
    double length = sim->m_left->m_attackBack + sim->m_left->m_attackHead;
    if ( useFirstResourceArrivalTimeAsInitialAttack )
    {
        length = sim->m_left->m_initialAttackBack
               + sim->m_left->m_initialAttackHead;
    }
    double width  = FBL_SurfaceFireWidth( length, lwRatio );
    double perim  = FBL_SurfaceFirePerimeter( length, width );
    double size   = FBL_SurfaceFireArea( length, width );
    size *= 0.1;    // ac

    // Store results
    if ( useFirstResourceArrivalTimeAsInitialAttack )
    {
        vContainAttackBack->update( sim->m_left->m_initialAttackBack );
        vContainAttackHead->update( sim->m_left->m_initialAttackHead );
    }
    else
    {
        vContainAttackBack->update( sim->m_left->m_attackBack );
        vContainAttackHead->update( sim->m_left->m_attackHead );
    }
    vContainAttackPerimeter->update( perim );
    vContainAttackSize->update( size );
    vContainCost->update( sim->m_finalCost );
    vContainLine->update( sim->m_finalLine );
    vContainPoints->update( sim->m_left->m_step + 1 );
    vContainReportBack->update( sim->m_left->m_reportBack );
    vContainReportHead->update( sim->m_left->m_reportHead );
    vContainResourcesUsed->update( sim->m_used );
    vContainSize->update( finalSize );
    vContainStatus->updateItem( status );
    vContainTime->update( sim->m_finalTime );
    vContainXMax->update( sim->m_xMax );
    vContainXMin->update( sim->m_xMin );
    vContainYMax->update( sim->m_yMax );

    // Log results
    if( m_log )
    {
        int outputs = 14 + sim->m_left->m_step + 1;
        fprintf( m_log, "%sbegin proc ContainFF() 5 %d\n", Margin, outputs );
        fprintf( m_log, "%s  i vContainReportSpread %g %d %s\n", Margin,
            vContainReportSpread->m_displayValue,
            vContainReportSpread->m_displayDecimals,
            vContainReportSpread->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainReportSize %g %d %s\n", Margin,
            vContainReportSize->m_displayValue,
            vContainReportSize->m_displayDecimals,
            vContainReportSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainReportRatio %g %d %s\n", Margin,
            vContainReportRatio->m_displayValue,
            vContainReportRatio->m_displayDecimals,
            vContainReportRatio->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainAttackTactic %d %d %s\n", Margin,
            vContainAttackTactic->activeItemDataIndex(),
            0,
            vContainAttackTactic->activeItemName().latin1() );
        fprintf( m_log, "%s  i vContainAttackDist %g %d %s\n", Margin,
            vContainAttackDist->m_displayValue,
            vContainAttackDist->m_displayDecimals,
            vContainAttackDist->m_displayUnits.latin1() );

        fprintf( m_log, "%s  o vContainAttackSize %g %d %s\n", Margin,
            vContainAttackSize->m_displayValue,
            vContainAttackSize->m_displayDecimals,
            vContainAttackSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackPerimeter %g %d %s\n", Margin,
            vContainAttackPerimeter->m_displayValue,
            vContainAttackPerimeter->m_displayDecimals,
            vContainAttackPerimeter->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainStatus %d %d %s\n", Margin,
            vContainStatus->activeItemDataIndex(),
            0,
            vContainStatus->activeItemName().latin1() );
        fprintf( m_log, "%s  o vContainTime %g %d %s\n", Margin,
            vContainTime->m_displayValue,
            vContainTime->m_displayDecimals,
            vContainTime->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainSize %g %d %s\n", Margin,
            vContainSize->m_displayValue,
            vContainSize->m_displayDecimals,
            vContainSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainLine %g %d %s\n", Margin,
            vContainLine->m_displayValue,
            vContainLine->m_displayDecimals,
            vContainLine->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainResourcesUsed %g %d ratio\n", Margin,
            vContainResourcesUsed->m_displayValue,
            vContainResourcesUsed->m_displayDecimals );
        fprintf( m_log, "%s  o vContainCost %g %d %s\n", Margin,
            vContainCost->m_displayValue,
            vContainCost->m_displayDecimals,
            vContainCost->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackBack %g %d %s\n", Margin,
            vContainAttackBack->m_displayValue,
            vContainAttackBack->m_displayDecimals,
            vContainAttackBack->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackHead %g %d %s\n", Margin,
            vContainAttackHead->m_displayValue,
            vContainAttackHead->m_displayDecimals,
            vContainAttackHead->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportBack %g %d %s\n", Margin,
            vContainReportBack->m_displayValue,
            vContainReportBack->m_displayDecimals,
            vContainReportBack->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportHead %g %d %s\n", Margin,
            vContainReportHead->m_displayValue,
            vContainReportHead->m_displayDecimals,
            vContainReportHead->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainXMin %g %d %s\n", Margin,
            vContainXMin->m_displayValue,
            vContainXMin->m_displayDecimals,
            vContainXMin->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainXMax %g %d %s\n", Margin,
            vContainXMax->m_displayValue,
            vContainXMax->m_displayDecimals,
            vContainXMax->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainYMax %g %d %s\n", Margin,
            vContainYMax->m_displayValue,
            vContainYMax->m_displayDecimals,
            vContainYMax->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainPoints %g %d ratio\n", Margin,
            vContainPoints->m_displayValue,
            vContainPoints->m_displayDecimals );
        // The coordinates need to be converted from chains to display units
        double factor, offset;
        appSiUnits()->conversionFactorOffset( vContainXMax->m_nativeUnits,
            vContainXMax->m_displayUnits, &factor, &offset );
        for ( int pt = 0;
              pt <= sim->m_left->m_step;
              pt++ )
        {
            fprintf( m_log, " %s      %f %f\n", Margin,
                offset + factor * sim->m_x[pt],
                offset + factor * sim->m_y[pt] );
        }
    }
    // Free resources
    delete force;   force = 0;
    delete sim;     sim = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief ContainFFReportRatio
 *
 *  Dependent Variables (Outputs)
 *      vContainReportRatio (ch/ch)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengthToWidth (ft/ft)
 */

void EqCalc::ContainFFReportRatio( void )
{
    // Access current input values
    double ratio = vSurfaceFireLengthToWidth->m_nativeValue;
    // Store results
    vContainReportRatio->update( ratio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc ContainFFReportRatio() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireArea %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportRatio %g %s\n", Margin,
            vContainReportRatio->m_nativeValue,
            vContainReportRatio->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief ContainFFReportSize
 *
 *  Dependent Variables (Outputs)
 *      vContainReportSize (ac)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireArea (ac)
 */

void EqCalc::ContainFFReportSize( void )
{
    // Access current input values
    double acres = vSurfaceFireArea->m_nativeValue;
    // Store results
    vContainReportSize->update( acres );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc ContainFFReportSize() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireArea %g %s\n", Margin,
            vSurfaceFireArea->m_nativeValue,
            vSurfaceFireArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportSize %g %s\n", Margin,
            vContainReportSize->m_nativeValue,
            vContainReportSize->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief ContainFFReportSpread - simply transforms vSurfaceFireSpreadAtHead
 *	from its native units (ft/min) into the F&F Contain native units (ch/hr)
 *
 *  Dependent Variables (Outputs)
 *      vContainReportSpread (ch/h)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 */

void EqCalc::ContainFFReportSpread( void )
{
    // Access current input values
    double fpm = vSurfaceFireSpreadAtHead->m_nativeValue;
    // Store results
    vContainReportSpread->update( fpm * 60. / 66. );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc ContainFFReportSpread() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportSpread %g %s\n", Margin,
            vContainReportSpread->m_nativeValue,
            vContainReportSpread->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief ContainFFSingle is Contain a'la Fried & Freid for single resources
 *
 *  Dependent Variables (Outputs)
 *      vContainAttackBack (ch)
 *      vContainAttackHead (ch)
 *      vContainAttackPerimeter (ch)
 *      vContainAttackSize (ac)
 *      vContainCost (none)
 *      vContainLine (ch)
 *      vContainPoints
 *      vContainReportBack (ch)
 *      vContainReportHead (ch)
 *      vContainResourcesUsed (count)
 *      vContainSize (ac)
 *      vContainStatus
 *      vContainTime (min since report)
 *      vContainXMax (ch)
 *      vContainXMin (ch)
 *      vContainYMax (ch)
 *
 *  Independent Variables (Inputs)
 *      vContainAttackDist  (ch)
 *      vContainAttackTactic (Head or Rear)
 *      vContainReportRatio (ch/ch)
 *      vContainReportSize (ac)
 *      vContainReportSpread (ch/h)
 *      vContainResourceArrival (min since fire report)
 *      vContainResourceBaseCost
 *      vContainResourceDuration (min)
 *      vContainResourceHourCost
 *      vContainResourceName
 *      vContainResourceProd (ch/h)
 */

void EqCalc::ContainFFSingle( void )
{
    // Crosswalk from Contain status codes to BehavePlus status codes.
    // 0 == Contained
    // 1 == Withdrawn
    // 2 == Escaped
    static int Status[8] =
    {
        2, // 0=Unreported  Fire started but not yet reported (init() not called)
        2, // 1=Reported    Fire reported but not yet attacked (init() called)
        2, // 2=Attacked    Fire attacked but not yet resolved
        0, // 3=Contained   Fire contained by attacking forces
        1, // 4=Overrun     Attacking resources are overrun
        2, // 5=Exhausted   Attacking resources are exhausted
        2, // 6=Overflow    Simulation max step overflow
        2, // 7=DistLimit   Fire spread distance limit exceeded
    };

    // We'll need to get some properties
    PropertyDict *prop = m_eqTree->m_propDict;

    // Access current input values
    double attackDist = vContainAttackDist->m_nativeValue;
    int    tactic     = vContainAttackTactic->activeItemDataIndex();
    double lwRatio    = vContainReportRatio->m_nativeValue;
    double reportSize = vContainReportSize->m_nativeValue;
    double reportRate = vContainReportSpread->m_nativeValue;
    double distLimit  = vContainLimitDist->m_nativeValue;
    if ( prop->boolean( "containConfLimitDistOff" ) )
    {
        distLimit = 1000000.;
    }

    // Resources
    QString name      = vContainResourceName->m_store;
    double arrival    = vContainResourceArrival->m_nativeValue;
    double duration   = vContainResourceDuration->m_nativeValue;
    double prod       = vContainResourceProd->m_nativeValue;
    double baseCost   = 0.;
    double hourCost   = 0.;
    bool   doCost     = vContainCost->m_isUserOutput;
    if ( doCost )
    {
        baseCost   = vContainResourceBaseCost->m_nativeValue;
        hourCost   = vContainResourceHourCost->m_nativeValue;
    }

    // Build the containment resources array
    ContainForce *force = new ContainForce();
    checkmem( __FILE__, __LINE__, force, "ContainForce force", 1 );
    force->addResource( arrival, prod, duration, LeftFlank, name.latin1(),
            baseCost, hourCost );

    // Calculate results
    int maxSteps = prop->integer( "containConfMaxSteps" );
    int minSteps = prop->integer( "containConfMinSteps" );
    bool retry   = prop->boolean( "containConfRetry" );
    ContainSim *sim = new ContainSim( reportSize, reportRate, lwRatio,
        force, (ContainTactic) tactic, attackDist, distLimit,
        retry, minSteps, maxSteps );
    checkmem( __FILE__, __LINE__, sim, "ContainSim sim", 1 );
    sim->run();
    int    status = Status[ sim->m_left->m_status ];
    double finalSize = sim->m_finalSize;
    // If not contained...
    if ( status != 0 && status != 3 )
    {
        // Reset final size to -1 acres
        finalSize = -1;
        // If fire line was built, then Withdrawn, otherwise Escaped
        status = ( sim->m_finalLine > 0.0 ) ? 1 : 2;
    }

    // Determine perimeter and size at initial attack
    double length = sim->m_left->m_attackBack + sim->m_left->m_attackHead;
    double width  = FBL_SurfaceFireWidth( length, lwRatio );
    double perim  = FBL_SurfaceFirePerimeter( length, width );
    double size   = FBL_SurfaceFireArea( length, width );  // ch2
    size *= 0.1;    // ac

    // Store results
    vContainAttackBack->update( sim->m_left->m_attackBack );
    vContainAttackHead->update( sim->m_left->m_attackHead );
    vContainAttackPerimeter->update( perim );
    vContainAttackSize->update( size );
    vContainCost->update( sim->m_finalCost );
    vContainLine->update( sim->m_finalLine );
    vContainPoints->update( sim->m_left->m_step + 1 );
    vContainReportBack->update( sim->m_left->m_reportBack );
    vContainReportHead->update( sim->m_left->m_reportHead );
    vContainResourcesUsed->update( sim->m_used );
    vContainSize->update( finalSize );
    vContainStatus->updateItem( status );
    vContainTime->update( sim->m_finalTime );
    vContainXMax->update( sim->m_xMax );
    vContainXMin->update( sim->m_xMin );
    vContainYMax->update( sim->m_yMax );

    // Log results
    if( m_log )
    {
        int outputs = 14 + sim->m_left->m_step + 1;
        fprintf( m_log, "%sbegin proc ContainFF() 5 %d\n", Margin, outputs );
        fprintf( m_log, "%s  i vContainReportSpread %g %d %s\n", Margin,
            vContainReportSpread->m_displayValue,
            vContainReportSpread->m_displayDecimals,
            vContainReportSpread->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainReportSize %g %d %s\n", Margin,
            vContainReportSize->m_displayValue,
            vContainReportSize->m_displayDecimals,
            vContainReportSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainReportRatio %g %d %s\n", Margin,
            vContainReportRatio->m_displayValue,
            vContainReportRatio->m_displayDecimals,
            vContainReportRatio->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vContainAttackTactic %d %d %s\n", Margin,
            vContainAttackTactic->activeItemDataIndex(),
            0,
            vContainAttackTactic->activeItemName().latin1() );
        fprintf( m_log, "%s  i vContainAttackDist %g %d %s\n", Margin,
            vContainAttackDist->m_displayValue,
            vContainAttackDist->m_displayDecimals,
            vContainAttackDist->m_displayUnits.latin1() );

        fprintf( m_log, "%s  o vContainAttackSize %g %d %s\n", Margin,
            vContainAttackSize->m_displayValue,
            vContainAttackSize->m_displayDecimals,
            vContainAttackSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackPerimeter %g %d %s\n", Margin,
            vContainAttackPerimeter->m_displayValue,
            vContainAttackPerimeter->m_displayDecimals,
            vContainAttackPerimeter->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainStatus %d %d %s\n", Margin,
            vContainStatus->activeItemDataIndex(),
            0,
            vContainStatus->activeItemName().latin1() );
        fprintf( m_log, "%s  o vContainTime %g %d %s\n", Margin,
            vContainTime->m_displayValue,
            vContainTime->m_displayDecimals,
            vContainTime->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainSize %g %d %s\n", Margin,
            vContainSize->m_displayValue,
            vContainSize->m_displayDecimals,
            vContainSize->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainLine %g %d %s\n", Margin,
            vContainLine->m_displayValue,
            vContainLine->m_displayDecimals,
            vContainLine->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainResourcesUsed %g %d ratio\n", Margin,
            vContainResourcesUsed->m_displayValue,
            vContainResourcesUsed->m_displayDecimals );
        fprintf( m_log, "%s  o vContainCost %g %d %s\n", Margin,
            vContainCost->m_displayValue,
            vContainCost->m_displayDecimals,
            vContainCost->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackBack %g %d %s\n", Margin,
            vContainAttackBack->m_displayValue,
            vContainAttackBack->m_displayDecimals,
            vContainAttackBack->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainAttackHead %g %d %s\n", Margin,
            vContainAttackHead->m_displayValue,
            vContainAttackHead->m_displayDecimals,
            vContainAttackHead->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportBack %g %d %s\n", Margin,
            vContainReportBack->m_displayValue,
            vContainReportBack->m_displayDecimals,
            vContainReportBack->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainReportHead %g %d %s\n", Margin,
            vContainReportHead->m_displayValue,
            vContainReportHead->m_displayDecimals,
            vContainReportHead->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainXMin %g %d %s\n", Margin,
            vContainXMin->m_displayValue,
            vContainXMin->m_displayDecimals,
            vContainXMin->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainXMax %g %d %s\n", Margin,
            vContainXMax->m_displayValue,
            vContainXMax->m_displayDecimals,
            vContainXMax->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainYMax %g %d %s\n", Margin,
            vContainYMax->m_displayValue,
            vContainYMax->m_displayDecimals,
            vContainYMax->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vContainPoints %g %d ratio\n", Margin,
            vContainPoints->m_displayValue,
            vContainPoints->m_displayDecimals );
        // The coordinates need to be converted from chains to display units
        double factor, offset;
        appSiUnits()->conversionFactorOffset( vContainXMax->m_nativeUnits,
            vContainXMax->m_displayUnits, &factor, &offset );
        for ( int pt = 0;
              pt <= sim->m_left->m_step;
              pt++ )
        {
            fprintf( m_log, " %s      %f %f\n", Margin,
                offset + factor * sim->m_x[pt],
                offset + factor * sim->m_y[pt] );
        }
    }
    // Free resources
    delete force;   force = 0;
    delete sim;     sim = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireArea
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireArea (ac)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 *      vSurfaceFireWidthDist (ft)
 */

void EqCalc::FireArea( void )
{
    // Access current input values
    double length = vSurfaceFireLengDist->m_nativeValue;
    double width  = vSurfaceFireWidthDist->m_nativeValue;
    // Calculate results
    double acres  = FBL_SurfaceFireArea( length, width ) * 2.295684e-05;
    // Store results
    vSurfaceFireArea->update( acres );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireWidthDist %g %s\n", Margin,
            vSurfaceFireWidthDist->m_nativeValue,
            vSurfaceFireWidthDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireArea %g %s\n", Margin,
            vSurfaceFireArea->m_nativeValue,
            vSurfaceFireArea->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireCharacteristicsDiagram
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireCharacteristicsDiagram
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireHeatPerUnitArea (Btu/ft2/min)
 */

void EqCalc::FireCharacteristicsDiagram( void )
{
    // Access current input values
    double x = vSurfaceFireCharacteristicsDiagram->m_nativeValue;
    // Calculate results (dummy)
    x++;
    // Store results
    vSurfaceFireCharacteristicsDiagram->update( x );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireCharacteristicsDiagram() 3 1\n", Margin );
        //fprintf( stderr, "%sbegin proc FireCharacteristicsDiagram() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        //fprintf( stderr, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
        //      vSurfaceFireSpreadAtHead->m_nativeValue,
        //    vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
               vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
        //fprintf( stderr, "%s  i vSurfaceFireSpreadAtVector %g %s\n", Margin,
        //      vSurfaceFireSpreadAtVector->m_nativeValue,
        //    vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        //fprintf( stderr, "%s  i vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
        //      vSurfaceFireHeatPerUnitArea->m_nativeValue,
        //    vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireCharacteristicsDiagram %g\n", Margin,
            vSurfaceFireCharacteristicsDiagram->m_nativeValue );
        //fprintf( stderr, "%s  o vSurfaceFireCharacteristicsDiagram %g\n", Margin,
        //    vSurfaceFireCharacteristicsDiagram->m_nativeValue );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireDistAtBack
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtBack (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtBack (ft/min)
 */

void EqCalc::FireDistAtBack( void )
{
    // Access current input values
    double ros     = vSurfaceFireSpreadAtBack->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtBack->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtBack() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtBack %g %s\n", Margin,
            vSurfaceFireSpreadAtBack->m_nativeValue,
            vSurfaceFireSpreadAtBack->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtBack %g %s\n", Margin,
            vSurfaceFireDistAtBack->m_nativeValue,
            vSurfaceFireDistAtBack->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief FireDistAtBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtBeta (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtBeta (ft/min)
 */

void EqCalc::FireDistAtBeta( void )
{
    // Access current input values
    double ros     = vSurfaceFireSpreadAtBeta->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtBeta->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtBeta() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtBeta %g %s\n", Margin,
            vSurfaceFireSpreadAtBeta->m_nativeValue,
            vSurfaceFireSpreadAtBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtBeta %g %s\n", Margin,
            vSurfaceFireDistAtBeta->m_nativeValue,
            vSurfaceFireDistAtBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireDistAtFlank
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtFlank (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtFlank (ft/min)
 */

void EqCalc::FireDistAtFlank( void )
{
    // Access current input values
    double ros  = vSurfaceFireSpreadAtFlank->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtFlank->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtFlank() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtFlank %g %s\n", Margin,
            vSurfaceFireSpreadAtFlank->m_nativeValue,
            vSurfaceFireSpreadAtFlank->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtFlank %g %s\n", Margin,
            vSurfaceFireDistAtFlank->m_nativeValue,
            vSurfaceFireDistAtFlank->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireDistAtHead
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtHead (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtHead (ft/min)
 */

void EqCalc::FireDistAtHead( void )
{
    // Access current input values
    double ros  = vSurfaceFireSpreadAtHead->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtHead->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtHead() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtHead %g %s\n", Margin,
            vSurfaceFireDistAtHead->m_nativeValue,
            vSurfaceFireDistAtHead->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireDistAtPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtPsi (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtPsi (ft/min)
 */

void EqCalc::FireDistAtPsi( void )
{
    // Access current input values
    double ros     = vSurfaceFireSpreadAtPsi->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtPsi->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtPsi() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtPsi %g %s\n", Margin,
            vSurfaceFireSpreadAtPsi->m_nativeValue,
            vSurfaceFireSpreadAtPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtPsi %g %s\n", Margin,
            vSurfaceFireDistAtPsi->m_nativeValue,
            vSurfaceFireDistAtPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireDistAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireDistAtVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireSpreadAtVector (ft/min)
 */

void EqCalc::FireDistAtVector( void )
{
    // Access current input values
    double ros     = vSurfaceFireSpreadAtVector->m_nativeValue;
    double elapsed = vSurfaceFireElapsedTime->m_nativeValue;
    // Calculate results
    double dist = ros * elapsed;
    // Store results
    vSurfaceFireDistAtVector->update( dist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireDistAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
            vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %s\n", Margin,
            vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireDistAtVector %g %s\n", Margin,
            vSurfaceFireDistAtVector->m_nativeValue,
            vSurfaceFireDistAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireEccentricity
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireEccentricity (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengthToWidth (fraction)
 */

void EqCalc::FireEccentricity ( void )
{
    // Access current input values
    double lw = vSurfaceFireLengthToWidth->m_nativeValue;
    // Calculate results
    double ec = FBL_SurfaceFireEccentricity( lw );
    // Store results
    vSurfaceFireEccentricity->update( ec );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireEccentricity() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEccentricity %g %s\n", Margin,
            vSurfaceFireEccentricity->m_nativeValue,
            vSurfaceFireEccentricity->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireEllipseF
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireEllipseF (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 */

void EqCalc::FireEllipseF ( void )
{
    // Access current input values
    double length = vSurfaceFireLengDist->m_nativeValue;
    // Calculate results
    double f = FBL_SurfaceFireEllipseF( length );
    // Store results
    vSurfaceFireEllipseF->update( f );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireEllipseF() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEllipseF %g %s\n", Margin,
            vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireEllipseG
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireEllipseG (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 *		vSurfaceFireDistAtBack (ft)
 */

void EqCalc::FireEllipseG ( void )
{
    // Access current input values
    double length = vSurfaceFireLengDist->m_nativeValue;
	double back = vSurfaceFireDistAtBack->m_nativeValue;
    // Calculate results
    double g = FBL_SurfaceFireEllipseG( length, back );
    // Store results
    vSurfaceFireEllipseG->update( g );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireEllipseG() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireDistAtBack %g %s\n", Margin,
            vSurfaceFireDistAtBack->m_nativeValue,
            vSurfaceFireDistAtBack->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEllipseG %g %s\n", Margin,
            vSurfaceFireEllipseG->m_nativeValue,
            vSurfaceFireEllipseG->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireEllipseH
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireEllipseH (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireWidthDist (ft)
 */

void EqCalc::FireEllipseH ( void )
{
    // Access current input values
    double width = vSurfaceFireWidthDist->m_nativeValue;
    // Calculate results
    double h = FBL_SurfaceFireEllipseH( width );
    // Store results
    vSurfaceFireEllipseH->update( h );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireEllipseH() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireWidthDist %g %s\n", Margin,
            vSurfaceFireWidthDist->m_nativeValue,
            vSurfaceFireWidthDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEllipseH %g %s\n", Margin,
            vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireEffWindAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireEffWindAtVector (mi/h)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireNoWindRate (ft/min)
 *      vSurfaceFireSpreadAtVector (ft/min)
 */

void EqCalc::FireEffWindAtVector( void )
{
    // Access current input values
    double rosV = vSurfaceFireSpreadAtVector->m_nativeValue;
    // Calculate results
	double effWind = 0.;
#ifdef INCLUDE_V5_CODE
    double ros0 = vSurfaceFireNoWindRate->m_nativeValue;
    effWind = FBL_SurfaceFireEffectiveWindSpeedAtVector( ros0, rosV );
#elif INCLUDE_V6_CODE
	effWind = m_Bp6SurfaceFire->getEffectiveWindSpeedAtVector( rosV );
#endif
    // Store results
    vSurfaceFireEffWindAtVector->update( effWind );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireEffWindAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireNoWindRate %g %s\n", Margin,
            vSurfaceFireNoWindRate->m_nativeValue,
            vSurfaceFireNoWindRate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
            vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEffWindAtVector %g %s\n", Margin,
            vSurfaceFireEffWindAtVector->m_nativeValue,
            vSurfaceFireEffWindAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireFlameHtAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireFlameHtAtVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireFlameLengAtVector (ft)
 */

void EqCalc::FireFlameHtAtVector( void )
{
    // Access current input values
    double fl = vSurfaceFireFlameLengAtVector->m_nativeValue;
    //double fa = vSurfaceFireFlameAngleAtVector->m_nativeValue;
    // Calculate results
    double fh = fl;
    // Store results
    vSurfaceFireFlameHtAtVector->update( fh );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireFlameHtAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireFlameAngleAtVector %g %s\n", Margin,
            vSurfaceFireFlameAngleAtVector->m_nativeValue,
            vSurfaceFireFlameAngleAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameHtAtVector %g %s\n", Margin,
            vSurfaceFireFlameHtAtVector->m_nativeValue,
            vSurfaceFireFlameHtAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireFlameLengAtBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireFlameLengAtBeta (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLineIntAtBeta (Btu/ft/s)
 */

void EqCalc::FireFlameLengAtBeta( void )
{
    // Access current input values
    double firelineIntensity = vSurfaceFireLineIntAtBeta->m_nativeValue;
    // Calculate results
    double fl = FBL_SurfaceFireFlameLength( firelineIntensity );
    // Store results
    vSurfaceFireFlameLengAtBeta->update( fl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireFlameLengAtBeta() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtBeta %g %s\n", Margin,
            vSurfaceFireLineIntAtBeta->m_nativeValue,
            vSurfaceFireLineIntAtBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtBeta %g %s\n", Margin,
            vSurfaceFireFlameLengAtBeta->m_nativeValue,
            vSurfaceFireFlameLengAtBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireFlameLengAtHead
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireFlameLengAtHead (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLineIntAtHead (Btu/ft/s)
 */

void EqCalc::FireFlameLengAtHead( void )
{
    // Access current input values
    double firelineIntensity = vSurfaceFireLineIntAtHead->m_nativeValue;
    // Calculate results
    double fl = FBL_SurfaceFireFlameLength( firelineIntensity );
    // Store results
    vSurfaceFireFlameLengAtHead->update( fl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireFlameLengAtHead() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtHead %g %s\n", Margin,
            vSurfaceFireLineIntAtHead->m_nativeValue,
            vSurfaceFireLineIntAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtHead %g %s\n", Margin,
            vSurfaceFireFlameLengAtHead->m_nativeValue,
            vSurfaceFireFlameLengAtHead->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireFlameLengAtPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireFlameLengAtPsi (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLineIntAtPsi (Btu/ft/s)
 */

void EqCalc::FireFlameLengAtPsi( void )
{
    // Access current input values
    double firelineIntensity = vSurfaceFireLineIntAtPsi->m_nativeValue;
    // Calculate results
    double fl = FBL_SurfaceFireFlameLength( firelineIntensity );
    // Store results
    vSurfaceFireFlameLengAtPsi->update( fl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireFlameLengAtPsi() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtPsi %g %s\n", Margin,
            vSurfaceFireLineIntAtPsi->m_nativeValue,
            vSurfaceFireLineIntAtPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtPsi %g %s\n", Margin,
            vSurfaceFireFlameLengAtPsi->m_nativeValue,
            vSurfaceFireFlameLengAtPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireFlameLengAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireFlameLengAtVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 */

void EqCalc::FireFlameLengAtVector( void )
{
    // Access current input values
    double firelineIntensity = vSurfaceFireLineIntAtVector->m_nativeValue;
    // Calculate results
    double fl = FBL_SurfaceFireFlameLength( firelineIntensity );
    // Store results
    vSurfaceFireFlameLengAtVector->update( fl );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireFlameLengAtVector() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireHeatPerUnitArea
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireHeatPerUnitArea (Btu/ft2)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireHeatPerUnitArea( void )
{
    // Access current input values
    double rxi  = vSurfaceFireReactionInt->m_nativeValue;
    double tau  = vSurfaceFireResidenceTime->m_nativeValue;
    // Calculate results
    double hpua = FBL_SurfaceFireHeatPerUnitArea( rxi, tau ) ;
    // Store results
    vSurfaceFireHeatPerUnitArea->update( hpua );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireHeatPerUnitArea() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireHeatSource
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireHeatSource (btu/ft2/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFuelBedHeatSink (btu/ft3)
 */

void EqCalc::FireHeatSource( void )
{
    // Access current input values
    double spread = vSurfaceFireSpreadAtHead->m_nativeValue;
    double sink   = vSurfaceFuelBedHeatSink->m_nativeValue;
    // Calculate results
    double source = FBL_SurfaceFireHeatSource( spread, sink );
    // Store results
    vSurfaceFireHeatSource->update( source );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireHeatSource() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedHeatSink %g %s\n", Margin,
            vSurfaceFuelBedHeatSink->m_nativeValue,
            vSurfaceFuelBedHeatSink->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireHeatSource %g %s\n", Margin,
            vSurfaceFireHeatSource->m_nativeValue,
            vSurfaceFireHeatSource->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLengDist
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLengDist (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtBack (ft)
 *      vSurfaceFireDistAtHead (ft)
 */

void EqCalc::FireLengDist( void )
{
    // Access current input values
    double bd = vSurfaceFireDistAtBack->m_nativeValue;
    double fd = vSurfaceFireDistAtHead->m_nativeValue;
    // Calculate results
    double ld = bd + fd;
    // Store results
    vSurfaceFireLengDist->update( ld );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLengDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtBack %g %s\n", Margin,
            vSurfaceFireDistAtBack->m_nativeValue,
            vSurfaceFireDistAtBack->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireDistAtHead %g %s\n", Margin,
            vSurfaceFireDistAtHead->m_nativeValue,
            vSurfaceFireDistAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLengMapDist
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLengMapDist (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireLengMapDist( void )
{
    // Access current input values
    double ld = vSurfaceFireLengDist->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * ld / 5280.;
    // Store results
    vSurfaceFireLengMapDist->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLengMapDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLengMapDist %g %s\n", Margin,
            vSurfaceFireLengMapDist->m_nativeValue,
            vSurfaceFireLengMapDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLengthToWidth
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLengthToWidth (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireEffWindAtHead (mi/h)
 */

void EqCalc::FireLengthToWidth( void )
{
    // Access current input values
    double effWind = vSurfaceFireEffWindAtHead->m_nativeValue;
    // Calculate results
    double lwRatio = FBL_SurfaceFireLengthToWidthRatio( effWind );
    // Store results
    vSurfaceFireLengthToWidth->update( lwRatio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLengthToWidth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEffWindAtHead %g %s\n", Margin,
            vSurfaceFireEffWindAtHead->m_nativeValue,
            vSurfaceFireEffWindAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLineIntAtBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLineIntAtBeta (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtBeta (ft/min)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireLineIntAtBeta( void )
{
    // Access current input values
    double ros = vSurfaceFireSpreadAtBeta->m_nativeValue;
    double tau = vSurfaceFireResidenceTime->m_nativeValue;
    double rxi = vSurfaceFireReactionInt->m_nativeValue;
     // Calculate results
    double fli = FBL_SurfaceFireFirelineIntensity( ros, rxi, tau ) ;
    // Store results
    vSurfaceFireLineIntAtBeta->update( fli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLineIntAtBeta() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtBeta %g %s\n", Margin,
            vSurfaceFireSpreadAtBeta->m_nativeValue,
            vSurfaceFireSpreadAtBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtBeta %g %s\n", Margin,
            vSurfaceFireLineIntAtBeta->m_nativeValue,
            vSurfaceFireLineIntAtBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLineIntAtHead
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLineIntAtHead (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireLineIntAtHead( void )
{
    // Access current input values
    double ros = vSurfaceFireSpreadAtHead->m_nativeValue;
    double tau = vSurfaceFireResidenceTime->m_nativeValue;
    double rxi = vSurfaceFireReactionInt->m_nativeValue;
    // Calculate results
    double fli = FBL_SurfaceFireFirelineIntensity( ros, rxi, tau ) ;
    // Store results
    vSurfaceFireLineIntAtHead->update( fli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLineIntAtHead() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtHead %g %s\n", Margin,
            vSurfaceFireLineIntAtHead->m_nativeValue,
            vSurfaceFireLineIntAtHead->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLineIntAtPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLineIntAtPsi (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtPsi (ft/min)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireLineIntAtPsi( void )
{
    // Access current input values
    double ros = vSurfaceFireSpreadAtPsi->m_nativeValue;
    double tau = vSurfaceFireResidenceTime->m_nativeValue;
    double rxi = vSurfaceFireReactionInt->m_nativeValue;
     // Calculate results
    double fli = FBL_SurfaceFireFirelineIntensity( ros, rxi, tau ) ;
    // Store results
    vSurfaceFireLineIntAtPsi->update( fli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLineIntAtPsi() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtPsi %g %s\n", Margin,
            vSurfaceFireSpreadAtPsi->m_nativeValue,
            vSurfaceFireSpreadAtPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtPsi %g %s\n", Margin,
            vSurfaceFireLineIntAtPsi->m_nativeValue,
            vSurfaceFireLineIntAtPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLineIntAtVectorFromBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtBeta (ft/min)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireLineIntAtVectorFromBeta( void )
{
    // Access current input values
    double ros = vSurfaceFireSpreadAtBeta->m_nativeValue;
    double tau = vSurfaceFireResidenceTime->m_nativeValue;
    double rxi = vSurfaceFireReactionInt->m_nativeValue;
     // Calculate results
    double fli = FBL_SurfaceFireFirelineIntensity( ros, rxi, tau ) ;
    // Store results
    vSurfaceFireLineIntAtVector->update( fli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLineIntAtVectorFromBeta() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtBeta %g %s\n", Margin,
            vSurfaceFireSpreadAtBeta->m_nativeValue,
            vSurfaceFireSpreadAtBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireLineIntAtVectorFromPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireSpreadAtPsi (ft/min)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireResidenceTime (min)
 */

void EqCalc::FireLineIntAtVectorFromPsi( void )
{
    // Access current input values
    double ros = vSurfaceFireSpreadAtPsi->m_nativeValue;
    double tau = vSurfaceFireResidenceTime->m_nativeValue;
    double rxi = vSurfaceFireReactionInt->m_nativeValue;
     // Calculate results
    double fli = FBL_SurfaceFireFirelineIntensity( ros, rxi, tau ) ;
    // Store results
    vSurfaceFireLineIntAtVector->update( fli );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireLineIntAtVectorFromPsi() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtPsi %g %s\n", Margin,
            vSurfaceFireSpreadAtPsi->m_nativeValue,
            vSurfaceFireSpreadAtPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtBack
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtBack (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtBack (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtBack( void )
{
    // Access current input values
    double bd = vSurfaceFireDistAtBack->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd / 5280.;
    // Store results
    vSurfaceFireMapDistAtBack->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtBack() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtBack %g %s\n", Margin,
            vSurfaceFireDistAtBack->m_nativeValue,
            vSurfaceFireDistAtBack->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtBack %g %s\n", Margin,
            vSurfaceFireMapDistAtBack->m_nativeValue,
            vSurfaceFireMapDistAtBack->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtBeta (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtBeta (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtBeta( void )
{
    // Access current input values
    double bd = vSurfaceFireDistAtBeta->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd / 5280.;
    // Store results
    vSurfaceFireMapDistAtBeta->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtBeta() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtBeta %g %s\n", Margin,
            vSurfaceFireDistAtBeta->m_nativeValue,
            vSurfaceFireDistAtBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtBeta %g %s\n", Margin,
            vSurfaceFireMapDistAtBeta->m_nativeValue,
            vSurfaceFireMapDistAtBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtFlank
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtFlank (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtFlank (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtFlank( void )
{
    // Access current input values
    double fd = vSurfaceFireDistAtFlank->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * fd / 5280.;
    // Store results
    vSurfaceFireMapDistAtFlank->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtFlank() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtFlank %g %s\n", Margin,
            vSurfaceFireDistAtFlank->m_nativeValue,
            vSurfaceFireDistAtFlank->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtFlank %g %s\n", Margin,
            vSurfaceFireMapDistAtFlank->m_nativeValue,
            vSurfaceFireMapDistAtFlank->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtHead
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtHead (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtHead (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtHead( void )
{
    // Access current input values
    double fd = vSurfaceFireDistAtHead->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * fd / 5280.;
    // Store results
    vSurfaceFireMapDistAtHead->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtHead() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtHead %g %s\n", Margin,
            vSurfaceFireDistAtHead->m_nativeValue,
            vSurfaceFireDistAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtHead %g %s\n", Margin,
            vSurfaceFireMapDistAtHead->m_nativeValue,
            vSurfaceFireMapDistAtHead->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtPsi (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtPsi (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtPsi( void )
{
    // Access current input values
    double vd = vSurfaceFireDistAtPsi->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * vd / 5280.;
    // Store results
    vSurfaceFireMapDistAtPsi->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtPsi() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtPsi %g %s\n", Margin,
            vSurfaceFireDistAtPsi->m_nativeValue,
            vSurfaceFireDistAtPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtPsi %g %s\n", Margin,
            vSurfaceFireMapDistAtPsi->m_nativeValue,
            vSurfaceFireMapDistAtPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMapDistAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMapDistAtVector (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireDistAtVector (ft)
 *      vMapScale (in/mi)
 */

void EqCalc::FireMapDistAtVector( void )
{
    // Access current input values
    double vd = vSurfaceFireDistAtVector->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * vd / 5280.;
    // Store results
    vSurfaceFireMapDistAtVector->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMapDistAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireDistAtVector %g %s\n", Margin,
            vSurfaceFireDistAtVector->m_nativeValue,
            vSurfaceFireDistAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMapDistAtVector %g %s\n", Margin,
            vSurfaceFireMapDistAtVector->m_nativeValue,
            vSurfaceFireMapDistAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMaxDirDiagram
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMaxDirDiagram (diagram)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireMaxDirFromNorth (degrees clockwise from North)
 *      vSurfaceFireMaxDirFromUpslope (degrees clockwise from North)
 *      vSiteAspectDirFromNorth (degrees clockwise from North)
 *      vSiteUpslopeDirFromNorth (degrees clockwise from North)
 *      vWindDirFromNorth (degrees clockwqise from North)
 *      vWindDirFromUpslope (degrees clockwqise from North)
 */

void EqCalc::FireMaxDirDiagram( void )
{
    // Access current input values
    double x = vSurfaceFireMaxDirDiagram->m_nativeValue;
    // Calculate results (dummy)
    x++;
    // Store results
    vSurfaceFireMaxDirDiagram->update( x );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMaxDirDiagram() 3 1\n",
            Margin );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromUpslope %g %d %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_displayValue,
            vSurfaceFireMaxDirFromUpslope->m_displayDecimals,
            vSurfaceFireMaxDirFromUpslope->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromNorth %g %d %s\n", Margin,
            vSurfaceFireMaxDirFromNorth->m_displayValue,
            vSurfaceFireMaxDirFromNorth->m_displayDecimals,
            vSurfaceFireMaxDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromUpslope %g %d %s\n", Margin,
            vWindDirFromUpslope->m_displayValue,
            vWindDirFromUpslope->m_displayDecimals,
            vWindDirFromUpslope->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromNorth %g %d %s\n", Margin,
            vWindDirFromNorth->m_displayValue,
            vWindDirFromNorth->m_displayDecimals,
            vWindDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSiteAspectDirFromNorth %g %d %s\n", Margin,
            vSiteAspectDirFromNorth->m_displayValue,
            vSiteAspectDirFromNorth->m_displayDecimals,
            vSiteAspectDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSiteUpslopeDirFromNorth %g %d %s\n", Margin,
            vSiteUpslopeDirFromNorth->m_displayValue,
            vSiteUpslopeDirFromNorth->m_displayDecimals,
            vSiteUpslopeDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMaxDirDiagram %g %s\n", Margin,
            vSurfaceFireMaxDirDiagram->m_nativeValue,
            vSurfaceFireMaxDirDiagram->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireMaxDirFromNorth
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireMaxDirFromNorth (degrees clockwise from north)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireMaxDirFromUpslope (degrees clockwise from upslope)
 *      vSiteUpslopeDirFromNorth (degrees clockwise from North)
 */

void EqCalc::FireMaxDirFromNorth( void )
{
    // Access current input values
    double sd = vSiteUpslopeDirFromNorth->m_nativeValue;
    double fd = vSurfaceFireMaxDirFromUpslope->m_nativeValue;
    // Calculate results
    double dd = sd + fd;
    if ( dd >= 360. )
    {
        dd -= 360.;
    }
    if ( dd < 0. )
    {
        dd += 360.;
    }
    if ( fabs( dd ) < 0.50 )
    {
        dd = 0.0;
    }
    // Store results
    vSurfaceFireMaxDirFromNorth->update( dd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireMaxcDirFromNorth() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSiteUpslopeDirFromNorth %g %s\n", Margin,
            vSiteUpslopeDirFromNorth->m_nativeValue,
            vSiteUpslopeDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromUpslope %g %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_nativeValue,
            vSurfaceFireMaxDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMaxDirFromNorth %g %s\n", Margin,
            vSurfaceFireMaxDirFromNorth->m_nativeValue,
            vSurfaceFireMaxDirFromNorth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireNoWindRate
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireNoWindRate (ft/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedHeatSink (Btu/ft3)
 *      vSurfaceFirePropagatingFlux (fraction)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 */

void EqCalc::FireNoWindRate ( void )
{
    // Access current input values
    double rbQig    = vSurfaceFuelBedHeatSink->m_nativeValue;
    double rxInt    = vSurfaceFireReactionInt->m_nativeValue;
    double propFlux = vSurfaceFirePropagatingFlux->m_nativeValue;
    // Calculate results
    double ros0 = FBL_SurfaceFireNoWindNoSlopeSpreadRate(
                    rxInt, propFlux, rbQig ) ;
    // Store results
    vSurfaceFireNoWindRate->update( ros0 );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireNoWindRate() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedHeatSink %g %s\n", Margin,
            vSurfaceFuelBedHeatSink->m_nativeValue,
            vSurfaceFuelBedHeatSink->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFirePropagatingFlux %g %s\n", Margin,
            vSurfaceFirePropagatingFlux->m_nativeValue,
            vSurfaceFirePropagatingFlux->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireNoWindRate %g %s\n", Margin,
            vSurfaceFireNoWindRate->m_nativeValue,
            vSurfaceFireNoWindRate->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FirePerimeter
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFirePerimeter (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 *      vSurfaceFireWidthDist (ft)
 */

void EqCalc::FirePerimeter( void )
{
    // Access current input values
    double ld = vSurfaceFireLengDist->m_nativeValue;
    double wd = vSurfaceFireWidthDist->m_nativeValue;
    // Calculate results
    double perim = FBL_SurfaceFirePerimeter( ld, wd );
    // Store results
    vSurfaceFirePerimeter->update( perim );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FirePerimeter() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireWidthDist %g %s\n", Margin,
            vSurfaceFireWidthDist->m_nativeValue,
            vSurfaceFireWidthDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFirePerimeter %g %s\n", Margin,
            vSurfaceFirePerimeter->m_nativeValue,
            vSurfaceFirePerimeter->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FirePropagatingFlux
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFirePropagatingFlux (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedPackingRatio (fraction)
 *      vSurfaceFuelBedSigma (ft2/ft3)
 */

void EqCalc::FirePropagatingFlux( void )
{
    // Access current input values
    double beta = vSurfaceFuelBedPackingRatio->m_nativeValue;
    double sigma = vSurfaceFuelBedSigma->m_nativeValue;
    // Calculate results
    double propFlux = FBL_SurfaceFirePropagatingFlux( beta, sigma );
    // Store results
    vSurfaceFirePropagatingFlux->update( propFlux );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FirePropagatingFlux() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedPackingRatio %g %s\n", Margin,
            vSurfaceFuelBedPackingRatio->m_nativeValue,
            vSurfaceFuelBedPackingRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedSigma %g %s\n", Margin,
            vSurfaceFuelBedSigma->m_nativeValue,
            vSurfaceFuelBedSigma->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFirePropagatingFlux %g %s\n", Margin,
            vSurfaceFirePropagatingFlux->m_nativeValue,
            vSurfaceFirePropagatingFlux->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireReactionInt
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireReactionIntDead (Btu/ft2/min)
 *      vSurfaceFireReactionIntLive (Btu/ft2/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedMextDead (fraction)
 *      vSurfaceFuelBedMoisDead (fraction)
 *      vSurfaceFuelBedMextLive (fraction)
 *      vSurfaceFuelBedMoisLive (fraction)
 */

void EqCalc::FireReactionInt( void )
{
#ifdef INCLUDE_V5_CODE
    // Access current input values
    double deadMext = vSurfaceFuelBedMextDead->m_nativeValue;
    double deadMois = vSurfaceFuelBedMoisDead->m_nativeValue;
    double liveMext = vSurfaceFuelBedMextLive->m_nativeValue;
    double liveMois = vSurfaceFuelBedMoisLive->m_nativeValue;
    // Calculate results
    double rxIntDead = 0.0;
    double rxIntLive = 0.0;
    double rxInt = FBL_SurfaceFireReactionIntensity( deadMois, deadMext,
                        liveMois, liveMext, &rxIntDead, &rxIntLive );
#elif INCLUDE_V6_CODE
	double rxInt = m_Bp6SurfaceFire->getTotalRxInt();
	double rxIntDead = m_Bp6SurfaceFire->getDeadRxInt();
	double rxIntLive = m_Bp6SurfaceFire->getLiveRxInt();
#endif
    // Store results
    vSurfaceFireReactionInt->update( rxInt );
    vSurfaceFireReactionIntDead->update( rxIntDead );
    vSurfaceFireReactionIntLive->update( rxIntLive );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireReactionIntensity() 4 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedMextDead %g %s\n", Margin,
            vSurfaceFuelBedMextDead->m_nativeValue,
            vSurfaceFuelBedMextDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedMoisDead %g %s\n", Margin,
            vSurfaceFuelBedMoisDead->m_nativeValue,
            vSurfaceFuelBedMoisDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedMextLive %g %s\n", Margin,
            vSurfaceFuelBedMextLive->m_nativeValue,
            vSurfaceFuelBedMextLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedMoisLive %g %s\n", Margin,
            vSurfaceFuelBedMoisLive->m_nativeValue,
            vSurfaceFuelBedMoisLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireResidenceTime
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireResidenceTime (min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedSigma (ft2/ft3)
 */

void EqCalc::FireResidenceTime( void )
{
    // Access current input values
    double sigma = vSurfaceFuelBedSigma->m_nativeValue;
    // Calculate results
    double rt = FBL_SurfaceFireResidenceTime( sigma );
    // Store results
    vSurfaceFireResidenceTime->update( rt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireResidenceTime() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedSigma %g %s\n", Margin,
            vSurfaceFuelBedSigma->m_nativeValue,
            vSurfaceFuelBedSigma->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireResidenceTime %g %s\n", Margin,
            vSurfaceFireResidenceTime->m_nativeValue,
            vSurfaceFireResidenceTime->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireScorchHtFromFlameLengAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireScorchHtatVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vSurfaceFireFlameLengAtVector (ft)
 *      vWindSpeedAtMidflame (mi/h)
 *
 *  Intermediate Variables
 *      bi - Byram's fireline intensity (btu/ft/s)
 */

void EqCalc::FireScorchHtFromFlameLengAtVector( void )
{
    // Access current input values
    double airTemp     = vWthrAirTemp->m_nativeValue;
    double windSpeed   = vWindSpeedAtMidflame->m_nativeValue;
    double flameLength = vSurfaceFireFlameLengAtVector->m_nativeValue;
    // Calculate results
    double firelineIntensity = FBL_SurfaceFireFirelineIntensity( flameLength );
    double scorchHt = FBL_SurfaceFireScorchHeight( firelineIntensity,
                        windSpeed, airTemp );
    // Store results
    vSurfaceFireScorchHtAtVector->update( scorchHt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireScorchHtAtVector() 3 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireScorchHtFromFliAtVector
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireScorchHtAtVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 *      vSurfaceFireFlameLeng (ft)
 */

void EqCalc::FireScorchHtFromFliAtVector( void )
{
    // Access current input values
    double airTemp           = vWthrAirTemp->m_nativeValue;
    double windSpeed         = vWindSpeedAtMidflame->m_nativeValue;
    double firelineIntensity = vSurfaceFireLineIntAtVector->m_nativeValue;
    // Calculate results
    double scorchHt = FBL_SurfaceFireScorchHeight( firelineIntensity,
                            windSpeed, airTemp );
    // Store results
    vSurfaceFireScorchHtAtVector->update( scorchHt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireScorchHtAtVector() 3 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireShapeDiagram
 *
 *  Dependent Variables (outputs)
 *      vSurfaceFireShapeDiagram (diagram)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireArea
 *      vSurfaceFireDistAtHead
 *      vSurfaceFireDistAtBack
 *      vSurfaceFireElapsedTime
 *      vSurfaceFireLengDist
 *      vSurfaceFirePerimeter
 *      vSurfaceFireMaxDirFromNorth (degrees clockwise from North)
 *      vSurfaceFireMaxDirFromUpslope (degrees clockwise from Upslope)
 *      vSurfaceFireWidthDist
 *      vSiteAspectDirFromNorth
 *      vWindDirFromUpslope
 *      vWindDirFromNorth
 */

void EqCalc::FireShapeDiagram( void )
{
    // Access current input values
    double x = vSurfaceFireShapeDiagram->m_nativeValue;
    // Calculate results (dummy)
    x++;
    // Store results
    vSurfaceFireShapeDiagram->update( x );
    // Log results
    // The DISPLAY values, decimals, and units are printed so they can
    // annotate the diagram.
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireShapeDiagram() 8 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %d %s\n", Margin,
            vSurfaceFireLengDist->m_displayValue,
            vSurfaceFireLengDist->m_displayDecimals,
            vSurfaceFireLengDist->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireWidthDist %g %d %s\n", Margin,
            vSurfaceFireWidthDist->m_displayValue,
            vSurfaceFireWidthDist->m_displayDecimals,
            vSurfaceFireWidthDist->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireArea %g %d %s\n", Margin,
            vSurfaceFireArea->m_displayValue,
            vSurfaceFireArea->m_displayDecimals,
            vSurfaceFireArea->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFirePerimeter %g %d %s\n", Margin,
            vSurfaceFirePerimeter->m_displayValue,
            vSurfaceFirePerimeter->m_displayDecimals,
            vSurfaceFirePerimeter->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElapsedTime %g %d %s\n", Margin,
            vSurfaceFireElapsedTime->m_displayValue,
            vSurfaceFireElapsedTime->m_displayDecimals,
            vSurfaceFireElapsedTime->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireDistAtHead %g %d %s\n", Margin,
            vSurfaceFireDistAtHead->m_displayValue,
            vSurfaceFireDistAtHead->m_displayDecimals,
            vSurfaceFireDistAtHead->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireDistAtBack %g %d %s\n", Margin,
            vSurfaceFireDistAtBack->m_displayValue,
            vSurfaceFireDistAtBack->m_displayDecimals,
            vSurfaceFireDistAtBack->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromUpslope %g %d %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_displayValue,
            vSurfaceFireMaxDirFromUpslope->m_displayDecimals,
            vSurfaceFireMaxDirFromUpslope->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromNorth %g %d %s\n", Margin,
            vSurfaceFireMaxDirFromNorth->m_displayValue,
            vSurfaceFireMaxDirFromNorth->m_displayDecimals,
            vSurfaceFireMaxDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromUpslope %g %d %s\n", Margin,
            vWindDirFromUpslope->m_displayValue,
            vWindDirFromUpslope->m_displayDecimals,
            vWindDirFromUpslope->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromNorth %g %d %s\n", Margin,
            vWindDirFromNorth->m_displayValue,
            vWindDirFromNorth->m_displayDecimals,
            vWindDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  i vSiteAspectDirFromNorth %g %d %s\n", Margin,
            vSiteAspectDirFromNorth->m_displayValue,
            vSiteAspectDirFromNorth->m_displayDecimals,
            vSiteAspectDirFromNorth->m_displayUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireShapeDiagram %g %s\n", Margin,
            vSurfaceFireShapeDiagram->m_nativeValue,
            vSurfaceFireShapeDiagram->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtBack
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtBack (ft/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireEccentricity (fraction)
 *      vSurfaceFireSpreadAtHead (ft/min)
 */

void EqCalc::FireSpreadAtBack( void )
{
    // Access current input values
    double eccent = vSurfaceFireEccentricity->m_nativeValue;
    double rosMax = vSurfaceFireSpreadAtHead->m_nativeValue;
    // Calculate results
    double rosBacking = FBL_SurfaceFireBackingSpreadRate( rosMax, eccent );
    // Store results
    vSurfaceFireSpreadAtBack->update( rosBacking );
    // Log results

	if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtBack() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEccentricity %g %s\n", Margin,
            vSurfaceFireEccentricity->m_nativeValue,
            vSurfaceFireEccentricity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtBack %g %s\n", Margin,
            vSurfaceFireSpreadAtBack->m_nativeValue,
            vSurfaceFireSpreadAtBack->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtVector (ft/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireEccentricity (fraction)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireVectorBeta (degrees)
 */
void EqCalc::FireSpreadAtBeta( void )
{
    // Access current input values
    double eccent = vSurfaceFireEccentricity->m_nativeValue;
    double rosMax = vSurfaceFireSpreadAtHead->m_nativeValue;
    double beta   = vSurfaceFireVectorBeta->m_nativeValue;
    // Calculate results
    double rosVec   = FBL_SurfaceFireSpreadRateAtBeta( rosMax, eccent, beta );
    // Store results
    vSurfaceFireSpreadAtBeta->update( rosVec );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtBeta() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEccentricity %g %s\n", Margin,
            vSurfaceFireEccentricity->m_nativeValue,
            vSurfaceFireEccentricity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorBeta %g %s\n", Margin,
            vSurfaceFireVectorBeta->m_nativeValue,
            vSurfaceFireVectorBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtBeta %g %s\n", Margin,
            vSurfaceFireSpreadAtBeta->m_nativeValue,
            vSurfaceFireSpreadAtBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtFlank
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtFlank (ft/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengthToWidth(fraction)
 *      vSurfaceFireSpreadAtBack (ft/min)
 *      vSurfaceFireSpreadAtHead (ft/min)
 */

void EqCalc::FireSpreadAtFlank( void )
{
    // Access current input values
    double lw = vSurfaceFireLengthToWidth->m_nativeValue;
    double head = vSurfaceFireSpreadAtHead->m_nativeValue;
    double back = vSurfaceFireSpreadAtBack->m_nativeValue;
    // Calculate results
    double flank = 0.5 * ( head + back ) / lw;
    // Store results
    vSurfaceFireSpreadAtFlank->update( flank );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtFlank() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtBack %g %s\n", Margin,
            vSurfaceFireSpreadAtBack->m_nativeValue,
            vSurfaceFireSpreadAtBack->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtFlank %g %s\n", Margin,
            vSurfaceFireSpreadAtFlank->m_nativeValue,
            vSurfaceFireSpreadAtFlank->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtHead
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireMaxDirFromUpslope (clockwise from upslope)
 *      vSurfaceFireEffWindAtHead (mi/h)
 *      vSurfaceFireWindSpeedLimit (mi/h)
 *      vSurfaceFireWindSpeedFlag (flag)
 *      vSurfaceFiewSlopeFactor (dl)
 *      vSurfaceFireWindFactor (dl)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireNoWindRate (ft/min)
 *		vSurfaceFireReactionInt (btu/ft2/min)
 *      vSiteSlopeFraction (rise/reach)
 *      vWindDirFromUpslope (degrees)
 *      vWindSpeedAtMidflame (mi/h)
 *
 *  Intermediate Variables
 *      phiEw
 */

void EqCalc::FireSpreadAtHead( void )
{
    // Access current input values
    double ros0  = vSurfaceFireNoWindRate->m_nativeValue;
    double rxi   = vSurfaceFireReactionInt->m_nativeValue;
    double slope = vSiteSlopeFraction->m_nativeValue;
    double windSpeed = vWindSpeedAtMidflame->m_nativeValue;
    double windDir = vWindDirFromUpslope->m_nativeValue;

    // We'll need to get some properties
    PropertyDict *prop = m_eqTree->m_propDict;
    bool applyWindLimit = prop->boolean( "surfaceConfWindLimitApplied" );
    bool aspenFuels = prop->boolean( "surfaceConfFuelAspen" );
    if ( aspenFuels )
    {
        applyWindLimit = false;
    }

    // Calculate results
    double dirMax = 0.;
	double effWind = 0.;
	double maxWind = 0.;
	double windFactor = 0.;
	double slopeFactor = 0.;
	double rosMax = 0.;
    int    windLimit = 0;
#ifdef INCLUDE_V5_CODE
    rosMax = FBL_SurfaceFireForwardSpreadRate( ros0, rxi, slope,
        windSpeed, windDir, &dirMax, &effWind, &maxWind, &windLimit,
        &windFactor, &slopeFactor, applyWindLimit );
#elif INCLUDE_V6_CODE
	double aspect = vSiteAspectDirFromNorth->m_nativeValue;
	m_Bp6SurfaceFire->setSite( slope, aspect, 88.*windSpeed, windDir, applyWindLimit );
	rosMax      = m_Bp6SurfaceFire->getSpreadRateAtHead();
	dirMax      = m_Bp6SurfaceFire->getHeadDirFromUpslope();
	effWind     = m_Bp6SurfaceFire->getEffectiveWindSpeed() / 88.;
	maxWind     = m_Bp6SurfaceFire->getWindSpeedLimit() / 88.;
	windLimit   = m_Bp6SurfaceFire->getWindLimitExceeded() ? 1 : 0;
	windFactor  = m_Bp6SurfaceFire->getWindFactor();
	slopeFactor = m_Bp6SurfaceFire->getSlopeFactor();
#endif

	// Store results
    vSurfaceFireSpreadAtHead->update( rosMax );
    vSurfaceFireMaxDirFromUpslope->update( dirMax );
    vSurfaceFireEffWindAtHead->update( effWind );
    vSurfaceFireWindSpeedLimit->update( maxWind );
    vSurfaceFireWindSpeedFlag->updateItem( windLimit );
    vSurfaceFireWindFactor->update( windFactor );
    vSurfaceFireSlopeFactor->update( slopeFactor );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtHead() 5 7\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireNoWindRate %g %s\n", Margin,
            vSurfaceFireNoWindRate->m_nativeValue,
            vSurfaceFireNoWindRate->m_nativeUnits.latin1() );
		logInput( vSurfaceFireReactionInt );
        fprintf( m_log, "%s  i vSiteSlopeFraction %g %s\n", Margin,
            vSiteSlopeFraction->m_nativeValue,
            vSiteSlopeFraction->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromUpslope %g %s\n", Margin,
            vWindDirFromUpslope->m_nativeValue,
            vWindDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMaxDirFromUpslope %g %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_nativeValue,
            vSurfaceFireMaxDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEffWindAtHead %g %s\n", Margin,
            vSurfaceFireEffWindAtHead->m_nativeValue,
            vSurfaceFireEffWindAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWindSpeedLimit %g %s\n", Margin,
            vSurfaceFireWindSpeedLimit->m_nativeValue,
            vSurfaceFireWindSpeedLimit->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWindSpeedFlag %g %s\n", Margin,
            vSurfaceFireWindSpeedFlag->m_nativeValue,
            vSurfaceFireWindSpeedFlag->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWindFactor %g %s\n", Margin,
            vSurfaceFireWindFactor->m_nativeValue,
            vSurfaceFireWindFactor->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSlopeFactor %g %s\n", Margin,
            vSurfaceFireSlopeFactor->m_nativeValue,
            vSurfaceFireSlopeFactor->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtPsi (ft/min)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseG (ft)
 *		vSurfaceFireEllipseH (ft)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireVectorPsi (degrees)
 *
 *  NOTE: Changed as of Build 617 to use rates rather than distances,
 *  thus eliminating the need of elapsed time as an input
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireSpreadAtBack (ft/min)
 *		vSurfaceFireSpreadAtHead( ft/min)
 *		vSurfaceFireLengthToWidth (ratio)
 *      vSurfaceFireVectorPsi (degrees)
 */
void EqCalc::FireSpreadAtPsi( void )
{
    // Access current input values
    double psi = vSurfaceFireVectorPsi->m_nativeValue;
	// Before Build 617
	double f = vSurfaceFireEllipseF->m_nativeValue;
	double g = vSurfaceFireEllipseG->m_nativeValue;
	double h = vSurfaceFireEllipseH->m_nativeValue;
	double t = vSurfaceFireElapsedTime->m_nativeValue;
	// As of Build 617
	double rosBack = vSurfaceFireSpreadAtBack->m_nativeValue;
	double rosHead = vSurfaceFireSpreadAtHead->m_nativeValue;
	double lwRatio = vSurfaceFireLengthToWidth->m_nativeValue;
	double length  = rosHead + rosBack;
	double width   = length / lwRatio;
	f = FBL_SurfaceFireEllipseF( length );
	g = FBL_SurfaceFireEllipseG( length, rosBack );
	h = FBL_SurfaceFireEllipseH( width );
	t = 1.0;

	// Calculate results
    double rosVec = FBL_SurfaceFireExpansionRateAtPsi( f, g, h, t, psi );
    // Store results
    vSurfaceFireSpreadAtPsi->update( rosVec );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtPsi() 5 1\n", Margin );
         fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            f, // vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseG %g %s\n", Margin,
            g, // vSurfaceFireEllipseG->m_nativeValue,
            vSurfaceFireEllipseG->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            h, // vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElaspedTime %g %s\n", Margin,
            t, // vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorPsi %g %s\n", Margin,
            vSurfaceFireVectorPsi->m_nativeValue,
            vSurfaceFireVectorPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtPsi %g %s\n", Margin,
            vSurfaceFireSpreadAtPsi->m_nativeValue,
            vSurfaceFireSpreadAtPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtVectorFromBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtVector (ft/min)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireEccentricity (fraction)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireVectorBeta (degrees)
 */
void EqCalc::FireSpreadAtVectorFromBeta( void )
{
    // Access current input values
    double eccent = vSurfaceFireEccentricity->m_nativeValue;
    double rosMax = vSurfaceFireSpreadAtHead->m_nativeValue;
    double beta   = vSurfaceFireVectorBeta->m_nativeValue;
    // Calculate results
    double rosVec  = FBL_SurfaceFireSpreadRateAtBeta( rosMax, eccent, beta );
    // Store results
    vSurfaceFireSpreadAtVector->update( rosVec );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtVectorFromBeta() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEccentricity %g %s\n", Margin,
            vSurfaceFireEccentricity->m_nativeValue,
            vSurfaceFireEccentricity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorBeta %g %s\n", Margin,
            vSurfaceFireVectorBeta->m_nativeValue,
            vSurfaceFireVectorBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
            vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireSpreadAtVectorFromPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireSpreadAtPsi (ft/min)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseG (ft)
 *		vSurfaceFireEllipseH (ft)
 *      vSurfaceFireElapsedTime (min)
 *      vSurfaceFireVectorPsi (degrees)
 *
 *  NOTE: Changed as of Build 617 to use rates rather than distances,
 *  thus eliminating the need of elapsed time as an input
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFireSpreadAtBack (ft/min)
 *		vSurfaceFireSpreadAtHead( ft/min)
 *		vSurfaceFireLengthToWidth (ratio)
 *      vSurfaceFireVectorPsi (degrees)
 */
void EqCalc::FireSpreadAtVectorFromPsi( void )
{
    // Access current input values
    double psi = vSurfaceFireVectorPsi->m_nativeValue;
	// Before Build 617
	double f = vSurfaceFireEllipseF->m_nativeValue;
	double g = vSurfaceFireEllipseG->m_nativeValue;
	double h = vSurfaceFireEllipseH->m_nativeValue;
	double t = vSurfaceFireElapsedTime->m_nativeValue;
	// As of Build 617
	double rosBack = vSurfaceFireSpreadAtBack->m_nativeValue;
	double rosHead = vSurfaceFireSpreadAtHead->m_nativeValue;
	double lwRatio = vSurfaceFireLengthToWidth->m_nativeValue;
	double length  = rosHead + rosBack;
	double width   = length / lwRatio;
	f = FBL_SurfaceFireEllipseF( length );
	g = FBL_SurfaceFireEllipseG( length, rosBack );
	h = FBL_SurfaceFireEllipseH( width );
	t = 1.0;

    // Calculate results
    double rosVec = FBL_SurfaceFireExpansionRateAtPsi( f, g, h, t, psi );
    // Store results
    vSurfaceFireSpreadAtVector->update( rosVec );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireSpreadAtVectorFromPsi() 5 1\n", Margin );
         fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            f, // vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseG %g %s\n", Margin,
            g, // vSurfaceFireEllipseG->m_nativeValue,
            vSurfaceFireEllipseG->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            h, // vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireElaspedTime %g %s\n", Margin,
            t, // vSurfaceFireElapsedTime->m_nativeValue,
            vSurfaceFireElapsedTime->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorPsi %g %s\n", Margin,
            vSurfaceFireVectorPsi->m_nativeValue,
            vSurfaceFireVectorPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
            vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorBetaFromTheta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorBeta (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorThta (degrees)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseG (ft)
 *		vSurfaceFireEllipseH (ft)
 */

void EqCalc::FireVectorBetaFromTheta ( void )
{
    // Access current input values
    double theta = vSurfaceFireVectorTheta->m_nativeValue;
    double f = vSurfaceFireEllipseF->m_nativeValue;
    double g = vSurfaceFireEllipseG->m_nativeValue;
    double h = vSurfaceFireEllipseH->m_nativeValue;
    // Calculate results
	double beta = FBL_SurfaceFireEllipseBetaFromThetaDegrees( f, g, h, theta );
    // Store results
    vSurfaceFireVectorBeta->update( beta );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorBetaFromTheta() 4 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseG %g %s\n", Margin,
            vSurfaceFireEllipseG->m_nativeValue,
            vSurfaceFireEllipseG->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorTheta %g %s\n", Margin,
            vSurfaceFireVectorTheta->m_nativeValue,
            vSurfaceFireVectorTheta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorBeta %g %s\n", Margin,
            vSurfaceFireVectorBeta->m_nativeValue,
            vSurfaceFireVectorBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorBetaFromUpslope
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorBeta (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireMaxDirFromUpslope (degrees)
 *      vSurfaceFireVectorDirFromUpslope (degrees)
 */
void EqCalc::FireVectorBetaFromUpslope( void )
{
    // Access current input values
    double rosMaxAz = vSurfaceFireMaxDirFromUpslope->m_nativeValue;
    double rosVecAz = vSurfaceFireVectorDirFromUpslope->m_nativeValue;
    // Calculate results
    double beta = FBL_SurfaceFireVectorBeta( rosMaxAz, rosVecAz );
    // Store restults
    vSurfaceFireVectorBeta->update( beta );
    // Log results
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorBetaFromUpslope() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromUpslope %g %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_nativeValue,
            vSurfaceFireMaxDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorDirFromUpslope %g %s\n", Margin,
            vSurfaceFireVectorDirFromUpslope->m_nativeValue,
            vSurfaceFireVectorDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorBeta %g %s\n", Margin,
            vSurfaceFireVectorBeta->m_nativeValue,
            vSurfaceFireVectorBeta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorDirFromNorth
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorDirFromNorth (degrees clockwise from North)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorDirFromCompass (compass point name)
 */

void EqCalc::FireVectorDirFromNorth( void )
{
    // Access current input values
    int id = vSurfaceFireVectorDirFromCompass->activeItemDataIndex();
    // Calculate results
    double fd = 22.5 * id;
    // Store results
    vSurfaceFireVectorDirFromNorth->update( fd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorDirFromNorth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireVectorDirFromCompass %d %s\n", Margin,
            vSurfaceFireVectorDirFromCompass->activeItemDataIndex(),
            vSurfaceFireVectorDirFromCompass->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorDirFromNorth %g %s\n", Margin,
            vSurfaceFireVectorDirFromNorth->m_nativeValue,
            vSurfaceFireVectorDirFromNorth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorDirFromUpslope
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorDirFromUpslope (degrees clockwise from upslope)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorDirFromNorth (degrees clockwise from north)
 *      vSiteUpslopeDirFromNorth (degrees clockwise from north)
 */

void EqCalc::FireVectorDirFromUpslope( void )
{
    // Access current input values
    double sd = vSiteUpslopeDirFromNorth->m_nativeValue;
    double fd = vSurfaceFireVectorDirFromNorth->m_nativeValue;
    // Calculate results
    double dd = fd - sd;
    if ( dd < 0. )
    {
        dd += 360.;
    }
    // Store results
    vSurfaceFireVectorDirFromUpslope->update( dd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorDirFromUpslope() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSiteUpslopeDirFromNorth %g %s\n", Margin,
            vSiteUpslopeDirFromNorth->m_nativeValue,
            vSiteUpslopeDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorDirFromNorth %g %s\n", Margin,
            vSurfaceFireVectorDirFromNorth->m_nativeValue,
            vSurfaceFireVectorDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorDirFromUpslope %g %s\n", Margin,
            vSurfaceFireVectorDirFromUpslope->m_nativeValue,
            vSurfaceFireVectorDirFromUpslope->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorPsiFromTheta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorPsi (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorBeta (degrees)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseH (ft)
 *      vSurfaceFireVectorTheta (degrees)
 */

void EqCalc::FireVectorPsiFromTheta ( void )
{
    // Access current input values
    double beta = vSurfaceFireVectorBeta->m_nativeValue;
    double theta = vSurfaceFireVectorTheta->m_nativeValue;
    double f = vSurfaceFireEllipseF->m_nativeValue;
    double h = vSurfaceFireEllipseH->m_nativeValue;
    // Calculate results
	double psi = FBL_SurfaceFireEllipsePsiFromThetaDegrees( f, h, theta );
    // Store results
    vSurfaceFireVectorPsi->update( psi );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorPsiFromTheta() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorTheta %g %s\n", Margin,
            vSurfaceFireVectorTheta->m_nativeValue,
            vSurfaceFireVectorTheta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorPsi %g %s\n", Margin,
            vSurfaceFireVectorPsi->m_nativeValue,
            vSurfaceFireVectorPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorPsiFromUpslope
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorPsi (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireMaxDirFromUpslope (degrees)
 *      vSurfaceFireVectorDirFromUpslope (degrees)
 */
void EqCalc::FireVectorPsiFromUpslope( void )
{
    // Access current input values
    double rosMaxAz = vSurfaceFireMaxDirFromUpslope->m_nativeValue;
    double rosVecAz = vSurfaceFireVectorDirFromUpslope->m_nativeValue;
    // Calculate results
    double psi = FBL_SurfaceFireVectorBeta( rosMaxAz, rosVecAz );
    // Store restults
    vSurfaceFireVectorPsi->update( psi );
    // Log results
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorPsiFromUpslope() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireMaxDirFromUpslope %g %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_nativeValue,
            vSurfaceFireMaxDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorDirFromUpslope %g %s\n", Margin,
            vSurfaceFireVectorDirFromUpslope->m_nativeValue,
            vSurfaceFireVectorDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorPsi %g %s\n", Margin,
            vSurfaceFireVectorPsi->m_nativeValue,
            vSurfaceFireVectorPsi->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorThetaFromBeta
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorTheta (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorBeta (degrees)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseG (ft)
 *		vSurfaceFireEllipseH (ft)
 */

void EqCalc::FireVectorThetaFromBeta ( void )
{
    // Access current input values
    double b = vSurfaceFireVectorBeta->m_nativeValue;
    double f = vSurfaceFireEllipseF->m_nativeValue;
    double g = vSurfaceFireEllipseG->m_nativeValue;
    double h = vSurfaceFireEllipseH->m_nativeValue;
    // Calculate results
	double theta = FBL_SurfaceFireEllipseThetaFromBetaDegrees( f, g, h, b );
    // Store results
    vSurfaceFireVectorTheta->update( theta );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorThetaFromBeta() 4 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseG %g %s\n", Margin,
            vSurfaceFireEllipseG->m_nativeValue,
            vSurfaceFireEllipseG->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorBeta %g %s\n", Margin,
            vSurfaceFireVectorBeta->m_nativeValue,
            vSurfaceFireVectorBeta->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorTheta %g %s\n", Margin,
            vSurfaceFireVectorTheta->m_nativeValue,
            vSurfaceFireVectorTheta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireVectorThetaFromPsi
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireVectorTheta (degrees)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireVectorPsi (degrees)
 *		vSurfaceFireEllipseF (ft)
 *		vSurfaceFireEllipseH (ft)
 */

void EqCalc::FireVectorThetaFromPsi ( void )
{
    // Access current input values
    double p = vSurfaceFireVectorPsi->m_nativeValue;
    double f = vSurfaceFireEllipseF->m_nativeValue;
    double h = vSurfaceFireEllipseH->m_nativeValue;
    // Calculate results
	double theta = FBL_SurfaceFireEllipseThetaFromPsiDegrees( f, h, p );
    // Store results
    vSurfaceFireVectorTheta->update( theta );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireVectorThetaFromPsi() 3 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireEllipseF %g %s\n", Margin,
            vSurfaceFireEllipseF->m_nativeValue,
            vSurfaceFireEllipseF->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireEllipseH %g %s\n", Margin,
            vSurfaceFireEllipseH->m_nativeValue,
            vSurfaceFireEllipseH->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireVectorPsi %g %s\n", Margin,
            vSurfaceFireVectorPsi->m_nativeValue,
            vSurfaceFireVectorPsi->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireVectorTheta %g %s\n", Margin,
            vSurfaceFireVectorTheta->m_nativeValue,
            vSurfaceFireVectorTheta->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireWidthDist
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireWidthDist (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireLengDist (ft)
 *      vSurfaceFireLengthToWidth (fraction)
 */

void EqCalc::FireWidthDist( void )
{
    // Access current input values
    double ld = vSurfaceFireLengDist->m_nativeValue;
    double lw = vSurfaceFireLengthToWidth->m_nativeValue;
    // Calculate results
    double wd = FBL_SurfaceFireWidth( ld, lw );
    // Store results
    vSurfaceFireWidthDist->update( wd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireWidthDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireLengDist %g %s\n", Margin,
            vSurfaceFireLengDist->m_nativeValue,
            vSurfaceFireLengDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireLengthToWidth %g %s\n", Margin,
            vSurfaceFireLengthToWidth->m_nativeValue,
            vSurfaceFireLengthToWidth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWidthDist %g %s\n", Margin,
            vSurfaceFireWidthDist->m_nativeValue,
            vSurfaceFireWidthDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FireWidthMapDist
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireWidthMapDist (in)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireWidthDist (ft)
 *      vMapScale (fraction)
 */

void EqCalc::FireWidthMapDist( void )
{
    // Access current input values
    double wd = vSurfaceFireWidthDist->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * wd / 5280.;
    // Store results
    vSurfaceFireWidthMapDist->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FireWidthMapDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireWidthDist %g %s\n", Margin,
            vSurfaceFireWidthDist->m_nativeValue,
            vSurfaceFireWidthDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWidthMapDist %g %s\n", Margin,
            vSurfaceFireWidthMapDist->m_nativeValue,
            vSurfaceFireWidthMapDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelAspenModel
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelBedDepth (ft)
 *      vSurfaceFuelBedMextDead (lb/lb)
 *      vSurfaceFuelAspenLoadDead1 (lb/ft2)
 *      vSurfaceFuelAspenLoadDead10 (lb/ft2)
 *      vSurfaceFuelAspenLoadLiveHerb (lb/ft2)
 *      vSurfaceFuelAspenLoadLiveWoody (lb/ft2)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelAspenCuring (%)
 *      vSurfaceFuelAspenType (discrete)
 */

void EqCalc::FuelAspenModel( void )
{
    // Access current input values
    double curing    = vSurfaceFuelAspenCuring->m_nativeValue;
    int    typeIndex = vSurfaceFuelAspenType->activeItemDataIndex();
    // Calculate
    double depth         = FBL_AspenFuelBedDepth( typeIndex, curing );
    double mextDead      = FBL_AspenFuelMextDead( typeIndex, curing );
    double loadDead1     = FBL_AspenLoadDead1( typeIndex, curing );
    double loadDead10    = FBL_AspenLoadDead10( typeIndex, curing );
    double loadLiveHerb  = FBL_AspenLoadLiveHerb( typeIndex, curing );
    double loadLiveWoody = FBL_AspenLoadLiveWoody( typeIndex, curing );
    double savrDead1     = FBL_AspenSavrDead1( typeIndex, curing );
    double savrDead10    = FBL_AspenSavrDead10( typeIndex, curing );
    double savrLiveHerb  = FBL_AspenSavrLiveHerb( typeIndex, curing );
    double savrLiveWoody = FBL_AspenSavrLiveWoody( typeIndex, curing );
    // Update
    vSurfaceFuelBedDepth->update( depth );
    vSurfaceFuelBedMextDead->update( mextDead );
    vSurfaceFuelAspenLoadDead1->update( loadDead1 );
    vSurfaceFuelAspenLoadDead10->update( loadDead10 );
    vSurfaceFuelAspenLoadLiveHerb->update( loadLiveHerb );
    vSurfaceFuelAspenLoadLiveWoody->update( loadLiveWoody );
    vSurfaceFuelAspenSavrDead1->update( savrDead1 );
    vSurfaceFuelAspenSavrDead10->update( savrDead10 );
    vSurfaceFuelAspenSavrLiveHerb->update( savrLiveHerb );
    vSurfaceFuelAspenSavrLiveWoody->update( savrLiveWoody );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelAspenModel() 2 10\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelAspenCuring %g %s\n", Margin,
            vSurfaceFuelAspenCuring->m_nativeValue,
            vSurfaceFuelAspenCuring->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelAspenType %d %s\n", Margin,
            vSurfaceFuelAspenType->activeItemDataIndex(),
            vSurfaceFuelAspenType->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedDepth %g %s\n", Margin,
            vSurfaceFuelBedDepth->m_nativeValue,
            vSurfaceFuelBedDepth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedMextDead %g %s\n", Margin,
            vSurfaceFuelBedMextDead->m_nativeValue,
            vSurfaceFuelBedMextDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenLoadDead1 %g %s\n", Margin,
            vSurfaceFuelAspenLoadDead1->m_nativeValue,
            vSurfaceFuelAspenLoadDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenLoadDead10 %g %s\n", Margin,
            vSurfaceFuelAspenLoadDead10->m_nativeValue,
            vSurfaceFuelAspenLoadDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenLoadLiveHerb %g %s\n", Margin,
            vSurfaceFuelAspenLoadLiveHerb->m_nativeValue,
            vSurfaceFuelAspenLoadLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenLoadLiveWoody %g %s\n", Margin,
            vSurfaceFuelAspenLoadLiveWoody->m_nativeValue,
            vSurfaceFuelAspenLoadLiveWoody->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenSavrDead1 %g %s\n", Margin,
            vSurfaceFuelAspenSavrDead1->m_nativeValue,
            vSurfaceFuelAspenSavrDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenSavrDead10 %g %s\n", Margin,
            vSurfaceFuelAspenSavrDead10->m_nativeValue,
            vSurfaceFuelAspenSavrDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenSavrLiveHerb %g %s\n", Margin,
            vSurfaceFuelAspenSavrLiveHerb->m_nativeValue,
            vSurfaceFuelAspenSavrLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelAspenSavrLiveWoody %g %s\n", Margin,
            vSurfaceFuelAspenSavrLiveWoody->m_nativeValue,
            vSurfaceFuelAspenSavrLiveWoody->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelAspenParms
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelDens# (lb/ft3)
 *      vSurfaceFuelHeat# (Btu/lb)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelLoad# (lb/ft2)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *      vSurfaceFuelSeff# (fraction)
 *      vSurfaceFuelStot# (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelAspenDepth (ft)
 *      vSurfaceFuelAspenLoadDead1 (lb/ft2)
 *      vSurfaceFuelAspenLoadDead10 (lb/ft2)
 *      vSurfaceFuelAspenLoadDeadFoliage (lb/ft2)
 *      vSurfaceFuelAspenLoadLitter (lb/ft2)
 *      vSurfaceFuelAspenLoadLive1 (lb/ft2)
 *      vSurfaceFuelAspenLoadLive10 (lb/ft2)
 *      vSurfaceFuelAspenLoadLiveFoliage (lb/ft2)
 */

void EqCalc::FuelAspenParms( void )
{
	// Initialize to default values
    for ( int i = 0; i < MaxParts; i++ )
    {
		vSurfaceFuelLife[i]->updateItem( FuelLifeType_DeadTimeLag );
        vSurfaceFuelDens[i]->update( 32.0 );
        vSurfaceFuelHeat[i]->update( 8000. );
		vSurfaceFuelLoad[i]->update( 0. );
		vSurfaceFuelSavr[i]->update( 30.0 );
        vSurfaceFuelSeff[i]->update( 0.010 );
        vSurfaceFuelStot[i]->update( 0.055 );
    }
    // Assign life values
    vSurfaceFuelLife[0]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 0.0 - 0.25"
    vSurfaceFuelLife[1]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 0.25 - 1.0"
    vSurfaceFuelLife[2]->updateItem( FuelLifeType_LiveHerb );    // Live herbaceous
    vSurfaceFuelLife[3]->updateItem( FuelLifeType_LiveWood );    // Live woody
    // Assign load values
    vSurfaceFuelLoad[0]->update( vSurfaceFuelAspenLoadDead1->m_nativeValue );
    vSurfaceFuelLoad[1]->update( vSurfaceFuelAspenLoadDead10->m_nativeValue );
    vSurfaceFuelLoad[2]->update( vSurfaceFuelAspenLoadLiveHerb->m_nativeValue );
    vSurfaceFuelLoad[3]->update( vSurfaceFuelAspenLoadLiveWoody->m_nativeValue );
    // Assign savr values
    vSurfaceFuelSavr[0]->update( vSurfaceFuelAspenSavrDead1->m_nativeValue );
    vSurfaceFuelSavr[1]->update( vSurfaceFuelAspenSavrDead10->m_nativeValue );
    vSurfaceFuelSavr[2]->update( vSurfaceFuelAspenSavrLiveHerb->m_nativeValue );
    vSurfaceFuelSavr[3]->update( vSurfaceFuelAspenSavrLiveWoody->m_nativeValue );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelAspenParms() 0 0\n", Margin );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelBedHeatSink
 *      Performs all computations affected by changes in any of the
 *      fuel particle moisture contents.
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelBedHeatSink (Btu/ft3)
 *      vSurfaceFuelBedMextLive (fraction)
 *      vSurfaceFuelBedMoisDead (fraction)
 *      vSurfaceFuelBedMoisLive (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedBulkDensity (lb/ft3)
 *      vSurfaceFuelBedMextDead (fraction)
 *      vSurfaceFuelMois# (fraction)
 */

void EqCalc::FuelBedHeatSink( void )
{
    // Access current input values
    double mois[MaxParts];
    for ( int p=0; p<MaxParts; p++)
    {
        mois[p] = vSurfaceFuelMois[p]->m_nativeValue;
    }
    // Calculate results
    double deadMois = 0.;
	double liveMois = 0.;
	double liveMext = 0.;
	double rbQig = 0.;
#ifdef INCLUDE_V5_CODE
    double bulkDensity = vSurfaceFuelBedBulkDensity->m_nativeValue;
    double deadMext = vSurfaceFuelBedMextDead->m_nativeValue;
    rbQig = FBL_SurfaceFuelBedHeatSink( bulkDensity, deadMext, mois,
        &deadMois, &liveMois, &liveMext );
#elif INCLUDE_V6_CODE
	// If using Rothermel & Philpot chaparral fuel models ...
	double liveMextChaparral = 0.;
    PropertyDict *prop = m_eqTree->m_propDict;
//  if ( prop->boolean( "surfaceConfFuelChaparral" ) )
//	{
//		int typeIndex = vSurfaceFuelChaparralType->activeItemDataIndex();
//		liveMextChaparral = (typeIndex==0) ? 0.65 : 0.74;
//	}
	m_Bp6SurfaceFire->setMoisture( mois, liveMextChaparral );
	deadMois = m_Bp6SurfaceFire->getDeadMois();
	liveMois = m_Bp6SurfaceFire->getLiveMois();
	liveMext = m_Bp6SurfaceFire->getLiveMextApplied();
	rbQig    = m_Bp6SurfaceFire->getRbQig();
#endif
    // Store results
    vSurfaceFuelBedMoisDead->update( deadMois );
    vSurfaceFuelBedMoisLive->update( liveMois );
    vSurfaceFuelBedMextLive->update( liveMext );
    vSurfaceFuelBedHeatSink->update( rbQig );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelBedHeatSink() 2 4\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedBulkDensity %g %s\n", Margin,
            vSurfaceFuelBedBulkDensity->m_nativeValue,
            vSurfaceFuelBedBulkDensity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedMextDead %g %s\n", Margin,
            vSurfaceFuelBedMextDead->m_nativeValue,
            vSurfaceFuelBedMextDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedHeatSink %g %s\n", Margin,
            vSurfaceFuelBedHeatSink->m_nativeValue,
            vSurfaceFuelBedHeatSink->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedMoisLive %g %s\n", Margin,
            vSurfaceFuelBedMoisLive->m_nativeValue,
            vSurfaceFuelBedMoisLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedMextLive %g %s\n", Margin,
            vSurfaceFuelBedMextLive->m_nativeValue,
            vSurfaceFuelBedMextLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedMoisDead %g %s\n", Margin,
            vSurfaceFuelBedMoisDead->m_nativeValue,
            vSurfaceFuelBedMoisDead->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelBedIntermediates
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireWindFactorB (dl)
 *      vSurfaceFireWindFactorK (dl)
 *      vSurfaceFuelBedBetaRatio (fraction)
 *      vSurfaceFuelBedBulkDensity (lbs/ft3)
 *      vSurfaceFuelBedDeadFraction (fraction)
 *		vSurfaceFuelBedLiveFraction (fraction)
 *      vSurfaceFuelBedPackingRatio (fraction)
 *      vSurfaceFuelBedSigma (ft2/ft3)
 *      vSurfaceFuelLoadDead (lbs/ft2)
 *      vSurfaceFuelLoadDeadHerb (lbs/ft2)
 *      vSurfaceFuelLoadLive (lbs/ft2)
 *      vSurfaceFuelLoadUndeadHerb (lbs/ft2)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedDepth (ft)
 *      vSurfaceFuelBedMextDead (lb/lb)
 *      vSurfaceFuelDens# (lbs/ft3)
 *      vSurfaceFuelHeat# (Btu/lb)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelLoad# (lbs/ft2)
 *      vSurfaceFuelMois# (fraction)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *      vSurfaceFuelSeff# (fraction)
 *      vSurfaceFuelStot# (fraction)
 *      vSurfaceFuelLoadTransferFraction (fraction)
 */

void EqCalc::FuelBedIntermediates ( void )
{
    // Access current input values
    double dens[MaxParts], heat[MaxParts], load[MaxParts], savr[MaxParts];
    double seff[MaxParts], stot[MaxParts], mois[MaxParts];
    int    life[MaxParts];
    double deadLoad = 0.;
    double liveLoad = 0.;
    for ( int p=0; p<MaxParts; p++ )
    {
        dens[p] = vSurfaceFuelDens[p]->m_nativeValue;
        heat[p] = vSurfaceFuelHeat[p]->m_nativeValue;
        life[p] = vSurfaceFuelLife[p]->activeItemDataIndex();
        load[p] = vSurfaceFuelLoad[p]->m_nativeValue;
        savr[p] = vSurfaceFuelSavr[p]->m_nativeValue;
        seff[p] = vSurfaceFuelSeff[p]->m_nativeValue;
        stot[p] = vSurfaceFuelStot[p]->m_nativeValue;
        mois[p] = vSurfaceFuelMois[p]->m_nativeValue;
        // Dead=0, Herb=1, Wood=2, Litter=3
        if ( life[p] == 0 || life[p] == 3 )
        {
            deadLoad += load[p];
        }
        else
        {
            liveLoad += load[p];
        }
    }
    // Fuel load transfer
    double fraction = 0.0;
    int transferEq = vSurfaceFuelLoadTransferEq->activeItemDataIndex();
    if ( transferEq )
    {
        fraction = vSurfaceFuelLoadTransferFraction->m_nativeValue;
    }
    int LiveHerb = 3;
    int DeadHerb = 5;
    if ( fraction > 0.00001 )
    {
        load[DeadHerb] = fraction * load[LiveHerb];
        load[LiveHerb] -= load[DeadHerb];
        deadLoad += load[DeadHerb];
        liveLoad -= load[DeadHerb];
    }
    // Fuel bed dead load fraction
    double totalLoad = deadLoad + liveLoad;
    double deadFraction = ( totalLoad < SMIDGEN )
                        ? ( 0. )
                        : ( deadLoad / totalLoad );

    // Store results so far
    vSurfaceFuelBedDeadFraction->update( deadFraction );
    vSurfaceFuelBedLiveFraction->update( 1. - deadFraction );
    vSurfaceFuelLoadDead->update( deadLoad );
    vSurfaceFuelLoadDeadHerb->update( load[DeadHerb] );
    vSurfaceFuelLoadLive->update( liveLoad );
    vSurfaceFuelLoadUndeadHerb->update( load[LiveHerb] );

    // Fuel bed intermediates
    double depth    = vSurfaceFuelBedDepth->m_nativeValue;
    double deadMext = vSurfaceFuelBedMextDead->m_nativeValue;

    double bulkDensity = 0.;
	double packingRatio = 0.;
	double betaRatio = 0.;
	double windB = 0.;
	double windK = 0.;
	double sigma = 0.;

#ifdef INCLUDE_V5_CODE
    sigma = FBL_SurfaceFuelBedIntermediates( depth, deadMext, MaxParts,
        life, load, savr, heat, dens, stot, seff,
        &bulkDensity, &packingRatio, &betaRatio, &windB, &windK );
    //double betaOpt = packingRatio / betaRatio;
#elif INCLUDE_V6_CODE
	m_Bp6SurfaceFire->setFuel( depth, deadMext, MaxParts,
        life, load, savr, heat, dens, stot, seff );
	sigma = m_Bp6SurfaceFire->getSigma();
	bulkDensity = m_Bp6SurfaceFire->getBulkDensity();
	packingRatio = m_Bp6SurfaceFire->getPackingRatio();
	betaRatio = m_Bp6SurfaceFire->getBetaRatio();
	windB = m_Bp6SurfaceFire->getWindB();
	windK = m_Bp6SurfaceFire->getWindK();
#endif
    // Store results
    vSurfaceFireWindFactorB->update( windB );
    vSurfaceFireWindFactorK->update( windK );
    vSurfaceFuelBedBetaRatio->update( betaRatio );
    vSurfaceFuelBedBulkDensity->update( bulkDensity );
    vSurfaceFuelBedPackingRatio->update( packingRatio );
    vSurfaceFuelBedSigma->update( sigma );

//printf( "%-4.4s %6.1f %6.4f %6.4f %6.4f %6.4f\n",
//  vSurfaceFuelBedModel->activeItemName().latin1(),
//  sigma, bulkDensity, packingRatio, betaOpt, betaRatio );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelBedIntermediates() 3 12\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelLoadTransferEq %d %s\n", Margin,
            vSurfaceFuelLoadTransferEq->activeItemDataIndex(),
            vSurfaceFuelLoadTransferEq->activeItemName().latin1() );
        logInput( vSurfaceFuelBedDepth );
        logInput( vSurfaceFuelBedMextDead );
		logOutput( vSurfaceFireWindFactorB );
		logOutput( vSurfaceFireWindFactorK );
		logOutput( vSurfaceFuelBedBetaRatio );
    	logOutput( vSurfaceFuelBedBulkDensity );
    	logOutput( vSurfaceFuelBedDeadFraction );
    	logOutput( vSurfaceFuelBedLiveFraction );
    	logOutput( vSurfaceFuelBedPackingRatio );
    	logOutput( vSurfaceFuelBedSigma );
    	logOutput( vSurfaceFuelLoadDead );
    	logOutput( vSurfaceFuelLoadDeadHerb );
    	logOutput( vSurfaceFuelLoadLive );
    	logOutput( vSurfaceFuelLoadUndeadHerb );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelBedModel
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelLoadTransferEq (category)
 *      vSurfaceFuelBedDepth (ft)
 *      vSurfaceFuelBedMextDead (fraction )
 *      vSurfaceFuelHeatDead (Btu/lb)
 *      vSurfaceFuelHeatLive (Btu/lb)
 *      vSurfaceFuelLoadDead1 (lb/ft2)
 *      vSurfaceFuelLoadDead10 (lb/ft2)
 *      vSurfaceFuelLoadDead100 (lb/ft2)
 *      vSurfaceFuelLoadLiveHerb (lb/ft2)
 *      vSurfaceFuelLoadLiveWood (lb/ft2)
 *      vSurfaceFuelSavrDead1 (ft2/ft3)
 *      vSurfaceFuelSavrLiveHerb (ft2/ft3)
 *      vSurfaceFuelSavrLiveWood (ft2/ft3)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedModel (item)
 */

void EqCalc::FuelBedModel( void )
{
    // Access current input values
    FuelModel *fm = currentFuelModel( 0 );

    // Copy values from the FuelModel into the EqTree
    vSurfaceFuelLoadTransferEq->updateItem( fm->m_transfer );

	vSurfaceFuelBedModelNumber->update( fm->m_number );	// Added in Build 607
	vSurfaceFuelBedModelCode->m_store = fm->m_name;		// Added in Build 607

    vSurfaceFuelBedDepth->update( fm->m_depth );
    vSurfaceFuelBedDepth->m_store.setNum( vSurfaceFuelBedDepth->m_displayValue,
        'f', vSurfaceFuelBedDepth->m_displayDecimals );

    vSurfaceFuelBedMextDead->update( fm->m_mext );
    vSurfaceFuelBedMextDead->m_store.setNum( vSurfaceFuelBedMextDead->m_displayValue,
        'f', vSurfaceFuelBedMextDead->m_displayDecimals );

    vSurfaceFuelHeatDead->update( fm->m_heatDead );
    vSurfaceFuelHeatDead->m_store.setNum( vSurfaceFuelHeatDead->m_displayValue,
        'f', vSurfaceFuelHeatDead->m_displayDecimals );

    vSurfaceFuelHeatLive->update( fm->m_heatLive );
    vSurfaceFuelHeatLive->m_store.setNum( vSurfaceFuelHeatLive->m_displayValue,
        'f', vSurfaceFuelHeatLive->m_displayDecimals );

    vSurfaceFuelLoadDead1->update( fm->m_load1 );
    vSurfaceFuelLoadDead1->m_store.setNum( vSurfaceFuelLoadDead1->m_displayValue,
        'f', vSurfaceFuelLoadDead1->m_displayDecimals );

    vSurfaceFuelLoadDead10->update( fm->m_load10 );
    vSurfaceFuelLoadDead10->m_store.setNum( vSurfaceFuelLoadDead10->m_displayValue,
        'f', vSurfaceFuelLoadDead10->m_displayDecimals );

    vSurfaceFuelLoadDead100->update( fm->m_load100 );
    vSurfaceFuelLoadDead100->m_store.setNum( vSurfaceFuelLoadDead100->m_displayValue,
        'f', vSurfaceFuelLoadDead100->m_displayDecimals );

    vSurfaceFuelLoadLiveHerb->update( fm->m_loadHerb );
    vSurfaceFuelLoadLiveHerb->m_store.setNum( vSurfaceFuelLoadLiveHerb->m_displayValue,
        'f', vSurfaceFuelLoadLiveHerb->m_displayDecimals );

    vSurfaceFuelLoadLiveWood->update( fm->m_loadWood );
    vSurfaceFuelLoadLiveWood->m_store.setNum( vSurfaceFuelLoadLiveWood->m_displayValue,
        'f', vSurfaceFuelLoadLiveWood->m_displayDecimals );

    vSurfaceFuelSavrDead1->update( fm->m_savr1 );
    vSurfaceFuelSavrDead1->m_store.setNum( vSurfaceFuelSavrDead1->m_displayValue,
        'f', vSurfaceFuelSavrDead1->m_displayDecimals );

    vSurfaceFuelSavrLiveHerb->update( fm->m_savrHerb );
    vSurfaceFuelSavrLiveHerb->m_store.setNum( vSurfaceFuelSavrLiveHerb->m_displayValue,
        'f', vSurfaceFuelSavrLiveHerb->m_displayDecimals );

    vSurfaceFuelSavrLiveWood->update( fm->m_savrWood );
    vSurfaceFuelSavrLiveWood->m_store.setNum( vSurfaceFuelSavrLiveWood->m_displayValue,
        'f', vSurfaceFuelSavrLiveWood->m_displayDecimals );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelBedModel() 1 13\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedModel %d %s\n", Margin,
            vSurfaceFuelBedModel->activeItemDataIndex(),
            vSurfaceFuelBedModel->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadTransferEq %d %s\n", Margin,
            vSurfaceFuelLoadTransferEq->activeItemDataIndex(),
            vSurfaceFuelLoadTransferEq->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedDepth %g %s\n", Margin,
            vSurfaceFuelBedDepth->m_nativeValue,
            vSurfaceFuelBedDepth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedMextDead %g %s\n", Margin,
            vSurfaceFuelBedMextDead->m_nativeValue,
            vSurfaceFuelBedMextDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelHeatDead %g %s\n", Margin,
            vSurfaceFuelHeatDead->m_nativeValue,
            vSurfaceFuelHeatDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelHeatLive %g %s\n", Margin,
            vSurfaceFuelHeatLive->m_nativeValue,
            vSurfaceFuelHeatLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadDead1 %g %s\n", Margin,
            vSurfaceFuelLoadDead1->m_nativeValue,
            vSurfaceFuelLoadDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadDead10 %g %s\n", Margin,
            vSurfaceFuelLoadDead10->m_nativeValue,
            vSurfaceFuelLoadDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadDead100 %g %s\n", Margin,
            vSurfaceFuelLoadDead100->m_nativeValue,
            vSurfaceFuelLoadDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadLiveHerb %g %s\n", Margin,
            vSurfaceFuelLoadLiveHerb->m_nativeValue,
            vSurfaceFuelLoadLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadLiveWood %g %s\n", Margin,
            vSurfaceFuelLoadLiveWood->m_nativeValue,
            vSurfaceFuelLoadLiveWood->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelSavrDead1 %g %s\n", Margin,
            vSurfaceFuelSavrDead1->m_nativeValue,
            vSurfaceFuelSavrDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelSavrLiveHerb %g %s\n", Margin,
            vSurfaceFuelSavrLiveHerb->m_nativeValue,
            vSurfaceFuelSavrLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelSavrLiveWood %g %s\n", Margin,
            vSurfaceFuelSavrLiveWood->m_nativeValue,
            vSurfaceFuelSavrLiveWood->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelBedParms
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelDens# (lb/ft3)
 *      vSurfaceFuelHeat# (Btu/lb)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelLoad# (lb/ft2)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *      vSurfaceFuelSeff# (fraction)
 *      vSurfaceFuelStot# (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelHeatDead (Btu/lb)
 *      vSurfaceFuelHeatLive (Btu/lb)
 *      vSurfaceFuelSavrDead1 (ft2/ft3)
 *      vSurfaceFuelSavrLiveHerb (ft2/ft3)
 *      vSurfaceFuelSavrLiveWood (ft2/ft3)
 *      vSurfaceFuelLoadDead1 (lbs/ft2)
 *      vSurfaceFuelLoadDead100 (lbs/ft2)
 *      vSurfaceFuelLoadDead100 (lbs/ft2)
 *      vSurfaceFuelLoadLiveHerb (lbs/ft2)
 *      vSurfaceFuelLoadLiveWood (lbs/ft2)
 *
 *  Notes
 *      This allows input of the traditional fuel bed parameters
 *      which are then assigned to the vSurfaceFuelLife#, vSurfaceFuelLoad#, vSurfaceFuelSavr#,
 *      vSurfaceFuelHeat#, and vSurfaceFuelDens# values.
 *      The intent is to support "custom fuel modeling".
 */

void EqCalc::FuelBedParms( void )
{
    // Give default values to all particles.
    for ( int i=0; i<MaxParts; i++ )
    {
        vSurfaceFuelDens[i]->update( 32. );
        vSurfaceFuelHeat[i]->update( 8000. );
        vSurfaceFuelLife[i]->updateItem( 0 );
        vSurfaceFuelLoad[i]->update( 0.0 );
        vSurfaceFuelSavr[i]->update( 1. );
        vSurfaceFuelStot[i]->update( 0.0555 );
        vSurfaceFuelSeff[i]->update( 0.0100 );
    }
    // Assign fixed life values
    vSurfaceFuelLife[0]->updateItem( 0 );   // 1-h
    vSurfaceFuelLife[1]->updateItem( 0 );   // 10-h
    vSurfaceFuelLife[2]->updateItem( 0 );   // 100-h
    vSurfaceFuelLife[3]->updateItem( 1 );   // live herb
    vSurfaceFuelLife[4]->updateItem( 2 );   // live wood
    vSurfaceFuelLife[5]->updateItem( 0 );   // dead herb

    // Access current input values
    // Assign load values
    vSurfaceFuelLoad[0]->update( vSurfaceFuelLoadDead1->m_nativeValue );
    vSurfaceFuelLoad[1]->update( vSurfaceFuelLoadDead10->m_nativeValue );
    vSurfaceFuelLoad[2]->update( vSurfaceFuelLoadDead100->m_nativeValue );
    vSurfaceFuelLoad[3]->update( vSurfaceFuelLoadLiveHerb->m_nativeValue );
    vSurfaceFuelLoad[4]->update( vSurfaceFuelLoadLiveWood->m_nativeValue );
    vSurfaceFuelLoad[5]->update( 0.0 );
    // Assign heat values
    vSurfaceFuelHeat[0]->update( vSurfaceFuelHeatDead->m_nativeValue );
    vSurfaceFuelHeat[1]->update( vSurfaceFuelHeatDead->m_nativeValue );
    vSurfaceFuelHeat[2]->update( vSurfaceFuelHeatDead->m_nativeValue );
    vSurfaceFuelHeat[3]->update( vSurfaceFuelHeatLive->m_nativeValue );
    vSurfaceFuelHeat[4]->update( vSurfaceFuelHeatLive->m_nativeValue );
    vSurfaceFuelHeat[5]->update( vSurfaceFuelHeatDead->m_nativeValue );
    // Assign savr values
    vSurfaceFuelSavr[0]->update( vSurfaceFuelSavrDead1->m_nativeValue );
    vSurfaceFuelSavr[1]->update( 109.0 );
    vSurfaceFuelSavr[2]->update( 30.0 );
    vSurfaceFuelSavr[3]->update( vSurfaceFuelSavrLiveHerb->m_nativeValue );
    vSurfaceFuelSavr[4]->update( vSurfaceFuelSavrLiveWood->m_nativeValue );
    vSurfaceFuelSavr[5]->update( vSurfaceFuelSavrLiveHerb->m_nativeValue );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelBedParms() 0 0\n", Margin );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelBedWeighted
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFireReactionInt (Btu/ft2/min)
 *      vSurfaceFireSpreadAtHead (ft/min)
 *      vSurfaceFireSpreadAtVector (ft/min)
 *      vSurfaceFireMaxDirFromUpslope (clockwise from upslope)
 *      vSurfaceFireEffWindAtHead (mi/h)
 *      vSurfaceFireEffWindAtVector (mi/h)
 *      vSurfaceFireWindSpeedLimit (mi/h)
 *      vSurfaceFireWindSpeedFlag (flag)
 *      vSurfaceFireHeatPerUnitArea (Btu/ft2)
 *      vSurfaceFireLineIntAtHead (Btu/ft/s)
 *      vSurfaceFireLineIntAtVector (Btu/ft/s)
 *      vSurfaceFireFlameLengIntAtHead (ft)
 *      vSurfaceFireFlameLengIntAtVector (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelBedModel1 (item)
 *      vSurfaceFuelBedModel2 (item)
 *      vSurfaceFuelBedCoverage1 (fraction)
 *      vSiteSlopeFraction (rise/reach)
 *      vWindDirFromUpslope (degrees)
 *      vWindSpeedAtMidflame (mi/h)
 *      vSurfaceFuelMoisDead1 (fraction)
 *      vSurfaceFuelMoisDead10 (fraction)
 *      vSurfaceFuelMoisDead100 (fraction)
 *      vSurfaceFuelMoisDead1000 (fraction)
 *      vSurfaceFuelMoisLiveHerb (fraction)
 *      vSurfaceFuelMoisLiveWood (fraction)
 *      vSurfaceFuelLoadTransferFraction (fraction)
 *      vSurfaceFireVectorDirFromUpslope (deg)
 */

void EqCalc::FuelBedWeighted( void )
{
    // Get the primary and secondary fuel models
    FuelModel *fm[2];
    fm[0] = currentFuelModel( 1 );
    fm[1] = currentFuelModel( 2 );

    // Get the primary and secondary fuel model coverages
    double cov[2];
    cov[0] = vSurfaceFuelBedCoverage1->m_nativeValue;
    cov[1] = 1. - cov[0];

    // We'll need to get some properties
    PropertyDict *prop = m_eqTree->m_propDict;

    //----------------------------------------
    // Determine individual fuel model outputs
    //----------------------------------------

    // Intermediate outputs for each fuel model
    double rosh[2], rosv[2];	// ros at head and vector
	double flih[2], fliv[2];	// fireline intensity at head and vector
	double flh[2], flv[2];		// flame length at head and vector
	double ewsh[2], ewsv[2];	// effective wind speed at head and vector
	double flw[2];				// fire length-to-width ratio
    double rxi[2], hua[2], mxd[2];	// reaction intensity, heat per unit area, dir of max spread
	double waf[2], wmf[2];		// wind adjustment factor and wind speed at midflame
	double wsl[2];				// wind speed limit
	int    wsf[2];				// wind speed flag
	for ( int i=0; i<2; i++ )
    {
		rosh[i] = rosv[i] = flih[i] = fliv[i] = flh[i] = flv[i] = flw[i] = 0.;
		ewsh[i] = ewsv[i] = rxi[i] = hua[i] = mxd[i] = waf[i] = wmf[i] =wsl[i] = wsf[i] = 0.;
	}

    // Calculate fire outputs for each fuel model
    for ( int i=0; i<2; i++ )
    {
        // Load the 13 fuel attributes into the equation tree's fuel model
        // This replaces the call to FuelBedModel() which normally updates these variables
        vSurfaceFuelBedDepth->update( fm[i]->m_depth );
        vSurfaceFuelBedMextDead->update( fm[i]->m_mext );
        vSurfaceFuelHeatDead->update( fm[i]->m_heatDead );
        vSurfaceFuelHeatLive->update( fm[i]->m_heatLive );
        vSurfaceFuelLoadDead1->update( fm[i]->m_load1 );
        vSurfaceFuelLoadDead10->update( fm[i]->m_load10 );
        vSurfaceFuelLoadDead100->update( fm[i]->m_load100 );
        vSurfaceFuelLoadLiveHerb->update( fm[i]->m_loadHerb );
        vSurfaceFuelLoadLiveWood->update( fm[i]->m_loadWood );
        vSurfaceFuelSavrDead1->update( fm[i]->m_savr1 );
        vSurfaceFuelSavrLiveHerb->update( fm[i]->m_savrHerb );
        vSurfaceFuelSavrLiveWood->update( fm[i]->m_savrWood );
		vSurfaceFuelLoadTransferEq->updateItem( fm[i]->m_transfer );

        // Load the equation tree's fuel model into fuel parameter arrays
        FuelBedParms();

        // Load life class moistures into the equation tree time-lag classes
        if ( prop->boolean( "surfaceConfMoisLifeCat" ) )
        {
            FuelMoisLifeClass();		// vSurfaceFuelMoisLifeDead, vSurfaceFuelMoisLifeLive
        }
        // Load dead category and live class moistures into the equation tree time-lag classes
        else if ( prop->boolean( "surfaceConfMoisDeadHerbWood" ) )
        {
            FuelMoisDeadHerbWood();		// vSurfaceFuelMoisLifeDead, vSurfaceFuelMoisLiveHerb, vSurfaceFuelMoisLiveWood
        }
        // or load moisture scenario into the equation tree time-lag classes
        else if ( prop->boolean( "surfaceConfMoisScenario" ) )
        {
            FuelMoisScenarioModel();
        }
        // Load equation tree time-lag classes into the fuel parameter array
        // begin proc FuelMoisTimeLag() 14 8
        //  i vSurfaceFuelMoisDead1
        //  i vSurfaceFuelMoisDead10
        //  i vSurfaceFuelMoisDead100
        //  i vSurfaceFuelMoisDead1000
        //  i vSurfaceFuelMoisLiveHerb
        //  i vSurfaceFuelMoisLiveWood
        //  i vSurfaceFuelLife[0..9] 
        //  i vSurfaceFuelSavr[0..9]
        //  o vSurfaceFuelMois[0..9]
        FuelMoisTimeLag();

		// Perform dynamic fuel loading adjustments
        // begin proc FuelLoadTransferFraction() 2 6
		//  i vSurfaceFuelMoisLiveHerb
        //  o vSurfaceFuelLoadTransferFraction
		FuelLoadTransferFraction();

        // begin proc FuelBedIntermediates() 3 11
		//  i vSurfaceFuelLoadTransferEq
        //  i vSurfaceFuelBedDepth
        //  i vSurfaceFuelBedMextDead
        //  o vSurfaceFuelBedBetaRatio
        //  o vSurfaceFuelBedBulkDensity
        //  o vSurfaceFuelBedPackingRatio
        //  o vSurfaceFuelBedSigma
        //  o vSurfaceFuelLoadTransferFraction ?? TODO CHECK THIS!!
        //  o vSurfaceFuelLoadDeadHerb
        //  o vSurfaceFuelLoadUndeadHerb
        //  o vSurfaceFuelLoadDead
        //  o vSurfaceFuelLoadLive
        //  o vSurfaceFuelBedDeadFraction
        //  o vSurfaceFuelBedLiveFraction
        FuelBedIntermediates();

        // begin proc FuelBedHeatSink() 2 4
        //  i vSurfaceFuelBedBulkDensity
        //  i vSurfaceFuelBedMextDead
        //  o vSurfaceFuelBedHeatSink
        //  o vSurfaceFuelBedMoisLive
        //  o vSurfaceFuelBedMextLive
        //  o vSurfaceFuelBedMoisDead
        FuelBedHeatSink();

        // begin proc FirePropagatingFlux() 2 1
        //  i vSurfaceFuelBedPackingRatio
        //  i vSurfaceFuelBedSigma
        //  o vSurfaceFirePropagatingFlux
        FirePropagatingFlux();

        // begin proc FireReactionIntensity() 4 1
		//  i vSurfaceFuelBedMextDead
        //  i vSurfaceFuelBedMoisDead
        //  i vSurfaceFuelBedMextLive
        //  i vSurfaceFuelBedMoisLive
        //  o vSurfaceFireReactionInt
        FireReactionInt();
        rxi[i] = vSurfaceFireReactionInt->m_nativeValue;

		// begin proc FireNoWindRate() 3 1
        //  i vSurfaceFuelBedHeatSink
        //  i vSurfaceFireReactionInt
        //  i vSurfaceFirePropagatingFlux
        //  o vSurfaceFireNoWindRate
        FireNoWindRate();

		// If necessary, calculate wind adjustment factor from canopy and fuel parameters
		if ( prop->boolean( "surfaceConfWindSpeedAt10MCalc" )
		  || prop->boolean( "surfaceConfWindSpeedAt20FtCalc" ) )
		{
			WindAdjFactor();
		}
		waf[i] = vWindAdjFactor->m_nativeValue;

		// If necessary, calculate 20-ft wind speed from 10-m wind speed
		if ( prop->boolean( "surfaceConfWindSpeedAt10M" )
		  || prop->boolean( "surfaceConfWindSpeedAt10MCalc" ) )
		{
			WindSpeedAt20Ft();
		}

		// If necessary, calculate midflame wind speed from 20-ft wind speed and wind adj factor
		if ( ! prop->boolean( "surfaceConfWindSpeedAtMidflame" ) )
		{
			WindSpeedAtMidflame();
		}
		wmf[i] = vWindSpeedAtMidflame->m_nativeValue;

        // begin proc FireSpreadAtHead() 4 5
        //  i vSurfaceFireNoWindRate
        //  i vSiteSlopeFraction
        //  i vWindDirFromUpslope
        //  i vWindSpeedAtMidflame
        //  o vSurfaceFireSpreadAtHead
        //  o vSurfaceFireMaxDirFromUpslope
        //  o vSurfaceFireEffWindAtHead
        //  o vSurfaceFireWindSpeedLimit
        //  o vSurfaceFireWindSpeedFlag
        //  o vSurfaceFireWindFactor
        //  o vSurfaceFireWindFactorB
        //  o vSurfaceFireWindFactorK
        //  o vSurfaceFireSlopeFactor
        FireSpreadAtHead();
        rosh[i] = vSurfaceFireSpreadAtHead->m_nativeValue;
        mxd[i]  = vSurfaceFireMaxDirFromUpslope->m_nativeValue;
        ewsh[i] = vSurfaceFireEffWindAtHead->m_nativeValue;
        wsl[i]  = vSurfaceFireWindSpeedLimit->m_nativeValue;
		wsf[i]  = ( wmf[i] > wsl[i] ) ? 1 : 0;

        // begin proc FireDistAtHead() 2 1
		//  i vSurfaceFireSpreadAtHead
        //  i vSurfaceFireElapsedTime
        //  o vSurfaceFireDistAtHead
		FireDistAtHead();

        // begin proc FireLengthToWidth() 1 1
		//  i vSurfaceFireEffWindAtHead
        //  o vSurfaceFireLengthToWidth
        FireLengthToWidth();
		flw[i] = vSurfaceFireLengthToWidth->m_nativeValue;

        // begin proc FireEccentricity() 1 1
        //  i vSurfaceFireLengthToWidth
        //  o vSurfaceFireEccentricity
        FireEccentricity();

		// Calculate beta and psi vectors
		bool psi_fli = true;
		bool psi_ros = true;
		bool beta_vector = prop->boolean( "surfaceConfSpreadDirPointSourcePsi" )
		  || prop->boolean( "surfaceConfSpreadDirPointSourceBeta" );

		if ( beta_vector )
		{
			// The upslope direction IS beta
			// begin proc FireVectorBetaFromUpslope() 2 1
			//  i vSurfaceFireMaxDirFromUpslope
			//  i vSurfaceFireVectorDirFromUpslope
			//  o vSurfaceFireVectorBeta
			FireVectorBetaFromUpslope();

			psi_ros = false;
			psi_fli = prop->boolean( "surfaceConfSpreadDirPointSourcePsi" );
		}
		else	// Psi-based directions
		{
			if ( prop->boolean( "surfaceConfSpreadDirHead" ) )
			{
 				vSurfaceFireVectorPsi->update( 0. );
			}
			else if ( prop->boolean( "surfaceConfSpreadDirBack" ) )
			{
 				vSurfaceFireVectorPsi->update( 180. );
			}
			else if ( prop->boolean( "surfaceConfSpreadDirFlank" ) )
			{
	 			vSurfaceFireVectorPsi->update( 90. );
			}
			else if ( prop->boolean( "surfaceConfSpreadDirFireFront" ) )
			{
		        // begin proc FireVectorPsiFromUpslope() 2 1
				//  i vSurfaceFireMaxDirFromUpslope
				//  i vSurfaceFireVectorDirFromUpslope
				//  o vSurfaceFireVectorPsi
				FireVectorPsiFromUpslope();
			}

			// begin proc FireVectorThetaFromPsi() 3 1
			//  i vSurfaceFireEllipseF
			//  i vSurfaceFireEllipseH
			//  i vSurfaceFireVectorPsi
			//  o vSurfaceFireVectorTheta
			FireVectorThetaFromPsi();

			// begin proc FireVectorBetaFromTheta() 4 1
			//  i vSurfaceFireEllipseF
			//  i vSurfaceFireEllipseG
			//  i vSurfaceFireEllipseH
			//  i vSurfaceFireVectorTheta
			//  o vSurfaceFireVectorBeta
			FireVectorBetaFromTheta();
		}

		// begin proc FireSpreadAtVectorFromBeta() 3 1
		//  i vSurfaceFireEccentricity
		//  i vSurfaceFireSpreadAtHead
		//  i vSurfaceFireVectorBeta
		//  o vSurfaceFireSpreadAtVector
		FireSpreadAtVectorFromBeta();
        rosv[i] = vSurfaceFireSpreadAtVector->m_nativeValue;

		// Calculate the fire spread from ignition point (BETA)
		// begin proc FireSpreadAtBeta() 3 1
        //  i vSurfaceFireEccentricity
        //  i vSurfaceFireSpreadAtHead
        //  i vSurfaceFireVectorBeta
        //  o vSurfaceFireSpreadAtBeta
		FireSpreadAtBeta();

        // begin proc FireSpreadAtBack() 2 1
        //  i vSurfaceFireEccentricity
        //  i vSurfaceFireSpreadAtHead
        //  o vSurfaceFireSpreadAtBack
		FireSpreadAtBack();

        // begin proc FireDistAtBack() 2 1
        //  i vSurfaceFireSpreadAtBack
        //  i vSurfaceFireElapsedTime
        //  o vSurfaceFireDistAtBack
		FireDistAtBack();

        // begin proc FireLengDist() 2 1
        //  i vSurfaceFireDistAtBack
        //  i vSurfaceFireDistAtHead
        //  o vSurfaceFireLengDist
		FireLengDist();

        // begin proc FireEllipseF() 1 1
        //  i vSurfaceFireLengDist
        //  o vSurfaceFireEllipseF
		FireEllipseF();

        // begin proc FireEllipseG() 2 1
        //  i vSurfaceFireLengDist
        //  i vSurfaceFireDistAtBack
        //  o vSurfaceFireEllipseG
		FireEllipseG();

        // begin proc FireWidthDist() 2 1
        //  i vSurfaceFireLengDist
        //  i vSurfaceFireLengthToWidth
        //  o vSurfaceFireWidthDist
		FireWidthDist();

        // begin proc FireEllipseH() 1 1
        //  i vSurfaceFireWidthDist
        //  o vSurfaceFireEllipseH
		FireEllipseH();

		// If using Beta vectors, need to derive Psi
		if ( beta_vector )
		{
			// begin proc FireVectorThetaFromBeta() 4 1
			//  i vSurfaceFireEllipseF
			//  i vSurfaceFireEllipseG
			//  i vSurfaceFireEllipseH
			//  i vSurfaceFireVectorBeta
			//  o vSurfaceFireVectorTheta
			FireVectorThetaFromBeta();

			// begin proc FireVectorPsiFromTheta() 3 1
			//  i vSurfaceFireEllipseF
			//  i vSurfaceFireEllipseH
			//  i vSurfaceFireVectorTheta
			//  o vSurfaceFireVectorPsi
			FireVectorPsiFromTheta();
		}
		else
		{
		}

        //begin proc FireSpreadAtPsi() 4 1
        //  i vSurfaceFireEllipseF
        //  i vSurfaceFireEllipseG
        //  i vSurfaceFireEllipseH
        //  i vSurfaceFireElaspedTime
		//  i vSurfaceFireVectorPsi
        //  o vSurfaceFireSpreadAtPsi
		FireSpreadAtPsi();

		// Calculate and store fire spread at vector based upon confiuration
		if ( psi_ros )
		{
			// begin proc FireSpreadAtVectorFromPsi() 4 1
		    //  i vSurfaceFireEllipseF
			//  i vSurfaceFireEllipseG
			//  i vSurfaceFireEllipseH
	        //  i vSurfaceFireElaspedTime
		    //  o vSurfaceFireSpreadAtVector
			FireSpreadAtVectorFromPsi();
	        rosv[i] = vSurfaceFireSpreadAtVector->m_nativeValue;
		}

        // Calculate fire residence time for FLI computations
        //  i vSurfaceFuelBedSigma
        //  o vSurfaceFireResidenceTime
        FireResidenceTime();

        // Calculate and store the fireline intensity at the requested vector
		if ( psi_fli )
		{
			//begin proc FireLineIntAtVectorFromPsi() 3 1
			//  i vSurfaceFireSpreadAtPsi
			//  i vSurfaceFireResidenceTime
			//  i vSurfaceFireReactionInt
			//  o vSurfaceFireLineIntAtVector
			FireLineIntAtVectorFromPsi();
	        fliv[i] = vSurfaceFireLineIntAtVector->m_nativeValue;
			//FireLineIntAtPsi();
			//fliv[i] = vSurfaceFireLineIntAtPsi->m_nativeValue;
		}
		else
		{

			// begin proc FireLineIntAtVectorFromBeta() 3 1
			//  i vSurfaceFireSpreadAtBeta
			//  i vSurfaceFireResidenceTime
			//  i vSurfaceFireReactionInt
			//  o vSurfaceFireLineIntAtVector
			FireLineIntAtVectorFromBeta();
	        fliv[i] = vSurfaceFireLineIntAtVector->m_nativeValue;
			// FireLineIntAtBeta();
			// fliv[i] = vSurfaceFireLineIntAtBeta->m_nativeValue;
		}

        // Calculate and store vSurfaceFireHeatPerUnitArea
        // begin proc FireHeatPerUnitArea() 2 1
        //  i vSurfaceFireReactionInt
        //  i vSurfaceFireResidenceTime
        //  o vSurfaceFireHeatPerUnitArea
        FireHeatPerUnitArea();
        hua[i] = vSurfaceFireHeatPerUnitArea->m_nativeValue;

		// Calculate vSurfaceFireLineIntAtHead
        FireLineIntAtHead();
        flih[i] = vSurfaceFireLineIntAtHead->m_nativeValue;

		// Calculate beta and psi spread, fireline intensity, and flame length
        // begin proc FireLineIntAtBeta() 3 1
        //  i vSurfaceFireSpreadAtBeta
        //  i vSurfaceFireResidenceTime
        //  i vSurfaceFireReactionInt
        //  o vSurfaceFireLineIntAtBeta
		FireLineIntAtBeta();

        //begin proc FireLineIntAtPsi() 3 1
        //  i vSurfaceFireSpreadAtPsi
        //  i vSurfaceFireResidenceTime
        //  i vSurfaceFireReactionInt
        //  o vSurfaceFireLineIntAtPsi
		FireLineIntAtPsi();

        // Calculate vSurfaceFireFlameLengAtHead
        FireFlameLengAtHead();
        flh[i] = vSurfaceFireFlameLengAtHead->m_nativeValue;

        // begin proc FireFlameLengAtVector() 1 1
        //  i vSurfaceFireLineIntAtVector
        //  o vSurfaceFireFlameLengAtVector
        FireFlameLengAtVector();
        flv[i] = vSurfaceFireFlameLengAtVector->m_nativeValue;

        // begin proc FireFlameLengAtBeta() 1 1
        //  i vSurfaceFireLineIntAtBeta
        //  o vSurfaceFireFlameLengAtBeta
		FireFlameLengAtBeta();

        // begin proc FireFlameLengAtPsi() 1 1
        //  i vSurfaceFireLineIntAtPsi
        //  o vSurfaceFireFlameLengAtPsi
		FireFlameLengAtPsi();

        // Calculate vSurfaceFireEffWindAtVector
		// DO WE NEED THIS ANYMORE??
        FireEffWindAtVector();
        ewsv[i] = vSurfaceFireEffWindAtVector->m_nativeValue;
    }

    //------------------------------------------------
    // Determine and store combined fuel model outputs
    //------------------------------------------------

    // Fire spread rate depends upon the weighting method...
    double wtdh = 0.;	// value at head
    double wtdv = 0.;	// value at vector
    double wtd  = 0.;	// anything
    // If area weighted spread rate ...
    if ( prop->boolean( "surfaceConfFuelAreaWeighted" ) )
    {
        wtdh = ( cov[0] * rosh[0] ) + ( cov[1] * rosh[1] ) ;
        wtdv = ( cov[0] * rosv[0] ) + ( cov[1] * rosv[1] ) ;
    }
    // else if harmonic mean spread rate...
    else if ( prop->boolean( "surfaceConfFuelHarmonicMean" ) )
    {
        wtdv = 0.0;
        wtdh = 0.0;
        if ( rosh[0] > 0.000001 && rosh[1] > 0.000001 )
        {
            wtdh = 1. / ( ( cov[0] / rosh[0] ) + ( cov[1] / rosh[1] ) ) ;
            wtdv = 1. / ( ( cov[0] / rosv[0] ) + ( cov[1] / rosv[1] ) ) ;
        }
    }
    // else if Finney's 2-dimensional spread rate...
    else if ( prop->boolean( "surfaceConfFuel2Dimensional" ) )
    {
        double lbRatio = vSurfaceFireLengthToWidth->m_nativeValue;
        int samples  = prop->integer( "surfaceConfFuel2DSamples" );
        int depth    = prop->integer( "surfaceConfFuel2DDepth" );
        int laterals = prop->integer( "surfaceConfFuel2DLaterals" );
        wtdh = FBL_SurfaceFireExpectedSpreadRate( rosh, cov, 2, lbRatio,
                    samples, depth, laterals );
        wtdv = FBL_SurfaceFireExpectedSpreadRate( rosv, cov, 2, lbRatio,
                    samples, depth, laterals );
    }
    vSurfaceFireSpreadAtHead->update( wtdh );
    vSurfaceFireSpreadAtVector->update( wtdv );

    // The following assignments are based on Pat's rules:
	// If only 1 fuel is present (whether primary or secondary), use its values exclusively
	if ( cov[0] > 0.999 || cov[1] > 0.999)
	{
		int i = ( cov[0] > 0.999 ) ? 0 : 1;
		vSurfaceFireReactionInt->update( rxi[i] );
		vSurfaceFireMaxDirFromUpslope->update( mxd[i] );
		vWindAdjFactor->update( waf[i] );
		vWindSpeedAtMidflame->update( wmf[i] );
	    vSurfaceFireEffWindAtHead->update( ewsh[i] );
		vSurfaceFireEffWindAtVector->update( ewsv[i] );
		vSurfaceFireWindSpeedLimit->update( wsl[i] );
		vSurfaceFireWindSpeedFlag->updateItem( wsf[i] );
		vSurfaceFireLengthToWidth->update( flw[i] );
		vSurfaceFireHeatPerUnitArea->update( hua[i] );
		vSurfaceFireLineIntAtHead->update( flih[i] );
		vSurfaceFireLineIntAtVector->update( fliv[i] );
		vSurfaceFireFlameLengAtHead->update( flh[i] );
	    vSurfaceFireFlameLengAtVector->update( flv[i] );
		vSurfaceFuelBedDepth->update( fm[i]-> m_depth );
	}
	// Otherwise the wtd value depends upon Pat's criteria; could be wtd, min, max, or primary
	else
	{
		// Reaction intensity is the maximum of the two models
		wtd = ( rxi[0] > rxi[1] ) ? rxi[0] : rxi[1];
		vSurfaceFireReactionInt->update( wtd );

		// Direction of maximum spread is for the FIRST (not necessarily dominant) fuel model
		vSurfaceFireMaxDirFromUpslope->update( mxd[0] );

	    // Wind adjustment factor is for the FIRST (not necessarily dominant) fuel model
		vWindAdjFactor->update( waf[0] );

	    // Midflame wind speed is for the FIRST (not necessarily dominant) fuel model
		vWindSpeedAtMidflame->update( wmf[0] );

	    // Effective wind speed is for the FIRST (not necessarily dominant) fuel model
		vSurfaceFireEffWindAtHead->update( ewsh[0] );
		vSurfaceFireEffWindAtVector->update( ewsv[0] );

		// Maximum reliable wind speed is the minimum of the two models
		wtd = ( wsl[0] < wsl[1] ) ? wsl[0] : wsl[1];
		vSurfaceFireWindSpeedLimit->update( wtd );

	    // If either wind limit is exceeded, set the flag
		vSurfaceFireWindSpeedFlag->updateItem( wsf[0] || wsf[1] );

		// Fire length-to-width ratio is for the FIRST (not necessarily dominant) fuel model
		vSurfaceFireLengthToWidth->update( flw[0] );

	    // Heat per unit area is the maximum of the two models
		wtd = ( hua[0] > hua[1] ) ? hua[0] : hua[1];
		vSurfaceFireHeatPerUnitArea->update( wtd );

		// Fireline intensity is the maximum of the two models
		wtdh = ( flih[0] > flih[1] ) ? flih[0] : flih[1];
		wtdv = ( fliv[0] > fliv[1] ) ? fliv[0] : fliv[1];
		vSurfaceFireLineIntAtHead->update( wtdh );
		vSurfaceFireLineIntAtVector->update( wtdv );

	    // Flame length is the maximum of the two models
		wtdh = ( flh[0] > flh[1] ) ? flh[0] : flh[1];
	    wtdv = ( flv[0] > flv[1] ) ? flv[0] : flv[1];
		vSurfaceFireFlameLengAtHead->update( wtdh );
	    vSurfaceFireFlameLengAtVector->update( wtdv );

	    // Fuel bed depth is the maximum of the two fuel bed depths
		wtd = ( fm[0]->m_depth > fm[1]-> m_depth ) ? fm[0]->m_depth : fm[1]->m_depth;
		vSurfaceFuelBedDepth->update( wtd );
	}
    //------------
    // Log results
    //------------

    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelBedWeighted() 6 13\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelBedModel1 %d %s\n", Margin,
            vSurfaceFuelBedModel1->activeItemDataIndex(),
            vSurfaceFuelBedModel1->activeItemName().latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedModel2 %d %s\n", Margin,
            vSurfaceFuelBedModel2->activeItemDataIndex(),
            vSurfaceFuelBedModel2->activeItemName().latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedCoverage1 %g %s\n", Margin,
            vSurfaceFuelBedCoverage1->m_nativeValue,
            vSurfaceFuelBedCoverage1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteSlopeFraction %g %s\n", Margin,
            vSiteSlopeFraction->m_nativeValue,
            vSiteSlopeFraction->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromUpslope %g %s\n", Margin,
            vWindDirFromUpslope->m_nativeValue,
            vWindDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );

        fprintf( m_log, "%s  o vSurfaceFireReactionInt %g %s\n", Margin,
            vSurfaceFireReactionInt->m_nativeValue,
            vSurfaceFireReactionInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtHead %g %s\n", Margin,
            vSurfaceFireSpreadAtHead->m_nativeValue,
            vSurfaceFireSpreadAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireSpreadAtVector %g %s\n", Margin,
            vSurfaceFireSpreadAtVector->m_nativeValue,
            vSurfaceFireSpreadAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireMaxDirFromUpslope %g %s\n", Margin,
            vSurfaceFireMaxDirFromUpslope->m_nativeValue,
            vSurfaceFireMaxDirFromUpslope->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEffWindAtHead %g %s\n", Margin,
            vSurfaceFireEffWindAtHead->m_nativeValue,
            vSurfaceFireEffWindAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireEffWindAtVector %g %s\n", Margin,
            vSurfaceFireEffWindAtVector->m_nativeValue,
            vSurfaceFireEffWindAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWindSpeedLimit %g %s\n", Margin,
            vSurfaceFireWindSpeedLimit->m_nativeValue,
            vSurfaceFireWindSpeedLimit->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireWindSpeedFlag %g %s\n", Margin,
            vSurfaceFireWindSpeedFlag->m_nativeValue,
            vSurfaceFireWindSpeedFlag->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireHeatPerUnitArea %g %s\n", Margin,
            vSurfaceFireHeatPerUnitArea->m_nativeValue,
            vSurfaceFireHeatPerUnitArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtHead %g %s\n", Margin,
            vSurfaceFireLineIntAtHead->m_nativeValue,
            vSurfaceFireLineIntAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireLineIntAtVector %g %s\n", Margin,
            vSurfaceFireLineIntAtVector->m_nativeValue,
            vSurfaceFireLineIntAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtHead %g %s\n", Margin,
            vSurfaceFireFlameLengAtHead->m_nativeValue,
            vSurfaceFireFlameLengAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief FuelChaparralAgeFromDepthType
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelChaparralAge (years)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelChaparralDepth( ft)
 *      vSurfaceFuelChaparralType (discrete)
 */
void EqCalc::FuelChaparralAgeFromDepthType( void )
{
    // Access current input values
    double depth = vSurfaceFuelChaparralDepth->m_nativeValue;
    int    typeIndex = vSurfaceFuelChaparralType->activeItemDataIndex();
	// Calculate result
	double age = FBL_ChaparralAgeFromDepth( depth, (typeIndex == 0) );
    // Update
    vSurfaceFuelChaparralAge->update( age );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralAgeFromDepthType() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralDepth %g %s\n", Margin,
            vSurfaceFuelChaparralDepth->m_nativeValue,
            vSurfaceFuelChaparralDepth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralType %d %s\n", Margin,
            vSurfaceFuelChaparralType->activeItemDataIndex(),
            vSurfaceFuelChaparralType->activeItemName().latin1() );
		fprintf( m_log, "%s  o vSurfaceFuelChaparralAge %g %s\n", Margin,
            vSurfaceFuelChaparralAge->m_nativeValue,
            vSurfaceFuelChaparralAge->m_nativeUnits.latin1() );
	}
	return;
}
//------------------------------------------------------------------------------
/*! \brief FuelChaparralDeadFuelFractionFromAge
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelChaparralDeadFuelFraction (ratio)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelChaparralAge (years)
 */
void EqCalc::FuelChaparralDeadFuelFractionFromAge( void )
{
    // Access current input values
    double age = vSurfaceFuelChaparralAge->m_nativeValue;
	// Calculate result
	double fraction = FBL_ChaparralDeadFuelFraction( age );
    // Update
    vSurfaceFuelChaparralDeadFuelFraction->update( fraction );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralDeadFuelFractionFromAge() 1 1\n", Margin );
		fprintf( m_log, "%s  1 vSurfaceFuelChaparralAge %g %s\n", Margin,
            vSurfaceFuelChaparralAge->m_nativeValue,
            vSurfaceFuelChaparralAge->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralDeadFuelFraction %g %s\n", Margin,
            vSurfaceFuelChaparralDeadFuelFraction->m_nativeValue,
            vSurfaceFuelChaparralDeadFuelFraction->m_nativeUnits.latin1() );
	}
	return;
}
//------------------------------------------------------------------------------
/*! \brief FuelChaparralDepthFromAge
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelChaparralDepth (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelChaparralAge (years)
 *		vSurfaceFuelChaparralType (discrete)
 */
void EqCalc::FuelChaparralDepthFromAge( void )
{
    // Access current input values
    double age = vSurfaceFuelChaparralAge->m_nativeValue;
    int    typeIndex = vSurfaceFuelChaparralType->activeItemDataIndex();
	// Calculate result
	double depth = FBL_ChaparralDepth( age, (typeIndex == 0) );
    // Update
    vSurfaceFuelChaparralDepth->update( depth );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralDepthFromAge() 2 1\n", Margin );
		fprintf( m_log, "%s  1 vSurfaceFuelChaparralAge %g %s\n", Margin,
            vSurfaceFuelChaparralAge->m_nativeValue,
            vSurfaceFuelChaparralAge->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralType %d %s\n", Margin,
            vSurfaceFuelChaparralType->activeItemDataIndex(),
            vSurfaceFuelChaparralType->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralDepth %g %s\n", Margin,
            vSurfaceFuelChaparralDepth->m_nativeValue,
            vSurfaceFuelChaparralDepth->m_nativeUnits.latin1() );
	}
	return;
}
//------------------------------------------------------------------------------
/*! \brief FuelChaparralLoadTotalFromAgeType
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelChaparralLoadTotal (lb/ft2)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelChaparralAge (years)
 *		vSurfaceFuelChaparralType (discrete)
 */
void EqCalc::FuelChaparralLoadTotalFromAgeType( void )
{
    // Access current input values
    double age = vSurfaceFuelChaparralAge->m_nativeValue;
    int    typeIndex = vSurfaceFuelChaparralType->activeItemDataIndex();
	// Calculate result
	double load = FBL_ChaparralTotalFuelLoad( age, (typeIndex == 0) );
    // Update
    vSurfaceFuelChaparralLoadTotal->update( load );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralLoadTotalFromAgeType() 2 1\n", Margin );
		fprintf( m_log, "%s  1 vSurfaceFuelChaparralAge %g %s\n", Margin,
            vSurfaceFuelChaparralAge->m_nativeValue,
            vSurfaceFuelChaparralAge->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralType %d %s\n", Margin,
            vSurfaceFuelChaparralType->activeItemDataIndex(),
            vSurfaceFuelChaparralType->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadTotal %g %s\n", Margin,
            vSurfaceFuelChaparralLoadTotal->m_nativeValue,
            vSurfaceFuelChaparralLoadTotal->m_nativeUnits.latin1() );
	}
	return;
}
//------------------------------------------------------------------------------
/*! \brief FuelChaparralModel
 *
 *  Dependent Variables (Outputs)
 *		vSurfaceFuelChaparralLoadDead1 (lb/ft2)
 *		vSurfaceFuelChaparralLoadDead2 (lb/ft2)
 *		vSurfaceFuelChaparralLoadDead3 (lb/ft2)
 *		vSurfaceFuelChaparralLoadDead4 (lb/ft2)
 *		vSurfaceFuelChaparralLoadLiveLeaf (lb/ft2)
 *		vSurfaceFuelChaparralLoadLive1 (lb/ft2)
 *		vSurfaceFuelChaparralLoadLive2 (lb/ft2)
 *		vSurfaceFuelChaparralLoadLive3 (lb/ft2)
 *		vSurfaceFuelChaparralLoadLive4 (lb/ft2)
 *		vSurfaceFuelChaparralLoadTotalDead (lb/ft2)
 *		vSurfaceFuelChaparralLoadTotalLive (lb/ft2)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFuelChaparralDeadFuelFraction (fraction)
 *		vSurfaceFuelChaparralLoadTotal (lb/ft2)
*/

void EqCalc::FuelChaparralModel( void )
{
    // Access current input values
	double fraction  = vSurfaceFuelChaparralDeadFuelFraction->m_nativeValue;
	double totalLoad = vSurfaceFuelChaparralLoadTotal->m_nativeValue;
    // Calculate
	double totalDead = fraction * totalLoad;
	double totalLive = ( 1. - fraction ) * totalLoad;
    double loadDead1 = FBL_ChaparralLoadDead( totalLoad, fraction, 0 );
    double loadDead2 = FBL_ChaparralLoadDead( totalLoad, fraction, 1 );
    double loadDead3 = FBL_ChaparralLoadDead( totalLoad, fraction, 2 );
    double loadDead4 = FBL_ChaparralLoadDead( totalLoad, fraction, 3 );
    double loadLiveLeaf = FBL_ChaparralLoadLive( totalLoad, fraction, 0 );
    double loadLive1 = FBL_ChaparralLoadLive( totalLoad, fraction, 1 );
    double loadLive2 = FBL_ChaparralLoadLive( totalLoad, fraction, 2 );
    double loadLive3 = FBL_ChaparralLoadLive( totalLoad, fraction, 3 );
    double loadLive4 = FBL_ChaparralLoadLive( totalLoad, fraction, 4 );
	// Sanity checks
	double sanityDead = loadDead1 + loadDead2 + loadDead3 + loadDead4;
	double sanityLive = loadLive1 + loadLive2 + loadLive3 + loadLive4 + loadLiveLeaf;
	loadLive4 = totalLive - loadLiveLeaf - loadLive1 - loadLive2 - loadLive3;
	loadLive4 = (loadLive4 < 0. ) ? 0. : loadLive4;
	double sanityTotal = sanityDead + sanityLive;
    // Update
    vSurfaceFuelChaparralLoadTotalDead->update( totalLoad * fraction );
    vSurfaceFuelChaparralLoadTotalLive->update( totalLoad * ( 1. - fraction ) );
    vSurfaceFuelChaparralLoadDead1->update( loadDead1 );
    vSurfaceFuelChaparralLoadDead2->update( loadDead2 );
    vSurfaceFuelChaparralLoadDead3->update( loadDead3 );
    vSurfaceFuelChaparralLoadDead4->update( loadDead4 );
    vSurfaceFuelChaparralLoadLive1->update( loadLive1 );
    vSurfaceFuelChaparralLoadLive2->update( loadLive2 );
    vSurfaceFuelChaparralLoadLive3->update( loadLive3 );
    vSurfaceFuelChaparralLoadLive4->update( loadLive4 );
    vSurfaceFuelChaparralLoadLiveLeaf->update( loadLiveLeaf );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralModel() 2 11\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralDeadFuelFraction %g %s\n", Margin,
            vSurfaceFuelChaparralDeadFuelFraction->m_nativeValue,
            vSurfaceFuelChaparralDeadFuelFraction->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelChaparralLoadTotal %g %s\n", Margin,
            vSurfaceFuelChaparralLoadTotal->m_nativeValue,
            vSurfaceFuelChaparralLoadTotal->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadDead1 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadDead1->m_nativeValue,
            vSurfaceFuelChaparralLoadDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadDead2 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadDead2->m_nativeValue,
            vSurfaceFuelChaparralLoadDead2->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadDead3 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadDead3->m_nativeValue,
            vSurfaceFuelChaparralLoadDead3->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadDead4 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadDead4->m_nativeValue,
            vSurfaceFuelChaparralLoadDead4->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadLiveLeaf %g %s\n", Margin,
            vSurfaceFuelChaparralLoadLiveLeaf->m_nativeValue,
            vSurfaceFuelChaparralLoadLiveLeaf->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadLive1 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadLive1->m_nativeValue,
            vSurfaceFuelChaparralLoadLive1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadLive2 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadLive2->m_nativeValue,
            vSurfaceFuelChaparralLoadLive2->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadLive3 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadLive3->m_nativeValue,
            vSurfaceFuelChaparralLoadLive3->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelChaparralLoadLive4 %g %s\n", Margin,
            vSurfaceFuelChaparralLoadLive4->m_nativeValue,
            vSurfaceFuelChaparralLoadLive4->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelChaparralParms
 *
 *  Dependent Variables (Outputs)
 *		vSurfaceFuelBedDepth (ft)
 *		vSurfaceFuelBedMextDead (ratio)
 *      vSurfaceFuelDens# (lb/ft3)
 *      vSurfaceFuelHeat# (Btu/lb)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelLoad# (lb/ft2)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *      vSurfaceFuelSeff# (fraction)
 *      vSurfaceFuelStot# (fraction)
 *
 *  Independent Variables (Inputs)
 *		vSurfaceFuelChaparralDepth (ft)
 *      vSurfaceFuelChaparralLoadDead1 (lb/ft2)
 *      vSurfaceFuelChaparralLoadDead2 (lb/ft2)
 *      vSurfaceFuelChaparralLoadDead3 (lb/ft2)
 *      vSurfaceFuelChaparralLoadDead4 (lb/ft2)
 *      vSurfaceFuelChaparralLoadLiveLeaf (lb/ft2)
 *      vSurfaceFuelChaparralLoadLive1 (lb/ft2)
 *      vSurfaceFuelChaparralLoadLive2 (lb/ft2)
 *      vSurfaceFuelChaparralLoadLive3 (lb/ft2)
 *      vSurfaceFuelChaparralLoadLive4 (lb/ft2)
 */

void EqCalc::FuelChaparralParms( void )
{
	// Initialize to default values
    for ( int i=0; i<MaxParts; i++ )
    {
		vSurfaceFuelLife[i]->updateItem( FuelLifeType_DeadTimeLag );
        vSurfaceFuelDens[i]->update( 46.0 );
        vSurfaceFuelHeat[i]->update( 8000. );
		vSurfaceFuelLoad[i]->update( 0. );
		vSurfaceFuelSavr[i]->update( 1.0 );
        vSurfaceFuelSeff[i]->update( 0.015 );
        vSurfaceFuelStot[i]->update( 0.055 );
    }
	// Assign live leaf density and silica-free content
	vSurfaceFuelDens[4]->update( 32.0 );
	vSurfaceFuelSeff[4]->update( 0.035 );

	// Assign depth and extinction moisture
    double depth = vSurfaceFuelChaparralDepth->m_nativeValue;
    vSurfaceFuelBedDepth->update( depth );
    vSurfaceFuelBedMextDead->update( 0.3 );
    // Assign life values
    vSurfaceFuelLife[0]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 640 0.075"
    vSurfaceFuelLife[1]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 127 0.378"
    vSurfaceFuelLife[2]->updateItem( FuelLifeType_DeadTimeLag ); // Dead  61 0.787"
    vSurfaceFuelLife[3]->updateItem( FuelLifeType_DeadTimeLag ); // Dead  27 1.778"
    vSurfaceFuelLife[4]->updateItem( FuelLifeType_LiveHerb );    // Live leaf 2200 0.0218"
    vSurfaceFuelLife[5]->updateItem( FuelLifeType_LiveWood );    // Live 640 0.075"
    vSurfaceFuelLife[6]->updateItem( FuelLifeType_LiveWood );    // Live 127 0.378"
    vSurfaceFuelLife[7]->updateItem( FuelLifeType_LiveWood );    // Live  61 0.787"
    vSurfaceFuelLife[8]->updateItem( FuelLifeType_LiveWood );    // Live  27 1.778"
    // Assign load values
    vSurfaceFuelLoad[0]->update( vSurfaceFuelChaparralLoadDead1->m_nativeValue );
    vSurfaceFuelLoad[1]->update( vSurfaceFuelChaparralLoadDead2->m_nativeValue );
    vSurfaceFuelLoad[2]->update( vSurfaceFuelChaparralLoadDead3->m_nativeValue );
    vSurfaceFuelLoad[3]->update( vSurfaceFuelChaparralLoadDead4->m_nativeValue );
    vSurfaceFuelLoad[4]->update( vSurfaceFuelChaparralLoadLiveLeaf->m_nativeValue );
    vSurfaceFuelLoad[5]->update( vSurfaceFuelChaparralLoadLive1->m_nativeValue );
    vSurfaceFuelLoad[6]->update( vSurfaceFuelChaparralLoadLive2->m_nativeValue );
    vSurfaceFuelLoad[7]->update( vSurfaceFuelChaparralLoadLive3->m_nativeValue );
    vSurfaceFuelLoad[8]->update( vSurfaceFuelChaparralLoadLive4->m_nativeValue );
    // Assign savr values
    vSurfaceFuelSavr[0]->update( 640. );	// Dead
    vSurfaceFuelSavr[1]->update( 127. );	// Dead
    vSurfaceFuelSavr[2]->update(  61. );	// Dead
    vSurfaceFuelSavr[3]->update(  27. );	// Dead
    vSurfaceFuelSavr[4]->update( 2200. );	// Live leaf
    vSurfaceFuelSavr[5]->update( 640. );	// Live stem
    vSurfaceFuelSavr[6]->update( 127. );	// Live stem
    vSurfaceFuelSavr[7]->update(  61. );	// Live stem
    vSurfaceFuelSavr[8]->update(  27. );	// Live stem
	// Assign heat values
    //double heatLeaf = vSurfaceFuelChaparralHeatLiveLeaf->m_nativeValue;
    //double heatStem = vSurfaceFuelChaparralHeatLiveStem->m_nativeValue;
	vSurfaceFuelHeat[4]->update( 10500. ); // Live leaf
    vSurfaceFuelHeat[5]->update( 10500. ); // Live stem
    vSurfaceFuelHeat[6]->update( 9500. ); // Live stem
    vSurfaceFuelHeat[7]->update( 9500. ); // Live stem
    vSurfaceFuelHeat[8]->update( 9500. ); // Live stem

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelChaparralParms() 0 0\n", Margin );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelLoadTransferFraction
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelLoadTransferFraction (fraction)
 *
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelLoadTransferEq (index)
 *      vSurfaceFuelMoisLiveHerb (fraction)
 *
 *  Notes
 *      Implements NFDRS live herbaceous fuel load transfer fraction.
 */

void EqCalc::FuelLoadTransferFraction( void )
{
    // Access current input values
    double herbMois = vSurfaceFuelMoisLiveHerb->m_nativeValue;
    int transferEq  = vSurfaceFuelLoadTransferEq->activeItemDataIndex();

    // Derive herbaceous load transfer fraction
    double fraction = 0.0;
    if ( transferEq != 0 )
    {
        fraction = FBL_HerbaceousFuelLoadCuredFraction( herbMois );
    }

    // Store results
    vSurfaceFuelLoadTransferFraction->update( fraction );
    vSurfaceFuelLoadTransferFraction->m_store.setNum(
        vSurfaceFuelLoadTransferFraction->m_displayValue,
        'f', vSurfaceFuelLoadTransferFraction->m_displayDecimals );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelLoadTransferFraction() 2 6\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLiveHerb %g %s\n", Margin,
            vSurfaceFuelMoisLiveHerb->m_nativeValue,
            vSurfaceFuelMoisLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelLoadTransferFraction %g %s\n", Margin,
            vSurfaceFuelLoadTransferFraction->m_nativeValue,
            vSurfaceFuelLoadTransferFraction->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelMoisDeadHerbWood
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelMoisDead1 (fraction)
 *      vSurfaceFuelMoisDead10 (fraction)
 *      vSurfaceFuelMoisDead100 (fraction)
 *      vSurfaceFuelMoisDead1000 (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisLifeDead (fraction)
 *
 *  Notes
 *      Provides a means of assigning fuel moisture to size classes
 *      given generic dead fuel moisture and specific herb and stem mositure.
 */

void EqCalc::FuelMoisDeadHerbWood( void )
{
    // Access current input values
    double dead = vSurfaceFuelMoisLifeDead->m_nativeValue;

    vSurfaceFuelMoisDead1->update( dead );
    vSurfaceFuelMoisDead1->m_store.setNum( vSurfaceFuelMoisDead1->m_displayValue,
        'f', vSurfaceFuelMoisDead1->m_displayDecimals );

    vSurfaceFuelMoisDead10->update( dead );
    vSurfaceFuelMoisDead10->m_store.setNum( vSurfaceFuelMoisDead10->m_displayValue,
        'f', vSurfaceFuelMoisDead10->m_displayDecimals );

    vSurfaceFuelMoisDead100->update( dead );
    vSurfaceFuelMoisDead100->m_store.setNum( vSurfaceFuelMoisDead100->m_displayValue,
        'f', vSurfaceFuelMoisDead100->m_displayDecimals );

    vSurfaceFuelMoisDead1000->update( dead );
    vSurfaceFuelMoisDead1000->m_store.setNum( vSurfaceFuelMoisDead1000->m_displayValue,
        'f', vSurfaceFuelMoisDead1000->m_displayDecimals );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelMoisDeadHerbWood() 1 4\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLifeDead %g %s\n", Margin,
            vSurfaceFuelMoisLifeDead->m_nativeValue,
            vSurfaceFuelMoisLifeDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead10 %g %s\n", Margin,
            vSurfaceFuelMoisDead10->m_nativeValue,
            vSurfaceFuelMoisDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1000 %g %s\n", Margin,
            vSurfaceFuelMoisDead1000->m_nativeValue,
            vSurfaceFuelMoisDead1000->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief FuelMoisLifeClass
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelMoisDead1 (fraction)
 *      vSurfaceFuelMoisDead10 (fraction)
 *      vSurfaceFuelMoisDead100 (fraction)
 *      vSurfaceFuelMoisDead1000 (fraction)
 *      vSurfaceFuelMoisLiveHerb (fraction)
 *      vSurfaceFuelMoisLiveWood (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisLifeDead (fraction)
 *      vSurfaceFuelMoisLifeLive (fraction)
 *
 *  Notes
 *      Provides a means of assigning fuel moisture to size classes
 *      given generic fuel moisture by life category.
 */

void EqCalc::FuelMoisLifeClass( void )
{
    // Access current input values
    double dead = vSurfaceFuelMoisLifeDead->m_nativeValue;
    double live = vSurfaceFuelMoisLifeLive->m_nativeValue;

    vSurfaceFuelMoisDead1->update( dead );
    vSurfaceFuelMoisDead1->m_store.setNum( vSurfaceFuelMoisDead1->m_displayValue,
        'f', vSurfaceFuelMoisDead1->m_displayDecimals );

    vSurfaceFuelMoisDead10->update( dead );
    vSurfaceFuelMoisDead10->m_store.setNum( vSurfaceFuelMoisDead10->m_displayValue,
        'f', vSurfaceFuelMoisDead10->m_displayDecimals );

    vSurfaceFuelMoisDead100->update( dead );
    vSurfaceFuelMoisDead100->m_store.setNum( vSurfaceFuelMoisDead100->m_displayValue,
        'f', vSurfaceFuelMoisDead100->m_displayDecimals );

    vSurfaceFuelMoisDead1000->update( dead );
    vSurfaceFuelMoisDead1000->m_store.setNum( vSurfaceFuelMoisDead1000->m_displayValue,
        'f', vSurfaceFuelMoisDead1000->m_displayDecimals );

    vSurfaceFuelMoisLiveHerb->update( live );
    vSurfaceFuelMoisLiveHerb->m_store.setNum( vSurfaceFuelMoisLiveHerb->m_displayValue,
        'f', vSurfaceFuelMoisLiveHerb->m_displayDecimals );

    vSurfaceFuelMoisLiveWood->update( live );
    vSurfaceFuelMoisLiveWood->m_store.setNum( vSurfaceFuelMoisLiveWood->m_displayValue,
        'f', vSurfaceFuelMoisLiveWood->m_displayDecimals );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelMoisLifeClass() 2 6\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLifeDead %g %s\n", Margin,
            vSurfaceFuelMoisLifeDead->m_nativeValue,
            vSurfaceFuelMoisLifeDead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLifeLive %g %s\n", Margin,
            vSurfaceFuelMoisLifeLive->m_nativeValue,
            vSurfaceFuelMoisLifeLive->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead10 %g %s\n", Margin,
            vSurfaceFuelMoisDead10->m_nativeValue,
            vSurfaceFuelMoisDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1000 %g %s\n", Margin,
            vSurfaceFuelMoisDead1000->m_nativeValue,
            vSurfaceFuelMoisDead1000->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisLiveHerb %g %s\n", Margin,
            vSurfaceFuelMoisLiveHerb->m_nativeValue,
            vSurfaceFuelMoisLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisLiveWood %g %s\n", Margin,
            vSurfaceFuelMoisLiveWood->m_nativeValue,
            vSurfaceFuelMoisLiveWood->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelMoisScenarioModel
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelMoisDead1 (fraction)
 *      vSurfaceFuelMoisDead10 (fraction)
 *      vSurfaceFuelMoisDead100 (fraction)
 *      vSurfaceFuelMoisDead1000 (fraction)
 *      vSurfaceFuelMoisLiveHerb (fraction)
 *      vSurfaceFuelMoisLiveWood (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisScenario (item)
 *
 *  Notes
 *      Provides a means of assigning fuel moisture to size classes
 *      given a fuel mositure scenario.
 */

void EqCalc::FuelMoisScenarioModel( void )
{
    // Access current input values
    // Get current moisture scenario item name
    QString name = vSurfaceFuelMoisScenario->activeItemName();

    // Get the MoisScenario class pointer for the item name
    MoisScenario *ms =
        m_eqTree->m_moisScenarioList->moisScenarioByScenarioName( name );
    if ( ! ms )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqCalc:MoisScenarioNotFound",
            vSurfaceFuelMoisScenario->activeItemName() );
        bomb( text );
    }
    // Copy values from the MoisScenario into the EqTree
    vSurfaceFuelMoisDead1->update( ms->m_moisDead1 );
    vSurfaceFuelMoisDead1->m_store.setNum( vSurfaceFuelMoisDead1->m_displayValue,
        'f', vSurfaceFuelMoisDead1->m_displayDecimals );

    vSurfaceFuelMoisDead10->update( ms->m_moisDead10 );
    vSurfaceFuelMoisDead10->m_store.setNum( vSurfaceFuelMoisDead10->m_displayValue,
        'f', vSurfaceFuelMoisDead10->m_displayDecimals );

    vSurfaceFuelMoisDead100->update( ms->m_moisDead100 );
    vSurfaceFuelMoisDead100->m_store.setNum( vSurfaceFuelMoisDead100->m_displayValue,
        'f', vSurfaceFuelMoisDead100->m_displayDecimals );

    vSurfaceFuelMoisDead1000->update( ms->m_moisDead1000 );
    vSurfaceFuelMoisDead1000->m_store.setNum( vSurfaceFuelMoisDead1000->m_displayValue,
        'f', vSurfaceFuelMoisDead1000->m_displayDecimals );

    vSurfaceFuelMoisLiveHerb->update( ms->m_moisLiveHerb );
    vSurfaceFuelMoisLiveHerb->m_store.setNum( vSurfaceFuelMoisLiveHerb->m_displayValue,
        'f', vSurfaceFuelMoisLiveHerb->m_displayDecimals );

    vSurfaceFuelMoisLiveWood->update( ms->m_moisLiveWood );
    vSurfaceFuelMoisLiveWood->m_store.setNum( vSurfaceFuelMoisLiveWood->m_displayValue,
        'f', vSurfaceFuelMoisLiveWood->m_displayDecimals );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelMoisScenarioModel1 6\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisScenario %s\n", Margin,
            name.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead10 %g %s\n", Margin,
            vSurfaceFuelMoisDead10->m_nativeValue,
            vSurfaceFuelMoisDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisDead1000 %g %s\n", Margin,
            vSurfaceFuelMoisDead1000->m_nativeValue,
            vSurfaceFuelMoisDead1000->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisLiveHerb %g %s\n", Margin,
            vSurfaceFuelMoisLiveHerb->m_nativeValue,
            vSurfaceFuelMoisLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelMoisLiveWood %g %s\n", Margin,
            vSurfaceFuelMoisLiveWood->m_nativeValue,
            vSurfaceFuelMoisLiveWood->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelMoisTimeLag
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelMois# (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisDead1 (fraction)
 *      vSurfaceFuelMoisDead10 (fraction)
 *      vSurfaceFuelMoisDead100 (fraction)
 *      vSurfaceFuelMoisDead1000 (fraction)
 *      vSurfaceFuelMoisLiveHerb (fraction)
 *      vSurfaceFuelMoisLiveWood (fraction)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *
 *  Notes
 *      Provides a means of assigning fuel moistures to fuel particles
 *      given fuel moistures by size class.
 */

void EqCalc::FuelMoisTimeLag( void )
{
    // Access current input values
    int    life, i;
    double savr, mois;

    for ( i=0; i<MaxParts; i++ )
    {
        life = vSurfaceFuelLife[i]->activeItemDataIndex();
        savr = vSurfaceFuelSavr[i]->m_nativeValue;
        // Live herbaceous fuel types (1)
        if ( life == FuelLifeType_LiveHerb )
        {
            mois = vSurfaceFuelMoisLiveHerb->m_nativeValue;
        }
        // Live woody fuel types (2)
        else if ( life == FuelLifeType_LiveWood )
        {
            mois = vSurfaceFuelMoisLiveWood->m_nativeValue;
        }
        // Dead litter fuel types (3)
        else if ( life == FuelLifeType_DeadLitter )
        {
            mois = vSurfaceFuelMoisDead100->m_nativeValue;
        }
        // Dead time lag fuel types (0)
        else if ( savr > 192. )
        {
            mois = vSurfaceFuelMoisDead1->m_nativeValue;
        }
        else if ( savr > 48. )
        {
            mois = vSurfaceFuelMoisDead10->m_nativeValue;
        }
        else if ( savr > 16. )
        {
            mois = vSurfaceFuelMoisDead100->m_nativeValue;
        }
        else
        {
            mois = vSurfaceFuelMoisDead1000->m_nativeValue;
        }
        vSurfaceFuelMois[i]->update( mois );
    }
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelMoisTimeLag() %d %d\n", Margin,
            6+MaxParts, MaxParts );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead10 %g %s\n", Margin,
            vSurfaceFuelMoisDead10->m_nativeValue,
            vSurfaceFuelMoisDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead1000 %g %s\n", Margin,
            vSurfaceFuelMoisDead1000->m_nativeValue,
            vSurfaceFuelMoisDead1000->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLiveHerb %g %s\n", Margin,
            vSurfaceFuelMoisLiveHerb->m_nativeValue,
            vSurfaceFuelMoisLiveHerb->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelMoisLiveWood %g %s\n", Margin,
            vSurfaceFuelMoisLiveWood->m_nativeValue,
            vSurfaceFuelMoisLiveWood->m_nativeUnits.latin1() );
        for ( i = 0; i < MaxParts; i++ )
        {
            fprintf( m_log, "%s  i vSurfaceFuelLife%d %g %s\n", Margin, i,
                vSurfaceFuelLife[i]->m_nativeValue,
                vSurfaceFuelLife[i]->m_nativeUnits.latin1() );
            fprintf( m_log, "%s  i vSurfaceFuelSavr%d %g %s\n", Margin, i,
                vSurfaceFuelSavr[i]->m_nativeValue,
                vSurfaceFuelSavr[i]->m_nativeUnits.latin1() );
        }
        for ( i = 0; i < MaxParts; i++ )
        {
            fprintf( m_log, "%s  o vSurfaceFuelMois%d %g %s\n", Margin, i,
                vSurfaceFuelMois[i]->m_nativeValue,
                vSurfaceFuelMois[i]->m_nativeUnits.latin1() );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelPalmettoModel
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelBedDepth (ft)
 *      vSurfaceFuelBedMextDead (lb/lb)
 *      vSurfaceFuelPalmettoLoadDead1 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadDead10 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadDeadFoliage (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLitter (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLive1 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLive10 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLiveFoliage (lb/ft2)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelPalmettoAge (yr)
 *      vSurfaceFuelPalmettoCover (%)
 *      vSurfaceFuelPalmettoHeight (ft)
 *      vSurfaceFuelPalmettoOverstoryBasalArea (ft2/ac)
 */

void EqCalc::FuelPalmettoModel( void )
{
    // Access current input values
    double age   = vSurfaceFuelPalmettoAge->m_nativeValue;
    double cover = vSurfaceFuelPalmettoCover->m_nativeValue;
    double ht    = vSurfaceFuelPalmettoHeight->m_nativeValue;
    double ba    = vSurfaceFuelPalmettoOverstoryBasalArea->m_nativeValue;
    // Calculate
    double dead1  = FBL_PalmettoGallberyDead1HrLoad( age, ht );
    double dead10 = FBL_PalmettoGallberyDead10HrLoad( age, cover );
    double deadf  = FBL_PalmettoGallberyDeadFoliageLoad( age, cover );
    double depth  = FBL_PalmettoGallberyFuelBedDepth( ht );
    double live1  = FBL_PalmettoGallberyLive1HrLoad( age, ht );
    double live10 = FBL_PalmettoGallberyLive10HrLoad( age, ht );
    double livef  = FBL_PalmettoGallberyLiveFoliageLoad( age, cover, ht );
    double litter = FBL_PalmettoGallberyLitterLoad( age, ba ) ;
    // Update
    vSurfaceFuelBedDepth->update( depth );
    vSurfaceFuelBedMextDead->update( 0.40 );
    vSurfaceFuelPalmettoLoadDead1->update( dead1 );
    vSurfaceFuelPalmettoLoadDead10->update( dead10 );
    vSurfaceFuelPalmettoLoadDeadFoliage->update( deadf );
    vSurfaceFuelPalmettoLoadLitter->update( litter );
    vSurfaceFuelPalmettoLoadLive1->update( live1 );
    vSurfaceFuelPalmettoLoadLive10->update( live10 );
    vSurfaceFuelPalmettoLoadLiveFoliage->update( livef );

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelPalmettoModel() 4 8\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelPalmettoAge %g %s\n", Margin,
            vSurfaceFuelPalmettoAge->m_nativeValue,
            vSurfaceFuelPalmettoAge->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelPalmettoCover %g %s\n", Margin,
            vSurfaceFuelPalmettoCover->m_nativeValue,
            vSurfaceFuelPalmettoCover->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelPalmettoHeight %g %s\n", Margin,
            vSurfaceFuelPalmettoHeight->m_nativeValue,
            vSurfaceFuelPalmettoHeight->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelPalmettoOverstoryBasalArea %g %s\n", Margin,
            vSurfaceFuelPalmettoOverstoryBasalArea->m_nativeValue,
            vSurfaceFuelPalmettoOverstoryBasalArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadDead1 %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadDead1->m_nativeValue,
            vSurfaceFuelPalmettoLoadDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadDead10 %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadDead10->m_nativeValue,
            vSurfaceFuelPalmettoLoadDead10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadDeadFoliage %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadDeadFoliage->m_nativeValue,
            vSurfaceFuelPalmettoLoadDeadFoliage->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadLitter %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadLitter->m_nativeValue,
            vSurfaceFuelPalmettoLoadLitter->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadLive1 %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadLive1->m_nativeValue,
            vSurfaceFuelPalmettoLoadLive1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadLive10 %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadLive10->m_nativeValue,
            vSurfaceFuelPalmettoLoadLive10->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelPalmettoLoadLiveFoliage %g %s\n", Margin,
            vSurfaceFuelPalmettoLoadLiveFoliage->m_nativeValue,
            vSurfaceFuelPalmettoLoadLiveFoliage->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelBedDepth %g %s\n", Margin,
            vSurfaceFuelBedDepth->m_nativeValue,
            vSurfaceFuelBedDepth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelPalmettoParms
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelDens# (lb/ft3)
 *      vSurfaceFuelHeat# (Btu/lb)
 *      vSurfaceFuelLife# (category)
 *      vSurfaceFuelLoad# (lb/ft2)
 *      vSurfaceFuelSavr# (ft2/ft3)
 *      vSurfaceFuelSeff# (fraction)
 *      vSurfaceFuelStot# (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelPalmettoDepth (ft)
 *      vSurfaceFuelPalmettoLoadDead1 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadDead10 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadDeadFoliage (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLitter (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLive1 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLive10 (lb/ft2)
 *      vSurfaceFuelPalmettoLoadLiveFoliage (lb/ft2)
 */

void EqCalc::FuelPalmettoParms( void )
{
	// Initialize to default values
    for ( int i = 0; i < MaxParts; i++ )
    {
		vSurfaceFuelLife[i]->updateItem( FuelLifeType_DeadTimeLag );
        vSurfaceFuelDens[i]->update( 30.0 );	// PG specific
        vSurfaceFuelHeat[i]->update( 8300. );	// PG specific
		vSurfaceFuelLoad[i]->update( 0. );
		vSurfaceFuelSavr[i]->update( 1.0 );
        vSurfaceFuelSeff[i]->update( 0.010 );
        vSurfaceFuelStot[i]->update( 0.030 );	// PG specific
    }
    // Assign life values
    vSurfaceFuelLife[0]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 0.0 - 0.25"
    vSurfaceFuelLife[1]->updateItem( FuelLifeType_DeadTimeLag ); // Dead 0.25 - 1.0"
    vSurfaceFuelLife[2]->updateItem( FuelLifeType_DeadTimeLag ); // Dead foliage
    vSurfaceFuelLife[3]->updateItem( FuelLifeType_LiveWood );    // Live 0.0 - 0.25"
    vSurfaceFuelLife[4]->updateItem( FuelLifeType_LiveWood );    // Live 0.25 - 1.0"
    vSurfaceFuelLife[5]->updateItem( FuelLifeType_LiveHerb );    // Live foliage
    vSurfaceFuelLife[6]->updateItem( FuelLifeType_DeadLitter );  // Litter
    // Assign load values
    vSurfaceFuelLoad[0]->update( vSurfaceFuelPalmettoLoadDead1->m_nativeValue );
    vSurfaceFuelLoad[1]->update( vSurfaceFuelPalmettoLoadDead10->m_nativeValue );
    vSurfaceFuelLoad[2]->update( vSurfaceFuelPalmettoLoadDeadFoliage->m_nativeValue );
    vSurfaceFuelLoad[3]->update( vSurfaceFuelPalmettoLoadLive1->m_nativeValue );
    vSurfaceFuelLoad[4]->update( vSurfaceFuelPalmettoLoadLive10->m_nativeValue );
    vSurfaceFuelLoad[5]->update( vSurfaceFuelPalmettoLoadLiveFoliage->m_nativeValue );
    vSurfaceFuelLoad[6]->update( vSurfaceFuelPalmettoLoadLitter->m_nativeValue );
    // Assign savr values
    vSurfaceFuelSavr[0]->update( 350.0 );
    vSurfaceFuelSavr[1]->update( 140.0 );
    vSurfaceFuelSavr[2]->update( 2000.0 );
    vSurfaceFuelSavr[3]->update( 350.0 );
    vSurfaceFuelSavr[4]->update( 140.0 );
    vSurfaceFuelSavr[5]->update( 2000.0 );
    // \todo - find appropriate savr for palmetto-gallberry litter
    vSurfaceFuelSavr[6]->update( 2000.0 );
    // Assign dens values
    vSurfaceFuelDens[0]->update( 30.0 );
    vSurfaceFuelDens[1]->update( 30.0 );
    vSurfaceFuelDens[2]->update( 30.0 );
    vSurfaceFuelDens[3]->update( 46.0 );
    vSurfaceFuelDens[4]->update( 46.0 );
    vSurfaceFuelDens[5]->update( 46.0 );
    vSurfaceFuelDens[6]->update( 30.0 );
    // Assign seff values
    vSurfaceFuelSeff[0]->update( 0.010 );
    vSurfaceFuelSeff[1]->update( 0.010 );
    vSurfaceFuelSeff[2]->update( 0.010 );
    vSurfaceFuelSeff[3]->update( 0.015 );
    vSurfaceFuelSeff[4]->update( 0.015 );
    vSurfaceFuelSeff[5]->update( 0.015 );
    vSurfaceFuelSeff[6]->update( 0.010 );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelPalmettoParms() 0 0\n", Margin );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelTemp
 *
 *  Dependent Variables (Outputs)
 *      vSurfaceFuelTemp (oF)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vSiteSunShading (fraction)
 */

void EqCalc::FuelTemp( void )
{
    // Access current input values
    double airTemp  = vWthrAirTemp->m_nativeValue;
    double sunShade = vSiteSunShading->m_nativeValue;
    // Calculate results
    double fuelTemp = FBL_SurfaceFuelTemperature( airTemp, sunShade );
    // Store results
    vSurfaceFuelTemp->update( fuelTemp );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc FuelTemp() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteSunShading %g %s\n", Margin,
            vSiteSunShading->m_nativeValue,
            vSiteSunShading->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSurfaceFuelTemp %g %s\n", Margin,
            vSurfaceFuelTemp->m_nativeValue,
            vSurfaceFuelTemp->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief IgnitionFirebrandFuelMoisFromDead1Hr
 *
 *  Dependent Variables (Outputs)
 *      vIgnitionFirebrandFuelMois (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisDead1 (fraction)
 */

void EqCalc::IgnitionFirebrandFuelMoisFromDead1Hr( void )
{
    // Access current input values
    double fm = vSurfaceFuelMoisDead1->m_nativeValue;
    // Store results
    vIgnitionFirebrandFuelMois->update( fm );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc IgnitionFirebrandFuelMoisFromDead1Hr() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead1 %g %s\n", Margin,
            vSurfaceFuelMoisDead1->m_nativeValue,
            vSurfaceFuelMoisDead1->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vIgnitionFirebrandFuelMois %g %s\n", Margin,
            vIgnitionFirebrandFuelMois->m_nativeValue,
            vIgnitionFirebrandFuelMois->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief IgnitionFirebrandProb
 *
 *  Dependent Variables (Outputs)
 *      vIgnitionFirebrandProb (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelTemp (oF)
 *      vIgnitionFirebrandFuelMois (fraction)
 */

void EqCalc::IgnitionFirebrandProb( void )
{
    // Access current input values
    double fuelTemp = vSurfaceFuelTemp->m_nativeValue;
    double deadMois = vIgnitionFirebrandFuelMois->m_nativeValue;
    double prob     = FBL_SurfaceFireFirebrandIgnitionProbability(
                        fuelTemp, deadMois );
    // Store results
    vIgnitionFirebrandProb->update( prob );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc IgnitionFirebrandProb() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelTemp %g %s\n", Margin,
            vSurfaceFuelTemp->m_nativeValue,
            vSurfaceFuelTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vIgnitionFirebrandFuelMois %g %s\n", Margin,
            vIgnitionFirebrandFuelMois->m_nativeValue,
            vIgnitionFirebrandFuelMois->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vIgnitionFirebrandProb %g %s\n", Margin,
            vIgnitionFirebrandProb->m_nativeValue,
            vIgnitionFirebrandProb->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief IgnitionLightningFuelMoisFromDead100Hr
 *
 *  Dependent Variables (Outputs)
 *      vIgnitionLightningFuelMois (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFuelMoisDead100 (fraction)
 */

void EqCalc::IgnitionLightningFuelMoisFromDead100Hr( void )
{
    // Access current input values
    double fm = vSurfaceFuelMoisDead100->m_nativeValue;
    // Store results
    vIgnitionLightningFuelMois->update( fm );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc IgnitionLightningFuelMoisFromDead100Hr() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFuelMoisDead100 %g %s\n", Margin,
            vSurfaceFuelMoisDead100->m_nativeValue,
            vSurfaceFuelMoisDead100->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vIgnitionLightningFuelMois %g %s\n", Margin,
            vIgnitionLightningFuelMois->m_nativeValue,
            vIgnitionLightningFuelMois->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief IgnitionLightningProb
 *
 *  Dependent Variables (Outputs)
 *      vIgnitionLightningProb (fraction)
 *
 *  Independent Variables (Inputs)
 *      vIgnitionLightningFuelType (discrete)
 *      vIgnitionLightningDuffDepth (in)
 *      vIgnitionLightningFuelMois (fraction)
 *      vWthrLightningStrikeType (discrete)
 */

void EqCalc::IgnitionLightningProb( void )
{
    // Access current input values
    int    bed    = vIgnitionLightningFuelType->activeItemDataIndex();
    double depth  = vIgnitionLightningDuffDepth->m_nativeValue;
    double mois   = vIgnitionLightningFuelMois->m_nativeValue;
    int    charge = vWthrLightningStrikeType->activeItemDataIndex();
    // Calculate results
    double prob = FBL_SurfaceFireLightningIgnitionProbability(
                    bed, depth, mois, charge );
    // Store results
    vIgnitionLightningProb->update( prob );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc IgnitionLightningProb() 4 1\n", Margin );
        fprintf( m_log, "%s  i vIgnitionLightningFuelType %d %s\n", Margin,
            vIgnitionLightningFuelType->activeItemDataIndex(),
            vIgnitionLightningFuelType->activeItemName().latin1() );
        fprintf( m_log, "%s  i vIgnitionLightningDuffDepth %g %s\n", Margin,
            vIgnitionLightningDuffDepth->m_nativeValue,
            vIgnitionLightningDuffDepth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vIgnitionLightningFuelMois %g %s\n", Margin,
            vIgnitionLightningFuelMois->m_nativeValue,
            vIgnitionLightningFuelMois->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrLightningStrikeType %d %s\n", Margin,
            vWthrLightningStrikeType->activeItemDataIndex(),
            vWthrLightningStrikeType->activeItemName().latin1() );
        fprintf( m_log, "%s  o vIgnitionLightningProb %g %s\n", Margin,
            vIgnitionLightningProb->m_nativeValue,
            vIgnitionLightningProb->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief MapScale
 *
 *  Dependent Variables (Outputs)
 *      vMapScale (in/mi)
 *
 *  Independent Variables (Inputs)
 *      vMapFraction (none)
 */

void EqCalc::MapScale( void )
{
    // Access current input values
    double denom = vMapFraction->m_nativeValue;
    // Calculate results
    double scale = ( denom < SMIDGEN )
                 ? ( 0.0 )
                 : ( 63360. / denom );
    // Store results
    vMapScale->update( scale );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc MapScale() 2 1\n", Margin );
        fprintf( m_log, "%s  i vMapFraction %g %s\n", Margin,
            vMapFraction->m_nativeValue,
            vMapFraction->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief MapSlope
 *
 *  Dependent Variables (Outputs)
 *      vSiteSlopeDegrees (degrees)
 *      vMapSlopeRise (ft)
 *      vMapSlopeReach (ft)
 *
 *  Independent Variables (Inputs)
 *      vMapFraction (1:x)
 *      vMapContourCount (none)
 *      vMapContourInterval (ft)
 *      vMapDist (in)
 */

void EqCalc::MapSlope( void )
{
    // Access current input values
    double cIntervalFt = vMapContourInterval->m_nativeValue;
    double cCount      = vMapContourCount->m_nativeValue;
    double mapFraction = vMapFraction->m_nativeValue;
    double mapDistIn   = vMapDist->m_nativeValue;
    // Calculate results
    double riseFt      = cIntervalFt * cCount;
    double reachFt     = mapFraction * mapDistIn / 12.;
    double degrees     = ( reachFt < SMIDGEN )
                       ? ( 0.0 )
                       : ( ( 180. / M_PI ) * atan( riseFt / reachFt ) );
    // Store results
    vSiteSlopeDegrees->update( degrees );
    vSiteSlopeRise->update( riseFt );
    vSiteSlopeReach->update( reachFt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc MapSlope() 4 3\n", Margin );
        fprintf( m_log, "%s  i vMapFraction %g %s\n", Margin,
            vMapFraction->m_nativeValue,
            vMapFraction->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapDist %g %s\n", Margin,
            vMapDist->m_nativeValue,
            vMapDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapContourCount %g %s\n", Margin,
            vMapContourCount->m_nativeValue,
            vMapContourCount->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapContourInterval %g %s\n", Margin,
            vMapContourInterval->m_nativeValue,
            vMapContourInterval->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteSlopeDegrees %g %s\n", Margin,
            vSiteSlopeDegrees->m_nativeValue,
            vSiteSlopeDegrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteSlopeRise %g %s\n", Margin,
            vSiteSlopeRise->m_nativeValue,
            vSiteSlopeRise->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteSlopeReach %g %s\n", Margin,
            vSiteSlopeReach->m_nativeValue,
            vSiteSlopeReach->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SafetyZoneRadius
 *
 *  Dependent Variables (Outputs)
 *      vSafteyZoneLength (ft)
 *      vSafteyZoneRadius (ft)
 *      vSafetyZoneSize (ft2)
 *      vSafetyZoneSizeSquare (ft2)
 *
 *  Independent Variables (Inputs)
 *      vSafteyZoneSepDist (ft)
 *      vSafetyZoneEquipmentArea (ft2)
 *      vSafetyZoneEquipmentNumber
 *      vSafetyZonePersonnelArea (ft2)
 *      vSafetyZonePersonnelNumber
 */

void EqCalc::SafetyZoneRadius( void )
{
    // Access current input values
    double eArea  = vSafetyZoneEquipmentArea->m_nativeValue;
    double eNumb  = vSafetyZoneEquipmentNumber->m_nativeValue;
    double pArea  = vSafetyZonePersonnelArea->m_nativeValue;
    double pNumb  = vSafetyZonePersonnelNumber->m_nativeValue;
    double sDist  = vSafetyZoneSepDist->m_nativeValue;
    // Calculate results
    double radius = FBL_SafetyZoneRadius( sDist, pNumb, pArea, eNumb, eArea );
    double size   = M_PI * ( radius * radius );
	double length = 2.0 * radius;
	double square = length * length;
    // Store results
    vSafetyZoneRadius->update( radius );
    vSafetyZoneSize->update( size );
    vSafetyZoneLength->update(length );
    vSafetyZoneSizeSquare->update( square );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SafetyZoneAtVector() 5 4\n", Margin );
        fprintf( m_log, "%s  i vSafetyZoneSepDist %g %s\n", Margin,
            vSafetyZoneSepDist->m_nativeValue,
            vSafetyZoneSepDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSafetyZoneEquipmentArea %g %s\n", Margin,
            vSafetyZoneEquipmentArea->m_nativeValue,
            vSafetyZoneEquipmentArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSafetyZoneEquipmentNumber %g %s\n", Margin,
            vSafetyZoneEquipmentNumber->m_nativeValue,
            vSafetyZoneEquipmentNumber->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSafetyZonePersonnelArea %g %s\n", Margin,
            vSafetyZonePersonnelArea->m_nativeValue,
            vSafetyZonePersonnelArea->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSafetyZonePersonnelNumber %g %s\n", Margin,
            vSafetyZonePersonnelNumber->m_nativeValue,
            vSafetyZonePersonnelNumber->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSafetyZoneLength %g %s\n", Margin,
            vSafetyZoneLength->m_nativeValue,
            vSafetyZoneLength->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSafetyZoneRadius %g %s\n", Margin,
            vSafetyZoneRadius->m_nativeValue,
            vSafetyZoneRadius->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSafetyZoneSize %g %s\n", Margin,
            vSafetyZoneSize->m_nativeValue,
            vSafetyZoneSize->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSafetyZoneSizeSquare %g %s\n", Margin,
            vSafetyZoneSizeSquare->m_nativeValue,
            vSafetyZoneSizeSquare->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SafetyZoneSepDist
 *
 *  Dependent Variables (Outputs)
 *      vSafetyZoneSepDist (ft)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireFlameLengAtHead (ft)
 */

void EqCalc::SafetyZoneSepDist( void )
{
    // Access current input values
    double flameLeng = vSurfaceFireFlameLengAtHead->m_nativeValue;
    // Calculate results
    double sepDist = FBL_SafetyZoneSeparationDistance( flameLeng );
    // Store results
    vSafetyZoneSepDist->update( sepDist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SafetyZoneSepDist() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtHead %g %s\n", Margin,
            vSurfaceFireFlameLengAtHead->m_nativeValue,
            vSurfaceFireFlameLengAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSafetyZoneSepDist %g %s\n", Margin,
            vSafetyZoneSepDist->m_nativeValue,
            vSafetyZoneSepDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SiteAspectDirFromNorth
 *
 *  Dependent Variables (Outputs)
 *      vSiteAspectDirFromNorth (degrees clockwise from North)
 *
 *  Independent Variables (Inputs)
 *      vSiteAspectDirFromCompass (compass point name)
 */

void EqCalc::SiteAspectDirFromNorth( void )
{
    // Access current input values
    int id = vSiteAspectDirFromCompass->activeItemDataIndex();
    // Calculate results
    double deg = 22.5 * (double) id;
    // Store results
    vSiteAspectDirFromNorth->update( deg );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SiteAspectDirFromNorth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSiteAspectDirFromCompass %d %s\n", Margin,
            vSiteAspectDirFromCompass->activeItemDataIndex(),
            vSiteAspectDirFromCompass->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSiteAspectDirFromNorth %g %s\n", Margin,
            vSiteAspectDirFromNorth->m_nativeValue,
            vSiteAspectDirFromNorth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SiteRidgeToValleyDist
 *
 *  Dependent Variables (Outputs)
 *      vSiteRidgeToValleyDist (mi)
 *
 *  Independent Variables (Inputs)
 *      vSiteRidgeToValleyMapDist (in)
 *      vMapScale (in/mi)
 */

void EqCalc::SiteRidgeToValleyDist( void )
{
    // Access current input values
    double md = vSiteRidgeToValleyMapDist->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double mi = ( ms < SMIDGEN )
              ? ( 0. )
              : ( md / ms );
    // Store results
    vSiteRidgeToValleyDist->update( mi );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SiteRidgeToValleyDist() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSiteRidgeToValleyMapDist %g %s\n", Margin,
            vSiteRidgeToValleyMapDist->m_nativeValue,
            vSiteRidgeToValleyMapDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteRidgeToValleyDist %g %s\n", Margin,
            vSiteRidgeToValleyDist->m_nativeValue,
            vSiteRidgeToValleyDist->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SiteSlopeFraction
 *
 *  Dependent Variables (Outputs)
 *      vSiteSlopeFraction (rise/reach)
 *
 *  Independent Variables (Inputs)
 *      vSiteSlopeDegrees (degrees)
 */

void EqCalc::SiteSlopeFraction ( void )
{
    // Access current input values
    double d = vSiteSlopeDegrees->m_nativeValue;
    // Calculate results
    double f = tan( d * M_PI / 180. );
    // Store results
    vSiteSlopeFraction->update( f );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SiteSlopeFraction() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSiteSlopeDegrees %g %s\n", Margin,
            vSiteSlopeDegrees->m_nativeValue,
            vSiteSlopeDegrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteSlopeFraction %g %s\n", Margin,
            vSiteSlopeFraction->m_nativeValue,
            vSiteSlopeFraction->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SiteUpslopeDirFromNorth
 *
 *  Dependent Variables (Outputs)
 *      vSiteUpslopeDirFromNorth (azimuth degrees)
 *
 *  Independent Variables (Inputs)
 *      vSiteAspectDirFromNorth (azimuth degrees)
 */

void EqCalc::SiteUpslopeDirFromNorth( void )
{
    // Access current input values
    double aspect  = vSiteAspectDirFromNorth->m_nativeValue;
    // Calculate results
    double upslope = ( aspect >= 180. )
                   ? ( aspect - 180. )
                   : ( aspect + 180. );
    // Store results
    vSiteUpslopeDirFromNorth->update( upslope );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SiteUpslopeDirFromNorth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vSiteAspectDirFromNorth %g %s\n", Margin,
            vSiteAspectDirFromNorth->m_nativeValue,
            vSiteAspectDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSiteUpslopeDirFromNorth %g %s\n", Margin,
            vSiteUpslopeDirFromNorth->m_nativeValue,
            vSiteUpslopeDirFromNorth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotDistActiveCrown
 *
 *  Dependent Variables (Outputs)
 *      vSpotCoverHtActiveCrown (ft)
 *      vSpotDistActiveCrown (mi)
 *      vSpotFirebrandHtActiveCrown (ft)
 *      vSpotFlatDistActiveCrown (mi)
 *
 *  Independent Variables (Inputs)
 *      vTreeCoverHt (ft)
 *      vCrownFireActiveFireLineInt (Btu/ft/s)
 *      vSpotEmberDiamAtSurface (mm)
 *      vWindSpeedAt20Ft (mi/h)
 *      vSpotFireSource (0-3)
 *      vSiteRidgeToValleyDist (mi)
 *      vSiteRidgeToValleyElev (ft)
 */

void EqCalc::SpotDistActiveCrown( void )
{
	// Marshall the active crown fire spotting distance input variables
	double canopyHtFt   = vTreeCoverHt->m_nativeValue;
	double emberDiamMm  = vSpotEmberDiamAtSurface->m_nativeValue;
	double fireIntBtu   = vCrownFireActiveFireLineInt->m_nativeValue;
	double windSpeedMph = vWindSpeedAt20Ft->m_nativeValue;
	
	// Convert from English to metric units
	double canopyHtM    = 0.3048 * canopyHtFt;
	double fireIntKw    = 3.46414 * fireIntBtu;
	double windSpeedKph = 1.60934 * windSpeedMph;
	double windHtM      = 0.3048 * 20.;

	// Calculate flat terrain spotting distance results
	Sem::Fire::Crown::Firebrand::Processor *processor = new
		Sem::Fire::Crown::Firebrand::Processor(
			canopyHtM, fireIntKw, windSpeedKph, windHtM, emberDiamMm );
	double firebrandHtM = processor->getFirebrandHeight();	// m
	double flameHtM     = processor->getFlameHeight();		// m
	double flatDistM    = processor->getFirebrandDistance();// m

	// Convert results from metric to English units
	double firebrandHtFt = firebrandHtM / 0.3048;
	double flameHtFt     = flameHtM / 0.3048;
	double flatDistMi    = flatDistM / 0.3048 / 5280.;

	// Determine terrain-adjusted spotting distance...
    double rvElevFt = vSiteRidgeToValleyElev->m_nativeValue;
    double rvHorzMi = vSiteRidgeToValleyDist->m_nativeValue;
    int    source   = vSpotFireSource->activeItemDataIndex();
	double spotDistMi = FBL_SpotDistanceMountainTerrain(
		flatDistMi, source, rvHorzMi, rvElevFt );

    // Store results
    vSpotDistActiveCrown->update( spotDistMi );
    vSpotFirebrandHtActiveCrown->update( firebrandHtFt );
    vSpotFlameHtActiveCrown->update( flameHtFt );
    vSpotFlatDistActiveCrown->update( flatDistMi );
    
	// Clean up
	delete processor;

    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotDistActiveCrown() 7 4\n", Margin );
        fprintf( m_log, "%s  i vTreeCoverHt %g %s\n", Margin,
            vTreeCoverHt->m_nativeValue,
            vTreeCoverHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSpotEmberDiamAtSurface %g %s\n", Margin,
            vSpotEmberDiamAtSurface->m_nativeValue,
            vSpotEmberDiamAtSurface->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vCrownFireActiveFireLintInt %g %s\n", Margin,
            vCrownFireActiveFireLineInt->m_nativeValue,
            vCrownFireActiveFireLineInt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyElev %g %s\n", Margin,
            vSiteRidgeToValleyElev->m_nativeValue,
            vSiteRidgeToValleyElev->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyDist %g %s\n", Margin,
            vSiteRidgeToValleyDist->m_nativeValue,
            vSiteRidgeToValleyDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSpotFireSource %d %s\n", Margin,
            vSpotFireSource->activeItemDataIndex(),
            vSpotFireSource->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSpotDistActiveCrown %g %s\n", Margin,
            vSpotDistActiveCrown->m_nativeValue,
            vSpotDistActiveCrown->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlameHtActiveCrown %g %s\n", Margin,
            vSpotFlameHtActiveCrown->m_nativeValue,
            vSpotFlameHtActiveCrown->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFirebrandHtActiveCrown %g %s\n", Margin,
            vSpotFirebrandHtActiveCrown->m_nativeValue,
            vSpotFirebrandHtActiveCrown->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlatDistActiveCrown %g %s\n", Margin,
            vSpotFlatDistActiveCrown->m_nativeValue,
            vSpotFlatDistActiveCrown->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotDistBurningPile
 *
 *  Dependent Variables (Outputs)
 *      vSpotCoverHtBurningPile (ft)
 *      vSpotDistBurningPile (mi)
 *      vSpotFirebrandHtBurningPile (ft)
 *      vSpotFlatDistBurningPile (mi)
 *
 *  Independent Variables (Inputs)
 *      vSpotFireSource (0-3)
 *      vSiteRidgeToValleyDist (mi)
 *      vSiteRidgeToValleyElev (ft)
 *      vTreeCoverHtDownwind (ft)
 *		vTreeCanopyCoverDownwind (Open or Closed)
 *      vWindSpeedAt20Ft (mi/h)
 *      vSurfaceFireFlameHtPile (ft)
 */

void EqCalc::SpotDistBurningPile( void )
{
    // Access current input values
    int    source  = vSpotFireSource->activeItemDataIndex();
    int    canopy  = vTreeCanopyCoverDownwind->activeItemDataIndex();
    double coverHt = vTreeCoverHtDownwind->m_nativeValue;
    double flameHt = vSurfaceFireFlameHtPile->m_nativeValue;
    double rvElev  = vSiteRidgeToValleyElev->m_nativeValue;
    double rvHorz  = vSiteRidgeToValleyDist->m_nativeValue;
    double wind    = vWindSpeedAt20Ft->m_nativeValue;
    // Calculate results
    double htUsed, firebrandHt, flatDist;
    double spotDist = FBL_SpotDistanceFromBurningPile(
        source, rvHorz, rvElev, coverHt, canopy, wind, flameHt,
        &htUsed, &firebrandHt, &flatDist );
    // Store results
    vSpotCoverHtBurningPile->update( htUsed );
    vSpotDistBurningPile->update( spotDist );
    vSpotFirebrandHtBurningPile->update( firebrandHt );
    vSpotFlatDistBurningPile->update( flatDist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotDistBurningPile() 7 4\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireFlameHtPile %g %s\n", Margin,
            vSurfaceFireFlameHtPile->m_nativeValue,
            vSurfaceFireFlameHtPile->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyDist %g %s\n", Margin,
            vSiteRidgeToValleyDist->m_nativeValue,
            vSiteRidgeToValleyDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyElev %g %s\n", Margin,
            vSiteRidgeToValleyElev->m_nativeValue,
            vSiteRidgeToValleyElev->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHtDownwind %g %s\n", Margin,
            vTreeCoverHtDownwind->m_nativeValue,
            vTreeCoverHtDownwind->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCanopyCoverDownwind %d %s\n", Margin,
            vTreeCanopyCoverDownwind->activeItemDataIndex(),
            vTreeCanopyCoverDownwind->activeItemName().latin1() );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSpotFireSource %d %s\n", Margin,
            vSpotFireSource->activeItemDataIndex(),
            vSpotFireSource->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSpotDistBurningPile %g %s\n", Margin,
            vSpotDistBurningPile->m_nativeValue,
            vSpotDistBurningPile->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotCoverHtBurningPile %g %s\n", Margin,
            vSpotCoverHtBurningPile->m_nativeValue,
            vSpotCoverHtBurningPile->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFirebrandHtBurningPile %g %s\n", Margin,
            vSpotFirebrandHtBurningPile->m_nativeValue,
            vSpotFirebrandHtBurningPile->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlatDistBurningPile %g %s\n", Margin,
            vSpotFlatDistBurningPile->m_nativeValue,
            vSpotFlatDistBurningPile->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotDistSurfaceFire
 *
 *  Dependent Variables (Outputs)
 *      vSpotCoverHtSurfaceFire (ft)
 *      vSpotDistSurfaceFire (mi)
 *      vSpotFirebrandDriftSurfaceFire (mi)
 *      vSpotFirebrandHtSurfaceFire (ft)
 *      vSpotFlatDistSurfaceFire (mi)
 *
 *  Independent Variables (Inputs)
 *      vSpotFireSource (0-3)
 *      vSiteRidgeToValleyDist (mi)
 *      vSiteRidgeToValleyElev (ft)
 *      vTreeCoverHtDownwind (ft)
 *		vTreeCanopyCoverDownwind (Open or Closed)
 *      vWindSpeedAt20Ft (mi/h)
 *      vSurfaceFireFlameLengAtHead (ft)
 */

void EqCalc::SpotDistSurfaceFire( void )
{
    // Access current input values
    int    canopy   = vTreeCanopyCoverDownwind->activeItemDataIndex();
    double coverHt  = vTreeCoverHtDownwind->m_nativeValue;
    double flameLen = vSurfaceFireFlameLengAtHead->m_nativeValue;
    double rvElev   = vSiteRidgeToValleyElev->m_nativeValue;
    double rvHorz   = vSiteRidgeToValleyDist->m_nativeValue;
    int    source   = vSpotFireSource->activeItemDataIndex();
    double wind     = vWindSpeedAt20Ft->m_nativeValue;
    // Calculate results
    double htUsed, firebrandHt, flatDist, firebrandDrift;
    double spotDist = FBL_SpotDistanceFromSurfaceFire(
        source, rvHorz, rvElev, coverHt, canopy, wind, flameLen,
        &htUsed, &firebrandHt, &firebrandDrift, &flatDist );
    // Store results
    vSpotCoverHtSurfaceFire->update( htUsed );
    vSpotDistSurfaceFire->update( spotDist );
    vSpotFirebrandDriftSurfaceFire->update( firebrandDrift );
    vSpotFirebrandHtSurfaceFire->update( firebrandHt );
    vSpotFlatDistSurfaceFire->update( flatDist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotDistSurfaceFire() 7 5\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtHead %g %s\n", Margin,
            vSurfaceFireFlameLengAtHead->m_nativeValue,
            vSurfaceFireFlameLengAtHead->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyDist %g %s\n", Margin,
            vSiteRidgeToValleyDist->m_nativeValue,
            vSiteRidgeToValleyDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyElev %g %s\n", Margin,
            vSiteRidgeToValleyElev->m_nativeValue,
            vSiteRidgeToValleyElev->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHtDownwind %g %s\n", Margin,
            vTreeCoverHtDownwind->m_nativeValue,
            vTreeCoverHtDownwind->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCanopyCoverDownwind %d %s\n", Margin,
            vTreeCanopyCoverDownwind->activeItemDataIndex(),
            vTreeCanopyCoverDownwind->activeItemName().latin1() );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSpotFireSource %d %s\n", Margin,
            vSpotFireSource->activeItemDataIndex(),
            vSpotFireSource->activeItemName().latin1() );
        fprintf( m_log, "%s  o vSpotDistSurfaceFire %g %s\n", Margin,
            vSpotDistSurfaceFire->m_nativeValue,
            vSpotDistSurfaceFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotCoverHtSurfaceFire %g %s\n", Margin,
            vSpotCoverHtSurfaceFire->m_nativeValue,
            vSpotCoverHtSurfaceFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFirebrandDriftSurfaceFire %g %s\n", Margin,
            vSpotFirebrandDriftSurfaceFire->m_nativeValue,
            vSpotFirebrandDriftSurfaceFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFirebrandHtSurfaceFire %g %s\n", Margin,
            vSpotFirebrandHtSurfaceFire->m_nativeValue,
            vSpotFirebrandHtSurfaceFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlatDistSurfaceFire %g %s\n", Margin,
            vSpotFlatDistSurfaceFire->m_nativeValue,
            vSpotFlatDistSurfaceFire->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotDistTorchingTrees
 *
 *  Dependent Variables (Outputs)
 *      vSpotCoverHtTorchingTrees (ft)
 *      vSpotDistTorchingTrees (mi)
 *      vSpotFirebrandHtTorchingTrees (ft)
 *      vSpotFlameDurTorchingTrees (ratio)
 *      vSpotFlameHtTorchingTrees (ft)
 *      vSpotFlameRatioTorchingTrees (ratio)
 *      vSpotFlatDistTorchingTrees (mi)
 *
 *  Independent Variables (Inputs)
 *      vSpotFireSource (0-3)
 *      vSiteRidgeToValleyDist (mi)
 *      vSiteRidgeToValleyElev (ft)
 *      vTreeCoverHtDownwind (ft)
 *		vTreeCanopyCoverDownwind (Open or Closed)
 *      vWindSpeedAt20Ft (mi/h)
 *      vSpotTorchingTrees (none)
 *      vTreeDbh (in)
 *      vTreeHt (ft)
 *      vTreeSpeciesSpot (category)
 */

void EqCalc::SpotDistTorchingTrees( void )
{
    // Access current input values
    int    canopy  = vTreeCanopyCoverDownwind->activeItemDataIndex();
    double coverHt = vTreeCoverHtDownwind->m_nativeValue;
    double rvElev  = vSiteRidgeToValleyElev->m_nativeValue;
    double rvHorz  = vSiteRidgeToValleyDist->m_nativeValue;
    int    source  = vSpotFireSource->activeItemDataIndex();
    double wind    = vWindSpeedAt20Ft->m_nativeValue;
    double tnum    = vSpotTorchingTrees->m_nativeValue;
    double dbh     = vTreeDbh->m_nativeValue;
    double treeHt  = vTreeHt->m_nativeValue;
    int    spp     = vTreeSpeciesSpot->activeItemDataIndex();
    // Catch species errors
    if ( spp < 0 || spp >= 14 )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqCalc:TreeSpeciesNotFound",
            QString( "%1" ).arg( spp ) );
        bomb( text );
    }
    // Calculate results
    double htUsed, firebrandHt, flatDist, flameHt, flameDur, flameRatio;
    double spotDist = FBL_SpotDistanceFromTorchingTrees(
        source, rvHorz, rvElev, coverHt, canopy, wind, tnum, dbh, treeHt, spp,
        &htUsed, &flameHt, &flameRatio, &flameDur, &firebrandHt, &flatDist );
    // Store results
    vSpotCoverHtTorchingTrees->update( htUsed );
    vSpotDistTorchingTrees->update( spotDist );
    vSpotFirebrandHtTorchingTrees->update( firebrandHt );
    vSpotFlameDurTorchingTrees->update( flameDur );
    vSpotFlameHtTorchingTrees->update( flameHt );
    vSpotFlameRatioTorchingTrees->update( flameRatio );
    vSpotFlatDistTorchingTrees->update( flatDist );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotDistTorchingTrees() 10 7\n", Margin );
        fprintf( m_log, "%s  i vSiteRidgeToValleyDist %g %s\n", Margin,
            vSiteRidgeToValleyDist->m_nativeValue,
            vSiteRidgeToValleyDist->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteRidgeToValleyElev %g %s\n", Margin,
            vSiteRidgeToValleyElev->m_nativeValue,
            vSiteRidgeToValleyElev->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSpotFireSource %d %s\n", Margin,
            vSpotFireSource->activeItemDataIndex(),
            vSpotFireSource->activeItemName().latin1() );
        fprintf( m_log, "%s  i vSpotTorchingTrees %g %s\n", Margin,
            vSpotTorchingTrees->m_nativeValue,
            vSpotTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHtDownwind %g %s\n", Margin,
            vTreeCoverHtDownwind->m_nativeValue,
            vTreeCoverHtDownwind->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCanopyCoverDownwind %d %s\n", Margin,
            vTreeCanopyCoverDownwind->activeItemDataIndex(),
            vTreeCanopyCoverDownwind->activeItemName().latin1() );
        fprintf( m_log, "%s  i vTreeHt %g %s\n", Margin,
            vTreeHt->m_nativeValue,
            vTreeHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeSpeciesSpot %d %s\n", Margin,
            vTreeSpeciesSpot->activeItemDataIndex(),
            vTreeSpeciesSpot->activeItemName().latin1() );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotDistTorchingTrees %g %s\n", Margin,
            vSpotDistTorchingTrees->m_nativeValue,
            vSpotDistTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotCoverHtTorchingTrees %g %s\n", Margin,
            vSpotCoverHtTorchingTrees->m_nativeValue,
            vSpotCoverHtTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFirebrandHtTorchingTrees %g %s\n", Margin,
            vSpotFirebrandHtTorchingTrees->m_nativeValue,
            vSpotFirebrandHtTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlameDurTorchingTrees %g %s\n", Margin,
            vSpotFlameDurTorchingTrees->m_nativeValue,
            vSpotFlameDurTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlameHtTorchingTrees %g %s\n", Margin,
            vSpotFlameHtTorchingTrees->m_nativeValue,
            vSpotFlameHtTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlameRatioTorchingTrees %g %s\n", Margin,
            vSpotFlameRatioTorchingTrees->m_nativeValue,
            vSpotFlameRatioTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotFlatDistTorchingTrees %g %s\n", Margin,
            vSpotFlatDistTorchingTrees->m_nativeValue,
            vSpotFlatDistTorchingTrees->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotMapDistActiveCrown
 *
 *  Dependent Variables (Outputs)
 *      vSpotMapDistActiveCrown (in)
 *
 *  Independent Variables (Inputs)
 *      vSpotDistActiveCrown (mi)
 *      vMapScale (in/mi)
 */

void EqCalc::SpotMapDistActiveCrown( void )
{
    // Access current input values
    double bd = vSpotDistActiveCrown->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd;
    // Store results
    vSpotMapDistActiveCrown->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotMapDistActiveCrown() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSpotDistActiveCrown %g %s\n", Margin,
            vSpotDistActiveCrown->m_nativeValue,
            vSpotDistActiveCrown->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotMapDistActiveCrown %g %s\n", Margin,
            vSpotMapDistActiveCrown->m_nativeValue,
            vSpotMapDistActiveCrown->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief SpotMapDistBurningPile
 *
 *  Dependent Variables (Outputs)
 *      vSpotMapDistBurningPile (in)
 *
 *  Independent Variables (Inputs)
 *      vSpotDistBurningPile (mi)
 *      vMapScale (in/mi)
 */

void EqCalc::SpotMapDistBurningPile( void )
{
    // Access current input values
    double bd = vSpotDistBurningPile->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd;
    // Store results
    vSpotMapDistBurningPile->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotMapDistBurningPile() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSpotDistBurningPile %g %s\n", Margin,
            vSpotDistBurningPile->m_nativeValue,
            vSpotDistBurningPile->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotMapDistBurningPile %g %s\n", Margin,
            vSpotMapDistBurningPile->m_nativeValue,
            vSpotMapDistBurningPile->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotMapDistSurfaceFire
 *
 *  Dependent Variables (Outputs)
 *      vSpotMapDistSurfaceFire (in)
 *
 *  Independent Variables (Inputs)
 *      vSpotDistSurfaceFire (mi)
 *      vMapScale (in/mi)
 */

void EqCalc::SpotMapDistSurfaceFire( void )
{
    // Access current input values
    double bd = vSpotDistSurfaceFire->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd;
    // Store results
    vSpotMapDistSurfaceFire->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotMapDistSurfaceFire() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSpotDistSurfaceFire %g %s\n", Margin,
            vSpotDistSurfaceFire->m_nativeValue,
            vSpotDistSurfaceFire->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotMapDistSurfaceFire %g %s\n", Margin,
            vSpotMapDistSurfaceFire->m_nativeValue,
            vSpotMapDistSurfaceFire->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief SpotMapDistTorchingTrees
 *
 *  Dependent Variables (Outputs)
 *      vSpotMapDistTorchingTrees (in)
 *
 *  Independent Variables (Inputs)
 *      vSpotDistTorchingTrees (mi)
 *      vMapScale (in/mi)
 */

void EqCalc::SpotMapDistTorchingTrees( void )
{
    // Access current input values
    double bd = vSpotDistTorchingTrees->m_nativeValue;
    double ms = vMapScale->m_nativeValue;
    // Calculate results
    double md = ms * bd;
    // Store results
    vSpotMapDistTorchingTrees->update( md );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc SpotMapDistTorchingTree() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSpotDistTorchingTrees %g %s\n", Margin,
            vSpotDistTorchingTrees->m_nativeValue,
            vSpotDistTorchingTrees->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vMapScale %g %s\n", Margin,
            vMapScale->m_nativeValue,
            vMapScale->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vSpotMapDistTorchingTrees %g %s\n", Margin,
            vSpotMapDistTorchingTrees->m_nativeValue,
            vSpotMapDistTorchingTrees->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TimeJulianDate
 *
 *  Dependent Variables (Outputs)
 *      vTimeJulianDate (days)
 *      Determines the modified Julian date.
 *      Valid for any date since January 1, 4713 B.C.
 *      Julian calendar is used up to 1582 October 4.
 *      Gregorian calendar is used  after 1582 October 15.
 *      Mjd is JD - 2400000.5, and the day changes at midnight
 *      and not at noon like the Julian Date.
 *
 *  Independent Variables (Inputs)
 *      vTimeIntegerDate (days)
 */

void EqCalc::TimeJulianDate( void )
{
    // Access current input values
    double a = vTimeIntegerDate->m_nativeValue;
    // Calculate results
    int idate = (int) a;
    int year  = idate / 10000;
    int temp  = idate - 10000 * year;
    int month = temp / 100;
    int day   = temp - 100 * month;
    int hour  = (int) (24. * a - (double) idate);
    double jd = CDT_JulianDate( year, month, day, hour, 0, 0, 0 );
    double mjd = CDT_ModifiedJulianDate( jd );
    // Store results
    vTimeJulianDate->update( mjd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TimeJulianDate() 1 1\n", Margin );
        fprintf( m_log, "%s  i vTimeIntegerDate %g %s\n", Margin,
            vTimeIntegerDate->m_nativeValue,
            vTimeIntegerDate->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTimeJulianDate %g %s\n", Margin,
            vTimeJulianDate->m_nativeValue,
            vTimeJulianDate->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeBarkThicknessBehave - DEPRECATED, HISTORICAL ONLY
 *
 *  Dependent Variables (Outputs)
 *      treeBarkThickness (in)
 *
 *  Independent Variables (Inputs)
 *      vTreeDbh (in)
 *      vTreeSpecies (index)
 */

void EqCalc::TreeBarkThicknessBehave( void )
{
    // Access current input values
    int    spp = vTreeSpecies->activeItemDataIndex();
    double dbh = vTreeDbh->m_nativeValue;
    // Calculate results
    double bt  = FBL_TreeBarkThicknessBehave( spp, dbh );
    // Store results
    vTreeBarkThickness->update( bt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeBarkThickness() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  o vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeBarkThicknessFofem - DEPRECATED, HISTORICAL ONLY
 *
 *  Dependent Variables (Outputs)
 *      treeBarkThickness (in)
 *
 *  Independent Variables (Inputs)
 *      vTreeDbh (in)
 *      vTreeSpecies (index)
 */

void EqCalc::TreeBarkThicknessFofem( void )
{
    // Access current input values
    int    spp = vTreeSpecies->activeItemDataIndex();
    double dbh = vTreeDbh->m_nativeValue;
    // Calculate results
    // The mortality model always limits dbh lower bound to 1.0 inch.
    if ( dbh < 1.0 )
    {
        dbh = 1.0;
    }
    double bt  = FBL_TreeBarkThicknessFofem( spp, dbh );
    // Store results
    vTreeBarkThickness->update( bt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeBarkThickness() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  o vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief TreeBarkThicknessFofem6
 *
 *  Dependent Variables (Outputs)
 *      treeBarkThickness (in)
 *
 *  Independent Variables (Inputs)
 *      vTreeDbh (in)
 *      vTreeSpecies (index)
 */

void EqCalc::TreeBarkThicknessFofem6( void )
{
    // Access current input values
    const char* spp = vTreeSpecies->activeItemName().latin1();
    double dbh = vTreeDbh->m_nativeValue;
    // Calculate results
    // The mortality model always limits dbh lower bound to 1.0 inch.
    if ( dbh < 1.0 )
    {
        dbh = 1.0;
    }
	double bt = Fofem6BarkThickness::barkThickness( spp, dbh );
    // Store results
    vTreeBarkThickness->update( bt );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeBarkThickness() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  o vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeCrownBaseHt
 *
 *  Dependent Variables (Outputs)
 *      vTreeCrownBaseHt (ft)
 *
 *  Independent Variables (Inputs)
 *      vTreeCrownRatio (fraction)
 *      vTreeHt (ft)
 */

void EqCalc::TreeCrownBaseHt( void )
{
    // Access current input values
    double cr = vTreeCrownRatio->m_nativeValue;
    double ht = vTreeHt->m_nativeValue;
    // Calculate results
    double bh = FBL_TreeCrownBaseHeight( cr, ht );
    // Store results
    vTreeCrownBaseHt->update( bh );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeCrownBaseHt() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCrownRatio %g %s\n", Margin,
            vTreeCrownRatio->m_nativeValue,
            vTreeCrownRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeHt %g %s\n", Margin,
            vTreeHt->m_nativeValue,
            vTreeHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeCrownBaseHt %g %s\n", Margin,
            vTreeCrownBaseHt->m_nativeValue,
            vTreeCrownBaseHt->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeCrownRatio
 *
 *  Dependent Variables (Outputs)
 *      vTreeCrownRatio (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeCrownBaseHt (ft)
 *      vTreeCoverHt (ft)
 */

void EqCalc::TreeCrownRatio( void )
{
    // Access current input values
    double bh = vTreeCrownBaseHt->m_nativeValue;
    double ht = vTreeCoverHt->m_nativeValue;
    // Calculate results
    double cr = FBL_TreeCrownRatio( bh, ht );
    // Store results
    vTreeCrownRatio->update( cr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeCrownRatio() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCrownBaseHt %g %s\n", Margin,
            vTreeCrownBaseHt->m_nativeValue,
            vTreeCrownBaseHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHt %g %s\n", Margin,
            vTreeCoverHt->m_nativeValue,
            vTreeCoverHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeCrownRatio %g %s\n", Margin,
            vTreeCrownRatio->m_nativeValue,
            vTreeCrownRatio->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeCrownVolScorchedAtVector
 *
 *  Dependent Variables (Outputs)
 *      vTreeCrownLengScorchedAtVector (ft)
 *      vTreeCrownLengFractionScorchedAtVector (fraction)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vSurfaceFireScorchHtAtVector (ft)
 *      vTreeCrownRatio (fraction)
 *      vTreeCoverHt (ft)
 */

void EqCalc::TreeCrownVolScorchedAtVector( void )
{
    // Access current input values
    double crownRatio = vTreeCrownRatio->m_nativeValue;
    double scorchHt   = vSurfaceFireScorchHtAtVector->m_nativeValue;
    double coverHt    = vTreeCoverHt->m_nativeValue;
    // Calculate results
    double scorchLeng, scorchFrac, scorchVol;
    scorchVol = FBL_TreeCrownScorch( coverHt, crownRatio, scorchHt,
        &scorchLeng, &scorchFrac );
    // Store results
    vTreeCrownLengScorchedAtVector->update( scorchLeng );
    vTreeCrownLengFractionScorchedAtVector->update( scorchFrac );
    vTreeCrownVolScorchedAtVector->update( scorchVol );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeCrownVolScorchedAtVector() 3 3\n", Margin );
        fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownRatio %g %s\n", Margin,
            vTreeCrownRatio->m_nativeValue,
            vTreeCrownRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHt %g %s\n", Margin,
            vTreeCoverHt->m_nativeValue,
            vTreeCoverHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeCrownLengFractionScorchedAtVector %g %s\n", Margin,
            vTreeCrownLengFractionScorchedAtVector->m_nativeValue,
            vTreeCrownLengFractionScorchedAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeCrownLengScorchedAtVector %g %s\n", Margin,
            vTreeCrownLengScorchedAtVector->m_nativeValue,
            vTreeCrownLengScorchedAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeMortalityCountAtVector
 *
 *  Dependent Variables (Outputs)
 *      VTreeMortalityCountAtVector (per ac)
 *
 *  Independent Variables (Inputs)
 *      VTreeCount (per ac)
 *      vTreeMortalityRateAtVector (fraction)
 */

void EqCalc::TreeMortalityCountAtVector( void )
{
    // Access current input values
    double mr = vTreeMortalityRateAtVector->m_nativeValue;
    double tc = vTreeCount->m_nativeValue;
    // Calculate results
    double mc = mr * tc;
    // Store results
    vTreeMortalityCountAtVector->update( mc );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityCountAtVector() 2 1\n", Margin );
        fprintf( m_log, "%s  i vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCount %g %s\n", Margin,
            vTreeCount->m_nativeValue,
            vTreeCount->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityCountAtVector %g %s\n", Margin,
            vTreeMortalityCountAtVector->m_nativeValue,
            vTreeMortalityCountAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateAspenAtVector
 *
 *  Dependent Variables (Outputs)
 *      VTreeMortalityRateAs[enAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeDbh (in)
 *      vSurfaceFireFlameLengAtVector (ft)
 *      vSurfaceFireSeverityAspen (discrete)
 */

void EqCalc::TreeMortalityRateAspenAtVector( void )
{
    // Access current input values
    double dbh = vTreeDbh->m_nativeValue;
    double flame = vSurfaceFireFlameLengAtVector->m_nativeValue;
    int    severity = vSurfaceFireSeverityAspen->activeItemDataIndex();
    // Calculate results
    double mr = FBL_AspenMortality( severity, flame, dbh );
    // Store results
    vTreeMortalityRateAspenAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateAspenAtVector() 3 1\n", Margin );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireSeverityAspen %d %s\n", Margin,
            vSurfaceFireSeverityAspen->activeItemDataIndex(),
            vSurfaceFireSeverityAspen->activeItemName().latin1() );
        fprintf( m_log, "%s  i vSurfaceFireFlameLengAtVector %g %s\n", Margin,
            vSurfaceFireFlameLengAtVector->m_nativeValue,
            vSurfaceFireFlameLengAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAspenAtVector %g %s\n", Margin,
            vTreeMortalityRateAspenAtVector->m_nativeValue,
            vTreeMortalityRateAspenAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateBehaveAtVector - DEPRECATED, HISTORICAL ONLY!
 *
 *  Dependent Variables (Outputs)
 *      VTreeMortalityRateAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeBarkThickness (in)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *      vSurfaceFireScorchHtAtVector (ft)
 */

void EqCalc::TreeMortalityRateBehaveAtVector( void )
{
    // Access current input values
    double barkThickness = vTreeBarkThickness->m_nativeValue;
    double scorchHt = vSurfaceFireScorchHtAtVector->m_nativeValue;
    double crownVolScorched = vTreeCrownVolScorchedAtVector->m_nativeValue;
    // Calculate results
    double mr = FBL_TreeMortalityBehave( barkThickness, scorchHt,
        crownVolScorched ) ;
    // Store results
    vTreeMortalityRateAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateBehaveAtVector() 3 1\n", Margin );
        fprintf( m_log, "%s  i vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateFofemAtVector - FOFEM Version - DEPRECATED, HISTORICAL ONLY!
 *
 *      This does not include Equation 3 for Populus tremuloides,
 *      nor does it handles cases where dbh < 1".
 *
 *  Dependent Variables (Outputs)
 *      vTreeMortalityRateAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeBarkThickness (in)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *      vTreeSpecies (index)
 *      vSurfaceFireScorchHtAtVector
 */

void EqCalc::TreeMortalityRateFofemAtVector( void )
{
    // Access current input values
    int    speciesIndex  = vTreeSpecies->activeItemDataIndex();
    double barkThickness = vTreeBarkThickness->m_nativeValue;
    double crownVolScorched = vTreeCrownVolScorchedAtVector->m_nativeValue;
    double scorchHt = vSurfaceFireScorchHtAtVector->m_nativeValue;

    // Calculate results
    double mr = FBL_TreeMortalityFofem( speciesIndex, barkThickness,
        crownVolScorched, scorchHt );
    // Store results
    vTreeMortalityRateAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateFofemAtVector() 4 1\n", Margin );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  i vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateFofem2AtVector - FOFEM Version - DEPRECATED, HISTORICAL ONLY!
 *
 *      This does not include Equation 3 for Populus tremuloides,
 *      nor does it handles cases where dbh < 1".
 *
 *  Dependent Variables (Outputs)
 *      vTreeMortalityRateAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeBarkThickness (in)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *      vTreeSpeciesMortality (index)
 *      vSurfaceFireScorchHtAtVector
 */

void EqCalc::TreeMortalityRateFofem2AtVector( void )
{
    // Access current input values
    int    speciesIndex  = vTreeSpeciesMortality->activeItemDataIndex();
    double barkThickness = vTreeBarkThickness->m_nativeValue;
    double crownVolScorched = vTreeCrownVolScorchedAtVector->m_nativeValue;
    double scorchHt = vSurfaceFireScorchHtAtVector->m_nativeValue;

    // 0==nonspruce, 1=spruce
    speciesIndex = ( speciesIndex == 0 ) ? 0 : 105;

    // Calculate results
    double mr = FBL_TreeMortalityFofem( speciesIndex, barkThickness,
        crownVolScorched, scorchHt );
    // Store results
    vTreeMortalityRateAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateFofem2AtVector() 4 1\n", Margin );
        fprintf( m_log, "%s  i vTreeSpeciesMortality %d %s\n", Margin,
            vTreeSpeciesMortality->activeItemDataIndex(),
            vTreeSpeciesMortality->activeItemName().latin1() );
        fprintf( m_log, "%s  i vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateFofem6AtVector
 *
 *  Combines the FOFEM and Hood models in a single entry point as per FOFEM v6.
 *
 *  Dependent Variables (Outputs)
 *      vTreeMortalityRateAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeBarkThickness (in)
 *      vTreeCrownLengScorchedAtVector (fraction)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *      vTreeSpecies (index)
 *      vSurfaceFireScorchHtAtVector
 */

void EqCalc::TreeMortalityRateFofem6AtVector( void )
{
    // Access current input values
    const char* spp = vTreeSpecies->activeItemName().latin1();
    double dbh = vTreeDbh->m_nativeValue;						/* inches */
	double cls = vTreeCrownLengFractionScorchedAtVector->m_nativeValue;	/* fraction */
    double cvs = vTreeCrownVolScorchedAtVector->m_nativeValue;	/* fraction */
    double brk = vTreeBarkThickness->m_nativeValue;				/* inches */
    double sht = vSurfaceFireScorchHtAtVector->m_nativeValue;	/* ft */

	double mr = Fofem6Mortality::mortalityRate( spp, dbh, brk, sht, cls, cvs );

    // Store results
    vTreeMortalityRateAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateFofem6AtVector() 6 1\n", Margin );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
		fprintf( m_log, "%s  i vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
		fprintf( m_log, "%s  i vTreeCrownLengFractionScorchedAtVector %g %s\n", Margin,
            vTreeCrownLengFractionScorchedAtVector->m_nativeValue,
            vTreeCrownLengFractionScorchedAtVector->m_nativeUnits.latin1() );
 		fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief TreeMortalityRateFofemHoodAtVector - DEPRECATED, HISTORICAL ONLY!
 *
 *  Combines the FOFEM and Hood models in a single entry point.
 *
 *  Dependent Variables (Outputs)
 *      vTreeMortalityRateAtVector (fraction)
 *
 *  Independent Variables (Inputs)
 *      vTreeBarkThickness (in)
 *      vTreeCrownLengScorchedAtVector (fraction)
 *      vTreeCrownVolScorchedAtVector (fraction)
 *      vTreeSpecies (index)
 *      vSurfaceFireScorchHtAtVector
 */

void EqCalc::TreeMortalityRateFofemHoodAtVector( void )
{
    // Access current input values
    int    spp = vTreeSpecies->activeItemDataIndex();			/* index */
    double dbh = vTreeDbh->m_nativeValue;						/* inches */
	double cls = vTreeCrownLengFractionScorchedAtVector->m_nativeValue;	/* fraction */
    double cvs = vTreeCrownVolScorchedAtVector->m_nativeValue;	/* fraction */
    double brk = vTreeBarkThickness->m_nativeValue;				/* inches */
    double sht = vSurfaceFireScorchHtAtVector->m_nativeValue;	/* ft */
	double mr  = 0.;

    // Determine whether to use FOFEM or Hood based on species index
    int hood = -1;
    if ( spp == 2 ) hood = 0;                       // ABICON (FOFEM v6 Eq 10)
    else if ( spp == 3 || spp == 4 ) hood = 1;      // ABIGRA & ABILAS (Eq 11)
    else if ( spp == 5 ) hood = 2;                  // ABIMAG (Eq 16)
    else if ( spp == 78 ) hood = 3;                 // LIBDEC (Eq 12)
    else if ( spp == 77) hood = 4;                  // LAROCC (Eq 14)
    else if ( spp == 110 || spp == 114 ) hood = 5;  // PINALB & PINCON (Eq 17)
    else if ( spp == 103 ) hood = 6;                // PICENG (Eq 15)
    else if ( spp == 120 ) hood = 7;                // PINLAM (Eq 18)
    else if ( spp == 123 || spp == 119 ) hood = 8;  // PINPON & PINJEF (Eq 19)
    else if ( spp == 147 ) hood = 9;                // PSEMEN (Eq 20)

    // If using Hood ...
    if ( hood >= 0 )
    {
        mr = FBL_TreeMortalityHood( hood, ( 2.54 * dbh ), cls, cvs );
    }
    // ... otherwise using FOFEM
    else
    {
        mr = FBL_TreeMortalityFofem( spp, brk, cvs, sht );
    }
    // Store results
    vTreeMortalityRateAtVector->update( mr );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc TreeMortalityRateFofemHoodAtVector() 6 1\n", Margin );
        fprintf( m_log, "%s  i vTreeSpecies %d %s\n", Margin,
            vTreeSpecies->activeItemDataIndex(),
            vTreeSpecies->activeItemName().latin1() );
        fprintf( m_log, "%s  i vTreeDbh %g %s\n", Margin,
            vTreeDbh->m_nativeValue,
            vTreeDbh->m_nativeUnits.latin1() );
		fprintf( m_log, "%s  i vTreeBarkThickness %g %s\n", Margin,
            vTreeBarkThickness->m_nativeValue,
            vTreeBarkThickness->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownVolScorchedAtVector %g %s\n", Margin,
            vTreeCrownVolScorchedAtVector->m_nativeValue,
            vTreeCrownVolScorchedAtVector->m_nativeUnits.latin1() );
		fprintf( m_log, "%s  i vTreeCrownLengFractionScorchedAtVector %g %s\n", Margin,
            vTreeCrownLengFractionScorchedAtVector->m_nativeValue,
            vTreeCrownLengFractionScorchedAtVector->m_nativeUnits.latin1() );
 		fprintf( m_log, "%s  i vSurfaceFireScorchHtAtVector %g %s\n", Margin,
            vSurfaceFireScorchHtAtVector->m_nativeValue,
            vSurfaceFireScorchHtAtVector->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vTreeMortalityRateAtVector %g %s\n", Margin,
            vTreeMortalityRateAtVector->m_nativeValue,
            vTreeMortalityRateAtVector->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WindAdjFactor
 *
 *  Dependent Variables (Outputs)
 *      vTreeCanopyCrownFraction (ft3/ft3).
 *      vWindAdjFactor (dl)
 *      vWindAdjMethod (discrete)
 *
 *  Independent Variables (Inputs)
 *      vTreeCanopyCover (fraction)
 *      vTreeCoverHt (ft)
 *      vTreeCrownRatio (fraction)
 *      vSurfaceFuelBedDepth (ft)
 */

void EqCalc::WindAdjFactor( void )
{
    // Access current input values
    double cc = vTreeCanopyCover->m_nativeValue;
    double ch = vTreeCoverHt->m_nativeValue;
    double cr = vTreeCrownRatio->m_nativeValue;
    double fd = vSurfaceFuelBedDepth->m_nativeValue;
    // Calculate results
    int method = 0;
    double fraction = 0.0;
    double waf = FBL_WindAdjustmentFactor( cc, ch, cr, fd, &fraction, &method );
    // Store results
    vWindAdjFactor->update( waf );
    vWindAdjMethod->updateItem( method );
    vTreeCanopyCrownFraction->update( fraction );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WindAdjFactor() 4 1\n", Margin );
        fprintf( m_log, "%s  i vTreeCanopyCover %d %s\n", Margin,
            vTreeCanopyCover->activeItemDataIndex(),
            vTreeCanopyCover->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCoverHt %d %s\n", Margin,
            vTreeCoverHt->activeItemDataIndex(),
            vTreeCoverHt->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vTreeCrownRatio %d %s\n", Margin,
            vTreeCrownRatio->activeItemDataIndex(),
            vTreeCrownRatio->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSurfaceFuelBedDepth %d %s\n", Margin,
            vSurfaceFuelBedDepth->activeItemDataIndex(),
            vSurfaceFuelBedDepth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWindAdjFactor %d %s\n", Margin,
            vWindAdjFactor->activeItemDataIndex(),
            vWindAdjFactor->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWindAdjMethod %d %s\n", Margin,
            vWindAdjMethod->activeItemDataIndex(),
            vWindAdjMethod->activeItemName().latin1() );
        fprintf( m_log, "%s  o vTreeCanopyCrownFraction %d %s\n", Margin,
            vTreeCanopyCrownFraction->activeItemDataIndex(),
            vTreeCanopyCrownFraction->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WindDirFromNorth
 *
 *  Dependent Variables (Outputs)
 *      vWindDirFromNorth (degrees clockwise from North)
 *
 *  Independent Variables (Inputs)
 *      vWindDirFromCompass (compass point name)
 */

void EqCalc::WindDirFromNorth( void )
{
    // Access current input values
    int id = vWindDirFromCompass->activeItemDataIndex();
    // Calculate results
    double dd = 22.5 * (double) id;
    // Store results
    vWindDirFromNorth->update( dd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WindDirFromNorth() 1 1\n", Margin );
        fprintf( m_log, "%s  i vWindDirFromCompass %d %s\n", Margin,
            vWindDirFromCompass->activeItemDataIndex(),
            vWindDirFromCompass->activeItemName().latin1() );
        fprintf( m_log, "%s  o vWindDirFromNorth %g %s\n", Margin,
            vWindDirFromNorth->m_nativeValue,
            vWindDirFromNorth->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WindDirFromUpslope
 *
 *  Dependent Variables (Outputs)
 *      vWindDirFromUpslope (VECTOR degrees clockwise from upslope)
 *
 *  Independent Variables (Inputs)
 *      vSiteUpslopeDirFromNorth
 *      vWindDirFromNorth (SOURCE degrees clockwise from north)
 */

void EqCalc::WindDirFromUpslope( void )
{
    // Access current input values
    double sd = vSiteUpslopeDirFromNorth->m_nativeValue;
    double wd = vWindDirFromNorth->m_nativeValue;
    // Calculate results
    // Convert from source wind to wind vector
    if ( (wd -= 180.) < 0. )
    {
        wd += 360.;
    }
    // Get the angle between slope and wind vector
    double dd = wd - sd;
    if ( dd < 0. )
    {
        dd += 360.;
    }
    if ( fabs( dd ) < 0.5 )
    {
        dd = 0.0;
    }
    // Store results
    vWindDirFromUpslope->update( dd );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WindDirFromUpslope() 2 1\n", Margin );
        fprintf( m_log, "%s  i vSiteUpslopeDirFromNorth %g %s\n", Margin,
            vSiteUpslopeDirFromNorth->m_nativeValue,
            vSiteUpslopeDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindDirFromNorth %g %s\n", Margin,
            vWindDirFromNorth->m_nativeValue,
            vWindDirFromNorth->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWindDirFromUpslope %g %s\n", Margin,
            vWindDirFromUpslope->m_nativeValue,
            vWindDirFromUpslope->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WindSpeedAt20Ft
 *
 *  Dependent Variables (Outputs)
 *      vWindSpeedAt20Ft (mi/h)
 *
 *  Independent Variables (Inputs)
 *      vWindSpeedAt10M (mi/h)
 */

void EqCalc::WindSpeedAt20Ft ( void )
{
    // Access current input values
    double w10m = vWindSpeedAt10M->m_nativeValue;
    // Calculate results
    double w20ft = FBL_WindSpeedAt20Ft( w10m );
    // Store results
    vWindSpeedAt20Ft->update( w20ft );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WindSpeedAt20Ft() 1 1\n", Margin );
        fprintf( m_log, "%s  i vWindSpeedAt10M %g %s\n", Margin,
            vWindSpeedAt10M->m_nativeValue,
            vWindSpeedAt10M->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WindSpeedAtMidflame
 *
 *  Dependent Variables (Outputs)
 *      vWindSpeedAtMidflame (mi/h)
 *
 *  Independent Variables (Inputs)
 *      vWindSpeedAt20Ft (mi/h)
 *      vWindAdjFactor (fraction)
 */

void EqCalc::WindSpeedAtMidflame ( void )
{
    // Access current input values
    double ws = vWindSpeedAt20Ft->m_nativeValue;
    double rf = vWindAdjFactor->m_nativeValue;
    // Calculate results
    double wm = ws * rf;
    // Store results
    vWindSpeedAtMidflame->update( wm );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WindSpeedAtMidflame() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWindSpeedAt20Ft %g %s\n", Margin,
            vWindSpeedAt20Ft->m_nativeValue,
            vWindSpeedAt20Ft->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindAdjFactor %g %s\n", Margin,
            vWindAdjFactor->m_nativeValue,
            vWindAdjFactor->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrCumulusBaseHt
 *      From http://www.usatoday.com/weather/whumcalc.htm
 *
 *  Dependent Variables (Outputs)
 *      vWthrCumulusBaseHt (ft)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWthrDewPointTemp (oF)
 */

void EqCalc::WthrCumulusBaseHt( void )
{
    // Access current input values
    double Tf = vWthrAirTemp->m_nativeValue;
    double Tdf = vWthrDewPointTemp->m_nativeValue;
    // Calculate results
    double ht = 222. * (Tf - Tdf);
    if ( ht < 0. )
    {
        ht = 0.0;
    }
    // Store results
    vWthrCumulusBaseHt->update( ht );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrCumulusBaseHt() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrDewPointTemp %g %s\n", Margin,
            vWthrDewPointTemp->m_nativeValue,
            vWthrDewPointTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrCumulusBaseHt %g %s\n", Margin,
            vWthrCumulusBaseHt->m_nativeValue,
            vWthrCumulusBaseHt->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrDewPointTemp
 *
 *  Dependent Variables (Outputs)
 *      vWthrDewPointTemp (oF)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWthrWetBulbTemp (oF)
 *      vSiteElevation (ft)
 */

void EqCalc::WthrDewPointTemp( void )
{
    // Access current input values
    double dbulb = vWthrAirTemp->m_nativeValue;
    double wbulb = vWthrWetBulbTemp->m_nativeValue;
    double elev  = vSiteElevation->m_nativeValue;
    // Calculate results
    double dewpoint = FBL_DewPointTemperature( dbulb, wbulb, elev );
    // Store results
    vWthrDewPointTemp->update( dewpoint );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrDewPointTemp() 3 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrWetBulbTemp %g %s\n", Margin,
            vWthrWetBulbTemp->m_nativeValue,
            vWthrWetBulbTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vSiteElevation %g %s\n", Margin,
            vSiteElevation->m_nativeValue,
            vSiteElevation->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrDewPointTemp %g %s\n", Margin,
            vWthrDewPointTemp->m_nativeValue,
            vWthrDewPointTemp->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrHeatIndex
 *      From http://www.usatoday.com/weather/whumcalc.htm
 *
 *  Dependent Variables (Outputs)
 *      vWthrHeatIndex (dl)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWthrRelativeHumidity (%)
 */

void EqCalc::WthrHeatIndex( void )
{
    // Access current input values
    double at = vWthrAirTemp->m_nativeValue;
    double rh = vWthrRelativeHumidity->m_nativeValue;
    // Calculate results
    double hi = FBL_HeatIndex1( at, rh );
    // Store results
    vWthrHeatIndex->update( hi );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrHeatIndex() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrRelativeHumidity %g %s\n", Margin,
            vWthrRelativeHumidity->m_nativeValue,
            vWthrRelativeHumidity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrHeatIndex %g %s\n", Margin,
            vWthrHeatIndex->m_nativeValue,
            vWthrHeatIndex->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrRelativeHumidity
 *
 *  Dependent Variables (Outputs)
 *      vWthrRelativeHumidity (percent)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWthrDewPointTemp (oF)
 */

void EqCalc::WthrRelativeHumidity( void )
{
    // Access current input values
    double dbulb = vWthrAirTemp->m_nativeValue;
    double dewpt = vWthrDewPointTemp->m_nativeValue;
    // Calculate results
    double rh    = 100. * FBL_RelativeHumidity( dbulb, dewpt );
    // Store results
    vWthrRelativeHumidity->update( rh );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrRelativeHumidity() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrDewPointTemp %g %s\n", Margin,
            vWthrDewPointTemp->m_nativeValue,
            vWthrDewPointTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrRelativeHumidity %g %s\n", Margin,
            vWthrRelativeHumidity->m_nativeValue,
            vWthrRelativeHumidity->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrSummerSimmerIndex
 *      From http://www.usatoday.com/weather/whumcalc.htm
 *
 *  Dependent Variables (Outputs)
 *      vWthrSummerSimmerIndex (dl)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWthrRelativeHumidity (%)
 */

void EqCalc::WthrSummerSimmerIndex( void )
{
    // Access current input values
    double at = vWthrAirTemp->m_nativeValue;
    double rh = vWthrRelativeHumidity->m_nativeValue;
    // Calculate results
    double ssi = FBL_SummerSimmerIndex( at, rh );
    // Store results
    vWthrSummerSimmerIndex->update( ssi );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrSummerSimmerIndex() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWthrRelativeHumidity %g %s\n", Margin,
            vWthrRelativeHumidity->m_nativeValue,
            vWthrRelativeHumidity->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrSummerSimmerIndex %g %s\n", Margin,
            vWthrSummerSimmerIndex->m_nativeValue,
            vWthrSummerSimmerIndex->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief WthrWindChillTemp
 *
 *  Dependent Variables (Outputs)
 *      vWthrWindChillTemp (oF)
 *
 *  Independent Variables (Inputs)
 *      vWthrAirTemp (oF)
 *      vWindSpeedAtMidflame (mi/h)
 */

void EqCalc::WthrWindChillTemp( void )
{
    // Access current input values
    double at = vWthrAirTemp->m_nativeValue;
    double ws = vWindSpeedAtMidflame->m_nativeValue;
    // Calculate results
    double wc = FBL_WindChillTemperature( at, ws );
    // Store results
    vWthrWindChillTemp->update( wc );
    // Log results
    if( m_log )
    {
        fprintf( m_log, "%sbegin proc WthrWindChill() 2 1\n", Margin );
        fprintf( m_log, "%s  i vWthrAirTemp %g %s\n", Margin,
            vWthrAirTemp->m_nativeValue,
            vWthrAirTemp->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  i vWindSpeedAtMidflame %g %s\n", Margin,
            vWindSpeedAtMidflame->m_nativeValue,
            vWindSpeedAtMidflame->m_nativeUnits.latin1() );
        fprintf( m_log, "%s  o vWthrWindChillTemp %g %s\n", Margin,
            vWthrWindChillTemp->m_nativeValue,
            vWthrWindChillTemp->m_nativeUnits.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqCalc constructor.
 */

EqCalc::EqCalc( EqTree *eqTree ) :
    m_eqTree(eqTree),
    m_log(0)
{
	m_Bp6SurfaceFire		= new Bp6SurfaceFire();
	m_Bp6CrownFire			= new Bp6CrownFire();

    vContainAttackBack       = m_eqTree->getVarPtr( "vContainAttackBack" );
    vContainAttackDist       = m_eqTree->getVarPtr( "vContainAttackDist" );
    vContainAttackHead       = m_eqTree->getVarPtr( "vContainAttackHead" );
    vContainAttackPerimeter  = m_eqTree->getVarPtr( "vContainAttackPerimeter" );
    vContainAttackSize       = m_eqTree->getVarPtr( "vContainAttackSize" );
    vContainAttackTactic     = m_eqTree->getVarPtr( "vContainAttackTactic" );
    vContainCost             = m_eqTree->getVarPtr( "vContainCost" );
    vContainDiagram          = m_eqTree->getVarPtr( "vContainDiagram" );
    vContainLimitDist        = m_eqTree->getVarPtr( "vContainLimitDist" );
    vContainLine             = m_eqTree->getVarPtr( "vContainLine" );
    vContainPoints           = m_eqTree->getVarPtr( "vContainPoints" );
    vContainReportBack       = m_eqTree->getVarPtr( "vContainReportBack" );
    vContainReportHead       = m_eqTree->getVarPtr( "vContainReportHead" );
    vContainReportRatio      = m_eqTree->getVarPtr( "vContainReportRatio" );
    vContainReportSize       = m_eqTree->getVarPtr( "vContainReportSize" );
    vContainReportSpread     = m_eqTree->getVarPtr( "vContainReportSpread" );
    vContainResourceArrival  = m_eqTree->getVarPtr( "vContainResourceArrival" );
    vContainResourceBaseCost = m_eqTree->getVarPtr( "vContainResourceBaseCost" );
    vContainResourceDuration = m_eqTree->getVarPtr( "vContainResourceDuration" );
    vContainResourceHourCost = m_eqTree->getVarPtr( "vContainResourceHourCost" );
    vContainResourceName     = m_eqTree->getVarPtr( "vContainResourceName" );
    vContainResourceProd     = m_eqTree->getVarPtr( "vContainResourceProd" );
    vContainResourcesUsed    = m_eqTree->getVarPtr( "vContainResourcesUsed" );
    vContainSize             = m_eqTree->getVarPtr( "vContainSize" );
    vContainStatus           = m_eqTree->getVarPtr( "vContainStatus" );
    vContainTime             = m_eqTree->getVarPtr( "vContainTime" );
    vContainXMax             = m_eqTree->getVarPtr( "vContainXMax" );
    vContainXMin             = m_eqTree->getVarPtr( "vContainXMin" );
    vContainYMax             = m_eqTree->getVarPtr( "vContainYMax" );

	vCrownFireActiveCritOpenWindSpeed	= m_eqTree->getVarPtr( "vCrownFireActiveCritOpenWindSpeed" );
	vCrownFireActiveCritSurfSpreadRate	= m_eqTree->getVarPtr( "vCrownFireActiveCritSurfSpreadRate" );
    vCrownFireActiveCrown				= m_eqTree->getVarPtr( "vCrownFireActiveCrown" );
	vCrownFireActiveFireArea			= m_eqTree->getVarPtr( "vCrownFireActiveFireArea" );
	vCrownFireActiveFireLineInt			= m_eqTree->getVarPtr( "vCrownFireActiveFireLineInt" );
	vCrownFireActiveFirePerimeter		= m_eqTree->getVarPtr( "vCrownFireActiveFirePerimeter" );
	vCrownFireActiveFireWidth			= m_eqTree->getVarPtr( "vCrownFireActiveFireWidth" );
	vCrownFireActiveFlameLeng			= m_eqTree->getVarPtr( "vCrownFireActiveFlameLeng" );
	vCrownFireActiveHeatPerUnitArea		= m_eqTree->getVarPtr( "vCrownFireActiveHeatPerUnitArea" );
    vCrownFireActiveRatio				= m_eqTree->getVarPtr( "vCrownFireActiveRatio" );
	vCrownFireActiveSpreadDist			= m_eqTree->getVarPtr( "vCrownFireActiveSpreadDist" );
	vCrownFireActiveSpreadMapDist		= m_eqTree->getVarPtr( "vCrownFireActiveSpreadMapDist" );
	vCrownFireActiveSpreadRate			= m_eqTree->getVarPtr( "vCrownFireActiveSpreadRate" );
    //vCrownFireArea					= m_eqTree->getVarPtr( "vCrownFireArea" );
	vCrownFireCanopyFractionBurned		= m_eqTree->getVarPtr( "vCrownFireCanopyFractionBurned" );
    vCrownFireCritCrownSpreadRate		= m_eqTree->getVarPtr( "vCrownFireCritCrownSpreadRate" );
    vCrownFireCritSurfFireInt			= m_eqTree->getVarPtr( "vCrownFireCritSurfFireInt" );
    vCrownFireCritSurfFlameLeng			= m_eqTree->getVarPtr( "vCrownFireCritSurfFlameLeng" );
    vCrownFireCritSurfSpreadRate		= m_eqTree->getVarPtr( "vCrownFireCritSurfSpreadRate" );
    //vCrownFireFlameLeng				= m_eqTree->getVarPtr( "vCrownFireFlameLeng" );
    vCrownFireFuelLoad					= m_eqTree->getVarPtr( "vCrownFireFuelLoad" );
	//vCrownFireHeatPerUnitArea			= m_eqTree->getVarPtr( "vCrownFireHeatPerUnitArea" );
    vCrownFireHeatPerUnitAreaCanopy		= m_eqTree->getVarPtr( "vCrownFireHeatPerUnitAreaCanopy" );
	vCrownFireLengthToWidth				= m_eqTree->getVarPtr( "vCrownFireLengthToWidth" );
    //vCrownFireLineInt					= m_eqTree->getVarPtr( "vCrownFireLineInt" );
	vCrownFirePassiveFireArea			= m_eqTree->getVarPtr( "vCrownFirePassiveFireArea" );
	vCrownFirePassiveFireLineInt		= m_eqTree->getVarPtr( "vCrownFirePassiveFireLineInt" );
	vCrownFirePassiveFirePerimeter		= m_eqTree->getVarPtr( "vCrownFirePassiveFirePerimeter" );
	vCrownFirePassiveFireWidth			= m_eqTree->getVarPtr( "vCrownFirePassiveFireWidth" );
	vCrownFirePassiveFlameLeng			= m_eqTree->getVarPtr( "vCrownFirePassiveFlameLeng" );
	vCrownFirePassiveHeatPerUnitArea	= m_eqTree->getVarPtr( "vCrownFirePassiveHeatPerUnitArea" );
	vCrownFirePassiveSpreadDist			= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadDist" );
    vCrownFirePassiveSpreadMapDist		= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadMapDist" );
	vCrownFirePassiveSpreadRate			= m_eqTree->getVarPtr( "vCrownFirePassiveSpreadRate" );
	//vCrownFirePerimeter				= m_eqTree->getVarPtr( "vCrownFirePerimeter" );
    vCrownFirePowerOfFire				= m_eqTree->getVarPtr( "vCrownFirePowerOfFire" );
    vCrownFirePowerOfWind				= m_eqTree->getVarPtr( "vCrownFirePowerOfWind" );
    vCrownFirePowerRatio				= m_eqTree->getVarPtr( "vCrownFirePowerRatio" );
    //vCrownFireSpreadDist				= m_eqTree->getVarPtr( "vCrownFireSpreadDist" );
    //vCrownFireSpreadMapDist			= m_eqTree->getVarPtr( "vCrownFireSpreadMapDist" );
    //vCrownFireSpreadRate				= m_eqTree->getVarPtr( "vCrownFireSpreadRate" );
    vCrownFireTransRatio				= m_eqTree->getVarPtr( "vCrownFireTransRatio" );
    vCrownFireTransToCrown				= m_eqTree->getVarPtr( "vCrownFireTransToCrown" );
    vCrownFireType						= m_eqTree->getVarPtr( "vCrownFireType" );
    vCrownFireWindDriven				= m_eqTree->getVarPtr( "vCrownFireWindDriven" );

    vDocDescription      = m_eqTree->getVarPtr( "vDocDescription" );
    vDocFireAnalyst      = m_eqTree->getVarPtr( "vDocFireAnalyst" );
    vDocFireName         = m_eqTree->getVarPtr( "vDocFireName" );
    vDocFirePeriod       = m_eqTree->getVarPtr( "vDocFirePeriod" );
    vDocRxAdminUnit      = m_eqTree->getVarPtr( "vDocRxAdminUnit" );
    vDocRxName           = m_eqTree->getVarPtr( "vDocRxName" );
    vDocRxPreparedBy     = m_eqTree->getVarPtr( "vDocRxPreparedBy" );
    vDocTrainingCourse   = m_eqTree->getVarPtr( "vDocTrainingCourse" );
    vDocTrainingExercise = m_eqTree->getVarPtr( "vDocTrainingExercise" );
    vDocTrainingTrainee  = m_eqTree->getVarPtr( "vDocTrainingTrainee" );

    vIgnitionFirebrandFuelMois  = m_eqTree->getVarPtr( "vIgnitionFirebrandFuelMois" );
    vIgnitionFirebrandProb      = m_eqTree->getVarPtr( "vIgnitionFirebrandProb" );
    vIgnitionLightningDuffDepth = m_eqTree->getVarPtr( "vIgnitionLightningDuffDepth" );
    vIgnitionLightningFuelMois  = m_eqTree->getVarPtr( "vIgnitionLightningFuelMois" );
    vIgnitionLightningFuelType  = m_eqTree->getVarPtr( "vIgnitionLightningFuelType" );
    vIgnitionLightningProb      = m_eqTree->getVarPtr( "vIgnitionLightningProb" );

    vMapContourCount			= m_eqTree->getVarPtr( "vMapContourCount" );
    vMapContourInterval			= m_eqTree->getVarPtr( "vMapContourInterval" );
    vMapDist					= m_eqTree->getVarPtr( "vMapDist" );
    vMapFraction				= m_eqTree->getVarPtr( "vMapFraction" );
    vMapScale					= m_eqTree->getVarPtr( "vMapScale" );

    vSafetyZoneEquipmentArea	= m_eqTree->getVarPtr( "vSafetyZoneEquipmentArea" );
    vSafetyZoneEquipmentNumber	= m_eqTree->getVarPtr( "vSafetyZoneEquipmentNumber" );
    vSafetyZonePersonnelArea	= m_eqTree->getVarPtr( "vSafetyZonePersonnelArea" );
    vSafetyZonePersonnelNumber	= m_eqTree->getVarPtr( "vSafetyZonePersonnelNumber" );
    vSafetyZoneLength			= m_eqTree->getVarPtr( "vSafetyZoneLength" );
    vSafetyZoneRadius			= m_eqTree->getVarPtr( "vSafetyZoneRadius" );
    vSafetyZoneSepDist			= m_eqTree->getVarPtr( "vSafetyZoneSepDist" );
    vSafetyZoneSize				= m_eqTree->getVarPtr( "vSafetyZoneSize" );
    vSafetyZoneSizeSquare		= m_eqTree->getVarPtr( "vSafetyZoneSizeSquare" );

    vSiteAspectDirFromCompass	= m_eqTree->getVarPtr( "vSiteAspectDirFromCompass" );
    vSiteAspectDirFromNorth		= m_eqTree->getVarPtr( "vSiteAspectDirFromNorth" );
    vSiteElevation				= m_eqTree->getVarPtr( "vSiteElevation" );
    vSiteLatitude				= m_eqTree->getVarPtr( "vSiteLatitude" );
    vSiteLongitude				= m_eqTree->getVarPtr( "vSiteLongitude" );
    vSiteRidgeToValleyDist		= m_eqTree->getVarPtr( "vSiteRidgeToValleyDist" );
    vSiteRidgeToValleyElev		= m_eqTree->getVarPtr( "vSiteRidgeToValleyElev" );
    vSiteRidgeToValleyMapDist	= m_eqTree->getVarPtr( "vSiteRidgeToValleyMapDist" );
    vSiteSlopeDegrees			= m_eqTree->getVarPtr( "vSiteSlopeDegrees" );
    vSiteSlopeFraction			= m_eqTree->getVarPtr( "vSiteSlopeFraction" );
    vSiteSlopeReach				= m_eqTree->getVarPtr( "vSiteSlopeReach" );
    vSiteSlopeRise				= m_eqTree->getVarPtr( "vSiteSlopeRise" );
    vSiteSunShading				= m_eqTree->getVarPtr( "vSiteSunShading" );
    vSiteTimeZone				= m_eqTree->getVarPtr( "vSiteTimeZone" );
    vSiteUpslopeDirFromNorth	= m_eqTree->getVarPtr( "vSiteUpslopeDirFromNorth" );

    vSpotCoverHtBurningPile        = m_eqTree->getVarPtr( "vSpotCoverHtBurningPile" );
    vSpotCoverHtSurfaceFire        = m_eqTree->getVarPtr( "vSpotCoverHtSurfaceFire" );
    vSpotCoverHtTorchingTrees      = m_eqTree->getVarPtr( "vSpotCoverHtTorchingTrees" );
    vSpotDistActiveCrown           = m_eqTree->getVarPtr( "vSpotDistActiveCrown" );
    vSpotDistBurningPile           = m_eqTree->getVarPtr( "vSpotDistBurningPile" );
    vSpotDistSurfaceFire           = m_eqTree->getVarPtr( "vSpotDistSurfaceFire" );
    vSpotDistTorchingTrees         = m_eqTree->getVarPtr( "vSpotDistTorchingTrees" );
    vSpotEmberDiamAtSurface        = m_eqTree->getVarPtr( "vSpotEmberDiamAtSurface" );
    vSpotFirebrandDriftSurfaceFire = m_eqTree->getVarPtr( "vSpotFirebrandDriftSurfaceFire" );
    vSpotFirebrandHtActiveCrown    = m_eqTree->getVarPtr( "vSpotFirebrandHtActiveCrown" );
    vSpotFirebrandHtBurningPile    = m_eqTree->getVarPtr( "vSpotFirebrandHtBurningPile" );
    vSpotFirebrandHtSurfaceFire    = m_eqTree->getVarPtr( "vSpotFirebrandHtSurfaceFire" );
    vSpotFirebrandHtTorchingTrees  = m_eqTree->getVarPtr( "vSpotFirebrandHtTorchingTrees" );
    vSpotFlameDurTorchingTrees     = m_eqTree->getVarPtr( "vSpotFlameDurTorchingTrees" );
    vSpotFlameHtActiveCrown        = m_eqTree->getVarPtr( "vSpotFlameHtActiveCrown" );
    vSpotFlameHtTorchingTrees      = m_eqTree->getVarPtr( "vSpotFlameHtTorchingTrees" );
    vSpotFlameRatioTorchingTrees   = m_eqTree->getVarPtr( "vSpotFlameRatioTorchingTrees" );
    vSpotFlatDistActiveCrown       = m_eqTree->getVarPtr( "vSpotFlatDistActiveCrown" );
    vSpotFlatDistBurningPile       = m_eqTree->getVarPtr( "vSpotFlatDistBurningPile" );
    vSpotFlatDistSurfaceFire       = m_eqTree->getVarPtr( "vSpotFlatDistSurfaceFire" );
    vSpotFlatDistTorchingTrees     = m_eqTree->getVarPtr( "vSpotFlatDistTorchingTrees" );
    vSpotFireSource                = m_eqTree->getVarPtr( "vSpotFireSource" );
    vSpotMapDistActiveCrown        = m_eqTree->getVarPtr( "vSpotMapDistActiveCrown" );
    vSpotMapDistBurningPile        = m_eqTree->getVarPtr( "vSpotMapDistBurningPile" );
    vSpotMapDistSurfaceFire        = m_eqTree->getVarPtr( "vSpotMapDistSurfaceFire" );
    vSpotMapDistTorchingTrees      = m_eqTree->getVarPtr( "vSpotMapDistTorchingTrees" );
    vSpotTorchingTrees             = m_eqTree->getVarPtr( "vSpotTorchingTrees" );

    vSurfaceFireArea                 = m_eqTree->getVarPtr( "vSurfaceFireArea" );
    vSurfaceFireCharacteristicsDiagram = m_eqTree->getVarPtr( "vSurfaceFireCharacteristicsDiagram" );
    vSurfaceFireDistAtBack           = m_eqTree->getVarPtr( "vSurfaceFireDistAtBack" );
    vSurfaceFireDistAtBeta           = m_eqTree->getVarPtr( "vSurfaceFireDistAtBeta" );
    vSurfaceFireDistAtFlank          = m_eqTree->getVarPtr( "vSurfaceFireDistAtFlank" );
    vSurfaceFireDistAtHead           = m_eqTree->getVarPtr( "vSurfaceFireDistAtHead" );
    vSurfaceFireDistAtPsi            = m_eqTree->getVarPtr( "vSurfaceFireDistAtPsi" );
    vSurfaceFireDistAtVector         = m_eqTree->getVarPtr( "vSurfaceFireDistAtVector" );
    vSurfaceFireEccentricity         = m_eqTree->getVarPtr( "vSurfaceFireEccentricity" );
    vSurfaceFireEllipseF             = m_eqTree->getVarPtr( "vSurfaceFireEllipseF" );
    vSurfaceFireEllipseG             = m_eqTree->getVarPtr( "vSurfaceFireEllipseG" );
    vSurfaceFireEllipseH             = m_eqTree->getVarPtr( "vSurfaceFireEllipseH" );
    vSurfaceFireEffWindAtHead        = m_eqTree->getVarPtr( "vSurfaceFireEffWindAtHead" );
    vSurfaceFireEffWindAtVector      = m_eqTree->getVarPtr( "vSurfaceFireEffWindAtVector" );
    vSurfaceFireElapsedTime          = m_eqTree->getVarPtr( "vSurfaceFireElapsedTime" );
    vSurfaceFireFlameAngleAtVector   = m_eqTree->getVarPtr( "vSurfaceFireFlameAngleAtVector" );
    vSurfaceFireFlameHtAtVector      = m_eqTree->getVarPtr( "vSurfaceFireFlameHtAtVector" );
    vSurfaceFireFlameHtPile          = m_eqTree->getVarPtr( "vSurfaceFireFlameHtPile" );
    vSurfaceFireFlameLengAtBeta      = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtBeta" );
    vSurfaceFireFlameLengAtHead      = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtHead" );
    vSurfaceFireFlameLengAtPsi       = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtPsi" );
    vSurfaceFireFlameLengAtVector    = m_eqTree->getVarPtr( "vSurfaceFireFlameLengAtVector" );
    vSurfaceFireHeatPerUnitArea      = m_eqTree->getVarPtr( "vSurfaceFireHeatPerUnitArea" );
    vSurfaceFireHeatSource           = m_eqTree->getVarPtr( "vSurfaceFireHeatSource" );
    vSurfaceFireLengDist             = m_eqTree->getVarPtr( "vSurfaceFireLengDist" );
    vSurfaceFireLengMapDist          = m_eqTree->getVarPtr( "vSurfaceFireLengMapDist" );
    vSurfaceFireLengthToWidth        = m_eqTree->getVarPtr( "vSurfaceFireLengthToWidth" );
    vSurfaceFireLineIntAtBeta        = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtBeta" );
    vSurfaceFireLineIntAtHead        = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtHead" );
    vSurfaceFireLineIntAtPsi         = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtPsi" );
    vSurfaceFireLineIntAtVector      = m_eqTree->getVarPtr( "vSurfaceFireLineIntAtVector" );
    vSurfaceFireMapDistAtBack        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtBack" );
    vSurfaceFireMapDistAtBeta        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtBeta" );
    vSurfaceFireMapDistAtFlank       = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtFlank" );
    vSurfaceFireMapDistAtHead        = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtHead" );
    vSurfaceFireMapDistAtPsi         = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtPsi" );
    vSurfaceFireMapDistAtVector      = m_eqTree->getVarPtr( "vSurfaceFireMapDistAtVector" );
    vSurfaceFireMaxDirDiagram        = m_eqTree->getVarPtr( "vSurfaceFireMaxDirDiagram" );
    vSurfaceFireMaxDirFromNorth      = m_eqTree->getVarPtr( "vSurfaceFireMaxDirFromNorth" );
    vSurfaceFireMaxDirFromUpslope    = m_eqTree->getVarPtr( "vSurfaceFireMaxDirFromUpslope" );
    vSurfaceFireNoWindRate           = m_eqTree->getVarPtr( "vSurfaceFireNoWindRate" );
    vSurfaceFirePerimeter            = m_eqTree->getVarPtr( "vSurfaceFirePerimeter" );
    vSurfaceFirePropagatingFlux      = m_eqTree->getVarPtr( "vSurfaceFirePropagatingFlux" );
    vSurfaceFireReactionInt          = m_eqTree->getVarPtr( "vSurfaceFireReactionInt" );
    vSurfaceFireReactionIntDead      = m_eqTree->getVarPtr( "vSurfaceFireReactionIntDead" );
    vSurfaceFireReactionIntLive      = m_eqTree->getVarPtr( "vSurfaceFireReactionIntLive" );
    vSurfaceFireResidenceTime        = m_eqTree->getVarPtr( "vSurfaceFireResidenceTime" );
    vSurfaceFireScorchHtAtVector     = m_eqTree->getVarPtr( "vSurfaceFireScorchHtAtVector" );
    vSurfaceFireSeverityAspen        = m_eqTree->getVarPtr( "vSurfaceFireSeverityAspen" );
    vSurfaceFireShapeDiagram         = m_eqTree->getVarPtr( "vSurfaceFireShapeDiagram" );
    vSurfaceFireSlopeFactor          = m_eqTree->getVarPtr( "vSurfaceFireSlopeFactor" );
    vSurfaceFireSpreadAtBack         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtBack" );
    vSurfaceFireSpreadAtBeta         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtBeta" );
    vSurfaceFireSpreadAtFlank        = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtFlank" );
    vSurfaceFireSpreadAtHead         = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtHead" );
    vSurfaceFireSpreadAtPsi          = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtPsi" );
    vSurfaceFireSpreadAtVector       = m_eqTree->getVarPtr( "vSurfaceFireSpreadAtVector" );
    vSurfaceFireVectorBeta           = m_eqTree->getVarPtr( "vSurfaceFireVectorBeta" );
    vSurfaceFireVectorDirFromCompass = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromCompass" );
    vSurfaceFireVectorDirFromNorth   = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromNorth" );
    vSurfaceFireVectorDirFromUpslope = m_eqTree->getVarPtr( "vSurfaceFireVectorDirFromUpslope" );
    vSurfaceFireVectorPsi            = m_eqTree->getVarPtr( "vSurfaceFireVectorPsi" );
    vSurfaceFireVectorTheta          = m_eqTree->getVarPtr( "vSurfaceFireVectorTheta" );
    vSurfaceFireWidthDist            = m_eqTree->getVarPtr( "vSurfaceFireWidthDist" );
    vSurfaceFireWidthMapDist         = m_eqTree->getVarPtr( "vSurfaceFireWidthMapDist" );
    vSurfaceFireWindFactor           = m_eqTree->getVarPtr( "vSurfaceFireWindFactor" );
    vSurfaceFireWindFactorB          = m_eqTree->getVarPtr( "vSurfaceFireWindFactorB" );
    vSurfaceFireWindFactorK          = m_eqTree->getVarPtr( "vSurfaceFireWindFactorK" );
    vSurfaceFireWindSpeedFlag        = m_eqTree->getVarPtr( "vSurfaceFireWindSpeedFlag" );
    vSurfaceFireWindSpeedLimit       = m_eqTree->getVarPtr( "vSurfaceFireWindSpeedLimit" );

    vSurfaceFuelAspenCuring          = m_eqTree->getVarPtr( "vSurfaceFuelAspenCuring" );
    vSurfaceFuelAspenLoadDead1       = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadDead1" );
    vSurfaceFuelAspenLoadDead10      = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadDead10" );
    vSurfaceFuelAspenLoadLiveHerb    = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadLiveHerb" );
    vSurfaceFuelAspenLoadLiveWoody   = m_eqTree->getVarPtr( "vSurfaceFuelAspenLoadLiveWoody" );
    vSurfaceFuelAspenSavrDead1       = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrDead1" );
    vSurfaceFuelAspenSavrDead10      = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrDead10" );
    vSurfaceFuelAspenSavrLiveHerb    = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrLiveHerb" );
    vSurfaceFuelAspenSavrLiveWoody   = m_eqTree->getVarPtr( "vSurfaceFuelAspenSavrLiveWoody" );
    vSurfaceFuelAspenType            = m_eqTree->getVarPtr( "vSurfaceFuelAspenType" );

    vSurfaceFuelBedBetaRatio    = m_eqTree->getVarPtr( "vSurfaceFuelBedBetaRatio" );
    vSurfaceFuelBedBulkDensity  = m_eqTree->getVarPtr( "vSurfaceFuelBedBulkDensity" );
    vSurfaceFuelBedCoverage1    = m_eqTree->getVarPtr( "vSurfaceFuelBedCoverage1" );
    vSurfaceFuelBedDeadFraction = m_eqTree->getVarPtr( "vSurfaceFuelBedDeadFraction" );
    vSurfaceFuelBedLiveFraction = m_eqTree->getVarPtr( "vSurfaceFuelBedLiveFraction" );
    vSurfaceFuelBedDepth        = m_eqTree->getVarPtr( "vSurfaceFuelBedDepth" );
    vSurfaceFuelBedHeatSink     = m_eqTree->getVarPtr( "vSurfaceFuelBedHeatSink" );
    vSurfaceFuelBedMextDead     = m_eqTree->getVarPtr( "vSurfaceFuelBedMextDead" );
    vSurfaceFuelBedMextLive     = m_eqTree->getVarPtr( "vSurfaceFuelBedMextLive" );
    vSurfaceFuelBedModel        = m_eqTree->getVarPtr( "vSurfaceFuelBedModel" );
    vSurfaceFuelBedModel1       = m_eqTree->getVarPtr( "vSurfaceFuelBedModel1" );
    vSurfaceFuelBedModel2       = m_eqTree->getVarPtr( "vSurfaceFuelBedModel2" );
    vSurfaceFuelBedModelCode    = m_eqTree->getVarPtr( "vSurfaceFuelBedModelCode" );
    vSurfaceFuelBedModelNumber  = m_eqTree->getVarPtr( "vSurfaceFuelBedModelNumber" );
    vSurfaceFuelBedMoisDead     = m_eqTree->getVarPtr( "vSurfaceFuelBedMoisDead" );
    vSurfaceFuelBedMoisLive     = m_eqTree->getVarPtr( "vSurfaceFuelBedMoisLive" );
    vSurfaceFuelBedPackingRatio = m_eqTree->getVarPtr( "vSurfaceFuelBedPackingRatio" );
    vSurfaceFuelBedSigma        = m_eqTree->getVarPtr( "vSurfaceFuelBedSigma" );

    vSurfaceFuelChaparralAge			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralAge" );
    vSurfaceFuelChaparralDeadFuelFraction=m_eqTree->getVarPtr( "vSurfaceFuelChaparralDeadFuelFraction" );
    vSurfaceFuelChaparralDepth			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralDepth" );
    vSurfaceFuelChaparralLoadDead1		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead1" );
    vSurfaceFuelChaparralLoadDead2		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead2" );
    vSurfaceFuelChaparralLoadDead3		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead3" );
    vSurfaceFuelChaparralLoadDead4		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadDead4" );
    vSurfaceFuelChaparralLoadLiveLeaf	= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLiveLeaf" );
    vSurfaceFuelChaparralLoadLive1		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive1" );
    vSurfaceFuelChaparralLoadLive2		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive2" );
    vSurfaceFuelChaparralLoadLive3		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive3" );
    vSurfaceFuelChaparralLoadLive4		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadLive4" );
    vSurfaceFuelChaparralLoadTotal		= m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotal" );
    vSurfaceFuelChaparralLoadTotalDead  = m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotalDead" );
    vSurfaceFuelChaparralLoadTotalLive  = m_eqTree->getVarPtr( "vSurfaceFuelChaparralLoadTotalLive" );
    vSurfaceFuelChaparralType			= m_eqTree->getVarPtr( "vSurfaceFuelChaparralType" );

    vSurfaceFuelDens[0] = m_eqTree->getVarPtr( "vSurfaceFuelDens0" );
    vSurfaceFuelDens[1] = m_eqTree->getVarPtr( "vSurfaceFuelDens1" );
    vSurfaceFuelDens[2] = m_eqTree->getVarPtr( "vSurfaceFuelDens2" );
    vSurfaceFuelDens[3] = m_eqTree->getVarPtr( "vSurfaceFuelDens3" );
    vSurfaceFuelDens[4] = m_eqTree->getVarPtr( "vSurfaceFuelDens4" );
    vSurfaceFuelDens[5] = m_eqTree->getVarPtr( "vSurfaceFuelDens5" );
    vSurfaceFuelDens[6] = m_eqTree->getVarPtr( "vSurfaceFuelDens6" );
    vSurfaceFuelDens[7] = m_eqTree->getVarPtr( "vSurfaceFuelDens7" );
    vSurfaceFuelDens[8] = m_eqTree->getVarPtr( "vSurfaceFuelDens8" );
    vSurfaceFuelDens[9] = m_eqTree->getVarPtr( "vSurfaceFuelDens9" );

    vSurfaceFuelHeat[0] = m_eqTree->getVarPtr( "vSurfaceFuelHeat0" );
    vSurfaceFuelHeat[1] = m_eqTree->getVarPtr( "vSurfaceFuelHeat1" );
    vSurfaceFuelHeat[2] = m_eqTree->getVarPtr( "vSurfaceFuelHeat2" );
    vSurfaceFuelHeat[3] = m_eqTree->getVarPtr( "vSurfaceFuelHeat3" );
    vSurfaceFuelHeat[4] = m_eqTree->getVarPtr( "vSurfaceFuelHeat4" );
    vSurfaceFuelHeat[5] = m_eqTree->getVarPtr( "vSurfaceFuelHeat5" );
    vSurfaceFuelHeat[6] = m_eqTree->getVarPtr( "vSurfaceFuelHeat6" );
    vSurfaceFuelHeat[7] = m_eqTree->getVarPtr( "vSurfaceFuelHeat7" );
    vSurfaceFuelHeat[8] = m_eqTree->getVarPtr( "vSurfaceFuelHeat8" );
    vSurfaceFuelHeat[9] = m_eqTree->getVarPtr( "vSurfaceFuelHeat9" );
    vSurfaceFuelHeatDead = m_eqTree->getVarPtr( "vSurfaceFuelHeatDead" );
    vSurfaceFuelHeatLive = m_eqTree->getVarPtr( "vSurfaceFuelHeatLive" );

    vSurfaceFuelLife[0] = m_eqTree->getVarPtr( "vSurfaceFuelLife0" );
    vSurfaceFuelLife[1] = m_eqTree->getVarPtr( "vSurfaceFuelLife1" );
    vSurfaceFuelLife[2] = m_eqTree->getVarPtr( "vSurfaceFuelLife2" );
    vSurfaceFuelLife[3] = m_eqTree->getVarPtr( "vSurfaceFuelLife3" );
    vSurfaceFuelLife[4] = m_eqTree->getVarPtr( "vSurfaceFuelLife4" );
    vSurfaceFuelLife[5] = m_eqTree->getVarPtr( "vSurfaceFuelLife5" );
    vSurfaceFuelLife[6] = m_eqTree->getVarPtr( "vSurfaceFuelLife6" );
    vSurfaceFuelLife[7] = m_eqTree->getVarPtr( "vSurfaceFuelLife7" );
    vSurfaceFuelLife[8] = m_eqTree->getVarPtr( "vSurfaceFuelLife8" );
    vSurfaceFuelLife[9] = m_eqTree->getVarPtr( "vSurfaceFuelLife9" );

    vSurfaceFuelLoad[0] = m_eqTree->getVarPtr( "vSurfaceFuelLoad0" );
    vSurfaceFuelLoad[1] = m_eqTree->getVarPtr( "vSurfaceFuelLoad1" );
    vSurfaceFuelLoad[2] = m_eqTree->getVarPtr( "vSurfaceFuelLoad2" );
    vSurfaceFuelLoad[3] = m_eqTree->getVarPtr( "vSurfaceFuelLoad3" );
    vSurfaceFuelLoad[4] = m_eqTree->getVarPtr( "vSurfaceFuelLoad4" );
    vSurfaceFuelLoad[5] = m_eqTree->getVarPtr( "vSurfaceFuelLoad5" );
    vSurfaceFuelLoad[6] = m_eqTree->getVarPtr( "vSurfaceFuelLoad6" );
    vSurfaceFuelLoad[7] = m_eqTree->getVarPtr( "vSurfaceFuelLoad7" );
    vSurfaceFuelLoad[8] = m_eqTree->getVarPtr( "vSurfaceFuelLoad8" );
    vSurfaceFuelLoad[9] = m_eqTree->getVarPtr( "vSurfaceFuelLoad9" );

    vSurfaceFuelLoadDead       = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead" );
    vSurfaceFuelLoadDead1      = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead1" );
    vSurfaceFuelLoadDead10     = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead10" );
    vSurfaceFuelLoadDead100    = m_eqTree->getVarPtr( "vSurfaceFuelLoadDead100" );
    vSurfaceFuelLoadDeadHerb   = m_eqTree->getVarPtr( "vSurfaceFuelLoadDeadHerb" );
    vSurfaceFuelLoadLive       = m_eqTree->getVarPtr( "vSurfaceFuelLoadLive" );
    vSurfaceFuelLoadLiveHerb   = m_eqTree->getVarPtr( "vSurfaceFuelLoadLiveHerb" );
    vSurfaceFuelLoadLiveWood   = m_eqTree->getVarPtr( "vSurfaceFuelLoadLiveWood" );
    vSurfaceFuelLoadTransferEq = m_eqTree->getVarPtr( "vSurfaceFuelLoadTransferEq" );
    vSurfaceFuelLoadTransferFraction = m_eqTree->getVarPtr( "vSurfaceFuelLoadTransferFraction" );
    vSurfaceFuelLoadUndeadHerb = m_eqTree->getVarPtr( "vSurfaceFuelLoadUndeadHerb" );

    vSurfaceFuelMois[0] = m_eqTree->getVarPtr( "vSurfaceFuelMois0" );
    vSurfaceFuelMois[1] = m_eqTree->getVarPtr( "vSurfaceFuelMois1" );
    vSurfaceFuelMois[2] = m_eqTree->getVarPtr( "vSurfaceFuelMois2" );
    vSurfaceFuelMois[3] = m_eqTree->getVarPtr( "vSurfaceFuelMois3" );
    vSurfaceFuelMois[4] = m_eqTree->getVarPtr( "vSurfaceFuelMois4" );
    vSurfaceFuelMois[5] = m_eqTree->getVarPtr( "vSurfaceFuelMois5" );
    vSurfaceFuelMois[6] = m_eqTree->getVarPtr( "vSurfaceFuelMois6" );
    vSurfaceFuelMois[7] = m_eqTree->getVarPtr( "vSurfaceFuelMois7" );
    vSurfaceFuelMois[8] = m_eqTree->getVarPtr( "vSurfaceFuelMois8" );
    vSurfaceFuelMois[9] = m_eqTree->getVarPtr( "vSurfaceFuelMois9" );

    vSurfaceFuelMoisDead1    = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead1" );
    vSurfaceFuelMoisDead10   = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead10" );
    vSurfaceFuelMoisDead100  = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead100" );
    vSurfaceFuelMoisDead1000 = m_eqTree->getVarPtr( "vSurfaceFuelMoisDead1000" );
    vSurfaceFuelMoisLifeDead = m_eqTree->getVarPtr( "vSurfaceFuelMoisLifeDead" );
    vSurfaceFuelMoisLifeLive = m_eqTree->getVarPtr( "vSurfaceFuelMoisLifeLive" );
    vSurfaceFuelMoisLiveHerb = m_eqTree->getVarPtr( "vSurfaceFuelMoisLiveHerb" );
    vSurfaceFuelMoisLiveWood = m_eqTree->getVarPtr( "vSurfaceFuelMoisLiveWood" );
    vSurfaceFuelMoisScenario = m_eqTree->getVarPtr( "vSurfaceFuelMoisScenario" );

    vSurfaceFuelPalmettoAge             = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoAge" );
    vSurfaceFuelPalmettoCover           = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoCover" );
    vSurfaceFuelPalmettoHeight          = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoHeight" );
    vSurfaceFuelPalmettoLoadDead1       = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDead1" );
    vSurfaceFuelPalmettoLoadDead10      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDead10" );
    vSurfaceFuelPalmettoLoadDeadFoliage = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadDeadFoliage" );
    vSurfaceFuelPalmettoLoadLitter      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLitter" );
    vSurfaceFuelPalmettoLoadLive1       = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLive1" );
    vSurfaceFuelPalmettoLoadLive10      = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLive10" );
    vSurfaceFuelPalmettoLoadLiveFoliage = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoLoadLiveFoliage" );
    vSurfaceFuelPalmettoOverstoryBasalArea = m_eqTree->getVarPtr( "vSurfaceFuelPalmettoOverstoryBasalArea" );

    vSurfaceFuelSavr[0] = m_eqTree->getVarPtr( "vSurfaceFuelSavr0" );
    vSurfaceFuelSavr[1] = m_eqTree->getVarPtr( "vSurfaceFuelSavr1" );
    vSurfaceFuelSavr[2] = m_eqTree->getVarPtr( "vSurfaceFuelSavr2" );
    vSurfaceFuelSavr[3] = m_eqTree->getVarPtr( "vSurfaceFuelSavr3" );
    vSurfaceFuelSavr[4] = m_eqTree->getVarPtr( "vSurfaceFuelSavr4" );
    vSurfaceFuelSavr[5] = m_eqTree->getVarPtr( "vSurfaceFuelSavr5" );
    vSurfaceFuelSavr[6] = m_eqTree->getVarPtr( "vSurfaceFuelSavr6" );
    vSurfaceFuelSavr[7] = m_eqTree->getVarPtr( "vSurfaceFuelSavr7" );
    vSurfaceFuelSavr[8] = m_eqTree->getVarPtr( "vSurfaceFuelSavr8" );
    vSurfaceFuelSavr[9] = m_eqTree->getVarPtr( "vSurfaceFuelSavr9" );

    vSurfaceFuelSavrDead1    = m_eqTree->getVarPtr( "vSurfaceFuelSavrDead1" );
    vSurfaceFuelSavrLiveHerb = m_eqTree->getVarPtr( "vSurfaceFuelSavrLiveHerb" );
    vSurfaceFuelSavrLiveWood = m_eqTree->getVarPtr( "vSurfaceFuelSavrLiveWood" );

    vSurfaceFuelSeff[0] = m_eqTree->getVarPtr( "vSurfaceFuelSeff0" );
    vSurfaceFuelSeff[1] = m_eqTree->getVarPtr( "vSurfaceFuelSeff1" );
    vSurfaceFuelSeff[2] = m_eqTree->getVarPtr( "vSurfaceFuelSeff2" );
    vSurfaceFuelSeff[3] = m_eqTree->getVarPtr( "vSurfaceFuelSeff3" );
    vSurfaceFuelSeff[4] = m_eqTree->getVarPtr( "vSurfaceFuelSeff4" );
    vSurfaceFuelSeff[5] = m_eqTree->getVarPtr( "vSurfaceFuelSeff5" );
    vSurfaceFuelSeff[6] = m_eqTree->getVarPtr( "vSurfaceFuelSeff6" );
    vSurfaceFuelSeff[7] = m_eqTree->getVarPtr( "vSurfaceFuelSeff7" );
    vSurfaceFuelSeff[8] = m_eqTree->getVarPtr( "vSurfaceFuelSeff8" );
    vSurfaceFuelSeff[9] = m_eqTree->getVarPtr( "vSurfaceFuelSeff9" );

    vSurfaceFuelStot[0] = m_eqTree->getVarPtr( "vSurfaceFuelStot0" );
    vSurfaceFuelStot[1] = m_eqTree->getVarPtr( "vSurfaceFuelStot1" );
    vSurfaceFuelStot[2] = m_eqTree->getVarPtr( "vSurfaceFuelStot2" );
    vSurfaceFuelStot[3] = m_eqTree->getVarPtr( "vSurfaceFuelStot3" );
    vSurfaceFuelStot[4] = m_eqTree->getVarPtr( "vSurfaceFuelStot4" );
    vSurfaceFuelStot[5] = m_eqTree->getVarPtr( "vSurfaceFuelStot5" );
    vSurfaceFuelStot[6] = m_eqTree->getVarPtr( "vSurfaceFuelStot6" );
    vSurfaceFuelStot[7] = m_eqTree->getVarPtr( "vSurfaceFuelStot7" );
    vSurfaceFuelStot[8] = m_eqTree->getVarPtr( "vSurfaceFuelStot8" );
    vSurfaceFuelStot[9] = m_eqTree->getVarPtr( "vSurfaceFuelStot9" );

    vSurfaceFuelTemp    = m_eqTree->getVarPtr( "vSurfaceFuelTemp" );

    vTimeIntegerDate = m_eqTree->getVarPtr( "vTimeIntegerDate" );
    vTimeJulianDate  = m_eqTree->getVarPtr( "vTimeJulianDate" );

    vTreeBarkThickness            = m_eqTree->getVarPtr( "vTreeBarkThickness" );
    vTreeCanopyBulkDens           = m_eqTree->getVarPtr( "vTreeCanopyBulkDens" );
    vTreeCanopyCrownFraction      = m_eqTree->getVarPtr( "vTreeCanopyCrownFraction" );
    vTreeCanopyCover              = m_eqTree->getVarPtr( "vTreeCanopyCover" );
    vTreeCanopyCoverDownwind      = m_eqTree->getVarPtr( "vTreeCanopyCoverDownwind" );
    vTreeCount                    = m_eqTree->getVarPtr( "vTreeCount" );
    vTreeCoverHt                  = m_eqTree->getVarPtr( "vTreeCoverHt" );
    vTreeCoverHtDownwind          = m_eqTree->getVarPtr( "vTreeCoverHtDownwind" );
    vTreeCrownBaseHt              = m_eqTree->getVarPtr( "vTreeCrownBaseHt" );
    vTreeCrownLengFractionScorchedAtVector = m_eqTree->getVarPtr( "vTreeCrownLengFractionScorchedAtVector" );
    vTreeCrownLengScorchedAtVector= m_eqTree->getVarPtr( "vTreeCrownLengScorchedAtVector" );
    vTreeCrownRatio               = m_eqTree->getVarPtr( "vTreeCrownRatio" );
    vTreeCrownVolScorchedAtVector = m_eqTree->getVarPtr( "vTreeCrownVolScorchedAtVector" );
    vTreeDbh                      = m_eqTree->getVarPtr( "vTreeDbh" );
    vTreeFoliarMois               = m_eqTree->getVarPtr( "vTreeFoliarMois" );
    vTreeHt                       = m_eqTree->getVarPtr( "vTreeHt" );
    vTreeMortalityCountAtVector   = m_eqTree->getVarPtr( "vTreeMortalityCountAtVector" );
    vTreeMortalityRateAspenAtVector = m_eqTree->getVarPtr( "vTreeMortalityRateAspenAtVector" );
    vTreeMortalityRateAtVector    = m_eqTree->getVarPtr( "vTreeMortalityRateAtVector" );
    vTreeSpecies                  = m_eqTree->getVarPtr( "vTreeSpecies" );
    vTreeSpeciesMortality         = m_eqTree->getVarPtr( "vTreeSpeciesMortality" );
    vTreeSpeciesSpot              = m_eqTree->getVarPtr( "vTreeSpeciesSpot" );

    vWindAdjFactor       = m_eqTree->getVarPtr( "vWindAdjFactor" );
    vWindAdjMethod       = m_eqTree->getVarPtr( "vWindAdjMethod" );
    vWindDirFromCompass  = m_eqTree->getVarPtr( "vWindDirFromCompass" );
    vWindDirFromNorth    = m_eqTree->getVarPtr( "vWindDirFromNorth" );
    vWindDirFromUpslope  = m_eqTree->getVarPtr( "vWindDirFromUpslope" );
    vWindSpeedAt10M      = m_eqTree->getVarPtr( "vWindSpeedAt10M" );
    vWindSpeedAt20Ft     = m_eqTree->getVarPtr( "vWindSpeedAt20Ft" );
    vWindSpeedAtMidflame = m_eqTree->getVarPtr( "vWindSpeedAtMidflame" );

    vWthrAirTemp             = m_eqTree->getVarPtr( "vWthrAirTemp" );
    vWthrCumulusBaseHt       = m_eqTree->getVarPtr( "vWthrCumulusBaseHt" );
    vWthrDewPointTemp        = m_eqTree->getVarPtr( "vWthrDewPointTemp" );
    vWthrHeatIndex           = m_eqTree->getVarPtr( "vWthrHeatIndex" );
    vWthrLightningStrikeType = m_eqTree->getVarPtr( "vWthrLightningStrikeType" );
    vWthrRelativeHumidity    = m_eqTree->getVarPtr( "vWthrRelativeHumidity" );
    vWthrSummerSimmerIndex   = m_eqTree->getVarPtr( "vWthrSummerSimmerIndex" );
    vWthrWetBulbTemp         = m_eqTree->getVarPtr( "vWthrWetBulbTemp" );
    vWthrWindChillTemp       = m_eqTree->getVarPtr( "vWthrWindChillTemp" );

    fContainFF             = m_eqTree->setEqFunAddress( "fContainFF", &EqCalc::ContainFF );
    fContainFFReportRatio  = m_eqTree->setEqFunAddress( "fContainFFReportRatio", &EqCalc::ContainFFReportRatio );
    fContainFFReportSize   = m_eqTree->setEqFunAddress( "fContainFFReportSize", &EqCalc::ContainFFReportSize );
    fContainFFReportSpread = m_eqTree->setEqFunAddress( "fContainFFReportSpread", &EqCalc::ContainFFReportSpread );
    fContainFFSingle       = m_eqTree->setEqFunAddress( "fContainFFSingle", &EqCalc::ContainFFSingle );

	fCrownFireActiveCritSurfSpreadRate = m_eqTree->setEqFunAddress( "fCrownFireActiveCritSurfSpreadRate", &EqCalc::V6CrownFireActiveCriticalSurfaceSpreadRate );
	fCrownFireActiveCrown         = m_eqTree->setEqFunAddress( "fCrownFireActiveCrown", &EqCalc::V6CrownFireActiveCrown );
    fCrownFireActiveRatio         = m_eqTree->setEqFunAddress( "fCrownFireActiveRatio", &EqCalc::V6CrownFireActiveRatio );
    fCrownFireActiveFireArea      = m_eqTree->setEqFunAddress( "fCrownFireActiveFireArea", &EqCalc::V6CrownFireActiveFireArea );
    fCrownFireActiveFireLineInt   = m_eqTree->setEqFunAddress( "fCrownFireActiveFireLineInt", &EqCalc::V6CrownFireActiveFireLineIntensity );
    fCrownFireActiveFireLineIntFromFlameLeng = m_eqTree->setEqFunAddress( "fCrownFireActiveFireLineIntFromFlameLeng", &EqCalc::V6CrownFireActiveFireLineIntensityFromFlameLength );
    fCrownFireActiveFirePerimeter = m_eqTree->setEqFunAddress( "fCrownFireActiveFirePerimeter", &EqCalc::V6CrownFireActiveFirePerimeter );
    fCrownFireActiveFireWidth     = m_eqTree->setEqFunAddress( "fCrownFireActiveFireWidth", &EqCalc::V6CrownFireActiveFireWidth );
    fCrownFireActiveFlameLeng     = m_eqTree->setEqFunAddress( "fCrownFireActiveFlameLeng", &EqCalc::V6CrownFireActiveFlameLength );
    fCrownFireActiveHeatPerUnitArea = m_eqTree->setEqFunAddress( "fCrownFireActiveHeatPerUnitArea", &EqCalc::V6CrownFireActiveHeatPerUnitArea );
    fCrownFireActiveSpreadDist    = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadDist", &EqCalc::V6CrownFireActiveSpreadDist );
    fCrownFireActiveSpreadMapDist = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadMapDist", &EqCalc::V6CrownFireActiveSpreadMapDist );
    fCrownFireActiveSpreadRate    = m_eqTree->setEqFunAddress( "fCrownFireActiveSpreadRate", &EqCalc::V6CrownFireActiveSpreadRate );
	fCrownFireCanopyFractionBurned = m_eqTree->setEqFunAddress( "fCrownFireCanopyFractionBurned", &EqCalc::V6CrownFireCanopyFractionBurned );
	fCrownFireCritCrownSpreadRate = m_eqTree->setEqFunAddress( "fCrownFireCritCrownSpreadRate", &EqCalc::V6CrownFireCritCrownSpreadRate );
    fCrownFireCritSurfFireInt     = m_eqTree->setEqFunAddress( "fCrownFireCritSurfFireInt", &EqCalc::V6CrownFireCritSurfFireInt );
    fCrownFireCritSurfFlameLeng   = m_eqTree->setEqFunAddress( "fCrownFireCritSurfFlameLeng", &EqCalc::V6CrownFireCritSurfFlameLeng );
    fCrownFireCritSurfSpreadRate  = m_eqTree->setEqFunAddress( "fCrownFireCritSurfSpreadRate", &EqCalc::V6CrownFireCritSurfSpreadRate );
    fCrownFireFuelLoad            = m_eqTree->setEqFunAddress( "fCrownFireFuelLoad", &EqCalc::V6CrownFireFuelLoad );
    fCrownFireHeatPerUnitAreaCanopy = m_eqTree->setEqFunAddress( "fCrownFireHeatPerUnitAreaCanopy", &EqCalc::V6CrownFireHeatPerUnitAreaCanopy );
    fCrownFireLengthToWidth       = m_eqTree->setEqFunAddress( "fCrownFireLengthToWidth", &EqCalc::V6CrownFireLengthToWidth );
    fCrownFirePassiveFireArea     = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireArea", &EqCalc::V6CrownFirePassiveFireArea );
    fCrownFirePassiveFireLineInt  = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireLineInt", &EqCalc::V6CrownFirePassiveFireLineIntensity );
    fCrownFirePassiveFirePerimeter= m_eqTree->setEqFunAddress( "fCrownFirePassiveFirePerimeter", &EqCalc::V6CrownFirePassiveFirePerimeter );
    fCrownFirePassiveFireWidth    = m_eqTree->setEqFunAddress( "fCrownFirePassiveFireWidth", &EqCalc::V6CrownFirePassiveFireWidth );
    fCrownFirePassiveFlameLeng    = m_eqTree->setEqFunAddress( "fCrownFirePassiveFlameLeng", &EqCalc::V6CrownFirePassiveFlameLength );
    fCrownFirePassiveHeatPerUnitArea = m_eqTree->setEqFunAddress( "fCrownFirePassiveHeatPerUnitArea", &EqCalc::V6CrownFirePassiveHeatPerUnitArea );
    fCrownFirePassiveSpreadDist   = m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadDist", &EqCalc::V6CrownFirePassiveSpreadDist );
    fCrownFirePassiveSpreadMapDist= m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadMapDist", &EqCalc::V6CrownFirePassiveSpreadMapDist );
    fCrownFirePassiveSpreadRate   = m_eqTree->setEqFunAddress( "fCrownFirePassiveSpreadRate", &EqCalc::V6CrownFirePassiveSpreadRate );
	fCrownFirePowerOfFire         = m_eqTree->setEqFunAddress( "fCrownFirePowerOfFire", &EqCalc::V6CrownFirePowerOfFire );
    fCrownFirePowerOfWind         = m_eqTree->setEqFunAddress( "fCrownFirePowerOfWind", &EqCalc::V6CrownFirePowerOfWind );
    fCrownFirePowerRatio          = m_eqTree->setEqFunAddress( "fCrownFirePowerRatio", &EqCalc::V6CrownFirePowerRatio );
    fCrownFireTransRatioFromFireIntAtVector = m_eqTree->setEqFunAddress( "fCrownFireTransRatioFromFireIntAtVector", &EqCalc::V6CrownFireTransRatioFromFireIntAtVector );
    fCrownFireTransRatioFromFlameLengAtVector = m_eqTree->setEqFunAddress( "fCrownFireTransRatioFromFlameLengAtVector", &EqCalc::V6CrownFireTransRatioFromFlameLengAtVector );
    fCrownFireTransToCrown        = m_eqTree->setEqFunAddress( "fCrownFireTransToCrown", &EqCalc::V6CrownFireTransToCrown );
    fCrownFireType                = m_eqTree->setEqFunAddress( "fCrownFireType", &EqCalc::V6CrownFireType );
    fCrownFireWindDriven          = m_eqTree->setEqFunAddress( "fCrownFireWindDriven", &EqCalc::V6CrownFireWindDriven );

    fIgnitionFirebrandFuelMoisFromDead1Hr   = m_eqTree->setEqFunAddress( "fIgnitionFirebrandFuelMoisFromDead1Hr", &EqCalc::IgnitionFirebrandFuelMoisFromDead1Hr );
    fIgnitionFirebrandProb                  = m_eqTree->setEqFunAddress( "fIgnitionFirebrandProb", &EqCalc::IgnitionFirebrandProb );
    fIgnitionLightningFuelMoisFromDead100Hr = m_eqTree->setEqFunAddress( "fIgnitionLightningFuelMoisFromDead100Hr", &EqCalc::IgnitionLightningFuelMoisFromDead100Hr );
    fIgnitionLightningProb                  = m_eqTree->setEqFunAddress( "fIgnitionLightningProb", &EqCalc::IgnitionLightningProb );

    fMapScale = m_eqTree->setEqFunAddress( "fMapScale", &EqCalc::MapScale );
    fMapSlope = m_eqTree->setEqFunAddress( "fMapSlope",  &EqCalc::MapSlope );

    fSafetyZoneRadius  = m_eqTree->setEqFunAddress( "fSafetyZoneRadius",  &EqCalc::SafetyZoneRadius );
    fSafetyZoneSepDist = m_eqTree->setEqFunAddress( "fSafetyZoneSepDist",  &EqCalc::SafetyZoneSepDist );

    fSiteAspectDirFromNorth  = m_eqTree->setEqFunAddress( "fSiteAspectDirFromNorth",  &EqCalc::SiteAspectDirFromNorth );
    fSiteRidgeToValleyDist   = m_eqTree->setEqFunAddress( "fSiteRidgeToValleyDist",  &EqCalc::SiteRidgeToValleyDist );
    fSiteSlopeFraction       = m_eqTree->setEqFunAddress( "fSiteSlopeFraction", &EqCalc::SiteSlopeFraction );
    fSiteUpslopeDirFromNorth = m_eqTree->setEqFunAddress( "fSiteUpslopeDirFromNorth", &EqCalc::SiteUpslopeDirFromNorth );

    fSpotDistActiveCrown      = m_eqTree->setEqFunAddress( "fSpotDistActiveCrown", &EqCalc::SpotDistActiveCrown );
    fSpotDistBurningPile      = m_eqTree->setEqFunAddress( "fSpotDistBurningPile", &EqCalc::SpotDistBurningPile );
    fSpotDistSurfaceFire      = m_eqTree->setEqFunAddress( "fSpotDistSurfaceFire", &EqCalc::SpotDistSurfaceFire );
    fSpotDistTorchingTrees    = m_eqTree->setEqFunAddress( "fSpotDistTorchingTrees", &EqCalc::SpotDistTorchingTrees );
    fSpotMapDistActiveCrown   = m_eqTree->setEqFunAddress( "fSpotMapDistActiveCrown", &EqCalc::SpotMapDistActiveCrown );
    fSpotMapDistBurningPile   = m_eqTree->setEqFunAddress( "fSpotMapDistBurningPile", &EqCalc::SpotMapDistBurningPile );
    fSpotMapDistSurfaceFire   = m_eqTree->setEqFunAddress( "fSpotMapDistSurfaceFire", &EqCalc::SpotMapDistSurfaceFire );
    fSpotMapDistTorchingTrees = m_eqTree->setEqFunAddress( "fSpotMapDistTorchingTrees", &EqCalc::SpotMapDistTorchingTrees );

    fSurfaceFireArea              = m_eqTree->setEqFunAddress( "fSurfaceFireArea", &EqCalc::FireArea );
    fSurfaceFireCharacteristicsDiagram = m_eqTree->setEqFunAddress( "fSurfaceFireCharacteristicsDiagram", &EqCalc::FireCharacteristicsDiagram );
    fSurfaceFireDistAtBack        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtBack", &EqCalc::FireDistAtBack );
    fSurfaceFireDistAtBeta        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtBeta", &EqCalc::FireDistAtBeta );
    fSurfaceFireDistAtFlank       = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtFlank", &EqCalc::FireDistAtFlank );
    fSurfaceFireDistAtHead        = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtHead", &EqCalc::FireDistAtHead );
    fSurfaceFireDistAtPsi         = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtPsi", &EqCalc::FireDistAtPsi );
    fSurfaceFireDistAtVector      = m_eqTree->setEqFunAddress( "fSurfaceFireDistAtVector", &EqCalc::FireDistAtVector );
    fSurfaceFireEccentricity      = m_eqTree->setEqFunAddress( "fSurfaceFireEccentricity", &EqCalc::FireEccentricity );
    fSurfaceFireEllipseF          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseF", &EqCalc::FireEllipseF );
    fSurfaceFireEllipseG          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseG", &EqCalc::FireEllipseG );
    fSurfaceFireEllipseH          = m_eqTree->setEqFunAddress( "fSurfaceFireEllipseH", &EqCalc::FireEllipseH );
    fSurfaceFireEffWindAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireEffWindAtVector", &EqCalc::FireEffWindAtVector );
    fSurfaceFireFlameHtAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameHtAtVector", &EqCalc::FireFlameHtAtVector );
    fSurfaceFireFlameLengAtBeta   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtBeta", &EqCalc::FireFlameLengAtBeta );
    fSurfaceFireFlameLengAtHead   = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtHead", &EqCalc::FireFlameLengAtHead );
    fSurfaceFireFlameLengAtPsi    = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtPsi", &EqCalc::FireFlameLengAtPsi );
    fSurfaceFireFlameLengAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireFlameLengAtVector", &EqCalc::FireFlameLengAtVector );
    fSurfaceFireHeatPerUnitArea   = m_eqTree->setEqFunAddress( "fSurfaceFireHeatPerUnitArea", &EqCalc::FireHeatPerUnitArea );
    fSurfaceFireHeatSource        = m_eqTree->setEqFunAddress( "fSurfaceFireHeatSource", &EqCalc::FireHeatSource );
    fSurfaceFireLengDist          = m_eqTree->setEqFunAddress( "fSurfaceFireLengDist", &EqCalc::FireLengDist );
    fSurfaceFireLengMapDist       = m_eqTree->setEqFunAddress( "fSurfaceFireLengMapDist", &EqCalc::FireLengMapDist );
    fSurfaceFireLengthToWidth     = m_eqTree->setEqFunAddress( "fSurfaceFireLengthToWidth", &EqCalc::FireLengthToWidth );
    fSurfaceFireLineIntAtBeta     = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtBeta", &EqCalc::FireLineIntAtBeta );
    fSurfaceFireLineIntAtHead     = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtHead", &EqCalc::FireLineIntAtHead );
    fSurfaceFireLineIntAtPsi      = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtPsi", &EqCalc::FireLineIntAtPsi );
    fSurfaceFireLineIntAtVectorFromBeta = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtVectorFromBeta", &EqCalc::FireLineIntAtVectorFromBeta );
    fSurfaceFireLineIntAtVectorFromPsi  = m_eqTree->setEqFunAddress( "fSurfaceFireLineIntAtVectorFromPsi", &EqCalc::FireLineIntAtVectorFromPsi );
    fSurfaceFireMapDistAtBack     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtBack", &EqCalc::FireMapDistAtBack );
    fSurfaceFireMapDistAtBeta     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtBeta", &EqCalc::FireMapDistAtBeta );
    fSurfaceFireMapDistAtFlank    = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtFlank", &EqCalc::FireMapDistAtFlank );
    fSurfaceFireMapDistAtHead     = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtHead", &EqCalc::FireMapDistAtHead );
    fSurfaceFireMapDistAtPsi      = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtPsi", &EqCalc::FireMapDistAtPsi );
    fSurfaceFireMapDistAtVector   = m_eqTree->setEqFunAddress( "fSurfaceFireMapDistAtVector", &EqCalc::FireMapDistAtVector );
    fSurfaceFireMaxDirFromNorth   = m_eqTree->setEqFunAddress( "fSurfaceFireMaxDirFromNorth", &EqCalc::FireMaxDirFromNorth );
    fSurfaceFireMaxDirDiagram     = m_eqTree->setEqFunAddress( "fSurfaceFireMaxDirDiagram", &EqCalc::FireMaxDirDiagram );
    fSurfaceFireNoWindRate        = m_eqTree->setEqFunAddress( "fSurfaceFireNoWindRate", &EqCalc::FireNoWindRate );
    fSurfaceFirePerimeter         = m_eqTree->setEqFunAddress( "fSurfaceFirePerimeter", &EqCalc::FirePerimeter );
    fSurfaceFirePropagatingFlux   = m_eqTree->setEqFunAddress( "fSurfaceFirePropagatingFlux", &EqCalc::FirePropagatingFlux );
    fSurfaceFireReactionInt       = m_eqTree->setEqFunAddress( "fSurfaceFireReactionInt", &EqCalc::FireReactionInt );
    fSurfaceFireResidenceTime     = m_eqTree->setEqFunAddress( "fSurfaceFireResidenceTime", &EqCalc::FireResidenceTime );
    fSurfaceFireScorchHtFromFliAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireScorchHtFromFliAtVector", &EqCalc::FireScorchHtFromFliAtVector );
    fSurfaceFireScorchHtFromFlameLengAtVector = m_eqTree->setEqFunAddress( "fSurfaceFireScorchHtFromFlameLengAtVector", &EqCalc::FireScorchHtFromFlameLengAtVector );
    fSurfaceFireShapeDiagram      = m_eqTree->setEqFunAddress( "fSurfaceFireShapeDiagram", &EqCalc::FireShapeDiagram );
    fSurfaceFireSpreadAtBack      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtBack", &EqCalc::FireSpreadAtBack );
    fSurfaceFireSpreadAtBeta      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtBeta", &EqCalc::FireSpreadAtBeta );
    fSurfaceFireSpreadAtFlank     = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtFlank", &EqCalc::FireSpreadAtFlank );
    fSurfaceFireSpreadAtHead      = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtHead", &EqCalc::FireSpreadAtHead );
    fSurfaceFireSpreadAtPsi       = m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtPsi", &EqCalc::FireSpreadAtPsi );
    fSurfaceFireSpreadAtVectorFromBeta	= m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtVectorFromBeta", &EqCalc::FireSpreadAtVectorFromBeta );
    fSurfaceFireSpreadAtVectorFromPsi	= m_eqTree->setEqFunAddress( "fSurfaceFireSpreadAtVectorFromPsi", &EqCalc::FireSpreadAtVectorFromPsi );
    fSurfaceFireVectorBetaFromTheta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorBetaFromTheta", &EqCalc::FireVectorBetaFromTheta );
    fSurfaceFireVectorBetaFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorBetaFromUpslope", &EqCalc::FireVectorBetaFromUpslope );
    fSurfaceFireVectorDirFromNorth		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorDirFromNorth", &EqCalc::FireVectorDirFromNorth );
    fSurfaceFireVectorDirFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorDirFromUpslope", &EqCalc::FireVectorDirFromUpslope );
    fSurfaceFireVectorPsiFromTheta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorPsiFromTheta", &EqCalc::FireVectorPsiFromTheta );
    fSurfaceFireVectorPsiFromUpslope	= m_eqTree->setEqFunAddress( "fSurfaceFireVectorPsiFromUpslope", &EqCalc::FireVectorPsiFromUpslope );
    fSurfaceFireVectorThetaFromBeta		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorThetaFromBeta", &EqCalc::FireVectorThetaFromBeta );
    fSurfaceFireVectorThetaFromPsi		= m_eqTree->setEqFunAddress( "fSurfaceFireVectorThetaFromPsi", &EqCalc::FireVectorThetaFromPsi );
    fSurfaceFireWidthDist		= m_eqTree->setEqFunAddress( "fSurfaceFireWidthDist", &EqCalc::FireWidthDist );
    fSurfaceFireWidthMapDist	= m_eqTree->setEqFunAddress( "fSurfaceFireWidthMapDist", &EqCalc::FireWidthMapDist );

    fSurfaceFuelAspenModel = m_eqTree->setEqFunAddress( "fSurfaceFuelAspenModel", &EqCalc::FuelAspenModel );
    fSurfaceFuelAspenParms = m_eqTree->setEqFunAddress( "fSurfaceFuelAspenParms", &EqCalc::FuelAspenParms );

	fSurfaceFuelBedHeatSink		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedHeatSink", &EqCalc::FuelBedHeatSink );
    fSurfaceFuelBedIntermediates = m_eqTree->setEqFunAddress( "fSurfaceFuelBedIntermediates", &EqCalc::FuelBedIntermediates );
    fSurfaceFuelBedModel		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedModel", &EqCalc::FuelBedModel );
    fSurfaceFuelBedParms		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedParms", &EqCalc::FuelBedParms );
    fSurfaceFuelBedWeighted		 = m_eqTree->setEqFunAddress( "fSurfaceFuelBedWeighted", &EqCalc::FuelBedWeighted );

	fSurfaceFuelChaparralAgeFromDepthType   = m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralAgeFromDepthType", &EqCalc::FuelChaparralAgeFromDepthType );
    fSurfaceFuelChaparralLoadTotalFromAgeType = m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralLoadTotalFromAgeType", &EqCalc::FuelChaparralLoadTotalFromAgeType );
	fSurfaceFuelChaparralModel				= m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralModel", &EqCalc::FuelChaparralModel );    
    fSurfaceFuelChaparralParms				= m_eqTree->setEqFunAddress( "fSurfaceFuelChaparralParms", &EqCalc::FuelChaparralParms );

	fSurfaceFuelLoadTransferFraction= m_eqTree->setEqFunAddress( "fSurfaceFuelLoadTransferFraction", &EqCalc::FuelLoadTransferFraction );
    fSurfaceFuelMoisDeadHerbWood	= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisDeadHerbWood", &EqCalc::FuelMoisDeadHerbWood );
    fSurfaceFuelMoisLifeClass		= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisLifeClass", &EqCalc::FuelMoisLifeClass );
    fSurfaceFuelMoisScenarioModel	= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisScenarioModel", &EqCalc::FuelMoisScenarioModel );
    fSurfaceFuelMoisTimeLag			= m_eqTree->setEqFunAddress( "fSurfaceFuelMoisTimeLag", &EqCalc::FuelMoisTimeLag );
    fSurfaceFuelPalmettoModel		= m_eqTree->setEqFunAddress( "fSurfaceFuelPalmettoModel", &EqCalc::FuelPalmettoModel );
    fSurfaceFuelPalmettoParms		= m_eqTree->setEqFunAddress( "fSurfaceFuelPalmettoParms", &EqCalc::FuelPalmettoParms );
    fSurfaceFuelTemp				= m_eqTree->setEqFunAddress( "fSurfaceFuelTemp", &EqCalc::FuelTemp );

    fTimeJulianDate = m_eqTree->setEqFunAddress( "fTimeJulianDate", &EqCalc::TimeJulianDate );

    fTreeBarkThicknessFofem = m_eqTree->setEqFunAddress( "fTreeBarkThicknessFofem", &EqCalc::TreeBarkThicknessFofem );
    fTreeBarkThicknessFofem6 = m_eqTree->setEqFunAddress( "fTreeBarkThicknessFofem6", &EqCalc::TreeBarkThicknessFofem6 );
    fTreeCrownBaseHt = m_eqTree->setEqFunAddress( "fTreeCrownBaseHt", &EqCalc::TreeCrownBaseHt );
    fTreeCrownRatio  = m_eqTree->setEqFunAddress( "fTreeCrownRatio", &EqCalc::TreeCrownRatio );
    fTreeCrownVolScorchedAtVector = m_eqTree->setEqFunAddress( "fTreeCrownVolScorchedAtVector", &EqCalc::TreeCrownVolScorchedAtVector );
    fTreeMortalityCountAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityCountAtVector", &EqCalc::TreeMortalityCountAtVector );
    fTreeMortalityRateAspenAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateAspenAtVector", &EqCalc::TreeMortalityRateAspenAtVector );
    fTreeMortalityRateFofemAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofemAtVector", &EqCalc::TreeMortalityRateFofemAtVector );
    fTreeMortalityRateFofem2AtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofem2AtVector", &EqCalc::TreeMortalityRateFofem2AtVector );
    fTreeMortalityRateFofem6AtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofem6AtVector", &EqCalc::TreeMortalityRateFofem6AtVector );
    fTreeMortalityRateFofemHoodAtVector = m_eqTree->setEqFunAddress( "fTreeMortalityRateFofemHoodAtVector", &EqCalc::TreeMortalityRateFofemHoodAtVector );

    fWindAdjFactor = m_eqTree->setEqFunAddress( "fWindAdjFactor", &EqCalc::WindAdjFactor );
    fWindSpeedAt20Ft = m_eqTree->setEqFunAddress( "fWindSpeedAt20Ft", &EqCalc::WindSpeedAt20Ft );
    fWindSpeedAtMidflame = m_eqTree->setEqFunAddress( "fWindSpeedAtMidflame", &EqCalc::WindSpeedAtMidflame );
    fWindDirFromNorth = m_eqTree->setEqFunAddress( "fWindDirFromNorth", &EqCalc::WindDirFromNorth );
    fWindDirFromUpslope = m_eqTree->setEqFunAddress( "fWindDirFromUpslope", &EqCalc::WindDirFromUpslope );

    fWthrCumulusBaseHt = m_eqTree->setEqFunAddress( "fWthrCumulusBaseHt", &EqCalc::WthrCumulusBaseHt );
    fWthrDewPointTemp = m_eqTree->setEqFunAddress( "fWthrDewPointTemp", &EqCalc::WthrDewPointTemp );
    fWthrHeatIndex = m_eqTree->setEqFunAddress( "fWthrHeatIndex", &EqCalc::WthrHeatIndex );
    fWthrRelativeHumidity = m_eqTree->setEqFunAddress( "fWthrRelativeHumidity", &EqCalc::WthrRelativeHumidity );
    fWthrSummerSimmerIndex = m_eqTree->setEqFunAddress( "fWthrSummerSimmerIndex", &EqCalc::WthrSummerSimmerIndex );
    fWthrWindChillTemp = m_eqTree->setEqFunAddress( "fWthrWindChillTemp", &EqCalc::WthrWindChillTemp );
    return;
}

//------------------------------------------------------------------------------
//  End of xeqcalc.cpp
//------------------------------------------------------------------------------
