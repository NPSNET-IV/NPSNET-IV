// File: <odt.cc>

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
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */
/** File:  odt.cc **/

/* Programmer:  Paul T. Barham, Computer Specialist                         */
/*              Naval Postgraduate School, Computer Science Department      */
/*              Code CS - Barham, Monterey, CA 93943                        */
/*              E-mail:  barham@cs.nps.navy.mil                             */

/* Date:        Changes                                         Programmer: */
/* 8/14/96      Initial cut at getting NPS to talk with ODT.    Barham      */

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
#include <string.h>

#include <bstring.h>
#include <sys/time.h>

#include <math.h>


#include "odt.h"     /* For the ODT C++ class definitions, etc.           */

#ifdef FIXCPLUSPLUS
extern "C" 
{
int ioctl ( int, int, termio* );
}
#endif


/* Define bit masks to extract each bit from a byte using logical AND.      */

#define bit1 (char)0x01
#define bit2 (char)0x02
#define bit3 (char)0x04
#define bit4 (char)0x08
#define bit5 (char)0x10
#define bit6 (char)0x20
#define bit7 (char)0x40
#define bit8 (char)0x80

/* Define bit masks to extract lower and upper nibbles (4 bits) from a byte */
/* using logical AND.                                                       */

#define LOWNIBBLE 0x0f
#define HINIBBLE  0xf0

/* Define constants used for communication with the ODT.  Communications  */
/* format shown below constant definitions:                                 */

#define MAX_BUFFER 255
#define MAX_POLL_RETRIES 20000
#define DELTA_XY_SIGNAL (char) 118    // v
#define ABSOLUTE_XY_SIGNAL (char)112  // p
#define SPEED_SIGNAL (char) 115       // s
#define HOLD_SIGNAL (char) 104        // h
#define RESUME_SIGNAL (char) 114      // r
#define AZIMUTH_SIGNAL (char) 97      // a
#define STOP_CHAR (char) 10           // line feed
#define DATA_SEPARATOR (char) 44      // comma
#define BELL (char) 7

#define MAX_FRAME_TIME 0.0333

#define MASK_TYPE char
#define DELTA_XY_MASK bit1
#define ABSOLUTE_XY_MASK bit2
#define SPEED_MASK bit3
#define HOLD_MASK bit4
#define RESUME_MASK bit5
#define AZIMUTH_MASK bit6


// Local prototypes

static double get_tod();
static int open_port ( const char *port_name, int &portfd );
static int get_data ( int portfd, char *data, int max_bytes );
#if 0
static int get_num_data ( int portfd, int num, char *data );
#endif
static void send_data ( int portfd, char *data, int num_bytes );
#if 0
static void show_data ( char *data, int num_bytes );
#endif
static int get_member ( int portfd, char *buffer,
                        int max_bytes, int &num_bytes );
static int get_packet ( MASK_TYPE ptype, int portfd, char *buffer, int &num_bytes );

static volatile int L_odt_signal;
static volatile odt *L_odt;
static volatile char *L_odt_buffer;
static volatile int L_odt_copying_buffer;
static volatile int L_odt_got_data;
static volatile int L_odt_numcpus;

#ifdef STATS
static volatile long L_odt_packet_count;
static volatile long L_odt_bad_packet_count;
static volatile double L_odt_start_time;
#endif

// Static member variables initializations

int odt::num_odt = 0;


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

      term.c_cflag = B9600;  /* 9600 Baud rate */
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

int get_data ( int portfd, char *data, int max_bytes )
   {
   char buffer[1];
   int count = 0;
   char *current_byte;

   buffer[0] = 0;
   current_byte = data; /* Define pointer into current position in array    */

   /* As long as their are characters waiting, keep reading.                */

   while ( (count < max_bytes) && (read(portfd,buffer,1)) )
      {
      *current_byte++ = *buffer;
      count++;
      }

   return count;

   } /* end get_data */

#if 0
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
#ifdef DEBUG
         cerr << "Got " << int(*buffer) << endl;
#endif
      }

   return count;

   } /* end get_data */
#endif

/* Function "send_data" writes num_bytes characters from the character      */
/* array data to the port identified by portfd.  No error checking is done. */

