//------------------------------------------------------------------------------
/*! \file bpdocentry.cpp
 *  \version BehavePlus5
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocEntry class methods.
 */

// Custom include files
#include "bpdocentry.h"
#include "bpdocument.h"

// Qt include files
#include <qlineedit.h>

//------------------------------------------------------------------------------
/*! \brief BpDocEntry constructor.
 */

BpDocEntry::BpDocEntry( BpDocument *dptr, int id, QWidget *p_parent,
        const char *p_name ) :
    QLineEdit( "", p_parent, p_name ),
    m_doc(dptr),
    m_id(id),
	m_validate(true)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Handles RightButton mouse clicks in the entry field.
 *
 *  \param event Pointer to the QMouseEvent.
 */

void BpDocEntry::mousePressEvent( QMouseEvent *p_event )
{
    ButtonState bs = p_event->button();
    if ( bs == RightButton )
    {
        m_doc->worksheetEntryClicked( m_id );
    }
    else
    {
        QLineEdit::mousePressEvent( p_event );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Traps attempts to leave the entry field so we can validate the
 *  contents and store them.
 *
 *  \bug
 *  Traps FocusOut events when the user clicks on the Guide button while the
 *  current entry field has invalid contents.
 *
 *  \param event Pointer to the QFocusEvent.
 *
 *  \return TRUE if the event is handled, FALSE if nobody wants it.
 */

bool BpDocEntry::event( QEvent *e )
{
    // Just catch key press events on the BpDocEntry
	int route = 0;
    if ( e->type() == QEvent::KeyPress )
    {
        // Backward field movements
        QKeyEvent *k = (QKeyEvent *)e;
        if ( k->key() == Key_Backtab
        || ( k->key() == Key_Tab && ( k->state() & ShiftButton ) )
        || ( k->key() == Key_Return && ( k->state() & ShiftButton ) )
        || ( k->key() == Key_Enter && ( k->state() & ShiftButton ) ) )
        {
			route = 1;
            // Validate the field before we leave it
            if ( m_doc->m_doValidation && edited() && ! valid() )
            {
                return( true );
            }
            // Move focus to the previous field and select the whole thing
            QFocusEvent::setReason( QFocusEvent::Tab );
            m_doc->focusPrev( 0, -1 );
            QFocusEvent::resetReason();
            return( true );
        }
        // Forward field movements
        else if ( k->key() == Key_Tab
               || k->key() == Key_Return
               || k->key() == Key_Enter )
        {
			route = 2;
            //fprintf( stderr, "%s: Tab\n", name() );
            // Validate the field before we leave it
            if ( m_doc->m_doValidation && edited() && ! valid() )
			{
                return( true );
            }
            // Move focus to the next field and select the whole thing
            QFocusEvent::setReason( QFocusEvent::Tab );
            m_doc->focusNext( 0, -1 );
            QFocusEvent::resetReason();
            return( true );
        }
    }
    // Focus out
    else if ( e->type() == QEvent::FocusOut )
    {
        QFocusEvent *f = (QFocusEvent *)e;
        if ( f->reason() == QFocusEvent::Mouse
          || f->reason() == QFocusEvent::Tab
          || f->reason() == QFocusEvent::Other
		)
        {
			route = 3;
            //fprintf( stderr, "%s: FocusOut\n", name() );
            // Validate the field before we leave it
            if ( m_doc->m_doValidation && edited() )
			{
				if ( ! valid() )
				{
					return( true );
				}
            }
        }
    }
    else if ( e->type() == QEvent::FocusIn )
    {
		route = 4;
        fprintf( stderr, "%s: FocusIn\n", name() );
    }
    return( QWidget::event( e ) );
}

//------------------------------------------------------------------------------
/*! \brief Validates the BpDocEntry contents.
 */

bool BpDocEntry::valid( void )
{
    // Validate the entry; blanks are ok for now.
    int tokens, l_pos, len;
    if ( ! m_doc->validateWorksheetEntry( m_id, text(), &tokens, &l_pos, &len ) )
    {
        // Invalid, so retain focus here and return FALSE
		if ( m_validate )
		{
			m_doc->focusThis( m_id, l_pos, len );
			return( false );
		}
    }
    // Valid, so mark it as unedited and return TRUE
    setEdited( false );
    return( true );
}

//------------------------------------------------------------------------------
//  End of bpdocentry.cpp
//------------------------------------------------------------------------------

