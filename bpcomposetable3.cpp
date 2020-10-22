//------------------------------------------------------------------------------
/*! \file bpcomposetable3.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument table output composer for 2 range variables.
 *
 *  Additional BehavePlusDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposeworksheet.cpp
 *      - bpworksheet.cpp
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
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
#include <qfontmetrics.h>
#include <qpen.h>

//------------------------------------------------------------------------------
/*! \brief Composes all the fire behavior output tables for
 *  range case 4: TWO  continuous and ZERO discrete range variables,
 *  range case 5: ZERO continuous and TWO  discrete range variables, or
 *  range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  Calls composeTable3( int, EqVar*, EqVar* ) for each output table variable.
 *
 *  \param rowVar  Pointer to the table's row EqVar.
 *  \param colVar  Pointer to the table's column EqVar.
 */

void BpDocument::composeTable3( EqVar *rowVar, EqVar *colVar )
{
    for ( int vid = 0;
          vid < tableVars();
          vid++ )
    {
        if ( tableVar(vid)->isDiscrete() || tableVar(vid)->isContinuous() )
        {
            if ( ! tableVar(vid)->isDiagram() )
            {
                composeTable3( vid, rowVar, colVar );
            }
        }
    }

    // Write the spreadsheet file
    composeTable3Spreadsheet( rowVar, colVar );
    composeTable3Html( rowVar, colVar );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior two-way output tables.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 4: TWO  continuous and ZERO discrete range variables,
 *  - range case 5: ZERO continuous and TWO  discrete range variables, or
 *  - range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  \param vid Index of the table's value variable.
 *  \param rowVar Pointer to the table's row EqVar .
 *  \param colVar Pointer to the table's column EqVar .
 */

void BpDocument::composeTable3( int vid, EqVar *rowVar, EqVar *colVar )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
    QFont subTitleFont( property()->string( "tableSubtitleFontFamily" ),
                    property()->integer( "tableSubtitleFontSize" ) );
    QPen subTitlePen( property()->color( "tableSubtitleFontColor" ) );
    QFontMetrics subTitleMetrics( subTitleFont );

    QFont textFont( property()->string( "tableTextFontFamily" ),
                    property()->integer( "tableTextFontSize" ) );
    QPen textPen( property()->color( "tableTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    QFont titleFont( property()->string( "tableTitleFontFamily" ),
                    property()->integer( "tableTitleFontSize" ) );
    QPen titlePen( property()->color( "tableTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    QFont valueFont( property()->string( "tableValueFontFamily" ),
                    property()->integer( "tableValueFontSize" ) );
    QPen valuePen( property()->color( "tableValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    bool doRowBg = property()->boolean( "tableRowBackgroundColorActive" );
    QBrush rowBrush( property()->color( "tableRowBackgroundColor" ),
        Qt::SolidPattern );

    QString text("");

    // Store pixel resolution into local variables.
    double yppi  = m_screenSize->m_yppi;
    double xppi  = m_screenSize->m_xppi;
    double m_padWd = m_pageSize->m_padWd;

    // Determine the height of the various display fonts.
    double subTitleHt, textHt, titleHt, valueHt, rowHt, x0, x1;
    subTitleHt  = ( subTitleMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    rowHt   = ( textHt > valueHt )
              ? textHt
              : valueHt;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Determine the number of rows we can display on a page.
    int rowsPerPage = (int)
        ( ( m_pageSize->m_bodyHt - 5. * titleHt - 4. * textHt ) / rowHt );

    // Number of pages the table requires to accomodate all the rows.
    int pagesLong = 1 + (int) ( tableRows() / rowsPerPage );

    // Arrays to hold the output values' column information.
    // Page on which the output value column appears.
    int *colPage = new int[ tableCols() ];
    checkmem( __FILE__, __LINE__, colPage, "int colPage", tableCols() );
    // Horizontal position of each output value column.
    double *colXPos = new double[ tableCols() ];
    checkmem( __FILE__, __LINE__, colXPos, "double colXPos", tableCols() );
    // Column header text.
    QString *colText = new QString[ tableCols() ];
    checkmem( __FILE__, __LINE__, colText, "double colText", tableCols() );

    // Prescription shading?
    bool doRx = property()->boolean( "tableShading" );
    bool doBlank = property()->boolean( "tableShadingBlank" );

    // Determine the row variable's (left-most) column width.
    int row, iid, cell;
    double len;
    QString qStr;
    // Start wide enough to hold the variable name and units.
    double rowWd = m_padWd
        + ( (double) headerWidth( rowVar, textMetrics ) / xppi );
    // Enlarge it to hold the fattest row value.
	m_rowDecimals = 0;
    for ( row = 0;
          row < tableRows();
          row++ )
    {
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row );
            qStr = rowVar->m_itemList->itemName( iid ) + "MMM";
        }
        else if ( rowVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				qStr.sprintf( "%1.*fMMM", rowVar->m_displayDecimals, tableRow( row ) );
			}
			else
			{
				// Start with 6 decimals for this row value
				int decimals = 6;
				qStr.sprintf( "%1.*f", decimals, tableRow( row ) );
				// Remove all trailing zeros
				while ( qStr.endsWith( "0" ) )
				{
					qStr = qStr.left( qStr.length()-1 );
					decimals--;
				}
				// Update m_rowDecimals digits
				m_rowDecimals = ( decimals > m_rowDecimals ) ? decimals : m_rowDecimals;
				qStr.append( "MWM" );
			}
        }
        len = (double) textMetrics.width( qStr ) / xppi;
        if ( len > rowWd )
        {
            rowWd = len;
        }
    }
    // Find the fattest output value for this table variable.
    int col;
    int out = vid;
    EqVar *outVar = tableVar(vid);
    double colWd = 0;
    for ( row = 0;
          row < tableRows();
          row++ )
    {
        for ( col = 0;
              col < tableCols();
              col++ )
        {
            if ( outVar->isDiscrete() )
            {
                iid = (int) tableVal( out );
                qStr = outVar->m_itemList->itemName( iid ) + "WM";
            }
            else if ( outVar->isContinuous() )
            {
                qStr.sprintf( "%1.*fWM",
                    outVar->m_displayDecimals, tableVal( out ) );
            }
            len = (double) textMetrics.width( qStr ) / xppi;
            if ( len > colWd )
            {
                colWd = len;
            }
            out += tableVars();
        }   // Next table column.
    } // Next table row.

    // Set the column header value text.
	m_colDecimals = 0;
    for ( col = 0;
          col < tableCols();
          col++ )
    {
        if ( colVar->isDiscrete() )
        {
            iid = (int) tableCol( col );
            colText[col] = colVar->m_itemList->itemName( iid );
        }
        else if ( colVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				colText[col].sprintf( " %1.*f",	colVar->m_displayDecimals, tableCol( col ) );
			}
			else
			{
				// Start with 6 decimals for this row value
				int decimals = 6;
				colText[col].sprintf( " %1.*f", decimals, tableCol( col ) );
				// Remove all trailing zeros
				while ( colText[col].endsWith( "0" ) )
				{
					colText[col] = colText[col].left( colText[col].length()-1 );
					decimals--;
				}
				// Update Decimals digits
				m_colDecimals = ( decimals > m_colDecimals ) ? decimals : m_colDecimals;
			}
        }
        // Expand the column width to accomodate the header value text?
        len = (double) textMetrics.width( colText[col] ) / xppi;
        if ( len > colWd )
        {
            colWd = len;
        }
    }   // Next table column.
	// CDB DECIMALS MOD
	for ( col = 0;  col < tableCols(); col++ )
	{
		if ( colVar->isContinuous() )
		{
			colText[col].sprintf( " %1.*f", m_colDecimals, tableCol( col ) );
		}
	}
    // Add padding between each column.
    colWd += m_padWd;

    // Determine each column's position on each page.
    int pagesWide = 1;
    // Horizontal position of first column on the first page.
    double xpos = m_pageSize->m_bodyLeft + rowWd + 2. * m_padWd;
    for ( col = 0;
          col < tableCols();
          col++ )
    {
        // If past the right page edge, start a new page.
        if ( xpos + colWd > m_pageSize->m_bodyRight )
        {
            // The table page width has just increased.
            pagesWide++;
            // This will be the first column on the new page.
            xpos = m_pageSize->m_bodyLeft + rowWd + 2. * m_padWd;
        }
        // Store the page and position of this output variable's column.
        colXPos[col] = xpos;
        colPage[col] = pagesWide;
        // Update the position pointer.
        xpos += ( 2. * m_padWd + colWd );
    }

    // Determine the column title width (inches).
    double colTitleWd = textMetrics.width( *(colVar->m_label) ) / xppi;
    if ( ( textMetrics.width( colVar->m_displayUnits ) / xppi ) > colTitleWd )
    {
        colTitleWd = textMetrics.width( colVar->m_displayUnits ) / xppi;
    }
    colTitleWd += ( 2. * m_padWd );

    // Determine an offset shift to center each pageAcross.
    double *shift = new double[ pagesWide + 1 ];
    checkmem( __FILE__, __LINE__, shift, "double shift", pagesWide + 1 );
    for ( col = 0;
          col < tableCols();
          col++ )
    {
        // Table must be at least this wide.
        double minLeft = m_pageSize->m_bodyLeft + rowWd + 2. * m_padWd + colTitleWd;
        // Does it need to be wider to accomodate this column?
        if ( colXPos[col] + colWd > minLeft )
        {
            minLeft = colXPos[col] + colWd;
        }
        // Just the last column of each page finally gets stored in shift[].
        shift[colPage[col]] = 0.5 * ( m_pageSize->m_bodyRight - minLeft );
    }

    // Start drawing the table.
    double yPos, s, bgLeft, bgRight, rightEdge, leftEdge;
    int i;
    // Loop for each page down.
    int thisPage = 1;
    for ( int pageDown = 1;
          pageDown <= pagesLong;
          pageDown++ )
    {
        // Loop for each page across.
        for ( int pageAcross = 1;
              pageAcross <= pagesWide;
              pageAcross++, thisPage++ )
        {
            // Table title indicates the table portion
            translate( text, "BpDocument:Table:PageOf",
                *(tableVar(vid)->m_label),
                QString( "%1" ).arg( thisPage ),
                QString( "%1" ).arg( pagesLong * pagesWide ) );

            // Start a new page
            startNewPage( text, TocTable );
            yPos = m_pageSize->m_marginTop + titleHt;

            // Draw a map of where we are.
            composePageMap( ( 2. * titleHt - 0.1 ), pagesLong, pagesWide,
                pagesLong, pagesWide, 1, 1, pageDown-1, pageAcross-1 );

            // Display the table title::description
            m_composer->font( titleFont );          // use tableTitleFont
            m_composer->pen( titlePen );            // use tableTitleFontColor
            qStr = m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace();
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // start at UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                Qt::AlignVCenter|Qt::AlignHCenter,  // center alignement
                qStr );                             // display description
            yPos += titleHt;

			// Display the table subtitle - (Fire Direction Option)
			qStr = m_eqTree->m_eqCalc->getSubtitle();
			qStr.ascii();
			if ( qStr.length() > 0 )
			{
				m_composer->font( textFont );			// use a small-ish font ...
				m_composer->pen( subTitlePen );			// but use the title color scheme
				m_composer->text(
					m_pageSize->m_marginLeft, yPos,     // start at UL corner
					m_pageSize->m_bodyWd, textHt,       // width and height
					Qt::AlignVCenter|Qt::AlignCenter,   // center alignment
					qStr );                             // display description
				yPos += textHt;
			}
            // Display the table title::variable
            m_composer->font( subTitleFont );       // use tableSubtitleFont
            m_composer->pen( subTitlePen );         // use tableSubtitleFontColor
            qStr = *(outVar->m_label);
            if ( outVar->isContinuous() )
            {
                qStr = *(outVar->m_label) + " " + outVar->displayUnits(true);
            }
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // start at UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                Qt::AlignVCenter|Qt::AlignHCenter,  // center alignment
                qStr );                             // table variable name
            yPos += subTitleHt;

            // Display the table title::portion
            //m_composer->text(
            //    m_pageSize->m_marginLeft,   yPos,
            //    m_pageSize->m_bodyWd,       titleHt,
            //    Qt::AlignVCenter|Qt::AlignHCenter,
            //    portion );
            //yPos += titleHt;
            yPos += titleHt;

            // Everything else on this page is shifted
            s = shift[pageAcross];

            // Determine left and right edges of the table.
            leftEdge = -1;
            for ( col = 0;
                  col < tableCols();
                  col++ )
            {
                if ( pageAcross == colPage[col] )
                {
                    rightEdge = colXPos[col] + colWd + s;
                    if ( leftEdge < 0. )
                    {
                        leftEdge = colXPos[col] + s;
                    }
                }
            }
            // Must be at least wide enough to accomodate column header text.
            if ( rightEdge < leftEdge + colTitleWd )
            {
                rightEdge = leftEdge + colTitleWd;
            }
            bgLeft = m_pageSize->m_marginLeft + s - m_padWd ;
            bgRight = rightEdge - leftEdge + rowWd + 4 * m_padWd ;

            // Display a colored row column header background?
            if ( doRowBg )
            {
                m_composer->fill(
                    bgLeft,     yPos,
                    bgRight,    3 * textHt,
                    rowBrush );
            }
            // Display the row column header0.
            m_composer->font( textFont );           // use tableTextFont
            m_composer->pen( textPen );             // use tableTextFontColor
            m_composer->text(
                m_pageSize->m_marginLeft + s,   yPos,
                rowWd,                          textHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                *(rowVar->m_hdr0) );

            // Display the row column header1.
            m_composer->text(
                m_pageSize->m_marginLeft + s,   yPos + textHt,
                rowWd,                          textHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                *(rowVar->m_hdr1) );

            // Display the row column units.
            m_composer->text(
                m_pageSize->m_marginLeft + s,   yPos + 2. * textHt,
                rowWd,                          textHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                rowVar->displayUnits() );
            // Display the row column header underline
            // only if we are not coloring row backgrounds.
            int skipLines = 3;
            if ( ! doRowBg )
            {
                m_composer->line(
                    m_pageSize->m_marginLeft + s,           yPos + 3.5 * textHt,
                    m_pageSize->m_marginLeft + rowWd + s,   yPos + 3.5 * textHt );
                skipLines = 4;
            }
            // Display the output column headers.
            for ( col = 0;
                  col < tableCols();
                  col++ )
            {
                if ( pageAcross == colPage[col] )
                {
                    // Display the output column units.
                    m_composer->text(
                        colXPos[col] + s,   yPos + 2. * textHt,
                        colWd,              textHt,
                        Qt::AlignVCenter|Qt::AlignRight,
                        colText[col] );
                    // Display the output column underline.
                    if ( ! doRowBg )
                    {
                        m_composer->line(
                            colXPos[col] + s,           yPos + 3.5 * textHt,
                            colXPos[col] + colWd + s,   yPos + 3.5 * textHt );
                    }
                }
            }
            // Display a centered column variable name and units header.
            m_composer->text(
                leftEdge,                   yPos,
                ( rightEdge - leftEdge ),   textHt,
                Qt::AlignVCenter|Qt::AlignHCenter,
                *(colVar->m_label) );

            if ( colVar->isContinuous() )
            {
                m_composer->text(
                    leftEdge,                   yPos + textHt,
                    ( rightEdge - leftEdge ),   textHt,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    colVar->displayUnits() );
            }

            // If there are previous columns, display a visual cue.
            if ( pageAcross > 1 )
            {
                for ( i = 0;
                      i < 3;
                      i++ )
                {
                    m_composer->text(
                        0, ( yPos + i * textHt ),
                        ( m_pageSize->m_marginLeft + s - m_padWd ),   textHt,
                        Qt::AlignVCenter|Qt::AlignRight,
                        "<" );
                }
            }
            // If there are subsequent column pages, display a visual clue.
            if ( pageAcross < pagesWide )
            {
                for ( i = 0;
                      i < 3;
                      i++ )
                {
                    m_composer->text(
                        ( rightEdge + m_padWd ), ( yPos + i * textHt ),
                        ( m_pageSize->m_pageWd - rightEdge ), textHt,
                        Qt::AlignVCenter|Qt::AlignLeft,
                        ">" );
                }
            }
            // Set vertical start of rows.
            yPos += skipLines * textHt;

            // Determine the rows range to display on this page.
            int rowFrom = ( pageDown - 1 ) * rowsPerPage;
            int rowThru = pageDown * rowsPerPage - 1;
            if ( rowThru >= tableRows() )
            {
                rowThru = tableRows() - 1;
            }
            // Determine the columns range to display on this page.
            int colFrom = -1;
            int colThru = 0;
            for ( col = 0;
                  col < tableCols();
                  col++ )
            {
                if ( colPage[col] == pageAcross )
                {
                    if ( colFrom == -1 )
                    {
                        colFrom = col;
                    }
                    colThru = col;
                }
            }

            // Loop for each row on this page.
            bool doThisRowBg = false;
            for ( row = rowFrom;
                  row <= rowThru;
                  row++ )
            {
                // Display a colored row background?
                if ( doRowBg && doThisRowBg )
                {
                    m_composer->fill(
                        bgLeft,     yPos,
                        bgRight,    textHt,
                        rowBrush );
                }
                doThisRowBg = ! doThisRowBg;

                // Left-most (row variable) column value.
                if ( rowVar->isDiscrete() )
                {
                    iid = (int) tableRow( row );
                    qStr = rowVar->m_itemList->itemName( iid );
                }
                else if ( rowVar->isContinuous() )
                {
					// CDB DECIMALS MOD
					if ( false )
					{
						qStr.sprintf( "%1.*f", rowVar->m_displayDecimals, tableRow( row ) );
					}
					else
					{
						qStr.sprintf( "%1.*f", m_rowDecimals, tableRow( row ) );
					}
                }
                m_composer->font( textFont );       // use tableTextFont
                m_composer->pen( textPen );         // use tableTextFontColor
                m_composer->text(
                    m_pageSize->m_marginLeft + s,   yPos,
                    rowWd,                          textHt,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qStr );

                // Loop for each column value on this page.
                m_composer->font( valueFont );      // use tableValueFont
                m_composer->pen( valuePen );        // use tableValueFontColor
                out = vid
                    + colFrom * tableVars()
                    + row * tableCols() * tableVars();
                for ( col = colFrom;
                      col <= colThru;
                      col++ )
                {
                    // Determine whether to hatch this cell
                    cell = col + row * tableCols();
                    bool hatch = doRx && ! tableInRx( cell );

                    // Discrete variables use their item name.
                    if ( outVar->isDiscrete() )
                    {
                        iid = (int) tableVal( out );
                        qStr = outVar->m_itemList->itemName( iid );
                    }
                    // Continuous variables use the current display units format.
                    else if ( outVar->isContinuous() )
                    {
                        qStr.sprintf( " %1.*f",
                            outVar->m_displayDecimals, tableVal( out ) );
                    }
                    // Display the output value.
                    if ( hatch && doBlank )
                    {
                        // draw nothing
                    }
                    else
                    {
                        m_composer->text(
                            colXPos[col] + s,   yPos,
                            colWd,              textHt,
                            Qt::AlignVCenter|Qt::AlignRight,
                            qStr );
                    }
                    out += tableVars();
                    // RX hatching
                    if ( hatch && ! doBlank && ! outVar->isDiagram() )
                    {
                        x0 = colXPos[col] + s - m_padWd;
                        x1 = colXPos[col] + s + m_padWd + colWd;
                        m_composer->line( x0, yPos, x1, ( yPos + textHt ) );
                        m_composer->line( x0, ( yPos + textHt ), x1, yPos );
                    }
                } // Next table output variable.
                yPos += rowHt;
            } // Next table row.
        } // Next pageAcross.
    } // Next pageDown.

    // Be polite and stop the composer.
    m_composer->end();

    // Clean up and return.
    delete[] colPage;
    delete[] colXPos;
    delete[] colText;
    delete[] shift;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior two-way output tables.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 4: TWO  continuous and ZERO discrete range variables,
 *  - range case 5: ZERO continuous and TWO  discrete range variables, or
 *  - range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  \param vid Index of the table's value variable.
 *  \param rowVar Pointer to the table's row EqVar .
 *  \param colVar Pointer to the table's column EqVar .
 */

