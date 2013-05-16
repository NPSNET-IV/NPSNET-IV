/*
 * File:  ip_cl_sockcomm.cc                      Version: 001
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


#include "ip_cl_sockcomm.h"


IP_CL_SockCommClass::IP_CL_SockCommClass ( const unsigned short port,
                                           const char *interface,
                                           const int loopback,
                                           const short comm_flags )
   : IP_SockCommClass (port,interface,loopback,comm_flags),
     CL_SockCommClass ()
{
}

IP_CL_SockCommClass::~IP_CL_SockCommClass ()
{
}


char *
IP_CL_SockCommClass::get_origin ( sockaddr *from )
{
   return get_ip_of_sender ( (sockaddr_in *)from );
}


int
IP_CL_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( IP_CL_SockCommClass::isaexact(query_kind) ||
                   CL_SockCommClass::isa(query_kind) ||
                   IP_SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
IP_CL_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UDP);
}


// End file - ip_cl_sockcomm.cc
