//------------------------------------------------------------------------------
/*! \file varcheckbox.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief QCheckBox that displays its informational text in a QTextView
 *  whenever the mouse enters the QCheckBox.
 */

#ifndef _VARCHECKBOX_H_
/*! \def _VARCHECKBOX_H_
    \brief Prevent redundant includes.
 */
#define _VARCHECKBOX_H_ 1

// Class references
#include <qcheckbox.h>
class HelpBrowser;
class QEvent;
class QTimer;

//------------------------------------------------------------------------------
/*! \class VarCheckBox varcheckbox.h
 *
 *  \brief An extended QCheckBox that displays informational text in a
 *  HelpBrowser whenever the mouse pauses over it.
*/

class VarCheckBox : public QCheckBox
{
// Enable signals and slots
    Q_OBJECT
// Methods
public:
    VarCheckBox( const QString &buttonText, const QString &htmlFile,
        HelpBrowser *browser, QWidget *parent, const char *name=0 ) ;
    ~VarCheckBox( void ) ;

protected:
    virtual bool event( QEvent *e ) ;

protected slots:
    virtual void timerDone( void ) ;

// Private data members
private:
    QString      m_html;        //!< Name of the HTML file to display
    HelpBrowser *m_browser;     //!< Pointer to the shared HelpBrowser
    QTimer      *m_timer;       //!< Pointer to the message display timer
};

#endif

//------------------------------------------------------------------------------
//  End of varcheckbox.h
//------------------------------------------------------------------------------

