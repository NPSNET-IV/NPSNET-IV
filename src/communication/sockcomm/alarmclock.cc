/*
 * File:  alarmclock.cc                          Version: 001
 * Created By:  Paul T. Barham                   Date: April 4, 1996
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

#include "alarmclock.h"
#include <sys/time.h>
#include <signal.h>

// Since the different OS implementations of the signal hanlding is
// not standard, we have to prototype the signal handler for the
// alarm according to the OS.

   static void alarmfunc(int) {}

//#ifdef __sun
//   static void alarmfunc(int) {}
//#endif

//#ifdef __hpux
//   static void alarmfunc(int) {}
//#endif


AlarmClockClass::AlarmClockClass()
{
   alarm_activated = FALSE;
}


AlarmClockClass::~AlarmClockClass()
{
   reset_alarm();
}


int
AlarmClockClass::alarm_is_set ()
{
   return (alarm_activated);
}

  
int
AlarmClockClass::set_alarm ( const unsigned int sec, const unsigned int usec )
{
   int success = TRUE;
   static struct itimerval itv;
   static struct sigaction newact;

   itv.it_interval.tv_sec = sec;   // num full seconds until first signal
   itv.it_interval.tv_usec = usec; // num micro seconds until first signal
   itv.it_value = itv.it_interval; // time until remaining signals to be
                                   // same as the first

   if ( alarm_activated ) {
      success = reset_alarm();
   }

   if ( success ) { 
      // Set up a signal handler to trap SIGALRM signals
      newact.sa_handler = alarmfunc;
      newact.sa_flags = SA_RESTART;
      sigemptyset(&newact.sa_mask);
      if ( (sigaction(SIGALRM, &newact, NULL)) != -1 ) {
         // Set the timer to generate regularly timed SIGALRM signals
         success = ((setitimer(ITIMER_REAL,&itv,(struct itimerval *)0)) == 0);
         alarm_activated = success;
      }
      else {
         success = FALSE;
      }
   }

   return (success);
}


int
AlarmClockClass::reset_alarm()
{
   static struct itimerval itv;
   int success = alarm_activated;
   static struct sigaction newact;

   itv.it_interval.tv_sec = 0;     // Set all time values to zero to
   itv.it_interval.tv_usec = 0;    // reset the alarm.
   itv.it_value = itv.it_interval;

   if ( success ) {
      // Ignore SIGALRM signals
      newact.sa_handler = SIG_IGN;
      newact.sa_flags = SA_RESTART;
      sigemptyset(&newact.sa_mask);
      if ( (sigaction(SIGALRM, &newact, NULL)) != -1 ) {
         // Reset timer to zeros thus disabling the timer
         success = ((setitimer(ITIMER_REAL,&itv,(struct itimerval *)0)) == 0);
         alarm_activated = !success;
      }
      else {
         success = FALSE;
      }
   }

   return (success);
}


int
AlarmClockClass::wait_for_alarm()
{
   int success = alarm_activated;
   sigset_t sigoldmask;
   sigset_t sigset;

   if ( success ) {
      // Wait for the next regularly schedule SIGALRM alarm signal.
      sigprocmask(SIG_SETMASK, NULL, &sigoldmask);
      sigprocmask(SIG_SETMASK, NULL, &sigset);
      sigaddset(&sigset,SIGALRM);
      sigprocmask(SIG_BLOCK, &sigset, NULL);
      sigdelset(&sigset, SIGALRM);
      sigsuspend(&sigset);
      sigprocmask(SIG_SETMASK, &sigoldmask, NULL);
   }

   return success;
}


int
AlarmClockClass::sleep ( const unsigned int secs, const unsigned int usecs )
{
   int success;

   if ( (set_alarm(secs,usecs)) && (wait_for_alarm()) && (reset_alarm()) ) {
      success = TRUE;
   }
   else {
      success = FALSE;
   }

   return success;
}


// End file - alarmclock.cc
