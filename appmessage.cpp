//------------------------------------------------------------------------------
/*! \file appmessage.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Application-wide, shared message handler functions.  Includes
 *  methods for the HelpDialog and MessageDialog classes.
 */

// Custom include files
#include "appdialog.h"
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "platform.h"
#include "textview.h"

// Qt include files
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtextedit.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \var AppGuiEnabled
 *  \brief Determines whether messages are displayed in dialogs or at terminal.
 */
static bool AppGuiEnabled = false;

//------------------------------------------------------------------------------
/*! \var AppLogFile
 *  \brief Application-wide log file name.
 */
static QString AppLogFile( "" );

//------------------------------------------------------------------------------
/*! \var AppLogFptr
 *  \brief Application-wide log file FILE pointer (0 if no log file).
 */
static FILE *AppLogFptr = 0;

//------------------------------------------------------------------------------
/*! \var AppTranslatorEnabled
 *  \brief If TRUE, messages to info(), warn(), error(), bomb(), and yesno()
 *  are treated as translation keys that are resolved through the
 *  translation dictionary for the current language.
 */
static bool AppTranslatorEnabled = false;

//------------------------------------------------------------------------------
/*! \var BombLevel
 *  \brief Determines the action taken by calls to bomb().
 *  \arg    0 = return to caller
 *  \arg    1 = call exit(1)
 *  \arg    2 = call abort()
 */
static int BombLevel = 1;

//------------------------------------------------------------------------------
/*! \brief HelpDialog constructor.
 *
 *  \param parent       Pointer to parent widget.
 *  \param captionKey   Caption text translator key.
 *  \param dismissKey   Dismiss button text translator key.
 *  \param htmlFile     HTML help file base name.
 */

