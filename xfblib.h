//------------------------------------------------------------------------------
/*! \file xfblib.h
 *  \version BehavePlus5
 *  \author Copyright (C) 2002-2011 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Fire behavior C API library.
 *
 *  This file is a collection of fire behavior functions containing the
 *  algorithms used by BehavePlus.
 *
 *  Complex data structures are eschewed as function parameters;
 *  all parameters are passed as scalers or simple arrays,
 *  never as pointers to typedefs or classes.
 *  This allows the API to be called by higher level functions with their
 *  own notion of data structures or to be wrapped into C++ classes.
 */

#ifndef _FBLIB_H_
/*! \def _FBLIB_H_
 *  \brief Prevent redundant includes.
 */
#define _FBLIB_H_ 1

#ifdef __cplusplus
/*! \def EXTERN
 *  \brief Prevents name-mangling of C source code by C++ compilers.
 */
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

// Custom include files
#include "newext.h"         // Part of Mark Finney's EXRATE package
#include "randfuel.h"       // Part of Mark Finney's EXRATE package
#include "randthread.h"     // Part of Mark Finney's EXRATE package

// Standard include files
#include <math.h>

/*! \def M_PI
 *  \brief Some compilers don't define this.
 */
#ifndef M_PI
#define M_PI 3.141592654
#endif

/*! \def SMIDGEN
 *  \brief Number used to test for "close enough to zero"
 *  to prevent divide-by-zero, sqrt(0), etc.
 */
const double SMIDGEN = 1.0e-07;

/*! \enum LifeType
 *  \brief A code used to determine both the fuel life category (dead or live)
 *  and how moisture is assigned to it.
 */
enum FuelLifeType
{
    FuelLifeType_DeadTimeLag=0, //!< Dead category, dead time lag size class moisture
    FuelLifeType_LiveHerb=1,    //!< Live category, live herbaceous moisture
    FuelLifeType_LiveWood=2,    //!< Live category, live woody moisture
    FuelLifeType_DeadLitter=3   //!< Dead category, 100-h time lag moisture
};

double FBL_AspenInterpolate(
            double curing,
            double *valueArray ) ;

double FBL_AspenFuelBedDepth(
            int typeIndex,
            double curing ) ;

double FBL_AspenFuelMextDead(
            int typeIndex,
            double curing ) ;

double FBL_AspenLoadDead1(
            int typeIndex,
            double curing ) ;

double FBL_AspenLoadDead10(
            int typeIndex,
            double curing ) ;

double FBL_AspenLoadLiveHerb(
            int typeIndex,
            double curing ) ;

double FBL_AspenLoadLiveWoody(
            int typeIndex,
            double curing ) ;

double FBL_AspenMortality(
            int severity,
            double flameLength,
            double dbh ) ;

double FBL_AspenSavrDead1(
            int typeIndex,
            double curing ) ;

double FBL_AspenSavrDead10(
            int typeIndex,
            double curing ) ;

double FBL_AspenSavrLiveHerb(
            int typeIndex,
            double curing ) ;

double FBL_AspenSavrLiveWoody(
            int typeIndex,
            double curing ) ;

double FBL_ChaparralDaysSinceMay1(
			int month,
			int day );

double FBL_ChaparralAgeFromDepth(
			double depth,
			bool   isChamise );

double FBL_ChaparralDeadFuelFraction(
			double age,
			bool   isAvereage=true) ;

double FBL_ChaparralDepth(
			double age,
			bool   isChamise );

double FBL_ChaparralHeatLiveLeaf(
			double daysSinceMay1 );

double FBL_ChaparralHeatLiveStem(
			double daysSinceMay1 );

double FBL_ChaparralLiveExtinctionMoisture(
			bool   isChamise );

double FBL_ChaparralLoadDead(
			double totalLoad,
			double deadLoadFraction,
			int    size );

double FBL_ChaparralLoadLive(
			double totalLoad,
			double deadLoadFraction,
			int    size );

double FBL_ChaparralMoistureLiveLeaf(
			double daysSinceMay1 );

double FBL_ChaparralMoistureLiveStem(
			double daysSinceMay1 );

double FBL_ChaparralTotalFuelLoad(
			double age,
			bool   isChamise );

double FBL_CompassConstrainDegrees(
            double degrees ) ;

double FBL_CompassDegreesToRadians(
            double degrees ) ;

double FBL_CompassRadiansToDegrees(
            double radians ) ;

