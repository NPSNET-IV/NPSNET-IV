// File: <birdlib.c>

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

   This is file birdlib.c

   This file contains the library of Ascension Bird routines developed
   by Michael J. Zyda.

   Routines included for public view:



   int initialize_bird(char *port_name)

      port_name       = "/dev/ttyd2" if the Bird is plugged into IRIS port 2.

      Initialize the Bird device's port.


   void get_bird_position(float xyz[3], short device_type)

      xyz[3] = A usable xyz coordinate position for the Bird.
               What I mean by usable is that I hide all "funny"
               transformations of this point in this routine,
               i.e. I switch the axes to make sense as opposed
               to what the Bird really returns. I also take
               the absolute value of the location so no funny
               flips of the coordinates as the Bird moves away
               from the transmitter.


   void get_bird_angles(float *heading, float *pitch, float *roll)

      heading = Bird manual calls this azimuth.

                The value is in the range +- 180.0 degrees.
                0 degrees is straight towards the screen.
                90 degrees is to your right.
                -90 degrees is to your left.
                -180 degrees and 180 degrees is out of the screen.
                   Note: There is a bad flip at the 180/-180 transition
                         point.
                   Note: heading is basically a rotation about the y axis
                         but the 0 degree point is different than expected.

      pitch   = Bird manual calls this elevation.

                The value is in the range +- 90 degrees.
                Note: pitch is basically a rotation about the x axis
                      but the 0 degree point is different than expected.
                0 degrees is flat Bird on transmitter.
                -90 degrees is Bird butt-end down, front pointing
                    towards the sky.
                90 degrees is front end down, butt-end pointing
                    towards the sky.
                0 degrees is also when the Bird is upside down
                    and on its back.

      roll    = Bird manual calls this roll.

                The value is in the range +- 180 degrees.
                Note: roll is basically a rotation about the z axis.
                0 degrees is Bird flat on the transmitter!!!
                90 degrees is the Bird on its left side.
                180/-180 degrees is upside down Bird.
                -90 degrees is on right side.

      Note: Some comments copied directly from the Bird manual:
            (1) As elevation approaches +-90, azimuth and roll
                become very noisy and exhibit large errors.

            (2) At 90 degrees, azimuth and roll become undefined
                and this is a characteristic of Euler angles.
                -- Use a MATRIX command for more stable representation
                   at high elevation angles.


   void get_bird_buttons(int buttons[3])

      buttons[0] = Right mouse button's value (0 or 1).
             [1] = Middle mouse button's value (0 or 1).
             [2] = Left mouse button's value (0 or 1).

      This routine returns the current value of the buttons
      on the Bird.

      Note: This routine is for the 3-button Bird buttons AND NOT
            for the buttons on the similar product called the
            Simgraphics Flying Mouse.
            -- The Simgraphics Flying Mouse buttons are fielded just
               like the buttons on the standard SGI mouse.


*/


/* Miscellaneous includes required by the library of Bird functions */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <math.h>
#include <iostream.h>

#include "birdlib.h"

/* Some standard & useful defines for the development of this package */

// two magic numbers used in the original code
#define POSMAX     (36.0 / 32767.0)
#define ANGMAX	   (180.0 / 32767.0)

#define DEGTORAD   0.017453293


/* Commands issuable to the Bird */

#define BIRD_ANGLES              0x57   /* Command for returning angles from the Bird */
#define BIRD_POSITION            0x56   /* Command for returning position from the Bird */
#define BIRD_POSITION_AND_ANGLES 0x59
#define BIRD_BUTTONS             0x4E   /* Command for getting the Bird button info */



/* Global variables required by the Bird library */

static int birdfd;   /* File descriptor for the Bird device */

/*


   int initialize_bird(char *port_name)

      port_name       = /dev/ttyd2 if the Bird is plugged into IRIS port 2.

      This routine performs all the initializations necessary for the
      Ascension Bird with respect to its port.


*/

