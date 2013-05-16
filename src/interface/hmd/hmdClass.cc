// *********************************************************************
// File    : hmdClass.cc
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : 11 December 1995
// Summary : Functions to handle HMD tracking using my Fastrak device
//         : driver.  Based on traker.cc by Paul Barham, et al.
// Modified: 
// *********************************************************************

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

#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bstring.h>
#include <pf.h>

#include "hmdClass.h"
#include "FastrakClass.h"

//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
hmdClass::hmdClass(const char *config_filename)
{
   valid = FALSE;
   fastrak_unit = NULL;
   hmd_sensor = FSTK_STATION1;  // this is where to specify
                                // the HMD station/port number

   // open configuration file
   ifstream config_fileobj(config_filename);

   if (!config_fileobj) {
      cerr << "Error: opening configuration file: "
         << config_filename << endl;
   }
   else {
      // at some point you MUST ensure that upperbody stuff is not being
      // used; otherwise, undefined behaviour will result.
      fastrak_unit = new FastrakClass(config_fileobj,
                                      FSTK_16BIT_QUAT_MASK);

      if (fastrak_unit->exists() &&
          fastrak_unit->getState(hmd_sensor)) {
         calibrate();
         valid = TRUE;
      }
   }
}

//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
hmdClass::~hmdClass()
{
   if ((fastrak_unit != NULL) && (fastrak_unit->exists())) {
      delete fastrak_unit;
      fastrak_unit = NULL;
   }
}

//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void hmdClass::calibrate()
{
   char keys[5];
   cerr << "PLACE HMD IN BORESIGHT POSITION AND PRESS ENTER." << endl;
   cin.getline(keys, 4);

   // buffer latest fastrak sensor data;
   fastrak_unit->copyBuffer();

   // get HMD calibration (essentially measure orientation of sensor
   // inside of the HMD with respect to a predefined frame.
   pfMatrix H_hmd_cal;
   fastrak_unit->getHMatrix(hmd_sensor, H_hmd_cal);
   pfTransposeMat(H_rx_to_hmd, H_hmd_cal);
}

//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void hmdClass::getOrientation(pfMatrix H_veh_to_hmd)
{
   pfMakeIdentMat(H_veh_to_hmd);

   if (fastrak_unit->exists()) {
      int i,j;

      // buffer latest fastrak sensor data;
      fastrak_unit->copyBuffer();

      // get HMD calibration (essentially measure orientation of sensor
      // inside of the HMD with respect to a predefined frame.
      pfMatrix H_veh_to_rx;
      fastrak_unit->getHMatrix(hmd_sensor, H_veh_to_rx);

      // rotation matrix multiply and transpose all at once:
      for (i=0; i<3; i++) {
         for (j=0; j<3; j++) {
            H_veh_to_hmd[j][i] = H_veh_to_rx[i][0]*H_rx_to_hmd[0][j] +
                                 H_veh_to_rx[i][1]*H_rx_to_hmd[1][j] +
                                 H_veh_to_rx[i][2]*H_rx_to_hmd[2][j];
         }
      }
   }
}
