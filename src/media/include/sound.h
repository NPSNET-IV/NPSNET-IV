// File: <sound.h>

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

// NPSNET Sound library 
// Written by:  Paul T. Barham, barham@cs.nps.navy.mil
// Based on SGI playaiff program
// Naval Postgraduate School, Computer Science Department
// Code CS, Monterey, CA 93940-5100 USA


// Notes on usage:
//    To use this library, you must include sound.h in your program.
//    You must also specify the libraries: -lmpc -lm -laudio
//    when compiling.


#ifndef __NPS_SOUND__
#define __NPS_SOUND__


/* Public defines */

#define NUMSOUNDS	32       /* Max number of indiv. sound entries supp. */
#define MAXSNDLOD	5        /* Max LOD sounds per entry */
#define NUMAPORTS	4        /* Max number of audio channels         */
#define MAXPRIORITY	10       /* Max number of priorities --          */
                                 /*    lower numbers have higher priority*/
#define MAXSNDDIS	5000.0f /* Max distance at which sound is heard */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif

/* Public types */
typedef	struct {
	double	x;
	double	y;
	double	z;
} LocationRec;

/* Public routines */

/* Returns TRUE if the machine has audio hardware support.  Otherwise,   */
/*    returns FALSE.                                                     */
/* This function should be called BEFORE calling any others to determine */
/*    if the machine can play sounds.  If FALSE is returned, DO NOT      */
/*    call any of the routines below.                                    */
int
sound_capable ();

/* This function loads the AIFF sound files that are listed in the file  */
/* specified by the filename parameter.                                  */
/* The format of the file should be, one entry per line:                 */
/* <Sound number between 0 and NUMSOUNDS-1> <Filename of the AIFF file>  */
/* This function also allocates all unused audio ports to your program.  */
void
init_sounds ( char *search_path=NULL );
/*init_sounds ( char * , char *search_path=NULL );*/


/* Parameters:  First EntityLocation is our entities (ear) position,     */
/*              Second EntityLocation is the sound originating position, */
/*              Third is the sound number, 0 to NUMSOUNDS-1,             */
/*              Fourth is the priority, -1 to MAXPRIORITY.               */
/* This routine requests that the sound corresponding to the third       */
/*    parameter be played.  The sound will be played if an available     */
/*    audio port is open.  If no port is currently available, the sound  */
/*    will be played if its priority is LOWER than one currently being   */
/*    played on an audio port.  A priority of -1 is the highest priority.*/
/*    If priority is 0, then the priority will be internally computed    */
/*    based on distance from the entity (i.e. the further away, the less */
/*    likely the sound will be played).  The sound will NOT be played if */
/*    the distance from the entity to the sound is greater than          */
/*    MAXSNDDIS.                                                         */
int
playsound ( LocationRec */*Entity Position*/,
            LocationRec */*Sound Position*/,
            int /*Sound number*/,
            int priority = 0);

/* This function closes all audio ports that were opened by the call to  */
/* init_sounds.  This function should be called before your program      */
/* exits.                                                                */
void
closeaudioports();

#endif
