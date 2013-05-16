// File: <effects.h>

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


#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <pf.h>

//local data stuctures used to hold the fireballs, and craters
typedef struct {
   pfDCS *dcs;
   pfSequence *seq;
   int active;
} ANIMNODETYPE;

void init_animations ();
void load_animation();
/*
int load_animation ( int index, int count, char *filename, 
                     long mode, long reps );
*/
int play_animation ( int index, pfVec3 position, pfVec3 orientation );


void makecrater(pfVec3);
void dump_craters(void);
void dump_fire(void);
void make_fireball_array(void);
void make_crater_array(void);
void makecrater_pos(pfVec3 );
void makecrater(pfVec3,pfVec3);
void makefireball(pfVec3 );
int ground_conform(pfCoord *);

void init_blinking_lights ();
int read_blinking_lights ();
void makeblinkinglight (pfVec3 loc, pfVec3 thecolor, float the_size, double rate);
void check_blinking_lights ();
void check_animations ();


#endif