double FBL_CrownFireArea(
            double fireLength,
            double fireWidth ) ;

double FBL_CrownFireActiveRatio(
            double crownSpreadRate,
            double criticalSpreadRate ) ;

double FBL_CrownFireActiveWindSpeed(
		double canopyBulkDensity,
		double reactionIntensity,
		double heatSink,
		double slopeFactor );

double FBL_CrownFireCanopyFractionBurned(
		double ros,
		double rInitiation,
		double rsa );

double FBL_CrownFireCriticalCrownFireSpreadRate(
            double crownBulkDensity ) ;

double FBL_CrownFireCriticalSurfaceFireIntensity(
            double criticalFlameLength ) ;

double FBL_CrownFireCriticalSurfaceFireIntensity(
            double foliarMoisture,
            double crownBaseHt ) ;

double FBL_CrownFireCriticalSurfaceFlameLength(
            double criticalFirelineIntensity ) ;

double FBL_CrownFireCriticalSurfaceFireSpreadRate(
			double criticalSurfaceFireIntensity,
			double surfaceFireHpua ) ;

double FBL_CrownFireFirelineIntensity(
            double crownFireHpua,
            double crownFireRos ) ;

double FBL_CrownFireFirelineIntensityFromFlameLength(
			double crownFireFlameLength );

double FBL_CrownFireFlameLength(
            double crownFirelineIntensity ) ;

double FBL_CrownFireHeatPerUnitArea(
            double surfaceHpua,
            double canopyHpua ) ;

double FBL_CrownFireHeatPerUnitAreaCanopy(
            double crownFuelLoad,
            double lowHeatOfCombustion=8000. ) ;

double FBL_CrownFireLengthToWidthRatio(
            double windSpeedAt20ft ) ;

double FBL_CrownFirePassiveSpreadRate(
            double surfaceFireRos,
			double activeCrownFireRos,
            double canopyFractionBurned ) ;

double FBL_CrownFirePerimeter(
            double fireLength,
            double fireWidth ) ;

double FBL_CrownFirePowerOfFire(
            double crownFirelineIntensity ) ;

double FBL_CrownFirePowerOfWind(
            double windSpeedAt20Ft,
            double crownFireSpreadRate ) ;

double FBL_CrownFirePowerRatio(
            double firePower,
            double windPower ) ;

double FBL_CrownFireSpreadRate(
            double windAt20ft,
            double mc1,
            double mc10,
            double mc100,
            double mcWood ) ;

double FBL_CrownFireTransitionRatio(
            double surfaceFireInt,
            double criticalFireInt ) ;

double FBL_CrownFireWidth(
            double fireLength,
            double lengthWidthRatio ) ;

double FBL_CrownFuelLoad(
            double canopyBulkDensity,
            double canopyHt,
			double baseHt ) ;

double FBL_DewPointTemperature(
            double dryBulb,
            double wetBulb,
            double elev ) ;

int    FBL_FireType(
            double transitionRatio,
            double activeRatio ) ;

int    FBL_FuelLife( int lifeCode ) ;

double FBL_HeatIndex1(
            double at,
            double rh ) ;

double FBL_HeatIndex2(
            double at,
            double rh ) ;

double FBL_HerbaceousFuelLoadCuredFraction( double moistureContent ) ;

double FBL_PalmettoGallberyDead1HrLoad(
            double age,
            double height ) ;

double FBL_PalmettoGallberyDead10HrLoad(
            double age,
            double cover ) ;

double FBL_PalmettoGallberyDeadFoliageLoad(
            double age,
            double cover ) ;

double FBL_PalmettoGallberyFuelBedDepth(
            double height ) ;

double FBL_PalmettoGallberyLitterLoad(
            double age,
            double ba ) ;

double FBL_PalmettoGallberyLive1HrLoad(
            double age,
            double height ) ;

double FBL_PalmettoGallberyLive10HrLoad(
            double age,
            double height ) ;

double FBL_PalmettoGallberyLiveFoliageLoad(
            double age,
            double cover,
            double height ) ;

double FBL_RelativeHumidity(
            double dryBulb,
            double dewPt ) ;

double FBL_SafetyZoneLength(
            double flameHt,
            double pNumb,
            double pArea,
            double eNumb,
            double eArea ) ;

double FBL_SafetyZoneRadius(
            double flameHt,
            double pNumb,
            double pArea,
            double eNumb,
            double eArea ) ;

double FBL_SafetyZoneSeparationDistance(
            double flameHt ) ;

