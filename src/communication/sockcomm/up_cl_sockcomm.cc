/*
 * File:  up_cl_sockcomm.cc                      Version: 001
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

#include "up_cl_sockcomm.h"
#include "alarmclock.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;
#include <arpa/inet.h>
#include <net/if.h>


UP_CL_SockCommClass::UP_CL_SockCommClass ( const char *local_path, 
                                           const char *remote_path )
   : UP_SockCommClass (),
     CL_SockCommClass ()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      if ( local_path ) {
         strncpy ( my_path, local_path, UP_PATH_SIZE );
         my_path[UP_PATH_SIZE] = '\0';
      }
      else {
         strcpy ( my_path, "" );
         sockstate = SC_SOCKET_ERROR;
      }

      if ( remote_path ) {
         strncpy ( partner_path, remote_path, UP_PATH_SIZE );
         partner_path[UP_PATH_SIZE] = '\0';
      }
      else {
         strcpy ( partner_path, "" );
         sockstate = SC_SOCKET_ERROR;
      }

   }
}


UP_CL_SockCommClass::~UP_CL_SockCommClass ()
{
   unlink(my_path);
   my_path[0] = '\0';
   partner_path[0] = '\0';
}


char *
UP_CL_SockCommClass::get_origin ( sockaddr *from )
{
   struct sockaddr_un *from_unix = (sockaddr_un *)from;
   from_unix->sun_path[UP_PATH_SIZE] = '\0';
   return from_unix->sun_path;
}


int
UP_CL_SockCommClass::configure_socket ( int &error_num )
{
   int success = (sockstate != SC_SOCKET_ERROR);
   error_num = 0;
   int sock_size = 0;
   AlarmClockClass my_timer;

   if ( (sockop == SC_SOCKET_WRITE) || (sockop == SC_SOCKET_READ) ) {

      if ( success ) {
         unlink(my_path);
         memset ( (void *)&unix_sockaddr,0,(size_t)sizeof(unix_sockaddr) );
         unix_sockaddr.sun_family = AF_UNIX;
         strcpy(unix_sockaddr.sun_path, my_path );
         sock_size = strlen(unix_sockaddr.sun_path) +
                     sizeof(unix_sockaddr.sun_family);
      }
   
      if ( success &&
           ((bind(socketid, (struct sockaddr *) &unix_sockaddr,
                  sock_size)) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if (success && (sockop == SC_SOCKET_WRITE)) {
         strcpy(unix_sockaddr.sun_path, partner_path);
         sock_size = strlen(unix_sockaddr.sun_path) +
                     sizeof(unix_sockaddr.sun_family);
      }

      if ( success ) {
         socket_address = (sockaddr *)&(unix_sockaddr);
         address_size = sock_size;
      }
 
   } // if operation SOCKET_WRITE or SOCKET_READ

   if ( !success ) {
      if ( sockstate == SC_SOCKET_OPEN ) {
         close(socketid);
      }
      sockstate = SC_SOCKET_ERROR;
   }

   return success;
}


int
UP_CL_SockCommClass::filter ( sockaddr * )
{
   return FALSE;
}


int
UP_CL_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( UP_CL_SockCommClass::isaexact(query_kind) ||
                   CL_SockCommClass::isa(query_kind) ||
                   UP_SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
UP_CL_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UNIX_DATAGRAM);
}


// End file - up_cl_sockcomm.cc
