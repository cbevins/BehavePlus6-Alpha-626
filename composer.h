//------------------------------------------------------------------------------
/*! \file composer.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Composer class declaration.
 */

#ifndef _COMPOSER_H_
/*! \def _COMPOSER_H_
 *  \brief Prevent redundant includes.
 */
#define _COMPOSER_H_ 1

// Forward class references
class Graph;

// Qt class references
#include <qdatastream.h>
#include <qfile.h>
class QBrush;
class QColorGroup;
class QFont;
class QPaintDevice;
class QPainter;
class QPen;
class QPixmap;
class QString;

//------------------------------------------------------------------------------
/*! \class Composer composer.h
 *
 *  \brief Virtual device (screen or printer) drawing composition object.
 *
 *  The Composer knows how to write QPainter type primitives and their
 *  parameters to a file.  Usually a separate file is generated per display
 *  page.  The Composer is then asked to display each page as needed,
 *  which it does by opening the page ("composer") file and executing its
 *  drawing commands on the specified QPaintDevice, which can be the screen,
 *  the printer, or even a pixmap.
 *
 *  A Composer must be activated with the begin() method, which takes an
 *  output file name as an argument.  The makeFileName() method generates
 *  a composer file name based upon the process id and page number.
 *
 *  Once activated, the following QPainter-style drawing methods are available:
 *  \arg brush()
 *  \arg ellipse()
 *  \arg fill()
 *  \arg font()
 *  \arg graph()
 *  \arg line()
 *  \arg pen()
 *  \arg pie()
 *  \arg rect()
 *  \arg restore()
 *  \arg rotate()
 *  \arg rotateText()
 *  \arg save()
 *  \arg text()
 *
 *  The drawing methods mimic their QPainter counterparts, except that
 *  arguments are in inches rather than pixels.  This way the composition
 *  remains the same between devices of different resolutions.
 *
 *  When the composition is completed, the Composer is de-actived with the
 *  end() method.
 *
 *  When the program needs a page displayed, it simply calls the paint()
 *  method with the name of the composer file to be executed and a pointer
 *  to the QPaintDevice, which can be a QPicture, QPixmap, QPrinter, or
 *  QWidget (e.g., screen).
 */

class Composer
{
// Public methods
public:
    Composer( void ) ;
    ~Composer( void ) ;

    // Functions that control the Composer recording state.
    bool begin( const QString &fileName ) ;
    bool end( void ) ;
    void makeFileName( int docId, int pageNo, QString &composerFile ) ;

    // Functions that record Composer commands.
    void brush( const QBrush &brush ) ;
    void ellipse( double x, double y, double width, double height ) ;
    void fill( double x, double y, double width, double height,
            const QBrush &brush ) ;
    void font( const QFont &font ) ;
    void graph( const Graph &graph, double marginLeft, double marginTop,
            double bodyWd, double bodyHt ) ;
    void line( double x0, double y0, double x1, double y1 ) ;
    void pen( const QPen &pen );
    void pie( double x, double y, double width, double height,
            int angle, int angleLength ) ;
    void pixmap( const QPixmap &pixmap, double x, double y,
            double width, double height );
    void printerOn( bool on ) ;
    void rect( double x, double y, double width, double height, double r=0. ) ;
    void restore( void ) ;
    void rotate( double deg ) ;
    void rotateEllipse( double rx, double ry, double x, double y,
            double width, double height, double deg ) ;
    void rotateLine( double rx, double ry, double x1, double y1,
            double x2, double y2, double deg ) ;
    void rotateText( double x, double y, double width, double height,
            double deg, const QString &text ) ;
    void save( void ) ;
    void screenOn( bool on ) ;
    void text( double x, double y, double width, double height,
            int alignmentFlags, const QString &text ) ;
    void text( double x, double y, const QString &text ) ;
    void translate( double x, double y ) ;
    void wraptext( double x, double y, double w, double h,
            const QString &text ) ;

    // Function to execute the Composer commands on the screen or printer.
    bool paint( const QString &fileName, QPaintDevice *devicePtr,
            double xppi, double yppi, double fontScale,
            bool toPrinter=false ) ;

// Private methods
private:
    void paintGraph( QPainter *p, double fontScale ) ;
    int  xPix( double inches ) const ;
    int  yPix( double inches ) const ;

// Public data members
public:
    QFile       m_file;     //!< Current composition file full path name
    QDataStream m_stream;   //!< Composer file input-output data stream
    double      m_xppi;     //!< Current paint() x pixels per inch
    double      m_yppi;     //!< Current paint() y pixels per inch
};

#endif

//------------------------------------------------------------------------------
//  End of composer.h
//------------------------------------------------------------------------------

