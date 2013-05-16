/*
 * File:  up_sockcomm.cc                         Version: 001
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


#include "up_sockcomm.h"
#include <string.h>


UP_SockCommClass::UP_SockCommClass ( )
   : SockCommClass()
{
   if ( sockstate != SC_SOCKET_ERROR ) {
      sockfamily = AF_UNIX;
      memset ( (void *)&unix_sockaddr, 0, sizeof(struct sockaddr_un) );
   }
}


UP_SockCommClass::~UP_SockCommClass ()
{
}


int
UP_SockCommClass::isa ( SockKind query_kind )
{
   static int truth_value;
   truth_value = ( UP_SockCommClass::isaexact(query_kind) ||
                   SockCommClass::isa(query_kind) );
   return (truth_value);
}


int
UP_SockCommClass::isaexact ( SockKind query_kind )
{
   return (query_kind == SC_UNIX_PROTO);
}


// End file - up_sockcomm.cc
