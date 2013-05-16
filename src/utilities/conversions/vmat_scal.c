/****************************************************************************
*   File: vmat_scal.c                                                         *
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
*       Contents: Code to multiply by scalars                               *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

void vmat2_scal_vec_mul32(s, v, r)
    float32 s, v[2], r[2];
{   VMAT2_SCAL_VEC_MUL(s, v, r);  }

void vmat3_scal_vec_mul32(s, v, r)
    float32 s, v[3], r[3];
{   VMAT3_SCAL_VEC_MUL(s, v, r);  }

void vmat2_scal_vec_mul64(s, v, r)
    float64 s, v[2], r[2];
{   VMAT2_SCAL_VEC_MUL(s, v, r);  }

void vmat3_scal_vec_mul64(s, v, r)
    float64 s, v[3], r[3];
{   VMAT3_SCAL_VEC_MUL(s, v, r);  }

void vmat2e_scal_vec_mul32(s, vx, vy, r)
    float32 s, vx, vy, r[2];
{   VMAT2E_SCAL_VEC_MUL(s, vx, vy, r);  }

void vmat3e_scal_vec_mul32(s, vx, vy, vz, r)
    float32 s, vx, vy, vz, r[3];
{   VMAT3E_SCAL_VEC_MUL(s, vx, vy, vz, r);  }

void vmat2e_scal_vec_mul64(s, vx, vy, r)
    float64 s, vx, vy, r[2];
{   VMAT2E_SCAL_VEC_MUL(s, vx, vy, r);  }

void vmat3e_scal_vec_mul64(s, vx, vy, vz, r)
    float64 s, vx, vy, vz, r[3];
{   VMAT3E_SCAL_VEC_MUL(s, vx, vy, vz, r);  }

void vmat2_scal_mat_mul32(s, m, r)
    float32 s, m[2][2], r[2][2];
{
    r[X][X] = s * m[X][X];
    r[X][Y] = s * m[X][Y];
    r[Y][X] = s * m[X][X];
    r[Y][Y] = s * m[Y][Y];
}

void vmat3_scal_mat_mul32(s, m, r)
    float32 s, m[3][3], r[3][3];
{
    r[X][X] = s * m[X][X];
    r[X][Y] = s * m[X][Y];
    r[X][Z] = s * m[X][Z];
    r[Y][X] = s * m[Y][X];
    r[Y][Y] = s * m[Y][Y];
    r[Y][Z] = s * m[Y][Z];
    r[Z][X] = s * m[Z][X];
    r[Z][Y] = s * m[Z][Y];
    r[Z][Z] = s * m[Z][Z];
}

void vmat2_scal_mat_mul64(s, m, r)
    float64 s, m[2][2], r[2][2];
{
    r[X][X] = s * m[X][X];
    r[X][Y] = s * m[X][Y];
    r[Y][X] = s * m[X][X];
    r[Y][Y] = s * m[Y][Y];
}

void vmat3_scal_mat_mul64(s, m, r)
    float64 s, m[3][3], r[3][3];
{
    r[X][X] = s * m[X][X];
    r[X][Y] = s * m[X][Y];
    r[X][Z] = s * m[X][Z];
    r[Y][X] = s * m[Y][X];
    r[Y][Y] = s * m[Y][Y];
    r[Y][Z] = s * m[Y][Z];
    r[Z][X] = s * m[Z][X];
    r[Z][Y] = s * m[Z][Y];
    r[Z][Z] = s * m[Z][Z];
}
