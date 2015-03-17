//------------------------------------------------------------------------------
/*! \file graphmarker.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphMarker class declaration.
 */

#ifndef _GRAPHMARKER_H_
/*! \def _GRAPHMARKER_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPHMARKER_H_ 1

// Qt class references
#include <qcolor.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qstring.h>
class QDataStream;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class GraphMarker graphmarker.h
 *
 *  \brief GraphMarker defines one complete graph marker including world
 *  x and y coordinates, text, font, color, and pixmap.
 *
 *  \note Note that there are 2 coordinate systems in use.
 *  \arg CANVAS coordinates are pixels corresponding to the physical device.
 *  \arg WORLD coordinates map the canvas to the graph points.
 *       All GraphBar, GraphLine, and GraphMarker coordinates are
 *       in World Coordinates.
 */

class GraphMarker
{
// Public methods
public:
    GraphMarker( void ) ;
    GraphMarker( double x, double y, const QString &text, const QFont &font,
            const QColor &color, int align=Qt::AlignLeft|Qt::AlignTop ) ;
    GraphMarker( GraphMarker &source ) ;
    void setGraphMarker( double x, double y, const QString &text,
            const QFont &font, const QColor &color,
            int align=Qt::AlignLeft|Qt::AlignTop );
    void setGraphMarkerPixmap( const QPixmap &pixmap ) ;
    void unsetGraphMarkerPixmap( void ) ;

// Serialization
    friend QDataStream& operator<<( QDataStream &s, const GraphMarker &r ) ;
    friend QDataStream& operator>>( QDataStream &s, GraphMarker &r ) ;

// Public data members
public:
    double  m_x;        //!< Marker pixmap center horizontal location in world coordinates.
    double  m_y;        //!< Marker pixmap center vertical location in world coordinates.
    QPixmap m_pixmap;   //!< Reference to pixmap to center over point (m_x, m_y).
    bool    m_usePixmap;//!< TRUE if a pixmap is to be drawn
    QString m_text;     //!< Label text
    QFont   m_font;     //!< Label font
    QColor  m_color;    //!< Label color
    int     m_align;    //!< Label alignment
};

// Serialization
    QDataStream& operator<<( QDataStream &s, const GraphMarker &r ) ;
    QDataStream& operator>>( QDataStream &s, GraphMarker &r ) ;

#endif

//------------------------------------------------------------------------------
//  End of graphmarker.h
//------------------------------------------------------------------------------