int initialize_bird( char *port_name )
{

   int i;                 /* temp variables */
   static int baud = 9600;  /* initial guess at the baud rate */  
   struct termio term;      /* port structure */
   char buf[256];           /* Buffer to use to send commands to the Bird */


   /* Open the port for read/write */
   if ( (birdfd = open (port_name, O_RDWR )) < 0 )
   {
      cerr << "initialize_bird(): Could not open " << port_name << "\n";
      return( 0 );
   }

   /* Clear the control structure */
   memset(&term,0,sizeof(term) );

   /* yes, I know that above I set the baud rate to a hardcoded
      value of 9600 baud BUT the example code I was sent allowed
      various values and I did not want to lose the following information...
   */
   switch(baud)
   {
      case 1200:	term.c_cflag = B1200; break;
      case 4800:	term.c_cflag = B4800; break;
      case 9600:	term.c_cflag = B9600; break;
      case 19200:	term.c_cflag = B19200; break;
      default:
         cerr << "initialize_bird():Bad baud value " << baud << "\n";
         return( 0 );
   }

   /* The following stuff is left as it was without comment.
      This is just setting the characteristics of the port,
      in the standard Unix ioctl way...
    */
   term.c_iflag =  IXOFF;
   term.c_oflag = 0;
   term.c_cflag |= CS8 | CLOCAL | CREAD;
   term.c_lflag = 0;
   term.c_cc[VMIN]  = 0;
   term.c_cc[VTIME] = 2;

   /* try and configure the port appropriately... */	 
   i = ioctl(birdfd,TCSETA,&term);

   if (i < 0) 
   {
      cerr << "initialize_bird(): Could not configure tty port "
	   << port_name
	   << "\n";
      return( 0 );
   }

   /* Set the Bird to autobaud */
   buf[0] = 0x20;
   write(birdfd,buf,1);
   sleep(1);   /* must wait to take effect */

   /* The Bird configuration information was here but is not needed for the new
      style Bird. 
   */

   return( 1 );

}  /* End initialize_bird() */	

/*	

   void get_bird_position(float xyz[3], short device_type)

      xyz[3] = A usable xyz coordinate position for the Bird.
               What I mean by usable is that I hide all "funny"
               transformations of this point in this routine,
               i.e. I switch the axes to make sense as opposed
               to what the Bird really returns. I also take
               the absolute value of the location so no funny
               flips of the coordinates as the Bird moves away
               from the transmitter.

*/

void get_bird_position( float xyz[3], short device_type )
{

   void select_bird_operation(int command);

   void return_bird_record();

   char buf[80];   /* Read buffer for this operation */

   float txyz[3];  /* temp coord hold (so we can sanitize before returning) */


   /* Select the proper Bird operation to get positions */
   select_bird_operation(BIRD_POSITION); 

   /* Tell the Bird to send us a record */
   return_bird_record();

   /* A position record is 6 bytes long */
   read(birdfd,buf,6);

   if(!(buf[0] & 0x80))	   /* check first word MSB */
   {
      /* printf("get_bird_position(): bad Bird block \n"); */

   }

   /* Turn the information in the buffer into position data.
      Note: the original code I copied this from averaged the
      last 8 positions and returned that average. That probably
      means this data is very noisy. For purposes of simplicity,
      I have deleted that averaging from my code.
   */
   txyz[0] = (short)(((buf[1] << 7) | (buf[0] & 0x7f)) << 2);	
   txyz[1] = (short)(((buf[3] << 7) | buf[2]) << 2);	
   txyz[2] = (short)(((buf[5] << 7) | buf[4]) << 2);

   /* Put some limits on the Bird values */
     txyz[0] = txyz[0] * POSMAX;
     txyz[1] = txyz[1] * POSMAX;
     txyz[2] = txyz[2] * POSMAX;	

/*

   This following code assumes that the world coordinate frame
   used by your program has:

   x axis pointing to the right.
   y axis pointing up.
   z axis pointing out of the screen.

   The Bird has a much different coordinate system:

   +x out of the screen.
   +y pointing to the left.
   +z pointing down.? It seems that + is pointintg up.

   This assumes the transmitter of the Bird is between you
   and the screen and the transmitter's power cord extends
   in the direction toward the screen.   

   Flip the coords so they make sense going out.

*/

   xyz[0] = -txyz[1];
   xyz[1] =  txyz[2];
   xyz[2] =  txyz[0];

   /* Take the absolute values of the Bird position information.
      Note: I do the absolute value as the sign of the location
      on the Bird does flip but the magnitude is correct.
   */
   if (device_type == BIRD_AS_BIRD) {
     if(xyz[0] < 0.0) { xyz[0] = -xyz[0]; }

     if(xyz[1] < 0.0) { xyz[1] = -xyz[1]; }

     if(xyz[2] < 0.0) { xyz[2] = -xyz[2]; }
   } 

}   /* End get_bird_position() */


