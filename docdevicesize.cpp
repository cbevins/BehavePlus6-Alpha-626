//------------------------------------------------------------------------------
/*! \file docpagesize.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  DocDeviceSize class methods.
 *  Note that all device dimensions are in pixels.
 */

// Custom include files
#include "docdevicesize.h"
#include "docpagesize.h"

// Qt include files
#include <qpaintdevicemetrics.h>

//------------------------------------------------------------------------------
/*! \brief DocDeviceSize default constructor.
 */

DocDeviceSize::DocDeviceSize( QPaintDevice *device, DocPageSize *page,
        double scale )
{
    init( device, page, scale );
    return;
}

//------------------------------------------------------------------------------
/*~ \brief Initializes the DocDeviceSizePriv member data based upon
 *  1 the dimensions of the physical page defined in DocPageSize, and
 *  2 the resolution of the physical QPaintDevice
 *  (usually the QApplication::desktop() or a printer device)
 *
 *  Called by the DocDeviceSizePriv constructor and whenever a new page size
 *  or display device is applied to the application.
 */

void DocDeviceSize::init( QPaintDevice *qPaintDevice, DocPageSize *docPageSize,
        double scale )
{
    m_page         = docPageSize;
    m_device       = qPaintDevice;
    m_scale        = scale;
    QPaintDeviceMetrics pdm( qPaintDevice );
    m_xppi         = (double) pdm.logicalDpiX();
    m_yppi         = (double) pdm.logicalDpiY();
    m_xppi        *= scale;
    m_yppi        *= scale;
    m_lxppi        = pdm.logicalDpiX();
    m_lyppi        = pdm.logicalDpiY();
    m_marginTop    = (int) ( m_yppi * docPageSize->m_marginTop );
    m_marginBottom = (int) ( m_yppi * docPageSize->m_marginBottom );
    m_marginLeft   = (int) ( m_xppi * docPageSize->m_marginLeft );
    m_marginRight  = (int) ( m_xppi * docPageSize->m_marginRight );
    m_padWd        = (int) ( m_xppi * docPageSize->m_padWd );
    m_padHt        = (int) ( m_yppi * docPageSize->m_padHt );
    m_pageWd       = (int) ( m_xppi * docPageSize->m_pageWd );
    m_pageHt       = (int) ( m_yppi * docPageSize->m_pageHt );
    m_tabWd        = (int) ( m_xppi * docPageSize->m_tabWd );
    update();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Allows reuse of the DocDeviceSize for a different QPaintDevice,
 *  DocPageSize, or scale factor.
 */

void DocDeviceSize::reset( QPaintDevice *qPaintDevice,
        DocPageSize *docPageSize, double scale )
{
    init( qPaintDevice, docPageSize, scale );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the "m_body*" member data of a DocDeviceSize that depend
 *  upon the raw page dimension "m_page*" and "m_margin*" variables.
 */

void DocDeviceSize::update( void )
{
    m_bodyHt    = m_pageHt - m_marginTop  - m_marginBottom;
    m_bodyWd    = m_pageWd - m_marginLeft - m_marginRight;
    m_bodyEnd   = m_pageHt - m_marginBottom;
    m_bodyRight = m_pageWd - m_marginLeft;
    return;
}

//------------------------------------------------------------------------------
//  End of docdevicesize.cpp
//------------------------------------------------------------------------------

