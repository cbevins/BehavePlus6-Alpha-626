//------------------------------------------------------------------------------
/*! \file moisscenario.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief  MoisScenario and MoisScenarioList class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "moisscenario.h"

// Qt include files
#include <qfileinfo.h>
#include <qstring.h>

// Standard include files
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//------------------------------------------------------------------------------
/*! \brief MoisScenario default constructor.
 */

MoisScenario::MoisScenario() :
    m_file(""),
    m_name(""),
    m_desc(""),
    m_moisDead1(0.10),
    m_moisDead10(0.10),
    m_moisDead100(0.10),
    m_moisDead1000(0.10),
    m_moisLiveHerb(1.0),
    m_moisLiveWood(1.0)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelModel full constructor.
 */

MoisScenario::MoisScenario(
        const QString &file,    const QString &name,    const QString &desc,
        double moisDead1,       double moisDead10,      double moisDead100,
        double moisDead1000,    double moisLiveHerb,    double moisLiveWood ) :
    m_file(file),
    m_name(name),
    m_desc(desc),
    m_moisDead1(moisDead1),
    m_moisDead10(moisDead10),
    m_moisDead100(moisDead100),
    m_moisDead1000(moisDead1000),
    m_moisLiveHerb(moisLiveHerb),
    m_moisLiveWood(moisLiveWood)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a QString containing an HTML table of the moisture scenario
 *  parameters.
 *
 *  Called by AttachDialog::displayContents() and GuideDialog::displayContents().
 */

