#ifndef lint
static char rcsid [] = "\
$RCSfile: coord_rotate.c,v $ \
$Revision: 1.1 $ \
$State: Exp $";
#endif
/****************************************************************************
*   File: coord_rotate.c                                                    *
*                                                                           *
*       Copyright 1995, Reality by Design, Inc.   All Rights Reserved.      *
*                                                                           *
*               Reality by Design, Inc.					    *
*               12 County Road						    *
*               Reading, MA  01867-3754					    *
*               Tel:  617-942-0440                                          *
*               Fax:  617-944-6209                                          *
*               Email:  info@rbd.com					    *
*                                                                           *
*       This software is based on conversion routines from the Loral	    *
*       "libentity" library.						    *
*                                                                           *
*       Contents: Code to do simple orientation conversions                 *
*       Created: Wed May 24 1995                                            *
*       Author: Paul J. Metzger (pjm@rbd.com)				    *
*		(based on work by jesmith)				    *
*                                                                           *
*       Remarks:                                                            *
*       ========                                                            *
*	    While "libcoordinates" doesn't seem to be the logical	    *
*	    place to perform Euler Angle <=> Direction/Cosine Matrix	    *
*	    conversions, it fits here better than any other existing	    *
*	    library.  Also, if you're going to be performing coordinate	    *
*	    conversions in your application, you will also be likely 	    *
*	    to require orientation conversions too.			    *
*                                                                           *
****************************************************************************/

#include "libcoord_local.h"
#include <libvecmat.h>
#include <stdext.h> /*common/include/global*/
#include <math.h>
#define M_PI            3.14159265358979323846


/* NaN preventers: */
static float64 safe_sqrt(x)
    float64 x;
{
    if (x < 0.0) return 0.0;
    else return fsqrt(x);
}

static float64 safe_asin(x)
    float64 x;
{
    if (x < -1.0) return -0.5*PI;
    else if (x > 1.0) return 0.5*PI;
    else return asin(x);
}

static void coord_ptp_to_rot(psi, theta, phi, h)
    float64 psi;
    float64 theta;
    float64 phi;
    float64 h[3][3];
{
    float64 cos_psi    = cos(psi);
    float64 sin_psi    = sin(psi);
    float64 cos_theta  = cos(theta);
    float64 sin_theta  = sin(theta);
    float64 cos_phi    = cos(phi);
    float64 sin_phi    = sin(phi);

    h[0][0] = cos_psi * cos_theta;
    h[0][1] = - sin_psi * cos_phi + cos_psi * sin_theta * sin_phi;
    h[0][2] = sin_psi * sin_phi + cos_psi * sin_theta * cos_phi;
    h[1][0] = sin_psi * cos_theta;
    h[1][1] = cos_psi * cos_phi + sin_psi * sin_theta * sin_phi;
    h[1][2] = - cos_psi * sin_phi + sin_psi * sin_theta * cos_phi;
    h[2][0] = - sin_theta;
    h[2][1] = cos_theta * sin_phi;
    h[2][2] = cos_theta * cos_phi;
}

static void coord_rot_to_ptp(rotation, psi, theta, phi)
    float64  rotation[3][3];
    float64 *psi;
    float64 *theta;
    float64 *phi;
{
    float64 cos_theta;

    cos_theta = safe_sqrt(1.0 - rotation[Z][X]*rotation[Z][X]);
    if (cos_theta == 0.0) /* Singularity here */
      cos_theta = 0.000001;

    *psi = safe_asin(rotation[Y][X] / cos_theta);

    if (rotation[X][X] < 0.0)
    {
        if (*psi < 0.0)
          *psi = -M_PI - (*psi);
        else
          *psi = M_PI - (*psi);
    }

    *theta = - safe_asin(rotation[Z][X]);

    *phi = safe_asin(rotation[Z][Y]  / cos_theta);

    /* Correct for quadrant */
    if (rotation[Z][Z] < 0.0)
    {
        if (*phi < 0.0)
          *phi = -M_PI - (*phi);
        else
          *phi = M_PI - (*phi);
    }
}


static void z_flop(in, out)
    float64 in[3][3];
    float64 out[3][3];
{
    /* Converts between SIMNET and DIS entity coordinate systems by
     * swaping X and Y rows and inverting Z.
     * Note this will NOT work if in and out are the same matrix!
     */
    bcopy((char *)in[1], (char *)out[0], 3*sizeof(float64));
    bcopy((char *)in[0], (char *)out[1], 3*sizeof(float64));
    VMAT3_NEGATE(in[2], out[2]);
}

void coord_dis_eulers_to_rot(psi, theta, phi, tcc, rot)
    float64 psi;
    float64 theta;
    float64 phi;
    COORD_TCC_PTR tcc;
    float64 rot[3][3];
{
    float64 g_to_t_rot[3][3];	/* GCC to TCC matrix */
    float64 world_to_hull_gcc_z_down[3][3];
    float64 hull_to_world_gcc_z_down[3][3];
    float64 hull_to_world_tcc_z_down[3][3];

    /* Note that you must compute the inverse/transpose.
     * You can't do the pre-multiply/post-multiply trick to turn
     * at tcc-to-gcc into a gcc-to-tcc, like you can do with vector
     * multiplication.
     */
    vmat3_transpose64(tcc->t_to_g_rot, g_to_t_rot);

    coord_ptp_to_rot(psi, theta, phi, world_to_hull_gcc_z_down);
    vmat3_transpose64(world_to_hull_gcc_z_down, hull_to_world_gcc_z_down);
    vmat3_mat_mat_mul64(hull_to_world_gcc_z_down, g_to_t_rot,
                            hull_to_world_tcc_z_down);
    z_flop(hull_to_world_tcc_z_down, rot);
}

void coord_rot_to_dis_eulers(rot, tcc, psi, theta, phi)
    float64 rot[3][3];
    COORD_TCC_PTR tcc;
    float64 *psi;
    float64 *theta;
    float64 *phi;
{
    float64 hull_to_world_tcc_z_down[3][3];
    float64 hull_to_world_gcc_z_down[3][3];
    float64 world_to_hull_gcc_z_down[3][3];

    /* Need to convert TCC rotation to GCC */
    z_flop(rot, hull_to_world_tcc_z_down);
    vmat3_mat_mat_mul64(hull_to_world_tcc_z_down, tcc->t_to_g_rot,
                            hull_to_world_gcc_z_down);
    vmat3_transpose64(hull_to_world_gcc_z_down, world_to_hull_gcc_z_down);
    coord_rot_to_ptp(world_to_hull_gcc_z_down, psi, theta, phi);
}

