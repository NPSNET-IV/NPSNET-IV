// File: <fcs.cc>

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
/** File:  fcs.cc **/

/* Source file for HOTAS Flight Control System, with split and non-split    */
/* Weapons Control System written in C++.                                   */
/* Version: 2.1                                                             */
/* Programmer:  Paul T. Barham, Computer Specialist                         */
/*              Naval Postgraduate School, Computer Science Department      */
/*              Code CS - Barham, Monterey, CA 93943                        */
/*              E-mail:  barham@cs.nps.navy.mil                             */

/* Date:        Changes                                         Programmer: */
/* 4/7/93       Initial release.                                Barham      */
/* 4/8/93       Initialize now checks for a HOTAS to respond                */
/*              on the port and therefore "exists" now truly                */
/*              means that a HOTAS is on the port and talking.  Barham      */
/* 4/30/93      FCS code now can operate in parallel.           Barham      */
/* 5/2/93	FCS producer can be suspended/resumed.          Barham      */
/* 6/7/93       Added DEBUG statements and added close_fcs                  */
/*              so application can can destructor code.         Barham      */
/* 6/24/93      Fixed close_fcs so that a suspended fcs thread  Barham      */
/*                 can now be killed.                                       */
/* Early 95     Added support for newer FCS/WCS systems.        Barham      */
/* 5/4/95       Added support for Rudder Control System RCS.    Barham      */

//#define _BSD_SIGNALS
#include <iostream.h>/* For C++ standard I/O stuff                          */
#include <fstream.h> /* For C++ file I/O stuff                              */
#include <unistd.h>  /* For standard Unix read, write stuff                 */
#include <fcntl.h>   /* For file constant definitions and flags             */
#include <termio.h>  /* For terminal I/O stuff                              */
#include <termios.h> /* For terminal I/O stuff                              */
#include <stdlib.h>  /* Standard library stuff                              */
#include <sys/types.h> /* For system type stuff                             */
#include <sys/prctl.h> /* For process control stuff                         */
#include <signal.h>/* For process signal stuff                          */
#include <sys/sysmp.h> /* For system multi-process stuff                    */
#include <assert.h>

#include <bstring.h>
#include <sys/time.h>

#include <math.h>


#include "fcs.h"     /* For the HOTAS C++ class definitions, etc.           */

#ifdef FIXCPLUSPLUS
extern "C" 
{
int ioctl ( int, int, termio* );
}
#endif


/* The HOTAS has a 14 byte packet with the following format:                */
/*    Bytes numbered 0 - 13 in order of reading.  Bits numbered 1 - 8 in    */
/*    low order to high order, i.e. 87654321.                               */
/* Byte 0:  Start character, hex FF.                                        */
/* Byte 1:  Roll value, hex 00 - FF.  Smaller values when stick to left.    */
/* Byte 2:  Pitch value, hex 00 - FF.  Smaller values when stick to front.  */
/* Byte 3:  Stick thumb-wheel value, hex:                                   */
/*          0=middle, 1=up, 2=down, 4=left, 8=right.                        */
/* Byte 4:  Stick buttons, stick trigger and throttle toggle, hex:          */
/*          High order nibble:                                              */
/*             Bit 8:  Trigger, 1=not pressed, 0=pressed.                   */
/*             Bit 7:  Top Button, 1=not pressed, 0=pressed.                */
/*             Bit 6:  Middle Button, 1=not pressed, 0=pressed.             */
/*             Bit 5:  Bottom Button, 1=not pressed, 0=pressed.             */
/*          Low order nibble:                                               */
/*             Throttle toggle:  0=middle, 1=down, 2=up.                    */
/* Byte 5:  Left Throttle (Throttle for non-split devices), hex 00 - FF.    */
/*          Smaller values when throttle to rear.                           */
/* Byte 6:  Right Throttle (Always 0 for non-split devices), hex 00 - FF.   */
/*          Smaller values when throttle to rear.                           */
/* Byte 7:  Throttle dial, hex 00 - FF.                                     */
/* Byte 8:  Throttle thumb-dial, azimuth, hex 00 - FF.  00=rest position.   */
/* Byte 9:  Throttle thumb-dial, elevation, hex 00 - FF.  00=rest position. */
/* Byte 10: Throttle buttons, hex:                                          */
/*          Buttons numbered, left-to-right, top-to-bottom, front-to-rear.  */
/*             Bit 8:  Unused.                                              */
/*             Bit 7:  Button 7, 1=not pressed, 0=pressed                   */
/*             Bit 6:  Button 6, 1=not pressed, 0=pressed                   */
/*             Bit 5:  Button 5, 1=not pressed, 0=pressed                   */
/*             Bit 4:  Button 4, 1=not pressed, 0=pressed                   */
/*             Bit 3:  Button 3, 1=not pressed, 0=pressed                   */
/*             Bit 2:  Button 2, 1=not pressed, 0=pressed                   */
/*             Bit 1:  Button 1, 1=not pressed, 0=pressed                   */
/* Byte 11: Unused by this program.  Believed to be left foot petal.        */
/* Byte 12: Unused by this program.  Believed to be right foot petal.       */
/* Byte 13: Stop character, hex FF.                                         */

/* Communications note:                                                     */
/*    The IBM type serial connections need to be converted to RS232 serial  */
/*    connections.  This is done by swapping pins 2 and 3 and rerouting     */
/*    ground from pin 5 to pin 7.                                           */


// Local defines

#define NO_CONTROLS 0
#define OLD_CONTROLS 1
#define NEW_CONTROLS 2

/* Define bit masks to extract each bit from a byte using logical AND.      */

#define bit1 0x01
#define bit2 0x02
#define bit3 0x04
#define bit4 0x08
#define bit5 0x10
#define bit6 0x20
#define bit7 0x40
#define bit8 0x80

/* Define bit masks to extract lower and upper nibbles (4 bits) from a byte */
/* using logical AND.                                                       */

#define LOWNIBBLE 0x0f
#define HINIBBLE  0xf0

/* Define constants used for communication with the HOTAS.  Communications  */
/* format shown below constant definitions:                                 */

#define FCS_ID 16
#define PACKET_SIZE 12
#define PACKET_SIZE2 14
#define TIME_OUT 1.0
#define INITIAL_TRIES 2
#define MAX_POLL_RETRIES 1250
#define QUERY_CHAR (char) 255
#define STOP_CHAR (char) 255 
#define QUERY_CHAR2 (char) 0x34
#define STOP_CHAR2 (char) 0x06
#define BELL (char) 7
#define ENQUIRE (char) 0x05
#define ACKNOWLEDGE (char) 0x06

#define MAX_FRAME_TIME 0.0333

// Local prototypes

double get_tod();
int open_port ( const char *port_name, int &portfd );
int get_data ( int portfd, char *data );
int get_num_data ( int portfd, int num, char *data );
void send_data ( int portfd, char *data, int num_bytes );
void show_data ( char *data, int num_bytes );
int get_packet ( int portfd, char *buffer, int &num_bytes );
int get_packet2 ( int portfd, char *buffer, int &num_bytes );

volatile int L_signal;
volatile fcs *L_fcs;
volatile char *L_my_buffer;
volatile int L_copying_buffer;
volatile int L_got_data;
volatile int L_numcpus;

#ifdef STATS
volatile long L_packet_count;
volatile long L_bad_packet_count;
volatile double L_start_time;
#endif

const int KNOWN_PRODUCT_ID[] = { 16 };
const int NUM_PRODUCT_IDS = 1;
const int KNOWN_HW_VERSIONS[] = { 33, 35 };
const int NUM_HW_VERSIONS = 2;

// Static member variables initializations

int fcs::num_fcs = 0;


// Local functions

double get_tod()
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

/* Function open_port attempts to open the port indicated by port_name.     */
/* If the port cannot be opened or cannot be initialized after openning     */
/* then the function returns 0, otherwise if successful then returns 1 and  */
/* returns the port's file descriptor used by read and write in portfd.     */

int open_port ( const char *port_name, int &portfd )
   {
   int success = 0;
   int initok = 0;
   struct termio term;

   /* Try to open the port for reading and writing.  Configure the port     */
   /* so that read will not wait if no data is present.                     */

   success = ((portfd = open ( port_name, O_RDWR | O_NDELAY )) != -1);

   /* Try to configure the port.                                            */

   if (success)
      {

      memset ( &term, 0, sizeof(term) );

      term.c_cflag = B19200;  /* 19200 Baud rate */
      term.c_iflag = IGNBRK;  /* Ignore break */
      term.c_oflag = 0;
      /* Configure for HUPCL = hang up on last close, CS8 = 8 bits,         */
      /*               CLOCAL = local line, CREAD = enable receiver.        */
      term.c_cflag |= HUPCL | CS8 | CLOCAL | CREAD;
      term.c_lflag = 0;
      term.c_cc[VMIN] = 0;    /* Do not wait for any characters */
      term.c_cc[VTIME] = 2;   /* Wait for 0.2 seconds */

      /* Initialize port with settings.                                     */
      initok = ioctl ( portfd, TCSETA, &term );

      if ( initok < 0 )
         {
         /* If we can't initialize the port, then close it and fail.        */
         close ( portfd );
         success = 0;
         }

      }
   return success;
   } /* end open_port */

