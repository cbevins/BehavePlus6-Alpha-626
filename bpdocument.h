//------------------------------------------------------------------------------
/*! \file bpdocument.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument class declaration.
 */

#ifndef _BPDOCUMENT_H_
/*! \def _BPDOCUMENT_H_
 *  \brief Prevent redundant includes.
 */
#define _BPDOCUMENT_H_ 1

// Forward class references.
#include "document.h"
#include "xeqvar.h"

#include <qmainwindow.h>
#include <qmemarray.h>
#include <qpixmap.h>

class AppWindow;
class Composer;
class BpDocEntry;
class EqApp;
class EqTree;
class Graph;
class GraphAxleParms;
class PropertyDict;
class QButtonGroup;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QWorkspace;

//------------------------------------------------------------------------------
/*! \class BpDocument bpdocument.h
 *
 *  \brief The BpDocument class is the GUI display for BehavePlus "Run" files.
 *
 *  BehavePlus uses a single AppWindow instance to define the main application
 *  window with a main menu bar, tool bar, status bar, and a \e Multiple
 *  \e Document \e Interface (MDI).  The MDI allows multiple Document
 *  instances, including BpDocuments, to be open simultaneously.
 *
 *  Because BpDocuments are derived from the Document abstract base class,
 *  they can:
 *  \arg  read files from disk,
 *  \arg  display them in the scrolling view port,
 *  \arg  scale the display page,
 *  \arg  navigate from page to page,
 *  \arg  print one or more pages,
 *  \arg  capture pages to an image file, and
 *  \arg  save the file under a new name.
 *
 *  While the BpDocument provides the GUI capabilities for a BehavePlus run,
 *  its embedded EqTree class instance manages all the variables and
 *  computations.
 *
 *  A single BpDocument instance exists for every open Run (or Worksheet) file.
 *  Each BpDocument is associated with a "Run" file.  Even \b File->New
 *  actually loads a standard "Worksheet" file, which is a Run files
 *  with no input values.
 *
 *  The BpDocument class performs the following tasks:
 *  \arg reads/writes BehavePlus "Run" files to/from file storage,
 *  \arg displays its run file contents in an input worksheet,
 *  \arg allows the user to calculate outputs based on input worksheet
 *       values (e.g., make a "run"), and
 *  \arg displays the tabular and graphical result pages to the screen
 *       and/or printer.
 *
 *  In addition to storing the values of all input variables for a specific
 *  BehavePlus run, the Run file also includes parameters defining the:
 *  \arg module configuration (active/inactive modules and outputs),
 *  \arg application appearance (colors, fonts, graphs, tables), and
 *  \arg input and output variable units of measure and decimal places.
 *
 *  BpDocument also supports:
 *  \arg extensive entry and run-time checking of input variable values,
 *  \arg on-line context-sensitive help and information browsers,
 *  \arg on-line "guide button" for every input variable that allows the
 *       user to enter ranges or choose discrete values,
 *  \arg numerous output table and graph display styles,
 *  \arg custom fuel modeling, and
 *  \arg "moisture scenarios" to describe complete moisture conditions
 *       (e.g., "97-th percentil weather at Missoula, MT) analogous to the
 *       way fuel models describe complete fuel conditions.
 *
 *  BehavePlus supports the \e Multiple \e Document \e Interface, e.g.,
 *  any number of BpDocuments may be open at one time.  To manage this
 *  all BpDocument instances have QWorkspace as a parent.
 */

