/****************************************************************************
*   File: libvecmat.h                                                       *
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
*       Contents: Public header file for libvecmat                          *
*       Created: Tue Jul 14 1992                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.15 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#ifndef _LIBVECMAT_H_INCLUDED
#define _LIBVECMAT_H_INCLUDED

#include <stdtypes.h> /*common/include/global*/

/* vmat_print_mat:
 *
 * Prints the passed matrix to stdout.
 */
#define VMAT2_PRINT_MAT(info, m) \
printf("info\n% 1.4f % 1.4f\n% 1.4f % 1.4f\n",\
       m[0][0], m[0][1], m[1][0], m[1][1])
#define VMAT3_PRINT_MAT(info, m) \
printf("info\n% 1.4f % 1.4f % 1.4f\n% 1.4f % 1.4f % 1.4f\n% 1.4f % 1.4f % 1.4f\n",\
       m[0][0], m[0][1], m[0][2], \
       m[1][0], m[1][1], m[1][2], \
       m[2][0], m[2][1], m[2][2])
#define VMAT4_PRINT_MAT(info, m) \
printf("info\n% 1.4f % 1.4f % 1.4f % 1.4f\n% 1.4f % 1.4f % 1.4f % 1.4f\n% 1.4f % 1.4f % 1.4f % 1.4f\n% 1.4f % 1.4f % 1.4f % 1.4f\n",\
       m[0][0], m[0][1], m[0][2], m[0][3], \
       m[1][0], m[1][1], m[1][2], m[1][3], \
       m[2][0], m[2][1], m[2][2], m[2][3], \
       m[3][0], m[3][1], m[3][2], m[3][3])

/* vmat_recast_vec:
 *
 * Copies the vector using assignment statements, in order to achieve
 * type conversion.  bcopy is much more efficient when the two
 * arguments are the same type.
 */
#define VMAT2_RECAST_VEC(v,r) ((r)[0]=(v)[0],(r)[1]=(v)[1])
#define VMAT3_RECAST_VEC(v,r) ((r)[0]=(v)[0],(r)[1]=(v)[1],(r)[2]=(v)[2])

/* vmat_recast_mat:
 *
 * Copies the matrix using assignment statements, in order to achieve
 * type conversion.  bcopy is much more efficient when the two
 * arguments are the same type.
 */
#define VMAT2_RECAST_MAT(m,r) (VMAT2_RECAST_VEC(m[0],r[0]), \
			       VMAT2_RECAST_VEC(m[1],r[1]))
#define VMAT3_RECAST_MAT(m,r) (VMAT3_RECAST_VEC(m[0],r[0]), \
			       VMAT3_RECAST_VEC(m[1],r[1]), \
			       VMAT3_RECAST_VEC(m[2],r[2]))

/* vmat_unit:
 *
 * Makes a unit vector pointing in the same direction as the passed
 * vector.  If the passed vector is all zeros, returns the a unit
 * vector aligned with the X axis.
 */
extern void vmat2_unit32(/*  float32 v[2],       r[2] */);
extern void vmat3_unit32(/*  float32 v[3],       r[3] */);
extern void vmat2_unit64(/*  float64 v[2],       r[2] */);
extern void vmat3_unit64(/*  float64 v[3],       r[3] */);
extern void vmat2e_unit32(/* float32 vx, vy,     r[2] */);
extern void vmat3e_unit32(/* float32 vx, vy, vz, r[3] */);
extern void vmat2e_unit64(/* float64 vx, vy,     r[2] */);
extern void vmat3e_unit64(/* float64 vx, vy, vz, r[3] */);

/* vmat_negate:
 *
 * Makes the negative of the passed vector.
 */
extern void vmat2_negate32(/*  float32 v[2],       r[2] */);
extern void vmat3_negate32(/*  float32 v[3],       r[3] */);
extern void vmat2_negate64(/*  float64 v[2],       r[2] */);
extern void vmat3_negate64(/*  float64 v[3],       r[3] */);
extern void vmat2e_negate32(/* float32 vx, vy,     r[2] */);
extern void vmat3e_negate32(/* float32 vx, vy, vz, r[3] */);
extern void vmat2e_negate64(/* float64 vx, vy,     r[2] */);
extern void vmat3e_negate64(/* float64 vx, vy, vz, r[3] */);

