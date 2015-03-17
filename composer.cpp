//------------------------------------------------------------------------------
/*! \file composer.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Composer class methods.
 */

// Custom include files
#include "composer.h"
#include "graph.h"
#include "platform.h"

// Qt include files
#include <qbrush.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qprinter.h>

//------------------------------------------------------------------------------
/*! \brief Composer default constructor.
*/

Composer::Composer() :
    m_file(""),
    m_stream(),
    m_xppi(72.0),
    m_yppi(72.0)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Composer destructor.
*/

Composer::~Composer( void )
{
    if ( m_file.isOpen() )
    {
        m_file.close();
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens a composer file for writing.
 *
 *  \param fileName Full path name to the output composer file.
 *
 *  \retval TRUE if the file was successfully opened for writing.
 *  \retval FALSE if the file could not be opened for writing.
 */

bool Composer::begin( const QString &fileName )
{
    // Make sure the composer was previously closed.
    if ( ! m_file.name().isEmpty() && m_file.isOpen() )
    {
        end();
    }
    m_file.setName( fileName );

    // Open the composition file in overwrite mode.
    if ( ! m_file.open( IO_WriteOnly ) )
    {
        return( false );
    }

    // Set the text stream device to this file and clear total pages.
    m_stream.setDevice( &m_file );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Closes a composer file when finished writing.
 *
 *  \retval TRUE if the file was open and then closed.
 *  \retval FALSE if the file was already closed.
 */

bool Composer::end( void )
{
    if ( m_file.isOpen() )
    {
        m_file.close();
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Generates a composer file name which uniquely identifies the file
 *  by pid, document number, and page number.
 *
 *  \param docId A positive integer identifying the document being composed.
 *  \param pageNo A positive integer identifying the the page being composed.
 *  \param composerFile Reference to the generated composer file name.
 *
 *  \return The generated file name is returned in the passed \a composerFile
 *  reference. The function itself returns nothing.
 */

void Composer::makeFileName( int docId, int pageNo, QString &composerFile )
{
    // Write the file name in the composerFile.
    composerFile.sprintf( "c%05d%03d%03d.tmp",
        platformGetPid(), docId, pageNo );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Composer's QBrush by writing its attributes to the file.
 *
 * \param brush Reference to a QBrush whose properties are to be applied.
 */

void Composer::brush( const QBrush &brush )
{
    m_stream << QString("brush") << brush;
}

//------------------------------------------------------------------------------
/*! \brief Draws an ellipse with center at (x+w/2), (y+h/2).
 *
 *  \param x, y Upper left corner of the ellipse's bounding rectangle (inches).
 *  \param w, h Ellipse width and height (inches).
 */

void Composer::ellipse( double x, double y, double w, double h )
{
    m_stream << QString("ellipse") << x << y << w << h;
}

//------------------------------------------------------------------------------
/*! \brief Fills the rectangle with the specified QBrush.
 *
 *  You can specify a QColor as \a brush since there is a QBrush constructor
 *  that takes a QColor argument and creates a solid pattern brush.
 *
 *  \param x, y Upper left corner of the rectangle (inches).
 *  \param w, h Rectangle width and height (inches).
 *  \param brush Reference to the QBrush used to fill the rectangle.
 *
 *  \sa rect()
 */

void Composer::fill( double x, double y, double w, double h,
    const QBrush &brush  )
{
    m_stream << QString("fill") << x <<  y << w << h << brush;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Composer's QFont by writing its attributes to the file.
 *
 * \param font Reference to a QFont whose properties are to be applied.
 */

void Composer::font( const QFont &font )
{
    m_stream << QString("font") << font;
}

//------------------------------------------------------------------------------
/*! \brief Draws a serialized graph into the composer file.
 *
 *  \param graph Reference to the Graph instance to be drawn.
 *  \param x, y Graph's upper left corner on the page (inches).
 *  \param w, h Graph body width and height (inches).
 */

void Composer::graph( const Graph &graph, double x, double y, double w,
    double h )
{
    m_stream << QString("graph") << x << y << w << h << graph;
}

//------------------------------------------------------------------------------
/*! \brief Draws a line from (x1, y1) to (x2, y2) and sets (x2, y2) to be the
 *  new current pen location.  The current pen attributes are used.
 *
 *  \param x1, y1 Line starting point coordinates (inches).
 *  \param x2, y2 Line ending point coordinates (inches).
 */

void Composer::line( double x0, double y0, double x1, double y1 )
{
    m_stream << QString("line") << x0 << y0 << x1 << y1;
}

//------------------------------------------------------------------------------
/*! \brief Sets the Composer's QPen by writing its attributes to the file.
 *
 * \param pen Reference to a QPen whose properties are to be applied.
 */

void Composer::pen ( const QPen &pen )
{
    m_stream << QString("pen") << pen;
}

//------------------------------------------------------------------------------
/*! \brief Draws a pie with center at (x+w/2), (y+h/2) and filled with the
 *  current brush.
 *
 *  \a angle and \a angleLength are in 1/16 of a degree starting at the
 *  3 o'clock position.  So there are 16*360 = 5760 possible values.
 *  Positive values are counter-clockwise, negative values are clockwise.
 *  The pie is filled with the current brush pattern/color.
 *
 *  \param x, y Upper left corner of the ellipse's bounding rectangle (inches).
 *  \param w, h Ellipse width and height (inches).
 *  \param a    Start angle in 1/16 degrees.
 *  \param l    Arc length in 1/16 degrees.
 */

void Composer::pie( double x, double y, double w, double h, int a, int l )
{
    m_stream << QString("pie") << x << y << w << h << a << l;
}

//------------------------------------------------------------------------------
/*! \brief Draws a pixmap sclaed to the output device.
 *
 *  \param pixmap   Reference to the QPixmap to be drawn.
 *  \param x, y     Pixmap starting point page coordinates (inches).
 *  \param w, h     Pixmap output dimensions (inches).
 */

void Composer::pixmap( const QPixmap &pixmap, double x, double y, double w,
    double h )
{
    m_stream << QString("pixmap") << pixmap << x << y << w << h;
}

//------------------------------------------------------------------------------
/*! \brief Sets the printer on/off toggle.
 *
 * \param on If TRUE, subsequent output is sent to printer output device.
 *           If FALSE, subsequent output is NOT sent to printer output device.
 */

void Composer::printerOn ( bool on )
{
    m_stream << QString("printerOn") << ( on ? 1 : 0 );
}

//------------------------------------------------------------------------------
/*! \brief Draws a rectangle (unfilled) with the current pen.
 *
 *  \param x, y Upper left corner of the rectangle (inches).
 *  \param w, h Rectangle width and height (inches).
 *  \param r    Corner radius (inches).
 *              If less than 0.01, no corner arc is drawn.
 *
 *  \sa fill()
 */

void Composer::rect( double x, double y, double w, double h, double r )
{
    if ( r < 0.01 )
    {
        m_stream << QString("rect") << x << y << w << h;
    }
    else
    {
        m_stream << QString("roundrect") << x << y << w << h << r;
    }
}

//------------------------------------------------------------------------------
/*! \brief Restores the painter state from the FIFO stack.
 *  Usually called after calling rotate(), translate(), etc.
 */

void Composer::restore( void )
{
    m_stream << QString("restore");
}

//------------------------------------------------------------------------------
/*! \brief Rotates the coordinate system by the specified number of degrees.
 *
 *  \param d Degrees of rotation [0.-360.].
 */

void Composer::rotate( double d )
{
    m_stream << QString("rotate") << d;
}

//------------------------------------------------------------------------------
/*! \brief Draws rotated ellipse using the current pen and font.
 *
 *  \param x0, y0   Rotation point
 *  \param x, y     Upper left corner of the ellipse bounding rectangle.
 *  \param w, h     Width and height of the ellipse bounding rectangle (inches).
 *  \param d        Degrees ellipse is rotated [0.-360.].
 */

void Composer::rotateEllipse( double rx, double ry, double x, double y,
    double w, double h, double d )
{
    m_stream << QString("rotateEllipse") << rx << ry << x << y << w << h << d;
}

//------------------------------------------------------------------------------
/*! \brief Draws rotated line using the current pen and font.
 *
 *  \param rx, ry   Rotation point
 *  \param x1, y1   First point
 *  \param x2, y2   Second point
 *  \param d        Degrees of rotation [0.-360.].
 */

void Composer::rotateLine( double rx, double ry, double x1, double y1,
    double x2, double y2, double d )
{
    m_stream << QString("rotateLine") << rx << ry << x1 << y1 << x2 << y2 << d;
}

//------------------------------------------------------------------------------
/*! \brief Draws rotated text using the current pen and font, centered within a
 *  bounding rectangle, and rotated by a specified angle.
 *
 *  \param x, y Upper left corner of the text bounding rectangle.
 *              (this is also the rotation point)
 *  \param w, h Width and height of the text bounding rectangle (inches).
 *  \param d    Degrees text is rotated [0.-360.].
 *  \param text Reference to the text string to be drawn.
 */

void Composer::rotateText( double x, double y, double w, double h, double d,
    const QString &text )
{
    m_stream << QString("rotateText") << x << y << w << h << d << text;
}

//------------------------------------------------------------------------------
/*! \brief Saves the painter state on the FIFO stack.
 *  Usually called before calling rotate(), translate(), etc.
 */

void Composer::save( void )
{
    m_stream << QString("save");
}

//------------------------------------------------------------------------------
/*! \brief Sets the screen on/off toggle.
 *
 * \param on If TRUE, subsequent output is sent to screen output device.
 *           If FALSE, subsequent output is NOT sent to screen output device.
 */

void Composer::screenOn ( bool on )
{
    m_stream << QString("screenOn") << ( on ? 1 : 0 );
}

//------------------------------------------------------------------------------
/*! \brief Draws text using the current pen and font, aligned within a bounding
 *  rectangle.
 *
 *  \param x, y Upper left corner of the text bounding rectangle (in).
 *  \param w, h Width and height of the text bounding rectangle (in).
 *  \param f    One or more of the Qt::AlignmentFlags values.
 *  \param text Reference to the text string to be drawn.
 */

void Composer::text( double x, double y, double w, double h, int f,
    const QString &text )
{
    m_stream << QString("atxt") << x << y << w << h << f << text;
}

//------------------------------------------------------------------------------
/*! \brief Draws text using the current pen and font at the specified starting
 *  point.
 *
 *  \attention  Note that \a y specifies the \e baseline position!
 *
 *  \param x, y Starting baseline position of the text (in).
 *  \param text Reference to the text string to be drawn.
 */

void Composer::text( double x, double y, const QString &text )
{
    m_stream << QString("text") << x << y << text;
}

//------------------------------------------------------------------------------
/*! \brief Translates the coordinate origin to the new x, y.
 *
 *  \param x, y Shift origin by this amount (in).
 */

void Composer::translate( double x, double y )
{
    m_stream << QString("translate") << x << y;
}

//------------------------------------------------------------------------------
/*! \brief Draws wrapped text at the position and within a rectangle.
 *
 *  \param x        Starting x coordinate
 *  \param y        Starting y coordinate
 *  \param w        Clipping rectangle width.
 *  \param h        Clipping rectangle height.
 *  \param text     Text to be printed
 */

void Composer::wraptext( double x, double y, double w, double h,
        const QString &text )
{
    m_stream << QString( "wraptext" ) << x << y << w << h << text;
}

//------------------------------------------------------------------------------
/*! \brief Executes the composer commands from \a fileName onto the
 *  \a devicePtr QPaintDevice using the specified resolution and scale.
 *
 *  \param fileName Full path name of the composer file.
 *  \param devicePtr Pointer to the QPaintDevice to be drawn on.
 *  \param xppi, yppi Device pixels-per-inch resolution.
 *  \param fontScale Font scaling factor.
 *  \param toPrinter TRUE if output is to printer
 *
 *  \retval TRUE if able to open the composer file and execute the commands.
 *  \retval FALSE if unable to open the composer file.
 */

bool Composer::paint( const QString  &fileName, QPaintDevice *devicePtr,
    double xppi, double yppi, double fontScale, bool toPrinter )
{
    // Make sure the composition is finished,
    if ( m_file.isOpen() )
    {
        end();
    }
    // Open the composition file in read mode.
    m_file.setName( fileName );
    if ( ! m_file.open( IO_ReadOnly ) )
    {
        return( false );
    }
    m_stream.setDevice( & m_file );

    // Create a local painter.
    QPainter painter;
    painter.begin( devicePtr );

    // Get the composition device resolution.
    m_xppi = xppi;
    m_yppi = yppi;

    // Assume both screen and printer is on
    int printerOn = 1;
    int screenOn = 1;
    bool toScreen = ! toPrinter;

    // Local input variables.
    QString cmd;
    double  x, y, w, h, x1, y1, r, rx, ry, deg;
    int     align, angle, angleLength;
    QBrush  brush;
    QFont   font;
    QPen    pen;
    QString text;

    // Read each command line from the composition file.
    while ( ! m_stream.eof() )
    {
        // Read the next command.
        m_stream >> cmd;

        // Execute the command.
        if ( cmd == "atxt" )
        {
            m_stream >> x >> y >> w >> h >> align >> text;
            painter.drawText( xPix(x), yPix(y), xPix(w), yPix(h), align, text );
        }
        else if ( cmd == "brush" )
        {
            m_stream >> brush;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.setBrush( brush );
            }
        }
        else if ( cmd == "ellipse" )
        {
            m_stream >> x >> y >> w >> h;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawEllipse( xPix(x), yPix(y), xPix(w), yPix(h) );
            }
        }
        else if ( cmd == "fill" )
        {
            m_stream >> x >> y >> w >> h >> brush;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.fillRect( xPix(x), yPix(y), xPix(w), yPix(h), brush );
            }
        }
        else if ( cmd == "font" )
        {
            m_stream >> font;
            // The passed xppi and yppi already account for drawing scale,
            // but all screen fonts must still be rescaled!
            if ( devicePtr->devType() != QInternal::Printer )
            {
                font.setPointSize( (int)
                    ( 0.1 + fontScale * (double) font.pointSize() ) );
            }
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.setFont( font );
            }
        }
        else if ( cmd == "graph" )
        {
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                paintGraph( &painter, fontScale );
            }
        }
        else if ( cmd == "line" )
        {
            m_stream >> x >> y >> x1 >> y1;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawLine( xPix(x), yPix(y), xPix(x1), yPix(y1) );
            }
        }
        else if ( cmd == "pen" )
        {
            m_stream >> pen;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.setPen( pen );
            }
        }
        else if ( cmd == "pie" )
        {
            m_stream >> x >> y >> w >> h >> angle >> angleLength;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawPie( xPix(x), yPix(y), xPix(w), yPix(h),
                angle, angleLength );
            }
        }
        else if ( cmd == "pixmap" )
        {
            QPixmap pixmap;
            m_stream >> pixmap >> x >> y >> w >> h;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                double xscale = w * xppi / pixmap.width();
                double yscale = h * yppi / pixmap.height();
                QWMatrix matrix = painter.worldMatrix();
                painter.scale( xscale, yscale );
                painter.drawPixmap( (int) (xPix(x)/xscale), (int) (yPix(y)/yscale),
                    pixmap );
                painter.setWorldMatrix( matrix );
            }
        }
        else if ( cmd == "printerOn" )
        {
            m_stream >> printerOn;
        }
        else if ( cmd == "rect" )
        {
            m_stream >> x >> y >> w >> h;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawRect( xPix(x), yPix(y), xPix(w), yPix(h) );
            }
        }
        else if ( cmd == "roundrect" )
        {
            m_stream >> x >> y >> w >> h >> r;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                int x1 = xPix(x);
                int y1 = yPix(y);
                int x2 = xPix(x+w);
                int y2 = yPix(y+h);
                int rx = xPix(r);
                int ry = yPix(r);
                int len = 16 * 90;
                painter.drawLine( x1+rx, y1, x2-rx, y1 );   // top
                painter.drawLine( x1+rx, y2, x2-rx, y2 );   // bottom
                painter.drawLine( x1, y1+ry, x1, y2-ry );   // left
                painter.drawLine( x2, y1+ry, x2, y2-ry );   // right
                painter.drawArc( x2-2*rx, y1, 2*rx, 2*ry, 0, len );     // ur
                painter.drawArc( x1, y1, 2*rx, 2*ry, 16*90, len );      // ul
                painter.drawArc( x1, y2-2*ry, 2*rx, 2*ry, 16*180, len ); // ll
                painter.drawArc( x2-2*rx, y2-2*ry, 2*rx, 2*ry, 16*270, len );
            }
        }
        else if ( cmd == "restore" )
        {
            painter.restore();
        }
        else if ( cmd == "rotate" )
        {
            m_stream >> deg;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.rotate( deg );
            }
        }
        else if ( cmd == "rotateEllipse" )
        {
            m_stream >> rx >> ry >> x >> y >> w >> h >> deg;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.save();
                painter.translate( xPix(rx), yPix(ry) );
                painter.rotate( deg );
                painter.drawEllipse( xPix((x-rx)), yPix((y-ry)),
                    xPix(w), xPix(h) );
                painter.restore();
            }
        }
        else if ( cmd == "rotateLine" )
        {
            m_stream >> rx >> ry >> x >> y >> x1 >> y1 >> deg;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.save();
                painter.translate( xPix(rx), yPix(ry) );
                painter.rotate( deg );
                painter.drawLine( xPix((x-rx)), yPix((y-ry)),
                    xPix(x1-rx), xPix(y1-ry) );
                painter.restore();
            }
        }
        else if ( cmd == "rotateText" )
        {
            m_stream >> x >> y >> w >> h >> deg >> text;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.save();
                painter.translate( xPix(x), yPix(y) );
                painter.rotate( deg );
                painter.drawText( 0, 0, xPix(w), xPix(h),
                    Qt::AlignHCenter|Qt::AlignVCenter, text );
                painter.restore();
            }
        }
        else if ( cmd == "save" )
        {
            painter.save();
        }
        else if ( cmd == "screenOn" )
        {
            m_stream >> screenOn;
        }
        else if ( cmd == "text" )
        {
            m_stream >> x >> y >> text;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawText( xPix(x), yPix(y), text );
            }
        }
        else if ( cmd == "translate" )
        {
            m_stream >> x >> y;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.translate( xPix(x), yPix(y) );
            }
        }
        else if ( cmd == "wraptext" )
        {
            m_stream >> x >> y >> w >> h >> text;
            if ( ( toScreen && screenOn ) || (toPrinter && printerOn ) )
            {
                painter.drawText( xPix(x), yPix(y), xPix(w), yPix(h),
                    Qt::WordBreak, text );
            }
        }
        // Simply skip unknown commands for now.
        else
        {
            qDebug( QString(
                "Composer::paint() - unknown command %1 from %2." )
                .arg( cmd ).arg( fileName ) );
        }
    }

    // Clean up and return.
    m_file.close();
    painter.end();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Private function to handle all graph drawing onto the output device.
 *
 *  \param painterPtr Pointer to the QPainter in use.
 *  \param fontScale Font scale factor passed to paint().
 *
 *  This function is only called by paint().
 */

