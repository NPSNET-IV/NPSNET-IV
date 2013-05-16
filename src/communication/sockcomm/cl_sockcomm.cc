/*
 * File:  cl_sockcomm.cc                         Version: 001
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


#include "cl_sockcomm.h"

#include <errno.h>
extern int errno;


CL_SockCommClass::CL_SockCommClass ()
   : SockCommClass()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      socktype = SOCK_DGRAM;
   }
}


CL_SockCommClass::~CL_SockCommClass ()
{
}


int
CL_SockCommClass::write_to_socket ( const char *buffer, 
                                    const unsigned int length,
                                    int &error_num ) 
{
   int bytes_sent = 0;
   error_num = 0;

   if ( (bytes_sent = sendto(socketid, buffer, length, 0,
                             (sockaddr *)socket_address,
                             address_size)) == -1 ) {
      error_num = errno;
   }

   return bytes_sent;
   
}


int
CL_SockCommClass::read_from_socket ( char *buffer, struct sockaddr &from,
                                     const unsigned int buffer_length,
                                     int &error_num )
{
   int len = sizeof(from);
   int bytes_read = 0;
   error_num = 0;

   if ( (bytes_read = recvfrom(socketid, buffer,
                               buffer_length, 0,
                               (struct sockaddr *) &from, &len)) == -1 ) {
      error_num = errno;
   } 

   return bytes_read;

}

int
CL_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( CL_SockCommClass::isaexact(query_kind) || 
                   SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
CL_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_CONNECT_LESS);
}


// End File - cl_sockcomm.cc
