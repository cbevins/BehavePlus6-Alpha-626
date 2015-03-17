//------------------------------------------------------------------------------
/*! \file randfuel.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Part of Mark Finney's EXRATE package for determining expected
 *  and harmonic mean spread rate in randomly arranged fuels.
 *
 *  Original code by Mark Finney.  CDB has renamed all functions and
 *  variables according to BehavePlus3 coding style.  The following files
 *  make up the entire code package:
 *  - newext.cpp
 *  - newext.h
 *  - randfuel.cpp
 *  - randfuel.h
 *  - randthread.cpp
 *  - randthread.h
 */

#ifndef _RANDFUEL_H_
#define _RANDFUEL_H_

// Custom include files
#include "newext.h"
#include "randthread.h"

//------------------------------------------------------------------------------
/*! \typedef FuelType
 *  \brief Contains fuel types and their properties (RandFuel)
  */

typedef struct
{
    double  m_relRos;   //!< relative spread rate 0-1
    double  m_absRos;   //!< actual spread rate
    double  m_fract;    //!< fraction of landscape occupied
} FuelType;

//------------------------------------------------------------------------------
/*! \class Randfuel randfuel.h
 */

class RandFuel
{
// Public methods
public:
    RandFuel();
    ~RandFuel();
    bool    allocFuels( long p_fuels );
    bool    calcCombinations( long p_nX, long p_nY, long *p_nT,
                double ***p_ca, double ***p_ra ) ;
    void    calcExtendedSpreadRates2( long p_cols, long p_rows,
                long p_latCombs, double **p_combArray, double **p_rosArray,
                double *p_latRosArray, double *p_maxRosExtArray, long p_laterals ) ;
    double  computeSpread2( long p_samples, long p_depths, double p_lbRatio,
                long p_threads, double *p_maxRos, double *p_harmonicRos,
                long p_exts, long p_lessIgns );
    void    freeFuels( void );
    double  recomputeSpread( double *p_harmonicRos );
    void    setCellDimensions( double p_cellSize );
    void    setFuelData( long p_type, double p_ros, double p_fract ) ;
    void    spliceExtensions2(double *p_ca, double *p_ra, double ***p_cs,
                double ***p_rs, long p_oldCols ) ;

// Private methods
private:
    bool    allocRandThreads( void ) ;
    void    calcSpreadRates( void ) ;
    void    closeRandThreads( void ) ;
    void    freeBlockArrays( void ) ;
    void    init( void ) ;

// Private data
private:
    long        m_samples;          //!< number of cols in block
    long        m_depths;           //!< number rows in block
    long        m_fuels;            //!< number of fuel types
    long        m_combs;            //!< number of combinations in block
    long        m_exts;             //!< number of combinations in extension
    long        m_threads;          //!< number of threads allocated (<64)
    long        m_lessIgns;         //!< number of ignition points FEWER than NumSamples;
    double      m_lbRatio;          //!< length to breadth ratio of fire
    double      m_cellSize;         //!< size of raster cell
    double    **m_combArray;        //!< array of block probabilities
    double    **m_rosArray;         //!< array of spread rates in block
    double    **m_combExtArray;     //!< lateral extension array of prob
    double    **m_rosExtArray;      //!< lateral extension array of ros
    double     *m_maxRosArray;      //!< max spread rate for all blocks
    double     *m_maxRosExtArray;   //!< max spread rate for all blocks in extension
    FuelType   *m_fuelTypeArray;    //!< array of FuelType structs
    RandThread *m_randThread;       //!< array of RandThread classes=m_threads
};

#endif // RANDFUELHEADER

//------------------------------------------------------------------------------
//  End of randfuel.h
//------------------------------------------------------------------------------

