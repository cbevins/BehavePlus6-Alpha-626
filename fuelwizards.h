//------------------------------------------------------------------------------
/*! \file fuelwizards.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BpDocument live fuel parameter wizards.
 */

#ifndef _FUELWIZARDS_H_
/*! \def _FUELWIZARDS_H_
 *  \brief Prevent redundant includes.
 */
#define _FUELWIZARDS_H_ 1

// Class references
#include "wizarddialog.h"

//------------------------------------------------------------------------------
/*! \class FuelWizard  fuelwizards.h
 *
 *  \brief Fuel parameter wizard base class.
 *
 *  \brief Base class for the following wizards:
 *  \arg FuelBedDepthWizard
 *  \arg FuelBedMextDeadWizard
 *  \arg FuelHeatWizard
 *  \arg FuelLoadDead1Wizard
 *  \arg FuelLoadDead10Wizard
 *  \arg FuelLoadDead100Wizard
 *  \arg FuelLoadLiveWizard
 *  \arg FuelSavrDead1Wizard
 *  \arg FuelSavrLiveWizard
 *  \arg PalmettoCoverWizard
 *  \arg PalmettoOverstoryBasalAreaWizard
 */

class FuelWizard : public WizardDialog
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelWizard( BpDocument *bp, EqVar *var, unsigned int items,
        const double values[], const QString models[],
        const QString &pictureFile, const QString &pictureTip,
        const char *name, const QString &headerKey="Wizard:Fuel:FuelModel" ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelBedDepthWizard  fuelwizards.h
 *
 *  \brief Dead fuel moisture of extinction wizard.
 */

class FuelBedDepthWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelBedDepthWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelBedMextDeadWizard  fuelwizards.h
 *
 *  \brief Dead fuel moisture of extinction wizard.
 */

class FuelBedMextDeadWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelBedMextDeadWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelHeatWizard  fuelwizards.h
 *
 *  \brief Fuel heat of combustion
 */

class FuelHeatWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelHeatWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelLoadDead1Wizard  fuelwizards.h
 *
 *  \brief Dead 1-h fuel load wizard dialog.
 */

class FuelLoadDead1Wizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelLoadDead1Wizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelLoadDead10Wizard  fuelwizards.h
 *
 *  \brief Dead 10-h fuel load wizard dialog.
 */

class FuelLoadDead10Wizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelLoadDead10Wizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelLoadDead100Wizard  fuelwizards.h
 *
 *  \brief Dead 100-h fuel load wizard dialog.
 */

class FuelLoadDead100Wizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelLoadDead100Wizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelLoadLiveWizard  fuelwizards.h
 *
 *  \brief Live herbaceous and woody fuel load Wizard dialog.
 */

class FuelLoadLiveWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelLoadLiveWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelSavrDead1Wizard  fuelwizards.h
 *
 *  \brief Dead 1-h fuel surface area-tovolume wizard dialog.
 */

class FuelSavrDead1Wizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelSavrDead1Wizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class FuelSavrLiveWizard  fuelwizards.h
 *
 *  \brief Live herbaceous and woody fuel surface area-to-volume wizard.
 */

class FuelSavrLiveWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    FuelSavrLiveWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};
//------------------------------------------------------------------------------
/*! \class HeatPerUnitAreaWizard  fuelwizards.h
 *
 *  \brief Surface fire heat per unit area.
 */

class HeatPerUnitAreaWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    HeatPerUnitAreaWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class PalmettoAgeWizard  fuelwizards.h
 *
 *  \brief Palmetto-gallberry rough age wizard.
 */

class PalmettoAgeWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    PalmettoAgeWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class PalmettoCoverWizard  fuelwizards.h
 *
 *  \brief Palmetto-gallberry fuel coverage wizard.
 */

class PalmettoCoverWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    PalmettoCoverWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

//------------------------------------------------------------------------------
/*! \class PalmettoOverstoryBasalAreaWizard  fuelwizards.h
 *
 *  \brief Palmetto-gallberry overstory basal area wizard.
 */

class PalmettoOverstoryBasalAreaWizard : public FuelWizard
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    PalmettoOverstoryBasalAreaWizard( BpDocument *bp, EqVar *var, const char *name ) ;
};

#endif

//------------------------------------------------------------------------------
//  End of fuelwizards.h
//------------------------------------------------------------------------------