void Composer::paintGraph( QPainter *painterPtr, double fontScale )
{
    // Read the rest of the Graph parameters from the composer file.
    Graph graph;
    double marginLeft, marginTop, bodyWd, bodyHt;
    m_stream >> marginLeft >> marginTop >> bodyWd >> bodyHt >> graph;

    // Set graph area on page.
    int x0 = (int) (marginLeft * m_xppi );
    int y0 = (int) (marginTop * m_yppi );
    int wd = (int) (bodyWd * m_xppi );
    int ht = (int) (bodyHt * m_yppi );

    // TO DO: Optionally call a dialog here to change graph attributes?

    // Portrait graphs.
    if ( true )
    {
        graph.setCanvas( x0, y0, wd, ht, 0 );
    }
    // Landscape graphs.
    else
    {
        graph.setCanvas( x0, y0+ht, ht, wd, 0 );
        graph.setCanvasRotation( 270. );
    }

    // Draw the graph and return.
    graph.draw( painterPtr, fontScale );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines the x-pixel corresponding to the passed inches.
 *
 *  For printer devices, this is always within the current page.
 *  For screen devices, this is relative to all pages.
 *
 *  \return The x-pixel offset from left edge of the device.
 */

inline int Composer::xPix( double inches ) const
{
    return( (int) (m_xppi * inches) );
}

//------------------------------------------------------------------------------
/*! \brief Determines the y-pixel corresponding to the passed inches.
 *
 *  For printer devices, this is always within the current page.
 *  For screen devices, this is relative to all pages.
 *
 *  \return The y-pixel offset from the top of the device.
 */

inline int Composer::yPix( double inches ) const
{
    return( (int) (m_yppi * inches) );
}

//------------------------------------------------------------------------------
//  End of composer.cpp
//------------------------------------------------------------------------------

