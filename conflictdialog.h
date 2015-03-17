//------------------------------------------------------------------------------
/*! \file conflictdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument input conflict resolution dialog declarationss.
 */

#ifndef _CONFLICTDIALOG_H_
/*! \def _CONFLICTDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _CONFLICTDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class BpDocument;
class QButtonGroup;
class QRadioButton;

//------------------------------------------------------------------------------
/*! \class Conflict1Dialog conflictdialog.h
 *
 *  \brief Allows user to resolve input conflicts between spotting from a
 *  wind driven surface fire and a request for spread rate other than in the
 *  direction of maximum spread.
 */

class Conflict1Dialog : public AppDialog
{
// ENable signals and slots
    Q_OBJECT
// Public methods
public:
    Conflict1Dialog( BpDocument *bp, const char *name=0 ) ;
    ~Conflict1Dialog( void ) ;

protected slots:
    void store( void ) ;

// Private data members
private:
    BpDocument   *m_bp;             //!< Pointer to the parent BpDocument
    QButtonGroup *m_buttonGroup;    //!< Pointer to the conflict resolution options button group
    QRadioButton *m_button0;        //!< Pointer to first conflict resolution option button
    QRadioButton *m_button1;        //!< Pointer to second conflict resolution option button
};

//------------------------------------------------------------------------------
/*! \class Conflict2Dialog conflictdialog.h
 *
 *  \brief Allows user to resolve input conflicts between Surface Module
 *  using Midflame Wind Speed and the Spot Module using 20Ft Wind Speed.
 */

class Conflict2Dialog : public AppDialog
{
// ENable signals and slots
    Q_OBJECT
// Public methods
public:
    Conflict2Dialog( BpDocument *bp, const char *name=0 ) ;
    ~Conflict2Dialog( void ) ;

protected slots:
    void store( void ) ;

// Private data members
private:
    BpDocument   *m_bp;             //!< Pointer to the parent BpDocument
    QButtonGroup *m_buttonGroup;    //!< Pointer to the conflict resolution options button group
    QRadioButton *m_button0;        //!< Pointer to first conflict resolution option button
    QRadioButton *m_button1;        //!< Pointer to second conflict resolution option button
    QRadioButton *m_button2;        //!< Pointer to third conflict resolution option button
    QRadioButton *m_button3;        //!< Pointer to fourth conflict resolution option button
    QRadioButton *m_button4;        //!< Pointer to fifith conflict resolution option button
};

#endif

//------------------------------------------------------------------------------
//  End of conflictdialog.h
//------------------------------------------------------------------------------

