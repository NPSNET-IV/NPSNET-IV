// File: <kaflight.h>

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
/** File:  kaflight.h **/


/* Header file for Kinney Aero Flight Control System, with non-split        */
/* Throttle,  written in C++.                                               */
/* Version: 1.0                                                             */
/* Programmer:  Paul T. Barham, Computer Specialist                         */
/*              Naval Postgraduate School, Computer Science Department      */
/*              Code CS - Barham, Monterey, CA 93943                        */
/*              E-mail:  barham@cs.nps.navy.mil                             */

/* Date:        Changes                                         Programmer: */
/* 7/9/93       Initial release.                                Barham      */

/* Notes on compiling the KAflight library with your code:                  */
/*    You will need to use the compiler switch option "-lmpc" since this    */
/*    library using multi-processing programming techniques.                */

/* A note about documentation:                                              */
/*    The term "device" is used to refer to the KAflight devices            */
/*       collectively.                                                      */
/*    The term "stick" is used to refer to the joystick of the KAflight.    */
/*    The term "throttle" is used to refer to the throttle of the KAflight. */
/*    The term "component" is used to refer to a particular button, switch  */
/*       dial or ball on one of the devices (i.e. stick or throttle).       */

#ifndef __NPS_KAFLIGHT__
#define __NPS_KAFLIGHT__

#include <ulocks.h>
#include "input_device.h"
#include "nps_device.h"


/* Define directions returned from stick thumbwheel and throttle toggle     */

#define FORWARD		NPS_SWITCH_FORWARD
#define BACKWARD	NPS_SWITCH_BACKWARD

#define MIDDLE		NPS_SWITCH_CENTER
#define UP		NPS_SWITCH_UP
#define DOWN		NPS_SWITCH_DOWN
#define LEFT		NPS_SWITCH_LEFT
#define RIGHT		NPS_SWITCH_RIGHT

/* Define initialization item values to be passed to "initialize".          */

#define RESTING 0
#define STICK 1
#define THROTTLE 2
#define STICKTHROTTLE 3
#define BUTTONSSINGLE 4
#define BUTTONSCOMBO 5
#define ALL 6

/* Define filename for file to store and retrieve initial values, format:   */
/* Line 1:  raw_roll_minimum   raw_roll_at_rest   raw_roll_maximum          */
/* Line 2:  raw_pitch_minimum  raw_pitch_at_rest  raw_pitch_maximum         */
/* Line 3:  raw_left_throttle_minimum   raw_left_throttle_maximum           */
/* Line 4:  raw_right_throttle_minimum  raw_right_throttle_maximum          */

#define KFILENAME "/tmp/kaflight.init.data"

/* Define command to alter permissions on init file to be read and write    */
/* for everyone.                                                            */

#define KMAKEREADFILE "chmod 0666 /tmp/kaflight.init.data"

/* Define filename for the shared data arena used by processes.             */

#define KARENA_FILE "/tmp/kaflight.arena.data"

/* Define permission option to chmod command to alter permissions on arena  */
/* file to be read and write for everyone.                                  */

#define KARENA_PERMISSIONS 0666

/* Define size of communication buffer - a little more than double the      */
/* size needed to prevent array overflow in case of communication backup.   */

#define KBUFF_SIZE 20

/* Define the size and type data for individual component return values     */

typedef short fcsdata;

/* Define the structure for all valuator (i.e. return a range) components   */
/* that will be scaled.                                                     */

typedef struct
           {
           fcsdata min;
           fcsdata mid;
           fcsdata max;
           fcsdata deadmin;
           fcsdata deadmax;
           fcsdata negrange;
           fcsdata posrange;
           double sensitive;
           double deaden;
           }  Kvaluator;

/* *********************** Class Definition ******************************* */

class kaflight : public input_device {

/* ********************** Private Declarations **************************** */

private:

/* Define the member variables to hold device information including data    */
/* received during communication.  The use of two buffers is for debugging  */
/* during failed communications and to keep the last "good" data from       */
/* being corrupted during failed communications.                            */
/* Also used in parallel code for producer to read into one buffer while    */
/* consumer reads out of other buffer.                                      */

   static int num_kaflight; /* Number of kaflight processes by parent       */
   int portfd;              /* Serial port file descriptor                  */
   int valid;               /* Boolean set to true if FCS communicates OK   */
   int hasdata;             /* Boolean to signal new data from FCS          */
   int is_reading;          /* Boolean to indicate active read status       */
   char buffer1[KBUFF_SIZE]; /* First data buffer                            */
   char buffer2[KBUFF_SIZE]; /* Second data buffer                           */
   char *data;              /* Pointer to data buffer being used by Consumer*/
   Kvaluator rollinfo;       /* Roll range, sensitivity and deaden info      */
   Kvaluator pitchinfo;      /* Pitch range, sensitivity and deaden info     */
   int flip_pitch;
   int flip_roll;
   Kvaluator lthrottle;      /* LThrottle range, sensitivity and deaden info */
   Kvaluator rthrottle;      /* RThrottle range, sensitivity and deaden info */
   ulock_t datalock;        /* Lock for data buffer mutual exclusion        */
   usema_t *resetsema;       /* Semaphore for re-initiailizing the FCS       */
   usptr_t *arena;          /* Shared memory arena for parallel processes   */
   int fcspid;              /* PID for fcs read parallel process (producer) */
   int killflag;            /* Flag to signal producer to terminate         */
   fcsdata t_buttons[4];
   fcsdata s_buttons[16];
   fcsdata t_switches[9];
   fcsdata s_hat[5];
/* Function "read_file" reads the initialization file and loads the initial */
/* values into the respective member variables.                             */