/*

   void get_bird_angles(float *heading, float *pitch, float *roll)

      heading = Bird manual calls this azimuth.

                The value is in the range +- 180.0 degrees.
                0 degrees is straight towards the screen.
                90 degrees is to your right.
                -90 degrees is to your left.
                -180 degrees and 180 degrees is out of the screen.
                   Note: There is a bad flip at the 180/-180 transition
                         point.
                   Note: heading is basically a rotation about the y axis
                         but the 0 degree point is different than expected.

      pitch   = Bird manual calls this elevation.

                The value is in the range +- 90 degrees.
                Note: pitch is basically a rotation about the x axis
                      but the 0 degree point is different than expected.
                0 degrees is flat Bird on transmitter.
                -90 degrees is Bird butt-end down, front pointing
                    towards the sky.
                90 degrees is front end down, butt-end pointing
                    towards the sky.
                0 degrees is also when the Bird is upside down
                    and on its back.

      roll    = Bird manual calls this roll.

                The value is in the range +- 180 degrees.
                Note: roll is basically a rotation about the z axis.
                0 degrees is Bird flat on the transmitter!
                90 degrees is the Bird on its left side.
                180/-180 degrees is upside down Bird.
                -90 degrees is on right side.

      Note: Some comments copied directly from the Bird manual:
            (1) As elevation approaches +-90, azimuth and roll
                become very noisy and exhibit large errors.

            (2) At 90 degrees, azimuth and roll become undefined
                and this is a characteristic of Euler angles.
                -- Use a MATRIX command for more stable representation
                   at high elevation angles.
*/

void get_bird_angles( float *heading, float *pitch, float *roll )
{

   void select_bird_operation(int command);
   void return_bird_record();
   char buf[80];   /* Read buffer for this operation */

   long h,p,r;     /* Temp. heading, pitch & roll values before
                      conversion.
                   */



   /* Select the proper Bird operation to get angles */
   select_bird_operation(BIRD_ANGLES); 

   /* Tell the Bird to send us a record */
   return_bird_record();

   /* An angles record is 6 bytes long */
   read(birdfd,buf,6);

   if(!(buf[0] & 0x80))	   /* check first word MSB */
   {
      /* printf("get_bird_angles(): bad Bird block \n"); */

   }

   /* Turn the data in the buffer into heading, pitch & roll */
   h = (short)(((buf[1] << 7) | (buf[0] & 0x7f)) << 2);	
   p = (short)(((buf[3] << 7) | buf[2]) << 2);	
   r = (short)(((buf[5] << 7) | buf[4]) << 2);

   *heading = (float)(h) * ANGMAX;
   *pitch   = (float)(p) * ANGMAX;
   *roll    = (float)(r) * ANGMAX;	

}


/*

   void get_bird_buttons(int buttons[3])

      buttons[0] = Right mouse button's value (0 or 1).
             [1] = Middle mouse button's value (0 or 1).
             [2] = Left mouse button's value (0 or 1).

      This routine returns the current value of the buttons
      on the Bird.

      Note: This routine is for the 3-button Bird buttons AND NOT
            for the buttons on the similar product called the
            Simgraphics Flying Mouse.
            -- The Simgraphics Flying Mouse buttons are fielded just
               like the buttons on the standard SGI mouse.

*/

