/*
 * File:  ip_sockcomm.cc                         Version: 001
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


#include "ip_sockcomm.h"

#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;

#ifdef __sun
#include <sys/sockio.h>
#endif


/* Local type defintions */
typedef struct {
   struct in_addr       net_inet_addr;
   char                 ascii_inet_addr[NETADDRSIZE];
   char                 interface[NETADDRSIZE];
   struct sockaddr_in   broadaddr;
} valid_intrf_info;



IP_SockCommClass::IP_SockCommClass ( const unsigned short port,
                                     const char *interface,
                                     const int loopback,
                                     const short comm_flags )
   : SockCommClass()
{
   int error_num = 0;

   if ( sockstate != SC_SOCKET_ERROR ) {

      sockfamily = AF_INET;


      memset ( (void *)&internet_sockaddr, 0, sizeof(struct sockaddr_in) );
      memset ( (void *)&net_inet_addr, 0, sizeof(struct in_addr) );
      strcpy ( ascii_inet_addr, "" );
      memset ( (void *)&broadaddr, 0, sizeof(struct sockaddr_in) );

      // Assign network port
      if ( port == 0 ) {
         net_port = DEFAULT_IP_PORT;
      }
      else {
         net_port = port;
      }

      // Assign network interface (primarily for multihomed machines)
      if ( interface ) {
         if ( (strcasecmp(interface,"NONE")) == 0 ) {
            strcpy ( net_interface, "" );
         }
         else {
            strncpy ( net_interface, interface, NETADDRSIZE-1 );
         }
      }
      else {
         strcpy ( net_interface, "" );
      }

      if ( !get_interface(comm_flags,error_num) ) {
         print_error ( "IP_SockCommClass constructor", error_num );
         if ( sockstate == SC_SOCKET_OPEN ) {
            close(socketid);
         }
         sockstate = SC_SOCKET_ERROR;
      }

      net_loopback = loopback;

   }
}


IP_SockCommClass::~IP_SockCommClass ()
{
}


int
IP_SockCommClass::get_ip_given_name ( const char *machine_name,
                                      char *machine_address )
{
   hostent *host = NULL;
   struct in_addr *host_ip = NULL;
   int success = TRUE;

   if ( (host = gethostbyname(machine_name)) == NULL ) {
      success = FALSE;
      strcpy ( machine_address, "0.0.0.0" );
   }
   else {
      host_ip = (struct in_addr *)host->h_addr;
      strncpy ( machine_address, inet_ntoa ( *host_ip ), NETADDRSIZE-1 );
   } 

   return (success);
}


int
IP_SockCommClass::get_interface ( short flags, int &error_num )
{
   valid_intrf_info valid[4];
   int current = 0;
   int count = 0;
   int found = FALSE;
   unsigned int	i = 0;
   char buf[BUFSIZ];
   struct ifconf ifc;
   struct ifreq *ifr = NULL;
   struct sockaddr_in *in_addr = NULL;
   int sock_temp = 0;

   /* Open temporary socket for gathering interface information */
   if ((sock_temp = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      error_num = errno;
      return FALSE;
   }

   /* Get InterFace CONFig */
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock_temp, SIOCGIFCONF, (char *) &ifc) == -1) {
      error_num = errno;
      close(sock_temp);
      return (FALSE);
   }

   ifr = ifc.ifc_req;           /* ptr to interface structure */
   current = 0;
   for (i = 0; i < (ifc.ifc_len / sizeof(struct ifreq)); i++, ifr++) {

      if (ifr->ifr_addr.sa_family != AF_INET)   /* Internet only */
         continue;

      /* Get InterFace FLAGS */
      if (ioctl(sock_temp, SIOCGIFFLAGS, (char *) ifr) == -1) {
         error_num = errno;
         close(sock_temp);
         return (FALSE);
      }

      /*  Skip boring cases */
      if ( ((ifr->ifr_flags & IFF_UP) == 0) || // interface down?
           (ifr->ifr_flags & IFF_LOOPBACK)   // local loopback?
           || ((ifr->ifr_flags & flags) == 0) ) {  // Pass flag check?
         continue;
      }

      /* Get and save InterFace ADDRess */
      if (ioctl(sock_temp, SIOCGIFADDR, (char *) ifr) == -1) {
         error_num = errno;
         close(sock_temp);
         return (FALSE);
      }

      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);

      valid[current].net_inet_addr = in_addr->sin_addr;
      strncpy ( valid[current].ascii_inet_addr,
               inet_ntoa(valid[current].net_inet_addr), NETADDRSIZE-1 );
      strncpy ( valid[current].interface, ifr->ifr_name, NETADDRSIZE-1 );

      if ( flags & IFF_BROADCAST ) {
         /* Get InterFace BRoaDcast ADDRess */

         if (ioctl(sock_temp, SIOCGIFBRDADDR, (char *) ifr) == -1) {
            error_num = errno;
            close(sock_temp);
            return (FALSE);
         }
         memcpy ( (void *)&(valid[current].broadaddr),
                  (void *)&(ifr->ifr_broadaddr),
                  (size_t)sizeof(ifr->ifr_broadaddr));
      }

      current++;

   } // for - retreiving interface flags


   if ( current < 1 ) {
      close(sock_temp);
      return (FALSE);
   }
   else {
      found = FALSE;
      count = 0;

      while ( (!found) && (count < current) ) {
         if ( (strcmp("",net_interface)) == 0 ) {
            found = TRUE;
         }
         else if ( (strcmp(net_interface,valid[count].interface)) == 0 ) {
            found = TRUE;
         }
         else {
            count++;
         }
      }

      if (!found) {
         close(sock_temp);
         error_num = 0;
         return (FALSE);
      }

      net_inet_addr = valid[count].net_inet_addr;
      strncpy ( (char *)ascii_inet_addr,
                valid[count].ascii_inet_addr,
                NETADDRSIZE-1 );
      strncpy ( (char *)net_interface, valid[count].interface,
                NETADDRSIZE-1 );
      if ( flags & IFF_BROADCAST )
         memcpy ( (void *)&(broadaddr),
                  (void *) &(valid[count].broadaddr),
                  (size_t)sizeof(ifr->ifr_broadaddr));
   }

   close(sock_temp);

   return (TRUE);

}

char *
IP_SockCommClass::get_ip_of_sender ( sockaddr_in *from )
{
   static char ip_of_sender[NETADDRSIZE];
   if ( from ) {
      strncpy ( ip_of_sender, inet_ntoa(from->sin_addr), NETADDRSIZE-1 );
   }
   else {
      strcpy ( ip_of_sender, "0.0.0.0" );
   }

   return (ip_of_sender);
}


int
IP_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( IP_SockCommClass::isaexact(query_kind) ||
                   SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
IP_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_IP_PROTO);
}


// End file - ip_sockcomm.cc
