// File: <disbridge.cc>

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

#include <iostream.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bstring.h>
#include <string.h>
#include <sys/wait.h>

#ifdef RUNTIME_STATS
#include <curses.h>
#endif

#include "disbridge.h"
#include "disconvert.h"

/* Local constants */
#define NETREAD 0
#define NETWRITE 1


/* Local type defintions */
typedef struct 
   {
   struct in_addr	net_inet_addr;
   char			ascii_inet_addr[NET_NAMES_SIZE];
   char			interface[NET_NAMES_SIZE];
   struct sockaddr_in	dest;
   } valid_intrf_info;


/* External coordinate conversion error flag */
extern volatile int G_coord_error;


/* Static member initializations */
int	DIS_bridge::num_net_bridges = 0;


/* Local prototypes */
inline void
swap_dbl ( volatile double &, volatile double & );

void
signal_handler (int, ... );

inline int
in_bounds ( EntityLocation, double, double, double, double, double, double );


/* Member functions */

/* init_member_vars          ***********************************************/

void
DIS_bridge::init_member_vars()
   {
   /* num_net_bridges is static and is initialized above */

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge init_member_vars started." << endl;
#endif

   /* Initialize simple member variables to a default (safe) state */
   init_ok = TRUE;
   up_and_running = FALSE;
   num_pdu_types = 0;
   receive_list = NULL;
   receive_all = 0;
   feedback = FALSE;
   verify_size = FALSE;
   bounds_check = FALSE;
   min_x = 0.0;
   min_y = 0.0;
   min_z = 0.0;
   max_x = 0.0;
   max_y = 0.0;
   max_z = 0.0;
   net_read_flag = TRUE;
   round_defined = FALSE;
   bridge_pid = -1;
   bzero ( (char *)&bounding_entity, sizeof(EntityID) );
   bound_based_on_entity = FALSE;
 
   /* Initialize read socket information */ 
   read_info.port = 0;
   strcpy ( (char *)read_info.mc_group, "" );
   read_info.mc_ttl = 0;
   read_info.mode = ERROR_MODE;
   read_info.round_conv_direction = NO_CONVERSION;
   read_info.exercise_id = 0;
   read_info.socket = -1;
   bzero ( (char *)&read_info.socket_info, sizeof(sockaddr_in) );
   bzero ( (char *)&read_info.internet_addr, sizeof(in_addr) );
   strcpy ( (char *)read_info.ascii_internet_addr, "" );
   bzero ( (char *)&read_info.dest, sizeof(sockaddr_in) );

   /* Initialize write socket information by copying from read information */
   bcopy ( (char *)&read_info, (char *)&write_info, sizeof(interface_info) );

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge init_member_vars finished." << endl;
#endif

   } /* init_member_vars */


/* get_interface             ***********************************************/

int
DIS_bridge::get_interface ( network_mode mode, int read_or_write )
   {
   valid_intrf_info	valid[4];
   int			i, current, count, found;
   char			buf[BUFSIZ];
   struct ifconf	ifc;
   struct ifreq		*ifr;
   struct sockaddr_in	*in_addr;
   int			sock_temp;
   char			interface[NET_NAMES_SIZE];

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge get_interface started." << endl;
#endif

   if ( read_or_write == NETREAD )
      strncpy ( interface, (char *)read_info.interface, NET_NAMES_SIZE-1 );
   else
      strncpy ( interface, (char *)write_info.interface, NET_NAMES_SIZE-1 );

   /* Open temporary socket for gathering interface information */
   if ((sock_temp = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to open temporary socket -\n   ");
      return (FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge temporary socket opened." << endl;
#endif

   /* Get InterFace CONFig */
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock_temp, SIOCGIFCONF, (char *) &ifc) < 0)
      {
      close(sock_temp);
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
      perror("ERROR:\tDIS_bridge unable to get interface config -\n   ");
      return (FALSE);
      }

   /* Check interfaces (assumes same interface for sender and receiver) */
   ifr = ifc.ifc_req;           /* ptr to interface structure */
   current = 0;
   for (i = 0; i < (ifc.ifc_len / sizeof(struct ifreq)); i++, ifr++)
      {

      if (ifr->ifr_addr.sa_family != AF_INET)   /* Internet only */
         continue;

      /* Get InterFace FLAGS */
      if (ioctl(sock_temp, SIOCGIFFLAGS, (char *) ifr) < 0)
         {
         close(sock_temp);
         perror("ERROR: DIS_bridge unable to get interface flags -\n   ");
#ifdef DEBUG
         cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
         return (FALSE);
         }

      /*  Skip boring cases */
      if ( ((ifr->ifr_flags & IFF_UP) == 0) ||            /* interface down? */
           (ifr->ifr_flags & IFF_LOOPBACK)  ||            /* local loopback? */
           ((mode == IP_BROADCAST)&&((ifr->ifr_flags&IFF_BROADCAST) == 0)) ||
           ((mode == IP_MULTICAST)&&((ifr->ifr_flags&IFF_MULTICAST) == 0)) )
         {
         continue;
         }
      /* Get and save InterFace ADDRess */
      if (ioctl(sock_temp, SIOCGIFADDR, (char *) ifr) == -1)
         {
         close(sock_temp);
#ifdef DEBUG
         cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
         perror("ERROR:\tDIS_bridge unable to determine inet address-\n   ");
         return (FALSE);
         }
      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);

      valid[current].net_inet_addr = in_addr->sin_addr;
      strncpy ( valid[current].ascii_inet_addr,
               inet_ntoa(valid[current].net_inet_addr), NET_NAMES_SIZE-1 );
      strncpy ( valid[current].interface, ifr->ifr_name, NET_NAMES_SIZE-1 );

      if ( mode == IP_BROADCAST )
         {
         /* Get InterFace BRoaDcast ADDRess */
         if (ioctl(sock_temp, SIOCGIFBRDADDR, (char *) ifr) < 0)
            { 
            close(sock_temp);
#ifdef DEBUG
            cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
            perror("ERROR:\tDIS_bridge unable to determine BCAST address -\n   ");
            return (FALSE);
            }
         bcopy((char *) &(ifr->ifr_broadaddr),
               (char *) &(valid[current].dest),
               sizeof(ifr->ifr_broadaddr));
         }


#ifdef DATA
         cerr << "DATA:\tDIS_bridge detected ";
         if ( mode == IP_BROADCAST )
            cerr << "broadcast";
         else if ( mode == IP_MULTICAST )
            cerr << "multicast";
         else if ( mode == IP_UNICAST )
            cerr << "unicast";
         cerr << " capable interface = " << valid[current].interface << endl;
         cerr << "DATA:\tDIS_bridge detected ";
         if ( mode == IP_BROADCAST )
            cerr << "broadcast";
         else if ( mode == IP_MULTICAST )
            cerr << "multicast";
         else if ( mode == IP_UNICAST )
            cerr << "unicast";
         cerr << " capable interface inet = "
              << valid[current].ascii_inet_addr << endl;
         if ( mode == IP_BROADCAST )
            {
            cerr << "DATA:\tDIS_bridge detected broadcast address of "
                 << inet_ntoa(valid[current].dest.sin_addr) << endl;
            }
#endif

      current++;

      }

   if ( current < 1 )
      {
      close(sock_temp);
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
      cerr << "ERROR:\tDIS_bridge cannot find a valid interface." << endl;
      return (FALSE);
      }
   else
      {
      found = FALSE;
      count = 0;
      while ( (!found) && (count < current) )
         {
         if ( !strcmp("",interface) )
            {
            found = TRUE;
            if ( current > 1 )
               {
               if ( read_or_write == NETREAD )
                  count = 0;
               else
                  count = 1;
               cerr << "WARNING:\tDIS_bridge arbitrarily chose the "
                    << "interface " << valid[count].interface;
               if ( read_or_write == NETREAD )
                  cerr << " for reading." << endl;
               else
                  cerr << " for writing." << endl;
               cerr << "\tThis machine has " << current << " interfaces."
                    << endl;
               cerr << "\tOther possible choices were:" << endl;
               for ( int temp = 0; temp < current; temp++ )
                  cerr << "\t\t" << valid[temp].interface << endl;
               }
#ifdef DEBUG
            else
               {
               cerr << "DEBUG:\tDIS_bridge chose interface "
                    << valid[count].interface;
               if ( read_or_write == NETREAD )
                  cerr << " for reading by default." << endl;
               else
                  cerr << " for writing by default." << endl;
               }
#endif
            }
         else if ( !strcmp(interface,valid[count].interface ) )
            {
            found = TRUE;
#ifdef DEBUG
            cerr << "DEBUG:\tDIS_bridge chose interface "
                 << valid[count].interface;
            if ( read_or_write == NETREAD )
               cerr << " for reading as requested." << endl;
            else
               cerr << " for writing as requested." << endl;
#endif
            }
         else
            count++;
         }
      if (!found)
         {
         count = 0;
         if ( current == 1 )
            {
            cerr << "WARNING:\tDIS_bridge could not find the requested"
                 << " interface:  " << interface;
            if ( read_or_write == NETREAD )
               cerr << " for reading." << endl;
            else
               cerr << " for writing." << endl;
            cerr << "\tUsing " << valid[count].interface << " instead." << endl;
            }
         else
            {
            cerr << "ERROR:\tDIS_bridge could not find the requested"
                 << " interface:  " << interface;
            if ( read_or_write == NETREAD )
               cerr << " for reading." << endl;
            else
               cerr << " for writing." << endl;
            cerr << "\tYour possible choices are:" << endl;
            for ( int temp = 0; temp < current; temp++ )
               cerr << "\t\t" << valid[temp].interface << endl;
            close(sock_temp);
#ifdef DEBUG
            cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif
            return (FALSE);
            }
         }
      if ( read_or_write == NETREAD )
         {
         read_info.internet_addr = valid[count].net_inet_addr;
         strncpy ( (char *)read_info.ascii_internet_addr, 
                   valid[count].ascii_inet_addr,
                   NET_NAMES_SIZE-1 );
         strncpy ( (char *)read_info.interface, valid[count].interface,
                   NET_NAMES_SIZE-1 );
         if ( mode == IP_BROADCAST )
            bcopy((char *) &(valid[count].dest),
                  (char *) &(read_info.dest),
                  sizeof(ifr->ifr_broadaddr));
#ifdef DATA
         cerr << "DATA:\tDIS_bridge read interface = "
              << read_info.interface << endl;
         cerr << "DATA:\tDIS_bridge read internet addr = "
              << read_info.ascii_internet_addr << endl;
         if ( mode == IP_BROADCAST )
            cerr << "DATA:\tDIS_bridge read broadcast address = "
                 << inet_ntoa(read_info.dest.sin_addr) << endl;
#endif
         }
      else
         {
         write_info.internet_addr = valid[count].net_inet_addr;
         strncpy ( (char *)write_info.ascii_internet_addr, 
                   valid[count].ascii_inet_addr,
                   NET_NAMES_SIZE-1 );
         strncpy ( (char *)write_info.interface, valid[count].interface,
                   NET_NAMES_SIZE-1 );
         if ( mode == IP_BROADCAST )
            bcopy((char *) &(valid[count].dest),
                  (char *) &(write_info.dest),
                  sizeof(ifr->ifr_broadaddr));
#ifdef DATA
         cerr << "DATA:\tDIS_bridge write interface = "
              << write_info.interface << endl;
         cerr << "DATA:\tDIS_bridge write internet addr = "
              << write_info.ascii_internet_addr << endl;
         if ( mode == IP_BROADCAST )
            cerr << "DATA:\tDIS_bridge write broadcast address = "
                 << inet_ntoa(write_info.dest.sin_addr) << endl;
#endif
         }

      }

   close(sock_temp);

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge temporary socket closed." << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge get_interface finished." << endl;
#endif

   return (TRUE);

   }


