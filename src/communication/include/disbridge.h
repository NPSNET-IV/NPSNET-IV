// File: <disbridge.h>

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

#ifndef __NPS_DIS_BRIDGE__
#define __NPS_DIS_BRIDGE__

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
#include "rwstruct.h"


#ifndef FALSE
#   define FALSE	0
#endif

#ifndef TRUE
#   define TRUE	!FALSE
#endif

#define IN_ADDR_SIZE    4

#include "pdu.h"        // list this first
#include "appearance.h"
#include "articulat.h"
#include "category.h"
#include "country.h"
#include "datum.h"
#include "radar.h"
#include "repair.h"
#include "sites.h"

#define DIS_EXERCISE_ALL	0

#define DEFAULT_BC_EXERCISE	DIS_EXERCISE_ALL
#define DEFAULT_BC_PORT		3000

#define DEFAULT_MC_EXERCISE	DIS_EXERCISE_ALL
#define DEFAULT_MC_GROUP	"224.2.121.93"
#define DEFAULT_MC_TTL		3
#define DEFAULT_MC_PORT		3111

#define DEFAULT_UC_EXERCISE	1
#define DEFAULT_UC_PORT		3005
#define DEFAULT_UC_INET_ADDRESS	"131.120.7.51"

#define SEND_BUFFER_SIZE	65000
#define RECEIVE_BUFFER_SIZE	65000

#define NUM_SEND_TRIES		3

#define FILE_NAME_SIZE		255
#define NET_NAMES_SIZE		25


enum network_mode	{ ERROR_MODE, IP_BROADCAST, IP_MULTICAST, IP_UNICAST };
enum convert_direction	{ NO_CONVERSION, FLAT_TO_ROUND, ROUND_TO_FLAT };
enum coordinate_mode	{ DEFAULT, FLAT_WORLD, ROUND_WORLD };

typedef struct
   {
   char			interface[NET_NAMES_SIZE];
   unsigned short	port;
   char			mc_group[NET_NAMES_SIZE];
   unsigned char	mc_ttl;
   network_mode		mode;
   convert_direction	round_conv_direction;
   unsigned char	exercise_id;
   int			socket;
   struct sockaddr_in	socket_info; 
   struct in_addr	internet_addr;
   char			ascii_internet_addr[NET_NAMES_SIZE];
   struct sockaddr_in	dest;
   } interface_info;


/* Convenience functions */

int  pdutype_to_name ( PDUType, char * );


/**********************************************************************/ 
class DIS_bridge {

public:

   /* Constructor */
   DIS_bridge ( const char *write_net_interface,
                const network_mode write_network_mode,
                const char *read_net_interface,
                const network_mode read_network_mode );
               
   /* Destructor */ 
   ~DIS_bridge ();

   /* Unicast */
   void define_read_socket ( const unsigned short port,
                             const char *address );
   void define_write_socket ( const unsigned short port,
                              const char *address );

   /* Broadcast */
   void define_read_socket ( const unsigned short port ); 
   void define_write_socket ( const unsigned short port );

   /* Multicast */
   void define_read_socket ( const unsigned short port,
                            const char *group,
                            const unsigned char ttl ); 
   void define_write_socket ( const unsigned short port,
                             const char *group,
                             const unsigned char ttl );

   /* Both Broadcast and Multicast */
   void define_read_exercise ( unsigned char exercise );
   void define_write_exercise ( unsigned char exercise );

   int define_round_world ( const roundWorldStruct *round_world_file );
   int define_read_coord_conversion ( const convert_direction direction );
   int define_write_coord_conversion ( const convert_direction direction );

   /* Geographic filters */
   /* After round conversion on read if any */
   int restrict_bounds ( double minx, double miny, double minz,
                          double maxx, double maxy, double maxz,
                          convert_direction mode = NO_CONVERSION );
  
   void bound_around_entity ( EntityID entity_info );

   /* PDU type filters */   
   int pass_all_pdus ();
   int pass_no_pdus ();
   int pass_pdu ( PDUType );
   int no_pass_pdu ( PDUType );

   /* Debugging help */
   void print_feedback ( int true_or_false = TRUE );
   void verify_pdu_size ( int true_or_false = TRUE );

   /* Open (start) and close (stop) the DIS bridge */
   int net_open ();
   void net_close ();

/**********************************************************************/

private:

   /* Private Variables */

   static int           	num_net_bridges;

   int				init_ok;
   int				up_and_running;
   int				num_pdu_types;

   volatile u_char		*receive_list;
   volatile int			receive_all;
   volatile int			feedback;
   volatile int			verify_size;
   volatile int			bounds_check;
   volatile coordinate_mode	bounds_mode;
   EntityID			bounding_entity;
   volatile int			bound_based_on_entity;
   volatile double		min_x;
   volatile double		min_y;
   volatile double		min_z;
   volatile double		max_x;
   volatile double		max_y;
   volatile double		max_z;
   volatile int			net_read_flag;
   volatile int			round_defined;

   char				destination[NET_NAMES_SIZE];
   int				bridge_pid;

   volatile interface_info	read_info;
   volatile interface_info	write_info;
   
    
   /* Private Functions */
   void	init_member_vars();
   int	get_interface ( network_mode mode, int read_or_write );
   int	open_read_socket ( network_mode mode );
   int	open_write_socket ( network_mode mode );
   int	write_pdu ( char *pdu, PDUType type );

   friend void bridge_process ( void *tempthis );

};

/**********************************************************************/

#endif
