//------------------------------------------------------------------------------
/*! \file fuelmodel.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief FuelModel and FuelModelList class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "fuelmodel.h"

// Qt include files
#include <qfileinfo.h>
#include <qstring.h>

// Standard include files
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//------------------------------------------------------------------------------
/*! \brief FuelModel default constructor.
 */

FuelModel::FuelModel() :
    m_file(""),
    m_name(""),
    m_desc(""),
    m_transfer( "S" ),
    m_depth(1.),
    m_mext(.20),
    m_heatDead(8000.),
    m_heatLive(8000.),
    m_load1(0.),
    m_load10(0.),
    m_load100(0.),
    m_loadHerb(0.),
    m_loadWood(0.),
    m_savr1(1500),
    m_savrHerb(1500.),
    m_savrWood(1500.),
    m_number(0)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelModel full constructor.
 */

FuelModel::FuelModel( const QString &file,
        int number,      const QString &name, const QString &desc,
        double depth,    double mext,     double heatDead,  double heatLive,
        double load1,    double load10,   double load100,   double loadHerb,
        double loadWood, double savr1,    double savrHerb,  double savrWood,
        const QString &transfer ) :
    m_file(file),
    m_name(name),
    m_desc(desc),
    m_transfer(transfer),
    m_depth(depth),
    m_mext(mext),
    m_heatDead(heatDead),
    m_heatLive(heatLive),
    m_load1(load1),
    m_load10(load10),
    m_load100(load100),
    m_loadHerb(loadHerb),
    m_loadWood(loadWood),
    m_savr1(savr1),
    m_savrHerb(savrHerb),
    m_savrWood(savrWood),
    m_number(number)
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a QString containing an HTML table of the FuelModel
 *  parameters.
 *
 *  Called by AttachDialog::displayContents() and GuideDialog::displayContents().
 */

