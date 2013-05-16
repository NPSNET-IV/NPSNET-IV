// *********************************************************************
// File    : FastrakClass.h
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

#ifndef __NPS_FASTRAK_CLASS__
#define __NPS_FASTRAK_CLASS__

#include <ulocks.h>
#include <fstream.h>

// Boolean values
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

const int PORT_NAME_SIZE = 48;

// Assumptions and limitations which influence the use of the FASTRAK
// program:
//
// 1). I assume that the user of the FASTRAK program will use it in a
//     single process, say the application process in PERFORMER, that
//     is, a instance of the "FastrakClass" class will only be used in a
//     process in which it is created.  Hopefully, this is not too
//     restricted. 
//
//     The problem with the current version when it is used in multiple
//     processes is that the read_data(), read_posorient() and
//     read_homomatrix() methods are not guarded with the
//     data-record-parameter-updating binary semophere.  Instead, a lock
//     is used for guarding data buffer switching. Refer to the
//     implementation for details. 
//
//     It is easy to fix this problem. Basically, what you need to do is
//     to guard the two methods with the above binary semophere. The
//     cost is that now the
//     read_data()/read_posorient()/read_homomatrix() method and the
//     get_packet() method are almost totally mutually exclusive, which
//     may slow the system performance down. If this is done, the lock
//     for guarding data buffer switching can be eliminated.
//
// 2). The FASTRAK program was witten so that it can be used to process
//     any number of sensors. The only thing you need to do is to change
//     the constant FSTK_NUM_STATIONS to the number of sensors you
//     have. However, there is a limit caused by the system on the
//     number of sensors you can use. Basically, the problem is that the
//     data buffer size, BUFFER_SIZE, is constrained by the size of the
//     largest one-dimensional array allowed by the system. 


// Conventions used in this file and FastrakClass.cc:
//
//    All the constants and data types intended to be used by the user
//    of the FSTK start with the prefix FSTK. Those that do not have
//    this prefix should be used only in this file and FastrakClass.cc.
//
//    All constants are in capital letters.

// Terms used in this file and FastrakClass.cc:
//
//    station: Each trasmitter and receiver pair is called a station in
//             the 3SPACE USER'S MANUAL. 

// The algorithm:
//
//    In single process mode, a single process, which is the one which
//    creates the the "FastrakClass" object, requests a required type of data
//    packet from the FSTK when it needs one. 
//
//    In multiprocess mode, the process which creates the "FastrakClass"
//    object spawns a light weight child process which continuously
//    polls the FSTK to get a required type of data packet, which is the
//    data producer and runs in parallel with the parent process, the
//    data consumer.
//
//    The data packet is decomposed in the parent (or single) process,
//    the consumer, to generate the required type of data when a data is
//    requested by the user of the FSTK. 

// the masks used to specify the types of data requested from the FSTK
#define FSTK_COORD_MASK       0x001   // Cartesian coordinate (X,Y,Z)
#define FSTK_EULER_MASK       0x002   // Euler angles (Azim,Elev,Roll)
#define FSTK_XCOS_MASK        0x004   // X-axis directional cosines
#define FSTK_YCOS_MASK        0x008   // Y-axis directional cosines
#define FSTK_ZCOS_MASK        0x010   // Z-axis directional cosines
#define FSTK_QUAT_MASK        0x020   // Quaternion (W, X, Y, Z)
#define FSTK_16BIT_COORD_MASK 0x040   // 16BIT format coordinate data
#define FSTK_16BIT_EULER_MASK 0x080   // 16BIT format euler angles
#define FSTK_16BIT_QUAT_MASK  0x100   // 16BIT format quaternion
#define FSTK_CRLF_MASK        0x200   // the framing CR/LF characters
#define FSTK_DEFAULT_MASK     0x0c0   // 16BIT COORD and EULER_MASKs

// The sizes of the types of data returned from the FSTK -- in bytes
// IEEE single precision floating point format has 4 bytes per number
// 16BIT obviously has two bytes per number.
#define FSTK_HEADER_SIZE      3   // data record header from the FSTK
#define FSTK_COORD_SIZE       12  // Position coordinates
#define FSTK_EULER_SIZE       12  // Euler angles
#define FSTK_XCOS_SIZE        12  // X-axis directional cosines
#define FSTK_YCOS_SIZE        12  // Y-axis directional cosines
#define FSTK_ZCOS_SIZE        12  // Z-axis directional cosines
#define FSTK_QUAT_SIZE        16  // Quaternion
#define FSTK_16BIT_COORD_SIZE 6   // 16BIT format coordinates
#define FSTK_16BIT_EULER_SIZE 6   // 16BIT format euler angles
#define FSTK_16BIT_QUAT_SIZE  8   // 16BIT format quaternions
#define FSTK_CRLF_SIZE        2   // Carriage Return, Line Feed 

// There can be up to 4 stations active at the same time in the FASTRAK.
#define FSTK_NUM_STATIONS 4

