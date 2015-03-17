//------------------------------------------------------------------------------
/*! \file graphaxle.h
 *  \version BehavePlus5
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphAxle class declaration.
 */

#ifndef _GRAPHAXLE_H_
/*! \def _GRAPHAXLE_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPHAXLE_H_ 1

// Qt class references
#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>
class QDataStream;
class QString;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \var GraphAxleSide
 *  \brief Defines the side of the Graph on which the GraphAxle is placed,
 *  as well as side of the graphAxle that tic marks and labels are placed.
 */

enum GraphAxleSide
{
    GraphAxleNone=0,
    GraphAxleTop,
    GraphAxleBottom,
    GraphAxleLeft,
    GraphAxleRight
};

//------------------------------------------------------------------------------
/*! \class GraphAxle graphaxle.h
 *
 *  \brief GraphAxle defines one complete graph axle including scale, axle,
 *  tic marks, and titles.
 *
 *  \note Note that there are 3 coordinate systems in use.
 *  \arg CANVAS coordinates are pixels corresponding to the physical device.
 *  \arg WORLD coordinates map the canvas to the graph points.
 *       All GraphLine and GraphBar coordinates are in World Coordinates.
 *  \arg AXLE coordinates are usually in WORLD coordinates.
 *       Exceptions are if two or more different Y axis are being displayed,
 *       for example flame length and scorch height.
 */

class GraphAxle
{
// Public methods
public:
    GraphAxle( void );
    GraphAxle( double scaleBeg, double scaleEnd, const QPen &pen );
    GraphAxle( GraphAxle &source );
#ifdef _DEVELOPMENTAL_
    void print( FILE *fptr ) ;
#endif
    void setGraphAxle( double scaleBeg, double scaleEnd, const QPen &pen );
    void setWorld( double xBeg, double yBeg, double xEnd, double yEnd );
    void setMajorGrid( const QPen &gridPen, double length );
    void setMajorLabels( const QFont &font, const QColor &color,
            int decimals=-1 );
    void setMajorTics( GraphAxleSide side, double first, double last,
            double step, double length, const QPen &pen );
    void setMinorGrid( const QPen &gridPen, double length );
    void setMinorTics( GraphAxleSide side, double first, double last,
            double step, double length, const QPen &pen );
    void setSubTitle( const QString &text, GraphAxleSide side,
            const QFont &font, const QColor &color );
    void setTitle( const QString &text, GraphAxleSide side, const QFont &font,
            const QColor &color );

// Serialization
    friend QDataStream& operator<<( QDataStream &s, const GraphAxle &r ) ;
    friend QDataStream& operator>>( QDataStream &s, GraphAxle &r ) ;

// Public data members
public:
    // GraphAxle coordinate scale: setGraphAxle()
    double    m_axleScaleBeg;       //!< Value of axle scale at left or lower end
    double    m_axleScaleEnd;       //!< Value of axle scale at right or upper end
    QPen      m_axlePen;            //!< GraphAxle pen color, width, and style
    // Major grid lines: setMajorGrid()
    QPen      m_majorGridPen;       //!< Grid line color, width, and style
    double    m_majorGridLength;    //!< Grid line length in perpendicular World units
    // Major tic labels: setMajorLabels()
    QFont     m_majorLabelsFont;    //!< Font used to label the tics
    QColor    m_majorLabelsColor;   //!< Font color for tic labels
    int       m_majorLabelsDecimals;//!< -1 indicates no labels
    // Major tic marks: setMajorTics()
    GraphAxleSide  m_majorTicSide;  //!< Tic side
    double    m_majorTicFirst;      //!< Location of first tic mark in GraphAxle coords
    double    m_majorTicLast;       //!< Location of last tic mark in GraphAxle coords
    double    m_majorTicStep;       //!< Tic mark step in GraphAxle coordinates
    double    m_majorTicLength;     //!< Tic length in World coordinates
    QPen      m_majorTicPen;        //!< Tic color, width, and style
    // Minor grid lines: setMinorGrid()
    QPen      m_minorGridPen;       //!< Grid line color, width, and style
    double    m_minorGridLength;    //!< Grid line len in World perpendicular units
    // Minor tic marks: setMinorTics()
    GraphAxleSide  m_minorTicSide;  //!< Tic side
    double    m_minorTicFirst;      //!< Location of first tic mark in GraphAxle coords
    double    m_minorTicLast;       //!< Location of last tic mark in GraphAxle coords
    double    m_minorTicStep;       //!< Tic mark step in GraphAxle coordinates
    double    m_minorTicLength;     //!< Tic length in World coordinates
    QPen      m_minorTicPen;        //!< Tic color, width, and style
    // GraphAxle subTitle attributes
    QString   m_subTitleText;       //!< Subtitle text lines
    GraphAxleSide  m_subTitleSide;  //!< Subtitle side
    QFont     m_subTitleFont;       //!< Subtitle font
    QColor    m_subTitleColor;      //!< Subtitle font color
    // GraphAxle title attributes
    QString   m_titleText;          //!< Title text lines
    GraphAxleSide  m_titleSide;     //!< Title side
    QFont     m_titleFont;          //!< Title font
    QColor    m_titleColor;         //!< Title font color
    // Location in the canvas's WORLD coordinate space: setWorld();
    double    m_worldXBeg;          //!< GraphAxle's start X in world coordinate space
    double    m_worldYBeg;          //!< GraphAxle's start Y in world coordinate space
    double    m_worldXEnd;          //!< GraphAxle's stop X in world coordinate space
    double    m_worldYEnd;          //!< GraphAxle's stop Y in world coordinate space
};

//------------------------------------------------------------------------------
/*! \class GraphAxleParms graphaxle.h
 *
 *  \brief Defines graph axle range, tic marks, and decimals.
 *
 *  Used as a mechanism to allow both automatic and user-specified axle
 *  parameters.
 */

class GraphAxleParms
{
// Public methods
public:
    GraphAxleParms( double dataMin=0., double dataMax=1., int tics=11 ) ;
    void init( void ) ;
    //void set( double dataMin=0., double dataMax=1., int tics=11 ) ;
    void setAxle( double axleMin, double axleMax, int tics=1 ) ;
    void useOrigin( void );

// Public data
public:
    double m_dataMin;
    double m_dataMax;
    double m_axleMin;
    double m_axleMax;
    double m_majorStep;
    int    m_tics;
    int    m_majorTics;
    int    m_decimals;
};

//------------------------------------------------------------------------------
//  Support function prototypes
//------------------------------------------------------------------------------

// Serialization
    QDataStream& operator<<( QDataStream &s, const GraphAxle &r ) ;
    QDataStream& operator>>( QDataStream &s, GraphAxle &r ) ;

// Axle conformation
void niceAxis( double vMin, double vMax, int vTics,
    double *gMin, double *gMax, int *gTics, double *gStep, int *gDec,
	bool fixed_range=false ) ;

double niceNumber( double val, bool round=false ) ;

#endif

//------------------------------------------------------------------------------
//  End of graphaxle.h
//------------------------------------------------------------------------------

