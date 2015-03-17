//------------------------------------------------------------------------------
/*! \file standardwizards.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlusDocument standard wizards.
 */

#ifndef _STANDARDWIZARDS_H_
/*! \def _STANDARDWIZARDS_H_
    \brief Prevent redundant includes.
 */
#define _STANDARDWIZARDS_H_ 1

// Class references
#include "wizarddialog.h"

//------------------------------------------------------------------------------
/*! \class StandardWizard  standardwizards.h
 *
 *  \brief Base class for the following wizards:
 *  \arg BarkThicknessWizard
 *  \arg CompassNorthWizard
 *  \arg CompassUpslopeWizard
 *  \arg CrownRatioWizard
 *  \arg FireLineIntWizard
 *  \arg LiveMoistureWizard
 *  \arg SlopeDegreesWizard
 *  \arg SlopeFractionWizard
 */

class StandardWizard : public WizardDialog
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    StandardWizard( BpDocument *bp, EqVar *var,
        unsigned int items, const double value[], const QString text[],
        const QString &col0Text, const QString &col1Text,
        const QString &pictureFile, const QString &pictureTip,
        const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class BarkThicknessWizard  standardwizards.h
 *
 *  \brief Tree bark thickness wizard.
 */

class BarkThicknessWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    BarkThicknessWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class CanopyBulkDensityWizard  standardwizards.h
 *
 *  \brief Slope steepness (degrees) wizard.
 */

class CanopyBulkDensityWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    CanopyBulkDensityWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class CompassNorthWizard  standardwizards.h
 *
 *  \brief Compass direction from north wizard.
 */

class CompassNorthWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    CompassNorthWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class CompassUpslopeWizard  standardwizards.h
 *
 *  \brief Compass direction from upslope wizard.
 */

class CompassUpslopeWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    CompassUpslopeWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class CrownRatioWizard  standardwizards.h
 *
 *  \brief Crown ratio wizard.
 */

class CrownRatioWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    CrownRatioWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FireLineIntWizard  standardwizards.h
 *
 *  \brief Fireline intensity wizard.
 */

class FireLineIntWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    FireLineIntWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class LiveMoistureWizard  standardwizards.h
 *
 *  \brief Live fuel moisture wizard.
 */

class LiveMoistureWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    LiveMoistureWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class LiveHerbMoistureWizard  standardwizards.h
 *
 *  \brief Live herbaceous fuel moisture wizard.
 */

class LiveHerbMoistureWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    LiveHerbMoistureWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class LiveWoodMoistureWizard  standardwizards.h
 *
 *  \brief Live woody fuel moisture wizard.
 */

class LiveWoodMoistureWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    LiveWoodMoistureWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class MapFractionWizard  standardwizards.h
 *
 *  \brief Map representative fraction wizard.
 */

class MapFractionWizard : public WizardDialog
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    MapFractionWizard( BpDocument *bp, EqVar *var, const char *name ) ;
    ~MapFractionWizard( void ) ;
};

//------------------------------------------------------------------------------
/*! \class SafetyZoneEquipmentAreaWizard  standardwizards.h
 *
 *  \brief Safety zone equipment area wizard.
 */

class SafetyZoneEquipmentAreaWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    SafetyZoneEquipmentAreaWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class SafetyZonePersonnelAreaWizard  standardwizards.h
 *
 *  \brief Safety zone equipment area wizard.
 */

class SafetyZonePersonnelAreaWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    SafetyZonePersonnelAreaWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class SlopeDegreesWizard  standardwizards.h
 *
 *  \brief Slope steepness (degrees) wizard.
 */

class SlopeDegreesWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    SlopeDegreesWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class SlopeFractionWizard  standardwizards.h
 *
 *  \brief Slope steepness (rise/reach) wizard.
 */

class SlopeFractionWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    SlopeFractionWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class WindAdjWizard  standardwizards.h
 *
 *  \brief Wind adjustment factor wizard.
 */

class WindAdjWizard : public StandardWizard
{
// Enable signals and slots
    Q_OBJECT
// Public constructor
public:
    WindAdjWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of standardwizards.h
//------------------------------------------------------------------------------

