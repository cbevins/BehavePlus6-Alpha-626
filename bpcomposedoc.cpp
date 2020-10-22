//------------------------------------------------------------------------------
/*! \file bpcomposedoc.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument documentation output composer.
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
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "fuelmodel.h"
#include "parser.h"
#include "property.h"
#include "xeqapp.h"
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

void BpDocument::composeDocumentation( void )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
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
    double textHt, titleHt, valueHt;
    textHt  = ( textMetrics.lineSpacing()  + m_screenSize->m_padHt ) / yppi;
    titleHt = ( titleMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    valueHt = ( valueMetrics.lineSpacing() + m_screenSize->m_padHt ) / yppi;
    // END THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS

    double eop = m_pageSize->m_bodyEnd - textHt;

    QString pageTitle("");
    translate( pageTitle, "BpDocument:Doc:CodesUsed" );

    // Determine variable label and description minimum column widths.
    int codeWdPixels = 0;
    int descWdPixels = 0;
    int lid, iid, len;
    QString qStr, qDesc;
    EqVar *varPtr;
	FuelModel *fm = 0;
    // Loop for each input variable.
    for ( lid = 1;
          lid < leafCount();
          lid++ )
    {
        varPtr = leaf(lid);
        // Only list unmasked discrete variables
        if ( ! varPtr->isDiscrete()
            || varPtr->m_isMasked )
        {
            continue;
        }
        // Loop for each item
        for ( iid = 0;
              iid < (int) varPtr->m_itemList->count();
              iid++ )
        {
            qStr = varPtr->m_itemList->itemName(iid);
			qStr.latin1();
			if ( varPtr->m_name == "vSurfaceFuelBedModel" )
			{
				fm = m_eqApp->m_fuelModelList->fuelModelByModelName( qStr );
				qStr = QString( "%1    %2" )
					.arg( fm->m_number, 3 ).arg( fm->m_name, 7 );
			}
            if ( ( len = valueMetrics.width( qStr ) ) > codeWdPixels )
            {
                codeWdPixels = len;
            }
            qStr = *(varPtr->m_itemList->itemDesc(iid));
			qStr.latin1();
			if ( varPtr->m_name == "vSurfaceFuelBedModel" )
			{
				qStr = qStr + fm->m_desc;
			}
            if ( ( len = valueMetrics.width( qStr ) ) > descWdPixels )
            {
                descWdPixels = len;
            }
        }
    }
    // Add padding for differences in screen and printer font sizes
    int wmPad = textMetrics.width( "WM" );
    int nameWdPixels = m_screenSize->m_bodyWd;
    descWdPixels += wmPad;
    codeWdPixels += wmPad;
    // If the desc is too wide for the page, reduce the desc field width.
    if ( ( codeWdPixels
         + descWdPixels
         + 2 * m_screenSize->m_padWd ) > m_screenSize->m_bodyWd )
    {
        descWdPixels = m_screenSize->m_bodyWd
                     - codeWdPixels
                     - 2 * m_screenSize->m_padWd;
    }
    // Convert name and units widths from pixels to inches.
    double nameWd = (double) nameWdPixels / xppi;
    double codeWd = (double) codeWdPixels / xppi;
    double descWd = (double) descWdPixels / xppi;

    // Determine column offsets.
    double nameColX = m_pageSize->m_marginLeft;
    double codeColX = nameColX + m_pageSize->m_tabWd;
    double descColX = codeColX + codeWd + m_pageSize->m_padWd;

    // Open the composer and start with a new page.
    startNewPage( pageTitle, TocListOut );
    double yPos = m_pageSize->m_marginTop + titleHt;

    // Print the table header.
    m_composer->font( titleFont );                  // use tableTitleFont
    m_composer->pen( titlePen );                    // use tableTitleFontColor
    qStr = m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace();
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        Qt::AlignVCenter|Qt::AlignCenter,           // center alignment
        pageTitle );                                // display description
    yPos += titleHt;
    m_composer->text(
        m_pageSize->m_marginLeft, yPos,             // start at UL corner
        m_pageSize->m_bodyWd, titleHt,              // width and height
        Qt::AlignVCenter|Qt::AlignCenter,           // center alignment
        qStr );                                     // display description
    yPos += titleHt;

    // Draw each discrete input variable on its own line.
    QString token;
    m_composer->font( textFont );
    m_composer->pen( textPen );
    for ( lid = 1;
          lid < leafCount();
          lid++ )
    {
        varPtr = leaf(lid);
        // Only list unmasked discrete variables
        if ( ! varPtr->isDiscrete()
            || varPtr->m_isMasked )
        {
            continue;
        }
        // Get the next y position.
        if ( ( yPos += 2*textHt ) > eop )
        {
            //startNewPage( pageTitle, TocListOut );
            yPos = newWorksheetPage( textHt, TocListOut );
            m_composer->font( textFont );
            m_composer->pen( textPen );
        }
        // Write the discrete variable name.
        m_composer->font( textFont );               // use tableTextFont
        m_composer->pen( textPen );                 // use tableTextFontColor
        qStr = *(varPtr->m_label);                  // display label text
        m_composer->text(
            nameColX,   yPos,                       // start at UL corner
            nameWd,     textHt,                     // width and height
            Qt::AlignVCenter|Qt::AlignLeft,         // left justified
            qStr );                                 // display label text
        // Discrete variable codes
        Parser parser( " \t,\"", "", "" );
        parser.parse( varPtr->m_store );
        for ( int i = 0;
              i < parser.tokens();
              i++ )
        {
            // Get the next line's y position.
            if ( ( yPos += valueHt ) > eop )
            {
                yPos = newWorksheetPage( textHt, TocListOut );
                m_composer->font( valueFont );  // use tableValueFont
                m_composer->pen( valuePen );    // use tableValueFOnttColor
            }
			// Get the next token
            token = parser.token( i );
			token.latin1();
            // Display the item's code
            m_composer->font( valueFont );
            m_composer->pen( valuePen );
			if ( varPtr->m_name == "vSurfaceFuelBedModel" )
			{
				fm = m_eqApp->m_fuelModelList->fuelModelByModelName( token );
				qStr = QString( "%1    %2" )
					.arg( fm->m_number, 3 ).arg( fm->m_name, 7 );
				qDesc = fm->m_desc;
			}
			else
			{
	            iid = varPtr->m_itemList->itemIdWithName( token );
	            qStr = varPtr->m_itemList->itemName(iid);
	            qDesc = *(varPtr->m_itemList->itemDesc(iid));
			}
            m_composer->text(
                codeColX, yPos,
                codeWd,   valueHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                qStr );
            // Display the item description.
            m_composer->font( textFont );
            m_composer->pen( textPen );
            m_composer->text(
                descColX, yPos,
                descWd,   textHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                qDesc );
        }
    }
    // Be polite and stop the composer.
    m_composer->end();
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposedoc.cpp
//------------------------------------------------------------------------------

