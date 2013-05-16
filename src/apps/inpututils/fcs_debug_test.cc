// File: <raw.cc>

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

/** File:  joytest.C **/

/* FCS Joystick and Throttle test program.                                   */
/* This program tests every component of the FCS and provides feedback       */
/* to the user.                                                              */
/* Programmer:  Paul T. Barham, Computer Specialist                         */
/*              Naval Postgraduate School, Computer Science Department      */
/*              Code CS - Barham, Monterey, CA 93943                        */
/*              E-mail:  barham@cs.nps.navy.mil                             */

#include <iostream.h> /* For C++ I/O */
#include <string.h> /* For strcpy */
#include <unistd.h> /* For sleep */
#include <sys/time.h>

#include "fcs.h"  /* For FCS object */

#define NUM_TO_QUIT 5             /* Number of trigger squeezes to quit     */
#define DEFAULT_PORT "/dev/ttyd4" /* Default port to use for FCS            */
#define MAX_PORT_LENGTH 12        /* Maximum string length to hold port name*/
#define MAX_DATA_COUNT 500000     /* Loop counter for pause during suspend  */

#ifdef STATS
double get_time()
{
  struct timeval time ;
  struct timezone tzone;
  double secs, usecs;

  tzone.tz_minuteswest = 8100;
  gettimeofday(&time, &tzone);
  secs = (double)time.tv_sec;
  usecs = (double)time.tv_usec;
  usecs *= 0.000001;
  secs = secs + usecs;
  return secs;
}
#endif

void
main ( int argc, char* argv[] )
{
char *port_name = NULL;
#ifdef STATS
long info_count = 0;
double start_time = get_time();
double stop_time;
#endif

/* If any command line arguments exist, assume the first one is the port to  */
/* use for the FCS.                                                          */

if ( argc > 1 )
   port_name = *(argv+1);
else
   {
   port_name = new char[MAX_PORT_LENGTH];
   strcpy ( port_name, DEFAULT_PORT );
   }

/* Declare the object for the FCS.                                           */

fcs device ( port_name, 1, 1 );

/* Check to make sure the FCS was found on the desired port and that the     */
/* FCS is communication correctly.                                           */

if ( !device.exists() )
   cerr << "Unable to communicate with FCS." << endl;

else
   {

   /* Define variables to hold component information.                        */

   int trigger_count = 0;

   cout << "To exit this program, squeeze the trigger " << NUM_TO_QUIT
        << " times." << endl;

   /* Deaden the pitch and roll so that the user can rest their hand on the  */
   /* stick without causing movement.                                        */

   device.deaden_roll(0.02);
   device.deaden_pitch(0.02);

   /* Continue to read the FCS information until the trigger has been        */
   /* pressed a given number of times.                                       */

   while (trigger_count < NUM_TO_QUIT ) 
      {

      /* Check to make sure the FCS has new data                             */

      if ( device.new_data() )
         {
         
         /* Get the data and put a lock on it.                               */

         device.get_data();
#ifdef STATS
         info_count++;
#endif

         }

      } /* END while trigger */

   } /* END if FCS exists */

#ifdef STATS  
stop_time = get_time();
cerr << "Consumer total reads: " << info_count << endl;
cerr << "Consumer packet rate:  " <<  info_count/(stop_time - start_time)
     << endl;
#endif

} /* END program */

