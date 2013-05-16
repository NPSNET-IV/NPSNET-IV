// File: <water.h>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *    URL: http://www.cs.nps.navy.mil/research/npsnet/
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *
 * Thank you to our sponsors:  ARL, ARPA, DMSO, STRICOM, TEC, TRAC and
 *                             NPS Direct Funding
 */

#ifndef _WATER_H_
#define _WATER_H_

#include "common_defs.h"

typedef struct _wave
Wave;

struct _wave
{
    float	angle;
    float	length;
    float	height;
    float	speed;
    float	phase;
};

typedef struct _water
Water;

struct _water
{
    int		w;		/* number of wedges */
    float	a0;		/* start angle from +X */
    float	a1;		/* end angle from +X */
    
    int		t;		/* number of tesselations */
    float	t0;		/* minimum radius */
    float	t1;		/* maximum radius */
    float	dd;		/* minimum delta */
    float	ad;		/* additive delta */
    float	md;		/* multiplicative delta */
    
    int 	v;		/* number of vertices */
    pfVec3 	*vertex;	/* vertex coordinates */
    pfVec3 	*normal;	/* vertex normals */
    pfVec4 	*color;		/* vertex colors */
    
    char	name[256];	/* texture file name */
    float	scale;		/* texture scale factor */
    pfVec2 	*texture;	/* texture coordinates */
    pfTexture	*map;		/* texture definition */
    
    float 	*range;		/* range to vertex */
    
    Wave	wave[4];	/* wave definitions */
    
    float	tvx;		/* texture x-velocity */
    float	tvy;		/* texture y-velocity */
};


extern pfGroup *makeWater(Water *water);
extern void animateWater (Water *water);
extern void copyWater(Water * newwater, Water * oldwater);

#endif
