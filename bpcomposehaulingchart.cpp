//------------------------------------------------------------------------------
/*! \file bpcomposehaulingchart.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class page composition methods.
 *
 *  Additional BehavePlusDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefiremaxdir.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposetable3.cpp
 *      - bpcomposeworksheet.cpp
 *      - bpworksheet.cpp
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "graph.h"
#include "graphaxle.h"
#include "graphbar.h"
#include "graphline.h"
#include "graphmarker.h"
#include "property.h"
#include "xeqtree.h"

// Standard include files
#include <math.h>

// Hauling chart large XPM pixmaps
//#include "bpcomposehaulingchart.xpm"

// Hauling chart small XPM pixmaps
#include "bpcomposehaulingchart2.xpm"

//------------------------------------------------------------------------------
/*! \brief Draws the final fire characteristics chart.
 *
 *  \return TRUE if the diagram is active and was drawn.
 */

void BpDocument::composeFireCharacteristicsDiagram( void )
{
    // Surface Module must be active and using fuel model inputs
    PropertyDict *prop = m_eqTree->m_propDict;
    if ( ! prop->boolean( "surfaceModuleActive" )
      || ! prop->boolean( "surfaceCalcFireCharacteristicsDiagram" ) )
    {
        return;
    }

    // Graph fonts.
    QFont  textFont( property()->string( "graphTextFontFamily" ),
                     property()->integer( "graphTextFontSize" ) );
    QColor textColor( property()->color( "graphTextFontColor" ) );
    QPen   textPen( textColor );
    QFont  subTitleFont( property()->string( "graphSubtitleFontFamily" ),
                    property()->integer( "graphSubtitleFontSize" ) );
    QColor subTitleColor( property()->color( "graphSubtitleFontColor" ) );

    // Open the result file
    QString resultFile = m_eqTree->m_resultFile;
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( resultFile.latin1(), "r" ) ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "BpDocument:FireCharacteristicsDiagram:NoLogOpen",
            resultFile );
        error( text );
        return;
    }
    // Allocate ros and hpua data arrays
    int rows  = tableRows();
    int cols  = tableCols();
    int cells = rows * cols;
    double *hpua = new double[ cells ];
    checkmem( __FILE__, __LINE__, hpua, "double hpua", cells );
    double *ros = new double[ cells ];
    checkmem( __FILE__, __LINE__, ros, "double ros", cells );
    // Set the variable names we're looking for
    const char* hpuaName = "vSurfaceFireHeatPerUnitArea";
    const char* rosName = "vSurfaceFireSpreadAtHead";
    if ( prop->boolean( "surfaceConfSpreadDirInput" ) )
    {
        rosName = "vSurfaceFireSpreadAtVector";
    }
    // Read and store the ros and hpua values
    char   buffer[1024], varName[128], varUnits[128];
    int    row, col, cell;
    double value;
    double rosMax = 0.0;
    double hpuaMax = 0.0;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        if ( strncmp( buffer, "CELL", 4 ) == 0 )
        {
            if ( strstr( buffer, hpuaName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf %s",
                    &row, &col, varName, &value, varUnits );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                if ( ( hpua[ cell ] = value ) > hpuaMax )
                {
                    hpuaMax = value;
                }
            }
            else if ( strstr( buffer, rosName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf %s",
                    &row, &col, varName, &value, varUnits );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                if ( ( ros[ cell ] = value ) > rosMax )
                {
                    rosMax = value;
                }
            }
        }
    }
    fclose( fptr );

    // Get variable pointers
    EqVar *hpuaVar = m_eqTree->m_varDict->find( "vSurfaceFireHeatPerUnitArea" );
    EqVar *rosVar = m_eqTree->m_varDict->find( "vSurfaceFireSpreadAtHead" );
    EqVar *fliVar = m_eqTree->m_varDict->find( "vSurfaceFireLineIntAtHead" );
    EqVar *flVar = m_eqTree->m_varDict->find( "vSurfaceFireFlameLengAtHead" );

    // Conversion factor
    double flFactor, fliFactor, rosFactor, hpuaFactor, offset;
    appSiUnits()->conversionFactorOffset(
        flVar->m_nativeUnits, flVar->m_displayUnits, &flFactor, &offset );
    appSiUnits()->conversionFactorOffset(
        fliVar->m_nativeUnits, fliVar->m_displayUnits, &fliFactor, &offset );
    appSiUnits()->conversionFactorOffset(
        hpuaVar->m_nativeUnits, hpuaVar->m_displayUnits, &hpuaFactor, &offset );
    appSiUnits()->conversionFactorOffset(
        rosVar->m_nativeUnits, rosVar->m_displayUnits, &rosFactor, &offset );

    // Determine which of four different chart scales to use
    static const int Scales = 4;
    static double RosScale[Scales] = { 100., 200., 400., 800. };        // ft/min
    static double HpuaScale[Scales] = { 2000., 4000., 8000., 16000. };  // Btu/ft2
    double rosScale = 0.;       // Max y-axis ros
    double hpuaScale = 0.;      // Max x-axis hpua
    int scale;
    for ( scale=0; scale<Scales; scale++ )
    {
        if ( rosMax < ( rosScale = RosScale[ scale ] ) )
        {
            break;
        }
    }
    for ( scale=0; scale<Scales; scale++ )
    {
        if ( hpuaMax < ( hpuaScale = HpuaScale[scale] ) )
        {
            break;
        }
    }
    // Set axis maximums to appropriate predefined scale in display units
    rosMax  = rosFactor * rosScale;
    hpuaMax = hpuaFactor * hpuaScale;
    double ratio = rosMax / hpuaMax;

    // Create the graph
    Graph  graph;
    GraphLine *graphLine;
    GraphMarker *graphMarker;
    static const int Points = 100;
    double l_x[Points];
    double l_y[Points];

    // Draw the four standard hauling chart fli-fl levels
    static const int Lines = 4;
    static const double Fli[Lines] = { 100., 500., 1000., 2000. };  // Btu/ft/s
    static const double Fl[Lines] = { 4., 8., 11., 15. };           // ft

    // Create the hauling chart lines
    // Put Fireline Int label 65% of the way along the HPUA axis (display units)
    double  xPosFli = 0.65 * hpuaMax;
    // Put Flame Length label 85% of the way along the HPUA axis (display units)
    double  xPosFl  = 0.85 * hpuaMax;
    // Fireline Int and Flame Length label Y positions (display units)
    double  yPosFl[Lines], yPosFli[Lines];
    // Icon locations (in display units)
    double xIcon[Lines+1], yIcon[Lines+1];
    double diff, minDiff;
    QString label;
    QPen    redPen( "red", 1 );
    QColor  blackColor( "black" );
    int     alignCenter = Qt::AlignHCenter | Qt::AlignVCenter;
    // Fireline intensity - flame length curves
    int     line, point;
    for ( line = 0; line < Lines; line++ )
    {
        minDiff = 999999999.;
        for ( point = 0; point < Points; point++ )
        {
            // Hpua value in native units (Btu/ft2)
            l_x[point] = ( (point+1) * hpuaScale ) / (double) Points;
            // Ros value in native units (ft/min)
            l_y[point] = 60. * Fli[line] / l_x[point];
            // Convert to display units
            l_x[point] *= hpuaFactor;
            l_y[point] *= rosFactor;
            // Check for curve inflection point (for icon placement)
            if ( ( diff = fabs( l_y[point]/l_x[point] - ratio ) ) < minDiff )
            {
                minDiff = diff;
                xIcon[line+1] = l_x[point];
                yIcon[line+1] = l_y[point];
            }
        }
        // Create a graph line (with its own copy of the data).
        graphLine = graph.addGraphLine( Points, l_x, l_y, redPen );

        // Fireline intensity label
        label = QString( "%1" ).arg( ( Fli[line] * fliFactor ), 0, 'f', 0 );
        yPosFli[line] = rosFactor * ( 60. * Fli[line] / ( xPosFli / hpuaFactor ) );
        graph.addGraphMarker( xPosFli, yPosFli[line], label, textFont,
            blackColor, alignCenter );

        // Flame length label
        label = QString( "%1" ).arg( ( Fl[line] * flFactor ), 0, 'f', 0 );
        yPosFl[line] = rosFactor * ( 60. * Fli[line] / ( xPosFl / hpuaFactor ) );
        graph.addGraphMarker( xPosFl, yPosFl[line], label, textFont,
            blackColor, alignCenter );
    } // Next line

    // Fireline intensity label and units
    translate( label, "BpDocument:FireCharacteristicsDiagram:FLI" );
    graph.addGraphMarker( xPosFli, ( yPosFli[Lines-1] + 0.10 * rosMax ),
        label, textFont, blackColor, alignCenter );
    graph.addGraphMarker( xPosFli, ( yPosFli[Lines-1] + 0.05 * rosMax ),
        fliVar->m_displayUnits, textFont, blackColor, alignCenter );

    // Flame length label and units
    translate( label, "BpDocument:FireCharacteristicsDiagram:FL" );
    graph.addGraphMarker( xPosFl, ( yPosFl[Lines-1] + 0.10 * rosMax ),
        label, textFont, blackColor, alignCenter );
    graph.addGraphMarker( xPosFl, ( yPosFl[Lines-1] + 0.05 * rosMax ),
        flVar->m_displayUnits, textFont, blackColor, alignCenter );

    // Add icons
    QPixmap pixmap[Lines];
    pixmap[0] = QPixmap( fireman_xpm );
    pixmap[1] = QPixmap( dozer_xpm );
    pixmap[2] = QPixmap( torchtree_xpm );
    pixmap[3] = QPixmap( mtnfire_xpm );
    xIcon[0] = yIcon[0] = 0.0;
    for ( line=0; line<Lines; line++ )
    {
        graphMarker = graph.addGraphMarker(
            xIcon[line] + ( 0.5 * ( xIcon[line+1] - xIcon[line] ) ),
            yIcon[line] + ( 0.5 * ( yIcon[line+1] - yIcon[line] ) ),
            "", textFont, blackColor, alignCenter );
        graphMarker->setGraphMarkerPixmap( pixmap[line] );
    }

    // Finally, add a marker for each output result
    QColor  bluePen( "blue" );
    for ( cell=0; cell<cells; cell++ )
    {
        //fprintf( stderr, "%02d: %3.2f  %3.2f\n", i, hpua[i], ros[i] );
        graph.addGraphMarker( hpua[cell], ros[cell],
            QString( "%1" ).arg( cell + 1 ), textFont, bluePen, alignCenter );
    }
    // Compose the graph
    EqVar *zVar = 0;
    GraphAxleParms xParms( 0.0, hpuaMax, 11 );
    GraphAxleParms yParms( 0.0, rosMax, 11 );
    composeGraphBasics( &graph, true, hpuaVar, rosVar, zVar, Lines,
        &xParms, &yParms );

    // Create a separate page for this graph.
    translate( label, "BpDocument:FireCharacteristicsDiagram:Caption" );
    graph.setSubTitle( label, subTitleFont, subTitleColor );
    startNewPage( label, TocHaulChart );

    // This is how we save the graph and its composer.
    m_composer->graph( graph,
        m_pageSize->m_marginLeft
            + m_pageSize->m_bodyWd * property()->real( "graphXOffset" ),
        m_pageSize->m_marginTop
            + m_pageSize->m_bodyHt * property()->real( "graphYOffset" ),
        m_pageSize->m_bodyWd * property()->real( "graphScaleWidth" ),
        m_pageSize->m_bodyHt * property()->real( "graphScaleHeight" )
    );
    // Be polite and stop the composer.
    m_composer->end();
    delete[] ros;   ros  = 0;
    delete[] hpua;  hpua = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposehaulingchart.cpp
//------------------------------------------------------------------------------