void BpDocument::composeTable3Html( EqVar *rowVar, EqVar *colVar )
{
    // Attempt to open the html file
    QString fileName = appFileSystem()->composerPath()
        + "/" + property()->string( "exportHtmlFile" );
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( fileName, "w" ) ) )
    {
        return;
    }

    // Write the header
    composeTableHtmlHeader( fptr );

    // Loop for each output variable
    for ( int vid = 0; vid < tableVars(); vid++ )
    {
        if ( ( tableVar(vid)->isDiscrete() || tableVar(vid)->isContinuous() )
          && ! tableVar(vid)->isDiagram() )
        {
            composeTable3Html( fptr, vid, rowVar, colVar );
        }
    }

    // Write the footer
    composeTableHtmlFooter( fptr );

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior two-way output tab-delimited spreadsheet.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 4: TWO  continuous and ZERO discrete range variables,
 *  - range case 5: ZERO continuous and TWO  discrete range variables, or
 *  - range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  \param vid Index of the table's value variable.
 *  \param rowVar Pointer to the table's row EqVar .
 *  \param colVar Pointer to the table's column EqVar .
 */

void BpDocument::composeTable3Html( FILE *fptr, int vid,
    EqVar *rowVar, EqVar *colVar )
{
    EqVar *outVar = tableVar(vid);
    int iid;

    // Prescription shading?
    bool doRx = property()->boolean( "tableShading" );
    bool doBlank = property()->boolean( "tableShadingBlank" );

    QString text = "";
    if ( outVar->isContinuous() )
    {
        text = "(" + outVar->displayUnits() + ")";
    }
	QString qStr = m_eqTree->m_eqCalc->getSubtitle();
	QString subTitle = "";
	if ( qStr.length() > 0 )
	{
		subTitle = "  <h3 class=\"bp2subhead\"><b>" + qStr + "</b></h3>\n";
	}
    fprintf( fptr,
        "<!-- Begin 2-Way Output Table for %s -->\n"	// Title (display var label)
        "<p class=\"bp2\">\n%s"			// SURFACE subtitle
        "  <h3 class=\"bp2\">Results for: %s %s</h3>\n"	// Display variable label and units
        "  <table cellpadding=\"5\" cellspacing=\"2\" border=\"0\">\n",
        (*(outVar->m_label)).latin1(),
 		subTitle.latin1(),
        (*(outVar->m_label)).latin1(),
        text.latin1()
    );

    // First column headers
    fprintf( fptr,
        "    <tr>\n"
        "      <td class=\"bp2hdr\" align=\"center\">\n"
        "        %s\n"
        "      </td>\n",
        (*(rowVar->m_hdr0)).latin1()
    );
    fprintf( fptr,
        "      <td class=\"bp2hdr\" colspan=\"%d\" align=\"center\">\n"
        "        %s\n"
        "      </td>\n"
        "    </tr>\n",
        tableCols(),
        (*(colVar->m_label)).latin1()
    );

    // Second column header
    fprintf( fptr,
        "    <tr>\n"
        "      <td class=\"bp2hdr\" align=\"center\">\n"
        "        %s\n"
        "      </td>\n",
        (*(rowVar->m_hdr1)).latin1()
    );
    text = "";
    if ( colVar->isContinuous() )
    {
        text = colVar->displayUnits();
    }
    fprintf( fptr,
        "      <td class=\"bp2hdr\" colspan=\"%d\" align=\"center\">\n"
        "        %s\n"
        "      </td>\n"
        "    </tr>\n",
        tableCols(),
        text.latin1()
    );

    // Third column header
    // Row variable's units
    text = "";
    if ( rowVar->isContinuous() )
    {
        text = rowVar->displayUnits();
    }
    fprintf( fptr,
        "    <tr>\n"
        "      <td class=\"bp2hdr\" align=\"center\">\n"
        "        %s\n"
        "      </td>\n",
        text.latin1()
    );
    // Column variable values
    int row, col;
    for ( col = 0; col < tableCols(); col++ )
    {
        if ( colVar->isDiscrete() )
        {
            iid = (int) tableCol( col );
            text = colVar->m_itemList->itemName( iid );
        }
        else if ( colVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				text.sprintf( " %1.*f",	colVar->m_displayDecimals, tableCol( col ) );
			}
			else
			{
				text.sprintf( " %1.*f", m_colDecimals, tableCol( col ) );
			}
        }
        fprintf( fptr,
            "      <td class=\"bp2hdr\" align=\"center\">\n"
            "        %s\n"
            "      </td>\n",
            text.latin1()
        );
    }
    fprintf( fptr, "    </tr>\n" );

    // Table body
    int out = vid;
    for ( row = 0; row < tableRows(); row++ )
    {
        // Row value is in the first column
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row );
            text = rowVar->m_itemList->itemName( iid );
        }
        else if ( rowVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				text.sprintf( "%1.*f", rowVar->m_displayDecimals, tableRow( row ) );
			}
			else
			{
				text.sprintf( "%1.*f", m_rowDecimals, tableRow( row ) );
			}
        }
        fprintf( fptr,
            "    <tr>\n"
            "      <td class=\"bp2hdr\" align=\"center\">\n"
            "        %s\n"
            "      </td>\n",
            text.latin1()
        );

        // Remaining columns
        for ( col = 0; col < tableCols(); col++ )
        {
            if ( outVar->isDiscrete() )
            {
                iid = (int) tableVal( out );
                text = outVar->m_itemList->itemName( iid );
            }
            else if ( outVar->isContinuous() )
            {
                text.sprintf( "%1.*f",
                    outVar->m_displayDecimals, tableVal( out ) );
            }
            // Display the output value.
            if ( doRx )
            {
                int cell = col + row * tableCols();
                if ( tableInRx( cell ) )
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%din\" align=\"center\">\n"
                        "        %s\n"
                        "      </td>\n",
                        row%2, text.latin1()
                    );
                }
                else if ( doBlank )
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%dout\" align=\"center\">\n"
                        "        &nbsp;\n"
                        "      </td>\n",
                        row%2
                    );
                }
                else
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%dout\" align=\"center\">\n"
                        "        %s\n"
                        "      </td>\n",
                        row%2, text.latin1()
                    );
                }
            }
            else
            {
                fprintf( fptr,
                    "      <td  class=\"bp2row%d\" align=\"center\">\n"
                    "        %s\n"
                    "      </td>\n",
                    row%2, text.latin1()
                );
            }
            out += tableVars();
        }   // Next table column
        fprintf( fptr, "    </tr>\n" );
    } // Next table row

    // End of the table for this variable
    fprintf( fptr,
        "  </table>\n"
        "</p>\n"
        "<!-- End 2-Way Output Table for %s -->\n\n",
        (*(outVar->m_label)).latin1()
    );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior two-way output tab-delimited spreadsheet.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 4: TWO  continuous and ZERO discrete range variables,
 *  - range case 5: ZERO continuous and TWO  discrete range variables, or
 *  - range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  \param vid Index of the table's value variable.
 *  \param rowVar Pointer to the table's row EqVar .
 *  \param colVar Pointer to the table's column EqVar .
 */

