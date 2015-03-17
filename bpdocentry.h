//------------------------------------------------------------------------------
/*! \file bpdocentry.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocEntry class declaration.
 */

#ifndef _BPDOCENTRY_H_
/*! \def _BPDOCENTRY_H_
    \brief Prevent redundant includes.
 */
#define _BPDOCENTRY_H_ 1

// Class references
#include <qlineedit.h>
class BpDocument;
class QEvent;

//------------------------------------------------------------------------------
/*! \class BpDocEntry bpdocentry.h
 *  \brief Document entry field class to provide special handling of
 *  focus and mouse events.
 */

class BpDocEntry : public QLineEdit
{
// Public methods
public:
    BpDocEntry( BpDocument *dptr, int id, QWidget *p_parent, const char *p_name ) ;

// Protected methods
protected:
    virtual bool event( QEvent *e ) ;
    virtual void mousePressEvent( QMouseEvent *p_event ) ;

// Private methods
private:
    bool valid( void ) ;

// Public data
public:
    BpDocument *m_doc;  //!< Pointer to the parent BpDocument.
    int         m_id;   //!< This entry's index into the BpDocument m_entry[]
	bool        m_validate;	//!< V5.0.5 validation flag
};

#endif

//------------------------------------------------------------------------------
//  End of bpdocentry.h
//------------------------------------------------------------------------------
