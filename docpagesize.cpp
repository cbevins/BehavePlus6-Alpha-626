//------------------------------------------------------------------------------
/*! \file docpagesize.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  DocPageSize class methods.
 *  Note that all page dimensions are in inches.
 */

#include "docpagesize.h"

//------------------------------------------------------------------------------
/*! \brief DocPageSize default constructor.
 *
 *  Creates a default 8.5"x11.0" page with 1.0" margins all around.  But all
 *  these dimensions are reduced by 0.33" to account for printer margins.
 *  Default tab spacing is 0.50", pad width is 0.10" and pad height
 *  (inter-line spacing) is 0.05".
 */

DocPageSize::DocPageSize( void )
{
    init(
        10.67,      // pageHt (in)
        8.16,       // pageWd (in)
        0.67,       // marginTop (in)
        0.67,       // marginBottom (in)
        0.67,       // marginLeft (in)
        0.67,       // marginRight (in)
        0.50,       // tabWd (in)
        0.10,       // padWd (in)
        0.05        // padHt (in)
    );
    return;
}

//------------------------------------------------------------------------------
/*! \brief DocPageSize custom constructor.
 */

DocPageSize::DocPageSize( double pageHt, double pageWd,
    double marginTop, double marginBottom, double marginLeft, double marginRight,
    double tabWd, double padWd, double padHt )
{
    init( pageHt, pageWd, marginTop, marginBottom, marginLeft, marginRight,
        tabWd, padWd, padHt );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Initializes the DocPageSize member data from the passed parameters.
 *  Called by the DocPageSize constructor or any function that needs to update
 *  all the page dimensions.
 */

void DocPageSize::init(
        double pageHt,      double pageWd,
        double marginTop,   double marginBottom,
        double marginLeft,  double marginRight,
        double tabWd,       double padWd,       double padHt )
{
    m_pageHt        = pageHt;
    m_pageWd        = pageWd;
    m_marginBottom  = marginBottom;
    m_marginLeft    = marginLeft;
    m_marginRight   = marginRight;
    m_marginTop     = marginTop;
    m_tabWd         = tabWd;
    m_padWd         = padWd;
    m_padHt         = padHt;
    update();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets a new bottom margin and updates the page's m_bodyHt and
 *  m_bodyEnd.
 *
 *  \param inches New page bottom margin (inches).
 *
 *  \return New page bottom margin (inches).
 */

double DocPageSize::setMarginBottom( double inches )
{
    m_marginBottom = inches;
    update();
    return( m_marginBottom );
}

//------------------------------------------------------------------------------
/*! \brief Sets a new left margin and updates the page's m_bodyWd.
 *
 *  \param inches New page left margin (inches).
 *
 *  \return New page left margin (inches).
 */

double DocPageSize::setMarginLeft( double inches )
{
    m_marginLeft = inches;
    update();
    return( m_marginLeft );
}

//------------------------------------------------------------------------------
/*! \brief Sets a new right margin and updates the page's m_bodyWd and
 *  m_bodyRight.
 *
 *  \param inches New page right margin (inches).
 *
 *  \return New page right margin (inches).
 */

double DocPageSize::setMarginRight( double inches )
{
    m_marginRight = inches;
    update();
    return( m_marginRight );
}

//------------------------------------------------------------------------------
/*! \brief Sets a new top margin and updates the page's m_bodyHt.
 *
 *  \param inches New page top margin (inches).
 *
 *  \return New page bottom margin (inches).
 */

double DocPageSize::setMarginTop( double inches )
{
    m_marginTop = inches;
    update();
    return( m_marginTop );
}

//------------------------------------------------------------------------------
/*! \brief Sets a new page height and the page's m_bodyHt and m_bodyEnd.
 *
 *  \param inches New page height (inches).
 *
 *  \return New page height (inches).
 */

double DocPageSize::setPageHt( double inches )
{
    m_pageHt = inches;
    update();
    return( m_pageHt );
}

//------------------------------------------------------------------------------
/*! \brief Sets a new page width and the page's m_bodyWd and m_bodyRight.
 *
 *  \param inches New page width (inches).
 *
 *  \return New page width (inches).
 */

double DocPageSize::setPageWd( double inches )
{
    m_pageWd = inches;
    update();
    return( m_pageWd );
}

//------------------------------------------------------------------------------
/*! \brief Updates the "m_body*" member data of a DocPageSize that depend upon
 *  the raw page dimension "m_page*" and "m_margin*" variables.
 */

void DocPageSize::update( void )
{
    m_bodyHt    = m_pageHt - m_marginTop - m_marginBottom;
    m_bodyWd    = m_pageWd - m_marginLeft - m_marginRight;
    m_bodyEnd   = m_pageHt - m_marginBottom;
    m_bodyRight = m_pageWd - m_marginRight;
    m_bodyLeft  = m_marginLeft;
    m_bodyStart = m_marginTop;
    return;
}

//------------------------------------------------------------------------------
//  End of docpagesize.cpp
//------------------------------------------------------------------------------

