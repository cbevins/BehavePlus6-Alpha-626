//------------------------------------------------------------------------------
/*! \file graphbar.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphBar class methods.
 */

// Custom include files
#include "graphbar.h"

// Qt include files
#include <qdatastream.h>

//------------------------------------------------------------------------------
/*! \brief GraphBar default constructor.
 *
 *  Creates a white bar with a black outline and no label.
 */

GraphBar::GraphBar( void ) :
    m_barPixmap( 1, 1 )
{
    // GraphBar attributes
    QBrush brush( "white", Qt::SolidPattern );
    QPen pen( "black", 1, Qt::SolidLine );
    setGraphBar( 0., 0., 0., 0., brush, pen );

    // GraphBar label attributes
    QColor color( Qt::black );
    QFont font( "Times New Roman", 12, QFont::Normal );
    setGraphBarLabel( "", 0., 0., font, color );
    unsetGraphBarPixmap();
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphBar custom constructor.
 *
 *  \param x0 GraphBar's left boundary in world coordinates.
 *  \param y0 GraphBar's bottom boundary in world coordinates.
 *  \param x1 GraphBar's right boundary in world coordinates.
 *  \param x2 GraphBar's top boundary in world coordinates.
 *  \param brush Reference to the QBrush used to fill the GraphBar.
 *  \param pen Reference to the QPen used to draw the GraphBar outline.
 */

GraphBar::GraphBar( double x0, double y0, double x1, double y1,
            const QBrush &brush, const QPen &pen ) :
    m_barPixmap( 1, 1 )
{
    // GraphBar attributes
    setGraphBar( x0, y0, x1, y1, brush, pen );

    // GraphBar label attributes
    QColor color( Qt::black );
    QFont font( "Times New Roman", 12, QFont::Normal );
    setGraphBarLabel( "", 0., 0., font, color );
    unsetGraphBarPixmap();
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphBar copy constructor.
 */

GraphBar::GraphBar( GraphBar &r )
{
    m_barX0=          r.m_barX0;
    m_barY0=          r.m_barY0;
    m_barX1=          r.m_barX1;
    m_barY1=          r.m_barY1;
    m_barBrush=       r.m_barBrush;
    m_barPen=         r.m_barPen;
    m_label=          r.m_label;
    m_labelX=         r.m_labelX;
    m_labelY=         r.m_labelY;
    m_labelFont=      r.m_labelFont;
    m_labelColor=     r.m_labelColor;
    m_labelRotate=    r.m_labelRotate;
    m_barUsePixmap=   r.m_barUsePixmap;
    m_barPixmap=      r.m_barPixmap;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the GraphBar data elements to the file stream.
 */

#ifdef _DEVELOPMENTAL_

void GraphBar::print( FILE *fptr )
{
    // setGraphBar();
    fprintf( fptr,
        "setGraphBar( m_barX0=%f, m_barY0=%f, m_barX1=%f, m_barY1=%f, "
        "m_barBrush=(color(%d,%d,%d), style=%d), m_barPen(color(%d,%d,%d), "
        "width=%d, style=%d) );\n",
        m_barX0, m_barY0, m_barX1, m_barY1,
        m_barBrush.color().red(), m_barBrush.color().green(), m_barBrush.color().blue(),
        m_barBrush.style(), m_barPen.color().red(), m_barPen.color().green(),
        m_barPen.color().blue(), m_barPen.width(), m_barPen.style() );
    // setGraphBarLabel();
    fprintf( fptr,
        "setGraphBarLabel( m_label==\"%s\", m_labelX=%f, m_labelY=%f, "
        "barF_font(%s, %d, %d), m_barColor(%d,%d,%d), m_rotate=%f );\n",
        m_label.latin1(), m_labelX, m_labelY, m_labelFont.family().latin1(),
        m_labelFont.pointSize(), m_labelFont.weight(), m_labelColor.red(),
        m_labelColor.green(), m_labelColor.blue(), m_labelRotate );
    // setGraphBarPixmap()
    if ( m_barPixmap.isNull() || ! m_barUsePixmap )
    {
        fprintf( fptr, "unsetGraphBarPixmap( NULL )\n");
    }
    else
    {
        fprintf( fptr, "setGraphBarPixmap( width=%d, height=%d );\n",
            m_barPixmap.width(), m_barPixmap.height() );
    }
    return;
}

#endif

//------------------------------------------------------------------------------
/*! \brief Sets the GraphBar parameters.
 *
 *  \param x0 GraphBar's left boundary in world coordinates.
 *  \param y0 GraphBar's bottom boundary in world coordinates.
 *  \param x1 GraphBar's right boundary in world coordinates.
 *  \param x2 GraphBar's top boundary in world coordinates.
 *  \param brush Reference to the QBrush used to fill the GraphBar.
 *  \param pen Reference to the QPen used to draw the GraphBar outline.
 */

void GraphBar::setGraphBar( double x0, double y0, double x1, double y1,
        const QBrush &brush, const QPen &pen )
{
    m_barX0    = x0;
    m_barY0    = y0;
    m_barX1    = x1;
    m_barY1    = y1;
    m_barBrush = brush;
    m_barPen   = pen;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphBar's label characteristics.
 *
 *  \param text Label text.
 *  \param x Label horizontal position in world coordinates.
 *  \param y Label vertical position in world coordinates.
 *  \param font Reference to the QFont in which the label is drawn.
 *  \param color Reference to the QColor in which the label is drawn.
 *  \param rotate Label rotation in degrees.
 */

void GraphBar::setGraphBarLabel( const QString &text, double x, double y,
    const QFont &font, const QColor &color, double rotate )
{
    m_label       = text;
    m_labelX      = x;
    m_labelY      = y;
    m_labelFont   = font;
    m_labelColor  = color;
    m_labelRotate = rotate;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphBar's background pixmap.
 *
 *  \param pixmap Reference to the QPixmap used to fill the GraphBar body.
 */

void GraphBar::setGraphBarPixmap( const QPixmap &pixmap )
{
    m_barUsePixmap = true;
    m_barPixmap = pixmap;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stops use of the GraphBar's background pixmap.
 */

void GraphBar::unsetGraphBarPixmap( void )
{
    m_barUsePixmap = false;
    m_barPixmap.resize( 1, 1);
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphBar operator<<().
 *
 *  Serializes the GraphBar to a data stream.
 *
 *  \return Reference to the output QDataStream.
 */

QDataStream& operator<<( QDataStream &s, const GraphBar &r )
{
    s << r.m_barX0
      << r.m_barY0
      << r.m_barX1
      << r.m_barY1
      << r.m_barBrush
      << r.m_barPen
      << r.m_label
      << r.m_labelX
      << r.m_labelY
      << r.m_labelFont
      << r.m_labelColor
      << r.m_labelRotate
      << (Q_INT32) r.m_barUsePixmap
      << r.m_barPixmap ;
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief GraphBar operator>>().
 *
 *  Reads a GraphBar from a data stream.
 *
 *  \return Reference to the input QDataStream.
 */

QDataStream& operator>>( QDataStream &s, GraphBar &r )
{
    s >> r.m_barX0
      >> r.m_barY0
      >> r.m_barX1
      >> r.m_barY1
      >> r.m_barBrush
      >> r.m_barPen
      >> r.m_label
      >> r.m_labelX
      >> r.m_labelY
      >> r.m_labelFont
      >> r.m_labelColor
      >> r.m_labelRotate
      >> (Q_INT32 &) r.m_barUsePixmap
      >> r.m_barPixmap ;
    return( s );
}

//------------------------------------------------------------------------------
//  End of graphbar.cpp
//------------------------------------------------------------------------------

