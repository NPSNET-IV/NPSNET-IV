// File: <fasttrak.cc>

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
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#define bool xbool
#include <curses.h>
#undef bool

#include "FastrakClass.h"

FastrakClass *fastrak_unit;
FSTK_stations test_sensor;
float hemisphere[3] = { 0.0,  0.0, -1.0};

void getData(float *pos, float *quat);

void main(int argc, char **argv)
{
   cerr << "Fastrak calibration program.\n";

   if (argc < 2) {
      cerr << "Error: must specify configuration filename." << endl;
      exit(1);
   }
      
   // open configuration file
   ifstream config_fileobj(argv[1]);

   if (!config_fileobj) {
      cerr << "Error: opening configuration file: " << argv[1] << endl;
      exit(1);
   }

   fastrak_unit = new FastrakClass(config_fileobj,
                                   FSTK_COORD_MASK|FSTK_QUAT_MASK);

   if (fastrak_unit->exists()) {
      float poso[3], posx[3], posy[3];
      float xhat[3], yhat[3], zhat[3];
      float quat[4];
      test_sensor = FSTK_STATION1;
      char  tmp[5];

      //fastrak_unit->setHemisphere(test_sensor, hemisphere);
      //fastrak_unit->resetAlignment(test_sensor);

      cerr << "Place sensor at the origin and press ENTER." << endl;
      cin.getline(tmp, 4);
      getData(poso, quat);

      cerr << "Place sensor along the X-axis (right) and press ENTER."
           << endl;
      cin.getline(tmp, 4);
      getData(posx, quat);

      cerr << "Place sensor along the Y-axis (forward) and press ENTER."
           << endl;
      cin.getline(tmp, 4);
      getData(posy, quat);

      for (int i=0; i<3; i++) {
         xhat[i] = posx[i] - poso[i];
         yhat[i] = posy[i] - poso[i];
      }

      zhat[0] = xhat[1]*yhat[2] - xhat[2]*yhat[1];
      zhat[1] = xhat[2]*yhat[0] - xhat[0]*yhat[2];
      zhat[2] = xhat[0]*yhat[1] - xhat[1]*yhat[0];

      yhat[0] = zhat[1]*xhat[2] - zhat[2]*xhat[1];
      yhat[1] = zhat[2]*xhat[0] - zhat[0]*xhat[2];
      yhat[2] = zhat[0]*xhat[1] - zhat[1]*xhat[0];

      float tmpx = sqrt(xhat[0]*xhat[0] + xhat[1]*xhat[1] + xhat[2]*xhat[2]);
      float tmpy = sqrt(yhat[0]*yhat[0] + yhat[1]*yhat[1] + yhat[2]*yhat[2]);
      for (i=0; i<3; i++) {
         xhat[i] /= tmpx;
         yhat[i] /= tmpy;
      }

      cout << endl;
      cout << "STATION1_PARAM:" << endl;
      cout << "  hemisphere:      0      0     -1" << endl;
      cout << "  origin:         " << poso[0]/100.0 << " "
           << poso[1]/100.0 << " " << poso[2]/100.0 << endl;
      cout << "  x_point:        " << xhat[0] << " " << xhat[1] << " "
           << xhat[2] << endl;
      cout << "  y_point:        " << yhat[0] << " " << yhat[1] << " "
           << yhat[2] << endl;

      delete fastrak_unit;
   }
   else {
      cerr << "Fastrak not opened.\n";
   }
}


void getData(float pos[3], float quat[4])
{
   // buffer latest fastrak sensor data;
   fastrak_unit->copyBuffer();

   // get HMD calibration (essentially measure orientation of sensor
   // inside of the HMD with respect to a predefined frame.
   fastrak_unit->getPosOrient(test_sensor, FSTK_QUAT_TYPE,
                              pos, quat);

   cerr << "Position, meters: (x,y,z) = (" << pos[0]/100.0 << ", "
        << pos[1]/100.0 << ", " << pos[2]/100.0 << ")" << endl;
   cerr << "Quaternion (q0,q1,q2,q3) = (" << quat[0] << ", "
        << quat[1] << ", " << quat[2] << ", " << quat[3]
        << ")" << endl;
}