HelpDialog::HelpDialog( QWidget *p_parent, const QString &captionKey,
        const QString &dismissKey, const QString &htmlFile ) :
    AppDialog(
        p_parent,                                 // Parent widget
        captionKey,                             // Caption
        "",                                     // No picture file
        "",                                     // No picture name
        htmlFile,                               // HelpBrowser file
        "helpBrowser",                          // Widget name
        dismissKey,                             // Accept button text key
        "" )                                    // No reject button
{
    // Hide the content pane
    midFrame()->hide();
    optionFrame()->hide();
    // Start small enough to fit on an 800x600 monitor.
    resize( 600, 400 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief MessageDialog constructor.
 *
 *  \param p_parent       Pointer to the parent widget
 *  \param captionKey   Translator key for dialog caption.
 *  \param pictureFile  Base name of picture file to display in the left pane.
 *  \param pictureName  Picture title.
 *  \param message      Messagew text to diaplay.
 *  \param p_name         Widget internal name
 *  \param acceptKey    Translator key for text displayed on the #m_acceptBtn
 *                      (default is "AppDialog:Button:Ok").
 *                      The #m_acceptBtn is always displayed.
 *  \param rejectKey    Translator key for text displayed on the #m_rejectBtn.
 *                      (default is "").
 *                      If NULL or empty, #m_rejectBtn is not displayed.
 */

MessageDialog::MessageDialog( QWidget *p_parent, const QString &captionKey,
        const QString &pictureFile, const QString &pictureName,
        const QString &message,     const char *p_name,
        const QString &acceptKey,   const QString &rejectKey ) :
    AppDialog( p_parent, captionKey, pictureFile, pictureName,
        "" /* No HelpBrowser */,  p_name, acceptKey, rejectKey ),
    m_textView(0)
{
    // Hide the content pane
    m_page->m_contentFrame->hide();
    // Add a text view to the main area
    m_textView = new TextView( m_page, "m_textView" );
    checkmem( __FILE__, __LINE__, m_textView, "TextView m_textView", 1 );
    m_textView->setTextFormat( Qt::RichText );
    m_textView->setText( message );
    m_textView->setReadOnly( true );
    // STart at a decent size
    int l_width = widthHint() + 300;
    int l_height = sizeHint().height();
    resize( l_width, ( l_height < 300 )
        ? 300
        : l_height );
    return;
}

//------------------------------------------------------------------------------
/*! \brief MessageDialog destructor.
 */

MessageDialog::~MessageDialog( void )
{
    delete m_textView;  m_textView = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets MessageDialog minimum width.
*
*   \param minWidth Minimum dialog width in pixels.
 */

void MessageDialog::setMinWidth( int minWidth )
{
    m_textView->setMinimumWidth( minWidth );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Converts the \a msg into HTML by
 *  -# converting "\\n" into "<BR>", and
 *  -# converting "\n" into "".
 *
 *  \param msg  Message to convert.
 */

void applyHtml( QString &msg )
{
    int pos;
    // The "\\n" is created by the XML parser whenever it sees a "\n"
    while ( ( pos = msg.find( "\\n" ) ) >= 0 )
    {
        msg.replace( pos, 2, "<BR>" );
    }
    // The "\n" is created by the XML parser whenever it sees a newline
    while ( ( pos = msg.find( "\n" ) ) >= 0 )
    {
        msg.replace( pos, 1, "<BR>" );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the AppGuiEnabled state.
 *
 *  \param enabled  If TRUE, all error(), fata(), info(), warn(), and yesno()
 *                  calls display there messages in a dialog.
 *                  Otherwise their messages are displayed to stderr.
 *
 *  \return TRUE if GUI is enabled, FALSE if not enabled.
 */

bool appGuiEnabled( bool enabled )
{
    return( AppGuiEnabled = enabled );
}

//------------------------------------------------------------------------------
/*! \brief Sets the AppTranslatorEnabled state.
 *
 *  \param enabled  If TRUE, all error(), fata(), info(), warn(), and yesno()
 *                  messages are treated as keys which are first translated
 *                  into the current language before being displayed.
 *                  Otherwise their messages are displayed verbatim.
 *
 *  \return TRUE if translator is enabled, FALSE if not enabled.
 */

bool appTranslatorEnabled( bool enabled )
{
    return( AppTranslatorEnabled = enabled );
}

//------------------------------------------------------------------------------
/*! \brief Displays a fatal error message and aborts.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param msg      Fully translated error message text to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void bomb( const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** FATAL: %1\n" ).arg( msg ) );

    // Display the message to the screen
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines to <BR>, etc.
            QString html( msg );
            applyHtml( html );
            // Display dialog with html and translated controls
            bombDialog( html, minWidth );
        }
        else
        {
            QMessageBox::critical( 0,
                QString( appWindow()->m_program + " " + appWindow()->m_version ),
                msg, "Quit" );
        }
    }
    // or to the terminal
    else
    {
        fprintf( stderr, "\n*** FATAL:\n    %s\n", msg.latin1() );
    }
    // Take action depending upon current BombLevel.
    if ( BombLevel == 1 )
    {
        platformExit(1);
    }
    else if ( BombLevel > 1 )
    {
        platformAbort();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays a fatal error dialog containing a picture,
 *  a scrollable rich text window with the message, and a single "Ok" button,
 *  then terminates the program with a core dump.
 *
 *  \param message  Fully translated error message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void bombDialog( const QString &message, int minWidth )
{
    // Build translated message with optional caption
    QString caption("");
    QString text("");
    translate( caption, "AppMessage:Caption:Fatal" );
    translate( text, "AppMessage:Text:Fatal" );
    QString str = QString( "<H3>%1</H3><HR>%2<P><B>%3</B>" )
        .arg( caption ).arg( message ).arg( text );

    // Display the MessageDialog
    qApp->beep();
    MessageDialog dialog(
        appWindow(),                    // Parent
        "AppMessage:Caption:Fatal",     // Dialog caption
        "BlueWolf2.png",                // Picture file
        "Blue Wolf",                    // Picture name
        str,                            // Message text
        "fatalDialog",                  // Widget name
        "AppMessage:Button:Abort" );    // Button text key
    dialog.setMinWidth( minWidth
        ? minWidth
        : 400 );
    dialog.exec();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the BombLevel, which determines the action taken by calls to
 *  bomb().
 *
 *  \param level == 0: normal return to caller
 *  \param level == 1: bomb() calls exit(1)
 *  \param level == 2: bomb() calls abort()
 *
 *  \return Current BombLevel value.
 */

int bombLevel( int level )
{
    return( BombLevel = level );
}

//------------------------------------------------------------------------------
/*! \brief Test function that checks that \a pointer is not zero.
 *  Should be called after every "new", "alloc()", or "malloc()" call.
 *
 *  \param fileName Caller's file name (usually specified as __FILE__).
 *  \param line     Caller's line number (usually specified as __LINE__).
 *  \param pointer  Pointer to test.
 *  \param itemDesc Pointer description (e.g., "MyClass myClass" or "double x").
 *  \param items    Number of the items that were allocated.
 *
 *  \return Never returns if \a pointer is 0;
 *          calls bomb() to report error and exit.
 */

void checkmem( const char *fileName, int line, void *pointer,
        const QString &itemDesc, int items )
{
    if ( ! pointer )
    {
        bomb( QString( "%1:%2 - unable to allocate %3[%4]" )
            .arg( fileName ).arg( line ).arg( itemDesc ).arg( items ) );
    }
}

//------------------------------------------------------------------------------
/*! \brief Displays an error message without a caption and returns.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param msg      Fully translated error message text to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void error( const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** ERROR:\n    %1\n" ).arg( msg ) );

    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines to <BR>, etc.
            QString html( msg );
            applyHtml( html );
            // Display without a caption.
            errorDialog( 0, html, minWidth );
        }
        else
        {
            QMessageBox::critical( 0,
                QString( appWindow()->m_program + " " + appWindow()->m_version ),
                msg, "Bummer" );
        }
    }
    // or to the terminal.
    else
    {
        fprintf( stderr, "\n*** ERROR:\n    %s\n", msg.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays an error caption and message and returns.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param caption  Fully translated error caption text to display.
 *  \param msg      Fully translated error message text to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void error( const QString &caption, const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** ERROR: %1\n    %2\n" ).arg( caption ).arg( msg ) );

    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines to <BR>, etc.
            QString html( msg );
            applyHtml( html );
            // Display with a caption.
            errorDialog( caption, html, minWidth );
        }
        else
        {
            QMessageBox::critical( 0, caption, msg, "Ok" );
        }
    }
    // or to the terminal.
    else
    {
        fprintf( stderr, "\n*** ERROR: %s\n    %s\n",
            caption.latin1(), msg.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays an error dialog containing a picture,
 *  a scrollable rich text window with the message, and a single "Ok" button.
 *
 *  \param caption  Fully translated caption text
 *  \param message  Fully translated message text
 *  \param minWidth Dialog minimum width in pixels
 */

void errorDialog( const QString &caption, const QString &message, int minWidth )
{
    // Build translated message with optional caption
    QString str("");
    if ( ! caption.isNull() )
    {
        str = QString( "<H3>%1</H3><HR>" ).arg( caption );
    }
    str += message;

    // Display the MessageDialog
    qApp->beep();
    MessageDialog dialog(
        appWindow(),                // Parent
        "AppMessage:Caption:Error", // Dialog caption
        "BlueWolf1.png",            // Picture file
        "Blue Wolf",                // Picture name
        str,                        // Message
        "errorDialog",              // Widget name
        "AppMessage:Button:Ok" );   // Button text key
    dialog.setMinWidth( minWidth
        ? minWidth
        : 400 );
    dialog.exec();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Help dialog convenience routine.
 *
 *  \param htmlFile Name of the HTML file to display.
 */

void helpDialog( const QString &htmlFile )
{
    // Display the help dialog
    HelpDialog dialog(
        appWindow(),
        "AppMessage:Caption:Help",      // Caption key
        "AppMessage:Button:Dismiss",    // Dismiss button text key
        htmlFile );
    dialog.exec();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays an informational message and returns.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param msg      Fully translated info message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void info( const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** FYI:\n    %1\n" ).arg( msg ) );

    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines into <BR>, etc.
            QString html( msg );
            applyHtml( html );
            infoDialog( html, minWidth );
        }
        else
        {
            QMessageBox::information( 0,
                QString( appWindow()->m_program + " " + appWindow()->m_version ),
                msg, "Ok" );
        }
    }
    // or to the terminal
    else
    {
        fprintf( stderr, "\n*** FYI:\n    %s\n", msg.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays an informational dialog containing a picture,
 *  a scrollable rich text window with the message,
 *  and a single "Ok" button.
 *
 *  \param message  Fully translated info message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void infoDialog( const QString &message, int minWidth )
{
    // Display the MessageDialog
    MessageDialog dialog(
        appWindow(),                // Parent
        "AppMessage:Caption:Info",  // Dialog caption
        "LandscapesOfTheMind.png",  // Picture file
        "Landscapes of the Mind",   // Picture name
        message,                    // Message
        "infoDialog",               // Widget name
        "AppMessage:Button:Ok" );   // Dialog button text key
    dialog.setMinWidth( minWidth
        ? minWidth
        : 400 );
    dialog.exec();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Writes the message to the log file (if one is open).
 */

void log( const QString &message, bool addLineFeed )
{
    static QString margin("");
    bool isBegin = false;
    const char *sep = "";
    if ( AppLogFptr )
    {
        if ( message.startsWith( "End Section: " ) )
        {
            margin = margin.mid( 4 );
            if ( margin.isNull() )
            {
                margin = "";
            }
        }
        else if ( ( isBegin = message.startsWith( "Beg Section: " ) ) )
        {
            sep = "\n";
        }
        fprintf( AppLogFptr, ( addLineFeed ? "%s%s%s\n" : "%s%s%s" ),
            sep, margin.latin1(), message.latin1() );
        fflush( AppLogFptr );
        if ( isBegin )
        {
            margin.append( "    " );
        }
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Closes the log file (if one is open).
 */

void logClose( void )
{
    if ( AppLogFptr )
    {
        fclose( AppLogFptr );
        AppLogFptr = 0;
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a log file for writing.
 */

bool logOpen( const char *fileName )
{
    if ( ( AppLogFptr = fopen( fileName, "w" ) ) != 0 )
    {
        AppLogFile = fileName;
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Constructs a release string in "1.23.45" format
 *  from the \a relNumber.
 *
 *  \param relNumber Release number where 10000 is release "1.00.00"
 *                   and 99999 is release "9.99.99"
 *  \param str Reference to the string to contain the release string.
 *
 *  \return The newlu constructed release string is returned in \a str.
 */

void releaseString( int relNumber, QString &str )
{
    // Display version information
    int major = relNumber / 10000;
    int minor = relNumber / 100 - major * 100;
    int patch = relNumber - major * 10000 - minor * 100;
    str.sprintf( "%d.%02d.%02d", major, minor, patch );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Replaces all new lines "\n" in \a str with single spaces.
 *
 *  \param str String whose newlines are to be replaced with spaces.
 *
 *  \return Copy of str with newlines replaced by spaces.
 */

QString stripNewlines( const QString &str )
{
    QString str2( str );
    int pos = -1;
    while ( ( pos = str2.find( "\n" ) ) >= 0 )
    {
        str2 = str2.replace( pos, 1, " " );
    }
    return( str2 );
}

//------------------------------------------------------------------------------
/*! \brief Displays a warning message and returns.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param msg      Fully translated message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void warn( const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** WARNING:\n    %1\n" ).arg( msg ) );

    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines to <BR>, etc.
            QString html( msg );
            applyHtml( html );
            // Display without a caption.
            warnDialog( 0, html, minWidth );
        }
        else
        {
            QMessageBox::warning( 0,
                QString( appWindow()->m_program + " " + appWindow()->m_version ),
                msg, "Ok" );
        }
    }
    // or to the terminal.
    else
    {
        fprintf( stderr, "\n*** WARNING:\n    %s\n", msg.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays a warning caption and message and returns.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param caption  Fully translated caption to display.
 *  \param msg      Fully translated message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void warn( const QString &caption, const QString &msg, int minWidth )
{
    // Log the message
    log( QString( "\n*** WARNING: %1\n    %2" ).arg( caption ).arg( msg ) );

    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines into <BR>, etc.
            QString html( msg );
            applyHtml( html );
            warnDialog( caption, html, minWidth );
        }
        else
        {
            QMessageBox::warning( 0, caption, msg, "Ok" );
        }
    }
    // or to the terminal
    else
    {
        fprintf( stderr, "\n*** WARNING: %s\n    %s",
            caption.latin1(), msg.latin1() );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays an warning dialog containing a picture,
 *  a scrollable rich text window with the message,
 *  and a single "Ok" button.
 *
 *  \param caption  Fully translated caption to display.
 *  \param msg      Fully translated message to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 */

void warnDialog( const QString &caption, const QString &message, int minWidth )
{
    // Build translated message with optional caption
    QString str("");
    if ( ! caption.isNull() )
    {
        str = QString( "<H3>%1</H3><HR>" ).arg( caption );
    }
    str += message;

    // Display the MessageDialog
    qApp->beep();
    MessageDialog dialog(
        appWindow(),                                // Parent
        "AppMessage:Caption:Warn",                  // Dialog caption
        "RestoringTheWolf.png",                     // Picture file
        "Restoring the Wolf to Yellowstone Park",   // Picture name
        str,                                        // Message
        "warnDialog",                               // Widget name
        "AppMessage:Button:Ok" );                   // Button text key
    dialog.setMinWidth( minWidth
        ? minWidth
        : 400 );
    dialog.exec();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Displays a Yes-or-No type question to the user and gets the result.
 *  If AppGuiEnabled, the message is displayed in a dialog box.
 *  Otherwise the message is printed to stderr.
 *
 *  \param caption  Fully translated caption to display.
 *  \param prompt   Fully translated prompt to display.
 *  \param minWidth Minimum width of the dialog (pixels)
 *
 *  \retval 1 if "Y" or "y" is pressed
 *  \retval 0 if any other character is pressed
 */

int yesno( const QString &caption, const QString &prompt, int minWidth )
{
    // Display the message to the screen ...
    if ( AppGuiEnabled )
    {
        if ( AppTranslatorEnabled )
        {
            // Convert newlines into <BR>, ect.
            QString html( prompt );
            applyHtml( html );
            return( yesnoDialog( caption, html, minWidth ) );
        }
        else
        {
            int btn = QMessageBox::information( 0, caption, prompt,
                "Yes", "No" );
            return( btn == 0 );
        }
    }

    // or from the terminal.
    // Get user keypress
    fprintf( stdout, "\n%s:\n%s (enter y or n and press RETURN): ",
            caption.latin1(), prompt.latin1() );
    char answer = fgetc( stdin );
    if ( answer == 'y' || answer == 'Y' )
    {
        fprintf( stdout, "[Yes]\n" );
        return( 1 );
    }
    fprintf( stdout, "[No]\n" );
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Requests a Yes-or-No answer from the user.
 *
 *  If AppGuiEnabled, displays a Yes-No dialog containing a picture,
 *  a scrollable rich text window with the prompt, and "Yes" and "No" buttons.
 *  Otherwise the prompt is printed to stdout and 'y' or 'n' is read from stdin.
 *
 *
 *  \param caption  Fully translated caption to display.
 *  \param message  Fully translated message to display.
 *  \param minWidth Minimum dialog width (pixels).
 *
 *  \retval 0 if the No button is pressed
 *  \retval 1 if the Yes button is pressed
 */

int yesnoDialog( const QString &caption, const QString &message, int minWidth )
{
    // Build translated message with optional caption
    QString str = QString( "<H3>%1</H3><HR>%2" ).arg( caption ).arg( message );

    // Display the MessageDialog
    qApp->beep();
    MessageDialog dialog(
        appWindow(),                // Parent
        "AppMessage:Caption:YesNo", // Dialog caption
        "CabinFever.png",           // Picture file
        "Cabin Fever",              // Picture name
        str,                        // Message
        "yesnoDialog",              // Widget name
        "AppMessage:Button:Yes",    // Accept button text key
        "AppMessage:Button:No" );   // Reject button text key
    dialog.setMinWidth( minWidth
        ? minWidth
        : 400 );
    return( dialog.exec() );
}

//------------------------------------------------------------------------------
//  End of appmessage.h
//------------------------------------------------------------------------------

