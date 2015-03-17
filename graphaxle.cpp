//------------------------------------------------------------------------------
/*! \file graphaxle.cpp
 *  \version BehavePlus5
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief GraphAxle class methods.
 */

// Custom include files
#include "graphaxle.h"

// Qt include files
#include <qdatastream.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief GraphAxle default constructor.
 */

GraphAxle::GraphAxle( void )
{
    QColor color( Qt::black );
    QPen pen( color, 1, Qt::SolidLine );
    QFont font( "Times New Roman", 12, QFont::Normal, false );
    // The following completely define an GraphAxle & initialize all members.
    setGraphAxle( 0., 0., pen );
    setWorld( 0, 0, 0, 0 );
    setMajorGrid( pen, 0. );
    setMajorTics( GraphAxleLeft, 0., 0., 0., 0., pen );
    setMajorLabels( font, color, -1 );
    setMinorGrid( pen, 0. );
    setMinorTics( GraphAxleLeft, 0., 0., 0., 0., pen );
    setSubTitle( "", GraphAxleLeft, font, color );
    setTitle( "", GraphAxleLeft, font, color );
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphAxle custom constructor.
 *
 *  \param scaleBeg First scale value.
 *  \param scaleEnd Last scale value.
 *  \param axlePen Reference to the QPen used to draw the axle.
 */

GraphAxle::GraphAxle( double scaleBeg, double scaleEnd, const QPen &axlePen )
{
    QColor color( Qt::black );
    QPen pen( color, 1, Qt::SolidLine );
    QFont font( "Times NEw Roman", 12, QFont::Normal, false );
    // The following completely define an GraphAxle & initialize all members.
    setGraphAxle( scaleBeg, scaleEnd, axlePen );
    setWorld( 0, 0, 0, 0 );
    setMajorGrid( pen, 0. );
    setMajorTics( GraphAxleLeft, 0., 0., 0., 0., pen );
    setMajorLabels( font, color, -1 );
    setMinorGrid( pen, 0. );
    setMinorTics( GraphAxleLeft, 0., 0., 0., 0., pen );
    setSubTitle( "", GraphAxleLeft, font, color );
    setTitle( "", GraphAxleLeft, font, color );
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphAxle copy constructor.
 */

GraphAxle::GraphAxle( GraphAxle &s )
{
    // 1 All members that are classes have been default constructed; REASSIGN
    m_axleScaleBeg =        s.m_axleScaleBeg;
    m_axleScaleEnd =        s.m_axleScaleEnd;
    m_axlePen =             s.m_axlePen;
    m_worldXBeg =           s.m_worldXBeg;
    m_worldYBeg =           s.m_worldYBeg;
    m_worldXEnd =           s.m_worldXEnd;
    m_worldYEnd =           s.m_worldYEnd;
    m_majorGridPen =        s.m_majorGridPen;
    m_majorGridLength =     s.m_majorGridLength;
    m_majorLabelsFont =     s.m_majorLabelsFont;
    m_majorLabelsColor =    s.m_majorLabelsColor;
    m_majorLabelsDecimals = s.m_majorLabelsDecimals;
    m_majorTicSide =        s.m_majorTicSide;
    m_majorTicFirst =       s.m_majorTicFirst;
    m_majorTicLast =        s.m_majorTicLast;
    m_majorTicStep =        s.m_majorTicStep;
    m_majorTicLength =      s.m_majorTicLength;
    m_majorTicPen =         s.m_majorTicPen;
    m_minorGridPen =        s.m_minorGridPen;
    m_minorGridLength =     s.m_minorGridLength;
    m_minorTicSide =        s.m_minorTicSide;
    m_minorTicFirst =       s.m_minorTicFirst;
    m_minorTicLast =        s.m_minorTicLast;
    m_minorTicStep =        s.m_minorTicStep;
    m_minorTicLength =      s.m_minorTicLength;
    m_minorTicPen =         s.m_minorTicPen;
    m_subTitleText =        s.m_subTitleText;
    m_subTitleSide =        s.m_subTitleSide;
    m_subTitleFont =        s.m_subTitleFont;
    m_subTitleColor =       s.m_subTitleColor;
    m_titleText =           s.m_titleText;
    m_titleSide =           s.m_titleSide;
    m_titleFont =           s.m_titleFont;
    m_titleColor =          s.m_titleColor;
    // 2 Chain to base class assignment operator: NONE
    // 3 Chain to data member's assignment operators
    // Manage memory: NONE
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the GraphAxle's data elements to the file stream.
 */

#ifdef _DEVELOPMENTAL_

void GraphAxle::print( FILE *fptr )
{
    // setGraphAxle();
    fprintf( fptr,
        "setGraphAxle( m_axleScaleBeg=%f, m_axleScaleEnd=%f, m_pen(color(%d,%d,%d), "
        "width=%d, style=%d) );\n",
        m_axleScaleBeg, m_axleScaleEnd,
        m_axlePen.color().red(), m_axlePen.color().green(),
        m_axlePen.color().blue(), m_axlePen.width(), m_axlePen.style() );
    // setWorld();
    fprintf( fptr,
        "setWorld( m_worldXBeg=%f, m_worldYBeg=%f, m_worldXEnd=%f, m_worldYEnd=%f );\n",
        m_worldXBeg, m_worldYBeg, m_worldXEnd, m_worldYEnd );
    // setMajorGrid();
    fprintf( fptr,
        "setMajorGrid( m_majorGridPen( color(%d,%d,%d), width=%d, style=%d), "
        "m_majorGridPenLength=%f );\n",
        m_majorGridPen.color().red(), m_majorGridPen.color().green(),
        m_majorGridPen.color().blue(), m_majorGridPen.width(),
        m_majorGridPen.style(), m_majorGridLength );
    // setMajorLabels();
    fprintf( fptr,
        "setMajorLabels( m_majorLabelsFont(%s, %d, %d), "
        "m_majorLabelsColor(%d,%d,%d), m_majorLabelsDecimals=%d );\n",
        m_majorLabelsFont.family().latin1(), m_majorLabelsFont.pointSize(),
        m_majorLabelsFont.weight(), m_majorLabelsColor.red(),
        m_majorLabelsColor.green(), m_majorLabelsColor.blue(),
        m_majorLabelsDecimals );
    // setMajorTic();
    fprintf( fptr,
        "setMajorTics( m_majorTicSide=%d, m_majorTicFirst=%f, "
        "m_majorTicLast=%f, m_majorTicStep=%f, m_majorTicLength=%f, "
        "m_majorTicPen( color(%d,%d,%d), width=%d, style=%d );\n",
        m_majorTicSide, m_majorTicFirst, m_majorTicLast, m_majorTicStep,
        m_majorTicLength, m_majorTicPen.color().red(),
        m_majorTicPen.color().green(), m_majorTicPen.color().blue(),
        m_majorTicPen.width(), m_majorTicPen.style() );
    // setMinorGrid();
    fprintf( fptr,
        "setMinorGrid( m_minorTicPen( color(%d,%d,%d), width=%d, style=%d), "
        "m_minorTicLength=%f );\n",
        m_minorGridPen.color().red(), m_minorGridPen.color().green(),
        m_minorGridPen.color().blue(), m_minorGridPen.width(),
        m_minorGridPen.style(), m_minorGridLength );
    // setMinorTic();
    fprintf( fptr,
        "setMinorTics( m_minorTicSide=%d, m_minorTicFirst=%f, "
        "m_minorTicLast=%f, m_minorTicStep=%f, m_minorTicLength=%f, "
        "m_minorTicPen( color(%d, %d, %d), width=%d, style=%d );\n",
        m_minorTicSide, m_minorTicFirst, m_minorTicLast, m_minorTicStep,
        m_minorTicLength, m_minorTicPen.color().red(),
        m_minorTicPen.color().green(), m_minorTicPen.color().blue(),
        m_minorTicPen.width(), m_minorTicPen.style() );
    // setSubTitle();
    fprintf( fptr,
        "setSubTitle( m_subTitlteText=\"%s\", m_subTitleSide=%d, "
        "m_subTitleFont(%s, %d, %d), m_subTitleColor(%d, %d, %d) );\n",
        m_subTitleText.latin1(), m_subTitleSide,
        m_subTitleFont.family().latin1(), m_subTitleFont.pointSize(),
        m_subTitleFont.weight(), m_subTitleColor.red(),
        m_subTitleColor.green(), m_subTitleColor.blue() );
    // setTitle();
    fprintf( fptr,
        "setTitle( m_titleText=\"%s\", m_titleSide=%d, "
        "m_titleFont(%s, %d, %d), m_titleColor(%d, %d, %d) );\n",
        m_titleText.latin1(), m_titleSide,
        m_titleFont.family().latin1(), m_titleFont.pointSize(),
        m_titleFont.weight(), m_titleColor.red(),
        m_titleColor.green(), m_titleColor.blue() );
    return;
}

#endif

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's axle scale and QPen.
 *
 *  \param scaleBeg First scale value.
 *  \param scaleEnd Last scale value.
 *  \param axlePen Reference to the QPen used to draw the axle.
 */

void GraphAxle::setGraphAxle( double scaleBeg, double scaleEnd, const QPen &pen )
{
    m_axleScaleBeg = scaleBeg;
    m_axleScaleEnd = scaleEnd;
    m_axlePen      = pen;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's major grid QPen and length.
 *
 *  \brief pen Reference to the QPen to apply to the grid lines.
 *  \brief length Grid line length in axle scale units.
 */

void GraphAxle::setMajorGrid( const QPen &pen, double length )
{
    m_majorGridPen    = pen;
    m_majorGridLength = length;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's major label QFont, QColor, and decimals.
 *
 *  \brief font Reference to the QFont to apply to the labels.
 *  \brief color Reference to the QColor to apply to the labels.
 *  \brief decimals Number of decimal places to display on scale labels.
 */

void GraphAxle::setMajorLabels( const QFont &font, const QColor &color,
    int decimals )
{
    m_majorLabelsFont     = font;
    m_majorLabelsColor    = color;
    m_majorLabelsDecimals = decimals;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's major tic mark parameters.
 *
 *  \brief side GraphAxleSide where the tic marks are drawn.
 *  \brief first First tic mark is drawn at this scale value.
 *  \brief last Last tic mark is drawn at this scale value.
 *  \brief length Tic mark length in axle scale units.
  * \brief pen Reference to a QPen used to draw the tic marks.
 */

void GraphAxle::setMajorTics( GraphAxleSide side, double first, double last,
        double step, double length, const QPen &pen )
{
    m_majorTicSide   = side;
    m_majorTicFirst  = first;
    m_majorTicLast   = last;
    m_majorTicStep   = step;
    m_majorTicLength = length;
    m_majorTicPen    = pen;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's minor grid QPen and length.
 *
 *  \brief pen Reference to the QPen to apply to the grid lines.
 *  \brief length Grid line length in axle scale units.
 */

void GraphAxle::setMinorGrid( const QPen &pen, double length )
{
    m_minorGridPen    = pen;
    m_minorGridLength = length;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's minor tic mark parameters.
 *
 *  \brief side GraphAxleSide where the tic marks are drawn.
 *  \brief first First tic mark is drawn at this scale value.
 *  \brief last Last tic mark is drawn at this scale value.
 *  \brief length Tic mark length in axle scale units.
  * \brief pen Reference to a QPen used to draw the tic marks.
 */

void GraphAxle::setMinorTics( GraphAxleSide side, double first, double last,
    double step, double length, const QPen &pen )
{
    m_minorTicSide   = side;
    m_minorTicFirst  = first;
    m_minorTicLast   = last;
    m_minorTicStep   = step;
    m_minorTicLength = length;
    m_minorTicPen    = pen;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's subtitle.
 *
 *  \param text Subtitle text.
 *  \brief side GraphAxleSide where the subtitle is drawn.
 *  \brief font Reference to the QFont to apply to the subtitle.
 *  \brief color Reference to the QColor to apply to the subtitle.
 */

void GraphAxle::setSubTitle( const QString &text, GraphAxleSide side,
    const QFont &font, const QColor &color )
{
    m_subTitleText  = text;
    m_subTitleSide  = side;
    m_subTitleFont  = font;
    m_subTitleColor = color;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the GraphAxle's title.
 *
 *  \param text Title text.
 *  \brief side GraphAxleSide where the title is drawn.
 *  \brief font Reference to the QFont to apply to the title.
 *  \brief color Reference to the QColor to apply to the title.
 */

void GraphAxle::setTitle( const QString &text, GraphAxleSide side, const QFont &font,
    const QColor &color )
{
    m_titleText  = text;
    m_titleSide  = side;
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

void GraphAxle::setWorld( double xBeg, double yBeg, double xEnd, double yEnd )
{
    m_worldXBeg = xBeg;
    m_worldYBeg = yBeg;
    m_worldXEnd = xEnd;
    m_worldYEnd = yEnd;
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphAxle operator<<().
 *
 *  Serializes the GraphAxle to a data stream.
 *
 *  \return Reference to the output QDataStream.
 */

QDataStream& operator<<( QDataStream &s, const GraphAxle &r )
{
    s << r.m_axleScaleBeg
      << r.m_axleScaleEnd
      << r.m_axlePen
      << r.m_majorGridPen
      << r.m_majorGridLength
      << r.m_majorLabelsFont
      << r.m_majorLabelsColor
      << r.m_majorLabelsDecimals
      << (Q_INT32) r.m_majorTicSide
      << r.m_majorTicFirst
      << r.m_majorTicLast
      << r.m_majorTicStep
      << r.m_majorTicLength
      << r.m_majorTicPen
      << r.m_minorGridPen
      << r.m_minorGridLength
      << (Q_INT32) r.m_minorTicSide
      << r.m_minorTicFirst
      << r.m_minorTicLast
      << r.m_minorTicStep
      << r.m_minorTicLength
      << r.m_minorTicPen
      << r.m_subTitleText
      << (Q_INT32) r.m_subTitleSide
      << r.m_subTitleFont
      << r.m_subTitleColor
      << r.m_titleText
      << (Q_INT32) r.m_titleSide
      << r.m_titleFont
      << r.m_titleColor
      << r.m_worldXBeg
      << r.m_worldYBeg
      << r.m_worldXEnd
      << r.m_worldYEnd ;
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief GraphAxle operator>>().
 *
 *  Reads a GraphAxle from a data stream.
 *
 *  \return Reference to the input QDataStream.
 */

QDataStream& operator>>( QDataStream &s, GraphAxle &r )
{
    s >> r.m_axleScaleBeg
      >> r.m_axleScaleEnd
      >> r.m_axlePen
      >> r.m_majorGridPen
      >> r.m_majorGridLength
      >> r.m_majorLabelsFont
      >> r.m_majorLabelsColor
      >> r.m_majorLabelsDecimals
      >> (Q_INT32 &) r.m_majorTicSide
      >> r.m_majorTicFirst
      >> r.m_majorTicLast
      >> r.m_majorTicStep
      >> r.m_majorTicLength
      >> r.m_majorTicPen
      >> r.m_minorGridPen
      >> r.m_minorGridLength
      >> (Q_INT32 &) r.m_minorTicSide
      >> r.m_minorTicFirst
      >> r.m_minorTicLast
      >> r.m_minorTicStep
      >> r.m_minorTicLength
      >> r.m_minorTicPen
      >> r.m_subTitleText
      >> (Q_INT32 &) r.m_subTitleSide
      >> r.m_subTitleFont
      >> r.m_subTitleColor
      >> r.m_titleText
      >> (Q_INT32 &) r.m_titleSide
      >> r.m_titleFont
      >> r.m_titleColor
      >> r.m_worldXBeg
      >> r.m_worldYBeg
      >> r.m_worldXEnd
      >> r.m_worldYEnd ;
    return( s );
}

//------------------------------------------------------------------------------
/*! \brief GraphAxleParms constructor.
 */

GraphAxleParms::GraphAxleParms( double dataMin, double dataMax, int tics ) :
    m_dataMin( dataMin ),
    m_dataMax( dataMax ),
    m_axleMin( dataMin ),
    m_axleMax( dataMax ),
    m_majorStep( dataMax - dataMin ),
    m_tics( tics ),
    m_majorTics( 1 ),
    m_decimals( 0 )

{
    init();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Attempts to determine a nice graph axle size.
 */

void GraphAxleParms::init( void )
{
    //niceAxis( m_dataMin, m_dataMax, m_tics,
    niceAxis( m_axleMin, m_dataMax, m_tics,
        &m_axleMin, &m_axleMax, &m_majorTics, &m_majorStep, &m_decimals );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Not sure if or where this is used
 */

#ifdef OMIT
void GraphAxleParms::set( double dataMin, double dataMax, int tics )
{
    m_dataMin = dataMin;
    m_dataMax = dataMax;
    m_axleMin = dataMin;
    m_axleMax = dataMax;
    m_majorStep = dataMax - dataMin;
    m_tics = tics;
    m_majorTics = 1;
    m_decimals = 0;
    init();
    return;
}
#endif

//------------------------------------------------------------------------------
/*! \brief Forces the use of the specified axle ranges.
    Called by BpDocument::composeGraphs() when min==max
 */

void GraphAxleParms::setAxle( double axleMin, double axleMax, int tics )
{
    m_axleMin = axleMin;
    m_axleMax = axleMax;
    m_majorStep = axleMax - axleMin;
    m_tics = tics;
    m_majorTics = 1;
    m_decimals = 0;
    return;
}

void GraphAxleParms::useOrigin( void )
{
    m_axleMin = 0.00;
    m_majorStep = m_axleMax - m_axleMin;
    init();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Attempts to determine a nice graph axis size.
 *
 *  \param vMin  Actual graph value minimum.
 *  \param vMax  Actual graph value maximum.
 *  \param vTics Approximate number of requested tic marks (>=2).
 *  \param gMin  Address of returned nice graph minimum value.
 *  \param gMax  Address of returned nice graph maximum value.
 *  \param gTics Address of returned nice graph tic marks.
 *  \param gStep Address of returned nice graph tic step size.
 *  \param gDec  Address of returned number of decimal places.
 *	\param fixed_range If TRUE, vMin and vMax are the fixed axel range (default==FALSE).
 *
 *  \return On return the calculated "nice" numbers are stored at the
 *  \a gMin, \a gMax, \a gTics, \a gSteps, and \a gDec argument addresses.
 */

void niceAxis( double vMin, double vMax, int vTics,
    double *gMin, double *gMax, int *gTics, double *gStep, int *gDec, bool fixed_range )
{
    // Check for case where vMin==vMax (constant)
    if ( (vMax-vMin) < 1.0e-10 )
    {
        // If zero, make this middle of the range [-1, 1]
        if ( fabs(vMax) < 1.0e-10 )
        {
            vMin = -1.;
            vMax = 1.;
        }
        // If not zero, make this the middle the range [0, 2*vMax].
        else if ( vMin > 0. )
        {
            vMin = 0.;
            vMax = 2. * vMax;
        }
        else if ( vMin < 0. )
        {
            vMax = 0.;
            vMin = 2. * vMin;
        }
    }
	double range = fixed_range ? (vMax-vMin) : niceNumber( (vMax-vMin), false );
    double step = niceNumber( (range/(double) (vTics-1)), true );
    if ( vTics < 2 )
	{
        vTics = 2;
	}
	if ( fixed_range )
	{
		*gMin = vMin;
		*gMax = vMax;
	}
	else
	{
		*gMin = floor(vMin/step) * step ;
		*gMax = ceil(vMax/step) * step;
	}
    *gTics = 1 + (int) ((*gMax - *gMin) / step);
    if ( (*gDec = (int) (-floor( log10(step) ))) < 0 )
	{
        *gDec = 0;
	}
    *gStep = step;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Finds a nice round number.
 *
 *  Called by niceLabel().
 *
 *  \return The nice, round number.
 */

double niceNumber( double val, bool round )
{
    double e = floor(log10(val));
    double f = val / pow(10., e);
    double nice;
    if ( round )
    {
        if ( f < 1.5 )
            nice = 1.0;
        else if ( f < 3.0 )
            nice = 2.0;
        else if ( f < 7.0 )
            nice = 5.0;
        else
            nice = 10.0;
    }
    else
    {
        if ( f <= 1.0 )
            nice = 1.0;
        else if ( f <= 2.0 )
            nice = 2.0;
        else if ( f <= 5.0 )
            nice = 5.0;
        else
            nice = 10.0;
    }
    return ( nice * pow(10., e) );
}

//------------------------------------------------------------------------------
//  End of graphaxle.cpp
//------------------------------------------------------------------------------