class BpDocument : public Document
{
// Enable signals and slots on this class
    Q_OBJECT

//! Enumerates the available context menu options.
enum ContextMenuOption
{
    ContextCalculate=0, //!< Calculates results using current input values.
    ContextSave=1,      //!< Saves the current values to the Run file.
    ContextSaveAs=2,    //!< Saves the current values to another Run, Worksheet, Fuel Model, or Moisture Scenario file.
    ContextPrint=3,     //!< Prints one or more pages of the current run.
    ContextCapture=4,   //!< Captures an image of the current run page.
    ContextClose=5      //!< Closes the current run page.
};

// Public methods
public:
    BpDocument( QWorkspace *workspace, int docId, EqApp *eqApp,
        const char* name, int wflags ) ;
    virtual ~BpDocument( void ) ;

// Public methods that may be called by the AppWindow class.
    virtual bool capture( void ) ;
    virtual void clear( bool showRunDialog=true ) ;
    virtual void composeDocumentation( void ) ;
    virtual void composeGuideButtonPixmap( int wd, int ht ) ;
    virtual bool composeGraphs( bool lineGraphs, bool showDialogs ) ;
    virtual void composeLogo( double x0, double y0,
                    double wd, double ht, int penWd ) ;
    virtual void composeTable1( void ) ;
    virtual void composeTable2( EqVar *rowVar) ;
    virtual void composeTable3( EqVar *rowVar, EqVar *colVar ) ;
    virtual void configure( void ) ;
    virtual void configureAppearance( void ) ;
    virtual void configureFuelModels( void ) ;
    virtual void configureMoistureScenarios( void ) ;
    virtual void configureModules( void ) ;
    virtual void configureUnits( const QString &unitsSet ) ;
    virtual void maintenance( void ) ;
    virtual bool open( const QString &fileName, const QString &fileType ) ;
    virtual bool print( void ) ;
    virtual bool printPS( int fromPage, int thruPage ) ;
    virtual void reset( bool showRunDialog=true ) ;
    virtual void run( bool showRunDialog=true ) ;
    virtual void setFocus( void ) ;
    virtual void save( const QString &fileName, const QString &fileType ) ;
    virtual void viewMenuAboutToShow( QPopupMenu *viewMenu ) ;

// Public access methods
    EqVar *leaf( int lid ) const ;
    int    leafCount( void ) const ;
    PropertyDict *property( void ) const ;
    EqVar *root( int rid ) const ;
    int    rootCount( void ) const ;
    double tableCol( int vid ) const ;
    int    tableCols( void ) const ;
    bool   tableInRx( int vid ) const ;
    double tableRow( int vid ) const ;
    int    tableRows( void ) const ;
    double tableVal( int vid ) const ;
    EqVar *tableVar( int vid ) const ;
    int    tableVars( void ) const;

// Public methods called by docentry.cpp
    void focusNext( int position=-1, int length=-1 ) ;
    void focusPrev( int position=-1, int length=-1 ) ;
        void focusRx( int rx, QLineEdit *entry );
    void focusThis( int lid, int position=-1, int length=-1 ) ;
    bool validateWorksheetEntry( int lid, const QString &text,
            int *tokens, int *position, int *length ) ;
    void worksheetEntryClicked ( int id ) ;

// Public slots
public slots:
    virtual void regenerateWorksheet( void ) ;

// Protected slots.
protected slots:
    virtual void contextMenuActivated( int id ) ;
    virtual void guideClicked( int id );
    virtual void fuelClicked( void ) ;
    virtual void maintenanceMenuActivated( int id ) ;
    virtual void rescale( int points ) ;

// Protected methods
protected:
    virtual void composeNewPage( void ) ;
    virtual void composeWorksheet( void ) ;
    virtual void contextMenuCreate( void ) ;
    virtual void getStores( void ) ;
    virtual void releaseCheck( void ) const ;
    virtual void resizeWorksheet( void ) ;
    virtual void showPage( int pageNo ) ;

// Protected convenience functions
protected:
    bool   runGraphs( void ) ;
    bool   runTables( const QString &logFile ) ;

// Private methods
private:
    void    activeModules( QString &str ) ;
    void    barYMinMax( int yid, double &yMin, double &yMax ) ;
    void    composeBarGraph( int yid, EqVar *xVar, EqVar *yVar,
                GraphAxleParms *xParms, GraphAxleParms *yParms ) ;
    void    composeContainDiagram( void ) ;
    void    composeDiagrams( void ) ;
    void    composeFireCharacteristicsDiagram( void ) ;
    void    composeFireShapeDiagram( void ) ;
    void    composeFireMaxDirDiagram( void ) ;
    void    composeGraphBasics( Graph *g,
                bool isLineGraph, EqVar *xVar, EqVar *yVar, EqVar *zVar,
                int curves, GraphAxleParms *xParms, GraphAxleParms *yParms ) ;
    void    composeLineGraph( int yid, EqVar *xVar, EqVar *yVar, EqVar *zVar,
                GraphAxleParms *xParms, GraphAxleParms *yParms ) ;
    void    composePageMap( double dimension, int tabRows, int tabCols,
                int pageRows, int pageCols, int paneRows, int paneCols,
                int thisRow, int thisCol ) ;
    void    composeTableHtmlFooter( FILE *fptr ) ;
    void    composeTableHtmlHeader( FILE *fptr ) ;
    void    composeTable1Html( void ) ;
    void    composeTable1Spreadsheet( void ) ;
    void    composeTable2Html( EqVar *rowVar ) ;
    void    composeTable2Spreadsheet( EqVar *rowVar ) ;
    void    composeTable3Html( EqVar *rowVar, EqVar *colVar );
    void    composeTable3Html( FILE *fptr, int vid, EqVar *rowVar, EqVar *colVar ) ;
    void    composeTable3Spreadsheet( EqVar *rowVar, EqVar *colVar ) ;
    void    composeTable3Spreadsheet( FILE *fptr, int vid, EqVar *rowVar, EqVar *colVar );
    void    composeTable3( int vid, EqVar *rowVar, EqVar *colVar ) ;
    void    graphYMinMax( int yid, double &yMin, double &yMax ) ;
    void    grayInputs( void ) ;
    int     headerWidth( EqVar *varPtr, const QFontMetrics &fm ) ;
    void    loadNotes( void ) ;
    double  newWorksheetPage( double lineHt, TocType=TocInput ) ;
    void    runOptions( QString* runOpt, int& nOptions ) ;
    bool    runWorksheet( const QString &traceFile, const QString &resultFile,
                bool showRunDialog=true ) ;
    void    saveAsFuelModelExportFile( const QString &fileType ) ;
    void    saveAsFuelModelFile( const QString &fileName ) ;
    void    saveAsMoistureScenarioFile( const QString &fileName ) ;
    void    saveResults( const QString &fileName ) ;
    void    saveAsRunFile( const QString &fileName, bool clone=false ) ;
    void    saveAsUnitsSetFile( const QString &fileName ) ;
    void    saveAsWorksheetFile( const QString &fileName ) ;
    void    setPageTabs( void ) ;
    void    storeEntries( void ) ;
    void    storeNotes( void ) ;
    bool    validateWorksheet( void ) ;

// Public data members
public:
    //! Pointer to the parent EqApp instance.
    EqApp  *m_eqApp;
    //! Pointer to the dynamically-allocated EqTree class.
    EqTree *m_eqTree;
    //! Number of pages in the input worksheet
    int  m_worksheetPages;
    //! Id of the current entry field (NOT YET USED!)
    int  m_focusEntry;
    //! Set if the worksheet was edited since last composition.
    bool m_worksheetEdited;
    //! Determines if DpDocEntry::event() performs worksheet field validation
    bool m_doValidation;