/* open_read_socket          ***********************************************/

int 
DIS_bridge::open_read_socket ( network_mode mode )
   {
   int on = 1;
   struct ip_mreq       mreq;
   int rcvbuff = RECEIVE_BUFFER_SIZE;

#ifdef TRACE
   cerr << "TRACE:\topen_read_socket started." << endl;
#endif

   /* Open receive socket */
   if ((read_info.socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to open receive socket-\n   ");
      return (FALSE);
      }
#ifdef DEBUG
    else
      cerr << "DEBUG:\tDIS_bridge receive socket opened." << endl;
#endif

   /* Resize the socket receive buffer to be large enough not to overflow */
   if ((setsockopt(read_info.socket,SOL_SOCKET,SO_RCVBUF,
                   (char *)&rcvbuff, sizeof(rcvbuff))) < 0 )
      {
      perror("WARNING:\tDIS_bridge unable to resize socket receive buffer -\n\t");
      cerr << "\tAttempted size change to " << rcvbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge receive buffer resized to "
           << rcvbuff << " bytes." << endl;
#endif

   /* Bind port numbers to sockets */
   read_info.socket_info.sin_family = AF_INET;
   if ( mode == IP_BROADCAST )
      {
      read_info.socket_info.sin_addr.s_addr = read_info.dest.sin_addr.s_addr;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge read socket sin_addr set to "
           << inet_ntoa(read_info.socket_info.sin_addr) << endl;
#endif
      }
   else if ( mode == IP_MULTICAST )
      {
      read_info.socket_info.sin_addr.s_addr = INADDR_ANY;
      }
   else if ( mode == IP_UNICAST )
      {
      read_info.socket_info.sin_addr.s_addr = read_info.internet_addr.s_addr;
      }

   read_info.socket_info.sin_port = htons(read_info.port);
#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge read socket port set to "
        << int(read_info.port) << endl;
#endif
   if ( mode == IP_MULTICAST )
      {
      mreq.imr_multiaddr.s_addr = inet_addr((char *)read_info.mc_group);
      mreq.imr_interface.s_addr = read_info.internet_addr.s_addr;
      if (setsockopt(read_info.socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,
                     &mreq,sizeof(mreq)) < 0)
         {
         perror("ERROR:\tDIS_bridge unable to join multicast group -\n   ");
         return (FALSE);
         }
      read_info.internet_addr.s_addr = inet_addr((char *)read_info.mc_group);
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge joined multicast group = "
           << inet_ntoa(mreq.imr_multiaddr) << endl;
      cerr << "\t on interface = " << inet_ntoa(mreq.imr_interface) 
           << endl;
#endif
      }

   if ( setsockopt(read_info.socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to reuse read port address -\n   ");
      return(FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge specified reuse address on read socket." << endl;
#endif

   if (bind(read_info.socket, (struct sockaddr *) &read_info.socket_info, 
            sizeof(read_info.socket_info)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to bind receive socket port -\n   ");
      return (FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge bind successful on read socket." << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\topen_read_socket finished." << endl;
#endif

   return (TRUE);

   }


/* open_write_socket         ***********************************************/

int
DIS_bridge::open_write_socket ( network_mode mode )
   {
   int on = 1;
   u_char off = 0;
   int sendbuff = SEND_BUFFER_SIZE;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge open_write_socket started." << endl;
#endif

   /* Open send socket */
   if ((write_info.socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to open send socket -\n   ");
      return (FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge send socket opened." << endl;
#endif
 
   /* Resize the socket send buffer to be large enought not to overflow */
   if ((setsockopt(write_info.socket,SOL_SOCKET,SO_SNDBUF,
                   (char *)&sendbuff, sizeof(sendbuff))) < 0 )
      {
      perror("WARNING:\tDIS_bridge unable to resize socket send buffer -\n\t");
      cerr << "\tAttempted size change to " << sendbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge send buffer resized to "
           << sendbuff << " bytes." << endl;
#endif
 
   if ( mode == IP_BROADCAST )
      {
      /* Mark send socket to allow broadcasting */
      if (setsockopt(write_info.socket, SOL_SOCKET, SO_BROADCAST, 
                     &on, sizeof(on)) < 0)
         {
         perror("ERROR:\tDIS_bridge unable to mark send socket for BCAST -\n   ");
         return (FALSE);
         }
#ifdef DEBUG
      else
         cerr << "DEBUG:\tDIS_bridge send socket configured for Broadcast."
              << endl;
#endif
      }
   else if ( mode == IP_MULTICAST )
      {
      /* Configure send socket time-to-live for multicast */
      if (setsockopt(write_info.socket,IPPROTO_IP,IP_MULTICAST_TTL,
                     (const void *)&write_info.mc_ttl,
                     sizeof(write_info.mc_ttl)) < 0)
         {
         perror("ERROR:\tDIS_bridge unable to mark send socket for MCAST -\n   ");
         return(FALSE);
         }
#ifdef DEBUG
      else
         cerr << "DEBUG:\tDIS_bridge send socket multicast TTl set to "
              << int(write_info.mc_ttl) << endl;
#endif

      /* Configure local packet loopback for multicast */
      if (setsockopt(write_info.socket,IPPROTO_IP,IP_MULTICAST_LOOP,
                     &off,sizeof(off)) < 0)
         {
         perror("ERROR:\tDIS_bridge unable to configure loopback -\n   ");
         return(FALSE);
         }
#ifdef DEBUG
      else
         cerr << "DEBUG:\tDIS_bridge send socket multicast loopback disabled."
              << endl;
#endif
      }

   /* Bind port numbers to sockets */
   write_info.socket_info.sin_family = AF_INET;
   if ( mode == IP_BROADCAST )
      {
      write_info.socket_info.sin_addr.s_addr = write_info.internet_addr.s_addr;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge send socket sin_addr set to "
           << inet_ntoa(write_info.socket_info.sin_addr) << endl;
#endif
      }
   else
      {
      if(setsockopt(write_info.socket,IPPROTO_IP,IP_MULTICAST_IF,
                    (const void *)&(write_info.internet_addr),
                    sizeof(IN_ADDR_SIZE)) < 0 )
         {
         perror("ERROR:\tDIS_bridge unable to assign multicast interface -\n   ");
         return (FALSE);
         }
#ifdef DEBUG
      else
         cerr << "DEBUG:\tDIS_bridge send socket multicast using interface = "
              << inet_ntoa(write_info.internet_addr) << endl;
#endif
      write_info.socket_info.sin_addr.s_addr = 
         inet_addr((char *)write_info.mc_group);
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge send socket sin_addr set to multicast "
           << "group of " << inet_ntoa(write_info.socket_info.sin_addr) << endl;
#endif
      } 

/* COMMENTED OUT FOR THE MOMENT
   if ( setsockopt(write_info.socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
      {
      perror("ERROR:\tDIS_bridge unable to reuse write port address -\n   ");
      return(FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_bridge specified reuse address on write socket."
           << endl;
#endif
 
*/

   write_info.socket_info.sin_port = htons(0);
   if ( (mode == IP_BROADCAST) || (mode == IP_UNICAST) )
      {
      if (bind(write_info.socket, (struct sockaddr *) &write_info.socket_info, 
               sizeof(write_info.socket_info)) < 0)
         {
         perror("ERROR:\tDIS_bridge unable to bind send socket port-\n   ");
         return (FALSE);
         }
#ifdef DEBUG
      else
         cerr << "DEBUG:\tDIS_bridge bind successful on write socket." << endl;
#endif
      }

   write_info.socket_info.sin_port = htons(write_info.port);
#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge write socket sin_port set to "
        << int(ntohs(write_info.socket_info.sin_port))
        << " for destination." << endl;
#endif

   if ( (mode == IP_BROADCAST) || (mode == IP_UNICAST) )
      {
      bcopy((char *) &(write_info.dest.sin_addr), 
            (char *) &(write_info.socket_info.sin_addr),
            IN_ADDR_SIZE);
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge write socket sin_addr set to "
           << inet_ntoa(write_info.socket_info.sin_addr) 
           << " for destination." << endl;
#endif 
      }

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge open_write_socket finished." << endl;
#endif

   return (TRUE);

   }


/* constructor - DIS_bridge  ***********************************************/

DIS_bridge::DIS_bridge ( const char *write_net_interface,
                         const network_mode write_network_mode,
                         const char *read_net_interface,
                         const network_mode read_network_mode )
   {
   u_char all_ones = 0xFF;
   PDUType pdutype;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge constructor started." << endl;
#endif

   init_member_vars();

   /* Determine the number of possible PDU types from the size of the */
   /* PDUType type definition.                                        */
   bcopy ( (char *)&all_ones, (char *)&pdutype, sizeof(pdutype) );
   num_pdu_types = int(pdutype) + 1;

   /* Allocate pdu-to-receive list -- also initializes all values to FALSE. */
   receive_list = (volatile u_char *)new u_char[num_pdu_types];
   if ( receive_list == NULL )
      {
      cerr << "ERROR:\tDIS_bridge unable to allocate memory for "
           << "pdu-to-receive list." << endl;
      cerr << "\tAll PDUS will be passed through." << endl;
      }
   else
      bzero ( (char *)receive_list, sizeof(u_char) );
   receive_all = TRUE;

   /* Initialize coordinate conversion flag to FALSE */
   G_coord_error = 0;

   if ( write_net_interface != NULL )
      strncpy ( (char *)write_info.interface, write_net_interface, 
                NET_NAMES_SIZE );
   else
      strcpy ( (char *)write_info.interface, "" );

   switch ( write_network_mode )
      {
      case IP_BROADCAST:
         write_info.mode = write_network_mode;
         write_info.port = DEFAULT_BC_PORT;
         write_info.exercise_id = DEFAULT_BC_EXERCISE;
         break;
      case IP_MULTICAST:
         write_info.mode = write_network_mode;
         write_info.port = DEFAULT_MC_PORT;
         strncpy ( (char *)write_info.mc_group, DEFAULT_MC_GROUP, 
                   NET_NAMES_SIZE-1 );
         write_info.mc_ttl = DEFAULT_MC_TTL;
         write_info.exercise_id = DEFAULT_MC_EXERCISE;
         break;
      case IP_UNICAST:
         write_info.mode = write_network_mode;
         write_info.port = DEFAULT_UC_PORT;
         write_info.exercise_id = DEFAULT_UC_EXERCISE;
         write_info.dest.sin_addr.s_addr = inet_addr(DEFAULT_UC_INET_ADDRESS);
         break;
      default:
         cerr << "ERROR:\tDIS_bridge unknown write network mode." << endl;
         cerr << "\tDefaulting to IP Broadcast for writing." << endl;
         write_info.mode = IP_BROADCAST;
         write_info.port = DEFAULT_BC_PORT;
         write_info.exercise_id = DEFAULT_BC_EXERCISE;
         break;
      }

   if ( !get_interface ( write_network_mode, NETWRITE ) )
      {
      init_ok = FALSE;
      return;
      }

   if ( read_net_interface != NULL )
      strncpy ( (char *)read_info.interface, read_net_interface, 
                NET_NAMES_SIZE-1 );
   else
      strcpy ( (char *)read_info.interface, "" );
   switch ( read_network_mode )
      {
      case IP_BROADCAST:
         read_info.mode = read_network_mode;
         read_info.port = DEFAULT_BC_PORT;
         read_info.exercise_id = DEFAULT_BC_EXERCISE;
         break;
      case IP_MULTICAST:
         read_info.mode = read_network_mode;
         read_info.port = DEFAULT_MC_PORT;
         strncpy ( (char *)write_info.mc_group, DEFAULT_MC_GROUP, 
                   NET_NAMES_SIZE-1 );
         read_info.mc_ttl = DEFAULT_MC_TTL;
         read_info.exercise_id = DEFAULT_MC_EXERCISE;
         break;
      case IP_UNICAST:
         read_info.mode = read_network_mode;
         read_info.port = DEFAULT_UC_PORT;
         read_info.exercise_id = DEFAULT_UC_EXERCISE;
         read_info.dest.sin_addr.s_addr = inet_addr(DEFAULT_UC_INET_ADDRESS);
         break;
      default:
         cerr << "ERROR:\tDIS_bridge unknown read network mode." << endl;
         cerr << "\tDefaulting to IP Broadcast for reading." << endl;
         read_info.mode = IP_BROADCAST;
         read_info.port = DEFAULT_BC_PORT;
         read_info.exercise_id = DEFAULT_BC_EXERCISE;
         break;
      }

   if ( !get_interface ( read_network_mode, NETREAD ) )
      {
      init_ok = FALSE;
      return;
      }

#ifdef TRACE
      cerr << "TRACE:\tDIS_bridge constructor finished." << endl;
#endif

   }


/* net_close                 ***********************************************/

void
DIS_bridge::net_close()
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge close started." << endl;
#endif

   if ( up_and_running && (bridge_pid != -1) )
      {
      net_read_flag = FALSE;
      kill ( bridge_pid, SIGTERM );
      while ( (wait(NULL)) != bridge_pid );
      up_and_running = FALSE;
      bridge_pid= -1;
      }
 
   if ( receive_list != NULL )
      {
      delete ( receive_list );
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge pdus-to-receive list deallocated." << endl;
#endif
      receive_list = NULL;
      }

   if ( write_info.socket != -1 )
      {
      close(write_info.socket);
      write_info.socket = -1;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge write socket closed." << endl;
#endif
      }

   if ( read_info.socket != -1 )
      {
      close(read_info.socket);
      read_info.socket = -1;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge read socket closed." << endl;
#endif
      }

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge close finished." << endl;
#endif

   }


/* Destructor - DIS_bridge   ***********************************************/

DIS_bridge::~DIS_bridge()
   {
#ifdef TRACE
   cerr << "TRACE:	DIS_bridge destructor started." << endl;
#endif

   this->net_close();

#ifdef TRACE
   cerr << "TRACE:	DIS_bridge destructor finished." << endl;
#endif

   }


/* net_open                  ***********************************************/

int
DIS_bridge::net_open ()
   {

   if ( !up_and_running )
      {
      if ( init_ok )
         {
         up_and_running = open_write_socket ( write_info.mode );
         if ( up_and_running )
            up_and_running = open_read_socket ( read_info.mode );
         else
            cerr << "ERROR:\tDIS_bridge could not open write socket." << endl;
         if ( up_and_running )
            {
            bridge_pid = sproc ( bridge_process, PR_SALL, (void *)this );
            if ( bridge_pid < 0 )
               {
               perror("ERROR:\tDIS_bridge unable to sproc bridge process -\n   ");
               up_and_running = FALSE;
               }
            else
               {
#ifdef DEBUG
               cerr << "DEBUG:\tDIS_bridge bridge process id = "
                    << bridge_pid << endl;
#endif
               }
            }
         else
            cerr << "ERROR:\tDIS_bridge could not open read socket." << endl;
         return up_and_running;
         }
      else
         {
         cerr << "ERROR:\tDIS_bridge could not start bridge.  Init failed!"
              << endl;
         return FALSE;
         }
      }
   else
      {
      cerr << "WARNING:\tDIS_bridge net_open called on already open connection."
           << endl;
      return FALSE;
      }
   }


/* write_pdu                 ***********************************************/

int
DIS_bridge::write_pdu (char *pdu, PDUType type)
{
   int                          length, bytes_sent;
   PDUHeader                    *header;
   int				sendcount;

   /* Fill in header data */
   header = (PDUHeader *) pdu;
   length = int(header->length);
   if ( write_info.exercise_id != DIS_EXERCISE_ALL )
      header->exercise_ident = write_info.exercise_id;
   /* calculate length */
   switch (type) {
      case (EntityStatePDU_Type):
         switch ( write_info.round_conv_direction )
            {
            case FLAT_TO_ROUND:
               convert_entity_state_to_round ( (EntityStatePDU *)pdu );
               break;
            case ROUND_TO_FLAT:
               convert_entity_state_from_round ( (EntityStatePDU *)pdu );
               break;
            case NO_CONVERSION:
            default:
               break;
            }
         break;
      case (FirePDU_Type):
         switch ( write_info.round_conv_direction )
            {
            case FLAT_TO_ROUND:
               convert_fire_to_round ( (FirePDU *)pdu );
               break;
            case ROUND_TO_FLAT:
               convert_fire_from_round ( (FirePDU *)pdu );
               break;
            case NO_CONVERSION:
            default:
               break;
            }
         break;
      case (DetonationPDU_Type):
         switch ( write_info.round_conv_direction )
            {
            case FLAT_TO_ROUND:
               convert_detonation_to_round ( (DetonationPDU *)pdu );
               break;
            case ROUND_TO_FLAT:
               convert_detonation_from_round ( (DetonationPDU *)pdu );
               break;
            case NO_CONVERSION:
            default:
               break;
            }
         break;
      case CollisionPDU_Type:
      case ServiceRequestPDU_Type:
      case ResupplyOfferPDU_Type:
      case ResupplyReceivedPDU_Type:
      case ResupplyCancelPDU_Type:
      case RepairCompletePDU_Type:
      case RepairResponsePDU_Type:
      case CreateEntityPDU_Type:
      case RemoveEntityPDU_Type:
      case StartResumePDU_Type:
      case StopFreezePDU_Type:
      case AcknowledgePDU_Type:
      case ActionRequestPDU_Type:
      case ActionResponsePDU_Type:
      case DataQueryPDU_Type:
      case SetDataPDU_Type:
      case DataPDU_Type:
      case EventReportPDU_Type:
      case MessagePDU_Type:
      case EmissionPDU_Type:
      case LaserPDU_Type:
      case TransmitterPDU_Type:
      case SignalPDU_Type:
      case ReceiverPDU_Type:
         break;
      default:
         cerr << "WARNING:\tDIS_bridge write_pdu non-standard DIS PDU type "
              << int(type) << ", written." << endl;
   }

   if ( write_info.round_conv_direction != NO_CONVERSION )
      {
      assert ( G_coord_error == 0 );
      if ( G_coord_error )
         {
         cerr << "ERROR:\tDIS_bridge coordinate conversion error "
              << "during write_pdu." << endl;
         cerr << "\tPDU of type " << int(type) << " not being sent." << endl; 
         G_coord_error = 0;
         return (FALSE);
         }
      }

   /* Actually send it */
   sendcount = 0;
   while ( (sendcount++ < NUM_SEND_TRIES ) &&
           ((bytes_sent = sendto(write_info.socket, pdu, length, 0,
                                (sockaddr_in *)&write_info.socket_info, 
                                sizeof(write_info.socket_info))) < 0) )
      {
      perror("WARNING:\tDIS_bridge sendto failed - \n   ");
      }
   if ( sendcount > 1 )
      {
      if ( sendcount <= NUM_SEND_TRIES )
         cerr << "WARNING:\tDIS_bridge sent PDU on try # " << (sendcount-1)
              << endl;
      else
         {
         cerr << "ERROR:\tDIS_bridge failed to send PDU after "
              << (sendcount-1) << " tries." << endl;
         return (FALSE);
         }
      }

   return (TRUE);
}


/* pdutype_to_name           ***********************************************/

int
pdutype_to_name ( PDUType pdutype, char *pduname )
   {
   int identified = FALSE;

   switch ( pdutype )
      {
      case OtherPDU_Type:
         strcpy ( pduname, "Other PDU" );
         identified = TRUE;
         break;
      case EntityStatePDU_Type:
         strcpy ( pduname, "Entity State PDU" );
         identified = TRUE;
         break;
      case FirePDU_Type:
         strcpy ( pduname, "Fire PDU" );
         identified = TRUE;
         break;
      case DetonationPDU_Type:
         strcpy ( pduname, "Detonation PDU" );
         identified = TRUE;
         break;
      case CollisionPDU_Type:
         strcpy ( pduname, "Collision PDU" );
         identified = TRUE;
         break;
      case ServiceRequestPDU_Type:
         strcpy ( pduname, "Service Request PDU" );
         identified = TRUE;
         break;
      case ResupplyOfferPDU_Type:
         strcpy ( pduname, "Resupply Offer PDU" );
         identified = TRUE;
         break;
      case ResupplyReceivedPDU_Type:
         strcpy ( pduname, "Resupply Received PDU" );
         identified = TRUE;
         break;
      case ResupplyCancelPDU_Type:
         strcpy ( pduname, "Resupply Cancel PDU" );
         identified = TRUE;
         break;
      case RepairCompletePDU_Type:
         strcpy ( pduname, "Repair Complete PDU" );
         identified = TRUE;
         break;
      case RepairResponsePDU_Type:
         strcpy ( pduname, "Repair Response PDU" );
         identified = TRUE;
         break;
      case CreateEntityPDU_Type:
         strcpy ( pduname, "Create Entity PDU" );
         identified = TRUE;
         break;
      case RemoveEntityPDU_Type:
         strcpy ( pduname, "Remove Entity PDU" );
         identified = TRUE;
         break;
      case StartResumePDU_Type:
         strcpy ( pduname, "Start/Resume PDU" );
         identified = TRUE;
         break;
      case StopFreezePDU_Type:
         strcpy ( pduname, "Stop/Freeze PDU" );
         identified = TRUE;
         break;
      case AcknowledgePDU_Type:
         strcpy ( pduname, "Acknowledge PDU" );
         identified = TRUE;
         break;
      case ActionRequestPDU_Type:
         strcpy ( pduname, "Action Request PDU" );
         identified = TRUE;
         break;
      case ActionResponsePDU_Type:
         strcpy ( pduname, "Action Response PDU" );
         identified = TRUE;
         break;
      case DataQueryPDU_Type:
         strcpy ( pduname, "Data Query PDU" );
         identified = TRUE;
         break;
      case SetDataPDU_Type:
         strcpy ( pduname, "Set Data PDU" );
         identified = TRUE;
         break;
      case DataPDU_Type:
         strcpy ( pduname, "Data PDU" );
         identified = TRUE;
         break;
      case EventReportPDU_Type:
         strcpy ( pduname, "Event Report PDU" );
         identified = TRUE;
         break;
      case MessagePDU_Type:
         strcpy ( pduname, "Message PDU" );
         identified = TRUE;
         break;
      case EmissionPDU_Type:
         strcpy ( pduname, "Electromagnetic Emission PDU" );
         identified = TRUE;
         break;
      case LaserPDU_Type:
         strcpy ( pduname, "Designator PDU" );
         identified = TRUE;
         break;
      case TransmitterPDU_Type:
         strcpy ( pduname, "Transmitter PDU" );
         identified = TRUE;
         break;
      case SignalPDU_Type:
         strcpy ( pduname, "Signal PDU" );
         identified = TRUE;
         break;
      case ReceiverPDU_Type:
         strcpy ( pduname, "Receiver PDU" );
         identified = TRUE;
         break;
      default:
         strcpy ( pduname, "ERROR:  UNKNOWN PDU TYPE" );
         identified = FALSE;
         break;
      }
   return identified;
   }


/* define_read_socket        ***********************************************/

void
DIS_bridge::define_read_socket ( const unsigned short port,
                                 const char *address )
   {
   unsigned long net_address = 0;
   struct hostent *hp;

   /* Unicast socket info */
#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket started." << endl;
#endif

   read_info.port = port;

   bzero ( (char *)&read_info.dest.sin_addr, sizeof(net_address) );

   if ( (net_address = inet_addr(address)) != INADDR_NONE )
      bcopy ( (char *)&net_address, (char *)&read_info.dest.sin_addr,
              sizeof(net_address) );
   else if ( (hp = gethostbyname(address)) == NULL )
      {
      cerr << "ERROR:\tDIS_bridge unicast address is not an internet address"
           << endl;
      cerr << "\tor a known host name.  Proceeding to send to 0.0.0.0." << endl;
      }
   else
      {
      bcopy ( (char *)hp->h_addr, (char *)&read_info.dest.sin_addr,
              sizeof(hp->h_length) );
      cerr << "\tUnicast host " << address << " was identified as "
           << inet_ntoa(read_info.dest.sin_addr) << endl;
      }

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge read port set to " << int(port) << endl;
   cerr << "DEBUG:\tDIS_bridge source address set to "
        << inet_ntoa(read_info.dest.sin_addr) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket finished." << endl;
#endif

   }


/* define_write_socket       ***********************************************/

void
DIS_bridge::define_write_socket ( const unsigned short port,
                                  const char *address )
   {
   unsigned long net_address = 0;
   struct hostent *hp;

   /* Unicast socket info */
#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket started." << endl;
#endif

   write_info.port = port;

   bzero ( (char *)&write_info.dest.sin_addr, sizeof(net_address) );

   if ( (net_address = inet_addr(address)) != INADDR_NONE )
      bcopy ( (char *)&net_address, (char *)&write_info.dest.sin_addr,
              sizeof(net_address) );
   else if ( (hp = gethostbyname(address)) == NULL )
      {
      cerr << "ERROR:\tDIS_bridge unicast address is not an internet address"
           << endl;
      cerr << "\tor a known host name.  Proceeding to send to 0.0.0.0." << endl;
      }
   else
      {
      bcopy ( (char *)hp->h_addr, (char *)&write_info.dest.sin_addr,
              sizeof(hp->h_length) );
      cerr << "\tUnicast host " << address << " was identified as "
           << inet_ntoa(write_info.dest.sin_addr) << endl;
      }

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge write port set to " << int(port) << endl;
   cerr << "DEBUG:\tDIS_bridge destination address set to "
        << inet_ntoa(write_info.dest.sin_addr) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket finished." << endl;
#endif

   }

/* define_read_socket        ***********************************************/

void
DIS_bridge::define_read_socket ( const unsigned short port )
   {
   /* Broadcast socket info */
#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket started." << endl;
#endif

   read_info.port = port;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge read port set to " << int(port) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket finished." << endl;
#endif

   }


/* define_write_socket       ***********************************************/

void
DIS_bridge::define_write_socket ( const unsigned short port )
   {
   /* Broadcast socket info */
#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket started." << endl;
#endif

   write_info.port = port;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge write port set to " << int(port) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket finished." << endl;
#endif

   }


/* define_read_socket        ***********************************************/

void 
DIS_bridge::define_read_socket ( const unsigned short port,
                                 const char *group,
                                 const unsigned char ttl )
   {
   /* Multicast socket info */

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket started." << endl;
#endif

   read_info.port = port;
   if ( group != NULL )
      strncpy ( (char *)read_info.mc_group, group, NET_NAMES_SIZE-1 );
   else
      {
      cerr << "ERROR:\tDIS_bridge group parameter to define_read_socket is NULL."
           << endl;
      cerr << "\tMulticast group keeps current value:"
           << (char *)read_info.mc_group << endl;
      }
   read_info.mc_ttl = ttl;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge read port set to " << int(port) << endl;
   cerr << "DEBUG:\tDIS_bridge read group set to " << group << endl;
   cerr << "DEBUG:\tDIS_bridge read ttl set to " << int(ttl) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_socket finished." << endl;
#endif

   }


/* define_write_socket       ***********************************************/

void 
DIS_bridge::define_write_socket ( const unsigned short port,
                                  const char *group,
                                  const unsigned char ttl )
   {
   /* Multicast socket info */

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket started." << endl;
#endif

   write_info.port = port;
   if ( group != NULL )
      strncpy ( (char *)write_info.mc_group, group, NET_NAMES_SIZE-1 );
   else
      {
      cerr << "ERROR:\tDIS_bridge group parameter to define_write_socket is NULL."
           << endl;
      cerr << "\tMulticast group keeps current value:"
           << (char *)write_info.mc_group << endl;
      }
   write_info.mc_ttl = ttl;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge write port set to " << int(port) << endl;
   cerr << "DEBUG:\tDIS_bridge write group set to " << group << endl;
   cerr << "DEBUG:\tDIS_bridge write ttl set to " << int(ttl) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_socket finished." << endl;
#endif

   }


/* define_read_exercise      ***********************************************/

void
DIS_bridge::define_read_exercise ( unsigned char exercise )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_exercise started." << endl;
#endif

   read_info.exercise_id = exercise;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge read exercise set to " 
        << int(exercise) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_exercise finished." << endl;
#endif

   }


/* define_write_exercise     ***********************************************/

void
DIS_bridge::define_write_exercise ( unsigned char exercise )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_exercise started." << endl;
#endif

   write_info.exercise_id = exercise;

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge write exercise set to " 
        << int(exercise) << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_exercise finished." << endl;
#endif

   }


int
DIS_bridge::define_round_world ( const roundWorldStruct *round_world_file )
   {

#ifdef TRACE
   cerr << "TRACE:DIS_bridge define_round_world started." << endl;
#endif

   if ( round_world_file == NULL )
      {
      cerr << "ERROR:\tDIS_bridge define_read_round_world called with NULL"
           << endl;
      cerr << "\tpointer for round_world_file." << endl;
      cerr << "\tPrevious conversion state remains." << endl;
      round_defined = FALSE;
      }
   else
      {
      if ((init_round_world((roundWorldStruct *)round_world_file)) == FALSE )
         {
         cerr << "ERROR:\tDIS_bridge unable to define round world"
              << " conversion." << endl;
//         cerr << "\tTried to use data from file:  "
//              << round_world_file << endl;
         round_defined = FALSE;
         }
      else
         {
#ifdef DEBUG
         cerr << "DEBUG:\tDIS_bridge defined round world conversion "
              << "from file:\n\t" << round_world_file << endl;
#endif
         round_defined = TRUE;
         }
      }

#ifdef TRACE
   cerr << "TRACE:DIS_bridge define_round_world finished." << endl;
#endif

   return round_defined;
   }


/* define_read_coord_conversion ********************************************/

int
DIS_bridge::define_read_coord_conversion ( const convert_direction direction )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_read_coord_conversion started." << endl;
#endif

   if ( round_defined )
      {
      read_info.round_conv_direction = direction;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge round world read state is now:" << endl;
      switch ( direction )
         {
         case NO_CONVERSION:
            cerr << "\tNo conversion on read." << endl;
            break;
         case FLAT_TO_ROUND:
            cerr << "\tConverting incoming flat to round." << endl;
            break;
         case ROUND_TO_FLAT:
            cerr << "\tConverting incoming round to flat." << endl;
            break;
         default:
            cerr << "\tERROR STATE - CONTACT SOFTWARE SUPPORT!" << endl;
            break;
         }
#endif
      }
   else
      {
      cerr << "ERROR:\tDIS_bridge define_read_coord_conversion called before"
           << endl;
      cerr << "\tround world conversion file defined." << endl;
      }

#ifdef TRACE
      cerr << "TRACE:\tDIS_bridge define_read_coord_conversion finished." << endl;
#endif

   return round_defined;

   }


/* define_write_round_world  ***********************************************/

int
DIS_bridge::define_write_coord_conversion ( const convert_direction direction )
                                           
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge define_write_coord_conversion started." << endl;
#endif

   if ( round_defined )
      {
      write_info.round_conv_direction = direction;
#ifdef DEBUG
      cerr << "DEBUG:\tDIS_bridge round world write state is now:" << endl;
      switch ( direction )
         {
         case NO_CONVERSION:
            cerr << "\tNo conversion on read." << endl;
            break;
         case FLAT_TO_ROUND:
            cerr << "\tConverting incoming flat to round." << endl;
            break;
         case ROUND_TO_FLAT:
            cerr << "\tConverting incoming round to flat." << endl;
            break;
         default:
            cerr << "\tERROR STATE - CONTACT SOFTWARE SUPPORT!" << endl;
            break;
         }
#endif
      }
   else
      {
      cerr << "ERROR:\tDIS_bridge define_write_coord_conversion called before"
           << endl;
      cerr << "\tround world conversion file defined." << endl;
      }

#ifdef TRACE
      cerr << "TRACE:\tDIS_bridge define_write_coord_conversion finished." << endl;
#endif

   return round_defined;

   }


/* swap_dbl                  ***********************************************/

inline void
swap_dbl ( volatile double &op1, volatile double &op2 )
   {
   double temp;
   temp = op1;
   op1 = op2;
   op2 = temp;
   }


/* restrict_bounds           ***********************************************/

int
DIS_bridge::restrict_bounds ( double minx, double miny, double minz,
                              double maxx, double maxy, double maxz,
                              convert_direction mode )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge restrict_bounds started." << endl;
#endif

   if ( (mode != NO_CONVERSION) &&
        (!round_defined) ) 
      {
      cerr << "ERROR:\tDIS_bridge restrict_bounds() cannot convert bounds:"
           << endl;
      cerr << "\tdefine_round_world() must be called first in order to define ";
      cerr << "\tthe conversion file." << endl;
      return FALSE;
      }

   if ( mode == FLAT_TO_ROUND )
      {
      convert_location_to_round ( &minx, &miny, &minz );
      convert_location_to_round ( &maxx, &maxy, &maxz );
      if ( G_coord_error )
         {
         cerr << "ERROR:\tDIS_bridge restrict_bounds() could not convert "
              << "bounds to round world." << endl;
         G_coord_error = FALSE;
         return FALSE;
         }
      else
         bounds_mode = ROUND_WORLD;
      }
   else if ( mode == ROUND_TO_FLAT )
      {
      convert_location_from_round ( &minx, &miny, &minz );
      convert_location_from_round ( &maxx, &maxy, &maxz );
      if ( G_coord_error )
         {
         cerr << "ERROR:\tDIS_bridge restrict_bounds() could not convert "
              << "bounds from round world." << endl;
         G_coord_error = FALSE;
         return FALSE;
         }
      else
         bounds_mode = FLAT_WORLD;
      }
   else
      bounds_mode = DEFAULT;

   bounds_check = TRUE;
   bounds_mode = DEFAULT;
   min_x = minx;
   min_y = miny;
   min_z = minz;
   max_x = maxx;
   max_y = maxy;
   max_z = maxz;
   if ( min_x > max_x )
      swap_dbl ( min_x, max_x );
   if ( min_y > max_y )
      swap_dbl ( min_y, max_y );
   if ( min_z > max_z )
      swap_dbl ( min_z, max_z );

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge bounds restriction enabled..." << endl;
   cerr << "\tMin (x,y,z) = " << min_x << ", " << min_y << ", " << min_z << endl;
   cerr << "\tMax (x,y,z) = " << max_x << ", " << max_y << ", " << max_z << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge restrict_bounds finished." << endl;
#endif

  return TRUE;

   }


/* bound_around_entity       ***********************************************/

void
DIS_bridge::bound_around_entity ( EntityID entity_info )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge bound_around_entity started." << endl;
#endif

   bound_based_on_entity = TRUE;
   bcopy ( (char *)&entity_info, (char *)&bounding_entity, sizeof(EntityID) );

#ifdef DEBUG
   cerr << "DEBUG:\tDIS_bridge will bound around entity: "
        << int(bounding_entity.address.site) << ", "
        << int(bounding_entity.address.host) << ", "
        << int(bounding_entity.entity) << "." << endl;
#endif

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge bound_around_entity finished." << endl;
#endif

   }


/* pass_all_pdus             ***********************************************/

int
DIS_bridge::pass_all_pdus ()
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_all_pdus started." << endl;
#endif

   unsigned char one = 1;
   receive_all = TRUE;
   if ( receive_list != NULL )
      bcopy ( (char *)&one, (char *)receive_list, sizeof(receive_list) );
   else
      return FALSE;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_all_pdus finished." << endl;
#endif

   return TRUE;

   }


/* pass_no_pdus              ***********************************************/

int
DIS_bridge::pass_no_pdus ()
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_no_pdus started." << endl;
#endif

   receive_all = FALSE;
   if ( receive_list != NULL )
      bzero ( (char *)receive_list, sizeof(receive_list) );
   else
      return FALSE;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_no_pdus finished." << endl;
#endif

   return TRUE;

   }


/* pass_pdu                  ***********************************************/

int
DIS_bridge::pass_pdu ( PDUType pdutype )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_pdu started." << endl;
#endif

#ifdef DEBUG
   char pdu_name[255];
   pdutype_to_name ( pdutype, pdu_name );
   cerr << "DEBUG:\tDIS_bridge asked to pass all " 
        << pdu_name << endl;
#endif

   receive_all = FALSE;
   if ( receive_list != NULL )
      receive_list[pdutype] = TRUE;
   else 
      return FALSE;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge pass_pdu finished." << endl;
#endif

   return TRUE;
   }


/* no_pass_pdu               ***********************************************/

int
DIS_bridge::no_pass_pdu ( PDUType pdutype )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge no_pass_pdu started." << endl;
#endif

#ifdef DEBUG
   char pdu_name[255];
   pdutype_to_name ( pdutype, pdu_name );
   cerr << "DEBUG:\tDIS_bridge asked NOT to pass any " 
        << pdu_name << endl;
#endif

   receive_all = FALSE;
   if ( receive_list != NULL )
      receive_list[pdutype] = FALSE;
   else
      return FALSE;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge no_pass_pdu finished." << endl;
#endif

   return TRUE;
   }


/* print_feedback            ***********************************************/

void
DIS_bridge::print_feedback ( int true_or_false )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge print_feedback started." << endl;
#endif

   feedback = true_or_false;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge print_feedback finished." << endl;
#endif

   }


