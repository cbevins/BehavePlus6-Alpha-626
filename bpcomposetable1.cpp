//------------------------------------------------------------------------------
/*! \file bpcomposetable1.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument table output composer for 0 range variables.
 *
 *  Additional BehavePlusDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefiremaxdir.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposetable3.cpp
 *      - bpcomposeworksheet.cpp
 */

// Custom include files
#include "appfilesystem.h"
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
/*! \brief Composes the fire behavior simple output tables.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 1: ZERO continuous and ZERO discrete range variables.
 */

void BpDocument::composeTable1( void )
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

    // Store pixel resolution into local variables.
    double yppi = m_screenSize->m_yppi;
    double xppi = m_screenSize->m_xppi;
    double subTitleHt, textHt, titleHt, valueHt;
    subTitleHt = ( subTitleMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS

    QString results("");
    translate( results, "BpDocument:Table:Results" );

    // Determine variable label, value, and units minimum column widths.
    int nameWdPixels   = 0;
    int resultWdPixels = 0;
    int unitsWdPixels  = 0;
    int vid, len;
    QString qStr;
    EqVar *varPtr;
    // Loop for each output variable.
    for ( vid = 0;
          vid < tableVars();
          vid++ )
    {
        varPtr = tableVar(vid);
        // Label width.
        len = textMetrics.width( *(varPtr->m_label) );
        if ( len > nameWdPixels )
        {
            nameWdPixels = len;
        }
        // Units width.
        len = textMetrics.width( varPtr->m_displayUnits );
        if ( len > unitsWdPixels )
        {
            unitsWdPixels = len;
        }
        // Value width.
        if ( varPtr->isContinuous() )
        {
            qStr.sprintf( "%1.*f",
                varPtr->m_displayDecimals, tableVal(vid) );
            len = valueMetrics.width( qStr );
            if ( len > resultWdPixels )
            {
                resultWdPixels = len;
            }
        }
        else if ( varPtr->isDiscrete() )
        {
            int iid = (int) tableVal(vid);
            qStr = varPtr->m_itemList->itemName(iid);
            len = valueMetrics.width( varPtr->m_itemList->itemName(iid) );
            if ( len > resultWdPixels )
            {
                resultWdPixels = len;
            }
        }
    }
    // Add padding for differences in screen and printer font sizes
    int wmPad = textMetrics.width( "WM" );
    unitsWdPixels  += wmPad;
    nameWdPixels   += wmPad;
    resultWdPixels += valueMetrics.width( "WM" );
    // If the name is too wide for the page, reduce the name field width.
    if ( ( nameWdPixels
         + unitsWdPixels
         + resultWdPixels
         + 2 * m_screenSize->m_padWd ) > m_screenSize->m_bodyWd )
    {
        nameWdPixels = m_screenSize->m_bodyWd
                     - resultWdPixels
                     - unitsWdPixels
                     - 2 * m_screenSize->m_padWd;
    }
    // Convert name and units widths from pixels to inches.
    double resultWd = (double) resultWdPixels / xppi;
    double nameWd   = (double) nameWdPixels / xppi;
    double unitsWd  = (double) unitsWdPixels / xppi;

    // Determine offset (inches) required to horizontally center the table.
    double offsetX  = ( m_screenSize->m_bodyWd
                    - nameWdPixels
                    - resultWdPixels
                    - ( unitsWdPixels - wmPad )
                    - 2 * m_screenSize->m_padWd )
                    / ( 2. * xppi );
    // Determine column offsets.
    double nameColX   = m_pageSize->m_marginLeft + offsetX;
    double resultColX = nameColX   + nameWd   + m_pageSize->m_padWd;
    double unitsColX  = resultColX + resultWd + m_pageSize->m_padWd;

    // Open the composer and start with a new page.
    startNewPage( results, TocListOut );
    double yPos = m_pageSize->m_marginTop + titleHt;

    // Print the table header.
    m_composer->font( titleFont );                  // use tableTitleFont
    m_composer->pen( titlePen );                    // use tableTitleFontColor
    qStr = m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace();
    //if ( qStr.isNull() || qStr.isEmpty() )
    //{
    //  translate( qStr, "BpDocument:NoRunDescription" );
    //}
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        Qt::AlignVCenter|Qt::AlignCenter,           // center alignment
        qStr );                                     // display description
    yPos += titleHt;

	// Display the table subtitle (Fire Direction Option)
    m_composer->font( textFont );					// use a small-ish font ...
    m_composer->pen( subTitlePen );					// but use the title color scheme
	qStr = m_eqTree->m_eqCalc->getSubtitle();
	qStr.ascii();
	if ( qStr.length() > 0 )
	{
		m_composer->text(
			m_pageSize->m_marginLeft, yPos,             // start at UL corner
			m_pageSize->m_bodyWd, textHt,               // width and height
			Qt::AlignVCenter|Qt::AlignCenter,           // center alignment
			qStr );                                     // display description
		yPos += textHt;
	}

    // Draw each output variable on its own line.
    for ( vid = 0;
          vid < tableVars();
          vid++ )
    {
        varPtr = tableVar(vid);
        if ( varPtr->isDiagram() )
        {
            continue;
        }
        // Get the next y position.
        if ( ( yPos += textHt ) > m_pageSize->m_bodyEnd )
        {
            startNewPage( results, TocBlank );
            yPos = m_pageSize->m_marginTop;
        }
        // Write the variable name.
        m_composer->font( textFont );               // use tableTextFont
        m_composer->pen( textPen );                 // use tableTextFontColor
        qStr = *(varPtr->m_label);                  // display label text
        m_composer->text(
            nameColX,   yPos,                       // start at UL corner
            nameWd,     textHt,                     // width and height
            Qt::AlignVCenter|Qt::AlignLeft,         // left justified
            qStr );                                 // display label text
        // Continuous variable value and units.
        if ( varPtr->isContinuous() )
        {
            qStr.sprintf( "%1.*f", varPtr->m_displayDecimals, tableVal(vid) );
            m_composer->font( valueFont );          // use tableValueFont
            m_composer->pen( valuePen );            // use tableValueFontColor
            m_composer->text(
                resultColX, yPos,                   // start at UL corner
                resultWd,   valueHt,                // width and height
                Qt::AlignVCenter|Qt::AlignRight,    // right justified
                qStr );                             // display value text

            m_composer->font( textFont );           // use tableTextFont
            m_composer->pen( textPen );             // use tableTextFontColor
            m_composer->text(
                unitsColX,  yPos,                   // start at UL corner
                unitsWd,    textHt,                 // width and height
                Qt::AlignVCenter|Qt::AlignLeft,     // left justified
                varPtr->displayUnits() );           // display units text
        }
        // Discrete variable value name
        else if ( varPtr->isDiscrete() )
        {
            int id = (int) tableVal(vid);
            m_composer->font( valueFont );          // use tableValueFont
            m_composer->pen( valuePen );            // use tableValueFontColor
            m_composer->text(
                resultColX,    yPos,                // start at UL corner
                resultWd,   valueHt,                // width and height
                Qt::AlignVCenter|Qt::AlignRight,    // right justify
                varPtr->m_itemList->itemName(id) ); // display item name
        }
    }

    // Write any prescription results
    if ( false && property()->boolean( "tableShading" ) )
    {
        // Get the next y position.
        if ( ( yPos += textHt ) > m_pageSize->m_bodyEnd )
        {
            startNewPage( results, TocBlank );
            yPos = m_pageSize->m_marginTop;
        }

        // Write the prescription label
        m_composer->font( textFont );               // use tableTextFont
        m_composer->pen( textPen );                 // use tableTextFontColor
        translate( qStr, "BpDocument:Results:RxVar:Label" );
        m_composer->text(
            nameColX,   yPos,                       // start at UL corner
            nameWd,     textHt,                     // width and height
            Qt::AlignVCenter|Qt::AlignLeft,         // left justified
            qStr );                                 // display label text

        // Write the result
        translate( qStr, tableInRx(0) ?
            "BpDocument:Results:RxVar:Yes" : "BpDocument:Results:RxVar:No" );
        m_composer->font( valueFont );              // use tableValueFont
        m_composer->pen( valuePen );                // use tableValueFontColor
        m_composer->text(
            resultColX,    yPos,                    // start at UL corner
            resultWd,   valueHt,                    // width and height
            Qt::AlignVCenter|Qt::AlignRight,        // right justify
            qStr );                                 // display item name
    }
    // Be polite and stop the composer.
    m_composer->end();

    // Write the spreadsheet files
    composeTable1Spreadsheet();
    composeTable1Html();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior simple output HTML file.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 1: ZERO continuous and ZERO discrete range variables.
 */

