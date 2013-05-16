// File: <read_bird.c>

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
/*
 * By Jiang Zhu
 */

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <unistd.h>

#include "birdlib.h"

void
main()
{
   int finished = 0;
   
/*
   float heading, pitch, roll;  // Bird orientation angles
   float xyz[3];                // The actual coordinates to use
*/
   int buttons[3];   		// The buttons on the Bird

   /* Initialine the port of the Bird */
   if ( initialize_bird( "/dev/ttyd3" ) )
   {

     while(!finished)
     {

       /* read the buttons on the Bird (if any) */
       get_bird_buttons(buttons);

       if ((buttons[2] == 1) && (buttons[1] == 1) && (buttons[0] == 1))
	 cerr << "210-210 -> left, middle, and right button pressed together\n\n";
       else if ((buttons[2] == 1) && (buttons[1] == 1))
	 cerr << "21-21 -> left and middle buttton pressed together\n\n";
       else if ((buttons[2] == 1) && (buttons[0] == 1))
	 cerr << "20-20 -> left and right buttton pressed together\n\n";
       else if ((buttons[1] == 1) && (buttons[0] == 1))
	 cerr << "10-10 -> middle and right buttton pressed together\n\n";
       else if (buttons[2] == 1)
	 cerr << "1-1 -> left buttton pressed\n\n";
       else if (buttons[1] == 1)
	 cerr << "1-1 -> middle buttton pressed\n\n";
       else if (buttons[0] == 1)
	 cerr << "1-1 -> right buttton pressed\n\n";

/*
       get_bird_position(xyz, BIRD_AS_MOUSE);
       cerr << " bird position: "
	    << " xyz[0] = " << xyz[0]
	    << " xyz[1] = " << xyz[1]
	    << " xyz[2] = " << xyz[2]
	    << "\n\n";

       get_bird_angles(&heading,&pitch,&roll);
       cerr << " bird angles: "
	    << " heading = " << heading 
	    << " pitch = " << pitch 
	    << " roll = " << roll 
	    << "\n\n";
*/
     } // end while
   } // end if
exit(0);
}
