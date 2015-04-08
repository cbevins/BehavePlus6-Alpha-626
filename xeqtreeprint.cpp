//------------------------------------------------------------------------------
/*! \file xeqtreeprint.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Debugging print support for the Experimental Equation Tree
 *  EqApp, EqFun, EqTree, and EqVar classes.
 */

#define XEQTREE_PRINT_SUPPORT 1
#ifdef XEQTREE_PRINT_SUPPORT

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "module.h"
#include "property.h"
#include "xeqapp.h"
#include "xeqtree.h"
#include "xeqvar.h"
#include "xeqvaritem.h"

// Qt include files
#include <qdict.h>
#include <qstringlist.h>

// Standard include files
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/*! \brief Prints the EqApp's shared m_fun[] function array.
 */

void EqApp::printFunctions( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqApp m_fun[] Function Array\n" );
    fprintf( fptr, "There are %d functions:\n", m_funCount );
    for ( int id = 0;
          id < m_funCount;
          id++ )
    {
        m_fun[id]->print( fptr );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqApp's shared m_var[] variable array.
 */

void EqApp::printVariables( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqApp m_var[] Variable Array\n" );
    fprintf( fptr, "There are %d variables:\n", m_varCount );
    for ( int id = 0;
          id < m_varCount;
          id++ )
    {
        m_var[id]->print( fptr );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqFun's parameters to the file stream.
 */

void EqFun::print( FILE *fptr ) const
{
    // Display the function name and input and output variables
    fprintf( fptr,"\n%s (%lu)\n",
        m_name.latin1(), (unsigned long) this );
    fprintf( fptr, "    Flags   : %s\n",
        ( m_active ) ? "ACTIVE" : "inactive" );
    // Input variables
    EqVar *varPtr;
    int i;
    fprintf( fptr, "    Inputs  : %d\n", m_inputs );
    for ( i = 0;
          i < m_inputs;
          i++ )
    {
        varPtr = m_input[i];
        if ( varPtr->isContinuous() )
        {
            fprintf( fptr, "    %7d : %s [%1.*f %s] (%lu) [%s]\n",
                i,
                varPtr->m_name.latin1(),
                varPtr->m_nativeDecimals,
                varPtr->m_nativeValue,
                varPtr->m_nativeUnits.latin1(),
                (unsigned long) varPtr,
                ( m_dirty[i] ) ? "DIRTY" : "clean" );
        }
        else if ( varPtr->isDiscrete() )
        {
            fprintf( fptr, "    %7d : %s [%d %s] (%lu)\n",
                i,
                varPtr->m_name.latin1(),
                varPtr->activeItemDataIndex(),
                varPtr->activeItemName().latin1(),
                (unsigned long) varPtr );
        }
    }
    // Output variables
    fprintf( fptr, "    Outputs : %d\n", m_outputs );
    for ( i = 0;
          i < m_outputs;
          i++ )
    {
        varPtr = m_output[i];
        if ( varPtr->isContinuous() )
        {
            fprintf( fptr, "    %7d : %s [%1.*f %s] (%lu)\n",
                i,
                varPtr->m_name.latin1(),
                varPtr->m_nativeDecimals,
                varPtr->m_nativeValue,
                varPtr->m_nativeUnits.latin1(),
                (unsigned long) varPtr );
        }
        else if ( varPtr->isDiscrete() )
        {
            fprintf( fptr, "    %7d : %s [%d %s] (%lu)\n",
                i,
                varPtr->m_name.latin1(),
                varPtr->activeItemDataIndex(),
                varPtr->activeItemName().latin1(),
                (unsigned long) varPtr );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the specified root EqVar's current value by recursively
 *  calling its producer functions.
 *
 *  This is the debug version. See also EqTree::calculateVariable().
 *
 *  This method uses upward propagation of the EqVar dirty flag so that
 *  calculation recursion can stop at the first clean variable.
 *  Dirty flags are cleaned as the recursion unwinds.
 */

void EqTree::calculateVariableDebug( EqVar *varPtr, int level )
{
    EqFun *funPtr;
    EqVar *subVarPtr;
    char   margin[256];
    // Display debug message for this level.
    if ( m_debug )
    {
        memset( margin, ' ', sizeof(margin) );
        margin[4*level] = '\0';
    }
    // Get the current producer for this variable.
    // If NULL, then this is user input (leaf), simply return.
    if ( ! ( funPtr = varPtr->activeProducerFunPtr() ) )
    {
        if ( m_debug )
        {
            if ( varPtr->isDiscrete() )
            {
                printf( "%s%02d: <%s> (%d %s) is produced by [UserInput].\n",
                    margin, level,
                    varPtr->m_name.latin1(),
                    varPtr->activeItemDataIndex(),
                    varPtr->m_activeItemName.latin1() );
            }
            else if ( varPtr->isContinuous() )
            {
                printf("%s%02d: <%s> (%1.*f %s) is produced by [UserInput].\n",
                    margin, level,
                    varPtr->m_name.latin1(),
                    varPtr->m_nativeDecimals,
                    varPtr->m_nativeValue,
                    varPtr->m_nativeUnits.latin1() );
            }
        }
        return;
    }
    // Otherwise this is a derived variable.
    if ( m_debug )
    {
        if ( varPtr->isDiscrete() )
        {
            printf("%s%02d: <%s> (%d %s) is produced by [%s].\n",
                margin, level,
                varPtr->m_name.latin1(),
                varPtr->activeItemDataIndex(),
                varPtr->m_activeItemName.latin1(),
                funPtr->m_name.latin1() );
        }
        else if ( varPtr->isContinuous() )
        {
            printf("%s%02d: <%s> (%1.*f %s) is produced by [%s].\n",
                margin, level,
                varPtr->m_name.latin1(),
                varPtr->m_nativeDecimals,
                varPtr->m_nativeValue,
                varPtr->m_nativeUnits.latin1(),
                funPtr->m_name.latin1() );
        }
    }
    // Check if any of the EqFun's inputs are dirty.
    int doCalc = 0;
    for ( int inputId = 0;
          inputId < funPtr->m_inputs;
          inputId++ )
    {
        if ( funPtr->m_dirty[inputId] )
        {
            doCalc++;
            subVarPtr = funPtr->m_input[inputId];
            if ( m_debug )
            {
                printf("%s    [%s] input %d <%s> is DIRTY; recursing...\n",
                    margin,
                    funPtr->m_name.latin1(),
                    inputId,
                    subVarPtr->m_name.latin1() );
            }
            calculateVariableDebug( subVarPtr, level+1 );
            funPtr->m_dirty[inputId] = false;
        }
        else
        {
            if ( m_debug )
            {
                subVarPtr = funPtr->m_input[inputId];
                printf("%s    [%s] input %d <%s> is CLEAN.\n",
                    margin,
                    funPtr->m_name.latin1(),
                    inputId,
                    subVarPtr->m_name.latin1() );
            }
        }
    }
    // If doCalc, we have to recalculate this variable.
    if ( doCalc )
    {
        // Recalculate this variable's value.
        ( m_eqCalc->*funPtr->m_function )();
        if ( m_debug )
        {
            if ( varPtr->isDiscrete() )
            {
                printf("%s|-> [%s] with %d of %d dirty inputs -- UPDATED <%s> to %d (%s).\n",
                    margin,
                    funPtr->m_name.latin1(),
                    doCalc,
                    funPtr->m_inputs,
                    varPtr->m_name.latin1(),
                    varPtr->activeItemDataIndex(),
                    varPtr->m_activeItemName.latin1() );
            }
            else if ( varPtr->isContinuous() )
            {
                printf("%s|-> [%s] with %d of %d dirty inputs -- UPDATED <%s> to %1.*f %s.\n",
                    margin,
                    funPtr->m_name.latin1(),
                    doCalc,
                    funPtr->m_inputs,
                    varPtr->m_name.latin1(),
                    varPtr->m_nativeDecimals,
                    varPtr->m_nativeValue,
                    varPtr->m_nativeUnits.latin1() );
            }
        }
    }
    else
    // Otherwise this variable and its ancestors were all clean.
    {
        if ( m_debug )
        {
            if ( varPtr->isDiscrete() )
            {
                printf("%s|-> [%s] with 0 of %d dirty inputs produces CLEAN <%s> with %d (%s).\n",
                    margin,
                    funPtr->m_name.latin1(),
                    funPtr->m_inputs,
                    varPtr->m_name.latin1(),
                    varPtr->activeItemDataIndex(),
                    varPtr->m_activeItemName.latin1() );
            }
            else if ( varPtr->isContinuous() )
            {
                printf("%s|-> [%s] with 0 of %d dirty inputs produces CLEAN <%s> with %1.*f %s.\n",
                    margin,
                    funPtr->m_name.latin1(),
                    funPtr->m_inputs,
                    varPtr->m_name.latin1(),
                    varPtr->m_nativeDecimals,
                    varPtr->m_nativeValue,
                    varPtr->m_nativeUnits.latin1() );
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the entire EqTree to a file in Graphviz "dot" file format
 *  for generating PostScript directed graphs.
 *
 *  \param fileName Name of the output Graphviz "dot" file.
 */

void EqTree::printDotFile( const QString &fileName, int release ) const
{
    // Try to open the output file
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "EqTree:UnopenedDotFile", fileName );
        bomb( text );
    }
    // Create a dictionary of printed variables
    QDict<EqVar> *varDict = new QDict<EqVar>( m_varDict->size() );
    Q_CHECK_PTR( varDict );

    // Print the dot file header
    fprintf( fptr, "digraph B {\n" );
    fprintf( fptr, "rankdir=LR;\n" );
    // This produces many pages that must be tiled together for printer output
    if ( false )
    {
        fprintf( fptr, "page=\"8.5,11.0\";\n" );
        fprintf( fptr, "rotate=90;\n" );
    }
    // This produces a single, long page for plotter output
    else if ( true )
    {
        fprintf( fptr, "size=\"80.0,34.0\";\n" );
        fprintf( fptr, "ratio=compress;\n" );
    }
    // Print the nodes and edges
    EqFun *fun;
    EqVar *var;
    for ( int id = 0;
          id < m_funCount;
          id++ )
    {
        fun = m_fun[id];
        // Only process functions for the current release
        if ( fun->isCurrent( release ) )
        {
            // Display function box
            fprintf( fptr, "%s [shape=box,style=filled,color=\"magenta1\"];\n",
                fun->m_name.latin1() );
            // Process all the function's input variables
            for ( int input = 0;
                  input < fun->m_inputs;
                  input++ )
            {
                var = fun->m_input[input];
                // Only display each variable node once
                if ( ! varDict->find( var->m_name ) )
                {
                    varDict->insert( var->m_name, var );
                    fprintf( fptr,
                        "%s [shape=ellipse,style=filled,color=\"goldenrod1\"];\n",
                        var->m_name.latin1() );
                }
                // Only display variables for the current release
                if ( var->isCurrent( release ) )
                {
                    // Print edge from this function node to its input variable node
                    fprintf( fptr, "%s -> %s;\n",
                        var->m_name.latin1(), fun->m_name.latin1() );
                }
            }
            // Process all the function's output variables
            for ( int output = 0;
                  output < fun->m_outputs;
                  output++ )
            {
                var = fun->m_output[output];
                // Only display each variable node once
                if ( ! varDict->find( var->m_name ) )
                {
                    varDict->insert( var->m_name, var );
                    fprintf( fptr,
                        "%s [shape=ellipse,style=filled,color=\"goldenrod1\"];\n",
                        var->m_name.latin1() );
                }
                // Only display variables for the current release
                if ( var->isCurrent( release ) )
                {
                    // Print edge from this function node to its output variable node
                    fprintf( fptr, "%s -> %s;\n",
                        fun->m_name.latin1(), var->m_name.latin1() );
                }
            }
        }   // if ( fun->isCurrent( release ) )
    }
    // Print subgraphs
    Module *mod;
    for ( mod = m_eqApp->m_moduleList->first();
          mod != 0;
          mod = m_eqApp->m_moduleList->next() )
    {
        if ( ! mod->isCurrent( release ) )
        {
            continue;
        }
        fprintf( fptr, "subgraph \"%s\" {\n", mod->m_name.latin1() );
        fprintf( fptr, "  label=\"%s\";\n", mod->m_name.latin1() );
        for ( int id = 0;
              id < m_funCount;
              id++ )
        {
            if ( m_fun[id]->m_module == mod->m_name
              && m_fun[id]->isCurrent( release ) )
            {
                fprintf( fptr, "  %s;\n", m_fun[id]->m_name.latin1() );
            }
        }
        fprintf( fptr, "  }\n" );
    }
    // Print dot file footer
    fprintf( fptr, "}\n" );
    // Close the file and return
    fclose( fptr );
    delete varDict;     varDict = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints all the EqVarItemLists in the EqTree.
 */

void EqTree::printEqVarItemLists( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree EqVarItemLists\n" );
    // Print the language options
    QString *lang;
    fprintf( fptr, "\nSupported languages include:\n" );
    for ( lang = m_eqApp->m_languageList->first();
          lang != 0;
          lang = m_eqApp->m_languageList->next() )
    {
        fprintf( fptr, "  %s\n", lang->latin1() );
    }
    // Print all the items in the current language
    EqVarItemList *list;
    EqVarItem *item;
#ifdef FROM_POINTER_ARRAY
    // From the pointer array
    for ( int i = 0;
          i < m_itemListCount;
          i++ )
    {
        list = m_itemList[i];
        fprintf( fptr, "\n%02d: %s\n",
            i+1, list->m_name.latin1() );
        for ( item = list->first();
              item != 0;
              item= list->next() )
        {
            fprintf( fptr, "        %s %4d %s [%s]\n",
                item->m_sort.latin1(),
                item->m_index,
                item->m_name.latin1(),
                item->m_desc ? item->m_desc->latin1() : "MISSING" );
        }
    }
#else
    // From the dictionary
    fprintf( fptr, "\n" );
    QDictIterator<EqVarItemList> it( *m_itemListDict );
    while( it.current() )
    {
        list = (EqVarItemList *) it.current();
        fprintf( fptr, "\n%s (%s)\n",
            it.currentKey().latin1(),
            list->m_name.latin1() );
        for ( item = list->first();
              item != 0;
              item= list->next() )
        {
            fprintf( fptr, "        %s %4d %s [%s]\n",
                item->m_sort.latin1(),
                item->m_index,
                item->m_name.latin1(),
                item->m_desc ? item->m_desc->latin1() : "MISSING" );
        }
        ++it;
    }
#endif
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local m_funDict function dictionary.
 */

void EqTree::printFunDict( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_funDict Function Dictionary\n" );
    fprintf( fptr, "Count=%d, Size=%d\n",
        m_funDict->count(), m_funDict->size() );
    QDictIterator<EqFun> it( *m_funDict );
    EqFun *funPtr;
    while( it.current() )
    {
        funPtr = (EqFun *) it.current();
        funPtr->print( fptr );
        ++it;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local m_fun[] function array.
 */

void EqTree::printFunctions( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_fun[] Function Array\n" );
    fprintf( fptr, "There are %d functions:\n", m_funCount );
    for ( int id = 0;
          id < m_funCount;
          id++ )
    {
        m_fun[id]->print( fptr );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the names of all the EqVars required as inputs
 *  (stored in the leaf[] array) for the current configuration.
 */

void EqTree::printInputs( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_leafCount[] Variable Array\n" );
    fprintf( fptr, "There are %d leaf variables:\n", m_leafCount );
    EqVar *varPtr;
    int id, l, wName, wNative, wDisplay, wLabel;
    // Get string lengths for column widths.
    wName = wNative = wDisplay = wLabel = 0;
    for ( id = 0;
          id < m_leafCount;
          id++ )
    {
        varPtr = m_leaf[id];
        if ( ( l = strlen( varPtr->m_name.latin1() ) ) > wName )
            wName = l;
        if ( ( l = strlen( varPtr->m_nativeUnits.latin1() ) ) > wNative )
            wNative = l;
        if ( ( l = strlen( varPtr->m_displayUnits.latin1() ) ) > wDisplay )
            wDisplay = l;
        if ( varPtr->m_label
         && ( l = strlen( varPtr->m_label->latin1() ) ) > wLabel )
            wLabel = l;
    }
    // Print the inputs table.
    fprintf( fptr, "\nRequired Inputs (%d):\n", m_leafCount );
    for ( id = 0;
          id < m_leafCount;
          id++ )
    {
        varPtr = m_leaf[id];
        if ( varPtr->isDiscrete() )
        {
            fprintf( fptr, "  %02d [%s]: %-*s [%-*s] %8d %s %s\n",
                id, varPtr->m_inpOrder.latin1(), wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->activeItemDataIndex(),
                varPtr->activeItemName().latin1(),
                ( varPtr->m_isMasked ? "HIDDEN" : "" ) );
        }
        else if ( varPtr->isContinuous() )
        {
            fprintf( fptr,
                "  %02d [%s]: %-*s [%-*s] %8.*f %-*s %8.*f %-*s %s (ntv)\n",
                id,  varPtr->m_inpOrder.latin1(), wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->m_displayDecimals, varPtr->m_displayValue,
                wDisplay, varPtr->m_displayUnits.latin1(),
                varPtr->m_nativeDecimals, varPtr->m_nativeValue,
                wNative, varPtr->m_nativeUnits.latin1(),
                ( varPtr->m_isMasked ? "HIDDEN" : "" ) );
        }
        else if ( varPtr->isText() )
        {
            fprintf( fptr, "  %02d [%s]: %-*s [%-*s] [%s]\n",
                id, varPtr->m_inpOrder.latin1(), wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->m_store.latin1() );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the names of all the EqVars requested as outputs
 *  (stored in the root[] array) for the current configuration.
 */

void EqTree::printOutputs( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_rootCount[] Variable Array\n" );
    fprintf( fptr, "There are %d root variables:\n", m_rootCount );
    EqVar *varPtr;
    int id, l, wName, wNative, wDisplay, wLabel;
    // Get string lengths for column widths.
    wName = wNative = wDisplay = wLabel = 0;
    for ( id = 0;
          id < m_rootCount;
          id++ )
    {
        varPtr = m_root[id];
        if ( ( l = strlen( varPtr->m_name.latin1() ) ) > wName )
            wName = l;
        if ( ( l = strlen( varPtr->m_nativeUnits.latin1() ) ) > wNative )
            wNative = l;
        if ( ( l = strlen( varPtr->m_displayUnits.latin1() ) ) > wDisplay )
            wDisplay = l;
        if ( varPtr->m_label
         && ( l = strlen( varPtr->m_label->latin1() ) ) > wLabel )
            wLabel = l;
    }
    // Print the output table.
    fprintf( fptr, "\nRequested Outputs (%d):\n", m_rootCount );
    for ( id = 0;
          id < m_rootCount;
          id++ )
    {
        varPtr = m_root[id];
        if ( varPtr->isDiscrete() )
        {
            fprintf( fptr, "  %02d [%s]: %-*s [%-*s] %10d %s\n",
                id,  varPtr->m_outOrder.latin1(), wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->activeItemDataIndex(),
                varPtr->activeItemName().latin1() );
        }
        else if ( varPtr->isContinuous() )
        {
            fprintf( fptr, "  %02d [%s]: %-*s [%-*s] %10.*f %-*s %10.*f %-*s (ntv)\n",
                id,  varPtr->m_outOrder.latin1(), wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->m_displayDecimals, varPtr->m_displayValue,
                wDisplay, varPtr->m_displayUnits.latin1(),
                varPtr->m_nativeDecimals, varPtr->m_nativeValue,
                wNative, varPtr->m_nativeUnits.latin1() );
        }
        else if ( varPtr->isText() )
        {
            fprintf(fptr, "  %02d [%s]: %-*s [%-*s] [%s]\n",
                id, varPtr->m_outOrder.latin1(),wName, varPtr->m_name.latin1(),
                wLabel, ( varPtr->m_label ? varPtr->m_label->latin1() : "NONE" ),
                varPtr->m_store.latin1() );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints Pat's table of variable names, modules, and units.
 */

void EqTree::printPatsTable( FILE *fptr, bool all ) const
{
    // Create an array of pointers for the variables
    EqVar **var = new EqVar *[ m_varCount ];
    checkmem( __FILE__, __LINE__, var, "EqVar *var", m_varCount );

    // Copy them over and get max string length
    int inpGroup, outGroup, inpActive, outActive, id, thisVar, pos;
    int maxLen = 0;
    for (  id = 0;
           id < m_varCount;
           id++
        )
    {
        var[id] = m_var[id];
        // Check length of i/o variables only
        sscanf( var[id]->m_inpOrder, "%d:%d:%d",
            &inpGroup, &thisVar, &inpActive );
        sscanf( var[id]->m_outOrder, "%d:%d:%d",
            &outGroup, &thisVar, &outActive );
        if ( ! all && ! inpActive && ! outActive )
        {
            continue;
        }
        if ( inpGroup <= 7 || outGroup <= 8 )
        {
            if ( (int) strlen( var[id]->m_label->latin1() ) > maxLen )
            {
                maxLen = strlen( var[id]->m_label->latin1() );
            }
        }
    }
    // Sort the array by input order
    qsort( var, m_varCount, sizeof(EqVar *), EqTree_InpOrderCompare );

    // Worksheet input section names from BpDocument::composeWorksheet()
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "INPUT VARIABLES & MODULES\n\n" );
    QString Group[8];
    Group[0] = "DOCUMENTATION";
    translate( Group[1], "BpDocument:Worksheet:Group:Fuel" );
    translate( Group[2], "BpDocument:Worksheet:Group:Moisture" );
    translate( Group[3], "BpDocument:Worksheet:Group:Weather" );
    translate( Group[4], "BpDocument:Worksheet:Group:Terrain" );
    translate( Group[5], "BpDocument:Worksheet:Group:Fire" );
    translate( Group[6], "BpDocument:Worksheet:Group:Map" );
    translate( Group[7], "BpDocument:Worksheet:Group:Suppression" );
    QString label(""), moduleList(""), choices("");
    int lastGroup = -1;
    // Print in worksheet order
    for (  id = 0;
           id < m_varCount;
           id++
        )
    {
        // Scan the variable's input section index from m_sortIn
        sscanf( var[id]->m_inpOrder, "%d:%d:%d",
            &inpGroup, &thisVar, &inpActive );
        if ( inpGroup > 7
          || ( ! all && ! inpActive ) )
        {
            continue;
        }
        // Do we need a new input section heading?
        if ( inpGroup != lastGroup )
        {
            lastGroup = inpGroup;
            fprintf( fptr,
                "\n%02d: %-*.*s     From - Thru [SU SI CO SP SC MO IG RH] Range Units\n",
                inpGroup, maxLen, maxLen,
                Group[inpGroup].latin1() );
        }
        // Replace any newlines with spaces
        label = *(var[id]->m_label);
        while ( ( pos = label.find( '\n' ) ) >= 0 )
        {
            label = label.replace( pos, 1, " " );
        }
        // Get the list of modules for this variable
        variableModuleList( var[id], moduleList );
        // Display the variable sortIn, label, releases, display units,
        // display decimals, and module io
        fprintf( fptr, "    %03d %-*.*s %d-%d [%s]",
            thisVar,
            maxLen, maxLen, label.latin1(),
            var[id]->m_releaseFrom,
            var[id]->m_releaseThru,
            moduleList.latin1() );
        if ( var[id]->isContinuous() )
        {
            fprintf( fptr, " %1.*f - %1.*f %s\n",
            var[id]->m_displayDecimals,
            var[id]->m_displayMinimum,
            var[id]->m_displayDecimals,
            var[id]->m_displayMaximum,
            var[id]->m_displayUnits.latin1() );
        }
        else if ( var[id]->isDiscrete() )
        {
            choices = QString( "%1: " ).arg( var[id]->m_itemList->count() );
            for ( int iid = 0;
                  iid < (int) var[id]->m_itemList->count();
                  iid++ )
            {
                choices += var[id]->m_itemList->itemName(iid) + " ";
            }
            fprintf( fptr, " %s\n", choices.latin1() );
        }
        else
        {
            fprintf( fptr, "\n" );
        }
    }

    // Sort the array by output order
    qsort( var, m_varCount, sizeof(EqVar *), EqTree_OutOrderCompare );
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "OUTPUT VARIABLES & MODULES\n\n" );
    // Module names from BpDocument::composeWorksheet()
    QString Module[9];
    Module[0] = "Documentation";
    translate( Module[1], "BpDocument:Module:Surface" );
    translate( Module[2], "BpDocument:Module:Size" );
    translate( Module[3], "BpDocument:Module:Contain" );
    translate( Module[4], "BpDocument:Module:Spot" );
    translate( Module[5], "BpDocument:Module:Scorch" );
    translate( Module[6], "BpDocument:Module:Mortality" );
    translate( Module[7], "BpDocument:Module:Ignition" );
    translate( Module[8], "BpDocument:Module:Weather" );
    // Print in output order
    lastGroup = -1;
    for (  id = 0;
           id < m_varCount;
           id++
        )
    {
        // Scan the variable's input section index from m_sortIn
        sscanf( var[id]->m_outOrder, "%d:%d:%d",
            &outGroup, &thisVar, &outActive );
        if ( outGroup > 8
          || ( ! all && ! outActive ) )
        {
            continue;
        }
        // Do we need a new input section heading?
        if ( outGroup != lastGroup )
        {
            lastGroup = outGroup;
            fprintf( fptr, "\n%02d: %-*.*s     From - Thru Units (Dec)\n",
                outGroup,
                maxLen, maxLen, Module[outGroup].latin1() );
        }
        // Replace any newlines with spaces
        label = *(var[id]->m_label);
        while ( ( pos = label.find( '\n' ) ) >= 0 )
        {
            label = label.replace( pos, 1, " " );
        }
        // Display the variable name and label
        fprintf( fptr, "    %03d %-*.*s %d-%d",
            thisVar, maxLen, maxLen,
            label.latin1(),
            var[id]->m_releaseFrom,
            var[id]->m_releaseThru );
        if ( var[id]->isContinuous() )
        {
            fprintf( fptr, " %s (%d)\n",
                var[id]->m_displayUnits.latin1(),
                var[id]->m_displayDecimals );
        }
        else if ( var[id]->isDiscrete() )
        {
            choices = QString( "%1: " ).arg( var[id]->m_itemList->count() );
            for ( int iid = 0;
                  iid < (int) var[id]->m_itemList->count();
                  iid++ )
            {
                choices += var[id]->m_itemList->itemName(iid) + " ";
            }
            fprintf( fptr, " %s\n", choices.latin1() );
        }
        else
        {
            fprintf( fptr, "\n" );
        }
    }
    // Release resources and retrun
    delete[] var;   var = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Builds a readable text list of all modules consuming or
 *  producing the specified variable.
 *
 *  The string looks like this:
 *  "IO IO IO IO IO IO IO IO"
 *  where "I" appears for every module that consumers the variable,
 *  "O" appears for every module that produces the variable,
 *  " " appears where it is not consumed or produced,
 *  and a single space separates each module.
 */

void EqTree::variableModuleList( EqVar *varPtr, QString &str ) const
{
    static QString ModName[8] =
    {
        "surface", "size", "contain", "spot",
        "scorch", "mortality", "ignition", "weather"
    };
    int id, mid;
    str = ".. .. .. .. .. .. .. ..";
    // Examine all functions
    for ( int fid = 0;
          fid < m_funCount;
          fid++ )
    {
        // Does the function use this variable as input?
        for ( id = 0;
              id <= m_fun[fid]->m_inputs;
              id++
            )
        {
            // Does this function input variable match our variable?
            if ( m_fun[fid]->m_input[id] == varPtr )
            {
                // Yep! - set the "I"
                for ( mid = 0;
                      mid < 8;
                      mid++
                    )
                {
                    // Which module is this?
                    if ( m_fun[fid]->m_module == ModName[mid] )
                    {
                        str = str.replace( mid*3, 1, "I" );
                        break;
                    }
                }
            }
        }
        // Does the function use this variable as output?
        for ( id = 0;
              id <= m_fun[fid]->m_outputs;
              id++
            )
        {
            // Does this function output variable match our variable?
            if ( m_fun[fid]->m_output[id] == varPtr )
            {
                // Yep! - set the "O"
                for ( mid = 0;
                      mid < 8;
                      mid++
                    )
                {
                    // Which module is this?
                    if ( m_fun[fid]->m_module == ModName[mid] )
                    {
                        str = str.replace( mid*3+1, 1, "O" );
                        break;
                    }
                }
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local property dictionary.
 */

void EqTree::printPropertyDict( FILE *fptr ) const
{
    static const QString Type[] = { "None", "Boolean", "Color", "Integer",
        "Real", "String", "Unknown" };
    // Print the dictionary in alphabetical order
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_propDict Property Dictionary\n" );
    fprintf( fptr, "Count=%d, Size=%d\n",
        m_propDict->count(), m_propDict->size() );
    QStringList propList;
    QDictIterator<Property> it( *m_propDict );
    Property *prop;
    QString str;
    while( it.current() )
    {
        prop = (Property *) it.current();
        str = it.currentKey().leftJustify( 41, ' ' )
            + Type[prop->m_type].leftJustify( 8, ' ' )
            + "[" + prop->m_value + "]";
        propList.append( str );
        ++it;
    }
    // Sort the list and print it out
    propList.sort();
    fprintf( fptr, "\n---------------------------------------- -------  --------\n" );
    fprintf( fptr, "%-40.40s Type     [Value]\n", "Property" );
    fprintf( fptr, "---------------------------------------- -------  --------\n" );
    for ( QStringList::Iterator sit=propList.begin();
          sit != propList.end();
          ++sit )
    {
        fprintf( fptr, "%s\n", (*sit).latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the current results table.
 */

void EqTree::printResults( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree Results\n" );
    if ( m_rangeVars == 0 )
    {
        printResults0( fptr );
    }
    else if ( m_rangeVars == 1 )
    {
        printResults1( fptr );
    }
    else if ( m_rangeVars == 2 )
    {
        printResults2( fptr );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the results table for 0 range input variables.
 */

void EqTree::printResults0( FILE *fptr ) const
{
    // Get string lengths for column widths.
    int var, l, wDisplay, wLabel;
    wDisplay = wLabel = 0;
    EqVar *varPtr;
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        if ( ( l = strlen( varPtr->m_displayUnits.latin1() ) ) > wDisplay )
        {
            wDisplay = l;
        }
        if ( varPtr->m_label )
        {
            if ( ( l = strlen( varPtr->m_label->latin1() ) ) > wLabel )
            {
                wLabel = l;
            }
        }
        else
        {
            if ( ( l = strlen( varPtr->m_name.latin1() ) ) > wLabel )
            {
                wLabel = l;
            }
        }
    }
    // Display a little table
    double value;
    int iid;
    QString name("");
    QString Dash("-------------------------------------------------------" );
    fprintf( fptr, "\n%-*.*s         Value  Units\n%-*.*s    %-10.10s  %-*.*s\n",
        wLabel, wLabel, "Variable",
        wLabel, wLabel, Dash.latin1(),
        Dash.latin1(),
        wDisplay, wDisplay, Dash.latin1() );
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        value = getResult( 0, 0, var );
        if ( varPtr->isContinuous() )
        {
            fprintf( fptr, "%-*s    %10.*f  %s\n",
                wLabel,
                ( varPtr->m_label ? varPtr->m_label->latin1() : varPtr->m_name.latin1() ),
                varPtr->m_displayDecimals,
                value,
                varPtr->m_displayUnits.latin1() );
        }
        else if ( varPtr->isDiscrete() )
        {
            iid = (int) value;
            name = varPtr->getItemName( iid );
            fprintf( fptr, "%-*s    %10d  %s\n",
                wLabel,
                ( varPtr->m_label ? varPtr->m_label->latin1() : varPtr->m_name.latin1() ),
                iid, name.latin1() );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the results table for 1 range input variables.
 */

void EqTree::printResults1( FILE *fptr ) const
{
    int row, var;
    EqVar *rowPtr = m_rangeVar[0];
    EqVar *varPtr;
    // Header
    fprintf( fptr, "\n%10.10s", rowPtr->m_hdr0->latin1() );
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        fprintf( fptr, "%10.10s", varPtr->m_hdr0->latin1() );
    }
    fprintf( fptr, "\n%10.10s", rowPtr->m_hdr1->latin1() );
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        fprintf( fptr, "%10.10s", varPtr->m_hdr1->latin1() );
    }
    fprintf( fptr, "\n%10.10s",
        rowPtr->isContinuous() ? rowPtr->m_displayUnits.latin1() : "" );
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        fprintf( fptr, "%10.10s",
            varPtr->isContinuous() ? varPtr->m_displayUnits.latin1() : "" );
    }
    fprintf( fptr, "\n----------" );
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        fprintf( fptr, "  --------" );
    }
    fprintf( fptr, "\n" );

    // Print results
    double value;
    QString name("");
    int iid;
    // Loop for each row
    for ( row = 0;
          row < m_tableRows;
          row++ )
    {
        value = m_tableRow[row];
        if ( rowPtr->isContinuous() )
        {
            fprintf( fptr, "%10.*f", rowPtr->m_displayDecimals, value );
        }
        else if ( rowPtr->isDiscrete() )
        {
            iid = (int) m_tableRow[row];
            name = rowPtr->getItemName( iid );
            fprintf( fptr, "%10.10s", name.latin1() );
        }
        // Loop for each variable
        for ( var = 0;
              var < m_tableVars;
              var++ )
        {
            varPtr = m_tableVar[var];
            value = getResult( row, 0, var );
            if ( varPtr->isContinuous() )
            {
                fprintf( fptr, "%10.*f", varPtr->m_displayDecimals, value );
            }
            else if ( varPtr->isDiscrete() )
            {
                iid = (int) value;
                name = varPtr->getItemName( iid );
                fprintf( fptr, "%10.10s", name.latin1() );
            }
        }
        fprintf( fptr, "\n" );
    }   // Next row
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the results table for 2 range input variables.
 */

void EqTree::printResults2( FILE *fptr ) const
{
    EqVar *rowPtr = m_rangeVar[0];
    EqVar *colPtr = m_rangeVar[1];
    EqVar *varPtr;
    int    row, col, var, iid;
    double value;
    QString name("");
    // Separate table for each output variable
    for ( var = 0;
          var < m_tableVars;
          var++ )
    {
        varPtr = m_tableVar[var];
        if ( varPtr->isContinuous() )
        {
            fprintf( fptr, "\n\n%s (%s):\n",
                varPtr->m_label->latin1(), varPtr->m_displayUnits.latin1() );
        }
        else
        {
            fprintf( fptr, "\n\n%s:\n", varPtr->m_label->latin1() );
        }

        // Header
        fprintf( fptr, "%10.10s\n%10.10s",
            rowPtr->m_hdr0->latin1(), rowPtr->m_hdr1->latin1() );
        fprintf( fptr, "    %s (%s)\n",
            colPtr->m_label->latin1(), colPtr->m_displayUnits.latin1() );
        fprintf( fptr, "%10.10s", rowPtr->m_displayUnits.latin1() );
        for ( col = 0;
              col < m_tableCols;
              col++ )
        {
            value = m_tableCol[col];
            if ( colPtr->isContinuous() )
            {
                fprintf( fptr, "%10.*f", colPtr->m_displayDecimals, value );
            }
            else if ( colPtr->isDiscrete() )
            {
                iid = (int) value;
                name = colPtr->getItemName( iid );
                fprintf( fptr, "%-10.10s", name.latin1() );
            }
        }
        fprintf( fptr, "\n----------" );
        for ( col = 0;
              col < m_tableCols;
              col++ )
        {
            fprintf( fptr, "  --------" );
        }
        fprintf( fptr, "\n" );

        // Loop for each row
        for ( row = 0;
              row < m_tableRows;
              row++ )
        {
            value = m_tableRow[row];
            if ( rowPtr->isContinuous() )
            {
                fprintf( fptr, "%10.*f", rowPtr->m_displayDecimals, value );
            }
            else if ( rowPtr->isDiscrete() )
            {
                iid = (int) m_tableRow[row];
                name = rowPtr->getItemName( iid );
                fprintf( fptr, "%10.10s", name.latin1() );
            }
            // Loop for each column
            for ( col = 0;
                  col < m_tableCols;
                  col++ )
            {
                value = getResult( row, col, var );
                if ( varPtr->isContinuous() )
                {
                    fprintf( fptr, "%10.*f", varPtr->m_displayDecimals, value );
                }
                else if ( varPtr->isDiscrete() )
                {
                    iid = (int) value;
                    name = varPtr->getItemName( iid );
                    fprintf( fptr, "%10.10s", name.latin1() );
                }
            }
            fprintf( fptr, "\n" );
        }   // Next row
    }   // Next variable
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints a summary of the EqTree's resources.
 */

void EqTree::printSummary( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree Resource Summary\n" );

    fprintf( fptr, "\nDictionary Usage\n" );
    fprintf( fptr, "%-12s   Count    Size\n", "Dictionary" );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Function", m_funDict->count(), m_funDict->size() );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Variable", m_varDict->count(), m_varDict->size() );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Property", m_propDict->count(), m_propDict->size() );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Translator", appTranslator()->count(), appTranslator()->size() );
    // Count number of variables used by this release
    int id;
    int varCount = 0;
    for ( id = 0;
          id < m_varCount;
          id++ )
    {
        if ( m_var[id]->isCurrent( appWindow()->m_release ) )
        {
            varCount++;
        }
    }
    // Count number of functions used by this release
    int funCount = 0;
    for ( id = 0;
          id < m_funCount;
          id++ )
    {
        if ( m_fun[id]->isCurrent( appWindow()->m_release ) )
        {
            funCount++;
        }
    }
    // Count number of properties used by this release
    QDictIterator<Property> it( *m_propDict );
    Property *prop;
    int propCount = 0;
    while( it.current() )
    {
        prop = (Property *) it.current();
        if ( prop->isCurrent( appWindow()->m_release ) )
        {
            propCount++;
        }
        ++it;
    }
    // Display counts
    fprintf( fptr, "\nRelease Usage\n" );
    fprintf( fptr, "%-12s   %05d   Total\n",
        "Array", appWindow()->m_release );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Function", funCount, m_funCount );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Variable", varCount, m_varCount );
    fprintf( fptr, "%-12s %7d %7d\n",
        "Property", propCount, m_propDict->count() );

    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local m_varDict variable dictionary.
 */

void EqTree::printVarCsv( FILE *fptr ) const
{
    QDictIterator<EqVar> it( *m_varDict );
    EqVar *v;
    while( it.current() )
    {
        v = (EqVar *) it.current();
		fprintf( fptr, "\"%s\",\"6\",\"'%s\",\"'%s\",\"%s\",\"%s\",\"%s\",",
			v->m_name.latin1(),
			v->m_inpOrder.latin1(),
			v->m_outOrder.latin1(),
			v->m_label ? v->m_label->latin1() : "NONE",
			v->m_hdr0  ? v->m_hdr0->latin1()  : "NONE",
			v->m_hdr1  ? v->m_hdr1->latin1()  : "NONE" );
		// Display continuous variable parameters
		if ( v->isContinuous() )
		{
			fprintf( fptr, "\"%s\",%f,%f,",
				v->m_nativeUnits.latin1(),
				v->m_nativeMinimum,
				v->m_nativeMaximum );
		}
	    // Display discrete variable values
		else if ( v->isDiscrete() )
		{
			fprintf( fptr, "discrete,,," );
	    }
		// Display text variable values
		else if ( v->isText() )
		{
			fprintf( fptr, "text,,," );
	    }
		fprintf( fptr, "\"%s\",\"%s\"\n",
			v->m_help.latin1(),
			v->m_desc ? v->m_desc->latin1() : "NONE" );
        ++it;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local m_varDict variable dictionary.
 */

void EqTree::printVarDict( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_varDict Variable Dictionary\n" );
    fprintf( fptr, "Count=%d, Size=%d\n",
        m_varDict->count(), m_varDict->size() );
    QDictIterator<EqVar> it( *m_varDict );
    EqVar *varPtr;
    while( it.current() )
    {
        varPtr = (EqVar *) it.current();
        varPtr->print( fptr );
        ++it;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqTree's local m_var[] variable array.
 */

void EqTree::printVariables( FILE *fptr ) const
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "EqTree m_var[] Variable Array\n" );
    fprintf( fptr, "There are %d variables:\n", m_varCount );
    int id;
    int maxLen = 0;
    for ( id = 0; id < m_varCount; id++ )
    {
        if ( (int) strlen( m_var[id]->m_name.latin1() ) > maxLen )
        {
            maxLen = strlen( m_var[id]->m_name.latin1() );
        }
    }
    for ( id = 0; id < m_varCount; id++ )
    {
        fprintf( fptr, "%-*.*s %s %s %s\n",
            maxLen, maxLen,
            m_var[id]->m_name.latin1(),
            m_var[id]->m_inpOrder.latin1(),
            m_var[id]->m_outOrder.latin1(),
            m_var[id]->m_label->latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqVar's parameters to the file stream.
 */

void EqVar::print( FILE *fptr ) const
{
    // Display variable's name, label, description, and headers
    fprintf( fptr, "\n%s (%lu)\n",
        m_name.latin1(), (unsigned long) this );
    fprintf( fptr, "    Label   : %s\n",
        m_label ? m_label->latin1() : "NONE" );
    fprintf( fptr, "    Headers : %s\n              %s\n",
        m_hdr0 ? m_hdr0->latin1() : "NONE",
        m_hdr1 ? m_hdr1->latin1() : "NONE" );
    fprintf( fptr, "    Desc    : %s\n",
        m_desc ? m_desc->latin1() : "NONE" );
    fprintf( fptr, "    Store   : [%s] (%d tokens)\n",
        m_store.latin1(), m_tokens );
    // Display the variable's flags
    fprintf( fptr, "    Flags   :%s%s%s%s\n",
        ( m_isUserInput ) ?  " INPUT" : "",
        ( m_isUserOutput ) ? " OUTPUT" : "",
        ( m_isConstant ) ?   " CONSTANT" : "",
        ( m_isMasked ) ?     " MASKED" : "" );
    // Display continuous variable parameters
    if ( isContinuous() )
    {
        fprintf( fptr, "    Range   : %1.*f - %1.*f %s\n",
            m_nativeDecimals, m_nativeMinimum,
            m_nativeDecimals, m_nativeMaximum, m_nativeUnits.latin1() );
        fprintf( fptr, "    Default : %1.*f %s\n",
            m_nativeDecimals, m_defaultValue, m_nativeUnits.latin1() );
        fprintf( fptr, "    Native  : %1.*f %s\n",
            m_nativeDecimals, m_nativeValue, m_nativeUnits.latin1() );
        fprintf( fptr, "    Display : %1.*f %s\n",
            m_displayDecimals, m_displayValue, m_displayUnits.latin1() );
        fprintf( fptr, "    English : %s (%d decimals)\n",
            m_englishUnits.latin1(), m_englishDecimals );
        fprintf( fptr, "    Metric  : %s (%d decimals)\n",
            m_metricUnits.latin1(), m_metricDecimals );
    }
    // Display discrete variable values
    else if ( isDiscrete() )
    {
        fprintf( fptr, "    Item    : (%d) %s [%s]\n",
            activeItemDataIndex(),
            activeItemName().latin1(),
            activeItemDesc() ? activeItemDesc()->latin1() : "" );
    }
    // Display text variable values
    else if ( isText() )
    {
        fprintf( fptr, "    Text    : %s\n",
            m_store.latin1() );
    }
    // Display producer functions.
    EqFun *funPtr;
    fprintf( fptr, "    Producer: %d\n", m_producers );
    for ( int pid = 0;
          pid < m_producers;
          pid++ )
    {
        funPtr = m_producer[pid];
        fprintf( fptr, "    %7d : %s (%lu)\n",
            pid, funPtr->m_name.latin1(), (unsigned long) funPtr );
    }
    // Display consumer functions.
    fprintf( fptr, "    Consumer: %d\n", m_consumers );
    for ( int cid = 0;
          cid < m_consumers;
          cid++ )
    {
        funPtr = m_consumer[cid];
        fprintf( fptr, "    %7d : %s (%lu)\n",
            cid, funPtr->m_name.latin1(), (unsigned long) funPtr );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the EqApp's shared translation dictionary.
 */

void printTranslationDict( FILE *fptr )
{
    fprintf( fptr, "\n-------------------------------------------------\n" );
    fprintf( fptr, "Global Translation Dictionary\n" );
    fprintf( fptr, "Count=%d, Size=%d\n",
        appTranslator()->count(), appTranslator()->size() );
    // Print the dictionary in alphabetical order
    QStringList dictList;
    QDictIterator<QString> it( *appTranslator() );
    QString *text, str;
    while( it.current() )
    {
        text = (QString *) it.current();
        str = it.currentKey().leftJustify( 52, ' ' ) + "[" + *text + "]";
        dictList.append( str );
        ++it;
    }
    // Sort the list and print it out
    dictList.sort();
    fprintf( fptr, "\n--------------------------------------------------- ------\n" );
    fprintf( fptr, "%-52.52s [Text]\n", "Key" );
    fprintf( fptr, "--------------------------------------------------- ------\n" );
    for ( QStringList::Iterator sit=dictList.begin();
          sit != dictList.end();
          ++sit )
    {
        fprintf( fptr, "%s\n", (*sit).latin1() );
    }
    return;
}

#endif // #ifdef XEQTREE_PRINT_SUPPORT

//------------------------------------------------------------------------------
//  End of xeqtree.cpp
//------------------------------------------------------------------------------

