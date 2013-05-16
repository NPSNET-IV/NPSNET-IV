/*
 * File:  alarmclock.h                           Version: 001
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

#ifndef __NPS_ALARMCLOCK__
#define __NPS_ALARMCLOCK__

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

class AlarmClockClass {
// This class implements an alarm clock that can be used
// to put a process to sleep for an amount of time.  The time
// to be suspended has a resolution in microseconds.

public:

   AlarmClockClass();
   ~AlarmClockClass();
 
   int alarm_is_set ();
      // Returns TRUE if the alarm is currently set, FALSE otherwise.
   int set_alarm ( const unsigned int /*seconds*/,
                   const unsigned int /*micro-seconds*/ );
      // Sets the alarm to go off every sec plus microsecs.
      // If a previous alarm was set, that alarm is cancelled in favor
      // of the new alarm.
      // Returns TRUE if successful, FALSE otherwise.
      // Example: set_alarm(1,250000) will set an alarm to go off every
      //          1.25 seconds.
   int reset_alarm();
      // Resets the alarm to be off.
      // Returns TRUE if successful, FALSE otherwise.
   int wait_for_alarm();
      // Waits (pauses) for the alarm to go off.
      // Returns TRUE if the alarm was set and went off.
      // Returns FALSE if no alarm was set.
   int sleep ( const unsigned int /*seconds*/,
               const unsigned int /*micro-seconds*/ );
      // Suspends the current process until the given time has expired
      // (seconds + micro-seconds).  Returns TRUE if successful, FALSE
      // otherwise.

protected:

private:
   int alarm_activated; // TRUE if an alarm has been set, FALSE otherwise.

};

#endif

// End file - alarmclock.h