void send_data ( int portfd, char *data, int num_bytes )
   {
   write ( portfd, data, num_bytes );
   } /* end send_data */

 #if 0
/* Function "show_data" writes num_bytes characters from the character      */
/* array data to standard output in hexadecimal form.                       */

void show_data ( char *data, int num_bytes )
   {
   cout << dec << num_bytes << " bytes (hex): ";
   for ( int i = 0; i < num_bytes; i++ )
      cout << char(data[i]);
   } /* end show_data */
#endif


int get_member ( int portfd, char *buffer, int max_bytes, int &num_bytes )
{
   char *current;
   int num_this_time = 0;
   int retries = 0;
   int done = 0;

   current = buffer; /* Set up a pointer into the character buffer          */
   num_bytes = 0;

   /* Constantly read the port for data until the stop char is received     */

   while ( (!done) && (retries < MAX_POLL_RETRIES) ) {

      /* Read all the characters that are in the ports input buffer         */
      num_this_time = get_data ( portfd, current, (max_bytes-num_bytes) );

      if ( num_this_time == 0 )
         retries++;   /* Nothing was there */
      else { /* Something there, add it to what we have already               */
         retries = 0;
         if ( num_bytes < max_bytes ) {
            num_bytes += num_this_time;
            current += num_this_time;
            done = (*(current-1) == STOP_CHAR);
         }
         else {
            num_bytes = 0;
            done = FALSE;
            retries = MAX_POLL_RETRIES;
         }
      }
   }
   if ( !done ) {
      num_bytes = 0;
   }
   return done;

}

int get_packet ( MASK_TYPE ptype, int portfd, char *buffer, int &num_bytes )
   {
   char query[2];
   char temp_buffer[MAX_BUFFER];
   char *current = temp_buffer;
   char charmsg[1];
   int num_this_time;
   int total_msg_length = 0;

   /* Empty out the buffer */
   while ( read(portfd,charmsg,1) );
   num_this_time = 0;

   if ( ptype & DELTA_XY_MASK ) {
      query[0] = DELTA_XY_SIGNAL;
      query[1] = STOP_CHAR;
      *current++ = query[0];
      send_data ( portfd, query, 2 );
      if ( get_member(portfd,current,
                      (MAX_BUFFER-total_msg_length),num_this_time) ) {
         current += num_this_time;
         total_msg_length += num_this_time;
      }
      else {
         current--;
      }
   }
   if ( ptype & ABSOLUTE_XY_MASK ) {
      query[0] = ABSOLUTE_XY_SIGNAL;
      query[1] = STOP_CHAR;
      *current++ = query[0];
      send_data ( portfd, query, 2 );
      if ( get_member(portfd,current,
                      (MAX_BUFFER-total_msg_length),num_this_time) ) {
         current += num_this_time;
         total_msg_length += num_this_time;
      }
      else {
         current--;
      }
   }
   if ( ptype & SPEED_MASK ) {
      query[0] = SPEED_SIGNAL;
      query[1] = STOP_CHAR;
      *current++ = query[0];
      send_data ( portfd, query, 2 );
      if ( get_member(portfd,current,
                      (MAX_BUFFER-total_msg_length),num_this_time) ) {
         current += num_this_time;
         total_msg_length += num_this_time;
      }
      else {
         current--;
      }
   }
   if ( ptype & AZIMUTH_MASK ) {
      query[0] = AZIMUTH_SIGNAL;
      query[1] = STOP_CHAR;
      *current++ = query[0];
      send_data ( portfd, query, 2 );
      if ( get_member(portfd,current,
                      (MAX_BUFFER-total_msg_length),num_this_time) ) {
         // cerr << "HEADING MESSAGE FROM ODT ==>";
         // for ( int j = 0; j < num_this_time; j++ ) {
            // if ( current[j] == (char)10 ) {
               // cerr << "L";
            // }
            // else {
               // cerr << (char)current[j];
            // }
         // }
         // cerr << "<==" << endl;
         current += num_this_time;
         total_msg_length += num_this_time;
      }
      else {
         current--;
      }
   }

   num_bytes = total_msg_length;
   bcopy ( (char *)temp_buffer, (char *)buffer, num_bytes );
   return(num_bytes > 0);

   } /* end get_packet */