#define VMAT2E_NEGATE(vx,vy,r) ((r)[0]=(-(vx)),(r)[1]=(-(vy)))
#define VMAT3E_NEGATE(vx,vy,vz,r) ((r)[0]=(-(vx)),\
				   (r)[1]=(-(vy)),\
				   (r)[2]=(-(vz)))
#define VMAT2_NEGATE(v,r) VMAT2E_NEGATE(v[0],v[1],r)
#define VMAT3_NEGATE(v,r) VMAT3E_NEGATE(v[0],v[1],v[2],r)

/* vmat_dot_prod:
 *
 * Returns the dot product of the passed vectors.
 */
extern float32 vmat2_dot_prod32(/*  float32 w[2],       v[2] */);
extern float32 vmat3_dot_prod32(/*  float32 w[3],       v[3] */);
extern float64 vmat2_dot_prod64(/*  float64 w[2],       v[2] */);
extern float64 vmat3_dot_prod64(/*  float64 w[3],       v[3] */);
extern float32 vmat2e_dot_prod32(/* float32 wx, wy,     vx, vy */);
extern float32 vmat3e_dot_prod32(/* float32 wx, wy, wz, vx, vy, vz */);
extern float64 vmat2e_dot_prod64(/* float64 wx, wy,     vx, vy */);
extern float64 vmat3e_dot_prod64(/* float64 wx, wy, wz, vx, vy, vz */);

#define VMAT2E_DOT_PROD(wx,wy,vx,vy) ((wx)*(vx)+(wy)*(vy))
#define VMAT3E_DOT_PROD(wx,wy,wz,vx,vy,vz) ((wx)*(vx)+(wy)*(vy)+(wz)*(vz))
#define VMAT2_DOT_PROD(w,v) VMAT2E_DOT_PROD(w[0],w[1],v[0],v[1])
#define VMAT3_DOT_PROD(w,v) VMAT3E_DOT_PROD(w[0],w[1],w[2],v[0],v[1],v[2])

/* vmat_cross_prod:
 *
 * Returns the cross product of the passed vectors.  The 2D version
 * returns the magitude of the Z vector, the 3D version actually returns
 * the vector.
 */
extern float32 vmat2_cross_prod32(/* float32 w[2],       v[2]             */);
extern void    vmat3_cross_prod32(/* float32 w[3],       v[3],       r[3] */);
extern float64 vmat2_cross_prod64(/* float64 w[2],       v[2]             */);
extern void    vmat3_cross_prod64(/* float64 w[3],       v[3],       r[3] */);
extern float32 vmat2e_cross_prod32(/*float32 wx, wy,     vx, vy           */);
extern void    vmat3e_cross_prod32(/*float32 wx, wy, wz, vx, vy, vz, r[3] */);
extern float64 vmat2e_cross_prod64(/*float64 wx, wy,     vx, vy           */);
extern void    vmat3e_cross_prod64(/*float64 wx, wy, wz, vx, vy, vz, r[3] */);

#define VMAT2E_CROSS_PROD(wx,wy,vx,vy) ((wx)*(vy)-(wy)*(vx))
#define VMAT3E_CROSS_PROD(wx,wy,wz,vx,vy,vz,r) ((r)[0]=(wy)*(vz)-(wz)*(vy), \
						(r)[1]=(wz)*(vx)-(wx)*(vz), \
						(r)[2]=(wx)*(vy)-(wy)*(vx))
#define VMAT2_CROSS_PROD(w,v) VMAT2E_CROSS_PROD(w[0],w[1],v[0],v[1])
#define VMAT3_CROSS_PROD(w,v,r) VMAT3E_CROSS_PROD(w[0],w[1],w[2],\
						  v[0],v[1],v[2],r)

/* vmat_vec_mag_sq:
 *
 * Computes the dot product of passed vector and itself
 *
 */
extern float32 vmat2_vec_mag_sq32(/* float32 v[2] */);
extern float32 vmat3_vec_mag_sq32(/* float32 v[3] */);
extern float64 vmat2_vec_mag_sq64(/* float64 v[2] */);
extern float64 vmat3_vec_mag_sq64(/* float64 v[3] */);
extern float32 vmat2e_vec_mag_sq32(/* float32 vx, vy */);
extern float32 vmat3e_vec_mag_sq32(/* float32 vx, vy, vz */);
extern float64 vmat2e_vec_mag_sq64(/* float64 vx, vy */);
extern float64 vmat3e_vec_mag_sq64(/* float64 vx, vy, vz */);

