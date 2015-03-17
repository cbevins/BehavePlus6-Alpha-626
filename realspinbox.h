//------------------------------------------------------------------------------
/*! \file realspinbox.h
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins
 *
 *  \brief RealSpinBox class declaration.
 */

#ifndef _REALSPINBOX_H_
#define _REALSPINBOX_H_

// Class declarations
#include <qspinbox.h>
class QDoubleValidator;

//------------------------------------------------------------------------------
/*! \class RealSpinBox realspinbox.h
 *  \brief A real-number (floating point) spin box.
 */

class RealSpinBox : public QSpinBox
{
// Enable signals and slots
Q_OBJECT

// Public methods
public:
    RealSpinBox( double min, double max, double step, double start,
        int decimals, QWidget *parent, const char *name=0 );
    ~RealSpinBox( void ) ;
    virtual double  realValue( void ) ;

// Protected members
protected:
    virtual QString mapValueToText( int value ) ;
    virtual int     mapTextToValue( bool *ok ) ;
	int mapRealToSpin( double real_value ) ;
	int mapRealToStep( double real_value ) ;
	double mapSpinToReal( int spin_value ) ;

// Public data
public:
	double m_spinMin;
	double m_spinMax;
	double m_spinRange;
    double m_minReal;
    double m_maxReal;
    double m_stepReal;
    double m_startReal;
    double m_factor;
    double m_round;
    int    m_decimals;
    QDoubleValidator *m_validator;
};


#endif // _REALSPINBOX_H_

//------------------------------------------------------------------------------
//  End of realspinbox.h
//------------------------------------------------------------------------------

