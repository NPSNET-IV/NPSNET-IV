/*
 * File:  uc_ip_cl_sockcomm.cc                   Version: 001
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

#include "uc_ip_cl_sockcomm.h"

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ctype.h>
#include <errno.h>
extern int errno;


#define IN_ADDR_SIZE 4

UC_IP_CL_SockCommClass::UC_IP_CL_SockCommClass ( const unsigned short port,
                                                 const char *interface,
                                                 const char *friend_ip,
                                                 const int loopback )
   : IP_CL_SockCommClass ( port, interface, loopback, IFF_UP )
{
   if ( sockstate != SC_SOCKET_ERROR ) {

      // Assign network port
      if ( port == 0 ) {
         net_port = DEFAULT_UC_PORT;
      }
      else {
         net_port = port;
      }

      if ( friend_ip ) {
         if ( isdigit(friend_ip[0]) ) {
            strncpy ( partner_ip, friend_ip, NETADDRSIZE-1 );
         }
         else {
            get_ip_given_name(friend_ip,partner_ip);
         }
      }
      else {
         sockstate = SC_SOCKET_ERROR;
      }

   }
}


UC_IP_CL_SockCommClass::~UC_IP_CL_SockCommClass ()
{
   partner_ip[0] = '\0';
}


int
UC_IP_CL_SockCommClass::configure_socket ( int &error_num )
{
   int success = (sockstate != SC_SOCKET_ERROR);
   error_num = 0;
   int on = 1;

   if (sockop == SC_SOCKET_WRITE) { 

      if ( success ) {
         memset ( (void *)&internet_sockaddr,0,
                  (size_t)sizeof(internet_sockaddr) );
         internet_sockaddr.sin_family = AF_INET;
         internet_sockaddr.sin_addr.s_addr = net_inet_addr.s_addr;
         internet_sockaddr.sin_port = htons(0);
      }

      if ( success &&
           ((bind(socketid, (struct sockaddr *) &internet_sockaddr,
                  sizeof(internet_sockaddr))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if ( success ) {
         internet_sockaddr.sin_port = htons(net_port);
         internet_sockaddr.sin_addr.s_addr = inet_addr(partner_ip);
      }

   } // if operation SOCKET_WRITE

   else if (sockop == SC_SOCKET_READ) {

      if ( success ) {
         memset ( (void *)&internet_sockaddr,0,
                  (size_t)sizeof(internet_sockaddr) );
         internet_sockaddr.sin_family = AF_INET;
         internet_sockaddr.sin_addr.s_addr = net_inet_addr.s_addr;
         internet_sockaddr.sin_port = htons(net_port);
      }
 
      if ( success && 
           (setsockopt(socketid,SOL_SOCKET,SO_REUSEADDR,
                     (char *)&on,sizeof(on))) < 0) {
         error_num = errno;
         success = FALSE;
      }

#ifdef __sgi
      if ( success &&
           ((setsockopt(socketid, SOL_SOCKET,
                        SO_REUSEPORT, (char *)&on, sizeof(on))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }
#endif

      if ( success &&
           ((bind(socketid, (struct sockaddr *) &internet_sockaddr,
                  sizeof(internet_sockaddr))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

   if ( success ) {
      internet_sockaddr.sin_addr.s_addr = inet_addr(partner_ip);
   }


   } // if operation SOCKET_READ

   if ( success ) {
      socket_address = (sockaddr *)&(internet_sockaddr);
      address_size = sizeof(internet_sockaddr);
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
UC_IP_CL_SockCommClass::filter ( sockaddr *from )
{
   sockaddr_in *from_in = (sockaddr_in *)from;
   if ((memcmp((void *) &(from_in->sin_addr),
               (void *) &(internet_sockaddr.sin_addr),
               (size_t)IN_ADDR_SIZE)) == 0) {
      return FALSE;
   }
   else {
      return TRUE;
   }

}


int
UC_IP_CL_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( UC_IP_CL_SockCommClass::isaexact(query_kind) ||
                   IP_CL_SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
UC_IP_CL_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UDP_UNICAST);
}


// End file - uc_ip_cl_sockcomm.cc