/* Function "get_data" reads characters from the port identified by portfd  */
/* into the character array data and returns the number of characters read. */

int get_data ( int portfd, char *data )
   {
   char buffer[1];
   int count = 0;
   char *current_byte;

   buffer[0] = 0;
   current_byte = data; /* Define pointer into current position in array    */

   /* As long as their are characters waiting, keep reading.                */

   while ( read ( portfd, buffer, 1 ) )
      {
      *current_byte++ = *buffer;
      count++;
      }

   return count;

   } /* end get_data */

int get_num_data ( int portfd, int num, char *data )
   {
   char buffer[1];
   int count = 0;
   char *current_byte;

   buffer[0] = 0;
   current_byte = data; /* Define pointer into current position in array    */

   /* As long as their are characters waiting, keep reading.                */

   while ( read ( portfd, buffer, 1 ) && (count < num) )
      {
      *current_byte++ = *buffer;
      count++;
      }

   return count;

   } /* end get_data */

/* Function "send_data" writes num_bytes characters from the character      */
/* array data to the port identified by portfd.  No error checking is done. */

void send_data ( int portfd, char *data, int num_bytes )
   {
   write ( portfd, data, num_bytes );
   } /* end send_data */


/* Function "show_data" writes num_bytes characters from the character      */
/* array data to standard output in hexadecimal form.                       */

void show_data ( char *data, int num_bytes )
   {
   cout << dec << num_bytes << " bytes (hex): " << hex;
   for ( int i = 0; i < num_bytes; i++ )
      cout << int(data[i]) << ".";
   cout << dec << endl;
   } /* end show_data */


int authenticate_old_controls ( int portfd, int tries, float success_rate )
   {
   char buffer[BUFF_SIZE*2];
   char query[1];
   char *current;
   char charmsg[1];
   int num_this_time;
   int retries;
   int done;
   int temp;
   int big_retries;
   int cont;
   int numtries = 0;
   int startmsg;
   int num_tries;
   int num_bytes;
   int success_count = 0;
   float rate;

   for ( num_tries = 0; num_tries < tries; num_tries++ )
      {
      done = 0;
      while ( !done && (numtries++ < tries) )
         {
         /* Empty out the buffer */
         while ( read(portfd,charmsg,1) );
   
         query[0] = QUERY_CHAR;
   
         retries = 0;
         cont = 0;
         big_retries = 0;
      
         /* Signal the HOTAS by sending the query byte FF hex.                 */
         send_data ( portfd, query, 1 );
      
         while ( !cont && (big_retries++ < MAX_POLL_RETRIES) )
            {
            startmsg = 0;
            while ( (!startmsg) && (retries++ < MAX_POLL_RETRIES) )
               startmsg = (read (portfd,charmsg,1) && (charmsg[0] == STOP_CHAR) );
            if ( startmsg )
               {
#ifdef DEBUG
               cerr << "Got initial response." << endl;
#endif
//               send_data ( portfd, query, 1 );
               retries = 0;
               temp = 0;
               while ( !temp && (retries++ < MAX_POLL_RETRIES) )
                  temp = read(portfd,charmsg,1);
               if ( temp && (charmsg[0] != STOP_CHAR ) )
                  {
                  buffer[0] = charmsg[0];
                  num_bytes = 1;
                  cont = 1;
                  }
               }
            }
         if ( cont )
            {
            current = buffer+1; /* Set up a pointer into the character buffer     */
            retries = 0;
      
            /* Constantly read the port for data until a large enough packet is   */
            /* received or until we have tried reading with no success a given    */
            /* number of times.                                                   */
  
            while ( retries < MAX_POLL_RETRIES )
               {
         
               /* Read all the characters that are in the ports input buffer      */
               num_this_time = get_data ( portfd, current );
         
               if ( num_this_time == 0 )
                  retries++;   /* Nothing was there */
               else
                  { /* Something there, add it to what we have already            */
                  retries = 0;
                  num_bytes += num_this_time;
                  current += num_this_time;
                  }
      
               } /* end while */
#ifdef SHOW_DATA
            show_data ( (char *)buffer, num_bytes );
#endif
            if ( num_bytes == PACKET_SIZE )
               done = 1;
            else if ( (num_bytes == (PACKET_SIZE+1) ) &&
                      (buffer[num_bytes-1] == STOP_CHAR) )
               done = 1;
            else
               done = 0;
            }
         }

      if ( done )
         success_count++;

      }

   if ( tries > 0 )
      rate = success_count/float(tries);
   else
      rate = 0;

#ifdef DEBUG
   cerr << "The success rate was = " << rate << ", goal is = "
        << success_rate << endl;
#endif
  
   return ( rate >= success_rate );

   } /* end get_packet */


int authenticate_new_controls ( int portfd, int tries, float success_rate )
   {
   double start_time;
   int startmsg;
   int num_tries = 0;
   char buffer[BUFF_SIZE*2];
   int num_bytes = 0;
   int success_count = 0;
   float rate;   

   for ( num_tries = 0; num_tries < tries; num_tries++ )
      {
      startmsg = 0;
      start_time = get_tod();
      while ( !startmsg && ((get_tod() - start_time) < TIME_OUT) )
         startmsg = get_packet2 ( portfd, buffer, num_bytes );
#ifdef SHOW_DATA
      show_data ( (char *)buffer, num_bytes );
#endif
      if ( startmsg )
         success_count++;
      }

   if ( tries > 0 )
      rate = success_count/float(tries);
   else
      rate = 0;

#ifdef DEBUG
   cerr << "The success rate was = " << rate << ", goal is = "
        << success_rate << endl;
#endif
  
   return ( rate >= success_rate );
   }


int get_packet ( int portfd, char *buffer, int &num_bytes )
   {
   char query[1];
   char *current;
   char charmsg[1];
   int num_this_time;
   int retries;
   int done;
   int temp;
   int big_retries;
   int cont;

   /* Empty out the buffer */
   while ( read(portfd,charmsg,1) );

   query[0] = QUERY_CHAR;

   done = 0;
   retries = 0;
   cont = 0;
   big_retries = 0;

   /* Signal the HOTAS by sending the query byte FF hex.                    */
   send_data ( portfd, query, 1 );

   while ( !cont && (big_retries++ < MAX_POLL_RETRIES) )
      {
      while ( (!done) && (retries++ < MAX_POLL_RETRIES) )
         done = (read (portfd,charmsg,1) && (charmsg[0] == STOP_CHAR) ); 
      if ( done )
         {
         send_data ( portfd, query, 1 );
         retries = 0;
         temp = 0;
         while ( !temp && (retries++ < MAX_POLL_RETRIES) )
            temp = read(portfd,charmsg,1);
         if ( temp && (charmsg[0] != STOP_CHAR ) )
            {
            buffer[0] = charmsg[0];
            num_bytes = 1;
            cont = 1;
            }
         }
      }
      
    
   if ( cont )
   {
   current = buffer+1; /* Set up a pointer into the character buffer          */
   done = 0;
   retries = 0;

   /* Constantly read the port for data until a large enough packet is      */
   /* received or until we have tried reading with no success a given       */
   /* number of times.                                                      */

   while ( (!done) && (retries < MAX_POLL_RETRIES) )
      {

      /* Read all the characters that are in the ports input buffer         */
      num_this_time = get_num_data ( portfd, (PACKET_SIZE - num_bytes),
                                     current );

      if ( num_this_time == 0 )
         retries++;   /* Nothing was there */
      else
         { /* Something there, add it to what we have already               */
         retries = 0;
         num_bytes += num_this_time;
         current += num_this_time;

         /* See if we have enough to stop and if the stop signal has been   */
         /* received in the correct position.                               */
         done = (num_bytes == PACKET_SIZE);
         }
       
      } /* end while */
   return done;
   }
   else
      return 0;

   } /* end get_packet */


