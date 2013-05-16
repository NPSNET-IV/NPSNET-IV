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


#ifndef __NPS_DIS_NET__
#define __NPS_DIS_NET__

#include <ulocks.h> // needed for locks and semaphores in class
#include <netinet/in.h> // needed for socket address structures
#include "pdu.h"    // needed for PDU definitions
#include "rwstruct.h"

#ifndef FALSE
#   define FALSE	0
#endif

#ifndef TRUE
#   define TRUE	!FALSE
#endif

#define DEFAULT_BUF_SIZE 300

#ifndef DEFAULT_BC_EXERCISE
#define DEFAULT_BC_EXERCISE 1
#endif

#define DEFAULT_BC_PORT 3000

#ifndef DEFAULT_MC_EXERCISE
#define DEFAULT_MC_EXERCISE 1
#endif

#define DEFAULT_MC_GROUP "224.2.121.93"
#define DEFAULT_MC_TTL 3
#define DEFAULT_MC_PORT 3111

#define FILE_NAME_SIZE 255
#define NET_NAMES_SIZE 25

typedef struct 
   {
   union PDU		*pdu;
   int			current_pdu;
   int			over_written_pdus;
   int			over_write_mode;
   int			unread_pdus;
   } pdu_buffer;

enum netmode { ERROR_MODE, BCAST_MODE, MCAST_MODE, BOTH_MODE };

class DIS_net_manager {

public:

   DIS_net_manager ( const unsigned char exercise = DEFAULT_BC_EXERCISE,
                    const unsigned char roundworld = 0,
                    const roundWorldStruct *roundfile = NULL,
                    const int buffer_length = DEFAULT_BUF_SIZE,
                    char *net_interface = NULL,
                    const unsigned short receive_port = DEFAULT_BC_PORT );

   DIS_net_manager ( const char *group = NULL,
                    const unsigned char ttl = DEFAULT_MC_TTL,
                    const unsigned short receive_port = DEFAULT_MC_PORT,
                    const unsigned char exercise = DEFAULT_MC_EXERCISE,
                    const unsigned char roundworld = 0,
                    const roundWorldStruct *roundfile = NULL,
                    const int buffer_length = DEFAULT_BUF_SIZE,
                    char *net_interface = NULL );

   ~DIS_net_manager ();

   int net_open ();
   int read_pdu ( char **, PDUType *, sender_info &, int & );
   int write_pdu ( char *, PDUType );
   int write_buffer ( char *, int );
   void net_close ();

   int pdutype_to_name ( PDUType , char * );

   int receive_all_pdus ();
   int receive_no_pdus ();
   int add_to_receive_list ( PDUType );
   int remove_from_receive_list ( PDUType );

   int add_to_filter_list ( EntityID, int use_time = 0 );
   int remove_from_filter_list ( EntityID );
   inline int in_filter_list ( EntityID );
   void review_filter_list ();



private:

   /* Variables */

   static int           num_net_readers;
   static int		arena_counter;

   usptr_t		*netarena;
   char			arena_name[FILE_NAME_SIZE];
   usema_t		*swap_buf_sema;
   usema_t		*start_reading_sema;
   ulock_t              has_data;
   int			first_obj_read;

   char			interface[NET_NAMES_SIZE];
   struct in_addr       net_inet_addr;
   char                 ascii_inet_addr[NET_NAMES_SIZE];
   struct sockaddr_in	dest;


   netmode		network_mode;
   int			sock_send;
   int			sock_recv;
   struct sockaddr_in	sin_send;
   struct sockaddr_in	sin_recv;

   char			mc_group[NET_NAMES_SIZE];
   unsigned char	mc_ttl;
   unsigned char	loopback;
   unsigned short	port;

   int			producer_pid;
   int			consumer_pid;

   int			up_and_running;
   volatile int		*net_read_flag;

   int			round_world;
   roundWorldStruct	*round_file;
   unsigned char	exercise_id;

   volatile pdu_buffer	*buffer;
   volatile int		*current_net_buffer;
   int			max_pdus;
   int			num_pdu_types;
   volatile u_char	*receive_list;
   volatile int		*receive_all;
   volatile EntityID	*filter_list;
   volatile int		*filter_count;
   volatile int		*time_list;

  
   /* Functions */
   void init_member_vars();
   int configure ();
   int init_arena (); 
   int allocate_shared_memory();
   int init_sockets();

   friend void receiver_process ( void *tempthis );

};

#endif