/* Rewritten by Jiang Zhu, Nov. 17, 1994
 *
 * The loop is for continuous multiple samples to fix the bugs in the 
 * bird software (or hardware?).
 *
 * Basically, the problem is that (1) when the middle button is pressed, 
 * we may receive the left-button-pressed event after the middle button
 * event; (2) when the right button is pressed, we may receive the left-
 * button-pressed or the middle-button-pressed event or both after the
 * right button event.
 */

# define MIDDLE_BUTTON_MASK 	0x20
# define RIGHT_BUTTON_MASK 	0x40
# define SAMPLE_TIMES 		4

void get_bird_buttons(int buttons[3])
{

   char buf[80];   // read buffer for this operation
   int count[3] = {0, 0, 0};	// for counting multiple samples

   void select_bird_operation(int command);

   buttons[0] = buttons[1] = buttons[2] = 0;

   for (int i = 0; i < SAMPLE_TIMES; i++)
   {
     select_bird_operation(BIRD_BUTTONS); 
     read(birdfd,buf,1);

     /* According to the bird manual, 0x10 is sent to the host when the
      * left button is pressed; 0x30 when the middle button or middle and 
      * left are pressed; 0x70 is sent when the right button or right and 
      * any other are pressed; and 0x00 when no button is pressed.
      */ 
     if (buf[0] & RIGHT_BUTTON_MASK)
	count[0]++;
     else if (buf[0] & MIDDLE_BUTTON_MASK)
        count[1]++;
     else if (buf[0] == 0x10)
	count[2]++; 
   }

   if (count[0] == SAMPLE_TIMES)
     buttons[0] = 1;
   else if (count[1] == SAMPLE_TIMES)
     buttons[1] = 1;
   else if (count[2] == SAMPLE_TIMES)
     buttons[2] = 1;
}


/* 

   Put the Bird into either BIRD_ANGLE, BIRD_POSITION, BIRD_POSITION_AND_ANGLES,
   or BIRD_BUTTONS return mode.

   This routine is internal to this library and is not meant for
   public use.

   Routine can be called with any of the following three values:

   select_bird_operation(BIRD_ANGLES);
   select_bird_operation(BIRD_POSITION);
   select_bird_operation(BIRD_POSITION_AND_ANGLES);
   select_bird_operation(BIRD_BUTTONS);

   This routine ONLY puts the Bird into this mode.
   A return_bird_record() command must then be issued to actually be able
   to read the data.


*/

void select_bird_operation( int command )
{
	

   int i;               /* Temp. variable */

   char buf[80];        /* Buffer used to send the command to the Bird */

   struct termio term;  /* Termio structure */

   
   /* Switch based on which command has been requested */
   switch(command)
   {
      case BIRD_POSITION_AND_ANGLES:  
         i = 12; /* number of bytes minimum to read */
         break;

      case BIRD_ANGLES:  
         i = 6; 
         break;

      case BIRD_POSITION:
         i = 6;
         break;

      case BIRD_BUTTONS:
         i = 1;   /* 1 byte in a buttons return */
         break;

      default:	  
         i = 0;

   }   /* end switch statement */


   ioctl(birdfd,TCGETA,&term);	/* get current term values */

   term.c_cc[VMIN]  = i;	/* set min chars to read */
   term.c_cc[VTIME] = 2;	/* min time between chars */

   ioctl(birdfd,TCSETA,&term);	/* set termio */

   if(i != 0)
   {
      /* plug in the command we want to execute */
      buf[0] = command;

      /* Send the command to the Bird */
      write(birdfd,buf,1);
   }

}   /* end of select_bird_operation() */

/* 

   The following routine tells the Bird to return a record NOW.
   
*/
void return_bird_record()
{

   char buf[80];


   /* The Bird command to use to return a record is the POINT command,
      or 0x42.
   */
   buf[0] = 0x42;

   /* Send the POINT command to the Bird */
   write(birdfd,buf,1);
	
}   /* end return_bird_record */
