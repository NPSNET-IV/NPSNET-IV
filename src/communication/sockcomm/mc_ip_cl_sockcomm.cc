/*
 * File:  mc_ip_cl_sockcomm.cc                   Version: 001
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

#include "mc_ip_cl_sockcomm.h"
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
extern int errno;


#define IN_ADDR_SIZE 4


MC_IP_CL_SockCommClass::MC_IP_CL_SockCommClass ( const unsigned short port,
                                                 const char *interface,
                                                 const char *group,
                                                 const unsigned char ttl,
                                                 const int loopback )
   : IP_CL_SockCommClass ( port, interface, loopback, IFF_MULTICAST )
{

   if ( sockstate != SC_SOCKET_ERROR ) {

      // Assign network port
      if ( port == 0 ) {
         net_port = DEFAULT_MC_PORT;
      }
      else {
         net_port = port;
      }

      // Assign multicast group number
      if ( group ) {
         if ( ( (strcasecmp(group,"NONE")) == 0 ) ||
              ( (strcmp(group,"")) == 0 ) ) {
            strcpy ( mc_group, DEFAULT_MC_GROUP );
         }
         else {
            strncpy ( mc_group, group, NETADDRSIZE-1 );
         }
      }
      else {
         sockstate = SC_SOCKET_ERROR;
      }

      // Assign multicast time-to-live value
      mc_ttl = ttl;
   }
}


MC_IP_CL_SockCommClass::~MC_IP_CL_SockCommClass ()
{
   mc_group[0] = '\0';
   mc_ttl = 0;
}


int
MC_IP_CL_SockCommClass::configure_socket ( int &error_num )
{
   int success = SC_SOCKET_OPEN;
   error_num = 0;
   struct ip_mreq mreq;
   int on = 1;

   if ( sockop == SC_SOCKET_WRITE ) {

      // Configure time-to-live for multicast
      if ( success && 
           ((setsockopt(socketid,IPPROTO_IP,IP_MULTICAST_TTL,
                        (char *)&mc_ttl,sizeof(mc_ttl))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      // Configure local packet loopback for multicast 
      if ( success &&
           ((setsockopt(socketid,IPPROTO_IP,IP_MULTICAST_LOOP,
                        (char *)&net_loopback,sizeof(net_loopback))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if ( success &&
           ((setsockopt(socketid,IPPROTO_IP,IP_MULTICAST_IF,
                    (char *)&(net_inet_addr), sizeof(IN_ADDR_SIZE))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if ( success ) {
         memset ( (void *)&internet_sockaddr,0,
                  (size_t)sizeof(internet_sockaddr) );
         internet_sockaddr.sin_family = AF_INET;
         internet_sockaddr.sin_addr.s_addr = inet_addr(mc_group);
         internet_sockaddr.sin_port = htons(net_port);
      }

   } // if operation SOCKET_WRITE

   else if ( sockop == SC_SOCKET_READ ) {

      if ( success ) {
         memset ( (void *)&internet_sockaddr,0,
                  (size_t)sizeof(internet_sockaddr) );
         internet_sockaddr.sin_family = AF_INET;
         internet_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
         internet_sockaddr.sin_port = htons(net_port);

         mreq.imr_multiaddr.s_addr = inet_addr(mc_group);
         mreq.imr_interface.s_addr = net_inet_addr.s_addr;
      }

      if ( success &&
           ((setsockopt(socketid,IPPROTO_IP,IP_ADD_MEMBERSHIP,
                        (char *)&mreq,sizeof(mreq))) < 0) ) {
         error_num = errno;
         success = FALSE;
      }

      if ( success ) {
         net_inet_addr.s_addr = inet_addr(mc_group);
      }

      if ( success &&
           ((setsockopt(socketid, SOL_SOCKET,
                        SO_REUSEADDR, (char *)&on, sizeof(on))) < 0) ) {
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
MC_IP_CL_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( MC_IP_CL_SockCommClass::isaexact(query_kind) ||
                   IP_CL_SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
MC_IP_CL_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UDP_MULTICAST);
}


// End file - mc_ip_cl_sockcomm.cc
