//------------------------------------------------------------------------------
/*! \file modulesdialog.cpp
 *  \version BehavePlus4
 *  \author Copyright (C) 2002-20010 by Collin D. Bevins.  All rights reserved.
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
	m_pb2(0),
    m_modules(0)
{
    // Create arrays to hold ptrs to all module checkboxes and pushbuttons
    m_moduleList = appWindow()->m_eqApp->m_moduleList;
    m_modules = m_moduleList->count();
    m_cb = new QCheckBox *[ m_modules ];
    checkmem( __FILE__, __LINE__, m_cb, "QCheckBox *m_cb", m_modules );
    m_pb = new QPushButton *[ m_modules ];
    checkmem( __FILE__, __LINE__, m_pb, "QPushButton *m_pb", m_modules );
    m_pb2 = new QPushButton *[ m_modules ];
    checkmem( __FILE__, __LINE__, m_pb, "QPushButton *m_pb2", m_modules );

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
    int cols = lastCol + 2;         // Allow 2 columns for pushbutton

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
    QString widgetName(""), optionsLabel(""), inputsLabel(""),outputsLabel("");
	translate( optionsLabel, "ModulesDialog:Option:Options:Label" );
	translate( inputsLabel, "ModulesDialog:Option:Inputs:Label" );
	translate( outputsLabel, "ModulesDialog:Option:Outputs:Label" );
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
        widgetName = "m_" + mod->m_name + "PushButtonInputs";
        m_pb[row] = new QPushButton( inputsLabel, m_gridFrame, widgetName );
        Q_CHECK_PTR( m_pb[row] );
        m_gridLayout->addWidget( m_pb[row], row, lastCol+1 );
        m_guideBtnGrp->insert( m_pb[row] );
		if ( ! mod->m_inputs )
		{
			m_pb[row]->hide();
		}
        // Create the output options pushbutton
        widgetName = "m_" + mod->m_name + "PushButtonOutputs";
        m_pb2[row] = new QPushButton( outputsLabel, m_gridFrame, widgetName );
        Q_CHECK_PTR( m_pb2[row] );
        m_gridLayout->addWidget( m_pb2[row], row, lastCol+2 );
        m_guideBtnGrp->insert( m_pb2[row] );
		if ( ! mod->m_outputs )
		{
			m_pb2[row]->hide();
		}
        // Only show modules for this release
        if ( ! mod->isCurrent( appWindow()->m_release ) )
        {
            m_cb[row]->hide();
            m_pb[row]->hide();
            m_pb2[row]->hide();
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
    delete m_pb2;               m_pb2 = 0;
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
	int module = id/2;
    Module *mod = m_moduleList->at( module );
    if ( mod->isCurrent( appWindow()->m_release ) )
    {
        if ( id == 0 )
        {
            surfaceInputOptions();
        }
        else if ( id == 1 )
        {
            surfaceOutputOptions();
        }
        else if ( id == 2 )
        {
            crownInputOptions();
        }
        else if ( id == 3 )
        {
            crownOutputOptions();
        }
        else if ( id == 4 || id == 5 )
        {
            safetyOutputOptions();
        }
        else if ( id == 6 || id == 7 )
        {
            sizeOutputOptions();
        }
        else if ( id == 8 )
        {
            containInputOptions();
        }
        else if ( id == 9 )
        {
            containOutputOptions();
        }
        else if ( id == 10 || id == 11 )
        {
            spotOutputOptions();
        }
        else if ( id == 12 )
        {
            scorchInputOptions();
        }
        else if ( id == 13 )
        {
            scorchOutputOptions();
        }
        else if ( id == 14 || id == 15 )
        {
            mortalityOutputOptions();
        }
        else if ( id == 16 || id == 17 )
        {
            ignitionOutputOptions();
        }
        // Removed from Version 3
        //else if ( id == 9 )
        //{
        //    weatherOutputOptions();
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
/*! \brief Defines then displays the Contain Module's input options dialog.
 */

