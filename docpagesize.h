//------------------------------------------------------------------------------
/*! \file docpagesize.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  DocPageSize class declaration.
 *  Note that all page dimensions are in inches.
 */

#ifndef _DOCPAGESIZE_H_
/*! \def _DOCPAGESIZE_H_
    \brief Prevent redundant includes.
 */
#define _DOCPAGESIZE_H_ 1

//------------------------------------------------------------------------------
/*! \class DocPageSize docpagesize.h
 *
 *  \brief DocPageSize defines the Document page dimensions in inches.
 *  It includes the physical page size (m_pageHt nad m_pageWd),
 *  the page margins (m_marginBottom, m_marginTop, m_marginLeft, m_marginRight),
 *  the writeable page body size (m_bodyHt, m_bodyWd),
 *  and the end-of-writeable page positions (m_bodyEnd and m_bodyRight).
 */

class DocPageSize
{
// Public methods
public:
    DocPageSize( void ) ;
    DocPageSize( double pageHt,      double pageWd,
                 double marginTop,   double marginBottom,
                 double marginLeft,  double marginRight,
                 double tabWd,       double padWd,       double padHt ) ;
    void init(  double pageHt,      double pageWd,
                double marginTop,   double marginBottom,
                double marginLeft,  double marginRight,
                double tabWd,       double padWd,       double padHt ) ;
    double setMarginBottom( double inches ) ;
    double setMarginTop( double inches ) ;
    double setMarginLeft( double inches ) ;
    double setMarginRight( double inches ) ;
    double setPageHt( double inches ) ;
    double setPageWd( double inches ) ;
    void   update( void ) ;

// Public data elements
public:
    double  m_pageHt;           //!< Page total height (in)
    double  m_pageWd;           //!< Page total width (in)
    double  m_marginBottom;     //!< Page bottom margin height (in)
    double  m_marginLeft;       //!< Page left margin width (in)
    double  m_marginRight;      //!< Page right margin width (in)
    double  m_marginTop;        //!< Page top margin height (in)
    double  m_bodyStart;        //!< Page start-of-body vertical position (in)
    double  m_bodyEnd;          //!< Page end-of-body vertical position (in)
    double  m_bodyHt;           //!< Page body height (in)
    double  m_bodyLeft;         //!< Page start-of-body horizontal position (in)
    double  m_bodyRight;        //!< Page end-of-body horizontal position (in)
    double  m_bodyWd;           //!< Page body (e.g., line) width (in)
    double  m_tabWd;            //!< Page tab step (in)
    double  m_padWd;            //!< Padding between fields (in)
    double  m_padHt;            //!< Padding between lines (in)
};

#endif

//------------------------------------------------------------------------------
//  End of docpagesize.h
//------------------------------------------------------------------------------

