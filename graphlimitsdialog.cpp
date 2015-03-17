//------------------------------------------------------------------------------
/*! \file graphlimitsdialog.cpp
 *  \version BehavePlus5
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument run time graph limits dialog class declarations.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "bpdocument.h"
#include "graph.h"
#include "graphlimitsdialog.h"
#include "property.h"
#include "realspinbox.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <cmath>

//------------------------------------------------------------------------------
/*! \brief GraphLimitsDialog constructor.
 */

GraphLimitsDialog::GraphLimitsDialog( BpDocument *bp,
        QPtrList<GraphAxleParms> *yParmsList, const char *name ) :
    AppDialog( bp, "GraphLimitsDialog:Caption",
        "BlackfootRiver.png",
        "Blackfoot River",
        "graphLimitsDialog.html",
        name ),
    m_bp(bp),
    m_yParmsList( yParmsList ),
    m_gridFrame(0),
    m_gridLayout(0),
    m_vars( 0 )
{
    // Initialize all widget pointers
    int i;
    for ( i=0; i<MaxGraphs; i++ )
    {
        m_label[i] = m_range[i] = 0;
        m_min[i] = m_max[i] = 0;
    }

    // Hidden frame to contain a grid layout
    m_gridFrame = new QFrame( contentFrame(), "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::Raised );

    // Create the label-range-entry grid layout
    m_gridLayout = new QGridLayout( m_gridFrame, 4, 4, 0, 2,
        "m_gridLayout" ) ;
    Q_CHECK_PTR( m_gridLayout );

    // Column headers
    QString text("");
    QLabel *lbl;
    for ( i=0; i<4; i++ )
    {
        translate( text, QString( "GraphLimitsDialog:Header:%1" ).arg( i ) );
        lbl = new QLabel( text, m_gridFrame );
        Q_CHECK_PTR( lbl );
        m_gridLayout->addWidget( lbl, 0, i, AlignCenter );
    }

    // Add the name, range, and min and max spin boxes for each input
    EqVar *yVar;
    GraphAxleParms *yParms = m_yParmsList->first();
    int row = 1;
	double min_val = 0.;
	double max_val = 100000000.;
	double step_val = 1.;
    for ( int yid = 0;
          yid < m_bp->tableVars() && m_vars < MaxGraphs;
          yid++ )
    {
        // Get the variable pointer and make sure it is continuous
        yVar = m_bp->tableVar( yid );
        if ( yVar->isContinuous() )
        {
            // Variable label
            m_label[m_vars] = new QLabel( *(yVar->m_label), m_gridFrame );
            Q_CHECK_PTR( m_label[m_vars] );
            m_gridLayout->addWidget( m_label[m_vars], row, 0, AlignLeft );

            // Variable range and units
            text = QString( "%1 - %2 %3" )
                .arg( yParms->m_dataMin, 0, 'f', yVar->m_displayDecimals )
                .arg( yParms->m_dataMax, 0, 'f', yVar->m_displayDecimals )
                .arg( yVar->m_displayUnits );
            m_range[m_vars] = new QLabel( text, m_gridFrame );
            Q_CHECK_PTR( m_range[m_vars] );
            m_gridLayout->addWidget( m_range[m_vars], row, 1, AlignLeft );

            // Minimum Y axis value
            m_min[m_vars] = new RealSpinBox( min_val, max_val, step_val,
                yParms->m_axleMin, yVar->m_displayDecimals, m_gridFrame );
            Q_CHECK_PTR( m_min[m_vars] );
            m_gridLayout->addWidget( m_min[m_vars], row, 2 );

            // Maximum Y axis value
            m_max[m_vars] = new RealSpinBox( min_val, max_val, step_val,
                yParms->m_axleMax, yVar->m_displayDecimals, m_gridFrame );
            Q_CHECK_PTR( m_max[m_vars] );
            m_gridLayout->addWidget( m_max[m_vars], row, 3 );

            // Increment variable counter and return
            m_vars++;
            row++;
            yParms = m_yParmsList->next();
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief GraphLimitsDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

GraphLimitsDialog::~GraphLimitsDialog( void )
{
    for ( int i=0; i<m_vars; i++ )
    {
        delete m_label[i];  m_label[i] = 0;
        delete m_range[i];  m_range[i] = 0;
        delete m_min[i];    m_min[i] = 0;
        delete m_max[i];    m_max[i] = 0;
    }
    delete m_gridLayout;        m_gridLayout = 0;
    delete m_gridFrame;         m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stores the dialog settings into the document state variables.
 *  Usually called by the same function that created the GraphLimitsDialog,
 *  if it returns Accepted, and before it is destroyed.
 */

void GraphLimitsDialog::store( void )
{
    GraphAxleParms *yParms = m_yParmsList->first();
    EqVar *yVar;
    int var = 0;
    for ( int yid = 0;
          yid < m_bp->tableVars() && var < m_vars;
          yid++ )
    {
        // Get the variable pointer and make sure it is continuous
        yVar = m_bp->tableVar( yid );
        if ( yVar->isContinuous() )
        {
			// If the user has asked for different limits
			double old_min = m_min[var]->m_startReal;
			double old_max = m_max[var]->m_startReal;
			double new_min = m_min[var]->realValue();
			double new_max = m_max[var]->realValue();
			bool fixed_range = fabs( old_min - new_min ) > 0.00001
				            || fabs( old_max - new_max ) > 0.00001;

			yParms->m_axleMin = m_min[var]->realValue();
			yParms->m_axleMax = m_max[var]->realValue();
			niceAxis( yParms->m_axleMin, yParms->m_axleMax, yParms->m_tics,
				&yParms->m_axleMin, &yParms->m_axleMax,
				&yParms->m_majorTics, &yParms->m_majorStep, &yParms->m_decimals,
				fixed_range );
            var++;
            yParms = m_yParmsList->next();
        }
    }
    accept();
    return;
}

//------------------------------------------------------------------------------
//  End of graphlimitsdialog.cpp
//------------------------------------------------------------------------------

