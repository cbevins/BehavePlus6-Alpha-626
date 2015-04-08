//------------------------------------------------------------------------------
/*! \file xeqtree.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree class definitions.
 */

#ifndef _XEQTREE_H_
/*! \def _XEQTREE_H_
 *  \brief Prevent redundant includes.
 */
#define _XEQTREE_H_ 1

// Custom class references
#include "xeqvar.h"
class EqApp;
class EqCalc;
class EqFun;
class EqVarItem;
class EqVarItemList;
class FuelModelList;
class MoisScenarioList;
class PropertyDict;
class RxVarList;

// Qt class references
#include <qdict.h>
#include <qstring.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class EqTree xeqtree.h
 *
 *  \brief The EqTree class holds all the EqVars and dictionaries.
 */

class EqTree
{
// Public methods
public:
    EqTree( EqApp *eqApp, const QString &name, int funCount, int funPrime,
        int varCount, int varPrime, int propPrime,
        EqVarItemList **itemList, int itemListCount,
        QDict<EqVarItemList> *itemListDict,
        FuelModelList *fuelModelList, MoisScenarioList *moisScenarioList ) ;
    ~EqTree( void );

    bool   applyEnglishUnits( void ) ;
    bool   applyMetricUnits( void ) ;
    bool   applyNativeUnits( void ) ;
    bool   applyUnitsSet( const QString &fileName ) ;
    void   activateFunctions( bool toggle ) ;
    void   calculateVariable( EqVar *varPtr, int level ) ;
    void   calculateVariableDebug( EqVar *varPtr, int level ) ;
    void   clearUserInput( void );
    void   clearUserOutput( void );
    int    generateLeafList( int release ) ;
    void   generateLeafListNext( EqVar *varPtr, int release ) ;
    int    generateRootList( int release ) ;
    double getResult( int row, int col, int var ) const ;
    EqVar *getVarPtr( const QString &name ) const ;
    void   init( void ) ;
    void   resultFileClose( void ) ;
    bool   resultFileInit( const QString &fileName ) ;
    void   resultFileRemove( void ) ;
    void   traceFileClose( void ) ;
    bool   traceFileInit( const QString &fileName ) ;
    void   traceFileRemove( void ) ;
    // The print*() functions are all in xeqtreeprint.cpp
    void   printDotFile(  const QString &fileName, int release ) const ;
    void   printEqVarItemLists( FILE *fptr ) const ;
    void   printFunDict( FILE *fptr ) const ;
    void   printFunctions( FILE *fptr ) const ;
    void   printInputs( FILE *fptr ) const ;
    void   printOutputs( FILE *fptr ) const ;
    void   printPatsTable( FILE *fptr, bool all ) const ;
    void   printPropertyDict( FILE *fptr ) const ;
    void   printResults( FILE *fptr ) const ;
    void   printResults0( FILE *fptr ) const ;
    void   printResults1( FILE *fptr ) const ;
    void   printResults2( FILE *fptr ) const ;
    void   printSummary( FILE *fptr ) const ;
    void   printVarCsv( FILE *fptr ) const ;
    void   printVarDict( FILE *fptr ) const ;
    void   printVariables( FILE *fptr ) const ;
    bool   readXmlFile( const QString &fileName, bool unitsOnly=false ) ;
    void   reconfigure( int release ) ;
    int    rangeCase( void );
    bool   run( const QString &traceFile, const QString &resultFile ) ;
    void   runClean( void ) ;
    bool   runInit( bool graphTable ) ;
    void   runInitColsFromStore( void ) ;
    void   runInitRowsFromRange( void ) ;
    void   runInitRowsFromStore( void ) ;
    bool   runInitTableVars( void ) ;
    bool   runTable( const QString &traceFile="", const QString &resultFile="",
                bool graphTable=false ) ;
    EqFun *setEqFunAddress( const QString &name, PFV address ) ;
    void   setLabel( EqVar *varPtr, const QString &stuff ) ;
    void   setLanguage( const QString &lang ) ;
    double setResult( int row, int col, int var, double value ) ;
	QString tokenConverter( EqVar* varPtr, const QString &token ) ;
    int    validateInputs( int *badLid, int *badPosition, int *badLength ) ;
	int    validateRxInputs( int *badRx ) ;
    void   variableModuleList( EqVar *varPtr, QString &str ) const ;
    bool   writeXmlFile( const QString &fileName, const QString &elementName,
                const QString &elementType, int release, bool writeValues=true ) ;
    bool   writeXmlFile( FILE *fptr, int release, bool writeValues=true ) ;

// Public data members
public:
    EqApp          *m_eqApp;        //!< Ptr to the parent EqApp
    QString         m_name;         //!< Tree name
    QString         m_lang;         //!< Current language
    QString         m_type;         //!< Type read from file
    int             m_release;      //!< Release read from file
    EqFun         **m_fun;          //!< Array of ptrs to all EqFuns
    int             m_funCount;     //!< Number of EqFuns in m_fun[] array
    EqVar         **m_var;          //!< Array of ptrs to all EqVars
    int             m_varCount;     //!< Number of EqVars in m_var[] array
    EqVar         **m_leaf;         //!< Array of ptrs to current input EqVars
    int             m_leafCount;    //!< Number of inputs in the leaf[] array
    EqVar         **m_root;         //!< Array of ptrs to current output EqVars
    int             m_rootCount;    //!< Number of outputs in the root[] array
    EqVarItemList **m_itemList;     //!< SHARED ptr to array of EqVarItemList ptrs
    int             m_itemListCount;//!< SHARED number of entries in m_itemList[] array
    QDict<EqFun>   *m_funDict;      //!< Name lookup access to local EqFun ptrs
    QDict<EqVar>   *m_varDict;      //!< Name lookup access to local EqVar ptrs
    PropertyDict   *m_propDict;     //!< Name lookup access to local Property ptrs
    QDict<EqVarItemList> *m_itemListDict;   //!< SHARED name lookup access to EqVarItemList ptrs
    FuelModelList  *m_fuelModelList;//!< SHARED fuel model list
    MoisScenarioList *m_moisScenarioList;   //!< SHARED fuel model list
    // Run time data members
    RxVarList      *m_rxVarList;    //!< Prescription variable list and ranges
    bool            m_debug;        //!< Debug mode
    EqCalc         *m_eqCalc;       //!< Run time data
    int             m_maxRangeVars; //!< Size of the m_rangeVar[] array
    EqVar         **m_rangeVar;     //!< Arrays of ptrs to range vars
    int             m_rangeVars;    //!< Number of range variables
    int             m_rangeCase;    //!< Continuous/discrete range variable case
    int             m_tableRows;    //!< Results table rows
    int             m_tableCols;    //!< Results table columns
    int             m_tableVars;    //!< Results table variables
    int             m_tableCells;   //!< Results table cells
    double         *m_tableCol;     //!< Dynamic array of table column values
    double         *m_tableRow;     //!< Dynamic array of table row values
	double         *m_tableVal;     //!< Dynamic array of table results
    bool           *m_tableInRx;   	//!< Dynamic array of table shade toggles
    EqVar         **m_tableVar;     //!< Dynamic array of table EqVar ptrs
    QString         m_resultFile;   //!< Run time result file name
    QString         m_traceFile;    //!< Run time trace file name
    FILE           *m_resultFptr;   //!< Run time result file stream ptr
    FILE           *m_traceFptr;    //!< Run time trace file stream ptr
};

// Convenience routines

void printTranslationDict( FILE *fptr ) ;
int  strMinMax( const QString &qStr, double *minval, double *maxval ) ;

// Comparison functions for sorting variable ptr arrays
int EqTree_InpOrderCompare( const void *s1, const void *s2 ) ;
int EqTree_OutOrderCompare( const void *s1, const void *s2 ) ;

#endif

//------------------------------------------------------------------------------
//  End of xeqtree.h
//------------------------------------------------------------------------------

