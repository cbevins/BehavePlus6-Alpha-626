//------------------------------------------------------------------------------
/*! \file xeqvaritem.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Experimental Equation Tree EqVarItem and EqVarItemList class
 *  methods.
 */

// Custom include files
#include "appmessage.h"
#include "xeqvaritem.h"

static const QString emptyString( "" );

//------------------------------------------------------------------------------
/*! \brief EqVarItem constructor.
 *
 *  \param name  Name used as code entered onto worksheet and for language
 *                  dictionary key.
 *  \param sort  Sort key used to arrange EqVarItems in the EqVarItemList.
 *  \param index Index into any associated data arrays.
 *  \param isPerm Marks the EqVarItem as permanenet (cannot be deleted)
 */

EqVarItem::EqVarItem( const QString &name, const QString &sort, int index,
        bool isPerm ) :
    m_sort(sort),
    m_name(name),
    m_desc(0),
    m_index(index),
    m_perm(isPerm)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief EqVarItemList constructor.
 *
 *  \param name Name used as key for language dictionary lookups.
 */

EqVarItemList::EqVarItemList( const QString &name ) :
    m_name(name),
    m_nameDefault("")
{
    setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates and initializes a new EqVarItem and adds it to the list.
 *
 *  \param name  Name used as code entered onto worksheet and for language
 *                  dictionary key.
 *  \param sort  Sort key used to arrange EqVarItems in the EqVarItemList.
 *  \param index Index into any associated data arrays.
 *  \param isPerm Marks the EqVarItem as permanenet (cannot be deleted)
 *  \param isDefault Sets the EqVarItem as the default item.
 *
 *  \return Pointer to the newly allocated EqVarItem.
 */

EqVarItem *EqVarItemList::addItem( const QString &name, const QString &sort,
        int index, bool isPerm, bool isDefault )
{
    // Allocate memory the new EqVarItem
    EqVarItem *itemPtr = new EqVarItem( name, sort, index, isPerm );
    checkmem( __FILE__, __LINE__, itemPtr,
        QString( "EqVarItemList %1 item %2" ).arg( m_name ).arg( name ), 1 );
    // If this is the default, set it.
    if ( isDefault )
    {
        m_nameDefault = name;
    }
    // Insert it into the list
    inSort( itemPtr );
    return( itemPtr );
}

//------------------------------------------------------------------------------
/*! \brief Reimplementation of GCollection::compareItems() for EqVarItemList
 *  required to keep the list properly sorted.
 *
 *  \return Same values as strcmp().
 */

int EqVarItemList::compareItems( Item s1, Item s2 )
{
    EqVarItem *i1 = (EqVarItem *) s1;
    EqVarItem *i2 = (EqVarItem *) s2;
    return( strcmp( i1->m_sort.latin1(), i2->m_sort.latin1() ) );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem's description string
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's description string
 *          translated to the last setLanguage().
 *  \retval On failure returns an empty QString.
 */

QString *EqVarItemList::itemDesc( int id )
{
    static QString empty("");
    EqVarItem *itemPtr = (EqVarItem *) QPtrList<EqVarItem>::at( id );
    if ( itemPtr )
    {
        return( itemPtr->m_desc );
    }
    return( &empty );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first EqVarItem in the EqVarItemList
 *  with the specified name and returns its position in the list.
 *
 *  \param name EqVarItem name to locate.
 *  \param caseSen If TRUE, search is case sensisitver.
 *
 *  \retval On success returns the position index of the first EqVarItem
 *          with the name.
 *  \retval On failure returns -1.
 */

int EqVarItemList::itemIdWithName( const QString &name, bool caseSens )
{
    QString itemName, findName;
    findName = ( caseSens )
               ? name
               : name.upper();
    EqVarItem *itemPtr;
    int id = 0;
    for ( itemPtr = QPtrList<EqVarItem>::first();
          itemPtr != 0;
          itemPtr = QPtrList<EqVarItem>::next() )
    {
        itemName = ( caseSens )
                   ? itemPtr->m_name
                   : itemPtr->m_name.upper();
        if ( itemName == findName )
        {
            return( id );
        }
        id++;
    }
    return( -1 );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem index value
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's index.
 *  \retval On failure returns -1.
 */

int EqVarItemList::itemIndex( int id )
{
    EqVarItem *itemPtr = (EqVarItem *) QPtrList<EqVarItem>::at( id );
    return( ( itemPtr )
            ? itemPtr->m_index
            : -1 );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem's permanence status
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's permanence status.
 *  \retval On failure returns TRUE.
 */

bool EqVarItemList::itemIsPerm( int id )
{
    EqVarItem *itemPtr = (EqVarItem *) QPtrList<EqVarItem>::at( id );
    return( ( itemPtr )
            ? itemPtr->m_perm
            : true );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem's name string
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's name string.
 *  \retval On failure returns an empty QString.
 */

const QString &EqVarItemList::itemName( int id )
{
    EqVarItem *itemPtr = (EqVarItem *) QPtrList<EqVarItem>::at( id );
    return( ( itemPtr )
            ? itemPtr->m_name
            : emptyString );
}

//------------------------------------------------------------------------------
/*! \brief Access to an individual EqVarItem's sort string
 *  using an index into the EqVarItemList.
 *
 *  \param id Index into the EqVarItemList (base 0).
 *
 *  \retval On success returns the requested EqVarItem's sort string.
 *  \retval On failure returns an empty QString.
 */

const QString &EqVarItemList::itemSort( int id )
{
    EqVarItem *itemPtr = (EqVarItem *) QPtrList<EqVarItem>::at( id );
    return( ( itemPtr )
            ? itemPtr->m_sort
            : emptyString );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first EqVarItem in the EqVarItemList with the specified
 *  data index.
 *
 *  \retval index Data index to search for.
 *
 *  \retval On success returns a pointer to the first EqVarItem with the index.
 *  \retval On failure returns 0.
 */

EqVarItem *EqVarItemList::itemWithIndex( int index )
{
    EqVarItem *itemPtr;
    for ( itemPtr = QPtrList<EqVarItem>::first();
          itemPtr != 0;
          itemPtr = QPtrList<EqVarItem>::next() )
    {
        if ( itemPtr->m_index == index )
        {
            return( itemPtr );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first EqVarItem in the EqVarItemList
 *  with the specified name and returns its data index.
 *
 *  \param name EqVarItem name to locate.
 *  \param caseSen If TRUE, search is case sensistive.
 *
 *  \retval On success returns a pointer to the first EqVarItem with the name.
 *  \retval On failure returns 0.
 */

EqVarItem *EqVarItemList::itemWithName( const QString &name, bool caseSens )
{
    QString itemName, findName;
    findName = ( caseSens )
               ? name
               : name.upper();
    EqVarItem *itemPtr;
    for ( itemPtr = QPtrList<EqVarItem>::first();
          itemPtr != 0;
          itemPtr = QPtrList<EqVarItem>::next() )
    {
        itemName = ( caseSens )
                   ? itemPtr->m_name
                   : itemPtr->m_name.upper();
        if ( itemName == findName )
        {
            return( itemPtr );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first EqVarItem in the EqVarItemList
 *  with the specified sort vallue and returns its data index.
 *
 *  \param sort EqVarItem sort value to locate.
 *  \param caseSen If TRUE, search is case sensistive.
 *
 *  \retval On success returns a pointer to the first EqVarItem with the name.
 *  \retval On failure returns 0.
 */

EqVarItem *EqVarItemList::itemWithSort( const QString &sort, bool caseSens )
{
    QString itemSort, findSort;
    findSort = ( caseSens )
               ? sort
               : sort.upper();
    EqVarItem *itemPtr;
    for ( itemPtr = QPtrList<EqVarItem>::first();
          itemPtr != 0;
          itemPtr = QPtrList<EqVarItem>::next() )
    {
        itemSort = ( caseSens )
                   ? itemPtr->m_sort
                   : itemPtr->m_sort.upper();
		if ( itemSort.stripWhiteSpace() == findSort )
        {
            return( itemPtr );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the first EqVarItem in the EqVarItemList with the specified
 *  name and removes it from the list.
 *
 *  \param name EqVarItem name to locate.
 *  \param caseSen If TRUE, search is case sensisitver.
 *
 *  \return Returns TRUE if the name was found and the item removed.
 *  otherwise returns FALSE.
 */

bool EqVarItemList::removeItemByName( const QString &name, bool caseSens )
{
    QString itemName, findName;
    findName = ( caseSens )
               ? name
               : name.upper();
    EqVarItem *itemPtr;
    for ( itemPtr = first();
          itemPtr != 0;
          itemPtr = next() )
    {
        itemName = ( caseSens )
                   ? itemPtr->m_name
                   : itemPtr->m_name.upper();
        if ( itemName == findName )
        {
            remove( itemPtr );
            return( true );
        }
    }
    return( false );
}

//------------------------------------------------------------------------------
//  End of xeqvaritem.cpp
//------------------------------------------------------------------------------