int get_packet2 ( int portfd, char *buffer, int &num_bytes )
   {
   char query[1];
   char *current;
   int num_this_time;
   int retries;
   int done;
   char charmsg[1];

   /* Empty out the buffer */
   while ( read(portfd,charmsg,1) );

   query[0] = QUERY_CHAR2;
   current = buffer; /* Set up a pointer into the character buffer          */
   done = 0;
   retries = 0;
   num_bytes = 0;

   /* Signal the HOTAS by sending the query byte FF hex.                    */
   send_data ( portfd, query, 1 );

   /* Constantly read the port for data until a large enough packet is      */
   /* received or until we have tried reading with no success a given       */
   /* number of times.                                                      */

   while ( (!done) && (retries < MAX_POLL_RETRIES) )
      {

      /* Read all the characters that are in the ports input buffer         */
      num_this_time = get_num_data ( portfd, (PACKET_SIZE2 - num_bytes),
                                     current );

      if ( num_this_time == 0 )
         retries++;   /* Nothing was there */
      else
         { /* Something there, add it to what we have already               */
         retries = 0;
         num_bytes += num_this_time;
         current += num_this_time;

         /* See if we have enough to stop and if the stop signal has been   */
         /* received in the correct position.                               */
         done = ( (num_bytes == PACKET_SIZE2) );
// &&
//                  (buffer[num_bytes-1] == STOP_CHAR) );
         }

      } /* end while */
   return done;

   } /* end get_packet */


/* Function "read_file" reads initialization file, if one exists, for the   */
/* initial values of the device valuator components.                        */
/* The function returns 1 if the file can be opened, 0 otherwise.           */
/* The function performs no error checking on the file format.              */

int
fcs::read_file ()
   {

   /* Attempt to open the default file.                                     */
   ifstream fcs_file ( FILENAME );

   if (!fcs_file) return 0; /* Could not be opened */
   else
      { /* file opened, read values */
      fcs_file >> rollinfo.min >> rollinfo.mid >> rollinfo.max;
      if ( fcs_file.fail() )
         return 0;
      fcs_file >> pitchinfo.min >> pitchinfo.mid >> pitchinfo.max;
      if ( fcs_file.fail() )
         return 0;
      fcs_file >> lthrottle.min >> lthrottle.max;
      if ( fcs_file.fail() )
         return 0;
      fcs_file >> rthrottle.min >> rthrottle.max;
      if ( fcs_file.fail() )
         return 0;
      fcs_file >> rudderinfo.min >> rudderinfo.mid >> rudderinfo.max;
      if ( fcs_file.fail() )
         {
         cerr << "FCS Warning:\tNo rudder info in this file." << endl;
         hasrudder = 0;
         rudderinfo.min = 0;
         rudderinfo.mid = 127;
         rudderinfo.max = 255;
         }
      return 1;
      }

   } /* end read_file */


/* Function "write_file" writes the initialization file.  The function      */
/* returns 1 if the file was created or 0 if it cannot create the file.     */

int
fcs::write_file ()
   {
   ofstream fcs_file ( FILENAME, ios::out );
   if (!fcs_file)
      {
      cerr << BELL << "Could not create fcs data file." << endl;
      return 0;
      }
   else
      {
      fcs_file << rollinfo.min << " " << rollinfo.mid << " "
               << rollinfo.max << endl;
      fcs_file << pitchinfo.min << " " << pitchinfo.mid << " "
               << pitchinfo.max << endl;
      fcs_file << lthrottle.min << " " << lthrottle.max << endl;
      fcs_file << rthrottle.min << " " << rthrottle.max << endl;
      fcs_file << rudderinfo.min << " " << rudderinfo.mid << " "
               << rudderinfo.max << endl;
      fcs_file.close();
      return 1;
      }
   } /* end write_file */


int
fcs::open_controls( int tries )
   {
   char query[1];
   char charmsg[1];
   int done;
   int retries;
   int temp;
   double start_time = get_tod();
   int count = 0;
   int identified = 0;

   query[0] = ENQUIRE;
   done = 0;
   retries = 0;

   while ( !done && (retries++ < tries) )
      { 
      while ( !done && ( (get_tod() - start_time) < TIME_OUT) )
         {
         send_data ( portfd, query, 1 );
         temp = read(portfd,charmsg,1);
         sginap(1);
#ifdef TRACE
         cerr << "Initial response " << int(charmsg[0]) << endl;
#endif
         done = ( temp && (charmsg[0] == ACKNOWLEDGE) );
         }
      if ( done )
         {
         done = 0;
         count = 0;
         while ( read(portfd,charmsg,1) )
            {
#ifdef TRACE
            cerr << "Determining product: " << int(charmsg[0]) << endl;
#endif
            sginap(1);
            if ( count == 0 ) 
               {
               temp = 0;
               identified = 0;
               while ( (temp < NUM_PRODUCT_IDS) && (!identified) )
                  {
                  if ( int(charmsg[0]) == KNOWN_PRODUCT_ID[temp] )
                     {
                     identified = 1;
                     product_id = int(charmsg[0]);
                     count++;
#if defined(DEBUG) || defined(TRACE)
                     cerr << "Identified product as " << product_id << endl;
#endif
                     }
                  else
                     temp++;
                  }
               }
            else if ( count == 1 )
               {
               temp = 0;
               identified = 0;
               while ( (temp < NUM_HW_VERSIONS) && (!identified) )
                  {
                  if ( int(charmsg[0]) == KNOWN_HW_VERSIONS[temp] )
                     {
                     identified = 1;
                     hw_version = int(charmsg[0]);
                     count++;
#if defined(DEBUG) || defined(TRACE)
                     cerr << "Identified hw version as " << hw_version << endl;
#endif
                     }
                  else
                     temp++;
                  }
               }
            else if ( count == 2 )
               {
               sw_version = int(charmsg[0]);
#if defined(DEBUG) || defined(TRACE)
               cerr << "Identified sw version as " << sw_version << endl;
#endif
               count++;
               done = 1;
               }
            else if ( count > 2 )
               count++;
#if defined(DEBUG) || defined(TRACE)
            cerr << "Determining product: " << int(charmsg[0]) << endl;
#endif
            }
         }
      }
   return done; 
   }


// Class definition

void sig_handler2(int, ...)
   {
   L_got_data = 1;
   signal ( SIGUSR2, (void (*)(int))sig_handler2 );
   }


/* Function "fcs" is the constructor for the fcs class.  The function       */
/* attempts to open the communications port specified by port_name.  If the */
/* port is opened successfully, the function sets the valid flag to 1 and   */
/* initializes all appropriate member variables.                            */

