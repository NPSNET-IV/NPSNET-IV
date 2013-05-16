/*
 * $RCSfile: stdtypes.h,v $ $Revision: 1.4 $ $State: Exp $
 */
/****************************************************************************
*   File: stdtypes.h                                                        *
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
*       Contents: Common "machine independent" type definitions             *
*       Created: Fri May 29 1992                                            *
*       Author: jesmith                                                     *
*       Remarks: Use #ifdefs to customize for new hardware.                 *
*                                                                           *
****************************************************************************/

#ifndef _STDTYPES_H_INCLUDED
#define _STDTYPES_H_INCLUDED

/* Machine dependent native (useful for mixed K&R and ANSI parameters) 
   "and fastest" integer to use.
 */
typedef int FAST_INT;               /* here for backward compatibility */
typedef int NATIVE_INT;
typedef unsigned int NATIVE_UINT;

/* Machine dependent "fastest" floating point to use.
 */
typedef double FAST_REAL;

/* Generic address.  void *, caddr_t, and other such definitions are
 * not fully portable.  It is best to use void * if the compiler supports
 * it.
 */
#if __STDC__ == 1 || (defined(mips) && !defined(sony))
typedef void * ADDRESS;
#else
typedef char * ADDRESS;
#endif

/* Generic numeric types.
 * U means unsigned, the trailing digit is bit length.
 */

#if __STDC__ == 1 || defined(sgi) || defined(AIXV3)
typedef signed char    int8;
#else
typedef char           int8;
#endif

typedef short          int16;

typedef long           int32;

typedef unsigned char  uint8;

typedef unsigned short uint16;

typedef unsigned long  uint32;

typedef float          float32;

typedef double         float64;

#endif
