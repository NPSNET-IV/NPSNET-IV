/****************************************************************************
*   File: vmat_cross.c                                                        *
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
*       Contents: Code to compute cross products                            *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

float32 vmat2_cross_prod32(w, v)
    float32 w[2], v[2];
{   return VMAT2_CROSS_PROD(w, v);  }

void vmat3_cross_prod32(w, v, r)
    float32 w[3], v[3], r[3];
{   VMAT3_CROSS_PROD(w, v, r);  }

float64 vmat2_cross_prod64(w, v)
    float64 w[2], v[2];
{   return VMAT2_CROSS_PROD(w, v);  }

void vmat3_cross_prod64(w, v, r)
    float64 w[3], v[3], r[3];
{   VMAT3_CROSS_PROD(w, v, r);  }

float32 vmat2e_cross_prod32(wx, wy, vx, vy)
    float32 wx, wy, vx, vy;
{   return VMAT2E_CROSS_PROD(wx, wy, vx, vy);  }

void vmat3e_cross_prod32(wx, wy, wz, vx, vy, vz, r)
    float32 wx, wy, wz, vx, vy, vz, r[3];
{   VMAT3E_CROSS_PROD(wx, wy, wz, vx, vy, vz, r);  }

float64 vmat2e_cross_prod64(wx, wy, vx, vy)
    float64 wx, wy, vx, vy;
{   return VMAT2E_CROSS_PROD(wx, wy, vx, vy);  }

void vmat3e_cross_prod64(wx, wy, wz, vx, vy, vz, r)
    float64 wx, wy, wz, vx, vy, vz, r[3];
{   VMAT3E_CROSS_PROD(wx, wy, wz, vx, vy, vz, r);  }