void ModulesDialog::containInputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Contain:Inputs:Caption", "containDialog" );
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
    
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Contain Module's output options dialog.
 */

void ModulesDialog::containOutputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Contain:Outputs:Caption", "containDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

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
/*! \brief Defines then displays the Crown Module's input options dialog.
 */

void ModulesDialog::crownInputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Crown:Inputs:Caption", "crownDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Input Options" page
    p = dialog->addPage( "PropertyTabDialog:Crown:Inputs:Tab", 1, 1,
                         "Wildfire1.png",
                         "Wildfire",
                         "crownOptions.html" );
    bg = p->addButtonGroup( "PropertyTabDialog:Crown:Inputs:Intensity:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "crownConfUseFlameLeng",
                 "PropertyTabDialog:Crown:Inputs:Intensity:Flame",
                  bg );
    p->addRadio( "crownConfUseFireLineInt",
                 "PropertyTabDialog:Crown:Inputs:Intensity:Fli",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    bg->setMinimumWidth( bg->sizeHint().width()+20 );
     // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Crown Module's output options dialog.
 */

void ModulesDialog::crownOutputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Crown:Outputs:Caption", "crownDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );
    // Add the "Spread Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:SpreadOutputs:Tab", 1, 1,
                         "Wildfire2.png",
                         "Wildfire",
                         "selectOutput.html" );
    addOutput( p, "crownCalcCriticalSurfaceIntensity",
                  "vCrownFireCritSurfFireInt",
                  0, 0, 0, 0 );
    addOutput( p, "crownCalcCriticalSurfaceFlameLeng",
                  "vCrownFireCritSurfFlameLeng",
                  1, 0, 1, 0 );
    addOutput( p, "crownCalcTransitionRatio",
                  "vCrownFireTransRatio",
                  2, 0, 2, 0 );
    addOutput( p, "crownCalcTransitionToCrown",
                  "vCrownFireTransToCrown",
                  3, 0, 3, 0 );
    addOutput( p, "crownCalcCrownSpreadRate",
                  "vCrownFireSpreadRate",
                  4, 0, 4, 0 );
    addOutput( p, "crownCalcCriticalCrownSpreadRate",
                  "vCrownFireCritCrownSpreadRate",
                  5, 0, 5, 0 );
    addOutput( p, "crownCalcActiveRatio",
                  "vCrownFireActiveRatio",
                  6, 0, 6, 0 );
    addOutput( p, "crownCalcActiveCrown",
                  "vCrownFireActiveCrown",
                  7, 0, 7, 0 );
    addOutput( p, "crownCalcCrownFireType",
                  "vCrownFireType",
                  8, 0, 8, 0 );
    addOutput( p, "crownCalcCrownSpreadDist",
                  "vCrownFireSpreadDist",
                  9, 0, 9, 0 );
    addOutput( p, "crownCalcFireArea",
                  "vCrownFireArea",
                  10, 0, 10, 0 );
    addOutput( p, "crownCalcFirePerimeter",
                  "vCrownFirePerimeter",
                  11, 0, 11, 0 );
    addOutput( p, "crownCalcFireLengthToWidth",
                  "vCrownFireLengthToWidth",
                  12, 0, 12, 0 );
    dialog->showPage( p );
    
	// Add the "Intensity Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Crown:IntensityOutputs:Tab", 1, 1,
                         "Wildfire3.png",
                         "Wildfire",
                         "selectOutput.html" );
    addOutput( p, "crownCalcFireLineInt",
                  "vCrownFireLineInt",
                  0, 0, 0, 0 );
    addOutput( p, "crownCalcFlameLeng",
                  "vCrownFireFlameLeng",
                  1, 0, 1, 0 );
    addOutput( p, "crownCalcPowerOfFire",
                  "vCrownFirePowerOfFire",
                  2, 0, 2, 0 );
    addOutput( p, "crownCalcPowerOfWind",
                  "vCrownFirePowerOfWind",
                  3, 0, 3, 0 );
    addOutput( p, "crownCalcPowerRatio",
                  "vCrownFirePowerRatio",
                  4, 0, 4, 0 );
    addOutput( p, "crownCalcWindDriven",
                  "vCrownFireWindDriven",
                  5, 0, 5, 0 );
    addOutput( p, "crownCalcFuelLoad",
                  "vCrownFireFuelLoad",
                  6, 0, 6, 0 );
    addOutput( p, "crownCalcHeatPerUnitAreaCanopy",
                  "vCrownFireHeatPerUnitAreaCanopy",
                  7, 0, 7, 0 );
    addOutput( p, "crownCalcHeatPerUnitArea",
                  "vCrownFireHeatPerUnitArea",
                  8, 0, 8, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Ignition Module's output options dialog.
 */

void ModulesDialog::ignitionOutputOptions( void )
{
    PropertyPage  *p;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Ignition:Outputs:Caption", "ignitionDialog" );
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
/*! \brief Defines then displays the Mortality Module's output options dialog.
 */

void ModulesDialog::mortalityOutputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Mortality:Outputs:Caption", "mortalityDialog" );
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
/*! \brief Defines then displays the Safety Module's output options dialog.
 */

void ModulesDialog::safetyOutputOptions( void )
{
    PropertyPage *p;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Safety:Outputs:Caption", "safetyDialog" );
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
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Scorch Module's input options dialog.
 */

void ModulesDialog::scorchInputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Scorch:Inputs:Caption", "scorchDialog" );
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
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}
//------------------------------------------------------------------------------
/*! \brief Defines then displays the Scorch Module's output options dialog.
 */

void ModulesDialog::scorchOutputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Scorch:Outputs:Caption", "scorchDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

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

void ModulesDialog::sizeOutputOptions( void )
{
    PropertyPage *p;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Size:Outputs:Caption", "sizeDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Output Variables" page
    p = dialog->addPage( "PropertyTabDialog:Size:Outputs:Tab", 1, 1,
                         "Magpie.png",
                         "Magpie",
                         "selectOutput.html" );
    addOutput( p, "sizeCalcFireArea",
                  "vSurfaceFireArea",
                  0, 0, 0, 0 );
    addOutput( p, "sizeCalcFirePerimeter",
                  "vSurfaceFirePerimeter",
                  1, 0, 1, 0 );
    addOutput( p, "sizeCalcFireLengToWidth",
                  "vSurfaceFireLengthToWidth",
                  2, 0, 2, 0 );
    addOutput( p, "sizeCalcFireDistAtFront",
                  "vSurfaceFireDistAtHead",
                  3, 0, 3, 0 );
    addOutput( p, "sizeCalcFireDistAtBack",
                  "vSurfaceFireDistAtBack",
                  4, 0, 4, 0 );
    addOutput( p, "sizeCalcFireLengDist",
                  "vSurfaceFireLengDist",
                  5, 0, 5, 0 );
    addOutput( p, "sizeCalcFireWidthDist",
                  "vSurfaceFireWidthDist",
                  6, 0, 6, 0 );
    addOutput( p, "sizeCalcFireShapeDiagram",
                  "vSurfaceFireShapeDiagram",
                  7, 0, 7, 0 );
    // Show then delete the dialog.
    dialog->resize( sizeHint() );
    dialog->showPage( p );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Spot Module's options dialog.
 */

void ModulesDialog::spotOutputOptions( void )
{
    PropertyPage *p, *p1;
    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Spot:Outputs:Caption", "spotDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );

    // Add the "Basic Outputs" page
    p = dialog->addPage( "PropertyTabDialog:Spot:BasicOutputs:Tab", 1, 1,
                         "FirewiseCommunities1.png",
                         "Firewise Communities",
                         "selectOutput.html" );
    addOutput( p, "spotCalcDistTorchingTrees",
                  "vSpotDistTorchingTrees",
                  0, 0, 0, 0, true );
    addOutput( p, "spotCalcDistBurningPile",
                  "vSpotDistBurningPile",
                  1, 0, 1, 0, true );
    addOutput( p, "spotCalcDistSurfaceFire",
                  "vSpotDistSurfaceFire",
                  2, 0, 2, 0, true );
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
/*! \brief Defines then displays the Surface Module's input options dialog.
 */

void ModulesDialog::surfaceInputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
	int gridRows = 20;		// Force pages using 'gridRows' to pack items as if 20 rows per page, rather than spread out equally

    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Surface:Inputs:Caption", "surfaceDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );
	
    // Add the "Fuel" page 1
    p = dialog->addPage( "PropertyTabDialog:Surface:Fuel:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests1.png",
                         "Restoring America's Forests",
                         "fuelMoisOptions.html" );
    // Add the "Fuel" page 1
    // "Fuel is entered as" button group
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
    bg->setFixedHeight( bg->sizeHint().height() );
    // Add the "Fuel" page 1
    // "Dynamic curing percent load transfer" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:LoadTransfer:Caption",
                            1, 0, 1, 0 );
    p->addRadio( "surfaceConfLoadTransferCalc",
                 "PropertyTabDialog:Surface:LoadTransfer:Calc",
                  bg );
    p->addRadio( "surfaceConfLoadTransferInput",
                 "PropertyTabDialog:Surface:LoadTransfer:Input",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
 
    // Add the "Moisture" page 2
    p = dialog->addPage( "PropertyTabDialog:Surface:Moisture:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests1.png",
                         "Restoring America's Forests",
                         "fuelMoisOptions.html" );
	// Add the "Moisture" page 1
    // "Moisture is entered by" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Mois:Caption",
                            0, 0, 0, 0 );
    p->addRadio( "surfaceConfMoisTimeLag",
                 "PropertyTabDialog:Surface:Mois:Size",
                  bg );
    p->addRadio( "surfaceConfMoisLifeCat",
                 "PropertyTabDialog:Surface:Mois:Life",
                  bg );
    p->addRadio( "surfaceConfMoisScenario",
                 "PropertyTabDialog:Surface:Mois:Scenario",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

    // Add the "Wind Speed" page 3
    QString label= ( appWindow()->m_release < 20000 )
                   ? "PropertyTabDialog:Surface:Wind:Tab"
                   : "PropertyTabDialog:Surface:Wind:Tab20000";
    p = dialog->addPage( label,
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests2.png",
                         "Restoring America's Forests",
                         "windOptions.html" );
    // Add the "Wind Speed" page 3
    // "Wind speed is entered as" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Speed:Caption",
                            0, 0, 0, 0 );
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

    // Add the "Wind Speed" page 3
    // "Impose maximum reliable wind speed limit?" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Limit:Caption",
                            1, 0, 1, 0 );
    p->addRadio( "surfaceConfWindLimitApplied",
                 "PropertyTabDialog:Surface:Wind:Limit:Applied",
                  bg );
    p->addRadio( "surfaceConfWindLimitNotApplied",
                 "PropertyTabDialog:Surface:Wind:Limit:NotApplied",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );

    // Add the "Directions" page 4
    label= ( appWindow()->m_release < 20000 )
           ? "PropertyTabDialog:Surface:Dir:Tab"
           : "PropertyTabDialog:Surface:Dir:Tab20000";
    p = dialog->addPage( label,
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "RestoringAmericasForests3.png",
                         "Restoring America's Forests",
                         "directionOptions.html" );
    // Add the "Directions" page 4
    // "Rate of spread is calculated" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Dir:Spread:Caption",
                            0, 0, 0, 0 );
    //p->addRadio( "surfaceConfSpreadDirMax",
    //             "PropertyTabDialog:Surface:Dir:Spread:MaxDir",
    //             bg );
    //p->addRadio( "surfaceConfSpreadDirInput",
    //             "PropertyTabDialog:Surface:Dir:Spread:Input",
    //              bg );
    p->addRadio( "surfaceConfSpreadDirHeadFlankBack",
                 "PropertyTabDialog:Surface:Dir:Spread:HeadFlankBack",
                  bg );
    p->addRadio( "surfaceConfSpreadDirBeta",
                 "PropertyTabDialog:Surface:Dir:Spread:Beta",
                  bg );
    p->addRadio( "surfaceConfSpreadDirPsi",
                 "PropertyTabDialog:Surface:Dir:Spread:Psi",
                  bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    // Add the "Directions" page 4
    // "Wind direction is" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Wind:Dir:Caption",
                            1, 0, 1, 0 );
    p->addRadio( "surfaceConfWindDirUpCrossDown",
                 "PropertyTabDialog:Surface:Wind:Dir:UpCrossDown",
                  bg );
    p->addRadio( "surfaceConfWindDirInput",
                 "PropertyTabDialog:Surface:Wind:Dir:Input",
                 bg );
    //p->addRadio( "surfaceConfWindDirUpslope",
    //             "PropertyTabDialog:Surface:Wind:Dir:Upslope",
    //              bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    // Add the "Directions" page 4
    // "Wind & spread directions are" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Dir:Compass:Caption",
                            2, 0, 2, 0 );
    p->addRadio( "surfaceConfDegreesWrtUpslope",
                 "PropertyTabDialog:Surface:Dir:Compass:Upslope",
                 bg );
    p->addRadio( "surfaceConfDegreesWrtNorth",
                 "PropertyTabDialog:Surface:Dir:Compass:North",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );

    // Add the "Slope" page 5
    p = dialog->addPage( "PropertyTabDialog:Surface:Slope:Tab",
						 1, 1,	// allow group boxes to spread vertically to fill entire pane
                         "GoingToTheSun.png",
                         "Going To The Sun",
                         "slopeOptions.html" );
    // Add the "Slope" page 5
    // "Slope is specified as" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Slope:Units:Caption",
                     0, 0, 0, 0 );
    p->addRadio( "surfaceConfSlopeFraction",
                 "PropertyTabDialog:Surface:Slope:Units:Percent",
                 bg );
    p->addRadio( "surfaceConfSlopeDegrees",
                 "PropertyTabDialog:Surface:Slope:Units:Degrees",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );
    // Add the "Slope" page 5
    // "Slope steepness is" button group
    bg = p->addButtonGroup( "PropertyTabDialog:Surface:Slope:Entry:Caption",
                 1, 0, 1, 0 );
    p->addRadio( "surfaceConfSlopeInput",
                 "PropertyTabDialog:Surface:Slope:Entry:Input",
                 bg );
    p->addRadio( "surfaceConfSlopeDerived",
                 "PropertyTabDialog:Surface:Slope:Entry:Derived",
                 bg );
    bg->setFixedHeight( bg->sizeHint().height() );

    // Show then delete the dialog.
    dialog->resize( sizeHint() );
	dialog->setMinimumWidth( dialog->sizeHint().width() + 180 );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}
