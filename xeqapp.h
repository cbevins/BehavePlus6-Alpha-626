//------------------------------------------------------------------------------
/*! \file xeqapp.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqApp application class definition.
 */

#ifndef _XEQAPP_H_
/*! \def _XEQAPP_H_
    \brief Prevent redundant includes.
 */
#define _XEQAPP_H_ 1

// Custom class references
#include "module.h"
#include "xeqfile.h"
class EqFun;
class EqTree;
class EqVar;
class EqVarItem;
class EqVarItemList;
class FuelModelList;
class MoisScenarioList;

// Qt class references
#include <qdict.h>
#include <qptrlist.h>
#include <qstring.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class EqApp xeqapp.h
 *
 *  \brief Highest level Equation Tree class.  EqApp maintains a collection
 *  of EqTree instances and shared resources including
 *  \arg EqTree definition XML file,
 *  \arg language list,
 *  \arg shared translation dictionary,
 *  \arg shared discrete item lists,
 *  \arg default EqTree variables, functions, and property list,
 *  \arg file system navigation,
 *  \arg units conversion facilities, and
 *  \arg interface facilities.
 */

class EqApp
{
// Public methods
public:
    EqApp( const QString &fileName  );
    ~EqApp( void ) ;

    // Add a continuous variable to the tree
    EqVar *addEqVar(
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
        double         minimum,         //!< Minimum legal value in native units
        double         maximum,         //!< Maximum legal value in native units
        double         defaultValue,    //!< Initial default native value
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    );
    // Add a discrete variable to the tree
    EqVar *addEqVar(
        const QString &name,            //!< Unique variable name
        const QString &help,            //!< Help key
        const QString &inpOrder,        //!< Input order key
        const QString &outOrder,        //!< Output order key
        const QString &wizard,          //!< Optional wizard name
        EqVarItemList *itemList,        //!< EqVarItemList
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    ) ;
    // Add a text variable to the tree
    EqVar *addEqVar(
        const QString &name,            //!< Unique variable name
        const QString &help,            //!< Help key
        const QString &inpOrder,        //!< Input order key
        const QString &outOrder,        //!< Output order key
        const QString &wizard,          //!< Optional wizard name
        int            releaseFrom,     //!< Effective beginning at this release
        int            releaseThru      //!< Effective through this release
    ) ;
    // Add an EqFile to m_eqFileList
    EqFile *addEqFile( const QString &name, EqFileType type, const QString &permission,
                int releaseFrom, int releaseThru ) ;
    // Add a function to m_fun[] and m_funDict
    EqFun  *addEqFun( const QString &name, int inputs, int outputs,
                int releaseFrom, int releaseThru, const QString &module ) ;
    // Add a Module to m_moduleList
    Module *addModule( const QString &name, const QString &textKey,
                const QString sort, int indent, int releaseFrom,
                int releaseThru ) ;
	bool	addModuleOutputVariable( const QString &moduleName,
				const QString &varName );
	QString findProducerModule( const QString &varName ) ;

    bool    attachFuelModel( const QString &fileName ) ;
    bool    attachItem( const QString &listName, const QString &fileName,
                const QString &name, const QString &sort, int index,
                const QString &desc ) ;
    bool    attachMoisScenario( const QString &fileName ) ;
    void    countElements( void ) ;
    bool    deleteFuelModel( const QString &name ) ;
    bool    deleteItem( const QString &listName, const QString &itemName ) ;
    bool    deleteMoisScenario( const QString &name ) ;
    void    init( void ) ;
    EqTree *newEqTree( const QString &treeName, const QString &fileName="",
                const QString &language="" ) ;
    bool    parse( const QString &fileName, bool validate, bool debug=false );
    // The print*() functions are all in xeqtreeprint.cpp
    void    printFunctions( FILE *fptr ) const ;
    void    printVariables( FILE *fptr ) const ;
    void    setLanguage( const QString &language ) ;
    bool    validLanguage( const QString &language ) const ;

// Public data
public:
    QString             m_xmlFile;          //!< EqTree definition XML document name
    QString             m_language;         //!< Current language
    QPtrList<EqTree>   *m_eqTreeList;       //!< List of EqTrees
    QPtrList<EqFile>   *m_eqFileList;       //!< List of EqFiles
    QPtrList<QString>  *m_languageList;     //!< List of available languages
    QPtrList<Module>   *m_moduleList;       //!< List of Modules
    FuelModelList      *m_fuelModelList;    //!< SHARED fuel model list
    MoisScenarioList   *m_moisScenarioList; //!< SHARED moisture scenario list
    int                 m_release;          //!< Current release number
    int                 m_functionCount;    //!< Number of <function>s scanned
    int                 m_functionPrime;    //!< Size of m_funDict
    int                 m_itemListCount;    //!< Number of <itemList>s scanned
    int                 m_itemListPrime;    //!< Size of m_itemListDict
    int                 m_languageCount;    //!< Number of <language>s scanned
    int                 m_propertyCount;    //!< Number of <property>s scanned
    int                 m_propertyPrime;    //!< Size of m_propDict
    int                 m_translateCount;   //!< Number of <translate>s scanned
    int                 m_translatePrime;   //!< Size of m_trDict
    int                 m_variableCount;    //!< Number of <variable>s scanned
    int                 m_variablePrime;    //!< Size of m_varDict
    // These are read from XML document and used to create EqTrees.
    EqFun             **m_fun;              //!< Array of ptrs to default EqFuns
    int                 m_funCount;         //!< Counter used by parser
    EqVar             **m_var;              //!< Array of ptrs to default EqVars
    int                 m_varCount;         //!< Counter used by parser
    EqVarItemList     **m_itemList;         //!< Array of ptrs to all EqVarItemLists
    int                 m_itemCount;        //!< Counter used bu parser
    QDict<EqFun>       *m_funDict;          //!< Name lookup access to EqFun ptrs
    QDict<EqVar>       *m_varDict;          //!< Name lookup access to EqVar ptrs
    QDict<EqVarItemList> *m_itemListDict;//!< SHARED EqVarItemList ptrs by name
};

#endif

//------------------------------------------------------------------------------
//  End of xeqapp.h
//------------------------------------------------------------------------------

