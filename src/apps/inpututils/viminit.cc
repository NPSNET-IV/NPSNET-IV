// File: <viminit.cc>

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

#include <unistd.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "fasttrak.h"

const char BELL = char (7);

void main(int argc, char **argv)
{
   float x, y, z;
   int valid;
   float los[3];
   char portname[50];
   int i;

   cerr << "VIM tracker initialization program..." << endl;

   if (argc > 1)
      strcpy(portname, argv[1]);
   else
      strcpy(portname, "/dev/ttyd2");

   cerr << "Opening FASTTRAK sensor 1, reading on serial port:  "
      << portname << endl;

   isotrack tracker(1, portname);

   if (tracker.exists()) {
      tracker.debug(FALSE);
      tracker.quiet_mode(TRUE);
      tracker.set_units(CENTIMETERS);
      tracker.data_request(COORDINATES);

      los[0] = 0.0f;
      los[1] = 0.0f;
      los[2] = -1.0f;


      system("cp ./datafiles/tracker_init.dat ./datafiles/tracker_init.dat.bak");
      ofstream outfile("./datafiles/tracker_init.dat", ios::out);

      if (!outfile) {
         cerr << "Cannot open file ./datafiles/tracker_init.dat" << endl;
         exit(-1);
      }

      cerr << "Setting HEMISPHERE to 0.0, 0.0 -1.0." << endl;

      tracker.set_hemisphere(los[0], los[1], los[2]);

      outfile << "# Calibration data for HMD from FASTTRAK # 1" << endl;
      outfile << "#    - calibration from viminit program." << endl;
      outfile << "# Hemisphere" << endl;
      outfile << "0.0 0.0 -1.0" << endl;
      outfile << "# Origin" << endl;
      cerr << endl
         << "Put FASTTRAK sensor from port 1 to the ORIGIN..."
         << BELL << endl;
      for (i = 5; i > 0; i--) {
         cerr << i << "-";
         if (tracker.new_data()) {
            tracker.get_data();
            tracker.get_status();
            tracker.clear_data();
         }
         sleep(1);
      }
      cerr << endl << endl;
      if (tracker.new_data()) {
         tracker.get_data();
         tracker.get_status();
         if (valid = tracker.get_position(x, y, z)) {
            x /= 100.0f;
            y /= 100.0f;
            z /= 100.0f;
            cerr << "Position: (x,y,z) = "
               << x << ", " << y << ", " << z << endl;
            outfile << x << " " << y << " " << z << endl;
         }
         tracker.clear_data();
      }

      outfile << "# Point on X axis" << endl;
      cerr << "Put FASTTRAK sensor from port 1 to the X AXIS..."
         << BELL << endl;
      for (i = 5; i > 0; i--) {
         cerr << i << "-";
         if (tracker.new_data()) {
            tracker.get_data();
            tracker.get_status();
            tracker.clear_data();
         }
         sleep(1);
      }
      cerr << endl << endl;
      if (tracker.new_data()) {
         tracker.get_data();
         tracker.get_status();
         if (valid = tracker.get_position(x, y, z)) {
            x /= 100.0f;
            y /= 100.0f;
            z /= 100.0f;
            cerr << "Position: (x,y,z) = "
               << x << ", " << y << ", " << z << endl;
            outfile << x << " " << y << " " << z << endl;
         }
         tracker.clear_data();
      }

      outfile << "# Point on Y axis" << endl;
      cerr << "Put FASTTRAK sensor from port 1 to the Y AXIS..."
         << BELL << endl;
      for (i = 5; i > 0; i--) {
         cerr << i << "-";
         if (tracker.new_data()) {
            tracker.get_data();
            tracker.get_status();
            tracker.clear_data();
         }
         sleep(1);
      }
      cerr << endl << endl;
      if (tracker.new_data()) {
         tracker.get_data();
         tracker.get_status();
         if (valid = tracker.get_position(x, y, z)) {
            x /= 100.0f;
            y /= 100.0f;
            z /= 100.0f;
            cerr << "Position: (x,y,z) = "
               << x << ", " << y << ", " << z << endl;
            outfile << x << " " << y << " " << z << endl;
         }
         tracker.clear_data();
      }

      cerr << BELL << endl;

      outfile << "# Pelvis" << endl;
      outfile << "0.0 0.0 0.0" << endl;
      outfile.close();
   }
}
