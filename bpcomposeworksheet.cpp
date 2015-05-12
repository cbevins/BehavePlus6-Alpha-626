//------------------------------------------------------------------------------
/*! \file bpcomposeworksheet.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2012 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class worksheet composition methods.
 *
 *  Additional BpDocument method definitions are in:
 *      - bpdocument.cpp
 *      - bpcomposefireshape.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposetable3.cpp
 *      - bpworksheet.cpp
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocentry.h"
#include "bpdocument.h"
#include "composer.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "property.h"
#include "rxvar.h"
#include "xeqapp.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt Include files
#include <qcheckbox.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpen.h>
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtextview.h>

//------------------------------------------------------------------------------
/*! \brief Constructs a string of the current active module names.
 *
 *  \param str Reference to a QString to contain the result.
 *
 *  \return The newly constructed string is returned in the passed \a str.
 *  The function itself returns nothing.
 */

void BpDocument::activeModules( QString &str )
{
    // All module active property keys in order of desired appearance
    static const char *Module[] =
    {
        "surfaceModuleActive",
        "crownModuleActive",
        "safetyModuleActive",
        "sizeModuleActive",
        "containModuleActive",
        "spotModuleActive",
        "scorchModuleActive",
        "mortalityModuleActive",
        "ignitionModuleActive",
        "weatherModuleActive",
        NULL
    };
    // Corresponding translator dictionary keys
    static QString ModKey[] =
    {
        "BpDocument:Module:Surface",
        "BpDocument:Module:Crown",
        "BpDocument:Module:Safety",
        "BpDocument:Module:Size",
        "BpDocument:Module:Contain",
        "BpDocument:Module:Spot",
        "BpDocument:Module:Scorch",
        "BpDocument:Module:Mortality",
        "BpDocument:Module:Ignition",
        "BpDocument:Module:Weather"
    };
    // Create a string of active module names
    QString text("");
    translate( text, "BpDocument:Module:Modules" ); // "Modules:"
    str = text + ": ";
    int nmodels = 0;
    for ( int id = 0;
          Module[id];
          id++ )
    {
        if ( property()->boolean( Module[id] ) )
        {
            if ( nmodels )
            {
                str += ", ";
            }
            translate( text, ModKey[id] );
            str += text;
            nmodels++;
        }
    }
    if ( ! nmodels )
    {
        translate( text, "BpDocument:Module:None" ); // "NONE:"
        str += text;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the BpDocument Guide Button pixmap.
 *
 *  Resizes and redraws the internal m_guidePixmap to suit the current scale.
 *
 *  Called by composeWorksheet() and showPage().
 *
 *  \param wd New pixmap width (pixels)
 *  \param ht New pixmap height (pixels)
 */

void BpDocument::composeGuideButtonPixmap( int wd, int ht )
{
    // Resize
    if ( m_guidePixmap.width() == wd
      && m_guidePixmap.height() == ht )
    {
        return;
    }
    m_guidePixmap.resize( wd, ht );
    // Fill the background
    QPainter p( &m_guidePixmap );
    QColor bg(  51, 103, 160 );
    p.fillRect( 0, 0, wd, ht, QBrush( bg ) );

    // Draw the arrow
    QColor fg( 204, 160, 160 );
    int penWd = ( ht/8 < 2 ) ? 2 : ( ht/8);
    p.setPen( QPen( fg, penWd, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin ) );
    // Arrow shaft
    p.drawLine( (int) ( 0.2 * (double) wd ), ( ht/2 ),
                (int) ( 0.8 * (double) wd ), ( ht/2 ) );
    // Upper barb
    p.drawLine( ( wd/2 ),                    (int) ( 0.2 * (double) ht ),
                (int) ( 0.8 * (double) wd ), ( ht/2 ) );
    // Lower barb
    p.drawLine( ( wd/2 ),                    (int) ( 0.8 * (double) ht ),
                (int) ( 0.8 * (double) wd ), ( ht/2 ) );
    p.end();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the BpDocument input worksheet.
 *
 *  This function performs two separate but related tasks.
 *  -# It draws all stationary text and graphics onto the scrollview.
 *  -# It determines the positions and sizes of all widgets that are
 *  displayed on top of the scrollview.  These include entry field and
 *  guide button widgets for each input variable, and possibly a fuel value
 *  initialization button and a notes section.
 *
 *  BpDocument::showPage() actually controls which widgets get displayed on
 *  the current page.
 *
 *  \par Note
 *  Remember that this function actually writes composition instructions to
 *  a Composer file.  It uses the \e screen font metrics to determine such
 *  things as line spacing and column alignment.  In the future I may need
 *  to use both the printer and screen font metrics to determine the best
 *  page locations.
 */

void BpDocument::composeWorksheet( void )
{
    // START THE STANDARD PREAMBLE USED BY ALL TABLE COMPOSITION FUNCTIONS.
    // WIN98 requires that we actually create a font here and use it for
    // font metrics rather than using the widget's font.
    QFont entryFont( property()->string( "worksheetEntryFontFamily" ),
                    property()->integer( "worksheetEntryFontSize" ) );
    QPen entryPen( property()->color( "worksheetEntryFontColor" ) );
    QFontMetrics entryMetrics( entryFont );

    QFont textFont( property()->string( "worksheetTextFontFamily" ),
                    property()->integer( "worksheetTextFontSize" ) );
    QPen textPen( property()->color( "worksheetTextFontColor" ) );
    QFontMetrics textMetrics( textFont );

    QFont titleFont( property()->string( "worksheetTitleFontFamily" ),
                    property()->integer( "worksheetTitleFontSize" ) );
    QPen titlePen( property()->color( "worksheetTitleFontColor" ) );
    QFontMetrics titleMetrics( titleFont );

    QFont valueFont( property()->string( "worksheetValueFontFamily" ),
                    property()->integer( "worksheetValueFontSize" ) );
    QPen valuePen( property()->color( "worksheetValueFontColor" ) );
    QFontMetrics valueMetrics( valueFont );

    QPen noValuePen( property()->color( "worksheetNoValueFontColor" ) );

    //---------------------------
    // 1 - Determine page metrics
    //---------------------------

    // Determine current view scale.
    //double scale = (double) m_fontScaleSize / (double) m_fontBaseSize;
    double scale = m_screenSize->m_scale;

    // Store pixel resolution into local variables.
    double yppi = m_screenSize->m_yppi;
    double xppi = m_screenSize->m_xppi;

    // Determine variable's label-units maximum width for this input set.
    int nameWdPixels = 0;
    int lid;
    for ( lid = 0;
          lid < leafCount();
          lid++ )
    {
        int len = textMetrics.width( *(leaf(lid)->m_label) )
                + textMetrics.width( leaf(lid)->m_displayUnits );
        if ( len > nameWdPixels )
        {
            nameWdPixels = len;
        }
    }
    // Take prescription variable names and units into consideration
    RxVar *rxVar = 0;
    for ( rxVar = m_eqTree->m_rxVarList->first();
          rxVar;
          rxVar = m_eqTree->m_rxVarList->next() )
    {
        // Must be a user output
        EqVar *varPtr = rxVar->m_varPtr;
        if ( varPtr->m_isUserOutput )
        {
            int len = textMetrics.width( *(varPtr->m_label) )
                    + textMetrics.width( varPtr->m_displayUnits );
            if ( len > nameWdPixels )
            {
                nameWdPixels = len;
            }
        }
    }
    nameWdPixels = (int) ( (double) nameWdPixels * scale );

    // HACK to accommodate screen vs printer font differences.
    double hackFactor = 1.15;
    nameWdPixels = (int) ( (double) nameWdPixels * hackFactor );

    // Add spacing at the end of the label-units.
    nameWdPixels += m_screenSize->m_padWd;

    // Determine button field width and height.
    m_guideBtnWd =  m_guideBtnHt = 4 + entryMetrics.lineSpacing();
    // Adjust for current scale
    int btnWd = (int) ( scale * (double) m_guideBtnWd );
    composeGuideButtonPixmap( btnWd, btnWd );
    // Determine page width available to the entry field.
    int entryWdPixels =
          m_screenSize->m_bodyWd  // page body width (pixels)
        - m_screenSize->m_tabWd   // less tab distance (pixels)
        - nameWdPixels            // less label-units width (pixels)
        - btnWd                   // less button width (pixels)
        - m_screenSize->m_padWd   // less button-to-entry padding
        - m_screenSize->m_padWd;  // less entry-to-margin padding

    // If entry width is less than 1.5 inches,
    // shrink the available name space instead.
    int minEntryWdPixels = (int) ( 1.5 * xppi );
    if ( entryWdPixels < minEntryWdPixels )
    {
        nameWdPixels -= ( minEntryWdPixels - entryWdPixels );
        entryWdPixels = minEntryWdPixels;
    }

    // Determine field offsets and widths in inches.
    double nameX   = m_pageSize->m_marginLeft + m_pageSize->m_tabWd;
    double nameWd  = (double) nameWdPixels / xppi;
    double btnX    = nameX + ( nameWdPixels + m_screenSize->m_padWd ) / xppi;
    double entryX  = btnX + ( btnWd + m_screenSize->m_padWd ) / xppi;
    double entryWd = (double) entryWdPixels / xppi;

    // RxVar positions
    int rxGapPixels = textMetrics.width( " - " );
    int rxWdPixels  = ( entryWdPixels - rxGapPixels ) / 2;
    double rxGap    = (double) rxGapPixels / xppi;
    double rxWd     = (double) rxWdPixels / xppi;
    double rxEntryX = entryX + rxWd + rxGap;
    double checkBoxWd = ( (double) textMetrics.width( "X" ) ) / xppi;
    double checkBoxHt = 0.8 * (double) textMetrics.lineSpacing() * scale / yppi;

    // Adjust the entry field height to the font size.
    //int entryHtPixels = entryMetrics.lineSpacing() + 3;
    int entryHtPixels = 3 + (int) ( scale * entryMetrics.lineSpacing() );

    // Determine vertical line spacing in pixels and inches.
    int lineHtPixels = entryHtPixels;
    if ( (int) ( scale * textMetrics.lineSpacing() ) > entryHtPixels )
    {
        entryHtPixels = (int) ( scale * textMetrics.lineSpacing() );
    }
    lineHtPixels += m_screenSize->m_padHt;
    double lineHt  = (double) lineHtPixels / yppi;

    // Vertical position of the underline drawn behind the entry field.
    double entryHt = (double) (entryHtPixels-3) / yppi;

    // Page current line vertical position (inches).
    double yPos = m_pageSize->m_marginTop + lineHt;

    // Input groups.
    int thisGroup = 0;
    int lastGroup = thisGroup;
    int thisVar   = 0;
    int active;
    QString Group[9];
    translate( Group[0], "BpDocument:Worksheet:Group:Modules" );
    translate( Group[1], "BpDocument:Worksheet:Group:Understory" );
    translate( Group[2], "BpDocument:Worksheet:Group:Overstory" );
    translate( Group[3], "BpDocument:Worksheet:Group:Moisture" );
    translate( Group[4], "BpDocument:Worksheet:Group:Weather" );
    translate( Group[5], "BpDocument:Worksheet:Group:Terrain" );
    translate( Group[6], "BpDocument:Worksheet:Group:Fire" );
    translate( Group[7], "BpDocument:Worksheet:Group:Map" );
    translate( Group[8], "BpDocument:Worksheet:Group:Suppression" );

    //----------------------------------
    // 2 - Modules section
    //----------------------------------

    // Start a new page.
    QString text("");
    translate( text, "BpDocument:InputPage" );
    startNewPage( QString( "%1 %2" ).arg( text ).arg( m_pages+1 ), TocInput );

    // Display the list of activated modules.
    m_composer->font( titleFont );              // use worksheetTitleFont
    m_composer->pen( titlePen );                // use worksheetTitleFontColor
    QString str;
    activeModules( str );
    m_composer->text(
        m_pageSize->m_marginLeft,   yPos,       // start at UL corner
        m_pageSize->m_bodyWd,       lineHt,     // width and height
        Qt::AlignVCenter|Qt::AlignLeft,         // left justification
        str );                                  // draw active modules list

    //--------------------------------------
    // 3 - Display the Run Description line.
    //--------------------------------------

    // Determine the Run Description label, button, and entry field positions
    // (this can take up all the space not used by the label text).
    int iwTextWd     = (int) ( scale * textMetrics.width( *(leaf(0)->m_label) ) );
    int btnX0Pixels  = m_screenSize->m_marginLeft
                     + m_screenSize->m_tabWd
                     + iwTextWd + m_screenSize->m_padWd;
    int descX0Pixels = btnX0Pixels
                     + btnWd
                     + m_screenSize->m_padWd;
    int descWdPixels = m_screenSize->m_bodyWd
                     - descX0Pixels
                     + m_screenSize->m_marginLeft;
    double descX0Inches = (double) descX0Pixels / xppi;
    double descWdInches = (double) descWdPixels / xppi;

    // Display the Run Description label text.
    yPos += lineHt;
    m_composer->pen( textPen );                 // use worksheetTextFont
    m_composer->font( textFont );               // use worksheetTextColorFont
    m_composer->text(
        nameX,      yPos,                       // start at UL corner
        iwTextWd,   entryHt,                    // width and height
        Qt::AlignVCenter|Qt::AlignLeft,         // left justify
        *(leaf(0)->m_label) );                  // draw description label

    // Display the Run Description entry field text.
    // Right justified if text is shorter than the entry field.
    // Left justified if text is longer than the entry field.
    int align = ( descWdPixels
                < (int) ( scale * valueMetrics.width( m_entry[0]->text() ) ) )
                ? Qt::AlignVCenter | Qt::AlignLeft
                : Qt::AlignVCenter | Qt::AlignRight;
    m_composer->font( valueFont );              // use worksheetValueFont
    m_composer->pen( valuePen );                // use worksheetValueFontColor
    m_composer->text(
        descX0Inches,
        yPos,                                   // start at UL corner
        descWdInches - 0.1,
        lineHt,                                 // width and height
        align,                                  // left or right alignment
        m_entry[0]->text() );                   // draw description text

    // Display the Run Description entry field underline.
    m_composer->font( textFont );               // use worksheetTextFont
    m_composer->pen( textPen );                 // use worksheetTextFontColor
    m_composer->line(
        descX0Inches,                           // start x position
        yPos + entryHt,                         // start y position
        descX0Inches + descWdInches - 0.1,      // end x position
        yPos + entryHt );                       // end y position

    // Store the Run Description guide button position
    // (uses the m_entryPage[0] and m_entryY[0] ).
    m_guideBtn[0]->setPixmap( m_guidePixmap ) ;
    m_guideBtnX[0] = (int) ( (double) btnX0Pixels / scale );

    // Store the Run Description entry field QLineEdit widget position
    // so that it overlays the entry field text and underline.
    m_entryPage[0] = m_pages;
    m_entryY[0]    = (int) ( yppi * yPos / scale );
    m_entryX[0]    = (int) ( (double) descX0Pixels / scale );
    m_entryWd[0]   = 1 + (int) ( (double) descWdPixels / scale );
    m_entryHt[0]   = 1 + (int) ( (double) entryHtPixels / scale );

    //---------------------------------------------
    // 4 - Display one input variable on each line.
    //---------------------------------------------

    double eop = m_pageSize->m_bodyEnd - lineHt;

    for ( lid = 1;
          lid < leafCount();
          lid++ )
    {
        // Check if we need to print the group input order heading.
        sscanf( leaf(lid)->m_inpOrder, "%d:%d:%d",
            &thisGroup, &thisVar, &active );
        if ( lastGroup != thisGroup )
        {
            // Do we need a new page?
            // (subtract lineHt to avoid orphan headings)
            if ( ( yPos + 2*lineHt ) > eop )
            {
                yPos = newWorksheetPage( lineHt );
            }
            yPos += lineHt;
            // Display the group heading.
            lastGroup = thisGroup;
            m_composer->pen( titlePen );        // use worksheetTitleFont
            m_composer->font( titleFont );      // use worksheetTitleFontColor
            m_composer->text(
                m_pageSize->m_marginLeft, yPos, // start at UL corner
                m_pageSize->m_bodyWd,  entryHt, // width and height
                Qt::AlignVCenter|Qt::AlignLeft, // left justify
                QString( Group[thisGroup] ) );  // draw group label text
            m_composer->pen( textPen );         // use worksheetTextFontColor

            // If required, store the fuel value initialization button position.
            if ( thisGroup == 1
              && m_eqTree->m_eqCalc->showInitFromFuelModelButton() )
            {
                m_btnPage[0] = m_pages;
                m_btnX[0]    = (int) ( (double) entryX * xppi /scale );
                m_btnY[0]    = (int) ( yppi * yPos / scale );
                m_btnWd[0]   = (int) ( (double) entryWdPixels / scale );
                m_btnHt[0]   = (int) ( (double) entryHtPixels / scale );
            }
        }
        // Get the next line's y position and check for page break.
        if ( ( yPos += lineHt ) > eop )
        {
            yPos = newWorksheetPage( lineHt );
        }
        // Display the variable's label text
        m_composer->font( textFont );           // use worksheetTextFont
        m_composer->pen( textPen );             // use worksheetTextFontColor
        m_composer->text(
            nameX,  yPos,                       // start at UL corner
            nameWd, entryHt,                    // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            *(leaf(lid)->m_label) );			// draw variable's label

        // Display the continuous variable's units of measure.
        if ( leaf(lid)->isContinuous() )
        {
            str = leaf(lid)->displayUnits() + " ";
            m_composer->text(
                nameX,  yPos,                   // start at UL corner
                nameWd, entryHt,                // width and height
                Qt::AlignVCenter|Qt::AlignRight,// right justify
                str );                          // draw variable's units
        }
        // Display the variable's current entry field underline.
        m_composer->line(
            entryX,                             // start x position
            yPos + entryHt,                     // start y position
            entryX + entryWd - 0.1,             // end x position
            yPos + entryHt );                   // end y position

        // Display the variable's current entry field text.
        m_composer->font( valueFont );          // use worksheetValueFont
        m_composer->pen( leaf(lid)->m_isMasked
                         ? noValuePen
                         : valuePen );
        m_composer->text(
            entryX,     yPos,                   // start at UL corner
            entryWd,    entryHt,                // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            m_entry[lid]->text() );             // display entry text

        // Store the variable's input guide position.
        m_guideBtn[lid]->setPixmap( m_guidePixmap ) ;
        m_guideBtnX[lid] = (int) ( (double) btnX * xppi / scale );

        // Store the variable's entry field position so that it overlays
        // the entry field text and underline.
        m_entryPage[lid] = m_pages;
        m_entryX[lid]    = (int) ( (double) entryX * xppi / scale );
        m_entryY[lid]    = (int) ( yppi * yPos / scale );
        m_entryWd[lid]   = 1 + (int) ( (double) entryWdPixels / scale );
        m_entryHt[lid]   = 1 + (int) ( (double) entryHtPixels / scale );

        // Add optional codes for discrete variables.
        if ( property()->boolean( "worksheetShowChoices" )
          && leaf(lid)->isDiscrete() )
        {
            m_composer->font( textFont );       // use worksheetTextFont
            m_composer->pen( textPen );         // use worksheetTextFontColor
            for ( int iid = 0;
                  iid < (int) leaf(lid)->m_itemList->count();
                  iid++ )
            {
                // Get the next line's y position.
                if ( ( yPos += lineHt ) > eop )
                {
                    yPos = newWorksheetPage( lineHt );
                    m_composer->font( textFont );// use worksheetTextFont
                    m_composer->pen( textPen ); // use worksheetTextFontColor
                }
                // Display the item code and description.
                str = leaf(lid)->m_itemList->itemName(iid)
                    + ": "
                    + *(leaf(lid)->m_itemList->itemDesc(iid));
                m_composer->text(
                    nameX + m_pageSize->m_tabWd,
                    yPos,
                    m_pageSize->m_pageWd - nameX,
                    entryHt,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    str );
            }
        }
    }   // Display the next leaf(lid) variable.

    //-------------------------------------------------------
    // 5 - Display the prescription variables (if requested).
    //-------------------------------------------------------

    if ( property()->boolean( "tableShading" ) )
    {
        // Do we need a new page?
        // (Subtract lineHt to avoid orphan headings.)
        if ( ( yPos += lineHt ) > ( eop - lineHt ) )
        {
            yPos = newWorksheetPage( lineHt );
        }
        // Otherwise display a horizontal rule.
        else
        {
            yPos += 0.5 * lineHt;
            m_composer->font( textFont );       // use worksheetTextFont
            m_composer->pen( textPen );         // use worksheetTextFontColor
            m_composer->line(
                m_pageSize->m_bodyLeft,  yPos,  // start x,y position
                m_pageSize->m_bodyRight, yPos );// end x,y position
        }
        // Display the prescription variables heading.
        if ( ( yPos += lineHt ) > eop )
        {
            yPos = newWorksheetPage( lineHt );
        }
        m_composer->pen( titlePen );            // use worksheetTitleFontColor
        m_composer->font( titleFont );          // use worksheetTitleFont
        translate( text, "BpDocument:Worksheet:RxVariables" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,     // start at UL corner
            m_pageSize->m_bodyWd,   entryHt,    // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            text );                             // draw header text

        // Display each prescription variable.
        m_composer->font( textFont );           // use worksheetTextFont
        m_composer->pen( textPen );             // use worksheetTextFontColor
        int rx = 0;
        for ( rxVar = m_eqTree->m_rxVarList->first();
              rxVar;
              rxVar = m_eqTree->m_rxVarList->next(), rx++ )
        {
            // Must be a user output
            EqVar *varPtr = rxVar->m_varPtr;
            if ( ! varPtr->m_isUserOutput )
            {
                continue;
            }
            // Do we need a new page?
            if ( rxVar->isContinuous() )
            {
                if ( ( yPos += lineHt ) > eop )
                {
                    yPos = newWorksheetPage( lineHt );
                    m_composer->font( textFont );       // use worksheetTextFont
                    m_composer->pen( textPen );         // use worksheetTextFontColor
                }
            }
            else
            {
                int items = rxVar->items();
                if ( ( yPos + items * lineHt ) > eop )
                {
                    yPos = newWorksheetPage( lineHt );
                    m_composer->font( textFont );       // use worksheetTextFont
                    m_composer->pen( textPen );         // use worksheetTextFontColor
                }
                else
                {
                    yPos += lineHt;
                }
            }
            // Display the prescription variable's label
            m_composer->font( textFont );           // use worksheetTextFont
            m_composer->pen( textPen );             // use worksheetTextFontColor
            m_composer->text(
                nameX,  yPos,                       // start at UL corner
                nameWd, entryHt,                    // width and height
                Qt::AlignVCenter|Qt::AlignLeft,     // left justify
                *(varPtr->m_label) );                // draw variable's label

            if ( rxVar->isContinuous() )
            {
                // Display the units
                m_composer->text(
                    nameX,  yPos,                       // start at UL corner
                    nameWd, entryHt,                    // width and height
                    Qt::AlignVCenter|Qt::AlignRight,    // right justify
                    varPtr->displayUnits(true) );        // draw variable's units
            }

            // Store the RxVar's checkbox and entry positions IN PIXELS
            m_rxEntryPage[rx] = m_pages;
            m_rxEntryY[rx]    = (int) ( yppi * yPos / scale ) - 3;
            m_rxEntryWd[rx]   = 1 + (int) ( (double) rxWdPixels / scale );
            m_rxEntryHt[rx]   = 1 + (int) ( (double) entryHtPixels / scale );
            // NOTE: this is the x offset of the m_rxMaxEntry!!
            m_rxEntryX[rx]    = (int) ( (double) rxEntryX * xppi / scale );

            // Display the activation checkbox
            m_composer->rect( btnX, yPos, checkBoxWd, checkBoxHt );
            if ( m_rxCheckBox[rx]->isChecked() )
            {
                m_composer->text( btnX, yPos, checkBoxWd, checkBoxHt,
                    Qt::AlignVCenter|Qt::AlignCenter, "X" );
            }
            // Display continuous variable min-max values
            if ( rxVar->isContinuous() )
            {
                // Display the variable's current entry field underline.
                m_composer->line(
                    entryX,                             // start x position
                    yPos + entryHt,                     // start y position
                    entryX + rxWd - 0.1,                // end x position
                    yPos + entryHt );                   // end y position
                m_composer->line(
                    rxEntryX,                           // start x position
                    yPos + entryHt,                     // start y position
                    rxEntryX + rxWd - 0.1,              // end x position
                    yPos + entryHt );                   // end y position

                // Display the variable's current entry field text.
                m_composer->font( valueFont );          // use worksheetValueFont
                m_composer->pen( valuePen );
                m_composer->text(
                    entryX,     yPos,                   // start at UL corner
                    rxWd,       entryHt,                // width and height
                    Qt::AlignVCenter|Qt::AlignLeft,     // left justify
                    m_rxMinEntry[rx]->text() );         // display entry text
                m_composer->text(
                    entryX+rxWd,yPos,                   // start at UL corner
                    rxGap,      entryHt,                // width and height
                    Qt::AlignVCenter|Qt::AlignCenter,   // left justify
                    " - " );                            // display entry text
                m_composer->text(
                    rxEntryX,   yPos,                   // start at UL corner
                    rxWd,       entryHt,                // width and height
                    Qt::AlignVCenter|Qt::AlignLeft,     // left justify
                    m_rxMaxEntry[rx]->text() );         // display entry text
            }
            else
            {
                int items  = rxVar->items();
                int atItem = rxVar->m_firstItemBox;
                for ( int item=0; item<items; item++, atItem++ )
                {
                    // Display the "acceptable" checkbox
                    m_composer->rect( entryX, yPos, checkBoxWd, checkBoxHt );
                    if ( m_rxItemBox[atItem]->isChecked() )
                    {
                        m_composer->text( entryX, yPos, checkBoxWd, checkBoxHt,
                            Qt::AlignVCenter|Qt::AlignCenter, "X" );
                    }
                    // Display the item label
                    QString l_name = varPtr->m_itemList->itemName( item );
                    m_composer->text(
                        entryX + 1.5*checkBoxWd, yPos,
                        2*rxWd,  entryHt,                   // width and height
                        Qt::AlignVCenter|Qt::AlignLeft,     // left justify
                        l_name );
                    // Store the checkbox widget location
                    m_rxItemY.at(atItem) = (int) ( yppi * yPos / scale ) - 3;
                    yPos += lineHt;
                }
                yPos -= lineHt;
            }
        }
    }

    //--------------------------------------------
    // 6 - Display the run options (if requested).
    //--------------------------------------------

    if ( property()->boolean( "worksheetShowRunOptions" ) )
    {
        int nOptions = 20;
        QString runOpt[20];
        runOptions( runOpt, nOptions );

        QString module("");
        // Do we need a new page?
        // (Subtract lineHt to avoid orphan headings.)
        if ( ( yPos += lineHt ) > ( eop - lineHt ) )
        {
            yPos = newWorksheetPage( lineHt );
        }
        // Otherwise display a horizontal rule.
        else
        {
            yPos += 0.5 * lineHt;
            m_composer->font( textFont );       // use worksheetTextFont
            m_composer->pen( textPen );         // use worksheetTextFontColor
            m_composer->line(
                m_pageSize->m_bodyLeft,  yPos,  // start x,y position
                m_pageSize->m_bodyRight, yPos );// end x,y position
        }
        // Display the run options heading.
        if ( ( yPos += lineHt ) > eop )
        {
            yPos = newWorksheetPage( lineHt );
        }
        m_composer->pen( titlePen );            // use worksheetTitleFontColor
        m_composer->font( titleFont );          // use worksheetTitleFont
        translate( text, "BpDocument:Worksheet:RunOptions:Caption" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,     // start at UL corner
            m_pageSize->m_bodyWd,   entryHt,    // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            text );                             // draw header text
        m_composer->font( textFont );           // use worksheetTextFont
        m_composer->pen( textPen );             // use worksheetTextFontColor

        // Display the run options
        for ( int i=0; i<nOptions; i++ )
        {
            if ( ( yPos += lineHt ) > eop )
            {
                yPos = newWorksheetPage( lineHt );
                m_composer->font( textFont );   // use worksheetTextFont
                m_composer->pen( textPen );     // use worksheetTextFontColor
            }
            if ( runOpt[i].left( 1 ) == " " )
            {
                yPos -= 0.25 * entryHt;
            }
            m_composer->text( nameX, yPos,
                ( m_pageSize->m_pageWd - nameX ), entryHt,
                Qt::AlignVCenter|Qt::AlignLeft, runOpt[i] );
        }
    }

    //---------------------------------------------
    // 7 - Display output variables (if requested).
    //---------------------------------------------

    if ( property()->boolean( "worksheetShowOutputVars" ) )
    {
        // Do we need a new page?
        // (Subtract lineHt to avoid orphan headings.)
        if ( ( yPos += lineHt ) > ( eop - lineHt ) )
        {
            yPos = newWorksheetPage( lineHt );
        }
        // Otherwise display a horizontal rule.
        else
        {
            yPos += 0.5 * lineHt;
            m_composer->font( textFont );       // use worksheetTextFont
            m_composer->pen( textPen );         // use worksheetTextFontColor
            m_composer->line(
                m_pageSize->m_bodyLeft,  yPos,  // start x,y position
                m_pageSize->m_bodyRight, yPos );// end x,y position
        }
        // Display the output variables heading.
        if ( ( yPos += lineHt ) > eop )
        {
            yPos = newWorksheetPage( lineHt );
        }
        m_composer->pen( titlePen );            // use worksheetTitleFontColor
        m_composer->font( titleFont );          // use worksheetTitleFont
        translate( text, "BpDocument:Worksheet:OutputVariables" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,     // start at UL corner
            m_pageSize->m_bodyWd,   entryHt,    // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            text );                             // draw header text

        // Display each output variable.
        m_composer->font( textFont );           // use worksheetTextFont
        m_composer->pen( textPen );             // use worksheetTextFontColor
        int nOutputs = 0;
        for ( int rid = 0;
              rid < rootCount();
              rid++ )
        {
            if ( root(rid)->isText() )
            {
                continue;
            }
            // Do we need a new page?
            if ( ( yPos += lineHt ) > eop )
            {
                yPos = newWorksheetPage( lineHt );
                m_composer->font( textFont );   // use worksheetTextFont
                m_composer->pen( textPen );     // use worksheetTextFontColor
            }
            // Display the variable's name
            QString outVarName = *(root(rid)->m_label);
            // Add units (if not "ratio" or "")
            if ( root(rid)->isContinuous() )
            {
                outVarName += "  ";
                outVarName += root(rid)->displayUnits(true);
            }
            // Add the module name
            QString modules =  m_eqApp->findProducerModule( root(rid)->m_name );
            if ( ! modules.isEmpty() )
            {
                // HACK
                if ( modules == "ignition" )
                {
                    modules = "ignite";
                }
                outVarName += "  [" + modules.upper() + "]";
            }
            // Draw it!
            m_composer->text(
                nameX,
                yPos,
                m_pageSize->m_pageWd - nameX,
                entryHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                outVarName );
            nOutputs++;
        }
        // Let them know if there are no outputs
        if ( ! nOutputs )
        {
            // Do we need a new page?
            if ( ( yPos += lineHt ) > eop )
            {
                yPos = newWorksheetPage( lineHt );
                m_composer->font( textFont );   // use worksheetTextFont
                m_composer->pen( textPen );     // use worksheetTextFontColor
            }
            translate( text, "BpDocument:Worksheet:None" ); // "None"
            m_composer->text(
                nameX,
                yPos,
                m_pageSize->m_pageWd - nameX,
                entryHt,
                Qt::AlignVCenter|Qt::AlignLeft,
                text );
        }
    }

    //-----------------------------------
    // 8 - Display the notes edit window.
    //-----------------------------------

    if ( property()->boolean( "worksheetNotesActive" ) )
    {
        // Determine height of a notes line in inches
        QFontMetrics notesMetrics( m_notes->font() );
        int    notesLineHtPx  = 1 + notesMetrics.lineSpacing();
        double notesLineHtIn  = (double) notesLineHtPx / yppi;
        // Determine height of the notes area.
        int lines = property()->integer( "worksheetNotesLines" );
        m_notesHt = 1 + (int) ( (double) lines * yppi * notesLineHtIn / scale );
        m_notes->setFixedHeight( m_notesWd );
        // Determine width of the notes area.
        m_notesWd = m_screenSize->m_bodyWd - m_screenSize->m_tabWd;
        m_notesWd = 1 + (int) ( (double) m_notesWd / scale );
        m_notes->setFixedWidth( m_notesWd );
        // Do we need a new page?
        yPos += 2 * lineHt;
        if ( ( yPos + ( m_notesHt / yppi ) ) > eop )
        {
            m_composer->font( textFont );
            yPos = newWorksheetPage( lineHt );
        }
        // Display the group heading
        m_composer->pen( titlePen );            // use worksheetTitleFontColor
        m_composer->font( titleFont );          // use worksheetTitleFont
        translate( text, "BpDocument:Worksheet:Notes" );
        m_composer->text(
            m_pageSize->m_marginLeft, yPos,     // start at UL corner
            m_pageSize->m_bodyWd,  entryHt,     // width and height
            Qt::AlignVCenter|Qt::AlignLeft,     // left justify
            text );                             // draw header text
        yPos += lineHt;

        // Store the notes area position.
        m_notesPage = m_pages;
        m_notesX    = (int) ( (double) nameX * xppi / scale );
        m_notesY    = (int) ( yppi * yPos / scale );

        // Then draw the underlying notes
        m_composer->pen( valuePen );            // use worksheetValueFontColor
        m_composer->font( textFont );
        m_composer->wraptext(
            ( nameX + 0.1 ),
            yPos,
            ( scale * m_notesWd / xppi - 0.1 ),
            ( scale * m_notesHt / yppi ),
            m_notes->text()
        );
        // Draw enclosing box only on the printer
        m_composer->screenOn( false );
        m_composer->rect(
            nameX,
            yPos,
            ( scale * m_notesWd / xppi ),
            ( scale * m_notesHt / yppi )
        );
        m_composer->screenOn( true );
        yPos += ( scale * (double) notesMetrics.lineSpacing() / yppi );
    }

    //------------
    // 8 - Cleanup
    //------------

    // Be polite and stop the composer.
    m_composer->end();

    // Make this the active ToC item and show it.
    m_worksheetPages = m_pages;
    showPage( 1 );
    focusThis( 0 );

    // Mark the worksheet as unedited and return.
    m_worksheetEdited = false;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the standard components of a new worksheet page.
 *
 *  \param lineHt height of a standard text line (inches).
 *
 *  \return Vertical position to start darwing on the new page (inches).
 */

double BpDocument::newWorksheetPage( double lineHt, TocType tocType )
{
    // Display the continuation message
    QString text("");
    if ( m_pages > 0 )
    {
        translate( text, "BpDocument:Worksheet:Continued" );
        m_composer->text(
            m_pageSize->m_marginLeft,
            m_pageSize->m_bodyEnd - lineHt,
            m_pageSize->m_bodyWd,
            lineHt,
            Qt::AlignVCenter|Qt::AlignHCenter,
            text );
    }
    // Create a new page with decorations and add it to the ToC
    translate( text, "BpDocument:InputPage" );
    startNewPage( QString( "%1 %2" ).arg( text ).arg( m_pages+1 ), tocType );

    // Display the continuation message
    translate( text, "BpDocument:Worksheet:InputWorksheetContinued" );
    m_composer->text(
        m_pageSize->m_marginLeft,
        m_pageSize->m_marginTop,
        m_pageSize->m_bodyWd,
        lineHt,
        Qt::AlignVCenter|Qt::AlignHCenter,
        text );
    // Restore the font and return the yPos
    return( m_pageSize->m_marginTop + lineHt );
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior HTML file footer.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 1: ZERO continuous and ZERO discrete range variables.
 */

void BpDocument::composeTableHtmlFooter( FILE *fptr )
{
    fprintf( fptr,
        "<!-- Begin Footer Template -->\n"
        "        </td>\n"
        "        <td>&nbsp;</td>\n"
        "      </tr>"
        "    </table>"
        "    <h3 class=\"bp2\">End</h3>\n"
        "  </body>\n"
        "<!-- End Footer Template -->\n"
        "</html>\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the fire behavior HTML file header.
 *
 *  Tables are constructed for the following range cases:
 *  - range case 1: ZERO continuous and ZERO discrete range variables.
 */

void BpDocument::composeTableHtmlHeader( FILE *fptr )
{
    // Header meta data
    fprintf( fptr,
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
        "<!-- Begin Header Template -->\n"
        "<html>\n"
        "  <head>\n"
        "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" >\n"
        "    <meta name=\"Author\" content=\"BehavePlus6\">\n"
        "    <title>%s</title>\n"	// Use description as page title
        "    <style>\n"
        "      h3.bp2 { background-color: #9999ff; }\n"
        "      h3.bp2subhead { background-color: #ccccff; }\n"
        "      p.bp2  { background-color: #ffffff; }\n"
        "      td.bp2input { background-color: #cccccc; }\n"
        "      td.bp2masked { background-color: #ccccff; }\n"
        "      td.bp2hdr { background-color: #ccccff; }\n"
        "      td.bp2row0 { background-color: #ffffff; }\n"
        "      td.bp2row1 { background-color: #cccccc; }\n"
        "      td.bp2row0in { background-color: #ffffff; color: black }\n"
        "      td.bp2row1in { background-color: #cccccc; color: black }\n"
        "      td.bp2row0out { background-color: #ffffff; color: red }\n"
        "      td.bp2row1out { background-color: #cccccc; color: red }\n"
        "      span.bp2grp { font-weight: bold; color: #0000ff }\n"
        "    </style>\n"
        "  </head>\n"
        "  <body>\n"
        "    <table width=\"100%%\">\n"
        "      <tr>\n"
        "        <td width=\"90\" rowspan=\"2\">\n"
//        "          <img src=\"data:image/png;base64,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\""
//        "            width=\"90\" height=\"90\" alt=\"BehavePlus\" />"
        "        </td>\n"
        "        <td colspan=\"2\" align=\"center\" class=\"bp2input\">\n"
        "          %s %s (%s)\n"		// Program, version, (build)
        "        </td>\n"
        "      </tr>\n"
        "      <tr>\n"
        "        <td colspan=\"2\" align=\"center\">\n"
		"          <b>%s</b><br />%s<br />%s\n"	// Description, subtitle, run time
        "        </td>\n"
        "      </tr>\n"
        "      <tr>\n"
        "        <td>&nbsp;</td>\n"
        "        <td>\n"
        "<!-- End Header Template -->\n\n",
        m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace().latin1(),
        //appFileSystem()->imagePath( "logo90x90.png" ).latin1(),
        appWindow()->m_program.latin1(),
        appWindow()->m_version.latin1(),
        appWindow()->m_build.latin1(),
        m_eqTree->m_eqCalc->docDescriptionStore().stripWhiteSpace().latin1(),
		m_eqTree->m_eqCalc->getSubtitle().latin1(),
        m_runTime.latin1()
    );

    // Input Worksheet
    QString text;
    activeModules( text );
    fprintf( fptr,
        "<!-- Begin Input Worksheet -->\n"
        "<p class=\"bp2\">\n"
        "  <h3 class=\"bp2\">Input Worksheet</h3>\n"
        "  <table cellspacing=\"2\" cellpadding=\"5\" border=\"0\" width=\"100%%\">\n"
        "    <tr>\n"
        "      <td align=\"left\" colspan=\"4\">\n"
        "        <span class=\"bp2grp\">%s</span>\n"
        "      </td>\n"
        "    </tr>\n"
        "    <tr>\n"
        "      <td class=\"bp2hdr\" align=\"left\" colspan=\"2\">Input Variables</td>\n"
        "      <td class=\"bp2hdr\" align=\"right\">Units</td>\n"
        "      <td class=\"bp2hdr\" align=\"left\">Input Value(s)</td>\n"
        "    </tr>\n",
        text.latin1()
    );

    QString Group[9];
    translate( Group[0], "BpDocument:Worksheet:Group:Modules" );
    translate( Group[1], "BpDocument:Worksheet:Group:Understory" );
    translate( Group[2], "BpDocument:Worksheet:Group:Overstory" );
    translate( Group[3], "BpDocument:Worksheet:Group:Moisture" );
    translate( Group[4], "BpDocument:Worksheet:Group:Weather" );
    translate( Group[5], "BpDocument:Worksheet:Group:Terrain" );
    translate( Group[6], "BpDocument:Worksheet:Group:Fire" );
    translate( Group[7], "BpDocument:Worksheet:Group:Map" );
    translate( Group[8], "BpDocument:Worksheet:Group:Suppression" );
    int thisGroup = 0;
    int lastGroup = thisGroup;
    int thisVar   = 0;
    int active;
    QString units;
    for ( int lid = 1; lid < leafCount(); lid++ )
    {
        // Check if we need to print the group input order heading.
        sscanf( leaf(lid)->m_inpOrder, "%d:%d:%d",
            &thisGroup, &thisVar, &active );
        if ( lastGroup != thisGroup )
        {
            lastGroup = thisGroup;
            fprintf( fptr,
                "  <tr>\n"
                "    <td align=\"left\" colspan=\"4\">\n"
                "      <span class=\"bp2grp\">%s</span>\n"
                "    </td>\n"
                "  </tr>\n",
                QString( Group[thisGroup] ).latin1()
            );
        }

        // Display the variable's label, value, and units
        units = ( leaf(lid)->isContinuous() )
              ? leaf(lid)->displayUnits()
              : "&nbsp;" ;
        text  = ( leaf(lid)->m_isMasked )
              ? "bp2masked"
              : "bp2input" ;
        fprintf( fptr,
            "  <tr>\n"
            "    <td width=\"10\">&nbsp;</td>\n"
            "    <td align=\"left\" valign=\"top\">%s</td>\n"
            "    <td align=\"right\" valign=\"top\">%s</td>\n"
            "    <td class=\"%s\" align=\"left\">%s</td>\n"
            "  </tr>\n",
            (*(leaf(lid)->m_label)).latin1(),
            units.latin1(),
            text.latin1(),
            m_entry[lid]->text().latin1()
        );
    }
    // Notes
    if ( property()->boolean( "worksheetNotesActive" ) )
    {
        // Display the group heading
        translate( text, "BpDocument:Worksheet:Notes" );
        fprintf( fptr,
            "  <tr>\n"
            "    <td align=\"left\" colspan=\"4\">\n"
            "      <span class=\"bp2grp\">%s</span>\n"
            "    </td>\n"
            "  </tr>\n",
            text.latin1()
        );
        // Display the notes
        text = m_notes->text();
        text.replace( QRegExp( "\n" ), "<br />" );
        fprintf( fptr,
            "  <tr>\n"
            "    <td width=\"10\">&nbsp;</td>\n"
            "    <td align=\"left\" colspan=\"3\">%s</td>\n"
            "  </tr>\n",
            text.latin1()
        );
    }
    // End of inputs table
    fprintf( fptr,
        "  </table>\n"
        "</p>\n"
        "<!-- End Input Worksheet -->\n\n"
    );

    // Run options
    if ( property()->boolean( "worksheetShowRunOptions" ) )
    {
        int nOptions = 40;
        QString runOpt[40];
        runOptions( runOpt, nOptions );

        // Display the run options heading.
        translate( text, "BpDocument:Worksheet:RunOptions:Caption" );
        fprintf( fptr,
            "<!-- Begin Run Options -->\n"
            "<h3 class=\"bp2\">%s</h3>\n"
            "<p class=\"bp2\">\n"
            "  <table cellspacing=\"2\" cellpadding=\"5\" border=\"0\" width=\"100%%\">\n",
            text.latin1()
        );

        // Display the run options
        for ( int i=0; i<nOptions; i++ )
        {
            if ( i == 0 )
            {
                fprintf( fptr, "<tr><td>%s", runOpt[i].latin1() );
            }
            else if ( runOpt[i].left( 1 ) != " " )
            {
                fprintf( fptr, "</td></tr><tr><td>%s", runOpt[i].latin1() );
            }
            else
            {
                fprintf( fptr, "%s", runOpt[i].latin1() );
            }
        }
        if ( nOptions )
        {
            fprintf( fptr, "</td></tr>" );
        }
    }
    fprintf( fptr,
        "  </table>\n"
        "</p>\n"
        "<!-- End Input Worksheet -->\n\n"
    );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the standard components of a new worksheet page.
 *
 *  \param runOpt   Pointer to an array of QString that on return contains
 *                  Run Option Notes text.
 *  \param nOptions On input, contains the size of runOpt array.
 *                  On return, contains number of runOpts.
 */

void BpDocument::runOptions( QString* runOpt, int& nOptions )
{
    // Initialize
    for ( int i=0; i<nOptions; i++ )
    {
        runOpt[i] = "";
    }
    nOptions = 0;
	bool directionOptions = true;		// Build 607, Pat always wants these displayed

    // Surface module run options
    if ( property()->boolean( "surfaceModuleActive" ) )
    {
        if ( ! property()->boolean( "sizeModuleActive" )
          && ! property()->boolean( "containModuleActive" )
          && ! property()->boolean( "safetyModuleActive" )
          && ! property()->boolean( "spotModuleActive" )
          && ! property()->boolean( "mortalityModuleActive" )
          && ! property()->boolean( "scorchModuleActive" )
          && ! property()->boolean( "surfaceConfFuelPalmettoGallberry" )
          && ! property()->boolean( "surfaceCalcFireCharacteristicsDiagram" )
          && ! property()->boolean( "surfaceCalcFireDist" )
          && ! property()->boolean( "surfaceCalcFireEffWind" )
          && ! property()->boolean( "surfaceCalcFireFlameLeng" )
          && ! property()->boolean( "surfaceCalcFireHeatPerUnitArea" )
          && ! property()->boolean( "surfaceCalcFireLineInt" )
          && ! property()->boolean( "surfaceCalcFireMaxDirDiagram" )
          && ! property()->boolean( "surfaceCalcFireMaxDirFromUpslope" )
          && ! property()->boolean( "surfaceCalcFireReactionInt" )
          && ! property()->boolean( "surfaceCalcFireSpread" )
          && ! property()->boolean( "surfaceCalcFireWindSpeedFlag" )
          && ! property()->boolean( "surfaceCalcFireWindSpeedLimit" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDead1" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDead10" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadDeadFoliage" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLitter" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLive1" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLive10" )
          && ! property()->boolean( "surfaceCalcPalmettoLoadLiveFoliage" )
          && ! property()->boolean( "surfaceCalcPalmettoBedDepth" )
          && ! property()->boolean( "surfaceCalcWindAdjFactor" )
          && ! property()->boolean( "surfaceCalcWindSpeedAtMidflame" ) )
        {
            // nothing
        }
        else
        {
            // Maximum reliable wind speed.
            if ( property()->boolean( "surfaceConfWindLimitApplied" )
            && ! property()->boolean( "surfaceConfFuelAspen" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:Wind:Limit:Applied" );
            }
            else
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:Wind:Limit:NotApplied" );
            }
			// 2 fuel model options
            if ( property()->boolean( "surfaceConfFuel2Dimensional" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:2Dimensional" );
            }
            else if ( property()->boolean( "surfaceConfFuelHarmonicMean" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:HarmonicMean" );
            }
            else if ( property()->boolean( "surfaceConfFuelAreaWeighted" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:TwoFuel:AreaWeighted" );
            }
			// Special case fuel model options
            else if ( property()->boolean( "surfaceConfFuelPalmettoGallberry" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:PG1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:PG2" );
            }
            else if ( property()->boolean( "surfaceConfFuelAspen" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Aspen1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Aspen2" );
            }
            else if ( property()->boolean( "surfaceConfFuelChaparral" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Chaparral1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpecialFuel:Chaparral2" );
            }
            // Directions Page
            if ( property()->boolean( "surfaceConfSpreadDirHead" ) )	// Option 1 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirHead" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirBack" ) ) // Option 2 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirBack" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirFlank" ) ) // Option 3 of 6
			{
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirFlank" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirFireFront" ) ) // Option 4 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirFireFront" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirPointSourcePsi" ) ) // Option 5 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi2" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourcePsi3" );
			}
            else if ( property()->boolean( "surfaceConfSpreadDirPointSourceBeta" ) ) // Option 6 of 6
			{
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta2" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SpreadDirPointSourceBeta3" );
			}
			// Deprecated in V6
            else if ( property()->boolean( "surfaceConfSpreadDirMax" ) )
            {
				directionOptions = true;
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:MaxSpreadDir" );
                //translate( runOpt[nOptions++],
                //    "BpDocument:Worksheet:RunOptions:SpreadCalculations1" );
                //translate( runOpt[nOptions++],
                //  "BpDocument:Worksheet:RunOptions:SpreadCalculations2" );
            }
			// Deprecated in V6
            else // if ( property()->boolean( "surfaceConfSpreadDirInput" ) )
            {
				directionOptions = true;
                translate( runOpt[nOptions++],
                  "BpDocument:Worksheet:RunOptions:SpecifiedSpreadDir" );
                //translate( runOpt[nOptions++],
                //  "BpDocument:Worksheet:RunOptions:SpreadCalculations1" );
                //translate( runOpt[nOptions++],
				//  "BpDocument:Worksheet:RunOptions:SpreadCalculations2" );
			}
			if ( directionOptions )
			{
                if ( property()->boolean( "surfaceConfWindDirInput" ) )
                {
					translate( runOpt[nOptions++],
						"BpDocument:Worksheet:RunOptions:WindSpecifiedDirections" );
                    if ( property()->boolean( "surfaceConfDegreesWrtUpslope" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindTo" );
                        }
                    else // if ( property()->boolean( "surfaceConfDegreesWrtNorth" ) )
                    {
                       translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromNorth" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindFrom" );
                    }
                }
                else // if ( property()->boolean( "surfaceConfWindDirUpslope" ) )
                {
                    translate( runOpt[nOptions++],
                        "BpDocument:Worksheet:RunOptions:WindUpslope" );
                    if ( property()->boolean( "surfaceConfDegreesWrtUpslope" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromUpslope" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindTo" );
                    }
                    else // if ( property()->boolean( "surfaceConfDegreesWrtNorth" ) )
                    {
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindSpreadFromNorth" );
                        translate( runOpt[nOptions++],
                            "BpDocument:Worksheet:RunOptions:WindFrom" );
                    }
                }
			}
            // Linked to Safety Module
            if ( property()->boolean( "safetyModuleActive" ) )
            {
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SurfaceSafety1" );
                translate( runOpt[nOptions++],
                    "BpDocument:Worksheet:RunOptions:SurfaceSafety2" );
            }   // End of safetyModuleActive
        }
    } // End surfaceModuleActive

    // Safety Module run options
    if ( property()->boolean( "safetyModuleActive" ) )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:SafetyFlameLeng" );
    }

    // Contain module run options
    if ( property()->boolean( "containModuleActive" ) )
    {
        // Single or Multiple resources option
        if ( property()->boolean( "containConfResourcesSingle" ) )
        {
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Single1" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Single2" );
        }
        else
        {
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple1" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple2" );
            translate( runOpt[nOptions++],
                "BpDocument:Worksheet:RunOptions:Contain:Multiple3" );
        }
    }

    // Module Dialog - Map distances run option
    if ( property()->boolean( "mapCalcDist" ) )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:CalcMapDist" );
    }

    // If there are no options to display, say so.
    if ( ! nOptions )
    {
        translate( runOpt[nOptions++],
            "BpDocument:Worksheet:RunOptions:None" );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposeworksheet.cpp
//------------------------------------------------------------------------------

