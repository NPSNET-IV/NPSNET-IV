// File: <get_config.C>

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


/*********************#includes, #defines********************/

#include "get_config.h"

int read_environmentals ( char *filename, 
                          ENVIRON_REC environ_list[MAX_ENVIRONMENTALS] )
   {
   ifstream environ_file;
   char temp[120];
   int count = 0;
   int file_open;
   ENVIRON_REC *environ = environ_list;

   if ( (filename == NULL) || ( (strcmp(filename,"NONE")) == 0 ) )
      return 0;

   environ_file.open(filename, ios::in );

   if ( !environ_file )
      {
      cerr << "Environmental file not found:  " << filename << endl;   
      file_open = FALSE;
      }
   else
      {
      file_open = TRUE;
      while ( !environ_file.eof() && (count < MAX_ENVIRONMENTALS) )
         {
         environ_file >> hex >> environ->midi_note_num >> dec 
                      >> environ->location.x
                      >> environ->location.y
                      >> environ->location.z
                      >> environ->range_sqrd
                      >> environ->repeat_time;

         if ( !environ_file )
            {
            if ( !environ_file.eof() )
               {
               environ_file.clear();
               environ_file.getline(temp, 120);
               if ( temp[0] != '#' )
                  {
                  cerr << "Error in environmental sound file: "
                       << filename << ".\n";
                  cerr << "Incorrect format, line: " << temp << endl;
                  }
               }
            }
         else
            {
            environ_file.getline(temp, 120);
            environ->range_sqrd *= environ->range_sqrd;
            environ->last_play_time = 0.0;
            count++;
            environ++;
            }
         }
      }
   if ( !environ_file.eof() )
      {
      cerr << "ERROR:  Maximum number of environmental sounds exceeded.\n";
      cerr << "        Only first " << MAX_ENVIRONMENTALS
           << " sounds loaded from " << filename << endl;
      }
           
   if ( file_open )
      environ_file.close(); 
   return count;
   }

