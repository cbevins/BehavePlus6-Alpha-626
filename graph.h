//------------------------------------------------------------------------------
/*! \file graph.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Graph class declaration.
 */

#ifndef _GRAPH_H_
/*! \def _GRAPH_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPH_H_ 1

// Custom class references
#include "graphaxle.h"
#include "graphbar.h"
#include "graphline.h"
#include "graphmarker.h"

// Qt class references
#include <qbrush.h>
#include <qfont.h>
#include <qptrlist.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qwmatrix.h>
class QRect;
class QPainter;
class QString;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class Graph graph.h
 *  \brief Graph composition and drawing class.
 *
 *  \note Note that there are 3 coordinate systems in use.
 *  \arg CANVAS coordinates are pixels corresponding to the physical device.
 *  \arg WORLD coordinates map the canvas to the graph points.
 *       All line and Bar coordinates are in World Coordinates.
 *  \arg AXLE coordinates are usually in WORLD coordinates.
 *       Exceptions are if two or more different Y axis are being displayed,
 *       for example flame length and scorch height.
 */

class Graph
{
// Function members
public:
    Graph();
    ~Graph();
    GraphAxle   *addGraphAxle( double scaleBeg, double scaleEnd,
                    const QPen &pen ) ;
    GraphBar    *addGraphBar( double x0, double y0, double x1, double y1,
                    const QBrush &brush, const QPen &pen ) ;
    GraphLine   *addGraphLine( int points, double *x, double *y,
                    const QPen &pen ) ;
    GraphMarker *addGraphMarker( double x, double y, const QString &text,
                    const QFont &font, const QColor &color,
                    int align=Qt::AlignLeft|Qt::AlignTop ) ;

    void draw( QPainter *p, double fontScale=1.0 ) ;
    void drawBegin( QPainter *p ) ;
    void drawContent( QPainter *p ) ;
    void drawDecoration( QPainter *p ) ;
    void drawEnd( QPainter *p ) ;

    void drawGraphAxle( QPainter *p, GraphAxle *axle ) ;
    int  drawGraphAxleMajorTic( QPainter *p, GraphAxle *axle ) ;
    void drawGraphAxleMinorTic( QPainter *p, GraphAxle *axle ) ;
    int  drawGraphAxleSubTitle( QPainter *p, GraphAxle *axle, int offset ) ;
    int  drawGraphAxleTitle( QPainter *p, GraphAxle *axle, int offset ) ;
    void drawGraphBar( QPainter *p, GraphBar *bar ) ;
    void drawGraphBarLabel( QPainter *p, GraphBar *bar ) ;
    void drawCanvasBackground ( QPainter *p ) ;
    void drawCanvasSubTitle( QPainter *p ) ;
    void drawCanvasTitle( QPainter *p ) ;
    void drawWorldBackground ( QPainter *p ) ;
    void drawGraphLine( QPainter *p, GraphLine *line ) ;
    void drawGraphLineLabel( QPainter *p, GraphLine *line ) ;
    void drawGraphMarker( QPainter *p, GraphMarker *marker ) ;

    GraphAxle *getGraphAxle( unsigned int index ) ;
    GraphBar  *getGraphBar( unsigned int index ) ;
    GraphLine *getGraphLine( unsigned int index ) ;
    GraphMarker *getGraphMarker( unsigned int index ) ;

    void setCanvas( int x0, int y0, int width, int height, int border );
    void setCanvasBg( const QBrush &bgBrush ) ;
    void setCanvasBgPixmap( QPixmap &pixmap ) ;
    void setCanvasBox( const QPen &pen ) ;
    void setCanvasRotation( double rotation ) ;
    void setCanvasScale( double xScale, double yScale ) ;
    void setMultipleCurveLabel( const QString &text ) ;
    void setSubTitle( const QString &text, const QFont &font, const QColor &color );
    void setTitle( const QString &text, const QFont &font, const QColor &color );
    void setWorld( double xMin, double yMin, double xMax, double yMax ) ;
    void setWorldBg ( double xMin, double yMin, double xMax, double yMax,
        const QBrush &worldBrush ) ;
    void setWorldBgPixmap( const QPixmap & pixmap ) ;
    void unsetCanvasBgPixmap( void ) ;
    void unsetWorldBgPixmap( void ) ;

// Serialization
    friend QDataStream& operator<<( QDataStream &s, const Graph &r ) ;
    friend QDataStream& operator>>( QDataStream &s, Graph &r ) ;
    friend bool graphDump ( const Graph &r, const QString &fileName ) ;
    friend bool graphLoad ( Graph &r, const QString &fileName ) ;

