//------------------------------------------------------------------------------
/*! \file requestdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief RequestDialog class declaration.
 */

// Custom include files
#include "requestdialog.h"

// Qt include files
#include <qlabel.h>
#include <qlineedit.h>

//------------------------------------------------------------------------------
/*! \brief Constructs a Request dialog with the \a prompt and an initial
 *  value \a initVal.
 *
 *  \param prompt   Fully translated prompt.
 *  \param initVal  Initial text that appears in the entry field.
 *  \param parent   Pointer to the #QWidget parent.
 *  \param name     Name for this #QWidget.
 */

RequestDialog::RequestDialog( const QString &prompt, const QString &initVal,
        const QString &htmlFile, QWidget *parent, const char *name ) :
    AppDialog(
        parent,                     // Parent widget
        "RequestDialog:Caption",    // Caption
        "AfterHours.png",           // Picture file
        "After Hours",              // Picture name
        htmlFile,                   // HelpBrowser file
        name ),                     // Widget name
    m_entry(0)
{
    m_label = new QLabel( prompt, contentFrame(), "m_requestLabel" );
    Q_CHECK_PTR( m_label );

    m_entry = new QLineEdit( initVal, contentFrame(), "m_requestEntry" );
    Q_CHECK_PTR( m_entry );
    return;
}

//------------------------------------------------------------------------------
/*! \brief A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

RequestDialog::~RequestDialog( void )
{
    delete m_entry;     m_entry = 0;
    delete m_label;     m_label = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current text in the Request entry field.
 *
 *  \param str Reference to a QString to contain the returned text.
 */

void RequestDialog::text( QString &str )
{
    str = m_entry->text().stripWhiteSpace();
    return;
}

//------------------------------------------------------------------------------
//  End of requestdialog.cpp
//------------------------------------------------------------------------------

