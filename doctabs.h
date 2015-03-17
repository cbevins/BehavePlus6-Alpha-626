//------------------------------------------------------------------------------
/*  \file doctabs.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief DocTabs class declarations.
 */

#ifndef _DOCTABS_H_
/*! \def _DOCTABS_H_
    \brief Prevent redundant includes.
 */
#define _DOCTABS_H_ 1

// Qt class references
#include <qcolor.h>
class QFont;
class QString;

//------------------------------------------------------------------------------
/*! \class DocTabs doctabs.h
 *
 *  \brief Class to display cut tabs along the long side of a Document page.
 */

class DocTabs
{
// Public methods
public:
    DocTabs( void );
    DocTabs( int tab, int tabs, const QString &text ) ;
    DocTabs( int tab, int tabs, const QString &text, const QFont &font,
        const QColor &color ) ;
    bool            active( void ) const ;
    bool            setActive( bool isActive ) ;
    void            setFont( const QString &fontFamily, int fontSize,
                        const QColor &fontColor ) ;
    const QColor   &setFontColor( const QColor &color ) ;
    const QString  &setFontFamily( const QString &family ) ;
    int             setFontSize( int points ) ;
    int             setTab( int tab ) ;
    void            setTab( int tab, int tabs, const QString &tabText ) ;
    int             setTabs( int tabs ) ;
    const QString  &setTabText( const QString &text ) ;

// Private data members.
public:
    bool    m_active;       //!< Tab is drawn only if this is TRUE
    int     m_tab;          //!< Tab shown on this document (base 1)
    int     m_tabs;         //!< Number of tabs possible per page (for tab length)
    int     m_fontSize;     //!< Tab font size
    QColor  m_fontColor;    //!< Tab font color
    QString m_fontFamily;   //!< Tab font family
    QString m_tabText;      //!< Text to appear on the tab
};

#endif

//------------------------------------------------------------------------------
//  End of doctabs.h
//------------------------------------------------------------------------------

