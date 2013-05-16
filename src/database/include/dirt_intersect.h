// File: <dirt_intersect.h>

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
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */


#ifndef __NPS_DIRT_INTERSECT
#define __NPS_DIRT_INTERSECT

#include <pf.h>

void set_intersect_mask(pfGroup *, long, long);

int grnd_normal ( pfCoord &posture, pfVec3 point, pfVec3 normal );

float gnd_level(pfVec3 &);

float gnd_level2(pfVec3);

float get_altitude (pfVec3 pos);

int grnd_orient(pfCoord &, pfBox *bounding_box = NULL);

int grnd_orient2(pfCoord &);

int water_orient(pfCoord &);

int inplay(pfVec3);

int load_terrain();

int isNanVec3 (pfVec3);

int get_isect_terrain (float &, pfVec3 &, pfVec3);

int get_isect_fixed (float &, pfVec3 &, pfVec3);

int lase_isect (pfCoord, pfVec3 &);

void person_altitude (float &, pfCoord &, pfVec3 &);

int person_orient(pfCoord &posture);

#endif