void BpDocument::composeTable1Html( void )
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

    // Write the body
	QString qStr = m_eqTree->m_eqCalc->getSubtitle();
	QString subTitle = "";
	if ( qStr.length() > 0 )
	{
		subTitle = "  <h3 class=\"bp2subhead\"><b>" + qStr + "</b></h3>\n";
	}
	fprintf( fptr,
		"<!-- Begin Simple Output Table -->\n"
		"<p class=\"bp2\">\n%s\n"
		"  <h3 class=\"bp2\">Results</h3>\n"
		"  <table cellpadding=\"5\" cellspacing=\"2\" border=\"0\">\n"
		"    <tr>\n"
		"      <td class=\"bp2hdr\" align=\"left\">Output Variable</td>\n"
		"      <td class=\"bp2hdr\" align=\"right\">Value</td>\n"
		"      <td class=\"bp2hdr\" align=\"left\">Units</td>\n"
		"    </tr>\n",
		subTitle.latin1()
	);

    // Loop for each output variable
    QString fld1, fld2, fld3;
    EqVar *varPtr;
    for ( int vid = 0;
          vid < tableVars();
          vid++ )
    {
        varPtr = tableVar( vid );
        if ( varPtr->isDiagram() )
        {
            continue;
        }

        // First field is the variable label
        fld1 = *(varPtr->m_label);

        // Continuous variable value and units
        if ( varPtr->isContinuous() )
        {
            fld2.sprintf( "%1.*f", varPtr->m_displayDecimals, tableVal(vid) );
            fld3 = varPtr->displayUnits().latin1();
        }
        // Discrete variable value name and index
        else if ( varPtr->isDiscrete() )
        {
            int id = (int) tableVal(vid);
            fld2 = varPtr->m_itemList->itemName(id).latin1();
            fld3 = "";
        }
        // Write the record
        fprintf( fptr,
            "  <tr>\n"
            "    <td align=\"left\">%s</td>\n"
            "    <td align=\"right\">%s</td>\n"
            "    <td align=\"left\">%s</td>\n"
            "  </tr>\n",
            fld1.latin1(), fld2.latin1(), fld3.latin1() );
    }
    fprintf( fptr,
        "  </table>\n"
        "</p>\n"
        "<!-- End Simple Output Table -->\n\n"
    );

    // Write the standard footer
    composeTableHtmlFooter( fptr );

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior simple output tab-delimited spreadsheet.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 1: ZERO continuous and ZERO discrete range variables.
 */

void BpDocument::composeTable1Spreadsheet( void )
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
    QString fld1, fld2, fld3;
    EqVar *varPtr;
    for ( int vid = 0;
          vid < tableVars();
          vid++ )
    {
        varPtr = tableVar( vid );
        if ( varPtr->isDiagram() )
        {
            continue;
        }

        // First field is the variable label
        fld1 = *(varPtr->m_label);

        // Continuous variable value and units
        if ( varPtr->isContinuous() )
        {
            fld2.sprintf( "%1.*f", varPtr->m_displayDecimals, tableVal(vid) );
            fld3 = varPtr->displayUnits().latin1();
        }
        // Discrete variable value name and index
        else if ( varPtr->isDiscrete() )
        {
            int id = (int) tableVal(vid);
            fld2 = varPtr->m_itemList->itemName(id).latin1();
            fld3 = QString( "%1" ).arg( id );
        }
        // Write the record
        fprintf( fptr, "%s\t%s\t%s\n",
            fld1.latin1(), fld2.latin1(), fld3.latin1() );
    }

    // Close the file and return
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposetable1.cpp
//------------------------------------------------------------------------------

