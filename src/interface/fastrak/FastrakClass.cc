// *********************************************************************
// File    : FastrakClass.cc
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants/Insertion of Humans into VEs
// Created : August 1995
// Summary : This file contains the declarations for a C++ class to
//         : manage the Polhemus 3Space Fastrak.
//         :
//         : For detailed information on the operation of the Fastrak,
//         : refer to the 3SPACE USER'S MANUAL.
//         :
//         : This program was based on the ISOTRACK program written by
//         : Paul T. Barham in Sept. 1993 for single sensor case.  Major
//         : modifications have been made to adapt to multiple sensor
//         : case.  The resulting code, written by Jiang Zhu in
//         : Jan. 1995, underwent another major modification to support
//         : binary data in continuous mode. 
// *********************************************************************

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

#include <stdlib.h>          // C standard library stuff
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <bstring.h>
#include <iostream.h>        // For C++ standard I/O stuff
#include <fstream.h>         // For C++ file I/O stuff
#include <unistd.h>          // For standard Unix read, write stuff
#include <errno.h>
#include <fcntl.h>           // For file constant definitions and flags
#include <termio.h>          // For terminal I/O stuff
#include <termios.h>         // For terminal I/O stuff
#include <sys/types.h>       // For system type stuff
#include <sys/prctl.h>       // For process control stuff
#include <sys/signal.h>      // For process signal stuff

#include "FastrakClass.h"

//#include "jointmods.h"

// the file for creating the shared data arena used by parallel
// processes. 
#define IARENA_FILE "/tmp/fastrak.arena.data"

// the permission option to chmod command to alter the permissions on
// the arena file to be read and written by everyone.
#define ARENA_PERMISSIONS 0666

// other convenient constants

//#define DEBUG         1
#define BELL   (char) 7

#define FSTK_X        0
#define FSTK_Y        1
#define FSTK_Z        2
#define FSTK_AZ       0        // azimuth
#define FSTK_EL       1        // elevation
#define FSTK_RO       2        // roll

#define RTOD 180.0/M_PI
#define DTOR M_PI/180.0

// local functions
static void f_sig_handler(int sig, ...);

//----------------------------------------------------------------------
//   Function: report_syserr
//    Returns: 
// Parameters: 
//    Summary: Report system errors 
//----------------------------------------------------------------------
static void report_syserr(char* err_info, char* location)
{
   cerr << BELL << "Error in " << err_info << ".\n"
        << "  Error Number in *FastrakClass.cc*" << location << ": "
        << errno << ";\n";
   perror("  Error Message");
}  // end report_syserr()


//----------------------------------------------------------------------
//   Function: pollContinuously
//    Returns: 
// Parameters: 
//    Summary: This is the data producer which is called in multiprocess
//           : mode in a process running in parallel with the one that
//           : calls the constructor and most of other methods of
//           : "FastrakClass". It continuously reads the Fastrak for
//           : data and stores the data in the datarec. This process runs
//           : until the "kill_flag" is set by the consumer. 
//----------------------------------------------------------------------
void pollContinuously(void* tracker_object)
{
   static     void (*temp_signal)(int);
   FastrakClass *tracker = (FastrakClass*)tracker_object;

   // The following call is necessary for this child process to actually 
   // respond to SIGHUP signals when the parent process dies.            

   prctl(PR_TERMCHILD);

   if ((signal ( SIGTERM, (void (*)(int))f_sig_handler )) == SIG_ERR)
            perror("FastrakClass:\tError setting SIGTERM handler -\n   ");

   if ((signal ( SIGHUP, (void (*)(int))f_sig_handler )) == SIG_ERR)
            perror("FastrakClass:\tError setting SIGHUP handler -\n   ");

   if ( (temp_signal = signal ( SIGINT, (void (*)(int))f_sig_handler ))
      != SIG_DFL ) {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("FastrakClass:\tError setting SIGINT ignore -\n   ");
   }

   if ((signal ( SIGQUIT, (void (*)(int))f_sig_handler )) == SIG_ERR)
      perror("FastrakClass:\tError setting SIGQUIT handler -\n   ");

#if DEBUG
   cerr << "Fastrak polling process initiated." << endl;
#endif

   if (write(tracker->port_fd, "C", 1) != 1){
      report_syserr("sending FASTRAK C command",
                    "pollContinuously");
   }

   while (!tracker->kill_flag) {
      tracker->getPacket();
   } // end while loop

   if (write(tracker->port_fd, "c", 1) != 1){
      report_syserr("sending FASTRAK c command",
                    "pollContinuously");
   }

   tracker->kill_flag = FALSE;

#if DEBUG
   cerr << "Fastrak polling process terminated." << endl;
#endif

   exit(0);
} // end pollContinuously()


//----------------------------------------------------------------------
//   Function: f_sig_handler
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void f_sig_handler(int sig, ...)
{
   if ((signal ( sig, SIG_IGN )) == SIG_ERR) {
      perror("FastrakClass:\tSignal Error -\n   ");
   }

   switch ( sig ) {
      case SIGTERM:
         exit(0);
         break;
      case SIGHUP:
         if ( getppid() == 1 ) {
            cerr << "DEATH NOTICE:" << endl;
            cerr << "\tParent process terminated."
                 << endl;
            cerr << "\tFastrakClass terminating to prevent orphan process."
                 << endl;
            exit(0);
         }
         break;
      case SIGINT:
         cerr << "DEATH NOTICE: FastrakClass exitting due to interrupt "
              << "signal." << endl;
         exit(0);
         break;
      case SIGQUIT:
         cerr << "DEATH NOTICE: FastrakClass exitting due to quit "
              << "signal." << endl;
         exit(0);
         break;
      default:
         cerr << "DEATH NOTICE: FastrakClass exitting due to signal:  "
              << sig << endl;
         exit(0);
         break;
   }
   signal ( sig, (void (*)(int))f_sig_handler );
}

//======================================================================
// FastrakClass class methods
//======================================================================

//----------------------------------------------------------------------
//   Function: FastrakClass::FastrakClass
//    Returns: 
// Parameters: 
//    Summary: constructor for FastrakClass object
//----------------------------------------------------------------------
FastrakClass::FastrakClass(ifstream &config_fileobj, short datatype_flags)
{
   // Initialize instance variables.
   initState();

   // Read in the configurations and open the FASTRACK port.
   if ((valid_flag = readConfig(config_fileobj)) == TRUE) {
      valid_flag = openIOPort();
   }

   if (valid_flag) {
      // the ^Y command for resetting FASTRAK
      // Refer to pp 76, the 3 SPACE USER'S MANUAL for details.
      static char command[] = { 0x19 };

      cerr << "Initializing Fastrak to start-up state." << endl
           << "    This takes about 10 seconds...";
      if (write(port_fd, command, 1) == -1) {  
         valid_flag = FALSE;
         report_syserr("sending FASTRAK ^Y command",
                       "FastrakClass::FastrakClass");
      }
      else {
         sleep(2);
         for (int i=9; i>-1; i--) {
            cerr << i << ' ';
            sleep(1);
         }
         cerr << endl;
      }
   }

   if ((valid_flag) && (valid_flag = checkState())) {
      valid_flag = setUnits(FSTK_CENTIMETER);
      for (int i = 0; i < FSTK_NUM_STATIONS; i++) {
         if ((valid_flag) && (active_state[i])) {
            valid_flag = setDataTypes((FSTK_stations)i, datatype_flags);
         }
      }
   }

   for (int station_num = 0; station_num < FSTK_NUM_STATIONS; station_num++) {
      if ((valid_flag) && (active_state[station_num])) {
         valid_flag = setHemisphere((FSTK_stations)station_num,
                                     hemisphere[station_num]);
         if (valid_flag) {
            valid_flag = setAlignment((FSTK_stations)station_num, 
                                       alignment[station_num][0],
                                       alignment[station_num][1],
                                       alignment[station_num][2]);
         }
      }
   }

   // mcmillan - 950814 - new code to read IEEE binary format data
   if (valid_flag) {
      // Enable binary output format from the FASTRAK.
      // Refer to pp 114, the 3 SPACE USER'S MANUAL.
      if (write(port_fd, "f", 1) != 1) {
         report_syserr("sending FASTRAK f command",
                       "FastrakClass::FastrakClass");
         valid_flag = FALSE;
      }
   }

   if (valid_flag) {
      // Initialize the multiprocess mode, i.e., creating the locks
      // and semaphors and sprocing the Fastrak data producer process.
      valid_flag = initMultiprocessing();
   }

   if (valid_flag) {
      is_polling_flag = TRUE;
   }

#if DEBUG
   cerr << "The FASTRAK object constructed.\n";
   if (valid_flag) {
      cerr << "The FASTRAK intialization is successful." << endl;
   }
   else {
      cerr << "The FASTRAK intialization is unsuccessful." << endl;
   }
#endif

}  // end FastrakClass::FastrakClass()