#define VMAT2_VEC_MAG_SQ(w)  (VMAT2_DOT_PROD((w), (w)))
#define VMAT3_VEC_MAG_SQ(w)  (VMAT3_DOT_PROD((w), (w)))
#define VMAT2E_VEC_MAG_SQ(wx, wy)     (VMAT2E_DOT_PROD((wx), (wy), (wx), (wy)))
#define VMAT3E_VEC_MAG_SQ(wx, wy, wz) (VMAT3E_DOT_PROD((wx), (wy), (wz), (wx), (wy), (wz)))


/* vmat_vec_add:
 *
 * Adds two vectors.
 */
extern void vmat2_vec_add32(/*  float32 w[2],       v[2],       r[2] */);
extern void vmat3_vec_add32(/*  float32 w[3],       v[3],       r[3] */);
extern void vmat2_vec_add64(/*  float64 w[2],       v[2],       r[2] */);
extern void vmat3_vec_add64(/*  float64 w[3],       v[3],       r[3] */);
extern void vmat2e_vec_add32(/* float32 wx, wy,     vx, vy,     r[2] */);
extern void vmat3e_vec_add32(/* float32 wx, wy, wz, vx, vy, vz, r[3] */);
extern void vmat2e_vec_add64(/* float64 wx, wy,     vx, vy,     r[2] */);
extern void vmat3e_vec_add64(/* float64 wx, wy, wz, vx, vy, vz, r[3] */);

#define VMAT2E_VEC_ADD(wx,wy,vx,vy,r) ((r)[0]=(wx)+(vx),(r)[1]=(wy)+(vy))
#define VMAT3E_VEC_ADD(wx,wy,wz,vx,vy,vz,r) ((r)[0]=(wx)+(vx), \
					     (r)[1]=(wy)+(vy), \
					     (r)[2]=(wz)+(vz))
#define VMAT2_VEC_ADD(w,v,r) VMAT2E_VEC_ADD(w[0],w[1],v[0],v[1],r)
#define VMAT3_VEC_ADD(w,v,r) VMAT3E_VEC_ADD(w[0],w[1],w[2],v[0],v[1],v[2],r)

/* vmat_vec_sub:
 *
 * Subtracts one vector from another.
 */
extern void vmat2_vec_sub32(/*  float32 w[2],       v[2],       r[2] */);
extern void vmat3_vec_sub32(/*  float32 w[3],       v[3],       r[3] */);
extern void vmat2_vec_sub64(/*  float64 w[2],       v[2],       r[2] */);
extern void vmat3_vec_sub64(/*  float64 w[3],       v[3],       r[3] */);
extern void vmat2e_vec_sub32(/* float32 wx, wy,     vx, vy,     r[2] */);
extern void vmat3e_vec_sub32(/* float32 wx, wy, wz, vx, vy, vz, r[3] */);
extern void vmat2e_vec_sub64(/* float64 wx, wy,     vx, vy,     r[2] */);
extern void vmat3e_vec_sub64(/* float64 wx, wy, wz, vx, vy, vz, r[3] */);

#define VMAT2E_VEC_SUB(wx,wy,vx,vy,r) ((r)[0]=(wx)-(vx),(r)[1]=(wy)-(vy))
#define VMAT3E_VEC_SUB(wx,wy,wz,vx,vy,vz,r) ((r)[0]=(wx)-(vx), \
					     (r)[1]=(wy)-(vy), \
					     (r)[2]=(wz)-(vz))
#define VMAT2_VEC_SUB(w,v,r) VMAT2E_VEC_SUB(w[0],w[1],v[0],v[1],r)
#define VMAT3_VEC_SUB(w,v,r) VMAT3E_VEC_SUB(w[0],w[1],w[2],v[0],v[1],v[2],r)

/* vmat_scal_vec_mul:
 *
 * Multiplies a vector by a scalar.
 */
