//------------------------------------------------------------------------------
/*! \file xeqapp.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2016 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief EqTree application class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appproperty.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "fuelmodel.h"
#include "module.h"
#include "moisscenario.h"
#include "xeqapp.h"
#include "xeqappparser.h"
#include "xeqfile.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qfileinfo.h>

// Local static functions
static int getPrimeGTE( int n ) ;

//------------------------------------------------------------------------------
/*! \brief EqApp constructor.
 *
 *  Reads an EqApp definition XML file and stores its specifications so that
 *  new EqTree instances can be quickly created and initialized.
 *
 *  Also stores shared information (item lists, translation dictionaries, etc)
 *  and maintains a list of all EqTree instances.
 */

EqApp::EqApp( const QString &fileName ) :
    m_xmlFile(fileName),
    m_language("en_US"),
    m_eqTreeList(0),
    m_eqFileList(0),
    m_languageList(0),
    m_moduleList(0),
    m_fuelModelList(0),
    m_moisScenarioList(0),
    m_release(0),
    m_functionCount(0),
    m_functionPrime(0),
    m_itemListCount(0),
    m_itemListPrime(0),
    m_languageCount(0),
    m_propertyCount(0),
    m_propertyPrime(0),
    m_translateCount(0),
    m_translatePrime(0),
    m_variableCount(0),
    m_variablePrime(0),
    m_fun(0),
    m_funCount(0),
    m_var(0),
    m_varCount(0),
    m_itemList(0),
    m_itemCount(0),
    m_funDict(0),
    m_varDict(0),
    m_itemListDict(0)
{
    log( "\nBeg Section: EqApp constructor\n" );
    // Create the EqTree list
    m_eqTreeList = new QPtrList<EqTree>();
    Q_CHECK_PTR( m_eqTreeList );
    m_eqTreeList->setAutoDelete( true );

    // Create the EqFile list
    m_eqFileList = new QPtrList<EqFile>();
    Q_CHECK_PTR( m_eqFileList );
    m_eqFileList->setAutoDelete( true );

    // Create the language list
    m_languageList = new QPtrList<QString>();
    Q_CHECK_PTR( m_languageList );
    m_languageList->setAutoDelete( true );

    // Create the module list
    m_moduleList = new QPtrList<Module>();
    Q_CHECK_PTR( m_moduleList );
    m_moduleList->setAutoDelete( true );

    // Count occurrences of <language>, <itemList>, <function>, and <variable>
    log( "    Counting XML elements...\n" );
    countElements();

    // Create the application-wide, shared, SI units converter
    log( "    Creating SI Units ...\n" );
    appSiUnitsCreate();

    // Create the application-wide, shared, default property dictionary
    log( QString( "    Creating property dictionary with %1 slots...\n" )
        .arg( m_propertyPrime ) );
    appPropertyCreate( m_propertyPrime );

    // Create the application-wide, shared translation dictionary
    log( QString( "    Creating translation dictionary with %1 slots...\n" )
        .arg( m_translatePrime ) );
    appTranslatorCreate( m_translatePrime );

    // Create the function name lookup dictionary
    log( QString( "    Creating function dictionary with %1 slots...\n" )
        .arg( m_functionPrime ) );
    m_funDict = new QDict<EqFun>( m_functionPrime, true );
    Q_CHECK_PTR( m_funDict );
    m_funDict->setAutoDelete( true );

    // Create the variable name lookup dictionary
    log( QString( "    Creating variable dictionary with %1 slots...\n" )
        .arg( m_variablePrime ) );
    m_varDict = new QDict<EqVar>( m_variablePrime, true );
    Q_CHECK_PTR( m_varDict );
    m_varDict->setAutoDelete( true );

    // Create the itemList lookup dictionary
    log( QString( "    Creating item list dictionary with %1 slots...\n" )
        .arg( m_itemListPrime ) );
    m_itemListDict = new QDict<EqVarItemList>( m_itemListPrime );
    Q_CHECK_PTR( m_itemListDict );
    m_itemListDict->setAutoDelete( true );

    // Allocate all dynamic storage.
    log( QString( "    Creating function array with %1 slots...\n" )
        .arg( m_functionCount ) );
    m_fun = new EqFun *[ m_functionCount ];
    checkmem( __FILE__, __LINE__, m_fun, "EqFun *m_fun", m_functionCount );

    log( QString( "    Creating variable array with %1 slots...\n" )
        .arg( m_variableCount ) );
    m_var = new EqVar *[ m_variableCount ];
    checkmem( __FILE__, __LINE__, m_fun, "EqVar *m_var", m_variableCount );

    log( QString( "    Creating item list array with %1 slots...\n" )
        .arg( m_itemListCount ) );
    m_itemList = new EqVarItemList *[ m_itemListCount ];
    checkmem( __FILE__, __LINE__, m_itemList, "EqVar *m_itemList",
        m_itemListCount );

    int id;
    for ( id = 0;
          id < m_variableCount;
          id++ )
    {
        m_var[id] = 0;
    }
    for ( id = 0;
          id < m_functionCount;
          id++ )
    {
        m_fun[id] = 0;
    }
    for ( id = 0;
          id < m_itemListCount;
          id++ )
    {
        m_itemList[id] = 0;
    }
    // Parse the EqApp definition.  This performs the following tasks:
    //  fills the application-wide, shared, translation dictionary
    //  fills the application-wide, shared, property dictionary
    //  fills the m_itemList[] array
    //  fills the m_fun[] array
    //  fills the m_var[] array
    log( "    Parsing XML file ...\n" );
    parse( m_xmlFile,
        true,                       // Perform name cross-validation?
        false );                    // Print debug output?

    // The translator is now enabled
    appTranslatorEnabled( true );

    // Get the address of the "FuelBedModel" EqVarItemList
    QString text("");
    EqVarItemList *itemListPtr = m_itemListDict->find( "FuelBedModel" );
    if ( ! itemListPtr )
    // This code block should never be executed!
    {
        translate( text, "EqApp:MissingItemList", "FuelBedModel" );
        bomb( text );
    }
    // Create the fuel model list
    m_fuelModelList = new FuelModelList();
    checkmem( __FILE__, __LINE__, m_fuelModelList,
        "FuelModelList m_fuelModelList", 1 );
    // Add the 60 standard fuel models
    m_fuelModelList->addStandardFuelModels();
    // Add the standard fuel models to the FuelBedModel item list
    FuelModel *fmPtr;
    int index = 0;
    QString sortKey("");
    for ( fmPtr=m_fuelModelList->first();
          fmPtr != 0;
          fmPtr=m_fuelModelList->next() )
    {
        //sortKey.sprintf( "%3d", fmPtr->m_number );
		sortKey = QString("%1").arg( fmPtr->m_number, 3 );
        itemListPtr->addItem( fmPtr->m_name, sortKey, index++, true, false );
    }

    // Get the address of the "FuelMoisScenario" EqVarItemList
    if ( ! ( itemListPtr = m_itemListDict->find( "FuelMoisScenario" ) ) )
    // This code block should never be executed!
    {
        translate( text, "EqApp:MissingItemList", "FuelMoisScenario" );
        bomb( text );
    }
    // Create the moisture scenario list
    m_moisScenarioList = new MoisScenarioList();
    checkmem( __FILE__, __LINE__, m_moisScenarioList,
        "MoisScenarioList m_moisScenarioList", 1 );
    // Create the standard fuel MoisScenarios
    // Pat doesn't want to do this!
    if ( false )
    {
        m_moisScenarioList->addStandardMoisScenarios();
        // Add the standard MoisScenarios to the FuelMoisScenario item list
        MoisScenario *msPtr;
        index = 0;
        for ( msPtr=m_moisScenarioList->first();
              msPtr != 0;
              msPtr=m_moisScenarioList->next() )
        {
            sortKey.sprintf( "%03d", index );
            itemListPtr->addItem( msPtr->m_name, sortKey, index++, true, false );
        }
    }
    // Set the language to the appLanguage property
    // If it is blank or unknown, en_US will be used
    setLanguage( appProperty()->string( "appLanguage" ) );

    // Create all the EqVar consumer/producer and EqFun input/output
    // cross references
    log( "    Initializing all arrays ....\n" );
    init();
    log( "End Section: EqApp constructor\n" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqApp destructor.
 */

EqApp::~EqApp( void )
{
    delete[] m_fun;                 m_fun = 0;
    delete[] m_var;                 m_var = 0;
    delete[] m_itemList;            m_itemList = 0;
    delete   m_funDict;             m_funDict = 0;
    delete   m_varDict;             m_varDict = 0;
    delete   m_itemListDict;        m_itemListDict = 0;
    delete   m_fuelModelList;       m_fuelModelList = 0;
    delete   m_moisScenarioList;    m_moisScenarioList = 0;
    delete   m_moduleList;          m_moduleList = 0;
    delete   m_languageList;        m_languageList = 0;
    delete   m_eqFileList;          m_eqFileList = 0;
    delete   m_eqTreeList;          m_eqTreeList = 0;
    appTranslatorDelete();
    appPropertyDelete();
    appSiUnitsDelete();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates an EqFile and adds it to the m_eqFileList.
 *
 *  \param name         File path name relative to INSTALLDIR
 *  \param type         EqFileType
 *  \param permission   File permission "[E|e][R|r][W|w]"
 *  \param releaseFrom  File is required beginning with this release
 *  \param releaseThru  File is required through with this release
 *
 *  \return Pointer to the newly allocated EqFun.
 */

EqFile *EqApp::addEqFile( const QString &name, EqFileType type,
        const QString &permission, int releaseFrom, int releaseThru )
{
    // Create a new EqFile
    EqFile *filePtr = new EqFile( name, type, permission,
        releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, filePtr, "EqFile filePtr", 1 );
    // Add EqFile to the m_eqFileList
    m_eqFileList->append( filePtr );
    return( filePtr );
}

//------------------------------------------------------------------------------
/*! \brief Creates an EqFun and adds its pointer to the m_fun[] array.
 *
 *  The function's address must be added at some point!
 *  In our case, it is added by EqTreeParser::startElement() whenever
 *  the <function> "input=" and "output=" attributes are parsed.
 *
 *  \return Pointer to the newly allocated EqFun.
 */

EqFun *EqApp::addEqFun( const QString &name, int inputs, int outputs,
        int releaseFrom, int releaseThru, const QString &module )
{
    // Make sure there's room in the m_fun[] array.
    if ( m_funCount >= m_functionCount )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqApp:FunctionOverflow", name,
            QString( "%1" ).arg( m_functionCount ) );
        bomb( text );
    }
    // Create a new EqFun
    EqFun *funPtr = new EqFun( name, (PFV) 0, inputs, outputs,
        releaseFrom, releaseThru, module );
    checkmem( __FILE__, __LINE__, funPtr, "EqFun funPtr", 1 );
    // Add function name to the dictionary
    m_funDict->insert( name, funPtr );
    // Add the EqFun to the array and increment the counter.
    m_fun[m_funCount++] = funPtr;
    return( funPtr );
}

//------------------------------------------------------------------------------
/*! \brief Creates a continuous EqVar
 *  and stores its pointer in the EqApp's m_var[] array.
 *
 *  \return Pointer to the newly allocated EqVar.
 */

EqVar *EqApp::addEqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder, const QString &wizard,
        const QString &nativeUnits, int nativeDecimals,
        const QString &englishUnits, int englishDecimals,
        const QString &metricUnits, int metricDecimals,
        double nativeMinimum, double nativeMaximum, double defaultValue,
        int releaseFrom, int releaseThru )
{
    // Make sure there's room in the m_var[] array.
    if ( m_varCount >= m_variableCount )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqApp:VariableOverflow", name,
            QString( "%1" ).arg( m_variableCount ) );
        bomb( text );
    }
    // Create a new EqVar (without any producer/consumer arrays)
    EqVar *varPtr = new EqVar( name, help, inpOrder, outOrder, wizard,
        nativeUnits, nativeDecimals, englishUnits, englishDecimals,
        metricUnits, metricDecimals, nativeMinimum, nativeMaximum,
        defaultValue, releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, varPtr, "EqVar varPtr", 1 );
    // Add the new EqVar ptr to the EqVar name lookup dictionary
    m_varDict->insert( name, varPtr );
    // Add the new EqVar ptr to the m_var[] array and increment the counter.
    m_var[m_varCount++] = varPtr;
    return( varPtr );
}