#define MAX_PACKET_SIZE   91  // All of the above summed together
#define BUFFER_SIZE       364 // MAX_PACKET_SIZE*FSTK_NUM_STATIONS

// 16BIT format requires scaling information for the various types of
// data:
#define FSTK_16BIT_TO_CM        (300.0/8192)
#define FSTK_16BIT_TO_INCHES    (118.11/8192)
#define FSTK_16BIT_TO_DEGREES   (180.0/8192)
#define FSTK_16BIT_TO_QUAT      (1.0/8192)

// Station numbers
enum FSTK_stations 
      { FSTK_STATION1 = 0,    // This may be bad programming style, but
        FSTK_STATION2,        // these are used to index into arrays.
        FSTK_STATION3,
        FSTK_STATION4 };

// The FASTTRACk can return up to 6 different types of data.
#define FSTK_NUM_DATATYPES  9                // number of data types
enum FSTK_datatypes 
        { FSTK_COORD_TYPE = 0, // This may be bad programming style, but
          FSTK_EULER_TYPE,     // these are used to index into arrays.
          FSTK_XCOS_TYPE, 
          FSTK_YCOS_TYPE,
          FSTK_ZCOS_TYPE,
          FSTK_QUAT_TYPE,
          FSTK_16BIT_COORD_TYPE,
          FSTK_16BIT_EULER_TYPE,
          FSTK_16BIT_QUAT_TYPE };

// units used to measure the FSTK positions 
enum FSTK_units
        { FSTK_INCH, FSTK_CENTIMETER };

//======================================================================
// FastrakClass definitions
//======================================================================

class FastrakClass
{
private:
   int   port_fd;                      // the serial port file descriptor
   char  port_name[PORT_NAME_SIZE];     // the name of the Fastrak port

   // info for the individual byte buffers for the four Fastrak stations.
   char  datarec[FSTK_NUM_STATIONS][MAX_PACKET_SIZE]; 
   char  datarec_buf[FSTK_NUM_STATIONS][MAX_PACKET_SIZE]; 
                                           // data records for each station
   short max_datarec_size;                 // size of the largest station pkt
   short datarec_size[FSTK_NUM_STATIONS];  // data rec. size for each station
   short datatype_mask[FSTK_NUM_STATIONS]; // data types for each station
   short datatype_start[FSTK_NUM_STATIONS][FSTK_NUM_DATATYPES];        
                                           // the position of each requested
                                           //   type in the data record

   short fstk_packet_size;           // the sum of the data record sizes
                                     //   for all the active stations, i.e.,
                                     //   the size of a complete data
                                     //   packet returned from the Fastrak 
   char  read_buffer[BUFFER_SIZE];   // pollContinuously's temporary buffer
   unsigned int   read_index;        // current location in temp buffer

   // Process ID and process for the serial port polling process.
   int parpoll_pid;        
   friend void pollContinuously(void *); // the sproc'ed fcn to read port
   void getPacket();                 // read a packet from the FSTK


   // locks and binary semaphores for ensuring mutual exclusive access 
   // to critical sections.
   // Note that a boolean flag, param_setting, is used together with the
   // semaphore.  Basically, when the consumer process is already
   // holding the binary semaphore, it should not request the semaphore
   // again. Otherwise, deadlock would occur.  It is needed when a
   // consumer data record parameter setting method needs to call
   // another such method. Now there is one such case, set_state()
   // calling specify_datatype(). Note that param_set_flag should never
   // be used in the data producer process.
   ulock_t datalock;    // a lock used to guide switching data buffers
   usema_t *paramsema;  // a binary semaphore used to guide setting data
                        //    packet parameters.
   usptr_t *arena;      // arena used to create lock and semaphore

   // Boolean flags
   int param_set_flag;  // TRUE if the data packet parameters are being set.
   int valid_flag;      // TRUE if initializing the FSTK is successfull
   int is_polling_flag; // TRUE if the papallel polling process is not
                        //    being suspended 
   volatile int data_ready_flag; // TRUE if new data has been read
                                 //    after control parameter update 
   volatile int kill_flag;       // TRUE if exiting the parallel polling
                                 //    process has been requested 

   // Fastrak state variables:
   int active_setting[FSTK_NUM_STATIONS];   // h/w switch settings
   int active_state[FSTK_NUM_STATIONS];     // s/w setting     
   float alignment[FSTK_NUM_STATIONS][3][3];// pp. 42-50 in User's Manual
   float boresight[FSTK_NUM_STATIONS][3];   // pp. 51-55
   float hemisphere[FSTK_NUM_STATIONS][3];  // pp. 88-92
   FSTK_units units;                        // CENTIMETERS and INCHES

   // private methods
   void initState();                    // init the member variables
   int  readConfig(ifstream &config_fileobj); // read the config file
   int  openIOPort();                   // open the FSTK serial io-port
   int  initMultiprocessing();          // initialize multiprocess mode

