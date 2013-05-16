/*
 * File:  up_co_sockcomm.cc                      Version: 001
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

#include "up_co_sockcomm.h"
#include "alarmclock.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;
#include <arpa/inet.h>
#include <net/if.h>


#define MAX_CONNECT_TRIES 240      // 240 tries, once every 0.25 seconds =
                                   // 60 seconds to wait for connect
#define CONNECT_DELAY_SECS 0       // 0 seconds
#define CONNECT_DELAY_USECS 250000 // 0.25 seconds


UP_CO_SockCommClass::UP_CO_SockCommClass ( const char *local_path, 
                                           const char *remote_path )
   : UP_SockCommClass (),
     CO_SockCommClass ()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      if ( local_path ) {
         strncpy ( my_path, local_path, UP_PATH_SIZE );
         my_path[UP_PATH_SIZE] = '\0';
      }
      else {
         sockstate = SC_SOCKET_ERROR;
      }
   
      if ( remote_path ) {
         strncpy ( partner_path, remote_path, UP_PATH_SIZE );
         partner_path[UP_PATH_SIZE] = '\0';
      }
      else {
         sockstate = SC_SOCKET_ERROR;
      }

   }
}


UP_CO_SockCommClass::~UP_CO_SockCommClass ()
{
   unlink(my_path);
   my_path[0] = '\0';
   partner_path[0] = '\0';
}


char *
UP_CO_SockCommClass::get_origin ( sockaddr * )
{
   return partner_path;
}


int
UP_CO_SockCommClass::configure_socket ( int &error_num )
{
   int success = (sockstate != SC_SOCKET_ERROR);
   error_num = 0;
   int temp_socket = 0;
   int connected = FALSE;
   int partner_size = 0;
   int my_size = 0;
   int tries = 0;
   AlarmClockClass my_timer;

   if (sockop == SC_SOCKET_WRITE) { 

      tries = 0;
      while ( (success) && (tries < MAX_CONNECT_TRIES) ) {

         unlink(my_path);
         memset ( (void *)&unix_sockaddr,0,(size_t)sizeof(unix_sockaddr) );
         unix_sockaddr.sun_family = AF_UNIX;
         strcpy(unix_sockaddr.sun_path, my_path );
         my_size = strlen(unix_sockaddr.sun_path) +
                   sizeof(unix_sockaddr.sun_family);
   
         if ( success &&
              ((bind(socketid, (struct sockaddr *) &unix_sockaddr,
                     my_size)) < 0) ) {
            error_num = errno;
            success = FALSE;
         }
  
         if ( success ) { 
            strcpy(unix_sockaddr.sun_path, partner_path);
            partner_size = strlen(unix_sockaddr.sun_path) +
                           sizeof(unix_sockaddr.sun_family);
         }
 
         if ( success &&
              ((connect(socketid, (struct sockaddr *) &unix_sockaddr,
                        partner_size)) < 0) ) {
            error_num = errno;
            if (error_num == ENOENT) {
               close(socketid);
               tries++;
               if ( tries < MAX_CONNECT_TRIES ) {
                  my_timer.sleep(CONNECT_DELAY_SECS,CONNECT_DELAY_USECS);
                  if ( (socketid=socket(sockfamily,socktype,0)) < 0 ) {
                     error_num = errno;
                     success = FALSE;
                     tries = MAX_CONNECT_TRIES;
                  }
               }
               else {
                  success = FALSE; 
               }
            }
            else {
               success = FALSE;
               tries = MAX_CONNECT_TRIES;
            }
         }
      else {
         tries = MAX_CONNECT_TRIES;
      }
   }
   if ( success ) {
      error_num = 0;
   }

   } // if operation SOCKET_WRITE

   else if (sockop == SC_SOCKET_READ) {

      if ( success ) {
         unlink(my_path);
         memset ( (void *)&unix_sockaddr,0,(size_t)sizeof(unix_sockaddr) );
         unix_sockaddr.sun_family = AF_UNIX;
         strcpy(unix_sockaddr.sun_path, my_path );
         my_size = strlen(unix_sockaddr.sun_path) +
                   sizeof(unix_sockaddr.sun_family);
      }

      if ( success &&
           ((bind(socketid, (struct sockaddr *) &unix_sockaddr,
                  my_size)) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if (success && (listen(socketid,5) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      partner_size = sizeof(unix_sockaddr);
      while (success && !connected ) {
         if ( (temp_socket = accept(socketid,
                                    (struct sockaddr *) &unix_sockaddr,
                                    &(partner_size))) < 0) {
            error_num = errno;
            success = FALSE;
         }
         if ( ((strcmp(unix_sockaddr.sun_path, partner_path)) == 0) ) {
            connected = TRUE;
            close ( socketid );
            socketid = temp_socket;
         }
         else {
            close (temp_socket);
            partner_size = sizeof(unix_sockaddr);
         }
 
      }


   } // if operation SOCKET_READ


   if ( success ) {
      socket_address = (sockaddr *)&(unix_sockaddr);
      address_size = partner_size;
   }
   else {
      if ( sockstate == SC_SOCKET_OPEN ) {
         close(socketid);
      }
      sockstate = SC_SOCKET_ERROR;
   }

   return success;
}


int
UP_CO_SockCommClass::filter ( sockaddr * )
{
   return FALSE;
}


int
UP_CO_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( UP_CO_SockCommClass::isaexact(query_kind) ||
                   CO_SockCommClass::isa(query_kind) ||
                   UP_SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
UP_CO_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UNIX_STREAM);
}


// End file - up_co_sockcomm.cc
