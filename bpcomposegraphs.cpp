//------------------------------------------------------------------------------
/*! \file bpcomposegraphs.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument graph composition methods.
 *
 *  Additional BpDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefiremaxdir.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposetable3.cpp
 *      - bpcomposeworksheet.cpp
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "docscrollview.h"
#include "graph.h"
#include "graphaxle.h"
#include "graphbar.h"
#include "graphlimitsdialog.h"
#include "graphline.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qapplication.h>
#include <qprogressdialog.h>

//! Maximum number of bars per graph.
const int graphMaxBars  = 255;

//! Maximum number of lines per graph.
const int graphMaxLines = 255;

//! Maximum number of points calculated per graph line.
const int graphMaxSteps = 100;

//------------------------------------------------------------------------------
/*! \brief Determines the minimum and maximum data point Y values
 *  of all bars of a bar graph.
 *
 *  \param yMin Reference to the returned minimum y data value.
 *  \param yMax Reference to the returned maximum y data value.
 *
 *  Called only by BpDocument::composeGraphs() in preparation for
 *  determining nice axle parameters.
 */

void BpDocument::barYMinMax( int yid, double &yMin, double &yMax )
{
    // Store the x-variable range and step size in locals.
    int    bars  = ( tableRows() < graphMaxBars )
                 ? tableRows()
                 : graphMaxBars ;
    int    vid   = yid;
    int    vStep = tableVars();
    double val;
    yMin = yMax = tableVal( yid );
    for ( int row = 1;
          row <= bars;
          row++ )
    {
        val  = tableVal( vid );
        vid += vStep;
        yMin = ( val < yMin ) ? val : yMin;
        yMax = ( val > yMax ) ? val : yMax;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes a single bar graph with two or more bars.
 *
 *  \param yid      Index of the y-variable.  This is actually an index
 *                  into the m_eqTree->tableVar[] array.
 *  \param xVar     Pointer to the EqVar used for the x-axis (bars).
 *                  This is stored in the EqTree as the row variable, and
 *                  its values are returned via the tableRow(row) function.
 *  \param yVar     Pointer to the EqVar used for the y-axis.  This is one
 *                  of the variables in the tableVar[] array.
 *  \param xParms   Pointer to x-axle parameters.
 *  \param yParms   Pointer to y-axle parameters.
 *
 *  Called only by composeGraphs().
 */

void BpDocument::composeBarGraph( int yid, EqVar *xVar, EqVar *yVar,
        GraphAxleParms *xParms, GraphAxleParms *yParms )
{
    //--------------------------------------------------------------------------
    // 1: Set up fonts, pens, and colors used by this graph
    //--------------------------------------------------------------------------

    // Graph fonts.
    QFont textFont( property()->string( "graphTextFontFamily" ),
                    property()->integer( "graphTextFontSize" ) );
    QColor textColor( property()->color( "graphTextFontColor" ) );
    QPen textPen( textColor );

    // How many colors are requested?
    QString colorName( property()->color( "graphBarColor" ) );
    int colors = 1;
    if ( colorName == "rainbow" )
    {
        colors = property()->integer( "graphRainbowColors" );
    }
    // Allocate number of requested colors.
    QColor *color = new QColor [colors];
    checkmem( __FILE__, __LINE__, color, "QColor color", colors );
    int colorId = 0;
    // If only 1 color...
    if ( colors == 1 )
    {
        color[0].setNamedColor( colorName );
    }
    // else if rainbow colors are requested.
    else
    {
        // \todo - add some code to check conflicts with graphBackgroundColor
        // and graphGridColor (if graphGridWidth > 0 ).
        int hue = 0;
        for ( colorId = 0;
              colorId < colors;
              colorId++ )
        {
            color[colorId].setHsv( hue, 255, 255);
            hue += 360 / colors;
        }
        colorId = 0;
    }
    // Setup bar brush and color
    QBrush barBrush( color[colorId], Qt::SolidPattern );

    //--------------------------------------------------------------------------
    // 2: Create the graph and its text and axles.
    //--------------------------------------------------------------------------

    // Initialize graph and variables
    Graph g;
    int bars  = ( tableRows() < graphMaxBars )
              ? tableRows()
              : graphMaxBars ;
    int vStep = tableVars();

    // Draw thew basic graph (axis and text)
    composeGraphBasics( &g, false, xVar, yVar, 0, bars, xParms, yParms );

    //--------------------------------------------------------------------------
    // 3: Add the bars.
    //--------------------------------------------------------------------------

    // Each bar occupies 2/3 of its x range, plus a 1/3 padding on right.
    double xMin = xParms->m_axleMin;
    double xMax = xParms->m_axleMax;
    double xMinorStep = ( xMax - xMin ) / (double) ( 3 * bars + 1 );
    double xMajorStep = 3. * xMinorStep;

    // Create each data bar and add it to the graph.
    double x0, x1, y0, y1, xlabel, ylabel;
    double rotation = 0.;
    QString label;
    int row, vid;
    for ( row = 0, vid = yid;
          row < bars;
          row++, vid += vStep )
    {
        x0 = xMin + xMinorStep + row * xMajorStep;
        x1 = xMin + ( row + 1 ) * xMajorStep;
        y0 = yParms->m_axleMin;
        y1 = tableVal( vid );

        // If we're out of colors, start over.
        if ( colorId >= colors )
        {
            colorId = 0;
        }

        // Set the bar brush to this color.
        barBrush.setColor( color[colorId++] );

        // Create the graph bar.
        GraphBar *bar = g.addGraphBar( x0, y0, x1, y1, barBrush, textPen );

        // Create the bar label.
        int iid = (int) tableRow( row );
        label = xVar->m_itemList->itemName( iid );
        xlabel = 0.5 * (x0 + x1) ;
		ylabel = y0;
        bar->setGraphBarLabel( label, xlabel, ylabel, textFont, textColor, rotation );
    }

    //--------------------------------------------------------------------------
    // 4: Add an output page on which to draw the graph.
    //--------------------------------------------------------------------------

    // Create a separate page for this graph.
    QString text("");
    translate( text, "BpDocument:Graphs:By" );
    label = QString( "%1 %2 %3" )
        .arg( *(yVar->m_label) )
        .arg( text )
        .arg( *(xVar->m_label) );
    startNewPage( label, TocBarGraph );

    // This is how we save the graph and its composer.
    m_composer->graph( g,
        m_pageSize->m_marginLeft
            + m_pageSize->m_bodyWd * property()->real( "graphXOffset" ),
        m_pageSize->m_marginTop
            + m_pageSize->m_bodyHt * property()->real( "graphYOffset" ),
        m_pageSize->m_bodyWd * property()->real( "graphScaleWidth" ),
        m_pageSize->m_bodyHt * property()->real( "graphScaleHeight" )
    );

    // Be polite and stop the composer.
    m_composer->end();
    delete[] color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Controls the composition of all bar and line graphs for the run.
 *  One graph is composed for each output variable.
 *
 *  \param lineGraphs   If TRUE, displays line graphs.
 *                      If FALSE, displays bar graphs.
 *  \param showDialogs  If TRUE, the Run Dialog and possibly the Graph Limits
 *              Dialog will be shown.  This is set FALSE if -auto is specified.
 *
 *  Called only by BpDocument::runWorksheet().
 *
 *  \return TRUE if all the graphs are composed, FALSE if not.
 */

bool BpDocument::composeGraphs( bool lineGraphs, bool showDialogs )
{

    // Bar graph settings
    int    tics = 2;
    double max  = 100.;
    double min  = 0.;
    // Line graph settings
    if ( lineGraphs )
    {
        tics = 11;
        max = tableRow( tableRows()-1 );
        min = tableRow(0);
    }
    GraphAxleParms *xParms = new GraphAxleParms( min, max, tics );
    checkmem( __FILE__, __LINE__, xParms, "GraphAxleParms xParms", 1 );
    if ( lineGraphs && min > 0.
      && property()->boolean( "graphXOriginAtZero" ) )
    {
        xParms->useOrigin();
    }

    // Determine the x-, y-, and z-variables.
    EqVar *xVar = m_eqTree->m_rangeVar[0];
    EqVar *zVar = m_eqTree->m_rangeVar[1];
    EqVar *yVar = 0;

    //--------------------------------------------------------------------------
    // 2: Determine nice axis parameters for the x- and all the y-variables.
    //--------------------------------------------------------------------------

    // Initialize the Y axle for each graph to a nice range
    QPtrList<GraphAxleParms> *yParmsList = new QPtrList<GraphAxleParms>();
    yParmsList->setAutoDelete( true );
    GraphAxleParms *yParms = 0;
    int yid;
    for ( yid = 0;
          yid < tableVars();
          yid++ )
    {
        yVar = tableVar( yid );
        yParms = 0;
        // The yVar must be continuous.
        if ( yVar->isContinuous() )
        {
            // Get Y range
            if ( lineGraphs )
            {
                graphYMinMax( yid, min, max );
            }
            else
            {
                barYMinMax( yid, min, max );
            }
            // Determine and store nice axis parameters
            tics = 11;
            yParms = new GraphAxleParms( min, max, tics );
            checkmem( __FILE__, __LINE__, yParms, "GraphAxleParms yParms", 1 );
            yParmsList->append( yParms );
            // If bar graph, or line graphs must start at origin
            if ( min > 0. )
            {
                if ( ! lineGraphs
                    || property()->boolean( "graphYOriginAtZero" ) )
                {
                    yParms->useOrigin();
                }
            }
            // If min and max are the same, make them different
            if ( min == max )
            {
                yParms->setAxle( min-1., max+1., 3 );
            }
        }
    }

    //--------------------------------------------------------------------------
    // 1: Allow the user to change the axle parameters.
    //--------------------------------------------------------------------------

    if ( showDialogs
      && property()->boolean( "graphYUserRange" ) )
    {
        GraphLimitsDialog *dialog =
            new GraphLimitsDialog( this, yParmsList, "graphLimitsDialog" );
        checkmem( __FILE__, __LINE__, dialog, "GraphLimitsDialog dialog", 1 );
        if ( dialog->exec() == QDialog::Accepted )
        {
            dialog->store();
        }
        delete dialog;  dialog = 0;
    }

    //--------------------------------------------------------------------------
    // 3: Draw each graph.
    //--------------------------------------------------------------------------

    // Set up the progress dialog.
    int step = 0;
    int steps = tableVars();
    QString text(""), button("");
    if ( lineGraphs )
    {
        translate( text, "BpDocument:Graphs:DrawingLineGraphs" );
    }
    else
    {
        translate( text, "BpDocument:Graphs:DrawingBarGraphs" );
    }
    translate( button, "BpDocument:Graphs:Abort" );
    QProgressDialog *progress = new QProgressDialog(
        QString( text ).arg( steps ), button, steps );
    Q_CHECK_PTR( progress );
    progress->setMinimumDuration( 0 );
    progress->setProgress( 0 );

    // Loop for each output variable: one graph is composed per output variable.
    bool result = true;
    for ( yid = 0, yParms = yParmsList->first();
          yid < tableVars();
          yid++ )
    {
        yVar = tableVar( yid );
        // The yVar must be continuous.
        if ( yVar->isContinuous() )
        {
            // Recompute nice Y axis
            //min = ( yParms->m_axleMin < yParms->m_dataMin )
            //    ? yParms->m_axleMin
            //    : yParms->m_dataMin;
            min = yParms->m_axleMin;
            //max = ( yParms->m_axleMax > yParms->m_dataMax )
            //    ? yParms->m_axleMax
            //    : yParms->m_dataMax;
            max = yParms->m_axleMax;
            // If min and max are the same, make them different
            tics = 11;
            if ( min == max )
            {
                yParms->setAxle( min-1., max+1., 3 );
            }
            // Compose this graph.
            if ( lineGraphs )
            {
                composeLineGraph( yid, xVar, yVar, zVar, xParms, yParms );
            }
            else
            {
                composeBarGraph( yid, xVar, yVar, xParms, yParms );
            }
            // Update progress dialog.
            progress->setProgress( ++step );
            qApp->processEvents();
            if ( progress->wasCancelled() )
            {
                result = false;
                break;
            }
            yParms = yParmsList->next();
        }
    }
    // Cleanup and return.
    delete progress;    progress = 0;
    delete xParms;      xParms = 0;
    delete yParmsList;  yParmsList = 0;
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Composes the basic graph decoration including its background,
 *  titles, and axis.
 *
 *  \param g        Pointer to the Graph instance under composition.
 *  \param isLineGraph  TRUE if composing a line graph,
 *                      FALSE if composing a bar graph.
 *  \param xVar     Pointer to the x-axis EqVar.
 *  \param yVar     Pointer to the y-axis EqVar.
 *  \param zVar     Pointer to the curve family EqVar (not used by bar graphs).
 *  \param curves   Number of line curves to calculate and compose.
 *  \param xParms   Pointer to x-axle parameters.
 *  \param yParms   Pointer to y-axle parameters.
 *
 *  \return The function returns nothing.
 */

void BpDocument::composeGraphBasics( Graph *g,
    bool isLineGraph, EqVar *xVar, EqVar *yVar, EqVar *zVar,
    int curves, GraphAxleParms *xParms, GraphAxleParms *yParms )
{
    //--------------------------------------------------------------------------
    // Set the logical fonts and colors here
    //--------------------------------------------------------------------------

    // Axis text fonts.
    QFont  textFont( property()->string( "graphTextFontFamily" ),
                    property()->integer( "graphTextFontSize" ) );
    QPen   textPen( property()->color( "graphTextFontColor" ) );

    QFont  axleFont( textFont );
    QColor axleColor( property()->color( "graphAxleColor" ) );
    QPen   axlePen( axleColor,
                property()->integer( "graphAxleWidth" ), Qt::SolidLine );
    QPen   gridPen( property()->color( "graphGridColor" ),
                property()->integer( "graphGridWidth" ), Qt::SolidLine );

    // Title fonts and colors.
    QFont  subTitleFont( property()->string( "graphSubtitleFontFamily" ),
                    property()->integer( "graphSubtitleFontSize" ) );
    QColor subTitleColor( property()->color( "graphSubtitleFontColor" ) );

    QFont  titleFont( property()->string( "graphTitleFontFamily" ),
                    property()->integer( "graphTitleFontSize" ) );
    QColor titleColor( property()->color( "graphTitleFontColor" ) );

    // Background and canvas colors.
    QBrush worldBg( property()->color( "graphBackgroundColor" ),
                Qt::SolidPattern );
    QBrush canvasBg( "white", SolidPattern );
    QPen   canvasBox( "black", 1, NoPen );

	QString qStr("");

    //--------------------------------------------------------------------------
    // Create the graph canvas, world, and decoration.
    //--------------------------------------------------------------------------

    // Get the x- and y-axle parameters
    double xValMin    = xParms->m_axleMin;
    double xValMax    = xParms->m_axleMax;
    double xMajorStep = xParms->m_majorStep;
    int    xDec       = xParms->m_decimals;

    double yValMin    = yParms->m_axleMin;
    double yValMax    = yParms->m_axleMax;
    double yMajorStep = yParms->m_majorStep;
    int    yDec       = yParms->m_decimals;

    // Graph title and subtitle.
    QString title = m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace();
    //if ( title.isNull() || title.isEmpty() )
    //{
    //  translate( title, "BpDocument:NoRunDescription" );
    //}
	const char *str = title.latin1();
    g->setTitle( title, titleFont, titleColor );

	QString subTitle("");
    if ( property()->boolean( "graphTitleActive" ) )
    {
        QString text("");
        translate( text, "BpDocument:Graphs:By" );
        subTitle = *(yVar->m_label) + "\n" + text + " " + *(xVar->m_label);
		str = subTitle.latin1();
        if ( curves > 1 && isLineGraph && zVar )
        {
            translate( text, "BpDocument:Graphs:And" );
            subTitle += "\n" + text + " " + *(zVar->m_label);
			str = subTitle.latin1();
        }
		str = subTitle.latin1();
    }
	// Added in Build 607 to display the Fire Direction as the subtitle
	// This method returns "Head Fire", "Backing Fire", "Flanking Fire", etc.
	QString fireDir = m_eqTree->m_eqCalc->getSubtitle();
	str = fireDir.latin1();
	if ( fireDir.length() > 0 )
	{
		subTitle += ( subTitle.length() > 0 ) ? "\n" : "";
		subTitle += fireDir;
		str = subTitle.latin1();
	}
	g->setSubTitle( subTitle, subTitleFont, subTitleColor );

	// Use portrait orientation (assuming screen output, not printer).
    g->setCanvasRotation( 0.0 );
    g->setCanvasScale( 1.0, 1.0 );
    // NOTE: to change the graph location or size,
    // we must manipulate the m_composer->graph() args!
    g->setCanvas(
        m_screenSize->m_marginLeft,     // upper left X pixel
        m_screenSize->m_marginTop,      // upper left Y pixel
        m_screenSize->m_bodyWd,         // graph canvas width in pixels
        m_screenSize->m_bodyHt,         // graph canvas height in pixels
        0                               // graph border in pixels
    );

    // Canvas background and box.
    g->setCanvasBg( canvasBg );
    g->setCanvasBox( canvasBox );

    // Make the world coordinate system 20% larger than the data range.
    g->setWorld(
        xValMin - 0.2 * (xValMax-xValMin),  // canvas WORLD left coordinate
        yValMin - 0.2 * (yValMax-yValMin),  // canvas WORLD bottom coordinate
        xValMax + 0.1 * (xValMax-xValMin),  // canvas WORLD right coordinate
        yValMax + 0.1 * (yValMax-yValMin)   // canvas WORLD top coordinate
    );

    // Provide a colored background for the graph area.
    g->setWorldBg(
        xValMin,                        // colored bg WORLD left coordinate
        yValMin,                        // colored bg WORLD bottom coordinate
        xValMax,                        // colored bg WORLD right coordinate
        yValMax,                        // colored bg WORLD top coordinate
        worldBg                         // brush color and pattern
    );

    //--------------------------------------------------------------------------
    // Create a bottom axle with endpoints in WORLD coordinates (not AXLE).
    // This has all elements of an axle EXCEPT a minor grid and subtitle.
    //--------------------------------------------------------------------------

    GraphAxle *b = g->addGraphAxle(
        xValMin,                        // axle's min scale (NOT WORLD) value
        xValMax,                        // axle's max scale (NOT WORLD) value
        axlePen                         // axle line pen (color, width, style)
    );

    b->setWorld(
        xValMin,                        // axle's start WORLD x coordinate
        yValMin,                        // axle's start WORLD y coordinate
        xValMax,                        // axle's end WORLD x coordinate
        yValMin                         // axle's end WORLD y coordinate
    );

    // Line graph bottom axles need labels and tic marks.
    if ( isLineGraph )
    {
        b->setMajorLabels(
            axleFont,                   // label font
            axleColor,                  // font color
            xDec                        // decimal places (-1 mean no label)
        );

        b->setMajorTics(
            GraphAxleBottom,                 // side of axle to draw tic marks
            xValMin,                    // first tic mark location in AXLE units
            xValMax,                    // last tic mark location in AXLE units
            xMajorStep,                 // tic spacing in AXLE units
            0.05 * (yValMax-yValMin),   // tic mark length in Y WORLD units
            axlePen                     // tic pen (color, width, style)
        );

        b->setMinorTics(
            GraphAxleBottom,                 // side of axle to draw tic marks
            xValMin,                    // first tic mark location in AXLE units
            xValMax,                    // last tic mark location in AXLE units
            0.50 * xMajorStep,          // tic spacing in AXLE units
            0.03 * (yValMax-yValMin),   // tic mark length in Y WORLD units
            axlePen                     // tic pen (color, width, style)
        );

        b->setMajorGrid(
            gridPen,                    // grid pen color, width, style
            (yValMax-yValMin)           // length in Y WORLD units !!
        );

        // Don't show the units for ratio variables.
        qStr = *(xVar->m_label) + " " + xVar->displayUnits(true);
        b->setTitle(
            qStr,                       // axle title string
            GraphAxleBottom,                 // axle side to write the string
            axleFont,                   // axle title font
            axleColor                   // axle title color
        );
    }
    // Bar graphs just need the title.
    else
    {
        b->setTitle(
            *(xVar->m_label),           // axle title string
            GraphAxleBottom,                 // axle side to write the string
            axleFont,                   // axle title font
            axleColor                   // axle title color
        );
    }

    //--------------------------------------------------------------------------
    // Create a top axle with endpoints in WORLD coordinates (not AXLE)
    // Same as bottom axle EXCEPT no tic labels or axle label
    //--------------------------------------------------------------------------

    // Line graph top axles need tic marks.
    if ( isLineGraph )
    {
        GraphAxle *t = g->addGraphAxle(
            xValMin,                    // axle's min scale (NOT WORLD) value
            xValMax,                    // axle's max scale (NOT WORLD) value
            axlePen                     // axle line pen (color, width, style)
        );

        t->setWorld(
            xValMin,                    // axle's start WORLD x coordinate
            yValMax,                    // axle's start WORLD y coordinate
            xValMax,                    // axle's end WORLD x coordinate
            yValMax                     // axle's end WORLD y coordinate
        );

        if ( false )
        t->setMajorTics(
            GraphAxleTop,                    // side of axle to draw tic marks
            xValMin,                    // first tic mark location in AXLE units
            xValMax,                    // last tic mark location in AXLE units
            xMajorStep,                 // tic spacing in AXLE units
            0.05 * (yValMax-yValMin),   // tic mark length in WORLD units
            axlePen                     // tic pen (color, width, style)
        );

        if ( false )
        t->setMinorTics(
            GraphAxleTop,                    // side of axle to draw tic marks
            xValMin,                    // first tic mark location in AXLE units
            xValMax,                    // last tic mark location in AXLE units
            0.50 * xMajorStep,          // tic spacing in AXLE units
            0.03 * (yValMax-yValMin),   // tic mark length in WORLD units
            axlePen                     // tic pen (color, width, style)
        );
    }

    //--------------------------------------------------------------------------
    // Create a left axle with endpoints in WORLD coordinates (not AXLE)
    // This has all elements of an axle EXCEPT a minor grid and subtitle
    //--------------------------------------------------------------------------

    GraphAxle *l = g->addGraphAxle(
        yValMin,                        // axle's min scale (NOT WORLD) value
        yValMax,                        // axle's max scale (NOT WORLD) value
        axlePen                         // axle line pen (color, width, style)
    );

    l->setWorld(
        xValMin,                        // axle's start WORLD x coordinate
        yValMin,                        // axle's start WORLD y coordinate
        xValMin,                        // axle's end WORLD x coordinate
        yValMax                         // axle's end WORLD y coordinate
    );

    l->setMajorLabels(
        axleFont,                       // label font
        axleColor,                      // font color
        yDec                            // decimal places (-1 mean no label)
    );

    l->setMajorTics(
        GraphAxleLeft,                       // side of axle to draw tic marks
        yValMin,                        // first tic mark location in AXLE units
        yValMax,                        // last tic mark location in AXLE units
        yMajorStep,                     // tic spacing in AXLE units
        0.04 * (xValMax-xValMin),       // tic mark length in X WORLD units
        axlePen                         // tic pen (color, width, style)
    );

    l->setMinorTics(
        GraphAxleLeft,                       // side of axle to draw tic marks
        yValMin,                        // first tic mark location in AXLE units
        yValMax,                        // last tic mark location in AXLE units
        0.50 * yMajorStep,              // tic spacing in AXLE units
        0.02 * (xValMax-xValMin),       // tic mark length in X WORLD units
        axlePen                         // tic pen (color, width, style)
    );

    l->setMajorGrid(
        gridPen,                        // grid pen color, width, style
        (xValMax-xValMin)               // length in X axis WORLD units !!
    );

    // Don't show the units for fraction or ratio variables.
    qStr = *(yVar->m_label) + " " + yVar->displayUnits(true);
    l->setTitle(
        qStr,                           // axle title string
        GraphAxleLeft,                  // axle side to write the string
        axleFont,                       // axle title font
        axleColor                       // axle title font color
    );

    //--------------------------------------------------------------------------
    // Create a right axle with endpoints in WORLD coordinates (not AXLE)
    // Same as the left axle EXCEPT no tic labels or axle label.
    //--------------------------------------------------------------------------

    GraphAxle *r = g->addGraphAxle(
        yValMin,                        // axle's min scale (NOT WORLD) value
        yValMax,                        // axle's max scale (NOT WORLD) value
        axlePen                         // axle line pen (color, width, style)
    );

    r->setWorld(
        xValMax,                        // axle's start WORLD x coordinate
        yValMin,                        // axle's start WORLD y coordinate
        xValMax,                        // axle's end WORLD x coordinate
        yValMax                         // axle's end WORLD y coordinate
    );

    if ( false )
    r->setMajorTics(
        GraphAxleRight,                      // side of axle to draw tic marks
        yValMin,                        // first tic mark location in AXLE units
        yValMax,                        // last tic mark location in AXLE units
        yMajorStep,                     // tic spacing in AXLE units
        0.04 * (xValMax-xValMin),       // tic mark length in X WORLD units
        axlePen                         // tic pen (color, width, style)
    );

    if ( false )
    r->setMinorTics(
        GraphAxleLeft,                       // side of axle to draw tic marks
        yValMin,                        // first tic mark location in AXLE units
        yValMax,                        // last tic mark location in AXLE units
        0.50 * yMajorStep,              // tic spacing in AXLE units
        0.02 * (xValMax-xValMin),       // tic mark length in X WORLD units
        axlePen                         // tic pen (color, width, style)
    );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes a single line graph with one or more curves.
 *
 *  \param yid      Index of the y-variable.  This is actually an index
 *                  into the m_eqTree->tableVar[] array.
 *  \param xVar     Pointer to the EqVar used for the x-axis.
 *                  This is stored in the EqTree as the row variable,
 *                  and its values are returned via the tableRow(row) function.
 *  \param yVar     Pointer to the EqVar used for the y-axis.
 *                  This is one of the variables in the tableVar[] array.
 *  \param zVar     Pointer to the EqVar used for each curve (line) on the graph.
 *                  For simple graphs, this can be any variable since it
 *                  doesn't really matter.  For multi-valued variables,
 *                  a separate curve is plotted for each zVar value,
 *                  which is returned by the tableCol(col) function.
 *  \param xParms   Pointer to x-axle parameters.
 *  \param yParms   Pointer to y-axle parameters.
 *
 *  Called only by composeGraphs().
 */

void BpDocument::composeLineGraph( int yid, EqVar *xVar, EqVar *yVar,
    EqVar *zVar, GraphAxleParms *xParms, GraphAxleParms *yParms )
{
    //--------------------------------------------------------------------------
    // 1: Set up fonts, pens, and colors used by this graph
    //--------------------------------------------------------------------------

    // Graph fonts
    QFont  textFont( property()->string( "graphTextFontFamily" ),
                    property()->integer( "graphTextFontSize" ) );
    QColor textColor( property()->color( "graphTextFontColor" ) );
    QPen   textPen( textColor );
    // How many colors are requested?
    QString colorName( property()->color( "graphLineColor" ) );
    int colors = 1;
    if ( colorName == "rainbow" )
    {
        colors = property()->integer( "graphRainbowColors" );
    }
    // Allocate number of requested colors.
    QColor *color = new QColor [colors];
    checkmem( __FILE__, __LINE__, color, "QColor color", colors );
    int colorId = 0;
    // If only 1 color...
    if ( colors == 1 )
    {
        color[0].setNamedColor( colorName );
    }
    // else if rainbow colors are requested...
    else
    {
        // todo - add some code to check conflicts with graphBackgroundColor
        // and graphGridColor (if graphGridWidth > 0 ).
        int hue = 0;
        for ( colorId = 0;
              colorId < colors;
              colorId++ )
        {
            color[colorId].setHsv( hue, 255, 255);
            hue += 360 / colors;
        }
        colorId = 0;
    }
    // Set up line width and color
    int lineWidth = property()->integer( "graphLineWidth" );
    QPen pen( color[0], lineWidth, SolidLine );

    //--------------------------------------------------------------------------
    // 2: Create the graph and add its curves.
    //--------------------------------------------------------------------------

    // Initialize graph and variables
    Graph      g;
    GraphLine *line[graphMaxLines];
    double     l_x[graphMaxSteps];
    double     l_y[graphMaxSteps];
    int        curves = ( tableCols() < graphMaxLines )
                  ? ( tableCols() )
                  : ( graphMaxLines );
    int        points = tableRows();
    int        vStep  = tableCols() * tableVars();

    // Loop for each zVar family curve value in this graph (or at least once!).
    // Note that zVar count is in tableCols(), e.g. each column stores a curve,
    // and zVar values are in tableCol( col ).
    int col, vid;
    for ( col = 0;
          col < curves;
          col++ )
    {
        // tableVal[] offset of first y-value for this curve.
        vid = yid + col * tableVars();
        // Set up the y[point] array for this curve.
        // Note number of points is in tableRows() and
        // point x values are in tableRow( point ).
        for ( int point = 0;
              point < points;
              point++ )
        {
            l_x[point] = tableRow( point );
            l_y[point] = tableVal( vid );
            vid += vStep;
        }
        // If we're out of colors, start over.
        if ( colorId >= colors )
        {
            colorId = 0;
        }
        // Create a graph line (with its own copy of the data).
        pen.setColor( color[colorId++] );
        line[col] = g.addGraphLine( points, l_x, l_y, pen );
    } // Next z-variable curve.

    //--------------------------------------------------------------------------
    // 3: Add curve labels if there is more than 1 curve.
    //--------------------------------------------------------------------------

    QString label;
    if ( curves > 1 )
    {
        colorId = 0;

#define GRAPH_LABEL_METHOD_1

#ifdef GRAPH_LABEL_METHOD_1
        // Label x array index step size between labels
        int j1 = points / curves;
        if ( j1 < 1 )
        {
            j1 = 1;
        }
        // Label x array index offset
        int j0 = j1 / 2;
#endif
        double xLabel, yLabel;
        int idx;
        // Loop for each z-variable curve.
        for ( col = 0;
              col < curves;
              col++ )
        {
            // Get a new color for the curve.
            if ( colorId >= colors )
            {
                colorId = 0;
            }
            // Set the curve label.
            if ( zVar->isDiscrete() )
            {
                int iid = (int) tableCol( col );
                label = zVar->m_itemList->itemName( iid );
            }
            else if ( zVar->isContinuous() )
            {
				int decimals = zVar->m_displayDecimals;
                label.sprintf( "%1.*f",
                    zVar->m_displayDecimals, tableCol( col ) );
				// Remove all trailing zeros
				while ( decimals && label.endsWith( "0" ) )
				{
					label = label.left( label.length()-1 );
					decimals--;
				}
            }

#ifdef GRAPH_LABEL_METHOD_1
            // Determine an x-axis index for the label position.
            idx = ( j0 + col * j1 ) % points;
            xLabel = line[col]->m_x[idx];
            yLabel = line[col]->m_y[idx];
#endif

#ifdef GRAPH_LABEL_METHOD_2
            // NEW LABEL POSITIONING METHOD STARTS HERE
            // Find the x position where this variable has the maximum y
            // clearance
            // Loop for each x value for this curve
            idx = 0;
            double dMax = 0;
            double dir  = 1.;
            double yMin = yParms->m_axleMin;
            double yMax = yParms->m_axleMax;
            for ( int row=1; row<tableRows()-1; row++ )
            {
                // Find vertical clearance above and below this x point
                double y0 = m_eqTree->getResult( row, col, yid );
                // Don't consider locations outside the viewport
                if ( y0 < yMin || y0 > yMax )
                {
                    continue;
                }
                double below = y0 - yMin;
                double above = yMax - y0;
                double dist, y1;
                // Loop for each family member curve
                for ( int c=0; c<curves; c++ )
                {
                    // Skip self
                    if ( c == col )
                    {
                        continue;
                    }
                    y1 = m_eqTree->getResult( row, c, yid );
                    y1 = ( y1 < yMax ) ? y1 : yMax;
                    y1 = ( y1 > yMin ) ? y1 : yMin;
                    // Shrink vertical clearance above and below this x point?
                    if ( y0 <= y1 )
                    {
                        dist = y1 - y0;
                        above = ( dist < above ) ? dist : above;
                    }
                    else
                    {
                        dist = y0 - y1;
                        below = ( dist < below ) ? dist : below;
                    }
                }
                // Is this the maximum vertical clearance so far?
                if ( above + below > dMax )
                {
                    dMax = above + below;
                    idx  = row;
                    dir  = ( above > below ) ? 1. : -1;
                }
            }
            xLabel = line[col]->m_x[idx];
            double offset = dir * 0.02 * ( yMax - yMin );
            yLabel = line[col]->m_y[idx] + offset;
#endif
            // Set the label text, font, color, and position.
            line[col]->setGraphLineLabel( label, xLabel, yLabel, textFont,
                QColor( color[colorId++] ) );
        } // Next curve.

        // Add a z-variable label to the graph.
        label = *(zVar->m_label);
        if ( zVar->isContinuous() )
        {
            label = *(zVar->m_label) + "\n" + zVar->displayUnits(true);
        }
        g.setMultipleCurveLabel( label );
    }

    //--------------------------------------------------------------------------
    // 4: Compose the graph and add an output page to draw it onto
    //--------------------------------------------------------------------------

    composeGraphBasics( &g, true, xVar, yVar, zVar, curves, xParms, yParms );

    // Create a separate output page for this graph.
    QString text("");
    translate( text, "BpDocument:Graphs:By" );
    label = *(yVar->m_label) + " " + text + " " + *(xVar->m_label);
    if ( curves > 1 )
    {
        translate( text, "BpDocument:Graphs:And" );
        label += " " + text + " " + *(zVar->m_label);
    }
    startNewPage( label, TocLineGraph );

    // This is how we save the graph and its composer.
    m_composer->graph( g,
        m_pageSize->m_marginLeft
            + m_pageSize->m_bodyWd * property()->real( "graphXOffset" ),
        m_pageSize->m_marginTop
            + m_pageSize->m_bodyHt * property()->real( "graphYOffset" ),
        m_pageSize->m_bodyWd * property()->real( "graphScaleWidth" ),
        m_pageSize->m_bodyHt * property()->real( "graphScaleHeight" )
    );

    // Be polite and stop the composer.
    m_composer->end();
    delete[] color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the minimum and maximum data point Y values
 *  of all curves of a line graph.
 *
 *  \param yMin Reference to the returned minimum y data value.
 *  \param yMax Reference to the returned maximum y data value.
 *
 *  Called only by BpDocument::composeGraphs() in preparation for
 *  determining nice axle parameters.
 */

void BpDocument::graphYMinMax( int yid, double &yMin, double &yMax )
{
    // Store the step size and number of curves
    int vStep  = tableCols() * tableVars();
    int curves = ( tableCols() < graphMaxLines )
               ? ( tableCols() )
               : graphMaxLines;
    bool firstOne = true;
    double val;
    int vid;
    yMin = yMax = 0.;

    // Loop for each zVar family curve value in this graph (or at least once!).
    // Note that zVar count is in tableCols(), e.g. each column stores a curve,
    // and zVar values are in tableCol( col ).
    for ( int col = 0;
          col < curves;
          col++ )
    {
        // tableVal( vid ) offset of first y-value for this curve.
        vid = yid + col * tableVars();
        // Examine every y point for this curve.
        // Note number of points is in tableRows().
        for ( int point = 0;
              point < tableRows();
              point++ )
        {
            val = tableVal( vid );
            vid += vStep;
            // If this is the first point, initialize yMin and yMax.
            if ( firstOne )
            {
                yMin = yMax = val;
                firstOne = false;
            }
            // otherwise accumulate yMin and yMax.
            else
            {
                yMin = ( val < yMin ) ? val : yMin;
                yMax = ( val > yMax ) ? val : yMax;
            }
        }
    } // Next z-variable curve.
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposegraphs.cpp
//------------------------------------------------------------------------------