//----------------------------------------------------------------------
//   Function: FastrakClass::~FastrakClass
//    Returns: 
// Parameters: 
//    Summary: destructor for FastrakClass class
//----------------------------------------------------------------------
FastrakClass::~FastrakClass()
{
#if DEBUG
      cerr << "Fastrak destructor called.\n";
#endif

   if (valid_flag) {
      // In multiprocess mode, signal the producer process to die.  
      // Then, free the lock and semaphore.

      if (parpoll_pid != -1) {
         kill_flag = TRUE;
         if (!is_polling_flag) usvsema(paramsema);
         while (kill_flag);
         sleep(1);

         parpoll_pid = -1;
         usfreelock(datalock, arena);
         usfreesema(paramsema, arena);
      }

      // Flush all characters from the serial port and then close it.
      tcflush (port_fd, TCIOFLUSH);
      close (port_fd);
      valid_flag = FALSE;
   }
} // FastrakClass::~FastrakClass()


//----------------------------------------------------------------------
//   Function: initState
//    Returns: 
// Parameters: 
//    Summary: Initialize instance variables to their default states.
//----------------------------------------------------------------------
void FastrakClass::initState()
{
   bzero(read_buffer, BUFFER_SIZE);
   read_index = 0;
   max_datarec_size = 0;

   parpoll_pid = -1;
   param_set_flag = FALSE;
   is_polling_flag = FALSE;
   data_ready_flag = FALSE;
   kill_flag = FALSE;

   // Initialize hemispheres and alignments.
   // Refer to pp 42 - 50 and pp 88 - 92, the 3 SPACE USER'S MANUAL 
   // for the default values.
   for (int station_num = 0; station_num < FSTK_NUM_STATIONS; station_num++) {
      hemisphere[station_num][FSTK_X] = 1.0;
      hemisphere[station_num][FSTK_Y] = 0.0;
      hemisphere[station_num][FSTK_Z] = 0.0;

      boresight[station_num][FSTK_AZ] = 0.0;
      boresight[station_num][FSTK_EL] = 0.0;
      boresight[station_num][FSTK_RO] = 0.0;

      // origins
      alignment[station_num][0][FSTK_X] = 0.0;
      alignment[station_num][0][FSTK_Y] = 0.0;
      alignment[station_num][0][FSTK_Z] = 0.0;

      // X directions
      alignment[station_num][1][FSTK_X] = 1.0;
      alignment[station_num][1][FSTK_Y] = 0.0;
      alignment[station_num][1][FSTK_Z] = 0.0;

      // Y directions
      alignment[station_num][2][FSTK_X] = 0.0;
      alignment[station_num][2][FSTK_Y] = 1.0;
      alignment[station_num][2][FSTK_Z] = 0.0;

      // Initialize data record parameters.
      active_state[station_num] = FALSE;
      datarec_size[station_num] = 0;
      bzero(datarec_buf[station_num], MAX_PACKET_SIZE);
      bzero(datarec[station_num], MAX_PACKET_SIZE);

      for (int type_num = 0; type_num < FSTK_NUM_DATATYPES; type_num++)
         datatype_start[station_num][type_num] = -1;
   }

   fstk_packet_size = 0;
}  // end initState()


//----------------------------------------------------------------------
//   Function: readConfig
//    Returns: TRUE if the read is successful. Otherwise, return FALSE.
// Parameters: 
//    Summary: Read the configuration file for the FASTRAK.  Called by a
//           : "FastrakClass" constructor to do initialization. They should
//           : not be called elsewhere.  
//----------------------------------------------------------------------

#define MAX_CONFIGFILE_LINESIZE        255
#define CONFIGFILE_COMMENT_CHAR        '#'

int FastrakClass::readConfig(ifstream &config_fileobj)
{
   int success = TRUE;
   char tmp_str[MAX_CONFIGFILE_LINESIZE];
   int station_num;

   while (config_fileobj >> tmp_str)
   {
      // When a comment char is read, skip the rest of the line.
      if (tmp_str[0] == CONFIGFILE_COMMENT_CHAR) {
         config_fileobj.getline(tmp_str, MAX_CONFIGFILE_LINESIZE);
      }
      else if (strncmp(tmp_str, "PORT", 4) == 0) {
         config_fileobj >> port_name;
      }
      else if (strncmp(tmp_str, "WANTED_STATIONS", 8) == 0) {
         int state;
         for (station_num = 0; station_num < FSTK_NUM_STATIONS;
                               station_num++) { 
            config_fileobj >> state;
            active_state[station_num] = state;
         }
      }
      else {
         char param_str[30];
         int i, j;
         for (int station_num = 0; station_num < FSTK_NUM_STATIONS;
                                   station_num++) {
            sprintf(param_str, "STATION%d_PARAM", station_num+1);

            do {
                 if (tmp_str[0] == CONFIGFILE_COMMENT_CHAR) 
                   config_fileobj.getline(tmp_str, MAX_CONFIGFILE_LINESIZE);
               else if (strncmp(tmp_str, param_str, 10) == 0) {
                  char param_name[30];

                  config_fileobj >> param_name;
                  for (i = 0; i < 3; i++)
                     config_fileobj >> hemisphere[station_num][i];

                  for (i = 0; i < 3; i++) {
                     config_fileobj >> param_name;
                     for (j = 0; j < 3; j++)
                        config_fileobj >> alignment[station_num][i][j];
                  }
                  
                  config_fileobj >> tmp_str;
                  break;
               }
               else {
                  success = FALSE;
                     cerr << BELL << "Error in reading config file.\n"
                       << "  in *FastrakClass.cc*FastrakClass::readConfig;"
                       << " illegal string: " << tmp_str << endl;
               }
            } while (config_fileobj >> tmp_str);

         } // end for
      } // end if
   } // end while 

#if DEBUG
   int i, j;
   cerr << "** readConfig:\n" << "   FASTRAK port: " << port_name << "\n"
        << "   Stations requested: ";

   for (i = 0; i < FSTK_NUM_STATIONS; i++)
      cerr << active_state[i] << " ";
   cerr << endl << endl;

   for (i = 0; i < FSTK_NUM_STATIONS; i++) {
      cerr << "STATION" << i+1 << "_PARAM:\n";
      cerr << "  hemisphere:\t";
      for (j = 0; j < 3; j++)
         cerr << hemisphere[i][j] << " ";
      cerr << "\n  origin:\t";
      for (j = 0; j < 3; j++)
         cerr << alignment[i][0][j] << " ";
      cerr << "\n  x_point:\t";
      for (j = 0; j < 3; j++)
         cerr << alignment[i][1][j] << " ";
      cerr << "\n  y_point:\t";
      for (j = 0; j < 3; j++)
         cerr << alignment[i][2][j] << " ";
      cerr << endl;
   }
   cerr << endl;

   cerr << "FASTRAK configuration parameters read.\n";
#endif

   return (success);
}  // end FastrakClass::readConfig()


//----------------------------------------------------------------------
//   Function: openIOPort
//    Returns: TRUE for a successful opening. Otherwise, return FALSE.
// Parameters: 
//    Summary: Open the FASTRAK io-port
//----------------------------------------------------------------------
int FastrakClass::openIOPort()
{
   int success = TRUE;

   // Test to see if the FASTRAK if on.
   if ((port_fd = open(port_name, O_RDWR|O_NONBLOCK)) == -1) {
      success = FALSE;
      report_syserr("opening the Fastrak port",
                    "FastrakClass::openIOPort");
   }
   else {
      char command[5], buffer[100];
      strcpy(command, "l1\r");

      if (write(port_fd, command, strlen(command)) == -1) {
         success = FALSE;
         report_syserr("sending FASTRAK l command",
                       "FastrakClass::openIOPort");
      } 

      sleep(1);

      if ((success) && (read(port_fd, buffer, 100) == -1)) {
         success = FALSE;
         report_syserr("reading FASTRAK",
                       "FastrakClass::openIOPort");
      }

      close(port_fd);
   }

   // Do a blocking read when polling the FASTRAK for data.
   if ((success) && ((port_fd = open(port_name, O_RDWR)) == -1)) {
      success = FALSE;
      report_syserr("opening the Fastrak port",
                    "FastrakClass::openIOPort");
   }
   else if (success)  {
      struct termio term;
      memset(&term, 0, sizeof(term));

      term.c_iflag = IXOFF;            /* FIXME */
      term.c_oflag = 0;
      term.c_cflag = B9600|CS8|CLOCAL|CREAD|HUPCL;
      term.c_lflag = 0;
      term.c_line  = 0;     // LDISC1;
      term.c_cc[VMIN] = 0;
      term.c_cc[VTIME] = 5;

      if (ioctl(port_fd, TCSBRK, 0) == -1) {
         success = FALSE;
         close (port_fd);
         report_syserr("sending a BREAK to the Fastrak port",
                       "FastrakClass::openIOPort");
      }
      else if (ioctl(port_fd, TCSETAF, &term) == -1) {
         success = FALSE;
         close (port_fd);
         report_syserr("setting the Fastrak port parameters",
                       "FastrakClass::openIOPort");
      }
   }

   // Just in case the fastrak was accidentally left in continuous mode?
   if (success) {
      char data[100];
      int nbr;
      while ((nbr = read(port_fd, data, 100)) > 0) {
         //cerr << "Warning: cleared " << nbr << "bytes from Fastrak buffer"
         //     << " in openIOport." << endl;
         write(port_fd, "c", 1);
         //debugData(data,nbr);
      }
   }

   if (success) {
      if (tcflush (port_fd, TCIOFLUSH) == -1) {
         success = FALSE;
         close (port_fd);
         report_syserr("flushing the Fastrak port",
                       "FastrakClass::openIOPort");
      }
   }

#if DEBUG
   cerr << "FASTRAK io-port, " << port_name << ", opened." << endl;
#endif

   return(success);
} // end FastrakClass::openIOPort()