// Class definition

void sig_odt_handler2(int, ...)
   {
   L_odt_got_data = 1;
   signal ( SIGUSR2, (void (*)(int))sig_odt_handler2 );
   }


/* Function "odt" is the constructor for the odt class.  The function       */
/* attempts to open the communications port specified by port_name.  If the */
/* port is opened successfully, the function sets the valid flag to 1 and   */
/* initializes all appropriate member variables.                            */

odt::odt ( const char *port_name, int prompt, int multi, int lock, int cpu )
   {
   (prompt);
   int stop = 0;
   the_cpu = cpu;
   the_lock = lock;
   char temp_buffer[MAX_BUFFER];
   int num_bytes;

   num_odt++;
   arena = NULL;

#ifdef DEBUG
   cerr << "Opening ODT on port " << port_name << endl;
#endif

   /* Try to open the port and communicate with ODT */

   if ( (valid = open_port ( port_name, portfd )) )
      {
#ifdef DEBUG
      cerr << "Port opened. Trying to open ODT communications." << endl;
#endif
      valid = get_packet ( SPEED_MASK|AZIMUTH_MASK, portfd, temp_buffer, num_bytes );
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
      odtpid = -1;
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

      if (!stop)
         {

         /* See how many processors this machine has                        */
         numcpus = sysmp ( MP_NAPROCS );
         L_odt_numcpus = numcpus;

         /* If this machine has more than one processor and the multi       */
         /* parameter is set, then create the locks and semaphors needed    */
         /* and sproc the ODT read process off to another processor as a    */
         /* lightweight thread.                                             */

         if ( multi )
            {
            char arena_filename[255];
            /* Create an arena file to get the locka and semaphore from     */
           
            sprintf ( arena_filename, "%s.%d.%d", ODT_ARENA_FILE,
                                                  getpid(), num_odt );
            usconfig(CONF_ARENATYPE, US_SHAREDONLY);
            arena = usinit ( arena_filename );
            if ( arena == NULL )
                {
                cerr << BELL << "Could not use arena file:  " << ODT_ARENA_FILE 
                     << endl;
                valid = 0;
                }

            else
               {
               /* Set up the arean file with read and write permissions     */
               /* for everyone.                                             */
               usconfig ( CONF_CHMOD, arena, ODT_ARENA_PERMISSIONS);

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
                  odtpid = sproc ( poll_odt_continuous, PR_SALL, (void *)this );
                  if ( odtpid == -1 )
                     {
                     perror("Could not start ODT read process -\n   ");
                     valid = 0;
                     }
                  else
                     {
                     signal(SIGCLD,SIG_IGN);
                     signal ( SIGUSR2, (void (*)(int))sig_odt_handler2 );
                     L_odt_got_data = 0;

#ifdef DEBUG
                     cout << "ODT read process spawned:  " << odtpid 
                          << endl;
#endif
                     }
                  }
               }
            }
#ifdef DEBUG
         else
            cout << "ODT read process part of main process." << endl;
#endif
         }

      } /* end if valid */
   else
      cerr << BELL << "ODT not responding on port " << port_name << endl; 

   } /* end odt */


void
odt::close_odt()
   {
   char query[2];

   if (valid)
      {

      /* If the producer was started as a lightweight thread, then signal   */
      /* the producer process to die.  Free the lock and semaphore.         */

      if ( odtpid != -1 )
         {
         killflag = 1;
         if (!is_reading)
            usvsema ( resetsema );
         while (killflag);
         signal ( SIGUSR2, SIG_IGN );
#ifdef STATS
         cerr << "Producer packets read from device:  "
              << L_odt_packet_count << endl;
         cerr << "Producer read rate:  "
              << L_odt_packet_count / ( get_tod() - L_odt_start_time ) << endl;
         cerr << "Total bad packets:  " << L_odt_bad_packet_count << endl;
         if ( L_odt_packet_count > 0 )
            cerr << "Percentage of bad packets:  "
                 << L_odt_bad_packet_count/float(L_odt_bad_packet_count+L_odt_packet_count)
                 << endl;
#endif

         sleep(1);
         odtpid = -1;
         usfreesema ( resetsema, arena );
         }

      if ( arena != NULL )  
         usdetach(arena);

      query[0] = HOLD_SIGNAL;
      query[1] = STOP_CHAR;
      send_data ( portfd, query, 2 );

      /* Flush all characters from the ODT port and then close it.          */
      tcflush ( portfd, TCIOFLUSH );
      close ( portfd );
      valid = 0;
      }
   } /* end close_odt */

