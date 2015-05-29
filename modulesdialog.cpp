//------------------------------------------------------------------------------
/*! \file modulesdialog.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2015 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus Module Selection dialog methods.
 *
 *  Invoked by the \b Configure->Modules menu selection or the \b Modules
 *  tool bar button.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "bpdocument.h"
#include "module.h"
#include "modulesdialog.h"
#include "propertydialog.h"
#include "property.h"
#include "varcheckbox.h"
#include "xeqapp.h"

// Qt include files
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtextbrowser.h>

//------------------------------------------------------------------------------
/*! \brief BehavePlus modules selection dialog constructor.
 */

ModulesDialog::ModulesDialog( BpDocument *bp, const QString &captionKey,
        const char *name ) :
    AppDialog( bp, captionKey,
        "Wildfire1.png", "Wildfire", "moduleSelection.html", name ),
    m_bp(bp),
    m_moduleList(0),
    m_gridFrame(0),
    m_gridLayout(0),
    m_mapFrame(0),
    m_mapCheckBox(0),
    m_tableShadingCheckBox(0),
    m_guideBtnGrp(0),
    m_cb(0),
    m_pb(0),
    m_modules(0)
{
    // Create arrays to hold ptrs to all module checkboxes and pushbuttons
    m_moduleList = appWindow()->m_eqApp->m_moduleList;
    m_modules = m_moduleList->count();
    m_cb = new QCheckBox *[ m_modules ];
    checkmem( __FILE__, __LINE__, m_cb, "QCheckBox *m_cb", m_modules );
    m_pb = new QPushButton *[ m_modules ];
    checkmem( __FILE__, __LINE__, m_pb, "QPushButton *m_pb", m_modules );

    // Find maximum indentation level
    Module *mod = 0;
    int maxIndent = 0;
    for ( mod = m_moduleList->first();
          mod != 0;
          mod = m_moduleList->next() )
    {
        if ( mod->m_indent > maxIndent )
        {
            maxIndent = mod->m_indent;
        }
    }
    int lastCol = maxIndent + 3;    // Allow 3 columns for module text
    int cols = lastCol + 2;         // Allow 1 column for pushbutton

    // Frame to outline the module checkboxes
    m_gridFrame = new QFrame( contentFrame(), "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );

    // Layout to hold the module checkboxes, labels, and option buttons
    m_gridLayout = new QGridLayout( m_gridFrame, m_modules, cols, 10, 2,
        "m_gridLayout" );
    Q_CHECK_PTR( m_gridLayout );

    // Create the guide button group and connect its shared callback slot.
    m_guideBtnGrp = new QButtonGroup( this, "m_guideBtnGroup" );
    Q_CHECK_PTR( m_guideBtnGrp );
    connect( m_guideBtnGrp,  SIGNAL( clicked(int) ),
             this,           SLOT( slotOptions(int) ) );
    m_guideBtnGrp->hide();

    // Create widgets for each module
    QString widgetName(""), buttonLabel("");
    translate( buttonLabel, "ModulesDialog:Option:Label" );
    QString labelKey(""), text("");
    int row = 0;
    for ( mod = m_moduleList->first();
          mod != 0;
          mod = m_moduleList->next() )
    {
        // Create the checkbox
        widgetName = "m_" + mod->m_name + "CheckBox";
        m_cb[row] = new QCheckBox( m_gridFrame, widgetName );
        Q_CHECK_PTR( m_cb[row] );
        // Add its label
        labelKey = "ModulesDialog:" + mod->m_name + ":Label";
        translate( text, labelKey );
        m_cb[row]->setText( text );
        // Set the checkbox state
        QString propName( mod->m_name + "ModuleActive" );
        m_cb[row]->setChecked( m_bp->property()->boolean( propName ) );
        // Set the checkbox into the grid layout
        m_gridLayout->addMultiCellWidget( m_cb[row], row, row,
            mod->m_indent, lastCol, AlignLeft );
        // Create the options pushbutton
        widgetName = "m_" + mod->m_name + "PushButton";
        m_pb[row] = new QPushButton( buttonLabel, m_gridFrame, widgetName );
        Q_CHECK_PTR( m_pb[row] );
        m_gridLayout->addWidget( m_pb[row], row, lastCol+1 );
        m_guideBtnGrp->insert( m_pb[row] );
        // Only show modules for this release
        if ( ! mod->isCurrent( appWindow()->m_release ) )
        {
            m_cb[row]->hide();
            m_pb[row]->hide();
        }
        row++;
    }
    // MAP distances frame
    m_mapFrame = new QButtonGroup( 1, Qt::Horizontal,
        contentFrame(), "m_mapFrame" );
    Q_CHECK_PTR( m_mapFrame );

    // MAP distance checkbox
    translate( text, "ModulesDialog:MapDistance:Label" );
    m_mapCheckBox = new QCheckBox( text, m_mapFrame, "m_mapCheckBox" );
    Q_CHECK_PTR( m_mapCheckBox );
    m_mapCheckBox->setChecked( m_bp->property()->boolean( "mapCalcDist" ) );

    // Table shading checkbox
    translate( text, "ModulesDialog:TableShading:Label" );
    m_tableShadingCheckBox = new QCheckBox( text, m_mapFrame,
                "m_tableShadingCheckBox" );
    Q_CHECK_PTR( m_tableShadingCheckBox );
    m_tableShadingCheckBox->setChecked(
                m_bp->property()->boolean( "tableShading" ) );

    // Freeze the height of the middleBox;
    m_mapFrame->setFixedHeight( m_mapFrame->sizeHint().height() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

ModulesDialog::~ModulesDialog( void )
{
    delete m_tableShadingCheckBox;      m_tableShadingCheckBox = 0;
    delete m_mapCheckBox;               m_mapCheckBox = 0;
    delete m_mapFrame;                  m_mapFrame = 0;
    for ( int id = 0;
          id < m_modules;
          id++ )
    {
        delete m_cb[id];        m_cb[id] = 0;
        delete m_pb[id];        m_pb[id] = 0;
    }
    delete m_cb;                m_cb = 0;
    delete m_pb;                m_pb = 0;
    delete m_gridLayout;        m_gridLayout = 0;
    delete m_gridFrame;         m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Builds the output variable's translation key and help file name
 *  and passes it on to PropertyDialog::addCheck().
 *
 *  \return Pointer to the newly created VarCheckBox.
 */

VarCheckBox *ModulesDialog::addOutput( PropertyPage *page,
        const QString &propName, const QString &varName,
        int rowBeg, int colBeg, int rowEnd, int colEnd, bool wrapped )
{
    // Start with a hardwired html file name, but then look it up.
    QString htmlFile = varName + ".html";
    EqVar* varPtr = 0;
    if ( ( varPtr = m_bp->m_eqApp->m_varDict->find( varName ) ) != 0 )
    {
        htmlFile = varPtr->m_help;
    }
    QString key = varName + ":Label";
    // Hack to remove the "(maximum)" from the end of RoS label
    // or the "(from Upslope)" from the end of maximum direction label
    if ( key == "vSurfaceFireSpreadAtHead:Label"
      || key == "vSurfaceFireMaxDirFromUpslope:Label" )
    {
        key += ":OutputVariables";
    }
    // Hack to enable label wrapping
    if ( wrapped )
    {
        key += ":Wrapped";
    }
    return( page->addCheck( propName, key, htmlFile,
        rowBeg, colBeg, rowEnd, colEnd ) );
}

//------------------------------------------------------------------------------
/*! \brief Accept button callback.
 *  Stores the dialog settings into the document state variables.
 */

void ModulesDialog::store( void )
{
    Module *mod = 0;
    QString propName("");
    int row = 0;
    for ( mod = m_moduleList->first();
          mod != 0;
          mod = m_moduleList->next() )
    {
        propName = mod->m_name + "ModuleActive";
        m_bp->property()->boolean( propName, m_cb[row]->isChecked() );
        row++;
    }
    m_bp->property()->boolean( "mapCalcDist", m_mapCheckBox->isChecked() );
    m_bp->property()->boolean( "tableShading",
        m_tableShadingCheckBox->isChecked() );
    accept();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Options button callback.
 *  Displays the OptionsDialog for the requested Module.
 */

void ModulesDialog::slotOptions( int id )
{
    // Only process modules for this release
    Module *mod = m_moduleList->at( id );
    if ( mod->isCurrent( appWindow()->m_release ) )
    {
        if ( id == 0 )
        {
            surfaceOptions();
        }
        else if ( id == 1 )
        {
            crownOptions();
        }
        else if ( id == 2 )
        {
            safetyOptions();
        }
        else if ( id == 3 )
        {
            sizeOptions();
        }
        else if ( id == 4 )
        {
            containOptions();
        }
        else if ( id == 5 )
        {
            spotOptions();
        }
        else if ( id == 6 )
        {
            scorchOptions();
        }
        else if ( id == 7 )
        {
            mortalityOptions();
        }
        else if ( id == 8 )
        {
            ignitionOptions();
        }
        // Removed from Version 3
        //else if ( id == 9 )
        //{
        //    weatherOptions();
        //}
        else
        // This code block should never be executed!
        {
            QString text("");
            translate( text, "ModulesDialog:UnknownModuleId",
                QString( "%1" ).arg( id ) );
            bomb( text );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Contain Module's options dialog.
 */

void ModulesDialog::containOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Contain:Caption", "containDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );


    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Contain:Inputs:Tab", 1, 1,
                         "ForestServiceHistory.png",
                         "Forest Service History",
                         "containOptions.html" );
    // Contain options button box
    bg = p->addButtonGroup( "PropertyTabDialog:Contain:Inputs:Resources:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "containConfResourcesSingle",
                 "PropertyTabDialog:Contain:Inputs:Resources:Single",
                 bg );
    p->addRadio( "containConfResourcesMultiple",
                 "PropertyTabDialog:Contain:Inputs:Resources:Multiple",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    bg->setFixedWidth( bg->sizeHint().width() );
    
    // Contain options button box
    // DISABLED on 2008-03-06
    //bg = p->addButtonGroup( "PropertyTabDialog:Contain:Inputs:LimitDist:Caption",
    //                        1, 0, 1, 0 );
    //p->addRadio( "containConfLimitDistOff",
    //             "PropertyTabDialog:Contain:Inputs:LimitDist:Off",
    //             bg );
    //p->addRadio( "containConfLimitDistOn",
    //             "PropertyTabDialog:Contain:Inputs:LimitDist:On",
    //             bg );
    //bg->setFixedHeight( bg->sizeHint().height() );
    //bg->setFixedWidth( bg->sizeHint().width() );

    // \todo add wizards for vContainAttackDist and vContainResources* ?
    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Contain:Outputs:Tab", 1, 1,
                         "MontanaHistoryLesson.png",
                         "Montana History Lesson",
                         "selectOutput.html" );
    addOutput( p, "containCalcAttackSize",
                  "vContainAttackSize",
                  0, 0, 0, 0 );
    addOutput( p, "containCalcAttackPerimeter",
                  "vContainAttackPerimeter",
                  1, 0, 1, 0 );
    addOutput( p, "containCalcStatus",
                  "vContainStatus",
                  2, 0, 2, 0 );
    addOutput( p, "containCalcTime",
                  "vContainTime",
                  3, 0, 3, 0 );
    addOutput( p, "containCalcSize",
                  "vContainSize",
                  4, 0, 4, 0 );
    addOutput( p, "containCalcLine",
                  "vContainLine",
                  5, 0, 5, 0 );
    addOutput( p, "containCalcResourcesUsed",
                  "vContainResourcesUsed",
                  6, 0, 6, 0 );
    addOutput( p, "containCalcCost",
                  "vContainCost",
                  7, 0, 7, 0 );
    addOutput( p, "containCalcDiagram",
                  "vContainDiagram",
                  8, 0, 8, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Crown Module's options dialog.
 */

void ModulesDialog::crownOptions( void )
{
    PropertyPage *p, *p1;
    QButtonGroup *bg;
	int row = 0;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Crown:Caption", "crownDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Crown:Inputs:Tab", 1, 1,
                         "Wildfire1.png",
                         "Wildfire",
                         "crownOptions.html" );

	// Add the "Crown fire is calculated using" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Crown:Inputs:Model:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "crownConfModelRothermel",
		"PropertyTabDialog:Crown:Inputs:Model:Rothermel",
                  bg );
    p->addRadio( "crownConfModelScottReinhardt",
		"PropertyTabDialog:Crown:Inputs:Model:ScottReinhardt",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    bg->setMinimumWidth( bg->sizeHint().width()+20 );

	// Add the "Surface fire intensity is entered as" button group
	bg = p->addButtonGroup( "PropertyTabDialog:Crown:Inputs:Intensity:Caption",
                            row, 0, row, 0 );
    p->addRadio( "crownConfUseFlameLeng",
                 "PropertyTabDialog:Crown:Inputs:Intensity:Flame",
                  bg );
    p->addRadio( "crownConfUseFireLineInt",
                 "PropertyTabDialog:Crown:Inputs:Intensity:Fli",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    bg->setMinimumWidth( bg->sizeHint().width()+20 );

 	// Add the "Basic Outputs" page
	row = 0;
    p = dialog->addPage( "PropertyTabDialog:Crown:BasicOutputs:Tab", 1, 1,
                         "Wildfire2.png",
                         "Wildfire",
                         "selectOutput.html" );
	p1 = p;
    addOutput( p, "crownCalcActiveSpreadRate",
                  "vCrownFireActiveSpreadRate",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveHeatPerUnitArea",
                  "vCrownFireActiveHeatPerUnitArea",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveFireLineInt",
                  "vCrownFireActiveFireLineInt",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveFlameLeng",
                  "vCrownFireActiveFlameLeng",
                  row, 0, row, 0 ); row++;
    p->addLabel( "!",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveSpreadRate",
                  "vCrownFirePassiveSpreadRate",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveHeatPerUnitArea",
                  "vCrownFirePassiveHeatPerUnitArea",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveFireLineInt",
                  "vCrownFirePassiveFireLineInt",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveFlameLeng",
                  "vCrownFirePassiveFlameLeng",
                  row, 0, row, 0 ); row++;

	// Add the "Fire Type Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:FireTypeOutputs:Tab", 1, 1,
                         "Wildfire3.png",
                         "Wildfire",
                         "selectOutput.html" );
	row = 0;
    addOutput( p, "crownCalcCrownFireType",
                  "vCrownFireType",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcTransitionToCrown",
                  "vCrownFireTransToCrown",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcTransitionRatio",
                  "vCrownFireTransRatio",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveCrown",
                  "vCrownFireActiveCrown",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveRatio",
                  "vCrownFireActiveRatio",
                  row, 0, row, 0 ); row++;
    p->addLabel( "!",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPowerOfFire",
                  "vCrownFirePowerOfFire",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPowerOfWind",
                  "vCrownFirePowerOfWind",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPowerRatio",
                  "vCrownFirePowerRatio",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcWindDriven",
                  "vCrownFireWindDriven",
                  row, 0, row, 0 ); row++;

	// Add the "Size Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:SizeOutputs:Tab", 1, 1,
                         "Wildfire1.png",
                         "Wildfire",
                         "selectOutput.html" );
	row = 0;
    addOutput( p, "crownCalcActiveSpreadDist",
                  "vCrownFireActiveSpreadDist",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveFireArea",
                  "vCrownFireActiveFireArea",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcActiveFirePerimeter",
                  "vCrownFireActiveFirePerimeter",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcFireLengthToWidth",
                  "vCrownFireLengthToWidth",
                  row, 0, row, 0 ); row++;
    p->addLabel( "!",
                  row, 0, row, 0 ); row++;
	addOutput( p, "crownCalcPassiveSpreadDist",
                  "vCrownFirePassiveSpreadDist",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveFireArea",
                  "vCrownFirePassiveFireArea",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcPassiveFirePerimeter",
                  "vCrownFirePassiveFirePerimeter",
                  row, 0, row, 0 ); row++;

	// Add the "Canopy Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:CanopyOutputs:Tab", 1, 1,
                         "Wildfire3.png",
                         "Wildfire",
                         "selectOutput.html" );
	row = 0;
    addOutput( p, "crownCalcFuelLoad",
                  "vCrownFireFuelLoad",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcHeatPerUnitAreaCanopy",
                  "vCrownFireHeatPerUnitAreaCanopy",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcCanopyFractionBurned",
                  "vCrownFireCanopyFractionBurned",
                  row, 0, row, 0 ); row++;

	// Add the "Intermediate Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:IntermediateOutputs:Tab", 1, 1,
                         "Wildfire2.png",
                         "Wildfire",
                         "selectOutput.html" );
	row = 0;
    addOutput( p, "crownCalcCriticalSurfaceIntensity",
                  "vCrownFireCritSurfFireInt",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcCriticalSurfaceFlameLeng",
                  "vCrownFireCritSurfFlameLeng",
                  row, 0, row, 0 ); row++;
	// TO BE REMOVED
    addOutput( p, "crownCalcCritSurfSpreadRate",
                  "vCrownFireCritSurfSpreadRate",
                  row, 0, row, 0 ); row++;
    addOutput( p, "crownCalcCriticalCrownSpreadRate",
                  "vCrownFireCritCrownSpreadRate",
                  row, 0, row, 0 ); row++;
	// TO BE REMOVED
    addOutput( p, "crownCalcActiveCritOpenWindSpeed",
                  "vCrownFireActiveCritOpenWindSpeed",
                  row, 0, row, 0 ); row++;

	// Unused
    //addOutput( p, "crownCalcActiveCritSurfSpreadRate",
    //              "vCrownFireActiveCritSurfSpreadRate",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcCrownSpreadRate",
    //              "vCrownFireSpreadRate",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcFireLineInt",
    //              "vCrownFireLineInt",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcFlameLeng",
    //              "vCrownFireFlameLeng",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcHeatPerUnitArea",
    //              "vCrownFireHeatPerUnitArea",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcCrownSpreadDist",
    //              "vCrownFireSpreadDist",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcFireArea",
    //              "vCrownFireArea",
    //              row, 0, row, 0 ); row++;
    //addOutput( p, "crownCalcFirePerimeter",
    //              "vCrownFirePerimeter",
    //              row, 0, row, 0 ); row++;

	// Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p1 );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Ignition Module's options dialog.
 */

void ModulesDialog::ignitionOptions( void )
{
    PropertyPage  *p;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Ignition:Caption", "ignitionDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Ignition:Outputs:Tab", 1, 1,
                         "RacingTheStorm.png",
                         "Racing The Storm",
                         "selectOutput.html" );
    addOutput( p, "ignitionCalcIgnitionFirebrandProb",
                  "vIgnitionFirebrandProb",
                  0, 0, 0, 0, true );
    addOutput( p, "ignitionCalcIgnitionLightningProb",
                  "vIgnitionLightningProb",
                  1, 0, 1, 0, true );
    addOutput( p, "ignitionCalcFuelTemp",
                  "vSurfaceFuelTemp",
                  2, 0, 2, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Mortality Module's options dialog.
 */

void ModulesDialog::mortalityOptions( void )
{
    PropertyPage *p;
    // QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Mortality:Caption", "mortalityDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Note: versions > 4.0.0 no longer offer direct bark thickness entry
#ifdef ALLOW_BARK_THICKNESS_INPUT
    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Mortality:Inputs:Tab", 1, 1,
                         "YellowstoneMagic1.png",
                         "Yellowstone Magic",
                         "mortalityOptions.html" );
    // Bark button box
    bg = p->addButtonGroup( "PropertyTabDialog:Mortality:Inputs:Bark:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "mortalityConfBarkInput",
                 "PropertyTabDialog:Mortality:Inputs:Bark:Input",
                 bg );
    p->addRadio( "mortalityConfBarkDerived",
                 "PropertyTabDialog:Mortality:Inputs:Bark:Derived",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
#endif
    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Mortality:Outputs:Tab", 1, 1,
                         "YellowstoneMagic2.png",
                         "Yellowstone Magic",
                         "selectOutput.html" );
    addOutput( p, "mortalityCalcBarkThickness",
                  "vTreeBarkThickness",
                  0, 0, 0, 0 );
    addOutput( p, "mortalityCalcTreeCrownLengScorched",
                  "vTreeCrownLengScorchedAtVector",
                  1, 0, 1, 0 );
    addOutput( p, "mortalityCalcTreeCrownVolScorched",
                  "vTreeCrownVolScorchedAtVector",
                  2, 0, 2, 0 );
    addOutput( p, "mortalityCalcTreeMortalityRate",
                  "vTreeMortalityRateAtVector",
                  3, 0, 3, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Safety Module's options dialog.
 */

void ModulesDialog::safetyOptions( void )
{
    PropertyPage *p;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Safety:Caption", "safetyDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Safety:Outputs:Tab", 1, 1,
                         "WildfiresOf2000.png",
                         "Wildfires of 2000",
                         "selectOutput.html" );
    addOutput( p, "safetyCalcSepDist",
                  "vSafetyZoneSepDist",
                  0, 0, 0, 0 );
    addOutput( p, "safetyCalcSize",
                  "vSafetyZoneSize",
                  1, 0, 1, 0 );
    addOutput( p, "safetyCalcRadius",
                  "vSafetyZoneRadius",
                  2, 0, 2, 0 );
    addOutput( p, "safetyCalcSizeSquare",
                  "vSafetyZoneSizeSquare",
                  3, 0, 3, 0 );
    addOutput( p, "safetyCalcLength",
                  "vSafetyZoneLength",
                  4, 0, 4, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Scorch Module's options dialog.
 */

void ModulesDialog::scorchOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Scorch:Caption", "scorchDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Scorch:Inputs:Tab", 1, 1,
                         "Wildfire2.png",
                         "Wildfire",
                         "scorchOptions.html" );
    bg = p->addButtonGroup( "PropertyTabDialog:Scorch:Inputs:Intensity:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "scorchConfUseFlameLeng",
                 "PropertyTabDialog:Scorch:Inputs:Intensity:Flame",
                  bg );
    p->addRadio( "scorchConfUseFireLineInt",
                 "PropertyTabDialog:Scorch:Inputs:Intensity:Fli",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    bg->setMinimumWidth( bg->sizeHint().width()+20 );
    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Scorch:Outputs:Tab", 1, 1,
                         "Wildfire3.png",
                         "Wildfire",
                         "selectOutput.html" );
    addOutput( p, "scorchCalcScorchHt",
                  "vSurfaceFireScorchHtAtVector",
                  0, 0, 0, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Size Module's options dialog.
 */

void ModulesDialog::sizeOptions( void )
{
    PropertyPage *p1;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Size:Caption", "sizeDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Output Variables" page
    p1 = dialog->addPage( "PropertyTabDialog:Size:Outputs:Tab", 1, 1,
                         "Magpie.png",
                         "Magpie",
                         "selectOutput.html" );
    addOutput( p1, "sizeCalcFireArea",
                  "vSurfaceFireArea",
                  0, 0, 0, 0 );
    addOutput( p1, "sizeCalcFirePerimeter",
                  "vSurfaceFirePerimeter",
                  1, 0, 1, 0 );
    addOutput( p1, "sizeCalcFireLengToWidth",
                  "vSurfaceFireLengthToWidth",
                  2, 0, 2, 0 );
    addOutput( p1, "sizeCalcFireDistAtFront",
                  "vSurfaceFireDistAtHead",
                  3, 0, 3, 0 );
    addOutput( p1, "sizeCalcFireDistAtFlank",
                  "vSurfaceFireDistAtFlank",
                  4, 0, 4, 0 );
    addOutput( p1, "sizeCalcFireDistAtBack",
                  "vSurfaceFireDistAtBack",
                  5, 0, 5, 0 );
    addOutput( p1, "sizeCalcFireLengDist",
                  "vSurfaceFireLengDist",
                  6, 0, 6, 0 );
    addOutput( p1, "sizeCalcFireWidthDist",
                  "vSurfaceFireWidthDist",
                  7, 0, 7, 0 );
    addOutput( p1, "sizeCalcFireShapeDiagram",
                  "vSurfaceFireShapeDiagram",
                  8, 0, 8, 0 );

    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p1 );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Spot Module's options dialog.
 */

void ModulesDialog::spotOptions( void )
{
    PropertyPage *p, *p1;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Spot:Caption", "spotDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Basic Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Spot:BasicOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcDistTorchingTrees",
                  "vSpotDistTorchingTrees",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcDistActiveCrown",
                  "vSpotDistActiveCrown",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcDistBurningPile",
                  "vSpotDistBurningPile",
                  2, 0, 2, 0, true );
    addOutput( p, "spotCalcDistSurfaceFire",
                  "vSpotDistSurfaceFire",
                  3, 0, 3, 0, true );
	p1 = p;
    
	// Add the "Torching Tree Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Spot:TorchingTreeOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcCoverHtTorchingTrees",
                  "vSpotCoverHtTorchingTrees",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcFlameHtTorchingTrees",
                  "vSpotFlameHtTorchingTrees",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcFlameRatioTorchingTrees",
                  "vSpotFlameRatioTorchingTrees",
                  2, 0, 2, 0, true );
    addOutput( p, "spotCalcFlameDurTorchingTrees",
                  "vSpotFlameDurTorchingTrees",
                  3, 0, 3, 0, true );
    addOutput( p, "spotCalcFirebrandHtTorchingTrees",
                  "vSpotFirebrandHtTorchingTrees",
                  4, 0, 4, 0, true );
    addOutput( p, "spotCalcFlatDistTorchingTrees",
                  "vSpotFlatDistTorchingTrees",
                  5, 0, 5, 0, true );

    // Add the "Crown Fire Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Spot:ActiveCrownOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcFlameHtActiveCrown",
                  "vSpotFlameHtActiveCrown",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcFirebrandHtActiveCrown",
                  "vSpotFirebrandHtActiveCrown",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcFlatDistActiveCrown",
                  "vSpotFlatDistActiveCrown",
                  2, 0, 2, 0, true );

    // Add the "Burning Pile" page
    p = dialog->addPage( "PropertyTabDialog:Spot:BurningPileOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcCoverHtBurningPile",
                  "vSpotCoverHtBurningPile",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcFirebrandHtBurningPile",
                  "vSpotFirebrandHtBurningPile",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcFlatDistBurningPile",
                  "vSpotFlatDistBurningPile",
                  2, 0, 2, 0, true );

    // Add the "SurfaceFireOutputs" page
    p = dialog->addPage( "PropertyTabDialog:Spot:SurfaceFireOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcCoverHtSurfaceFire",
                  "vSpotCoverHtSurfaceFire",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcFirebrandHtSurfaceFire",
                  "vSpotFirebrandHtSurfaceFire",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcFirebrandDriftSurfaceFire",
                  "vSpotFirebrandDriftSurfaceFire",
                  2, 0, 2, 0, true );
    addOutput( p, "spotCalcFlatDistSurfaceFire",
                  "vSpotFlatDistSurfaceFire",
                  3, 0, 3, 0, true );

    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p1 );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Surface Module's options dialog.
 */

void ModulesDialog::surfaceOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
	int gridRows = 20;		// Force pages using 'gridRows' to pack items as if 20 rows per page, rather than spread out equally
	int row = 0;

	// Create the second-level tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Surface:Caption", "surfaceDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

	QTabDialog *inTab = new QTabDialog();
	inTab->setOkButton( QString::null );
	dialog->addTab( inTab, "Input Options" );

	QTabDialog *outTab = new QTabDialog();
	outTab->setOkButton( QString::null );
	dialog->addTab( outTab, "Output Variables" );

	//--------------------------------------------------------------------------
    // Add the "Inputs" - "Fuel" tab 1
    p = dialog->addPage( "PropertyTabDialog:Surface:Fuel:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests1.png",
                         "Restoring America's Forests",
                         "fuelOptions.html" );
	inTab->addTab( p, "Fuel" );

    // Add the "Fuel" button group "Fuel is entered as" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Fuel:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "surfaceConfFuelModels",
                 "PropertyTabDialog:Surface:Fuel:Models",
                  bg );
    p->addRadio( "surfaceConfFuelParms",
                 "PropertyTabDialog:Surface:Fuel:Parms",
                  bg );
    p->addRadio( "surfaceConfFuel2Dimensional",
                 "PropertyTabDialog:Surface:Fuel:2Dimensional",
                  bg );
    p->addRadio( "surfaceConfFuelHarmonicMean",
                 "PropertyTabDialog:Surface:Fuel:HarmonicMean",
                  bg );
    p->addRadio( "surfaceConfFuelAreaWeighted",
                 "PropertyTabDialog:Surface:Fuel:AreaWeighted",
                  bg );
    p->addRadio( "surfaceConfFuelPalmettoGallberry",
                 "PropertyTabDialog:Surface:Fuel:PalmettoGallberry",
                  bg );
    p->addRadio( "surfaceConfFuelAspen",
                 "PropertyTabDialog:Surface:Fuel:Aspen",
                  bg );
    p->addRadio( "surfaceConfFuelChaparral",
                 "PropertyTabDialog:Surface:Fuel:Chaparral",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

	//--------------------------------------------------------------------------
    // Add the "Inputs" - "Moisture" tab 2
	row = 0;
    p = dialog->addPage( "PropertyTabDialog:Surface:Mois:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests2.png",
                         "Restoring America's Forests",
                         "moistureOptions.html" );
	inTab->addTab( p, "Moisture" );

    // Add the Moisture button group "Dynamic curing percent load transfer" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:LoadTransfer:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfLoadTransferCalc",
                 "PropertyTabDialog:Surface:LoadTransfer:Calc",
                  bg );
    p->addRadio( "surfaceConfLoadTransferInput",
                 "PropertyTabDialog:Surface:LoadTransfer:Input",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

    // Add the "Moisture" button group "Moisture is entered by"...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Mois:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfMoisTimeLag",
                 "PropertyTabDialog:Surface:Mois:Size",
                  bg );
    p->addRadio( "surfaceConfMoisLifeCat",
                 "PropertyTabDialog:Surface:Mois:Life",
                  bg );
    p->addRadio( "surfaceConfMoisDeadHerbWood",
                 "PropertyTabDialog:Surface:Mois:DeadHerbWood",
                  bg );
    p->addRadio( "surfaceConfMoisScenario",
                 "PropertyTabDialog:Surface:Mois:Scenario",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

 	//--------------------------------------------------------------------------
   // Add the "Inputs" - "Wind Speed" tab 3
	row = 0;
    QString label= ( appWindow()->m_release < 20000 )
                   ? "PropertyTabDialog:Surface:Wind:Tab"
                   : "PropertyTabDialog:Surface:Wind:Tab20000";
    p = dialog->addPage( label,
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests3.png",
                         "Restoring America's Forests",
                         "windOptions.html" );
	inTab->addTab( p, "Wind Speed" );

    // Add the "Wind Speed" button group "Wind speed is entered as" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Speed:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfWindSpeedAtMidflame",
                 "PropertyTabDialog:Surface:Wind:Speed:Midflame",
                  bg );
    p->addRadio( "surfaceConfWindSpeedAt20Ft",
                 "PropertyTabDialog:Surface:Wind:Speed:20Ft",
                  bg );
    p->addRadio( "surfaceConfWindSpeedAt20FtCalc",
                 "PropertyTabDialog:Surface:Wind:Speed:20FtCalc",
                  bg );
    p->addRadio( "surfaceConfWindSpeedAt10M",
                 "PropertyTabDialog:Surface:Wind:Speed:10M",
                  bg );
    p->addRadio( "surfaceConfWindSpeedAt10MCalc",
                 "PropertyTabDialog:Surface:Wind:Speed:10MCalc",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
	// Add the "Directions" button group "Wind direction is" ...
	if ( true )
	{
		bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Dir:Caption",
								row, 0, row, 0 ); row++;
		p->addRadio( "surfaceConfWindDirUpslope",
					 "PropertyTabDialog:Surface:Wind:Dir:Upslope",
					  bg );
		p->addRadio( "surfaceConfWindDirInput",
					 "PropertyTabDialog:Surface:Wind:Dir:Input",
					 bg );
		bg->setFixedHeight( bg->sizeHint().height() );
	}

    // Add the "Wind Speed" button group "Impose maximum reliable wind speed limit?" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Limit:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfWindLimitApplied",
                 "PropertyTabDialog:Surface:Wind:Limit:Applied",
                  bg );
    p->addRadio( "surfaceConfWindLimitNotApplied",
                 "PropertyTabDialog:Surface:Wind:Limit:NotApplied",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

	//--------------------------------------------------------------------------
    // Add the "Input Options" - "Slope" tab 4
    p = dialog->addPage( "PropertyTabDialog:Surface:Slope:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "GoingToTheSun.png",
                         "Going To The Sun",
                         "slopeOptions.html" );
	inTab->addTab( p, "Slope" );
 
	// Add the "Slope" button group "Slope is specified as" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Slope:Units:Caption",
                     0, 0, 0, 0 );
    p->addRadio( "surfaceConfSlopeFraction",
                 "PropertyTabDialog:Surface:Slope:Units:Percent",
                 bg );
    p->addRadio( "surfaceConfSlopeDegrees",
                 "PropertyTabDialog:Surface:Slope:Units:Degrees",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    
	// Add the "Slope" button group "Slope steepness is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Slope:Entry:Caption",
                 1, 0, 1, 0 );
    p->addRadio( "surfaceConfSlopeInput",
                 "PropertyTabDialog:Surface:Slope:Entry:Input",
                 bg );
    p->addRadio( "surfaceConfSlopeDerived",
                 "PropertyTabDialog:Surface:Slope:Entry:Derived",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );

	//--------------------------------------------------------------------------
    // Add the "Input Options" - "Directions" tab 5
	row = 0;
    label= ( appWindow()->m_release < 20000 )
           ? "PropertyTabDialog:Surface:Dir:Tab"
           : "PropertyTabDialog:Surface:Dir:Tab20000";
    p = dialog->addPage( label,
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests1.png",
                         "Restoring America's Forests",
                         "directionOptions.html" );
	inTab->addTab( p, "Directions" );
	inTab->showPage( p );

	// Add the "Directions" button group "Rate of spread is calculated" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Dir:Spread:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfSpreadDirHead",
                 "PropertyTabDialog:Surface:Dir:Spread:Head",
                 bg );
    p->addRadio( "surfaceConfSpreadDirFlank",
                 "PropertyTabDialog:Surface:Dir:Spread:Flank",
                  bg );
    p->addRadio( "surfaceConfSpreadDirBack",
                 "PropertyTabDialog:Surface:Dir:Spread:Back",
                  bg );
    p->addRadio( "surfaceConfSpreadDirFireFront",
                 "PropertyTabDialog:Surface:Dir:Spread:FireFront",
                  bg );
    p->addRadio( "surfaceConfSpreadDirPointSourcePsi",
                 "PropertyTabDialog:Surface:Dir:Spread:PointSourcePsi",
                  bg );
    p->addRadio( "surfaceConfSpreadDirPointSourceBeta",
                 "PropertyTabDialog:Surface:Dir:Spread:PointSourceBeta",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

	// Add the "Directions" button group "Wind direction is" ...
	if ( false )
	{
		bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Dir:Caption",
								row, 0, row, 0 ); row++;
		p->addRadio( "surfaceConfWindDirUpslope",
					 "PropertyTabDialog:Surface:Wind:Dir:Upslope",
					  bg );
		p->addRadio( "surfaceConfWindDirInput",
					 "PropertyTabDialog:Surface:Wind:Dir:Input",
					 bg );
		bg->setFixedHeight( bg->sizeHint().height() );
	}

    // Add the "Directions" button group "Wind & spread directions are" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Dir:Compass:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfDegreesWrtUpslope",
                 "PropertyTabDialog:Surface:Dir:Compass:Upslope",
                 bg );
    p->addRadio( "surfaceConfDegreesWrtNorth",
                 "PropertyTabDialog:Surface:Dir:Compass:North",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
if ( false )
{
    // Add the "Directions" button group "Ignition Pt fireline intensity" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Dir:Beta:Caption",
                            row, 0, row, 0 ); row++;
    p->addRadio( "surfaceConfSpreadDirBetaPsi",
		"PropertyTabDialog:Surface:Dir:Beta:Psi",
                 bg );
    p->addRadio( "surfaceConfSpreadDirBetaBeta",
		"PropertyTabDialog:Surface:Dir:Beta:Beta",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
}

	//--------------------------------------------------------------------------
    // Add the "Input Options" - "Chaparral" tab 6
    p = dialog->addPage( "PropertyTabDialog:Surface:Chaparral:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "GoingToTheSun.png",
                         "Going To The Sun",
                         "chaparralOptions.html" );
	inTab->addTab( p, "Chaparral" );
 
	// Add the "Total Load" button group "Total fuel load is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:TotalLoad:Caption",
                     0, 0, 0, 0 );
    p->addRadio( "surfaceConfFuelChaparralTotalLoadFromInput",
		"PropertyTabDialog:Surface:Chaparral:TotalLoad:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralTotalLoadFromAgeType",
		"PropertyTabDialog:Surface:Chaparral:TotalLoad:AgeType",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
	// Add the "Depth" button group "Depth is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:Depth:Caption",
                     1, 0, 1, 0 );
    p->addRadio( "surfaceConfFuelChaparralDepthFromInput",
		"PropertyTabDialog:Surface:Chaparral:Depth:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralDepthFromAgeType",
		"PropertyTabDialog:Surface:Chaparral:Depth:AgeType",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
	// Add the "Dead fuel fraction" button group "Dead fuel fraction is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:DeadFraction:Caption",
                     2, 0, 2, 0 );
    p->addRadio( "surfaceConfFuelChaparralDeadFractionFromInput",
		"PropertyTabDialog:Surface:Chaparral:DeadFraction:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralDeadFractionFromAge",
		"PropertyTabDialog:Surface:Chaparral:DeadFraction:Age",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
	// Add the "Live Heat" button group "Live fuel heat content is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:HeatLive:Caption",
                     3, 0, 3, 0 );
    p->addRadio( "surfaceConfFuelChaparralHeatLiveFromInput",
		"PropertyTabDialog:Surface:Chaparral:HeatLive:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralHeatLiveFromDays",
		"PropertyTabDialog:Surface:Chaparral:HeatLive:Days",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
	// Add the "Live Moisture" button group "Live fuel moisture content is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:MoisLive:Caption",
                     4, 0, 4, 0 );
    p->addRadio( "surfaceConfFuelChaparralMoisLiveFromInput",
		"PropertyTabDialog:Surface:Chaparral:MoisLive:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralMoisLiveFromDays",
		"PropertyTabDialog:Surface:Chaparral:MoisLive:Days",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
	// Add the "Days Since May 1" button group "Days since May 1 is" ...
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Chaparral:Days:Caption",
                     5, 0, 5, 0 );
    p->addRadio( "surfaceConfFuelChaparralDaysFromInput",
		"PropertyTabDialog:Surface:Chaparral:Days:Input",
                 bg );
    p->addRadio( "surfaceConfFuelChaparralDaysFromDate",
		"PropertyTabDialog:Surface:Chaparral:Days:Date",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Basic Outputs" tab 1
	bool threeTabs = true;
    int r = 0;
    label= ( appWindow()->m_release < 20000 )
           ? "PropertyTabDialog:Surface:Outputs:Tab"
           : "PropertyTabDialog:Surface:BasicOutputs";
    p = dialog->addPage( label,
						gridRows, 1, // pack items vertically as if there will be 20 rows
                         "FlatheadLake1.png",
                         "Flathead Lake (detail)",
                         "selectOutput.html" );

	outTab->addTab( p, "Basic" );
	outTab->showPage( p );

    addOutput( p, "surfaceCalcFireSpread",
                  "vSurfaceFireSpreadAtHead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireHeatPerUnitArea",
                  "vSurfaceFireHeatPerUnitArea",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireLineInt",
                  "vSurfaceFireLineIntAtHead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireFlameLeng",
                  "vSurfaceFireFlameLengAtHead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireReactionInt",
                  "vSurfaceFireReactionInt",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireMaxDirFromUpslope",
                  "vSurfaceFireMaxDirFromUpslope",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireDist",
                  "vSurfaceFireDistAtVector",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireMaxDirDiagram",
                  "vSurfaceFireMaxDirDiagram",
                  r, 0, r, 0 ); r++;
	// Removed from Build 603 (Pat's notes of 3/19/2012)
    //addOutput( p, "surfaceCalcFireCharacteristicsDiagram",
    //              "vSurfaceFireCharacteristicsDiagram",
    //              r, 0, r, 0 ); r++;

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Intermediate Directions" tab 2
	r = 0;
	label = "PropertyTabDialog:Surface:FireFront:Tab";
	p = dialog->addPage( label,
						 gridRows, 1, // pack items vertically as if there will be 20 rows
                         "Magpie.png",
                         "Magpie",
                         "selectOutput.html" );
	outTab->addTab( p, "Intermediate Directions" );
    addOutput( p, "surfaceCalcEllipseF",
                  "vSurfaceFireEllipseF",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcEllipseG",
                  "vSurfaceFireEllipseG",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcEllipseH",
                  "vSurfaceFireEllipseH",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcVectorTheta",
                  "vSurfaceFireVectorTheta",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcVectorOther",
                  "vSurfaceFireVectorPsi",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireSpreadAtOther",
                  "vSurfaceFireSpreadAtPsi",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireLineIntAtOther",
                  "vSurfaceFireLineIntAtPsi",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireFlameLengAtOther",
                  "vSurfaceFireFlameLengAtPsi",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireDistAtOther",
                  "vSurfaceFireDistAtPsi",
                  r, 0, r, 0 ); r++;

	//--------------------------------------------------------------------------
	// Add the "Output Variables" - "Wind" tab 3
	if ( threeTabs )
	{
		dialog->showPage( p );	// display the previous tab by default
		r = 0;
		label= "PropertyTabDialog:Surface:WindOutputs";
		p = dialog->addPage( label,
							gridRows, 1, // pack items vertically as if there will be 20 rows
				             "FlatheadLake2.png",
					         "Flathead Lake (detail)",
						     "selectOutput.html" );
		outTab->addTab( p, "Wind" );
	}
	addOutput( p, "surfaceCalcWindSpeedAtMidflame",
                  "vWindSpeedAtMidflame",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcWindAdjFactor",
                  "vWindAdjFactor",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcCrownRatio",
                  "vTreeCrownRatio",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcCrownFillPortion",
                  "vTreeCanopyCrownFraction",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcWindAdjMethod",
                  "vWindAdjMethod",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireEffWind",
                  "vSurfaceFireEffWindAtHead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireWindSpeedLimit",
                  "vSurfaceFireWindSpeedLimit",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireWindSpeedFlag",
                  "vSurfaceFireWindSpeedFlag",
                  r, 0, r, 0 ); r++;

	//--------------------------------------------------------------------------
	// Add the "Output Variables" - "Slope" tab 4
	if ( threeTabs )
	{
	    r = 0;
		label= "PropertyTabDialog:Surface:SlopeOutputs";
	    p = dialog->addPage( label,
							gridRows, 1,  // pack items vertically as if there will be 20 rows
			                 "FlatheadLake3.png",
				             "Flathead Lake (detail)",
					         "selectOutput.html" );
		outTab->addTab( p, "Slope" );
	}
    addOutput( p, "surfaceCalcSlopeSteepness",
                  "vSiteSlopeFraction",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcSlopeRise",
                  "vSiteSlopeRise",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcSlopeReach",
                  "vSiteSlopeReach",
                  r, 0, r, 0 ); r++;
	if ( ! threeTabs )
	{
		dialog->showPage( p );	// NOW display the one big single tab
	}    

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Intermediates" tab 5
    r = 0;
    p = dialog->addPage( "PropertyTabDialog:Surface:Intermediates:Tab20000",
						gridRows, 1,  // pack items vertically as if there will be 20 rows
                         "FlatheadLake1.png",
                         "Flathead Lake (detail)",
                         "selectOutput.html" );
	outTab->addTab( p, "Intermediates" );
    addOutput( p, "surfaceCalcFuelBedMoisDead",
                  "vSurfaceFuelBedMoisDead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedMoisLive",
                  "vSurfaceFuelBedMoisLive",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedMextLive",
                  "vSurfaceFuelBedMextLive",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedSigma",
                  "vSurfaceFuelBedSigma",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedBulkDensity",
                  "vSurfaceFuelBedBulkDensity",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedPackingRatio",
                  "vSurfaceFuelBedPackingRatio",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedBetaRatio",
                  "vSurfaceFuelBedBetaRatio",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireReactionIntDead",
                  "vSurfaceFireReactionIntDead",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireReactionIntLive",
                  "vSurfaceFireReactionIntLive",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireWindFactor",
                  "vSurfaceFireWindFactor",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireSlopeFactor",
                  "vSurfaceFireSlopeFactor",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireHeatSource",
                  "vSurfaceFireHeatSource",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFuelBedHeatSink",
                  "vSurfaceFuelBedHeatSink",
                  r, 0, r, 0 ); r++;
    addOutput( p, "surfaceCalcFireResidenceTime",
                  "vSurfaceFireResidenceTime",
                  r, 0, r, 0 ); r++;

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Fuel" tab 6
    if ( appWindow()->m_release >= 20000 )
    {
        r = 0;
        // NOTE: to overcome Qt layout bug (which groups all check boxes into
        // a single groupbox "row" ), start header at row 1 and declare an
        // additional grid row
        p = dialog->addPage( "PropertyTabDialog:Surface:FuelOutputs:Tab",
                             gridRows, 1, // pack items vertically as if there will be 20 rows
                             "FlatheadLake2.png",
                             "Flathead Lake (detail)",
                             "selectOutput.html" );
		outTab->addTab( p, "Fuel" );

        p->addLabel( "PropertyTabDialog:Surface:DynamicOutputs:Note",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelLoadTransferFraction",
                      "vSurfaceFuelLoadTransferFraction",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelLoadDeadHerb",
                      "vSurfaceFuelLoadDeadHerb",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelLoadUndeadHerb",
                      "vSurfaceFuelLoadUndeadHerb",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelLoadDead",
                      "vSurfaceFuelLoadDead",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelLoadLive",
                      "vSurfaceFuelLoadLive",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelBedDeadFraction",
                      "vSurfaceFuelBedDeadFraction",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcFuelBedLiveFraction",
                      "vSurfaceFuelBedLiveFraction",
                      r, 0, r, 0 ); r++;
    }

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Aspen" tab 7
    if ( appWindow()->m_release >= 20000 )
    {
        r = 0;
        // NOTE: to overcome Qt layout bug (which groups all check boxes into
        // a single groupbox "row" ), start header at row 1 and declare an
        // additional grid row
        p = dialog->addPage( "PropertyTabDialog:Surface:Aspen:Tab",
                             gridRows, 1,  // pack items vertically as if there will be 20 rows
                             "LoonsOfTheSwanValley.png",
                             "Loons Of The Swan Valley",
                             "selectOutput.html" );
		outTab->addTab( p, "Aspen" );

        p->addLabel( "PropertyTabDialog:Surface:Aspen:Header1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenLoadDead1",
                      "vSurfaceFuelAspenLoadDead1",
                      r, 0, r, 0 ); r++;
        //addOutput( p, "surfaceCalcAspenLoadDead10",
        //              "vSurfaceFuelAspenLoadDead10",
        //              r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenLoadLiveHerb",
                      "vSurfaceFuelAspenLoadLiveHerb",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenLoadLiveWoody",
                      "vSurfaceFuelAspenLoadLiveWoody",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenSavrDead1",
                      "vSurfaceFuelAspenSavrDead1",
                      r, 0, r, 0 ); r++;
        //addOutput( p, "surfaceCalcAspenSavrDead10",
        //              "vSurfaceFuelAspenSavrDead10",
        //              r, 0, r, 0 ); r++;
        //addOutput( p, "surfaceCalcAspenSavrLiveHerb",
        //              "vSurfaceFuelAspenSavrLiveHerb",
        //              r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenSavrLiveWoody",
                      "vSurfaceFuelAspenSavrLiveWoody",
                      r, 0, r, 0 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcAspenMortality",
                      "vTreeMortalityRateAspenAtVector",
                      r, 0, r, 0 ); r++;
        //addOutput( p, "surfaceCalcAspenBedDepth",
        //              "vSurfaceFuelBedDepth",
        //              r, 0, r, 0 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addLabel( "PropertyTabDialog:Surface:Aspen:Note1",
                      r, 0, r, 0 ); r++;
    }

	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Palmeto-Gallberry" tab 8
    if ( appWindow()->m_release >= 20000 )
    {
        r = 0;
        // NOTE: to overcome Qt layout bug (which groups all check boxes into
        // a single groupbox "row" ), start header at row 1 and declare an
        // additional grid row
        p = dialog->addPage( "PropertyTabDialog:Surface:PalmettoOutputs:Tab",
                              gridRows, 1,  // pack items vertically as if there will be 20 rows
                             "PileatedWoodpecker.png",
                             "Pileated Woodpeckers",
                             "selectOutput.html" );
		outTab->addTab( p, "Palmetto-Gallberry" );

        p->addLabel( "PropertyTabDialog:Surface:PalmettoOutputs:Note",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadDead1",
                      "vSurfaceFuelPalmettoLoadDead1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadDead10",
                      "vSurfaceFuelPalmettoLoadDead10",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadDeadFoliage",
                      "vSurfaceFuelPalmettoLoadDeadFoliage",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadLive1",
                      "vSurfaceFuelPalmettoLoadLive1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadLive10",
                      "vSurfaceFuelPalmettoLoadLive10",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadLiveFoliage",
                      "vSurfaceFuelPalmettoLoadLiveFoliage",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoLoadLitter",
                      "vSurfaceFuelPalmettoLoadLitter",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcPalmettoBedDepth",
                      "vSurfaceFuelBedDepth",
                      r, 0, r, 0 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addLabel( "PropertyTabDialog:Surface:PalmettoOutputs:Note2",
                      r, 0, r, 0 ); r++;
    }
	//--------------------------------------------------------------------------
    // Add the "Output Variables" - "Chaparral" tab 9
    if ( appWindow()->m_release >= 20000 )
    {
        r = 0;
        // NOTE: to overcome Qt layout bug (which groups all check boxes into
        // a single groupbox "row" ), start header at row 1 and declare an
        // additional grid row
        p = dialog->addPage( "PropertyTabDialog:Surface:ChaparralOutputs:Tab",
                              gridRows, 1,  // pack items vertically as if there will be 20 rows
                             "PileatedWoodpecker.png",
                             "Pileated Woodpeckers",
                             "selectOutput.html" );
		outTab->addTab( p, "Chaparral" );

        p->addLabel( "PropertyTabDialog:Surface:ChaparralOutputs:Note",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralDepth",
                      "vSurfaceFuelChaparralDepth",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralDeadFuelFraction",
                      "vSurfaceFuelChaparralDeadFuelFraction",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadTotal",
                      "vSurfaceFuelChaparralLoadTotal",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadTotalDead",
                      "vSurfaceFuelChaparralLoadTotalDead",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadTotalLive",
                      "vSurfaceFuelChaparralLoadTotalLive",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadDead1",
                      "vSurfaceFuelChaparralLoadDead1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadDead2",
                      "vSurfaceFuelChaparralLoadDead2",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadDead3",
                      "vSurfaceFuelChaparralLoadDead3",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadDead4",
                      "vSurfaceFuelChaparralLoadDead4",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadLiveLeaf",
                      "vSurfaceFuelChaparralLoadLiveLeaf",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadLive1",
                      "vSurfaceFuelChaparralLoadLive1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadLive2",
                      "vSurfaceFuelChaparralLoadLive2",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadLive3",
                      "vSurfaceFuelChaparralLoadLive3",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralLoadLive4",
                      "vSurfaceFuelChaparralLoadLive4",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralDaysSinceMay1",
                      "vSurfaceFuelChaparralDaysSinceMay1",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralHeatLiveLeaf",
                      "vSurfaceFuelChaparralHeatLiveLeaf",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralHeatLiveStem",
                      "vSurfaceFuelChaparralHeatLiveStem",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralMoisLiveLeaf",
                      "vSurfaceFuelChaparralMoisLiveLeaf",
                      r, 0, r, 0 ); r++;
        addOutput( p, "surfaceCalcChaparralMoisLiveStem",
                      "vSurfaceFuelChaparralMoisLiveStem",
                      r, 0, r, 0 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addLabel( "PropertyTabDialog:Surface:ChaparralOutputs:Note2",
                      r, 0, r, 0 ); r++;
    }

    // Show then delete the dialog.
    dialog->resize( sizeHint() );
	dialog->setMinimumWidth( dialog->sizeHint().width() + 180 );
    dialog->showPage( inTab );
    dialog->exec();
	delete inTab;	inTab = 0;
	delete outTab;	outTab = 0;
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Weather Module's options dialog.
 */

void ModulesDialog::weatherOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
        "PropertyTabDialog:Weather:Caption", "weatherDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Weather:Inputs:Tab", 1, 1,
                         "BlackfootClearwater1.png",
                         "Blackfoot-Clearwater Wildlife Management Area",
                         "weatherOptions.html" );
    // Humidity button group
    bg = p->addButtonGroup( "PropertyTabDialog:Weather:Inputs:RH",
                            0, 0, 0, 0 );
    p->addRadio( "weatherConfHumidityFromDewPoint",
                 "PropertyTabDialog:Weather:Inputs:RH:DpDb",
                 bg );
    p->addRadio( "weatherConfHumidityFromWetBulbElev",
                 "PropertyTabDialog:Weather:Inputs:RH:DbWbEl",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Weather:Outputs:Tab",
                         1, 1,
                         "BlackfootClearwater2.png",
                         "Blackfoot-Clearwater Wildlife Management Area",
                         "selectOutput.html" );
    addOutput( p, "weatherCalcWthrRelativeHumidity",
                  "vWthrRelativeHumidity",
                  0, 0, 0, 0 );
    addOutput( p, "weatherCalcWthrDewPointTemp",
                  "vWthrDewPointTemp",
                  1, 0, 1, 0 );
    addOutput( p, "weatherCalcWthrCumulusBaseHt",
                  "vWthrCumulusBaseHt",
                  2, 0, 2, 0 );
    addOutput( p, "weatherCalcWthrHeatIndex",
                  "vWthrHeatIndex",
                  3, 0, 3, 0 );
    addOutput( p, "weatherCalcWthrSummerSimmerIndex",
                  "vWthrSummerSimmerIndex",
                  4, 0, 4, 0 );
    addOutput( p, "weatherCalcWthrWindChillTemp",
                  "vWthrWindChillTemp",
                  5, 0, 5, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
//  End of modulesdialog.cpp
//------------------------------------------------------------------------------

