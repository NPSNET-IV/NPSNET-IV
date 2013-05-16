/****************************************************************************
*   File: vmat_negate.c                                                       *
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
*       Contents: Code to negate a vector                                   *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

void vmat2_negate32(v, r)
    float32 v[2], r[2];
{   VMAT2_NEGATE(v, r);  }

void vmat3_negate32(v, r)
    float32 v[3], r[3];
{   VMAT3_NEGATE(v, r);  }

void vmat2_negate64(v, r)
    float64 v[2], r[2];
{   VMAT2_NEGATE(v, r);  }

void vmat3_negate64(v, r)
    float64 v[3], r[3];
{   VMAT3_NEGATE(v, r);  }

void vmat2e_negate32(vx, vy, r)
    float32 vx, vy, r[2];
{   VMAT2E_NEGATE(vx, vy, r);  }

void vmat3e_negate32(vx, vy, vz, r)
    float32 vx, vy, vz, r[3];
{   VMAT3E_NEGATE(vx, vy, vz, r);  }

void vmat2e_negate64(vx, vy, r)
    float64 vx, vy, r[2];
{   VMAT2E_NEGATE(vx, vy, r);  }

void vmat3e_negate64(vx, vy, vz, r)
    float64 vx, vy, vz, r[3];
{   VMAT3E_NEGATE(vx, vy, vz, r);  }

