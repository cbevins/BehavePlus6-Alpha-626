//------------------------------------------------------------------------------
/*! \file bpcomposetable2.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument table output composer for 1 range variables.
 *
 *  Additional BehavePlusDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefiremaxdir.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable3.cpp
 *      - bpcomposeworksheet.cpp
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
/*! \brief Composes the fire behavior 1-way output tables.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 2: 1 continuous and 0 discrete range variables, and
 *  - range case 3: 0 continuous and 1 discrete range variables.
 *
 *  The table's left column contains the values of the \a rowVar.
 *  Results for each output variable appear in the remaining columns.
 *
 *  \param rowVar  Pointer to the table's row EqVar.
 */

void BpDocument::composeTable2( EqVar *rowVar )
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
    double subTitleHt, textHt, titleHt, valueHt, rowHt;
    subTitleHt = ( subTitleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    rowHt   = ( textHt > valueHt )
              ? textHt
              : valueHt;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.

    // Determine the number of rows we can display on a page.
    int rowsPerPage = (int)
        ( ( m_pageSize->m_bodyHt - 4. * titleHt - 4. * textHt ) / rowHt );

    // Number of pages the table requires to accommodate all the rows.
    int pagesLong = 1 + (int) ( tableRows() / rowsPerPage );

    // Arrays to hold the output variables' column information.
    // Page on which the output variable column appears.
    int *colPage = new int[ tableVars() ];
    checkmem( __FILE__, __LINE__, colPage, "int colPage", tableVars() );
    // Width of each output variable column (inches).
    double *colWd = new double[ tableVars() ];
    checkmem( __FILE__, __LINE__, colWd, "double colWd", tableVars() );
    // Horizontal position of each output variable column.
    double *colXPos = new double[tableVars() ];
    checkmem( __FILE__, __LINE__, colXPos, "double colXPos", tableVars() );

    // Prescription shading?
    bool doRx = property()->boolean( "tableShading" );
    bool doBlank = property()->boolean( "tableShadingBlank" );

    // Determine all column (output variable) header widths.
    int vid, iid;
    EqVar *varPtr;
    double rowWd = m_padWd
        + ( (double) headerWidth( rowVar, textMetrics ) / xppi );
    for ( vid = 0;
          vid < tableVars();
          vid++ )
    {
        varPtr = tableVar(vid);
        if ( varPtr->isDiagram() )
        {
            colWd[vid] = 0;
        }
        else
        {
            colWd[vid] = m_padWd
                + ( (double) headerWidth( varPtr, textMetrics ) / xppi );
        }
    }
    // Now adjust all the column widths to accommodate their data display values.
    QString qStr;
    double len;
    int out = 0;        // tableVal() index
    int row;
	m_rowDecimals = 0;
    for ( row = 0;
          row < ( tableRows() * tableCols() );
          row ++ )
    {
        // Adjust the left-most (row variable) column width for long values
        // (which is never a diagram type variable).
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row / tableCols() );
            qStr = rowVar->m_itemList->itemName( iid ) + "    ";
        }
        else if ( rowVar->isContinuous() )
        {
			// CDB DECIMALS MOD
			if ( false )
			{
				qStr.sprintf( " %1.*fWM", rowVar->m_displayDecimals, tableRow( row / tableCols() ) );
			}
			else
			{
				// Start with 6 decimals for this row value
				int decimals = 6;
				qStr.sprintf( " %1.*f", decimals, tableRow( row / tableCols() ) );
				// Remove all trailing zeros
				while ( qStr.endsWith( "0" ) )
				{
					qStr = qStr.left( qStr.length()-1 );
					decimals--;
				}
				// Update Decimals digits
				m_rowDecimals = ( decimals > m_rowDecimals ) ? decimals : m_rowDecimals;
				qStr.append( "WM" );
			}
        }
        len = (double) textMetrics.width( qStr ) / xppi;
        if ( len > rowWd )
        {
            rowWd = len;
        }
        // Adjust output variable column width for this row's column values.
        for ( vid = 0, out = 0;
              vid < tableVars();
              vid++, out++ )
        {
            varPtr = tableVar( vid );
            // Don't show diagram variables
            if ( varPtr->isDiagram() )
            {
                colWd[vid] = 0;
            }
            else
            {
                // Discrete variables use their item name.
                if ( varPtr->isDiscrete() )
                {
                    iid = (int) tableVal( out );
                    qStr = varPtr->m_itemList->itemName( iid ) + "WM";
                }
                // Continuous variables use the current display units format.
                else if ( varPtr->isContinuous() )
                {
                    qStr.sprintf( " %1.*fWM",
                        varPtr->m_displayDecimals+1, tableVal( out ) );
                }
                // Determine if the column width needs to be enlarged.
                len = (double) valueMetrics.width( qStr ) / xppi;
                if ( len > colWd[vid] )
                {
                    colWd[vid] = len;
                }
            }
        } // Next table output variable.
    } // Next table row.

    // Determine each output variable's column position.
    int pagesWide = 1;
    // Horizontal position of first column on the first page.
    double xpos = m_pageSize->m_bodyLeft + rowWd + 2. * m_padWd;
    for ( vid = 0;
          vid < tableVars();
          vid++ )
    {
        // If past the right edge, start a new page.
        if ( xpos + colWd[vid] + 1. > m_pageSize->m_bodyRight )
        {
            // The table width in pages has just increased.
            pagesWide++;
            // This will be the first column on the new page.
            xpos = m_pageSize->m_bodyLeft + rowWd + 2. * m_padWd;
        }
        // Store the page and position of this output variable's column.
        colXPos[vid] = xpos;
        colPage[vid] = pagesWide;
        // Update the position pointer.
        varPtr = tableVar( vid );
        if ( ! varPtr->isDiagram() )
        {
            xpos += ( 2. * m_padWd + colWd[vid] );
        }
    }

    // Determine an offset shift to center each pageAcross.
    double *shift = new double[ pagesWide + 1 ];
    checkmem( __FILE__, __LINE__, shift, "double shift", pagesWide + 1 );
    for ( vid = 0;
          vid < tableVars();
          vid++ )
    {
        // Just the last column of each page gets stored in shift[].
        shift[colPage[vid]] =
            0.5 * ( m_pageSize->m_bodyRight - ( colXPos[vid] + colWd[vid] ) );
    }

    // Start drawing the table.
    double yPos, s, bgRight, bgLeft, rightEdge, x0, x1;
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
            // Table title indicates the table portion.
            translate( text, "BpDocument:Table:PageOf",
                *(rowVar->m_label),
                QString( "%1" ).arg( thisPage ),
                QString( "%1" ).arg( pagesLong * pagesWide ) );
            // Start a new page.
            startNewPage( text, TocTable );
            yPos = m_pageSize->m_marginTop + titleHt;

            // Draw a map of where we are.
            composePageMap( ( 2. * titleHt - 0.1 ), pagesLong, pagesWide,
                pagesLong, pagesWide, 1, 1, pageDown-1, pageAcross-1 );

            // Display the table title::description.
            m_composer->font( titleFont );          // use tableTitleFont
            m_composer->pen( titlePen );            // use tableTitleFontColor
            qStr = m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace();
            m_composer->text(
                m_pageSize->m_marginLeft,   yPos,   // start at UL corner
                m_pageSize->m_bodyWd,       titleHt,// width and height
                Qt::AlignVCenter|Qt::AlignHCenter,  // center alignment
                qStr );                             // display description
            yPos += titleHt;                        // move to next line

			// Display the table subitle (Fire Direction Option)
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
	        // Display the table title::portion.
            //m_composer->font( subTitleFont );     // use tableSubtitleFont
            //m_composer->pen( subTitlePen );       // use tableSubtitleFontColor
            //m_composer->text(
            //    m_pageSize->m_marginLeft,   yPos,
            //    m_pageSize->m_bodyWd,       titleHt,
            //    Qt::AlignVCenter|Qt::AlignHCenter,
            //    qStr );
            //yPos += titleHt;
            yPos += titleHt;

            // Everything else on this page is shifted.
            s = shift[pageAcross];

            // Determine left and right edges of the table.
            for ( vid = 0;
                  vid < tableVars();
                  vid++ )
            {
                if ( pageAcross == colPage[vid] )
                {
                    rightEdge = colXPos[vid] + colWd[vid] + s;
                }
            }
            bgLeft = m_pageSize->m_marginLeft + s - m_padWd ;
            bgRight = rightEdge - bgLeft + m_padWd ;

            // Display a colored row column header background?
            if ( doRowBg )
            {
                m_composer->fill(
                    bgLeft,     yPos,               // start at UL corner
                    bgRight,    3 * textHt,         // width and height
                    rowBrush );                     // color & pattern
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

            // Display the row column units
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
            for ( vid = 0;
                  vid < tableVars();
                  vid++ )
            {
                if ( tableVar(vid)->isDiagram() )
                {
                    continue;
                }
                if ( pageAcross == colPage[vid] )
                {
                    // Display the output column header0.
                    m_composer->text(
                        colXPos[vid] + s,   yPos,
                        colWd[vid],         textHt,
                        Qt::AlignVCenter|Qt::AlignRight,
                        *(tableVar(vid)->m_hdr0) );
                    // Display the output column header1.
                    m_composer->text(
                        colXPos[vid] + s,   yPos + textHt,
                        colWd[vid],         textHt,
                        Qt::AlignVCenter|Qt::AlignRight,
                        *(tableVar(vid)->m_hdr1) );
                    // Display the output column units.
                    m_composer->text(
                        colXPos[vid] + s,   yPos + 2. * textHt,
                        colWd[vid],         textHt,
                        Qt::AlignVCenter|Qt::AlignRight,
                        tableVar(vid)->displayUnits() );
                    // Display the output column underline.
                    if ( ! doRowBg )
                    {
                        m_composer->line(
                            colXPos[vid] + s,               yPos + 3.5 * textHt,
                            colXPos[vid] + colWd[vid] + s,  yPos + 3.5 * textHt );
                    }
                }
            }
            // If there are previous column pages, display a visual clue.
            if ( pageAcross > 1 )
            {
                for ( i = 0;
                      i < 3;
                      i++ )
                {
                    m_composer->text(
                        0, ( yPos + i * textHt ),
                        ( m_pageSize->m_marginLeft + s - m_padWd ), textHt,
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
            if ( rowThru >= ( tableRows() * tableCols() ) )
            {
                rowThru = tableRows() * tableCols() - 1;
            }
            // Determine the columns range to display on this page.
            int vidFrom = -1;
            int vidThru = 0;
            for ( vid = 0;
                  vid < tableVars();
                  vid++ )
            {
                if ( colPage[vid] == pageAcross )
                {
                    if ( vidFrom == -1 )
                    {
                        vidFrom = vid;
                    }
                    vidThru = vid;
                }
            }

            // Loop for each row on this page.
            bool doThisRowBg = false;
            for ( row = rowFrom;
                  row <= rowThru;
                  row ++ )
            {
                // Determine whether to hatch this row
                bool hatch = doRx && ! tableInRx(row);

                // Display a colored row background?
                if ( doRowBg && doThisRowBg )
                {
                    m_composer->fill(
                        bgLeft,     yPos,
                        bgRight,    textHt,
                        rowBrush );
                }
                doThisRowBg = ! doThisRowBg;

                // Left-most (row variable) column value
                // (which is never a diagram type variable).
                if ( rowVar->isDiscrete() )
                {
                    iid = (int) tableRow( row / tableCols() );
                    qStr = rowVar->m_itemList->itemName( iid );
                }
                else if ( rowVar->isContinuous() )
                {
					// CDB DECIMALS MOD
					if ( false )
					{
						qStr.sprintf( "%1.*f", rowVar->m_displayDecimals, tableRow( row / tableCols() ) );
					}
					else
					{
						qStr.sprintf( "%1.*f", m_rowDecimals, tableRow( row / tableCols() ) );
					}
                }
                m_composer->font( textFont );       // use tableTextFont
                m_composer->pen( textPen );         // use tableTextFontColor
                m_composer->text(
                    m_pageSize->m_marginLeft + s,   yPos,
                    rowWd,                          textHt,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qStr );

                // Loop for each output variable column on this page
                m_composer->font( valueFont );      // use tableValueFont
                m_composer->pen( valuePen );        // use tableValueFontColor
                out = vidFrom + row * tableVars();
                for ( vid = vidFrom;
                      vid <= vidThru;
                      vid++, out++ )
                {
                    varPtr = tableVar( vid );
                    // Discrete variables use their item name.
                    if ( varPtr->isDiscrete() )
                    {
                        iid = (int) tableVal( out );
                        qStr = varPtr->m_itemList->itemName( iid );
                    }
                    // Continuous vars use the current display units format.
                    else if ( varPtr->isContinuous() )
                    {
                        qStr.sprintf( " %1.*f",
                            varPtr->m_displayDecimals, tableVal( out ) );
                    }
                    // Display the output value.
                    if ( hatch && doBlank )
                    {
                        // draw nothing
                    }
                    else
                    {
                        m_composer->text(
                            colXPos[vid] + s,   yPos,
                            colWd[vid],         textHt,
                            Qt::AlignVCenter|Qt::AlignRight,
                            qStr );
                    }
                    // RX hatching
                    if ( hatch && ! doBlank && ! varPtr->isDiagram() )
                    {
                        x0 = colXPos[vid] + s - m_padWd;
                        x1 = colXPos[vid] + s + m_padWd + colWd[vid];
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
    delete[] colWd;
    delete[] colXPos;
    delete[] shift;

    // Write the spreadsheet files
    composeTable2Spreadsheet( rowVar );
    composeTable2Html( rowVar );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the minimum width (pixels) required to accomodate the
 *  variable's header and display units text.
 *
 *  \param varPtr Pointer to the EqVar variable.
 *  \param fm Reference to the font metrics to apply.
 *
 *  \return Minimum column header width in pixels.
 */

int BpDocument::headerWidth( EqVar *varPtr, const QFontMetrics &fm )
{
    int wd = 0;
    int len;
    if ( ( len = fm.width( *(varPtr->m_hdr0) ) ) > wd )
    {
        wd = len;
    }
    if ( ( len = fm.width( *(varPtr->m_hdr1) ) ) > wd )
    {
        wd = len;
    }
    if ( ( len = fm.width( varPtr->m_displayUnits ) ) > wd )
    {
        wd = len;
    }
    //wd += fm.width( "WM" );
    return( wd );
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior 1-way output HTML file.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 2: 1 continuous and 0 discrete range variables, and
 *  - range case 3: 0 continuous and 1 discrete range variables.
 *
 *  The table's left column contains the values of the \a rowVar.
 *  Results for each output variable appear in the remaining columns.
 *
 *  \param rowVar  Pointer to the table's row EqVar.
 */

void BpDocument::composeTable2Html( EqVar *rowVar )
{
    // Attempt to open the html file
    QString fileName = appFileSystem()->composerPath()
        + "/" + property()->string( "exportHtmlFile" );
    FILE *fptr = 0;
    if ( ! ( fptr = fopen( fileName, "w" ) ) )
    {
        return;
    }

    // Prescription shading?
    bool doRx = property()->boolean( "tableShading" );
    bool doBlank = property()->boolean( "tableShadingBlank" );

    // Write the header
    composeTableHtmlHeader( fptr );

    // Write the body
	QString qStr = m_eqTree->m_eqCalc->getSubtitle();
	QString subTitle = "";
	if ( qStr.length() > 0 )
	{
		subTitle = "  <h3 class=\"bp2subhead\"><b>" + qStr + "</b></h3>\n";
	}
    fprintf( fptr,
        "<!-- Begin 1-Way Output Table -->\n"
        "<p class=\"bp2\">\n%s"		// SURFACE subtitle
        "  <h3 class=\"bp2\">Results</h3>\n"
        "  <table cellpadding=\"5\" cellspacing=\"2\" border=\"0\">\n"
        "    <tr>\n",
		subTitle.latin1()
    );
    // Table headers
    fprintf( fptr,
        "      <td class=\"bp2hdr\" align=\"center\">%s<br />%s</td>\n",
        (*(rowVar->m_hdr0)).latin1(),
        (*(rowVar->m_hdr1)).latin1()
    );
    // Column headers
    int vid, row, iid, out;
    EqVar *varPtr;
    for ( vid = 0; vid < tableVars(); vid++ )
    {
        varPtr = tableVar(vid);
        if ( ! varPtr->isDiagram() )
        {
            fprintf( fptr,
                "      <td class=\"bp2hdr\" align=\"center\">%s<br />%s</td>\n",
                (*(varPtr->m_hdr0)).latin1(),
                (*(varPtr->m_hdr1)).latin1()
            );
        }
    }
    fprintf( fptr,
        "    </tr>\n"
        "    <tr>\n"
    );
    // Units
    fprintf( fptr,
        "      <td class=\"bp2hdr\" align=\"center\">%s</td>\n",
        rowVar->displayUnits().latin1() );
    for ( vid = 0; vid < tableVars(); vid++ )
    {
        varPtr = tableVar(vid);
        if ( ! varPtr->isDiagram() )
        {
            fprintf( fptr,
                "      <td class=\"bp2hdr\" align=\"center\">%s</td>\n",
                varPtr->displayUnits().latin1() );
        }
    }

    // Loop for each output row
    for ( vid=0, row = 0; row < tableRows(); row++, vid++ )
    {
        fprintf( fptr,
            "    </tr>\n"
            "    <tr>\n"
        );
        // First column is the row value
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row );
            qStr = rowVar->getItemName( iid ).latin1();
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
        fprintf( fptr,
            "      <td class=\"bp2hdr\" align=\"center\">%s</td>\n",
            qStr.latin1() );

        // Loop for each output column
        out = row * tableVars();
        for ( vid = 0; vid < tableVars(); vid++, out++ )
        {
            varPtr = tableVar( vid );
            // Skip diagram variables
            if ( varPtr->isDiagram() )
            {
                continue;
            }
            // Discrete variables use their item name.
            if ( varPtr->isDiscrete() )
            {
                iid = (int) tableVal( out );
                qStr = varPtr->m_itemList->itemName( iid );
            }
            // Continuous vars use the current display units format.
            else if ( varPtr->isContinuous() )
            {
                qStr.sprintf( " %1.*f",
                    varPtr->m_displayDecimals, tableVal( out ) );
            }
            // Display the output value.
            if ( doRx )
            {
                if ( tableInRx( row ) )
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%din\" align=\"center\">%s</td>\n",
                        row%2, qStr.latin1() );
                }
                else if ( doBlank )
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%dout\" align=\"center\">&nbsp;</td>\n",
                        row%2 );
                }
                else
                {
                    fprintf( fptr,
                        "      <td class=\"bp2row%dout\" align=\"center\">%s</td>\n",
                        row%2, qStr.latin1() );
                }
            }
            else
            {
                fprintf( fptr,
                    "      <td class=\"bp2row%d\" align=\"center\">%s</td>\n",
                    row%2, qStr.latin1() );
            }
        }
    }
    fprintf( fptr,
        "  </table>\n"
        "</p>\n"
        "<!-- End 1-Way Output Table -->\n\n"
    );

    // Write the footer
    composeTableHtmlFooter( fptr );

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior 1-way output tab-delimited spreadsheet.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 2: 1 continuous and 0 discrete range variables, and
 *  - range case 3: 0 continuous and 1 discrete range variables.
 *
 *  The table's left column contains the values of the \a rowVar.
 *  Results for each output variable appear in the remaining columns.
 *
 *  \param rowVar  Pointer to the table's row EqVar.
 */

void BpDocument::composeTable2Spreadsheet( EqVar *rowVar )
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

    // First header row
    int vid, row, iid, out;
    EqVar *varPtr;
    QString qStr;
    fprintf( fptr, "%s", (*(rowVar->m_hdr0)).latin1() );
    for ( vid = 0; vid < tableVars(); vid++ )
    {
        varPtr = tableVar(vid);
        if ( ! varPtr->isDiagram() )
        {
            fprintf( fptr, "\t%s", (*(varPtr->m_hdr0)).latin1() );
        }
    }
    fprintf( fptr, "\n" );

    // Second header row
    fprintf( fptr, "%s", (*(rowVar->m_hdr1)).latin1() );
    for ( vid = 0; vid < tableVars(); vid++ )
    {
        varPtr = tableVar(vid);
        if ( ! varPtr->isDiagram() )
        {
            fprintf( fptr, "\t%s", (*(varPtr->m_hdr1)).latin1() );
        }
    }
    fprintf( fptr, "\n" );

    // Third header row (units)
    fprintf( fptr, "%s", rowVar->displayUnits().latin1() );
    for ( vid = 0; vid < tableVars(); vid++ )
    {
        varPtr = tableVar(vid);
        if ( ! varPtr->isDiagram() )
        {
            fprintf( fptr, "\t%s", varPtr->displayUnits().latin1() );
        }
    }
    fprintf( fptr, "\n" );

    // Loop for each output row
    for ( vid=0, row = 0; row < tableRows(); row++, vid++ )
    {
        // First column is the row value
        if ( rowVar->isDiscrete() )
        {
            iid = (int) tableRow( row );
            qStr = rowVar->getItemName( iid ).latin1();
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

        // Loop for each output column
        out = row * tableVars();
        for ( vid = 0; vid < tableVars(); vid++, out++ )
        {
            varPtr = tableVar( vid );
            // Skip diagram variables
            if ( varPtr->isDiagram() )
            {
            }
            // Discrete variables use their item name.
            else if ( varPtr->isDiscrete() )
            {
                iid = (int) tableVal( out );
                qStr = varPtr->m_itemList->itemName( iid );
                fprintf( fptr, "\t%s", qStr.latin1() );
            }
            // Continuous vars use the current display units format.
            else if ( varPtr->isContinuous() )
            {
                qStr.sprintf( " %1.*f",
                    varPtr->m_displayDecimals, tableVal( out ) );
                fprintf( fptr, "\t%s", qStr.latin1() );
            }
        }
        fprintf( fptr, "\n" );
    }

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposetable2.cpp
//------------------------------------------------------------------------------

