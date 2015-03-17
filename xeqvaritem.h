//------------------------------------------------------------------------------
/*! \file xeqvaritem.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree EqVarItem and EqVarItemList
 *  class definitions.
 */

#ifndef _XEQVARITEM_H_
/*! \def _XEQVARITEM_H_
 *  \brief Prevent redundant includes.
 */
#define _XEQVARITEM_H_ 1

// Qt class references
#include <qptrlist.h>
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class EqVarItem xeqvaritem.h
 *
 *  \brief Defines a single item in an EqVarItemList.
 *
 *  The actual description text is located in a language dictionary under
 *  the key "<itemList.name>:<item.name><language>".
 */

class EqVarItem
{
// Public methods
public:
    EqVarItem( const QString &name, const QString &sort, int index,
        bool isPerm=true ) ;

// Public data
public:
    QString  m_sort;    //!< Sort key within the EqItemList
    QString  m_name;    //!< Name entered onto worksheet or as it appears in tables or on lists and in language dictionary
    QString *m_desc;    //!< Assigned by EqTree::setLanguage()
    int      m_index;   //!< Index into any associated data arrays
    bool     m_perm;    //!< True if this is a permanent (non-deletable) item
};

//------------------------------------------------------------------------------
/*! \class EqVarItemList xeqvaritem.h
 *
 *  \brief List of valid codes for one or more discrete EqVars.
 */

class EqVarItemList : public QPtrList<EqVarItem>
{
// Public methods
public:
    EqVarItemList( const QString &name ) ;

    EqVarItem      *addItem( const QString &name, const QString &sort,
                        int index, bool isPermanent, bool isDefault );
    QString        *itemDesc( int id ) ;
    int             itemIdWithName( const QString &name, bool caseSens=false ) ;
    int             itemIndex( int id ) ;
    bool            itemIsPerm( int id ) ;
    const QString  &itemName( int id ) ;
    const QString  &itemSort( int id ) ;
    EqVarItem      *itemWithIndex( int index ) ;
    EqVarItem      *itemWithName( const QString &name, bool caseSens=false ) ;
    EqVarItem      *itemWithSort( const QString &sort, bool caseSens=false ) ;
    bool            removeItemByName( const QString &name, bool caseSens=false ) ;
private:
    int             compareItems( Item s1, Item s2 ) ;

// Public data
public:
    QString m_name;             //!< Name used for language dictionary keys.
    QString m_nameDefault;      //!< Name of the default item
};

#endif

//------------------------------------------------------------------------------
//  End of xeqvaritem.h
//------------------------------------------------------------------------------

