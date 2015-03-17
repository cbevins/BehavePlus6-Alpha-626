//------------------------------------------------------------------------------
/*! \file graphlimitsdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument run time graph limits dialog class declarations.
 */

#ifndef _GRAPHLIMITSDIALOG_H_
/*! \def _GRAPHLIMITSDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _GRAPHLIMITSDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
#include "graphaxle.h"
#include <qptrlist.h>
class BpDocument;
class QFrame;
class QGridLayout;
class QLabel;
class RealSpinBox;

//------------------------------------------------------------------------------
/*! \class GraphLimitsDialog graphlimitsdialog.h
 *
 *  \brief Dialog to allow user specification of output graph Y axis limits.
 */

static const int MaxGraphs = 100;

class GraphLimitsDialog : public AppDialog
{
// ENable signals and slots
    Q_OBJECT
// Public methods
public:
    GraphLimitsDialog( BpDocument *bp, QPtrList<GraphAxleParms> *yParmsList,
        const char *name=0 ) ;
    ~GraphLimitsDialog( void ) ;
    void store( void ) ;

// Private data members
private:
    BpDocument  *m_bp;                  //!< Pointer to parent BpDocument
    QPtrList<GraphAxleParms> *m_yParmsList;//!< Pointer to list of Y axle parameters
    QFrame      *m_gridFrame;           //!< Pointer to grid frame
    QGridLayout *m_gridLayout;          //!< Pointer to the layout widget
    QLabel      *m_label[MaxGraphs];    //!< Pointers to output variable name labels
    QLabel      *m_range[MaxGraphs];    //!< Pointers to output variable predicted range and units labels
    RealSpinBox *m_min[MaxGraphs];      //!< Pointers to axis minimum spin boxes
    RealSpinBox *m_max[MaxGraphs];      //!< Pointers to axis maximum spin boxes
    int          m_vars;
};

#endif

//------------------------------------------------------------------------------
//  End of graphlimitsdialog.h
//------------------------------------------------------------------------------

