//------------------------------------------------------------------------------
/*! \file bpcomposepage.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
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
#include "appproperty.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocument.h"
#include "composer.h"
#include "docpagesize.h"
#include "doctabs.h"
#include "property.h"

// Qt include files
#include <qpen.h>

// 36x36 logo
#include "logo036.xpm"

//------------------------------------------------------------------------------
/*! \brief Simply calls all the diagram composition routines in turn.
 *
 *  The diagrams appear after the pages and before the graphs.
 */

void BpDocument::composeDiagrams( void )
{
    composeFireMaxDirDiagram();
    composeFireCharacteristicsDiagram();
    composeFireShapeDiagram();
    composeContainDiagram();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws a map of the current page location in relation to all the
 *  pages for a multi-page table or diagram.
 *
 *  \param dimension Maximum map dimension in either direction (inches).
 *  \param tabRows   Number of rows in the results table
 *  \param tabCols   Number of columns in the results table
 *  \param pageRows  Number of rows of result pages
 *  \param pageCols  Number of columns of result pages
 *  \param paneRows  Number of rows of panes per result page
 *  \param paneCols  Number of columns of panes per result page
 *  \param pageRow   Index number (base 0) of this page row
 *  \param pageCol   Index number (base 0) of this page column
 *
 *  Called by:
 *      -   BpDocument::composeFireMaxDirDiagram()
 *      -   BpDocument::composeFireShapeDiagram()
 *      -   BpDocument::composeTable2()
 *      -   BpDocument::composeTable3()
 */

void BpDocument::composePageMap( double dimension, int tabRows, int tabCols,
        int pageRows, int pageCols, int paneRows, int paneCols,
        int pageRow, int pageCol )
{
    // Don't draw the map if there is only one page
    if ( pageRows <= 1 && pageCols <= 1 )
    {
        return;
    }
    // Determine the current page rectangle cell dimension
    int pageMax = ( pageRows > pageCols )
                ? ( pageRows )
                : ( pageCols ) ;
    double cellDim = dimension / pageMax;
    double cellWd  = cellDim / paneCols;
    double cellHt  = cellDim / paneRows;

    // Gray background shows coverage of all diagrams
    m_composer->fill(
        m_pageSize->m_marginLeft,
        m_pageSize->m_marginTop,
        ( tabCols * cellWd ),
        ( tabRows * cellHt ),
        QBrush( "gray90", SolidPattern ) );
    // Black rectangle shows coverage of all pages (>= diagrams)
    m_composer->pen( QPen( "black" ) );
    //m_composer->rect(
    //    m_pageSize->m_marginLeft,
    //    m_pageSize->m_marginTop,
    //    ( pageCols * cellDim ),
    //    ( pageRows * cellDim ) );
    // Draw each table cell
    for ( int row=0; row<=pageRows*paneRows; row++ )
    {
        m_composer->line(
            m_pageSize->m_marginLeft,
            ( m_pageSize->m_marginTop + row * cellHt ),
            ( m_pageSize->m_marginLeft + pageCols * paneCols * cellWd ),
            ( m_pageSize->m_marginTop + row * cellHt ) );
    }
    for ( int col=0; col<=pageCols*paneCols; col++ )
    {
        m_composer->line(
            ( m_pageSize->m_marginLeft + col * cellWd ),
            m_pageSize->m_marginTop,
            ( m_pageSize->m_marginLeft + col * cellWd ),
            ( m_pageSize->m_marginTop + pageRows * paneRows * cellHt ) );
    }
    // Red rectangle shows coverage of just this page
    m_composer->pen( QPen( "red", 2 ) );
    m_composer->rect(
        m_pageSize->m_marginLeft + pageCol * cellDim,
        m_pageSize->m_marginTop  + pageRow * cellDim,
        cellDim,
        cellDim );
	// Put a big 'X' on this page
    m_composer->pen( QPen( "black", 2 ) );
	m_composer->line(
        m_pageSize->m_marginLeft + pageCol * cellDim,
        m_pageSize->m_marginTop  + pageRow * cellDim,
        m_pageSize->m_marginLeft + pageCol * cellDim + cellDim,
        m_pageSize->m_marginTop  + pageRow * cellDim + cellDim );
	m_composer->line(
        m_pageSize->m_marginLeft + pageCol * cellDim,
        m_pageSize->m_marginTop  + pageRow * cellDim + cellDim,
        m_pageSize->m_marginLeft + pageCol * cellDim + cellDim,
        m_pageSize->m_marginTop  + pageRow * cellDim );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the page background and decoration.
 *
 *  This creates a page with the following elements:
 *      -   a white background,
 *      -   a rounded-rectangle border,
 *      -   a logo, program name, and version number in upper left header.
 *
 *  Re-implemented virtual function for the BehavePlusDocument class.
 *
 *  \attention Should \b only be called by Document::startNewPage().
 */

void BpDocument::composeNewPage( void )
{
    static QPixmap logoPixmap( logo036_xpm );

    // Convenience locals.
    Composer *cPtr    = m_composer;
    DocPageSize *pPtr = m_pageSize;
    double pad = 0.1;

    QPen pen;
    QColor color;

    // Fill the blank page with the background property color.
    color.setNamedColor( appProperty()->color( "pageBackgroundColor" ) );
    cPtr->fill(
        0., 0.,
        pPtr->m_pageWd, pPtr->m_pageHt,
        QBrush( color, Qt::SolidPattern ) );

    // Draw a rounded rectangle around the page border (use round = 7).
    int borderWidth = appProperty()->integer( "pageBorderWidth" );
    if ( property()->boolean( "pageBorderActive" ) && borderWidth )
    {
        color.setNamedColor( appProperty()->color( "pageBorderColor" ) );
        pen.setColor( color );
        pen.setWidth( borderWidth );
        //int round = property()->integer( "pageBorderRound" );
        cPtr->pen( pen );
        cPtr->rect(
            pPtr->m_bodyLeft - pad,
            pPtr->m_bodyStart - pad,
            pPtr->m_bodyWd + 2*pad,
            pPtr->m_bodyHt + 2*pad,
            0.25 );
    }

    // Draw program logo in left header
    double x0 = pPtr->m_bodyLeft;
    double y0 = 2*pad;
    double ht = pPtr->m_marginTop - 4*pad;
    cPtr->pixmap( logoPixmap, x0, y0, ht, ht );

    // Use the page decoration font.
    QFont l_font( property()->string( "pageDecorationFontFamily" ),
                property()->integer( "pageDecorationFontSize" ) );
    color.setNamedColor( property()->color( "pageDecorationFontColor" ) );
    pen.setColor( color );
    pen.setWidth( 1 );
    cPtr->font( l_font );
    cPtr->pen( pen );

    // Draw program-version next to the logo in left header
    cPtr->text(
        pPtr->m_bodyLeft + 0.3,
        0.5 * pPtr->m_marginTop - pad,
        pPtr->m_bodyWd,
        0.5 * pPtr->m_marginTop,
        Qt::AlignVCenter|Qt::AlignLeft,
        QString( appWindow()->m_program + " " + appWindow()->m_version ) );

    // Draw the page number in the right header
    QString text("");
    translate( text, "Document:Page" );
    QString qstr = QString( "%1 %2" ).arg( text ).arg( m_pages );
    cPtr->text(
        pPtr->m_bodyLeft,
        0.5 * pPtr->m_marginTop - pad,
        pPtr->m_bodyWd,
        0.5 * pPtr->m_marginTop,
        Qt::AlignVCenter|Qt::AlignRight,
        qstr.latin1() );

    // Draw date stamp in center of top header
    cPtr->text(
        pPtr->m_bodyLeft,
        0.5 * pPtr->m_marginTop - pad,
        pPtr->m_bodyWd,
        0.5 * pPtr->m_marginTop,
        Qt::AlignVCenter|Qt::AlignCenter,
        m_runTime.latin1() );

    // Add a page tab?
    if ( m_tabs->active() )
    {
        composeTab();
    }

    // Reset the font before returning.
    //cPtr->font( m_propFont );
    QFont textFont( property()->string( "worksheetTextFontFamily" ),
                    property()->integer( "worksheetTextFontSize" ) );
    cPtr->font( textFont );
    return;
}

#ifdef USE_VERSION_1_PAGE_STYLE
//------------------------------------------------------------------------------
/*! \brief Composes the page background and decoration.
 *
 *  This creates a page with the following elements:
 *      -   a white background,
 *      -   a rounded-rectangle border,
 *      -   page number on the upper right header,
 *      -   program name and version number on the lower right footer, and
 *      -   logo on the lower left footer.
 *
 *  Re-implemented virtual function for the BehavePlusDocument class.
 *
 *  \attention Should \b only be called by Document::startNewPage().
 */

void BpDocument::composeNewPage( void )
{
    static QPixmap logoPixmap( logo036_xpm );

    // Here are some styles to try.
    enum PageLocation
    {
        None,
        RightHeader, LeftHeader, CenterHeader,
        RightFooter, LeftFooter, CenterFooter
    };
    PageLocation pageNumber = RightHeader;
    PageLocation runTime    = LeftHeader;
    PageLocation version    = RightFooter;
    PageLocation logo       = LeftFooter;
    bool bottomLine         = false;

    // Convenience locals.
    Composer *cPtr    = m_composer;
    DocPageSize *pPtr = m_pageSize;
    double pad = 0.1;

    // Fill the blank page with the background property color.
    QString colorName = property()->color( "pageBackgroundColor" );
    cPtr->fill(
        0., 0.,
        pPtr->m_pageWd, pPtr->m_pageHt,
        QBrush( colorName, Qt::SolidPattern ) );

    // Draw a shaded header block (probably white).
    colorName = property()->color( "pageHeaderColor" );
    cPtr->fill(
        pPtr->m_bodyLeft - pad,
        0.5 * pPtr->m_marginTop - pad,
        pPtr->m_bodyWd + 2 * pad,
        0.5 * pPtr->m_marginTop,
        QBrush( colorName, Qt::SolidPattern ) );

    // Draw a shaded footer block (probably white).
    colorName = property()->color( "pageFooterColor" );
    cPtr->fill(
        pPtr->m_bodyLeft - pad,
        pPtr->m_bodyEnd + pad,
        pPtr->m_bodyWd + 2 * pad,
        0.5 * pPtr->m_marginBottom,
        QBrush( colorName, Qt::SolidPattern ) );

    // Draw a rounded rectangle around the page border (use round = 7).
    if ( property()->boolean( "pageBorderActive" )
      && property()->integer( "pageBorderWidth" ) )
    {
        QPen pen( property()->color( "pageBorderColor" ),
                  property()->integer( "pageBorderWidth" ) );
        //int round = property()->integer( "pageBorderRound" );
        cPtr->pen( pen );
        cPtr->rect(
            pPtr->m_bodyLeft - pad,
            pPtr->m_bodyStart - pad,
            pPtr->m_bodyWd + 2*pad,
            pPtr->m_bodyHt + 2*pad,
            0.25 );
    }

    // Use the page decoration font.
    QFont l_font( property()->string( "pageDecorationFontFamily" ),
                property()->integer( "pageDecorationFontSize" ) );
    QPen pen( property()->color( "pageDecorationFontColor" ) );
    cPtr->font( l_font );
    cPtr->pen( pen );

    // Draw page number at right header.
    QString text("");
    translate( text, "Document:Page" );
    QString qstr = QString( "%1 %2" ).arg( text ).arg( m_pages );
    if ( pageNumber == RightHeader )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            0.5 * pPtr->m_marginTop - pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginTop,
            Qt::AlignVCenter|Qt::AlignRight,
            qstr.latin1() );
    }

    // Draw date stamp at left header.
    if ( runTime == LeftHeader )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            0.5 * pPtr->m_marginTop - pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginTop,
            Qt::AlignVCenter|Qt::AlignLeft,
            m_runTime.latin1() );
    }

    // Draw program name and version info at right footer.
    if ( version == RightFooter )
    {
        cPtr->text(
            pPtr->m_bodyLeft,
            pPtr->m_bodyEnd + pad,
            pPtr->m_bodyWd,
            0.5 * pPtr->m_marginBottom,
            Qt::AlignVCenter|Qt::AlignRight,
            QString( appWindow()->m_program + " " + appWindow()->m_version ) );
    }

    // Draw program logo at left footer.
    if ( logo == LeftFooter )
    {
        double x0 = pPtr->m_bodyLeft;
        double y0 = pPtr->m_bodyEnd + 2*pad;
        double ht = pPtr->m_marginBottom - 4*pad;
        //double wd = ht * 13./4.;
        //int penWd = 1;
        //composeLogo( x0, y0, wd, ht, penWd );
        cPtr->pixmap( logoPixmap, x0, y0, ht, ht );
    }

    // Draw a line at the bottom of the page?
    if ( bottomLine )
    {
        cPtr->line(
            0.,             pPtr->m_pageHt-.1,
            pPtr->m_pageWd, pPtr->m_pageHt-.1 );
    }

    // Add a page tab?
    if ( m_tabs->active() )
    {
        composeTab();
    }

    // Reset the font before returning.
    //cPtr->font( m_propFont );
    QFont textFont( property()->string( "worksheetTextFontFamily" ),
                    property()->integer( "worksheetTextFontSize" ) );
    cPtr->font( textFont );
    return;
}
#endif
//------------------------------------------------------------------------------
//  End of bpcomposepage.cpp
//------------------------------------------------------------------------------

