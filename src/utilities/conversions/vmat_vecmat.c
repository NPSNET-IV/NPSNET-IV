/****************************************************************************
*   File: vmat_vecmat.c                                                       *
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
*       Contents: Code for vec*mat operations                               *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

/* Note that we have to use local variables, since the caller might
 * pass the same vector as both v and r.
 */

#define TWO_DIMENSIONAL(type, vx, vy, m, r)				\
{									\
    type rx, ry;							\
									\
    rx = vx * (m)SUB(X,X) + vy * (m)SUB(Y,X);				\
    ry = vx * (m)SUB(X,Y) + vy * (m)SUB(Y,Y);				\
    r[X] = rx;								\
    r[Y] = ry;								\
}

#define THREE_DIMENSIONAL(type, vx, vy, vz, m, r)			\
{									\
    type rx, ry, rz;							\
									\
    rx = vx * (m)SUB(X,X) + vy * (m)SUB(Y,X) + vz * (m)SUB(Z,X);	\
    ry = vx * (m)SUB(X,Y) + vy * (m)SUB(Y,Y) + vz * (m)SUB(Z,Y);	\
    rz = vx * (m)SUB(X,Z) + vy * (m)SUB(Y,Z) + vz * (m)SUB(Z,Z);	\
    r[X] = rx;								\
    r[Y] = ry;								\
    r[Z] = rz;								\
}

#define SUB(a,b) [a][b]

void vmat2_vec_mat_mul32(v, m, r)
    float32 v[2], m[2][2], r[2];
TWO_DIMENSIONAL(float32, v[X], v[Y], m, r)

void vmat3_vec_mat_mul32(v, m, r)
    float32 v[3], m[3][3], r[3];
THREE_DIMENSIONAL(float32, v[X], v[Y], v[Z], m, r)

void vmat2_vec_mat_mul64(v, m, r)
    float64 v[2], m[2][2], r[2];
TWO_DIMENSIONAL(float64, v[X], v[Y], m, r)

void vmat3_vec_mat_mul64(v, m, r)
    float64 v[3], m[3][3], r[3];
THREE_DIMENSIONAL(float64, v[X], v[Y], v[Z], m, r)

void vmat2e_vec_mat_mul32(vx, vy, m, r)
    float32 vx, vy, m[2][2], r[2];
TWO_DIMENSIONAL(float32, vx, vy, m, r)

void vmat3e_vec_mat_mul32(vx, vy, vz, m, r)
    float32 vx, vy, vz, m[3][3], r[3];
THREE_DIMENSIONAL(float32, vx, vy, vz, m, r)

void vmat2e_vec_mat_mul64(vx, vy, m, r)
    float64 vx, vy, m[2][2], r[2];
TWO_DIMENSIONAL(float64, vx, vy, m, r)

void vmat3e_vec_mat_mul64(vx, vy, vz, m, r)
    float64 vx, vy, vz, m[3][3], r[3];
THREE_DIMENSIONAL(float64, vx, vy, vz, m, r)

#undef SUB
#define SUB(a,b) [b][a]

void vmat2_mat_vec_mul32(m, v, r)
    float32 m[2][2], v[2], r[2];
TWO_DIMENSIONAL(float32, v[X], v[Y], m, r)

void vmat3_mat_vec_mul32(m, v, r)
    float32 m[3][3], v[3], r[3];
THREE_DIMENSIONAL(float32, v[X], v[Y], v[Z], m, r)

void vmat2_mat_vec_mul64(m, v, r)
    float64 m[2][2], v[2], r[2];
TWO_DIMENSIONAL(float64, v[X], v[Y], m, r)

void vmat3_mat_vec_mul64(m, v, r)
    float64 m[3][3], v[3], r[3];
THREE_DIMENSIONAL(float64, v[X], v[Y], v[Z], m, r)

void vmat2e_mat_vec_mul32(m, vx, vy, r)
    float32 m[2][2], vx, vy, r[2];
TWO_DIMENSIONAL(float32, vx, vy, m, r)

void vmat3e_mat_vec_mul32(m, vx, vy, vz, r)
    float32 m[3][3], vx, vy, vz, r[3];
THREE_DIMENSIONAL(float32, vx, vy, vz, m, r)

void vmat2e_mat_vec_mul64(m, vx, vy, r)
    float64 m[2][2], vx, vy, r[2];
TWO_DIMENSIONAL(float64, vx, vy, m, r)

void vmat3e_mat_vec_mul64(m, vx, vy, vz, r)
    float64 m[3][3], vx, vy, vz, r[3];
THREE_DIMENSIONAL(float64, vx, vy, vz, m, r)
