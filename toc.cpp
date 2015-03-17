//------------------------------------------------------------------------------
/*! \file toc.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Table of contents (TocList) and item (TocItem) class methods.
 */

// Custom include files
#include "toc.h"

// Qt include files
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>

// ToC XPM pixmaps
#include "toc.xpm"

/*! \typedef TocPixmapData
 *  \brief Defines the ToC pixmap cache names and their corresponding xmp data.
 */
typedef struct _tocPixmapData
{
    const char *name;
    const char **xpm;
} TocPixmapData;

/*! \var Pixmap[]
 *  \brief Defines the ToC pixmap cache names and their corresponding xmp data.
 */
static TocPixmapData Pixmap[12] =
{
    { "TocNone",        blank_xpm },
    { "TocBlank",       blank_xpm },
    { "TocBarGraph",    bargraph_xpm },
    { "TocDocument",    document_xpm },
    { "TocInput",       input_xpm },
    { "TocLineGraph",   linegraph_xpm },
    { "TocListOut",     list_xpm },
    { "TocTable",       table_xpm },
    { "TocCalendar",    calendar_xpm },
    { "TocDirection",   direction_xpm },
    { "TocShape",       shape_xpm },
    { "TocHaulChart",   haulchart_xpm }
};

//------------------------------------------------------------------------------
/*! \brief TocItem default constructor.
 */

TocItem::TocItem( void ) :
    m_page(0),
    m_text(""),
    m_type(TocNone)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief TocItem custom constructor.
 */

TocItem::TocItem( int page, const QString &text, TocType type ) :
    m_page(page),
    m_text(text),
    m_type(type)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief TocList default constructor.
 */

TocList::TocList( void )
{
    setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds a TocItem to the TocList.
 */

void TocList::addItem( int page, const QString &pageTitle, TocType tocType )
{
    TocItem *item = new TocItem( page, pageTitle, tocType );
    Q_CHECK_PTR( item );
    append( item );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the page number of the corresponding menu id (base 0).
 */

int TocList::itemPage( int menuId )
{
    int atId = 0;
    TocItem *item;
    for ( item = QPtrList<TocItem>::first();
          item != 0;
          item = QPtrList<TocItem>::next() )
    {
        if ( item->m_type == TocNone )
        {
            continue;
        }
        if ( atId == menuId )
        {
            return( item->m_page );
        }
        atId++;
    }
    return( 1 );
}

//------------------------------------------------------------------------------
/*! \brief Gets a reference to the specified TocType pixmap.
 */

QPixmap &TocList::pixmap( TocType tocType, QPixmap &pm )
{
    if ( ! QPixmapCache::find( Pixmap[tocType].name, pm ) )
    {
        pm = QPixmap( Pixmap[tocType].xpm );
        QPixmapCache::insert( Pixmap[tocType].name, pm );
    }
    return( pm );
}

//------------------------------------------------------------------------------
/*! \brief Rebuilds a either popup menu or a list view containing the current
 *  table of contents (ToC).
 */

void TocList::rebuildMenu( QPopupMenu *contentsMenu, int currentPage )
{
    contentsMenu->clear();
    QString text("");
    int tid = 0;        // ToC list index
    int mid = 0;        // Menu item id
    TocItem *item;
    QPixmap pm;
    for ( item = QPtrList<TocItem>::first();
          item != 0;
          item = QPtrList<TocItem>::next() )
    {
        text = QString( "%1 %2" ).arg( item->m_page, 2 ).arg( item->m_text );
        if ( item->m_type != TocNone )
        {
            pixmap( item->m_type, pm );
            mid = contentsMenu->insertItem( pm, text, tid++ );
            contentsMenu->setItemChecked( mid, currentPage == tid );
        }
    }
    return;
}

//------------------------------------------------------------------------------
//  End of toc.cpp
//------------------------------------------------------------------------------

