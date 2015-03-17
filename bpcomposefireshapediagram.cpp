//------------------------------------------------------------------------------
/*! \file bpcomposefireshapediagram.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Composes the Size Module's fire shape diagrams.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "property.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qapplication.h>
#include <qpen.h>
#include <qprogressdialog.h>

//------------------------------------------------------------------------------
/*! \brief Composes the Size Module's fire shape diagram.
 */

void BpDocument::composeFireShapeDiagram( void )
{
    // Does the user even want these figures?
    if ( ! property()->boolean( "sizeModuleActive" )
      || ! property()->boolean( "sizeCalcFireShapeDiagram" ) )
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
        property()->string( "sizeFireShapeTextFontFamily" ),
        property()->integer( "sizeFireShapeTextFontSize" ) );
    QPen textPen( property()->color( "sizeFireShapeTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    // Value font is used for numbers and other results in the diagram
    QFont valueFont(
        property()->string( "sizeFireShapeValueFontFamily" ),
        property()->integer( "sizeFireShapeValueFontSize" ) );
    QPen valuePen( property()->color( "sizeFireShapeValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    // Background color and frame
    QBrush bgBrush(
        property()->color( "sizeFireShapeBgColor" ),
        Qt::SolidPattern );
    QPen framePen(
        property()->color( "sizeFireShapeFrameColor" ),
        property()->integer( "sizeFireShapeFrameWidth" ),
        Qt::SolidLine );

    // Fire shape perimeter and fill
    QPen perimPen(
        property()->color( "sizeFireShapePerimColor" ),
        property()->integer( "sizeFireShapePerimWidth" ),
        Qt::SolidLine );
    QBrush fireBrush(
        property()->color( "sizeFireShapeFireColor" ),
        Qt::SolidPattern );
    QPen spreadPen(
        property()->color( "sizeFireShapeSpreadColor" ),
        property()->integer( "sizeFireShapeSpreadWidth" ),
        Qt::SolidLine );

    // Wind and slope lines
    QPen slopePen(
        property()->color( "sizeFireShapeSlopeColor"),
        property()->integer( "sizeFireShapeSlopeWidth" ),
        Qt::DotLine );
    QPen windPen(
        property()->color( "sizeFireShapeWindColor"),
        property()->integer( "sizeFireShapeWindWidth" ),
        Qt::DashLine );

    // Diagram legend
    QFont legendFont(
        property()->string( "sizeFireShapeLegendFontFamily" ),
        property()->integer( "sizeFireShapeLegendFontSize" ) );
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

    // Open the result file
    QString resultFile = m_eqTree->m_resultFile;
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( resultFile.latin1(), "r" ) ) )
    // This code block should never be executed!
    {
        translate( text, "BpDocument:Diagrams:UnableToOpenTempFile",
            resultFile );
        error( text );
        return;
    }

    // Store configuration properties
    bool degreesUpslope = property()->boolean( "surfaceConfDegreesWrtUpslope" );
    bool degreesNorth   = property()->boolean( "surfaceConfDegreesWrtNorth" );
    bool windDirUpslope = property()->boolean( "surfaceConfWindDirUpslope" );
    bool windDirInput   = property()->boolean( "surfaceConfWindDirInput" );
    bool spreadAtVector = property()->boolean( "surfaceConfSpreadDirInput" );
    bool surfaceModuleActive = property()->boolean( "surfaceModuleActive" );

    // Pointers to supporting information variables
    EqVar *rowVar   = m_eqTree->m_rangeVar[0];
    EqVar *colVar   = m_eqTree->m_rangeVar[1];

    EqVar *headVar  = m_eqTree->m_eqCalc->vSurfaceFireDistAtHead;
    EqVar *lengVar  = m_eqTree->m_eqCalc->vSurfaceFireLengDist;
    EqVar *widthVar = m_eqTree->m_eqCalc->vSurfaceFireWidthDist;

    char *headName  = "vSurfaceFireDistAtHead";
    char *lengName  = "vSurfaceFireLengDist";
    char *widthName = "vSurfaceFireWidthDist";

    EqVar *areaVar  = m_eqTree->m_eqCalc->vSurfaceFireArea;
    EqVar *backVar  = m_eqTree->m_eqCalc->vSurfaceFireDistAtBack;
    EqVar *perimVar = m_eqTree->m_eqCalc->vSurfaceFirePerimeter;
    EqVar *ratioVar = m_eqTree->m_eqCalc->vSurfaceFireLengthToWidth;
    EqVar *timeVar  = m_eqTree->m_eqCalc->vSurfaceFireElapsedTime;

    char *areaName  = "vSurfaceFireArea";
    char *backName  = "vSurfaceFireDistAtBack";
    char *timeName  = "vSurfaceFireElapsedTime";
    char *perimName = "vSurfaceFirePerimeter";

    // Get/use/display these variables only if linked to Surface Module
    EqVar *rosDirVar, *maxDirVar, *windVar, *aspectVar;
    char  *rosDirName, *maxDirName, *aspectName, *windName;
    if ( surfaceModuleActive )
    {
        if ( degreesNorth )
        {
            maxDirVar  = m_eqTree->m_eqCalc->vSurfaceFireMaxDirFromNorth;
            maxDirName = "vSurfaceFireMaxDirFromNorth";
            aspectVar  = m_eqTree->m_eqCalc->vSiteAspectDirFromNorth;
            aspectName = "vSiteAspectDirFromNorth";
        }
        else if ( degreesUpslope )
        {
            maxDirVar  = m_eqTree->m_eqCalc->vSurfaceFireMaxDirFromUpslope;
            maxDirName = "vSurfaceFireMaxDirFromUpslope";
            aspectName = "not used";
            aspectVar  = 0;
        }
        if ( windDirInput )
        {
            if ( degreesNorth )
            {
                windVar  = m_eqTree->m_eqCalc->vWindDirFromNorth;
                windName = "vWindDirFromNorth";
            }
            else if ( degreesUpslope )
            {
                windVar  = m_eqTree->m_eqCalc->vWindDirFromUpslope;
                windName = "vWindDirFromUpslope";
            }
        }
        else if ( windDirUpslope )
        {
            windVar  = 0;
            windName = "not used";
        }
        if ( spreadAtVector )
        {
            if ( degreesNorth )
            {
                rosDirVar  = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromNorth;
                rosDirName = "vSurfaceFireVectorDirFromNorth";
            }
            else if ( degreesUpslope )
            {
                rosDirVar  = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromUpslope;
                rosDirName = "vSurfaceFireVectorDirFromUpslope";
            }
        }
        else
        {
            rosDirVar  = 0;
            rosDirName = "not used";
        }
    }
    else
    {
        maxDirVar  = 0;
        maxDirName = "not used";
        windVar    = 0;
        windName   = "not used";
        aspectVar  = 0;
        aspectName = "not used";
        rosDirVar  = 0;
        rosDirName = "not used";
    }

    // Always need length, width, and head distance to draw the diagrams
    double *leng  = new double[ cells ];
    checkmem( __FILE__, __LINE__, leng, "double leng", cells );
    double *l_width = new double[ cells ];
    checkmem( __FILE__, __LINE__, l_width, "double l_width", cells );
    double *head  = new double[ cells ];
    checkmem( __FILE__, __LINE__, head, "double head", cells );
    // Only maxDir if linked to SURFACE
    double *maxDir = 0;
    if ( maxDirVar )
    {
        maxDir = new double[ cells ];
        checkmem( __FILE__, __LINE__, maxDir, "double maxDir", cells );
    }
    // Only need wind[] array if linked to SURFACE
    // and wind is a direct input (e.g., not upslope wind)
    double *wind = 0;
    if ( windVar )
    {
        wind = new double[ cells ];
        checkmem( __FILE__, __LINE__, wind, "double wind", cells );
    }
    // Only need aspect[] array if linked to SURFACE
    // and directions are wrt North (.e.g, not wrt upslope)
    double *aspect = 0;
    if ( aspectVar )
    {
        aspect = new double[ cells ];
        checkmem( __FILE__, __LINE__, aspect, "double aspect", cells );
    }
    // Only need rosDir[] array if linked to SURFACE
    // and user enters spread directions on worksheet
    double *rosDir = 0;
    if ( rosDirVar )
    {
        rosDir = new double[ cells ];
        checkmem( __FILE__, __LINE__, rosDir, "double rosDir", cells );
    }
    // Only need these arrays if they are requested as output variables
    double *area = 0;
    if ( areaVar->m_isUserOutput )
    {
        area = new double[ cells ];
        checkmem( __FILE__, __LINE__, area, "double area", cells );
    }
    double *perim = 0;
    if ( perimVar->m_isUserOutput )
    {
        perim = new double[ cells ];
        checkmem( __FILE__, __LINE__, perim, "double perim", cells );
    }
    // Elapsed time is always display in the title
    double *etime = 0;
    if ( timeVar->m_isUserInput )
    {
        etime = new double[ cells ];
        checkmem( __FILE__, __LINE__, etime, "double etime", cells );
    }
    double *back = 0;
    if ( backVar->m_isUserOutput )
    {
        back = new double[ cells ];
        checkmem( __FILE__, __LINE__, back, "double back", cells );
    }

    // Read and store up to 12 values per cell (head, leng, width, maxDir,
    // rosDir, wind, aspect, area, perim, etime, and back)
    char   buffer[1024], varName[128];
    int    row, col, cell;
    double value;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        if ( strncmp( buffer, "CELL", 4 ) == 0 )
        {
            if ( strstr( buffer, headName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                head[ cell ] = value;
            }
            else if ( strstr( buffer, lengName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                leng[ cell ] = value;
            }
            else if ( strstr( buffer, widthName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                l_width[ cell ] = value;
            }
            else if ( strstr( buffer, maxDirName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                maxDir[ cell ] = value;
            }
            else if ( wind && strstr( buffer, windName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                wind[ cell ] = value;
            }
            else if ( aspect && strstr( buffer, aspectName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                aspect[ cell ] = value;
            }
            else if ( rosDir && strstr( buffer, rosDirName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                rosDir[ cell ] = value;
            }
            else if ( area && strstr( buffer, areaName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                area[ cell ] = value;
            }
            else if ( perim && strstr( buffer, perimName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                perim[ cell ] = value;
            }
            else if ( etime && strstr( buffer, timeName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                etime[ cell ] = value;
            }
            else if ( back && strstr( buffer, backName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                back[ cell ] = value;
            }
        }
    }
    fclose( fptr );

    // Determine maximum values for scaling purposes
    double maxLeng = leng[0];
    double maxWidth = l_width[0];
    double maxHead = head[0];
    for ( cell=1; cell<cells; cell++ )
    {
        maxLeng  = ( leng[cell] > maxLeng )
                   ? leng[ cell ]
                   : maxLeng;
        maxWidth = ( l_width[ cell ] > maxWidth )
                   ? l_width[ cell ]
                   : maxWidth;
        maxHead  = ( head[ cell ] > maxHead )
                   ? head[ cell ]
                   : maxHead;
    }

    // Determine number of diagram panes on each page.
    int paneCols = property()->integer( "sizeFireShapeDiagramCols" );
    int paneRows = property()->integer( "sizeFireShapeDiagramRows" );
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
    int topLines = 1;                           // Y axis top label
    if ( rowVar ) topLines++;
    if ( colVar ) topLines++;

    // Each diagram may have 0-9 bottom lines.
    int botLines = 2;                           // Y axis bottom label
    if ( lengVar->m_isUserOutput && lengVar != rowVar && lengVar != colVar )
        botLines++;
    if ( widthVar->m_isUserOutput && widthVar != rowVar && widthVar != colVar )
        botLines++;
    if ( headVar->m_isUserOutput && headVar != rowVar && headVar != colVar )
        botLines++;
    if ( ratioVar->m_isUserOutput && ratioVar != rowVar && ratioVar != colVar )
        botLines++;
    if ( area && areaVar != rowVar && areaVar != colVar )
        botLines++;
    if ( perim && perimVar != rowVar && perimVar != colVar )
        botLines++;
    if ( etime && timeVar != rowVar && timeVar != colVar )
        botLines++;
    if ( back && backVar != rowVar && backVar != colVar )
        botLines++;
    //if ( rosDir )  botLines++;
    if ( surfaceModuleActive && maxDirVar->m_isUserOutput
     && maxDirVar != rowVar && maxDirVar != colVar )
        botLines++;

    // Determine space remaining for figure
    double figTop = topLines * textHt;          // figure top text margin
    double figBot = botLines * textHt;          // figure bottom text margin
    double figHt  = paneHt - figTop - figBot;   // figure drawing space

    // Determine location and center of each diagram pane on the page.
    double *top  = new double[ panes ];
    checkmem( __FILE__, __LINE__, top, "double top", panes );
    double *left = new double[ panes ];
    checkmem( __FILE__, __LINE__, left, "double left", panes );
    double *x0   = new double[ panes ];
    checkmem( __FILE__, __LINE__, x0, "double x0", panes );
    double *y0   = new double[ panes ];
    checkmem( __FILE__, __LINE__, y0, "double y0", panes );
    int pane;
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
            x0[pane] = left[pane] + 0.5 * paneWd;
            y0[pane] = top[pane] + figTop + 0.5 * figHt;
        }
    }
    // Get translated text
    QString north(""), south("");
    translate( north, "BpDocument:Diagrams:North" );
    translate( south, "BpDocument:Diagrams:South" );
    QString upSlope(""), downSlope("");
    translate( upSlope, "BpDocument:Diagrams:UpSlope" );
    translate( downSlope, "BpDocument:Diagrams:DownSlope" );
    QString upWind(""), downWind("");
    translate( upWind, "BpDocument:Diagrams:UpWind" );
    translate( downWind, "BpDocument:Diagrams:DownWind" );
    QString title(""), windVector("");
    translate( title, "BpDocument:Diagrams:FireShapeTitle" );
    translate( windVector, "BpDocument:Diagrams:DirOfWindVector" );
    QString desc( m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace() );

    // Loop for each PAGE across and down.
    double xPos, yPos, ellipseHt, ellipseWd, deg;
    double arrow = 0.5 * textHt;
    QString qStr1, qStr2;
    int thisPage = 1;
    int datum, datumRow, datumCol, iid;
    int center = Qt::AlignVCenter|Qt::AlignHCenter;
    for ( int pageRow = 0;
          pageRow < pageRows;
          pageRow++ )
    {
        for ( int pageCol = 0;
              pageCol < pageCols;
              pageCol++, thisPage++ )
        {
            // Table title indicates the table portion.
            translate( qStr1, "BpDocument:Diagrams:FireShapePage",
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
                    // Determine ellipse dimensions
                    // If vSurfaceFireMaxDirFromNorth is an output variable,
                    // ellipse height is half the figure space;
                    // otherwise it can use the entire figure space
                    // since it doesn't have to rotate around the origin
                    ellipseHt = ( maxLeng < 0.00001 )
                        ? 0.0
                        : 0.5 * figHt * leng[datum] / maxLeng;
                    ellipseWd = ( leng[datum] < 0.00001 )
                        ? 0.0
                        : ellipseHt * l_width[datum] / leng[datum];

                    // Draw the fire coordinate system
                    // If vSurfaceFireMaxDirFromNorth is an output variable,
                    // center the coordinate system in the middle of the figure
                    m_composer->pen( textPen ); // use tableTextFontColor
                    m_composer->line(
                        left[pane] + textHt,            y0[pane],
                        left[pane] + paneWd - textHt,   y0[pane] );
                    m_composer->line(
                        x0[pane],   top[pane] + figTop,
                        x0[pane],   top[pane] + figTop + figHt );

                    // Draw axis labels
                    if ( surfaceModuleActive )
                    {
                        if ( degreesNorth )
                        {
                            qStr1 = north;
                            qStr2 = south;
                        }
                        else if ( degreesUpslope )
                        {
                            qStr1 = upSlope;
                            qStr2 = downSlope;
                        }
                    }
                    else
                    {
                        qStr1 = upWind;
                        qStr2 = downWind;
                    }
                    m_composer->pen( titlePen );    // use tableTitleFontColor
                    m_composer->text(
                        left[pane],     top[pane] + ( topLines - 1 ) * textHt,
                        paneWd,         textHt,
                        center,         qStr1 );
                    m_composer->text(
                        left[pane],     top[pane] + figTop + figHt,
                        paneWd,         textHt,
                        center,         qStr2 );

                    // Draw the wind flow direction vector
                    //deg = ( wind )
                    //      ? wind[datum]
                    //      : 180.0;
                    //if ( degreesNorth )
                    //{
                    //    deg += ( deg < 180 )
                    //           ? 180.
                    //           : -180. ;
                    //}
                    if ( surfaceModuleActive )
                    {
                        if ( windDirInput )
                        {
                            deg = wind[datum];
                            qStr1 = *(windVar->m_label);
                        }
                        else if ( windDirUpslope )
                        {
                            if ( degreesUpslope )
                            {
                                deg = 0;
                            }
                            else if ( degreesNorth )
                            {
                                if ( ( deg = aspect[datum] - 180 ) < 0. )
                                {
                                    deg += 360.;
                                }
                            }
                            qStr1 = windVector;
                        }
                        yPos = top[pane] + figTop + 0.10 * figHt;
                        m_composer->font( legendFont );
                        m_composer->pen( windPen );
                        m_composer->rotateLine(
                            x0[pane],           y0[pane],
                            x0[pane],           y0[pane],
                            x0[pane],           yPos,
                            deg );
                        if ( windDirInput && degreesNorth )
                        {
                            // Draw arrow head in reverse direction
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos + 2 * arrow,
                                x0[pane] - arrow,   yPos + arrow,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos + 2 * arrow,
                                x0[pane] + arrow,   yPos + arrow,
                                deg );
                        }
                        else
                        {
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] - arrow,   yPos + arrow,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] + arrow,   yPos + arrow,
                                deg );
                        }
                        // Legend
                        yPos = top[pane] + figTop + figHt + 1.5 * textHt;
                        m_composer->line(
                            left[pane] + 1.0 * arrow,   yPos,
                            left[pane] + 6.0 * arrow,   yPos );
                        m_composer->line(
                            left[pane] + 5.5 * arrow,   yPos - 0.5 * arrow,
                            left[pane] + 6.0 * arrow,   yPos );
                        m_composer->line(
                            left[pane] + 5.5 * arrow,   yPos + 0.5 * arrow,
                            left[pane] + 6.0 * arrow,   yPos );
                        m_composer->pen( textPen );
                        m_composer->text(
                            left[pane] + 1.0 * arrow,   yPos + 0.75 * legendHt,
                            qStr1 );

                        // Draw the spread direction vector
                        if ( rosDir )
                        {
                            deg = rosDir[ datum ];
                            yPos = top[pane] + figTop + 0.05 * figHt;
                            m_composer->font( legendFont );
                            m_composer->pen( spreadPen );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] - arrow,   yPos + arrow,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] + arrow,   yPos + arrow,
                                deg );
                            // Legend
                            qStr1 = *(rosDirVar->m_label);
                            yPos = top[pane] + figTop + figHt + 0.5 * textHt;
                            m_composer->line(
                                left[pane] + 1.0 * arrow,   yPos,
                                left[pane] + 6.0 * arrow,   yPos );
                            m_composer->line(
                                left[pane] + 5.5 * arrow,   yPos - 0.5 * arrow,
                                left[pane] + 6.0 * arrow,   yPos );
                            m_composer->line(
                                left[pane] + 5.5 * arrow,   yPos + 0.5 * arrow,
                                left[pane] + 6.0 * arrow,   yPos );
                            m_composer->pen( textPen );
                            m_composer->text(
                                left[pane] + 1.0 * arrow,   yPos + 0.75 * legendHt,
                                qStr1 );
                        }

                        // Draw the upslope vector if using compass coordinate system
                        if ( aspectVar )
                        {
                            deg = ( aspect )
                                  ? aspect[datum]
                                  : 180.;
                            deg += ( deg < 180 )
                                   ? 180.
                                   : -180. ;
                            yPos = top[pane] + figTop + 0.20 * figHt;
                            m_composer->font( legendFont );
                            m_composer->pen( slopePen );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] - arrow,   yPos + arrow,
                                deg );
                            m_composer->rotateLine(
                                x0[pane],           y0[pane],
                                x0[pane],           yPos,
                                x0[pane] + arrow,   yPos + arrow,
                                deg );
                            // Legend
                            yPos = top[pane] + figTop + figHt - 0.50 * textHt;
                            m_composer->line(
                                left[pane] + 1.0 * arrow, yPos,
                                left[pane] + 6.0 * arrow, yPos );
                            m_composer->line(
                                left[pane] + 5.5 * arrow, yPos - 0.50 * arrow,
                                left[pane] + 6.0 * arrow, yPos );
                            m_composer->line(
                                left[pane] + 5.5 * arrow, yPos + 0.50 * arrow,
                                left[pane] + 6.0 * arrow, yPos );
                            m_composer->pen( textPen );
                            m_composer->text(
                                left[pane] + 1.0 * arrow, yPos + 0.75 * legendHt,
                                upSlope );
                        }
                    }   // if ( surfaceModuleActive )

                    // Upper-left corner of the unrotated fire ellipse
                    xPos = x0[pane] - 0.5 * ellipseWd;
                    yPos = ( leng[datum] < 0.00001 )
                        ? y0[pane]
                        : y0[pane] - ( ellipseHt * head[datum] / leng[datum] );

                    // Draw the fire ellipse rotated about the origin
                    m_composer->pen( perimPen );
                    deg = ( maxDir )
                          ? maxDir[datum]
                          : 0.;
                    m_composer->rotateEllipse( x0[pane], y0[pane],
                        xPos, yPos, ellipseWd, ellipseHt, deg );

                    // Display any requested user outputs
                    yPos = top[pane] + figTop + figHt + textHt + textHt;
                    m_composer->font( textFont );
                    m_composer->pen( valuePen );    // use tableTextValueColor
                    double vert = 0.9 * valueHt;
                    if ( areaVar->m_isUserOutput
                      && areaVar != rowVar
                      && areaVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(areaVar->m_label) )
                            .arg( area[datum], 0, 'f',
                                areaVar->m_displayDecimals )
                            .arg( areaVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( perimVar->m_isUserOutput
                      && perimVar != rowVar
                      && perimVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(perimVar->m_label) )
                            .arg( perim[datum], 0, 'f',
                                perimVar->m_displayDecimals )
                            .arg( perimVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( ratioVar->m_isUserOutput
                      && ratioVar != rowVar
                      && ratioVar != colVar )
                    {
                        double l_x = ( l_width[datum] < 0.00001 )
                            ? 0.0
                            : leng[datum] / l_width[datum];
                        qStr1 = QString( "%1    %2" )
                            .arg( *(ratioVar->m_label) )
                            .arg( l_x, 0, 'f', ratioVar->m_displayDecimals );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( headVar->m_isUserOutput
                      && headVar != rowVar
                      && headVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(headVar->m_label) )
                            .arg( head[datum], 0, 'f',
                                headVar->m_displayDecimals )
                            .arg( headVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( backVar->m_isUserOutput
                      && backVar != rowVar
                      && backVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(backVar->m_label) )
                            .arg( back[datum], 0, 'f',
                                backVar->m_displayDecimals )
                            .arg( backVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( lengVar->m_isUserOutput
                      && lengVar != rowVar
                      && lengVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(lengVar->m_label) )
                            .arg( leng[datum], 0, 'f',
                                lengVar->m_displayDecimals )
                            .arg( lengVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( widthVar->m_isUserOutput
                      && widthVar != rowVar
                      && widthVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(widthVar->m_label) )
                            .arg( l_width[datum], 0, 'f',
                                widthVar->m_displayDecimals )
                            .arg( widthVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( timeVar->m_isUserInput
                      && timeVar != rowVar
                      && timeVar != colVar )
                    {
                        qStr1 = QString( "%1    %2 %3" )
                            .arg( *(timeVar->m_label) )
                            .arg( etime[datum], 0, 'f',
                                timeVar->m_displayDecimals )
                            .arg( timeVar->m_displayUnits );
                        m_composer->text(
                            left[pane],     yPos,
                            paneWd,         textHt,
                            center,         qStr1 );
                        yPos += vert;
                    }
                    if ( surfaceModuleActive )
                    {
                        if ( maxDirVar->m_isUserOutput
                          && maxDirVar != rowVar
                          && maxDirVar != colVar )
                        {
                            qStr1 = QString( "%1    %2 %3" )
                                .arg( *(maxDirVar->m_label) )
                                .arg( maxDir[datum], 0, 'f',
                                    maxDirVar->m_displayDecimals )
                                .arg( maxDirVar->m_displayUnits );
                            m_composer->text(
                                left[pane],     yPos,
                                paneWd,         textHt,
                                center,         qStr1 );
                            yPos += vert;
                        }
                    }   // if ( surfaceModuleActive )
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

    // Clean up and return
    delete progress;    progress = 0;
    delete[] head;      head = 0;
    delete[] leng;      leng = 0;
    delete[] l_width;   l_width = 0;
    delete[] area;      area = 0;
    delete[] perim;     perim = 0;
    delete[] etime;     etime = 0;
    delete[] back;      back = 0;
    delete[] maxDir;    maxDir = 0;
    delete[] rosDir;    rosDir = 0;
    delete[] wind;      wind = 0;
    delete[] aspect;    aspect = 0;
    delete[] top;       top = 0;
    delete[] left;      left = 0;
    delete[] x0;        x0 = 0;
    delete[] y0;        y0 = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposefireshapediagram.cpp
//------------------------------------------------------------------------------

