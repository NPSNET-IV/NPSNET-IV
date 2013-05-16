// File: <soundlib.h>

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


#ifndef __NPS_SOUND_SOUNDLIB__
#define __NPS_SOUND_SOUNDLIB__

#include "globals.h"

#define length 2.0
#define max_range 12700
#define half_distance 50
#define pi         3.14159265358979323846
#define r_to_d     180/pi

int  initialize_port(char *);
void close_port(int);
int  trigger_3d_sound(int,
                      EntityLocation,
                      EntityLocation,
                      EntityOrientation,
                      int, int);
void trigger_sound(int, int, int, int);
void test(int,int);
void loadbank(int);
void my_sound_on(int,int,int);
void my_sound_off(int,int);
void my_attribute_on(int,int,int);
void my_attribute_off(int,int);
void my_sound_trigger(int,int,int );
void env_sound_on(int,int,int);
void env_sound_off(int,int);
void my_reset(int);
void asleep(int);
void send_midi_command(int , unsigned char );

float compute_vector_magnitude(float vec[]);


#endif