//----------------------------------------------------------------------
//   Function: checkState
//    Returns: TRUE if all the requested stations are available.
//           : Otherwise, return FALSE.
// Parameters: 
//    Summary: Check for the availability of the FASTRAK stations.
//----------------------------------------------------------------------
int FastrakClass::checkState()
{
   int success = TRUE;
   char command[5];
   char data[100];

   // Construct the "l" command to get the states of the stations.
   // Refer to pp 128 - 131, the 3 SPACE USER's MANUAL for details.
   // Choose any station to get the states for all stations.
   strcpy(command, "l1\r");

   if (write(port_fd, command, strlen(command)) == -1) {
      success = FALSE;
      report_syserr("sending FASTRAK l command",
                    "FastrakClass::checkState");
   }

   // Find out which station is active by hardware configuration.
   if (success == TRUE) {

      // 951002 - mcmillan - IMPORTANT BUG FIX:
      // do raw tty processing to get the answer because on the faster
      // machines and especially the onyx platforms the read occurs
      // sooner than the data is ready.
      const int NUM_BYTES = 9;
      const int MAX_POLL_RETRIES = 100000;
      int count = 0;
      int num_tries = 0;
      int nbr;

      while ((count < NUM_BYTES) &&
             (num_tries < MAX_POLL_RETRIES) &&
             ((nbr = read(port_fd, &data[count], 1)) != -1)) {
         num_tries++;
         count += nbr;
      }

      if (count != NUM_BYTES) {
         success = FALSE;
         if (num_tries == MAX_POLL_RETRIES) {
            cerr << BELL << "Error: too many retries reading fstk port\n";
         }
         else {
            cerr << BELL << "Error: fstk port read failed\n";
         }
         cerr << "  in *FastrakClass.cc*FastrakClass::checkState\n";
      }

#if DEBUG
      debugData(data, 9);
#endif
   }

   if (success == TRUE) {
      for (int i = 0; i < FSTK_NUM_STATIONS; i++) {
         active_setting[i] = (data[i + FSTK_HEADER_SIZE] == '1');

         if (active_setting[i]) {
            if (!active_state[i])
               setState((FSTK_stations)i, FALSE);
         }
         else {
            if (active_state[i]) {
               success = FALSE;
               cerr << BELL
                    << "Error in setting FASTRAK station state.\n"
                    << "  in *FastrakClass.cc*FastrakClass::checkState\n"
                    << "  Station" << i+1 << " is required to be active.\n"
                    << "  However, it is not set to be active by the"
                    << "  hardware switch.\n";
               debugData(data, 9);
            }
         }
      }  // end for
   }
   else {
      report_syserr("reading FASTRAK", "FastrakClass::checkState");
   }

#if DEBUG
   cerr << "The states of the FASTRAK stations checked." << endl;
#endif

   return(success);
}  // end FastrakClass::checkState()


//----------------------------------------------------------------------
//   Function: initMultiprocessing
//    Returns: TRUE if the initialization is successful. Otherwise,
//           : return FALSE. 
// Parameters: 
//    Summary: Initialize for multiprocess mode, including getting locks
//           : and semaphores and sprocing a child process for polling
//           : the FASTRAK. 
//----------------------------------------------------------------------
int FastrakClass::initMultiprocessing()
{
   int success = TRUE;

   // Create an arena file to get the needed lock and semaphore
   if ((arena = usinit(IARENA_FILE)) == NULL) {
      success = FALSE;
      report_syserr("getting an arena file",
                    "FastrakClass::initMultiprocessing");
   }
   else {
      // Set up the arena file with read and write permissions for
      // everyone. 
      if (usconfig(CONF_CHMOD, arena, ARENA_PERMISSIONS) == -1) {
         success = FALSE;
         report_syserr("configuring an arena",
                       "FastrakClass::initMultiprocessing");
      }

      // Create a lock to provide mutual exclusive access to the data
      // buffers.  Refer to getPacket() for more info. 
      if (success && ((datalock = usnewlock(arena)) == NULL)) {
         success = FALSE;
         report_syserr("creating a lock",
                       "FastrakClass::initMultiprocessing");
      }
      else {
         usinitlock(datalock);
      }

      // Create a binary semaphore for providing mutual exclusions so
      // that when the data record parameters are being set, the data
      // producer waits until the setting finishes.  Refer to
      // getPacket() and setDataTypes() for more info.
      if (success &&
          ((paramsema = usnewsema(arena, 1)) == NULL))  {
         success = FALSE;
         report_syserr("creating a binary semaphore",
                       "FastrakClass::initMultiprocessing");
      }

      // Fork the parallel data producer as a lightweight thread which
      // shares the data space with its parent process that is the
      // consumer of the FASTRAK data.
      if (success)  {
         parpoll_pid = sproc(pollContinuously, PR_SALL, (void *)this);
         if (parpoll_pid == -1) {
            success = FALSE;
            report_syserr("spawning the producer process",
                          "FastrakClass::initMultiprocessing");
         }
         else {
            signal(SIGCLD,SIG_IGN);
#if DEBUG
            cerr << "Fastrak poll process spawned: pid = "
                 << parpoll_pid << endl;
#endif
         }
      }
   }  // if arena

   return (success);
} // end FastrakClass::initMultiprocessing()


//----------------------------------------------------------------------
//   Function: getPacket
//    Returns: 
// Parameters: 
//    Summary: Read a packet from the FASTRAK.  In single process mode,
//           : it is called when the FASTTRACk user requests a data. In
//           : multiprocess mode, it is continuously called by the data
//           : producer, pollContinuously(), in a light weight process.
//----------------------------------------------------------------------
void FastrakClass::getPacket()
{
   int full_buffer;

   // The following piece of code is a critical section in multiprocess
   // mode.  During a read operation of the FASTRAK, the data record
   // parameters cannot be changed, e.g, through setDataTypes()
   // which runs in parallel with this method.
   if (parpoll_pid != -1) {
      if (uspsema(paramsema) == -1) { // entering the critical section
         report_syserr("getting semaphore",
                       "FastrakClass::getPacket");
      }
   }
   else {
      // In serial mode, ask for a packet from the FASTRAK.
      // Refer to pp 120, the 3 SPACE USER'S MANUAL.
      if (write (port_fd, "P", 1) != 1) {
         report_syserr("sending FASTRAK P command",
                       "FastrakClass::getPacket");
      }
   }

   // ==============================================
   // keep reading until device buffer is exhuasted
   do {
      unsigned int num_bytes_to_read = BUFFER_SIZE - read_index;
      int num_bytes_read = read(port_fd,
                                &read_buffer[read_index],
                                num_bytes_to_read);

      if (num_bytes_read > num_bytes_to_read) {
         cerr << "Error: fstk read too many bytes (nbr, nbtr): ("
              << num_bytes_read << "," << num_bytes_to_read
              << ").\n";
      }

      if (num_bytes_read == num_bytes_to_read) {
         full_buffer = TRUE;
         cerr << "Warning: fstk read max bytes: "
              << num_bytes_read << ".\n";
      }

      // process the data read
      if (num_bytes_read > 0) {
         unsigned int index = 0;
         read_index += num_bytes_read;

         // while there is enough information for a packet from a
         // single station process the data:
         while (!kill_flag && ((read_index - index) > max_datarec_size)) {

            // make sure header info is the first few bytes
            if ((read_buffer[index] == 0x30) &&
                ((read_buffer[index+1]&0xf0) == 0x30) &&
                (((int) (read_buffer[index+1]&0x0f) - 1) < 4)) {

               int station_num = (int) (read_buffer[index+1]&0x0f) - 1;

               // *****************************
               // entering the critical section
               if (ussetlock(datalock) == -1)
                  report_syserr("getting lock", "packetizer");

               memcpy(datarec_buf[station_num], &read_buffer[index],
                      datarec_size[station_num]);
               data_ready_flag = TRUE;

               usunsetlock(datalock);        // unlocking
               // exiting the critical section
               // *****************************

               index += datarec_size[station_num];
            }
            else { // find the header info and hopefully resynch.
               cerr << "Warning: resynching fstk "
                    << "(index, read_index): = ("
                    << index << ", " << read_index << ")\n";
               while ((index < (read_index-1)) &&
                      !((read_buffer[index] == 0x30) &&
                        ((read_buffer[index+1]&0xf0) == 0x30) &&
                        (((int) (read_buffer[index+1]&0x0f) - 1) < 4))) {
                  cerr << hex << (int) read_buffer[index] << dec
                       << '|';
                  index++;
               }
               cerr << hex << (int) read_buffer[index] << " "
                    << (int) read_buffer[index+1] << dec
                    << '|' << " index: " << index << endl;
            }
         } // while read_index-index

         // when done, shift the rest of the buffer down to the
         // beginning.
         if (index != read_index) {
            if (index > read_index) {
               cerr << "Error: fstk shifting too many bytes ("
                    << read_index << "-" << index << ").\n";
            }
            else {
               memcpy(read_buffer, &read_buffer[index],
                      read_index-index);
            }
         }
         read_index -= index;
         index = 0;
      } // if num_bytes_read

   } while (full_buffer && !kill_flag);

   // exiting the critical section
   if (parpoll_pid != -1) usvsema(paramsema);
} // end FastrakClass::getPacket()


