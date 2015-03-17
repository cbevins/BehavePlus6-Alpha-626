//------------------------------------------------------------------------------
/*! \file calendardocument.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Declaration of the CalendarDocument class.
 */

#ifndef _CALENDARDOCUMENT_H_
/*! \def _CALENDARDOCUMENT_H_
 *  \brief Prevent redundant includes.
 */
#define _CALENDARDOCUMENT_H_ 1

// Forward class references
#include <qmainwindow.h>
#include "document.h"
#include "toc.h"

// Class declarations
class Composer;
class DateTime;
class DocDeviceSize;
class DocPageSize;
class DocScrollView;
class DocTabs;
class DocumentPriv;
class GlobalPosition;
class PropertyDict;
class QPopupMenu;
class QString;
class QWorkspace;

//------------------------------------------------------------------------------
/*! \class CalendarDocument calendardocument.h
 *
 *  \brief The CalendarDocument class calculates and displays the seasonal
 *  dates and monthly sun-moon calendars for a geographic location.
 *
 *  BehavePlus uses a single ApplicationWindow instance to define the
 *  main application window with a main menu bar, tool bar, status bar, and
 *  a \e Multiple \e Document \e Interface (MDI).  The MDI allows multiple
 *  Document instances, including CalendarDocuments, to be open simultaneously.
 *
 *  Because CalendarDocuments are derived from the Document abstract base
 *  class, they can:
 *      - read files from disk,
 *      - display them in the scrolling view port,
 *      - scale the display page,
 *      - navigate from page to page,
 *      - print one or more pages,
 *      - capture pages to an image file, and
 *      - save the file under a new name.
 *
 *  The SunDialog, GeoCatalogDialog, and GeoPlaceDialog classes are used to
 *  gather user inputs used by CalendarDocument.  The private methods
 *  composeCalendar(), composeChart(), and composeSeasons() perform the
 *  output composition.
 *
 *  \sa GeoCatalogDialog
 *  \sa GeoPlaceDialog
 *  \sa SunDialog
*/

class CalendarDocument : public Document
{
// Enable signals and slots on this class
    Q_OBJECT

//! Enumerates the available context menu options.
enum ContextMenuOption
{
    //! Saves the current values to the Run file.
    ContextSave=0,
    //! Saves the current values to another Run, Worksheet, Fuel Model, or Moisture Scenario file.
    ContextSaveAs=1,
    //! Prints one or more pages of the current run.
    ContextPrint=2,
    //! Captures an image of the current run page.
    ContextCapture=3,
    //! Calculates calendar using current input values.
    ContextRun=4
};

// Public methods
public:
    CalendarDocument( QWorkspace *workspace, int docId,
        PropertyDict *pd, const char* name, int wflags ) ;
    virtual ~CalendarDocument( void ) ;

// Public methods that may be called by the ApplicationWindow class.
    virtual void clear( bool showRunDialog=true ) ;
    virtual void composeLogo( double x0, double y0,
                 double wd, double ht, int penWd ) ;
    virtual void configure( void );
    virtual void configureAppearance( void ) ;
    virtual void configureFuelModels( void ) ;
    virtual void configureMoistureScenarios( void ) ;
    virtual void configureModules( void ) ;
    virtual void configureUnits( const QString &unitsSet ) ;
    virtual bool open( const QString &fileName, const QString &fileType ) ;
    virtual bool print( void ) ;
    virtual void reset( bool showRunDialog=true ) ;
    virtual void run( bool showRunDialog=true ) ;
    virtual void save( const QString &fileName, const QString &fileType ) ;
    virtual void viewMenuAboutToShow( QPopupMenu *viewMenu ) ;

// Public methods called by ApplicationWindow.
    void composeCalendar( DateTime *dt, GlobalPosition *gp ) ;
    void composeChart( DateTime *dt, GlobalPosition *gp ) ;
    void composeSeasons( DateTime *dt, GlobalPosition *gp ) ;

// Protected slots
protected slots:
    virtual void contextMenuActivated( int id ) ;
    virtual void rescale( int points ) ;

// Protected methods called only from within this class or its derivatives
protected:
    virtual void composeNewPage( void ) ;
    virtual void contextMenuCreate( void ) ;

// Data members are protected so they can be accessed by derived classes.
protected:
    //! Stored pointer to the dictionary containing "cal*" properties.
    PropertyDict *m_prop;
};

#endif

//------------------------------------------------------------------------------
//  End of calendardocument.h
//------------------------------------------------------------------------------

