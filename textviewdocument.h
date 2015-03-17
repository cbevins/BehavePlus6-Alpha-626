//------------------------------------------------------------------------------
/*! \file textviewdocument.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief TextViewDocument class declaration.
 */

#ifndef _TEXTVIEWDOCUMENT_H_
/*! \def _TEXTVIEWDOCUMENT_H_
    \brief Prevent redundant includes.
 */
#define _TEXTVIEWDOCUMENT_H_ 1

// All class references are in document.h
#include "document.h"

//------------------------------------------------------------------------------
/*! \class TextViewDocument textviewdocument.h
 *
 *  \brief TextViewDocument displays an ASCII text file.
 *
 *  This class is derived from the Document abstract base class, so it can
 *  \arg read files from disk,
 *  \arg display them in the scrolling view port,
 *  \arg scale the display page,
 *  \arg navigate from page to page,
 *  \arg print one or more pages,
 *  \arg capture pages to an image file, and
 *  \arg save the file under a new name.
 *
 *  Because it is the least specialized of the derived Document classes,
 *  it only uses the Document default virtual methods as its implementation,
 *  which makes it really light weight.
 */

class TextViewDocument : public Document
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
    TextViewDocument( QWorkspace *workspace, int docId, const char* name,
        int wflags ) ;
// Public methods that may be called by AppWindow
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

// Protected slots
protected slots:
    virtual void contextMenuActivated( int id ) ;
    virtual void rescale( int points ) ;

// Protected methods called only from within this class or its derivatives
protected:
    virtual void composeNewPage( void ) ;
    virtual void contextMenuCreate( void ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of textviewdocument.h
//------------------------------------------------------------------------------