void BpDocument::composeTable3Spreadsheet( EqVar *rowVar, EqVar *colVar )
{
    // Attempt to open the spreadsheet file
    QString fileName = appFileSystem()->composerPath()
        + "/" + property()->string( "exportSpreadsheetFile" );
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( fileName, "w" ) ) )
    {
        return;
    }

    // BehavePlus name, version, and build
    fprintf( fptr, "%s\t%s\t%s\n\t\t\n",
        appWindow()->m_program.latin1(),
        appWindow()->m_version.latin1(),
        appWindow()->m_build.latin1() );

    // Run description, date, and time
    fprintf( fptr, "%s\t%s\t%s\n\t\t\n",
        m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace().latin1(),
        m_runTime.left(17).latin1(),
        m_runTime.right(8).latin1() );

    // Loop for each output variable
    for ( int vid = 0; vid < tableVars(); vid++ )
    {
        if ( ( tableVar(vid)->isDiscrete() || tableVar(vid)->isContinuous() )
          && ! tableVar(vid)->isDiagram() )
        {
            composeTable3Spreadsheet( fptr, vid, rowVar, colVar );
        }
    }

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior two-way output tab-delimited spreadsheet.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 4: TWO  continuous and ZERO discrete range variables,
 *  - range case 5: ZERO continuous and TWO  discrete range variables, or
 *  - range case 6: ONE  continuous and ONE  discrete range variable.
 *
 *  \param vid Index of the table's value variable.
 *  \param rowVar Pointer to the table's row EqVar .
 *  \param colVar Pointer to the table's column EqVar .
 */