/* verify_pdu_size           ***********************************************/

void
DIS_bridge::verify_pdu_size ( int true_or_false )
   {

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge verify_pdu_size started." << endl;
#endif

   verify_size = true_or_false;

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge verify_pdu_size finished." << endl;
#endif

   }


/* signal_handler            ***********************************************/

void
signal_handler(int sig, ...)
   {
   signal ( sig, SIG_IGN );
   switch ( sig )
      {
      case SIGTERM:
         break;
      case SIGHUP:
         if ( getppid() == 1 )
            {
            cerr << "DEATH NOTICE:" << endl;
            cerr << "\tParent process terminated without calling net_close()."
                 << endl;
            cerr << "\tDIS_bridge terminating to prevent orphan process."
                 << endl;
            exit(0);
            }
         break;
      case SIGINT:
         cerr << "DEATH NOTICE:\tDIS_bridge exiting due to interrupt "
              << "signal." << endl;
         exit(0);
         break;
      case SIGQUIT:
         cerr << "DEATH NOTICE:\tDIS_bridge exiting due to quit "
              << "signal." << endl;
         exit(0);
         break;
      default:
         cerr << "DEATH NOTICE:\tDIS_bridge exiting due to signal:  "
              << sig << endl;
         exit(0);
         break;
      }
   signal ( sig, (void (*)(int))signal_handler );
   }


