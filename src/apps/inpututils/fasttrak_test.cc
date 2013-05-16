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

#define bool xbool
#include <iostream.h>
#include <string.h>
#include "fasttrak.h"
#undef bool

void main(int argc, char **argv)
{
   float x, y, z;
   float heading, pitch, roll;
   float alpha, beta, gamma;
   float q0, q1, q2, q3;
   int valid;
   float origin[3], Xpos[3], Ypos[3], los[3];
   char portname[50];

   cerr << "Isotrack tracker test program.\n";

   if (argc > 1)
      strcpy(portname, argv[1]);
   else
      strcpy(portname, "/dev/ttyd4");

   cerr << "Opening ISOTRACK sensor reading on serial port:  "
      << portname << endl;

   isotrack tracker(1, portname, 1, 1);

   if (tracker.exists()) {
      tracker.debug(TRUE);
      tracker.quiet_mode(TRUE);
      tracker.set_units(CENTIMETERS);
//   tracker.set_boresight();
      tracker.data_request(COORDINATES | QUARTERNION);

      origin[0] = 0.0f;
      origin[1] = 0.0f;
      origin[2] = 0.0f;
      Xpos[0] = -1.0f;
      Xpos[1] = 0.0f;
      Xpos[2] = 0.0f;
      Ypos[0] = 0.0f;
      Ypos[1] = -1.0f;
      Ypos[2] = 0.0f;

/*
   cerr << "Setting ALIGNMENT." << endl;

   tracker.set_alignment ( origin[0], origin[1], origin[2],
   Xpos[0], Xpos[1], Xpos[2],
   Ypos[0], Ypos[1], Ypos[2] );
 */
      los[0] = 0.0f;
      los[1] = 0.0f;
      los[2] = -1.0f;

      cerr << "Setting HEMISPHERE." << endl;

      tracker.set_hemisphere(los[0], los[1], los[2]);

      cerr << "Starting LOOP." << endl;

      while (TRUE) {
         if (tracker.new_data()) {
            tracker.get_data();
            tracker.get_status();
            if (valid = tracker.get_position(x, y, z))
               cerr << "Position: (x,y,z) = " << x << ", " << y << ", " << z << endl;
            if (valid = tracker.get_orientation(heading, pitch, roll))
               cerr << "Orientation: (h,p,r) = " << heading << ", " << pitch
                  << ", " << roll << endl;
            if (valid = tracker.get_X_cosines(alpha, beta, gamma))
               cerr << "X Cosines: (a,b,g) = " << alpha << ", " << beta
                  << ", " << gamma << endl;
            if (valid = tracker.get_Y_cosines(alpha, beta, gamma))
               cerr << "Y Cosines: (a,b,g) = " << alpha << ", " << beta
                  << ", " << gamma << endl;
            if (valid = tracker.get_Z_cosines(alpha, beta, gamma))
               cerr << "Z Cosines: (a,b,g) = " << alpha << ", " << beta
                  << ", " << gamma << endl;
            if (valid = tracker.get_quarternion(q0, q1, q2, q3))
               cerr << "Quarternion (q0,q1,q2,q3) = " << q0 << ", " << q1
                  << ", " << q2 << ", " << q3 << endl;
            tracker.clear_data();
            cerr << endl;
         }
      }
   }
   else
      cerr << "Isotrack not opened.\n";
}