fcs::fcs ( const char *port_name, int prompt, int multi, int lock, int cpu )
   {
   int stop = 0;
   char key;
   int use_file = 0;
   int file_exists;
   the_cpu = cpu;
   the_lock = lock;

   hasrudder = 0;
   num_fcs++;

   /* Try to open the port and communicate with HOTAS */

   if ( (valid = open_port ( port_name, portfd )) )
      {
#ifdef DEBUG
      cerr << "Trying to open old type controls.\n";
#endif
      valid = authenticate_old_controls ( portfd, 10, 0.75f );
      if (!valid)
         {
         close(portfd);
         valid = open_port ( port_name, portfd ); 
         if ( valid )
            {
#ifdef DEBUG
            cerr << "Trying to open new type controls.\n";
#endif
            valid = authenticate_new_controls ( portfd, 30, 0.70f );
            }
         if ( valid )
            valid = NEW_CONTROLS;
         else
            valid = NO_CONTROLS;
         }
      else
         valid = OLD_CONTROLS;
      if ( valid == OLD_CONTROLS )
         {
#ifdef PRINTVERSION
         cerr << "     FCS: Old style controls detected.\n";
#endif
         }
      else if ( valid == NEW_CONTROLS )
         {
         product_id = -1;
         hw_version = -1;
         sw_version = -1;
#ifdef TRACE
         cerr << "FCS seems to be one of the new type." << endl;
#endif
         open_controls(5);
         if ( product_id != FCS_ID )
            valid = NO_CONTROLS;
         else
            {
            int found = 0;
            int count = 0;
            while ( !found && (count < NUM_HW_VERSIONS) )
               {
               found = (KNOWN_HW_VERSIONS[count] == hw_version);
               count++;
               }
            if (!found)
               {
               cerr << BELL << endl 
                    << "FCS Warning:  Unknown Hardware version: "
                    << hw_version << endl;
               cerr << "   Proceeding but errors are probable.\n\n";
               } 
            }
         if ( valid )
            {
#ifdef PRINTVERSION
            cerr << "     FCS:  New style controls detected.\n";
            cerr << "           Product ID: " << product_id << endl;
            cerr << "           Hardware version: " << hw_version << endl;
            cerr << "           Software version: " << sw_version << endl;
#endif
            }
         }
      }
   else
      {
      cerr << BELL << "Cannot open/initialize port: " << port_name << endl;
      cerr << "Probably due to incorrect read/write permissions." << endl;
      }


   if (valid)
      { /* port was opened */

       /* Initialize member variables to their default states.              */
      data = buffer1;
      hasdata = 0;
      is_reading = 1;
      fcspid = -1;
      killflag = 0;
      rollinfo.deaden = 0.0;
      pitchinfo.deaden = 0.0;
      lthrottle.deaden = 0.0;
      rthrottle.deaden = 0.0;
      rudderinfo.deaden = 0.0;
      rollinfo.sensitive = 1.0;
      pitchinfo.sensitive = 1.0;
      lthrottle.sensitive = 1.0;
      rthrottle.sensitive = 1.0;
      rudderinfo.sensitive = 1.0;
      parent_pid = int(getpid());
      hasrudder = 1;

      if ( file_exists = read_file() )
         { /* Initialization file exists, see if user wants to use it       */
         if ( prompt )
            {
            cout << "An fcs initialization file exists." << endl;
            cout << "Do you want to use that file? (y or n): " << endl;
            cin.get(key);
            cin.get();
            if ( ( key == 'y' ) || ( key == 'Y') )
               use_file = 1;
            }
         else
            {
#ifdef DEBUG
            cout << "Using FCS values from existing file:  " << FILENAME
                 << endl;
#endif
            use_file = 1;
            }
         } /* end if read_file */

      /* If the user does not want to use the file or if one does not exist,*/
      /* then have the user initialize the device.                          */
      if ( !use_file )
          {
          stop = !initialize();
          /* If we are creating the file for the first time, set the file   */
          /* permission to read and write for everyone.                     */
          if ((!stop) && (!file_exists))
             system ( MAKEREADFILE );
          }
      else
         {
         /* Initialize the ranges for valuators if file is used.            */
         deaden_roll ( rollinfo.deaden );
         deaden_pitch ( pitchinfo.deaden );
         deaden_lthrottle ( lthrottle.deaden );
         deaden_rthrottle ( rthrottle.deaden );
         deaden_rudder ( rudderinfo.deaden );
         }

      if (!stop)
         {

         /* See how many processors this machine has                        */
         numcpus = sysmp ( MP_NAPROCS );
         L_numcpus = numcpus;

         /* If this machine has more than one processor and the multi       */
         /* parameter is set, then create the locks and semaphors needed    */
         /* and sproc the FCS read process off to another processor as a    */
         /* lightweight thread.                                             */

         if ( multi )
            {
            char arena_filename[255];
            /* Create an arena file to get the locka and semaphore from     */
           
            sprintf ( arena_filename, "%s.%d.%d", ARENA_FILE,
                                                  getpid(), num_fcs );
            usconfig(CONF_ARENATYPE, US_SHAREDONLY);
            arena = usinit ( arena_filename );
            if ( arena == NULL )
                {
                cerr << BELL << "Could not use arena file:  " << ARENA_FILE 
                     << endl;
                valid = 0;
                }

            else
               {
               /* Set up the arean file with read and write permissions     */
               /* for everyone.                                             */
               usconfig ( CONF_CHMOD, arena, ARENA_PERMISSIONS);

               /* Create a semaphore for suspending the producer code while */
               /* the producer re-initializes the device.                   */

               resetsema = usnewsema ( arena, 1 );
               if ( resetsema == NULL )
                  {
                  cerr << BELL << "Could not obtain reset semaphore from arena."
                       << endl;
                  valid = 0;
                  }

               /* Start the producer code as a lightweight thread who       */
               /* shares all data space with this consumer process.         */
 
               if ( valid )
                  {
                  fcspid = sproc ( poll_continuous, PR_SALL, (void *)this );
                  if ( fcspid == -1 )
                     {
                     perror("Could not start FCS read process -\n   ");
                     valid = 0;
                     }
                  else
                     {
                     signal(SIGCLD,SIG_IGN);
                     signal ( SIGUSR2, (void (*)(int))sig_handler2 );
                     L_got_data = 0;

#ifdef DEBUG
                     cout << "FCS read process spawned:  " << fcspid 
                          << endl;
#endif
                     }
                  }
               }
            }
#ifdef DEBUG
         else
            cout << "FCS read process part of main process." << endl;
#endif
         arena = NULL;
         }

      } /* end if valid */
   else
      cerr << BELL << "FCS not responding on port " << port_name << endl; 

   } /* end fcs */


void
fcs::close_fcs()
   {
   if (valid)
      {

      /* If the producer was started as a lightweight thread, then signal   */
      /* the producer process to die.  Free the lock and semaphore.         */

      if ( fcspid != -1 )
         {
         killflag = 1;
         if (!is_reading)
            usvsema ( resetsema );
         while (killflag);
         signal ( SIGUSR2, SIG_IGN );
#ifdef STATS
         cerr << "Producer packets read from device:  "
              << L_packet_count << endl;
         cerr << "Producer read rate:  "
              << L_packet_count / ( get_tod() - L_start_time ) << endl;
         cerr << "Total bad packets:  " << L_bad_packet_count << endl;
         if ( L_packet_count > 0 )
            cerr << "Percentage of bad packets:  "
                 << L_bad_packet_count/float(L_bad_packet_count+L_packet_count)
                 << endl;
#endif

         sleep(1);
         fcspid = -1;
         usfreesema ( resetsema, arena );
         }

      if ( arena != NULL ) {
         usdetach(arena);
      }

      /* Flush all characters from the FCS port and then close it.          */
      tcflush ( portfd, TCIOFLUSH );
      close ( portfd );
      valid = 0;
      }
   } /* end close_fcs */

int fcs::is_lock()
  {
  return ( the_lock );
  }

int fcs::is_cpu()
  {
  return ( the_cpu );
  }


/* Function "~fcs" is the destructor for the fcs class.  The function       */
/* closes the port previously opened by the constructor (if any).           */

fcs::~fcs ()
   {
   close_fcs();
   } /* end ~fcs */


/* Function "poll_continuous" is the producer code that reads the FCS and   */
/* stores the data when there are more than one processors on the machine   */
/* and the multi parameter was set for the constructor.                     */
/* This process runs until the killflag is set by the consumer.             */

void sig_handler(int, ...)
   {
   if ( !L_copying_buffer )
      {
      if (L_fcs->hasdata)
         {
         L_fcs->data = L_my_buffer;
         if ( L_my_buffer == (char *)L_fcs->buffer1 )
            L_my_buffer = L_fcs->buffer2;
         else
            L_my_buffer = L_fcs->buffer1;
         L_fcs->hasdata = 0;
         if ( L_numcpus > 1 )
            L_got_data = 1;
         else
            kill ( L_fcs->parent_pid, SIGUSR2 ); 
         }
      }
   else
      L_signal = 1;

   signal ( SIGUSR1, (void (*)(int))sig_handler );
   }

