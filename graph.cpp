//------------------------------------------------------------------------------
/*! \file graph.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Graph class declaration.
 *
 *  \bug GraphAxle subtitles are improperly placed on left and top axles.
 */

// Custom include files
#include "appmessage.h"
#include "graph.h"
#include "graphaxle.h"
#include "graphbar.h"
#include "graphline.h"
#include "graphmarker.h"

// Qt include files
#include <qcolor.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qpainter.h>
#include <qpixmap.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief Graph constructor.
 */

Graph::Graph( void ) :
    m_canvasBgPixmap( 1, 1 ),
    m_canvasBgUsePixmap(false),
    m_worldBgPixmap( 1, 1 ),
    m_worldBgUsePixmap(false),
    m_axleList(),
    m_barList(),
    m_lineList(),
    m_markerList(),
    m_matrix(),
    m_multipleCurveLabel(""),
    m_fontScale(1.0)
{
    // The following completely define a Graph & initialize all members.
    setCanvas( 0, 0, 0, 0, 0 );
    setCanvasBg( QBrush( "white", Qt::NoBrush) );
    setCanvasBox( QPen( "black", 1, Qt::NoPen ) );
    setCanvasRotation( 0.0 );
    setCanvasScale( 1.0, 1.0 );
    setWorld( 0., 0., 0., 0. );
    setWorldBg( 0., 0., 0., 0., QBrush( "white", Qt::NoBrush) );
    setTitle( "", QFont( "Times New Roman", 14, QFont::Bold ), QColor( Qt::red ) );
    setSubTitle( "", QFont( "Times New Roman", 12, QFont::Normal ), QColor( Qt::red ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Graph destructor.
 */

Graph::~Graph( void )
{
    for ( GraphLine *line = m_lineList.first();
          line != 0;
          line = m_lineList.next() )
    {
        delete line;    line = 0;
    }
    for ( GraphBar *bar = m_barList.first();
          bar != 0;
          bar = m_barList.next() )
    {
        delete bar;     bar = 0;
    }
    for ( GraphAxle *axle = m_axleList.first();
          axle != 0;
          axle = m_axleList.next() )
    {
        delete axle;    axle = 0;
    }
}

//------------------------------------------------------------------------------
/*! \brief Adds an GraphAxle to the Graph.
 *
 *  \return Pointer to the newly allocated GraphAxle.
 */

GraphAxle *Graph::addGraphAxle( double scaleBeg, double scaleEnd, const QPen &pen )
{
    GraphAxle *axle = new GraphAxle( scaleBeg, scaleEnd, pen );
    checkmem( __FILE__, __LINE__, axle, "GraphAxle axle", 1 );
    m_axleList.append( axle );
    return( axle );
}

//------------------------------------------------------------------------------
/*! \brief Adds a GraphBar to the Graph.
 *
 *  \return Pointer to the newly allocated GraphBar.
 */

GraphBar *Graph::addGraphBar( double x0, double y0, double x1, double y1,
        const QBrush &brush, const QPen &pen )
{
    GraphBar *bar = new GraphBar( x0, y0, x1, y1, brush, pen );
    checkmem( __FILE__, __LINE__, bar, "GraphBar bar", 1 );
    m_barList.append( bar );
    return( bar );
}

//------------------------------------------------------------------------------
/*! \brief Adds a GraphLine to the Graph.
 *
 *  \return Pointer to the newly allocated GraphLine.
 */

GraphLine *Graph::addGraphLine( int points, double *x, double *y,
    const QPen &pen )
{
    GraphLine *line = new GraphLine( points, x, y, pen );
    checkmem( __FILE__, __LINE__, line, "GraphLine line", 1 );
    m_lineList.append( line );
    return( line );
}

//------------------------------------------------------------------------------
/*! \brief Adds a GraphMarker to the Graph.
 *
 *  \return Pointer to the newly allocated GraphMarker.
 */

GraphMarker *Graph::addGraphMarker( double x, double y, const QString &text,
        const QFont &font, const QColor &color, int align )
{
    GraphMarker *marker = new GraphMarker( x, y, text, font, color, align );
    checkmem( __FILE__, __LINE__, marker, "GraphMarker marker", 1 );
    m_markerList.append( marker );
    return( marker );
}

//------------------------------------------------------------------------------
/*! \brief Generic method of drawing graphs.
 *  Other functions may call any of the individal steps as needed.
 *  There are four steps:
 *  \arg drawBegin()
 *  \arg drawDecoration()
 *  \arg drawContent()
 *  \arg drawEnd()
 */

void Graph::draw( QPainter *p, double fontScale )
{
    // Allow font scaling only to the screen
    if ( p->device()->devType() == QInternal::Printer )
    {
        m_fontScale = 1.0;
    }
    else
    {
        m_fontScale = fontScale;
    }
    // Draw the graph using the font scaling
    drawBegin( p );
    drawDecoration( p );
    drawContent( p );
    drawEnd( p );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the previous painter state,
 *  and sets up the Graph's transformation matrix,
 *
 *  Called only by Graph::draw().
 *  This is step 1 of the drawBegin(), drawDecoration(), drawContent(),
 *  drawEnd() series.
 */

void Graph::drawBegin( QPainter *p )
{
    // Save the current painter state
    p->save();

    // Set the new transformation matrix using the canvas matrix values
    m_oldMatrix = p->worldMatrix();
    m_matrix.translate( m_canvasX0, m_canvasY0 );
    m_matrix.rotate( m_canvasRotation );
    m_matrix.scale( m_canvasScaleX, m_canvasScaleY );

    // Use the new transform matrix
    p->setWorldMatrix( m_matrix );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the Graphs' canvas background.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 */

void Graph::drawCanvasBackground ( QPainter *p )
{
    // Save the painter state
    p->save();

    // Create the canvas pixmap if necessary
    if ( m_canvasBgUsePixmap )
    {
        m_canvasBgBrush.setPixmap( m_canvasBgPixmap );
    }

    // Draw the canvas using the brush fill and the pen border
    p->setPen( m_canvasBoxPen );
    p->setBrush( m_canvasBgBrush );
    p->drawRect( 0, 0, m_canvasWd, m_canvasHt );

    // Restore the painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the Graph's subtitle.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 */

void Graph::drawCanvasSubTitle( QPainter *p )
{
    if ( m_subTitleText.isNull() || m_subTitleText.isEmpty() )
    {
        return;
    }

    // Save the painter state
    p->save();

    // Create the font and pen and apply them to the QPainter
    p->setPen( QPen( m_subTitleColor, 1) );
    QFont font( m_subTitleFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // Draw the subtitle and downsize the remaining layout space
    QRect rect;
    p->drawText(
        m_layout.left(),  m_layout.top(),
        m_layout.width(), m_layout.height(),
        Qt::AlignTop|Qt::AlignHCenter,
        m_subTitleText, -1, &rect );
    m_layout.setTop( rect.bottom() );

    // Restore painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the Graph's title.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 */

void Graph::drawCanvasTitle( QPainter *p )
{
    if ( m_titleText.isNull() || m_titleText.isEmpty() )
    {
        return;
    }

    // Save the painter state
    p->save();

    // Create the font and pen and apply them to the QPainter
    p->setPen( QPen( m_titleColor, 1) );
    QFont font( m_titleFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // Draw the title and downsize the remaining layout space
    QRect rect;
    p->drawText(
        m_layout.left(),  m_layout.top(),
        m_layout.width(), m_layout.height(),
        Qt::AlignTop|Qt::AlignHCenter,
        m_titleText, -1, &rect );
    m_layout.setTop( rect.bottom() );

    // Restore the painter and return.
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws all the Graph's lines/bars over the previously draw decoration.
 *
 *  Called only by Graph::draw().
 *  This is step 3 of the drawBegin(), drawDecoration(), drawContent(),
 *  drawEnd() series.
 */

void Graph::drawContent( QPainter *p )
{
    // Create a clipping rectangle before drawing lines.
    int x0 = toCanvasX( m_worldBgXMin );
    int y0 = toCanvasY( m_worldBgYMin );
    int x1 = toCanvasX( m_worldBgXMax );
    int y1 = toCanvasY( m_worldBgYMax );
    int x, y, w, h;
    // This clips unrotated graphs to the World area very nicely
    if ( m_canvasRotation == 0.0 )
    {
        // This worked in Qt 2 but not in Qt 3
        x = m_canvasX0 + x0;
        //y = m_canvasY0 + y0;
        w = x1 - x0;
        //h = y1 - y0;

        // This works in Qt 3
        y = m_canvasY0 + y1;
        h = y0 - y1;
    }
    // But rotated graphs are a bit sloppier.
    else
    {
        // This worked in Qt 2
        //x = m_canvasX0 + y1;
        //y = m_canvasY0 - m_canvasWd + x0;
        //w = y0 - y1;
        //h = x1 - x0;

        // Try this for Qt 3
        x = m_canvasX0 + y0;
        y = m_canvasY0 - m_canvasWd + x0;
        w = y1 - y0;
        h = x1 - x0;
    }
    p->setClipRect( x, y, w, h );

    // Draw the lines and bars while clipping is on.
    p->setClipping( true );
    GraphLine *line;
    for ( line = m_lineList.first();
          line != 0;
          line = m_lineList.next() )
    {
        drawGraphLine( p, line );
    }

    GraphBar *bar;
    for ( bar = m_barList.first();
          bar != 0;
          bar = m_barList.next() )
    {
        drawGraphBar( p, bar );
    }

    // Draw the bar and line labels AFTER the clipping has been turned off
    p->setClipping( false );
    int nlines = 0;
    for ( line = m_lineList.first();
          line != 0;
          line = m_lineList.next() )
    {
        drawGraphLineLabel( p, line );
        // If drawing second line of multiple lines, display the group name
        if ( nlines == 1 )
        {
            // Use the current line label font but with black pen
            p->save();
            QFont font( line->m_labelFont );
            font.setPointSize( (int)
                ( 0.1 + m_fontScale * (double) font.pointSize() ) );
            p->setFont( font );
            p->setPen( QPen( "black" ) );
            p->drawText( x0, y1+5, (x1-x0-5), (y0-y1),
                Qt::AlignRight|Qt::AlignTop, m_multipleCurveLabel );
            p->restore();
        }
        nlines++;
    }
    //p->setClipping( false );

    for ( bar = m_barList.first();
          bar != 0;
          bar = m_barList.next() )
    {
        drawGraphBarLabel( p, bar );
    }

    // Draw all markers
    GraphMarker *marker;
    for ( marker = m_markerList.first();
          marker != 0;
          marker = m_markerList.next() )
    {
        drawGraphMarker( p, marker );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws all the Graph's title, backgrounds, and axles.
 *
 *  Called only by Graph::draw().
 *  This is step 2 of the drawBegin(), drawDecoration(), drawContent(),
 *  drawEnd() series.
 */

void Graph::drawDecoration( QPainter *p )
{
    // Initialize the layout space
    m_layout.setRect(
        m_canvasBd,
        m_canvasBd,
        m_canvasWd - 2 * m_canvasBd,
        m_canvasHt - 2 * m_canvasBd );

    // Draw the canvas and its titles.
    drawCanvasBackground( p );
    drawCanvasTitle( p );
    drawCanvasSubTitle( p );
    drawWorldBackground( p );

    // Draw the axles.
    for ( GraphAxle *axle = m_axleList.first();
          axle != 0;
          axle = m_axleList.next() )
    {
        drawGraphAxle( p, axle );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Restores the previous painter state and transformation matrix.
 *
 *  Called only by Graph::draw().
 *  This is step 4 of the drawBegin(), drawDecoration(), drawContent(),
 *  drawEnd() series.
 */

void Graph::drawEnd( QPainter *p )
{
    // Restore the old world matrix and the painter state and return
    p->setWorldMatrix( m_oldMatrix );
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphAxle using the QPainter \a p.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param axle Pointer to the GraphAxle to be drawn.
 */

void Graph::drawGraphAxle( QPainter *p, GraphAxle *axle )
{
    // Save the painter state.
    p->save();

    // Transform axle World coordinates to canvas pixel coordinates
    int x0 = toCanvasX( axle->m_worldXBeg );
    int x1 = toCanvasX( axle->m_worldXEnd );
    int y0 = toCanvasY( axle->m_worldYBeg );
    int y1 = toCanvasY( axle->m_worldYEnd );

    // Draw the axle line.
    p->setPen( axle->m_axlePen );
    p->drawLine( x0, y0, x1, y1 );

    // Draw minor tic marks first.
    if ( axle->m_minorTicPen.width() > 0
      && axle->m_minorTicStep > 0.0
      && axle->m_minorTicPen.style() != Qt::NoPen )
    {
        drawGraphAxleMinorTic( p, axle );
    }

    // Overwrite them with any major tic marks
    int offset = 0;
    if ( axle->m_majorTicPen.width() > 0
      && axle->m_majorTicStep > 0.0
      && axle->m_majorTicPen.style() != Qt::NoPen )
    {
        offset = drawGraphAxleMajorTic( p, axle );
    }
    else if ( axle->m_worldXBeg == axle->m_worldXEnd )  // Vertical
    {
        if ( axle->m_majorTicSide == GraphAxleRight )
        {
            offset = toCanvasX( axle->m_worldXBeg + axle->m_majorTicLength ) + 1;
        }
        else if ( axle->m_majorTicSide == GraphAxleLeft )
        {
            offset = toCanvasX( axle->m_worldXBeg + axle->m_majorTicLength ) - 1;
        }
    }
    else if ( axle->m_worldYBeg == axle->m_worldYEnd )  // Horizontal
    {
        if ( axle->m_majorTicSide == GraphAxleTop )
        {
            offset = toCanvasY( axle->m_worldYBeg + axle->m_majorTicLength ) - 1;
        }
        else // if ( axle->majorTicSide == GraphAxleBottom )
        // Had to comment above test because if no majorTicSide was set
        // the offset was 0 and the title was printed at the top of the page
        {
            offset = toCanvasY( axle->m_worldYBeg + axle->m_majorTicLength ) + 1;
        }
    }

    // Add the title.
    offset = drawGraphAxleTitle( p, axle, offset );
    offset = drawGraphAxleSubTitle( p, axle, offset );

    // Restore the painter state and return.
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws all of a GraphAxle's major tic marks.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param axle Pointer to the GraphAxle to be drawn.
 *
 *  \return The outer-most canvas pixel used by the axle
 *  so that subsequent drawing ops can add more decoration such as titles.
 */

int  Graph::drawGraphAxleMajorTic( QPainter *p, GraphAxle *axle )
{
    // Dummy initialization to avoid possible use without initialization.
    int x0 = 100;
    int x1 = 200;
    int x2 = 300;
    int y0 = 100;
    int y1 = 200;
    int y2 = 300;
    int retVal = 400;
    QString qStr;

    // Set the tic pen and font
    p->setPen( axle->m_majorTicPen );
    QPen labelPen( axle->m_majorLabelsColor );
    QFont font( axle->m_majorLabelsFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // For this to work correctly with the printer,
    // we must get the FontMetrics directly from the QPainter
    // QFontMetrics fm( axle->majorLabelsFont ); doesn't work!
    QFontMetrics fm = p->fontMetrics();
    int wd = 0;
    int ht = fm.height();

    // Draw each tic mark.
    double val = axle->m_majorTicFirst;
    double pos;
    int tics = 1 + (int) ( (axle->m_majorTicLast - axle->m_majorTicFirst )
             / axle->m_majorTicStep );
    for ( int i = 0;
          i < tics;
          i++ )
    {
        val = axle->m_majorTicFirst + i * axle->m_majorTicStep;
        // Tic marks position (fraction) along the scale
        pos = (val - axle->m_axleScaleBeg)
            / (axle->m_axleScaleEnd - axle->m_axleScaleBeg);
        // Prepare the tic mark label.
        if ( axle->m_majorLabelsDecimals >= 0 )
        {
            qStr.sprintf("%1.*f", axle->m_majorLabelsDecimals, val);
            wd = fm.width( qStr );
        }
        // Vertical axle tic mark endpoints
        if ( axle->m_worldXBeg == axle->m_worldXEnd )
        {
            y0 = y1 = y2 = toCanvasY( axle->m_worldYBeg
                         + pos * (axle->m_worldYEnd - axle->m_worldYBeg) );
            if ( axle->m_majorTicSide == GraphAxleRight )
            {
                x0 = toCanvasX( axle->m_worldXBeg - axle->m_majorGridLength );
                x1 = toCanvasX( axle->m_worldXBeg + axle->m_majorTicLength );
                x2 = toCanvasX( axle->m_worldXBeg );
                retVal = x1;
                if ( axle->m_majorLabelsDecimals >= 0 )
                {
                    p->setPen( labelPen );
                    p->drawText( x1, y1-ht/2, 2*wd, ht,
                        Qt::AlignLeft|Qt::AlignVCenter, qStr );
                    retVal = x1 + wd;
                }
            }
            else
            {
                x0 = toCanvasX( axle->m_worldXBeg + axle->m_majorGridLength );
                x1 = toCanvasX( axle->m_worldXBeg - axle->m_majorTicLength );
                x2 = toCanvasX( axle->m_worldXBeg );
                retVal = x1;
                if ( axle->m_majorLabelsDecimals >= 0 )
                {
                    p->setPen( labelPen );
                    p->drawText( x1-2*wd, y1-ht/2, 2*wd, ht,
                        Qt::AlignRight|Qt::AlignVCenter, qStr);
                    retVal = x1 - wd;
                }
            }
        }
        // Horizontal axle tic mark endpoints
        else if ( axle->m_worldYBeg == axle->m_worldYEnd )
        {
            x0 = x1 = x2 = toCanvasX( axle->m_worldXBeg
                          + pos * (axle->m_worldXEnd - axle->m_worldXBeg) );
            if ( axle->m_majorTicSide == GraphAxleTop )
            {
                y0 = toCanvasY( axle->m_worldYBeg - axle->m_majorGridLength );
                y1 = toCanvasY( axle->m_worldYBeg + axle->m_majorTicLength );
                y2 = toCanvasY( axle->m_worldYBeg );
                retVal = y1;
                if ( axle->m_majorLabelsDecimals >= 0 )
                {
                    p->setPen( labelPen );
                    p->drawText( x1-wd, y1-ht, 2*wd, ht,
                        Qt::AlignHCenter|Qt::AlignTop, qStr);
                    retVal = y1 - ht;
                }
            }
            else if ( axle->m_majorTicSide == GraphAxleBottom )
            {
                y0 = toCanvasY( axle->m_worldYBeg + axle->m_majorGridLength );
                y1 = toCanvasY( axle->m_worldYBeg - axle->m_majorTicLength );
                y2 = toCanvasY( axle->m_worldYBeg );
                retVal = y1;
                if ( axle->m_majorLabelsDecimals >= 0 )
                {
                    p->setPen( labelPen );
                    p->drawText( x1-wd, y1, 2*wd, ht,
                        Qt::AlignHCenter|Qt::AlignBottom, qStr );
                    retVal = y1 + ht;
                }
            }
        }

        // Now that we know the endpoints, draw the tic mark
        if ( axle->m_majorTicLength > 0.
          && axle->m_majorTicStep > 0.
          && axle->m_majorTicPen.width() > 0
          && axle->m_majorTicPen.style() != Qt::NoPen )
        {
            p->setPen( axle->m_majorTicPen );
            p->drawLine( x2, y2, x1, y1 );
        }

        // Now that we know the endpoints, draw the grid line
        // The last two tests make sure we're not overwriting the first/last
        if ( axle->m_majorGridLength > 0.
          && axle->m_majorGridPen.width() > 0
          && axle->m_majorGridPen.style() != Qt::NoPen
          && fabs(val - axle->m_axleScaleBeg) > axle->m_majorTicStep/10.
          && fabs(val - axle->m_axleScaleEnd) > axle->m_majorTicStep/10. )
        {
            p->setPen( axle->m_majorGridPen );
            p->drawLine( x0, y0, x2, y2 );
        }
    }
    return( retVal );
}

//------------------------------------------------------------------------------
/*! \brief Draws all of a GraphAxle's minor tic marks.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param axle Pointer to the GraphAxle to be drawn.
 */

void Graph::drawGraphAxleMinorTic( QPainter *p, GraphAxle *axle )
{
    // Set the tic pen
    p->setPen( axle->m_minorTicPen );

    // Dummy initialization to avoid possible use without initialization.
    int x0 = 100;
    int x1 = 200;
    int x2 = 300;
    int y0 = 100;
    int y1 = 200;
    int y2 = 300;

    // Draw each tic mark.
    double pos = axle->m_minorTicFirst;
    double f;
    int tics = 1 + (int) ( (axle->m_minorTicLast - axle->m_minorTicFirst )
             / axle->m_minorTicStep );
    for ( int i = 1;
          i < tics;
          i++ )
    {
        pos = axle->m_minorTicFirst + i * axle->m_minorTicStep;
        f = (pos - axle->m_axleScaleBeg)
          / (axle->m_axleScaleEnd - axle->m_axleScaleBeg);
        // Vertical axle
        if ( axle->m_worldXBeg == axle->m_worldXEnd )
        {
            y0 = y1 = y2 = toCanvasY( axle->m_worldYBeg
                         + f * (axle->m_worldYEnd - axle->m_worldYBeg) );
            if ( axle->m_minorTicSide == GraphAxleRight )
            {
                x0 = toCanvasX( axle->m_worldXBeg - axle->m_minorGridLength );
                x1 = toCanvasX( axle->m_worldXBeg + axle->m_minorTicLength );
                x2 = toCanvasX( axle->m_worldXBeg );
            }
            else
            {
                x0 = toCanvasX( axle->m_worldXBeg + axle->m_minorGridLength );
                x1 = toCanvasX( axle->m_worldXBeg - axle->m_minorTicLength );
                x2 = toCanvasX( axle->m_worldXBeg );
            }
        }
        // Horizontal axle
        else if ( axle->m_worldYBeg == axle->m_worldYEnd )
        {
            x0 = x1 = x2 = toCanvasX( axle->m_worldXBeg
                         + f * (axle->m_worldXEnd - axle->m_worldXBeg) );
            if ( axle->m_minorTicSide == GraphAxleTop )
            {
                y0 = toCanvasY( axle->m_worldYBeg - axle->m_minorGridLength );
                y1 = toCanvasY( axle->m_worldYBeg + axle->m_minorTicLength );
                y2 = toCanvasY( axle->m_worldYBeg );
            }
            else
            {
                y0 = toCanvasY( axle->m_worldYBeg + axle->m_minorGridLength );
                y1 = toCanvasY( axle->m_worldYBeg - axle->m_minorTicLength );
                y2 = toCanvasY( axle->m_worldYBeg );
            }
        }
        if ( axle->m_minorTicLength > 0.
          && axle->m_minorTicStep > 0.
          && axle->m_minorTicPen.width() > 0
          && axle->m_minorTicPen.style() != Qt::NoPen )
        {
            p->setPen( axle->m_minorTicPen );
            p->drawLine( x2, y2, x1, y1 );
        }
        if ( axle->m_minorGridLength > 0.
          && axle->m_minorGridPen.width() > 0
          && axle->m_minorGridPen.style() != Qt::NoPen )
        {
            p->setPen( axle->m_minorGridPen );
            p->drawLine( x0, y0, x2, y2 );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the GraphAxle's sub title.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param axle Pointer to the GraphAxle to be drawn.
 *  \param offset A value usually returned by a previous call to
 *      drawGraphAxleMajorTic() to account for tic mark and tic label space.
 *
 *  \return The new offset after printing the subtitle.
 */

int Graph::drawGraphAxleSubTitle( QPainter *p, GraphAxle *axle, int offset )
{
    if ( axle->m_subTitleText.isNull() || axle->m_subTitleText.isEmpty() )
    {
        return( offset );
    }
    int newOffset = offset;

    // Save the painter state.
    p->save();

    // Transform axle center World coordinates to canvas pixel coordinates
    int x0 = toCanvasX( axle->m_worldXBeg );
    int x1 = toCanvasX( axle->m_worldXEnd );
    int y0 = toCanvasY( axle->m_worldYBeg );
    int y1 = toCanvasY( axle->m_worldYEnd );
    int yc = (y1+y0) / 2;

    // Create the axle title font.
    QPen pen( axle->m_subTitleColor );
    p->setPen( pen );
    QFont font( axle->m_subTitleFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // For this to work correctly with the printer,
    // we must get the FontMetrics directly from the QPainter
    // QFontMetrics fm( axle->m_subTitleFont ); doesn't work!
    QFontMetrics fm = p->fontMetrics();
    int ht = fm.height();

    // Vertical axle title
    if ( axle->m_worldXBeg == axle->m_worldXEnd )
    {
        if ( axle->m_subTitleSide == GraphAxleRight )
        {
            offset += ht;
        }
        else
        {
            offset -= ht/2;
        }
        drawRotatedText( p, 270., offset, yc+fm.width(axle->m_subTitleText)/2,
            axle->m_subTitleText );
    }
    // Horizontal axle title
    else if ( axle->m_worldYBeg == axle->m_worldYEnd )
    {
        if ( axle->m_subTitleSide == GraphAxleTop )
        {
            offset -= ht;
        }
        p->drawText( x0, offset, (x1-x0), ht,
            Qt::AlignCenter, axle->m_subTitleText );
    }

    // Restore the painter state and return.
    p->restore();
    return( newOffset + ht );
}

//------------------------------------------------------------------------------
/*! \brief Draws the GraphAxle's main title.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param axle Pointer to the GraphAxle to be drawn.
 *  \param offset A value usually returned by a previous call to
 *      drawGraphAxleMajorTic() to account for tic mark and tic label space.
 *
 *  \return The new offset after printing the title.
 */

int Graph::drawGraphAxleTitle( QPainter *p, GraphAxle *axle, int offset )
{
    if ( axle->m_titleText.isNull() || axle->m_titleText.isEmpty() )
    {
        return( offset );
    }
    int newOffset = offset;

    // Save the painter state.
    p->save();

    // Transform axle center World coordinates to canvas pixel coordinates
    int x0 = toCanvasX( axle->m_worldXBeg );
    int x1 = toCanvasX( axle->m_worldXEnd );
    int y0 = toCanvasY( axle->m_worldYBeg );
    int y1 = toCanvasY( axle->m_worldYEnd );
    int yc = (y1 + y0) / 2;

    // Create the axle title font.
    QPen pen( axle->m_titleColor );
    p->setPen( pen );
    QFont font( axle->m_titleFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // For this to work correctly with the printer,
    // we must get the FontMetrics directly from the QPainter
    // QFontMetrics fm( axle->m_titleFont ); doesn't work!
    QFontMetrics fm = p->fontMetrics();
    int ht = fm.height();

    // Vertical axle title
    if ( axle->m_worldXBeg == axle->m_worldXEnd )
    {
        if ( axle->m_titleSide == GraphAxleRight )
        {
            offset += ht;
        }
        else
        {
            offset -= ht/2;
        }
        int wd = fm.width( axle->m_titleText );

        drawRotatedText( p, 270., offset, yc+wd/2, axle->m_titleText );
    }
    // Horizontal axle title
    else if ( axle->m_worldYBeg == axle->m_worldYEnd )
    {
        if ( axle->m_titleSide == GraphAxleTop )
        {
            offset -= ht;
        }
        else if ( axle->m_titleSide == GraphAxleBottom )
        {
            if ( m_barList.count() > 0 )
            {
                offset += ht;
            }
        }
        p->drawText( x0, offset, (x1-x0), ht, Qt::AlignCenter, axle->m_titleText );
    }

    // Restore the painter state and return.
    p->restore();
    return( newOffset + ht );
}

//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphBar onto the QPainter.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param bar Pointer to the GraphBar to be drawn.
 */

void Graph::drawGraphBar( QPainter *p, GraphBar *bar )
{
    // Save the painter state.
    p->save();

    // Transform bar World coordinates to canvas pixel coordinates
    int x0 = toCanvasX( bar->m_barX0 );
    int x1 = toCanvasX( bar->m_barX1 );
    int y0 = toCanvasY( bar->m_barY0 );
    int y1 = toCanvasY( bar->m_barY1 );

    // Create the bar pixmap if necessary
    if ( bar->m_barUsePixmap )
    {
        bar->m_barBrush.setPixmap( bar->m_barPixmap );
    }

    // Create the bar brush and pen and apply it to the painter
    p->setPen( bar->m_barPen );
    p->setBrush( bar->m_barBrush );
    p->fillRect( x0, y1, (x1-x0), (y0-y1), bar->m_barBrush );

    // Restore the painter state and return.
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphBar's label onto the QPainter.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param bar Pointer to the GraphBar to be drawn.
 */

void Graph::drawGraphBarLabel( QPainter *p, GraphBar *bar )
{
    if ( bar->m_label.isNull() || bar->m_label.isEmpty() )
    {
        return;
    }

    QFont font( bar->m_labelFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // For this to work correctly with the printer,
    // we must get the FontMetrics directly from the QPainter
    // QFontMetrics fm( bar->m_labelFont ); doesn't work!
    QFontMetrics fm = p->fontMetrics();

    // Center the label over the x point
    int x0 = toCanvasX( bar->m_labelX ) - fm.width( bar->m_label ) / 2;
    int y0 = toCanvasY( bar->m_labelY ) + fm.height();
    drawRotatedText( p, bar->m_labelRotate, x0, y0, bar->m_label );
    return;
}
//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphLine to the specified QPainter.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param line Pointer to the GraphLine to be drawn.
 */

void Graph::drawGraphLine( QPainter *p, GraphLine *line )
{
    // Save the painter state
    p->save();
    // Construct the polyline and draw it.
    p->setPen( line->m_linePen );
    QPointArray a( line->m_points );
    for ( int i = 0;
          i < line->m_points;
          i++ )
    {
        a.setPoint( i, toCanvasX( line->m_x[i] ), toCanvasY( line->m_y[i] ) );
    }
    p->drawPolyline( a );

    // Restore the painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphLine label to the specified QPainter.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 *  \param line Pointer to the GraphLine to be drawn.
 */

void Graph::drawGraphLineLabel( QPainter *p, GraphLine *line )
{
    if ( line->m_label.isNull() || line->m_label.isEmpty() )
    {
        return;
    }

    // Save the painter state
    p->save();

    // Create the pen and font
    p->setPen( QPen( line->m_labelColor ) );
    QFont font( line->m_labelFont );
    font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
    p->setFont( font );

    // Draw the label
    p->drawText( toCanvasX( line->m_labelX ), toCanvasY( line->m_labelY ),
        line->m_label );

    // Restore the painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the specified GraphMarker pixmap and label
 *  to the specified QPainter.
 *
 *  \param p        Pointer to the QPainter to use for drawing.
 *  \param marker   Pointer to the GraphMarker to be drawn.
 */

void Graph::drawGraphMarker( QPainter *p, GraphMarker *marker )
{
    // Save the painter state
    p->save();
    // Draw a pixmap if one is specified
    int pixmapWd = 0;
    int pixmapHt = 0;
    if ( marker->m_usePixmap )
    {
        // Center the GraphMarker m_pixmap
        pixmapWd = marker->m_pixmap.width();
        pixmapHt = marker->m_pixmap.height();
        p->drawPixmap( ( toCanvasX( marker->m_x ) - pixmapWd / 2 ),
                       ( toCanvasY( marker->m_y ) - pixmapHt / 2 ),
                       marker->m_pixmap );
    }
    // Draw a label if one is specified
    if ( ! marker->m_text.isNull()
      && ! marker->m_text.isEmpty() )
    {
        // Create the pen and font
        p->setPen( QPen( marker->m_color ) );
        QFont font( marker->m_font );
        font.setPointSize( (int) ( 0.1 + m_fontScale * (double) font.pointSize() ) );
        p->setFont( font );
        // Draw the label
        QFontMetrics fm( font );
        QRect rect = fm.boundingRect( marker->m_text );
        int px = toCanvasX( marker->m_x );  // Text rect lower left x
        int py = toCanvasY( marker->m_y );  // Text rect lower left y
        if ( marker->m_align & Qt::AlignLeft )
        {
            px = px - pixmapWd/2 - rect.width();
        }
        else if ( marker->m_align & Qt::AlignRight )
        {
            px = px + pixmapWd/2;
        }
        else if ( marker->m_align & Qt::AlignHCenter )
        {
            px = px - rect.width()/2;
        }
        if ( marker->m_align & Qt::AlignTop )
        {
            py = py - pixmapHt/2;
        }
        else if ( marker->m_align & Qt::AlignBottom )
        {
            py = py + pixmapHt/2 + rect.height();
        }
        else if ( marker->m_align & Qt::AlignVCenter )
        {
            py = py + rect.height()/2;
        }
        p->drawText( px, py, marker->m_text );
    }
    // Restore the painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws the world background to the specified QPainter.
 *
 *  \param p Pointer to the QPainter to use for drawing.
 */

void Graph::drawWorldBackground ( QPainter *p )
{
    if ( m_worldBgBrush.style() == Qt::NoBrush )
    {
        return;
    }

    // Save the painter state
    p->save();

    // Create the canvas brush and pen
    if ( m_worldBgUsePixmap )
    {
        m_worldBgBrush.setPixmap( m_worldBgPixmap );
    }

    // Apply the World area brush and pen
    p->setPen( QPen( m_worldBgBrush.color() ) );
    p->setBrush( m_worldBgBrush );

    // Draw the world area using the brush fill and the pen border
    int x0 = toCanvasX( m_worldBgXMin );
    int y0 = toCanvasY( m_worldBgYMin );
    int x1 = toCanvasX( m_worldBgXMax );
    int y1 = toCanvasY( m_worldBgYMax );
    p->drawRect( x0, y0, (x1-x0), (y1-y0) );

    // Restore the painter state and return
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the GraphAxle at the specified m_axleList index.
 *
 *  \return Pointer to the GraphAxle at the specified index.
 */

GraphAxle *Graph::getGraphAxle( unsigned int index )
{
    return ( (GraphAxle *) m_axleList.at( index ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to the GraphBar at the specified m_barList index.
 *
 *  \return Pointer to the GraphBar at the specified index.
 */

GraphBar *Graph::getGraphBar( unsigned int index )
{
    return ( (GraphBar *) m_barList.at( index ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to the GraphLine at the specified m_lineList index.
 *
 *  \return Pointer to the GraphLine at the specified index.
 */

GraphLine *Graph::getGraphLine( unsigned int index )
{
    return ( (GraphLine *) m_lineList.at( index ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to the GraphMarker at the specified m_markerList index.
 *
 *  \return Pointer to the GraphMarker at the specified index.
 */

GraphMarker *Graph::getGraphMarker( unsigned int index )
{
    return ( (GraphMarker *) m_lineList.at( index ) );
}

//------------------------------------------------------------------------------
/*! Prints the Graph class data elements.
 */

#ifdef _DEVELOPMENTAL_

void Graph::print( FILE *fptr )
{
    fprintf( fptr, "\n\nGRAPH DEFINITION:\n");

    fprintf( fptr,
        "setCanvas( m_canvasX0=%d, m_canvasY0=%d, m_canvasWd=%d, "
        "m_canvasHt=%d, m_canvasBd=%d );\n",
        m_canvasX0, m_canvasY0, m_canvasWd, m_canvasHt, m_canvasBd );

    fprintf( fptr,
        "setCanvasBgBrush( m_canvasBgBrush(color(%d, %d, %d), style=%d );\n",
        m_canvasBgBrush.color().red(), m_canvasBgBrush.color().green(),
        m_canvasBgBrush.color().blue(), m_canvasBgBrush.style() );

    fprintf( fptr,
        "setCanvasBox( m_canvasBoxPen(color(%d, %d, %d), width=%d, style=%d );\n",
        m_canvasBoxPen.color().red(), m_canvasBoxPen.color().green(),
        m_canvasBoxPen.color().blue(), m_canvasBoxPen.width(),
        m_canvasBoxPen.style() );

    fprintf( fptr, "setCanvasRotation( %f );\n", m_canvasRotation);

    fprintf( fptr,
        "setCanvasScale( m_canvasScaleX=%f, m_canvasScaleY=%f );\n",
        m_canvasScaleX, m_canvasScaleY);

    fprintf( fptr,
        "setSubTitle( m_subTitleText=\"%s\", "
        "m_subTitleFont(%s, size=%d, wgt=%d), color=(%d, %d, %d) );\n",
        m_subTitleText.latin1(), m_subTitleFont.family().latin1(),
        m_subTitleFont.pointSize(), m_subTitleFont.weight(),
        m_subTitleColor.red(), m_subTitleColor.green(),
        m_subTitleColor.blue() );

    fprintf( fptr,
        "setTitle( m_titleText=\"%s\", m_titleFont(%s, size=%d, wgt=%d), "
        "color=(%d, %d, %d) );\n",
        m_titleText.latin1(), m_titleFont.family().latin1(),
        m_titleFont.pointSize(), m_titleFont.weight(), m_titleColor.red(),
        m_titleColor.green(), m_titleColor.blue() );

    fprintf( fptr,
        "setWorld( m_worldXMin=%f, m_worldXMax=%f, "
        "m_worldYMin=%f, m_worldYMax=%f );\n",
            m_worldXMin, m_worldXMax, m_worldYMin, m_worldYMax);

    fprintf( fptr,
        "setWorldBg( m_worldBgXMin=%f, m_worldbgYMin=%f, m_rorldBgXMax=%f, "
        "m_worldBgYMax=%f, m_worldBgBrush(color=(%d, %d, %d), style=%d) );\n",
        m_worldBgXMin, m_worldBgYMin, m_worldBgXMax, m_worldBgYMax,
        m_worldBgBrush.color().red(), m_worldBgBrush.color().green(),
        m_worldBgBrush.color().blue(), m_worldBgBrush.style() );

    // Axis
    fprintf( fptr, "AXLES: %d\n", m_axleList.count() );
    for ( GraphAxle *a = m_axleList.first();
          a != 0;
          a = m_axleList.next() )
    {
        a->print( fptr );
    }
    // GraphBars
    fprintf( fptr, "BARS: %d\n", m_barList.count() );
    for ( GraphBar *b = m_barList.first();
          b != 0;
          b = m_barList.next() )
    {
        b->print( fptr );
    }
    // GraphLines
    fprintf( fptr, "LINES: %d\n", m_lineList.count() );
    for ( GraphLine *l = m_lineList.first();
          l != 0;
          l = m_lineList.next() )
    {
        l->print( fptr );
    }
    return;
}

#endif

//------------------------------------------------------------------------------
/*! \brief Sets the size of the physical canvas in pixels.
 *
 *  Corresponds to the world dimensions.
 */

void Graph::setCanvas( int x0, int y0, int width, int height, int border )
{
    m_canvasX0 = x0;
    m_canvasY0 = y0;
    m_canvasWd = width;
    m_canvasHt = height;
    m_canvasBd = border;
    return;
}

//------------------------------------------------------------------------------
/*! Sets the Graph's canvas background brush.
 *
 *  \param brush Reference to the new canvas background QBrush.
 */

void Graph::setCanvasBg( const QBrush &brush )
{
    m_canvasBgBrush = brush;
    return;
}

//------------------------------------------------------------------------------
/*! Sets the Graph's canvas background pixmap.
 *
 *  \param brush Reference to the new canvas background QPixmap.
 */

void Graph::setCanvasBgPixmap( QPixmap & pixmap )
{
    m_canvasBgUsePixmap = true;
    m_canvasBgPixmap = pixmap;
    return;
}

//------------------------------------------------------------------------------
/*! Sets the Graph's canvas enclosing box pen.
*
*   \param pen Reference to the new canvas enclosing box QPen.
 */

void Graph::setCanvasBox( const QPen &pen )
{
    m_canvasBoxPen = pen;
    return;
}

//------------------------------------------------------------------------------
/*! Sets the Graph's canvas rotation.
 *
 *  \param rotation Rotation (in degrees?)
 */

void Graph::setCanvasRotation( double rotation )
{
    m_canvasRotation = rotation;
    return;
}

//------------------------------------------------------------------------------
/*! Sets the Graph's canvas scale factor.
 *
 *  \param xScale Horizontal scale factor.
 *  \param yScale Vertical scale factor.
 */

void Graph::setCanvasScale( double xScale, double yScale )
{
    m_canvasScaleX = xScale;
    m_canvasScaleY = yScale;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the curve family label used when there are multiple GraphLines.
 *
 *  \param text New curve family label text.
 */

void Graph::setMultipleCurveLabel( const QString &text )
{
    m_multipleCurveLabel = text;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Graph's subtitle text, font, and color.
 *
 *  \param text New subtitle text.
 *  \param font Reference to the QFont to use.
 *  \param color Reference to the QColor to use.
 */

void Graph::setSubTitle( const QString &text, const QFont &font,
    const QColor &color )
{
    m_subTitleText  = text;
    m_subTitleFont  = font;
    m_subTitleColor = color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Graph's title text, font, and color.
 *
 *  \param text New subtitle text.
 *  \param font Reference to the QFont to use.
 *  \param color Reference to the QColor to use.
 */

void Graph::setTitle( const QString &text, const QFont &font,
    const QColor &color )
{
    m_titleText  = text;
    m_titleFont  = font;
    m_titleColor = color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the world coordinates for the ENTIRE CANVAS.
 *
 *  \param xMin Minimum horizontal value in real world units.
 *  \param yMin Minimum vertical value in real world units.
 *  \param xMax Maximum horizontal value in real world units.
 *  \param xMax Maximum vertical value in real world units.
 */

void Graph::setWorld( double xMin, double yMin, double xMax, double yMax )
{
    m_worldXMin = xMin;
    m_worldXMax = xMax;
    m_worldYMin = yMin;
    m_worldYMax = yMax;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the world background image rectangle in WORLD coordinates.
 *
 *  \param xMin Minimum background horizontal value in real world units.
 *  \param yMin Minimum backgreound vertical value in real world units.
 *  \param xMax Maximum background horizontal value in real world units.
 *  \param xMax Maximum background vertical value in real world units.
 *  \param brush Reference to the QBrush to use.
 */

void Graph::setWorldBg ( double xMin, double yMin, double xMax, double yMax,
        const QBrush &worldBrush )
{
    m_worldBgXMin   = xMin;
    m_worldBgYMin   = yMin;
    m_worldBgXMax   = xMax;
    m_worldBgYMax   = yMax;
    m_worldBgBrush  = worldBrush;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the pixmap to use as the world background.
 *
 *  \param pixmap Reference to the QPixmap to use.
 */

void Graph::setWorldBgPixmap( const QPixmap &pixmap )
{
    m_worldBgUsePixmap = true;
    m_worldBgPixmap = pixmap;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Converts from world to canvas horizontal coordinates.
 *
 *  \param worldX World horizontal coordinate.
 *
 *  \return Corresponding Graph canvas horizontal coordinate (pixels).
 */

int Graph::toCanvasX( double worldX ) const
{
    double cx = (worldX - m_worldXMin) / (m_worldXMax - m_worldXMin);
    return( m_layout.left() + (int) (cx * (double) m_layout.width()) );
}

//------------------------------------------------------------------------------
/*! \brief Converts from world to canvas vertical coordinates.
 *
 *  \param worldY World vertical coordinate.
 *
 *  \return Corresponding Graph canvas vertical coordinate (pixels).
 */

int Graph::toCanvasY( double worldY ) const
{
    double cy = (worldY - m_worldYMin) / (m_worldYMax - m_worldYMin);
    return( m_layout.top() + m_layout.height()
        - (int) (cy * (double) m_layout.height()) );
}

//------------------------------------------------------------------------------
/*! \brief Discontinues use of the canvas background pixmap.
 */

void Graph::unsetCanvasBgPixmap( void )
{
    m_canvasBgUsePixmap = false;
    m_canvasBgPixmap.resize( 1, 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Discontinues use of the world background pixmap.
 */

void Graph::unsetWorldBgPixmap( void )
{
    m_worldBgUsePixmap = false;
    m_worldBgPixmap.resize( 1, 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Draws \a text rotated \a deg degrees about the \a x, \a y point.
 *
 *  \param p    Pointer to the QPainter in use.
 *  \param text Text to be drawn.
 *  \param deg Rotation degrees.
 *  \param x Canvas horizontal rotation point (pixels)
 *  \param y Canvas vertical rotation point (pixels)
 */

void drawRotatedText( QPainter *p, double deg, int x, int y,
        const QString &text )
{
    p->save();
    p->translate( x, y );
    p->rotate( deg );
    p->drawText( 0, 0, text );
    p->restore();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Graph operator<<().
 *
 *  Serializes the Graph and its GraphAxles, GraphBars, and GraphLines
 *  to a data stream.
 *
 *  \return Reference to the output QDataStream.
 */

QDataStream& operator<<( QDataStream &s, const Graph &r )
{
    s << r.m_canvasX0
      << r.m_canvasY0
      << r.m_canvasWd
      << r.m_canvasHt
      << r.m_canvasBd
      << r.m_canvasBgBrush
      << (Q_INT32) r.m_canvasBgUsePixmap
      << r.m_canvasBgPixmap
      << r.m_canvasBoxPen
      << r.m_canvasRotation
      << r.m_canvasScaleX
      << r.m_canvasScaleY
      << r.m_subTitleText
      << r.m_subTitleFont
      << r.m_subTitleColor
      << r.m_titleText
      << r.m_titleFont
      << r.m_titleColor
      << r.m_multipleCurveLabel
      << r.m_worldXMin
      << r.m_worldXMax
      << r.m_worldYMin
      << r.m_worldYMax
      << r.m_worldBgXMin
      << r.m_worldBgXMax
      << r.m_worldBgYMin
      << r.m_worldBgYMax
      << r.m_worldBgBrush
      << (Q_INT32) r.m_worldBgUsePixmap
      << r.m_worldBgPixmap
      << r.m_axleList.count()
      << r.m_barList.count()
      << r.m_lineList.count()
      << r.m_markerList.count() ;

    QPtrListIterator<GraphAxle> axleIt( r.m_axleList );
    for ( axleIt.toFirst();
          axleIt.current();
          ++axleIt )
    {
        GraphAxle &ar = *axleIt.current();
        s << ar;
    }
    QPtrListIterator<GraphBar> barIt( r.m_barList );
    for ( barIt.toFirst();
          barIt.current();
          ++barIt )
    {
        GraphBar &br = *barIt.current();
        s << br;
    }
    QPtrListIterator<GraphLine> lineIt( r.m_lineList );
    for ( lineIt.toFirst();
          lineIt.current();
          ++lineIt )
    {
        GraphLine &lr = *lineIt.current();
        s << lr;
    }
    QPtrListIterator<GraphMarker> markerIt( r.m_markerList );
    for ( markerIt.toFirst();
          markerIt.current();
          ++markerIt )
    {
        GraphMarker &mr = *markerIt.current();
        s << mr;
    }
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief Graph operator>>().
 *
 *  Reads a Graph and its GraphAxles, GraphBars, and GraphLines from a data
 *  stream.
 *  Assumes the referenced Graph has no axles, bars, or lines
 *  since these will be read in from the stream.
 *
 *  \return Reference to the input QDataStream.
 */

QDataStream& operator>>( QDataStream &s, Graph &r )
{
    int axles, bars, lines, markers, i;
    s >> r.m_canvasX0
      >> r.m_canvasY0
      >> r.m_canvasWd
      >> r.m_canvasHt
      >> r.m_canvasBd
      >> r.m_canvasBgBrush
      >> (Q_INT32 &) r.m_canvasBgUsePixmap
      >> r.m_canvasBgPixmap
      >> r.m_canvasBoxPen
      >> r.m_canvasRotation
      >> r.m_canvasScaleX
      >> r.m_canvasScaleY
      >> r.m_subTitleText
      >> r.m_subTitleFont
      >> r.m_subTitleColor
      >> r.m_titleText
      >> r.m_titleFont
      >> r.m_titleColor
      >> r.m_multipleCurveLabel
      >> r.m_worldXMin
      >> r.m_worldXMax
      >> r.m_worldYMin
      >> r.m_worldYMax
      >> r.m_worldBgXMin
      >> r.m_worldBgXMax
      >> r.m_worldBgYMin
      >> r.m_worldBgYMax
      >> r.m_worldBgBrush
      >> (Q_INT32 &) r.m_worldBgUsePixmap
      >> r.m_worldBgPixmap
      >> axles
      >> bars
      >> lines
      >> markers ;
    for ( i = 0;
          i < axles;
          i++ )
    {
        GraphAxle *axle = new GraphAxle;
        checkmem( __FILE__, __LINE__, axle, "GraphAxle axle", 1 );
        r.m_axleList.append( axle );
        GraphAxle &ar = *axle;
        s >> ar;
    }
    for ( i = 0;
          i < bars;
          i++ )
    {
        GraphBar *bar = new GraphBar;
        checkmem( __FILE__, __LINE__, bar, "GraphBar bar", 1 );
        r.m_barList.append( bar );
        GraphBar &br = *bar;
        s >> br;
    }
    for ( i = 0;
          i < lines;
          i++ )
    {
        GraphLine *line = new GraphLine;
        checkmem( __FILE__, __LINE__, line, "GraphLine line", 1 );
        r.m_lineList.append( line );
        GraphLine &lr = *line;
        s >> lr;
    }
    for ( i = 0;
          i < markers;
          i++ )
    {
        GraphMarker *marker = new GraphMarker;
        checkmem( __FILE__, __LINE__, marker, "GraphMarker marker", 1 );
        r.m_markerList.append( marker );
        GraphMarker &mr = *marker;
        s >> mr;
    }
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief Dumps the graph to a file.
 *
 *  \param r Reference to the Graph.
 *  \param fileName Output file name.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool graphDump ( const Graph &r, const QString &fileName )
{
    QFile file( fileName );
    if ( ! file.open( IO_WriteOnly ) )
    {
        return( false );
    }
    QDataStream stream( &file );
    stream << r;
    file.close();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Loads the graph from a file.
 *
 *  \param r Reference to the Graph.
 *  \param fileName Input file name.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool graphLoad ( Graph &r, const QString &fileName )
{
    QFile file( fileName );
    if ( ! file.open( IO_ReadOnly ) )
    {
        return( false );
    }
    QDataStream stream( &file );
    stream >> r;
    file.close();
    return( true );
}

//------------------------------------------------------------------------------
//  End of graph.cpp
//------------------------------------------------------------------------------

