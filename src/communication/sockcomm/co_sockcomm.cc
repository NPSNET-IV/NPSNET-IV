/*
 * File:  co_sockcomm.cc                         Version: 001
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


#include "co_sockcomm.h"
#include <unistd.h>
#include <errno.h>
extern int errno;


CO_SockCommClass::CO_SockCommClass ()
   : SockCommClass()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      socktype = SOCK_STREAM;
   }
}


CO_SockCommClass::~CO_SockCommClass ()
{
}


int
CO_SockCommClass::write_to_socket ( const char *buffer, 
                                    const unsigned int length,
                                    int &error_num ) 
{
   int bytes_sent = 0;
   int nleft = length;
   int error = FALSE;
   char *ptr = (char *)buffer;
   error_num = 0;

   while ( (nleft > 0) && !error ) {
      bytes_sent = send(socketid, ptr, nleft,0x0);
      if ( bytes_sent == -1 ) {
         error_num = errno;
         error = TRUE;
      }
      nleft -= bytes_sent;
      ptr += bytes_sent;
   }

   if ( error ) {
      return (-1);
   }
   else {
      return (length - nleft);
   }
   
}


int
CO_SockCommClass::read_from_socket ( char *buffer, struct sockaddr &,
                                     const unsigned int buffer_size,
                                     int &error_num )
{
   int bytes_read = 0;
   int nleft = buffer_size;
   int error = FALSE;
   char *ptr = buffer;
   error_num = 0;

   while ( (nleft > 0) && (!error) ) {
      bytes_read = recv(socketid, ptr, nleft,0x0);
      if ( bytes_read <= 0  ) {
         error_num = errno;
         error = TRUE;
      }
      else {
         nleft -= bytes_read;
         ptr += bytes_read;
      }
   } 

   if ( error ) {
      return (-1);
   }
   else {
      return (buffer_size - nleft);
   }

}


int
CO_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( CO_SockCommClass::isaexact(query_kind) || 
                   SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
CO_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_CONNECT_ORIENTED);
}


// End File - co_sockcomm.cc