double FBL_SpotCriticalCoverHt(
            double z,
            double coverHt ) ;

double FBL_SpotDistanceFlatTerrain(
            double firebrandHt,
            double coverHt,
            double windSpeedAt20Ft ) ;

double FBL_SpotDistanceFromBurningPile(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
			int    openCanopy,
            double windSpeedAt20Ft,
            double flameHt,
            double *htUsed=0,
            double *firebrandHt=0,
            double *flatDistance=0 );

double FBL_SpotDistanceFromSurfaceFire(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
			int    openCanopy,
            double windSpeedAt20Ft,
            double flameLength,
            double *htUsed=0,
            double *firebrandHt=0,
            double *firebrandDrift=0,
            double *flatDistance=0 );

double FBL_SpotDistanceFromTorchingTrees(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
 			int    openCanopy,
            double windSpeedAt20Ft,
            double torchingTrees,
            double treeDbh,
            double treeHt,
            int    treeSpecies,
            double *htUsed=0,
            double *flameHt=0,
            double *flameRatio=0,
            double *flameDur=0,
            double *firebrandHt=0,
            double *flatDistance=0 );

double FBL_SpotDistanceMountainTerrain(
            double flatDist,
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev ) ;

double FBL_SummerSimmerIndex(
            double airTemperature,
            double relativeHumidity ) ;

double FBL_SurfaceFireArea(
            double fireLength,
            double fireWidth ) ;

double FBL_SurfaceFireBackingSpreadRate(
            double headSpreadRate,
            double eccentricity ) ;

double FBL_SurfaceFireEccentricity(
            double lengthWidthRatio ) ;

double FBL_SurfaceFireEffectiveWindSpeedAtVector(
            double noWindSpreadRate,
            double vectorSpreadRate ) ;

double FBL_SurfaceFireEllipseBetaFromThetaDegrees(
			double f,
			double g,
			double h,
			double thetaDegrees );

double FBL_SurfaceFireEllipseBetaFromThetaRadians(
			double f,
			double g,
			double h,
			double thetaRadians );

double FBL_SurfaceFireEllipseF(
            double fireLength ) ;

double FBL_SurfaceFireEllipseG(
            double fireLength,
            double backingDist ) ;

double FBL_SurfaceFireEllipseH(
            double fireWidth ) ;

double FBL_SurfaceFireEllipsePsiFromThetaDegrees(
            double f,
            double h,
            double thetaDegrees ) ;

double FBL_SurfaceFireEllipsePsiFromThetaRadians(
            double f,
            double h,
            double thetaRadians ) ;

double FBL_SurfaceFireEllipseThetaFromBetaDegrees(
            double f,
            double g,
            double h,
            double betaDegrees ) ;

double FBL_SurfaceFireEllipseThetaFromBetaRadians(
            double f,
            double g,
            double h,
            double betaRadians ) ;

double FBL_SurfaceFireEllipseThetaFromPsiDegrees(
            double f,
            double h,
            double psiDegrees ) ;

double FBL_SurfaceFireEllipseThetaFromPsiRadians(
            double f,
            double h,
            double psiRadians ) ;

double FBL_SurfaceFireExpansionRateAtBeta(
            double f,
            double g,
            double h,
            double elapsedTime,
            double betaDegrees ) ;

double FBL_SurfaceFireExpansionRateAtPsi(
            double f,
            double g,
            double h,
            double elapsedTime,
            double psiDegrees ) ;

double FBL_SurfaceFireExpansionRateAtTheta(
            double f,
            double g,
            double h,
            double elapsedTime,
            double psiDegrees ) ;

double FBL_SurfaceFireExpectedSpreadRate(
            double *ros,
            double *cov,
            int     fuels,
            double  lbRatio,
            int     samples=3,
            int     depth=3,
            int     laterals=0,
            double *harmonicRos=0 ) ;

double FBL_SurfaceFireFirebrandIgnitionProbability(
            double fuelTemperature,
            double fuelMoisture ) ;

double FBL_SurfaceFireFirelineIntensity(
            double spreadRate,
            double reactionIntensity,
            double residenceTime ) ;

double FBL_SurfaceFireFirelineIntensity(
            double flameLength ) ;

double FBL_SurfaceFireFirelineIntensityAtPsi(
			double ros_psi,
			double ros_beta,
			double fli_beta );

double FBL_SurfaceFireFlameLength(
            double firelineIntensity ) ;

