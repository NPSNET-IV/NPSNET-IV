// File: <isotrack.h>

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
// File:        isotrack.h
// Created:     August 16, 1993 - September 20, 1993
// Programmer:  Paul T. Barham
//              Naval Postgraduate School
//              Code CS/Barham
//              Monterey, CA 93943
//              barham@cs.nps.navy.mil

// Purpose:  This file contains the declarations for a C++ class to manage
//           the Polhemus 3Space Isotrack.
// 
// For more information on the operation of the Isotrack, see the 3SPACE
//    USER'S MANUAL.

#ifndef __NPS_ISOTRACK__
#define __NPS_ISOTRACK__

#include <ulocks.h>


// Define filename for the shared data arena used by processes.
#define IARENA_FILE "/tmp/isotrack.arena.data"

// Define permission option to chmod command to alter permissions on arena
// file to be read and write for everyone.
#define ARENA_PERMISSIONS 0666


//Define bits for "data_request" mask
#define COORDINATES 0x01     // Cartesian coordinates (X,Y,Z)
#define ORIENTATION 0x02     // Orientation (Azimuth,Elevation,Roll)
#define XCOSINES 0x04        // X-axis directional cosines
#define YCOSINES 0x08        // Y-axis directional cosines
#define ZCOSINES 0x10        // Z-axis directional cosines
#define QUARTERNION 0x20     // Orientation quarternion
#define DEFAULT_REQUEST 0x03 // COORDINATES and ORIENTATION


//Define sizes of data items
                           // Size in bytes of:
#define COOR_SIZE 21       //    Coordinates (3x7 - format %7.2f)
#define ORIE_SIZE 21       //    Orientation (3x7 - format %7.2f)
#define XCOS_SIZE 21       //    X-axis directional cosines (3x7 - format %7.4f)
#define YCOS_SIZE 21       //    Y-axis directional cosines (3x7 - format %7.4f)
#define ZCOS_SIZE 21       //    Z-axis directional cosines (3x7 - format %7.4f)
#define QUAR_SIZE 28       //    Orientation quarternion (4x7 - format %7.4f)
#define CRLF_SIZE 2        //    Carriage Return, Line Feed 
#define STAT_SIZE 3        //    Status bytes from Isotrack
#define MAX_SIZE 140       //    All of the above summed together
                           //       plus 2 bytes for safety


#define MAX_REQ 20         // Maximum length in bytes of data request string


//Define possible units for measurements
#define CENTIMETERS 0
#define INCHES 1 
#define DEFAULT_UNITS CENTIMETERS 

#define MAXINCHES 65.48f
#define MAXCENTIMETERS 166.32f

//Define standard Boolean values
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


// Define isotrack class

class isotrack {

private:

   int portfd;             // Serial Port file descriptor
   int valid;              // Boolean indicating successfull communications
   int hasdata;            // Boolean indicating read data received
   int is_reading;         // Flag true if parallel process is reading data
   int isotrack_pid;       // Process ID for parallel poll_continuous
   int killflag;           // Signal poll_continuous process to die
   char station_num;       // The station number
   ulock_t datalock;       // Semaphore for mutual read/write exclusion
   usema_t *resetsema;     // Semaphore for suspending parallel producer
   usptr_t *arena;         // Multiprocessing arena
   short output_mask;      // Output mask for values desired from sensor
   char buffer1[MAX_SIZE]; // Buffer for holding sensor data
   char buffer2[MAX_SIZE]; // Another buffer for holding sensor data
   char *data;             // Pointer to current buffer holding data to be read
   int units;              // Units being used for measuring Isotrack
   int debug_flag;         // Debug flag
   int poll_data_size;     // Number of bytes of data to receive from Isotrack
   char poll_request[MAX_REQ];  // Request to Isotrack for data

   int poll ( int tries = 2 );
   friend void iso_poll_continuous ( void *tempthis );

public:

   // Constructor for class expects the name of the serial port to use and
   //    flags indicating whether multiprocess and debug modes are desired.
   isotrack ( char station, char *port_name, int multi = TRUE, 
              int debug = FALSE );