/* in_bounds                 ***********************************************/

inline int
in_bounds ( EntityLocation location,
            double minx, double miny, double minz,
            double maxx, double maxy, double maxz )
   {
   return ( (minx <= location.x) && (location.x <= maxx) &&
            (miny <= location.y) && (location.y <= maxy) &&
            (minz <= location.z) && (location.z <= maxz) );
   }


/* bridge_process            ***********************************************/

void
bridge_process ( void *tempthis )
   {
   DIS_bridge		*obj;
   struct sockaddr_in   from;
   union PDU            input_buf;
   int			len = sizeof(from);
   int			bytes_read;
   extern int		errno;
   int			good_read;
   PDUHeader            *header = (PDUHeader *) &input_buf;
   EntityStatePDU       *epdu = (EntityStatePDU *) &input_buf;
   DetonationPDU        *dpdu = (DetonationPDU *) &input_buf;
   FirePDU		*fpdu = (FirePDU *) &input_buf;
   struct hostent	*entity;
   int			print_error = FALSE;
   int			correct_size;
   static		void (*temp_signal)(int);
   EntityLocation	entity_location;
   EntityLocation	entity_min_bounds;
   EntityLocation       entity_max_bounds;


#if defined(STATS) || defined(RUNTIME_STATS)
   long			packets_accepted = 0;
   long			packets_bound_rejected = 0;
   long			packets_filter_rejected = 0;
   long			packets_exercise_rejected = 0;
   long			packets_coord_error_rejected = 0;
#endif

   obj = (DIS_bridge *)tempthis;

   entity_location.x = 0.0;
   entity_location.y = 0.0;
   entity_location.z = 0.0;
   entity_min_bounds.x = 0.0;
   entity_min_bounds.y = 0.0;
   entity_min_bounds.z = 0.0;
   entity_max_bounds.x = 0.0;
   entity_max_bounds.y = 0.0;
   entity_max_bounds.z = 0.0;

   prctl(PR_TERMCHILD);

   signal ( SIGTERM, (void (*)(int))signal_handler );
   signal ( SIGHUP, (void (*)(int))signal_handler );
   if ( (temp_signal = signal ( SIGINT, (void (*)(int))signal_handler )) 
         != SIG_DFL )
      {
      signal ( SIGINT, SIG_IGN );
#ifdef DEBUG
      cerr << "*** DIS_bridge process ignoring interupts." << endl;
#endif
      }
   signal ( SIGQUIT, (void (*)(int))signal_handler );

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge bridge_process started." << endl;
#endif


   while ( obj->net_read_flag )
      {

      /* Read packet off network into temp buffer */

      if ((bytes_read = recvfrom(obj->read_info.socket, (void *) &input_buf,
              sizeof(union PDU), 0, (struct sockaddr *) &from, &len)) == -1)
         {
         if ( (errno != EINTR) || obj->net_read_flag )
            {
            perror("ERROR:\tDIS_bridge receiver process, recvfrom failed -\n   ");
            if ( errno == EINTR )
               cerr << "WARNING:\tDIS_bridge interrupted but not "
                    << "quitting because net_read_flag is still true." << endl;
            else
               cerr << "\tERRNO:  " << errno  << ", net_read_flag = "
                    << obj->net_read_flag << endl;
            }
         good_read = FALSE;
         }
      else
         {
         good_read = TRUE;
         }

/*
      if ( getppid() == 1 )
         {
         cerr << "\nDEATH NOTICE:\tDIS_bridge exiting because parent"
              << " process died." << endl;
         obj->net_read_flag = 0;
         good_read = FALSE;
         }
*/

      /* Reject our own broadcasts */
      if ( good_read &&
         (bcmp((void *) &(from.sin_addr), (void *) &(obj->read_info.internet_addr),
                                                       IN_ADDR_SIZE) == 0) )
         {
         continue;      /* don't process packet further */
         }

      if ( good_read && (obj->read_info.mode == IP_UNICAST) &&
           (bcmp((void *) &(from.sin_addr), (void *) &(obj->read_info.dest.sin_addr),
                                                       IN_ADDR_SIZE) != 0) )
         {
#ifdef DEBUG
         cerr << "DEBUG:\tDIS_bridge got a packet in Unicast mode but the packet"
              << endl;
         cerr << "\twas not from " << inet_ntoa(obj->read_info.dest.sin_addr) 
              << endl;
         cerr << "\tinstead was from " << inet_ntoa(from.sin_addr) << endl;
#endif
         continue;
         }

      if ( obj->feedback && good_read )
         cerr << "PDU of type " << int(header->type);

      if ( good_read && !obj->receive_all )
         {
         if ( !obj->receive_list[int(header->type)] )
            {
            if ( obj->feedback )
               cerr << " DISCARDED - filtered type." << endl;
#ifdef DEBUG
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "DEBUG:\tDiscarded PDU was from "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
#endif
#if defined(STATS) || defined(RUNTIME_STATS)
            packets_filter_rejected++;
#endif
            continue;
            }
         }

      if ( obj->verify_size && good_read )
         {
         print_error = FALSE;
         switch ( header->type )
            {
            case (FirePDU_Type):
               if ( int(header->length) != sizeof(FirePDU) )
                  {
                  cerr << "WARNING:\tDIS_bridge specified length of "
                       << "incoming FIRE PDU is non-standard." << endl;
                  cerr << "\tStandard Fire PDU bytes = " 
                       << sizeof(FirePDU) << endl;
                  print_error = TRUE;
                  }
               break;
            case (EntityStatePDU_Type):
               correct_size = sizeof(EntityStatePDU) + 
                              ( sizeof(ArticulatParams) * 
                              (epdu->num_articulat_params - MAX_ARTICULAT_PARAMS));
               if ( int(header->length) != correct_size )
                  {
                  cerr << "WARNING:\tDIS_bridge specified length of "
                          "incoming Entity State PDU is non-standard." << endl;
                  cerr << "\tStandard Entity State PDU bytes with "
                       <<  int(epdu->num_articulat_params)
                       << " articulations is = " << correct_size << endl;
                  print_error = TRUE;
                  }
               break;
            case (DetonationPDU_Type):
               correct_size = sizeof(DetonationPDU) +
                              ( sizeof(ArticulatParams) *
                              (dpdu->num_articulat_params - MAX_ARTICULAT_PARAMS));
               if ( int(header->length) != correct_size )
                  {
                  cerr << "WARNING:\tDIS_bridge specified length of "
                          "incoming Detonation PDU is non-standard." << endl;
                  cerr << "\tStandard Detonation PDU bytes with "
                       <<  int(dpdu->num_articulat_params)
                       << " articulations is = " << correct_size << endl;
                  print_error = TRUE;
                  }
               break;
            default:
               break;
            }
   
         if ( int(header->length) != bytes_read )
               {
               cerr << "WARNING:\tDIS_bridge specified length of "
                    << "incoming PDU is not equal to actual bytes read."
                    << endl;
               print_error = TRUE;
               }
   
         if ( print_error )
            {
            cerr << "\tActual bytes read = " << bytes_read << endl;
            cerr << "\tBytes specified in PDU header = "
                 << int(header->length) << endl;
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "\tPacket came from:  "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
            }
   
         }

      if ( good_read && (header->length > sizeof(union PDU) ) )
         {
         cerr << "ERROR:\tDIS_bridge ignored PDU because "
              << "it was too large." << endl;
         good_read = FALSE;
         entity = gethostbyaddr ( (void *)&from.sin_addr,
                                  sizeof(struct in_addr), AF_INET );
         cerr << "\tPacket came from:  "
              << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
         cerr << "\tPacket type:  " << int(header->type)
              << ", Size:  " << int(header->length) << endl;
         }
  
      if ( good_read )
         {
         if ( (obj->read_info.exercise_id != DIS_EXERCISE_ALL) && 
              (int(header->exercise_ident) != int(obj->read_info.exercise_id)) )
            {
            good_read = FALSE;
            if ( obj->feedback )
               cerr << " DISCARDED - exercise " << int(header->exercise_ident)
                    << " not " << int(obj->read_info.exercise_id) << endl;
#ifdef DEBUG
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "DEBUG:\tDiscarded PDU was from "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
#endif
#if defined(STATS) || defined(RUNTIME_STATS)
            packets_exercise_rejected++;
#endif

            } 
         }

      
      if ( good_read && (obj->read_info.round_conv_direction != NO_CONVERSION) )
         {
         switch ( header->type )
            {
            case (EntityStatePDU_Type):
               switch ( obj->read_info.round_conv_direction )
                  {
                  case FLAT_TO_ROUND:
                     convert_entity_state_to_round 
                        ( (EntityStatePDU *)(&input_buf) );
                     break;
                  case ROUND_TO_FLAT:
                     convert_entity_state_from_round 
                        ( (EntityStatePDU *)(&input_buf) );
                     break;
                  case NO_CONVERSION:
                  default:
                     break;
                  }
               break;
            case (FirePDU_Type):
               switch ( obj->read_info.round_conv_direction )
                  {
                  case FLAT_TO_ROUND:
                     convert_fire_to_round ( (FirePDU *)(&input_buf) );
                     break;
                  case ROUND_TO_FLAT:
                     convert_fire_from_round ( (FirePDU *)(&input_buf) );
                     break;
                  case NO_CONVERSION:
                  default:
                     break;
                  }
               break;
            case (DetonationPDU_Type):
               switch ( obj->read_info.round_conv_direction )
                  {
                  case FLAT_TO_ROUND:
                     convert_detonation_to_round 
                        ( (DetonationPDU *)(&input_buf) );
                     break;
                  case ROUND_TO_FLAT:
                     convert_detonation_from_round 
                        ( (DetonationPDU *)(&input_buf) );
                     break;
                  case NO_CONVERSION:
                  default:
                     break;
                  }
               break;
            default:
               break;
            }
         if ( G_coord_error )
            {
            good_read = FALSE;
            G_coord_error = 0;
#if defined(STATS) || defined(RUNTIME_STATS)
            packets_coord_error_rejected++;
#endif
            cerr << "ERROR:\tDIS_bridge ignored PDU due to "
                 << "a conversion error." << endl;
            entity = gethostbyaddr ( (void *)&from.sin_addr, 
                                     sizeof(struct in_addr), AF_INET ); 
            cerr << "\tPacket came from:  "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
            cerr << "\tPacket type:  " << int(header->type)
                 << ", Size:  " << int(header->length) << endl;
            switch ( header->type )
               {
               case (EntityStatePDU_Type):
                  cerr << "\tSite: " << int(epdu->entity_id.address.site)
                       << ", Host: " << int(epdu->entity_id.address.host)
                       << ", Entity: " << int(epdu->entity_id.entity) << endl; 
                  break;
               default:
                  break;
               }
            }
         }


      if ( good_read )
         {

         if ((obj->bound_based_on_entity) &&
             (header->type == EntityStatePDU_Type) &&
             (bcmp((void *)&obj->bounding_entity, (void *)&epdu->entity_id,
                   sizeof(EntityID)) == 0))
            {
            if ( obj->feedback )
               cerr << " is from the bounding entity and is ";

            bcopy ( (char *)&epdu->entity_location, (char *)&entity_location,
                    sizeof(EntityLocation) );
            entity_min_bounds.x = obj->min_x + entity_location.x;
            entity_min_bounds.y = obj->min_y + entity_location.y;
            entity_min_bounds.z = obj->min_z + entity_location.z;
            entity_max_bounds.x = obj->max_x + entity_location.x;
            entity_max_bounds.y = obj->max_y + entity_location.y;
            entity_max_bounds.z = obj->max_z + entity_location.z;

#ifdef BOUNDS_DEBUG
            cerr << endl;
            cerr << "BOUNDS_DEBUG:\tDIS_bridge Bounding box around entity is:" 
                 << endl;
            cerr << "\tMIN: " << entity_min_bounds.x 
                 << ", " << entity_min_bounds.y
                 << ", " << entity_min_bounds.z << endl;
            cerr << "\tMAX: " << entity_max_bounds.x 
                 << ", " << entity_max_bounds.y
                 << ", " << entity_max_bounds.z << endl;
#endif

            }

         /* Perform bounds check */
         if ( obj->bounds_check )
            {
            switch ( header->type )
               {
               case EntityStatePDU_Type:
                  if ( obj->bound_based_on_entity )
                     good_read = in_bounds ( epdu->entity_location,
                                             entity_min_bounds.x,
                                             entity_min_bounds.y,
                                             entity_min_bounds.z,
                                             entity_max_bounds.x,
                                             entity_max_bounds.y,
                                             entity_max_bounds.z );
                  else
                     good_read = in_bounds ( epdu->entity_location,
                                             obj->min_x, obj->min_y, obj->min_z,
                                             obj->max_x, obj->max_y, obj->max_z );
                  break;
               case FirePDU_Type:
                  if ( obj->bound_based_on_entity )
                     good_read = in_bounds ( fpdu->location_in_world,
                                             entity_min_bounds.x,
                                             entity_min_bounds.y,
                                             entity_min_bounds.z,
                                             entity_max_bounds.x,
                                             entity_max_bounds.y,
                                             entity_max_bounds.z );
                  else
                     good_read = in_bounds ( fpdu->location_in_world,
                                             obj->min_x, obj->min_y, obj->min_z,
                                             obj->max_x, obj->max_y, obj->max_z );
                  break;
               case DetonationPDU_Type:
                  if ( obj->bound_based_on_entity )
                     good_read = in_bounds ( dpdu->location_in_world,
                                             entity_min_bounds.x,
                                             entity_min_bounds.y,
                                             entity_min_bounds.z,
                                             entity_max_bounds.x,
                                             entity_max_bounds.y,
                                             entity_max_bounds.z );
                  else
                     good_read = in_bounds ( dpdu->location_in_world,
                                             obj->min_x, obj->min_y, obj->min_z,
                                             obj->max_x, obj->max_y, obj->max_z );
                  break;
               default:
                  break;
               }
            if ( !good_read )
               {
               if (obj->feedback)
                  {
                  cerr << " DISCARDED - out of bounds."
                       << endl;
#ifdef BOUNDS_DEBUG
                  cerr << "\tEntity Location: "
                       << epdu->entity_location.x << ", " 
                       << epdu->entity_location.y << ", " 
                       << epdu->entity_location.z << endl;
#endif
                  }
#ifdef DEBUG
               entity = gethostbyaddr ( (void *)&from.sin_addr,
                                        sizeof(struct in_addr), AF_INET );
               cerr << "DEBUG:\tDiscarded PDU was from "
                    << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
#endif
#if defined(STATS) || defined(RUNTIME_STATS)
               packets_bound_rejected++;
#endif

               }
            }

         if ( good_read )
            {
#if defined(STATS) || defined(RUNTIME_STATS)
            packets_accepted++;
#endif
            if (obj->feedback)
               {
               cerr << " ACCEPTED";
               if (obj->read_info.exercise_id == DIS_EXERCISE_ALL)
                  cerr << ", Exercise ID = " << int(header->exercise_ident) << endl; 
               else
                  cerr << "." << endl;
               }
#ifdef PACKET
            else
               cerr << "PACKET:\tDIS_bridge processed a packet of type "
                    << int(header->type) << endl;
#endif


            obj->write_pdu ( (char *)&input_buf, header->type );
            }

         }

      }

#ifdef TRACE
   cerr << "TRACE:\tDIS_bridge bridge_process finished." << endl;
#endif

#if defined(STATS) || defined(RUNTIME_STATS)
   cerr << "STATS:\tDIS_bridge bridge_process passed "
        << packets_accepted << " packets." << endl;
   cerr << "STATS:\tDIS_bridge bridge_process rejected:" << endl;
   cerr << "\t" << packets_exercise_rejected 
        << " packets due to exercise filtering." << endl;
   cerr << "\t" << packets_bound_rejected
        << " packets due to geographic bounds filtering." << endl;
   cerr << "\t" << packets_filter_rejected
        << " packets due to PDU type filtering." << endl;
   cerr << "\t" << packets_coord_error_rejected
        << " packets due to coordinate conversion errors." << endl;


#endif

   obj->net_read_flag = TRUE;
   exit(0);
   }

