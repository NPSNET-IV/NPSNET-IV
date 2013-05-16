/****************************************************************************
*   File: vmat_angle.c                                                      *
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
*       Contents: Code to get angle between two vectors                     *
*       Created: Wed Jul 15 1992                                            *
*       Author: wtaylor                                                     *
*      $Revision: 1.4 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

/* returns the angle in radians between the two vectors. */

float64 vmat2_angle_between_vectors64(vec1, vec2)
    float64 vec1[2];
    float64 vec2[2];
{
    return(vmat2e_angle_between_vectors64(vec1[X], vec1[Y], vec2[X], vec2[Y]));
} 

float64 vmat2e_angle_between_vectors64(wx, wy, vx, vy)
    float64 wx, wy, vx, vy;
{
    float64 vec1_mag;
    float64 vec2_mag;
    float64 radians;
    
    vec1_mag = sqrt(VMAT2E_DOT_PROD(wx, wy, wx, wy));
    vec2_mag = sqrt(VMAT2E_DOT_PROD(vx, vy, vx, vy));
    if (! vec1_mag || ! vec2_mag)
      radians = 0;
    else
    {
	radians = VMAT2E_DOT_PROD(wx, wy, vx, vy) / (vec1_mag * vec2_mag);
	radians = NS_LIMITS(-1.0, radians, 1.0);
	radians = acos(radians);
    }
    
    return(copysign(radians, VMAT2E_CROSS_PROD(wx, wy, vx, vy)));
}

float64 vmat3_angle_between_vectors64(vec1, vec2)
    float64 vec1[3];
    float64 vec2[3];
{
    return(vmat3e_angle_between_vectors64(vec1[X], vec1[Y], vec1[Z], 
					  vec2[X], vec2[Y], vec2[Z]));
}

float64 vmat3e_angle_between_vectors64(wx, wy, wz, vx, vy, vz)
    float64 wx, wy, wz;
    float64 vx, vy, vz;
{
    float64 vec1_mag;
    float64 vec2_mag;
    float64 radians;
    
    vec1_mag = sqrt(VMAT3E_DOT_PROD(wx, wy, wz, wx, wy, wz));
    vec2_mag = sqrt(VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz));
    if (! vec1_mag || ! vec2_mag)
      radians = 0;
    else
    {
	radians = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz) / 
	  (vec1_mag * vec2_mag);
	radians = NS_LIMITS(-1.0, radians, 1.0);
	radians = acos(radians);
    }
    
    return(radians);
}


float32 vmat2_angle_between_vectors32(vec1, vec2)
    float32 vec1[2];
    float32 vec2[2];
{
    return(vmat2e_angle_between_vectors32(vec1[X], vec1[Y], vec2[X], vec2[Y]));
} 

float32 vmat2e_angle_between_vectors32(wx, wy, vx, vy)
    float32 wx, wy, vx, vy;
{
    float32 vec1_mag;
    float32 vec2_mag;
    float32 radians;
    
    vec1_mag = sqrt(VMAT2E_DOT_PROD(wx, wy, wx, wy));
    vec2_mag = sqrt(VMAT2E_DOT_PROD(vx, vy, vx, vy));
    if (! vec1_mag || ! vec2_mag)
      radians = 0;
    else
    {
	radians = VMAT2E_DOT_PROD(wx, wy, vx, vy) / (vec1_mag * vec2_mag);
	radians = NS_LIMITS(-1.0, radians, 1.0);
	radians = acos(radians);
    }
    
    return(copysign(radians, VMAT2E_CROSS_PROD(wx, wy, vx, vy)));
}


float32 vmat3_angle_between_vectors32(vec1, vec2)
    float32 vec1[3];
    float32 vec2[3];
{
    return(vmat3e_angle_between_vectors32(vec1[X], vec1[Y], vec1[Z], 
					  vec2[X], vec2[Y], vec2[Z]));
}

float32 vmat3e_angle_between_vectors32(wx, wy, wz, vx, vy, vz)
    float32 wx, wy, wz;
    float32 vx, vy, vz;
{
    float32 vec1_mag;
    float32 vec2_mag;
    float32 radians;
    
    vec1_mag = sqrt(VMAT3E_DOT_PROD(wx, wy, wz, wx, wy, wz));
    vec2_mag = sqrt(VMAT3E_DOT_PROD(vx, vy, vz, vx, vy, vz));
    if (! vec1_mag || ! vec2_mag)
      radians = 0;
    else
    {
	radians = VMAT3E_DOT_PROD(wx, wy, wz, vx, vy, vz) / 
	  (vec1_mag * vec2_mag);
	radians = NS_LIMITS(-1.0, radians, 1.0);
	radians = acos(radians);
    }
    
    return(radians);
}



