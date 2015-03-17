//------------------------------------------------------------------------------
/*! \file bpcomposecontaindiagram.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2004-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Composes the Contain Module's diagrams.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "graph.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qapplication.h>
#include <qpen.h>
#include <qprogressdialog.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief Composes the Contain Module's fire shape diagram.
 */

void BpDocument::composeContainDiagram( void )
{
    // Does the user even want these figures?
    if ( ! property()->boolean( "containModuleActive" )
      || ! property()->boolean( "containCalcDiagram" ) )
    {
        return;
    }
    // We're gonna need these right away
    int rows  = tableRows();
    int cols  = tableCols();
    int cells = rows * cols;

    // Set up the progress dialog.
    int step = 0;
    int steps = cells;
    QString text(""), button("");
    translate( text, "BpDocument:Diagrams:DrawingShapes",
        QString( "%1" ).arg( cells ) );
    translate( button, "BpDocument:Diagrams:Abort" );
    QProgressDialog *progress = new QProgressDialog( text, button, steps );
    Q_CHECK_PTR( progress );
    progress->setMinimumDuration( 0 );
    progress->setProgress( 0 );

    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.

    // Title font is used in title above the diagram
    QFont titleFont(
        property()->string( "tableTitleFontFamily" ),
        property()->integer( "tableTitleFontSize" ) );
    QPen titlePen( property()->color( "tableTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    // Subtitle font is used in subtitle above the diagram
    QFont subTitleFont(
        property()->string( "tableSubtitleFontFamily" ),
        property()->integer( "tableSubtitleFontSize" ) );
    QPen subTitlePen( property()->color( "tableSubtitleFontColor" ) );
    QFontMetrics subTitleMetrics( subTitleFont );

    // Text font is used for all text in the diagram
    QFont textFont(
        property()->string( "containDiagramTextFontFamily" ),
        property()->integer( "containDiagramTextFontSize" ) );
    QPen textPen( property()->color( "containDiagramTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    // Value font is used for numbers and other results in the diagram
    QFont valueFont(
        property()->string( "containDiagramValueFontFamily" ),
        property()->integer( "containDiagramValueFontSize" ) );
    QPen valuePen( property()->color( "containDiagramValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    // Background color and frame
    QBrush bgBrush(
        property()->color( "containDiagramBgColor" ),
        Qt::SolidPattern );
    QPen framePen(
        property()->color( "containDiagramFrameColor" ),
        property()->integer( "containDiagramFrameWidth" ),
        Qt::SolidLine );

    // Fire perimeter and fill at report and attack
    QPen attackPerimPen(
        property()->color( "containDiagramAttackPerimColor" ),
        property()->integer( "containDiagramAttackPerimWidth" ),
        Qt::SolidLine );
    QBrush attackPerimBrush(
        property()->color( "containDiagramAttackFireColor" ),
        Qt::SolidPattern );
    QPen firelinePen(
        property()->color( "containDiagramFirelinePerimColor" ),
        property()->integer( "containDiagramFirelinePerimWidth" ),
        Qt::SolidLine );
    QPen reportPerimPen(
        property()->color( "containDiagramReportPerimColor" ),
        property()->integer( "containDiagramReportPerimWidth" ),
        Qt::SolidLine );
    QBrush reportPerimBrush(
        property()->color( "containDiagramReportFireColor" ),
        Qt::SolidPattern );

    // Axis lines
    QPen axlePen(
        property()->color( "containDiagramAxisColor"),
        property()->integer( "containDiagramAxisWidth" ),
        Qt::SolidLine );
    QPen gridPen(
        property()->color( "containDiagramAxisColor"),
        property()->integer( "containDiagramAxisWidth" ),
        Qt::DotLine );
    QFont axleFont(
        property()->string( "containDiagramTextFontFamily" ),
        property()->integer( "containDiagramTextFontSize" ) );
    QColor axleColor( "black" );

    // Diagram legend
    QFont legendFont(
        property()->string( "containDiagramLegendFontFamily" ),
        property()->integer( "containDiagramLegendFontSize" ) );
    QFontMetrics legendMetrics( legendFont );

    // Store pixel resolution into local variables.
    double yppi  = m_screenSize->m_yppi;
    //double xppi  = m_screenSize->m_xppi;
    //double padWd = m_pageSize->m_padWd / 2.;

    // Determine the height of the various display fonts.
    double textHt, titleHt, valueHt, legendHt;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    legendHt = ( legendMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Attempt to open the temporary file with the fire shape data.
    QString fileName = m_eqTree->m_traceFile;
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( fileName.latin1(), "r" ) ) )
    // This code block should never be executed!
    {
        translate( text, "BpDocument:Diagrams:UnableToOpenTempFile", fileName );
        error( text );
        return;
    }

    // Subscript constants for the CONTAIN input and output variables
    const int SPREAD = 0;
    const int SIZE   = 1;
    const int RATIO  = 2;
    const int TACTIC = 3;
    const int DIST   = 4;
    const int ASIZE  = 5;
    const int APERIM = 6;
    const int STATUS = 7;
    const int TIME   = 8;
    const int AREA   = 9;
    const int LINE   = 10;
    const int USED   = 11;
    const int COST   = 12;
    const int ABACK  = 13;
    const int AHEAD  = 14;
    const int RBACK  = 15;
    const int RHEAD  = 16;
    const int XMIN   = 17;
    const int XMAX   = 18;
    const int YMAX   = 19;
    const int STEPS  = 20;
    const int PARMS  = 21;
    const int FIRSTINPUT  = 0;
    const int LASTINPUT   = 4;
    const int FIRSTOUTPUT = 5;
    const int LASTOUTPUT  = 14;
    const int INPUTS      = 5;

    // Range variable pointers
    EqVar *rowVar = m_eqTree->m_rangeVar[0];
    EqVar *colVar = m_eqTree->m_rangeVar[1];
    // Input variable pointers
    EqVar *var[PARMS];
    var[RATIO]    = m_eqTree->m_eqCalc->vContainReportRatio;
    var[SPREAD]   = m_eqTree->m_eqCalc->vContainReportSpread;
    var[SIZE]     = m_eqTree->m_eqCalc->vContainReportSize;
    var[DIST]     = m_eqTree->m_eqCalc->vContainAttackDist;
    var[TACTIC]   = m_eqTree->m_eqCalc->vContainAttackTactic;
    // Output variable pointers
    var[STATUS]   = m_eqTree->m_eqCalc->vContainStatus;
    var[TIME]     = m_eqTree->m_eqCalc->vContainTime;
    var[AREA]     = m_eqTree->m_eqCalc->vContainSize;
    var[LINE]     = m_eqTree->m_eqCalc->vContainLine;
    var[USED]     = m_eqTree->m_eqCalc->vContainResourcesUsed;
    var[COST]     = m_eqTree->m_eqCalc->vContainCost;
    // Supporting output variables
    var[ABACK]    = m_eqTree->m_eqCalc->vContainAttackBack;
    var[AHEAD]    = m_eqTree->m_eqCalc->vContainAttackHead;
    var[APERIM]   = m_eqTree->m_eqCalc->vContainAttackPerimeter;
    var[ASIZE]    = m_eqTree->m_eqCalc->vContainAttackSize;
    var[RBACK]    = m_eqTree->m_eqCalc->vContainReportBack;
    var[RHEAD]    = m_eqTree->m_eqCalc->vContainReportHead;
    var[XMIN]     = m_eqTree->m_eqCalc->vContainXMin;
    var[XMAX]     = m_eqTree->m_eqCalc->vContainXMax;
    var[YMAX]     = m_eqTree->m_eqCalc->vContainYMax;
    var[STEPS]    = m_eqTree->m_eqCalc->vContainPoints;

    // Label output flags
    bool l_show[PARMS];
    l_show[RATIO]  = l_show[SPREAD] = l_show[SIZE] = l_show[TACTIC] = l_show[DIST] = true;
    l_show[STATUS] = property()->boolean( "containCalcStatus" );
    l_show[TIME]   = property()->boolean( "containCalcTime" );
    l_show[AREA]   = property()->boolean( "containCalcSize" );
    l_show[LINE]   = property()->boolean( "containCalcLine" );
    l_show[USED]   = property()->boolean( "containCalcResourcesUsed" );
    l_show[COST]   = property()->boolean( "containCalcCost" );
    l_show[APERIM] = property()->boolean( "containCalcAttackPerimeter" );
    l_show[ASIZE]  = property()->boolean( "containCalcAttackSize" );
    l_show[ABACK]  = l_show[AHEAD] = l_show[RBACK] = l_show[RHEAD] = false;
    l_show[XMIN]   = l_show[XMAX] = l_show[YMAX] = l_show[STEPS] = false;

    // Other properties we have available
    //int minSteps = property()->integer( "containConfMinSteps" );
    //int maxSteps = property()->integer( "containConfMaxSteps" );
    //bool retry   = property()->boolean( "containConfRetry" );

    // Number of bottom lines of text to reserve in the figure
    int botLines = 0;
    int parm;
    for ( parm = FIRSTOUTPUT;
          parm <= LASTOUTPUT;
          parm++ )
    {
        if ( l_show[parm] )
        {
            botLines++;
        }
    }
    if ( botLines < INPUTS )
    {
        botLines = INPUTS;
    }

    // Allocate storage for all run input and output values
    double *val[PARMS];
    for ( parm = 0;
          parm < PARMS;
          parm++ )
    {
        val[parm] = new double[ cells ];
        checkmem( __FILE__, __LINE__, val[parm], "double val[parm]", cells );
    }

    // Allocate file location of coordinates for each contain simulation
    long int *fpos = new long int[ cells ];
    checkmem( __FILE__, __LINE__, fpos, "long int fpos", cells );

    // Allocate datum-to-dataSet crosswalk
    // There may be more data cells than actual data sets.
    int *dataSet = new int[ cells ];
    checkmem( __FILE__, __LINE__, dataSet, "int dataSet", cells );
    int dataSets = 0;       // This is the actual number of ContainFF() runs

    // Load all the contain data from the file into memory.
    char buffer[1024], io[256], l_name[256], units[256];
    int  lines = 0;
    int  decimals;
    int  datum = 0;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        lines++;
        // Check for ContainDiagram records
        if ( strstr( buffer, "begin proc ContainFF()" ) )
        {
            // The next 15 file lines contain the variable names and values
            for ( parm = 0;
                  parm < PARMS;
                  parm++ )
            {
                lines++;
                fgets( buffer, sizeof(buffer), fptr );
                if ( sscanf( buffer, "%s %s %lf %d %s",
                    io, l_name, val[parm]+dataSets, &decimals, units ) != 5 )
                // This code block should never be executed!
                {
                    translate( text, "BpDocument:Diagrams:UnableToReadFields",
                        "ContainDiagram", QString( "%1" ).arg( lines ) );
                    error( text );
                    fclose( fptr );
                    return;
                }
            }
            // The next line is the first coordinate pair
            fpos[dataSets] = ftell( fptr );
            dataSets++;
        }
        else if ( strstr( buffer, "vContainDiagram" )
               && strstr( buffer, "begin output" ) )
        {
            dataSet[datum++] = dataSets-1;
        }
    }

    // Rewind the file.
    rewind( fptr );

    // Determine maximum fire and fireline distances
    int i;
    double xValMax = val[XMAX][0];
    double yValMax = val[YMAX][0];
    double xValMin = val[XMIN][0];
    for ( i = 0;
          i < dataSets;
          i++ )
    {
        xValMin = ( val[XMIN][i] < xValMin ) ? val[XMIN][i] : xValMin;
        xValMin = ( -val[ABACK][i] < xValMin ) ? -val[ABACK][i] : xValMin;
        xValMin = ( -val[RBACK][i] < xValMin ) ? -val[RBACK][i] : xValMin;
        xValMax = ( val[XMAX][i] > xValMax ) ? val[XMAX][i] : xValMax;
        xValMax = ( val[AHEAD][i] > xValMax ) ? val[AHEAD][i] : xValMax;
        xValMax = ( val[RHEAD][i] > xValMax ) ? val[RHEAD][i] : xValMax;
        // Y-scale could be based on either constructed line or attack perim
        yValMax = ( val[YMAX][i] > yValMax ) ? val[YMAX][i] : yValMax;
        double length = val[ABACK][i] + val[AHEAD][i];
        double width  = length / val[RATIO][i];
        yValMax = ( width > yValMax ) ? width : yValMax;
    }

    // Determine number of diagram panes on each page.
    int paneCols = property()->integer( "containDiagramCols" );
    int paneRows = property()->integer( "containDiagramRows" );
    // If 2 or less diagrams, use half a page
    if ( cells <= 2 || cols == 1 )
    {
        paneCols = 1;
        paneRows = 2;
    }
    // Determine size of each diagram pane on the page.
    int    panes  = paneCols * paneRows;
    double paneWd = m_pageSize->m_bodyWd / (double) paneCols;
    double paneHt = ( m_pageSize->m_bodyHt - 2 * titleHt ) / (double) paneRows;

    // Determine number of pages across and down to accommodate all diagrams
    int pageCols = 1 + ( cols - 1 ) / paneCols;
    int pageRows = 1 + ( rows - 1 ) / paneRows;

    // Each diagram may have 0-2 top lines
    // (bottom lines were determined above)
    int topLines = 1;                           // Y axis top label
    if ( rowVar ) topLines++;
    if ( colVar ) topLines++;

    // Determine space remaining for figure
    double figTop = topLines * textHt;          // figure top text margin
    double figBot = botLines * textHt;          // figure bottom text margin
    double figHt  = paneHt - figTop - figBot;   // figure drawing space
    double figWd  = paneWd;

    // Determine  nice axis endpoints for largest figure
    double xMin, xMax, xStep, yMin, yMax, yStep;
    int xTics, xDec, yTics, yDec;
    niceAxis(  xValMin, xValMax, 5, &xMin, &xMax, &xTics, &xStep, &xDec );
    niceAxis( -yValMax, yValMax, 5, &yMin, &yMax, &yTics, &yStep, &yDec );

	// Determine graph WORLD coordinates within figHt and figWd
    // Note 1: the diagram must have a 1:1 aspect ratio since it is a map
	// Note 2: xWorld >= yWorld since its an ellipse running in the x dimension
	double xWorld = xMax - xMin;
	double yWorld = yMax - yMin;

	double gXMin, gXMax, gYMin, gYMax, w, h, l_x, l_y, z, l, fx, fy, fz;
    double aspect = figHt / figWd;
    if ( aspect < ( yWorld / xWorld ) )
    {
        gYMin = yMin - yStep;           // diagram WORLD minimum Y
        gYMax = yMax + yStep;           // diagram WORLD maximum Y
        h     = gYMax - gYMin;          // diagram WORLD total height
        w     = h / aspect;             // diagram WORLD total width
        l_x   = w - ( xMax - xMin );    // diagram WORLD extra width
        gXMin = xMin - 0.5 * l_x;       // diagram WORLD minimum X
        gXMax = xMax + 0.5 * l_x;       // diagram WORLD maximum X
    }
    else
    {
        gXMin = xMin - xStep;           // diagram WORLD minimum X
        gXMax = xMax + xStep;           // diagram WORLD maximum X
        w     = gXMax - gXMin;          // diagram WORLD total width
        h     = w * aspect;             // diagram WORLD total height
        l_x   = h - ( yMax - yMin );    // diagram WORLD extra height
        gYMin = yMin - 0.5 * l_x;       // diagram WORLD minimum Y
        gYMax = yMax + 0.5 * l_x;       // diagram WORLD maximum Y
    }
	// Build 607: x and y must have same scale!
    double cwRatio = figWd / w;         // ratio of COMPOSER-to-WORLD units

    // Create a graph with these WORLD dimensions.
    Graph *g = new Graph();
    checkmem( __FILE__, __LINE__, g, "Graph g", 1 );
    g->setCanvasRotation( 0.0 );
    g->setCanvasScale( 1.0, 1.0 );
    g->setWorld( gXMin, gYMin, gXMax, gYMax );

    // Create the x axis
    GraphAxle *xAxle = g->addGraphAxle( xMin, xMax, axlePen );
    xAxle->setWorld( xMin, yMin, xMax, yMin );
    xAxle->setMajorLabels( axleFont, axleColor, xDec );
    xAxle->setMajorTics( GraphAxleBottom, xMin, xMax, xStep,
        ( 0.03 * ( yMax - yMin ) ), axlePen );
    //xAxle->setMajorGrid( gridPen, ( yMax - yMin ) );
    xAxle->setTitle( var[LINE]->m_displayUnits,
        GraphAxleBottom, axleFont, axleColor );

    // Create the y axis
    GraphAxle *yAxle = g->addGraphAxle( yMin, yMax, axlePen );
    yAxle->setWorld( xMin, yMin, xMin, yMax );
    yAxle->setMajorLabels( axleFont, axleColor, yDec );
    yAxle->setMajorTics( GraphAxleLeft, yMin, yMax, yStep,
        (0.03 * ( xMax - xMin ) ), axlePen );
    //yAxle->setMajorGrid( gridPen, ( xMax - xMin ) );
    yAxle->setTitle( var[LINE]->m_displayUnits,
        GraphAxleLeft, axleFont, axleColor );

    // Determine location and center of each diagram pane on the page.
    double *top  = new double[ panes ];
    checkmem( __FILE__, __LINE__, top, "double top", panes );
    double *left = new double[ panes ];
    checkmem( __FILE__, __LINE__, left, "double left", panes );
    int row, col, pane;
    for ( pane = 0, row = 0;
          row < paneRows;
          row++ )
    {
        for ( col = 0;
              col < paneCols;
              col++, pane++ )
        {
            top[pane] = m_pageSize->m_marginTop + 2 * titleHt + row * paneHt;
            left[pane] = m_pageSize->m_marginLeft + col * paneWd;
        }
    }
    // Get translated text
    QString title("");
    translate( title, "BpDocument:Diagrams:ContainTitle" );
    QString desc( m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace() );

    // Loop for each PAGE across and down.
    double yPos;
    QString qStr1, qStr2;
    int thisPage = 1;
    int datumRow, datumCol, iid;
    int center = Qt::AlignVCenter|Qt::AlignHCenter;
    int aleft  = Qt::AlignVCenter|Qt::AlignLeft;
    int aright = Qt::AlignVCenter|Qt::AlignRight;
    for ( int pageRow = 0;
          pageRow < pageRows;
          pageRow++ )
    {
        for ( int pageCol = 0;
              pageCol < pageCols;
              pageCol++, thisPage++ )
        {
            // Table title indicates the table portion.
            translate( qStr1, "BpDocument:Diagrams:ContainPage",
                QString( "%1" ).arg( thisPage ),
                QString( "%1" ).arg( pageCols * pageRows ) );

            // Start a new page
            startNewPage( qStr1, TocShape );
            yPos = m_pageSize->m_marginTop;

            // Draw a map of where we are.
            composePageMap( ( 2. * titleHt - 0.1 ), rows, cols,
                pageRows, pageCols, paneRows, paneCols, pageRow, pageCol );

            // Display the run description as the table title
            m_composer->font( titleFont );          // use tableTitleFont
            m_composer->pen( titlePen );            // use tableTitleFontColor
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                center,                     desc ); // center text
            yPos += titleHt;

            // Display the diagram name as the table subtitle
            m_composer->font( subTitleFont );       // use tableSubtitleFont
            m_composer->pen( subTitlePen );         // use tableSubtitleFontColor
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // start at UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                center,                     title );// center text
            yPos += titleHt;

            // Loop for each PANE on the page.
            for ( pane = 0, row = 0;
                  row < paneRows;
                  row++ )
            {
                for ( col = 0;
                      col < paneCols;
                      col++, pane++ )
                {
                    // Which data element is this?
                    datumRow = row + paneRows * pageRow;
                    datumCol = col + paneCols * pageCol;
                    datum = datumCol + datumRow * tableCols();
                    if ( datumRow >= rows || datumCol >= cols )
                    {
                        continue;
                    }
                    // Pane background and border.
                    m_composer->fill( left[pane], top[pane], paneWd, paneHt,
                        bgBrush );
                    m_composer->pen( framePen );
                    m_composer->rect( left[pane], top[pane], paneWd, paneHt );

                    // Display current table row variable value (if any)
                    yPos = top[pane];
                    m_composer->font( textFont );   // use tableTextFont
                    m_composer->pen( titlePen );    // use tableTitleFontColor
                    if ( rowVar )
                    {
                        if ( rowVar->isDiscrete() )
                        {
                            iid = (int) tableRow( datumRow );
                            qStr1 = QString( "%1 : %2" )
                                .arg( *(rowVar->m_label) )
                                .arg( rowVar->m_itemList->itemName( iid ) );
                        }
                        else if ( rowVar->isContinuous() )
                        {
                            qStr1 = QString( "%1 : %2 %3" )
                                .arg( *(rowVar->m_label) )
                                .arg( tableRow( datumRow ), 0, 'f',
                                    rowVar->m_displayDecimals )
                                .arg( rowVar->displayUnits() );
                        }
                        m_composer->text(
                            left[pane], yPos,
                            paneWd,     textHt,
                            center,     qStr1 );
                        yPos += textHt;
                    }
                    // Display current table column variable value (if any)
                    if ( colVar )
                    {
                        if ( colVar->isDiscrete() )
                        {
                            iid = (int) tableCol( datumCol );
                            qStr1 = QString( "%1 : %2" )
                                .arg( *(colVar->m_label) )
                                .arg( colVar->m_itemList->itemName( iid ) );
                        }
                        else if ( colVar->isContinuous() )
                        {
                            qStr1 = QString( "%1 : %2 %3" )
                                .arg( *(colVar->m_label) )
                                .arg( tableCol( datumCol ), 0, 'f',
                                    colVar->m_displayDecimals )
                                .arg( colVar->displayUnits() );
                        }
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += textHt;
                    }

                    // Draw the fire coordinate system
                    m_composer->graph( *g, left[pane], ( top[pane] + figTop ),
                        figWd, figHt );

                    // Draw fire perimeter at initial attack
                    // length (l) and width (w) are in composer units
                    l = cwRatio * ( val[ABACK][dataSet[datum]]
                       + val[AHEAD][dataSet[datum]] );
                    w = l / val[RATIO][dataSet[datum]];
                    l_x = left[pane]
                      + ( ( -val[ABACK][dataSet[datum]] - gXMin )
                      / ( gXMax - gXMin ) )
                      * figWd;
                    l_y = top[pane] + figTop + 0.5 * ( figHt - w );
                    m_composer->pen( attackPerimPen );
                    //m_composer->brush( attackPerimBrush );
                    m_composer->ellipse( l_x, l_y, l, w );

                    // Draw fire perimeter at time of report
                    // (only if its not the same as the initial attack)
                    l_x = l;
                    l = cwRatio * ( val[RBACK][dataSet[datum]]
                      + val[RHEAD][dataSet[datum]] );
                    if ( fabs( l_x - l ) > 0.0001 )
                    {
                        w = l / val[RATIO][dataSet[datum]];
                        l_x = left[pane]
                          + ( ( -val[RBACK][dataSet[datum]] - gXMin )
                          / ( gXMax - gXMin ) )
                          * figWd;
                        l_y = top[pane] + figTop + 0.5 * ( figHt - w );
                        m_composer->pen( reportPerimPen );
                        //m_composer->brush( reportPerimBrush );
                        m_composer->ellipse( l_x, l_y, l, w );
                    }

                    // Draw axis through origin
                    m_composer->pen( axlePen );
                    fx = left[pane]
                       + figWd * ( ( xMax - gXMin ) / ( gXMax - gXMin ) );
                    l_x  = left[pane]
                       + figWd * ( ( xMin - gXMin ) / ( gXMax - gXMin ) );
                    fy = top[pane] + figTop
                       + figHt * ( ( gYMax - 0. ) / ( gYMax + gYMax ) );
                    m_composer->line( l_x, fy, fx, fy );

                    fx = left[pane]
                       + figWd * ( ( 0. - gXMin ) / ( gXMax - gXMin ) );
                    fy = top[pane] + figTop
                       + figHt * ( ( gYMax - yMax ) / ( gYMax + gYMax ) );
                    l_y  = top[pane] + figTop
                       + figHt * ( ( gYMax - yMin ) / ( gYMax + gYMax ) );
                    m_composer->line( fx, l_y, fx, fy );

                    // Draw fireline constructed
                    m_composer->pen( firelinePen );
                    if ( fseek( fptr, fpos[dataSet[datum]], SEEK_SET ) )
                    {
                        text = QString( "Unable to position to file %1 byte %2 "
                            " for Contain Diagram %3" )
                            .arg( fileName ).arg( fpos[dataSet[datum]] ).arg( datum );
                        bomb( text );
                    }
                    for ( i = 0;
                          i < (int) (val[STEPS][dataSet[datum]] + 0.1 );
                          i++ )
                    {
                        // Read the coordinate pair
                        fgets( buffer, sizeof(buffer), fptr );
                        sscanf( buffer, "%lf %lf", &fx, &fz );
                        // Convert from WORLD into DIAGRAM (inches)
                        fx = left[pane]
                           + figWd * ( ( fx - gXMin ) / ( gXMax - gXMin ) );
                        fy = top[pane] + figTop
                           + figHt * ( ( gYMax - fz ) / ( gYMax + gYMax ) );
                        fz = top[pane] + figTop
                           + figHt * ( ( gYMax + fz ) / ( gYMax + gYMax ) );
                        // If not the first point, draw the line segment
                        if ( i )
                        {
                            m_composer->line( l_x,  l_y, fx, fy );
                            m_composer->line( l_x,  z, fx, fz );
                        }
                        l_x = fx;
                        l_y = fy;
                        z = fz;
                    }

                    // Draw legend in upper right corner
                    yPos = top[pane] + figTop + 1.5 * valueHt;
                    l_x  = left[pane] + paneWd - 0.4;
                    fx = left[pane] + paneWd - 0.2;
                    m_composer->pen( reportPerimPen );
                    m_composer->line( l_x, yPos, fx, yPos );
                    yPos += valueHt;
                    m_composer->pen( attackPerimPen );
                    m_composer->line( l_x, yPos, fx, yPos );
                    yPos += valueHt;
                    m_composer->pen( firelinePen );
                    m_composer->line( l_x, yPos, fx, yPos );

                    yPos = top[pane] + figTop + valueHt;
                    m_composer->font( textFont );
                    m_composer->pen( valuePen );    // use tableTextValueColor
                    translate( qStr1,
                        "BpDocument:Diagrams:Legend:PerimAtReport" );
                    m_composer->text(
                        left[pane],     yPos,
                        paneWd-0.5,     textHt,
                        aright,         qStr1 );
                    yPos += valueHt;
                    translate( qStr1,
                        "BpDocument:Diagrams:Legend:PerimAtAttack" );
                    m_composer->text(
                        left[pane],     yPos,
                        paneWd-0.5,     textHt,
                        aright,         qStr1 );
                    yPos += valueHt;
                    translate( qStr1,
                        "BpDocument:Diagrams:Legend:FirelineConstructed" );
                    m_composer->text(
                        left[pane],     yPos,
                        paneWd-0.5,     textHt,
                        aright,         qStr1 );
                    yPos += valueHt;

                    // Display inputs in lower left corner
                    yPos = top[pane] + figTop + figHt;
                    m_composer->font( textFont );
                    m_composer->pen( valuePen );    // use tableTextValueColor
                    for ( parm = FIRSTINPUT;
                          parm <= LASTINPUT;
                          parm++ )
                    {
                        if ( var[parm]->isDiscrete() )
                        {
                            iid = (int) ( val[parm][dataSet[datum]] + 0.1 );
                            qStr1 = QString( "%1    %2" )
                                .arg( *(var[parm]->m_label) )
                                .arg( var[parm]->m_itemList->itemName( iid ) );
                        }
                        else if ( var[parm]->isContinuous() )
                        {
                            qStr1 = QString( "%1    %2 %3" )
                                .arg( *(var[parm]->m_label) )
                                .arg( val[parm][dataSet[datum]], 0, 'f',
                                    var[parm]->m_displayDecimals )
                                .arg( var[parm]->displayUnits() );
                        }
                        m_composer->text(
                            left[pane]+0.2, yPos,
                            paneWd-0.2,     textHt,
                            aleft,          qStr1 );
                        yPos += 0.9 * valueHt;
                    }
                    // Display outputs in lower right corner
                    yPos = top[pane] + figTop + figHt;
                    for ( parm = FIRSTOUTPUT;
                          parm <= LASTOUTPUT;
                          parm++ )
                    {
                        if ( l_show[parm] )
                        {
                            if ( var[parm]->isDiscrete() )
                            {
                                iid = (int) ( val[parm][dataSet[datum]] + 0.1 );
                                qStr1 = QString( "%1    %2" )
                                    .arg( *(var[parm]->m_label) )
                                    .arg( var[parm]->m_itemList->itemName( iid ) );
                            }
                            else if ( var[parm]->isContinuous() )
                            {
                                qStr1 = QString( "%1    %2 %3" )
                                    .arg( *(var[parm]->m_label) )
                                    .arg( val[parm][dataSet[datum]], 0, 'f',
                                        var[parm]->m_displayDecimals )
                                    .arg( var[parm]->displayUnits() );
                            }
                            m_composer->text(
                                left[pane],     yPos,
                                paneWd-0.2,     textHt,
                                aright,         qStr1 );
                            yPos += 0.9 * valueHt;
                        }
                    }
                    // Update progress dialog.
                    progress->setProgress( ++step );
                    qApp->processEvents();
                    if ( progress->wasCancelled() )
                    {
                        delete progress;    progress = 0;
                        return;
                    }
                }   // Next paneCol
            }   // Next paneRow
        }   // Next pageCol
    }   // Next pageRow

    // Close the file.
    fclose( fptr );

    // Clean up and return
    delete progress;    progress = 0;
    for ( parm = 0;
          parm < PARMS;
          parm++ )
    {
        delete[] val[parm]; val[parm] = 0;
    }
    delete   g;         g = 0;
    delete[] fpos;      fpos = 0;
    delete[] dataSet;   dataSet = 0;
    delete[] top;       top = 0;
    delete[] left;      left = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposecontaindiagram.cpp
//------------------------------------------------------------------------------

