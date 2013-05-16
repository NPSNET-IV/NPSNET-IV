// File: <macros.h>

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


#ifndef __NPS_MACROS__
#define __NPS_MACROS__

#include "constants.h"

#define REAL_SMALL 0.0001f
#define SMALL 0.5

#define isstealth(a)        (a == 0)
#define isalive(a)          (a< 100)
#define isgroundveh(a)      ((a >  0) && (a <  34))
#define isairveh(a)         ((a > 33) && (a <  60))
#define isdeadairveh(a)     ((a > 133) && (a <  160))
#define isseaveh(a)        ((a > 59) && (a < 88))
#define isguess(a)        ((a > 87) && (a < 100))

//-1 if negative, 1 if positive
#define POSNEG(a)    ((a<0.0f)?-1:1)

#define IS_ZERO(f)   ((f > -REAL_SMALL) && (f < REAL_SMALL))
#define IS_SMALL(f)  ((f > -SMALL) && (f < SMALL))

#define sqrd(num) ((num)*(num))

/*the difference functions between the DR and Real positions*/

// these macro have been replaced with inline functions.  ABS was
// broken, but is fix now.  We're not using them because I still think
// macro is not safe  -rb

//#define MY_ABS(a,b)   ((((a)<(b))? ((b)-(a)):((a)-(b))))
//#define ABS(a)   (((a < 0) ? (-(a)):(a)))

inline float  MY_ABS(float a, float b)
{ return (a < b) ? (b - a): ( a - b ); }

inline float ABS (float a) 
{ return ((a < 0) ? -a : a);}

#endif
