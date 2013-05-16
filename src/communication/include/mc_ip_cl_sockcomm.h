/*
 * File:  mc_ip_cl_sockcomm.h                    Version: 001
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


#ifndef __NPS_MC_IP_CL_SOCKCOMM__
#define __NPS_MC_IP_CL_SOCKCOMM__

#include "ip_cl_sockcomm.h"

#define DEFAULT_MC_GROUP "224.2.121.93"
#define DEFAULT_MC_TTL 3
#define DEFAULT_MC_PORT 3111


class MC_IP_CL_SockCommClass : public IP_CL_SockCommClass {

public:

   MC_IP_CL_SockCommClass ( const unsigned short /*network port*/,
                            const char *         /*network interface*/,
                            const char *         /*initial multicast group*/,
                            const unsigned char  /*initial multicast ttl*/,
                            const int            /*network loopback*/ );
   virtual ~MC_IP_CL_SockCommClass ();

   virtual int isa ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is a type of the given "query kind".
      // FALSE otherwise.
   virtual int isaexact ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is exactly of the given "query kind".
      // FALSE otherwise.

protected:
   char mc_group[NETADDRSIZE];
   unsigned char mc_ttl;

   int configure_socket ( int & /*error_num*/ );

   int filter ( sockaddr * ) { return FALSE; }
      //  Don't need to filter since the Multicast loopback is in place

private:

};

#endif

// End file - mc_ip_cl_sockcomm.h