double FBL_SurfaceFireForwardSpreadRate(
            double noWindNoSlopeSpreadRate,
            double reactionIntensity,
            double slopeFraction,
            double midflameWindSpeed,
            double windDirFromUpslope,
            double *maxDirFromUpslope,
            double *effectiveWindSpeed,
            double *windSpeedLimit,
            int    *windLimitExceeded,
            double *windFactor,
            double *slopeFactor,
            bool    applyWindLimit=true ) ;

double FBL_SurfaceFireHeatPerUnitArea(
            double reactionIntensity,
            double residenceTime ) ;

double FBL_SurfaceFireHeatSource(
            double spreadRateAtHead,
            double heatSink ) ;

double FBL_SurfaceFireLengthToWidthRatio(
            double effectiveWindSpeed ) ;

double FBL_SurfaceFireLightningIgnitionProbability(
            int    fuelType,
            double depth,
            double moisture,
            int    charge ) ;

double FBL_SurfaceFireNoWindNoSlopeSpreadRate(
            double reactionIntensity,
            double propagatingFlux,
            double heatSink ) ;

double FBL_SurfaceFirePerimeter(
            double fireLength,
            double fireWidth ) ;

double FBL_SurfaceFirePropagatingFlux(
            double fuelBedPackingRatio,
            double fuelBedSavr ) ;

double FBL_SurfaceFireReactionIntensity(
            double deadMois,
            double deadMext,
            double liveMois,
            double liveMext,
            double* deadRxInt=0,
            double* liveRxInt=0 ) ;

double FBL_SurfaceFireResidenceTime(
            double fuelBedSavr ) ;

double FBL_SurfaceFireScorchHeight(
            double firelineIntensity,
            double windSpeed,
            double airTemperature ) ;

double FBL_SurfaceFireSpreadRateAtBeta(
            double forwardSpreadRate,
            double eccentricity,
            double beta ) ;

double FBL_SurfaceFireVectorBeta(
            double maxSpreadDirFromUpslope,
            double vectorDirFromUpslope ) ;

double FBL_SurfaceFireVectorSpreadRate(
            double forwardSpreadRate,
            double maxSpreadDirFromUpslope,
            double eccentricity,
            double vectorDirFromUpslope ) ;

double FBL_SurfaceFireWidth(
            double fireLength,
            double lengthWidthRatio ) ;

double FBL_SurfaceFuelBedHeatSink(
            double bulkDensity,
            double deadMext,
            double *mois,
            double *deadFuelMois,
            double *liveFuelMois,
            double *liveFuelMext ) ;

double FBL_SurfaceFuelBedIntermediates(
            double depth,
            double deadMext,
            int    particles,
            int    *life,
            double *load,
            double *savr,
            double *heat,
            double *dens,
            double *stot,
            double *seff,
            double *fuelBedBulkDensity,
            double *fuelBedPackingRatio,
            double *fuelBedBetaRatio ) ;

double FBL_SurfaceFuelTemperature(
            double airTemperature,
            double sunShading ) ;

double FBL_TreeBarkThicknessBehave(
            int    speciesIndex,
            double dbh ) ;

double FBL_TreeBarkThicknessFofem(
            int    speciesIndex,
            double dbh ) ;

double FBL_TreeCrownBaseHeight(
            double crownRatio,
            double treeHt ) ;

double FBL_TreeCrownRatio(
            double baseHt,
            double treeHt ) ;

double FBL_TreeCrownScorch(
            double treeHt,
            double crownRatio,
            double scorchHt,
            double *crownLengthScorched,
            double *crownLengthFractionScorched ) ;

double FBL_TreeMortalityBehave(
            double barkThickness,
            double scorchHt,
            double crownVolScorched ) ;

double FBL_TreeMortalityFofem(
            int    speciesIndex,
            double barkThickness,
            double crownVolScorched,
            double scorchHt ) ;

double FBL_TreeMortalityHood(
            int speciesIndex,
            double dbh,
            double crownLengScorched,
            double crownVolScorched ) ;

double FBL_WindAdjustmentFactor(
            double canopyCover,
            double canopyHt,
            double crownRatio,
            double fuelDepth,
            double *fraction,
            int *method ) ;

double FBL_WindChillTemperature(
            double airTemperature,
            double windSpeed ) ;

double FBL_WindSpeedAt20Ft(
            double windSpeedAt10M ) ;

#endif

//------------------------------------------------------------------------------
//  End of xfblib.h
//------------------------------------------------------------------------------