void
poll_continuous ( void *tempthis )
   {
   int tries = 5;
   int done = 0;
   int count = 0;
   int num_read;
   char curr_buffer[BUFF_SIZE];
   int good_data;
   static double inter_frame_time = 0.0f;
   static double last_time = 0.0f;
   static double current_time = 0.0f;
   static int time_left;

#ifdef STATS
   L_packet_count = 0;
   L_bad_packet_count = 0;
   L_start_time = get_tod();
#endif

   L_fcs = (fcs *) tempthis;

   signal ( SIGTERM, SIG_DFL );
   signal ( SIGUSR1, (void (*)(int))sig_handler );
   
   if ( L_fcs->data == (char *)L_fcs->buffer1 )
      L_my_buffer = L_fcs->buffer2;
   else
      L_my_buffer = L_fcs->buffer1;

   L_copying_buffer = 0;
   L_fcs->hasdata = 0;
   L_signal = 0;

   if ( L_fcs->the_lock == 1 )
      {
      if ( sysmp(MP_MUSTRUN, L_fcs->the_cpu ) < 0 )
         cerr << " FCS must run Faliure..." << endl;
      }

   while ( !L_fcs->killflag ) {

   done = 0;
   count = 0;

   /* Maintain maximum poll frame rate */
   current_time = get_tod();
   inter_frame_time = current_time - last_time;
   if ( inter_frame_time < MAX_FRAME_TIME )
      {
      time_left = (long)ceil((MAX_FRAME_TIME-inter_frame_time)*CLK_TCK);
      do
         {
         time_left = sginap( time_left );
         }
      while ( time_left > 0 );
      }

   while (( count < tries ) && ( !done ))
      {

      /* Read data into the buffer that is currently not being used.        */
      /* This prevents overwritting good data with possibly bad data.       */

      /* Acquire the semaphore before reading data from the FCS.  This      */
      /* guarantees that the consumer cannot communicate with the FCS at    */
      /* same time via a call to initialize.                                */

      if ( uspsema ( L_fcs->resetsema ) == -1 )
         cerr << BELL << "Error reading reset semaphore." << endl; 
      if ( L_fcs->valid == OLD_CONTROLS )
         good_data = get_packet ( L_fcs->portfd, curr_buffer, num_read );
      else if ( L_fcs->valid == NEW_CONTROLS )
         {
         good_data = get_packet2 ( L_fcs->portfd, curr_buffer, num_read );
//         show_data ( (char *)curr_buffer, num_read );
         }
      else
         good_data = 0;

      usvsema ( L_fcs->resetsema );

      if ( good_data ) 
         {

         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         if ( ( (L_fcs->valid == OLD_CONTROLS) &&
                (num_read == PACKET_SIZE) &&
                (curr_buffer[9]&bit8) ) ||
              ( (L_fcs->valid == NEW_CONTROLS) &&
                (num_read == PACKET_SIZE2) ) )
            {
            L_copying_buffer = 1;
            if ( L_fcs->valid == OLD_CONTROLS )
               bcopy ( (char *)curr_buffer, (char *)L_my_buffer, PACKET_SIZE );
            else
               bcopy ( (char *)curr_buffer, (char *)L_my_buffer, PACKET_SIZE2 );
#ifdef SHOW_DATA
            show_data ( (char *)L_my_buffer, num_read );
#endif
#ifdef STATS
            L_packet_count++;
#endif
            L_copying_buffer = 0;
            if ( L_signal )
               {
               L_signal = 0;
               L_fcs->data = L_my_buffer;
               if ( L_my_buffer == (char *)L_fcs->buffer1 )
                  L_my_buffer = L_fcs->buffer2;
               else
                  L_my_buffer = L_fcs->buffer1;
               L_fcs->hasdata = 0;
               if ( L_numcpus > 1 )
                  L_got_data = 1;
               else
                  kill ( L_fcs->parent_pid, SIGUSR2 );
               }
            else
               L_fcs->hasdata = 1;
            done = 1;
            }

         /* Otherwise, the read failed.  This would mean that > 14 bytes    */
         /* were received and the last character happened to be FF.         */
         else
            {
            count++;
#ifdef STATS
            L_bad_packet_count++;
#endif
#ifdef DEBUG
         cerr << "Bad packet received." << endl;
         show_data ( (char *)curr_buffer, num_read );
#endif 
            }
         }

      /* Read failed.  Advance number of tries.                             */
      else
         {
#ifdef STATS
         L_bad_packet_count++;
#endif
#ifdef DEBUG
         cerr << "Bad packet received." << endl;
         show_data ( (char *)curr_buffer, num_read );
#endif
         count++;
         }

      } /* end while */

   last_time = current_time;

   } /* end while infinite loop */

   signal ( SIGUSR1, SIG_IGN );

#ifdef DEBUG
   cerr << "FCS read process terminating." << endl;
#endif
   L_fcs->killflag = 0;
   exit(0);

 } /* end poll_continous */


/* Function "poll" prompts the device to send its current status and then   */
/* waits for a response.  The function tries to get a valid packet for up   */
/* to the number of times indicated by parameter tries.  If after all tries */
/* a valid packet is not received, the function returns 0.  Otherwise, if a */
/* valid packet is received, the function returns 1.                        */

int
fcs::poll ( int tries )
   {
   int num_read;
   char *curr_buffer;
   int count = 0;
   int done = 0;
   int got_packet;

   /* Force tries to be positive */
   if ( tries < 0 ) 
      tries = 1;

   /* Try to get a valid packet until one is recieved or we have tried      */
   /* without success tries times.                                          */

   while (( count < tries ) && ( !done )) 
      {

      /* Read data into the buffer that is currently not being used.        */
      /* This prevents overwritting good data with possibly bad data.       */

      if ( data == (char *)buffer1 )
         curr_buffer = (char *)buffer2;
      else
         curr_buffer = (char *)buffer1;

      if ( valid == OLD_CONTROLS )
         {
         got_packet = get_packet ( portfd, curr_buffer, num_read );
         }
      else if ( valid == NEW_CONTROLS )
         got_packet = get_packet2 ( portfd, curr_buffer, num_read );
      else
         got_packet = 0;

      if ( got_packet )
         {
         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         if ( ( (valid == OLD_CONTROLS) &&
                (num_read == PACKET_SIZE) &&
                (curr_buffer[9]&bit8) ) ||
              ( (valid == NEW_CONTROLS) &&
                (num_read == PACKET_SIZE2) ) )
            {
            data = curr_buffer;
            done = 1;
#ifdef SHOW_DATA
            show_data ( (char *)curr_buffer, num_read );
#endif
            }

         /* Otherwise, the read failed.  This would mean that > 14 bytes    */
         /* were received and the last character happened to be FF.         */
         else
            count++;
         }

      /* Read failed.  Advance number of tries.                             */
      else
         count++;

      } /* end while */

   return done;

   } /* end poll */


/* Function "new_data" returns true if there is new data since the last     */
/* call to "clear_data".  If the FCS read process is running in parallel    */
/* then we check the hasdata flag.  If the read process is part of this     */
/* process, then we poll the FCS device for data and see if it returns any. */

int
fcs::new_data ()
   {
   if ( (is_reading) && (fcspid == -1) )
      {
      hasdata = poll(5);
      if (!hasdata)
         cerr << BELL << "FCS not responding..." << endl;
      }
   else if (!is_reading)
      hasdata = 0;
   return hasdata;
   }


/* Function "get_data" guarantees that the consumer and the producer are    */
/* using different buffers.                                                 */

void
fcs::get_data ()
   {
   if ( (fcspid != -1) && hasdata )
      {
      kill(fcspid,SIGUSR1);
      while ( !L_got_data );
      L_got_data = 0;
#ifdef TRACE
      if (valid == OLD_CONTROLS)
         {
         show_data ( (char *)data, PACKET_SIZE );
         cerr << "===" << endl;
         }
      else if (valid == NEW_CONTROLS)
         {
         show_data ( (char *)data, PACKET_SIZE2 );
         cerr << "===" << endl;
         }
#endif
      }

   }


/* Function "clear_data" clears the hasdata flag indicating old data is in  */
/* the current buffer.                                                      */

void
fcs::clear_data ()
   {
   }

void
fcs::init_resting ( int &stop, int prompt )
   {
   char keys[5];

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Release the stick to the center and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         pitchinfo.mid = fcsdata(data[1]);
         cout << "Info read - Pitch:  " << pitchinfo.mid << endl;
         rollinfo.mid = fcsdata(data[0]);
         cout << "Info read - Roll:  " << rollinfo.mid << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing resting stick position */
   }


void
fcs::init_stick ( int &stop, int prompt )
   {
   char keys[5];
   fcsdata temp;

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move the stick to the forward, left corner and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         pitchinfo.min = fcsdata(data[1]);
         rollinfo.min = fcsdata(data[0]);
         cout << "Info read - Pitch:  " << pitchinfo.min << endl;
         cout << "Info read - Roll:  " << rollinfo.min << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         } /* end processing forward, left corner */
      }

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move the stick to the forward, right corner and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         temp = fcsdata(data[1]);
         cout << "Info read - Pitch:  " << temp << endl;
         if ( temp < pitchinfo.min )
            {
            pitchinfo.min = temp;
            cout << "Using " << temp << " as min." << endl;
            }
         else
            cout << "Using " << pitchinfo.min << " as min." << endl;
         rollinfo.max = fcsdata(data[0]);
         cout << "Info read - Roll:  " << rollinfo.max << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing forward, right corner */

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move the stick to the rear, left corner and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         pitchinfo.max = fcsdata(data[1]);
         cout << "Info read - Pitch:  " << pitchinfo.max << endl;
         temp = fcsdata(data[0]);
         cout << "Info read - Roll:  " << temp << endl;
         if ( temp < rollinfo.min )
            {
            rollinfo.min = temp;
            cout << "Using " << temp << " as min." << endl;
            }
         else
            cout << "Using " << rollinfo.min << " as min." << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing rear, left corner */

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move the stick to the rear, right corner and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         temp = fcsdata(data[1]);
         cout << "Info read - Pitch:  " << temp << endl;
         if ( temp > pitchinfo.max )
            {
            pitchinfo.max = temp;
            cout << "Using " << temp << " as max." << endl;
            }
         else
            cout << "Using " << pitchinfo.max << " as max." << endl;
         temp = fcsdata(data[0]);
         cout << "Info read - Roll:  " << temp << endl;
         if ( temp > rollinfo.max )
            {
            rollinfo.max = temp;
            cout << "Using " << temp << " as max." << endl;
            }
         else
            cout << "Using " << rollinfo.max << " as max." << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing rear, right corner */
   }