extern void vmat2_scal_vec_mul32(/*  float32 s, v[2],       r[2] */);
extern void vmat3_scal_vec_mul32(/*  float32 s, v[3],       r[3] */);
extern void vmat2_scal_vec_mul64(/*  float64 s, v[2],       r[2] */);
extern void vmat3_scal_vec_mul64(/*  float64 s, v[3],       r[3] */);
extern void vmat2e_scal_vec_mul32(/* float32 s, vx, vy,     r[2] */);
extern void vmat3e_scal_vec_mul32(/* float32 s, vx, vy, vz, r[3] */);
extern void vmat2e_scal_vec_mul64(/* float64 s, vx, vy,     r[2] */);
extern void vmat3e_scal_vec_mul64(/* float64 s, vx, vy, vz, r[3] */);

#define VMAT2E_SCAL_VEC_MUL(s,vx,vy,r) ((r)[0]=(s)*(vx),(r)[1]=(s)*(vy))
#define VMAT3E_SCAL_VEC_MUL(s,vx,vy,vz,r) ((r)[0]=(s)*(vx), \
					   (r)[1]=(s)*(vy), \
					   (r)[2]=(s)*(vz))
#define VMAT2_SCAL_VEC_MUL(s,v,r) VMAT2E_SCAL_VEC_MUL(s,v[0],v[1],r)
#define VMAT3_SCAL_VEC_MUL(s,v,r) VMAT3E_SCAL_VEC_MUL(s,v[0],v[1],v[2],r)

/* vmat_vec_mat_mul:
 *
 * Multiplies a vector by a matrix and returns the resulting vector.
 */
extern void vmat2_vec_mat_mul32(/*  float32 v[2],       m[2][2], r[2] */);
extern void vmat3_vec_mat_mul32(/*  float32 v[3],       m[3][3], r[3] */);
extern void vmat2_vec_mat_mul64(/*  float64 v[2],       m[2][2], r[2] */);
extern void vmat3_vec_mat_mul64(/*  float64 v[3],       m[3][3], r[3] */);
extern void vmat2e_vec_mat_mul32(/* float32 vx, vy,     m[2][2], r[2] */);
extern void vmat3e_vec_mat_mul32(/* float32 vx, vy, vz, m[3][3], r[3] */);
extern void vmat2e_vec_mat_mul64(/* float64 vx, vy,     m[2][2], r[2] */);
extern void vmat3e_vec_mat_mul64(/* float64 vx, vy, vz, m[3][3], r[3] */);

/* vmat_mat_vec_mul:
 *
 * Pre-multiplies a vector by a matrix and returns the resulting vector.
 */
extern void vmat2_mat_vec_mul32(/*  float32 m[2][2], v[2],       r[2] */);
extern void vmat3_mat_vec_mul32(/*  float32 m[3][3], v[3],       r[3] */);
extern void vmat4_mat_vec_mul32(/*  float32 m[4][4], v[4],       r[4] */);
extern void vmat2_mat_vec_mul64(/*  float64 m[2][2], v[2],       r[2] */);
extern void vmat3_mat_vec_mul64(/*  float64 m[3][3], v[3],       r[3] */);
extern void vmat4_mat_vec_mul64(/*  float64 m[4][4], v[4],       r[4] */);
extern void vmat2e_mat_vec_mul32(/* float32 m[2][2], vx, vy,     r[2] */);
extern void vmat3e_mat_vec_mul32(/* float32 m[3][3], vx, vy, vz, r[3] */);
extern void vmat2e_mat_vec_mul64(/* float64 m[2][2], vx, vy,     r[2] */);
extern void vmat3e_mat_vec_mul64(/* float64 m[3][3], vx, vy, vz, r[3] */);

/* vmat_scal_mat_mul:
 *
 * Multiplies a matrix by a scalar.
 */
extern void vmat2_scal_mat_mul32(/* float32 s, m[2][2], r[2][2] */);
extern void vmat3_scal_mat_mul32(/* float32 s, m[3][3], r[3][3] */);
extern void vmat2_scal_mat_mul64(/* float64 s, m[2][2], r[2][2] */);
extern void vmat3_scal_mat_mul64(/* float64 s, m[3][3], r[3][3] */);

/* vmat_mat_mat_mul:
 *
 * Multiplies two matrices.
 */