int odt::is_lock()
  {
  return ( the_lock );
  }

int odt::is_cpu()
  {
  return ( the_cpu );
  }


/* Function "~odt" is the destructor for the odt class.  The function       */
/* closes the port previously opened by the constructor (if any).           */

odt::~odt ()
   {
   close_odt();
   } /* end ~odt */


/* Function "poll_odt_continuous" is the producer code that reads the ODT and   */
/* stores the data when there are more than one processors on the machine   */
/* and the multi parameter was set for the constructor.                     */
/* This process runs until the killflag is set by the consumer.             */

void sig_odt_handler(int, ...)
   {
   if ( !L_odt_copying_buffer )
      {
      if (L_odt->hasdata)
         {
         L_odt->data = L_odt_buffer;
         if ( L_odt_buffer == (char *)L_odt->buffer1 )
            L_odt_buffer = L_odt->buffer2;
         else
            L_odt_buffer = L_odt->buffer1;
         L_odt->hasdata = 0;
         if ( L_odt_numcpus > 1 )
            L_odt_got_data = 1;
         else
            kill ( L_odt->parent_pid, SIGUSR2 ); 
         }
      }
   else
      L_odt_signal = 1;

   signal ( SIGUSR1, (void (*)(int))sig_odt_handler );
   }

void
poll_odt_continuous ( void *tempthis )
   {
   int tries = 5;
   int done = 0;
   int count = 0;
   int num_read = 0;
   char curr_buffer[MAX_BUFFER];
   int good_data;
   static double inter_frame_time = 0.0f;
   static double last_time = 0.0f;
   static double current_time = 0.0f;
   static int time_left;
   char query[2];

#ifdef STATS
   L_odt_packet_count = 0;
   L_odt_bad_packet_count = 0;
   L_odt_start_time = get_tod();
#endif

   L_odt = (odt *) tempthis;

   signal ( SIGTERM, SIG_DFL );
   signal ( SIGUSR1, (void (*)(int))sig_odt_handler );
   
   if ( L_odt->data == (char *)L_odt->buffer1 )
      L_odt_buffer = L_odt->buffer2;
   else
      L_odt_buffer = L_odt->buffer1;

   L_odt_copying_buffer = 0;
   L_odt->hasdata = 0;
   L_odt_signal = 0;

   if ( L_odt->the_lock == 1 )
      {
      if ( sysmp(MP_MUSTRUN, L_odt->the_cpu ) < 0 )
         cerr << " ODT must run Faliure..." << endl;
      }

   query[0] = RESUME_SIGNAL;
   query[1] = STOP_CHAR;
   send_data ( L_odt->portfd, query, 2 );

   while ( !L_odt->killflag ) {

   done = 0;
   count = 0;
   num_read = 0;

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

      /* Acquire the semaphore before reading data from the ODT.  This      */
      /* guarantees that the consumer cannot communicate with the ODT at    */
      /* same time via a call to initialize.                                */

      if ( uspsema ( L_odt->resetsema ) == -1 )
         cerr << BELL << "Error reading reset semaphore." << endl; 

      good_data = get_packet ( SPEED_MASK|AZIMUTH_MASK, L_odt->portfd, curr_buffer, num_read );

      usvsema ( L_odt->resetsema );

      if ( good_data ) 
         {

         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         L_odt_copying_buffer = 1;
         bcopy ( (char *)curr_buffer, (char *)L_odt_buffer, num_read );
         L_odt_buffer[num_read] = 0;
#ifdef SHOW_DATA
         show_data ( (char *)L_odt_buffer, num_read );
#endif
#ifdef STATS
         L_odt_packet_count++;
#endif
         L_odt_copying_buffer = 0;
         if ( L_odt_signal )
            {
            L_odt_signal = 0;
            L_odt->data = L_odt_buffer;
            if ( L_odt_buffer == (char *)L_odt->buffer1 )
               L_odt_buffer = L_odt->buffer2;
            else
               L_odt_buffer = L_odt->buffer1;
            L_odt->hasdata = 0;
            if ( L_odt_numcpus > 1 )
               L_odt_got_data = 1;
            else
               kill ( L_odt->parent_pid, SIGUSR2 );
            }
         else
            L_odt->hasdata = 1;
         done = 1;

      }

      /* Read failed.  Advance number of tries.                             */
      else
         {
#ifdef STATS
         L_odt_bad_packet_count++;
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
   cerr << "ODT read process terminating." << endl;
#endif
   L_odt->killflag = 0;
   exit(0);

 } /* end poll_continous */


/* Function "poll" prompts the device to send its current status and then   */
/* waits for a response.  The function tries to get a valid packet for up   */
/* to the number of times indicated by parameter tries.  If after all tries */
/* a valid packet is not received, the function returns 0.  Otherwise, if a */
/* valid packet is received, the function returns 1.                        */

int
odt::poll ( int tries )
   {
   int num_read;
   char *curr_buffer;
   int count = 0;
   int done = 0;
   int got_packet;
   static int first_time = 1;
   char query[2];

   if ( first_time ) {
      query[0] = RESUME_SIGNAL;
      query[1] = STOP_CHAR;
      send_data ( portfd, query, 2 );
      first_time = 0;
#ifdef DEBUG
      cerr << "First time RESUME in poll." << endl;
#endif
   }

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

     got_packet = get_packet ( SPEED_MASK|AZIMUTH_MASK, portfd, curr_buffer, num_read );
      
      if ( got_packet )
         {
         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         curr_buffer[num_read] = 0;
         data = curr_buffer;
         done = 1;
#ifdef SHOW_DATA
         show_data ( (char *)curr_buffer, num_read );
#endif
         }

      /* Read failed.  Advance number of tries.                             */
      else
         count++;

      } /* end while */

   return done;

   } /* end poll */


/* Function "new_data" returns true if there is new data since the last     */
/* call to "clear_data".  If the ODT read process is running in parallel    */
/* then we check the hasdata flag.  If the read process is part of this     */
/* process, then we poll the ODT device for data and see if it returns any. */

int
odt::new_data ()
   {
   if ( (is_reading) && (odtpid == -1) )
      {
      hasdata = poll(5);
      if (!hasdata)
         cerr << BELL << "ODT not responding..." << endl;
      }
   else if (!is_reading)
      hasdata = 0;
   return hasdata;
   }


/* Function "get_data" guarantees that the consumer and the producer are    */
/* using different buffers.                                                 */

void
odt::get_data ()
   {
   if ( (odtpid != -1) && hasdata )
      {
      kill(odtpid,SIGUSR1);
      while ( !L_odt_got_data );
      L_odt_got_data = 0;
      }

   }


/* Function "clear_data" clears the hasdata flag indicating old data is in  */
/* the current buffer.                                                      */

void
odt::clear_data ()
   {
   }


/* Function "suspend" suspends the parallel, lightweight thread ODT read    */
/* read process and sets the "is_reading" flag to false for the ODT read    */
/* process.                                                                 */

void
odt::suspend ()
   {
   if ( is_reading )
      {
      if ( odtpid != -1 )
         uspsema ( resetsema );
      is_reading = 0;
      }
   }


/* Function "resume" awakens the lightweight thread ODT read process if it  */
/* has been put to sleep by a call to "suspend".  Also sets the "is_reading"*/
/* flag to true.                                                            */

void
odt::resume ()
   {
   if (!is_reading )
      {
      if ( odtpid != -1 )
         usvsema ( resetsema );
      is_reading = 1;
      }
   }


/* Function "deaden_roll" defines an area around the stick's resting        */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */
      
void
odt::deaden_roll ( double percent )
   {
   odtdata amount;

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
   amount = odtdata (percent * double(rollinfo.negrange));
   rollinfo.deadmin = rollinfo.mid - amount;
   rollinfo.negrange -= amount;

   /* In a similar fashion, compute the values for the area to the right of */
   /* of the resting position.                                              */

   rollinfo.posrange = rollinfo.max - rollinfo.mid;
   amount = odtdata (percent * double(rollinfo.posrange)); 
   rollinfo.deadmax = rollinfo.mid + amount;
   rollinfo.posrange -= amount;

   /* Update the deaden variable in the device information.                 */
   rollinfo.deaden = percent;

   } /* end deaden_roll */


/* Function "deaden_pitch" defines an area around the stick's resting       */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */

void
odt::deaden_pitch ( double percent )
   {
   odtdata amount;

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
   amount = odtdata (percent * double(pitchinfo.negrange));
   pitchinfo.deadmin = pitchinfo.mid - amount;
   pitchinfo.negrange -= amount; 

   /* In a similar fashion, compute the values for the area to the rear of  */
   /* of the resting position.                                              */

   pitchinfo.posrange = pitchinfo.max - pitchinfo.mid;
   amount = odtdata (percent * double(pitchinfo.posrange)); 
   pitchinfo.deadmax = pitchinfo.mid + amount;
   pitchinfo.posrange -= amount; 

   /* Update the deaden variable in the device information.                 */
   pitchinfo.deaden = percent;

   } /* end deaden_pitch */


/* Function "deaden_lthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
odt::deaden_lthrottle ( double percent )
   {
   odtdata amount;

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
   amount = odtdata (percent * double(lthrottle.posrange));
   lthrottle.deadmin = lthrottle.min + amount;
   lthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   lthrottle.deaden = percent;

   } /* end deaden_lthrottle */


/* Function "deaden_rthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
odt::deaden_rthrottle ( double percent )
   {
   odtdata amount;

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
   amount = odtdata (percent * double(rthrottle.posrange));
   rthrottle.deadmin = rthrottle.min + amount;
   rthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   rthrottle.deaden = percent;

   } /* end deaden_rthrottle */


/* Function "deaden_throttle" is designed for use by non-split throttle    */
/* systems since left and right then have no meaning.                      */

void
odt::deaden_throttle ( double percent )
   {
   this->deaden_lthrottle ( percent );
   } /* end deaden_throttle */


/* Function "deaden_rudder" defines an area around the rudders's resting    */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */

void
odt::deaden_rudder ( double percent )
   {
   odtdata amount;

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
   amount = odtdata (percent * double(rudderinfo.negrange));
   rudderinfo.deadmin = rudderinfo.mid - amount;
   rudderinfo.negrange -= amount;

   /* In a similar fashion, compute the values for the area to the right of */
   /* of the resting position.                                              */

   rudderinfo.posrange = rudderinfo.max - rudderinfo.mid;
   amount = odtdata (percent * double(rudderinfo.posrange));
   rudderinfo.deadmax = rudderinfo.mid + amount;
   rudderinfo.posrange -= amount;

   /* Update the deaden variable in the device information.                 */
   rudderinfo.deaden = percent;

   } /* end deaden_rudder */


/* Function "roll_sensitive" sets the roll scale value for the device      */
/* which is used to compute the value for the roll.                        */

void
odt::roll_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rollinfo.sensitive = percent;
   } /* end roll_sensitive */