//----------------------------------------------------------------------
//   Function: sendCommand
//    Summary: Send a command to the FASTRACK.
// Parameters: command string, its length, and initiating fcn name
//    Returns: TRUE if the operation is successful. Otherwise, FALSE.
//----------------------------------------------------------------------
int FastrakClass::sendCommand(char* command, int length, char* source)
{
   int success = TRUE;

   (source);
#if DEBUG
   cerr << "command from " << source << ":"
        << command << endl;
#endif

   // This is a critical section in multiprocess mode.
   if (parpoll_pid != -1) {
      if (uspsema(paramsema) == -1) {  // entering the critical section
         success = FALSE;
         report_syserr("getting semaphore", "FastrakClass::sendCommand");
      }
   }
  
   if (write(port_fd, command, length) == -1) {
      success = FALSE;
      report_syserr("sending FASTRACK command", "FastrakClass::sendCommand");
   }

   data_ready_flag = FALSE;  // See getPacket() for when it is set to TRUE.

   // exiting the critical section
   if (parpoll_pid != -1) usvsema(paramsema);

#if DEBUG
   cerr << "FASTRAK command sent: source being " << source << ".\n";
#endif

   return(success);
}  // FastrakClass::sendCommand()


//----------------------------------------------------------------------
//   Function: convertData
//    Returns: 
// Parameters: data record ptr, number of elements, output vector
//    Summary: Convert a DOS ordered bytes to Unix order (reverse)
//----------------------------------------------------------------------
void FastrakClass::convertData(char* data, int num_floats,
                               float data_dest[])
{
   char *ptr = data;
   char *fptr = (char *) data_dest;

   for (int i=0; i<num_floats; i++) {
      *(fptr+3) = *ptr++;
      *(fptr+2) = *ptr++;
      *(fptr+1) = *ptr++;
      *(fptr)   = *ptr++;

      fptr += 4;
   }
}  // FastrakClass::convertData()


//----------------------------------------------------------------------
//   Function: convert16BITData
//    Returns: 
// Parameters: data record ptr, number of elements, scale, output vector
//    Summary: Convert a Polhemus's 16BIT format to IEEE floating point
//----------------------------------------------------------------------
void FastrakClass::convert16BITData(char* data, int num_floats,
                                    float scale, float data_dest[])
{
   char *ptr = data;
   char lobyte;
   char hibyte;
   int sign_flag, num;

   for (int i=0; i<num_floats; i++) {
      lobyte = *ptr++;
      hibyte = *ptr++;
      sign_flag = (int) hibyte&0x040;
      num = ((hibyte << 7) + (lobyte&0x7f))&0x001fff;
      if (sign_flag) {
         num -= 0x02000; // 14 bit 2's complement conversion.
      }

      data_dest[i] = scale*num;
   }
}  // FastrakClass::convert16BITData()


//----------------------------------------------------------------------
//   Function: debugData
//    Returns: 
// Parameters: 
//    Summary: Write num_of_bytes starting from data_store as characters
//           : to cerr. This is a convienience function used to examine
//           : the data packet read in from FASTRAK. 
//----------------------------------------------------------------------
void FastrakClass::debugData(char *data_store, int num_of_bytes)
{
   cerr << "Record length: " << num_of_bytes << "\n";
   cerr << "|";
   for (int i = 0; i < num_of_bytes; i++)
      cerr << hex << (int) data_store[i] << dec << "|";
   cerr << "\n";
}


//----------------------------------------------------------------------
//   Function: detectError
//    Returns: 
// Parameters: 
//    Summary: 
//----------------------------------------------------------------------
int FastrakClass::detectError()
{
   char *station_data;
   int success = TRUE;

   // This method should only be used in critical sections. As a result,
   // no semaphore protection is needed here. 
   for (int i = 0; i < FSTK_NUM_STATIONS; i++) {
      station_data = datarec[i]; 

      switch (*station_data) {
         case '0': break; // No error for data record - do nothing
         case '2': cerr << "Fastrak Type 2 Record received.\n";
            break; 
         case 'A': cerr << BELL
                        << "HARDWARE ERROR found in Fastrak station"
                        << i+1
                        << "  EPROM CHECK SUM. (character A)" << endl;
                        break;
         case 'C': cerr << BELL
                        << "HARDWARE ERROR found in Fastrak station"
                        << i+1 
                        << "  RAM TEST. (character C)" << endl;
                        break;
         case 'S': cerr << BELL
                        << "HARDWARE ERROR found in Fastrak station"
                        << i+1 
                        << "  SELF-CALIBRATION. (character S)" << endl; 
                        break;
         case 'U': cerr << BELL
                        << "HARDWARE ERROR found in Fastrak station"
                        << i+1 
                        << "  SOURCE/SENSOR ID PROM. (character U)"
                        << endl;
                        break;
         case 'a': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  CALCULATE TRACE OF S4TS4. (character a)"
                        << endl;
                        break;
         case 'b': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  SELF-CAL DIVIDE. (character b)" << endl;
                        break;
         case 'c': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  SELF-CAL A/D INPUT. (character c)"
                        << endl;
                        break;
         case 'd': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  SENSOR A/D INPUT. (character d)" << endl;
                        break;
         case 'e': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  OUT OF ENVELOPE. (character e)" << endl;
                        break;
         case 'f': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  SELF-CAL OFFSET OVERFLOW. (character f)"
                        << endl;
                        break;
         case 'g': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1 
                        << "  TRMDI CALCULATION. (character g)" << endl;
                        break;
         case 'h': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  PATH1. (character h)" << endl;
                        break;
         case 'i': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  PATH2. (character i)" << endl;
                        break;
         case 'j': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  PATH3. (character j)" << endl;
                        break;
         case 'k': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  PATH4. (character k)" << endl;
                        break;
         case 'l': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  SYSTEM RUNNING TOO SLOW. (character l)"
                        << endl;
                        break;
         case 'n': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station" << i+1
                        << "  ATTITUDE MATRIX CALCULATION. (character n)"
                        << endl;
                        break;
         case 'p': cerr << BELL
                        << "SOFTWARE ERROR found in Fastrak station"
                        << i+1
                        << "  NORM OF XORVEC TOO LOW. (character p)"
                        << endl;
                        break;
         default : cerr << BELL
                        << "UNKNOWN ERROR found in Fastrak station"
                        << i+1 << ": (character " << *station_data
                        << ") ASCII code: " 
                        << int(*station_data) << endl;
                        break;
      } // end switch
   } // end for

   return(success);

}  // end detectError()



//----------------------------------------------------------------------
//   Function: reportStateError
//    Returns: 
// Parameters: 
//    Summary: Report errors caused by trying to use inactive stations.
//----------------------------------------------------------------------
void FastrakClass::reportStateError(char* location,
                                      FSTK_stations station_num)
{
   cerr << BELL << "Error in using station" << station_num+1 << "\n"
        << "  in *FastrakClass.cc*FastrakClass::" 
        << location << "; inactive station" << endl;
}

