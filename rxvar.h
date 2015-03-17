//------------------------------------------------------------------------------
/*! \file rxvar.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus RxVar class declaration.
 */

#ifndef _RXVAR_H_
//! Prevent redundant includes.
#define _RXVAR_H_ 1

// Class references
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qdict.h>
#include <qptrlist.h>

//------------------------------------------------------------------------------
/*! \class RxVar rxvar.h
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Contains a prescription range for a specific EqVar;
 *  when a single result iteration falls within all the active prescription
 *  ranges its table cell is highlighted.
 */

class RxVar
{
// Public methods
public:
    // Constructors and destructors
    RxVar( EqVar *varPtr=0 ) ;
    ~RxVar( void ) ;

    // Access methods
    void dump( const QString &prefix ) ;
    bool inRange( void ) const ;
    bool isContinuous( void ) const ;
    bool isDiscrete( void ) const ;
    bool isUserOutput( void ) const ;
    bool isValid( const QString &str ) ;
    bool itemChecked( int id ) const ;
    int  items( void ) const ;

    // Update functions
    bool applyDisplayUnits( const QString &units, int decimals ) ;
    bool applyEnglishUnits( void ) ;
    bool applyMetricUnits( void ) ;
    bool applyNativeUnits( void ) ;
    void setFirstItemBox( int id ) ;
    bool setItemChecked( int id, bool checked ) ;
    void storeMinMax( const QString &strMinimum, const QString &strMaximum ) ;
    void update( bool isActive, double nativeMinimum, double nativeMaximum,
                double displayMinimum, double displayMaximum ) ;

// Private data:
public:
    double   m_nativeMinimum;   //!< Lower prescription limit in native units
    double   m_nativeMaximum;   //!< Upper prescription limit in native units
    double   m_displayMinimum;  //!< Lower prescription limit in display units
    double   m_displayMaximum;  //!< Upper prescription limit in display units
    EqVar   *m_varPtr;          //!< Pointer to the prescription EqVar instance.
    bool     m_isActive;        //!< If TRUE, prescription range is active for this EqVar.
    QString  m_storeMinimum;    //!< Entry store for minimum value
    QString  m_storeMaximum;    //!< Entry store for maximum value
    int      m_itemChecked[8];  //!< Discrete item toggles, 0==unacceptable, 1==acceptable.
    int      m_firstItemBox;    //!< Index to first discrete item check box.
};

//------------------------------------------------------------------------------
/*! \class RxVarList rxvar.h
 *  \brief A linked list of all possible prescription variables.
 *
 *  The add() method inserts a new RxVar (created on the heap) into RxVarList.
 *  The list has autoDelete() set on, so its contents are automatically deleted
 *  from the heap when the list is deleted.
 *
 *  The RxVarList also contains a dictionary of RxVar names with pointers
 *  to their RxVar instances.
 */

class RxVarList : public QPtrList<RxVar>
{
// Public methods
public:
    RxVarList( void ) ;
    void    add( EqVar *varPtr ) ;
    bool    applyEnglishUnits( void ) ;
    bool    applyMetricUnits( void ) ;
    bool    applyNativeUnits( void ) ;
    RxVar  *find( EqVar *varPtr ) ;
    int     items( void ) ;
    int     outputItems( void ) ;
    RxVar  *rxVar( const QString &name ) const ;
    bool    writeXmlFile( FILE *fptr ) ;

// Private methods
private:
    QDict<RxVar> m_dict;    //!< Lookup dictionary
};

#endif

//------------------------------------------------------------------------------
//  End of rxvar.h
//------------------------------------------------------------------------------

