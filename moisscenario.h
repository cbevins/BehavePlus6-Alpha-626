//------------------------------------------------------------------------------
/*! \file moisscenario.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief MoisScenario and MoisScenarioList class definitions.
 */

#ifndef _MOISSCENARIO_H_
/*! \def _MOISSCENARIO_H_
    \brief Prevent redundant includes.
 */
#define _MOISSCENARIO_H_ 1

// Qt class references
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

// Standard include files
#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class MoisScenario moisscenario.h
 *
 *  \brief The MoisScenario Class holds values in native units for all the fuel
 *  moisture parameters provided by a moisture scenario.
 */

class MoisScenario
{
// Public constructors
public:
    MoisScenario( void ) ;
    MoisScenario(
        const QString &file,    const QString &name,    const QString &desc,
        double moisDead1,       double moisDead10,      double moisDead100,
        double moisDead1000,    double moisLiveHerb,    double moisLiveWood ) ;

    void formatHtmlTable( const QString &title, QString &msg ) ;
    bool loadBpm( const QString &fileName ) ;
    void print( FILE *fptr ) const;

// Public data members
public:
    QString m_file;             //!< Full Moisture Scenario file path name
    QString m_name;             //!< Moisture set name (appears on menu lists)
    QString m_desc;             //!< Moisture set description
    double  m_moisDead1;        //!< Dead 1-h moisture loading (fraction)
    double  m_moisDead10;       //!< Dead 10-h moisture loading (fraction)
    double  m_moisDead100;      //!< Dead 100-h moisture loading (fraction)
    double  m_moisDead1000;     //!< Dead 1000-h moisture loading (fraction)
    double  m_moisLiveHerb;     //!< Live herb moisture loading (fraction)
    double  m_moisLiveWood;     //!< Live wood moisture loading (fraction)
};

//------------------------------------------------------------------------------
/*! \class MoisScenarioList moisscenario.h
 *
 *  \brief The MoisScenarioList is a list of pointers to MoisScenarios.
 */

class MoisScenarioList : public QPtrList<MoisScenario>
{
// Public constructors
public:
    MoisScenarioList( void );

// Public manipulation methods
public:
    bool addMoisScenario( const QString &file,
            const QString &name, const QString &desc,
            double dead1,    double dead10,   double dead100,
            double dead1000, double liveHerb, double liveWood ) ;
    bool addStandardMoisScenarios( void ) ;
    bool deleteMoisScenario( const QString &name ) ;
    int  fileList( QStringList &list ) ;
    MoisScenario *moisScenarioByFileName( const QString &file ) ;
    MoisScenario *moisScenarioByScenarioName( const QString &name ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of moisscenario.h
//------------------------------------------------------------------------------