//----------------------------------------------------------------------
//   Function: checkReadError
//    Returns: TRUE if no read-data error, else FALSE
// Parameters: station number
//    Summary: Check for read-data error.
//----------------------------------------------------------------------
int FastrakClass::checkReadError(FSTK_stations station_num,
                                 char* source, FSTK_datatypes data_type)
{
   int success = TRUE;

   if (!active_state[station_num]) {
      reportStateError(source, station_num);
      success = FALSE;
   }

   if (datatype_start[station_num][data_type] < 0) {
      cerr << BELL << "Error in reading FASTRAK station"
           << station_num+1 << ".\n"
           << "  in *FastrakClass.cc*FastrakClass::" << source
             << "; unrequested data type\n" << endl;
      success = FALSE;
   }

   if (!is_polling_flag) {
      cerr << BELL << "Error in reading FASTRAK station"
           << station_num+1 << ".\n"
           << "  in *FastrakClass.cc*FastrakClass::" << source
           << "; polling process suspended\n" << endl;
      success = FALSE;
   }

   return(success);
}  // FastrakClass::checkReadError()


//----------------------------------------------------------------------
//   Function: suspend
//    Returns: TRUE if the suspension is successful. Otherwise,  FALSE.
// Parameters: 
//    Summary: In multiprocess mode, suspend the execution of the
//           : parallel polling process.  
//----------------------------------------------------------------------
int FastrakClass::suspend()
{
   int success = TRUE;

   if (is_polling_flag) {
      if (parpoll_pid != NULL) {
         if (uspsema(paramsema) == -1) {
            success = FALSE;
            report_syserr("suspending the polling process",
                          "FastrakClass::suspend");
         }
         else {
            is_polling_flag = FALSE;
         }
      }
#if DEBUG
      cerr << "Parallel polling process suspended: " << success << endl;
#endif
   }
   return(success);
}


//----------------------------------------------------------------------
//   Function: resume
//    Returns: 
// Parameters: 
//    Summary: Resume the execution of the parallel polling process.
//----------------------------------------------------------------------
void FastrakClass::resume()
{
   if (!is_polling_flag) {
      if (parpoll_pid != NULL) usvsema (paramsema);

      is_polling_flag = TRUE;
#if DEBUG
      cerr << "Parallel polling process resumed.\n";
#endif
   }
}


//----------------------------------------------------------------------
//   Function: setAlignment
//    Returns: TRUE if the operation is successful else, FALSE.
// Parameters: station number
//    Summary: set the alignment of a station.
//----------------------------------------------------------------------
int FastrakClass::setAlignment(FSTK_stations station_num, 
                               const float origin[3],
                               const float x_point[3],
                               const float y_point[3])
{
   if (!active_state[station_num]) {
      reportStateError("setAlignment", station_num);
      return(FALSE);
   }

   // Construct the "A" command to set the alignment of the station.
   // Refer to pp 42-49, the 3 SPACE USER's MANUAL for details.
   static char command[100];
   sprintf(command, "A%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r",
           station_num+1,
           origin[FSTK_X], origin[FSTK_Y], origin[FSTK_Z],
           x_point[FSTK_X], x_point[FSTK_Y], x_point[FSTK_Z],
           y_point[FSTK_X], y_point[FSTK_Y], y_point[FSTK_Z]);

   int success =  sendCommand(command, strlen(command),
                               "FastrakClass::setAlignment");

   if (success == TRUE) {
      for (int i = 0; i < 3; i++)
      {
         alignment[station_num][0][i] = origin[i];
         alignment[station_num][1][i] = x_point[i];
         alignment[station_num][2][i] = y_point[i];
      }
   }

#if DEBUG
   cerr << "FASTRAK alignment set.\n";
#endif

   return(success);
}  // FastrakClass::setAlignment()


//----------------------------------------------------------------------
//   Function: resetAlignment
//    Returns: TRUE if the operation is successful else, FALSE.
// Parameters: station number
//    Summary: Reset the alignment of a station.
//----------------------------------------------------------------------
int FastrakClass::resetAlignment(FSTK_stations station_num)
{
   if (!active_state[station_num]) {
      reportStateError("resetAlignment", station_num);
      return(FALSE);
   }

   // Construct the "R" command to reset the alignment of the station to
   // default.  Refer to pp 50, the 3 SPACE USER's MANUAL for details.
   char command[10];
   sprintf(command, "R%d\r", station_num+1);

   int success = sendCommand(command, strlen(command),
                              "FastrakClass::resetAlignment");

   if (success == TRUE) {
      for (int i = 0; i < 3; i++)
         for (int j = 0; j < 3; j++)
            alignment[station_num][i][j] = 0.0;

      alignment[station_num][1][0] = 1.0;        // X direction
      alignment[station_num][2][1] = 1.0;        // Y direction
   }

#if DEBUG
   cerr << "FASTRAK alignment reset.\n";
#endif

   return(success);
}  // FastrakClass::resetAlignment


//----------------------------------------------------------------------
//   Function: getAlignment
//    Returns: origin, x-axis, and y-axis vectors
// Parameters: station number
//    Summary: Get the alignment of the station.
//----------------------------------------------------------------------
void FastrakClass::getAlignment(FSTK_stations station_num,
                                 float origin[3],
                                 float x_point[3], float y_point[3])
{
   for (int i = 0; i < 3; i++) {
      origin[i]  = alignment[station_num][0][i];
      x_point[i] = alignment[station_num][1][i];
      y_point[i] = alignment[station_num][2][i];
   }
}  // FastrakClass::getAlignment()


//----------------------------------------------------------------------
//   Function: setBoresight
//    Returns: TRUE if the operation is successful else FALSE.
// Parameters: station number and orientation angles
//    Summary: Set the boresight of a station.
//----------------------------------------------------------------------
int FastrakClass::setBoresight(FSTK_stations station_num,
                               const float orient[3])
{
   if (!active_state[station_num]) {
      reportStateError("setBoresight", station_num);
      return(FALSE);
   }

   // Construct the "G" command to establish the boresight reference
   // angles.  Refer to pp 51 - 54, the 3 SPACE USER's MANUAL for
   // details.
   char command[50];
   sprintf(command, "G%d,%.1f,%.1f,%.1f\r", 
           station_num+1, orient[FSTK_AZ], orient[FSTK_EL], orient[FSTK_RO]);

   int success = sendCommand(command, strlen(command),
                              "FastrakClass::setBoresight");

   if (success == TRUE) {
      boresight[station_num][FSTK_AZ] = orient[FSTK_AZ];
      boresight[station_num][FSTK_EL] = orient[FSTK_EL];
      boresight[station_num][FSTK_RO] = orient[FSTK_RO];

      // Construct the "B" command to set the line_of_sight of the station.
      // Refer to pp 53, the 3 SPACE USER's MANUAL for details.
      sprintf(command, "B%d\r", station_num+1);

      success = sendCommand(command, strlen(command),
                            "FastrakClass::setBoresight");
   }

#if DEBUG
   cerr << "FASTRAK boresight set.\n";
#endif

   return(success);
}  // FastrakClass::setBoresight()


//----------------------------------------------------------------------
//   Function: resetBoresight
//    Returns: TRUE if operation is successful
// Parameters: station number
//    Summary: Reset the boresight.
//----------------------------------------------------------------------
int FastrakClass::resetBoresight(FSTK_stations station_num)
{
   if (!active_state[station_num]) {
      reportStateError("resetBoresight", station_num);

      return(FALSE);
   }

   // Construct the "b" command to reset the boresight of the station to
   // default. 
   // Refer to pp 55, the 3 SPACE USER's MANUAL for details.
   char command[10];
   sprintf(command, "b%d\r", station_num+1);

   int success = 
      sendCommand(command, strlen(command), "FastrakClass::resetBoresight");

   if (success == TRUE) {
      boresight[station_num][FSTK_AZ] = 0.0;
      boresight[station_num][FSTK_EL] = 0.0;
      boresight[station_num][FSTK_RO] = 0.0;
   }

#if DEBUG
   cerr << "FASTRAK boresight reset.\n";
#endif

   return(success);
}  // FastrakClass::resetBoresight()


//----------------------------------------------------------------------
//   Function: getBoresight
//    Returns: euler angles defining the boresight
// Parameters: station number
//    Summary: 
//----------------------------------------------------------------------
void FastrakClass::getBoresight(FSTK_stations station_num, float orient[3])
{
   orient[FSTK_AZ] = boresight[station_num][FSTK_AZ];
   orient[FSTK_EL] = boresight[station_num][FSTK_EL];
   orient[FSTK_RO] = boresight[station_num][FSTK_RO];
}  // FastrakClass::getBoresight()


