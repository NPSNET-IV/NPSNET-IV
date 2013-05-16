/****************************************************************************
*   File: vmat_trans.c                                                        *
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
*       Contents: Code to do matrix transform operations                    *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.6 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define SWAP(a,b) hold=a;a=b;b=hold

#define VMAT2_TRANSPOSE(type, m, r)					\
{									\
    type hold;								\
									\
    if (m == r)								\
    {									\
	SWAP(r[X][Y],r[Y][X]);						\
    }									\
    else								\
    {									\
	r[X][X] = m[X][X]; r[X][Y] = m[Y][X];				\
	r[Y][X] = m[X][Y]; r[Y][Y] = m[Y][Y];				\
    }									\
}

#define VMAT3_TRANSPOSE(type, m, r)					\
{									\
    type hold;								\
									\
    if (m == r)								\
    {									\
	SWAP(r[X][Y],r[Y][X]);						\
	SWAP(r[X][Z],r[Z][X]);						\
	SWAP(r[Y][Z],r[Z][Y]);						\
    }									\
    else								\
    {									\
	r[X][X] = m[X][X]; r[X][Y] = m[Y][X]; r[X][Z] = m[Z][X];	\
	r[Y][X] = m[X][Y]; r[Y][Y] = m[Y][Y]; r[Y][Z] = m[Z][Y];	\
	r[Z][X] = m[X][Z]; r[Z][Y] = m[Y][Z]; r[Z][Z] = m[Z][Z];	\
    }									\
}

void vmat2_transpose32(m, r)
    float32 m[2][2], r[2][2];
VMAT2_TRANSPOSE(float32, m, r)

void vmat3_transpose32(m, r)
    float32 m[3][3], r[3][3];
VMAT3_TRANSPOSE(float32, m, r)

void vmat2_transpose64(m, r)
    float64 m[2][2], r[2][2];
VMAT2_TRANSPOSE(float64, m, r)

void vmat3_transpose64(m, r)
    float64 m[3][3], r[3][3];
VMAT3_TRANSPOSE(float64, m, r)

#define VMAT3_ADJUGATE(m, r)						\
{									\
    r[X][X] =  m[Y][Y] * m[Z][Z] - m[Z][Y] * m[Y][Z];			\
    r[X][Y] =  m[Z][X] * m[Y][Z] - m[Y][X] * m[Z][Z];			\
    r[X][Z] =  m[Y][X] * m[Z][Y] - m[Z][X] * m[Y][Y];			\
									\
    r[Y][X] =  m[Z][Y] * m[X][Z] - m[X][Y] * m[Z][Z];			\
    r[Y][Y] =  m[X][X] * m[Z][Z] - m[Z][X] * m[X][Z];			\
    r[Y][Z] =  m[Z][X] * m[X][Y] - m[X][X] * m[Z][Y];			\
									\
    r[Z][X] =  m[X][Y] * m[Y][Z] - m[Y][Y] * m[X][Z];			\
    r[Z][Y] =  m[Y][X] * m[X][Z] - m[X][X] * m[Y][Z];			\
    r[Z][Z] =  m[X][X] * m[Y][Y] - m[Y][X] * m[X][Y];			\
}

void vmat3_adjugate32(m, r)
    float32 m[3][3], r[3][3];
{
    float32 s[3][3];

    if (m == r)
    {
	VMAT3_ADJUGATE(m, r)
    }
    else
    {
	VMAT3_ADJUGATE(m, s)
	bcopy((char *)s, (char *)r, sizeof(s));
    }
}

void vmat3_adjugate64(m, r)
    float64 m[3][3], r[3][3];
{
    float64 s[3][3];

    if (m == r)
    {
	VMAT3_ADJUGATE(m, r)
    }
    else
    {
	VMAT3_ADJUGATE(m, s)
	bcopy((char *)s, (char *)r, sizeof(s));
    }
}

#define VMAT2_INVERSE(type, m, r)					     \
{									     \
    type scale, rxx, rxy, ryx, ryy;					     \
									     \
    scale = m[X][X] * m[Y][Y] - m[X][Y] * m[X][X];			     \
    if (scale == 0.0)							     \
    {									     \
	printf("LIBVECMAT ERROR: Matrix uninvertable\n");		     \
	if (m != r)							     \
	  bcopy((char *)m, (char *)r, sizeof(m));			     \
    }									     \
    else								     \
    {									     \
	scale = 1.0 / scale;						     \
									     \
	rxx =  scale * m[Y][Y];						     \
	rxy = -scale * m[X][Y];						     \
	ryx = -scale * m[Y][X];						     \
	ryy =  scale * m[Y][Y];						     \
	r[X][X] = rxx;							     \
	r[X][Y] = rxy;							     \
	r[Y][X] = ryx;							     \
	r[Y][Y] = ryy;							     \
    }									     \
}

void vmat2_inverse32(m, r)
    float32 m[2][2], r[2][2];
VMAT2_INVERSE(float32, m, r)

void vmat2_inverse64(m, r)
    float64 m[2][2], r[2][2];
VMAT2_INVERSE(float64, m, r)

void vmat3_inverse32(m, r)
    float32 m[3][3], r[3][3];
{
    float32 s[3][3], scale;

    scale = vmat3_determinant32(m);
    if (scale == 0.0)
    {
	printf("LIBVECMAT ERROR: Matrix uninvertable\n");
	if (m != r)
	  bcopy((char *)m, (char *)r, sizeof(m));
    }

    vmat3_adjugate32(m,s);
    vmat3_transpose32(s,s);
    vmat3_scal_mat_mul32(1.0/scale,s,r);
}

void vmat3_inverse64(m, r)
    float64 m[3][3], r[3][3];
{
    float64 s[3][3], scale;

    scale = vmat3_determinant64(m);
    if (scale == 0.0)
    {
	printf("LIBVECMAT ERROR: Matrix uninvertable\n");
	if (m != r)
	  bcopy((char *)m, (char *)r, sizeof(m));
    }

    vmat3_adjugate64(m,s);
    vmat3_transpose64(s,s);
    vmat3_scal_mat_mul64(1.0/scale,s,r);
}

float32 vmat2_determinant32(m)
    float32 m[2][2];
{   return VMAT2_DETERMINANT(m);  }

float32 vmat3_determinant32(m)
    float32 m[3][3];
{   return VMAT3_DETERMINANT(m);  }

float64 vmat2_determinant64(m)
    float64 m[2][2];
{   return VMAT2_DETERMINANT(m);  }

float64 vmat3_determinant64(m)
    float64 m[3][3];
{   return VMAT3_DETERMINANT(m);  }