void MoisScenario::formatHtmlTable( const QString &title, QString &msg )
{
    // Display the contents
    msg = QString(
        "<H2>%1</H2><HR>"
        "<TABLE>"
            "<TR>"
            "  <TD>Scenario Name"
            "  <TD>%2"
            "<TR>"
            "  <TD>Description"
            "  <TD>%3"
            "<TR>"
            "  <TD>Dead 1-h Moisture"
            "  <TD>%4 percent"
            "<TR>"
            "  <TD>Dead 10-h Moisture"
            "  <TD>%5 percent"
            "<TR>"
            "  <TD>Dead 100-h Moisture"
            "  <TD>%6 percent"
            "<TR>"
            "  <TD>Live Herbaceous Moisture"
            "  <TD>%7 percent"
            "<TR>"
            "  <TD>Live Woody Moisture"
            "  <TD>%8 percent"
            "<TR>"
        "</TABLE>"
        )
        .arg( title )
        .arg( m_name )
        .arg( m_desc )
        .arg( m_moisDead1 * 100. )
        .arg( m_moisDead10 * 100. )
        .arg( m_moisDead100 * 100. )
        .arg( m_moisLiveHerb * 100. )
        .arg( m_moisLiveWood * 100. );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Opens and reads a BehavePlus file into the MoisScenario instance.
 *
 *  \return TRUE on success or FALSE on failure.
 */

typedef struct _moisVarData
{
    const char *name;
    const char *units;
    double      value;
} MoisVarData;

bool MoisScenario::loadBpm( const QString &fileName )
{
    // Variables to be loaded and their native units
    static const MoisVarData data[] =
    {
        { "name=\"vSurfaceFuelMoisDead1\"",    "fraction",    0.10 },
        { "name=\"vSurfaceFuelMoisDead10\"",   "fraction",    0.10 },
        { "name=\"vSurfaceFuelMoisDead100\"",  "fraction",    0.10 },
        { "name=\"vSurfaceFuelMoisDead1000\"", "fraction",    0.10 },
        { "name=\"vSurfaceFuelMoisLiveHerb\"", "fraction",    1.00 },
        { "name=\"vSurfaceFuelMoisLiveWood\"", "fraction",    1.00 },
        { NULL,                                 NULL,         0.00 }
    };
    // Store the file info
    QString msg("");
    QFileInfo fi( fileName );
    m_name = fi.baseName();
    m_file = fileName;

    // Open the file.
    FILE *fptr;
    if ( ! ( fptr = fopen( (const char *) fileName, "r") ) )
    // This code block should never be executed!
    {
        translate( msg, "MoisScenario:FileOpenError", fileName );
        error( msg );
        return( false );
    }
    // Store default parameter values
    double val[6], factor, offset, x;
    int i;
    for ( i = 0;
          i < 6;
          i++ )
    {
        val[i] = data[i].value;
    }
    // Read records
    int line = 0;
    char buffer[1024], value[256], units[256], *ptr;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        line++;
        // Check for the moisture scenario description <property>
        if ( strstr( buffer, "<property " ) )
        {
            if ( strstr( buffer, "name=\"appDescription\"" ) )
            {
                if ( ( ptr = strstr( buffer, "value=\"" ) ) )
                {
                    memset( value, '\0', sizeof(value) );
                    sscanf( ptr+7, "%[^\"]", value );
                    m_desc = value;
                }
            }
            continue;
        }
        // Skip <prescription> fields
        if ( strstr( buffer, "<prescription " ) )
        {
            continue;
        }
        // Check for MoisScenario variables
        for ( i = 0;
              data[i].name;
              i++ )
        {
            if ( strstr( buffer, data[i].name ) )
            {
                // Get the units
                if ( ! ( ptr = strstr( buffer, "units=\"" ) ) )
                // This code block should never be executed!
                {
                    translate( msg, "MoisScenario:MissingAttribute",
                        QString( "%1:%2" ).arg( fileName ).arg( line ),
                        "units" );
                    error( msg );
                    fclose( fptr );
                    return( false );
                }
                memset( units, '\0', sizeof(units) );
                sscanf( ptr+7, "%[^\"]", units );
                // Make sure units are convertible
                if ( ! appSiUnits()->conversionFactorOffset(
                    (const char *) units, data[i].units, &factor, &offset ) )
                {
                    translate( msg, "MoisScenario:BadUnits",
                        QString( "%1:%2" ).arg( fileName ).arg( line ),
                        appSiUnits()->htmlMsg() );
                    error( msg );
                    fclose( fptr );
                    return( false );
                }
                // Get the value
                if ( ! ( ptr = strstr( buffer, "value=\"" ) ) )
                {
                    translate( msg, "MoisScenario:MissingAttribute",
                        QString( "%1:%2" ).arg( fileName ).arg( line ),
                        "value" );
                    error( msg );
                    fclose( fptr );
                    return( false );
                }
                memset( value, '\0', sizeof(value) );
                sscanf( ptr+7, "%[^\"]", value );
                sscanf( value, "%lf", &x );
                // Convert and store the value
                val[i] = offset + factor * x;
                break;
            }
        }
        // Ignore all others.
    }
    fclose( fptr );

    // Store the items for this MoisScenario
    m_moisDead1    = val[0];
    m_moisDead10   = val[1];
    m_moisDead100  = val[2];
    m_moisDead1000 = val[3];
    m_moisLiveHerb = val[4];
    m_moisLiveWood = val[5];
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Prints the MoisScenario to the file stream.
 */

void MoisScenario::print( FILE *fptr ) const
{

    fprintf( fptr, "\nFile       : %s\n", m_file.latin1() );
    fprintf( fptr,   "Name       : %s\n", m_name.latin1() );
    fprintf( fptr,   "Description: %s\n", m_desc.latin1() );
    fprintf( fptr,   "Dead 1-h   : %4.2f (fraction)\n", m_moisDead1 );
    fprintf( fptr,   "Dead 10-h  : %4.2f (fraction)\n", m_moisDead10 );
    fprintf( fptr,   "Dead 100-h : %4.2f (fraction)\n", m_moisDead100 );
    fprintf( fptr,   "Dead 1000-h: %4.2f (fraction)\n", m_moisDead1000 );
    fprintf( fptr,   "Live Herb  : %4.2f (fraction)\n", m_moisLiveHerb );
    fprintf( fptr,   "Live Wood  : %4.2f (fraction)\n", m_moisLiveWood );
    return;
}

//------------------------------------------------------------------------------
/*! \brief MoisScenarioList constructor.
 */

MoisScenarioList::MoisScenarioList( void )
{
    setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a single MoisScenario and adds it to the MoisScenarioList.
 *
 *  \param file     File name (empty for standard scenarios).
 *  \param name     Name used as input.
 *  \param desc     Description of the moisture scenario.
 *  \param dead1    Dead 1-h time-lag fuel moisture content (lb/lb)
 *  \param dead10   Dead 10-h time-lag fuel moisture content (lb/lb)
 *  \param dead100  Dead 100-h time-lag fuel moisture content (lb/lb)
 *  \param dead1000 Dead 1000-h time-lag fuel moisture content (lb/lb)
 *  \param liveHerb Live herbaceous fuel moisture content (lb/lb)
 *  \param liveWood Live woody fuel moisture content (lb/lb)

 *  \returns TRUE on success, FALSE on failure.
 */

bool MoisScenarioList::addMoisScenario( const QString &file,
        const QString &name, const QString &desc,
        double dead1,    double dead10,   double dead100,  double dead1000,
        double liveHerb, double liveWood )
{
    // Create the new MoisScenario.
    MoisScenario *msPtr = new MoisScenario( file, name, desc,
        dead1, dead10, dead100, dead1000, liveHerb, liveWood );
    checkmem( __FILE__, __LINE__, msPtr, "MoisScenario msPtr", 1 );
    // Add it to this MoisScenarioList
    append( msPtr );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Creates the the 13 standard fire behavior MoisScenarios
 *  and adds them to the MoisScenarioList.
 *
 *  \returns TRUE on success, FALSE on failure.
 */

bool MoisScenarioList::addStandardMoisScenarios( void )
{
    if ( ! ( addMoisScenario ( "", "1-low",
            "TSTMDL (3,4,5,70,70)",
            0.03, 0.04, 0.05, 0.08, 0.70, 0.70 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "2-med",
            "TSTMDL (6,7,8,120,120)",
            0.06, 0.07, 0.08, 0.14, 1.20, 1.20 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "3-high",
            "TSTMDL (12,13,14,170,170)",
            0.12, 0.13, 0.14, 0.24, 1.70, 1.70 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d1l1",
            "Very low dead, fully-cured herb",
            0.03, 0.04, 0.05, 0.07, 0.30, 0.60 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d1l2",
            "Very low dead, 2/3-cured herb",
            0.03, 0.04, 0.05, 0.07, 0.60, 0.90 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d1l3",
            "Very low dead, 1/3-cured herb",
            0.03, 0.04, 0.05, 0.07, 0.90, 1.20 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d1l4",
            "Very low dead, fully-green herb",
            0.03, 0.04, 0.05, 0.07, 1.20, 1.50 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d2l1",
            "Low dead, fully-cured herb",
            0.06, 0.07, 0.08, 0.10, 0.30, 0.60 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d2l2",
            "Low dead, 2/3-cured herb",
            0.06, 0.07, 0.08, 0.10, 0.60, 0.90 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d2l3",
            "Low dead, 1/3-cured herb",
            0.06, 0.07, 0.08, 0.10, 0.90, 1.20 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d2l4",
            "Low dead, fully-green herb",
            0.06, 0.07, 0.08, 0.10, 1.20, 1.50 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d3l1",
            "Moderate dead, fully-cured herb",
            0.09, 0.10, 0.11, 0.12, 0.30, 0.60 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d3l2",
            "Moderate dead, 2/3-cured herb",
            0.09, 0.10, 0.11, 0.12, 0.60, 0.90 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d3l3",
            "Moderate dead, 1/3-cured herb",
            0.09, 0.10, 0.11, 0.12, 0.90, 1.20 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d3l4",
            "Moderate dead, fully-green herb",
            0.09, 0.10, 0.11, 0.12, 1.20, 1.50 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d4l1",
            "High dead, fully-cured herb",
            0.12, 0.13, 0.14, 0.16, 0.30, 0.60 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d4l2",
            "High dead, 2/3-cured herb",
            0.12, 0.13, 0.14, 0.16, 0.60, 0.90 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d4l3",
            "High dead, 1/3-cured herb",
            0.12, 0.13, 0.14, 0.16, 0.90, 1.20 ) ) )
        return( false );

    if ( ! ( addMoisScenario ( "", "d4l4",
            "High dead, fully-green herb",
            0.12, 0.13, 0.14, 0.16, 1.20, 1.50 ) ) )
        return( false );

    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Removes the named MoisScenario from the MoisScenarioList,
 *  which also caused it to be deleted since the list is autoDelete.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool MoisScenarioList::deleteMoisScenario( const QString &name )
{
    MoisScenario *mPtr = moisScenarioByScenarioName( name );
    if ( ! mPtr )
    {
        return( false );
    }
    remove( mPtr );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Appends all the attached MoisScenario file names to the passed
 *  \a list.
 *
 *  Called by BehavePlusDocument::configureMoisScenarios() and passed on to
 *  AttachDialog() to mark those moisture scenario files that are already
 *  attached.
 *
 *  \return Number of file names appended to QStringList \a list.
*/

int MoisScenarioList::fileList( QStringList &list )
{
    int count = 0;
    for ( MoisScenario *mPtr = first();
          mPtr != 0;
          mPtr = next() )
    {
        if ( ! mPtr->m_file.isEmpty() )
        {
            list.append( mPtr->m_file );
            count++;
        }
    }
    return( count );
}

//------------------------------------------------------------------------------
/*! \brief Finds the requested MoisScenario by file name and returns its address.
 *
 *  Version 2.1 Change: Converts \a file and m_file to lowercase prior to
 *  comparison.
 *
 *  \return Pointer to MoisScenario if found, 0 if not found.
 */

MoisScenario *MoisScenarioList::moisScenarioByFileName( const QString &file )
{
    QString lcfile = file.lower();
 	lcfile.replace( QChar('\\'), "/" );
	QString match;
   for ( MoisScenario *mPtr = first();
          mPtr != 0;
          mPtr = next() )
    {
 		match = mPtr->m_file.lower();
		match.replace( QChar( '\\' ), "/" );
        if ( match == lcfile )
        {
            return( mPtr );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Finds the requested MoisScenario by moisture scenario name
 *  and returns its address.
 *
 *  Version 2.1 Change: Converts \a name and m_name to lowercase prior to
 *  comparison.
 *
 *  \return Pointer to MoisScenario if found, 0 if not found.
 */

MoisScenario *MoisScenarioList::moisScenarioByScenarioName( const QString &name )
{
    QString lcname = name.lower();
    for ( MoisScenario *mPtr = first();
          mPtr != 0;
          mPtr = next() )
    {
        if ( mPtr->m_name.lower() == lcname )
        {
            return( mPtr );
        }
    }
    return( 0 );
}

//------------------------------------------------------------------------------
//  End of moiscenario.cpp
//------------------------------------------------------------------------------

