//------------------------------------------------------------------------------
/*! \file realspinbox.cpp
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins
 *
 *  \brief RealSpinBox class methods.
 */

// Custom include files
#include "realspinbox.h"

// Qt include files
#include <qspinbox.h>
#include <qvalidator.h>

//------------------------------------------------------------------------------
/*! \brief RealSpinBox constructor.
 */

RealSpinBox::RealSpinBox( double min, double max, double step, double start,
        int decimals, QWidget *parent, const char *name ) :
    QSpinBox( -1000000000, 1000000000, 1, parent, name ),
    m_minReal( min ),
    m_maxReal( max ),
    m_stepReal( step ),
    m_startReal( start ),
    m_factor( 1.0 ),
    m_round( 0.1 ),
    m_decimals( decimals ),
    m_validator( 0 )
{
    // Create and use a validator
    m_validator = new QDoubleValidator( m_minReal, m_maxReal, m_decimals,
        this, "realSpinBoxValidator" );
    Q_CHECK_PTR( m_validator );
    setValidator( m_validator );
	setLineStep( mapRealToStep( step ) );
	setValue( mapRealToSpin( m_startReal ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief RealSpinBox destructor.
 */

RealSpinBox::~RealSpinBox( void )
{
    delete m_validator; m_validator = 0;
}

//------------------------------------------------------------------------------
/*! \brief Maps a real value to the spinner value.
 *  \param real_value Real value to be mapped
 *	\return Spinner integer representation of the value
 */
int RealSpinBox::mapRealToSpin( double real_value )
{
	double spin_range = maxValue() - minValue();
	double real_range = m_maxReal - m_minReal;
	double spin_value = 0.;
	if ( real_range != 0. )
	{
		spin_value = minValue() + spin_range * (real_value - m_minReal)/real_range;
	}
	return (int) spin_value;
}
//------------------------------------------------------------------------------
/*! \brief Maps a real step value to the spinner step value.
 *  \param real_value Real step value to be mapped
 *	\return Spinner step integer representation of the value
 */
int RealSpinBox::mapRealToStep( double real_value )
{
	double spin_range = maxValue() - minValue();
	double real_range = m_maxReal - m_minReal;
	double spin_value = 0.;
	if ( real_range != 0. )
	{
		spin_value = spin_range * ( real_value /real_range );
	}
	return (int) spin_value;
}

//------------------------------------------------------------------------------
/*! \brief Maps a spinner value to its real value representation.
 *  \param spin_value Spinner integer value to be mapped to the real value
 *	\return Real value represented by the spinner integer value
 */
double RealSpinBox::mapSpinToReal( int spin_value )
{
	double spin_range = maxValue() - minValue();
	double real_range = m_maxReal - m_minReal;
	double real_value = m_minReal + real_range * ((double) spin_value - minValue())/spin_range;
	return real_value;
}

//------------------------------------------------------------------------------
/*! \brief Reimplemented virtual function to map the current QSpinBox value
 *  to the text displayed in the entry field.
 */

QString RealSpinBox::mapValueToText( int value )
{
    double real = mapSpinToReal( value );
    return( QString( "%1" ).arg( real, 0, 'f', m_decimals ) );
}

//------------------------------------------------------------------------------
/*! \brief Reimplemented virtual function to map the current QSpinBox entry
 *  field to an integer value.
 */

int RealSpinBox::mapTextToValue( bool *ok )
{
    double real = cleanText().toFloat();
    *ok = true;
    return( mapRealToSpin( real ) );
}

//------------------------------------------------------------------------------
/*! \brief Public access to the current real value.
 */

double RealSpinBox::realValue( void )
{
    return( cleanText().toFloat() );
}

//------------------------------------------------------------------------------
//  End of realspinbox.h
//------------------------------------------------------------------------------

