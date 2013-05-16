// File: <munitions.h>

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


#ifndef __NPS_MUNITIONS_H__
#define __NPS_MUNITIONS_H__

#include <pf.h>

#include "common_defs.h"
#include "draw.h"

void initialize_weapons_array(int);

void firebuttonhit(wdestype ,int, pfCoord, pfCoord);

//  type pfIsect -> pfuIsect in pf1.2


int vehicle_hit(pfHit *);

int building_hit(pfHit *);

void deactive_weap(int mid);

void update_munitions(float,float);

int switch_store(pfTraverser *, void *);

void addtodeadlist(pfSwitch *,float,long);

void lazarus(void);

void super_lazarus(void);

//  type pfIsect -> pfuIsect in pf1.2

int vehiclehit (pfHit *); /*the function called when a vehicle is hit*/

int buildinghit (pfHit *); /*the function called when a building is hit*/

int target_lock (pfHit *);

#ifndef NONPSSHIP
int mounthit (pfHit *); /*the function called when a checking for mounted_vid*/
#endif

int hitveh(void);

int switch_store_veh(pfTraverser *, void *);
int vid_store(pfTraverser *,void *);
#endif