    /*! \name Input Entry Field Member Data
     *  \brief Entry field locations and sizes on the input worksheet.
     *  These are typically set in composeWorksheet() and used in showPage().
     */
    //@{
    //! Array of pointers to dynamically-allocated entry field QLineEdits.
    QMemArray<BpDocEntry *> m_entry;
    //! Array of entry field input worksheet page numbers.
    QMemArray<int>          m_entryPage;
    //! Array of entry field X screen pixel positions.
    QMemArray<int>          m_entryX;
    //! Array of entry field Y screen pixel positions.
    QMemArray<int>          m_entryY;
    //! Array of entry field screen pixel heights.
    QMemArray<int>          m_entryHt;
    //! Array of entry field screen pixel widths.
    QMemArray<int>          m_entryWd;
    //@}

    /*! \name Guide Button Member Data
     *  \brief Guide button locations and sizes on the input worksheet.
     *  These are typically set in composeWorksheet() and used in showPage().
     *  Guide buttons use the page number and Y screen pixel position from
     *  their corresponding entry field's m_entryPage and m_entryY.
     */
    //@{
    //! Array of pointers to dynamically-allocated guide QPushButtons.
    QMemArray<QPushButton *> m_guideBtn;
    //! Array of guide button X screen pixel positions.
    QMemArray<int>           m_guideBtnX;
    //! Guide button screen pixel height.
    int                      m_guideBtnHt;
    //! Guide button screen pixel width.
    int                      m_guideBtnWd;
    //! Pointer to the dynamically-allocated guide button group for sharing callback slots.
    QButtonGroup            *m_guideBtnGrp;
    // Guide button pixmap.
    QPixmap                     m_guidePixmap;
    //@}

