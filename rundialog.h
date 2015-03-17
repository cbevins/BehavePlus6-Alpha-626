//------------------------------------------------------------------------------
/*! \file rundialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument run time dialog class declarations.
 */

#ifndef _RUNDIALOG_H_
/*! \def _RUNDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _RUNDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class BpDocument;
class EqTree;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QSpinBox;
class QVButtonGroup;
class TextView;

//------------------------------------------------------------------------------
/*! \class RunDialog rundialog.h
 *
 *  BehavePlus run/calculate dialog.
 */

class RunDialog : public AppDialog
{
// ENable signals and slots
    Q_OBJECT
// Public methods
public:
    RunDialog( BpDocument *bp, const char *name=0 ) ;
    ~RunDialog( void ) ;
    void store( void ) ;

// Private data members
private:
    BpDocument    *m_bp;                //!< Pointer to parent BpDocument
    TextView      *m_textView;          //!< Pointer to info pane
    QVButtonGroup *m_graphButtonGroup;  //!< Pointer to graph output options button group
    QCheckBox     *m_graphsCheckBox;    //!< Pointer to graph output on/off checkbox
    QRadioButton  *m_graphsVar1;        //!< Pointer to graph range var 0 radio button
    QRadioButton  *m_graphsVar2;        //!< Pointer to graph range var 1 radio button
    QVButtonGroup *m_tableButtonGroup;  //!< Pointer to table output options button group
    QCheckBox     *m_tablesCheckBox;    //!< Pointer tp table output on/off checkbox
    QRadioButton  *m_tablesVar1;        //!< Pointer to table range var 0 radio button
    QRadioButton  *m_tablesVar2;        //!< Pointer to table range var 1 radio button
    QCheckBox     *m_graphLimitsCheckBox;  //!< Pointer to graph limits on/off checkbox
};

#endif

//------------------------------------------------------------------------------
//  End of rundialog.h
//------------------------------------------------------------------------------

