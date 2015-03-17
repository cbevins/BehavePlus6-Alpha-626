//------------------------------------------------------------------------------
/*! \file xeqvar.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree EqVar and EqFun class definitions.
 */

#ifndef _XEQVAR_H_
/*! \def _XEQVAR_H_
 *  \brief Prevent redundant includes.
 */
#define _XEQVAR_H_ 1

////////////////////////////// VERY IMPORTANT !!! //////////////////////////////
// We MUST "include xeqcalc.h" here rather than reference "class EqCalc;"
// otherwise Microsoft Visual C++ corrupts the stack when calling the
// EqFun constructor from the EqTree constructor.  It apparently cannot
// correctly determine the size of "typedef void (EqCalc::*PFV)(void)".

#include "xeqcalc.h"

// Custom class references
class EqFun;
class EqTree;
class EqVar;
class EqVarItem;
class EqVarItemList;

// Qt class references
#include <qstring.h>

// Standard include files
#include <stdio.h>

/*! \typedef (EqCalc::*PFV)(void)
 *  \brief Typedef for all EqCalc computation functions,
 *  which must take no arguments and return nothing.
 */

typedef void (EqCalc::*PFV)(void);    // Pointer to a function returning void.

//------------------------------------------------------------------------------
/*! \var VarType
 *  \brief Enumerations for the various types of variables supported or planned.
 */

enum VarType
{
    VarType_Continuous=0,
    VarType_Discrete=1,
    VarType_Text=2
};

//------------------------------------------------------------------------------
/*! \class EqVar xeqvar.h
 *
 *  \brief Equation tree variable class definition.
 */

class EqVar
{
// Public methods
public:
    // Continuous variable constructor
    EqVar(
        const QString &name,            //!< Unique variable name
        const QString &help,            //!< Help key
        const QString &inpOrder,        //!< Input order key
        const QString &outOrder,        //!< Output order key
        const QString &wizard,          //!< Optional wizard name
        const QString &nativeUnits,     //!< Native modeling units
        int            nativeDecimals,  //!< Displayed native decimal places
        const QString &englishUnits,    //!< English display units
        int            englishDecimals, //!< Displayed english decimal places
        const QString &metricUnits,     //!< Metric display units
        int            metricDecimals,  //!< Displayed metric decimal places
        double         nativeMinimum,   //!< Minimum legal value in native units
        double         nativeMaximum,   //!< Maximum legal value in native units
        double         defaultValue,    //!< Initial default native value
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    );
    // Discrete variable constructor
    EqVar(
        const QString &name,            //!< Unique variable name
        const QString &help,            //!< Help key
        const QString &inpOrder,        //!< Input order key
        const QString &outOrder,        //!< Output order key
        const QString &wizard,          //!< Optional wizard name
        EqVarItemList *itemList,        //!< EqVarItemList pointer
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    ) ;
    // Text variable constructor
    EqVar(
        const QString &name,            //!< Unique variable name
        const QString &help,            //!< Help key
        const QString &inpOrder,        //!< Input order key
        const QString &outOrder,        //!< Output order key
        const QString &wizard,          //!< Optional wizard name
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    ) ;
    // Destructor
    ~EqVar( void ) ;
    // Allocate internal arrays
    void init( void ) ;

