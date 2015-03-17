//------------------------------------------------------------------------------
/*! \file fuelmodel.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief FuelModel and FuelModelList class definitions.
 */

#ifndef _FUELMODEL_H_
/*! \def _FUELMODEL_H_
    \brief Prevent redundant includes.
 */
#define _FUELMODEL_H_ 1

// Qt class references
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class FuelModel fuelmodel.h
 *
 *  \brief The FuelModel Class holds values in native units for all the fuel
 *  parameters provided by a fuel model.
 */

class FuelModel
{
// Public constructors
public:
    FuelModel( void ) ;
    FuelModel( const QString &file,
        int number,      const QString &name, const QString &desc,
        double depth,    double mext,     double heatDead,  double heatLive,
        double load1,    double load10,   double load100,   double loadHerb,
        double loadWood, double savr1,    double savrHerb,  double savrWood,
        const QString &loadTransfer );
    void formatHtmlTable( const QString &title, QString &msg ) ;
    bool loadBpf( const QString &fileName ) ;
    void print( FILE *fptr ) const;
    bool isDynamic( void ) const ;
    bool isStatic( void ) const ;

// Public data members
public:
    QString m_file;     //!< Fully qualified FuelModel file path name
    QString m_name;     //!< Fuel model name (file basename, appears on viewlists)
    QString m_desc;     //!< Fuel model description
    QString m_transfer; //!< Herb fuel load transfer equation
    double  m_depth;    //!< Fuel bed depth (ft)
    double  m_mext;     //!< Dead fuel extinction moisture content (fraction)
    double  m_heatDead; //!< Dead fuel heat of combustion (Btu/lb)
    double  m_heatLive; //!< Live fuel heat of combustion (Btu/lb)
    double  m_load1;    //!< Dead 1-h fuel loading (lb/ft2)
    double  m_load10;   //!< Dead 10-h fuel loading (lb/ft2)
    double  m_load100;  //!< Dead 100-h fuel loading (lb/ft2)
    double  m_loadHerb; //!< Live herb fuel loading (lb/ft2)
    double  m_loadWood; //!< Live wood fuel loading (lb/ft2)
    double  m_savr1;    //!< Dead 1-h fuel surface area/volume ratios (ft2/ft3)
    double  m_savrHerb; //!< Live herb surface area/volume ratios (ft2/ft3)
    double  m_savrWood; //!< Live herb surface area/volume ratios (ft2/ft3)
    int     m_number;   //!< Fuel model's assigned unique id.
};

//------------------------------------------------------------------------------
/*! \class FuelModelList fuelmodel.h
 *
 *  \brief The FuelModelList is a list of pointers to FuelModels.
 *  Newly instantiated FuelModelLists have the new 60 standard FuelModels
 *  appended by the constructor.
 */

class FuelModelList : public QPtrList<FuelModel>
{
// Public constructor methods
public:
    FuelModelList( void ) ;

// Public manipulation methods
public:
    bool addFuelModel( const QString &file,
        int number, const QString &name, const QString &desc,
        double depth,    double mext,     double heatDead, double heatLive,
        double load1,    double load10,   double load100,  double loadHerb,
        double loadWood, double savr1,    double savrHerb, double savrWood,
        const QString &loadTransfer  );
    bool        addStandardFuelModels( void ) ;
    bool        deleteFuelModel( const QString &name ) ;
    int         fileList( QStringList &list ) ;
    FuelModel  *fuelModelByFileName( const QString &fileName ) ;
    FuelModel  *fuelModelByModelName( const QString &name ) ;
    FuelModel  *fuelModelByModelNumber( int number ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of fuelmodel.h
//------------------------------------------------------------------------------

