// File: <draw.h>

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


#ifndef __NPS_DRAW__
#define __NPS_DRAW__

#include <pf.h>
#include "display_types.h"

// This routine saves the Performer state and loads the identity
//    matrix on the GL matrix stack.  It should be called once
//    at the beginning of all functions performing 2D GL commands.
void hud_mode ();

// This routine restores the saved Performer state for normal drawing.
//    It should be called at the end of the function that previously
//    called hud_mode().
void performer_mode ();

void draw_process ( pfChannel *chan, void *data );

void basic_draw ( pfChannel *chan, void * );
void logo_draw ( pfChannel *chan, void *);
void aim_view_draw ( pfChannel *chan, void *);
void basic_draw_left ( pfChannel *chan, void * );
void basic_draw_right ( pfChannel *chan, void * );


void draw_information ( pfChannel *chan, void *data );

void init_pass_data ( PASS_DATA * );

void init_display_options ();

void update_display_options ( pfChannel *, PASS_DATA * );
void draw_aim_lines ( pfChannel *chan );

#endif
