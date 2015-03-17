//------------------------------------------------------------------------------
/*! \file printer.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Printer class declarations.
 */

// Custom include files
#include "printer.h"

//------------------------------------------------------------------------------
/*! \brief QPrinter default constructor.
 *
 */

Printer::Printer( void ) :
    QPrinter()
{
    setNumCopies( 1 );
    setFullPage( true );
    setPageSize( QPrinter::Letter );
    setOrientation( QPrinter::Portrait );
    setColorMode( QPrinter::Color );
    setOptionEnabled( QPrinter::PrintToFile, false );
    setOptionEnabled( QPrinter::PrintSelection, false );
    setOptionEnabled( QPrinter::PrintPageRange, true );
    setPrintRange( QPrinter::PageRange );
    return;
}

//------------------------------------------------------------------------------
/*! \brief QPrinter custom constructor.
 */

Printer::Printer( PageSize size, int copies, Orientation orient,
        PrinterMode mode ) :
    QPrinter( mode )
{
    setNumCopies( copies );
    setFullPage( true );
    setPageSize( size );
    setOrientation( orient );
    setColorMode( QPrinter::Color );
    setOptionEnabled( QPrinter::PrintToFile, false );
    setOptionEnabled( QPrinter::PrintSelection, false );
    setOptionEnabled( QPrinter::PrintPageRange, true );
    setPrintRange( QPrinter::PageRange );
    return;
}

//------------------------------------------------------------------------------
//  End of printer.cpp
//------------------------------------------------------------------------------