void BpDocument::composeTable3Spreadsheet( FILE *fptr, int vid,
    EqVar *rowVar, EqVar *colVar )
{
    EqVar *outVar = tableVar(vid);
    QString qStr;
    int iid;
    // Two blank lines between tables
    fprintf( fptr, "\n\n" );
    fprintf( fptr, "%s\t%s\n",
        (*(outVar->m_label)).latin1(),
        ( outVar->isContinuous() ) ? outVar->displayUnits().latin1() : "" );
    fprintf( fptr, "%s\t%s\n",
        (*(rowVar->m_label)).latin1(),
        ( rowVar->isContinuous() ) ? rowVar->displayUnits().latin1() : "" );
    fprintf( fptr, "%s\t%s\n",
        (*(colVar->m_label)).latin1(),
        ( colVar->isContinuous() ) ? colVar->displayUnits().latin1() : "" );

    // First column header row
    fprintf( fptr, "\n" );
    fprintf( fptr, "%s\t \t%s\t%s\n",
        (*(rowVar->m_hdr0)).latin1(),
        (*(colVar->m_hdr0)).latin1(),
        (*(colVar->m_hdr1)).latin1() );

    // Second column header row
    fprintf( fptr, "%s", (*(rowVar->m_hdr1)).latin1() );
    int row, col;
    for ( col = 0; col < tableCols(); col++ )
    {
        if ( colVar->isDiscrete() )
        {
            iid = (int) tableCol( col );
            qStr = colVar->m_itemList->itemName( iid );
        }
        else if ( colVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				qStr.sprintf( " %1.*f",	colVar->m_displayDecimals, tableCol( col ) );
			}
			else
			{
				qStr.sprintf( " %1.*f", m_colDecimals, tableCol( col ) );
			}
        }
        fprintf( fptr, "\t%s", qStr.latin1() );
    }
    fprintf( fptr, "\n" );

    // Table body
    int out = vid;
    for ( row = 0; row < tableRows(); row++ )
    {
        // Row value is in the first column
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row );
            qStr = rowVar->m_itemList->itemName( iid );
        }
        else if ( rowVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				qStr.sprintf( "%1.*f", rowVar->m_displayDecimals, tableRow( row ) );
			}
			else
			{
				qStr.sprintf( "%1.*f", m_rowDecimals, tableRow( row ) );
			}
        }
        fprintf( fptr, "%s", qStr.latin1() );
        // Remaining columns
        for ( col = 0; col < tableCols(); col++ )
        {
            if ( outVar->isDiscrete() )
            {
                iid = (int) tableVal( out );
                qStr = outVar->m_itemList->itemName( iid );
            }
            else if ( outVar->isContinuous() )
            {
                qStr.sprintf( "%1.*f",
                    outVar->m_displayDecimals, tableVal( out ) );
            }
            fprintf( fptr, "\t%s", qStr.latin1() );
            out += tableVars();
        }   // Next table column
        fprintf( fptr, "\n" );
    } // Next table row
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposetable3.cpp
//------------------------------------------------------------------------------

