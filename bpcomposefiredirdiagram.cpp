//------------------------------------------------------------------------------
/*! \file bpcomposefiredirdiagram.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Composes the Surface Module's wind/slope/fire spread direction
 *  diagrams.
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
/*! \brief Composes the Surface Module's wind/slope/fire spread direction
 *  diagram.
 */

void BpDocument::composeFireMaxDirDiagram( void )
{
    // Does the user even want these figures?
    if ( ! property()->boolean( "surfaceModuleActive" )
      || ! property()->boolean( "surfaceCalcFireMaxDirDiagram" ) )
    {
        return;
    }
    // Allocate ros and hpua data arrays
    int rows  = tableRows();
    int cols  = tableCols();
    int cells = rows * cols;

    // Set up the progress dialog.
    int step = 0;
    int steps = cells;
    QString text(""), button("");
    translate( text, "BpDocument:Diagrams:DrawingDirections",
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
        property()->string( "surfaceFireMaxDirDiagramTextFontFamily" ),
        property()->integer( "surfaceFireMaxDirDiagramTextFontSize" ) );
    QPen textPen( property()->color( "surfaceFireMaxDirDiagramTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    // Value font is used for numbers and other results in the diagram
    QFont valueFont(
        property()->string( "surfaceFireMaxDirDiagramValueFontFamily" ),
        property()->integer( "surfaceFireMaxDirDiagramValueFontSize" ) );
    QPen valuePen( property()->color( "surfaceFireMaxDirDiagramValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    // Background color and frame
    QBrush bgBrush(
        property()->color( "surfaceFireMaxDirDiagramBgColor" ),
        Qt::SolidPattern );
    QPen framePen(
        property()->color( "surfaceFireMaxDirDiagramFrameColor" ),
        property()->integer( "surfaceFireMaxDirDiagramFrameWidth" ),
        Qt::SolidLine );

    // Fire, slope and wind lines
    QPen maxDirPen(
        property()->color( "surfaceFireMaxDirDiagramFireColor" ),
        property()->integer( "surfaceFireMaxDirDiagramFireWidth" ),
        Qt::SolidLine );
    QPen spreadPen(
        property()->color( "surfaceFireMaxDirDiagramSpreadColor" ),
        property()->integer( "surfaceFireMaxDirDiagramSpreadWidth" ),
        Qt::SolidLine );
    QPen slopePen(
        property()->color( "surfaceFireMaxDirDiagramSlopeColor" ),
        property()->integer( "surfaceFireMaxDirDiagramSlopeWidth" ),
        Qt::DotLine );
    QPen windPen(
        property()->color( "surfaceFireMaxDirDiagramWindColor" ),
        property()->integer( "surfaceFireMaxDirDiagramWindWidth" ),
        Qt::DashLine );

    // Diagram legend
    QFont legendFont(
        property()->string( "surfaceFireMaxDirDiagramLegendFontFamily" ),
        property()->integer( "surfaceFireMaxDirDiagramLegendFontSize" ) );
    QFontMetrics legendMetrics( legendFont );

    // Store pixel resolution into local variables.
    double yppi  = m_screenSize->m_yppi;
    //double xppi  = m_screenSize->m_xppi;
    //double padWd = m_pageSize->m_padWd / 2.;

    // Determine the height of the various display fonts.
    double subTitleHt, textHt, titleHt, valueHt, legendHt, headerHt;
    subTitleHt = ( subTitleMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    legendHt = ( legendMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
	headerHt = titleHt + 2. * textHt;
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
	// Up through V5, either (1) a single upslope or (2) possible multiple input
	// fire spread directions had to be handled:
    // bool spreadAtVector = property()->boolean( "surfaceConfSpreadDirInput" );	// Not present in V6
    // bool spreadAtMax    = property()->boolean( "surfaceConfSpreadDirMax" );		// Not present in V6

	// As of V6, we now check for all 6 Fire Spread Direction options
	// These three are all single fire spread directions:
    bool spreadAtHead   = property()->boolean( "surfaceConfSpreadDirHead" );
	bool spreadAtFlank  = property()->boolean( "surfaceConfSpreadDirFlank" );
    bool spreadAtBack   = property()->boolean( "surfaceConfSpreadDirBack" );
	// These three user input (possibly multiple) fire spread directions
    bool spreadAtFireFront = property()->boolean( "surfaceConfSpreadDirFireFront" );	// possible multiple input
    bool spreadAtPointSourcePsi = property()->boolean( "surfaceConfSpreadDirPointSourcePsi" );
    bool spreadAtPointSourceBeta = property()->boolean( "surfaceConfSpreadDirPointSourceBeta" );
	// Re-purpose the spreadAtVector boolean to indicate single or multiple panes:
	bool spreadAtVector = spreadAtFireFront || spreadAtPointSourcePsi || spreadAtPointSourceBeta;

    // Determine which EqVars to use for max fire dir, wind dir, and aspect.
    EqVar *rowVar = m_eqTree->m_rangeVar[0];
    EqVar *colVar = m_eqTree->m_rangeVar[1];
    EqVar *maxDirVar  = 0;
    char  *maxDirName = "not used";
	EqVar *aspectVar  = 0;
	char  *aspectName = "not used";
	EqVar *rosDirVar  = 0;
	char  *rosDirName = "not used";
	EqVar *windVar    = 0;
	char  *windName   = "not used";
	EqVar *rosDirLabel = 0;

	if ( degreesNorth )
    {
        maxDirVar   = m_eqTree->m_eqCalc->vSurfaceFireMaxDirFromNorth;
        maxDirName  = "vSurfaceFireMaxDirFromNorth";
		rosDirLabel = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromNorth;	// use this for the legend label
		// If heading, flanking, or backing ...
        rosDirVar  = maxDirVar;
	    rosDirName = maxDirName;
		if ( spreadAtVector )
		{
			rosDirVar  = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromNorth;
			rosDirName = "vSurfaceFireVectorDirFromNorth";
		}
		aspectVar  = m_eqTree->m_eqCalc->vSiteAspectDirFromNorth;
        aspectName = "vSiteAspectDirFromNorth";
        if ( windDirInput )
	    {
            windVar  = m_eqTree->m_eqCalc->vWindDirFromNorth;
            windName = "vWindDirFromNorth";
        }
	}
    else if ( degreesUpslope )
    {
        maxDirVar  = m_eqTree->m_eqCalc->vSurfaceFireMaxDirFromUpslope;
        maxDirName = "vSurfaceFireMaxDirFromUpslope";
		rosDirLabel = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromUpslope;	// use this for the legend label
		// If heading, flanking, or backing ...
        rosDirVar  = maxDirVar;
	    rosDirName = maxDirName;
		if ( spreadAtVector )
		{
            rosDirVar  = m_eqTree->m_eqCalc->vSurfaceFireVectorDirFromUpslope;
            rosDirName = "vSurfaceFireVectorDirFromUpslope";
		}
        if ( windDirInput )
	    {
            windVar  = m_eqTree->m_eqCalc->vWindDirFromUpslope;
            windName = "vWindDirFromUpslope";
        }
    }

    // Always need maxDir to draw the diagrams
    double *maxDir = new double[ cells ];
    checkmem( __FILE__, __LINE__, maxDir, "double maxDir", cells );
    // Only need wind[] array if wind is a direct input (e.g., not upslope wind)
    double *wind = 0;
    if ( windVar )
    {
        wind = new double[ cells ];
        checkmem( __FILE__, __LINE__, wind, "double wind", cells );
    }
    // Only need aspect[] array if directions are wrt North (.e.g, not wrt upslope)
    double *aspect = 0;
    if ( aspectVar )
    {
        aspect = new double[ cells ];
        checkmem( __FILE__, __LINE__, aspect, "double aspect", cells );
    }
	// Always need rosDir as of V6
    double *rosDir = 0;
    rosDir = new double[ cells ];
    checkmem( __FILE__, __LINE__, rosDir, "double rosDir", cells );

    char   buffer[1024], varName[128];
    int    row, col, cell;
    double value;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        if ( strncmp( buffer, "CELL", 4 ) == 0 )
        {
            if ( strstr( buffer, maxDirName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                maxDir[ cell ] = value;
            }
			// As of V6, the maxDirName could also be the rosDirName,
			// so need to get them both
            if ( rosDir && strstr( buffer, rosDirName ) )
            {
                sscanf( buffer, "CELL %d %d %s cont %lf",
                    &row, &col, varName, &value );
                cell = ( col - 1 ) + ( cols * ( row - 1) );
                rosDir[ cell ] = value;
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
        }
    }
    fclose( fptr );

    // Determine number of diagram panes on each page.
    int paneCols = property()->integer( "surfaceFireMaxDirDiagramCols" );
    int paneRows = property()->integer( "surfaceFireMaxDirDiagramRows" );
    // If 2 or fewer diagrams, use half page
    if ( cells <= 2 || cols == 1)
    {
        paneCols = 1;
        paneRows = 2;
    }
    // Determine size of each diagram pane on the page.
    int    panes  = paneCols * paneRows;
    double paneWd = m_pageSize->m_bodyWd / (double) paneCols;
    double paneHt = ( m_pageSize->m_bodyHt - headerHt ) / (double) paneRows;
	// NOTE: In Build 607, start using dynalic headerHt

    // Determine number of pages across and down to accomodate all diagrams
    int pageCols = 1 + ( cols - 1 ) / paneCols;
    int pageRows = 1 + ( rows - 1 ) / paneRows;

    // Each diagram may have 0-2 top lines and 0-9 bottom lines.
    int topLines = 1;                           // Y axis top label
    if ( rowVar ) topLines++;
    if ( colVar ) topLines++;
    int botLines = 1;                           // Y axis bottom label
    if ( maxDirVar->m_isUserOutput ) botLines++;
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
            top[pane] = m_pageSize->m_marginTop + headerHt + row * paneHt;
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
    translate( title, "BpDocument:Diagrams:FireMaxDirTitle" );
    translate( windVector, "BpDocument:Diagrams:DirOfWindVector" );
    QString desc( m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace() );

    // Loop for each PAGE across and down.
    double yPos, deg;
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
            translate( qStr1, "BpDocument:Diagrams:FireMaxDirPage",
                QString( "%1" ).arg( thisPage ),
                QString( "%1" ).arg( pageCols * pageRows ) );

            // Start a new page
            startNewPage( qStr1, TocDirection );
            yPos = m_pageSize->m_marginTop;

            // Draw a map of where we are.
            composePageMap( ( headerHt - 0.1 ), rows, cols,
                pageRows, pageCols, paneRows, paneCols, pageRow, pageCol );

            // Display the run description as the table title
            m_composer->font( titleFont );          // use tableTitleFont
            m_composer->pen( titlePen );            // use tableTitleFontColor
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                center,                     desc ); // center text
            yPos += titleHt;

			// Display the Fire Direction tab option as the third title line
            m_composer->font( textFont );			// use tableSubtitleFont
            m_composer->pen( subTitlePen );         // use tableSubtitleFontColor
			QString qStr = m_eqTree->m_eqCalc->getSubtitle();
			qStr.ascii();
			if ( qStr.length() > 0 )
			{
				m_composer->text(
					m_pageSize->m_marginLeft,   yPos,   // start at UL corner
					m_pageSize->m_bodyWd,       textHt,	// width and height
					center,                     qStr ); // center text
				yPos += textHt;
			}
            // Display the diagram name as the table subtitle
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // start at UL corner
                m_pageSize->m_bodyWd,       textHt,	// width and height
                center,                     title );// center text
            yPos += textHt;

            // Loop for each PANE on this page.
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
                    // Draw the compass coordinate system
                    m_composer->pen( textPen ); // use tableTextFontColor
                    m_composer->line(
                        left[pane] + textHt,
                        y0[pane],
                        left[pane] + paneWd - textHt,
                        y0[pane] );
                    m_composer->line(
                        x0[pane],
                        top[pane] + figTop,
                        x0[pane],
                        top[pane] + figTop + figHt );

                    // Draw axis labels
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
                    m_composer->pen( titlePen );    // use tableTitleFontColor
                    m_composer->text(
                        left[pane],     top[pane] + ( topLines - 1 ) * textHt,
                        paneWd,         textHt,
                        center,         qStr1 );
                    m_composer->text(
                        left[pane],     top[pane] + figTop + figHt,
                        paneWd,         textHt,
                        center,         qStr2 );

                    // Draw the direction of spread vector first
                    if ( rosDir )
                    {
						int nDirLines = 1;
                        deg = rosDir[datum];
						if ( spreadAtBack )
						{
							deg = ( deg < 180. ) ? ( deg + 180 ) : ( deg - 180. );
						}
						else if ( spreadAtFlank )
						{
							nDirLines = 2;
							deg = ( deg < 90. ) ? ( deg + 90 ) : ( deg - 90. );
						}
						for ( int nDirLine=0; nDirLine<nDirLines; nDirLine++ )
						{
	                        yPos = top[pane] + figTop + 0.05 * figHt;
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
							if ( nDirLine == 0 )	// if flanking
							{
								deg = ( deg < 180. ) ? ( deg + 180 ) : ( deg - 180. );
							}
						}
                        // Draw the fire direction vector Legend
                        yPos = top[pane] + figTop + figHt - 3.0 * textHt;
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
                        m_composer->font( legendFont );
                        m_composer->text(
                            left[pane] + 1.0 * arrow,   yPos + 0.75 * legendHt,
                            *(rosDirLabel->m_label) );
                    }

                    // Draw the direction of maximum spread vector
                    deg = maxDir[datum];
                    yPos = top[pane] + figTop + 0.12 * figHt;
                    m_composer->pen( maxDirPen );
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
                    // Draw the fire direction vector Legend
                    yPos = top[pane] + figTop + figHt - 2.0 * textHt;
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
                    m_composer->font( legendFont );
                    m_composer->text(
                        left[pane] + 1.0 * arrow,   yPos + 0.75 * legendHt,
                        *(maxDirVar->m_label) );

                    // Draw the wind flow direction vector
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
                    yPos = top[pane] + figTop + 0.19 * figHt;
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
                    // Draw the wind flow direction vector Legend
                    yPos = top[pane] + figTop + figHt - 1.0 * textHt;
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

                    // Draw the upslope vector if using compass coordinate system
                    if ( aspectVar )
                    {
                        deg = ( aspect )
                              ? aspect[datum]
                              : 180.;
                        deg += ( deg < 180 )
                              ? 180.
                              : -180. ;
                        yPos = top[pane] + figTop + 0.26 * figHt;
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
                        // Draw the upslope direction vector Legend
                        yPos = top[pane] + figTop + figHt - 0.0 * textHt;
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
                    // Display any requested user outputs
                    yPos = top[pane] + figTop + figHt + textHt;
                    m_composer->font( textFont );
                    m_composer->pen( valuePen );    // use tableTextValueColor
                    if ( maxDirVar->m_isUserOutput )
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
                        yPos += valueHt;
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

    // Clean up and return
    delete progress;    progress = 0;
    delete[] maxDir;     maxDir = 0;
    delete[] wind;      wind = 0;
    delete[] aspect;    aspect = 0;
    delete[] top;       top = 0;
    delete[] left;      left = 0;
    delete[] x0;        x0 = 0;
    delete[] y0;        y0 = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposefiredirdiagram.cpp
//------------------------------------------------------------------------------

