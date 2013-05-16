// File: <cpu.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
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
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */

#include <sys/types.h>
#include <sys/sysmp.h>
#include <unistd.h>
#include <iostream.h>

#include "cpu.h"



int npsFreeCPUs ( void )
  {
  long number_of_CPUs = sysmp ( MP_NPROCS );

  long number_of_availible_CPUs = sysmp ( MP_NAPROCS );

  if ( number_of_CPUs == number_of_availible_CPUs )
     {
     return ( 1 );
     }

  long user_id = geteuid();


  if ( user_id )
     {
     cerr << " *** You are not ROOT. Unable to free restricted CPUs... " 
          << endl;
     return ( 0 );
     }

  int i;
 
  for ( i = 0 ; i < number_of_CPUs ; i++ )
     {
     sysmp ( MP_UNISOLATE, i );
     sysmp ( MP_EMPOWER, i );
     }
  
  return ( 1 );
  }
  

int npsLockCPU ( int cpu, char *what )
  {
  long number_of_CPUs = sysmp ( MP_NPROCS );

  long user_id = geteuid();
  
  if ( number_of_CPUs <= cpu )
     {
     cerr << " *** The lock requested for " << what 
          << " is an invalid CPU number..." << endl;
     return ( 0 );
     }

  if ( sysmp ( MP_MUSTRUN, cpu ) < 0 )
     {
     cerr << " *** MUSTRUN for " << what 
          << " on CPU " << cpu << " failed..." << endl;
     return ( 0 );
     }
  else
     {
     //cerr << "     MUSTRUN for " << what 
     //     << " active on CPU " << cpu << "..."
     //     << endl;
     }

  if ( sysmp ( MP_RESTRICT, cpu ) < 0 )
     {
     cerr << " *** CPULOCK for " << what 
          << " on CPU " << cpu << " failed.";
     
     if ( user_id )
        {
        cerr << " You are not logged in as ROOT..." << endl;
        }
     else
        {
        cerr << ".." << endl;
        }
 
     return ( 0 );
     }
  else
     {
     sysmp ( MP_ISOLATE, cpu );
     //cerr << "     CPULOCK for " << what
     //     << " active on CPU " << cpu << "..."
     //     << endl;
     }
   
   return ( 1 );
   }

int npsMustRunCPU ( int cpu, char *what )
  {
  long number_of_CPUs = sysmp ( MP_NPROCS );
  
  if ( number_of_CPUs <= cpu )
     {
     cerr << " *** The lock requested for " << what 
          << " is an invalid CPU number..." << endl;
     return ( 0 );
     }

  if ( sysmp ( MP_MUSTRUN, cpu ) < 0 )
     {
     cerr << " *** MUSTRUN for " << what 
          << " on CPU " << cpu << " failed..." << endl;
     return ( 0 );
     }
  else
     {
     //cerr << "     MUSTRUN for " << what 
     //     << " active on CPU " << cpu << "..."
     //     << endl;
     }
   
  return ( 1 );
  }

  
