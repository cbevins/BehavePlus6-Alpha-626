//------------------------------------------------------------------------------
/*! \file docdevicesize.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  DocDeviceSize class declaration.
 *  Note that all device dimensions are in pixels.
 */

#ifndef _DOCDEVICESIZE_H_
/*! \def _DOCDEVICESIZE_H_
    \brief Prevent redundant includes.
 */
#define _DOCDEVICESIZE_H_ 1

// Custom class declarations
class DocPageSize;

// Qt class declarations
class QPaintDevice;

//------------------------------------------------------------------------------
/*! \class DocDeviceSize docdevicesize.h
 *
 *  \brief DocDeviceSize defines the Document screen or printer device
 *  dimensions in pixels.
 */

class DocDeviceSize
{
// Public methods
public:
    DocDeviceSize( QPaintDevice *device, DocPageSize *page, double scale=1.0 ) ;
    void init( QPaintDevice *qPaintDevice, DocPageSize *docPageSize,
        double scale ) ;
    void   reset( QPaintDevice *qPaintDevicePtr, DocPageSize *docPageSizePtr,
                double scale=1.0 ) ;
    void   update( void );

// Public data members
public:
    QPaintDevice *m_device;     //!< Pointer to the screen/printer physical device
    DocPageSize  *m_page;       //!< Pointer to the DocPageSize this is based upon
    double        m_scale;      //!< Scaling factor applied to xppi and yppi
    double        m_xppi;       //!< Horizontal pixels per inch (scaled)
    double        m_yppi;       //!< Vertical pixels per inch (scaled)
    int           m_lxppi;      //!< Logical horizontal pixels per inch (UNscaled)
    int           m_lyppi;      //!< Logical vertical pixels per inch (UNscaled)
    int           m_pageHt;         //!< Page total height (pixels)
    int           m_pageWd;         //!< Page total width (pixels)
    int           m_marginBottom;   //!< Page bottom margin height (pixels)
    int           m_marginLeft;     //!< Page left margin width (pixels)
    int           m_marginRight;    //!< Page right margin width (pixels)
    int           m_marginTop;      //!< Page top margin height (pixels)
    int           m_bodyEnd;        //!< Page end-of-body vertical position (pixels)
    int           m_bodyHt;         //!< Page body height (pixels)
    int           m_bodyRight;      //!< Page end-of-body horizontal position (pixels)
    int           m_bodyWd;         //!< Page body (e.g., line) width (pixels)
    int           m_tabWd;          //!< Page tab step (pixels)
    int           m_padWd;          //!< Padding between fields (pixels)
    int           m_padHt;          //!< Padding between lines (pixels)
};

#endif

//------------------------------------------------------------------------------
//  End of docdevicesize.h
//------------------------------------------------------------------------------

