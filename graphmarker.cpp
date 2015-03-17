//------------------------------------------------------------------------------
/*! \file graphmarker.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphMarker class methods.
 */

// Custom include files
#include "graphmarker.h"

// Qt include files
#include <qdatastream.h>

//------------------------------------------------------------------------------
/*! \brief GraphMarker default constructor.
 */

GraphMarker::GraphMarker( void ) :
    m_x(0.),
    m_y(0.),
    m_pixmap(1,1),
    m_usePixmap(false),
    m_text(""),
    m_font("Times New Roman",12),
    m_color("black"),
    m_align(Qt::AlignLeft|Qt::AlignTop)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphMarker constructor for text markers.
 *
 *  \param x      Marker pixmap center horizontal location in world coordinates.
 *  \param y      Marker pixmap center vertical location in world coordinates.
 *  \param text   Label text
 *  \param font   Label font
 *  \param color  Label color
 *  \param align  Label alignment
 */

GraphMarker::GraphMarker( double x, double y, const QString &text,
            const QFont &font, const QColor &color, int align ) :
    m_x(x),
    m_y(y),
    m_pixmap(1,1),
    m_usePixmap(false),
    m_text(text),
    m_font(font),
    m_color(color),
    m_align(align)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphMarker parameters.
 *
 *  \param x      Marker pixmap center horizontal location in world coordinates.
 *  \param y      Marker pixmap center vertical location in world coordinates.
 *  \param text   Label text
 *  \param font   Label font
 *  \param color  Label color
 *  \param align  Label alignment
 */

void GraphMarker::setGraphMarker( double x, double y, const QString &text,
            const QFont &font, const QColor &color, int align )
{
    m_x      = x;
    m_y      = y;
    m_text   = text;
    m_font   = font;
    m_color  = color;
    m_align  = align;
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphMarker copy constructor.
 */

GraphMarker::GraphMarker( GraphMarker &r )
{
    m_x         = r.m_x;
    m_y         = r.m_y;
    m_pixmap    = r.m_pixmap;
    m_usePixmap = r.m_usePixmap;
    m_text      = r.m_text;
    m_font      = r.m_font;
    m_color     = r.m_color;
    m_align     = r.m_align;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphMarker's background pixmap.
 *
 *  \param pixmap Reference to the QPixmap painted at the GraphMarker location.
 */

void GraphMarker::setGraphMarkerPixmap( const QPixmap &pixmap )
{
    m_usePixmap = true;
    m_pixmap = pixmap;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stops use of the GraphMarker's background pixmap.
 */

void GraphMarker::unsetGraphMarkerPixmap( void )
{
    m_usePixmap = false;
    m_pixmap.resize( 1, 1);
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphMarker operator<<().
 *
 *  Serializes the GraphMarker to a data stream.
 *
 *  \return Reference to the output QDataStream.
 */

QDataStream& operator<<( QDataStream &s, const GraphMarker &r )
{
    s << r.m_x
      << r.m_y
      << r.m_pixmap
      << (Q_INT32) r.m_usePixmap
      << r.m_text
      << r.m_font
      << r.m_color
      << (Q_INT32) r.m_align;
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief GraphMarker operator>>().
 *
 *  Reads a GraphMarker from a data stream.
 *
 *  \return Reference to the input QDataStream.
 */

QDataStream& operator>>( QDataStream &s, GraphMarker &r )
{
    s >> r.m_x
      >> r.m_y
      >> r.m_pixmap
      >> (Q_INT32 &) r.m_usePixmap
      >> r.m_text
      >> r.m_font
      >> r.m_color
      >> (Q_INT32 &) r.m_align;
    return( s );
}

//------------------------------------------------------------------------------
//  End of graphmarker.cpp
//------------------------------------------------------------------------------

