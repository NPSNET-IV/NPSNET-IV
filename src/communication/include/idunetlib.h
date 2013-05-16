// File: <disnetlib.h>

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


#ifndef __NPS_IDU_NET__
#define __NPS_IDU_NET__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/soioctl.h>
#include <ulocks.h>             // arena
#include <sys/prctl.h>          // sproc()
#include <signal.h>             // kill()
#include <assert.h>


#ifndef FALSE
#   define FALSE	0
#endif

#ifndef TRUE
#   define TRUE	!FALSE
#endif

#ifndef IN_ADDR_SIZE
#define IN_ADDR_SIZE    4
#endif

#include "idu.h"        // list this first

#define IDU_DEF_BUF_SIZE 300

#define IDU_DEF_BC_PORT 3201

#define IDU_DEF_MC_GROUP "224.200.200.200"
#define IDU_DEF_MC_TTL 3
#define IDU_DEF_MC_PORT 3200

#define IDU_SEND_BUFFER_SIZE        65000
#define IDU_RECEIVE_BUFFER_SIZE     65000

#define IDU_NUM_SEND_TRIES		3

#define IDU_FILE_NAME_SIZE 255
#define IDU_NET_NAMES_SIZE 25

typedef struct 
   {
   union IDU		*idu;
   int			current_idu;
   int			over_written_idus;
   int			over_write_mode;
   int			unread_idus;
   } idu_buffer;

enum IDUnetmode { IDU_ERROR_MODE, IDU_BCAST_MODE,
                  IDU_MCAST_MODE, IDU_BOTH_MODE };

class IDU_net_manager {

public:

   /* Broadcast constructor */
   IDU_net_manager ( const unsigned short receive_port = IDU_DEF_BC_PORT,
                     char *net_interface = NULL,
                     const int loopback = FALSE,
                     const int buffer_length = IDU_DEF_BUF_SIZE );

   /* Multicast constructor */
   IDU_net_manager ( const char *group = NULL,
                     const unsigned short receive_port = IDU_DEF_MC_PORT,
                     const unsigned char ttl = IDU_DEF_MC_TTL,
                     char *net_interface = NULL,
                     const int loopback = FALSE,
                     const int buffer_length = IDU_DEF_BUF_SIZE );

   /* Destructor */
   ~IDU_net_manager ();

   /* Returns TRUE if the constructor was able to open the network       */
   /* connection. Otherwise, FALSE and the net is not open.              */
   int net_open ();

   /* Reads a single IDU from the net.  Returns the number of IDUs       */
   /* waiting to be read from the net including the one currently being  */
   /* read.                                                              */
   int read_idu ( char **, IDUType *, sender_info &, int & );

   /* Writes a single IDU to the net.  Returns TRUE if the operation was */
   /* successful.  Otherwise, FALSE and the IDU was not sent.            */
   int write_idu ( char *, IDUType );

   /* Closes the network connections. */
   void net_close ();

   /* Enable local host loopback of IDU packets */
   int enable_loopback ();

   /* Disabled local host loopback of IDU packets */
   int disable_loopback ();

   /* By default, all IDU types are received.  If the default behavior   */
   /* has been modified by using the other three functions below, then   */
   /* receive_all_idus resets to the default behavior to recieve all.    */
   int receive_all_idus ();

   /* Blocks all IDUs from being received. */
   int receive_no_idus ();

   /* Enables a specific IDU type to be received.  The first time this   */
   /* is called, the default behavior is changed so that only the        */
   /* requested type will be received.  Subsequent calls increases the   */
   /* number of types received.                                          */
   int add_to_receive_list ( IDUType );

   /* Disables a specific IDU type from being received.                  */
   int remove_from_receive_list ( IDUType );


private:

   /* Variables */

   static int           num_net_readers;
   static int		arena_counter;

   usptr_t		*netarena;
   char			arena_name[IDU_FILE_NAME_SIZE];
   usema_t		*swap_buf_sema;
   usema_t		*start_reading_sema;
   ulock_t              has_data;
   int			first_obj_read;

   char			interface[IDU_NET_NAMES_SIZE];
   struct in_addr       net_inet_addr;
   char                 ascii_inet_addr[IDU_NET_NAMES_SIZE];
   struct sockaddr_in	dest;


   IDUnetmode		network_mode;
   int			sock_send;
   int			sock_recv;
   struct sockaddr_in	sin_send;
   struct sockaddr_in	sin_recv;

   char			mc_group[IDU_NET_NAMES_SIZE];
   unsigned char	mc_ttl;
   volatile u_char	*loopback;
   unsigned short	port;

   int			producer_pid;
   int			consumer_pid;

   int			up_and_running;
   volatile int		*net_read_flag;


   volatile idu_buffer	*buffer;
   volatile int		*current_net_buffer;
   int			max_idus;
   int			num_idu_types;
   volatile u_char	*receive_list;
   volatile int		*receive_all;

  
   /* Functions */
   void init_member_vars();
   int configure ( const int );
   int init_arena (); 
   int allocate_shared_memory();
   int init_sockets();
   void dump_idu ( char *, IDUType );
   friend void idu_receiver_process ( void *tempthis );

};

#endif
