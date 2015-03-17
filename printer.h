//------------------------------------------------------------------------------
/*! \file printer.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Printer class declarations.
 */

#ifndef _PRINTER_H_
/*! \def _PRINTER_H_
    \brief Prevent redundant includes.
 */
#define _PRINTER_H_ 1

// Qt class references
#include <qprinter.h>

//------------------------------------------------------------------------------
/*! \class Printer printer.h
 *
 *  \brief A QPrinter derived class that allows better control over margins.
 *
 *  The default Printer is a letter-sized portrait in screen resolution.
 */

class Printer : public QPrinter
{
// Public methods
public:
    Printer( void ) ;
    Printer( QPrinter::PageSize size, int copies=1,
             QPrinter::Orientation orient=QPrinter::Portrait,
             QPrinter::PrinterMode mode=QPrinter::ScreenResolution ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of printer.h
//------------------------------------------------------------------------------




