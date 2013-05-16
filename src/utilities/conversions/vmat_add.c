/****************************************************************************
*   File: vmat_add.c                                                          *
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
*       Contents: Code to add/subtract vectors                              *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define TWO_DIMENSIONAL(op, wx, wy, vx, vy, r)				\
{									\
    r[X] = wx op vx;							\
    r[Y] = wy op vy;							\
}

#define THREE_DIMENSIONAL(op, wx, wy, wz, vx, vy, vz, r)		\
{									\
    r[X] = wx op vx;							\
    r[Y] = wy op vy;							\
    r[Z] = wz op vz;							\
}

void vmat2_vec_add32(w, v, r)
    float32 w[2], v[2], r[2];
TWO_DIMENSIONAL(+, w[X], w[Y], v[X], v[Y], r)

void vmat3_vec_add32(w, v, r)
    float32 w[3], v[3], r[3];
THREE_DIMENSIONAL(+, w[X], w[Y], w[Z], v[X], v[Y], v[Z], r)

void vmat2_vec_add64(w, v, r)
    float64 w[2], v[2], r[2];
TWO_DIMENSIONAL(+, w[X], w[Y], v[X], v[Y], r)

void vmat3_vec_add64(w, v, r)
    float64 w[3], v[3], r[3];
THREE_DIMENSIONAL(+, w[X], w[Y], w[Z], v[X], v[Y], v[Z], r)

void vmat2e_vec_add32(wx, wy, vx, vy, r)
    float32 wx, wy, vx, vy, r[2];
TWO_DIMENSIONAL(+, wx, wy, vx, vy, r)

void vmat3e_vec_add32(wx, wy, wz, vx, vy, vz, r)
    float32 wx, wy, wz, vx, vy, vz, r[3];
THREE_DIMENSIONAL(+, wx, wy, wz, vx, vy, vz, r)

void vmat2e_vec_add64(wx, wy, vx, vy, r)
    float64 wx, wy, vx, vy, r[2];
TWO_DIMENSIONAL(+, wx, wy, vx, vy, r)

void vmat3e_vec_add64(wx, wy, wz, vx, vy, vz, r)
    float64 wx, wy, wz, vx, vy, vz, r[3];
THREE_DIMENSIONAL(+, wx, wy, wz, vx, vy, vz, r)

void vmat2_vec_sub32(w, v, r)
    float32 w[2], v[2], r[2];
TWO_DIMENSIONAL(-, w[X], w[Y], v[X], v[Y], r)

void vmat3_vec_sub32(w, v, r)
    float32 w[3], v[3], r[3];
THREE_DIMENSIONAL(-, w[X], w[Y], w[Z], v[X], v[Y], v[Z], r)

void vmat2_vec_sub64(w, v, r)
    float64 w[2], v[2], r[2];
TWO_DIMENSIONAL(-, w[X], w[Y], v[X], v[Y], r)

void vmat3_vec_sub64(w, v, r)
    float64 w[3], v[3], r[3];
THREE_DIMENSIONAL(-, w[X], w[Y], w[Z], v[X], v[Y], v[Z], r)

void vmat2e_vec_sub32(wx, wy, vx, vy, r)
    float32 wx, wy, vx, vy, r[2];
TWO_DIMENSIONAL(-, wx, wy, vx, vy, r)

void vmat3e_vec_sub32(wx, wy, wz, vx, vy, vz, r)
    float32 wx, wy, wz, vx, vy, vz, r[3];
THREE_DIMENSIONAL(-, wx, wy, wz, vx, vy, vz, r)

void vmat2e_vec_sub64(wx, wy, vx, vy, r)
    float64 wx, wy, vx, vy, r[2];
TWO_DIMENSIONAL(-, wx, wy, vx, vy, r)

void vmat3e_vec_sub64(wx, wy, wz, vx, vy, vz, r)
    float64 wx, wy, wz, vx, vy, vz, r[3];
THREE_DIMENSIONAL(-, wx, wy, wz, vx, vy, vz, r)