void FuelModel::formatHtmlTable( const QString &title, QString &msg )
{
    // Display the contents
	double f1 = 21.78;
	double f2 = 48.8243;
    msg = QString(
        "<h2>%1</h2><hr>"
        "<table>"
        "<tr>"
        "  <td>Fuel Model Number</td>"
        "  <td colspan='2'>%2</td>"
        "</tr>"
        "<table>"
        "<tr>"
        "  <td>Fuel Model Name</td>"
        "  <td colspan='2'>%3</td>"
        "</tr>"
        "<tr>"
        "  <td>Fuel Model Type</td>"
        "  <td colspan='2'>%4</td>"
        "</tr>"
        "<tr>"
        "  <td>Description</td>"
        "  <td colspan='2'>%5</td>"
        "</tr>"
        )
        .arg( title )
        .arg( m_number )
        .arg( m_name )
        .arg( isStatic() ? "Static" : "Dynamic" )
        .arg( m_desc );

    msg += QString(
        "<tr>"
        "  <td>1-h Fuel Load</td>"
        "  <td>%1 tons/ac</td>"
		"  <td>%2 tonnes/ha</td>"
        "</tr>"
		)
        .arg( m_load1 * f1, 0, 'g', 2 )
        .arg( m_load1 * f2, 0, 'g', 2 );

    msg += QString(
        "<tr>"
        "  <td>10-h Fuel Load</td>"
        "  <td>%1 tons/ac</td>"
		"  <td>%2 tonnes/ha</td>"
        "</tr>"
		)
        .arg( m_load10 * 21.78, 0, 'g', 2 )
        .arg( m_load10 * 48.8243, 0, 'g', 2 );

    msg += QString(
        "<tr>"
        "  <td>100-h Fuel Load</td>"
        "  <td>%1 tons/ac</td>"
		"  <td>%2 tonnes/ha</td>"
        "</tr>"
		)
        .arg( m_load100 * f1, 0, 'g', 2 )
        .arg( m_load100 * f2, 0, 'g', 2 );

    msg += QString(
        "<tr>"
        "  <td>Live Herbaceous Fuel Load</td>"
        "  <td>%1 tons/ac</td>"
		"  <td>%2 tonnes/ha</td>"
        "</tr>"
		)
        .arg( m_loadHerb * f1, 0, 'g', 2 )
        .arg( m_loadHerb * f2, 0, 'g', 2 );

    msg += QString(
        "<tr>"
        "  <td>Live Woody Fuel Load</td>"
        "  <td>%1 tons/ac</td>"
		"  <td>%2 tonnes/ha</td>"
        "</tr>"
        )
        .arg( m_loadWood * f1, 0, 'g', 2 )
        .arg( m_loadWood * f2, 0, 'g', 2 );

	f2 = 0.0328084;		// f2 * ft2/ft3 = cm2/cm3
    msg += QString(
        "<tr>"
        "  <td>1-h Surface Area/Vol Ratio</td>"
		"  <td>%1 ft2/ft3</td>"
		"  <td>%2 cm2/cm3</td>"
        "</tr>"
		)
        .arg( m_savr1 )
        .arg( m_savr1 * f2 );

    msg += QString(
        "<tr>"
        "  <td>Live Herbaceous Surface Area/Vol Ratio</td>"
		"  <td>%1 ft2/ft3</td>"
		"  <td>%2 cm2/cm3</td>"
        "</tr>"
		)
        .arg( m_savrHerb )
        .arg( m_savrHerb * f2 );

	msg += QString(
        "<tr>"
        "  <td>Live Woody Surface Area/Vol Ratio</td>"
		"  <td>%1 ft2/ft3</td>"
		"  <td>%2 cm2/cm3</td>"
        "</tr>"
		)
        .arg( m_savrWood )
        .arg( m_savrWood * f2 );
    
	msg += QString(
        "<tr>"
        "  <td>Fuel Bed Depth</td>"
		"  <td>%1 feet</td>"
		"  <td>%2 cm</td>"
        "</tr>"
		)
        .arg( m_depth )
        .arg( m_depth * 30.48 );
    
	msg += QString(
        "<tr>"
        "  <td>Dead Fuel Moisture of Extinction</td>"
		"  <td>%1 percent</td>"
		"  <td>%2 percent</td>"
        "</tr>"
		)
        .arg( m_mext * 100. )
        .arg( m_mext * 100. );
 
	f2 = 2.32779;		// f2 * Btu/lb = kiloJ / Kg
	msg += QString(
        "<tr>"
        "  <td>Dead Fuel Heat Content</td>"
		"  <td>%1 Btu/lb</td>"
		"  <td>%2 KJ/Kg</td>"
        "</tr>"
		)
        .arg( m_heatDead )
        .arg( m_heatDead * f2 );
    
	msg += QString(
        "<tr>"
        "  <td>Live Fuel Heat Content</td>"
		"  <td>%1 Btu/lb</td>"
		"  <td>%2 KJ/Kg</td>"
        "</tr>"
        "</table>"
        )
        .arg( m_heatLive )
        .arg( m_heatLive * f2 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Determines if the fuel model is dynamic.
 *
 *  \return TRUE if fuel model is dynamic, FALSE if fuel model is static.
 */

bool FuelModel::isDynamic( void ) const
{
    return( m_transfer == "D" || m_transfer == "d" );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the fuel model is static.
 *
 *  \return TRUE if fuel model is static, FALSE if fuel model is dynamic.
 */

bool FuelModel::isStatic( void ) const
{
    return( m_transfer == "S" || m_transfer == "s" );
}

//------------------------------------------------------------------------------
/*! \brief Opens and reads a BehavePlus file into the FuelModel instance.
 *
 *  \return TRUE on success or FALSE on failure.
 */

typedef struct _varData
{
    const char *name;
    const char *units;
    double      value;
} VarData;

bool FuelModel::loadBpf( const QString &fileName )
{
    // Variables to be loaded and their native units
    static const VarData data[] =
    {
        { "name=\"vSurfaceFuelBedDepth\"",     "ft",           1.0 },	// 0
        { "name=\"vSurfaceFuelBedMextDead\"",  "fraction",     .20 },	// 1
        { "name=\"vSurfaceFuelHeatDead\"",     "Btu/lb",    8000.0 },	// 2
        { "name=\"vSurfaceFuelHeatLive\"",     "Btu/lb",    8000.0 },	// 3
        { "name=\"vSurfaceFuelLoadDead1\"",    "lb/ft2",       0.0 },	// 4
        { "name=\"vSurfaceFuelLoadDead10\"",   "lb/ft2",       0.0 },	// 5
        { "name=\"vSurfaceFuelLoadDead100\"",  "lb/ft2",       0.0 },	// 6
        { "name=\"vSurfaceFuelLoadLiveHerb\"", "lb/ft2",       0.0 },	// 7
        { "name=\"vSurfaceFuelLoadLiveWood\"", "lb/ft2",       0.0 },	// 8
        { "name=\"vSurfaceFuelSavrDead1\"",    "ft2/ft3",   1500.0 },	// 9
        { "name=\"vSurfaceFuelSavrLiveHerb\"", "ft2/ft3",   1500.0 },	// 10
        { "name=\"vSurfaceFuelSavrLiveWood\"", "ft2/ft3",   1500.0 },	// 11
        { "name=\"vSurfaceFuelBedModelNumber\"","",           14.0 },	// 12
        { "name=\"vSurfaceFuelLoadTransferEq\"","ratio",         0 },	// 13
        { "name=\"vSurfaceFuelBedModelCode\"",  "ratio",       0.0 },	// 14
        { NULL,                                 NULL,          0.0 }	// 15
    };

    // Store the model name
    QString msg("");
    QFileInfo fi( fileName );
    m_name = fi.baseName();		// Start with the file base name as the default Fuel Model Cose
    m_file = fileName;

    // Open the file.
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "r") ) )
    // This code block should never be executed!
    {
        translate( msg, "FuelModel:FileOpenError", fileName );
        error( msg );
        return( false );
    }
    // Store default parameter values
    double val[15], factor, offset, x;
    int i;
    for ( i = 0;
          data[i].name;
          i++ )
    {
        val[i] = data[i].value;
    }

    // Read records
    int line = 0;
    char buffer[1024], units[256], value[256], *ptr;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        line++;
        // Check for the FuelModel description
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
        // Check for FuelModel variables
        for ( i = 0;
              data[i].name;
              i++ )
        {
            if ( strstr( buffer, data[i].name ) )
            {
                // Special handling for discrete variables
                //if ( data[i].name == "name=\"vSurfaceFuelLoadTransferEq\"" )
                if ( ! strcmp( data[i].name, "name=\"vSurfaceFuelLoadTransferEq\"" ) )
                {
                    if ( ! ( ptr = strstr( buffer, "code=\"" ) ) )
                    {
                        translate( msg, "FuelModel:MissingAttribute",
                            QString( "%1:%2" ).arg( fileName ).arg( line ),
                            "code" );
                        error( msg );
                        fclose( fptr );
                        return( false );
                    }
                    memset( value, '\0', sizeof(value) );
                    sscanf( ptr+6, "%[^\"]", value );
                    m_transfer = value;
                    break;
                }
                if ( ! strcmp( data[i].name, "name=\"vSurfaceFuelBedModelCode\"" ) )
                {
                    if ( ! ( ptr = strstr( buffer, "text=\"" ) ) )
                    {
                        translate( msg, "FuelModel:MissingAttribute",
                            QString( "%1:%2" ).arg( fileName ).arg( line ),
                            "code" );
                        error( msg );
                        fclose( fptr );
                        return( false );
                    }
                    memset( value, '\0', sizeof(value) );
                    sscanf( ptr+6, "%[^\"]", value );
                    m_name = value;
                    break;
                }
                // Get the units
                if ( ! ( ptr = strstr( buffer, "units=\"" ) ) )
                {
                    translate( msg, "FuelModel:MissingAttribute",
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
                    translate( msg, "FuelModel:BadUnits",
                        QString( "%1:%2" ).arg( fileName ).arg( line ),
                        appSiUnits()->htmlMsg() );
                    error( msg );
                    fclose( fptr );
                    return( false );
                }
                // Get the value
                if ( ! ( ptr = strstr( buffer, "value=\"" ) ) )
                {
                    translate( msg, "FuelModel:MissingAttribute",
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
        // Ignore all others
    }
    fclose( fptr );

    // Store the items for this FuelModel
    m_depth    = val[0];
    m_mext     = val[1];
    m_heatDead = val[2];
    m_heatLive = val[3];
    m_load1    = val[4];
    m_load10   = val[5];
    m_load100  = val[6];
    m_loadHerb = val[7];
    m_loadWood = val[8];
    m_savr1    = val[9];
    m_savrHerb = val[10];
    m_savrWood = val[11];
	m_number   = val[12];
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Prints the FuelModel to the file stream.
 */

void FuelModel::print( FILE *fptr ) const
{

    fprintf(fptr, "\nFile       : %s\n", m_file.latin1() );
    fprintf(fptr,   "Name       : %s\n", m_name.latin1() );
    fprintf(fptr,   "Description: %s\n", m_desc.latin1() );
    fprintf(fptr,   "Depth      : %2.1f (ft)\n", m_depth );
    fprintf(fptr,   "Mext       : %3.2f (fraction)\n", m_mext );
    fprintf(fptr,   "Dead Heat  : %1.0f (Btu/lb)\n", m_heatDead );
    fprintf(fptr,   "Live Heat  : %1.0f (Btu/lb)\n", m_heatLive );
    fprintf(fptr,   "Load 1     : %5.4f (lb/ft2)\n", m_load1 );
    fprintf(fptr,   "Load 10    : %5.4f (lb/ft2)\n", m_load10 );
    fprintf(fptr,   "Load 100   : %5.4f (lb/ft2)\n", m_load100 );
    fprintf(fptr,   "Load Herb  : %5.4f (lb/ft2)\n", m_loadHerb );
    fprintf(fptr,   "Load Wood  : %5.4f (lb/ft2)\n", m_loadWood );
    fprintf(fptr,   "Savr 1     : %5.4f (ft2/ft3)\n", m_savr1 );
    fprintf(fptr,   "Savr Herb  : %5.4f (ft2/ft3)\n", m_savrHerb );
    fprintf(fptr,   "Savr Wood  : %5.4f (ft2/ft3)\n", m_savrWood );
    fprintf(fptr,   "Transfer   : %s\n", m_transfer.latin1() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief FuelModelList constructor.
 */

FuelModelList::FuelModelList( void )
{
    setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Creates a single FuelModel and adds it to the FuelModelList.
 *
 *  Version 2.1 Change: Converts \a name to lower case prior before storing.
 *
 *  \returns TRUE on success, FALSE on failure.
 */

bool FuelModelList::addFuelModel( const QString &file,
        int number,      const QString &name, const QString &desc,
        double depth,    double mext,     double heatDead, double heatLive,
        double load1,    double load10,   double load100,  double loadHerb,
        double loadWood, double savr1,    double savrHerb, double savrWood,
        const QString &loadTransfer )
{
    // Create the new FuelModel and add it by name to the fuel model list.
    FuelModel *fmPtr = new FuelModel( file, number, name.lower(), desc,
        depth, mext, heatDead, heatLive,
        load1, load10, load100, loadHerb, loadWood,
        savr1, savrHerb, savrWood, loadTransfer );
    checkmem( __FILE__, __LINE__, fmPtr, "FuelModel fmPtr", 1 );
    // Add it to this FuelModelList
    append( fmPtr );

	// Create a new FuelModel and add it by number to the fuel model list
	if ( number != name.toInt() )
	{
		QString numName = QString( "%1" ).arg( number, 3 );
		fmPtr = new FuelModel( file, number, numName, desc,
			depth, mext, heatDead, heatLive,
			load1, load10, load100, loadHerb, loadWood,
			savr1, savrHerb, savrWood, loadTransfer );
		checkmem( __FILE__, __LINE__, fmPtr, "FuelModel fmPtr", 1 );
		// Add it to this FuelModelList
		append( fmPtr );
	}
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Creates the the new 60 standard fire behavior FuelModels
 *  and adds them to the FuelModelList.
 *
 *  Version 2.1 Change: Added Scott and Burgan's fuel models.
 *
 *  Version 2.1 Change: Converts \a name and m_name to lower case prior to
 *  comparison.
 *
 *  \returns TRUE on success, FALSE on failure.
 */

bool FuelModelList::addStandardFuelModels( void )
{
    // Traditional 13 fire behavior fuel models WITH NUMBER CODES
    if ( ! ( addFuelModel ( "", 1, "1",
            "Short grass",
            1.0, 0.12, 8000., 8000.,
            0.034, 0.000, 0.000, 0.000, 0.000,
            3500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 2, "2",
            //"Timber with grass and understory [2]",
            "Timber grass and understory",      // Pat's new name
            1.0, 0.15, 8000., 8000.,
            0.092, 0.046, 0.023, 0.023, 0.000,
            3000., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 3, "3",
            "Tall grass",
            2.5, 0.25, 8000., 8000.,
            0.138, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 4, "4",
            "Chaparral",
            6.0, 0.20, 8000., 8000.,
            0.230, 0.184, 0.092, 0.000, 0.230,
            2000., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 5, "5",
            "Brush",
            2.0, 0.20, 8000., 8000.,
            0.046, 0.023, 0.000, 0.000, 0.092,
            2000., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 6, "6",
            "Dormant brush, hardwood slash",
            2.5, 0.25, 8000., 8000.,
            0.069, 0.115, 0.092, 0.000, 0.000,
            1750., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 7, "7",
            "Southern rough",
            2.5, 0.40, 8000., 8000.,
            0.052, 0.086, 0.069, 0.000, 0.017,
            1750., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 8, "8",
            //"Closed timber litter [8]",
            "Short needle litter",      // Pat's new name
            0.2, 0.30, 8000., 8000.,
            0.069, 0.046, 0.115, 0.000, 0.000,
            2000., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 9, "9",
            //"Hardwood litter [9]",
            "Long needle or hardwood litter",   // Pat's new name
            0.2, 0.25, 8000., 8000.,
            0.134, 0.019, 0.007, 0.000, 0.000,
            2500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 10, "10",
            //"Timber with litter & understory [10]",
            "Timber litter & understory",  // Pat's new name
            1.0, 0.25, 8000., 8000.,
            0.138, 0.092, 0.230, 0.000, 0.092,
            2000., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 11, "11",
            "Light logging slash",
            1.0, 0.15, 8000., 8000.,
            0.069, 0.207, 0.253, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 12, "12",
            "Medium logging slash",
            2.3, 0.20, 8000., 8000.,
            0.184, 0.644, 0.759, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 13, "13",
            "Heavy logging slash",
            3.0, 0.25, 8000., 8000.,
            0.322, 1.058, 1.288, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    // New dynamic models
#ifdef USE_NONBURNABLE_FUEL_MODELS
    // Non-burnable
    if ( ! ( addFuelModel ( "", 91, "nb1",
            "Water [91]",
            1.0, 0.10, 8000., 8000.,
            0.000, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 92, "nb2",
            "Urban, Developed [92]",
            1.0, 0.10, 8000., 8000.,
            0.000, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 93, "nb3",
            "Bare Ground [93]",
            1.0, 0.10, 8000., 8000.,
            0.000, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 94, "nb4",
            "Agricultural [94]",
            1.0, 0.10, 8000., 8000.,
            0.000, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 95, "nb5",
            "Snow, ice [95]",
            1.0, 0.10, 8000., 8000.,
            0.000, 0.000, 0.000, 0.000, 0.000,
            1500., 1500., 1500., "S" ) ) )
        return( false );
#endif
    // Grass
    double f = 2000. / 43560.;
    if ( ! ( addFuelModel ( "", 101, "gr1",
            "Short, sparse, dry climate grass (D)",
            0.4, 0.15, 8000., 8000.,
            0.1*f, 0.000, 0.000, 0.3*f, 0.000,
            2200., 2000., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 102, "gr2",
            "Low load, dry climate grass (D)",
            1.0, 0.15, 8000., 8000.,
            0.1*f, 0.000, 0.000, 1.0*f, 0.000,
            2000., 1800., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 103, "gr3",
            "Low load, very coarse, humid climate grass (D)",
            2.0, 0.30, 8000., 8000.,
            0.1*f, 0.4*f, 0.000, 1.5*f, 0.000,
            1500., 1300., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 104, "gr4",
            "Moderate load, dry climate grass (D)",
            2.0, 0.15, 8000., 8000.,
            0.25*f, 0.000, 0.000, 1.9*f, 0.000,
            2000., 1800., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 105, "gr5",
            "Low load, humid climate grass (D)",
            1.5, 0.40, 8000., 8000.,
            0.4*f, 0.000, 0.000, 2.5*f, 0.000,
            1800., 1600., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 106, "gr6",
            "Moderate load, humid climate grass (D)",
            1.5, 0.40, 9000., 9000.,
            0.1*f, 0.000, 0.000, 3.4*f, 0.000,
            2200., 2000., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 107, "gr7",
            "High load, dry climate grass (D)",
            3.0, 0.15, 8000., 8000.,
            1.0*f, 0.000, 0.000, 5.4*f, 0.000,
            2000., 1800., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 108, "gr8",
            "High load, very coarse, humid climate grass (D)",
            4.0, 0.30, 8000., 8000.,
            0.5*f, 0.04591390, 0.000, 7.3*f, 0.000,
            1500., 1300., 1500., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 109, "gr9",
            "Very high load, humid climate grass (D)",
            5.0, 0.40, 8000., 8000.,
            1.0*f, 1.0*f, 0.000, 9.0*f, 0.000,
            1800., 1600., 1500., "D" ) ) )
        return( false );

    // Grass and shrub
    if ( ! ( addFuelModel ( "", 121, "gs1",
            "Low load, dry climate grass-shrub (D)",
            0.9, 0.15, 8000., 8000.,
            0.2*f, 0.000, 0.000, 0.5*f, 0.02984403,
            2000., 1800., 1800., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 122, "gs2",
            "Moderate load, dry climate grass-shrub (D)",
            1.5, 0.15, 8000., 8000.,
            0.5*f, 0.5*f, 0.000, 0.6*f, 1.0*f,
            2000., 1800., 1800., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 123, "gs3",
            "Moderate load, humid climate grass-shrub (D)",
            1.8, 0.40, 8000., 8000.,
            0.3*f, 0.25*f, 0.000, 1.45*f, 1.25*f,
            1800., 1600., 1600., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 124, "gs4",
            "High load, humid climate grass-shrub (D)",
            2.1, 0.40, 8000., 8000.,
            1.9*f, 0.3*f, 0.1*f, 3.4*f, 7.1*f,
            1800., 1600., 1600., "D" ) ) )
        return( false );

    // Shrub
    if ( ! ( addFuelModel ( "", 141, "sh1",
            "Low load, dry climate shrub (D)",
            1.0, 0.15, 8000., 8000.,
            0.25*f, 0.25*f, 0.000, 0.15*f, 1.3*f,
            2000., 1800., 1600., "D" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 142, "sh2",
            "Moderate load, dry climate shrub (S)",
            1.0, 0.15, 8000., 8000.,
            1.35*f, 2.4*f, 0.75*f, 0.000, 3.85*f,
            2000., 1800., 1600., "S" ) ) )
        return( false );

    if ( ! ( addFuelModel ( "", 143, "sh3",
            "Moderate load, humid climate shrub (S)",
            2.4, 0.40, 8000., 8000.,
            0.45*f, 3.0*f, 0.000, 0.000, 6.2*f,
            1600., 1800., 1400., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 144, "sh4",
            "Low load, humid climate timber-shrub (S)",
            3.0, 0.30, 8000., 8000.,
            0.85*f, 1.15*f, 0.2*f, 0.000, 2.55*f,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 145, "sh5",
            "High load, dry climate shrub (S)",
            6.0, 0.15, 8000., 8000.,
            3.6*f, 2.1*f, 0.000, 0.000, 2.9*f,
             750., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 146, "sh6",
            "Low load, humid climate shrub (S)",
            2.0, 0.30, 8000., 8000.,
            2.9*f, 1.45*f, 0.000, 0.000, 1.4*f,
             750., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 147, "sh7",
            "Very high load, dry climate shrub (S)",
            6.0, 0.15, 8000., 8000.,
            3.5*f, 5.3*f, 2.2*f, 0.000, 3.4*f,
             750., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 148, "sh8",
            "High load, humid climate shrub (S)",
            3.0, 0.40, 8000., 8000.,
            2.05*f, 3.4*f, 0.85*f, 0.000, 4.35*f,
             750., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 149, "sh9",
            "Very high load, humid climate shrub (D)",
            4.4, 0.40, 8000., 8000.,
            4.5*f, 2.45*f, 0.000, 1.55*f, 7.0*f,
             750., 1800., 1500., "D" ) ) )
         return( false );

    // Timber and understory
    if ( ! ( addFuelModel ( "", 161, "tu1",
            "Light load, dry climate timber-grass-shrub (D)",
            0.6, 0.20, 8000., 8000.,
            0.2*f, 0.9*f, 1.5*f, 0.2*f, 0.9*f,
            2000., 1800., 1600., "D" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 162, "tu2",
            "Moderate load, humid climate timber-shrub (S)",
            1.0, 0.30, 8000., 8000.,
            0.95*f, 1.8*f, 1.25*f, 0.000, 0.2*f,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 163, "tu3",
            "Moderate load, humid climate timber-grass-shrub (D)",
            1.3, 0.30, 8000., 8000.,
            1.1*f, 0.15*f, 0.25*f, 0.65*f, 1.1*f,
            1800., 1600., 1400., "D" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 164, "tu4",
            //"Dwarf conifer with understory (S)",
            "Dwarf conifer understory (S)", // Pat's new name
            0.5, 0.12, 8000., 8000.,
            4.5*f, 0.000, 0.000, 0.000, 2.0*f,
            2300., 1800., 2000., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 165, "tu5",
            "Very high load, dry climate timber-shrub (S)",
            1.0, 0.25, 8000., 8000.,
            4.0*f, 4.0*f, 3.0*f, 0.000, 3.0*f,
            1500., 1800., 750., "S" ) ) )
         return( false );

    // Timber and litter
    if ( ! ( addFuelModel ( "", 181, "tl1",
            "Low load, compact conifer litter (S)",
            0.2, 0.30, 8000., 8000.,
            1.0*f, 2.2*f, 3.6*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 182, "tl2",
            "Low load broadleaf litter (S)",
            0.2, 0.25, 8000., 8000.,
            1.4*f, 2.3*f, 2.2*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 183, "tl3",
            "Moderate load conifer litter (S)",
            0.3, 0.20, 8000., 8000.,
            0.5*f, 2.2*f, 2.8*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 184, "tl4",
            "Small downed logs (S)",
            0.4, 0.25, 8000., 8000.,
            0.5*f, 1.5*f, 4.2*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 185, "tl5",
            "High load conifer litter (S)",
            0.6, 0.25, 8000., 8000.,
            1.15*f, 2.5*f, 4.4*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 186, "tl6",
            "High load broadleaf litter (S)",
            0.3, 0.25, 8000., 8000.,
            2.4*f, 1.2*f, 1.2*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 187, "tl7",
            "Large downed logs (S)",
            0.4, 0.25, 8000., 8000.,
            0.3*f, 1.4*f, 8.1*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 188, "tl8",
            "Long-needle litter (S)",
            0.3, 0.35, 8000., 8000.,
            5.8*f, 1.4*f, 1.1*f, 0.000, 0.000,
            1800., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 189, "tl9",
            "Very high load broadleaf litter (S)",
            0.6, 0.35, 8000., 8000.,
            6.65*f, 3.30*f, 4.15*f, 0.000, 0.000,
            1800., 1800., 1600., "S" ) ) )
         return( false );

    // Slash and blowdown
    if ( ! ( addFuelModel ( "", 201, "sb1",
            "Low load activity fuel (S)",
            1.0, 0.25, 8000., 8000.,
            1.5*f, 3.0*f, 11.0*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 202, "sb2",
            "Moderate load activity or low load blowdown (S)",
            1.0, 0.25, 8000., 8000.,
            4.5*f, 4.25*f, 4.0*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 203, "sb3",
            "High load activity fuel or moderate load blowdown (S)",
            1.2, 0.25, 8000., 8000.,
            5.5*f, 2.75*f, 3.0*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    if ( ! ( addFuelModel ( "", 204, "sb4",
            "High load blowdown (S)",
            2.7, 0.25, 8000., 8000.,
            5.25*f, 3.5*f, 5.25*f, 0.000, 0.000,
            2000., 1800., 1600., "S" ) ) )
         return( false );

    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Removes the named FuelModel from the FuelModelList,
 *  which also caused it to be deleted since the list is autoDelete.
 *
 *  \return TRUE on success or FALSE on failure.
 */

bool FuelModelList::deleteFuelModel( const QString &name )
{
    FuelModel *mPtr = fuelModelByModelName( name );
    if ( ! mPtr )
    {
        return( false );
    }
    remove( mPtr );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Appends all the attached FuelModel file names to the passed \a list.
 *
 *  Called by BehavePlusDocument::configureFuelModels() and passed on to
 *  AttachDialog() to mark those FuelModel files that are already attached.
 *
 *  \return Number of file names appended to QStringList \a list.
*/

int FuelModelList::fileList( QStringList &list )
{
    int count = 0;
    for ( FuelModel *mPtr = first();
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
/*! \brief Finds the requested FuelModel by file name and returns its address.
 *
 *  Version 2.1 Change: Converts \a file and m_file to lowercase prior to
 *  comparison.
 *
 *  \return Pointer to FuelModel if found, 0 if not found.
 */

FuelModel *FuelModelList::fuelModelByFileName( const QString &file )
{
    QString lcfile = file.lower();
	lcfile.replace( QChar('\\'), "/" );
	QString match;
    for ( FuelModel *mPtr = first();
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
/*! \brief Finds the requested FuelModel by model name and returns its address.
 *
 *  Version 2.1 Change: Converts \a name and m_name to lowercase prior to
 *  comparison.
 *
 *  \return Pointer to FuelModel if found, 0 if not found.
 */

FuelModel *FuelModelList::fuelModelByModelName( const QString &name )
{
    QString lcname = name.lower();
    for ( FuelModel *mPtr = first();
          mPtr != 0;
          mPtr = next() )
    {
        if ( mPtr->m_name.lower() == lcname )
        {
            return( mPtr );
        }
    }
	// Build 607 - Check if model number is being used instead of model name
	return fuelModelByModelNumber( name.toInt() );
}

//------------------------------------------------------------------------------
/*! \brief Finds the requested FuelModel by model number and returns its address.
 *
 *  \return Pointer to FuelModel if found, 0 if not found.
 */

FuelModel *FuelModelList::fuelModelByModelNumber( int number )
{
    for ( FuelModel *mPtr = first();
          mPtr != 0;
          mPtr = next() )
    {
        if ( mPtr->m_number == number )
        {
            return( mPtr );
        }
    }
    return( 0 );
}
//------------------------------------------------------------------------------
//  End of fuelmodel.cpp
//------------------------------------------------------------------------------

