/*
 * File:  ip_co_sockcomm.cc                      Version: 001
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


#include "ip_co_sockcomm.h"
#include "alarmclock.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;
#include <arpa/inet.h>
#include <net/if.h>
#include <ctype.h>
#include <iostream.h>

#define IN_ADDR_SIZE 4

#define MAX_CONNECT_TRIES 14400 //   240
#define CONNECT_DELAY_SECS 1
#define CONNECT_DELAY_USECS 0

//#define CONNECT_DELAY_SECS 0
//#define CONNECT_DELAY_USECS 250000


IP_CO_SockCommClass::IP_CO_SockCommClass ( const unsigned short port,
                                           const char *interface,
                                           const char *friend_ip )
   : IP_SockCommClass (port,interface,FALSE,IFF_UP),
     CO_SockCommClass ()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      // Assign network port
      if ( port == 0 ) {
         net_port = DEFAULT_TCP_PORT;
      }
      else {
         net_port = port;
      }

      if ( friend_ip ) {
         if ( isdigit(friend_ip[0]) ) {
            strncpy ( partner_ip, friend_ip, NETADDRSIZE );
         }
         else {
            get_ip_given_name(friend_ip,partner_ip);
         }
      }
      else {
         sockstate = SC_SOCKET_ERROR;
         strcpy ( partner_ip, "" );
      }
   }
}


IP_CO_SockCommClass::~IP_CO_SockCommClass ()
{
   partner_ip[0] = '\0';
}


char *
IP_CO_SockCommClass::get_origin ( sockaddr * )
{
   return partner_ip;
}


int
IP_CO_SockCommClass::configure_socket ( int &error_num )
{
   int success = TRUE;
   error_num = 0;
   int on = 1;
   int temp_socket = 0;
   int connected = FALSE;
   int partner_size = 0;
   int tries = 0;
   AlarmClockClass my_timer;

   if (sockop == SC_SOCKET_WRITE) { 

      tries = 0;
      while ( (success) && (tries < MAX_CONNECT_TRIES) ) {

         memset ( (void *)&internet_sockaddr, 0,
                  (size_t)sizeof(internet_sockaddr) );
         internet_sockaddr.sin_family = AF_INET;
         internet_sockaddr.sin_addr.s_addr = net_inet_addr.s_addr;
         internet_sockaddr.sin_port = htons(net_port);
   
         if ( success &&
              ((bind(socketid, (struct sockaddr *) &internet_sockaddr,
                     sizeof(internet_sockaddr))) < 0) ) {
            error_num = errno;
            success = FALSE;
            close(socketid);
         }
   
         internet_sockaddr.sin_addr.s_addr = inet_addr(partner_ip);
 
         if ( success &&
              ((connect(socketid, (struct sockaddr *) &internet_sockaddr,
                     sizeof(internet_sockaddr))) < 0) ) {
            error_num = errno;
            if ( (error_num == ECONNREFUSED) ||
                 (error_num == ETIMEDOUT) ) {
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
                  close(socketid);
               }
            }
            else {
               success = FALSE;
               close(socketid);
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

      memset ( (void *)&internet_sockaddr,0,(size_t)sizeof(internet_sockaddr) );
      internet_sockaddr.sin_family = AF_INET;
      internet_sockaddr.sin_addr.s_addr = net_inet_addr.s_addr;
      internet_sockaddr.sin_port = htons(net_port);

      if (setsockopt(socketid,SOL_SOCKET,SO_REUSEADDR,
                     (char *)&on,sizeof(on)) < 0) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }

#ifdef __sgi
      if ( success &&
           ((setsockopt(socketid, SOL_SOCKET,
                        SO_REUSEPORT, (char *)&on, sizeof(on))) < 0) ) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }
#endif


      if ( success &&
           ((bind(socketid, (struct sockaddr *) &internet_sockaddr,
                  sizeof(internet_sockaddr))) < 0) ) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }

      if (success && (listen(socketid,5) < 0) ) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }

      partner_size = sizeof(internet_sockaddr);
      while (success && !connected ) {
         if ( (temp_socket = accept(socketid,
                                    (struct sockaddr *) &internet_sockaddr,
                                    &(partner_size))) < 0) {
            error_num = errno;
            success = FALSE;
            close(socketid);
         }
         if ( ((strcmp(inet_ntoa(internet_sockaddr.sin_addr),
                       partner_ip)) == 0) ) {
            connected = TRUE;
            close ( socketid );
            socketid = temp_socket;
         }
         else {
cerr << "Connect from someone other than my partner." << endl;
            close (temp_socket);
            partner_size = sizeof(internet_sockaddr);
         }
 
      }


   } // if operation SOCKET_READ

   if ( success ) {
      struct linger opts;
      socket_address = (sockaddr *)&(internet_sockaddr);
      address_size = sizeof(internet_sockaddr);

      if (setsockopt(socketid,SOL_SOCKET,SO_KEEPALIVE,
                     (char *)&on,sizeof(on)) < 0) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }
      opts.l_onoff = 1;
      opts.l_linger = 0;
      if (setsockopt(socketid,SOL_SOCKET,SO_LINGER,
                     (char *)&opts,sizeof(opts)) < 0) {
         error_num = errno;
         success = FALSE;
         close(socketid);
      }

   }
   else {
      sockstate = SC_SOCKET_ERROR;
   }

   return success;
}


int
IP_CO_SockCommClass::filter ( sockaddr * )
{
   return FALSE;
}


int
IP_CO_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( IP_CO_SockCommClass::isaexact(query_kind) ||
                   CO_SockCommClass::isa(query_kind) ||
                   IP_SockCommClass::isa(query_kind) ||
                   (query_kind == SC_TCP) );
   return (truth_value);
}


int
IP_CO_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_TCP_STREAM);
}


// End file - ip_co_sockcomm.cc
