//------------------------------------------------------------------------------
/*! \file bpdocument.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class methods.
 *
 *  This file contains those methods called by the AppWindow class
 *  and those re-implemented from Document class.
 *
 *  Additional BpDocument method definitions are in:
 *      - bpcomposecontaindiagram.cpp
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
 *      - bpfile.cpp
 */

// Custom include files
#include "appdialog.h"
#include "appearancedialog.h"
#include "appfilesystem.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "attachdialog.h"
#include "bpdocentry.h"
#include "bpdocument.h"
#include "composer.h"
#include "conflictdialog.h"
#include "docdevicesize.h"
#include "docpagesize.h"
#include "docscrollview.h"
#include "doctabs.h"
#include "fileselector.h"
#include "fuelinitdialog.h"
#include "fuelmodel.h"
#include "fuelmodeldialog.h"
#include "guidedialog.h"
#include "modulesdialog.h"
#include "moisscenario.h"
#include "property.h"
#include "rundialog.h"
#include "rxvar.h"
#include "xeqapp.h"
#include "xeqcalc.h"
#include "xeqtree.h"
#include "xeqvar.h"

// Qt include files
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qbuttongroup.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

//------------------------------------------------------------------------------
/*! \brief BpDocument class constructor.
 *
 *  \param workspace Pointer to the AppWindow::m_workSpace parent.
 *  \param docId     Unique serial number for the Document within this process.
 *  \param eqApp     Pointer to the master EqApp instance.
 *  \param name      Document's internal QWidget name.
 *  \param wflags    Standard QWidget #wflags.
 */

BpDocument::BpDocument( QWorkspace *workspace, int docId, EqApp *eqApp,
        const char* p_name, int wflags ) :
    Document( workspace, "BpDocument", docId, p_name, wflags ),
    m_eqApp(eqApp),
    m_eqTree(0),
    m_worksheetPages(0),
    m_focusEntry(0),
    m_worksheetEdited(false),
    m_doValidation(true),
    m_entry(0),
    m_entryPage(0),
    m_entryX(0),
    m_entryY(0),
    m_entryHt(0),
    m_entryWd(0),
    m_guideBtn(0),
    m_guideBtnX(0),
    m_guideBtnHt(0),
    m_guideBtnWd(0),
    m_guideBtnGrp(0),
//    m_guidePixmap(guide),
    m_btn(1),
    m_btnPage(1),
    m_btnX(1),
    m_btnY(1),
    m_btnHt(1),
    m_btnWd(1),
    m_rxMinEntry(0),
    m_rxMaxEntry(0),
    m_rxCheckBox(0),
    m_rxItemBox(0),
    m_rxItemY(0),
    m_rxEntryPage(0),
    m_rxEntryX(0),
    m_rxEntryY(0),
    m_rxEntryHt(0),
    m_rxEntryWd(0),
    m_notes(0),
    m_notesPage(0),
    m_notesX(0),
    m_notesY(0),
    m_notesWd(0),
    m_notesHt(0)
{
    // Popup context menu must be created here because it is declared a
    // pure virtual method in Document.
    contextMenuCreate();

    // Create the EqTree for this instance.
    // Note that all EqTrees share the single EqApp.
    m_eqTree = m_eqApp->newEqTree( p_name, "", m_eqApp->m_language );
    Q_CHECK_PTR( m_eqTree );
    // Set all display units to English
    m_eqTree->applyEnglishUnits();

    // Create the guide button group and connect its shared callback slot.
    m_guideBtnGrp = new QButtonGroup( this, "m_guideBtnGroup" );
    Q_CHECK_PTR( m_guideBtnGrp );
    connect( m_guideBtnGrp,  SIGNAL( clicked(int) ),
             this,           SLOT( guideClicked(int) ) );
    m_guideBtnGrp->hide();

    // Enable the QScrollView clipper.
    m_scrollView->enableClipper( true );

    // Create the additional push buttons.
    // For some reason, a widget must appear on every displayed page to force
    // the viewport's scollbars to work, so we used m_btn[0].
    m_btn[0] = new QPushButton( m_scrollView->viewport(), "m_btn[0]" );
    Q_CHECK_PTR( m_btn[0] );
    m_btn[0]->hide();
    m_btn[0]->setFocusPolicy( QWidget::StrongFocus );
    m_btnX[0] = m_btnY[0] = m_btnHt[0] = m_btnWd[0] = m_btnPage[0] = 0;
    QString text("");
    translate( text, "BpDocument:Button:InitFromFuelModel" );
    m_btn[0]->setText( text );

    // Connect the Fuel Initialization button to a callback slot.
    connect( m_btn[0], SIGNAL( clicked() ),
             this,     SLOT( fuelClicked() ) );

    // Adjust arrays containing the prescription variable widgets
    QColor bgColor = property()->color( "pageBackgroundColor" );
    RxVar *rxVar = 0;
    int rxVars = m_eqTree->m_rxVarList->count();
    m_rxCheckBox.resize( rxVars );
    m_rxMinEntry.resize( rxVars );
    m_rxMaxEntry.resize( rxVars );
    m_rxEntryPage.resize( rxVars );
    m_rxEntryX.resize( rxVars );
    m_rxEntryY.resize( rxVars );
    m_rxEntryWd.resize( rxVars );
    m_rxEntryHt.resize( rxVars );
    int rxItems = m_eqTree->m_rxVarList->items();
    m_rxItemBox.resize( rxItems );
    m_rxItemY.resize( rxItems );

    // Create the prescription variable entry widgets
    int rx = 0;
    int atItem = 0;
    for ( rxVar = m_eqTree->m_rxVarList->first();
          rxVar;
          rxVar = m_eqTree->m_rxVarList->next(), rx++ )
    {
        EqVar *rxPtr = rxVar->m_varPtr;
        QString label = *(rxPtr->m_label);
        // Create the activation checkbox widget
        m_rxCheckBox[rx] = new QCheckBox( "", this,
            QString( "m_rxCheckBox[%1]" ).arg(rx) );
        checkmem( __FILE__, __LINE__, m_rxCheckBox[rx],
            "QCheckBox m_rxCheckBox", rx );
        m_rxCheckBox.at(rx)->setFocusPolicy( QWidget::StrongFocus );
        m_rxCheckBox.at(rx)->hide();
        m_rxCheckBox.at(rx)->setPaletteBackgroundColor( bgColor );
        // Create the minimum value entry widget
        m_rxMinEntry.at(rx) = new QLineEdit( this,
            QString( "m_rxMinEntry[%1]" ).arg(rx) );
        checkmem( __FILE__, __LINE__, m_rxMinEntry.at(rx),
            "QLineEdit m_rxMinEntry", 1 );
        m_rxMinEntry.at(rx)->setFocusPolicy( QWidget::StrongFocus );
        m_rxMinEntry.at(rx)->hide();
        // Create the maximum value entry widget
        m_rxMaxEntry.at(rx) = new QLineEdit( this,
            QString( "m_rxMaxEntry[%1]" ).arg(rx) );
        checkmem( __FILE__, __LINE__, m_rxMaxEntry.at(rx),
            "QLineEdit m_rxMaxEntry", 1 );
        m_rxMaxEntry.at(rx)->setFocusPolicy( QWidget::StrongFocus );
        m_rxMaxEntry.at(rx)->hide();
        // Create the discrete item checkbox widgets (if any)
        int n = rxVar->items();
        if ( n > 0 )
        {
            rxVar->setFirstItemBox( atItem );
            for ( int i=0; i<n; i++, atItem++ )
            {
                m_rxItemBox[atItem] = new QCheckBox(
                    rxPtr->m_itemList->itemName( i ), this,
                    QString( "m_rxItemBox[%1]" ).arg(atItem) );
                checkmem( __FILE__, __LINE__, m_rxItemBox[atItem],
                    "QCheckBox m_rxItemBox", atItem );
                m_rxItemBox.at(atItem)->setFocusPolicy( QWidget::StrongFocus );
                m_rxItemBox.at(atItem)->hide();
                m_rxItemBox.at(atItem)->setPaletteBackgroundColor( bgColor );
                m_rxItemY.at(atItem) = 0;
            }
        }
        // Initialize the m_rxEntry positions.
        m_rxEntryPage.at(rx) = m_rxEntryX.at(rx) = m_rxEntryY.at(rx) = 0;
    }
    // Create the Notes widget.
    m_notes = new QTextEdit( this, "m_notes" );
    Q_CHECK_PTR( m_notes );
    m_notes->setTextFormat( Qt::PlainText );

    // Uncomment the next line to generate a blank.bpw from program defaults.
    //saveAsWorksheetFile( "blank.bpw" );
    return;
}

//------------------------------------------------------------------------------
/*! \brief BpDocument destructor.
 *
 *  \remark This destructor explicitly deletes widget children to convince
 *  memory leak detectors that dynamically-allocated resources have REALLY
 *  been released.  This is harmless, but redundant, since Qt destroys all
 *  children when a parent is destroyed.
 */