/* Function "pitch_sensitive" sets the pitch scale value for the device    */
/* which is used to compute the value for the pitch.                       */

void
odt::pitch_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   pitchinfo.sensitive = percent;
   } /* end pitch_sensitive */


/* Function "lthrottle_sensitive" sets the lthrottle scale value for the   */
/* device which is used to compute the value for the left throttle.        */

void
odt::lthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end lthrottle_sensitive */


/* Function "rthrottle_sensitive" sets the rthrottle scale value for the   */
/* device which is used to compute the value for the right throttle.       */

void
odt::rthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rthrottle.sensitive = percent;
   } /* end rthrottle_sensitive */


/* Function "throttle_sensitive" sets the throttle scale value for the     */
/*device which is used to compute the value for the left throttle.         */
/* Designed for single throttle systems where left and right make no sense.*/

void
odt::throttle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end throttle_sensitive */


/* Function "rudder_sensitive" sets the rudder scale value for the device  */
/* which is used to compute the value for the rudder.                      */

void
odt::rudder_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rudderinfo.sensitive = percent;
   } /* end rudder_sensitive */



/* Function "left_throttle" extracts the raw infomation from the correct   */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
odt::left_throttle () const
   {
   odtdata raw_throttle;
   double temp_throttle;
   
   /* Extract the raw information from the correct position of the buffer. */
   raw_throttle = odtdata(data[4]);

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
odt::right_throttle () const
   {
   odtdata raw_throttle;
   double temp_throttle;

   /* Extract the raw information from the correct position of the buffer. */
   raw_throttle = odtdata(data[5]);

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
odt::throttle () const
   {
   return this->left_throttle();
   } /* end throttle */


int
odt::speed ( float &x_speed, float &y_speed )
{
   char tempstr[MAX_BUFFER];
   char *token;
   int success = 0;
   char *start_msg;

   x_speed = 0.0f;
   y_speed = 0.0f;
   strcpy ( tempstr, (const char *)data );
   start_msg = strchr(tempstr,(int)SPEED_SIGNAL);
   if ( start_msg != NULL ) {
   start_msg++;
      token = strtok ( start_msg, "," );
      if ( token != NULL ) {
         x_speed = atof(token);
         token = strtok ( NULL, "," );
         if ( token != NULL ) {
            y_speed = atof(token);
            success = 1;
         } 
      }
   }
   
   return success;
} /* end speed */


int
odt::azimuth ( float &heading )
{
   char tempstr[MAX_BUFFER];
   char *token;
   int success = 0;
   char *start_msg;
   char end_token[2] = { STOP_CHAR, (char)0 };

   heading = 0.0f;
   strcpy ( tempstr, (const char *)data );
//cerr << "EXTRACTING HEADING FROM STRING " << tempstr << endl;
   start_msg = strchr(tempstr,(int)AZIMUTH_SIGNAL);
//cerr << "ACTUAL HEADING STRINGS STARTS AS " << start_msg << endl;
   if ( start_msg != NULL ) {
      start_msg++;
      token = strtok ( start_msg, end_token );
      if ( token != NULL ) {
//cerr << "EXTRACTED HEADING IS STRING " << token << endl;
         heading = atof(token);
//cerr << "EXTRACTED HEADING IS VALUE " << heading << endl;
         success = 1;
      }
   }

   return success;
} /* end speed */


void
odt::get_all_inputs ( pfChannel * )
{
   if ( (valid) && this->new_data() ) {
      this->get_data();
   }
}

int
odt::button_pressed ( const NPS_BUTTON, int &num_presses )
{
   num_presses = 0;
   return FALSE;
}

int
odt::valuator_set ( const NPS_VALUATOR valuator_num, float &value )
{
   int success = FALSE;

   if ( valid ) {
      success = TRUE;
      switch ( valuator_num ) {
         case NPS_ODT_HEADING:
            success = azimuth(value);
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
odt::switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL & )
{
   return FALSE;
}


int
odt::calibrate ( const NPS_VALUATOR,
                 const NPS_CALIBRATION which_cal,
                 const float )
{
   int success = TRUE;
   char query[2];

   if ( valid ) {
      if ( (which_cal == NPS_PAUSE) || (which_cal == NPS_RESUME) ) {

         if ( odtpid != -1 ) {
            if ( uspsema ( L_odt->resetsema ) == -1 )
               cerr << BELL << "Error reading reset semaphore." << endl;
         }

         if ( which_cal == NPS_PAUSE ) {
            query[0] = HOLD_SIGNAL;
            query[1] = STOP_CHAR;
            send_data ( portfd, query, 2 );
         }
         else if ( which_cal == NPS_RESUME ) {
            query[0] = RESUME_SIGNAL;
            query[1] = STOP_CHAR;
            send_data ( portfd, query, 2 );
         }

         if ( odtpid != -1 ) {
            usvsema ( L_odt->resetsema );
         }
      }
      else {
         success = FALSE;
      }
   }
   else {
      success = FALSE;
   }

   return success;
}

int
odt::get_multival (const NPS_MULTIVAL multi_val, void *data)
{
   int success = FALSE;
   if (multi_val == NPS_MV_ODT_VELOCITY) {
      float *temp_speed = (float *) data;
      success = speed( temp_speed[0], temp_speed[1] );
   }
   return success;
}

