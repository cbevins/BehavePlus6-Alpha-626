//------------------------------------------------------------------------------
/*  \file document.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief MDI document base class declaration.
 */

#ifndef _DOCUMENT_H_
/*! \def _DOCUMENT_H_
    \brief Prevent redundant includes.
 */
#define _DOCUMENT_H_ 1

// Custom class references
class Composer;
class DocDeviceSize;
class DocPageSize;
class DocScrollView;
class DocTabs;
class FileSys;
#include "toc.h"

// Qt class references
#include <qmainwindow.h>
class QEvent;
class QObject;
class QPopupMenu;
class QString;
class QWorkspace;

//------------------------------------------------------------------------------
/*! \class Document document.h
 *
 *  \brief MDI Document abstract base class.
 *
 *  The Document class is an abstract base class (e.g., it has pure virtual
 *  functions) for opening and displaying various types of documents within
 *  the ApplicationWindow.
 *
 *  A new concrete class must be derived for each type of Document we wish
 *  to handle.  Some potential derived classes include:
 *  \arg TextViewDocument (implemented)
 *  \arg TextEditDocument
 *  \arg GraphicsDocument
 *  \arg BehavePlusDocument
 *
 *  Each type of Document supported by the application must be derived from
 *       this base class and implement the following pure virtual methods:
 *  \arg composeLogo() Composes the Document's logo on a device.
 *  \arg composeNewPage() Composes a new page with decoration.
 *  \arg contextMenuActivated() Invokes the context methods.
 *  \arg contextMenuCreate() Creates the context menu.
 *  \arg open() Opens and reads the file and displays it in the scrollable
 *       view port
 *  \arg print()
 *  \arg rescale() Rescales the displayed page in an appropriate manner.
 *  \arg save()
 *  \arg saveAs()
 *  \arg setRunTime() (optional, not pure virtual)
 *  \arg viewMenuAboutToShow() Populates the \b View submenu with
 *       appropriate scale measures
 *
 *  Default versions of the above methods are provided.
 *
 *  The Document abstract class supports the following behaviors:
 *  \arg The Document is read from and written to a single file.
 *  \arg The Document may be displayed in one or more pages.
 *  \arg The active page is displayed in a scrollable viewport.
 *  \arg Navigation is provided between pages.
 *  \arg A \b Contents submenu provides additional page selection.
 *  \arg A popup context menu provides access to file operations.
 */

class Document : public QMainWindow
{
// Enable signals and slots on this class
    Q_OBJECT

/*! \enum ContextMenuOption
    \brief Enumerates the types of Document context menu options available.
 */
enum ContextMenuOption
{
    ContextSave=0,
    ContextSaveAs=1,
    ContextPrint=2,
    ContextCapture=3,
    ContextRun=4
};

// Public methods
public:
    Document( QWorkspace *workspace, const QString &docType, int docId,
                const char* name, int wflags ) ;
    virtual ~Document( void ) ;
    virtual bool capture( void ) ;
    virtual void clear( bool showRunDialog=true ) = 0 ;
    virtual void composeLogo( double x0, double y0, double wd, double ht,
            int penWd ) = 0;
    virtual void configure( void ) = 0;
    virtual void configureAppearance( void ) = 0;
    virtual void configureFuelModels( void ) = 0;
    virtual void configureMoistureScenarios( void ) = 0;
    virtual void configureModules( void ) = 0;
    virtual void configureUnits( const QString &unitsSet ) = 0;
    void contentsAddItem( int page, const QString &pageTitle, TocType tocType ) ;
    void contentsClear( void ) ;
    void contentsMenuActivated( int id ) ;
    void contentsMenuRebuild( QPopupMenu *contents ) ;
    void contextMenuShow( void ) ;
    int  fontScaleSize( void ) const ;
    virtual void maintenance( void ) ;
    virtual bool open( const QString &fileName, const QString &fileType ) = 0;
    void pageFirst( void ) ;
    void pageLast( void ) ;
    void pageNext( void ) ;
    void pagePrev( void ) ;
    int  pageHtPixels( void ) const ;
    int  pageWdPixels( void ) const ;
    virtual bool print( void ) = 0;
    virtual bool printPS( int fromPage, int thruPage ) ;
    virtual void removeComposerFiles( int fromPageNumber=1 ) ;
    virtual void reset( bool showRunDialog=true ) = 0 ;
    virtual void run( bool showRunDialog=true ) = 0 ;
    virtual void save( const QString &fileName, const QString &fileType ) = 0;
    virtual void setRunTime( void ) ;
    virtual void showPage( int pageNo ) ;
    void startNewPage( const QString &tocName, TocType tocType ) ;
    void status( const QString &msg ) ;
    void statusUpdate( void ) ;
    virtual void viewMenuAboutToShow( QPopupMenu *viewMenu ) = 0;

// Public signals
signals:
    void message( const QString &msg ) ;

// Protected slots
protected slots:
    virtual void contextMenuActivated( int id ) = 0;
    virtual void rescale( int points ) = 0;

// Protected methods
protected:
    virtual void composeNewPage( void ) = 0;
    virtual void composeTab( void ) ;
    virtual void contextMenuCreate( void ) = 0;

// Public data members
public:
    DocScrollView  *m_scrollView;   //!< Displays a single Document's page
    DocPageSize    *m_pageSize;     //!< Document page dimensions (inches)
    DocDeviceSize  *m_screenSize;   //!< Document screen dimensions (pixels)
    TocList        *m_tocList;      //!< Document's Table of Contents
    QPopupMenu     *m_contextMenu;  //!< Context menu invoked by right mouse button
    QPopupMenu     *m_maintenanceMenu;  //!< Maintenance menu
    Composer       *m_composer;     //!< Composer for drawing to composer files
    DocTabs        *m_tabs;         //!< Pointer to Document tabs
    QString         m_docType;      //!< Document type ("BehavePlus", "Text", etc. )
    QString         m_absPathName;  //!< Document file's full absolute path name
    QString         m_baseName;     //!< Document file's base name (no extension)
    QString         m_runTime;      //!< Run time stamp
    QFont           m_fixedFont;    //!< Document's fixed font
    QFont           m_propFont;     //!< Document's proportional font
    int             m_fontBaseSize; //!< Font point size at 100% view
    int             m_fontScaleSize;//!< Font point size at current scale
    int             m_docId;        //!< Serial number unique to this process
    int             m_pages;        //!< Document's total pages (composer files)
    int             m_page;         //!< Current page in DocScrollView
};

#endif

//------------------------------------------------------------------------------
//  End of document.h
//------------------------------------------------------------------------------