extern void vmat2_mat_mat_mul32(/* float32 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_mul32(/* float32 m[3][3], n[3][3], r[3][3] */);
extern void vmat4_mat_mat_mul32(/* float32 m[4][4], n[4][4], r[4][4] */);
extern void vmat2_mat_mat_mul64(/* float64 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_mul64(/* float64 m[3][3], n[3][3], r[3][3] */);
extern void vmat4_mat_mat_mul64(/* float64 m[4][4], n[4][4], r[4][4] */);

/* vmat_mat_mat_add:
 *
 * Adds two matrices.
 */
extern void vmat2_mat_mat_add32(/* float32 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_add32(/* float32 m[3][3], n[3][3], r[3][3] */);
extern void vmat2_mat_mat_add64(/* float64 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_add64(/* float64 m[3][3], n[3][3], r[3][3] */);

/* vmat_mat_mat_sub:
 *
 * Subtracts one matrix from another.
 */
extern void vmat2_mat_mat_sub32(/* float32 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_sub32(/* float32 m[3][3], n[3][3], r[3][3] */);
extern void vmat2_mat_mat_sub64(/* float64 m[2][2], n[2][2], r[2][2] */);
extern void vmat3_mat_mat_sub64(/* float64 m[3][3], n[3][3], r[3][3] */);

/* vmat_primary_rotation:
 *
 * Makes a rotation matrix which rotates by the specified angle (passed
 * as a sin/cos pair) around a primary axis (always Z for the 2D version,
 * X, Y or Z for the 3D version).
 */
extern void vmat2_primary_rotation32(/* float32 sin_angle, cos_angle,
					float32 r[2][2]
					*/);
extern void vmat3_primary_rotation32(/* int32 axis,
					float32 sin_angle, cos_angle,
					float32 r[3][3]
					*/);
extern void vmat2_primary_rotation64(/* float64 sin_angle, cos_angle,
					float64 r[2][2]
					*/);
extern void vmat3_primary_rotation64(/* int32 axis,
					float64 sin_angle, cos_angle,
					float64 r[3][3]
					*/);

/* vmat_rotation:
 *
 * Makes a rotation matrix which rotates by the specified angle (passed
 * as a sin/cos pair) around a passed axis.
 */
extern void vmat3_rotation32(/*  float32 axis[3],    sin_angle, cos_angle,
				 float32 r[3][3]
				 */);
extern void vmat3_rotation64(/*  float64 axis[3],    sin_angle, cos_angle,
				 float64 r[3][3]
				 */);
extern void vmat3e_rotation32(/* float32 ax, ay, az, sin_angle, cos_angle,
				 float32 r[3][3]
				 */);
extern void vmat3e_rotation64(/* float64 ax, ay, az, sin_angle, cos_angle,
				 float64 r[3][3]
				 */);

/* vmat_flat_rotation:
 *
 * Makes a rotation matrix which rotates the Y axis into alignment
 * with the passed vector, and leaves the Z axis pointing straight up.
 */
extern void vmat3_flat_rotation32(/*  float32 v[3],       r[3][3] */);
extern void vmat3_flat_rotation64(/*  float64 v[3],       r[3][3] */);
extern void vmat3e_flat_rotation32(/* float32 vx, vy, vz, r[3][3] */);
extern void vmat3e_flat_rotation64(/* float64 vx, vy, vz, r[3][3] */);

/* vmat_angle_rotation:
 *
 * Generates a rotation matrix based on an azimuth rotation about the Z axis,
 * and an elevation rotation about the X axis.  This assumes that an azimuth
 * of 0 means pointing down the Y axis and rotates counter-clockwise,
 * and that an elevation of 0 means pointing down the X-Y plane and
 * rotates upward toward positive Z.
 */
extern void vmat3_angle_rotation32(/* float32 sin_az,
				      float32 cos_az,
				      float32 sin_el,
				      float32 cos_el,
				      float32 r[3][3]
				      */);

extern void vmat3_angle_rotation64(/* float64 sin_az,
				      float64 cos_az,
				      float64 sin_el,
				      float64 cos_el,
				      float64 r[3][3]
				      */);

/* vmat_transpose:
 *
 * Generates the transpose of the passed matrix.
 */
extern void vmat2_transpose32(/* float32 m[2][2], r[2][2] */);
extern void vmat3_transpose32(/* float32 m[3][3], r[3][3] */);
extern void vmat2_transpose64(/* float64 m[2][2], r[2][2] */);
extern void vmat3_transpose64(/* float64 m[3][3], r[3][3] */);

