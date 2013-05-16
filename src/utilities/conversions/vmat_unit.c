/****************************************************************************
*   File: vmat_unit.c                                                         *
*                                                                           *
*       Copyright 1993 by Loral Advanced Distributed Simulation, Inc.       *
*                                                                           *
*               Loral Advanced Distributed Simulation, Inc.                 *
*               10 Moulton Street                                           *
*               Cambridge, MA 02238                                         *
*               617-873-1850                                                *
*                                                                           *
*       This software was developed by Loral under U. S. Government contracts *
*       and may be reproduced by or for the U. S. Government pursuant to    *
*       the copyright license under the clause at DFARS 252.227-7013        *
*       (OCT 1988).                                                         *
*                                                                           *
*       Contents: Code to make a unit vector                                *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.4 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define TWO_DIMENSIONAL(type,vx,vy,r)					\
{									\
    type mag;								\
									\
    mag = vx*vx + vy*vy;						\
    if (mag == 0.0)							\
      r[X] = 1.0, r[Y] = 0.0;						\
    else								\
    {									\
	mag = 1.0/sqrt(mag);						\
	r[X] = vx * mag;						\
	r[Y] = vy * mag;						\
    }									\
}

#define THREE_DIMENSIONAL(type,vx,vy,vz,r)				\
{									\
    type mag;								\
									\
    mag = vx*vx + vy*vy + vz*vz;					\
    if (mag == 0.0)							\
      r[X] = 1.0, r[Y] = 0.0, r[Z] = 0.0;				\
    else								\
    {									\
	mag = 1.0/sqrt(mag);						\
	r[X] = vx * mag;						\
	r[Y] = vy * mag;						\
	r[Z] = vz * mag;						\
    }									\
}

void vmat2_unit32(v, r)
    float32 v[2], r[2];
TWO_DIMENSIONAL(float32, v[X], v[Y], r)

void vmat3_unit32(v,r)
    float32 v[3], r[3];
THREE_DIMENSIONAL(float32, v[X], v[Y], v[Z], r)

void vmat2_unit64(v, r)
    float64 v[2], r[2];
TWO_DIMENSIONAL(float64, v[X], v[Y], r)

void vmat3_unit64(v,r)
    float64 v[3], r[3];
THREE_DIMENSIONAL(float64, v[X], v[Y], v[Z], r)

void vmat2e_unit32(vx, vy, r)
    float32 vx, vy, r[2];
TWO_DIMENSIONAL(float32, vx, vy, r)

void vmat3e_unit32(vx, vy, vz, r)
    float32 vx, vy, vz, r[3];
THREE_DIMENSIONAL(float32, vx, vy, vz, r)

void vmat2e_unit64(vx, vy, r)
    float64 vx, vy, r[2];
TWO_DIMENSIONAL(float64, vx, vy, r)

void vmat3e_unit64(vx, vy, vz, r)
    float64 vx, vy, vz, r[3];
THREE_DIMENSIONAL(float64, vx, vy, vz, r)