   int  checkState();                   // check which station is active

   void prepareToRead();                // parallel/serial i/f to getPacket

   int  sendCommand(char* command,      // send a command to the FSTK
                    int length,
                    char* source);

   void convertData(char* data,         // convert IEEE buffer data to n
                    int num_floats,     // floats:  DOS ordered bytes to
                    float data_dest[]); // Unix (reversed)

   void convert16BITData(char* data,    // convert a 16BIT format buffer
                    int num_floats,     // to n IEEE floating point nums
                    float scale,
                    float data_dest[]);


   // functions for debugging and error checking
   void debugData(char *data_store, int num_of_bytes);
   int  detectError();                  // detect errors in data packet 
   void reportStateError(char* location,
                         FSTK_stations station_num);
   int  checkReadError(FSTK_stations station_num, char* source,
                       FSTK_datatypes data_type);

   //  Define what values are requested from the station.  
   //  Values requested should be ORed together using the mask.
   //  Return TRUE if the operation is successful. 
   //  See page 97-111, the 3SPACE USER'S MANUAL.
   int setDataTypes(FSTK_stations station_num, short mask);

   // Set the state of the station and return TRUE if successful. 
   //  See page 128-131, the 3SPACE USER'S MANUAL.
   //
   // Note that when the state update is from FALSE (INACTIVE) to TRUE
   // (ACTIVE), a call
   // to setDataTypes() should follow the call to set_state() to
   // specify the data types.  By default, FSTK_DEFAULT_MASK is used.
   int setState(FSTK_stations station_num, int active_flag);

   // alignment reference frame functions:
   void getAlignment(FSTK_stations station_num, 
                     float origin[3],
                     float x_point[3], float y_point[3]);
   int  setAlignment(FSTK_stations station_num,
                     const float origin[3],
                     const float x_point[3],
                     const float y_point[3]);
   int  resetAlignment(FSTK_stations station_num);

   // boresight functions:
   void getBoresight(FSTK_stations station_num, float orient[3]); 
   int  setBoresight(FSTK_stations station_num, const float orient[3]);
   int  resetBoresight(FSTK_stations station_num);

   // active hemisphere functions. 
   void getHemisphere(FSTK_stations station_num, float zenith[3]);
   int  setHemisphere(FSTK_stations station_num, const float zenith[3]);
   int  resetHemisphere(FSTK_stations station_num); 

   // position measurement units. FSTK_CENTIMETER is default.
   int setUnits(FSTK_units pos_units);
   inline FSTK_units getUnits() const {
      return(units);
   }

public:
   // This constructor expects the name of a configuration file for the
   // FASTRACK and a flag list indicating data types desired.
   FastrakClass(ifstream &config_fileobj,
                short datatype_flags = FSTK_DEFAULT_MASK);
   ~FastrakClass();              // destructor

   // Return true if initializing the FSTK is successful.
   inline int exists() const {
      return valid_flag;
   }

   // In multiprocess mode, suspend and resume the execution of the
   // parallel polling process, the data producer. During the
   // suspension, no new data is produced.  Return TRUE if the
   // the is successful. 
   int suspend();
   void resume();

   // Get the state of the station.
   inline int getState(FSTK_stations station_num) const {
      return(active_state[station_num]);
   }

   // move data to second buffer (reduces lock overhead)
   void copyBuffer();

   // Read the specified type of data from the specified station.
   // For a successful read, data_dest[] contains the result.
   // Note that data_dest[] must be a 4-element array for quaternions; 
   // for the other types of data, data_dest[] is a 3-element array. 
   // Return TRUE when the read is successful.
   int readData(FSTK_stations station_num, FSTK_datatypes data_type, 
                float data_dest[]);

   // Read a homogeneous transformation matrix of the sensor with
   // respect to the transmitter. For a successful read, the upper left
   // 3x3 submatrix of matrix[][] contains the rotation matrix
   // constructed from the quaternion, euler angles, or X-, Y- and
   // Z-directional cosines (depending on which type was selected) of
   // the station which results in the X-cosine in the first row,
   // Y-cosine in the second, and Z-cosine in the third; if
   // FSTK_COORD_TYPE has been selected, the last column contains the
   // position of the station, otherwise, it is filled with 0.  Return
   // TRUE if the read is succesful. Otherwise, return FALSE.
   int getHMatrix(FSTK_stations station_num, float Hmatrix[4][4]);

   // Read the current position and orientation of the station together.
   // On a successful return, posit[] contains the position and orient[]
   // contains the orientation of the station. The type of the
   // orientation, euler-angle and quaternion, is determined by
   // orient_type.  Note that if orient_type is FSTK_EULER_TYPE, orient
   // is a 3-element array.  Otherwise, it must be a 4-element array.
   // Return TRUE if the read is succesful. Otherwise, return FALSE.
   int getPosOrient(FSTK_stations station_num,
                    FSTK_datatypes orient_type,
                    float pos[3], float orient[]);
};

#endif

