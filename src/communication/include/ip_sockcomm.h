/*
 * File:  ip_sockcomm.h                          Version: 001
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


#ifndef __NPS_IP_SOCKCOMM__
#define __NPS_IP_SOCKCOMM__

#include "sockcomm.h"
#include <netinet/in.h>

#define NETADDRSIZE 25
#define DEFAULT_IP_PORT 4000


class IP_SockCommClass : public virtual SockCommClass {
// Internet Protocol Socket Communication Class

public:

   IP_SockCommClass ( const unsigned short /*network port*/,
                      const char *         /*network interface*/,
                      const int            /*network loopback flag*/,
                      const short          /*network comm_flags*/ );
   virtual ~IP_SockCommClass ();

   virtual int isa ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is a type of the given "query kind".
      // FALSE otherwise.
   virtual int isaexact ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is exactly of the given "query kind".
      // FALSE otherwise.

protected:
   struct sockaddr_in internet_sockaddr; // Internet socket address structure
   char net_interface[NETADDRSIZE];      // Network interface name
   struct in_addr net_inet_addr;         // Network interface internet address
   char ascii_inet_addr[NETADDRSIZE];    // Network interface IP number string
   unsigned short net_port;              // Network port id
   unsigned char net_loopback;           // Receive own packets?
   struct sockaddr_in broadaddr;         // Network interface broadcast address

   int get_ip_given_name ( const char * /*machine name*/,
                           char *       /*machine ip address*/ );
      // Get the internet address for a machine given the machine's name.
      // Returns TRUE if successfull, FALSE otherwise.

   int get_interface ( short /*flags*/, int & /*error_num*/ );
      // Get the network interface internet address, ascii IP number,
      // and braodcast address for the interface supplied with the 
      // constructor.  Flags check for desire of broadcast/multicast
      // capability.  Returns TRUE if successfull, FALSE otherwise.

   char *get_ip_of_sender ( sockaddr_in * /*from*/ );
      // Extracts and returns the ascii IP address from the socket
      // address strucutre of the machine that sent a packet.  Returns
      // 0.0.0.0 on failure.

private:

};

#endif

// End file - ip_sockcomm.h
