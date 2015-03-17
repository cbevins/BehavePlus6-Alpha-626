//------------------------------------------------------------------------------
/*! \file textviewdocument.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief TextViewDocument class methods.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "textviewdocument.h"

// Qt include files
#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qfontmetrics.h>
#include <qimage.h>
#include <qpaintdevicemetrics.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qtextstream.h>
#include <qworkspace.h>

//------------------------------------------------------------------------------
/*! \brief TextViewDocument class constructor.
 *
 *  \param workspace Pointer to the #ApplicationWindow::m_workSpace.
 *  \param docId     Unique serial number for the Document in this process.
 *  \param name      Document's QWidget name.
 *  \param wflags    QWidget #wflags.
 */

TextViewDocument::TextViewDocument( QWorkspace *workspace, int docId,
        const char* name, int wflags ) :
    Document( workspace, "TextViewDocument", docId, name, wflags )
{
    // Popup context menu must be created here because it is pure virtual
    contextMenuCreate();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::clear( bool /* showRunDialog */ )
{
    Document::clear();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composes the logo at the specified offset with the specified size.
 *
 *  Text files can use the default Document::contextMenuActivated() method
 *  which draws nothing.
 *
 *  \param x0 Horizontal distance from the left edge of the page in inches.
 *  \param y0 Vertical distance from top edge of the page in inches.
 *  \param wd Width in inches.
 *  \param ht Height in inches.
 *  \param penWd Pen width in pixels.
 */

void TextViewDocument::composeLogo( double x0, double y0, double wd, double ht,
    int penWd )
{
    Document::composeLogo( x0, y0, wd, ht, penWd );
}

//------------------------------------------------------------------------------
/*! \brief Composes the page background and decoration.
 *
 *  Text files can use the default Document::composeNewPage() method
 *  which creates a page with a white background and
 *  page number on the upper right header.
 *
 *  Should \b only be called by Document::startNewPage().
 */

void TextViewDocument::composeNewPage( void )
{
    Document::composeNewPage();
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configure( void )
{
    Document::configure();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configureAppearance( void )
{
    Document::configureAppearance();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configureFuelModels( void )
{
    Document::configureFuelModels();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configureMoistureScenarios( void )
{
    Document::configureMoistureScenarios();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configureModules( void )
{
    Document::configureModules();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::configureUnits( const QString &unitsSet )
{
    Document::configureUnits( unitsSet );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Slot called when the Document's popup context menu has been selected.
 *
 *  Text files can use the default Document::contextMenuActivated() method
 *  that provides save, saveAs, Print, and Capture callbacks.
 */

void TextViewDocument::contextMenuActivated( int id )
{
    Document::contextMenuActivated( id );
}

//------------------------------------------------------------------------------
/*! \brief Creates the Document's popup context menu.
 *
 *  Text files can use the default Document::contextMenuCreate() method
 *  that provides Save, saveAs, Print, and Capture options.
 *
 *  Called only by the Document() constructor.
 */

void TextViewDocument::contextMenuCreate( void )
{
    Document::contextMenuCreate();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens the selected file into the Document's DocScrollView.
 *
 *  Text files can use the default Document::open() method.
 *
 *  Called only by ApplicationWindow::slotOpen().
 *
 *  \param fileName Name of the Document file to open.
 *  \param fileType A one-word string describing the file type.  If this
 *  method does not recognize the string, an error message is displayed.
 *
 *  \retval TRUE if the file is successfully opened.
 *  \retval FALSE if the file couldn't be opened.
 */

bool TextViewDocument::open( const QString &fileName, const QString &fileType )
{
    return( Document::open( fileName, fileType ) );
}

//------------------------------------------------------------------------------
/*! \brief Prints the TextViewDocument file to a printer device.
 *
 *  Text files can use the default Document::print() method
 *  which uses the Composer to paint compser files onto the print device.
 *
 *  Called only by ApplicationWindow::print().
 *
 *  \return TRUE on success, FALSE on failure or cancel.
 */

bool TextViewDocument::print( void )
{
    return( Document::print() );
}

//------------------------------------------------------------------------------
/*! \brief Re-scales the TextViewDocument's fonts and screen display using the
 *  ratio of the new \a points to the default font point size as a scaling
 *  factor.
 *
 *  Text files can use the default Document::rescale() method
 *  which offers 12 scales based on font point sizes.
 *
 *  \param points Font point size used for a scale factor.
 */

void TextViewDocument::rescale( int points )
{
    Document::rescale( points );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::reset( bool /* showRunDialog */ )
{
    Document::reset();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Virtual method implementation for non-BehavePlusDocuments.
 */

void TextViewDocument::run( bool /* showRunDialog */ )
{
    Document::run();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Writes the current Document to a (possibly) new file location
 *  and updates the Document's file name.
 *
 *  Called only by ApplicationWindow::save().
 *
 *  \param fileName Name of the file to be (over)written.
 *  If zero (0), then a file selection dialog is displayed using the current
 *  #m_absPathName as the default.
 *  \param fileType A one-word string describing the file type.  If this
 *  method does not recognize the string, an error message is displayed.
 */

void TextViewDocument::save( const QString &fileName, const QString &fileType )
{
    // Check if we can generate this type of output file
    if ( fileType != "Native" && fileType != "Text"
     &&  fileType != "native" && fileType != "text" )
    {
        QString text("");
        translate( text, "Document:Save:WrongType", fileType );
        error( text );
        return;
    }
    // Request the fileName if one wasn't specified
    QString newFileName = fileName;
    if ( newFileName == 0 )
    {
        newFileName = QFileDialog::getSaveFileName(
            m_absPathName,
            "*.*",
            this );
    }
    // Return if no fileName was selected
    if ( newFileName == 0 )
    {
        return;
    }
    // Store the new file name in m_absPathName and m_basename
    QFileInfo fi( newFileName );
    m_absPathName = fi.absFilePath();
    m_baseName = fi.baseName();
    // Save the file
    warn( "TextViewDocument::save() -- put some code to save it here." );
    // Update the status bar
    statusUpdate();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates the ApplicationWindow's \b View submenu options just
 *  before it displays itself.
 *
 *  Text files can use the default Document::rescale() method
 *  which offers 12 scales based on font point sizes.
 *
 *  \param viewMenu Pointer to the \b View menu.
 */

void TextViewDocument::viewMenuAboutToShow( QPopupMenu *viewMenu )
{
    Document::viewMenuAboutToShow( viewMenu );
    return;
}

//------------------------------------------------------------------------------
//  End of textviewdocument.cpp
//------------------------------------------------------------------------------