   // Destructor for class.
   ~isotrack ();

   // Return true if a port has been opened and an isotrack has responded to an
   //    initial pole.
   int exists () const { return valid; }

   // Return true if there is new data since the last clear data.
   int new_data ();

   // Acquire new data from Isotrack.
   void get_data ();

   // Clear out last data read.
   void clear_data ();

   // Reinitialize the entire system to the power-up state.
   int initialize ();

   // Resets the alignment reference frame for the sensor to that of the source
   int reset_alignment();

   // Sets the alignment reference frame for the sensor relative to that of
   // the source.  See pages 5-5 through 5-8 of 3SPACE USER'S MANUAL.
   int set_alignment ( float &originX, float &originY, float &originZ,
                       float &x_axisX, float &x_axisY, float &x_axisZ,
                       float &y_axisX, float &y_axisY, float &y_axisZ );

   // Resets the boresight orientation for the sensor coordinate frame to
   // the reference orientation defined by the source coordinate frame.
   int reset_boresight ();

   // Defines a new boresight orientation for the sensor coordinate frame.
   // See pages 5-10 and 5-11 of 3SPACE USER'S MANUAL. 
   int set_boresight ();

   // Resets active hemisphere to be the hemisphere whose zenith is in the
   // direction of the sensor x-axis.
   int reset_hemisphere ();

   // Defines the active hemisphere to be the hemisphere whose zenith is
   // in the direction of LOS vector.  See pages 5-18 and 5-19 of 3SPACE
   // USER'S MANUAL.
   int set_hemisphere ( float &losX, float &losY, float &losZ );


   // If state is true, quiet mode is enabled allowing input signal
   // averaging for smoother motion.  If false, input signal averaging
   // is disabled.  See pages 5-3 and 5-4 of 3SPACE USER'S MANUAL.
   int quiet_mode ( int state = TRUE );

   // Define what values are desired from the sensor when polled.  Values
   //    desired should be ORed together using the mask constants.
   // See pages 4-6 and 4-7 in the 3SPACE USER'S MANUAL.
   int data_request ( short mask );

   // Set the units returned by the sensor to inches or centimeters using
   // the defined constants INCHES and CENTIMETERS.
   int set_units ( int measure );

   // Suspend the Isotrack read process (i.e. producer thread) thus
   // putting it to sleep until resume is called.  Implies no new data.
   void suspend ();

   // Awakens previous suspended Isotrack read process thus resuming
   // data flow and new data arrival.
   void resume ();

   // Turn debug mode on or off for more descriptive messages.
   void debug ( int flag = TRUE ) { debug_flag = flag; }

   // Returns current units in use by Isotrack - either INCHES or CENTIMETERS.
   int get_units () const { return units; }

   // Returns 0 if no error occured during last read of Isotrack data;
   // otherwise returns 1 and outputs error message to standard error.
   int get_status ();

   // Return 1 and the Cartesian position of the sensor if the position is part
   //    of the data request mask.  Otherwise return 0.
   int get_position ( float &X, float &Y, float &Z );

   // Return 1 and the orientation of the sensor if the orientation is part
   //    of the data request mask.  Otherwise return 0.
   int get_orientation ( float &azimuth, float &elevation, float &roll );

   // Return 1 and the X cosines of the sensor if the X cosines are part
   //    of the data request mask.  Otherwise return 0.
   int get_X_cosines ( float &alpha, float &beta, float &gamma );

   // Return 1 and the Y cosines of the sensor if the Y cosines are part
   //    of the data request mask.  Otherwise return 0.
   int get_Y_cosines ( float &alpha, float &beta, float &gamma );

   // Return 1 and the Z cosines of the sensor if the Z cosines are part
   //    of the data request mask.  Otherwise return 0.
   int get_Z_cosines ( float &alpha, float &beta, float &gamma );

   // Return 1 and the quarternion of the sensor if the quarternion is part
   //    of the data request mask.  Otherwise return 0.
   int get_quarternion ( float &q0, float &q1, float &q2, float &q3 );

};

#endif

