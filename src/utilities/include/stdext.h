/*
 * $RCSfile: stdext.h,v $ $Revision: 1.14 $ $State: Exp $
 */
/****************************************************************************
*   File: stdext.h                                                          *
*                                                                           *
*       Copyright 1990 by BBN Systems and Technology Corporation            *
*                                                                           *
*               BBN Systems and Technology Corporation                      *
*               10 Moulton Street                                           *
*               Cambridge, MA 02238                                         *
*               617-873-1850                                                *
*                                                                           *
*       This software was developed under U.S. Government                   *
*       contract MDA903-86-C-0309.  Use, duplication and                    *
*       distribution of this software is subject to the                     *
*       provisions of DFARS 52.227-7013                                     *
*                                                                           *
*       Contents: Common macros/constants used everywhere                   *
*       Created: Fri May 29 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

/* For the sake of program clarity: Please do not include this file in
 * public header files.  There should be no need to do so.  Including this
 * in individual C files should help keep it obvious where these definitions
 * are coming from.
 */

#ifndef _STDEXT_H_INCLUDED
#define _STDEXT_H_INCLUDED

#include <stdtypes.h> /*common/include/global*/

/* The constants and macros defined here are intentionally left without
 * wrapping #ifdef's so that cpp will tell you if there are redefinitions
 * (except around integer definitions, which are done in such a way
 * that useless cpp errors will be suppressed).
 */


/*
 * ARRAYLENGTH returns the number of entries in any array.
 */
#define ARRAYLENGTH(a)    (sizeof(a)/sizeof(a[0]))



/**
 ** Physical constants.
 **/


#define PI 3.14159265358979323844

#define PI_OVER_TWO 1.57079632679489661922

#define TWO_PI 6.28318530717958647688

#define SPEED_OF_SOUND_MPS 331.3

#define EARTH_RADIUS_EQUATOR 6378137.0 /*meters*/

#define EARTH_RADIUS_POLAR 6356752.0 /*meters*/


#define GRAVITY_SEA_LEVEL 9.7988 /*meters/sec/sec*/

/**
 ** Programming conventions.
 **/

/* The funny #if structure will cause cpp to generate an error only
 * if the definitions are actually different.
 */

#if !defined(TRUE) || (TRUE != 1)
#define TRUE 1
#endif

#if !defined(FALSE) || (FALSE != 0)
#define FALSE 0
#endif

/*
 * We don't do this trick for NULL because it seems to make some compilers
 * complain.
 */
#if !defined(NULL)
#define NULL 0
#endif

/* Vector offsets. */
#if !defined(X) || (X != 0)
#define X 0
#endif

#if !defined(Y) || (Y != 1)
#define Y 1
#endif

#if !defined(Z) || (Z != 2)
#define Z 2
#endif

/* Argument types for varargs lists. */

#define A_END    0 
#define A_INT    1
#define A_DOUBLE 2

/* Note: the next four will be promoted to an int or a double on most
 * machines, feel free to #ifdef these if this is not the case for some
 * hardware/compiler.
 */

#define A_CHAR   1
#define A_SHORT  1
#define A_FLOAT  2
#define A_PTR    1

/**
 ** Conversions.
 **/

#define GALS_PER_LITER (7.481/28.32)
#define LITERS_PER_GAL (28.32/7.481)

#define DEG_TO_RAD(x) ((x) * PI / 180.0)

#define RAD_TO_DEG(x) ((x) * 180.0 / PI)

#define MIL_TO_RAD(x) ((x) * 0.05625 * PI / 180.0)

#define RAD_TO_MIL(x) ((x) * 180.0 * 17.777777778 / PI)

#define DEG_TO_MIL(x) ((x) * 6400.0 / 360.0)

#define MIL_TO_DEG(x) ((x) * 360.0 / 6400.0)

#define MPS_TO_KPH(x) ((x) * 3600.0 / 1000.0)

#define KPH_TO_MPS(x) ((x) * 1000.0 / 3600.0)

#define GALS_TO_LITERS(x) ((x) * LITERS_PER_GAL)

#define LITERS_TO_GALS(x) ((x) * GALS_PER_LITER)

#define MPS_TO_KTS(x) ((x) * 1.944)

#define KTS_TO_MPS(x) ((x) / 1.944)

/* Note that this constant used to be 1916.67, but it was officially
 * changed many years ago.  (Fun fact!)
 */
#define M_TO_NMI(x) ((x) / 1852.0)

#define NMI_TO_M(x) ((x) * 1852.0)

#define M_TO_FT(x) ((x) / 0.3048)

#define FT_TO_M(x) ((x) * 0.3048)

/* These three are defined this way to work on machines which cannot
 * cast numbers bigger 0x80000000 to doubles (namely, Mips).
 */

#define BAM_TO_RADIANS_FACTOR  (TWO_PI / 2147483647.0)

#define BAM_TO_RAD(x) ((double)((x) * 0.5 * BAM_TO_RADIANS_FACTOR))

#define RAD_TO_BAM(x) ((unsigned int)((x) / BAM_TO_RADIANS_FACTOR) << 1)

/**
 ** Comparitors (NOT SAFE, REFER TO ARGS MORE THAN ONCE!).
 **/

/* These two are defined this way to avoid dependencies and be alignment
 * safe...
 */
#ifndef NS_SIM_ADDRESSES_EQUAL
#define NS_SIM_ADDRESSES_EQUAL(x, y) \
  ((((uint16 *)(&(x)))[0] == ((uint16 *)(&(y)))[0]) && \
   (((uint16 *)(&(x)))[1] == ((uint16 *)(&(y)))[1]))
#endif

#ifndef NS_VEHICLE_IDS_EQUAL
#define NS_VEHICLE_IDS_EQUAL(x, y) \
  ((((uint16 *)(&(x)))[0] == ((uint16 *)(&(y)))[0]) && \
   (((uint16 *)(&(x)))[1] == ((uint16 *)(&(y)))[1]) && \
   (((uint16 *)(&(x)))[2] == ((uint16 *)(&(y)))[2]))
#endif

#ifndef NS_OBJECT_ID_NULL
#define NS_OBJECT_ID_NULL(x) \
  (!((uint16 *)(&(x)))[0] && !((uint16 *)(&(x)))[1] && !((uint16 *)(&(x)))[2])
#endif

#ifndef NS_OBJECT_IDS_EQUAL
#define NS_OBJECT_IDS_EQUAL NS_VEHICLE_IDS_EQUAL
#endif

#define NS_MAX(a, b) ((a) > (b) ? (a) : (b))

#define NS_MIN(a, b) ((a) < (b) ? (a) : (b))

#define NS_LIMITS(min, x, max) \
  ((x) < (min) ? (min) : ((x) > (max)) ? (max) : (x))

#define NS_BOUND(x, max) (NS_LIMITS((-max), x, max))

#endif /*_STDEXT_H_INCLUDED*/