//----------------------------------------------------------------------
//   Function: setHemisphere
//    Returns: TRUE if the operation is successful. Otherwise, FALSE.
// Parameters: station number and new zenith vector
//    Summary: Set the hemisphere of a station. 
//----------------------------------------------------------------------
int FastrakClass::setHemisphere(FSTK_stations station_num,
                                const float zenith[3])
{
   if (!active_state[station_num]) {
      reportStateError("setHemisphere", station_num);
      return(FALSE);
   }

   // Construct the "H" command to set the hemisphere of the station.
   // Refer to pp 88 - 92, the 3 SPACE USER's MANUAL for details.
   char command[50];
   sprintf(command, "H%d,%.1f,%.1f,%.1f\r", 
           station_num+1, zenith[FSTK_X], zenith[FSTK_Y], zenith[FSTK_Z]);

   int success = 
      sendCommand(command, strlen(command), "FastrakClass::setHemisphere");

   if (success == TRUE) {
      hemisphere[station_num][FSTK_X] = zenith[FSTK_X];
      hemisphere[station_num][FSTK_Y] = zenith[FSTK_Y];
      hemisphere[station_num][FSTK_Z] = zenith[FSTK_Z];
   }

#if DEBUG
   cerr << "FASTRAK hemisphere set:"
        << zenith[FSTK_X] << ", " << zenith[FSTK_Y] << ", "
        << zenith[FSTK_Z] << ".\n";
#endif

   return (success);
}  // FastrakClass::setHemisphere() 
 

//----------------------------------------------------------------------
//   Function: resetHemisphere
//    Returns: FALSE if station inactive or reset fails, TRUE otherwise
// Parameters: station number
//    Summary: Reset the hemisphere.
//----------------------------------------------------------------------
int FastrakClass::resetHemisphere(FSTK_stations station_num)
{
   if (!active_state[station_num]) {
      reportStateError("resetHemisphere", station_num);

      return(FALSE);
   }

   // Refer to pp 88 - 92, 3 SPACE USER's MANUAL for the default.
   float default_zenith[3];

   default_zenith[FSTK_X] = 1.0;
   default_zenith[FSTK_Y] = 0.0;
   default_zenith[FSTK_Z] = 0.0;

#if DEBUG
   cerr << "FASTRAK hemisphere reset.\n";
#endif

   return (setHemisphere(station_num, default_zenith));
}  // FastrakClass::resetHemisphere()


//----------------------------------------------------------------------
//   Function: getHemisphere
//    Returns: 
// Parameters: station number and zenith vector
//    Summary: Get the hemisphere of the station.
//----------------------------------------------------------------------
void FastrakClass::getHemisphere(FSTK_stations station_num, float zenith[3])
{
   zenith[FSTK_X] = hemisphere[station_num][FSTK_X];
   zenith[FSTK_Y] = hemisphere[station_num][FSTK_Y];
   zenith[FSTK_Z] = hemisphere[station_num][FSTK_Z];
}  // FastrakClass::getHemisphere()


//----------------------------------------------------------------------
//   Function: setUnits
//    Returns: TRUE if the operation is successful. Otherwais FALSE.
// Parameters: 
//    Summary: Set the position measuring unit for the FASTRAK.
//----------------------------------------------------------------------
int FastrakClass::setUnits(FSTK_units pos_units)
{
   // Construct the "U/u" command to set the unit for the FASTTRAC data.
   // Refer to pp 122 - 124, the 3 SPACE USER's MANUAL for details.
   char command[2];
   if (pos_units == FSTK_CENTIMETER)
      strcpy(command, "u\0");
   else if (pos_units == FSTK_INCH)
      strcpy(command, "U\0");
   else {
      cerr << "Error: invalid units specification " << pos_units << endl;
      return (FALSE);
   }

   int success = sendCommand(command, 1, "FastrakClass::setUnits");
   if (success) units = pos_units;

#if DEBUG
   cerr << "FASTRAK position units set.\n";
#endif

   return(success);
}  // end FastrakClass::setUnits


