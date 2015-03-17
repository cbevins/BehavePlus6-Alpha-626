//------------------------------------------------------------------------------
/*  \file doctabs.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief DocTabs class methods.
 */

// Custom include files
#include "doctabs.h"

// Qt include files
#include <qfont.h>
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \brief DocTabs default constructor.
 */

DocTabs::DocTabs( void ) :
    m_active(false),
    m_tab(0),
    m_tabs(0),
    m_fontSize(16),
    m_fontColor("black"),
    m_fontFamily("Times New Roman"),
    m_tabText("")
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief DocsTabs custom constructor.
 *
 *  \param tab This tab number (1 - \a tabs).
 *  \param tabs Number of tab positions along the edge of the page.
 *  \param text Tab text.
 */

DocTabs::DocTabs( int tab, int tabs, const QString &text ) :
    m_active(false),
    m_tab(tab),
    m_tabs(tabs),
    m_fontSize(16),
    m_fontColor("black"),
    m_fontFamily("Times New Roman"),
    m_tabText(text)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the current page tab active state.
 *
 *  \return TRUE if the page tab is active, FALSE if inactive.
 */

bool DocTabs::active( void ) const
{
    return( m_active && m_tab && m_tabs );
}

//------------------------------------------------------------------------------
/*! \brief Sets the page tab active state.
 *
 *  \return TRUE if the page tab is active, FALSE if inactive.
 */

bool DocTabs::setActive( bool isActive )
{
    m_active = isActive;
    return( m_active && m_tab && m_tabs );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current tab font values.
 *
 *  \param fontfamily Font family name.
 *  \param fontSize Font point size.
 *  \param fontColor Font color name.
 */

void DocTabs::setFont( const QString &fontFamily, int fontSize,
    const QColor &fontColor )
{
    setFontFamily( fontFamily );
    setFontSize( fontSize );
    setFontColor( fontColor );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the tab font color.
 *
 *  \return Reference to the current tab font color.
 */

const QColor &DocTabs::setFontColor( const QColor &fontColor )
{
    return( m_fontColor = fontColor );
}

//------------------------------------------------------------------------------
/*! \brief Sets the tab font family name.
 *
 *  \return The new font family name.
 */

const QString &DocTabs::setFontFamily( const QString &fontFamily )
{
    return( m_fontFamily = fontFamily );
}

//------------------------------------------------------------------------------
/*! \brief Sets the tab text font size.
 *
 *  \return The new tab text font size.
 */

int DocTabs::setFontSize( int fontSize )
{
    return( m_fontSize = fontSize );
}

//------------------------------------------------------------------------------
/*! \brief Sets the tab position to use on this document.
 *
 *  \return The new tab position for this document.
 */

int DocTabs::setTab( int tab )
{
    return( m_tab = tab );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current tab values.
 *
 *  \param tab This tab number (1 - \a tabs).
 *  \param tabs Number of tab positions along the edge of the page.
 *  \param text Tab text.
 */

void DocTabs::setTab( int tab, int tabs, const QString &tabText )
{
    m_tab  = tab;
    setTabs( tabs );
    if ( m_tab > m_tabs )
    {
        m_tab = m_tabs;
    }
    if ( m_tab < 0 )
    {
        m_tab = 0;
    }
    setTabText( tabText );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the current number of tabs per page.
 *
 *  \return The new number of tabs per page.
 */

int DocTabs::setTabs( int tabs )
{
    return( m_tabs = tabs );
}

//------------------------------------------------------------------------------
/*! \brief Sets the current tab text value.
 *
 *  \return Reference to the new tab text.
 */

const QString &DocTabs::setTabText( const QString &text )
{
    return( m_tabText = text );
}

//------------------------------------------------------------------------------
//  End of doctabs.cpp
//------------------------------------------------------------------------------

