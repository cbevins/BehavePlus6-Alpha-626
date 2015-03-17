//------------------------------------------------------------------------------
/*! \file bpfile.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class file i/o methods.
 *
 *  This file contains those file i/o methods called by the AppWindow class
 *  and those re-implemented from Document class.
 *
 *  Additional BpDocument method definitions are in:
 *      - bpcomposefiredirdiagram.cpp
 *      - bpcomposefireshapediagram.cpp
 *      - bpcomposegraphs.cpp
 *      - bpcomposelogo.cpp
 *      - bpcomposepage.cpp
 *      - bpcomposetable1.cpp
 *      - bpcomposetable2.cpp
 *      - bpcomposetable3.cpp
 *      - bpcomposeworksheet.cpp
 *      - bpdocentry.cpp
 *      - bpdocument.cpp
 */

// Custom include files
#include "appdialog.h"
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocentry.h"
#include "bpdocument.h"
#include "docscrollview.h"
#include "fileselector.h"
#include "fuelexportdialog.h"
#include "fuelmodel.h"
#include "parser.h"
#include "property.h"
#include "xeqapp.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qimage.h>

//------------------------------------------------------------------------------
/*! \brief Captures the current display page to an image file.
 *
 *  Called by ApplicationWindow::slotDocumentCapture() when the main menu
 *  \b File->Capture is selected or when the Document::contextMenuActivated()
 *  ContextCapture action is invoked.
 *
 *  Supports the following output formats:
 *  \arg BMP
 *  \arg JPEG
 *  \arg PBM
 *  \arg PGM
 *  \arg PNG
 *  \arg PPM
 *  \arg XBM
 *  \arg XPM
 *
 *  \attention This is a non-virtual function that should NOT be re-implemented
 *  in any derived class.
 *
 *  \return TRUE on success, FALSE on failure.
 */