/* vmat_adjugate:
 *
 * Generates the adjugate of the passed matrix.
 */
extern void vmat3_adjugate32(/* float32 m[3][3], r[3][3] */);
extern void vmat3_adjugate64(/* float64 m[3][3], r[3][3] */);

/* vmat_inverse:
 *
 * Generates the inverse of the passed matrix.
 */
extern void vmat2_inverse32(/* float32 m[2][2], r[2][2] */);
extern void vmat3_inverse32(/* float32 m[3][3], r[3][3] */);
extern void vmat2_inverse64(/* float64 m[2][2], r[2][2] */);
extern void vmat3_inverse64(/* float64 m[3][3], r[3][3] */);

/* vmat_determinant:
 *
 * Finds the determinant of the passed matrix.
 */
extern float32 vmat2_determinant32(/* float32 m[2][2] */);
extern float32 vmat3_determinant32(/* float32 m[3][3] */);
extern float64 vmat2_determinant64(/* float64 m[2][2] */);
extern float64 vmat3_determinant64(/* float64 m[3][3] */);

#define VMAT2_DETERMINANT(m) ((m)[0][0]*(m)[1][1]-(m)[0][1]*(m)[1][0])
#define VMAT3_DETERMINANT(m) ((m)[0][0]*(m)[1][1]*(m)[2][2]+ \
			      (m)[0][1]*(m)[1][2]*(m)[2][0]+ \
			      (m)[0][2]*(m)[1][0]*(m)[2][1]- \
			      (m)[0][0]*(m)[1][2]*(m)[2][1]- \
			      (m)[0][1]*(m)[1][0]*(m)[2][2]- \
			      (m)[0][2]*(m)[1][1]*(m)[2][0])

/* vmat_angle:
 *
 * returns the angle in radians between two vectors
 *
 */

extern float64 vmat2_angle_between_vectors64(/* float64 w[2],       v[2] */);
extern float64 vmat3_angle_between_vectors64(/* float64 w[3],       v[3] */);
extern float64 vmat2e_angle_between_vectors64(/*float64 wx, wy,     vx, vy */);
extern float64 vmat3e_angle_between_vectors64(/*float64 wx, wy, wz, vx, vy, vz\
					       */);
extern float32 vmat2_angle_between_vectors32(/* float32 w[2],       v[2] */);
extern float32 vmat3_angle_between_vectors32(/* float32 w[3],       v[3] */);
extern float32 vmat2e_angle_between_vectors32(/*float32 wx, wy,     vx, vy */);
extern float32 vmat3e_angle_between_vectors32(/*float32 wx, wy, wz, vx, vy, vz\
					       */);



/* vmat_project:
 *
 * Gives the component of Vector A which is parallel to Vector B
 *
 */

extern void vmat2_project32(/* float32 w[2],
			       float32 v[2],
			       float32 r[2] */);
extern void vmat3_project32(/* float32 w[3],
			       float32 v[3],
			       float32 r[3] */);
extern void vmat2_project64(/* float64 w[2],
			       float64 v[2],
			       float64 r[2] */);
extern void vmat3_project64(/* float64 w[3],
			       float64 v[3],
			       float64 r[3] */);
extern void vmat2e_project32(/* float32 wx, wy,
				float32 vx, vy,
				float32 r[2] */);
extern void vmat3e_project32(/* float32 wx, wy, wz,
				float32 vx, vy, vz,
				float32 r[3] */);
extern void vmat2e_project64(/* float64 wx, wy, wz,
				float64 vx, vy, vz,
				float64 r[2] */);
extern void vmat3e_project64(/* float64 wx, wy, wz,
				float64 vx, vy, vz,
				float64 r[3] */);

/* vmat_project_perp:
 *
 * Gives the component of Vector A which is perpendicular to Vector B
 */

extern void vmat2_project_perp32(/* float32 w[2],
				    float32 v[2],
				    float32 r[2] */);
extern void vmat3_project_perp32(/* float32 w[3],
				    float32 v[3],
				    float32 r[3] */);
extern void vmat2_project_perp64(/* float64 w[2],
				    float64 v[2],
				    float64 r[2] */);
extern void vmat3_project_perp64(/* float64 w[3],
				    float64 v[3],
				    float64 r[3] */);
