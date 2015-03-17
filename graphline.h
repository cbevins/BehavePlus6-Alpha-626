//------------------------------------------------------------------------------
/*! \file graphline.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphLine class declaration.
 */

#ifndef _GRAPHLINE_H_
/*! \def _GRAPHLINE_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPHLINE_H_ 1

// Qt class references
#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>
class QDataStream;
class QString;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class GraphLine graphline.h
 *
 *  \brief GraphLine defines one complete line for a line graph.
 */

class GraphLine
{
// Public methods
public:
    GraphLine( void ) ;
    GraphLine( int points, double *x, double *y, const QPen &pen ) ;
    ~GraphLine( void ) ;
    GraphLine( GraphLine &source ) ;
#ifdef _DEVELOPMENTAL_
    void print( FILE *fptr, bool printPoints=false ) ;
#endif
    void setGraphLine( int points, double *x, double *y, const QPen &pen ) ;
    void setGraphLineLabel( const QString &text, double x, double y,
            const QFont &font, const QColor &color ) ;

// Serialization
    friend QDataStream& operator<<( QDataStream &s, const GraphLine &r ) ;
    friend QDataStream& operator>>( QDataStream &s, GraphLine &r ) ;

// Public data members
public:
    // setGraphLine();
    double *m_x;            //!< Ptr to array of Cartesian X values
    double *m_y;            //!< Ptr to array of Cartesian Y values
    int     m_points;       //!< Points in the x[] and y[] arrays
    QPen    m_linePen;      //!< GraphLine color
    // setGraphLineLabel();
    QString m_label;        //!< GraphLine label
    double  m_labelX;       //!< GraphLine label x position
    double  m_labelY;       //!< GraphLine label y position
    QFont   m_labelFont;    //!< GraphLine label font
    QColor  m_labelColor;   //!< GraphLine label font color
};

// Serialization
    QDataStream& operator<<( QDataStream &s, const GraphLine &r ) ;
    QDataStream& operator>>( QDataStream &s, GraphLine &r ) ;

#endif

//------------------------------------------------------------------------------
//  End of graphline.h
//------------------------------------------------------------------------------

