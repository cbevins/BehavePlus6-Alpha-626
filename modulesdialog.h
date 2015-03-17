//------------------------------------------------------------------------------
/*! \file modulesdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus Module Selection dialog declaration.
 */

#ifndef _MODULESDIALOG_H_
/*! \def _MODULESDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _MODULESDIALOG_H_ 1

// Class references
#include "appdialog.h"
#include "module.h"
class BpDocument;
class PropertyPage;
class VarCheckBox;
class QButtonGroup;
class QCheckBox;
class QFrame;
class QGridLayout;
class QPushButton;

// Qt references
#include <qptrlist.h>

//------------------------------------------------------------------------------
/*! \class ModulesDialog modulesdialog.h
 *
 *  \brief BehavePlus Module Selection dialog class.
 */

class ModulesDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT
// Methods
public:
    ModulesDialog( BpDocument *bp,
        const QString &captionKey="ModulesDialog:Caption",
        const char *name="modulesDialog" ) ;
    ~ModulesDialog( void ) ;

// Private methods to construct module-specific OptionsDialogs.
private:
    VarCheckBox *addOutput( PropertyPage *page,
                const QString &propName, const QString &varName,
                int rowBeg, int colBeg, int rowEnd, int colEnd,
                bool wrapped=false ) ;
    void containOptions( void ) ;
    void crownOptions( void ) ;
    void ignitionOptions( void ) ;
    void mortalityOptions( void ) ;
    void scorchOptions( void ) ;
    void safetyOptions( void ) ;
    void sizeOptions( void ) ;
    void spotOptions( void ) ;
    void surfaceOptions( void ) ;
    void weatherOptions( void ) ;

public slots:
    void slotOptions( int id );
    virtual void store( void );

// Data members
private:
    BpDocument    *m_bp;            //!< Pointer to the parent BpDocument
    QPtrList<Module> *m_moduleList; //!< Pointer to EqApp's module list
    QFrame        *m_gridFrame;     //!< Pointer to main grid frame
    QGridLayout   *m_gridLayout;    //!< Pointer to dialog's grid layout
    QButtonGroup  *m_mapFrame;      //!< Pointer to map distance button frame
    QCheckBox     *m_mapCheckBox;   //!< Pointer to map distance check box
    QCheckBox     *m_tableShadingCheckBox;   //!< Pointer to table shading check box
    QButtonGroup  *m_guideBtnGrp;   //!< Pointer to option button group
    QCheckBox    **m_cb;            //!< Array of pointers to module check boxes
    QPushButton  **m_pb;            //!< Array of pointers to module option buttons
    int            m_modules;       //!< Number of modules
};

#endif

//------------------------------------------------------------------------------
//  End of modulesdialog.h
//------------------------------------------------------------------------------