BpDocument::~BpDocument( void )
{
    int rx = 0;
    int rxItems = m_eqTree->m_rxVarList->items();
    for ( rx=0; rx<rxItems; rx++ )
    {
        delete m_rxItemBox[rx]; m_rxItemBox[rx] = 0;
    }
    int rxVars = m_eqTree->m_rxVarList->count();
    for ( rx=0; rx<rxVars; rx++ )
    {
        delete m_rxCheckBox[rx];    m_rxCheckBox[rx] = 0;
        delete m_rxMinEntry[rx];    m_rxMinEntry[rx] = 0;
        delete m_rxMaxEntry[rx];    m_rxMaxEntry[rx] = 0;
    }
    m_eqApp->m_eqTreeList->remove( m_eqTree );
    m_eqTree = 0;
    delete m_btn[0];        m_btn[0] = 0;
    delete m_guideBtnGrp;   m_guideBtnGrp = 0;
    delete m_notes;         m_notes = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Clears text of all entry fields.
 */

void BpDocument::clear( bool /* showRunDialog */ )
{
    // Save the worksheet entry values
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        // Store the contents in the EqVar store.
        m_entry[lid]->setText( "" );
        leaf(lid)->m_store = m_entry[lid]->text();
        m_entry[lid]->setEdited( false );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience function that reconfigures and redraws the worksheet.
 *
 *  Called by configureAppearance(), configureModules(), configureUnits(),
 *  open(), CheckInputDialog1::store(), and CheckInputDialog2::store().
 */

void BpDocument::configure( void )
{
    // This catches any change in language
    QString text("");
    translate( text, "BpDocument:Button:InitFromFuelModel" );
    m_btn[0]->setText( text );

    // First store any unedited worksheet entry values into their
    // variable m_store, since these will be pulled back by reconfigure().
    storeEntries();

    // Reconfigure the EqTree
    m_eqTree->reconfigure( appWindow()->m_release );
    regenerateWorksheet();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the AppearanceDialog in response to a main menu
 *  \b Configure->Appearance selection.
 *
 *   Called only by ApplicationWindow::slotConfigureAppearance().
 */

void BpDocument::configureAppearance( void )
{
    // Display the appearance dialog.
    AppearanceDialog dialog( this );
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Reset the page tabs to the (possibly) new values.
    setPageTabs();
    // Must reconfigure the EqTree and generate and compose the worksheet.
    configure();
    // Display the first page in the Document's DocScrollView.
    showPage( 1 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the AttachDialog to attach/detach fuel models when the user
 *  selects \b Configure->Fuel \b model \b set \b selection from the main menu.
 *
 *  Called only by ApplicationWindow::slotConfigureFuelModels().
 */

void BpDocument::configureFuelModels( void )
{
    // Get the fuel model folder name and file extension.
    QString dirName = appFileSystem()->fuelModelPath();
    QString extName = appFileSystem()->fuelModelExt();

    // Get a list of all the attached fuel model file names.
    QStringList oldList;
    m_eqApp->m_fuelModelList->fileList( oldList );

    // Create the folder selection dialog.
    AttachDialog dialog(
        this,                               // BpDocument
        dirName,                            // units directory
        "Fuel Model",                       // column heading
        extName,                            // file extension
        &oldList,                           // Current list of attached files
        "attachFuelDialog" );               // widget name
    // Display the file selection dialog.
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Get the new list of attached files.
    QStringList newList;
    dialog.getSelectionList( newList );

    // Go through the oldList and remove any files not in the new list.
    QStringList::Iterator was;
    QStringList::Iterator now;
    bool remove;
    for ( was = oldList.begin();
          was != oldList.end();
          ++was )
    {
        remove = true;
        // Is this file still in the new list?
        for ( now = newList.begin();
              now != newList.end();
              ++now )
        {
            // If it is in the new list, keep it.
            if ( (*now) == (*was) )
            {
                remove = false;
                break;
            }
        }
        // If it wasn't in the new list, remove it.
        if ( remove )
        {
            QFileInfo fi( (*was) );
            m_eqApp->deleteFuelModel( fi.baseName() );
        }
    }
    // Go through the new list and attach any unattached files.
    for ( now = newList.begin();
          now != newList.end();
          ++now )
    {
        if ( ! m_eqApp->m_fuelModelList->fuelModelByFileName( (*now) ) )
        {
            m_eqApp->attachFuelModel( (*now) );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the AttachDialog to attach/detach moisture scenarios when
 *  the user selects \b Configure->moisture \b Scenario \b set \b selection.
 *  from the main menu.
 *
 *  Called only by ApplicationWindow::slotConfigureMoistureScenarios().
 */

void BpDocument::configureMoistureScenarios( void )
{
    // Get the moisture scenario model folder name and file extension.
    QString dirName = appFileSystem()->moisScenarioPath();
    QString extName = appFileSystem()->moisScenarioExt();

    // Get a list of all the attached moisture scenario file names.
    QStringList oldList;
    m_eqApp->m_moisScenarioList->fileList( oldList );

    // Create the folder selection dialog.
    AttachDialog dialog(
        this,                               // BpDocument
        dirName,                            // units directory
        "Moisture Scenario",                // file type name
        extName,                            // file extension
        &oldList,                           // Current list of attached files
        "attachMoisScenarioDialog" );       // widget name
    // Display the file selection dialog.
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Get the new list of attached files.
    QStringList newList;
    dialog.getSelectionList( newList );

    // Go through the oldList and remove any files not in the new list.
    QStringList::Iterator was;
    QStringList::Iterator now;
    bool remove;
    for ( was = oldList.begin();
          was != oldList.end();
          ++was )
    {
        remove = true;
        // Is this file still in the new list?
        for ( now = newList.begin();
              now != newList.end();
              ++now )
        {
            // If it is in the new list, keep it.
            if ( (*now) == (*was) )
            {
                remove = false;
                break;
            }
        }
        // If it wasn't in the new list, remove it.
        if ( remove )
        {
            QFileInfo fi( (*was) );
            m_eqApp->deleteMoisScenario( fi.baseName() );
        }
    }
    // Go through the new list and attach any unattached files
    for ( now = newList.begin();
          now != newList.end();
          ++now )
    {
        if ( ! m_eqApp->m_moisScenarioList->moisScenarioByFileName( (*now) ) )
        {
            m_eqApp->attachMoisScenario( (*now) );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the Modules Configuration Dialog (to activate/deactive
 *  BehavePlus modules and output variables) when the user selects
 *  \b Configure->Module \b selection from the main menu.
 *
 *  Called only by ApplicationWindow::slotConfigureModules().
 */

void BpDocument::configureModules( void )
{
    // Validate all the inputs (or at least the edited ones)
    // This is necessary because (1) the user could have entered text into an
    // entry field and NOT pressed return before clicking on Configure Modules
    // and (2) under WINDOZE an edited entry field causes a crash from within
    // resizeWorksheet() when it is hidden AND it is no longer one of the
    // displayed inputs (e.g., it was the 6th of 9 fields, but after
    // configuration, there are only 5 fields).
    storeEntries();
#ifdef REPLACED_BY_storeEntries
    int tokens, position, length;
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        if ( m_entry[lid]->edited() )
        {
            // Store the contents in the EqVar store.
            leaf(lid)->m_store = m_entry[lid]->text();

            // Validate the store.
            if ( ! leaf(lid)->isValidStore( &tokens, &position, &length ) )
            {
                // If invalid contents, reposition the focus and return.
                focusThis( lid, position, length );
                return;
            }
            m_entry[lid]->setEdited( false );
        }
    }
#endif
    // Present the Modules dialog.
    ModulesDialog dialog( this );
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Redraw the worksheet and return.
    configure();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Changes the units of measure set applied to all the EqTree,
 *  worksheet, and output variables.  The selected units set is stored with
 *  the run file.
 *
 *  If the user selects one of the following from the main menu:
 *      - \b Configure->Units->English
 *      - \b Configure->Units->Matric
 *      - \b Configure->Units->Native
 *  then the appropriate units set is applied.
 *
 *  If the user selects \b Configure->Custom from the main menu, a file
 *  selector dialog allows the user to choose and apply a custom units set.
 *
 *  Called only by one of the following:
 *      -   AppWindow::slotConfigureUnits().
 *      -   AppWindow::slotConfigureUnits().
 *      -   AppWindow::slotConfigureUnits().
 *      -   AppWindow::slotConfigureUnits().
 *
 *  Most the work is handled by EqTree::applyUnitsSet(), which applies either:
 *      -# one of the standard sets "English", "Metric", or "Native", or
 *      -# reads the file and applies changes to the EqTree variable store()s.
 *
 *  This function merely
 *      -# supplies a file selector dialog if needed, and
 *      -# copies the variable store()s into the worksheet.
 *
 *  \param unitsSet If "English", "Metric", or "Native", then the named
 *  standard units set is applied.  If "Custom", a file selection dialog
 *  is used.
 */

void BpDocument::configureUnits( const QString &unitsSet )
{
    // Initialization.
    QString fileName("");

    // If this is a standard (built-in) units set...
    if ( unitsSet == "English"
      || unitsSet == "Metric"
      || unitsSet == "Native" )
    {
        fileName = unitsSet;
    }
    // Otherwise we need to display the file selector.
    else
    {
        // Get the units directory.
        QString dirName = appFileSystem()->unitsSetPath();
        QString extName = appFileSystem()->unitsSetExt();

        // Create the file selection dialog.
        FileSelectorDialog dialog(
            appWindow(),                    // ApplicationWindow
            dirName,                        // units directory
            "Units Set",                    // file type
            extName,                        // file extension
            "unitsOpenDialog" );            // widget name
        // Display the file selection dialog.
        if ( dialog.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file selection.
        dialog.getFileSelection( fileName );
    }
    // Attempt to apply the units set to the EqTree.
    // On success, all the variable's store()s are converted.
    if ( m_eqTree->applyUnitsSet( fileName ) )
    {
        getStores();
        // Reconfigure, redraw, show, and return.
        configure();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback slot for the Document's popup context menu.
 *  Reimplemented for BpDocument.
 */

void BpDocument::contextMenuActivated( int id )
{
    if ( id == ContextSave )
    {
        save( m_absPathName, "Native" );
    }
    else if ( id == ContextSaveAs )
    {
        save( 0, "Native" );
    }
    else if ( id == ContextPrint )
    {
        print();
    }
    else if ( id == ContextCapture )
    {
        capture();
    }
    else if ( id == ContextCalculate )
    {
        run();
    }
    //else if ( id == ContextClose )
    //{
    //    appWindow()->slotDocumentClose();
    //}
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the popup context menu.
 *  Called only by the BpDocument constructor.
 */

void BpDocument::contextMenuCreate( void )
{
    // Create the context menu and store its pointer as private data.
    m_contextMenu = new QPopupMenu( 0, "m_contextMenu" );
    Q_CHECK_PTR( m_contextMenu );

    // Add the menu items.
    QString text("");
    int mid;
    // Calculate
    translate( text, "BpDocument:ContextMenu:Calculate" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextCalculate );
    // Save
    translate( text, "BpDocument:ContextMenu:Save" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextSave );
    // Save As
    translate( text, "BpDocument:ContextMenu:SaveAs" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextSaveAs );
    // Print
    translate( text, "BpDocument:ContextMenu:Print" );
    mid = m_contextMenu->insertItem( text,
            this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextPrint );
    // Capture
    translate( text, "BpDocument:ContextMenu:Capture" );
    mid = m_contextMenu->insertItem( text,
             this, SLOT( contextMenuActivated( int ) ) );
    m_contextMenu->setItemParameter( mid, ContextCapture );
    // Close
    //translate( text, "BpDocument:ContextMenu:Close" );
    //mid = m_contextMenu->insertItem( text,
    //         this, SLOT( contextMenuActivated( int ) ) );
    //m_contextMenu->setItemParameter( mid, ContextClose );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Moves focus to the next worksheet entry field.
 *
 *  The new focus field may optionally have a highlighted selection as
 *  specified by \a position and \a length.
 *
 *  \param position Start of highlighted (selected) text.  A value of "0"
 *  marks the first character, a value of "-1" disables selection.
 *
 *  \param length Number of characters to highlight.  If \a position is
 *  greater than or equal to zero (selection enabled) and \a length is
 *  less than zero, then the entire entry text is selected.
 */

void BpDocument::focusNext( int position, int length )
{
    // Find out who has the current focus.
    int oldFocus = 0;
    int newFocus = 0;
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        if ( m_entry[lid]->hasFocus() )
        {
            oldFocus = lid;
            break;
        }
    }
    // Determine who gets the new focus.
    if ( ( newFocus = oldFocus + 1 ) >= leafCount() )
    {
        newFocus = 0;
    }
    // Move the focus.
    focusThis( newFocus, position, length );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Moves focus to the previous worksheet entry field.
 *
 *  The new focus field may optionally have a highlighted selection as
 *  specified by \a position and \a length.
 *
 *  \param position Start of highlighted (selected) text.  A value of "0"
 *  marks the first character, a value of "-1" disables selection.
 *
 *  \param length Number of characters to highlight.  If \a position is
 *  greater than or equal to zero (selection enabled) and \a length is
 *  less than zero, then the entire entry text is selected.
 */

void BpDocument::focusPrev( int position, int length )
{
    // Find out who has the current focus
    int oldFocus = 0;
    int newFocus = 0;
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        if ( m_entry[lid]->hasFocus() )
        {
            oldFocus = lid;
            break;
        }
    }
    // Determine who gets the new focus
    if ( ( newFocus = oldFocus - 1 ) < 0 )
    {
        newFocus = leafCount() - 1;
    }
    // Move the focus
    focusThis( newFocus, position, length );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Moves focus to the \a lid worksheet entry field.
 *
 *  The new focus field may optionally have a highlighted selection as
 *  specified by \a position and \a length.
 *
 *  \param lid Entry field (leaf) index, starting at entry 0.
 *
 *  \param position Start of highlighted (selected) text.  A value of "0"
 *  marks the first character, a value of "-1" disables selection.
 *
 *  \param length Number of characters to highlight.  If \a position is
 *  greater than or equal to zero (selection enabled) and \a length is
 *  less than zero, then the entire entry text is selected.
 */

void BpDocument::focusRx( int rx, QLineEdit *entry )
{
    // If the new field is on another page, display the new page.
    if ( m_rxEntryPage[rx] > m_page )
    {
        pageNext();
    }
    else if ( m_rxEntryPage[rx] < m_page )
    {
        pagePrev();
    }
    // Set the focus and select all the text.
    entry->setFocus();
    entry->home( false );
    entry->selectAll();
    entry->setCursorPosition( 0 );

    // Make sure we can see this field.
    entry->show();
    m_scrollView->ensureVisible(
         m_scrollView->childX( entry ),
         m_scrollView->childY( entry ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Moves focus to the \a lid worksheet entry field.
 *
 *  The new focus field may optionally have a highlighted selection as
 *  specified by \a position and \a length.
 *
 *  \param lid Entry field (leaf) index, starting at entry 0.
 *
 *  \param position Start of highlighted (selected) text.  A value of "0"
 *  marks the first character, a value of "-1" disables selection.
 *
 *  \param length Number of characters to highlight.  If \a position is
 *  greater than or equal to zero (selection enabled) and \a length is
 *  less than zero, then the entire entry text is selected.
 */

void BpDocument::focusThis( int lid, int position, int length )
{
    // If the new field is on another page, display the new page.
    if ( m_entryPage[lid] > m_page )
    {
        pageNext();
    }
    else if ( m_entryPage[lid] < m_page )
    {
        pagePrev();
    }
    // Set the focus and select all the text.
    m_entry[lid]->setFocus();
    m_focusEntry = lid;
    m_entry[lid]->home( false );
    // Set the selection.
    if ( position >= 0 )
    {
        if ( length < 0 )
        {
            m_entry[lid]->selectAll();
        }
        else
        {
            m_entry[lid]->setSelection( position, length );
            m_entry[lid]->setCursorPosition( position + length );
        }
    }
    else
    {
        m_entry[lid]->setCursorPosition( 0 );
    }
    // Make sure we can see this field.
    m_entry[lid]->show();
    m_scrollView->ensureVisible(
         m_scrollView->childX( m_entry[lid] ),
         m_scrollView->childY( m_entry[lid] ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback slot for the "Initialize from a Fuel Model" button stored
 *  at m_btn[0].
 *
 *  Displays a fuel model selector dialog containing all attached fuel models.
 *  If one is selected, it is used to fill in all the fuel parameters with
 *  single values.
*/

void BpDocument::fuelClicked( void )
{
    // Request a fuel model from the user.
    FuelInitDialog dialog( this, "fuelInitDialog" );
    if ( dialog.exec() != QDialog::Accepted )
    {
        return;
    }
    // Get the selected fuel model parameters.
    QString result("");
    dialog.resultString( result );
    // Locate the fuel model and load its values.
    FuelModel *fm = m_eqApp->m_fuelModelList->fuelModelByModelName( result );
    if ( ! fm )
    // This code block should never be executed!
    {
        QString text("");
        translate( text, "BpDocument:FuelModelNotFound", result );
        bomb( text );
        return;
    }
    m_eqTree->m_eqCalc->initFuelInputs( fm );

    // Move m_store contents into the input area.
    getStores();

	// Reset the button label
    QString text("");
    translate( text, "BpDocument:Button:InitFromFuelModel" );
    m_btn[0]->setText( QString( "%1 (%2)" ).arg( text ).arg( fm->m_name ) );

    // Adjust input masks and return.
    m_eqTree->m_eqCalc->maskInputs();
    grayInputs();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Puts variables backing stores into the respective widgets.
 */

void BpDocument::getStores( void )
{
    // Now redisplay all the store()s in the worksheet entry fields.
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        m_entry[lid]->setText( leaf(lid)->m_store );
    }
    // And redisplay the stores in the prescription variables
    if ( property()->boolean( "tableShading" ) )
    {
        RxVar *rxVar = 0;
        int rxId = 0;
        for ( rxVar = m_eqTree->m_rxVarList->first();
              rxVar;
              rxVar = m_eqTree->m_rxVarList->next(), rxId++ )
        {
            EqVar *rxPtr = rxVar->m_varPtr;
            // Must be a user output
            if ( rxPtr->m_isUserOutput )
            {
                m_rxCheckBox[rxId]->setChecked( rxVar->m_isActive );
                if ( rxPtr->isContinuous() )
                {
                    m_rxMinEntry[rxId]->setText( rxVar->m_storeMinimum );
                    m_rxMaxEntry[rxId]->setText( rxVar->m_storeMaximum );
                }
                else
                {
                    int nItems = rxVar->items();
                    int rxItem = rxVar->m_firstItemBox;
                    for ( int item=0; item<nItems; item++, rxItem++ )
                    {
                        m_rxItemBox[rxItem]->setChecked(
                            rxVar->itemChecked( item ) );
                    }
                }
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Grays out the entry fields of any unneeded inputs.
 *
 *  Called immediately after EqCalc::maskInputs().
 */

void BpDocument::grayInputs( void )
{
    // Assume entry field 0 has the normal palette since its never masked.
    QPalette Normal( m_entry[0]->palette() );

    // Set up the masked input palette.
    QPalette Masked( m_entry[0]->palette() );
    QString baseColor = property()->color( "worksheetMaskColor" );
    Masked.setColor( QPalette::Normal,   QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Disabled, QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Active,   QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Inactive, QColorGroup::Base, baseColor );

    // Now enable/disable all the moisture entry fields.
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        if ( leaf( lid )->m_isMasked )
        {
            m_entry[lid]->setPalette( Masked );
        }
        else
        {
            m_entry[lid]->setPalette( Normal );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback slot to the worksheet Guide button group.
 *
 *  Displays the Guide Dialog allowing input if range values for continuous
 *  variables, or selection of choices for discrete variables.
 *
 *  \param lid Leaf variable index of the activated guide button.
 *  A pointer to the leaf variable is returned by a call to leaf( lid ).
 */

void BpDocument::guideClicked( int lid )
{
    QString qStr("");
	EqVar* var = leaf( lid );
	if (var->m_name == "vSurfaceFuelBedModel"
		|| var->m_name == "vSurfaceFuelBedModel1"
		|| var->m_name == "vSurfaceFuelBedModel2" )
	{
		FuelModelDialog dialog( this, lid, "fuelModelDialog" );
		if ( dialog.exec() != QDialog::Accepted )
		{
			return;
		}
		// Get and display the entered values.
		dialog.resultString( qStr );
	}
	else
	{
		// Create and display the Guide Dialog.
		GuideDialog dialog( this, lid, "guideDialog" );
		if ( dialog.exec() != QDialog::Accepted )
		{
			return;
		}
		// Get and display the entered values.
		dialog.resultString( qStr );
	}
    m_entry[lid]->setText( qStr );

    // Re-validate and take care of input masking (no tokens is ok here).
    int tokens=0;
    int position=0;
    int length=0;
    validateWorksheetEntry( lid, qStr, &tokens, &position, &length );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_leaf[] from BpDocuments.
 *
 *  \return Pointer to the input EqVar with \a lid input list index.
 */

EqVar *BpDocument::leaf( int lid ) const
{
    return( m_eqTree->m_leaf[ lid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_leafCount from BpDocuments.
 *
 *  \return Number of EqVars in the input list.
 */

int  BpDocument::leafCount( void ) const
{
    return( m_eqTree->m_leafCount );
}

//------------------------------------------------------------------------------
/*! \brief Loads the worksheetNotesText property into the notes widget.
 *
 *  Called only by open() after opening a run or worksheet file.
 */

void BpDocument::loadNotes( void )
{
    m_notes->clear();
    // Resize the notes and its font to match the scale.
    QFont textFont( property()->string( "worksheetTextFontFamily" ) );
    textFont.setPointSize( m_fontScaleSize );
    m_notes->setFont( textFont );
    m_notes->insert( property()->string( "worksheetNotesText" ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Invokes a the BpDocument maintenance menu.
 *
 *  Invoked by <Ctl>-<alt>-<m>.
 *
 *  Called only by DocScrollView::keyPressEvent().
 */

void BpDocument::maintenance( void )
{
    // Create the context menu and store its pointer as private data.
    m_maintenanceMenu = new QPopupMenu( 0, "m_maintenanceMenu" );
    Q_CHECK_PTR( m_maintenanceMenu );

    // Add the menu items.
    int mid;
    // Print dot file for the current release
    mid = m_maintenanceMenu->insertItem(
        "Print current release BehavePlus6.dot file",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 101 );
    // Print dot file for all releases
    mid = m_maintenanceMenu->insertItem(
        "Print full BehavePlus6.dot file",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 102 );

    // Print resource summary table
    mid = m_maintenanceMenu->insertItem(
        "Print resource summary table",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 200 );
    // Print function array
    mid = m_maintenanceMenu->insertItem(
        "Print function array",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 201 );
    // Print function dictionary
    mid = m_maintenanceMenu->insertItem(
        "Print function dictionary",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 202 );
    // Print variable array
    mid = m_maintenanceMenu->insertItem(
        "Print variable array",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 203 );
    // Print variable dictionary
    mid = m_maintenanceMenu->insertItem(
        "Print variable dictionary",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 204 );
    // Print property dictionary
    mid = m_maintenanceMenu->insertItem(
        "Print property dictionary",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 205 );
    // Print item list list
    mid = m_maintenanceMenu->insertItem(
        "Print discrete variable item list",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 206 );
    // Print translation dictionary
    mid = m_maintenanceMenu->insertItem(
        "Print translation dictionary",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 207 );
    // Print worksheet input list
    mid = m_maintenanceMenu->insertItem(
        "Print worksheet input variable list",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 301 );
    // Print worksheet output list
    mid = m_maintenanceMenu->insertItem(
        "Print worksheet output variable list",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 302 );
    // Print results
    mid = m_maintenanceMenu->insertItem(
        "Print run results",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 303 );
    // Print Pat's table
    mid = m_maintenanceMenu->insertItem(
        "Print Pat's table (all variables)",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 304 );
    // Print Pat's table
    mid = m_maintenanceMenu->insertItem(
        "Print Pat's table (user visible variables only)",
        this, SLOT( maintenanceMenuActivated( int ) ) );
    m_maintenanceMenu->setItemParameter( mid, 305 );
    // Show the maintenance menu
    m_maintenanceMenu->exec( QCursor::pos() );
    delete m_maintenanceMenu;   m_maintenanceMenu = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback slot for the BpDocument's popup maintenance menu.
 *
 *  Invoked by <Ctl>-<alt>-<m>.
 */

void BpDocument::maintenanceMenuActivated( int id )
{
    // These write to their own file
    if ( id == 101 )
    {
        m_eqTree->printDotFile( "BehavePlus6.dot", m_eqTree->m_release );
        return;
    }
    if ( id == 102 )
    {
        m_eqTree->printDotFile( "BehavePlus6.dot", 99999 );
        return;
    }
    // These write to a shared file
    FILE *fptr;
    if ( ! ( fptr = fopen( "collin.txt", "a" ) ) )
    {
        return;
    }
    if ( id == 200 )
    {
        m_eqTree->printSummary( fptr );
    }
    else if ( id == 201 )
    {
        m_eqTree->printFunctions( fptr );
    }
    else if ( id == 202 )
    {
        m_eqTree->printFunDict( fptr );
    }
    else if ( id == 203 )
    {
        m_eqTree->printVariables( fptr );
    }
    else if ( id == 204 )
    {
        m_eqTree->printVarDict( fptr );
    }
    else if ( id == 205 )
    {
        m_eqTree->printPropertyDict( fptr );
    }
    else if ( id == 206 )
    {
        m_eqTree->printEqVarItemLists( fptr );
    }
    else if ( id == 207 )
    {
        printTranslationDict( fptr );
    }
    else if ( id == 301 )
    {
        m_eqTree->printInputs( fptr );
    }
    else if ( id == 302 )
    {
        m_eqTree->printOutputs( fptr );
    }
    else if ( id == 303 )
    {
        m_eqTree->printResults( fptr );
    }
    else if ( id == 304 )
    {
        m_eqTree->printPatsTable( fptr, true );
    }
    else if ( id == 305 )
    {
        m_eqTree->printPatsTable( fptr, false );
    }
    fclose( fptr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Prints the BpDocument file to a printer device.  A printer
 *  setup dialog appears first to allow user setup.
 *
 *  Called only by ApplicationWindow::slotDocumentPrint().
 *
 *  Re-implemented for BpDocument class.
 *
 *  \retval TRUE on success.
 *  \retval FALSE on failure or cancel.
 */

bool BpDocument::print( void )
{
    // Store the notes before printing.
    storeNotes();

    // If the worksheet has been edited since it was last composed,
    // it must be recomposed so new entry values are written to the sheet.
    // This doesn't quite work right here if the user has made an
    // unvalidated change to a field AND has results pages,
    // in which case the changed field is not printed correctly,
    // for to do so would blow away the results pages.
    if ( m_worksheetEdited || m_pages == m_worksheetPages )
    {
        // These statements are equivalent to calling regenerateWorksheet()
        // but avoid the unnecessary overhead of re-creating the EqTree.
        removeComposerFiles();
        m_pages = m_page = m_worksheetPages = 0;
        m_tocList->clear();
        composeWorksheet();
        // This is removed to allow printing blank or incomplete worksheets
        //if ( ! validateWorksheet() )
        //{
        //    return( false );
        //}
    }
    // Let Document do the rest of the work.
    return( Document::print() );
}

//------------------------------------------------------------------------------
/*! \brief Prints pages to a PostScript file.
 *
 *  \return TRUE on success, FALSE on failure or cancel.
 */

bool BpDocument::printPS( int fromPage, int thruPage )
{
    return( Document::printPS( fromPage, thruPage ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to the BpDocument's EqTree property dictionary.
 *
 *  \return Pointer to the BpDocument's EqTree property dictionary.
 */

PropertyDict *BpDocument::property( void ) const
{
    return( m_eqTree->m_propDict );
}

//------------------------------------------------------------------------------
/*! \brief Cleans up the existing worksheet and generates a new one.
 *
 *  Must be called whenever EqTree::reconfigure() is called, indicating:
 *      -   a new Document has been loaded via the File->Open |
 *          BpDocument::open() | EqTree::readFile() pipeline, or
 *      -   an existing Document has been reconfigured via a dialog.
 *
 *  This function does all the housekeeping work, and calls
 *  composeWorksheet() to do the actual drawing.
 */

void BpDocument::regenerateWorksheet( void )
{
    // Remove all the temporary composer files used by this Document.
    removeComposerFiles();

    // Now we can reset the page counter and clear the ToC.
    m_pages = m_page = m_worksheetPages = 0;
    m_tocList->clear();

    // Generate EqTree leaf and root list using the latest configuration.
    //m_eqTree->generateRootList( appWindow()->m_release );
    //m_eqTree->generateLeafList( appWindow()->m_release );

    // Make entry fields, buttons, and variable ptr arrays large enough.
    resizeWorksheet();

    // Reset the m_entry[]->text from the current leaf(id)->m_store
    getStores();
    for ( int lid = 0;
          lid < leafCount();
          lid++ )
    {
        m_entry[lid]->setCursorPosition( 0 );
        m_entry[lid]->home( false );
        // Do we need to reset the entry field enable/disable colors?
        m_entry[lid]->setPalette( m_entry[0]->palette() );
    }
    // Mask inputs as required.
    m_eqTree->m_eqCalc->maskInputs();
    grayInputs();

    // Redraw the worksheet and return.
    composeWorksheet();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Re-scales the BpDocument's fonts and screen display using
 *  the ratio of the new \a points to the default font point size as a
 *  scaling factor.
 *
 *  Re-implemented for BpDocument class.
 *
 *  \param points Font point size used for a scale factor.  A point size of
 *  12 is considered 100%.
 */

void BpDocument::rescale( int points )
{
    // Prevent validation checking
    m_doValidation = false;
    // Rescale the screen device logical pixel-to-inch sizes.
    double scale = (double) points / (double) m_fontBaseSize;
    m_screenSize->reset( QApplication::desktop(), m_pageSize, scale );

    // Re-scale the document's basic fonts.
    m_fontScaleSize = points;
    m_fixedFont.setPointSize( m_fontScaleSize );
    m_propFont.setPointSize( m_fontScaleSize );

    // Rescale the backing QPixmap in the scrollView.
    m_scrollView->rescale();

    // Re-display the current page in the new scale.
    showPage( m_page );
    m_doValidation = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Resizes the worksheet input widget arrays.
 *
 *  The arrays containing the entry field pointers, sizes, and locations are
 *  expanded as necessary to accommodate a reconfigured worksheet.
 *
 *  \par Note 1
 *  It appears that all the Document's QScrollView child widgets must be
 *  created and added during the first call, because adding some later doesn't
 *  seem to get them displayed.
 *
 *  \par Note 2
 *  For some reason, the MSC6 compiler complains about all the *[i] array
 *  references as being ambiguous overloaded operations (but ONLY within
 *  this function).  So I changed them all to *.at(i).
 */

void BpDocument::resizeWorksheet( void )
{
    // Get the current and new array sizes.
    unsigned int oldSize = m_guideBtn.size();
    unsigned int newSize = leafCount();
    unsigned int id;

    // m_entry.at(id)->hide() triggers a series of focus events which
    // eventually gets handled by BpDocEntry::event() and BpDocEntry::valid(),
    // which can cause a cascade of error messages if one of the input fields
    // has invalid data (because <return> wasn't pressed).  So set a flag.
    m_doValidation = false;

    // Do we need to add new entries?
    if ( oldSize < newSize )
    {
        // Resize the entry field widget and position arrays.
        m_entry.resize( newSize );
        m_entryPage.resize( newSize );
        m_entryX.resize( newSize );
        m_entryY.resize( newSize );
        m_entryHt.resize( newSize );
        m_entryWd.resize( newSize );

        // Resize the guide button widget and position arrays.
        m_guideBtn.resize( newSize );
        m_guideBtnX.resize( newSize );

        // Create the additional widgets.
        for ( id = oldSize;
              id < newSize;
              id++ )
        {
            // Create the additional m_entry[] widgets
            m_entry.at(id) = new BpDocEntry( this, id, m_scrollView->viewport(),
                QString( "m_entry[%1]" ).arg(id) );
            checkmem( __FILE__, __LINE__, m_entry.at(id),
                "BpDocEntry m_entry", id );
            m_entry.at(id)->setFocusPolicy( QWidget::StrongFocus );
            m_entry.at(id)->hide();
            // Initialize the m_entry positions.
            m_entryPage.at(id) = m_entryX.at(id) = m_entryY.at(id) = 0;

            // Create the additional m_guideBtn[] guide button widgets.
            m_guideBtn.at(id) = new QPushButton( m_scrollView->viewport(),
                QString( "m_guideBtn[%1]" ).arg(id) );
            Q_CHECK_PTR( m_guideBtn.at(id) );
            m_guideBtn.at(id)->hide();
            m_guideBtn.at(id)->setFocusPolicy( QWidget::ClickFocus );
            // Obsolete in QT3: m_guideBtn.at(id)->setAutoResize( true );
            m_guideBtnX.at(id) = 0;
            // Add a pixmap to each guide button and adjust its size.
            m_guideBtn.at(id)->setPixmap( m_guidePixmap ) ;
            m_guideBtnGrp->insert( m_guideBtn.at(id) );
        }
    }
    // Assign the tab order.
    //setTabOrder( m_entry.at(m_entry.size()-1), m_entry.at(0) );
    //for ( id = 1;
    //      id < m_entry.size();
    //      id++ )
    //{
    //    setTabOrder( m_entry.at(id-1), m_entry.at(id) );
    //}
    // Hide all the entry fields and buttons and resize buttons to pixmap
    for ( id = 0;
          id < m_entry.size();
          id++ )
    {
        m_entry.at(id)->hide();
        m_guideBtn.at(id)->setFixedSize( m_guidePixmap.width(),
            m_guidePixmap.height() );
        m_guideBtn.at(id)->hide();
    }
    // Start validation up
    m_doValidation = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_root[] from BpDocuments.
 *
 *  \return Pointer to the input EqVar with \a rid output list index.
 */

EqVar *BpDocument::root( int rid ) const
{
    return( m_eqTree->m_root[ rid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_rootCount from BpDocuments.
 *
 *  \return Number of EqVars in the output list.
 */

int  BpDocument::rootCount( void ) const
{
    return( m_eqTree->m_rootCount );
}

//------------------------------------------------------------------------------
/*! \brief resets the entry fields to their last file-saved values.
 */

void BpDocument::reset( bool /* showRunDialog */ )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Runs BehavePlus using the current worksheet inputs to calculate
 *  output tables, graphs, diagrams, and documentation,
 *  which is displayed in additional pages after the worksheet.
 *
 *  Invoked when the user
 *  - selects \b File->Calculate from the main menu,
 *  - presses the \b Calculate button on the Tool Bar, or
 *  - selects \b calculate from the popup context menu.
 *
 *  Called only by ApplicationWindow::slotDocumentRun() and
 *  contextMenuActivated().
 */

void BpDocument::run( bool showRunDialog )
{
    // Store the notes before running.
    storeNotes();
    // Run.
    int page = m_page;
    QString resultFile = appFileSystem()->tempFilePath( 1 );
    QString traceFile = appFileSystem()->tempFilePath( 2 );
    if ( runWorksheet( traceFile, resultFile, showRunDialog ) )
    {
        page = m_worksheetPages + 1;
    }
    // Show the first result page.
    showPage( page );
    // MUST setFocus() so the focus is not passed to the next Document!!
    setFocus();
    // Remove the log files and return.
    if ( property()->boolean( "appDeleteRunLogFile" ) )
    {
        m_eqTree->resultFileRemove();
        m_eqTree->traceFileRemove();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Computes and displays results for the current worksheet.
 *
 *  \param traceFile        Name of trace file to be written.
 *  \param showRunDialog    If TRUE, the Run Dialog and possibly the
 *                          Graph Limits Dialog will be shown.  This is set
 *                          to FALSE if -auto is specified.
 *
 *  The following tasks are performed:
 *      -# validates the worksheet inputs and stores them in the EqTree,
 *      -# displays the RunDialog (if necessary),
 *      -# displays the progress dialog ( if necessary),
 *      -# computes and display output tables, and
 *      -# computes and display output graphs.
 */

bool BpDocument::runWorksheet( const QString &traceFile,
        const QString &resultFile, bool showRunDialog )
{
    // Validate worksheet entries and store them in the EqTree
    if ( ! validateWorksheet() )
    {
        return( false );
    }
    // Determine the range case.
    m_eqTree->rangeCase();

    // If there are no rangeVars, make a simple run (no graphs) and return.
    if ( m_eqTree->m_rangeVars == 0 )
    {
        // Generate all the answers in an optimal manner.
        if ( ! m_eqTree->runTable( traceFile, resultFile ) )
        {
            return( false );
        }
        // Store the run time and reset the worksheet.
        setRunTime();
        regenerateWorksheet();
        // Compose the results table.
        composeTable1();
        composeDiagrams();
        if ( property()->boolean( "worksheetShowUsedChoices" ) )
        {
            composeDocumentation();
        }
        m_eqTree->runClean();
        return( true );
    }

    // There is at least one range variable.
    // Display the Run Dialog (if requested; -auto will disable it),
    // which may change the rangeVar[] order.
    if ( showRunDialog )
    {
        RunDialog dialog( this, "runDialog" );
        if ( dialog.exec() != QDialog::Accepted )
        {
            return( false );
        }
        dialog.store();
        dialog.hide();
        m_scrollView->viewport()->update();
        qApp->processEvents();
    }
    // As of now, we need to redraw the worksheet
    bool drawWorksheet = true;
    bool tableVarsSwapped = false;

    // Only calculate tables if they are requested.
    if ( property()->boolean( "tableActive" ) )
    {
        // Check if we need to swap the row and column variables.
        if ( m_eqTree->m_rangeVars == 2
          && property()->boolean( "tableRowUseVar2" ) )
        {
            EqVar *tmp = m_eqTree->m_rangeVar[0];
            m_eqTree->m_rangeVar[0] = m_eqTree->m_rangeVar[1];
            m_eqTree->m_rangeVar[1] = tmp;
            tableVarsSwapped = true;
        }
        // Re-determine the range case.
        m_eqTree->rangeCase();

        // Generate all the answers in an optimal manner.
        if ( ! m_eqTree->runTable( traceFile, resultFile ) )
        {
            return( false );
        }
        // Store the run time and redisplay the worksheet.
        setRunTime();
        regenerateWorksheet();
        // Ok, the worksheet was redrawn
        drawWorksheet = false;

        // One range variable produces one table with output variable columns.
        if ( m_eqTree->m_rangeVars == 1 )
        {
            composeTable2( m_eqTree->m_rangeVar[0] );
        }
        // Two range variables produces a table for each output variable.
        else if ( m_eqTree->m_rangeVars == 2 )
        {
            composeTable3( m_eqTree->m_rangeVar[0], m_eqTree->m_rangeVar[1] );
        }

        // Summary and weighted tables are drawn next.
        // m_eqTree->m_eqCalc->weightedSpread( this, true, true );

        // Finally, draw any requested figures.
        composeDiagrams();
    }
	// V5.0.5 - Always generate the HTML run input table for later export
	else
	{
		// Attempt to open the html file
		QString fileName = appFileSystem()->composerPath()
			+ "/" + property()->string( "exportHtmlFile" );
		FILE *fptr = 0;
		if ( ( fptr = fopen( fileName, "w" ) ) )
		{
			composeTableHtmlHeader( fptr );
		    fprintf( fptr,
				"<p class=\"bp2\">\n"
				"  <h3 class=\"bp2\">Only Graph Output Was Selected</h3>\n"
				"</p>\n" );
			composeTableHtmlFooter( fptr );
			fclose( fptr );
		}
	}

    // Graphs!!!
    if ( property()->boolean( "graphActive" ) )
    {
        // Case 2: 1 continuous and 0 discrete range variables.
        if ( m_eqTree->m_rangeCase == 2 )
        {
            // Calculate the graph values.
            if ( m_eqTree->runTable( "", "", true ) )
            {
                // Compose the worksheet if it hasn't already been composed.
                if ( drawWorksheet )
                {
                    setRunTime();
                    regenerateWorksheet();
                    drawWorksheet = false;
                }
                // Compose the graph.
                composeGraphs( true, showRunDialog );
            }
        }
        // Case 3: 0 continuous and 1 discrete range variables.
        else if ( m_eqTree->m_rangeCase == 3 )
        {
            // If necessary, run tables, not graphs!
            if ( property()->boolean( "tableActive" )
              || m_eqTree->runTable( "", false ) )
            {
                // Compose the worksheet if it hasn't already been composed.
                if ( drawWorksheet )
                {
                    setRunTime();
                    regenerateWorksheet();
                    drawWorksheet = false;
                }
                // Compose the graph.
                composeGraphs( false, showRunDialog );
            }
        }
        // Case 4: 2 continuous and 0 discrete range variables.
        else if ( m_eqTree->m_rangeCase == 4 )
        {
            // Either continuous variable may be the graph x or z variables.
            if ( property()->boolean( "graphXVar1" ) && tableVarsSwapped
              || property()->boolean( "graphXVar2" ) && ! tableVarsSwapped )
            {
                EqVar *tmp = m_eqTree->m_rangeVar[0];
                m_eqTree->m_rangeVar[0] = m_eqTree->m_rangeVar[1];
                m_eqTree->m_rangeVar[1] = tmp;
                m_eqTree->rangeCase();
            }
            // Calculate the graph values.
            if ( m_eqTree->runTable( "", "", true ) )
            {
                // Compose the worksheet if it hasn't already been composed.
                if ( drawWorksheet )
                {
                    setRunTime();
                    regenerateWorksheet();
                    drawWorksheet = false;
                }
                // Compose the graph.
                composeGraphs( true, showRunDialog );
            }
        }
        // Case 5: 0 continuous and 2 discrete range variables.
        else if ( m_eqTree->m_rangeCase == 5 )
        {
            // Compose the worksheet if it hasn't already been composed.
            if ( drawWorksheet )
            {
                setRunTime();
                regenerateWorksheet();
                drawWorksheet = false;
            }
            // No graphs for standard 2 multi-valued discrete variables.
            /* NOTHING */ ;
        }
        // Case 6: 1 continuous and 1 discrete range variables.
        else if ( m_eqTree->m_rangeCase == 6 )
        {
            // The continuous variable MUST be the graph x variable
            // and the discrete variable MUST be the graph z variable.
            if ( m_eqTree->m_rangeVar[0]->isDiscrete() )
            {
                EqVar *tmp = m_eqTree->m_rangeVar[0];
                m_eqTree->m_rangeVar[0] = m_eqTree->m_rangeVar[1];
                m_eqTree->m_rangeVar[1] = tmp;
                m_eqTree->rangeCase();
            }
            // Calculate the graph values.
            if ( m_eqTree->runTable( "", "", true ) )
            {
                // Compose the worksheet if it hasn't already been composed.
                if ( drawWorksheet )
                {
                    setRunTime();
                    regenerateWorksheet();
                    drawWorksheet = false;
                }
                // Compose the graph.
                composeGraphs( true, showRunDialog );
            }
        }
    }

    // Display discrete variables
    if ( property()->boolean( "worksheetShowUsedChoices" ) )
    {
        composeDocumentation();
    }

    // Free the EqTree run resources.
    m_eqTree->runClean();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Sets the document's focus to the correct entry field.
 */

void BpDocument::setFocus( void )
{
    Document::setFocus();
    if ( m_entry.count() > 0
      && m_entry[m_focusEntry] )
    {
        m_entry[m_focusEntry]->setFocus();
        m_entry[m_focusEntry]->home( false );
    }
    qApp->processEvents();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the document's page tab options.
 *
 *  Called only by configureAppearance() and open().
 */

void BpDocument::setPageTabs( void )
{
    m_tabs->setFont(
        property()->string( "tabFontFamily" ),
        property()->integer( "tabFontSize" ),
        property()->color( "tabFontColor" ) );

    m_tabs->setTab(
        property()->integer( "tabPosition" ),
        property()->integer( "tabTabsPerPage" ),
        property()->string( "tabText" ) );

    m_tabs->setActive( property()->boolean( "tabActive" ) );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays the requested page.
 *
 *  Each page was previously drawn to a file by a Composer.  This function
 *  merely loads the page from the file and draws it onto the scrollview at
 *  the current scale.  All widgets appearing on the page are then scaled and
 *  overlaid on the scrollview.
 *
 *  Virtual method re-implemented to hide or show the entry fields and guide
 *  buttons before chaining to Document::showPage().
 *
 *  Usually called by the ApplicationWindow navigation slots.
 *
 *  \param pageNumber Number of the page to be displayed.
 */

void BpDocument::showPage( int pageNumber )
{
    // Determine current view scale.
    double scale = (double) m_fontScaleSize / (double) m_fontBaseSize;

    // Set up the entry font size for this scale.
    QFont entryFont( property()->string( "worksheetEntryFontFamily" ) );
    entryFont.setPointSize( m_fontScaleSize );

    // First hide all the widgets.
    // NOTE m_entry.at(lid)->hide() triggers a call to BpDocEntry::valid(),
    // which calls validateWorksheetEntry() which updates the m_worksheetEdited
    // and will trim results pages before drawing the next page.
    m_doValidation = false;
    int lid;
    for ( lid = 0;
          lid < (int) m_guideBtn.size();
          lid++ )
    {
        m_scrollView->moveChild( m_guideBtn.at(lid),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_scrollView->moveChild( m_entry.at(lid),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_guideBtn.at(lid)->hide();
		m_entry.at(lid)->m_validate = false;	// Stop any error message displays
        m_entry.at(lid)->hide();
		m_entry.at(lid)->m_validate = true;
    }
    // Hide all the RxVars
    RxVar *rxVar = 0;
    int rxId = 0;
    for ( rxVar = m_eqTree->m_rxVarList->first();
          rxVar;
          rxVar = m_eqTree->m_rxVarList->next(), rxId++ )
    {
        m_scrollView->moveChild( m_rxCheckBox.at(rxId),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_scrollView->moveChild( m_rxMinEntry.at(rxId),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_scrollView->moveChild( m_rxMaxEntry.at(rxId),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_rxCheckBox.at(rxId)->hide();
        m_rxMinEntry.at(rxId)->hide();
        m_rxMaxEntry.at(rxId)->hide();
    }
    int rxItems = m_eqTree->m_rxVarList->items();
    int item;
    for ( item=0; item<rxItems; item++ )
    {
        m_scrollView->moveChild( m_rxItemBox.at(item),
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_rxItemBox.at(item)->hide();
    }

    // moved to end of the method 
	//m_doValidation = true;

    // Results pages may have been trimmed!
    if ( pageNumber > m_pages )
    {
        pageNumber = m_pages;
    }
    // Show only the entry fields and guide buttons for this page.
    int x1, x2, x3, y1, y2, y3;
    composeGuideButtonPixmap(
        (int) ( scale * (double) m_guideBtnWd ),
        (int) ( scale * (double) m_guideBtnHt ) );
    QWidget *prevWidget = 0;
    for ( lid = 0;
          lid < (int) leafCount();
          lid++ )
    {
        if ( m_entryPage.at(lid) == pageNumber )
        {
            // Resize the guide button and entry field to match the scale.
            // Note that showPage() is called after every View change,
            // even though composeWorksheet() is not, so scale must be applied.
            m_guideBtn.at(lid)->setPixmap( m_guidePixmap ) ;
            m_guideBtn.at(lid)->setFixedSize(
                (int) ( scale * (double) m_guideBtnWd ),
                (int) ( scale * (double) m_guideBtnHt ) );
            m_entry.at(lid)->setFixedSize(
                (int) ( scale * (double) m_entryWd.at(lid) ),
                (int) ( scale * (double) m_entryHt.at(lid) ) );
            m_entry.at(lid)->setFont( entryFont );

            // Move the guide button and entry field into position.
            x1 = (int) ( scale * (double) m_guideBtnX.at(lid) );
            y1 = (int) ( scale * (double) m_entryY.at(lid) );
            m_scrollView->moveChild( m_guideBtn.at(lid), x1, y1 );

            x2 = (int) ( scale * (double) m_entryX.at(lid) );
            y2 = (int) ( scale * (double) m_entryY.at(lid) );
            m_scrollView->moveChild( m_entry.at(lid), x2, y2 );

            // Show the guide button and entry field.
            m_guideBtn.at(lid)->show();
            m_entry.at(lid)->show();

            // Set tab order
            if ( prevWidget )
            {
                setTabOrder( prevWidget, m_entry.at(lid) );
            }
            prevWidget = m_entry.at(lid);
        }
    }

    // Position the fuel initialization button if its on this page.
    if ( pageNumber == m_btnPage[0]
      && property()->boolean( "surfaceModuleActive" )
      && property()->boolean( "surfaceConfFuelParms" ) )
    {
        // Resize the button to match the scale.
        m_btn[0]->setFixedSize(
            (int) ( scale * (double) m_btnWd[0] ),
            (int) ( scale * (double) m_btnHt[0] ) );
        m_btn[0]->setFont( m_propFont );
        // Move the button into position and show it.
        m_scrollView->moveChild( m_btn[0],
            (int) ( scale * (double) m_btnX[0] ),
            (int) ( scale * (double) m_btnY[0] ) );
        m_btn[0]->show();
    }
    else
    // Otherwise hide the initialization button.
    {
        m_scrollView->moveChild( m_btn[0],
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_btn[0]->hide();
    }
    
    // Display any RxVars on this page
    if ( property()->boolean( "tableShading" ) )
    {
        for ( rxVar = m_eqTree->m_rxVarList->first(), rxId=0;
              rxVar;
              rxVar = m_eqTree->m_rxVarList->next(), rxId++ )
        {
            // Must be a user output on this page
            EqVar *rxPtr = rxVar->m_varPtr;
            if ( rxPtr->m_isUserOutput )
            {
                if ( m_rxEntryPage.at(rxId) == pageNumber )
                {
                    // Move activation checkbox into position
                    x1 = (int) ( scale * (double) m_guideBtnX.at(leafCount()-1) );
                    x2 = (int) ( scale * (double) m_entryX.at(leafCount()-1) );
                    x3 = (int) ( scale * (double) m_rxEntryX.at(rxId) );
                    y3 = (int) ( scale * (double) m_rxEntryY.at(rxId) );
                    m_scrollView->moveChild( m_rxCheckBox.at(rxId), x1, y3 );
                    m_rxCheckBox.at(rxId)->show();
                    setTabOrder( prevWidget, m_rxCheckBox.at(rxId) );
                    prevWidget = m_rxCheckBox.at(rxId);
                    // Move continuous minimum-maximum widgets into position
                    if ( rxPtr->isContinuous() )
                    {
                        // Resize the entry fields
                        m_rxMinEntry.at(rxId)->setFixedSize(
                            (int) ( scale * (double) m_rxEntryWd.at(rxId) ),
                            (int) ( scale * (double) m_rxEntryHt.at(rxId) ) );
                        m_rxMinEntry.at(rxId)->setFont( entryFont );

                        m_rxMaxEntry.at(rxId)->setFixedSize(
                            (int) ( scale * (double) m_rxEntryWd.at(rxId) ),
                            (int) ( scale * (double) m_rxEntryHt.at(rxId) ) );
                        m_rxMaxEntry.at(rxId)->setFont( entryFont );
                        // Move entry fields into position
                        m_scrollView->moveChild( m_rxMinEntry.at(rxId), x2, y3 );
                        m_scrollView->moveChild( m_rxMaxEntry.at(rxId), x3, y3 );
                        // Show the guide button and entry field.
                        m_rxMinEntry.at(rxId)->show();
                        m_rxMaxEntry.at(rxId)->show();
                        // Tab order
                        setTabOrder( prevWidget, m_rxMinEntry.at(rxId) );
                        setTabOrder( m_rxMinEntry.at(rxId), m_rxMaxEntry.at(rxId) );
                        prevWidget = m_rxMaxEntry.at(rxId);
                    }
                    // Move discrete checkboxes into position
                    else
                    {
                        int items  = rxVar->items();
                        int atItem = rxVar->m_firstItemBox;
                        for ( item=0; item<items; item++, atItem++ )
                        {
                            m_rxItemBox.at(atItem)->setFixedSize(
                                (int) ( scale * (double) 2.*m_rxEntryWd.at(rxId) ),
                                (int) ( scale * (double) m_rxEntryHt.at(rxId) ) );
                            y3 = (int) ( scale * (double) m_rxItemY.at(atItem) );
                            m_scrollView->moveChild( m_rxItemBox.at(atItem),
                                x2, y3 );
                            m_rxItemBox.at(atItem)->show();
                            setTabOrder( prevWidget, m_rxItemBox.at(atItem) );
                            prevWidget = m_rxItemBox.at(atItem);
                        }   // for
                    }   // end continuous else discrete
                }   // end if on this page
            }   // end if user output
        }   // next RxVar
    }   // end if table shading

    // Display the notes widget if its on this page.
    if ( pageNumber == m_notesPage
      && property()->boolean( "worksheetNotesActive" ) )
    {
        storeNotes();
        m_notes->setFixedSize(
            (int) ( scale * (double) m_notesWd ),
            (int) ( scale * (double) m_notesHt ) );
        loadNotes();
        // Move into position and show it.
        m_scrollView->moveChild( m_notes,
            (int) ( scale * (double) m_notesX ),
            (int) ( scale * (double) m_notesY ) );
        m_notes->show();
        // Tab order
        setTabOrder( prevWidget, m_notes );
        prevWidget = m_notes;
    }
    // Otherwise hide the notes widget.
    else
    {
        m_scrollView->moveChild( m_notes,
            -m_screenSize->m_pageWd, -m_screenSize->m_pageHt );
        m_notes->hide();
    }

    // Complete the tab order
    //setTabOrder( prevWidget, m_entry.at(0) );

    // Show the composed page in the scrollview and return.
    Document::showPage( pageNumber );
    m_entry.at(m_focusEntry)->setFocus();
	m_doValidation = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience function that stores current entry values into their
 *  variable.m_store prior to calling reconfigure(), which pulls them back
 *  from the m_store into the entry field.
 *
 *  This circumvents the case where a user edits an entry field,
 *  doesn't hit <enter>, but selects one of the Configure menu items,
 *  and after clicking Ok, the edits are gone because the entry field was
 *  refreshed from the m_store.
 *
 *  Called by configure() in response to a call by configureAppearance(),
 *  configureModules(), configureUnits(),  open(), CheckInputDialog1::store(),
 *  or CheckInputDialog2::store()
 */

void BpDocument::storeEntries( void )
{
    // Save the worksheet entry values
    int tokens, position, length;
	int leaf_count = leafCount();
    for ( int lid = 0;
          lid < leaf_count;
          lid++ )
    {
        if ( m_entry[lid]->edited() )
        {
            // Store the contents in the EqVar store.
            leaf(lid)->m_store = m_entry[lid]->text();

            // Validate the store.
            if ( ! leaf(lid)->isValidStore( &tokens, &position, &length ) )
            {
                // If invalid contents, reposition the focus and return.
                focusThis( lid, position, length );
                return;
            }
            m_entry[lid]->setEdited( false );
        }
    }

    // Save prescription variable entries
    if ( property()->boolean( "tableShading" ) )
    {
        RxVar *rxVar = 0;
        RxVar *rxPair = 0;
        int rxId = 0;
        for ( rxVar = m_eqTree->m_rxVarList->first();
              rxVar;
              rxVar = m_eqTree->m_rxVarList->next(), rxId++ )
        {
            // Must be a user output
            EqVar *rxPtr = rxVar->m_varPtr;
            if ( rxPtr->m_isUserOutput )
            {
                // Store activation check
                rxVar->m_isActive = m_rxCheckBox[rxId]->isChecked();
                // Store continuous variable acceptable min-max range
                if ( rxPtr->isContinuous() )
                {
                    rxVar->storeMinMax( m_rxMinEntry[rxId]->text(),
                        m_rxMaxEntry[rxId]->text() );
                    // HACK to keep *AtHead and *AtVector inputs in sync
                    EqCalc *eqCalc = m_eqTree->m_eqCalc;
                    if ( rxPtr == eqCalc->vSurfaceFireSpreadAtHead )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireSpreadAtVector );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                    else if ( rxPtr == eqCalc->vSurfaceFireSpreadAtVector )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireSpreadAtHead );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                    if ( rxPtr == eqCalc->vSurfaceFireLineIntAtHead )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireLineIntAtVector );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                    else if ( rxPtr == eqCalc->vSurfaceFireLineIntAtVector )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireLineIntAtHead );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                    if ( rxPtr == eqCalc->vSurfaceFireFlameLengAtHead )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireFlameLengAtVector );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                    else if ( rxPtr == eqCalc->vSurfaceFireFlameLengAtVector )
                    {
                        rxPair = m_eqTree->m_rxVarList->find(
                            eqCalc->vSurfaceFireFlameLengAtHead );
                        rxPair->m_isActive = m_rxCheckBox[rxId]->isChecked();
                        rxPair->storeMinMax( m_rxMinEntry[rxId]->text(),
                            m_rxMaxEntry[rxId]->text() );
                    }
                }
                // Store acceptable discrete variable items
                else
                {
                    int nItems = rxVar->items();
                    int rxItem = rxVar->m_firstItemBox;
                    for ( int item=0; item<nItems; item++, rxItem++ )
                    {
                        rxVar->m_itemChecked[item] =
                            m_rxItemBox[rxItem]->isChecked();
                    }
                }
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Stores the current notes contents (if active) in the property.
 *
 *  Called only by the run(), print(), and save() functions.
 */

void BpDocument::storeNotes( void )
{
    // Only store if notes are active.
    if ( property()->boolean( "worksheetNotesActive" ) )
    {
        property()->string( "worksheetNotesText", m_notes->text() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableCol[] from BpDocuments.
 *
 *  \return Value of the table column variable with the specified index.
 */

double BpDocument::tableCol( int vid ) const
{
    return( m_eqTree->m_tableCol[ vid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableCols from BpDocuments.
 *
 *  \return Number of output table columns.
 */

int BpDocument::tableCols( void ) const
{
    return( m_eqTree->m_tableCols );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableRow[] from BpDocuments.
 *
 *  \return Value of the table row variable with the specified index.
 */

double BpDocument::tableRow( int vid ) const
{
    return( m_eqTree->m_tableRow[ vid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableRows from BpDocuments.
 *
 *  \return Number of output table rows.
 */

int BpDocument::tableRows( void ) const
{
    return( m_eqTree->m_tableRows );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableVal[] from BpDocuments.
 *
 *  \return Output table value.
 */

bool BpDocument::tableInRx( int cell ) const
{
    return( m_eqTree->m_tableInRx[ cell ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableVal[] from BpDocuments.
 *
 *  \return Output table value.
 */

double BpDocument::tableVal( int vid ) const
{
    return( m_eqTree->m_tableVal[ vid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableVar[] from BpDocuments.
 *
 *  \return Pointer to the output table variable with the specified index.
 */

EqVar *BpDocument::tableVar( int vid ) const
{
    return( m_eqTree->m_tableVar[ vid ] );
}

//------------------------------------------------------------------------------
/*! \brief Convenience routine to access EqTree::m_tableVars from BpDocuments.
 *
 *  \return Number of output table variables.
 */

int BpDocument::tableVars( void ) const
{
    return( m_eqTree->m_tableVars );
}

//------------------------------------------------------------------------------
/*! \brief Stores the current m_entry[] text in their corresponding
 *  EqVar::m_stores, then calls EqTree->validate() to verify that:
 *      -# entry text is valid,
 *      -# all required inputs are present,
 *      -# there are no more than two range variables, and
 *      -# EqCalc::validate() passes all cross-validation tests.
 *
 *  If the validation fails for an input variable, the focus is repositioned
 *  to the offending m_entry[] field and position.
 *
 *  \retval TRUE if the worksheet passes muster.
 *  \retval FALSE if the worksheet contains invalid or incomplete entries.
 */

bool BpDocument::validateWorksheet( void )
{
    // Store all entry text in the EqVar m_stores before validating
    for ( int lid = 0;
          lid < m_eqTree->m_leafCount;
          lid++ )
    {
        m_eqTree->m_leaf[lid]->m_store = m_entry[lid]->text();
    }

    // Store all RxVar checkbox and entry text
    if ( property()->boolean( "tableShading" ) )
    {
        RxVar *rxVar = 0;
        int rxId = 0;
        for ( rxVar = m_eqTree->m_rxVarList->first();
              rxVar;
              rxVar = m_eqTree->m_rxVarList->next(), rxId++ )
        {
            // Must be a user output
            EqVar *rxPtr = rxVar->m_varPtr;
            if ( rxPtr->m_isUserOutput )
            {
                // Store activation check
                rxVar->m_isActive = m_rxCheckBox[rxId]->isChecked();
                // Store continuous variable acceptable min-max range
                if ( rxPtr->isContinuous() )
                {
                    rxVar->storeMinMax( m_rxMinEntry[rxId]->text(),
                        m_rxMaxEntry[rxId]->text() );
                }
                // Store acceptable discrete variable items
                else
                {
                    int nItems = rxVar->items();
                    int rxItem = rxVar->m_firstItemBox;
                    for ( int item=0; item<nItems; item++, rxItem++ )
                    {
                        rxVar->m_itemChecked[item] =
                            m_rxItemBox[rxItem]->isChecked();
                    }
                }
            }
        }
    }

    // Validate the entries
    // Note that EqTree->validateInputs() calls EqCalc->validateInputs().
    int result, badLid, badPos, badLeng;
    if ( ( result = m_eqTree->validateInputs( &badLid, &badPos, &badLeng ) ) < 0 )
    {
        // Invalid entry
        if ( result == -1 )
        {
            focusThis( badLid, badPos, badLeng );
        }
        // Missing entry
        else if ( result == -2 )
        {
            focusThis( badLid, 0, 0 );
        }
        // Too many range variables
        else if ( result == -3 )
        {
            // An error message was already displayed by EqTree::validateInputs()
            return( false );
        }
        // Cross-validation errors
        else if ( result == -4 )
        {
            // A warning message was already displayed by EqCalc::validateInputs()
            return( false );
        }
        return( false );
    }

    // Validate the RxVar entries
    int badRx;
    if ( ( result = m_eqTree->validateRxInputs( &badRx ) ) < 0 )
    {
        // Missing or invalid minimum entry
        if ( result == -1 || result == -3 )
        {
            focusRx( badRx, m_rxMinEntry[badRx] );
        }
        // Missing or invalid maximum entry
        else if ( result == -2 || result == -4 )
        {
            focusRx( badRx, m_rxMaxEntry[badRx] );
        }
        return( false );
    }

    // Catch the EqTree cross-checks that we can let the user fix.
	// NOTE: THIS IS NOT POSSIBLE IN VERSIONS 601 and 602
    if ( m_eqTree->m_eqCalc->conflict1() )
    {
        Conflict1Dialog dialog( this, "conflict1Dialog" );
		dialog.exec();
		// Always return false to force reconfiguration (See Issue #064)
		return false;
        //if ( dialog.exec() != QDialog::Accepted )
        //{
        //    return( false );
        //}
    }
    if ( m_eqTree->m_eqCalc->conflict2() )
    {
        Conflict2Dialog dialog( this, "conflict2Dialog" );
        if ( dialog.exec() != QDialog::Accepted )
        {
            return( false );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Validates a single worksheet entry field
 *  and displays error messages as appropriate.
 *
 *  Called by BpDocEntry::valid() after certain keypress events
 *  detected by BpDocEntry::event() and if the field isEdited().
 *
 *  Also called by BpDocument::guideClicked() after the GuideDialog()
 *  has been applied.
 *
 *  \param lid Leaf variable index.  A pointer to the leaf variable is
 *  returned by a call to leaf( lid ).
 *
 *  \param text Reference to the variable's entry text to be validate.
 *
 *  \param tokens Pointer to storage where the number of parsed tokens
 *  is returned.
 *
 *  \param position Pointer to storage where the start position in \a text
 *  of the first bad token is returned.
 *
 *  \param length Pointer to storage where the length of the invalid token
 *  is returned.
 *
 *  \retval TRUE if the entry field is valid.
 *  \retval \a tokens contains the number of valid tokens.
 *
 *  \retval FALSE if the entry field is invalid and:
 *  \retval \a tokens contains the bad token index (base 1).
 *  \retval \a position contains the position (base 0) of token's first char
 *  in \a text.
 *  \retval \a length contains the token length.
 */

bool BpDocument::validateWorksheetEntry( int lid, const QString &text,
    int *tokens, int *position, int *length )
{
    // Store the contents.
	int n = lid;
    leaf(lid)->m_store = text;

    // Validate the store.
    if ( ! leaf(lid)->isValidStore( tokens, position, length ) )
    {
        return( false );
    }
    // Since this field is now edited, remove any results pages.
    m_worksheetEdited = true;
    if ( m_pages > m_worksheetPages )
    {
        removeComposerFiles( m_worksheetPages + 1 );
        m_pages = m_worksheetPages;
        m_tocList->clear();
        QString ptext;
        translate( ptext, "BpDocument:InputPage" );
        for ( int page = 1;
              page <= m_pages;
              page++ )
        {
            contentsAddItem( page,
                QString( "%1 %2").arg( ptext ).arg( page ),
                TocInput );
        }
        statusUpdate();
    }
    // Do not check for masked, zero, or multiple tokens here,
    // since runWorksheetValiadation() handles that.

    // Gray out unneeded fuel moisture input variables.
    m_eqTree->m_eqCalc->maskInputs( leaf(lid) );
    grayInputs();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Creates the ApplicationWindow's \b View submenu options just
 *  before it displays itself.  This is necessary so th BpDocument's
 *  current scale setting will be checked in the View menu.
 *
 *  Called only by ApplicationWindow::slotViewMenuAboutToShow().
 *
 *  \param viewMenu Pointer to the \b View menu.
 */

void BpDocument::viewMenuAboutToShow( QPopupMenu *viewMenu )
{
    Document::viewMenuAboutToShow( viewMenu );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Public function is a hack to get around the protected slot call
 *  to BehavePlusDocument::guideClicked().
 *
 *  Displays a popup menu of guide options.
 *  Called only by DocEntry::mousePressEvent().
 *
 *  \param lid Leaf variable index.  A pointer to the leaf variable is
 *  returned by a call to leaf( lid ).
 */

void BpDocument::worksheetEntryClicked ( int lid )
{
    guideClicked( lid );
    return;
}

//------------------------------------------------------------------------------
//  End of bpdocument.cpp
//------------------------------------------------------------------------------

