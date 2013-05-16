/****************************************************************************
*   File: vmat_dot.c                                                          *
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
*       Contents: Code to compute dot products                              *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.3 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

float32 vmat2_dot_prod32(w, v)
    float32 w[2], v[2];
{   return VMAT2_DOT_PROD(w, v);  }

float32 vmat3_dot_prod32(w, v)
    float32 w[3], v[3];
{   return VMAT3_DOT_PROD(w, v);  }

float64 vmat2_dot_prod64(w, v)
    float64 w[2], v[2];
{   return VMAT2_DOT_PROD(w, v);  }

float64 vmat3_dot_prod64(w, v)
    float64 w[3], v[3];
{   return VMAT3_DOT_PROD(w, v);  }

float32 vmat2e_dot_prod32(wx, wy, vx, vy)
    float32 wx, wy, vx, vy;
{   return VMAT2E_DOT_PROD(wx, wy, vx, vy);  }

float32 vmat3e_dot_prod32(wx, wy, wz, vx, vy, vz)
    float32 wx, wy, wz, vx, vy, vz;
{   return VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz);  }

float64 vmat2e_dot_prod64(wx, wy, vx, vy)
    float64 wx, wy, vx, vy;
{   return VMAT2E_DOT_PROD(wx, wy, vx, vy);  }

float64 vmat3e_dot_prod64(wx, wy, wz, vx, vy, vz)
    float64 wx, wy, wz, vx, vy, vz;
{   return VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz);  }
