/* File: <convert.h> */

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

#ifndef __NPS_ROUND_CONVERT__
#define __NPS_ROUND_CONVERT__

#include <pdu.h>        // list this first
#include "rwstruct.h"

int init_round_world ( roundWorldStruct * );
void convert_fire_to_round ( FirePDU * );
void convert_entity_state_to_round ( EntityStatePDU * );
void convert_location_to_round ( double *, double *, double * );
void convert_detonation_to_round ( DetonationPDU * );
void convert_environment_to_round( EnvironmentalPDU *pdu);
void convert_fire_from_round ( FirePDU * );
void convert_entity_state_from_round ( EntityStatePDU * );
void convert_location_from_round ( double *, double *, double * );
void convert_detonation_from_round ( DetonationPDU * );
#ifndef NO_DVW
void convert_environment_from_round ( EnvironmentalPDU *pdu );
#endif // NO_DVW

#ifdef __cplusplus
extern "C" {
#endif

void find_lat_long_from_DIS ( EntityLocation );
void find_lat_long_from_NPS ( EntityLocation );

#ifdef __cplusplus
}
#endif

#endif
