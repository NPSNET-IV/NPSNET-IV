// File: <dr.cc>

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


#include "globals.h"
  
void move_dead_reckon ( ENTITY_REC &er )
  {
  double accelfac;
  double current_time = get_time();
  double deltatime = current_time - er.last_DR_time;
  
  switch(er.deadreckon.algorithm)
    {
    case DRAlgo_Static: //it is fixed, so it does not move
      break;
    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
      er.location.x += er.velocity.x*deltatime;
      er.location.y += er.velocity.y*deltatime;
      er.location.z += er.velocity.z*deltatime;
      break;
    case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
      accelfac = 0.5f*deltatime*deltatime;
      er.location.x += 
         er.velocity.x*deltatime+accelfac*er.deadreckon.linear_accel[0];
      er.location.y += 
         er.velocity.y*deltatime+accelfac*er.deadreckon.linear_accel[1];
      er.location.z += 
         er.velocity.z*deltatime+accelfac*er.deadreckon.linear_accel[2];
      er.velocity.x += accelfac * er.deadreckon.linear_accel[0];
      er.velocity.y += accelfac * er.deadreckon.linear_accel[1];
      er.velocity.z += accelfac * er.deadreckon.linear_accel[2];
      break;
    case DRAlgo_Other:
    case DRAlgo_DRM_RPW:
    case DRAlgo_DRM_RVW:
    case DRAlgo_DRM_FPB:
    case DRAlgo_DRM_RPB:
    case DRAlgo_DRM_RVB:
    case DRAlgo_DRM_FVB:
    default:
      //move the default vehicle type in a stright line
      er.location.x += er.velocity.x*deltatime;
      er.location.y += er.velocity.y*deltatime;
      er.location.z += er.velocity.z*deltatime;
      break;
  }

  er.last_DR_time = current_time;

}
