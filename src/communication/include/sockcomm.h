/*
 * File:  sockcomm.h                             Version: 001
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

#ifndef __NPS_SOCKCOMM__
#define __NPS_SOCKCOMM__

#include <sys/types.h>
#include <sys/socket.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

enum SockOperation { SC_SOCKET_DEFAULT, SC_SOCKET_WRITE, SC_SOCKET_READ };
enum SockState     { SC_SOCKET_CLOSED, SC_SOCKET_OPEN,  SC_SOCKET_ERROR };
enum SockKind      { SC_UNIX_STREAM, SC_UNIX_DATAGRAM, SC_TCP_STREAM,
                     SC_UDP_BROADCAST, SC_UDP_MULTICAST, SC_UDP_UNICAST,
                     SC_CONNECT_ORIENTED, SC_CONNECT_LESS,
                     SC_UNIX_PROTO, SC_IP_PROTO, SC_UDP, SC_TCP,
                     SC_SOCKET };

class SockCommClass {

public:

   SockCommClass ();
   virtual ~SockCommClass ();

   int open_communication ( const SockOperation /*socket_operation*/ );
      // Opens the socket for either reading or writing depending
      // on the value of "socket_operation".  Returns TRUE if the
      // operation was successfull, FALSE otherwise.  If the socket
      // was already open, the existing socket is closed and another
      // one is created in it's place for the new operation.

   int close_communication ();
      // Closes the socket.  Returns TRUE if successfull, FALSE otherwise.

   unsigned int write_buffer ( const char * /*buffer*/,
                               const unsigned int /*buffer length*/);
      // Writes the first "buffer length" bytes from "buffer" to
      // the socket.  Returns the actual number of bytes that were
      // successfully written.

   unsigned int read_buffer ( char * /*buffer*/,
                              const unsigned int /*buffer size/length*/,
                              char * /*sender*/ );
      // Fills the "buffer" with incoming data from the socket.
      // If using connection-oriented socket, then "buffer size/length"
      // should be pre-set with the actual number of bytes desired from
      // the read.
      // If using connection-less socket, then "buffer size/length" should
      // be pre-set to the maximum number of bytes the buffer can hold.
      // Passed in buffer "sender" will contain a string identifying the
      // source or sender of the data.
      // Returns the actual number of bytes that were successfully written.
   
   virtual int isa ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is a type of the given "query kind".
      // FALSE otherwise.
   virtual int isaexact ( SockKind /*query_kind*/ );
      // Returns TRUE if the class is exactly of the given "query kind".
      // FALSE otherwise.
   virtual int get_sock_id ();
      // Returns the socket id.

protected:

   int socketid   ;        // Socket descriptor, endpoint for communication.
   SockOperation sockop;   // Desired operation for socket, read/write. 
   SockState sockstate;    // Current state of the socket.
   struct sockaddr 
      *socket_address;     // Socket address structure ptr
   int address_size;       // Socket address structure size
   int sockfamily;         // Socket family
   int socktype;           // Socket type


   // Member functions provided by this class
   // *******************************************************************

   void print_error ( const char */*origin_string*/, 
                      const int /*error_num*/);
      // Prints an error description to stderr for the error num
      // indicated in addition to the origin_string passed in.

   void set_socket_to_default ();
      // Sets all socket related member variables to a known default state.

   int open_socket ( const SockOperation /*socket_operation*/,
                     int & /*error_num*/ );
      // Attempts to open a socket, returns TRUE/FALSE, if FALSE, then
      // error_num is set and can be printed by obtaining the error string
      // with strerror().

   int close_socket ( int & /*error_num*/ );
      // Attemps to close a socket, TRUE/FALSE, if FALSE, then
      // error_num is set and can be printed by obtaining the error string
      // with strerror().

   // Pure virtual member functions that must be provided by any
   // derived classes.
   // *******************************************************************

   virtual int configure_socket ( int & /*error_num*/ ) = 0;
      // Configures the socket according to the inherited class features

   virtual char *get_origin ( sockaddr * /*from*/ ) = 0;
      // Gets an identifying string of who sent a message (originator)

   virtual int write_to_socket ( const char * /*buffer*/,
                                 const unsigned int /*length*/,
                                 int & /*error_num*/ ) = 0;
      // Writes the buffer to the socket and returns the number of bytes
      // sent.  On error -1 is returned and error_num is set.

   virtual int read_from_socket ( char * /*buffer*/,
                                  struct sockaddr & /*from*/,
                                  const unsigned int /*length*/,
                                  int & /*error_num*/ ) = 0;
      // Reads from the socket into the buffer and returns the senders
      // information.  On error, -1 is returned and error_num is set.

   virtual int filter ( sockaddr * /*from*/ ) = 0;
      // Returns true to filter data based on who sent it (originator)


private:

};

#endif

// End file - sockcomm.h
