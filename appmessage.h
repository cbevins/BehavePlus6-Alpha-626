//------------------------------------------------------------------------------
/*! \file appmessage.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared logging and message handler functions.
 *  Includes more advanced methods for the HelpDialog and MessageDialog classes.
 *
 *  These functions provide an application-wide mechanism for displaying
 *  error, fatal, help, informational, and warning messages to a log file,
 *  standard error device, QMessage box, and/or custom dialog box with,
 *  language translation.
 *
 *  Normally an application progresses through a series of states as it is
 *  initialized.  First the log file is opened, then the gui is enabled,
 *  then  the translation and custom image files are read, and finally all
 *  services are available.
 *
 *  The log() function simply writes the message to a log file (if one is
 *  open).  logOpen() opens a log file and logClose() closes it.  An
 *  application may call logOpen() at startup and logClose() as its exits,
 *  then make any other log() calls in between as desired.  Calls to
 *  info(), warn(), error(), and bomb() also write to the log file.
 *
 *  The info(), warn(), error(), bomb(), and yesno() functions operate
 *  differently depending upon the application state variables.
 *
 *  If AppGuiEnabled is TRUE, their messages are displayed in a dialog window.
 *
 *  If AppGuiEnabled is FALSE, messages are displayed to stderr or stdout.
 *
 *  The application controls this switch via appGuiEnabled( bool enabled ).
 *
 *  If AppTranslatorEnabled is TRUE, the custom AppDialog is used to display
 *  messages (which are presumed to have already been translated).
 *
 *  If AppTranslatorEnabled is FALSE, messages are displayed in an
 *  appropriate QMessageBox box.
 *
 *  Calls to bomb() cause an abort() (if ABortOnBomb is TRUE) and never return.
 *  Calls to error(), info(), warn(), and yesno() do return.
 *
 *  Some message functions are overloaded to display just a message,
 *  or to display a caption and a message.
 */

#ifndef _APPMESSAGE_H_
/*! \def _APPMESSAGE_H_
 *  \brief Prevent redundant includes.
 */
#define _APPMESSAGE_H_ 1

// Custom class references
#include "appdialog.h"
class TextView;

// Qt class references
#include <qobject.h>
#include <qstring.h>
#include <qdialog.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class HelpDialog appmessage.h
 *
 *  \brief Standard application help browser dialog class.
 */

class HelpDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    HelpDialog( QWidget *p_parent, const QString &caption,
        const QString &dismissKey, const QString &htmlFile ) ;
};

//------------------------------------------------------------------------------
/*! \class MessageDialog appmessage.h
 *
 *  \brief The MessageDialog is the standard application message dialog class.
 *  It has no HelpBrowser, just an optional picture and a TextView content area.
 *
 *  It is usually only created/destroyed by the convenience functions
 *  error(), fatal(), help(), info(), warn(), and yesno().
 */

class MessageDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    MessageDialog( QWidget *p_parent, const QString &caption,
        const QString &pictureFile, const QString &pictureTip,
        const QString &message, const char *p_name,
        const QString &acceptText="AppDialog:Button:Ok",
        const QString &rejectText="" ) ;
    ~MessageDialog( void ) ;
    void setMinWidth( int minWidth ) ;

// Private data
private:
    TextView *m_textView;   //!< Pointer to scrollable TextView.
};

//------------------------------------------------------------------------------
/*! \brief Convenience rotuines.
 */

bool appGuiEnabled( bool enabled ) ;

bool appTranslatorEnabled( bool enabled ) ;

void applyHtml( QString &msg ) ;

void bomb( const QString &message, int minWidth=0 ) ;
void bombDialog( const QString &message, int minWidth ) ;
int  bombLevel( int level ) ;


void checkmem( const char *fileName, int line, void *pointer,
        const QString &itemName, int items ) ;

void error( const QString &message, int minWidth=0 ) ;
void error( const QString &caption, const QString &message, int minWidth=0 ) ;
void errorDialog( const QString &caption, const QString &message, int minWidth ) ;

void helpDialog( const QString &htmlFile="index.html" ) ;

void info( const QString &message, int minWidth=0 );
void infoDialog( const QString &message, int minWidth ) ;

void log( const QString &message, bool addLinefeed=false ) ;
void logClose( void );
bool logOpen( const char *fileName );

void releaseString( int relNumber, QString &str ) ;

QString stripNewlines( const QString &str ) ;

void warn( const QString &message, int minWidth=0 ) ;
void warn( const QString &caption, const QString &message, int minWidth=0 ) ;
void warnDialog( const QString &caption, const QString &message, int minWidth ) ;

int  yesno( const QString &caption, const QString &message, int minWidth=0 ) ;
int  yesnoDialog( const QString &caption, const QString &message, int minWidth ) ;

#endif

//------------------------------------------------------------------------------
//  End of appmessage.h
//------------------------------------------------------------------------------