void
fcs::init_throttle ( int &stop, int prompt )
   {
   char keys[5];

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move throttle(s) all the way forward and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         if ( valid == OLD_CONTROLS )
            lthrottle.max = fcsdata(data[4]);
         else
            lthrottle.max = fcsdata(data[5]);
         cout << "Info read - Left Throttle:  " << lthrottle.max << endl;
         if ( valid == OLD_CONTROLS )
            rthrottle.max = fcsdata(data[5]);
         else
            rthrottle.max = fcsdata(data[6]);
         cout << "Info read - Right Throttle:  " << rthrottle.max << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing forward throttle */

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move throttle(s) all the way backward and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         if ( valid == OLD_CONTROLS )
            lthrottle.min = fcsdata(data[4]);
         else
            lthrottle.min = fcsdata(data[5]);
         cout << "Info read - Left Throttle:  " << lthrottle.min << endl;
         if ( valid == OLD_CONTROLS )
            rthrottle.min = fcsdata(data[5]);
         else
            rthrottle.min = fcsdata(data[6]);
         cout << "Info read - Right Throttle:  " << rthrottle.min << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing backward throttle */
   }


void
fcs::init_rudder ( int &stop, int prompt )
   {
   char keys[5];

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move rudder to center, resting position "
              << "and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         if ( valid == OLD_CONTROLS )
            rudderinfo.mid = fcsdata(data[10]);
         else
            rudderinfo.mid = fcsdata(data[12]);
         cout << "Info read - Rudder:  " << rudderinfo.mid << endl;
         }
      else
         {
         cerr << BELL << "Rudder not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      hasrudder = 1;
      } /* end processing center rudder */

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move rudder with left foot all the way forward "
              << "and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         if ( valid == OLD_CONTROLS )
            rudderinfo.max = fcsdata(data[10]);
         else
            rudderinfo.max = fcsdata(data[12]);
         cout << "Info read - Rudder:  " << rudderinfo.max << endl;
         }
      else
         {
         cerr << BELL << "Rudder not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing maximum rudder */

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Move rudder with right foot all the way forward "
              << "and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll(10);
      if ( poll(10) )
         {
         if ( valid == OLD_CONTROLS )
            rudderinfo.min = fcsdata(data[10]);
         else
            rudderinfo.min = fcsdata(data[12]);
         cout << "Info read - Rudder:  " << rudderinfo.min << endl;
         }
      else
         {
         cerr << BELL << "Rudder not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing minimum rudder */

   }


/* Function "initialize" instructs the user to position the stick and       */
/* throttle so their default boundary values can be read.  These values are */
/* then placed in the newly created initialization file.  All relevant      */
/* device data is initialized to their default settings.                    */

int
fcs::initialize ( int item, int prompt )
   {
   int stop = 0;
   int use_file = 0;;
   int sema_set;

   stop = !is_reading;

   if ( stop )
      cerr << BELL << "Initialization attempted on suspended FCS." << endl;

   /* If user does not want to use the initialization file or if one does   */
   /* does not exists, then prompt user to position ball for reading of     */
   /* default extreme values.                                               */

   /* Grab the semaphore before proceeding to guarantee the producer is not */
   /* trying to read from the FCS at the same time when the producer is     */
   /* running in parallel.                                                  */

   if ( (!stop) && (fcspid != -1) )
      {
      sema_set = (uspsema ( resetsema ) != -1); 
      if (!sema_set)
         cerr << BELL << "Could not read reset semaphore." << endl;
      }
   else
      sema_set = 0;

      
   if (( item == FCSRESTING ) || ( item == FCSSTICK ) || 
       ( item == FCSSTICKTHROTTLE ) || ( item == FCSALL ) )
      init_resting(stop,prompt);

   if (( item == FCSSTICK ) || ( item == FCSSTICKTHROTTLE ) || 
       ( item == FCSALL ))
      init_stick(stop,prompt);

   if (( item == FCSTHROTTLE ) || ( item == FCSSTICKTHROTTLE ) || 
       ( item == FCSALL ))
      init_throttle(stop,prompt);

   if (( item == FCSRUDDER ) || ( item == FCSALL ))
      init_rudder(stop,prompt);
 
   /* If new data was read in about the default values then store it to the */
   /* initialization file.                                                  */
 
   if (!stop && !use_file)
      write_file();

   /* Compute new dead zones for the new default valuator ranges.           */

   if (!stop)
      {
      deaden_roll ( rollinfo.deaden );  
      deaden_pitch ( pitchinfo.deaden );
      deaden_lthrottle ( lthrottle.deaden );
      deaden_rthrottle ( rthrottle.deaden );
      deaden_rudder ( rudderinfo.deaden );
      } /* end computing dead zone */

   if ( (fcspid != -1) && (sema_set) )
      usvsema ( resetsema );

   return !stop;

   } /* end initialize */


/* Function "suspend" suspends the parallel, lightweight thread FCS read    */
/* read process and sets the "is_reading" flag to false for the FCS read    */
/* process.                                                                 */

void
fcs::suspend ()
   {
   if ( is_reading )
      {
      if ( fcspid != -1 )
         uspsema ( resetsema );
      is_reading = 0;
      }
   }


/* Function "resume" awakens the lightweight thread FCS read process if it  */
/* has been put to sleep by a call to "suspend".  Also sets the "is_reading"*/
/* flag to true.                                                            */

void
fcs::resume ()
   {
   if (!is_reading )
      {
      if ( fcspid != -1 )
         usvsema ( resetsema );
      is_reading = 1;
      }
   }


/* Function "deaden_roll" defines an area around the stick's resting        */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */
      
void
fcs::deaden_roll ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */
   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the left of the resting position.      */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the roll must exceed to be considered      */
   /*    as moved (by exceed, we mean less than in this case).              */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   rollinfo.negrange = rollinfo.mid - rollinfo.min; 
   amount = fcsdata (percent * double(rollinfo.negrange));
   rollinfo.deadmin = rollinfo.mid - amount;
   rollinfo.negrange -= amount;

   /* In a similar fashion, compute the values for the area to the right of */
   /* of the resting position.                                              */

   rollinfo.posrange = rollinfo.max - rollinfo.mid;
   amount = fcsdata (percent * double(rollinfo.posrange)); 
   rollinfo.deadmax = rollinfo.mid + amount;
   rollinfo.posrange -= amount;

   /* Update the deaden variable in the device information.                 */
   rollinfo.deaden = percent;

   } /* end deaden_roll */


/* Function "deaden_pitch" defines an area around the stick's resting       */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */

void
fcs::deaden_pitch ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the resting position.     */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the pitch must exceed to be considered     */
   /*    as moved (by exceed, we mean less than in this case).              */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   pitchinfo.negrange = pitchinfo.mid - pitchinfo.min;
   amount = fcsdata (percent * double(pitchinfo.negrange));
   pitchinfo.deadmin = pitchinfo.mid - amount;
   pitchinfo.negrange -= amount; 

   /* In a similar fashion, compute the values for the area to the rear of  */
   /* of the resting position.                                              */

   pitchinfo.posrange = pitchinfo.max - pitchinfo.mid;
   amount = fcsdata (percent * double(pitchinfo.posrange)); 
   pitchinfo.deadmax = pitchinfo.mid + amount;
   pitchinfo.posrange -= amount; 

   /* Update the deaden variable in the device information.                 */
   pitchinfo.deaden = percent;

   } /* end deaden_pitch */


/* Function "deaden_lthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
fcs::deaden_lthrottle ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the zero position.        */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the throttle must exceed to be considered  */
   /*    as moved (by exceed, we mean greater than in this case).           */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   lthrottle.posrange = lthrottle.max - lthrottle.min;
   amount = fcsdata (percent * double(lthrottle.posrange));
   lthrottle.deadmin = lthrottle.min + amount;
   lthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   lthrottle.deaden = percent;

   } /* end deaden_lthrottle */


/* Function "deaden_rthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
fcs::deaden_rthrottle ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the zero position.        */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the throttle must exceed to be considered  */
   /*    as moved (by exceed, we mean greater than in this case).           */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   rthrottle.posrange = rthrottle.max - rthrottle.min;
   amount = fcsdata (percent * double(rthrottle.posrange));
   rthrottle.deadmin = rthrottle.min + amount;
   rthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   rthrottle.deaden = percent;

   } /* end deaden_rthrottle */


/* Function "deaden_throttle" is designed for use by non-split throttle    */
/* systems since left and right then have no meaning.                      */

