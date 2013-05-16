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
// NPSMONO Version 4.0
// Written by:  Paul T. Barham and John T. Roesli
// Naval Postgraduate School, Computer Science Department
// Code CS, Monterey, CA 93940-5100 USA


#ifdef __cplusplus
extern "C" {
#endif

#include <strings.h>

int                       net_open(char *);
int                       net_read(char **, PDUType *);
void                      freePDU(char *);
void                      net_close();
int                       initialize_port(char *);
void                      close_port(int);
int                       trigger_3d_sound(int, 
					   EntityLocation,
                                           EntityLocation, 
					   EntityOrientation,
					   int);
void                      test(int,int);
void                      my_sound_on(int,int,int);
void                      my_sound_off(int,int);
void                      my_sound_trigger(int,int,int);
void                      env_sound_on(int,int,int);
void                      env_sound_off(int,int);
void                      my_reset(int);
void                      asleep(int);

#ifdef __cplusplus
}
#endif

/**************************c++ functions*********************/
int   get_config(char *);

/***************************#defines**************************/
#define RANGE 12700     /* the range of a sound */
#define DEFAULT_FILE    "./datafiles/config.trg"
#define HOST_FILE       "./datafiles/hosts.dat"
#define NULL 0
#define DEFAULT_DOM 36
#define DEFAULT_HOST 8
#define FILENAME_SIZE 50
#define MIDI_PORT_NAME "/dev/ttyd2"
/* HOST_FILE and FILENAME_SIZE are also defined in get_config.c!!!! */
#define MY_VOLUME 127
#define MY_UPDATE_TIME 15.0
#define ENV_RANGE 1000
#define UPDATE_TIME 1.0

/* sound definitions */

/* spatialized, triggered sounds - on channels 0-5*/
#define TEST_SOUND    0x43
/*#define BULLET_SHOT   0x3b*/
#define BULLET_SHOT   0x29 
#define BOMB_SHOT     0x3e
#define MISSLE_SHOT   0x40
#define BULLET_DET    0x41
#define BOMB_DET      0x43
#define MISSLE_DET    0x45
#define MOO           0x47
#define BAA           0x48
#define CLUCK         0x4A

/* constant volume sounds for "me"- each is defined on given note and two  */
/* notes above it - channel 6*/
/*#define F15           0x29 */ /* f1 */
#define F15           0x2b /* g1 */
#define TRUCK         0x30
#define SHIP          0x35
#define LOCK_ON       0x48 /* c4 */
#define WARNING_WHINE 0x56 /* d5 */
#define DEATH_SOUND   0x59 /* f5 */
#define HITSOUND     0x4d /* f4 */

/* continuous sounds- also on channel 7,looped, and pressure sensitive */
#define WATERFALL     0x30
#define TOWN          0x32 /* d2 */

/* channel 8 = subwoofer sound!! */