//------------------------------------------------------------------------------
/*! \brief Creates a discrete EqVar
 *  and stores its pointer in the EqApps's m_var[] array.
 *
 *  \return Pointer to the newly allocated EqVar.
 */

EqVar *EqApp::addEqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder, const QString &wizard,
        EqVarItemList *itemList, int releaseFrom, int releaseThru )
{
    // Make sure there's room in the m_var[] array.
    if ( m_varCount >= m_variableCount )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqApp:VariableOverflow", name,
            QString( "%1" ).arg( m_variableCount ) );
        bomb( text );
    }
    // Create a new EqVar
    EqVar *varPtr = new EqVar( name, help, inpOrder, outOrder, wizard,
        itemList, releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, varPtr, "EqVar varPtr", 1 );
    // Add variable name to the dictionary
    m_varDict->insert( name, varPtr );
    // Add the EqVar to the array and increment the counter.
    m_var[m_varCount++] = varPtr;
    return( varPtr );
}

//------------------------------------------------------------------------------
/*! \brief Creates a text EqVar
 *  and stores its pointer in the EqApp's m_var[] array.
 *
 *  \return Pointer to the newly allocated EqVar.
 */

EqVar *EqApp::addEqVar( const QString &name, const QString &help,
        const QString &inpOrder, const QString &outOrder,
        const QString &wizard, int releaseFrom, int releaseThru )
{
    // Make sure there's room in the m_var[] array.
    if ( m_varCount >= m_variableCount )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqApp:VariableOverflow", name,
            QString( "%1" ).arg( m_variableCount ) );
        bomb( text );
    }
    // Create a new EqVar
    EqVar *varPtr = new EqVar( name, help, inpOrder, outOrder, wizard,
        releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, varPtr, "EqVar varPtr", 1 );
    // Add variable name to the dictionary
    m_varDict->insert( name, varPtr );
    // Add the EqVar to the array and increment the counter.
    m_var[m_varCount++] = varPtr;
    return( varPtr );
}

