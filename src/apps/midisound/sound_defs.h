// File: <sound_defs.h>

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

/* File:        sound_defs.h
 * Description: sound server; global constants and c prototypes
 * Revision:    1.0- 23Jul93
 *
 *
 * Author:      Susannah Bloch
 *
 * Hardware:    John Roesli
 *              Susannah Bloch
 *
 * Internet:    bloch@cs.nps.navy.mil
 *              roesli@cs.nps.navy.mil
 *
 * Changes:
 *          date                change
 *         23 Jul 93           added new sound to note definitions
 *                             added my_sound_trigger
 *         24 Jul 93           added ENV_RANGE- how near you
 *                             hear baa, moo, or cluck sounds near the farm
 *                             or city sounds (also defined) near the city
 *                             added UPDATE_TIME; how often waterfall and city
 *                             are checked for updating their volume
 *                             added env_sound_on and env_sound off
 *         27 Jul 93           took city sounds out
 *
 *          6 Oct 93           changed RANGE FROM 12700 TO 25400 changed
 *                             Enviroment from 1000 to 3000
 */

/**************************c functions*********************/
#ifndef __NPS_SOUND_DEFS__
#define __NPS_SOUND_DEFS__


#ifdef __cplusplus
extern "C" {
#endif

#include <strings.h>

int                       initialize_port(char *);
void                      close_port(int);
int                       trigger_3d_sound(int, 
					   EntityLocation,
                                           EntityLocation, 
					   EntityOrientation,
					   int);
void                      test(int,int);
void                      loadbank(int, int);
void                      my_sound_on(int,int,int);
void                      my_sound_off(int,int);
void                      my_sound_trigger(int,int,int);
void                      env_sound_on(int,int,int);
void                      env_sound_off(int,int);
void                      my_reset(int);
void                      asleep(int);
void send_midi_command(int , unsigned char );


#ifdef __cplusplus
}
#endif

#endif
