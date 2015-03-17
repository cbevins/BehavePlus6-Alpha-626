//------------------------------------------------------------------------------
/*! \file appwindow.h
 *  \version 6
 *  \author Copyright (C) 2002-2014 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief AppWindow class definition.  This class creates the main
 *  application window including its workspace, menu bar, menu system,
 *  status bar, icon sets, splash page, etc.
 */

#ifndef _APPWINDOW_H_
/*! \def _APPWINDOW_H_
 *  \brief Prevent redundant includes.
 */
#define _APPWINDOW_H_ 1

// Custom class references
class BehavePlusApp;
class Document;
class EqApp;

// Qt class references
#include <qmainwindow.h>
#include <qpixmap.h>
class QFont;
class QIconSet;
class QLabel;
class QPopupMenu;
class QTimer;
class QToolBar;
class QToolButton;
class QVBox;
class QWorkspace;

//------------------------------------------------------------------------------
/*! \class AppWindow appwindow.h
 *
 *  \brief AppWindow class definition.  This class creates the main
 *  application window including its workspace, menu bar, menu system,
 *  status bar, icon sets, splash page, etc.
 */

class AppWindow: public QMainWindow
{
// Accept and generate signals
    Q_OBJECT
// Public methods
public:
    AppWindow( BehavePlusApp *bpApp, const QString &program,
        const QString &version, const QString &build,
        const QString &releaseNote ) ;
    ~AppWindow( void ) ;
    Document *openDocument( const QString &fileName, const QString &fileType,
                bool openAnyway=true ) ;

// Private methods
private:
    void checkCommandLineSwitches( void ) ;
    void checkWorkspaceSwitch( void ) ;
    bool testInstallation( void ) ;
    bool findInstallation( void ) ;
    Document *getActiveWindow( const QString &docType ) ;
    void initIconSets( void ) ;
    void initMenuCalculate( void ) ;
    void initMenuConfigure( void ) ;
    void initMenuFile( void ) ;
    void initMenuHelp( void ) ;
    void initMenuPages( void ) ;
    void initMenuTools( void ) ;
    void initMenuView( void ) ;
    void initMenuWindows( void ) ;
    void initResize( void ) ;
    void initToolBar( void ) ;
    Document *findDocument( const QString &fileName, bool focus ) ;
    void openStartups( void ) ;
    bool openStartupFile( const QString &fileName, bool run, bool print ) ;
    void setLanguage( const QString &language ) ;
    bool setStartupWorksheet( void ) ;

// Public slots
public slots:
    void slotDocumentClose( void ) ;

// Private slots
private slots:
    void slotAppInit( void ) ;
    void slotConfigureAppearance( void ) ;
    void slotConfigureFuelModels( void ) ;
    void slotConfigureLanguage( int id ) ;
    void slotConfigureMoistureScenarios( void ) ;
    void slotConfigureModules( void ) ;
    void slotConfigureUnitsCustom( void ) ;
    void slotConfigureUnitsEnglish( void ) ;
    void slotConfigureUnitsMetric( void ) ;
    void slotDocumentCapture( void ) ;
    void slotDocumentClear( void ) ;
    void slotDocumentClone( void ) ;
    void slotDocumentExportFuelModelsFarsiteEnglish( void ) ;
    void slotDocumentExportFuelModelsFarsiteMetric( void ) ;
    void slotDocumentExportResults( void ) ;
    void slotDocumentNew( void ) ;
    void slotDocumentOpen( void ) ;
    void slotDocumentPrint( void ) ;
    void slotDocumentReset( void ) ;
    void slotDocumentRun( void ) ;
    void slotDocumentSave( void ) ;
    void slotDocumentSaveAsFuelModel( void ) ;
    void slotDocumentSaveAsMoistureScenario( void ) ;
    void slotDocumentSaveAsRun( void ) ;
    void slotDocumentSaveAsWorksheet( void ) ;
    void slotDocumentWorkspace( bool clone=false ) ;
    void slotDocumentWorkspaceClone( void ) ;
    void slotDocumentWorkspaceNew( void ) ;
    void slotDocumentWorkspaceOpen( void ) ;
    void slotHelpAboutBehavePlus( void ) ;
    void slotHelpAboutHelp( void ) ;
    void slotHelpFeaturesPaper( void ) ;
    void slotHelpInstallationInfo( void ) ;
    void slotHelpMasterIndex( void ) ;
    void slotHelpUsersGuide( void ) ;
    void slotHelpVariableHelp( void ) ;
    void slotPageFirst( void ) ;
    void slotPageLast( void ) ;
    void slotPageNext( void ) ;
    void slotPagePrev( void ) ;
    void slotPagesMenuAboutToShow( void ) ;
    void slotPagesMenuActivated( int id ) ;
    void slotQuit( void ) ;
    void slotSetStartupWorksheet( void ) ;
    void slotSplashPageShow( void ) ;
    void slotStatusUpdate( void ) ;
    void slotToolsFdfmcDialog( void ) ;
    void slotToolsHorizontalDistance( void ) ;
    void slotToolsHumidityDialog( void ) ;
	void slotToolsSlopeFromMapMeasurements( void ) ;
    void slotToolsSunMoonCalendar( void ) ;
    void slotToolsUnitsConverter( void ) ;
    void slotToolsUnitsEditor( void ) ;
    void slotViewMenuAboutToShow( void ) ;
    void slotWindowsMenuAboutToShow( void ) ;
    void slotWindowsMenuActivated( int id ) ;

// Public data
public:
    // Internal data elements
    BehavePlusApp *m_bpApp;         //!< Pointer to BehavePlusApp parent
    QString      m_program;         //!< Application program name
    QString      m_version;         //!< Application version name
    QString      m_build;           //!< Build; "Build 241"
    QString      m_releaseNote;     //!< Release note; "Fake" or "RC1" or "Beta"
    QString      m_xmlFile;         //!< EqApp XML definition file
    QString      m_startupFile;     //!< File to open on startup
    QString      m_startupWorkspace;//!< Workspace to open on startup
    EqApp       *m_eqApp;           //!< Ptr to application's single EqApp
    int          m_release;         //!< Application release number (10000 is 1.00.00)
    int          m_docIdCount;      //!< Number of open documents
    bool         m_saveSplash;      //!< If TRUE, saves splash screen to BMP file.
    bool         m_killArg;         //!< TRUE if -kill arg specified
    bool         m_openArg;         //!< TRUE if -open arg specified
    bool         m_printArg;        //!< TRUE if -print arg specified
    bool         m_runArg;          //!< TRUE if -run arg specified
    // GUI elements
    QVBox       *m_vb;              //!< Vertical box to hold the m_workSpace
    QWorkspace  *m_workSpace;       //!< Shared QWorkspace
    QTimer      *m_initTimer;       //!< Initialization callback timer
    QFont       *m_propFont;        //!< App's default proportional font
    QFont       *m_fixedFont;       //!< App's default fixed font
    QPopupMenu  *m_calculateMenu;   //!< App's File menu
    QPopupMenu  *m_configMenu;      //!< App's Configure menu
    QPopupMenu  *m_configLangMenu;  //!< App's Configure->Language menu
    QPopupMenu  *m_configUnitsMenu; //!< App's Configure->Units menu
    QPopupMenu  *m_fileMenu;        //!< App's File menu
    QPopupMenu  *m_fileExportMenu;  //!< App's File->Export menu
    QPopupMenu  *m_fileExportFuelModelsMenu;//!< App's File->Export->Fuel models menu
    QPopupMenu  *m_fileSaveAsMenu;  //!< App's File->SaveAs menu
    QPopupMenu  *m_fileWorkspaceMenu; //!< App's File->SaveAs menu
    QPopupMenu  *m_helpMenu;        //!< App's Help menu
    QPopupMenu  *m_pagesMenu;       //!< App's Pages menu
    QPopupMenu  *m_toolsMenu;       //!< App's Tools menu
    QPopupMenu  *m_viewMenu;        //!< App's View menu
    QPopupMenu  *m_windowsMenu;     //!< App's Windows menu
    QToolBar    *m_toolBar;         //!< App's single tool bar
    QToolButton *m_fileNewButton;   //!< Toolbar File New button
    QToolButton *m_fileOpenButton;  //!< Toolbar File Open button
    //QToolButton *m_fileSaveButton;  //!< Toolbar File Save button
    QToolButton *m_filePrintButton; //!< Toolbar File Print button
    QToolButton *m_docModulesButton;//!< Toolbar Modules selection button
    QToolButton *m_docRunButton;    //!< Toolbar Caclulate button
    QToolButton *m_pageFirstButton; //!< Toolbar First Page button
    QToolButton *m_pageLastButton;  //!< Toolbar Last Page button
    QToolButton *m_pageNextButton;  //!< Toolbar Next Page button
    QToolButton *m_pagePrevButton;  //!< Toolbar Previous Page button
    QToolButton *m_helpHelpButton;  //!< Toolbar Help button
    // Menu ids
    int          m_idConfig;                //!< Configure Menu item id
    int          m_idConfigAppearance;      //!< Configure->Appearance menu item id
    int          m_idConfigLang;            //!< Configure->Language menu item id
    int          m_idConfigLanguage[100];   //!< Configure->Language menu item id
    int          m_idConfigFuel;            //!< Configure->Fuel model set menu item id
    int          m_idConfigModule;          //!< Configure->Module selection menu item id
    int          m_idConfigMois;            //!< Configure->Moisture scenario set menu item id
    int          m_idConfigUnits;           //!< Configure->Units menu item id
    int          m_idConfigUnitsCustom;     //!< Configure->Units->Custom menu item id
    int          m_idConfigUnitsEnglish;    //!< Configure->Units->English menu item id
    int          m_idConfigUnitsMetric;     //!< Configure->Units->Metric menu item id
    int          m_idFile;                  //!< File menu item id
    int          m_idFileClear;             //!< File->Clear worksheet inputs menu item id
    int          m_idFileClone;             //!< File->Clone run menu item id
    int          m_idFileNew;               //!< File->Open worksheet menu item id
    int          m_idFileOpen;              //!< File->Open run menu item id
    int          m_idFileSave;              //!< File->Save menu item id
    int          m_idFileSaveAs;            //!< File->saveAs menu item id
    int          m_idFileSaveAsRun;         //!< File->saveAs->Run menu item id
    int          m_idFileSaveAsWorksheet;   //!< File->saveAs->Worksheet menu item id
    int          m_idFileSaveAsFuelModel;   //!< File->saveAs->Fuel model menu item id
    int          m_idFileSaveAsMoistureScenario;    //!< File->saveAs->Moisture scenario menu item id
    int          m_idFileCalculate;         //!< File->Calculate menu item id
    int          m_idFilePrint;             //!< File->Print menu item id
    int          m_idFileReset;             //!< File->Print menu item id
    int          m_idFileExport;            //!< File->Export menu item id
    int          m_idFileExportFuelModels;  //!< File->Export->Fuel models menu item id
    int          m_idFileExportFuelModelsFarsiteEnglish;   //!< File->Export->Fuel models->Farsite format, English menu item id
    int          m_idFileExportFuelModelsFarsiteMetric;   //!< File->Export->Fuel models->Farsite format, metric menu item id
    int          m_idFileExportResults;     //!< File->Export->Fuel models menu item id
    int          m_idFileCapture;           //!< File->Capture menu item id
    int          m_idFileClose;             //!< File->Close menu item id
    int          m_idFileWorkspace;         //!< File->Worksheet menu item id
    int          m_idFileWorkspaceOpen;     //!< File->Worksheet->Open menu item id
    int          m_idFileWorkspaceNew;      //!< File->Worksheet->New menu item id
    int          m_idFileWorkspaceClone;    //!< File->Worksheet->Clone menu item id
    int          m_idFileQuit;              //!< File->Quit menu item id
    int          m_idHelp;                  //!< Help menu item id
    int          m_idHelpAboutBehavePlus;   //!< Help->About BehavePlus menu item id
    int          m_idHelpAboutHelp;         //!< Help->About Help menu item id
    int          m_idHelpInstallationInfo;  //!< Help->Installation Info menu item id
    int          m_idHelpMasterIndex;       //!< Help->Users Guide menu item id
    int          m_idHelpUsersGuide;        //!< Help->Users Guide menu item id
    int          m_idHelpSplashPage;        //!< Help->Splash Page menu item id
    int          m_idHelpVariableHelp;      //!< Help->Variable Help menu item id
    int          m_idPages;                 //!< Pages menu item id
    int          m_idTools;                 //!< Tools menu item id
    int          m_idToolsFdfmcDialog;      //!< Tools->Fine dead fuel moisture menu item id
    int          m_idToolsHorizontalDistance;   //!< Tools->horizontal distance menu item id
    int          m_idToolsHumidityDialog;   //!< Tools->relative humidity menu item id
	int			 m_idToolsSlopeFromMapMeasurements;//!< Tools->slope from map measurement item id
    int          m_idToolsSunCal;           //!< Tools->sun-moon calendar menu item id
    int          m_idToolsUnitsConverter;   //!< Tools->Units converter menu item id
    int          m_idToolsUnitsEditor;      //!< Tools->Units editor menu item id
    int          m_idView;                  //!< View menu item id
    int          m_idWindows;               //!< Window menu item id
    int          m_idWindowsCascade;        //!< Windows->Cascade menu item id
    int          m_idWindowsTile;           //!< Windows->Tile menu item id
    // Shared icon sets
    QIconSet    *m_cameraIcon;          //!< File->Capture icon set
    QIconSet    *m_checkedIcon;         //!< Windows menu checkmark icon set
    QIconSet    *m_documentIcon;        //!< File->New icon set
    QIconSet    *m_fileCopyIcon;        //!< File->Copy icon set
    QIconSet    *m_fileDeleteIcon;      //!< File->Delete icon set
    QIconSet    *m_fileNewIcon;         //!< File->New icon set
    QIconSet    *m_fileOpenIcon;        //!< File->Open icon set
    QIconSet    *m_fileRunIcon;         //!< File->Calculate icon set
    QIconSet    *m_fileSaveIcon;        //!< File->Save icon set
    QIconSet    *m_filePrintIcon;       //!< File->Print icon set
    QIconSet    *m_helpIcon;            //!< File->Help icon set
    QIconSet    *m_modulesIcon;         //!< Configure->Modules icon set
    QIconSet    *m_pageFirstIcon;       //!< Toolbar first page icon set
    QIconSet    *m_pageLastIcon;        //!< Toolbar last page icon set
    QIconSet    *m_pageNextIcon;        //!< Toolbar next page icon set
    QIconSet    *m_pagePrevIcon;        //!< Toolbar previous page icon set
    QIconSet    *m_paperClipIcon;       //!< Attach file menu paper clip icon set
    QIconSet    *m_quitIcon;            //!< File->Quit icon set
};

// Public convenience functions

AppWindow *appWindow( void ) ;

#endif

//------------------------------------------------------------------------------
//  End of appwindow.h
//------------------------------------------------------------------------------

