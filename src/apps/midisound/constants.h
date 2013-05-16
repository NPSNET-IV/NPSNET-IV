// File: <constants.h>

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
// NPSMONO Version 4.0
// Written by:  Paul T. Barham and John T. Roesli
// Naval Postgraduate School, Computer Science Department
// Code CS, Monterey, CA 93940-5100 USA


#ifndef __NPS_SOUND_CONSTANTS__
#define __NPS_SOUND_CONSTANTS__

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef NULL
#define NULL 0
#endif

#define NIL -1

#define NOTE_ON      0x90
#define NOTE_OFF     0x80
// below was 6
#define MY_CHANNEL   8
// below was 7
#define ENV_CHANNEL  9
#define ATTR_CHANNEL 9
#define SQRT2        1.414213562
#define CONST1       2238721.0
#define CONST2       0.05
#define EPSILON      0.000001 /* something really small */
#define SPEED_OF_SOUND 335.28  // speed in meters per second
                               // 70 fahren.,sea level

#define MAX_VOLUME 127
#define MIN_VOLUME 0

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f


#define HALFPI M_PI/2.0f

#define MAX_ENVIRONMENTALS 25

#define RANGE 12700   /* the range of a sound */
#define DEFAULT_DOM 36
#define DEFAULT_HOST 14
#define MIDI_PORT_NAME "/dev/ttyd2"

#define MAX_SOUND_EVENTS 300
#define MAP_FACTOR 0.8
#define DEFAULT_RADIUS 2000.0
#define MIN_RADIUS 50.0
#define MAX_RADIUS RANGE

#define MY_VOLUME 127
#define MY_UPDATE_TIME 15.0
#define ENV_RANGE 3000
#define UPDATE_TIME 1.0
#define TIMEOUT 12.0
#define DELAY_STATE_TIME 6.0

#define MAX_PDUS_PER_LOOP 5

/* sound definitions */

/* spatialized, triggered sounds - on channels 0-5*/

#define TEST_SOUND 0x32
#define DEAD_SOUND 0x3B

/* COMMENTED OUT
#define BULLET_SHOT   0x29
#define BOMB_SHOT     0x3e
#define MISSLE_SHOT   0x40
#define BULLET_DET    0x41
#define BOMB_DET      0x43
#define MISSLE_DET    0x45
#define MOO           0x47
#define BAA           0x48
#define CLUCK         0x4A
*/

/* constant volume sounds for "me"- each is defined on given note and two  */
/* notes above it - channel 6 */

/* COMMENTED OUT
#define F15           0x2b
#define TRUCK         0x30
#define SHIP          0x35
#define LOCK_ON       0x48
#define WARNING_WHINE 0x56
#define DEATH_SOUND   0x59
#define HIT_SOUND     0x4d 
*/

/* continuous sounds- also on channel 7,looped, and pressure sensitive */

/* COMMENTED OUT
#define WATERFALL     0x30
#define TOWN          0x32
*/


/* channel 8 = subwoofer sound!! */

#endif


