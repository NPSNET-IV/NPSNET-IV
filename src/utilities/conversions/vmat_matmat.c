/****************************************************************************
*   File: vmat_matmat.c                                                       *
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
*       Contents: Matrix/Matrix operations                                  *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.5 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define TWO_DIMENSIONAL(type, m, n, r)					\
{									\
    type rxx, rxy, ryx, ryy;						\
									\
    rxx = m[X][X] * n[X][X] + m[X][Y] * n[Y][X];			\
    rxy = m[X][X] * n[X][Y] + m[X][Y] * n[Y][Y];			\
    ryx = m[Y][X] * n[X][X] + m[Y][Y] * n[Y][X];			\
    ryy = m[Y][X] * n[X][Y] + m[Y][Y] * n[Y][Y];			\
    r[X][X] = rxx;							\
    r[X][Y] = rxy;							\
    r[Y][X] = ryx;							\
    r[Y][Y] = ryy;							\
}

#define THREE_DIMENSIONAL(type, m, n, r)				     \
{									     \
    type s[3][3];							     \
    int32 i,j;								     \
									     \
    if ((m == r) || (n == r))						     \
    {									     \
	for(i=0;i<3;i++)						     \
	  for(j=0;j<3;j++)						     \
	    s[i][j]=(m[i][X]*n[X][j]+m[i][Y]*n[Y][j]+m[i][Z]*n[Z][j]);	     \
	bcopy((char *)s, (char *)r, sizeof(s));				     \
    }									     \
    else								     \
    {									     \
	for(i=0;i<3;i++)						     \
	  for(j=0;j<3;j++)						     \
	    r[i][j]=(m[i][X]*n[X][j]+m[i][Y]*n[Y][j]+m[i][Z]*n[Z][j]);	     \
    }									     \
}

#define FOUR_DIMENSIONAL(type, m, n, r)					     \
{									     \
    type s[4][4];							     \
    int32 i,j;								     \
									     \
    if ((m == r) || (n == r))						     \
    {									     \
	for(i=0;i<4;i++)						     \
	  for(j=0;j<4;j++)						     \
	    s[i][j]=(m[i][X]*n[X][j]+m[i][Y]*n[Y][j]+			     \
		     m[i][Z]*n[Z][j]+m[i][3]*n[3][j]);	    		     \
	bcopy((char *)s, (char *)r, sizeof(s));				     \
    }									     \
    else								     \
    {									     \
	for(i=0;i<4;i++)						     \
	  for(j=0;j<4;j++)						     \
	    r[i][j]=(m[i][X]*n[X][j]+m[i][Y]*n[Y][j]+			     \
		     m[i][Z]*n[Z][j]+m[i][3]*n[3][j]);			     \
    }									     \
}

#define OP_BY_SIZE(type, count, op, m, n, r)				\
{									\
    uint32 i;								\
									\
    for(i=0;i<count;i++)						\
      ((type *)r)[i] = ((type *)m)[i] op ((type *)n)[i];		\
}

void vmat2_mat_mat_mul32(m, n, r)
    float32 m[2][2], n[2][2], r[2][2];
TWO_DIMENSIONAL(float32, m, n, r)

void vmat3_mat_mat_mul32(m, n, r)
    float32 m[3][3], n[3][3], r[3][3];
THREE_DIMENSIONAL(float32, m, n, r)

void vmat4_mat_mat_mul32(m, n, r)
    float32 m[4][4], n[4][4], r[4][4];
FOUR_DIMENSIONAL(float32, m, n, r)

void vmat2_mat_mat_mul64(m, n, r)
    float64 m[2][2], n[2][2], r[2][2];
TWO_DIMENSIONAL(float64, m, n, r)

void vmat3_mat_mat_mul64(m, n, r)
    float64 m[3][3], n[3][3], r[3][3];
THREE_DIMENSIONAL(float64, m, n, r)

void vmat4_mat_mat_mul64(m, n, r)
    float64 m[4][4], n[4][4], r[4][4];
FOUR_DIMENSIONAL(float64, m, n, r)

void vmat2_mat_mat_add32(m, n, r)
    float32 m[2][2], n[2][2], r[2][2];
OP_BY_SIZE(float32, 4, +, m, n, r)

void vmat3_mat_mat_add32(m, n, r)
    float32 m[3][3], n[3][3], r[3][3];
OP_BY_SIZE(float32, 9, +, m, n, r)

void vmat2_mat_mat_add64(m, n, r)
    float64 m[2][2], n[2][2], r[2][2];
OP_BY_SIZE(float64, 4, +, m, n, r)

void vmat3_mat_mat_add64(m, n, r)
    float64 m[3][3], n[3][3], r[3][3];
OP_BY_SIZE(float64, 9, +, m, n, r)

void vmat2_mat_mat_sub32(m, n, r)
    float32 m[2][2], n[2][2], r[2][2];
OP_BY_SIZE(float32, 4, -, m, n, r)

void vmat3_mat_mat_sub32(m, n, r)
    float32 m[3][3], n[3][3], r[3][3];
OP_BY_SIZE(float32, 9, -, m, n, r)

void vmat2_mat_mat_sub64(m, n, r)
    float64 m[2][2], n[2][2], r[2][2];
OP_BY_SIZE(float64, 4, -, m, n, r)

void vmat3_mat_mat_sub64(m, n, r)
    float64 m[3][3], n[3][3], r[3][3];
OP_BY_SIZE(float64, 9, -, m, n, r)