extern void vmat2e_project_perp32(/* float32 wx, wy,
				     float32 vx, vy,
				     float32 r[2] */);
extern void vmat3e_project_perp32(/* float32 wx, wy, wz,
				     float32 vx, vy, vz,
				     float32 r[3] */);
extern void vmat2e_project_perp64(/* float64 wx, wy, wz,
				     float64 vx, vy, vz,
				     float64 r[2] */);
extern void vmat3e_project_perp64(/* float64 wx, wy, wz,
				     float64 vx, vy, vz,
				     float64 r[3] */);


/* vmat_project_plane:
 *
 * Gives the component of Vector C which is in the plane defined by the
   vectors A and B
 *
 */

extern void vmat3_project_plane32(/* float32 u[3],
				     float32 w[3],
				     float32 v[3],
				     float32 r[3] */);
extern void vmat3_project_plane64(/* float64 u[3],
				     float64 w[3],
				     float64 v[3],
				     float64 r[3] */);
extern void vmat3e_project_plane32(/* float32 ux, uy, uz,
				      float32 wx, wy, wz,
				      float32 vx, vy, vz,
				      float32 r[3] */);
extern void vmat3e_project_plane64(/* float64 ux, uy, uz,
				      float64 wx, wy, wz,
				      float64 vx, vy, vz,
				      float64 r[3] */);

/* vmat_vec_copy:
 *
 * Copies the elements of Vector A to Vector B
 *
 */

#define VMAT2_VEC_COPY32(w, r)  (bcopy((char *)(w),(char *)(r),(2 * sizeof(float32))))
#define VMAT3_VEC_COPY32(w, r)  (bcopy((char *)(w),(char *)(r),(3 * sizeof(float32))))
#define VMAT2_VEC_COPY64(w, r)  (bcopy((char *)(w),(char *)(r),(2 * sizeof(float64))))
#define VMAT3_VEC_COPY64(w, r)  (bcopy((char *)(w),(char *)(r),(3 * sizeof(float64))))


/* vmat_mat_copy:
 *
 * Copies the elements of Matrix A to Matrix B
 *
 */

#define VMAT2_MAT_COPY32(w, r)  (bcopy((char *)(w),(char *)(r),(2 * 2 * sizeof(float32))))
#define VMAT3_MAT_COPY32(w, r)  (bcopy((char *)(w),(char *)(r),(3 * 3 * sizeof(float32))))
#define VMAT2_MAT_COPY64(w, r)  (bcopy((char *)(w),(char *)(r),(2 * 2 * sizeof(float64))))
#define VMAT3_MAT_COPY64(w, r)  (bcopy((char *)(w),(char *)(r),(3 * 3 * sizeof(float64))))


/* vmat_vec_equal:
 *
 * Returns 1 if the two vector arguments are equal, otherwise it returns 0
 *
 */

#define VMAT2_VEC_EQUAL32(w, r)  (!bcmp((char *)(w), (char *)(r),(2 * sizeof(float32))))
#define VMAT3_VEC_EQUAL32(w, r)  (!bcmp((char *)(w), (char *)(r),(3 * sizeof(float32))))
#define VMAT2_VEC_EQUAL64(w, r)  (!bcmp((char *)(w), (char *)(r),(2 * sizeof(float64))))
#define VMAT3_VEC_EQUAL64(w, r)  (!bcmp((char *)(w), (char *)(r),(3 * sizeof(float64))))


/* vmat_mat_equal:
 *
 * Returns 1 if the two matrix arguments are equal, otherwise it returns 0
 *
 */

#define VMAT2_MAT_EQUAL32(w, r)  (!bcmp((char *)(w), (char *)(r),(2 * 2 * sizeof(float32))))
#define VMAT3_MAT_EQUAL32(w, r)  (!bcmp((char *)(w), (char *)(r),(3 * 3 * sizeof(float32))))
#define VMAT2_MAT_EQUAL64(w, r)  (!bcmp((char *)(w), (char *)(r),(2 * 2 * sizeof(float64))))
#define VMAT3_MAT_EQUAL64(w, r)  (!bcmp((char *)(w), (char *)(r),(3 * 3 * sizeof(float64))))

#endif /*_LIBVECMAT_H_INCLUDED*/