//------------------------------------------------------------------------------
/*! \brief Defines then displays the Surface Module's output options dialog.
 */

void ModulesDialog::surfaceOutputOptions( void )
{
    PropertyPage *p;
    QButtonGroup *bg;
	int gridRows = 20;		// Force pages using 'gridRows' to pack items as if 20 rows per page, rather than spread out equally

    // Create the tabbed dialog
    PropertyTabDialog *dialog = new PropertyTabDialog( m_bp, m_bp->property(),
		"PropertyTabDialog:Surface:Outputs:Caption", "surfaceDialog" );
    checkmem( __FILE__, __LINE__, dialog, "PropertyTabDialog dialog", 1 );
	
    // Add the "Basic Outputs" page 1
	bool threeTabs = true;
    int r = 0;
    QString label= ( appWindow()->m_release < 20000 )
           ? "PropertyTabDialog:Surface:Outputs:Tab"
           : "PropertyTabDialog:Surface:BasicOutputs";
    p = dialog->addPage( label,
						gridRows, 2, // pack items vertically as if there will be 20 rows
                         "FlatheadLake1.png",
                         "Flathead Lake (detail)",
                         "selectOutput.html" );
	QLabel *indent = new QLabel( "      ", p->m_frame );
	p->m_grid->addWidget( indent, r++, 0 );
	 
	addOutput( p, "surfaceCalcFireSpread",
                  "vSurfaceFireSpreadAtHead",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireSpreadAtBeta",
                  "vSurfaceFireSpreadAtVector",
                  r, 1, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireSpreadAtPsi",
                  "vSurfaceFireSpreadAtPsi",
                  r, 1, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireHeatPerUnitArea",
                  "vSurfaceFireHeatPerUnitArea",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireLineInt",
                  "vSurfaceFireLineIntAtHead",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireFlameLeng",
                  "vSurfaceFireFlameLengAtHead",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireReactionInt",
                  "vSurfaceFireReactionInt",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireMaxDirFromUpslope",
                  "vSurfaceFireMaxDirFromUpslope",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireDist",
                  "vSurfaceFireDistAtHead",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireDistAtBeta",
                  "vSurfaceFireDistAtVector",
                  r, 1, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireDistAtPsi",
                  "vSurfaceFireDistAtPsi",
                  r, 1, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireMaxDirDiagram",
                  "vSurfaceFireMaxDirDiagram",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireCharacteristicsDiagram",
                  "vSurfaceFireCharacteristicsDiagram",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireVector",
                  "vSurfaceFireVectorPsiFromFire",
                  r, 0, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireVectorBeta",
                  "vSurfaceFireVectorDirFromNorth",
                  r, 1, r, 1 ); r++;
    addOutput( p, "surfaceCalcFireVectorPsi",
                  "vSurfaceFireVectorPsiFromNorth",
                  r, 1, r, 1 ); r++;

	// Add the "Wind Outputs" page 2
	if ( threeTabs )
	{
		dialog->showPage( p );	// display the previous tab by default
		r = 0;
		label= "PropertyTabDialog:Surface:WindOutputs";
		p = dialog->addPage( label,
							gridRows, 1, // pack items vertically as if there will be 20 rows
				             "FlatheadLake1.png",
					         "Flathead Lake (detail)",
						     "selectOutput.html" );
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

	// Add the "Slope Outputs" page 3
	if ( threeTabs )
	{
	    r = 0;
		label= "PropertyTabDialog:Surface:SlopeOutputs";
	    p = dialog->addPage( label,
							gridRows, 1,  // pack items vertically as if there will be 20 rows
			                 "FlatheadLake1.png",
				             "Flathead Lake (detail)",
					         "selectOutput.html" );
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

    // Add the "Intermediates" page 4
    r = 0;
    p = dialog->addPage( "PropertyTabDialog:Surface:Intermediates:Tab20000",
						gridRows, 1,  // pack items vertically as if there will be 20 rows
                         "FlatheadLake2.png",
                         "Flathead Lake (detail)",
                         "selectOutput.html" );
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

    // Add the "Fuel Outputs" page 5
    if ( appWindow()->m_release >= 20000 )
    {
        r = 0;
        // NOTE: to overcome Qt layout bug (which groups all check boxes into
        // a single groupbox "row" ), start header at row 1 and declare an
        // additional grid row
        p = dialog->addPage( "PropertyTabDialog:Surface:FuelOutputs:Tab",
                             gridRows, 1, // pack items vertically as if there will be 20 rows
                             "FlatheadLake3.png",
                             "Flathead Lake (detail)",
                             "selectOutput.html" );

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

    // Add the "Aspen Fuels" page 6
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

    // Add the "P-G Fuels" page 7
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

    // Show then delete the dialog.
    dialog->resize( sizeHint() );
	dialog->setMinimumWidth( dialog->sizeHint().width() + 180 );
    dialog->exec();
    delete dialog;  dialog = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Defines then displays the Weather Module's output options dialog.
 */

void ModulesDialog::weatherOutputOptions( void )
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

