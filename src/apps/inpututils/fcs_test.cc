// File: <joytest.cc>

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

   float theroll = 0.0;
   float thepitch = 0.0;
   float therudder = 0.0;
   fcsdata thumb_wheel = 0;
   fcsdata top_button = 0;
   fcsdata mid_button = 0;
   fcsdata bot_button = 0;
   fcsdata trigger = 0;
   float lthrottle = 0.0;
   float rthrottle = 0.0;
   fcsdata theswitch = 0;
   fcsdata dial = 0;
   fcsdata ball_az = 0;
   fcsdata ball_el = 0;
   fcsdata but1 = 0;
   fcsdata but2 = 0;
   fcsdata but3 = 0;
   fcsdata but4 = 0;
   fcsdata but5 = 0;
   fcsdata but6 = 0;
   fcsdata but7 = 0;
   
   fcsdata tempdata = 0;
   float tempfloat = 0.0;
   int trigger_count = 0;
   long no_data_count = 0;

   cout << "To exit this program, squeeze the trigger " << NUM_TO_QUIT
        << " times." << endl;

   /* Deaden the pitch and roll so that the user can rest their hand on the  */
   /* stick without causing movement.                                        */

   device.deaden_roll(0.02);
   device.deaden_pitch(0.02);
   device.deaden_rudder(0.05);

   /* Continue to read the FCS information until the trigger has been        */
   /* pressed a given number of times.                                       */

   while (trigger_count < NUM_TO_QUIT ) 
      {

      /* Check to make sure the FCS has new data                             */

      if ( device.new_data() )
         {
         
         no_data_count = 0;

         /* Get the data and put a lock on it.                               */

         device.get_data();
#ifdef STATS
         info_count++;
#endif

         /* Check each component to see if it has changed since the last     */
         /* time it was read.  If the component has changed, then display    */
         /* its new value.                                                   */

        /* NOTE:  This is the worst way to use the FCS library.  You should  */
        /* make every effort in your code to reduce the number of statements */
        /* between calls to get_data and clear_data.  Basically, you should  */
        /* call get_data, then extract only the items you need from the      */
        /* FCS and put that data in variables, then call clear_data, then    */
        /* actually use the data you read.                                   */ 

         if ( (tempfloat = device.roll()) != theroll )
            {
            theroll = tempfloat;
            cout << "Roll:  " << theroll << endl;
            }

         if ( (tempfloat = device.pitch()) != thepitch )
            {
            thepitch = tempfloat;
            cout << "Pitch:  " << thepitch << endl;
            }

         if ( (tempfloat = device.rudder()) != therudder )
            {
            therudder = tempfloat;
            cout << "Rudder:  " << therudder << endl;
            }

         if ( (tempdata = device.stck_thumb_wheel()) != thumb_wheel )
            {
            thumb_wheel = tempdata;
            cout << "Thumb wheel:  " << thumb_wheel << endl;

            if ( thumb_wheel & MIDDLE )
               cout << "   MIDDLE." << endl;
            if ( thumb_wheel & LEFT )
               cout << "   LEFT." << endl;
            if ( thumb_wheel & RIGHT )
               cout << "   RIGHT." << endl;
            if ( thumb_wheel & UP )
               cout << "   UP." << endl;
            if ( thumb_wheel & DOWN )
               cout << "   DOWN." << endl;

            switch (thumb_wheel)
               {
               /* While thumb wheel on stick is in middle, set the   */
               /* sensitivity (scale) to be 1.0 for roll and pitch   */
               case MIDDLE : device.roll_sensitive ( 1.0 );
                             device.pitch_sensitive ( 1.0 );
                             device.rudder_sensitive ( 1.0 );
                             break;
               case LEFT : 
               case RIGHT: device.roll_sensitive ( 0.5 );
                           device.pitch_sensitive ( 0.5 );
                           device.rudder_sensitive ( 0.5 );
                           break;
               case UP : 
               case DOWN: device.roll_sensitive ( 0.25 );
                          device.pitch_sensitive ( 0.25 );
                          device.rudder_sensitive ( 0.25 );
                          break;

               }
            } 

         if ( (tempdata = device.stck_top_button()) != top_button )
            {
            top_button = tempdata;
            cout << "Top Button:  " << top_button << endl;
            } 

         if ( (tempdata = device.stck_mid_button()) != mid_button )
            {
            mid_button = tempdata;
            cout << "Middle Button:  " << mid_button << endl;
            } 

         if ( (tempdata = device.stck_bot_button()) != bot_button )
            {
            bot_button = tempdata;
            cout << "Bottom Button:  " << bot_button << endl;
            } 

         if ( (tempdata = device.stck_trigger()) != trigger )
            {
            trigger = tempdata;
            cout << "Trigger:  " << trigger << endl;
            /* Each time the trigger is pressed and then released */
            /* count the trigger press.                           */
            if (!trigger)
               {
               trigger_count++;
               cout << "TRIGGER SQUEEZED " << (NUM_TO_QUIT - trigger_count);
               cout << " MORE TIMES TO QUIT." << endl;
               }
            } 

         if ( (tempfloat = device.left_throttle()) != lthrottle )
            {
            lthrottle = tempfloat;
            cout << "Left Throttle:  " << lthrottle << endl;
            }

         if ( (tempfloat = device.right_throttle()) != rthrottle )
            {
            rthrottle = tempfloat;
            cout << "Right Throttle:  " << rthrottle << endl;
            }

         if ( (tempdata = device.thrtl_switch()) != theswitch )
            {
            theswitch = tempdata;
            cout << "Switch:  " << theswitch << endl;
            switch (theswitch)
               { 
               /* When the throttle toggle is in the up position     */
               /* totally deaden the roll, pitch and throttles.      */
               /* Other buttons and dials still work.                */
               case UP : device.deaden_roll ( 1.0 );
                         device.deaden_pitch ( 1.0 );
                         device.deaden_lthrottle ( 1.0 );
                         device.deaden_rthrottle ( 1.0 );
                         device.deaden_rudder ( 1.0 );
                         cout << "   UP." << endl;
                         break;
               /* When the throttle toggle is in the middle position */
               /* deaden only the pitch and roll enough to prevent   */
               /* movement when hand is rested on stick.             */
               case MIDDLE : device.deaden_roll ( 0.02 );
                             device.deaden_pitch ( 0.02 );
                             device.deaden_lthrottle ( 0.0 );
                             device.deaden_rthrottle ( 0.0 );
                             device.deaden_rudder ( 0.0 );
                             cout << "   MIDDLE." << endl;
                             break;
               /* When the trottle toggle is in the down position    */
               /* deaden the roll, pitch and throttles by 50%.       */
               case DOWN : device.deaden_roll ( 0.5 );
                           device.deaden_pitch ( 0.5 );
                           device.deaden_lthrottle ( 0.5 );
                           device.deaden_rthrottle ( 0.5 );
                           device.deaden_rudder ( 0.5 );
                           cout << "   DOWN." << endl;
                           break;
               default:    cout << "   UNKNOWN." << endl;
                           break;
               }
            }

         if ( (tempdata = device.thrtl_dial()) != dial )
            {
            dial = tempdata;
            cout << "Dial:  " << dial << endl;
            }

         if ( (tempdata = device.thrtl_ball_az()) != ball_az )
            {
            ball_az = tempdata;
            cout << "Ball Az:  " << ball_az << endl;
            }

         if ( (tempdata = device.thrtl_ball_el()) != ball_el )
            {
            ball_el = tempdata;
            cout << "Ball El:  " << ball_el << endl;
            }

         if ( (tempdata = device.thrtl_but1()) != but1 )
            {
            but1 = tempdata;
            cout << "Button 1:  " << but1 << endl;
            }

         if ( (tempdata = device.thrtl_but2()) != but2 )
            {
            but2 = tempdata;
            cout << "Button 2:  " << but2 << endl;
            }

         if ( (tempdata = device.thrtl_but3()) != but3 )
            {
            but3 = tempdata;
            cout << "Button 3:  " << but3 << endl;
            }

         if ( (tempdata = device.thrtl_but4()) != but4 )
            {
            but4 = tempdata;
            cout << "Button 4:  " << but4 << endl;
            }

         if ( (tempdata = device.thrtl_but5()) != but5 )
            {
            but5 = tempdata;
            cout << "Button 5:  " << but5 << endl;
/*
            if (!but5)
              {
              device.suspend(); 
              cout << "Suspending FCS read process." << endl;
              }
*/
            }

         if ( (tempdata = device.thrtl_but6()) != but6 )
            {
            but6 = tempdata;
            cout << "Button 6:  " << but6 << endl;
/*
            if (!but6)
               device.initialize();
*/
            }

         if ( (tempdata = device.thrtl_but7()) != but7 )
            {
            but7 = tempdata;
            cout << "Button 7:  " << but7 << endl;
            }

         /* Release the lock on the FCS data acquired by get_data            */ 
         device.clear_data();

         } /* END if new_data */
      else
         {
         no_data_count++;
         if ( no_data_count > MAX_DATA_COUNT )
            {
            cout << "FCS has not had new data in a while." << endl;
            if ( device.is_suspended() )
               {
               cout << "FCS read process is suspended." << endl;
               device.resume();
               cout << "FCS read process has been awakened." << endl;
               no_data_count = 0;
               }
            else
               cout << "FCS read process is not suspended." << endl;
            }
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

