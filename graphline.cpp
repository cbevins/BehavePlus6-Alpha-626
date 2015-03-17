//------------------------------------------------------------------------------
/*! \file graphline.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphLine class methods.
 */

// Custom include files
#include "appmessage.h"
#include "graphline.h"

// Qt include files
#include <qdatastream.h>

//------------------------------------------------------------------------------
/*! \brief GraphLine default constructor.
 *
 *  Creates a black line with no points.
 */

GraphLine::GraphLine( void ) :
    m_x(0),
    m_y(0),
    m_points(0),
    m_linePen("black",1,Qt::SolidLine),
    m_label(""),
    m_labelX(0),
    m_labelY(0),
    m_labelFont("Times New Roman",12),
    m_labelColor("black")
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphLine custom constructor.
 *
 *  \param points Number of pints in thew x[] and y[] arrays.
 *  \param x Array of x values in world coordinates.
 *  \param y Array of y values in world coordinates.
 *  \param pen Reference to the QPen used to draw the line.
 */

GraphLine::GraphLine( int points, double *x, double *y, const QPen &pen ) :
    m_x(0),
    m_y(0),
    m_points(0),
    m_label(""),
    m_labelX(0),
    m_labelY(0),
    m_labelFont("Times New Roman",12),
    m_labelColor("black")
{
    setGraphLine( points, x, y, pen );
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphLine copy constructor.
 */

GraphLine::GraphLine( GraphLine &s )
{
    m_points     = s.m_points;
    m_linePen    = s.m_linePen;
    m_label      = s.m_label;
    m_labelX     = s.m_labelX;
    m_labelY     = s.m_labelY;
    m_labelFont  = s.m_labelFont;
    m_labelColor = s.m_labelColor;
    m_x = new double[ m_points ];
    checkmem( __FILE__, __LINE__, m_x, "double m_x", m_points );
    m_y = new double[ m_points ];
    checkmem( __FILE__, __LINE__, m_y, "double m_y", m_points );
    for ( int i = 0;
          i < m_points;
          i++ )
    {
        m_x[i] = s.m_x[i];
        m_y[i] = s.m_y[i];
    }
    return;
}

//------------------------------------------------------------------------------
/*! GraphLine destructor.
 */

GraphLine::~GraphLine( void )
{
    delete[] m_x;   m_x = 0;
    delete[] m_y;   m_y = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the GraphLine data elements to the file stream.
 */

#ifdef _DEVELOPMENTAL_

void GraphLine::print( FILE *fptr, bool printPoints )
{
    // setGraphLine();
    fprintf( fptr,
        "setLine( m_points=%d, m_pen(color(%d,%d,%d), width=%d, style=%d) );\n",
        m_points, m_linePen.color().red(), m_linePen.color().green(),
        m_linePen.color().blue(), m_linePen.width(), m_linePen.style() );
    if ( printPoints )
    {
        for ( int i = 0;
              i < m_points;
              i++ )
        {
            fprintf( fptr, "        %04d: %f %f\n", i, m_x[i], m_y[i] );
        }
    }
    // setGraphLineLabel();
    fprintf( fptr,
        "setGraphLineLabel( m_label=\"%s\", m_labelX=%f, m_labelY=%f, "
        "m_labelFont(%s, %d, %d), m_labelColor(%d,%d,%d) );\n",
        m_label.latin1(), m_labelX, m_labelY, m_labelFont.family().latin1(),
        m_labelFont.pointSize(), m_labelFont.weight(), m_labelColor.red(),
        m_labelColor.green(), m_labelColor.blue() );
    return;
}

#endif

//------------------------------------------------------------------------------
/*! \brief Sets the GraphLine parameters.
 *
 *  Note that a deep copy of the x[] and y[] arrays are created.
 *
 *  \param points Number of pints in thew x[] and y[] arrays.
 *  \param x Array of x values in world coordinates.
 *  \param y Array of y values in world coordinates.
 *  \param pen Reference to the QPen used to draw the line.
 */

void GraphLine::setGraphLine( int points, double *x, double *y, const QPen &pen )
{
    // Make sure this isn't a second call
    if ( m_points || m_x || m_y )
    {
        delete[] m_x;   m_x = 0;
        delete[] m_y;   m_y = 0;
    }
    m_linePen = pen;
    m_points  = points;
    m_x       = new double[m_points];
    checkmem( __FILE__, __LINE__, m_x, "double m_x", m_points );
    m_y       = new double[m_points];
    checkmem( __FILE__, __LINE__, m_y, "double m_y", m_points );
    for ( int i = 0;
          i < m_points;
          i++ )
    {
        m_x[i] = x[i];
        m_y[i] = y[i];
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphLine's label characteristics.
 *
 *  \param text Label text.
 *  \param x Label horizontal position in world coordinates.
 *  \param y Label vertical position in world coordinates.
 *  \param font Reference to the QFont in which the label is drawn.
 *  \param color Reference to the QColor in which the label is drawn.
 */

void GraphLine::setGraphLineLabel( const QString &text, double x, double y,
    const QFont &font, const QColor &color )
{
    m_label      = text;
    m_labelX     = x;
    m_labelY     = y;
    m_labelFont  = font;
    m_labelColor = color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphLine operator<<().
 *
 *  Serializes the GraphLine to a data stream.
 *
 *  \return Reference to the output QDataStream.
 */

QDataStream& operator<<( QDataStream &s, const GraphLine &r )
{
    s << r.m_points
      << r.m_linePen
      << r.m_label
      << r.m_labelX
      << r.m_labelY
      << r.m_labelFont
      << r.m_labelColor ;
    // Write out al the points
    for ( int i = 0;
          i < r.m_points;
          i++ )
    {
        s << r.m_x[i] << r.m_y[i] ;
    }
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief GraphLine operator>>().
 *
 *  Reads a GraphLine from a data stream.
 *
 *  \return Reference to the input QDataStream.
 */

QDataStream& operator>>( QDataStream &s, GraphLine &r )
{
    s >> r.m_points
      >> r.m_linePen
      >> r.m_label
      >> r.m_labelX
      >> r.m_labelY
      >> r.m_labelFont
      >> r.m_labelColor ;

    // Allocate points array and read them in
    r.m_x = new double[ r.m_points ];
    checkmem( __FILE__, __LINE__, r.m_x, "double r.m_x", r.m_points );
    r.m_y = new double[r.m_points];
    checkmem( __FILE__, __LINE__, r.m_y, "double r.m_y", r.m_points );
    for ( int i = 0;
          i < r.m_points;
          i++ )
    {
        s >> r.m_x[i] >> r.m_y[i] ;
    }
    return( s );
}

//------------------------------------------------------------------------------
//  End of graphline.cpp
//------------------------------------------------------------------------------