    int  toCanvasX( double worldX ) const ;
    int  toCanvasY( double worldY ) const ;

#ifdef _DEVELOPMENTAL_
    void print( FILE *fptr ) ;
#endif

// Data members
public:
    // setCanvas(): Canvas location, size, scale, and orientation
    int         m_canvasX0;         //!< Canvas UL corner x pixel position
    int         m_canvasY0;         //!< Canvas UL corner y pixel position
    int         m_canvasWd;         //!< Canvas width in pixels
    int         m_canvasHt;         //!< Canvas height in pixels
    int         m_canvasBd;         //!< Canvas internal border in pixels
    // setCanvasBg(): Canvas background
    QBrush      m_canvasBgBrush;    //!< Canvas background brush color
    // setCanvasBgPixmap(): optional canvas background pixmap
    QPixmap     m_canvasBgPixmap;   //!< Canvas background brush pixmap
    bool        m_canvasBgUsePixmap;
    // setCanvasBox(): Canvas enclosing box
    QPen        m_canvasBoxPen;     //!< Pen for the canvas enclosing box
    // setCanvasRotation():
    double      m_canvasRotation;   //!< Canvas rotation
    // setCanvasScale()
    double      m_canvasScaleX;     //!< X dimensions scale factor
    double      m_canvasScaleY;     //!< Y dimensions scale factor
    // setSubTitle()
    QString     m_subTitleText;     //!< Subtitle text lines
    QFont       m_subTitleFont;     //!< Subtitle font family
    QColor      m_subTitleColor;    //!< Subtitle font color
    // setTitle()
    QString     m_titleText;        //!< Title text lines
    QFont       m_titleFont;        //!< Title font family
    QColor      m_titleColor;       //!< Title font color
    // setWorld(): world coordinates for the corners of the paint device
    double      m_worldXMin;        //!< Minimum x value in world space
    double      m_worldXMax;        //!< Maximum x value in world space
    double      m_worldYMin;        //!< Minimum y value in world space
    double      m_worldYMax;        //!< Maximum y value in world space
    // setWorldBg(): sets the graph area location & dimensions
    double      m_worldBgXMin;      //!< Minimum x value in the World space
    double      m_worldBgXMax;      //!< Maximum x value in the World space
    double      m_worldBgYMin;      //!< Minimum y value in the World space
    double      m_worldBgYMax;      //!< Maximum y value in the World space
    QBrush      m_worldBgBrush;     //!< World graph area background brush color
    // setWorldBgPixmap(): optional canvas background pixmap
    QPixmap     m_worldBgPixmap;    //!< Canvas background brush pixmap
    bool        m_worldBgUsePixmap; //!< Toggle to use world background pixmap
    // Axis
    QPtrList<GraphAxle> m_axleList; //!< List of all GraphAxles (for automatic drawing)
    QPtrList<GraphBar>  m_barList;  //!< List of all GraphBars  (for automatic drawing)
    QPtrList<GraphLine> m_lineList; //!< List of all GraphLines (for automatic drawing)
    QPtrList<GraphMarker> m_markerList;//!< List of all GraphMarkers (for automatic drawing)
    // Transformation
    QWMatrix    m_matrix;           //!< Current transformation matrix
    QWMatrix    m_oldMatrix;        //!< Previous transformation matrix
    // Remaining space in the layout
    QRect       m_layout;           //!<
    QString     m_multipleCurveLabel;//!< Label applied to curve families
    double      m_fontScale;        //!< Font scaling factor
};

// Serialization
    QDataStream& operator<<( QDataStream &s, const Graph &r ) ;
    QDataStream& operator>>( QDataStream &s, Graph &r ) ;

//------------------------------------------------------------------------------
//  Support function prototypes
//------------------------------------------------------------------------------

void drawRotatedText( QPainter *p, double deg, int x, int y, const QString &text ) ;

bool graphDump ( const Graph &r, const QString &fileName ) ;

bool graphLoad ( Graph &r, const QString &fileName ) ;

#endif

//------------------------------------------------------------------------------
//  End of graph.h
//------------------------------------------------------------------------------

