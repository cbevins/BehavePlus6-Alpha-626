//------------------------------------------------------------------------------
/*! \file toc.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Table of contents (TocList) and item (TocItem) class declarations.
 */

#ifndef _TOC_H_
/*! \def _TOC_H_
    \brief Prevent redundant includes.
 */
#define _TOC_H_ 1

// Qt class references
#include <qptrlist.h>
#include <qstring.h>
class QPixmap;
class QPopupMenu;

//------------------------------------------------------------------------------
/*! \enum TocType
 *  \brief ToC Item Types and their pixmaps.
 */

enum TocType
{
    TocNone=0,          // Do not display any ToC entry for this page
    TocBlank,           // Display in ToC with no pixmap
    TocBarGraph,        // Display in ToC with "bar graph" pixmap
    TocDocument,        // Display in ToC with "document" pixmap
    TocInput,           // Display in Toc with "input entry" pixmap
    TocLineGraph,       // Display in ToC with "line graph" pixmap
    TocListOut,         // Display in ToC with "list" pixmap
    TocTable,           // Display in ToC with "table" pixmap
    TocCalendar,        // Display in ToC with "calendar" pixmap
    TocDirection,       // Display in ToC with "fire/wind/slope direction" pixmap
    TocShape,           // Display in ToC with "fire shape" pixmap
    TocHaulChart        // Display in ToC with "haulchart" pixmap
} ;

//------------------------------------------------------------------------------
/*! \class TocItem toc.h
 *
 *  \brief The TocItem class contains information for a single table-of-contents
 *  entry.
 */

class TocItem
{
// Public methods
public:
    TocItem( void ) ;
    TocItem( int page, const QString &title, TocType type ) ;

// Public data members
public:
    int     m_page;     //!< Page number
    QString m_text;     //!< ToC text
    TocType m_type;     //!< Toc pixmap (TocNone, TocDocument, TocTable, etc.)
};

//------------------------------------------------------------------------------
/*! \class TocList toc.h
 *
 *  \brief The TocList class maintains a list of TocItems.
 */

class TocList : public QPtrList<TocItem>
{
// Public methods
public:
    TocList( void );
    void addItem( int page, const QString &pageTitle, TocType tocType ) ;
    int  itemPage( int menuId ) ;
    QPixmap &pixmap( TocType tocType, QPixmap &pm ) ;
    void rebuildMenu( QPopupMenu *contentsMenu, int currentPage ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of toc.h
//------------------------------------------------------------------------------