bool BpDocument::capture( void )
{
    // Initialization
    QString fileName="";
    QString dirName = appFileSystem()->capturePath();
    // Request the fileName if one wasn't specified
    FileSaveAsDialog dialog(
        appWindow(),                // ApplicationWindow
        dirName,                    // subdirectory
        "Capture",                  // file type
        "*",                        // extension pattern
        "MyScreenCaptures",         // default folder
        fileName,                   // default file name
        "",                         // default description
        "captureDialog" );          // widget name
    // Display the file selection dialog
    if ( dialog.exec() != QDialog::Accepted )
    {
        return( false );
    }
    // Get the file name and format selection
    dialog.getFileSelection( fileName );
    const char *format = dialog.getFileFormat();

    // Force storage of entry values to the worksheet.
    // Removed because it strips off the output pages!!
    //regenerateWorksheet();

    // Save the file
    QImageIO iio;
    QImage   image;
    image = m_scrollView->m_backingPixmap;
    iio.setImage( image );
    iio.setFileName( fileName );
    iio.setFormat( format );
    if ( ! iio.write() )
    // This code block should never be executed!
    {
        QString text(""), l_caption("");
        translate( l_caption, "BpDocument:Capture:Caption" );
        translate( text, "BpDocument:Capture:Error", fileName, format );
        warn( l_caption, text );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Opens and displays a BpDocument file.
 *
 *  \param fileName Reference to the file's full path name.
 *  \param thisFileType Reference to a file type:
 *  \arg Native
 *  \arg Fuel Model
 *  \arg Moisture Scenario
 *  \arg Run
 *  \arg Units Set
 *  \arg Worksheet
 *
 *  Called only by ApplicationWindow::openDocument() and
 *  ApplicationWindow::slotToolsUnitsEditor().
 *
 *  \retval TRUE if the file is successfully opened.
 *  \retval FALSE if the file couldn't be opened.
 */

bool BpDocument::open( const QString &fileName, const QString &fileType )
{
    // Request a file name if one wasn't provided
    QString fileExt;
    QString openFileName = fileName;
    // Attempt to read and store the file contents.
    if ( ! m_eqTree->readXmlFile( openFileName ) )
    {
        return( false );
    }
    // Load the notes into the m_notes field.
    loadNotes();

    // Save the Document file path and base names.
    if ( fileType == "Run" )
    {
        QFileInfo fileInfo( openFileName );
        m_absPathName  = fileInfo.absFilePath();
        m_baseName = fileInfo.baseName();
    }
    else if ( fileType == "Worksheet" )
    {
        fileExt = appFileSystem()->runExt();
        openFileName = appFileSystem()->unNamedFile();
        m_baseName.sprintf( "%s%02d.%s",
            openFileName.latin1(), m_docId, fileExt.latin1() );
        // m_absPathName is purposefully left empty, so that when
        // ApplicationWindow::slotDocumentSave() calls
        // doc->save( doc->absPathName(), "Run" ), a FileSelectorDialog appears.
        m_absPathName = "";
    }
    // Reconfigure
    setCaption( m_baseName );
    setPageTabs();
    statusUpdate();
    configure();
    // Check the release and return
    releaseCheck();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Checks if the current BpDocument release preceeds the current
 *  application release; if so, an information dialog is displayed warning
 *  about new variables and their default units of measure.
 */

void BpDocument::releaseCheck( void ) const
{
    // Check if this is an old run/worksheet file
    if ( m_eqTree->m_release >= appWindow()->m_release )
    {
        return;
    }
    // Get a list of newer variables and their units/decimals
    QString varList =
        "<TABLE><TR><TH>Variable</TH><TH>Units</TH><TH>Decimals</TH></TR>";
    EqVar *var;
    for ( int id = 0;
          id < m_eqTree->m_varCount;
          id++ )
    {
        var = m_eqTree->m_var[id];
        if ( var->m_releaseFrom > m_eqTree->m_release
          && var->m_releaseFrom <= appWindow()->m_release )
        {
            varList += QString( "<TR><TD>%1</TD><TD>%2</TD><TD>%3</TD></TR>" )
                .arg( *(var->m_label) )
                .arg( var->m_displayUnits )
                .arg( var->m_displayDecimals );
        }
    }
    varList += "</TABLE>";

    // Display the warning
    QString appRelease(""), docRelease(""), text("");
    releaseString( appWindow()->m_release, appRelease );
    releaseString( m_eqTree->m_release, docRelease );
    //translate( text, "BpDocument:ReleaseCheck:Text",
    //    appRelease, docRelease, varList );
    translate( text, "BpDocument:ReleaseCheck:Text",
        appRelease, docRelease );
    info( text );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Writes the current BpDocument to a (possibly) new file
 *  location and updates its file name.
 *
 *  \param fileName Reference to the full file path name.
 *  \param fileType Reference to a file type:
 *      - Native
 *      - Run
 *      - Fuel Model
 *      - Moisture Scenario
 *      - Units Set
 *      - Worksheet
 *      - Export Fuel Farsite
 *      - Export Results Html
 *      - Export Results Spreadsheet
 *
 *  All the work is actually done by a method appropriate to the file type.
 *  Called by:
 *  - AppWindow::slotDocumentSave()
 *  - AppWindow::slotDocumentSaveAsFuelModel()
 *  - AppWindow::slotDocumentSaveAsMoistureScenario()
 *  - AppWindow::slotDocumentSaveAsRun()
 *  - AppWindow::slotDocumentSaveAsUnitsSet()
 *  - AppWindow::slotDocumentSaveAsWorksheet()
 */

void BpDocument::save( const QString &fileName, const QString &fileType )
{
    // Store the notes.
    storeNotes();

    // Call appropriate file type method.
    if ( fileType == "Native" || fileType == "Run" )
    {
        saveAsRunFile( fileName, false );
    }
    else if ( fileType == "RunClone" )
    {
        saveAsRunFile( fileName, true );
    }
    else if ( fileType == "Worksheet" )
    {
        saveAsWorksheetFile( fileName );
    }
    else if ( fileType == "Fuel Model" )
    {
        saveAsFuelModelFile( fileName );
    }
    else if ( fileType == "Export Fuel Farsite English" )
    {
        saveAsFuelModelExportFile( "Farsite English" );
    }
    else if ( fileType == "Export Fuel Farsite Metric" )
    {
        saveAsFuelModelExportFile( "Farsite Metric" );
    }
    else if ( fileType == "Export Results" )
    {
        saveResults( "" );
    }
    else if ( fileType == "Moisture Scenario" )
    {
        saveAsMoistureScenarioFile( fileName );
    }
    else if ( fileType == "Units Set" )
    {
        saveAsUnitsSetFile( fileName );
    }
    else
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "BpDocument:WrongFileType", fileType );
        warn( text );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the attached fuel models to a FARSITE export file.
 *
 *  \param fileType One of "Farsite English" or "Farsite Metric".
 */

void BpDocument::saveAsFuelModelExportFile( const QString &fileType )
{
	// Create and display the multi-fuel model export selection dialog
    FuelExportDialog dialog( this, "fuelExportDialog.html" );
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }

    // Get ALL the selected fuel models from the selection dialog
    QString dialogResult( "" );
    dialog.resultString( dialogResult );
	const char *dialogResultAsc = dialogResult.ascii();	// for debug purposes only
    Parser selectedModels( " \t,\"", "", "" );
    selectedModels.parse( dialogResult );

    // Request the export file name
    QString typeName, filter, l_caption, fileName, saved;
    translate( typeName, "BpDocument:SaveExportedFuelModel:Caption" );
    l_caption = "Save As " + typeName;
    filter  = typeName + " ( *.fmd )";
    bool more = true;
    while( more )
    {
        more = false;
        // Display the file dialog
        QFileDialog fd( this, "fuelModelExport", true );
        fd.setMode( QFileDialog::AnyFile );
        fd.setFilter( filter );
        fd.setCaption( l_caption );
        if ( fd.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the export file name
        fileName = fd.selectedFile();
        if ( fileName.isNull() || fileName.isEmpty() )
        {
            return;
        }
        // Guarantee a ".fmd" extension
        if ( fileName.right( 4 ) != ".fmd" )
        {
            fileName.append( ".fmd" );
        }
        // If the file exists, get permission to overwrite it.
        QFileInfo fi( fileName );
        if ( fi.exists() )
        {
            QString caption2, text;
            translate( caption2, "FileSelector:OverwriteFile:Caption" );
            translate( text, "FileSelector:OverwriteFile:Text",
                    typeName, fileName );
            if ( yesno( caption2, text ) == 0 )
            {
                more = true;
            }
        }
    }

	// Units conversion factors
	int    units         = ( fileType == "Farsite English" ) ? 0 : 1;
	const char *Units[2] = { "ENGLISH", "METRIC" };
	double fLoad[2]      = {     21.78,  48.8243};		// tons/ac, tonnes/ha
	double fSavr[2]      = {      1.00,   0.0328084 };	// ft2/ft3, cm2/cm3
	double fDepth[2]     = {      1.00,  30.48 };		// ft, cm
	double fMext[2]      = {    100.00, 100.00 };		// %, %
	double fHeat[2]      = {      1.00,   2.32779 };	// Btu/lb, kJ/kg

    // Write the export file
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "FuelExport:FileOpen:Error", fileName );
        warn( text );
        return;
    }

    // Write the file header
	fprintf( fptr, "%s\n", Units[units] );

	// Write each of the selected fuel models
 	const char *fmt = "%d %s %6.3f %6.3f %6.3f %6.3f %6.3f %s %d %d %d %6.3f %d %d %d      %s\n";
	QString fmCode;
    FuelModel *fmPtr;
    FuelModelList *fuelModelList = m_eqTree->m_eqApp->m_fuelModelList;
    for ( int i=0; i<selectedModels.tokens(); i++ )
    {
        fmCode = selectedModels.token(i);
		const char *fmCodeAsc = fmCode.ascii();		// for debug purposes only
        if ( ! ( fmPtr = fuelModelList->fuelModelByModelName( fmCode ) ) )
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "FuelExport:FindModelName:Error", fmCode );
            warn( text );
            return;
        }
        fprintf( fptr, fmt,
            fmPtr->m_number,
            fmPtr->m_name.latin1(),
            fmPtr->m_load1 * fLoad[units],
            fmPtr->m_load10 * fLoad[units],
            fmPtr->m_load100 * fLoad[units],
            fmPtr->m_loadHerb * fLoad[units],
            fmPtr->m_loadWood * fLoad[units],
            fmPtr->isStatic() ? "static" : "dynamic",
            (int) (fmPtr->m_savr1 * fSavr[units]),
            (int) (fmPtr->m_savrHerb * fSavr[units]),
            (int) (fmPtr->m_savrWood * fSavr[units]),
            fmPtr->m_depth * fDepth[units],
            (int) (fmPtr->m_mext * fMext[units]),
            (int) (fmPtr->m_heatDead * fHeat[units]),
            (int) ( fmPtr->m_heatLive * fHeat[units]),
            fmPtr->m_desc.stripWhiteSpace().latin1()
        );
    }
    fclose( fptr );

    // Confirm the file was written
    translate( saved, "BpDocument:SaveExportedFuelModel:Saved", fileName );
    info( saved );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current worksheet contents to a fuel model file
 *  and attaches it to the application.
 *
 *  Makes sure the user is doing fuel modeling and that a complete set of valid
 *  fuel model inputs are entered, otherwise an error message is displayed.
 *
 *  It then displays a FileSaveAsDialog to let the user enter a folder/file
 *  name.  The file is written and attached to the application so the
 *  fuel model can be selected from the guide and wizards.
 *
 *  Called only by save().
 *
 *  \param oldFileName Reference to the full file path name.
 *
 *  \par File SaveAs Dialog Behavior
 *
 *  The File->SaveAs->Fuel Model menu button displays a FileSaveAsDialog
 *  with the following behavior:
 *  -   The worksheet's fuel bed values are validated for existence and value.
 *  -   Dialog opens with 3 input fields initialized to
 *      -   Fuel Model Folder      [ MyFuelModels ],
 *      -   Fuel Model File        [ <blank> ],
 *      -   Fuel Model Description [ <current worksheet description> ].
 *  -   Single click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpm files and
 *      -   changes no input fields.
 *  -   Double click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpm files and
 *      -   changes the following input fields:
 *          -   Fuel Model Folder [ <selected subdirectory name> ].
 *  -   Single click on a file name
 *      -   highlights the file name and
 *      -   changes no input fields.
 *  -   Double click on a file name
 *      -   highlights the file name and
 *      -   changes the following input fields:
 *          -   Fuel Model Folder      [ <selected file's subdirectory> ],
 *          -   Fuel Model File        [ <selected file's name> ],
 *          -   Fuel Model Description [ <selected file's description> ].
 *  -   Saving the run to a new, non-existent file
 *      -   creates a new Fuel Model File[]
 *          in the current Fuel Model Folder[]
 *          with the current Fuel Model Description[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 *  -   Saving the run to an old, existing file
 *      -   displays a dialog to confirm the overwrite,
 *      -   overwrites the Fuel Model File[]
 *          in the FuelModel Folder[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 */

void BpDocument::saveAsFuelModelFile( const QString &oldFileName )
{
    // Must be doing fuel modeling!
    QString text("");
    if ( ! property()->boolean( "surfaceModuleActive" )
      || ! property()->boolean( "surfaceConfFuelParms" ) )
    {
        translate( text, "BpDocument:SaveFuelModel:SurfaceModuleInactive" );
        warn( text );
        return;
    }

	// Set default values
    QString newFileName = oldFileName;

	// Store all input texts, even those that haven't received an Enter.
    translate( text, "BpDocument:SaveFuelModel:MissingInputs" );
    text += "<UL>";
    bool errors = false;
    int lid, tokens, position, length;
    for ( lid = 0;
          lid < leafCount();
          lid++ )
    {
        if ( m_eqTree->m_eqCalc->isFuelModelVariable( leaf(lid) ) )
        {
            // Store the contents in the EqVar store
            leaf(lid)->m_store = m_entry[lid]->text();
            // Validate the store
            if ( ! leaf(lid)->isValidStore( &tokens, &position, &length ) )
            {
                // Invalid contents; reposition the focus and return NOW
                focusThis( lid, position, length );
                return;
            }
            // Must have some value entered
            if ( leaf(lid)->m_tokens < 1 )
            {
                text += QString( "<LI>%1</LI>" ).arg( *leaf(lid)->m_label );
                errors = true;
            }
			// If this is the fuel model code, store it as the file name
			if ( leaf(lid) == m_eqTree->m_eqCalc->vSurfaceFuelBedModelCode )
			{
				newFileName = leaf(lid)->m_store;
			}
        }
    }
    // Report missing entries.
    if ( errors )
    {
        text += "</UL>";
        error( text );
        return;
    }
    // Request the fileName if one wasn't specified
    QString oldDesc = m_eqTree->m_eqCalc->docDescriptionStore();
    QString newDesc = oldDesc;
	const char* c_oldFileName = oldFileName;
	const char* c_oldDesc = oldDesc;
	const char* c_newFileName = newFileName;
	const char* c_newDesc = newDesc;

    //if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        // Get the Fuel Model directory
        QString dirName = appFileSystem()->fuelModelPath();
        QString extName = appFileSystem()->fuelModelExt();
        // Create the file selection dialog
        FileSaveAsDialog dialog(
            appWindow(),                            // ApplicationWindow
            dirName,                                // subdirectory
            "Fuel Model",                           // file type name
            extName,                                // file extension
            "MyFuelModels",                         // default folder
            newFileName,                            // default file name
            oldDesc,								// default description
            "saveAsFuelModelFileDialog" );          // widget name
        // Display the file selection dialog.
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection
        dialog.getFileSelection( newFileName );
		c_newFileName = newFileName;
        // Get the (possibly new) file description
        dialog.getFileDescription( newDesc );
		c_newDesc = newDesc;
		// This was commented out to resolve Issue #077 in version 5.0.5 Build 306.
		// NOTE - This still picks up an 'old' description from the worksheet
		// if the user has entered a new description without hitting Tab/Return
		// or making a calculation (which forces the store/update).
//      if ( ! newDesc.isNull() && ! newDesc.isEmpty() && newDesc != oldDesc )
//      {
            m_entry[0]->setText( newDesc );
            m_eqTree->m_eqCalc->docDescriptionStore( newDesc );
            property()->string( "appDescription", newDesc );
//      }
    }
    // Write the file
    if ( m_eqTree->writeXmlFile( newFileName, "BehavePlus", "Fuel Model",
        appWindow()->m_release ) )
    {
        // Attach the fuel model file
        m_eqApp->attachFuelModel( newFileName );
        // Display a confirmation
        translate( text, "BpDocument:SaveFuelModel:Saved", newFileName );
        info( text );
    }
    // Restore the original description.
    if ( newDesc != oldDesc )
    {
        m_entry[0]->setText( oldDesc );
        m_eqTree->m_eqCalc->docDescriptionStore( oldDesc );
        property()->string( "appDescription", oldDesc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current worksheet contents to a moisture scenario file
 *  and attaches it to the application.
 *
 *  Makes sure the user is doing moisture modeling and that a complete set of
 *  valid moisture inputs are entered, otherwise an error message is displayed.
 *
 *  It then displays a FileSaveAsDialog to let the user enter a folder/file
 *  name.  The file is written and attached to the application so the
 *  moisture scenario can be selected from the guide and wizards.
 *
 *  Called only by save().
 *
 *  \param fileName Reference to the full file path name.
 *
 *  \par File SaveAs Dialog Behavior
 *
 *  The File->SaveAs->MoistureScenario menu button displays a FileSaveAsDialog
 *  with the following behavior:
 *  -   The worksheet's moisture values are validated for existence and value.
 *  -   Dialog opens with 3 input fields initialized to
 *      -   Moisture Scenario Folder      [ MyMoistureScenarios ],
 *      -   Moisture Scenario File        [ <blank> ],
 *      -   Moisture Scenario Description [ <current worksheet description> ].
 *  -   Single click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpm files and
 *      -   changes no input fields.
 *  -   Double click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpm files and
 *      -   changes the following input fields:
 *          -   Moisture Scenario Folder [ <selected subdirectory name> ].
 *  -   Single click on a file name
 *      -   highlights the file name and
 *      -   changes no input fields.
 *  -   Double click on a file name
 *      -   highlights the file name and
 *      -   changes the following input fields:
 *          -   Moisture Scenario Folder      [ <selected file's subdirectory> ],
 *          -   Moisture Scenario File        [ <selected file's name> ],
 *          -   Moisture Scenario Description [ <selected file's description> ].
 *  -   Saving the run to a new, non-existent file
 *      -   creates a new Moisture Scenario File[]
 *          in the current Moisture Scenario Folder[]
 *          with the current Moisture Scenario Description[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 *  -   Saving the run to an old, existing file
 *      -   displays a dialog to confirm the overwrite,
 *      -   overwrites the Moisture Scenario File[]
 *          in the MoistureScenario Folder[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 */

void BpDocument::saveAsMoistureScenarioFile( const QString &fileName )
{
    // Must be doing moisture modeling!
    QString text("");
    if ( ! property()->boolean( "surfaceModuleActive" )
      || ! property()->boolean( "surfaceConfMoisTimeLag" ) )
    {
        translate( text, "BpDocument:SaveMoisScenario:SurfaceModuleInactive" );
        warn( text );
        return;
    }
    // Store all input texts, even those that haven't received an Enter
    translate( text, "BpDocument:SaveMoisScenario:MissingInputs" );
    text += "<UL>";
    bool errors = false;
    int lid, tokens, position, length;
    for ( lid = 0;
          lid<leafCount();
          lid++ )
    {
        if ( m_eqTree->m_eqCalc->isMoisScenarioVariable( leaf(lid) ) )
        {
            // Store the contents in the EqVar store
            leaf(lid)->m_store = m_entry[lid]->text();
            // Validate the store
            if ( ! leaf(lid)->isValidStore( &tokens, &position, &length ) )
            {
                // Invalid contents; reposition the focus and return
                focusThis( lid, position, length );
                return;
            }
            // Must have some value entered
            if ( leaf(lid)->m_tokens < 1 )
            {
                text += QString( "<LI>%1</LI>" ).arg( *leaf(lid)->m_label );
                errors = true;
            }
        }
    }
    // Report missing entries
    if ( errors )
    {
        text += "</UL>";
        error( text );
        return;
    }
    // Request the fileName if one wasn't specified
    QString newFileName = fileName;
    QString oldDesc = m_eqTree->m_eqCalc->docDescriptionStore();
    QString newDesc = oldDesc;
    if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        // Get the Worksheet directory
        QString dirName = appFileSystem()->moisScenarioPath();
        QString extName = appFileSystem()->moisScenarioExt();
        // Create the file selection dialog
        FileSaveAsDialog dialog(
            appWindow(),                            // ApplicationWindow
            dirName,                                // subdirectory
            "Moisture Scenario",                    // file type name
            extName,                                // file extension
            "MyMoistureScenarios",                  // default folder
            fileName,                               // default file name
            m_eqTree->m_eqCalc->docDescriptionStore(),// default description
            "saveAsMoistureScenarioFileDialog" );   // widget name
        // Display the file selection dialog
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection
        dialog.getFileSelection( newFileName );
        // Get the (possibly new) file description
        dialog.getFileDescription( newDesc );
        if ( ! newDesc.isNull() && ! newDesc.isEmpty() && newDesc != oldDesc )
        {
            m_entry[0]->setText( newDesc );
            m_eqTree->m_eqCalc->docDescriptionStore( newDesc );
            property()->string( "appDescription", newDesc );
        }
    }
    // Write the file
    if ( m_eqTree->writeXmlFile( newFileName, "BehavePlus", "Moisture Scenario",
        appWindow()->m_release ) )
    {
        // Attach the moisture scenario file
        m_eqApp->attachMoisScenario( newFileName );
        // Display a confirmation
        translate( text, "BpDocument:SaveMoisScenario:Saved", newFileName );
        info( text );
    }
    // Restore the original description and return
    if ( newDesc != oldDesc )
    {
        m_entry[0]->setText( oldDesc );
        m_eqTree->m_eqCalc->docDescriptionStore( oldDesc );
        property()->string( "appDescription", oldDesc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current worksheet contents to a BehavePlus Run file.
 *
 *  Makes sure a complete set of valid inputs are entered, otherwise an error
 *  message is displayed. It then displays a FileSaveAsDialog to let the user
 *  enter a folder/file name.
 *
 *  Called only by save().
 *
 *  \param fileName Reference to the full file path name.
 *
 *  \par File SaveAs Dialog Behavior
 *
 *  The File->SaveAs->Run menu button displays a FileSaveAsDialog
 *  with the following behavior:
 *  -   All worksheet inputs are validated for existence and value.
 *  -   Dialog opens with 3 input fields initialized to
 *      -   Run Folder      [ MyRuns ],
 *      -   Run File        [ <blank> ],
 *      -   Run Description [ <current worksheet description> ].
 *  -   Single click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpr files and
 *      -   changes no input fields.
 *  -   Double click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpr files and
 *      -   changes the following input fields:
 *          -   Run Folder [ <selected subdirectory name> ].
 *  -   Single click on a file name
 *      -   highlights the file name and
 *      -   changes no input fields.
 *  -   Double click on a file name
 *      -   highlights the file name and
 *      -   changes the following input fields:
 *          -   Run Folder      [ <selected file's subdirectory> ],
 *          -   Run File        [ <selected file's name> ],
 *          -   Run Description [ <selected file's description> ].
 *  -   Saving the run to a new, non-existent file
 *      -   creates a new Run File[]
 *          in the current Run Folder[]
 *          with the current Run Description[],
 *      -   replaces the run's worksheet Description to Run Description[], and
 *      -   renames the current run to the Run Name[].
 *  -   Saving the run to an old, existing file
 *      -   displays a dialog to confirm the overwrite,
 *      -   overwrites hte Run File[] in the Run Folder[],
 *      -   replaces the worksheet Description with the Run Description[], and
 *      -   renames the current worksheet to the Run Name[].
 *  -   Saving files under \RunFolder\ExampleRuns is prohibited.
 */

void BpDocument::saveAsRunFile( const QString &fileName, bool clone )
{
    // Validate and store entry fields
    if ( ! validateWorksheet() )
    {
        return;
    }
    // Request the fileName if one wasn't specified
    QString text("");
    QString newFileName = fileName;
    if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        // Get the run directory
        QString dirName = appFileSystem()->runPath();
        QString extName = appFileSystem()->runExt();

        // Create the file selection dialog
        FileSaveAsDialog dialog(
            appWindow(),                            // ApplicationWindow
            dirName,                                // subdirectory
            "Run",                                  // file type name
            extName,                                // file extension
            "MyRuns",                               // default folder
            fileName,                               // default file name
            m_eqTree->m_eqCalc->docDescriptionStore(),   // default description
            "saveAsRunFileDialog" );                // widget name
        // Display the file selection dialog
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection
        dialog.getFileSelection( newFileName );
        // Get the (possibly new) file description
        QString desc;
        dialog.getFileDescription( desc );
        if ( ! desc.isNull() && ! desc.isEmpty()
            && desc != m_eqTree->m_eqCalc->docDescriptionStore() )
        {
            m_entry[0]->setText( desc );
            m_eqTree->m_eqCalc->docDescriptionStore( desc );
        }
    }
    // Cannot save to standard location
    QString reserved( QDir::separator() + QString( "RunFolder" )
                    + QDir::separator() + QString( "ExampleRuns" ) );
    if ( newFileName.find( reserved, 0, false ) >= 0 )
    {
        QString l_caption("");
        translate( l_caption, "BpDocument:ReservedLocation:Caption" );
        translate( text, "BpDocument:ReservedLocation:Text", reserved );
        warn( l_caption, text );
        return;
    }
    // Write the file
    property()->string( "appDescription", m_entry[0]->text() );
    if ( m_eqTree->writeXmlFile( newFileName, "BehavePlus", "Run",
        appWindow()->m_release ) )
    {
        if ( clone )
        {
            appWindow()->openDocument( newFileName, "Run" );
        }
        else
        {
            // Update the current Document's file name
            QFileInfo fi( newFileName );
            m_absPathName = fi.absFilePath();
            m_baseName    = fi.baseName();
            // Set the BpDocument widget's caption to the file base name
            // since the QWorkspace windowsList uses it to build the Windows menu.
            setCaption( m_baseName );
            statusUpdate();
            // Display a confirmation
            translate( text, "BpDocument:SaveRun:Saved", newFileName );
            info( text );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief saves the current input worksheet as a BehavePlus Units Set file.
 *
 *  Called only by save().
 *
 *  \param fileName Reference to the full file path name.
 *
 *  \par File SaveAs Dialog Behavior
 *
 *  The File->SaveAs->Units Set menu button displays a FileSaveAsDialog
 *  with the following behavior:
 *  -   Units Set inputs are NOT validated for existence nor value.
 *  -   Dialog opens with 3 input fields initialized to
 *      -   Units Set Folder      [ MyUnitsSets ],
 *      -   Units Set File        [ <blank> ],
 *      -   Units Set Description [ <current worksheet description> ].
 *  -   Single click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpu files and
 *      -   changes no input fields.
 *  -   Double click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpu files and
 *      -   changes the following input fields:
 *          -   Units Set Folder [ <selected subdirectory name> ].
 *  -   Single click on a file name
 *      -   highlights the file name and
 *      -   changes no input fields.
 *  -   Double click on a file name
 *      -   highlights the file name and
 *      -   changes the following input fields:
 *          -   Units Set Folder      [ <selected file's subdirectory> ],
 *          -   Units Set File        [ <selected file's name> ],
 *          -   Units Set Description [ <selected file's description> ].
 *  -   Saving the run to a new, non-existent file
 *      -   creates a new Units Set File[]
 *          in the current Units Set Folder[]
 *          with the current Units Set Description[], and
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 *  -   Saving the run to an old, existing file
 *      -   displays a dialog to confirm the overwrite,
 *      -   overwrites the Units Set File[] in the Units Set Folder[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 */

void BpDocument::saveAsUnitsSetFile( const QString &fileName )
{
	// Does the user even want to save these changes as a file?
    QString text("");
	QString title("");
	translate( text, "FileSelector:SaveUnitsSet:Prompt" );
	translate( title, "FileSelector:SaveUnitsSet:Title" );
	if ( ! yesno( title, text, 400 ) )
	{
		return;
	}

    // Request the fileName if one wasn't specified
    QString newFileName = fileName;
    QString oldDesc = m_eqTree->m_eqCalc->docDescriptionStore();
    QString newDesc = oldDesc;
    if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        // Get the Units Set directory
        QString dirName = appFileSystem()->unitsSetPath();
        QString extName = appFileSystem()->unitsSetExt();

        // Create the file selection dialog
        FileSaveAsDialog dialog(
            appWindow(),                            // ApplicationWindow
            dirName,                                // subdirectory
            "Units Set",                            // file type name
            extName,                                // file extension
            "MyUnitsSets",                          // default folder
            fileName,                               // default file name
            m_eqTree->m_eqCalc->docDescriptionStore(),   // default description
            "saveAsUnitsSetFileDialog" );           // widget name
        // Display the file selection dialog
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection
        dialog.getFileSelection( newFileName );
        // Get the (possibly new) file description
        dialog.getFileDescription( newDesc );
        if ( ! newDesc.isNull() && ! newDesc.isEmpty() && newDesc != oldDesc )
        {
            m_entry[0]->setText( newDesc );
            m_eqTree->m_eqCalc->docDescriptionStore( newDesc );
            property()->string( "appDescription", newDesc );
        }
    }
    // Write the file
    if ( m_eqTree->writeXmlFile( newFileName, "BehavePlus", "Units Set",
        appWindow()->m_release ) )
    {
        // Display a confirmation
        translate( text, "BpDocument:SaveUnitsSet:Saved", newFileName );
        info( text );
    }
    // Restore the original description before returning
    if ( newDesc != oldDesc )
    {
        m_entry[0]->setText( oldDesc );
        m_eqTree->m_eqCalc->docDescriptionStore( oldDesc );
        property()->string( "appDescription", oldDesc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the current worksheet contents to a BehavePlus Worksheet file.
 *
 *  Makes sure a complete set of valid inputs are entered, otherwise an error
 *  message is displayed. It then displays a FileSaveAsDialog to let the user
 *  enter a folder/file name.
 *
 *  Called only by save().
 *
 *  \param fileName Reference to the full file path name.
 *
 *  \par File SaveAs Dialog Behavior
 *
 *  The File->SaveAs->Worksheet menu button displays a FileSaveAsDialog
 *  with the following behavior:
 *  -   Worksheet inputs are NOT validated for existence nor value.
 *  -   Dialog opens with 3 input fields initialized to
 *      -   Worksheet Folder      [ MyWorksheets ],
 *      -   Worksheet File        [ <blank> ],
 *      -   Worksheet Description [ <current worksheet description> ].
 *  -   Single click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpw files and
 *      -   changes no input fields.
 *  -   Double click on a subdirectory
 *      -   opens the subdirectory and displays all the *.bpw files and
 *      -   changes the following input fields:
 *          -   Worksheet Folder [ <selected subdirectory name> ].
 *  -   Single click on a file name
 *      -   highlights the file name and
 *      -   changes no input fields.
 *  -   Double click on a file name
 *      -   highlights the file name and
 *      -   changes the following input fields:
 *          -   Worksheet Folder      [ <selected file's subdirectory> ],
 *          -   Worksheet File        [ <selected file's name> ],
 *          -   Worksheet Description [ <selected file's description> ].
 *  -   Saving the run to a new, non-existent file
 *      -   creates a new Worksheet File[]
 *          in the current Worksheet Folder[]
 *          with the current Worksheet Description[], and
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 *  -   Saving the run to an old, existing file
 *      -   displays a dialog to confirm the overwrite,
 *      -   overwrites the Worksheet File[] in the Worksheet Folder[],
 *      -   changes no worksheet values, and
 *      -   does NOT rename the current run.
 *  -   Saving files under \WorksheetFolder\ExampleWorksheets is prohibited.
 */

void BpDocument::saveAsWorksheetFile( const QString &fileName )
{
    // Request the fileName if one wasn't specified
    QString newFileName = fileName;
    QString oldDesc = m_eqTree->m_eqCalc->docDescriptionStore();
    QString newDesc = oldDesc;
    QString text("");
    if ( newFileName.isNull() || newFileName.isEmpty() )
    {
        // Get the Worksheet directory
        QString dirName = appFileSystem()->worksheetPath();
        QString extName = appFileSystem()->worksheetExt();

        // Create the file selection dialog
        FileSaveAsDialog dialog(
            appWindow(),                            // ApplicationWindow
            dirName,                                // subdirectory
            "Worksheet",                            // file type name
            extName,                                // file extension
            "MyWorksheets",                         // default folder
            fileName,                               // default file name
            m_eqTree->m_eqCalc->docDescriptionStore(),   // default description
            "saveAsWorksheetFileDialog" );          // widget name
        // Display the file selection dialog
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection
        dialog.getFileSelection( newFileName );
        // Get the (possibly new) file description
        dialog.getFileDescription( newDesc );
        if ( ! newDesc.isNull() && ! newDesc.isEmpty() && newDesc != oldDesc )
        {
            m_entry[0]->setText( newDesc );
            m_eqTree->m_eqCalc->docDescriptionStore( newDesc );
            property()->string( "appDescription", newDesc );
        }
    }
    // Cannot save to standard location
    QString reserved( QDir::separator() + QString( "WorksheetFolder" )
                    + QDir::separator() + QString( "ExampleWorksheets" ) );
    if ( newFileName.find( reserved, 0, false ) >= 0 )
    {
        QString l_caption("");
        translate( l_caption, "BpDocument:ReservedLocation:Caption" );
        translate( text, "BpDocument:ReservedLocation:Text", reserved );
        warn( l_caption, text );
        return;
    }
    // Write the file
    if ( m_eqTree->writeXmlFile( newFileName, "BehavePlus", "Worksheet",
        appWindow()->m_release, false ) )
    {
        // Display a confirmation
        translate( text, "BpDocument:SaveWorksheet:Saved", newFileName );
        info( text );
    }
    // Restore the original description before returning
    if ( newDesc != oldDesc )
    {
        m_entry[0]->setText( oldDesc );
        m_eqTree->m_eqCalc->docDescriptionStore( oldDesc );
        property()->string( "appDescription", oldDesc );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Saves the attached fuel models to a FARSITE export file.
 */

void BpDocument::saveResults( const QString &fileType )
{
    // There must be results
    if ( m_pages == m_worksheetPages )
    {
        QString errMsg("");
        translate( errMsg, "Menu:File:Export:Results:NoResults" );
        error( errMsg );
        return;
    }

    // Setup
    QString typeName, fileName;
    translate( typeName, "BpDocument:SaveExportedResults:Caption" );
    //QString filters = ( "Spreadsheet ( *.xls);;Html ( *.htm *.html)" );
    QString filters = ( "Html ( *.htm *.html)" );
    QString sourceFile = appFileSystem()->composerPath()
               + "/" + property()->string( "exportHtmlFile" );
    QString l_caption = "Save As " + typeName;
    // Request the output file
    bool more = true;
    while( more )
    {
        more = false;
        // Display the dialog
        QFileDialog fd( this, "saveResults", true );
        fd.setDir( appFileSystem()->exportPath() );
        fd.setMode( QFileDialog::AnyFile );
        fd.setFilters( filters );
        fd.setCaption( typeName );
        if ( fd.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file name
        fileName = fd.selectedFile();
        if ( fileName.isNull() || fileName.isEmpty() )
        {
            return;
        }
        // Determine the source file and extension
        QString selectedFilter = fd.selectedFilter();
        if ( selectedFilter.left( 4 ) == "Html" )
        {
            if ( fileName.right( 4 ) != ".htm"
              && fileName.right( 5 ) != ".html" )
            {
                fileName.append( ".html" );
            }
        }
        //else // if ( selectedFilter.left( 5 ) == "Spread" )
        //{
        //    if ( fileName.right( 4 ) != ".xls" )
        //    {
        //        fileName.append( ".xls" );
        //    }
        //}
        // If the file exists, get permission to overwrite it.
        QFileInfo fi( fileName );
        if ( fi.exists() )
        {
            QString caption2, text;
            translate( caption2, "FileSelector:OverwriteFile:Caption" );
            translate( text, "FileSelector:OverwriteFile:Text",
                    fileType, fileName );
            if ( yesno( l_caption, text ) == 0 )
            {
                more = true;
            }
        }
    }

    // Copy the source file to the export file
    QString errMsg("");
    if ( ! textFileCopy( sourceFile, fileName, errMsg ) )
    {
        warn( errMsg );
        return;
    }

    // Confirm the file was written
    QString saved;
    translate( saved, "BpDocument:SaveExportedHtml:Saved", fileName );
    info( saved );
    return;
}

//------------------------------------------------------------------------------
//  End of bpfile.cpp
//------------------------------------------------------------------------------

