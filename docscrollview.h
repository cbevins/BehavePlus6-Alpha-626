//------------------------------------------------------------------------------
/*  \file docscrollview.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief DocScrollView scrolling document class declarations.
 */

#ifndef _DOCSCROLLVIEW_H_
/*! \def _DOCSCROLLVIEW_H_
 *  \brief Prevent redundant includes.
 */
#define _DOCSCROLLVIEW_H_ 1

// Class declarations
class Composer;
class DocDeviceSize;
class Document;

// Qt class references
#include <qpixmap.h>
#include <qscrollview.h>

//------------------------------------------------------------------------------
/*! \class DocScrollView docscrollview.h
 *
 *  \brief DocScrollView displays a single Document page on screen.
 */

class DocScrollView : public QScrollView
{
// Public API
public:
    DocScrollView( QWidget *qMainWindow, DocDeviceSize *docDeviceSize,
        const char *name=0 ) ;
    void rescale( void ) ;
    bool showPage( Composer *composer, const QString &composerFile ) ;

// Protected API
protected:
    Document *document( void ) ;
    void drawContents( QPainter* painter, int cx, int cy, int cw, int ch ) ;
    void focusInEvent( QFocusEvent *e ) ;
    void focusOutEvent( QFocusEvent *e ) ;
    void keyPressEvent( QKeyEvent *e ) ;
    void resizeEvent( QResizeEvent *e ) ;
    void viewportResizeEvent( QResizeEvent *e ) ;
    virtual void viewportMouseReleaseEvent( QMouseEvent *e ) ;

// Public data members
public:
    DocDeviceSize  *m_screenSize;       //!< Device page dimensions (pixels)
    QPixmap         m_backingPixmap;    //!< Screen buffer store
};

#endif

//------------------------------------------------------------------------------
//  End of docscrollview.h
//------------------------------------------------------------------------------