//------------------------------------------------------------------------------
/*! \brief Creates an Module and adds it to the m_moduleList.
 *
 *  \param name         Module name (lower case, 1 word)
 *  \param textKey      Module Dialog checkbox text key.
 *  \param sort         Module Dialog checkbox order key.
 *  \param indent       Module Dialog indentation level.
 *  \param releaseFrom  Module available beginning with this release.
 *  \param releaseThru  Module available through with this release.
 *
 *  \return Pointer to the newly allocated EqFun.
 */

Module *EqApp::addModule( const QString &name, const QString &textKey,
        const QString sort, int indent, int releaseFrom, int releaseThru )
{
    // Create a new Module
    Module *modPtr = new Module( name, textKey, sort, indent,
        releaseFrom, releaseThru );
    checkmem( __FILE__, __LINE__, modPtr, "Module modPtr", 1 );
    // Add function name to the m_moduleList
    m_moduleList->append( modPtr );
    return( modPtr );
}

//------------------------------------------------------------------------------
/*! \brief Adds output variable names to the Module's list of output variables.
 *
 *  \param moduleName   Module name (lower case, 1 word)
 *  \param varName      Output variable name
 *
 *  \return TRUE if moduleName was found, FALSE if not found.
 */

bool EqApp::addModuleOutputVariable( const QString &moduleName,
    const QString &varName )
{
    for ( Module *module = m_moduleList->first();
          module;
          module = m_moduleList->next() )
    {
        if ( module->m_name == moduleName )
        {
            module->m_output.append( varName );
            return( true );
        }
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Opens and reads a Fuel Model file into a FuelModel,
 *  adds it to the application's m_fuelModelList,
 *  and adds it to the application's m_itemList.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool EqApp::attachFuelModel( const QString &fileName )
{
    QString text("");
    // Start of Version 2 Behavior Change
    // The following tests were removed so that fuel models could be saved
    // and automatically updated in the attached list.
    if ( false )
    {
        // Make sure this file isn't already in the list.
        if ( m_fuelModelList->fuelModelByFileName( fileName ) )
        {
            translate( text, "EqApp:FuelModelAlreadyAttached", fileName );
            warn( text );
            return( true );
        }
        // Make sure there isn't already a Fuel Model with this name.
        QFileInfo fi( fileName );
        if ( ( m_fuelModelList->fuelModelByModelName( fi.baseName() ) ) != 0 )
        {
            translate( text, "EqApp:FuelModelAlreadyExists",
                fileName, fi.baseName() );
            warn( text );
            return( false );
        }
	}
    // Instead, if this fuel model is in the attached list, remove it.
    FuelModel *fmPtr;
    if ( ( fmPtr = m_fuelModelList->fuelModelByFileName( fileName ) ) )
    {
        deleteFuelModel( fmPtr->m_name );
    }
    // End of Version 2 Behavior Change

    // Attempt to load the fuel model file.
    fmPtr = new FuelModel();
    checkmem( __FILE__, __LINE__, fmPtr, "FuelModel fmPtr", 1 );
    if ( ! ( fmPtr->loadBpf( fileName ) ) )
    {
        delete fmPtr;   fmPtr = 0;
        return( false );
    }

	// Build 610: check if the newly loaded fuel model has
	// a code or number already in the list
	if ( appProperty()->boolean( "fuelModelPreventDuplicateNumbers" ) )
	{
		if ( m_fuelModelList->fuelModelByModelName( fmPtr->m_name ) )
		{
            translate( text, "EqApp:FuelModelNameAlreadyExists",
                fileName, fmPtr->m_name );
            warn( text );
			return false;
		}
		QString num = QString( "%1" ).arg( fmPtr->m_number );
		if ( m_fuelModelList->fuelModelByModelName( num ) )
		{
            translate( text, "EqApp:FuelModelNumberAlreadyExists",
                fileName, num );
            warn( text );
			return false;
		}
	}

    // Add the FuelModel address to the application's m_fuelModelList
    m_fuelModelList->append( fmPtr );

    // Add the name, sort key, and description to the FuelBedModel EqVarItem
    // list and its description key to the translator
	QString sort = QString("%1").arg( fmPtr->m_number, 3 );
    attachItem( "FuelBedModel", fileName, fmPtr->m_name, sort,
        m_fuelModelList->count(), fmPtr->m_desc ) ;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Adds a newly attached FuelBedModel or FuelMoisScenario to its
 *  EqVarItem list and adds its description to the translator.
 */

bool EqApp::attachItem( const QString &listName, const QString &fileName,
        const QString &name, const QString &sort, int index,
        const QString &desc )
{
    QString text("");
    // Find the FuelBedModel or MoisScenario list
    EqVarItemList *itemList = m_itemListDict->find( listName );
    if ( ! itemList )
    // This code block should never be executed!
    {
        translate( text, "EqApp:AttachItem:ListNotFound", fileName, listName );
        bomb( text );
    }
    // Add this as a new item to the list
    EqVarItem *item = itemList->addItem( name, sort, index, false, false );

    // Add the item's description to the translator.
    // Since we have no way of translating this text between languages,
    // we will set keys for ALL languages even though they all return the
    // same native description text.
    // At least this way we get text back regardless of the language.
    QString *lang, key("");
    for ( lang = m_languageList->first();
          lang != 0;
          lang = m_languageList->next() )
    {
        key = listName + ":" + fileName;
        appTranslatorInsert( key, *lang, desc, true );
    }
    // Now we can set its description pointer for the current language
    key = QString( listName + ":" + fileName + ":" + m_language );
    if ( ! ( item->m_desc = appTranslatorFind( key ) ) )
    // This code block should never be executed!
    {
        translate( text, "EqApp:SetLanguage:NoKey", key );
        error( text );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Opens and reads a Moisture Scenario file into a MoisScenario,
 *  and adds it to the application's m_moisScenarioList.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool EqApp::attachMoisScenario( const QString &fileName )
{
    QString text("");
    // Start of Version 2 Behavior Change
    // The following tests were removed so that fuel models could be saved
    // and automatically updated in the attached list.
    if ( false )
    {
        // Make sure this file isn't already in the list.
        if ( m_moisScenarioList->moisScenarioByFileName( fileName ) )
        {
            translate( text, "EqApp:MoisScenarioAlreadyAttached", fileName );
            warn( text );
            return( true );
        }
        // Make sure there isn't already a Moisture Scenario with this name.
        QFileInfo fi( fileName );
        if ( ( m_moisScenarioList->moisScenarioByScenarioName( fi.baseName() ) )
            != 0 )
        {
            translate( text, "EqApp:MoisScenarioAlreadyExists",
                fileName, fi.baseName() );
            warn( text );
            return( false );
        }
    }
    // Instead, if this moisture scenario is in the attached list, remove it.
    MoisScenario *msPtr;
    if ( ( msPtr = m_moisScenarioList->moisScenarioByFileName( fileName ) ) )
    {
        deleteMoisScenario( msPtr->m_name );
    }
    // End of Version 2 Behavior Change

    // Create a MoisScenario instance.
    msPtr = new MoisScenario() ;
    checkmem( __FILE__, __LINE__, msPtr, "MoisScenario msPtr", 1 );
    if ( ! ( msPtr->loadBpm( fileName ) ) )
    {
        delete msPtr;   msPtr = 0;
        return( false );
    }
    // Add the FuelMoisScenario address to the application's m_moisScenarioList
    m_moisScenarioList->append( msPtr );

    // Add the name, sort key, and description to the FuelMoisScenario EqVarItem
    // list and its description key to the translator
    attachItem( "FuelMoisScenario", fileName, msPtr->m_name, msPtr->m_name,
        m_moisScenarioList->count(), msPtr->m_desc ) ;
    return( true );
}

//------------------------------------------------------------------------------
/*! /brief Opens the EqTree definition XML document file and does a quick and
 *  dirty count of the number of <function>, <variable>, <itemList>, and
 *  <translation > elements.  It then determines the prime number equal to
 *  or greater than each count.
 */

void EqApp::countElements( void )
{
    FILE *fptr;
    char buffer[1024];
    if ( ! ( fptr = fopen( m_xmlFile.latin1(), "r" ) ) )
    // This code block should never be executed!
    {
        // Cannot call translator since dictionary it hasn't been built yet!
        bomb( QString( "EqApp::countElements() -- "
            " unable to open XML file \"%1\"." ).arg( m_xmlFile ) );
    }
    while( fgets( buffer, sizeof( buffer ), fptr ) )
    {
        if ( strstr( buffer, "<variable name=" ) )
        {
            m_variableCount++;
        }
        else if ( strstr( buffer, "<translate key=" ) )
        {
            m_translateCount++;
        }
        else if ( strstr( buffer, "<property name=" ) )
        {
            m_propertyCount++;
        }
        else if ( strstr( buffer, "<function name=" ) )
        {
            m_functionCount++;
        }
        else if ( strstr( buffer, "<itemList name=" ) )
        {
            m_itemListCount++;
        }
        else if ( strstr( buffer, "<language name=" ) )
        {
            m_languageCount++;
        }
    }
    fclose( fptr );

    // Assume a translation for each language
    m_translateCount *= m_languageCount;

    // Get the next highest or equal prime number
    m_functionPrime  = getPrimeGTE( m_functionCount );
    m_itemListPrime  = getPrimeGTE( m_itemListCount );
    m_propertyPrime  = getPrimeGTE( m_propertyCount );
    m_translatePrime = getPrimeGTE( m_translateCount );
    m_variablePrime  = getPrimeGTE( m_variableCount );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Removes the named fuel model from the application's m_fuelModelList,
 *  which also caused it to be deleted since the list is autoDelete.
 *  Also removes it from the FuelBedModel EqVarItemList.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool EqApp::deleteFuelModel( const QString &name )
{
    // Remove this item from its EqVarItemList.
    if ( ! deleteItem( "FuelBedModel", name ) )
    // This code block should never be executed!
    {
        return( false );
    }
    // Remove the FuelModel from the application's m_fuelModelList.
    if ( ! m_fuelModelList->deleteFuelModel( name ) )
    // This code block should never be executed!
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Removes the named item from the application's m_itemList,
 *  and removes its keys from the translator.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool EqApp::deleteItem( const QString &listName, const QString &itemName )
{
    QString text("");
    // Get the EqVarItemList.
    EqVarItemList *itemList = m_itemListDict->find( listName );
    if ( ! itemList )
    // This code block should never be executed!
    {
        translate( text, "EqApp:DeleteItem:ListNotFound", itemName, listName );
        bomb( text );
    }
    // Locate this item from the item list.
    EqVarItem *itemPtr = itemList->itemWithName( itemName, true );
    if ( ! itemPtr )
    // This code block should never be executed!
    {
        translate( text, "EqApp:DeleteItem:ItemNotFound", itemName, listName );
        error( text );
        return( false );
    }
    // If this is a permanent item, display a warning and return.
    if ( itemPtr->m_perm )
    {
        translate( text, "EqApp:DeleteItem:ItemIsPerm", itemName );
        warn( text );
        return( false );
    }
    // Remove its translator keys
    QString *lang, key("");
    for ( lang = m_languageList->first();
          lang != 0;
          lang=m_languageList->next() )
    {
        key = listName + ":" + itemName;
        appTranslatorRemove( key, *lang );
    }
    // Remove it from the EqVarItemList.
    if ( ! itemList->removeItemByName( itemName ) )
    // This code block should never be executed!
    {
        translate( text, "EqApp:DeleteItem:NotRemoved", itemName );
        error( text );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Removes the named moisture scenario from the application's
 *  m_moisScenarioList, which also caused it to be deleted since the list is
 *  autoDelete. Also removes it from the FuelMoisScenario EqVarItemList.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool EqApp::deleteMoisScenario( const QString &name )
{
    // Remove this item from its EqVarItemList.
    if ( ! deleteItem( "FuelMoisScenario", name ) )
    // This code block should never be executed!
    {
        return( false );
    }
    // Remove the MoisScenario from moisScenarioList and delete its instance.
    if ( ! m_moisScenarioList->deleteMoisScenario( name ) )
    // This code block should never be executed!
    {
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines which modules (if any) produces the specified variable.
 *
 *  \param varName Output variable name
 *
 *  \return Space-separated string of zero or more module names.
 */

QString EqApp::findProducerModule( const QString &varName )
{
    QString modules("");
	// HACK for lenght-to-width ratio and forward spread distance,
	// which sometimes get calculated in SURFACE even though they are input to SIZE
	// Pat wants only their INPUT module to be listed (i.e., just 'SIZE').
	const char* str = varName.latin1();
	if ( ! strcmp( str, "vSurfaceFireLengthToWidth" ) )
	{
		modules = "SIZE";
		return( modules );
	}

    for ( Module *module = m_moduleList->first();
          module;
          module = m_moduleList->next() )
    {
        if ( module->producesVariable( varName ) )
        {
            if ( ! modules.isEmpty() )
            {
                modules += " ";
            }
            modules += module->m_name;
        }
    }
    return( modules );
}

//------------------------------------------------------------------------------
/*! \brief Initializes the individual EqVar and EqFun cross pointer arrays
 *  and creates the EqCalc.
 */

void EqApp::init( void )
{
    int fun, var, con, pro;
    EqFun *funPtr;
    EqVar *varPtr;

    // Allocate each EqVar's consumer/producer arrays and initialize them to 0
    for ( var = 0;
          var < m_varCount;
          var++ )
    {
        m_var[var]->init();
    }
    // Initialize each function's variables
    for ( fun = 0;
          fun < m_funCount;
          fun++ )
    {
        funPtr = m_fun[fun];
        // Initialize all the function's input variables
        for ( var = 0;
              var < funPtr->m_inputs;
              var++ )
        {
            varPtr = funPtr->m_input[var];
            // Now add the EqFun's address to the end of this EqVar's
            // m_producer[] array
            for ( con = 0;
                  con < varPtr->m_consumers;
                  con++ )
            {
                if ( ! varPtr->m_consumer[con] )
                {
                    varPtr->m_consumer[con] = funPtr;
                    break;
                }
            }
            // Check for overflow!
            if ( con >= varPtr->m_consumers )
            // This code block should never be executed!
            {
                QString text("");
                translate( text, "EqApp:ConsumerOverflow",
                    funPtr->m_name,
                    QString( "%1" ).arg( var ),
                    varPtr->m_name,
                    QString( "%1" ).arg( varPtr->m_consumers ) );
                bomb( text );
            }
        }
        // Initialize all the function's output variables
        for ( var = 0;
              var < funPtr->m_outputs;
              var++ )
        {
            varPtr = funPtr->m_output[var];
            // Now add the EqFun's address to the end of this EqVar's
            // m_producer[] array
            for ( pro = 0;
                  pro < varPtr->m_producers;
                  pro++ )
            {
                if ( ! varPtr->m_producer[pro] )
                {
                    varPtr->m_producer[pro] = funPtr;
                    break;
                }
            }
            // Check for overflow!
            if ( pro >= varPtr->m_producers )
            // This code block should never be executed!
            {
                QString text("");
                translate( text, "EqApp:ProducerOverflow",
                    funPtr->m_name,
                    QString( "%1" ).arg( var ),
                    varPtr->m_name,
                    QString( "%1" ).arg( varPtr->m_producers ) );
                bomb( text );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a new EqTree with appropriate array sizes
 *  and adds it to the EqTree list.
 *
 *  \param treeName Name given to this EqTree
 *  \param fileName Name of XML variable/property file to read and store
 *                  in the EqTree.  Ig blank, no file is loaded.
 *  \param language Name of language.  If blank, uses current EqApp language.
 */

EqTree *EqApp::newEqTree( const QString &treeName, const QString &fileName,
        const QString &language )
{
    // Create the EqTree
    EqTree *eqTree = new EqTree( this, treeName, m_functionCount,
        m_functionPrime, m_variableCount, m_variablePrime, m_propertyPrime,
        m_itemList, m_itemListCount, m_itemListDict,
        m_fuelModelList, m_moisScenarioList );
    checkmem( __FILE__, __LINE__, eqTree, "EqTree *eqTree", 1 );

    // Set the language
    if ( language.isNull() || language.isEmpty() )
    {
        eqTree->setLanguage( m_language );
    }
    else
    {
        eqTree->setLanguage( language );
    }
    // Add this tree to the list
    m_eqTreeList->append( eqTree );

    // If a file name is specified, load it
    if ( ! fileName.isNull() && ! fileName.isEmpty() )
    {
        eqTree->readXmlFile( fileName );
    }
    
	FILE* csv;
    if ( ( csv = fopen( "BehavePlus6Vars.csv", "w" ) ) && false )
    {
		eqTree->printVarCsv( csv );
		fclose( csv );
	}
	return( eqTree );
}

//------------------------------------------------------------------------------
/*! \brief Parses an EqTree definition XML document into the EqTree.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool EqApp::parse( const QString &fileName, bool validate, bool debug )
{
    EqAppParser *handler = new EqAppParser( this, fileName, validate, debug );
    checkmem( __FILE__, __LINE__, handler, "EqAppParser handler", 1 );
    QFile xmlFile( fileName );
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( handler );
    reader.setErrorHandler( handler );
    bool result = reader.parse( &source );
    delete handler;
    return( result );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current application-wide language and updates all the
 *  child EqTree variable labels, descriptions, and headers.
 *
 *  \param language Language to use.  If NULL, empty, or unknown,
 *                  en_US is used.
 */

void EqApp::setLanguage( const QString &language )
{
    // If valid language, use it, otherwise use en_US
    if ( validLanguage( language ) )
    {
        m_language = language;
    }
    else
    {
        m_language = "en_US";
    }
    // Tell the translator
    appTranslatorSetLanguage( m_language );

    // Update all the shared EqVarItemList and EqVarItem descriptions
    QString key;
    EqVarItemList *list;
    EqVarItem *item;
    for ( int id = 0;
          id < m_itemListCount;
          id++ )
    {
        list = m_itemList[id];
        for ( item = list->first();
              item != 0;
              item= list->next() )
        {
            // Build the key
            key = QString( list->m_name + ":" + item->m_name + ":" + m_language );
            // Locate the key
            if ( ! ( item->m_desc = appTranslatorFind( key ) ) )
            // This code block should never be executed!
            {
                QString msg("");
                translate( msg, "EqApp:SetLanguage:NoKey", key );
                error( msg );
            }
        }
    }
    // Update all the EqTree variable labels, descriptions, and headers.
    for ( EqTree *eqTree = m_eqTreeList->first();
          eqTree != 0;
          eqTree = m_eqTreeList->next() )
    {
        eqTree->setLanguage( m_language );
        //eqTree->reconfigure();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines if the requested language is known.
 *
 *  \return TRUE if language is known, FALSE if it is not.
 */

bool EqApp::validLanguage( const QString &language ) const
{
    // Test and store the new language
    for ( QString *lang = m_languageList->first();
          lang != 0;
          lang = m_languageList->next() )
    {
        if ( *lang == language )
        {
            return( true );
        }
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Returns the first prime number greater than or equal to the passed
 *  integer (up to 9973).
 */

static int Prime[] =
{
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31,
    37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
    233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
    353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
    547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
    607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
    661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
    811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
    947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013,
    1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
    1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151,
    1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223,
    1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
    1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
    1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451,
    1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
    1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583,
    1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657,
    1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733,
    1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
    1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889,
    1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987,
    1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053,
    2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
    2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213,
    2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287,
    2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357,
    2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
    2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531,
    2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617,
    2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687,
    2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741,
    2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819,
    2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903,
    2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999,
    3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
    3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181,
    3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257,
    3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331,
    3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
    3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511,
    3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571,
    3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643,
    3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727,
    3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821,
    3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907,
    3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989,
    4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057,
    4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139,
    4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231,
    4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297,
    4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409,
    4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493,
    4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583,
    4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657,
    4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751,
    4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831,
    4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937,
    4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003,
    5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087,
    5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179,
    5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279,
    5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387,
    5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443,
    5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521,
    5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639,
    5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693,
    5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791,
    5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857,
    5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939,
    5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053,
    6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133,
    6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221,
    6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301,
    6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367,
    6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473,
    6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571,
    6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673,
    6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761,
    6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833,
    6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917,
    6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997,
    7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103,
    7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207,
    7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297,
    7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411,
    7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499,
    7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561,
    7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643,
    7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723,
    7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829,
    7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919,
    7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017,
    8039, 8053, 8059, 8069, 8081, 8087, 8089, 8093, 8101, 8111,
    8117, 8123, 8147, 8161, 8167, 8171, 8179, 8191, 8209, 8219,
    8221, 8231, 8233, 8237, 8243, 8263, 8269, 8273, 8287, 8291,
    8293, 8297, 8311, 8317, 8329, 8353, 8363, 8369, 8377, 8387,
    8389, 8419, 8423, 8429, 8431, 8443, 8447, 8461, 8467, 8501,
    8513, 8521, 8527, 8537, 8539, 8543, 8563, 8573, 8581, 8597,
    8599, 8609, 8623, 8627, 8629, 8641, 8647, 8663, 8669, 8677,
    8681, 8689, 8693, 8699, 8707, 8713, 8719, 8731, 8737, 8741,
    8747, 8753, 8761, 8779, 8783, 8803, 8807, 8819, 8821, 8831,
    8837, 8839, 8849, 8861, 8863, 8867, 8887, 8893, 8923, 8929,
    8933, 8941, 8951, 8963, 8969, 8971, 8999, 9001, 9007, 9011,
    9013, 9029, 9041, 9043, 9049, 9059, 9067, 9091, 9103, 9109,
    9127, 9133, 9137, 9151, 9157, 9161, 9173, 9181, 9187, 9199,
    9203, 9209, 9221, 9227, 9239, 9241, 9257, 9277, 9281, 9283,
    9293, 9311, 9319, 9323, 9337, 9341, 9343, 9349, 9371, 9377,
    9391, 9397, 9403, 9413, 9419, 9421, 9431, 9433, 9437, 9439,
    9461, 9463, 9467, 9473, 9479, 9491, 9497, 9511, 9521, 9533,
    9539, 9547, 9551, 9587, 9601, 9613, 9619, 9623, 9629, 9631,
    9643, 9649, 9661, 9677, 9679, 9689, 9697, 9719, 9721, 9733,
    9739, 9743, 9749, 9767, 9769, 9781, 9787, 9791, 9803, 9811,
    9817, 9829, 9833, 9839, 9851, 9857, 9859, 9871, 9883, 9887,
    9901, 9907, 9923, 9929, 9931, 9941, 9949, 9967, 9973
};

static int getPrimeGTE( int n )
{
    int primes = sizeof(Prime) / sizeof(int);
    for ( int i = 0;
          i < primes;
          i++ )
    {
        if ( Prime[i] >= n )
        {
            return( Prime[i] );
        }
    }
    return( Prime[primes-1] );
}

//------------------------------------------------------------------------------
//  End of xeqapp.cpp
//------------------------------------------------------------------------------