    /*! \name Additional Push Button Member Data
     *  \brief Additional push button locations and sizes on the input worksheet.
     *  These are typically set in composeWorksheet() and used in showPage().
     *  m_btn[0] is the \e Fuel \e Initialization button.
     *  m_btn[1] - m_btn[4] are (someday) the table navigation buttons.
     */
    //@{
    //! Array of dynamically-allocated pointers to additional QPushButtons.
    QMemArray<QPushButton *> m_btn;
    //! Array of push button input worksheet page numbers.
    QMemArray<int>           m_btnPage;
    //! Array of push button X screen pixel positions.
    QMemArray<int>           m_btnX;
    //! Array of push button Y screen pixel positions.
    QMemArray<int>           m_btnY;
    //! Array of push button screen pixel heights.
    QMemArray<int>           m_btnHt;
    //! Array of push button screen pixel widths.
    QMemArray<int>           m_btnWd;
    //@}

    /*! \name RX Entry Field Member Data
     *  \brief RX entry field locations and sizes on the input worksheet.
     *  These are typically set in composeWorksheet() and used in showPage().
     */
    //@{
    //! Array of pointers to dynamically-allocated rx entry field QLineEdits.
    QMemArray<QLineEdit *>  m_rxMinEntry;
    QMemArray<QLineEdit *>  m_rxMaxEntry;
    QMemArray<QCheckBox *>  m_rxCheckBox;
    QMemArray<QCheckBox *>  m_rxItemBox;
    QMemArray<int>          m_rxItemY;
    //! Array of rx entry field input worksheet page numbers.
    QMemArray<int>          m_rxEntryPage;
    //! Array of rx entry field X screen pixel positions.
    QMemArray<int>          m_rxEntryX;
    //! Array of rx entry field Y screen pixel positions.
    QMemArray<int>          m_rxEntryY;
    //! Array of rx entry field screen pixel heights.
    QMemArray<int>          m_rxEntryHt;
    //! Array of entry field screen pixel widths.
    QMemArray<int>          m_rxEntryWd;
    //@}

    /*! \name Worksheet Notes Section Member Data
     *  \brief Worksheet notes section location and size on the input worksheet.
     *  The notes section line is controlled by the \a worksheetNotesLines and
     *  \a worksheetNotesActive properties.
     */
    //@{
    //! Pointer to the dynamically-allocated rich text editor.
    QTextEdit      *m_notes;
    //! Notes section input worksheet page number.
    int             m_notesPage;
    //! Notes section X screen pixel location.
    int             m_notesX;
    //! Notes section Y screen pixel location.
    int             m_notesY;
    //! Notes section screen pixel width.
    int             m_notesWd;
    //! Notes section screen pixel height.
    int             m_notesHt;
    //@}
	int m_colDecimals;
	int m_rowDecimals;
};

#endif

//------------------------------------------------------------------------------
//  End of bpdocument.h
//------------------------------------------------------------------------------