void
fcs::deaden_throttle ( double percent )
   {
   this->deaden_lthrottle ( percent );
   } /* end deaden_throttle */


/* Function "deaden_rudder" defines an area around the rudders's resting    */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */

void
fcs::deaden_rudder ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */
   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the left of the resting position.      */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the roll must exceed to be considered      */
   /*    as moved (by exceed, we mean less than in this case).              */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   rudderinfo.negrange = rudderinfo.mid - rudderinfo.min;
   amount = fcsdata (percent * double(rudderinfo.negrange));
   rudderinfo.deadmin = rudderinfo.mid - amount;
   rudderinfo.negrange -= amount;

   /* In a similar fashion, compute the values for the area to the right of */
   /* of the resting position.                                              */

   rudderinfo.posrange = rudderinfo.max - rudderinfo.mid;
   amount = fcsdata (percent * double(rudderinfo.posrange));
   rudderinfo.deadmax = rudderinfo.mid + amount;
   rudderinfo.posrange -= amount;

   /* Update the deaden variable in the device information.                 */
   rudderinfo.deaden = percent;

   } /* end deaden_rudder */


/* Function "roll_sensitive" sets the roll scale value for the device      */
/* which is used to compute the value for the roll.                        */

void
fcs::roll_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rollinfo.sensitive = percent;
   } /* end roll_sensitive */


/* Function "pitch_sensitive" sets the pitch scale value for the device    */
/* which is used to compute the value for the pitch.                       */

void
fcs::pitch_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   pitchinfo.sensitive = percent;
   } /* end pitch_sensitive */


/* Function "lthrottle_sensitive" sets the lthrottle scale value for the   */
/* device which is used to compute the value for the left throttle.        */

void
fcs::lthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end lthrottle_sensitive */


/* Function "rthrottle_sensitive" sets the rthrottle scale value for the   */
/* device which is used to compute the value for the right throttle.       */

void
fcs::rthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rthrottle.sensitive = percent;
   } /* end rthrottle_sensitive */


/* Function "throttle_sensitive" sets the throttle scale value for the     */
/*device which is used to compute the value for the left throttle.         */
/* Designed for single throttle systems where left and right make no sense.*/

void
fcs::throttle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end throttle_sensitive */


/* Function "rudder_sensitive" sets the rudder scale value for the device  */
/* which is used to compute the value for the rudder.                      */

void
fcs::rudder_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rudderinfo.sensitive = percent;
   } /* end rudder_sensitive */



/* Function "roll" extracts the raw roll infomation from the correct       */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
fcs::roll () const
   {
   fcsdata raw_roll;
   double temp_roll;

   /* Extract the raw information from the correct position of the buffer. */
   raw_roll = fcsdata(data[0]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_roll > rollinfo.max )
      raw_roll = rollinfo.max;
   else if ( raw_roll < rollinfo.min )
      raw_roll = rollinfo.min; 
   
   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_roll < rollinfo.deadmin )
      temp_roll = ( (raw_roll - rollinfo.deadmin) / double(rollinfo.negrange) );
   else if ( raw_roll > rollinfo.deadmax )
      temp_roll = ( (raw_roll - rollinfo.deadmax) / double(rollinfo.posrange) );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_roll = 0;

   /* Return the computed value scaled by the components sensitivity.      */
   return ( float( temp_roll * rollinfo.sensitive ) );

   } /* end roll */


/* Function "pitch" extracts the raw pitch infomation from the correct     */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
fcs::pitch () const
   {
   fcsdata raw_pitch;
   double temp_pitch;
   
   /* Extract the raw information from the correct position of the buffer. */
   raw_pitch = fcsdata(data[1]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_pitch > pitchinfo.max )
      raw_pitch = pitchinfo.max;
   else if ( raw_pitch < pitchinfo.min )
      raw_pitch = pitchinfo.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_pitch < pitchinfo.deadmin )
      temp_pitch = ( (raw_pitch - pitchinfo.deadmin) /
                     double(pitchinfo.negrange)        );
   else if ( raw_pitch > pitchinfo.deadmax )
      temp_pitch = ( (raw_pitch - pitchinfo.deadmax) /
                     double(pitchinfo.posrange)        );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_pitch = 0;

   /* Return the computed value scaled by the components sensitivity.      */
   return ( float( temp_pitch * pitchinfo.sensitive ) );

   } /* end pitch */


float
fcs::rudder () const
   {
   fcsdata raw_rudder;
   double temp_rudder;

   if ( !hasrudder ) return 0.0f;

   /* Extract the raw information from the correct position of the buffer. */
   if ( valid == OLD_CONTROLS )
      raw_rudder = fcsdata(data[10]);
   else
      raw_rudder = fcsdata(data[12]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_rudder > rudderinfo.max )
      raw_rudder = rudderinfo.max;
   else if ( raw_rudder < rudderinfo.min )
      raw_rudder = rudderinfo.min;
  
   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_rudder < rudderinfo.deadmin )
      temp_rudder = ( (raw_rudder - rudderinfo.deadmin) / 
                      double(rudderinfo.negrange) );
   else if ( raw_rudder > rudderinfo.deadmax )
      temp_rudder = ( (raw_rudder - rudderinfo.deadmax) / 
                      double(rudderinfo.posrange) );
   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_rudder = 0.0f;

   /* Return the computed value scaled by the components sensitivity.      */
   return ( float( temp_rudder * rudderinfo.sensitive ) );

   } /* end rudder */


/* Function "stck_thumb_wheel" extracts the raw information from the       */
/* correct position of the data buffer and returns an integer indicating   */
/* its current position as reflected by the directional constants.         */

fcsdata
fcs::stck_thumb_wheel () const
   {
   fcsdata temp_wheel;
   
   /* Extract the raw information from the correct position of the buffer. */
   if ( valid == OLD_CONTROLS )
      {
      temp_wheel = fcsdata(data[2]);

      /* Determine the wheel's direction and return the correct directional */
      /* constant.                                                          */
   
      if ( temp_wheel == 1 )
         return UP;
      else if ( temp_wheel == 2 )
         return DOWN;
      else if ( temp_wheel == 4 )
         return LEFT;
      else if ( temp_wheel == 8 )
         return RIGHT;
      else 
         return MIDDLE;
      }
   else
      {
      temp_wheel = fcsdata(0xFF & (~(0xF0 | (0x0F & data[2]))));
      return temp_wheel;
      }
      


   } /* end stck_thumb_wheel */


/* Function "stck_top_button" returns 1 if the top button on the stick     */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
fcs::stck_top_button () const
   {
   if ( valid == OLD_CONTROLS )
      return ((data[3] & bit7) == 0 ); 
   else 
      return ((data[3] & bit7) == 0 );
   } /* end stck_top_button */


/* Function "stck_mid_button" returns 1 if the middle button on the stick  */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
fcs::stck_mid_button () const
   {
   if ( valid == OLD_CONTROLS )
      return ((data[3] & bit6) == 0 );
   else 
      return ((data[3] & bit6) == 0 );
   } /* end stck_mid_button */


/* Function "stck_bot_button" returns 1 if the bottom button on the stick  */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
fcs::stck_bot_button () const
   {
   if ( valid == OLD_CONTROLS )
      return ((data[3] & bit5) == 0 );
   else
      return ((data[3] & bit5) == 0 );
   } /* end stck_bot_button */


/* Function "stck_trigger" returns 1 if the trigger on the stick is        */
/* pressed or 0 if it is not pressed.                                      */

fcsdata
fcs::stck_trigger () const
   {
   if ( valid == OLD_CONTROLS )
      return ((data[3] & bit8) == 0 );
   else
      return ((data[3] & bit8) == 0 );
   } /* end stck_trigger */


/* Function "left_throttle" extracts the raw infomation from the correct   */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
fcs::left_throttle () const
   {
   fcsdata raw_throttle;
   double temp_throttle;
   
   /* Extract the raw information from the correct position of the buffer. */
   if ( valid == OLD_CONTROLS )
      raw_throttle = fcsdata(data[4]);
   else
      raw_throttle = fcsdata(data[5]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_throttle > lthrottle.max )
      raw_throttle = lthrottle.max;
   else if ( raw_throttle < lthrottle.min )
      raw_throttle = lthrottle.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_throttle > lthrottle.deadmin )
      temp_throttle = ( (raw_throttle - lthrottle.deadmin) /
                        double(lthrottle.posrange)          );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_throttle = 0.0;

   /* Return the computed value scaled by the components sensitivity.      */
   return float (lthrottle.sensitive * temp_throttle); 

   } /* end left_throttle */