   int read_file ();

/* Function "write_file" writes the initialization file                     */

   int write_file ();

/* Function "poll" is the producer routine that runs as part of the main   */
/* process when a parallel process is not started.                         */

   int poll ( int tries = 5 );

   void init_stick ( int &stop );
   void init_resting ( int &stop, int prompt );
   void init_throttle ( int &stop );
   void init_button_singles ( int &stop );
   void init_button_combo ( int &stop );


/* Function "poll_continuous" is the producer routine that runs as a       */
/* parallel process to read the FCS continuously.                          */

   friend void Kpoll_continuous ( void *tempthis );


/* *********************** Public Declarations **************************** */

public:

/* Constructor expects the name of the port to use as the first             */
/* parameter (e.g. "/dev/ttyd4").  The port used should be setup for        */
/* reading and writing by everyone before running your program.             */
/* The second parameter is boolean and should be set to 1 if you want the   */
/* constructor to prompt the user if the initialization file is to used     */
/* when the file is present.                                                */
/* The third parameter is boolean and should be set to 1 if you want the    */
/* constructor to attempt to spawn the FCS read code (producer) to another  */
/* processor.  The constructor will spawn the FCS producer as a lightweight */
/* thread only if this parameter is set to 1 and there are more than one    */
/* processor in the machine.                                                */

   kaflight ( const char *port_name = "/dev/ttyd4", 
              int prompt = 1, int multi = 1 );

/* The destructor closes the port if one was successfully opened.           */
/* If a parallel process for the producer was spawned by the constructor,   */
/* then the process is signalled to terminate.                              */
/* The destructor code can be called directly from close_fcs.               */

   void close_kaflight();
   ~kaflight();

/* "Exists" returns 1 if the port is opened and initialized correctly and   */
/*  a HOTAS has responded to an intial query on the port.  Otherwise it     */
/*  returns 0.                                                              */

   int exists () { return valid; }

/* Function "new_data" should be called to see if the FCS has new data      */
/* since the last call to "clear_data".  If the FCS producer code was not   */
/* spawned as a parallel process, then new_data will will return 1 always   */
/* as long as good communications with the FCS are maintained.              */

   int new_data ();

/* Function "get_data" should be called prior to calling any of the data    */
/* extraction routines defined below.  This function guarantees mutual      */
/* exclusion for the FCS data read by the producer.                         */

   void get_data ();

/* Function "clear_data" should be called following the calls to the data   */
/* extraction routines to release the mutual exclusion lock engaged by the  */
/* call to "get_data" and to signal that the producer has used the data.    */
/* NOTE:  The programmer should make all effort to minimize the number of   */
/* statements between calls to "get_data" and "clear_data".                 */

   void clear_data ();

/* Function "initialize" is called by the constructor.  If desired, it can  */
/* be called again to re-initialize the raw minimum and maximum settings    */
/* for roll, pitch, and the throttle(s).  Initialize will recompute the     */
/* dead zones and sensitivity as per their previous settings.               */
/* This function should not be called if the fcs read process is currently  */
/* suspended.                                                               */
/* Initialize expects one parameter specifying what components to it should */
/* initialize.  See constants at top of header file for valid values.       */

   int initialize ( int item = ALL, int prompt = 1 );

/* Function "suspend" will suspend the FCS reader code.  Thus, after the    */
/* call to "suspend", all calls to "new_data" will return 0 until a call    */
/* to "resume" is made.  If a lightweight thread was started for the FCS    */
/* reader, then the producer process is actually put to sleep until the     */
/* the next call to "resume" is made.  If the FCS reader code is part of    */
/* the main process, then no polling to the FCS is performed.               */

   void suspend ();

/* Function "resume" enables the FCS read code after a call to suspend.  If */
/* the FCS reader was started as a parallel lightweight thread, then the    */
/* read process is awakened.  If the reader is part of the main process,    */
/* then that code will start polling the FCS device again.                  */

   void resume ();

/* Function "is_suspended" returns 1 if the FCS reader code has been        */
/* suspended by a call to "suspend".  Otherwise, the function returns 0.    */
/* This function should NOT be used to test for "good" or "established"     */
/* communications -- "exists" should be used instead.                       */

