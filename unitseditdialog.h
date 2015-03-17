//------------------------------------------------------------------------------
/*! \file unitseditdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Units editor dialog class declarations.
 */

#ifndef _UNITSEDITDIALOG_H_
/*! \def _UNITSEDITDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _UNITSEDITDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
#include <qptrlist.h>
class BpDocument;
class EqVar;
class QComboBox;
class QSpinBox;
class QGridLayout;
class UnitsEdit;
class UnitsEditPage;
class UnitsEditDialog;

//------------------------------------------------------------------------------
/*! \class UnitsEdit unitseditdialog.h
 *
 *  \brief Defines a single editor for a group of variables sharing the same
 *  units of measure.  The variable names are used to look up their EqVar
 *  addresses.  The first variable in the list is used to set the initial
 *  units and decimals value.
 */

class UnitsEdit
{
// Public methods
public:
    UnitsEdit( const char **varList, EqVar *var, QComboBox *cb, QSpinBox *sb ) ;

// Public data
public:
    const char   **m_varList;       //!< Pointer to array of EqVar names
    EqVar         *m_var;           //!< Pointer to EqVar whose name is in m_varList[0]
    QComboBox     *m_combo;         //!< Pointer to units editor combo box
    QSpinBox      *m_spin;          //!< Pointer to decimal places editor spin box
};

//------------------------------------------------------------------------------
/*! \class UnitsEditDialog unitseditdialog.h
 *
 *  \brief A tabbed dialog for editing units of measure and decimal places for
 *  groups of variables.
 */

class UnitsEditDialog : public AppTabDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    UnitsEditDialog( BpDocument *bp,
        const QString &captionKey="UnitsEditDialog:Caption",
        const char *name="unitsEditDialog",
        const QString &acceptKey="UnitsEditDialog:Button:Accept",
        const QString &rejectKey="UnitsEditDialog:Button:Reject" ) ;
    ~UnitsEditDialog( void ) ;

    virtual UnitsEditPage *addPage( const QString &tabKey, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile ) ;

// Protect slots
protected slots:
    virtual void store( void );

// Private data members
public:
    BpDocument           *m_bp;         //!< Pointer to the parent BpDocument
    QPtrList<UnitsEditPage> *m_pageList;//!< List of pointers to child UnitsEditPages
};

//------------------------------------------------------------------------------
/*! \class UnitsEditPage unitseditdialog.h
 *
 *  \brief UnitsEditPage defines a single tab page in the UnitsEditDialog.
 */

class UnitsEditPage : public AppPage
{
// Public methods
public:
    UnitsEditPage( UnitsEditDialog *dialog, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name=0 ) ;
    ~UnitsEditPage( void ) ;
    UnitsEdit *addEdit( const QString &labelKey, const char **varList,
        const char **unitsList, int row, bool showDecimals=true ) ;
    void addHeaders( void ) ;

// Public data members
public:
    UnitsEditDialog  *m_dialog;     //!< Pointer to parent PropertyDialog
    QFrame           *m_frame;      //!< Frame to hold the QGridLayout.
    QGridLayout      *m_grid;       //!< Pointer to the grid layout widget
    QPtrList<UnitsEdit> *m_editList;//!< List of pointers to page's UnitEdits
};

#endif

//------------------------------------------------------------------------------
//  End of unitseditdialog.h
//------------------------------------------------------------------------------