/* Function "right_throttle" extracts the raw infomation from the correct  */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
fcs::right_throttle () const
   {
   fcsdata raw_throttle;
   double temp_throttle;

   /* Extract the raw information from the correct position of the buffer. */
   if ( valid == OLD_CONTROLS )
      raw_throttle = fcsdata(data[5]);
   else
      raw_throttle = fcsdata(data[6]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_throttle > rthrottle.max )
      raw_throttle = rthrottle.max;
   else if ( raw_throttle < rthrottle.min )
      raw_throttle = rthrottle.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_throttle > rthrottle.deadmin )
      temp_throttle = ( (raw_throttle - rthrottle.deadmin) /
                        double(rthrottle.posrange)          );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_throttle = 0.0;

   /* Return the computed value scaled by the components sensitivity.      */
   return float (rthrottle.sensitive * temp_throttle);

   } /* end right_throttle */


/* Function "throttle" is designed for non-split throttles since left and  */
/* right do not apply in that case.                                        */

float
fcs::throttle () const
   {
   return this->left_throttle();
   } /* end throttle */


/* Function "thrtl_switch" extracts the raw information from the           */
/* correct position of the data buffer and returns an integer indicating   */
/* its current position as reflected by the directional constants.         */

fcsdata
fcs::thrtl_switch () const
   {
   fcsdata temp_switch;
   
   /* Extract the raw information from the correct position of the buffer. */
   if ( valid == OLD_CONTROLS )
      {
      temp_switch = fcsdata(data[3] & LOWNIBBLE);

      /* Determine the switch's direction and return the correct directional */
      /* constant.                                                           */
   
      if ( temp_switch == 1 )
         return DOWN;
      else if ( temp_switch == 2 )
         return UP;
      else
         return MIDDLE;
      }
   else
      {
      temp_switch = fcsdata(data[3] & LOWNIBBLE);
      if ( (temp_switch & bit1) == 0 )
         return UP;
      else if ( (temp_switch & bit2) == 0 )
         return DOWN;
      else
         return MIDDLE;
      }

   } /* end thrtl_switch */


/* Function "thrtl_dial" extracts the raw information from the correct      */
/* position of the data buffer and returns this raw data.                   */

fcsdata
fcs::thrtl_dial () const
   {
   if ( valid == OLD_CONTROLS )
      return data[6];
   else
      return 0;
   } /* end thrtl_dial */


/* Function "thrtl_ball_az" extracts the raw information from the correct  */
/* position of the data buffer and returns this raw data.                   */

fcsdata
fcs::thrtl_ball_az () const
   {
   if ( valid == OLD_CONTROLS )
      return data[7];
   else
      return 0;
   } /* end thrtl_ball_az */


/* Function "thrtl_ball_el" extracts the raw information from the correct  */
/* position of the data buffer and returns this raw data.                   */

fcsdata
fcs::thrtl_ball_el () const
   {
   if ( valid == OLD_CONTROLS )
      return data[8];
   else
      return 0;
   } /* end thrtl_ball_el */


/* Function "thrtl_but1" returns 1 if the button 1 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but1 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit1) == 0 );
   else
      return 0;
   } /* end thrtl_but1 */


/* Function "thrtl_but2" returns 1 if the button 2 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but2 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit2) == 0 );
   else
      {
      if ( hw_version == 33 )
         return ( (data[4] & bit1) == 0 );
      else
         return ( (data[9] & bit1) == 0 );
      }
   } /* end thrtl_but2 */


/* Function "thrtl_but3" returns 1 if the button 3 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but3 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit3) == 0 );
   else
      {
      if ( hw_version == 33 )
         return ( (data[4] & bit2) == 0 );
      else
         return ( (data[9] & bit2) == 0 );
      }
   } /* end thrtl_but3 */


/* Function "thrtl_but4" returns 1 if the button 4 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but4 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit4) == 0 );
   else
      {
      if ( hw_version == 33 )
         return ( (data[4] & bit3) == 0 );
      else
         return ( (data[9] & bit3) == 0 );
      }
   } /* end thrtl_but4 */


/* Function "thrtl_but5" returns 1 if the button 5 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but5 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit5) == 0 );
   else
      {
      if ( hw_version == 33 )
         return ( (data[4] & bit4) == 0 );
      else
         return ( (data[9] & bit4) == 0 );
      }
   } /* end thrtl_but5 */


/* Function "thrtl_but6" returns 1 if the button 6 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but6 () const
   {
      if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit6) == 0 );
   else
      {
      if ( hw_version == 33 ) 
         return ( (data[4] & bit5) == 0 );
      
      else 
         return ( (data[9] & bit5) == 0 );
      }
   } /* end thrtl_but6 */


/* Function "thrtl_but7" returns 1 if the button 7 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
fcs::thrtl_but7 () const
   {
   if ( valid == OLD_CONTROLS )
      return ( (data[9] & bit7) == 0 );
   else
      {
      if ( hw_version == 33 ) 
         return ( (data[4] & bit6) == 0 );
      else 
         return ( (data[9] & bit6) == 0 );
      }
   } /* end thrtl_but7 */



void
fcs::get_all_inputs ( pfChannel * )
{
   if ( (valid != NO_CONTROLS) && this->new_data() ) {
      this->get_data();
   }
}

int
fcs::button_pressed ( const NPS_BUTTON button_num, int &num_presses )
{
   int success = FALSE;
   num_presses = 0;

   if ( valid ) {
      success = TRUE;
      switch ( button_num ) {
         case NPS_STCK_TRIGGER:
            num_presses = this->stck_trigger();
            break;
         case NPS_STCK_TOP_BUTTON:
            num_presses = this->stck_top_button();
            break;
         case NPS_STCK_MID_BUTTON:
            num_presses = this->stck_mid_button();
            break;
         case NPS_STCK_BOT_BUTTON:
            num_presses = this->stck_bot_button();
            break;
         case NPS_THRTL_BUTTON_1:
            num_presses = this->thrtl_but1();
            break;
         case NPS_THRTL_BUTTON_2:
            num_presses = this->thrtl_but2();
            break;
         case NPS_THRTL_BUTTON_3:
            num_presses = this->thrtl_but3();
            break;
         case NPS_THRTL_BUTTON_4:
            num_presses = this->thrtl_but4();
            break;
         case NPS_THRTL_BUTTON_5:
            num_presses = this->thrtl_but5();
            break;
         case NPS_THRTL_BUTTON_6:
            num_presses = this->thrtl_but6();
            break;
         case NPS_THRTL_BUTTON_7:
            num_presses = this->thrtl_but7();
            break;
         default:
            num_presses = 0;
            success = FALSE;
            break;
      }
   }
   
   return success;
}

int
fcs::valuator_set ( const NPS_VALUATOR valuator_num, float &value )
{
   int success = FALSE;

   if ( valid ) {
      success = TRUE;
      switch ( valuator_num ) {
         case NPS_STCK_HORIZONTAL:
            value = this->roll();
            break;
         case NPS_STCK_VERTICAL:
            value = this->pitch();
            break;
         case NPS_THRTL:
            value = this->throttle();
            break;
         case NPS_THRTL_LEFT:
            value = this->left_throttle();
            break;
         case NPS_THRTL_RIGHT:
            value = this->right_throttle();
            break; 
         case NPS_RUDDER:
            value = this->rudder();
            break;
         default:
            value = 0.0f;
            success = FALSE;
            break;
      }
   }
   return success;
}

int
fcs::switch_set ( const NPS_SWITCH switch_num, NPS_SWITCH_VAL &value )
{
   int success = FALSE;

   if ( valid ) {
      success = TRUE;
      switch ( switch_num ) {
         case NPS_SWITCH_0:
            value = (NPS_SWITCH_VAL)stck_thumb_wheel();
            break;
         case NPS_SWITCH_1:
            value = (NPS_SWITCH_VAL)thrtl_switch();
            break;
         default:
            value = NPS_SWITCH_CENTER;
            success = FALSE;
            break;
      }
   }

   return success;
}


int
fcs::calibrate ( const NPS_VALUATOR which_valuator,
                 const NPS_CALIBRATION which_calibration,
                 const float value )
{
   int success = TRUE;

   switch ( which_valuator ) {
      case NPS_STCK:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(FCSRESTING,0); 
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_STCK_HORIZONTAL:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(FCSRESTING,0); 
               break;
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_roll(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->roll_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_STCK_VERTICAL:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(FCSRESTING,0); 
               break;
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_pitch(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->pitch_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL_LEFT:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_lthrottle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->lthrottle_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL_RIGHT:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_rthrottle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->rthrottle_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_throttle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->throttle_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_RUDDER:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_rudder(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->rudder_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      default:
         success = FALSE;
         break;
   }

   return success;
}