//----------------------------------------------------------------------
//   Function: setDataTypes
//    Summary: Specify the requested data types for the station. Before
//           : data can be read from the FASTRAK, the types of the data
//           : needed must be specified. By default, position
//           : coordinates and Euler orientations are returned from each
//           : station. 
// Parameters: station number, datatype mask
//    Returns: TRUE if the operation is successful. Otherwise, FALSE.
//----------------------------------------------------------------------
int FastrakClass::setDataTypes(FSTK_stations station_num, short mask)
{
   if (!active_state[station_num]) {
      reportStateError("setDataTypes", station_num);

      return(FALSE);
   }

   int  success = TRUE;

   // The following piece of code is a critical section in multiprocess
   // mode.  When data record parameters are being updated, data records
   // cannot be allowed to be read by getPacket() which runs in
   // parallel with this method.
   if ((parpoll_pid != -1) && (!param_set_flag)) {
      // entering the critical section
      if (uspsema(paramsema) == -1) {
              success = FALSE;
         report_syserr("getting semaphore",
                       "FastrakClass::setDataTypes");
      }
   }

   // Adjust the data record parameters.
   int i, j;
   fstk_packet_size -= datarec_size[station_num];

   datarec_size[station_num] = FSTK_HEADER_SIZE;

   for (i = 0; i < FSTK_NUM_DATATYPES; i++)
      datatype_start[station_num][i] = -1; 

   // Construct the "O" command to specify the type of data
   // we want from the FASTRAK station.
   // Refer to pp 97 - 111, the 3 SPACE USER's MANUAL for details.
   char command[20];
   sprintf(command, "O%d", station_num+1);

   if (mask & FSTK_COORD_MASK) {
      strcat(command, ",2");
      datatype_start[station_num][FSTK_COORD_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_COORD_SIZE;
   }

   if (mask & FSTK_EULER_MASK)  {
      strcat(command, ",4");
      datatype_start[station_num][FSTK_EULER_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_EULER_SIZE;
   }

   if (mask & FSTK_XCOS_MASK) {
      strcat(command, ",5");
      datatype_start[station_num][FSTK_XCOS_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_XCOS_SIZE;
   }

   if (mask & FSTK_YCOS_MASK) {
      strcat(command, ",6");
      datatype_start[station_num][FSTK_YCOS_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_YCOS_SIZE;
   }

   if (mask & FSTK_ZCOS_MASK) {
      strcat(command, ",7");
      datatype_start[station_num][FSTK_ZCOS_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_ZCOS_SIZE;
   }

   if (mask & FSTK_QUAT_MASK) {
      strcat(command, ",11");
      datatype_start[station_num][FSTK_QUAT_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_QUAT_SIZE;
   }

   if (mask & FSTK_16BIT_COORD_MASK) {
      strcat(command, ",18");
      datatype_start[station_num][FSTK_16BIT_COORD_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_16BIT_COORD_SIZE;
   }

   if (mask & FSTK_16BIT_EULER_MASK) {
      strcat(command, ",19");
      datatype_start[station_num][FSTK_16BIT_EULER_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_16BIT_EULER_SIZE;
   }

   if (mask & FSTK_16BIT_QUAT_MASK) {
      strcat(command, ",20");
      datatype_start[station_num][FSTK_16BIT_QUAT_TYPE] =
         datarec_size[station_num];
      datarec_size[station_num] += FSTK_16BIT_QUAT_SIZE;
   }

   if (mask & FSTK_CRLF_MASK) {
      strcat(command, ",1");
      datarec_size[station_num] += FSTK_CRLF_SIZE;
   }

   strcat(command, "\r");

   // recompute the maximum station data record size
   max_datarec_size = 0;
   for (i=0; i<FSTK_NUM_STATIONS; i++) {
      if (datarec_size[station_num] > max_datarec_size)
         max_datarec_size = datarec_size[station_num];
   }

#if DEBUG 
   cerr << "** setDataTypes:\n"
        << "   The command: " << command << "\n" 
        << "   Expecting station" << station_num+1 
        << " to contain " << datarec_size[station_num]
        << " bytes." << endl;
#endif

   // Note that sendCommand() is not be used here because 
   // adjusting data record params and sending the command must be in
   // the same critical section.
   if (write(port_fd, command, strlen(command)) == -1) {
      success = FALSE;
      report_syserr("sending FASTRAK O command",
                    "FastrakClass::setDataTypes");
   }
   
   datatype_mask[station_num] = mask;
   fstk_packet_size += datarec_size[station_num];

   for (i = station_num+1; i < FSTK_NUM_STATIONS; i++) {
      if (active_state[i]) {
         for (j = 0; j < FSTK_NUM_DATATYPES; j++)
            datatype_start[i][j] += datarec_size[station_num]; 
      }
   }

   data_ready_flag = FALSE; // See getPacket() for when it is set to TRUE.
   if ((parpoll_pid != -1) && (param_set_flag == FALSE))
      usvsema(paramsema);   // exiting the critical section

#if DEBUG
   cerr << "FASTRAK data type specified.\n";
   cerr << "Packet size set to " << fstk_packet_size << " bytes.\n";
#endif

   return(success);
}  // end FastrakClass::setDataTypes


//----------------------------------------------------------------------
//   Function: setState
//    Summary: Set the sate of the station: TRUE (active) or FALSE
//           : (inactive).  Note  that this routine may seem to be more
//           : complex than necessary. The complexity is due to the need
//           : to handle the different requirements at and after the
//           : initialization stage. This routine can be simplified by
//           : spliting it into two separate ones. However, then the
//           : program becomes longer.
// Parameters: station number, state
//    Returns: TRUE if the operation is successful. Otherwise, FALSE.
//----------------------------------------------------------------------
int FastrakClass::setState(FSTK_stations station_num, int active_flag)
{
   if (!active_setting[station_num])  {
      reportStateError("setState", station_num);
      return(FALSE);
   }
  
   int success = TRUE;

   // When fstk_packet_size = 0, the program is in the initialization stage,
   // where setState() is called from checkState(). Otherwise,
   // fstk_packet_size > 0. The initialization has finished and setState()
   // is called by the FASTRAK user.
   if ((fstk_packet_size == 0) ||
       (active_state[station_num] != active_flag)) {
      int i, j, active_station = -1, num_actives = 0;

      for (i = 0; i < FSTK_NUM_STATIONS; i++) {
         if (active_state[i]) {
            num_actives++;
            active_station = i;
         }
      }

      // Error! trying to deactivate the last remaining station.
      // At any time, at least one station must be active:
      if ((num_actives <= 1) &&
          (active_station == station_num) && !(active_flag)) {
         cerr << BELL << "Error in setting FASTRAK station"
              << station_num+1 << " state.\n"
              << "  in *FastrakClass.cc*FastrakClass::setState"
              << "  At least, one station must be active at any time."
              << endl;
         return(FALSE);
      }

      // Construct the "l" command to set the state of the station.
      // Refer to pp 128 - 131, the 3 SPACE USER's MANUAL for details.
      char command[10];
      int state_cmd = 0;
      if (active_flag) state_cmd = 1;

      sprintf(command, "l%d,%d\r", station_num+1, state_cmd);

#if DEBUG
      cerr << "** setState:\n"
           << "   The command: " << command << endl;
#endif

      // This is a critical section.
      if (parpoll_pid != -1) {
         if (uspsema(paramsema) == -1) {  // entering the critical section
            success = FALSE;
            report_syserr("getting semaphore", "FastrakClass::setState");
         }
      }

      // Note that sendCommand() is not be used here because
      // sending the command and updating data record params must be in
      // the same critical section.
      if (write(port_fd, command, strlen(command)) != -1) {
         active_state[station_num] = active_flag;
         
         // Update data record parameters when the change is from ACTIVE
         // to INACTIVE. On the other hand, when the change is from
         // INACTIVE to ACTIVE, setDataTypes() is called to update
         // record parameters. 
         if (!active_flag && (fstk_packet_size > 0)) {
            fstk_packet_size -= datarec_size[station_num];

            for (i = station_num+1; i < FSTK_NUM_STATIONS; i++) {
               if (active_state[i]) {
                  for (j = 0; j < FSTK_NUM_DATATYPES; j++)
                     datatype_start[i][j] -= datarec_size[station_num];
               }
            }

            datarec_size[station_num] = 0;
            for (i = 0; i < FSTK_NUM_DATATYPES; i++) {
               datatype_start[station_num][i] = -1;
            }
         }
         else if (active_flag) {
            param_set_flag = TRUE;
            setDataTypes(station_num, FSTK_DEFAULT_MASK);
            param_set_flag = FALSE;
         }
      }
      else {
         success = FALSE;
         report_syserr("sending FASTRAK l command",
                       "FastrakClass::setState");
      }  // end if (write())

      data_ready_flag = FALSE;  // See getPacket() for when it is set to TRUE.

      // exiting the critical section
      if (parpoll_pid != -1) usvsema(paramsema);
   }

#if DEBUG
   cerr << "FASTRAK station" << station_num << " state set." << endl;
#endif

   return(success);
}  // end FastrakClass::setState()


//----------------------------------------------------------------------
//   Function: copyBuffer()
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void FastrakClass::copyBuffer()
{
   // Use uspsema(paramsema) instead of ussetlock(datalock) if the
   // object of this class is to be used in multiple processes.
   while (!data_ready_flag) ;
   if (ussetlock(datalock) == -1)   // locking for update
      report_syserr("getting lock", "FastrakClass::readData");

   for (int station_num=0; station_num<FSTK_NUM_STATIONS; station_num++) {
      memcpy(datarec[station_num], datarec_buf[station_num],
             datarec_size[station_num]);
   }

   // Use usvsema(paramsema) if the object is to be used in multiple
   // processes.
   usunsetlock(datalock);        // unlocking
}


//----------------------------------------------------------------------
//   Function: readData
//    Summary: Decompose the data packet in the current data buffer. For
//           : a succesful read, data_dest[] contains the required type
//           : of data from the specified station.  Note that (1)
//           : data_dest[] must be a 4-element array for quaternions;
//           : for the other types, it is a 3-element array; (2) old
//           : data can be reused if invalid data packets were read by
//           : getPacket().
// Parameters: station number, required data types
//    Returns: TRUE if the read is succesful. Otherwise, return FALSE.
//----------------------------------------------------------------------
int FastrakClass::readData(FSTK_stations station_num,
                           FSTK_datatypes data_type,
                           float data_dest[])
{
   if (checkReadError(station_num, "readData", data_type) == FALSE) {
      return(FALSE);
   }

/*
   // Use uspsema(paramsema) instead of ussetlock(datalock) if the
   // object of this class is to be used in multiple processes.
   while (!data_ready_flag) ;
   if (ussetlock(datalock) == -1)   // locking for update
      report_syserr("getting lock", "FastrakClass::readData");
 */
#if DEBUG
   cerr << "** readData:\n"
        << "   station" << station_num+1 << "\n"
        << "   record start = " << datarec[station_num]
        << "; record size = " << datarec_size[station_num]
        << "; total size = " << fstk_packet_size
        << "; data type start = "
        << datatype_start[station_num][data_type] << endl;

   debugData(datarec[station_num], datarec_size[station_num]);
#endif

   // the starting position of the type of data wanted in the buffer
   char* start_pos = datarec[station_num] +
                     datatype_start[station_num][data_type];

   switch (data_type) {
      case FSTK_COORD_TYPE:
      case FSTK_EULER_TYPE:
      case FSTK_XCOS_TYPE: 
      case FSTK_YCOS_TYPE:
      case FSTK_ZCOS_TYPE:
         convertData(start_pos, 3, data_dest);
         break;

      case FSTK_QUAT_TYPE:
         convertData(start_pos, 4, data_dest);
         break;

      case FSTK_16BIT_COORD_TYPE:
         if (units == FSTK_CENTIMETER)
            convert16BITData(start_pos, 3,
                             FSTK_16BIT_TO_CM, data_dest);
         else
            convert16BITData(start_pos, 3,
                             FSTK_16BIT_TO_INCHES, data_dest);
         break;

      case FSTK_16BIT_EULER_TYPE:
         convert16BITData(start_pos, 3,
                          FSTK_16BIT_TO_DEGREES, data_dest);
         break;

      case FSTK_16BIT_QUAT_TYPE:
         convert16BITData(start_pos, 3,
                          FSTK_16BIT_TO_QUAT, data_dest);
         break;
   }

/*
   // Use usvsema(paramsema) if the object is to be used in multiple
   // processes.
   usunsetlock(datalock);        // unlocking
 */
   return(TRUE);
}  // end readData()


//----------------------------------------------------------------------
//   Function: getHmatrix
//    Summary: Read a homogeneous transformation matrix. On a successful
//           : return, the upper left 3x3 submatrix of matrix[][]
//           : contains a tranformation matrix constructed from the
//           : X-cosin, Y-cosin and Z-cosin vectors of the station with
//           : X-consin in the first row, Y-cosin in the second, and
//           : Z-cosin in the third; if FSTK_COORD_TYPE has been chosen
//           : in setDataTypes, the fourth COLUMN will contain the
//           : position of the sensor wrt the transmitter, otherwise,
//           : it is filled with 0.  The fourth ROW is filled with 0's
//           : except that matrix[3][3] = 1.
// Parameters: station number
//    Returns: TRUE if the read is succesful. Otherwise, return FALSE.
//----------------------------------------------------------------------
int FastrakClass::getHMatrix(FSTK_stations station_num,
                             float matrix[4][4])
{
   // the starting pos. of the types of data wanted in the data record
   char* start_pos; 

   matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
   matrix[3][3] = 1;
/*
   // Use uspsema(paramsema) instead of ussetlock(datalock) if the
   // object of this class is to be used in multiple processes.
   while (!data_ready_flag) ;
   if (ussetlock(datalock) == -1)   // locking for update
      report_syserr("getting lock", "FastrakClass::getHmatrix");
 */

   // get the position vector if FSTK_COORD_TYPE has been specified
   if (datatype_mask[station_num]&FSTK_16BIT_COORD_MASK) {
      float pos[3];
      start_pos = datarec[station_num] +
         datatype_start[station_num][FSTK_16BIT_COORD_TYPE];

      if (units == FSTK_CENTIMETER)
         convert16BITData(start_pos, 3, FSTK_16BIT_TO_CM, pos);
      else
         convert16BITData(start_pos, 3, FSTK_16BIT_TO_INCHES, pos);

      for (int i=0; i<3; i++) matrix[i][3] = pos[i];
   }
   else if (datatype_mask[station_num]&FSTK_COORD_MASK) {
      float pos[3];
      start_pos = datarec[station_num] +
         datatype_start[station_num][FSTK_COORD_TYPE];
      convertData(start_pos, 3, pos);
      for (int i=0; i<3; i++) matrix[i][3] = pos[i];
   }
   else {
      matrix[0][3] = matrix[1][3] = matrix[2][3] = 0;
   }

   // get the rotation matrix one of three ways
   if (datatype_mask[station_num]&(FSTK_QUAT_MASK|FSTK_16BIT_QUAT_MASK)) {
      float quat[4];

      if (datatype_mask[station_num]&FSTK_16BIT_QUAT_MASK) {
         start_pos = datarec[station_num] +
            datatype_start[station_num][FSTK_16BIT_QUAT_TYPE];
         convert16BITData(start_pos, 4, FSTK_16BIT_TO_QUAT, quat);
      }
      else {
         start_pos = datarec[station_num] +
            datatype_start[station_num][FSTK_QUAT_TYPE];
         convertData(start_pos, 4, quat);
      }
//      usunsetlock(datalock);       // unlocking

      // compute the rotation matrix from the quaternion info
      float xx = quat[1]*quat[1];
      float yy = quat[2]*quat[2];
      float zz = quat[3]*quat[3];
      float xy = quat[1]*quat[2];
      float yz = quat[2]*quat[3];
      float xz = quat[1]*quat[3];
      float sx = quat[0]*quat[1];
      float sy = quat[0]*quat[2];
      float sz = quat[0]*quat[3];

      matrix[0][0] = 1.0 - 2.0*(yy + zz); 
      matrix[0][1] = 2.0*(xy - sz);
      matrix[0][2] = 2.0*(xz + sy);

      matrix[1][0] = 2.0*(xy + sz);
      matrix[1][1] = 1.0 - 2.0*(xx + zz);
      matrix[1][2] = 2.0*(yz - sx);

      matrix[2][0] = 2.0*(xz - sy);
      matrix[2][1] = 2.0*(yz + sx);
      matrix[2][2] = 1.0 - 2.0*(xx + yy);
   }
   else if (datatype_mask[station_num]&
            (FSTK_EULER_MASK|FSTK_16BIT_EULER_MASK)) {
      float angles[3];

      if (datatype_mask[station_num]&FSTK_16BIT_EULER_MASK) {
         start_pos = datarec[station_num] +
            datatype_start[station_num][FSTK_16BIT_EULER_TYPE];
         convert16BITData(start_pos, 3, FSTK_16BIT_TO_DEGREES, angles);
      }
      else {
         start_pos = datarec[station_num] +
            datatype_start[station_num][FSTK_EULER_TYPE];
         convertData(start_pos, 3, angles);
      }
//      usunsetlock(datalock);       // unlocking

      // compute rotation matrix from the euler angle info
      angles[FSTK_AZ] *= DTOR;
      angles[FSTK_EL] *= DTOR;
      angles[FSTK_RO] *= DTOR;

      float ca = cos(angles[FSTK_AZ]);
      float sa = sin(angles[FSTK_AZ]);
      float ce = cos(angles[FSTK_EL]);
      float se = sin(angles[FSTK_EL]);
      float cr = cos(angles[FSTK_RO]);
      float sr = sin(angles[FSTK_RO]);

      float sesr = se*sr;
      float secr = se*cr;

      matrix[0][0] = ca*ce; 
      matrix[0][1] = ca*sesr - sa*cr;
      matrix[0][2] = ca*secr + sa*sr;

      matrix[1][0] = sa*ce;
      matrix[1][1] = sa*sesr + ca*cr;
      matrix[1][2] = sa*secr - ca*sr;

      matrix[2][0] = -se;
      matrix[2][1] = ce*sr;
      matrix[2][2] = ce*cr;
   }
   else if ((datatype_mask[station_num]&FSTK_XCOS_MASK) &&
            (datatype_mask[station_num]&FSTK_YCOS_MASK) &&
            (datatype_mask[station_num]&FSTK_ZCOS_MASK)) {
      for (int row = 0; row <3; row++) {
         start_pos = datarec[station_num] +
            datatype_start[station_num][row + FSTK_XCOS_TYPE];
         convertData(start_pos, 3, matrix[row]);
      }
//      usunsetlock(datalock);       // unlocking
   }
   else {
      cerr << "Error: no orientation information to build H-matrix\n";
//      usunsetlock(datalock);       // unlocking
      return (FALSE);
   }

   return(TRUE);
}  // end getHmatrix()


//----------------------------------------------------------------------
//   Function: getPosOrient
//    Summary: Read the current position and orientation of the station
//           : together.  On a successful return, posit[] contains the
//           : position and orient[] contains the orientation.  The type
//           : of the orientation, euler-angle and quaternion, is
//           : determined by orient_type.  Note that if orient_type is
//           : FSTK_EULER_TYPE, orient is a 3-element array. Otherwise, it
//           : must be a 4-element array.
// Parameters: station number, type of orientation
//    Returns: TRUE if the read is succesful. Otherwise, return FALSE.
//----------------------------------------------------------------------
int FastrakClass::getPosOrient(FSTK_stations station_num,
                               FSTK_datatypes orient_type,
                               float pos[3], float orient[])
{
   char* start_pos;

   if (checkReadError(station_num, "getPosOrient",
                      orient_type) == FALSE) {
         return(FALSE);
   }
/*
   // Use uspsema(paramsema) instead of ussetlock(datalock) if the
   // object of this class is to be used in multiple processes.
   while (!data_ready_flag) ;
   if (ussetlock(datalock) == -1)   // locking for update
      report_syserr("getting lock", "FastrakClass::getPosOrient");
 */
   // get the position vector
   if (datatype_mask[station_num]&FSTK_16BIT_COORD_MASK) {
      start_pos = datarec[station_num] +
         datatype_start[station_num][FSTK_16BIT_COORD_TYPE];

      if (units == FSTK_CENTIMETER)
         convert16BITData(start_pos, 3, FSTK_16BIT_TO_CM, pos);
      else
         convert16BITData(start_pos, 3, FSTK_16BIT_TO_INCHES, pos);
   }
   else if (datatype_mask[station_num]&FSTK_COORD_MASK) {
      start_pos = datarec[station_num] +
         datatype_start[station_num][FSTK_COORD_TYPE];
      convertData(start_pos, 3, pos);
   }
   else {
      cerr << "Error: no position type selected in "
           << "FastrackClass::getPosOrient\n";
      return(FALSE);
   }

   // get orientation vector
   start_pos = datarec[station_num] +
                     datatype_start[station_num][orient_type];

   switch (orient_type) {
      case FSTK_EULER_TYPE:
         convertData(start_pos, 3, orient);
         break;

      case FSTK_QUAT_TYPE:
         convertData(start_pos, 4, orient);
         break;

      case FSTK_16BIT_EULER_TYPE:
         convert16BITData(start_pos, 3,
                          FSTK_16BIT_TO_DEGREES, orient);
         break;

      case FSTK_16BIT_QUAT_TYPE:
         convert16BITData(start_pos, 3,
                          FSTK_16BIT_TO_QUAT, orient);
         break;
      default:
         cerr << "Error: invalid orientation type specified in "
              << "FastrackClass::getPosOrient\n";
         return(FALSE);
   }
/*
   usunsetlock(datalock);       // unlocking
 */
   return(TRUE);
}  // end getPosOrient()
