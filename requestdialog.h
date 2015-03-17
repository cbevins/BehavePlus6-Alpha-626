//------------------------------------------------------------------------------
/*! \file requestdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief RequestDialog class declaration.
 */

#ifndef _REQUESTDIALOG_H_
/*! \def _REQUESTDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _REQUESTDIALOG_H_ 1

// Class references
#include "appdialog.h"
class QLineEdit;

//------------------------------------------------------------------------------
/*! \class RequestDialog requestdialog.h
 *
 *  \brief Dialog to request a single input item.
 */

class RequestDialog : public AppDialog
{
// Public methods
public:
    RequestDialog( const QString &prompt, const QString &initVal,
        const QString &htmlFile, QWidget *parent, const char *name=0 ) ;
    ~RequestDialog( void ) ;
    void text( QString &str ) ;

// Private data members
private:
    QLabel    *m_label; //!< Pointer to the entry QLabel
    QLineEdit *m_entry; //!< Pointer to the entry field widget
};

#endif

//------------------------------------------------------------------------------
//  End of requestdialog.h
//------------------------------------------------------------------------------