   int is_suspended ( ) const { return !is_reading; }
 

/* The "deaden" functions define a dead region for the component indicated  */
/* in the name of the function.  The only parameter to these functions      */
/* sets a percentage (0.0 - 1.0) of the components range to be a dead area. */
/* Motion in this dead area will return a value of 0.0 by the subsequent    */
/* extraction operation for the component.                                  */
/* A value of 0.0 for percent makes the component very sensitive while a    */
/* value of 1.0 makes the component return a value of 0.0 always.           */
/* Depending on the device itself, a value of 0.02 - 0.05 should suffice    */
/* for most purposes.  Increasing the deaden percentage, decreases the      */
/* resolution of a given compontent.                                        */

   void deaden_roll ( double percent );
   void deaden_pitch ( double percent );
   void deaden_lthrottle ( double percent );
   void deaden_rthrottle ( double percent );
   void deaden_throttle ( double percent );

/* The "sensitive" functions define how sensitive a particular component    */
/* will be.  In a true sense, it defines the scale for the component.  The  */
/* only paramter should be a value greater than or equal to 0.0.  When      */
/* the value is set to 0.0, then the component always returns 0.0.  When    */
/* the value is set to 1.0, the components values range from 0.0 to 1.0.    */
/* The value can be set to greater than 1.0 but not to less than 0.0.       */
/* These "sensitive" functions do not affect the resolution of the component*/
/* For non-split throttles, either "lthrottle_sensitive" or                 */
/* "throttle_sensitive" can be used to set the sensitiviy of the throttle.  */

   void roll_sensitive ( double percent );
   void pitch_sensitive ( double percent );
   void lthrottle_sensitive ( double percent );
   void rthrottle_sensitive ( double percent );
   void throttle_sensitive ( double percent );


/* All of the following functions are considered data extraction routines.  */


/* Functions "roll" and "pitch" return the relative position of the stick   */
/* taking into consideration the dead zone and the sensitivity that have    */
/* been selected.  These functions return a value between                   */
/* -(sensitive.value) and +(sensitive.value) as set with the above 5        */
/* functions; between -1.0 (left, front) and 1.0 (right, rear) by default.  */

   float roll () const;
   float pitch () const;

/* Function "stck_thumb_wheel" returns the position of the stick's thumb    */
/* wheel as an integer defined by the constants UP, DOWN, LEFT, RIGHT and   */
/* MIDDLE.                                                                  */

   fcsdata stck_thumb_wheel () const;

/* The next four functions return a boolean value of 1 if the respective    */
/* button or trigger is pressed on the stick, and a value of 0 if released. */

   fcsdata stck_top_button () const;
   fcsdata stck_mid_button () const;
   fcsdata stck_bot_button () const;
   fcsdata stck_trigger () const;

/* Functions "left_throttle", "right_throttle", and "throttle" return the   */
/* relative position of the throttle taking into consideration the dead     */
/* zone and the sensitivity that have been selected.  These functions       */
/* return a value between 0.0  and +(sensitive.value) as set with the       */
/* "sensitive" functions above.  Between 0.0(rear) and 1.0(front) by default*/
/* For non-split throttles, either "left_throttle" or "throttle" can be     */
/* used; since right_throttle will always return 0.0 in this case.          */

   float left_throttle () const;
   float throttle () const;

/* NOTE:  Included only for consistancy with FCS.  Always return 0.         */

   float right_throttle () const;


/* Functions "thrtl_switch1&2" returns the position of the throttle's toggle*/
/* switches as an integer defined by the constants FORWARD, BACKWARD and    */
/* MIDDLE.                                                                  */

   fcsdata thrtl_switch1 () const;
   fcsdata thrtl_switch2 () const;

/* The next three functions return "raw" data from the ball in the range of */
/* 0 - 255.  Functions "thrtl_ball_az" and "thrtl_ball_el" return values    */
/* reflecting movement of the thumb ball on the throttle.                   */
/* Function thrtl_dial returns a value reflecting the position of the dial  */
/* on the throttle.                                                         */

   fcsdata thrtl_dial () const;

/* NOTE:  Included only for consistancy with FCS.  Always return 0.         */

   fcsdata thrtl_ball_az () const;
   fcsdata thrtl_ball_el () const;


/* The next seven functions return the Boolean state of the respective      */
/* button on the throttle with 1 being pressed and 0 being released.        */

   fcsdata thrtl_but1 () const;
   fcsdata thrtl_but2 () const;

/* NOTE:  Included only for consistancy with FCS.  Always return 0.         */  

   fcsdata thrtl_but3 () const;
   fcsdata thrtl_but4 () const;
   fcsdata thrtl_but5 () const;
   fcsdata thrtl_but6 () const;
   fcsdata thrtl_but7 () const;

   void get_all_inputs ( pfChannel * );
   int button_pressed ( const NPS_BUTTON, int & );
   int button_pressed ( const char, int &) {return FALSE;}
   int valuator_set ( const NPS_VALUATOR, float & );
   int switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL & );
   int calibrate ( const NPS_VALUATOR, const NPS_CALIBRATION,
                   const float value = 0.0f );

};

#endif
