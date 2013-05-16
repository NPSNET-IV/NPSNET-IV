/*
 * File:  sockcomm.cc                            Version: 00
 * Created By:  Paul T. Barham                   Date:  April 4, 1996
 * Modifications By:
 *    Name             Date               Mods:
 *    ---------------- ------------------ -------------------------------
 *
 * Copyright (c) 1996,
 *    Naval Postgraduate School
 *    Computer Science Department
 *    NPSNET Research Group
 *    Monterey, California 93943
 *    npsnet@cs.nps.navy.mil
 *    http://www-npsnet.cs.nps.navy.mil/npsnet/
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
 * Thank you to our current and past sponsors:
 *    ARL, ARPA, DMSO, NPS Direct Funding, NRaD, STRICOM, TEC and TRAC.
 *
 */


#include "sockcomm.h"
#include <iostream.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;

#define NUM_SEND_TRIES 1

#ifdef __sgi
#define SEND_BUFFER_SIZE 32768
#define RECEIVE_BUFFER_SIZE 32768
#endif


void
SockCommClass::set_socket_to_default ()
{
   socketid = -1;
   socket_address = NULL;
   address_size = 0;
   sockfamily = 0;
   socktype = 0;
   sockop = SC_SOCKET_DEFAULT;
   sockstate = SC_SOCKET_CLOSED;
}


SockCommClass::SockCommClass ( )
{
   set_socket_to_default();
}


SockCommClass::~SockCommClass ()
{
   int error_num = 0;
   if ( (sockstate == SC_SOCKET_OPEN) && (!close_socket(error_num)) ) {
      print_error ( "SockCommClass - destructor closing socket", error_num );
   }
   set_socket_to_default();
}


int
SockCommClass::open_communication ( const SockOperation socket_operation )
{
   int error_num = 0;
   int success = (sockstate != SC_SOCKET_ERROR);
  

   if ( success && (sockstate == SC_SOCKET_OPEN) ) {
      success = close_communication();
   }
 
   if ( success  ) {
      success = ( (socket_operation == SC_SOCKET_READ) ||
                  (socket_operation == SC_SOCKET_WRITE) );
   }
           
   if ( success &&
        (!open_socket ( socket_operation, error_num )) ) {
      success = FALSE;
      print_error ( "SockCommClass - open_communication", error_num );
   }


   if ( success && 
        (!configure_socket(error_num)) ) {
      close_communication();
      success = FALSE;
      print_error ( "SockCommClass - open_communication", error_num );
   }
    
 
   return success; 
}


int
SockCommClass::close_communication ()
{
   int success = TRUE;
   int error_num = 0;

   if ( !close_socket(error_num) ) {
      print_error ( "SockCommClass - close_communication", error_num );
      success = FALSE;
   }
 
   return success;
}


unsigned int
SockCommClass::write_buffer ( const char *buffer , const unsigned int length)
{
   int sendcount = 0;
   int bytes_sent = 0;
   int error_num = 0;

   if ( (sockstate == SC_SOCKET_OPEN) && (sockop == SC_SOCKET_WRITE) ) {
      while ( (sendcount++ < NUM_SEND_TRIES ) &&
              ((bytes_sent = write_to_socket(buffer,length,error_num)) < 0) ) {
         print_error ( "SockCommClass - write_buffer", error_num );
         bytes_sent = 0;
      }
   }

   return (unsigned int)bytes_sent;
   
}


unsigned int
SockCommClass::read_buffer ( char *buffer, const unsigned int length, 
                             char *sender )
{
   struct sockaddr from;
   int error_num = 0;
   int bytes_read = 0;
   int done = FALSE;

   if ( (sockstate == SC_SOCKET_OPEN) && (sockop == SC_SOCKET_READ) ) {
      while ( !done ) {
         if ((bytes_read = 
                 read_from_socket(buffer,from,length,error_num)) < 0 ) {
            print_error ( "SockCommClass - read_buffer", error_num );
            done = TRUE;
            bytes_read = 0;
         }
         else {
            if ( !filter(&from) ) {
               strcpy ( sender, get_origin(&from) );
               done = TRUE;
            }
         }
      }
   }

   return (unsigned int)bytes_read;

}


void
SockCommClass::print_error ( const char *origin_string, const int error_num )
{
   char *error_str = NULL;

   error_str = strerror(error_num);
   cerr << "Socket Communication Error from **" 
        << origin_string << "**" << endl;
   cerr << "\t" << error_str << endl;

}


int
SockCommClass::open_socket ( const SockOperation socket_operation,
                             int &error_num )
{
   int success = TRUE;
#ifdef __sgi
   int sendbuff = SEND_BUFFER_SIZE;
   int rcvbuff = RECEIVE_BUFFER_SIZE;
#endif

   if ( (socketid=socket(sockfamily,socktype,0)) < 0 ) {
      error_num = errno;
      success = FALSE;
   }
   else {
      sockstate = SC_SOCKET_OPEN;
      sockop = socket_operation;
   }

#ifdef __sgi
   if ( (sockstate == SC_SOCKET_OPEN) && (sockop == SC_SOCKET_WRITE) ) {
      // Resize the socket send buffer to help prevent overflow
      if ( (setsockopt(socketid,SOL_SOCKET,SO_SNDBUF,
                       (char *)&sendbuff, sizeof(sendbuff))) < 0 ) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }
   }
   else if ( (sockstate == SC_SOCKET_OPEN) && (sockop == SC_SOCKET_READ) ) {
      // Resize the socket receive buffer to help prevent overflow 
      if ( (setsockopt(socketid,SOL_SOCKET,SO_RCVBUF,
                       (char *)&rcvbuff, sizeof(rcvbuff))) < 0 ) {
         error_num = errno; 
         success = FALSE;
         close(socketid);
      }
   }
#endif

   if (!success) {
      sockstate = SC_SOCKET_ERROR;
      sockop = SC_SOCKET_DEFAULT;
   }

   return success;
   
}


int
SockCommClass::close_socket ( int &error_num )
{
   int success = TRUE;

   error_num = 0;
   if ( sockstate == SC_SOCKET_OPEN ) {
      if ( (close(socketid)) == -1 ) {
         error_num = errno;
         success = FALSE;
         sockstate = SC_SOCKET_ERROR;
      }
      else {
         sockstate = SC_SOCKET_CLOSED;
      }
   }
   else if ( sockstate != SC_SOCKET_CLOSED ) {
      success = FALSE;
   }

   return success;
}


int
SockCommClass::isa ( SockKind query_kind )
{
   return SockCommClass::isaexact(query_kind);
}


int
SockCommClass::isaexact ( SockKind query_kind )
{
   return ( query_kind == SC_SOCKET );
}


int
SockCommClass::get_sock_id ()
{
   return socketid;
}


// End File - sockcomm.cc
