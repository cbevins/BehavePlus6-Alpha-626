//------------------------------------------------------------------------------
/*! \file graphbar.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphBar class declaration.
 */

#ifndef _GRAPHBAR_H_
/*! \def _GRAPHBAR_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPHBAR_H_ 1

// Qt class references
#include <qbrush.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>
#include <qpixmap.h>
class QDataStream;
class QString;

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class GraphBar graphbar.h
 *
 *  \brief GraphBar defines one complete bar for a bar graph.
 */

class GraphBar
{
// Public methods
public:
    GraphBar( void );
    GraphBar( double x0, double y0, double x1, double y1,
        const QBrush &brush, const QPen &pen );
    GraphBar( GraphBar &source ) ;
#ifdef _DEVELOPMENTAL_
    void print( FILE *fptr ) ;
#endif
    void setGraphBar( double x0, double y0, double x1, double y1,
        const QBrush &brush, const QPen &pen );
    void setGraphBarLabel( const QString &text, double x, double y,
        const QFont &font, const QColor &color, double rotate=0. );
    void setGraphBarPixmap( const QPixmap &pixmap ) ;
    void unsetGraphBarPixmap( void ) ;

// Serialization
    friend QDataStream& operator<<( QDataStream &s, const GraphBar &r ) ;
    friend QDataStream& operator>>( QDataStream &s, GraphBar &r ) ;

// Public data members
public:
    // setGraphBar()
    double  m_barX0;            //!< GraphBar lower left coordinate
    double  m_barY0;            //!< GraphBar lower left coordinate
    double  m_barX1;            //!< GraphBar upper right coordinate
    double  m_barY1;            //!< GraphBar upper right coordinate
    QBrush  m_barBrush;         //!< GraphBar fill brush
    QPen    m_barPen;           //!< GraphBar outline pen
    // setGraphBarLabel()
    QString m_label;            //!< GraphBar label
    double  m_labelX;           //!< GraphBar label x position
    double  m_labelY;           //!< GraphBar label y position
    QFont   m_labelFont;        //!< GraphBar label font
    QColor  m_labelColor;       //!< GraphBar label font color
    double  m_labelRotate;      //!< GraphBar label rotation in degrees
    // setGraphBarPixmap()
    QPixmap m_barPixmap;        //!< GraphBar fill pixmap
    bool    m_barUsePixmap;     //!< Switch for barPixmap
};

// Serialization
    QDataStream& operator<<( QDataStream &s, const GraphBar &r ) ;
    QDataStream& operator>>( QDataStream &s, GraphBar &r ) ;

#endif

//------------------------------------------------------------------------------
//  End of graphbar.h
//------------------------------------------------------------------------------

