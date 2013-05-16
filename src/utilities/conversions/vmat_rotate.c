/****************************************************************************
*   File: vmat_rotate.c                                                       *
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
*       Contents: Code that does rotations                                  *
*       Created: Wed Jul 15 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.7 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libvecmat.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define VMAT2_PRIMARY_ROTATION(s, c, m)					\
{									\
    m[X][X] = m[Y][Y] = c;						\
    m[X][Y] = s;							\
    m[Y][X] = -s;							\
}

#define VMAT3_PRIMARY_ROTATION(axis, s, c, m)				     \
{									     \
    int32 a1 = (axis+1)%3;						     \
    int32 a2 = (axis+2)%3;						     \
									     \
    bzero((char *)m, 3*3*sizeof(m[0][0]));				     \
    m[axis][axis] = 1.0;						     \
    m[a1][a1] = m[a2][a2] = c;						     \
    m[a1][a2] = s;							     \
    m[a2][a1] = -s;							     \
}

void vmat2_primary_rotation32(sin_angle, cos_angle, m)
    float32 sin_angle, cos_angle, m[2][2];
VMAT2_PRIMARY_ROTATION(sin_angle, cos_angle, m)

void vmat3_primary_rotation32(axis, sin_angle, cos_angle, m)
    int32 axis;
    float32 sin_angle, cos_angle, m[3][3];
VMAT3_PRIMARY_ROTATION(axis, sin_angle, cos_angle, m)

void vmat2_primary_rotation64(sin_angle, cos_angle, m)
    float64 sin_angle, cos_angle, m[2][2];
VMAT2_PRIMARY_ROTATION(sin_angle, cos_angle, m)

void vmat3_primary_rotation64(axis, sin_angle, cos_angle, m)
    int32 axis;
    float64 sin_angle, cos_angle, m[3][3];
VMAT3_PRIMARY_ROTATION(axis, sin_angle, cos_angle, m)

#define VMAT3_ROTATION(type, ax, ay, az, s, c, m)				\
{									\
    type one_m_cos = 1.0 - c;						\
    type sx = ax * s;							\
    type sy = ay * s;							\
    type sz = az * s;							\
    type cxy = ax * ay * one_m_cos;					\
    type cxz = ax * az * one_m_cos;					\
    type cyz = ay * az * one_m_cos;					\
									\
    m[X][X] = c + ax * ax * one_m_cos;					\
    m[X][Y] = cxy - sz;							\
    m[X][Z] = cxz + sy;							\
    m[Y][X] = cxy + sz;							\
    m[Y][Y] = c + ay * ay * one_m_cos;					\
    m[Y][Z] = cyz - sx;							\
    m[Z][X] = cxz - sy;							\
    m[Z][Y] = cyz + sx;							\
    m[Z][Z] = c + az * az * one_m_cos;					\
}

void vmat3_rotation32(axis, sin_angle, cos_angle, m)
    float32 axis[3], sin_angle, cos_angle, m[3][3];
VMAT3_ROTATION(float32, axis[X], axis[Y], axis[Z], sin_angle, cos_angle, m)

void vmat3_rotation64(axis, sin_angle, cos_angle, m)
    float64 axis[3], sin_angle, cos_angle, m[3][3];
VMAT3_ROTATION(float64, axis[X], axis[Y], axis[Z], sin_angle, cos_angle, m)

void vmat3e_rotation32(ax, ay, az, sin_angle, cos_angle, m)
    float32 ax, ay, az, sin_angle, cos_angle, m[3][3];
VMAT3_ROTATION(float32, ax, ay, az, sin_angle, cos_angle, m)

void vmat3e_rotation64(ax, ay, az, sin_angle, cos_angle, m)
    float64 ax, ay, az, sin_angle, cos_angle, m[3][3];
VMAT3_ROTATION(float64, ax, ay, az, sin_angle, cos_angle, m)

#define VMAT3_FLAT_ROTATION(type, vx, vy, vz, m)				\
{									\
    type s;								\
									\
    m[Z][Z] = sqrt(1.0 - vz * vz);					\
    if (m[Z][Z] == 0.0)							\
    {									\
	m[X][X] = 1.0;							\
        m[X][Y] = 0.0;							\
    }									\
    else								\
    {									\
	s = 1.0 / m[Z][Z];						\
        m[X][X] = vy * s;						\
        m[X][Y] = -vx * s;						\
    }									\
    m[X][Z] = 0.0;							\
									\
    m[Y][X] = vx;							\
    m[Y][Y] = vy;							\
    m[Y][Z] = vz;							\
									\
    m[Z][X] = vz * m[X][Y];						\
    m[Z][Y] = -vz * m[X][X];						\
}

void vmat3_flat_rotation32(v, m)
    float32 v[3], m[3][3];
VMAT3_FLAT_ROTATION(float32, v[X], v[Y], v[Z], m)

void vmat3_flat_rotation64(v, m)
    float64 v[3], m[3][3];
VMAT3_FLAT_ROTATION(float64, v[X], v[Y], v[Z], m)

void vmat3e_flat_rotation32(vx, vy, vz, m)
    float32 vx, vy, vz, m[3][3];
VMAT3_FLAT_ROTATION(float32, vx, vy, vz, m)

void vmat3e_flat_rotation64(vx, vy, vz, m)
    float64 vx, vy, vz, m[3][3];
VMAT3_FLAT_ROTATION(float64, vx, vy, vz, m)


#define VMAT3_ANGLE_ROTATION(sin_az, cos_az, sin_el, cos_el, r)		     \
{									     \
    r[X][X] = cos_az;							     \
    r[X][Y] = sin_az * cos_el;						     \
    r[X][Z] = sin_az * sin_el;						     \
    r[Y][X] = -sin_az;							     \
    r[Y][Y] = cos_az * cos_el;						     \
    r[Y][Z] = cos_az * sin_el;						     \
    r[Z][X] = 0.0;							     \
    r[Z][Y] = -sin_el;							     \
    r[Z][Z] = cos_el;							     \
}

void vmat3_angle_rotation32(sin_az, cos_az, sin_el, cos_el, r)
    float32 sin_az;
    float32 cos_az;
    float32 sin_el;
    float32 cos_el;
    float32 r[3][3];
VMAT3_ANGLE_ROTATION(sin_az, cos_az, sin_el, cos_el, r)

void vmat3_angle_rotation64(sin_az, cos_az, sin_el, cos_el, r)
    float64 sin_az;
    float64 cos_az;
    float64 sin_el;
    float64 cos_el;
    float64 r[3][3];
VMAT3_ANGLE_ROTATION(sin_az, cos_az, sin_el, cos_el, r)