    const QString activeItemName( void ) const ;
    int      activeItemDataIndex( void ) const ;
    QString *activeItemDesc( void ) const ;
    EqFun   *activeProducerFunPtr( void ) const ;
    bool     applyEnglishUnits( void ) ;
    bool     applyMetricUnits( void ) ;
    bool     applyNativeUnits( void ) ;
    bool     convertStoreUnits( bool toNative ) ;
    QString  displayUnits( bool enclosed=false ) const ;
    const QString &getItemName( int id ) const ;
    bool     isContinuous( void ) const ;
    bool     isCurrent( int release ) const ;
    bool     isDiagram( void ) const ;
    bool     isDiscrete( void ) const ;
    bool     isText( void ) const ;
    bool     isValidItemName( const QString &itemName, bool caseSens ) const ;
    bool     isValidItemSort( const QString &itemSort, bool caseSens ) const ;
    bool     isValidRange( double value ) const ;
    bool     isValidStore( int *tokens, int *position, int *length ) ;
    bool     isValidString( const QString &str, int *tokens,
                 int *position, int *length ) ;
    double   nativeStore( double value ) ;
    double   nativeValue( double value ) ;
    // The print*() functions are all in xeqtreeprint.cpp
    void     print( FILE *fptr ) const ;
    void     propagateDirty( int level=0 ) ;
    bool     setDisplayUnits( const QString &units, int decimals ) ;
    double   setDisplayValue( double value ) ;
    QString &setHelp( const QString &help ) ;
    void     setItemName( const QString &itemName, bool doCheck=true ) ;
    void     setItemNameToDefault( void ) ;
    double   setNativeValue( double value ) ;
    QString &setStore( const QString &value ) ;
    void     update( double value ) ;
    void     updateItem( const QString &itemName ) ;
    void     updateItem( int itemDataIndex ) ;

// Public data members
public:
    QString  m_name;            //!< Variable's internal name (dictionary key)
    QString  m_help;            //!< Help key
    QString  m_inpOrder;        //!< Input sort order "group:sort"
    QString  m_outOrder;        //!< Output sort order "group:sort"
    QString  m_wizard;          //!< Name of an optional input wizard
    QString  m_master;          //!< Name of any master paired variable
    QString *m_label;           //!< Assigned by EqTree::setLanguage()
    QString *m_desc;            //!< Assigned by EqTree::setLanguage()
    QString *m_hdr0;            //!< Assigned by EqTree::setLanguage()
    QString *m_hdr1;            //!< Assigned by EqTree::setLanguage()
    VarType  m_varType;         //!< VarType_Continuous, _Discrete, _Text,
    EqTree  *m_eqTree;          //!< Ptr to parent EqTree
    EqFun  **m_consumer;        //!< Array of pointers to consumer EqFuns
    int      m_consumers;       //!< Size of m_consumer array
    EqFun  **m_producer;        //!< Array of pointers to producer EqFuns
    int      m_producers;       //!< Size of m_producer array
    int      m_tokens;          //!< Number of tokens in the store()
    QString  m_store;           //!< Input worksheet entry text backing store
    bool     m_isUserOutput;    //!< True if var is a requested output
    bool     m_isUserInput;     //!< True if this is a leaf (user input) variable
    bool     m_isConstant;      //!< True if var is a leaf constant (NOT user input)
    bool     m_isDiagram;       //!< True if var is a diagram toggle
    bool     m_isMasked;        //!< True if this is a leaf that is masked
    bool     m_isWrap;          //!< True if wrap-around values allowed (e.g., compass 0-360)
    bool     m_isShaded;        //!< True if outputs are to use shading
    int      m_boundaries;      //!< Size of m_boundary array
    double  *m_boundary;        //!< Array of boundary values (for result highlighting)
    int      m_releaseFrom;     //!< Effective beginning at this release
    int      m_releaseThru;     //!< Effective through this release

    // Used only by VarType_Continuous
    double   m_defaultValue;    //!< Default value in native units
    double   m_nativeMinimum;   //!< Minimum valid input value in native units
    double   m_nativeMaximum;   //!< Maximum valid input value in native units
    double   m_nativeValue;     //!< Current value in native units
    double   m_displayMinimum;  //!< Minimum valid input value in native units
    double   m_displayMaximum;  //!< Maximum valid input value in native units
    double   m_displayValue;    //!< Current value in display units
    int      m_nativeDecimals;  //!< Display decimals for native units
    int      m_englishDecimals; //!< Display decimals for english units
    int      m_metricDecimals;  //!< Display decimals for metric units
    int      m_displayDecimals; //!< Display decimals in current display units
    QString  m_nativeUnits;     //!< Variable's native (computational) units
    QString  m_englishUnits;    //!< Variable's English display set units
    QString  m_metricUnits;     //!< Variable's metric display set units
    QString  m_displayUnits;    //!< Current display units
    double   m_factor;          //!< Ratio conversion factor
    double   m_offset;          //!< Ratio conversion offset
    int      m_convert;         //!< 0=no conversion, 1=ratio, 2=other
    // Used only by VarType_Discrete
    EqVarItemList *m_itemList;  //!< Pointer to variable's EqVarItemList
    QString  m_activeItemName;  //!< Name of current active item
};

//------------------------------------------------------------------------------
/*! \class EqFun xeqvar.h
 *
 *  \brief EqFun contains state information about each function in the EqTree.
 */

class EqFun
{
// Public methods
public:
    EqFun( const QString &name, PFV func, int inputs, int outputs,
        int releaseFrom, int releaseThru, const QString &module ) ;
    ~EqFun( void ) ;
    bool isCurrent( int release ) const ;
    // The print*() functions are all in xeqtreeprint.cpp
    void print( FILE *fptr ) const ;
    bool setDirty( EqVar *varPtr ) ;
    void setDirtyAll( void ) ;

// Public data
public:
    QString  m_name;            //!< Function name
    EqVar  **m_input;           //!< Array of input EqVar pointers
    int      m_inputs;          //!< Size of the m_input[] array
    EqVar  **m_output;          //!< Array of output EqVar pointers
    int      m_outputs;         //!< Size of m_output[] array
    bool    *m_dirty;           //!< Array of dirty flags for each input variable
    bool     m_active;          //!< If TRUE, function is active
    PFV      m_function;        //!< Ptr to the calculation function
    int      m_releaseFrom;     //!< Effective beginning at this release
    int      m_releaseThru;     //!< Effective through this release
    QString  m_module;          //!< Name of the function's module
};

// Convenience functions

bool isValidDouble( const char *str, double *returnValue ) ;

#endif

//------------------------------------------------------------------------------
//  End of xeqvar.h
//------------------------------------------------------------------------------

