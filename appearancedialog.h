//------------------------------------------------------------------------------
/*! \file appearancedialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus AppearanceDialog class declaration.
 */

#ifndef _APPEARANCEDIALOG_H_
/*! \def _APPEARANCEDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _APPEARANCEDIALOG_H_ 1

// Forward class references
#include <qobject.h>
#include "propertydialog.h"
class BpDocument;
class QComboBox;
class QSpinBox;
class VarCheckBox;

//------------------------------------------------------------------------------
/*! \class AppearanceDialog appearancedialog.h
 *
 *  \brief AppearanceDialog allows the user to set BehavePlus appearance
 *  options and is invoked from the main menu by \b Configure->Appearance.
 *
 *  The AppearanceDialog uses PropertyDialog to display pages for
 *      - application-wide appearance options,
 *      - page tab appearance options,
 *      - output graph appearance options,
 *      - output table appearance options, and
 *      - input worksheet appearance options.
*/

class AppearanceDialog : public PropertyTabDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    AppearanceDialog( BpDocument *bp,
        const QString &captionKey="AppearanceDialog:Caption",
        const char *name="appearanceDialog",
        const QString &acceptKey="AppearanceDialog:Button:Accept",
        const QString &rejectKey="AppearanceDialog:Button:Reject" ) ;

// Protected methods
protected slots:
    virtual void store( void );
    void         tabsValueChanged( int ) ;

// Private data members
private:
    BpDocument  *m_bp;          //!< Pointer to the parent BpDocument
    QSpinBox    *m_tabPosition; //!< Pointer to the tab position spinbox item
    VarCheckBox *m_showBrowser; //!< Pointer to the application-wide property editor
    VarCheckBox *m_showPicture; //!< Pointer to the application-wide property editor
    QComboBox   *m_borderColor; //!< Pointer to the application-wide property editor
    QSpinBox    *m_borderWidth; //!< Pointer to the application-wide property editor
};

#endif

//------------------------------------------------------------------------------
//  End of appearancedialog.h
//------------------------------------------------------------------------------

