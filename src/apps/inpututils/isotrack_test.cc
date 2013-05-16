// File: <track.cc>

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
#include <iostream.h>/* For C++ standard I/O stuff                          */
#include <fstream.h> /* For C++ file I/O stuff                              */
#include <unistd.h>  /* For standard Unix read, write stuff                 */
#include <fcntl.h>   /* For file constant definitions and flags             */
#include <termio.h>  /* For terminal I/O stuff                              */
#include <termios.h> /* For terminal I/O stuff                              */
#include <stdlib.h>
#include <sys/types.h> /* For system type stuff                             */
#include <sys/prctl.h> /* For process control stuff                         */
#include <sys/signal.h>/* For process signal stuff                          */
#include <sys/sysmp.h> /* For system multi-process stuff                    */
#include <ulocks.h>
#include <string.h>


#define MAX_PACKET_SIZE 125
#define PACKET_SIZE 47
#define MAX_POLL_RETRIES 1200
#define QUERY_CHAR 'P'
#define COSINE_NUM_BYTES 21
#define POSITION_NUM_BYTES 21
#define ORIENTATION_NUM_BYTES 21
#define QUARTERNION_NUM_BYTES 28

int open_port ( char *port_name, int &portfd )
   {
   int success = 0;
   int initok = 0;
   struct termio term;

   /* Try to open the port for reading and writing.  Configure the port     */
   /* so that read will not wait if no data is present.                     */

   success = ((portfd = open ( port_name, O_RDWR /* | O_NDELAY */ )) != -1);

   /* Try to configure the port.                                            */

   if (success)
      {

      memset ( &term, 0, sizeof(term) );

      term.c_cflag = B9600;  /* 9600 Baud rate */
      term.c_iflag = IGNBRK | IXOFF;  /* Ignore break and Xon/Xoff input  */
      term.c_oflag = 0;
      /* Configure for HUPCL = hang up on last close, CS8 = 8 bits,         */
      /*               CLOCAL = local line, CREAD = enable receiver.        */
      term.c_cflag |= HUPCL | CS8 | CLOCAL | CREAD;
      term.c_lflag = ICANON; /* Canonical input mode */
      term.c_cc[VMIN] = 0;    /* Do not wait for any characters */
      term.c_cc[VTIME] = 0;   /* Do not wait for any lenth of time */

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
   int count = 0;

   count = read ( portfd, data, MAX_PACKET_SIZE );

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
   cerr << dec << num_bytes << " bytes (hex): ";
   for ( int i = 0; i < num_bytes; i++ )
      cerr << data[i] << "|";
   } /* end show_data */


int get_packet ( int portfd, char *buffer, int &num_bytes )
   {
   char query[1] =  { QUERY_CHAR };
   char *current;
   int retries;
   int done;

   /* Signal the HOTAS by sending the query byte FF hex.                    */
   send_data ( portfd, query, 1 );

   current = buffer; /* Set up a pointer into the character buffer          */
   done = 0;
   retries = 0;
   num_bytes = 0;

   /* Constantly read the port for data until a large enough packet is      */
   /* received or until we have tried reading with no success a given       */
   /* number of times.                                                      */

   while ( (!done) && (retries < MAX_POLL_RETRIES) )
      {

      /* Read all the characters that are in the ports input buffer         */
      num_bytes = get_data ( portfd, current );

      if ( num_bytes == 0 )
         {
         retries++;   /* Nothing was there */
         cerr << "nothing there..." << endl;
         }
      else
         { /* Something there, add it to what we have already               */
         retries = 0;
         done = (num_bytes == PACKET_SIZE);
         cerr << "only " << num_bytes << " received..." << endl;
         }

      } /* end while */

   return done;

   } /* end get_packet */

void 
main ( int argc, char **argv )
{
int isofd;
char buffer[MAX_PACKET_SIZE];
int num_bytes;
int count = 0;
char portname[50];
int loops;

if ( argc > 1 )
   strcpy ( portname, argv[1] );
else
   strcpy ( portname, "/dev/ttyd4" );

if ( argc > 2 )
   loops = atoi(argv[2]);
else
   loops = 20;

cerr << "Opening ISOTRACK on serial port: " << portname << endl;

if (!open_port ( portname, isofd ))
   {
   cerr << "Can't open port" << endl;
   exit(0);
   }

while (count < loops) 
   {
   if ( get_packet ( isofd, buffer, num_bytes ) )
      {
      show_data ( buffer, num_bytes );
      count++;
      }
   else
      cerr << "Dropped a packet!!\n";
   } 

close ( isofd );

exit(0);

}
