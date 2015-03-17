//------------------------------------------------------------------------------
/*! \file bpcomposelogo.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class logo composition methods. NO LONGER USED!
 *
 *  This is a pure virtual function required by the base Document class.
 *  It used to draw the earth-air-water-fire symbols, but all the code has
 *  been #ifdef'd out since it is not used anymore.
 *
 */

// Custom include files
#include "bpdocument.h"
#include "composer.h"

// Qt include files
#include <qpen.h>

//------------------------------------------------------------------------------
/*! \brief Composes the logo at the specified offset with the specified size.
 *
 *  \attention - This method is a required re-implementation of the Document
 *  virtual method.
 *
 *  \param x0 Horizontal distance from the left edge of the page in inches.
 *  \param y0 Vertical distance from top edge of the page in inches.
 *  \param wd Width in inches.
 *  \param ht Height in inches.
 *  \param penWd Pen width in pixels.
 */

void BpDocument::composeLogo(
    double x0,
    double y0,
    double wd,
    double ht,
    int penWd )
{
    // Prevent compiler warnings by using the args
    x0 = y0 = wd = ht = 0.0;
    penWd = 0;
#ifdef DRAW_LOGO
    // Convenience locals.
    Composer *cPtr = m_composer;

    // Gray background.
    cPtr->fill( x0, y0, wd, ht, QBrush( "lightGray", Qt::SolidPattern ) );

    // Fire symbol.
    double dx = wd / 13.;
    double dy = ht / 4.;
    double x[3], y[3];
    x[0] = x0 + 1.0 * dx;
    x[1] = x0 + 2.5 * dx;
    x[2] = x0 + 4.0 * dx;
    y[0] = y0 + 3.0 * dy;
    y[1] = y0 + 1.0 * dy;
    y[2] = y0 + 3.0 * dy;
    cPtr->pen( QPen( "red", penWd, Qt::SolidLine ) );
    cPtr->line( x[0], y[0], x[1], y[1] );
    cPtr->line( x[1], y[1], x[2], y[2] );
    cPtr->line( x[2], y[2], x[0], y[0] );

    // Water symbol.
    x[0] = x0 + 3.5 * dx;
    x[1] = x0 + 5.0 * dx;
    x[2] = x0 + 6.5 * dx;
    y[0] = y0 + 1.0 * dy;
    y[1] = y0 + 3.0 * dy;
    y[2] = y0 + 1.0 * dy;
    cPtr->pen( QPen( "blue", penWd, Qt::SolidLine ) );
    cPtr->line( x[0], y[0], x[1], y[1] );
    cPtr->line( x[1], y[1], x[2], y[2] );
    cPtr->line( x[2], y[2], x[0], y[0] );

    // Earth symbol.
    cPtr->pen( QPen( "green", penWd, Qt::SolidLine ) );
    x[0] = x0 + 7.0 * dx;                       // Left x
    x[1] = 2.0 * dx;                            // Width
    y[0] = y0 + 1.0 * dy;                       // Upper y
    y[1] = 2.0 * dy;                            // Height
    cPtr->ellipse( x[0], y[0], x[1], y[1] );
    x[0] = x0 + 7.0 * dx;
    x[1] = x0 + 9.0 * dx;
    y[0] = y0 + 2.0 * dy;
    y[1] = y0 + 2.0 * dy;
    cPtr->line( x[0], y[0], x[1], y[1] );       // Horizontal
    x[0] = x0 + 8.0 * dx;
    x[1] = x0 + 8.0 * dx;
    y[0] = y0 + 1.0 * dy;
    y[1] = y0 + 3.0 * dy;
    cPtr->line( x[0], y[0], x[1], y[1] );       // Vertical

    // Air symbol.
    x[0] = x0 + 9.5 * dx;
    x[1] = x0 + 12. * dx;
    y[0] = y0 + 1.5 * dy;
    y[1] = y0 + 2.5 * dy;
    cPtr->pen( QPen( "white", penWd, Qt::SolidLine ) );
    cPtr->line( x[0], y[0], x[1], y[0] );
    cPtr->line( x[0], y[1], x[1], y[1] );
    cPtr->pen( QPen( "black", penWd, SolidLine ) );
#endif
    return;
}

//------------------------------------------------------------------------------
//  End of bpcomposelogo.cpp
//------------------------------------------------------------------------------

