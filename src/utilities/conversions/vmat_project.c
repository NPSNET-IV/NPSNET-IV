/****************************************************************************
*   File: vmat_project.c                                                    *
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
*       Contents: Code to compute projections of vectors                    *
*       Created: Wed Aug 11 1993                                            *
*       Author: mlongtin                                                    *
*      $Revision: 1.1 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

void vmat2_project32(w, v, r)
    float32 w[2], v[2], r[2];
{
    float32 lambda;

    lambda = VMAT2_DOT_PROD(w, v) / VMAT2_DOT_PROD(v, v);
    VMAT2_SCAL_VEC_MUL(lambda, v, r);
}

void vmat3_project32(w, v, r)
    float32 w[3], v[3], r[3];
{
    float32 lambda;

    lambda = VMAT3_DOT_PROD(w, v) / VMAT3_DOT_PROD(v, v);
    VMAT3_SCAL_VEC_MUL(lambda, v, r);
}

void vmat2_project64(w, v, r)
    float64 w[2], v[2], r[2];
{
    float64 lambda;

    lambda = VMAT2_DOT_PROD(w, v) / VMAT2_DOT_PROD(v, v);
    VMAT2_SCAL_VEC_MUL(lambda, v, r);
}

void vmat3_project64(w, v, r)
    float64 w[3], v[3], r[3];
{
    float64 lambda;

    lambda = VMAT3_DOT_PROD(w, v) / VMAT3_DOT_PROD(v, v);
    VMAT3_SCAL_VEC_MUL(lambda, v, r);
}

void vmat2e_project32(wx, wy, vx, vy, r)
    float32 wx, wy, vx, vy, r[2];
{
    float32 lambda;

    lambda = VMAT2E_DOT_PROD(wx, wy, vx, vy) / VMAT2E_DOT_PROD(vx, vy, vx, vy);
    VMAT2E_SCAL_VEC_MUL(lambda, vx, vy, r);
}

void vmat3e_project32(wx, wy, wz, vx, vy, vz, r)
    float32 wx, wy, wz, vx, vy, vz, r[3];
{
    float32 lambda;

    lambda = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz) /
             VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz);
    VMAT3E_SCAL_VEC_MUL(lambda, vx, vy, vz, r);
}

void vmat2e_project64(wx, wy, vx, vy, r)
    float64 wx, wy, vx, vy, r[2];
{
    float64 lambda;

    lambda = VMAT2E_DOT_PROD(wx, wy, vx, vy) / VMAT2E_DOT_PROD(vx, vy, vx, vy);
    VMAT2E_SCAL_VEC_MUL(lambda, vx, vy, r);
}

void vmat3e_project64(wx, wy, wz, vx, vy, vz, r)
    float64 wx, wy, wz, vx, vy, vz, r[3];
{
    float64 lambda;

    lambda = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz) /
             VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz);
    VMAT3E_SCAL_VEC_MUL(lambda, vx, vy, vz, r);
}

void vmat2_project_perp32(w, v, r)
    float32 w[2], v[2], r[2];
{
    float32 tmp[2];

    vmat2_project32(w, v, tmp);
    VMAT2_VEC_SUB(w, tmp, r);
}

void vmat3_project_perp32(w, v, r)
    float32 w[3], v[3], r[3];
{
    float32 tmp[3];

    vmat3_project32(w, v, tmp);
    VMAT3_VEC_SUB(w, tmp, r);
}

void vmat2_project_perp64(w, v, r)
    float64 w[2], v[2], r[2];
{
    float64 tmp[2];

    vmat2_project64(w, v, tmp);
    VMAT2_VEC_SUB(w, tmp, r);
}

void vmat3_project_perp64(w, v, r)
    float64 w[3], v[3], r[3];
{
    float64 tmp[3];

    vmat3_project64(w, v, tmp);
    VMAT3_VEC_SUB(w, tmp, r);
}

void vmat2e_project_perp32(wx, wy, vx, vy, r)
    float32 wx, wy, vx, vy, r[2];
{
    float32 tmp[2];

    vmat2e_project32(wx, wy, vx, vy, tmp);
    VMAT2E_VEC_SUB(wx, wy, tmp[X], tmp[Y], r);
}

void vmat3e_project_perp32(wx, wy, wz, vx, vy, vz, r)
    float32 wx, wy, wz, vx, vy, vz, r[3];
{
    float32 tmp[3];

    vmat3e_project32(wx, wy, wz, vx, vy, vz, tmp);
    VMAT3E_VEC_SUB(wx, wy, wz, tmp[X], tmp[Y], tmp[Z], r);
}

void vmat2e_project_perp64(wx, wy, vx, vy, r)
    float64 wx, wy, vx, vy, r[2];
{
    float64 tmp[2];

    vmat2e_project64(wx, wy, vx, vy, tmp);
    VMAT2E_VEC_SUB(wx, wy, tmp[X], tmp[Y], r);
}

void vmat3e_project_perp64(wx, wy, wz, vx, vy, vz, r)
    float64 wx, wy, wz, vx, vy, vz, r[3];
{
    float64 tmp[3];

    vmat3e_project64(wx, wy, wz, vx, vy, vz, tmp);
    VMAT3E_VEC_SUB(wx, wy, wz, tmp[X], tmp[Y], tmp[Z], r);
}

void vmat3_project_plane32(u, w, v, r)
    float32 u[3], w[3], v[3], r[3];
{
    float32 dot_aa, dot_ab, dot_ac, dot_bb, dot_bc;
    float32 comp_a[3], comp_b[3];
    float32 determinant;
    float32 alpha, beta;

    dot_aa = VMAT3_DOT_PROD(w, w);
    dot_ab = VMAT3_DOT_PROD(w, v);
    dot_ac = VMAT3_DOT_PROD(w, u);
    dot_bb = VMAT3_DOT_PROD(v, v);
    dot_bc = VMAT3_DOT_PROD(v, u);

    determinant = dot_aa * dot_bb - dot_ab * dot_ab;
    alpha = ((dot_ac * dot_bb) - (dot_bc * dot_ab)) / determinant;
    beta  = ((dot_aa * dot_bc) - (dot_ab * dot_ac)) / determinant;
    VMAT3_SCAL_VEC_MUL(alpha, w, comp_a);
    VMAT3_SCAL_VEC_MUL(beta,  v, comp_b);
    VMAT3_VEC_ADD(comp_a, comp_b, r);
}

void vmat3_project_plane64(u, w, v, r)
    float64 u[3], w[3], v[3], r[3];
{
    float64 dot_aa, dot_ab, dot_ac, dot_bb, dot_bc;
    float64 comp_a[3], comp_b[3];
    float64 determinant;
    float64 alpha, beta;

    dot_aa = VMAT3_DOT_PROD(w, w);
    dot_ab = VMAT3_DOT_PROD(w, v);
    dot_ac = VMAT3_DOT_PROD(w, u);
    dot_bb = VMAT3_DOT_PROD(v, v);
    dot_bc = VMAT3_DOT_PROD(v, u);

    determinant = dot_aa * dot_bb - dot_ab * dot_ab;
    alpha = ((dot_ac * dot_bb) - (dot_bc * dot_ab)) / determinant;
    beta  = ((dot_aa * dot_bc) - (dot_ab * dot_ac)) / determinant;
    VMAT3_SCAL_VEC_MUL(alpha, w, comp_a);
    VMAT3_SCAL_VEC_MUL(beta,  v, comp_b);
    VMAT3_VEC_ADD(comp_a, comp_b, r);
}

void vmat3e_project_plane32(ux, uy, uz, wx, wy, wz, vx, vy, vz, r)
    float32 ux, uy, uz, wx, wy, wz, vx, vy, vz, r[3];
{
    float32 dot_aa, dot_ab, dot_ac, dot_bb, dot_bc;
    float32 comp_a[3], comp_b[3];
    float32 determinant;
    float32 alpha, beta;

    dot_aa = VMAT3E_DOT_PROD(wx, wy, wz, wx, wy, wz);
    dot_ab = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz);
    dot_ac = VMAT3E_DOT_PROD(wx, wy, wz, ux, uy, uz);
    dot_bb = VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz);
    dot_bc = VMAT3E_DOT_PROD(vx, vy, vz, ux, uy, uz);

    determinant = dot_aa * dot_bb - dot_ab * dot_ab;
    alpha = ((dot_ac * dot_bb) - (dot_bc * dot_ab)) / determinant;
    beta  = ((dot_aa * dot_bc) - (dot_ab * dot_ac)) / determinant;
    VMAT3E_SCAL_VEC_MUL(alpha, wx, wy, wz, comp_a);
    VMAT3E_SCAL_VEC_MUL(beta,  vx, vy, vz, comp_b);
    VMAT3_VEC_ADD(comp_a, comp_b, r);
}

void vmat3e_project_plane64(ux, uy, uz, wx, wy, wz, vx, vy, vz, r)
    float64 ux, uy, uz, wx, wy, wz, vx, vy, vz, r[3];
{
    float64 dot_aa, dot_ab, dot_ac, dot_bb, dot_bc;
    float64 comp_a[3], comp_b[3];
    float64 determinant;
    float64 alpha, beta;

    dot_aa = VMAT3E_DOT_PROD(wx, wy, wz, wx, wy, wz);
    dot_ab = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz);
    dot_ac = VMAT3E_DOT_PROD(wx, wy, wz, ux, uy, uz);
    dot_bb = VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz);
    dot_bc = VMAT3E_DOT_PROD(vx, vy, vz, ux, uy, uz);

    determinant = dot_aa * dot_bb - dot_ab * dot_ab;
    alpha = ((dot_ac * dot_bb) - (dot_bc * dot_ab)) / determinant;
    beta  = ((dot_aa * dot_bc) - (dot_ab * dot_ac)) / determinant;
    VMAT3E_SCAL_VEC_MUL(alpha, wx, wy, wz, comp_a);
    VMAT3E_SCAL_VEC_MUL(beta,  vx, vy, vz, comp_b);
    VMAT3_VEC_ADD(comp_a, comp_b, r);
}






















