
#ifdef SIG96HACK
#define SIG_Z_SHIFT 43.00
#endif

// File: <disnetlib.cc>

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
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bstring.h>
#include <string.h>
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
#include <sys/prctl.h>          // sproc()
#include <signal.h>             // kill()
#include <assert.h>

#include "disnetlib.h"
#include "disconvert.h"

#define IN_ADDR_SIZE    4
#define SEND_BUFFER_SIZE        65000
#define RECEIVE_BUFFER_SIZE     65000
#define NUM_SEND_TRIES          3
#define MAX_FILTER_ENTRIES 100


/* Local type defintions */
typedef struct 
   {
   struct in_addr	net_inet_addr;
   char			ascii_inet_addr[25];
   char			interface[25];
   struct sockaddr_in	dest;
   } valid_intrf_info;

/* External coordinate conversion error flag */
extern volatile int G_coord_error;

/* Static member initializations */
int	DIS_net_manager::num_net_readers = 0;
int	DIS_net_manager::arena_counter = 0;

/* Member functions */

void
DIS_net_manager::init_member_vars()
   {
   /* num_net_readers is static and is initialized above */
   /* arena_counter is static and is initialized above */
   netarena = NULL;
   strcpy ( arena_name, "" );
   swap_buf_sema = NULL;
   start_reading_sema = NULL;
   has_data = NULL;
   first_obj_read = TRUE;
   strcpy ( interface, "" );
   bzero ( (char *)&net_inet_addr, sizeof(net_inet_addr) );
   strcpy ( ascii_inet_addr, "" );
   network_mode = ERROR_MODE;
   sock_send = -1;
   sock_recv = -1;
   bzero ( (char *)&sin_send, sizeof(sin_send) );
   bzero ( (char *)&sin_recv, sizeof(sin_recv) );
   strcpy ( mc_group, "" );
   mc_ttl = 1;
   loopback = TRUE;
   port = 0;
   producer_pid = -1;
   consumer_pid = -1;
   up_and_running = FALSE;
   net_read_flag = NULL;
   round_world = FALSE;
   round_file = new roundWorldStruct;
   exercise_id = 0;
   buffer = NULL;
   current_net_buffer = NULL;
   max_pdus = -1;
   num_pdu_types = 0;
   receive_list = NULL;
   receive_all = NULL;
   }


int
DIS_net_manager::init_arena ()
   {
   int arena_size;
   u_char all_ones = 0xFF;
   PDUType pdutype;

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager init_arena started." << endl;
#endif

   bcopy ( (char *)&all_ones, (char *)&pdutype, sizeof(pdutype) );
   num_pdu_types = int(pdutype) + 1;
#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager " << num_pdu_types 
        << " possible PDU types from size of field." << endl;
#endif

   /* Compute arena size to hold 2 sets of pdu_buffer data each with the  */
   /*    requested number of union PDUs.  Then add the size of the */
   /*    semaphores, data locks, and flags.  Then add the size of the */
   /*    filter list and then a fudge factor of 20%. */
   arena_size = ( ( (sizeof(union PDU) * max_pdus) + sizeof(pdu_buffer) )  * 2 );
   /* have to use int instead of usema_t for compile */
   arena_size += ((2*sizeof(int)) + (sizeof(ulock_t)) + (4 * sizeof(int)));
   arena_size += num_pdu_types * sizeof(u_char);
   arena_size += sizeof(EntityID) * MAX_FILTER_ENTRIES;
   arena_size = int ( arena_size * 1.2f );

#ifdef DATA
   cerr << "DATA:	DIS_net_manager requesting arena size of "
        << arena_size << endl;
   cerr << "DATA:	DIS_net_manager union PDU size is " 
        << sizeof(union PDU) << endl;
#endif

   /* Set arena size */
   if (usconfig(CONF_INITSIZE, arena_size) == -1)
      {
      perror("ERROR:	DIS_net_manager initializing arena size -\n   ");
      return (FALSE);
      }

   /* Set arena to set the size up front with no autogrow */
   if(usconfig(CONF_AUTOGROW,0) == -1)
      {
      perror("ERROR:	DIS_net_manager setting autogrow to 0 -\n   ");
      return (FALSE);
      }

   /* Set arena to be shared by parent and child only */
   if (usconfig(CONF_ARENATYPE, US_SHAREDONLY) == -1)
      {
      perror("ERROR:	DIS_net_manager configuring arena -\n   ");
      return (FALSE);
      }

   /* Name and open arena */
   arena_counter++;
   sprintf ( arena_name, "/usr/tmp/DIS_arena.%d.%d", getpid(), arena_counter );
#ifdef DATA
   cerr << "DATA:	DIS_net_manager 'arena_counter' = " << arena_counter << endl;
   cerr << "DATA:	DIS_net_manager 'arena_name' = " << arena_name << endl;
#endif
   if ( (netarena = usinit(arena_name)) == NULL )
      {
      arena_counter--;
      perror("ERROR:	DIS_net_manager creating arena -\n   ");
#ifdef DATA
   cerr << "DATA:	DIS_net_manager 'arena_counter' = " << arena_counter << endl;
#endif
      return (FALSE);
      }

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager init_arena finished." << endl;
#endif
 
   return (TRUE);
 
   }


int
DIS_net_manager::allocate_shared_memory()
   {
   int zero = 0;

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager allocate_shared_memory started." << endl;
#endif

   if ( (buffer = (pdu_buffer *)usmalloc(2*sizeof(pdu_buffer), netarena)) ==
        NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate control structures -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager control structure allocated." << endl;
#endif

   buffer[0].pdu = (union PDU *)usmalloc(sizeof(union PDU)*max_pdus, netarena);
   if ( buffer[0].pdu == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate first PDU buffer-\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager first PDU buffer allocated." << endl;
#endif

   buffer[1].pdu = (union PDU *)usmalloc(sizeof(union PDU)*max_pdus, netarena);
   if ( buffer[1].pdu == NULL )
      {
      perror("ERROR:    DIS_net_manager unable to allocate second PDU buffer-\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager second PDU buffer allocated." << endl;
#endif

   buffer[0].current_pdu = 0;
   buffer[1].current_pdu = 0;
   
   buffer[0].over_written_pdus = 0;
   buffer[1].over_written_pdus = 0;

   buffer[0].over_write_mode = FALSE;
   buffer[1].over_write_mode = FALSE;

   buffer[0].unread_pdus = 0;
   buffer[1].unread_pdus = 0;

   /* Init semaphore */
   if ((swap_buf_sema = usnewsema(netarena, 1)) == NULL)
      {
      perror("ERROR:	DIS_net_manager unable to allocate swap semaphore -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager swap semaphore allocated." << endl;
#endif

   if ((start_reading_sema = usnewsema(netarena, 1)) == NULL)
      {
      perror("ERROR:	DIS_net_manager unable to allocate reading semaphore -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager reading semaphore allocated." << endl;
#endif

   if (uspsema(start_reading_sema) == -1)
      {
      perror("ERROR:	DIS_net_manager unable to initially set reading sema -\n   ");
      return (FALSE);
      }

   if ( (has_data = usnewlock ( netarena )) == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to alloate data lock -\n   ");
      return (FALSE);
      }
   
   usinitlock ( has_data );

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager data lock allocated." << endl;
#endif

   net_read_flag = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( net_read_flag == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate read flag -\n   ");
      return (FALSE);
      }
   *net_read_flag = TRUE;

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager read flag allocated." << endl;
#endif

   current_net_buffer = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( current_net_buffer == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate net buffer flag -\n   ");
      return (FALSE);
      }
   *current_net_buffer = 0;

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager net buffer flag allocated." << endl;
#endif

   receive_all = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( receive_all == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate receive_all flag -\n   ");
      return (FALSE);
      }
   *receive_all = 1;

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager receive_all flag allocated." << endl;
#endif

   receive_list = (volatile u_char *)usmalloc(num_pdu_types, netarena);
   if ( receive_list == NULL )
      {
      perror("ERROR:	DIS_net_manager unable to allocate receive list -\n   ");
      return (FALSE);
      }
   bcopy ( (char *)&zero, (char *)receive_list, num_pdu_types );

#ifdef DEBUG
   cerr << "DEBUG:	DIS_net_manager receive_list allocated "
        << num_pdu_types << " bytes." << endl;
#endif

   filter_count = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( filter_count == NULL )
      {
      perror("ERROR:\tDIS_net_manager unable to allocate filter_count -\n   ");
      return (FALSE);
      }
   *filter_count = 0;

#if defined(DEBUG) || defined(FDEBUG)
   cerr << "FDEBUG:\tDIS_net_manager filter_count allocated." << endl;
#endif

   filter_list = (volatile EntityID *)
                 usmalloc((sizeof(EntityID)*MAX_FILTER_ENTRIES), netarena);
   if ( filter_list == NULL )
      {
      perror("ERROR:\tDIS_net_manager unable to allocate filter list -\n   ");
      return (FALSE);
      }
   bzero ( (char *)filter_list, (sizeof(EntityID)*MAX_FILTER_ENTRIES) );

#if defined(DEBUG) || defined(FDEBUG)
   cerr << "FDEBUG:\tDIS_net_manager filter list allocated "
        << (sizeof(EntityID)*MAX_FILTER_ENTRIES) << " bytes." << endl;
#endif

   time_list = (volatile int *)
                 usmalloc((sizeof(int)*MAX_FILTER_ENTRIES), netarena);
   if ( time_list == NULL )
      {
      perror("ERROR:\tDIS_net_manager unable to allocate time list -\n   ");
      return (FALSE);
      }
   bzero ( (char *)time_list, (sizeof(int)*MAX_FILTER_ENTRIES) );

#if defined(DEBUG) || defined(FDEBUG)
   cerr << "FDEBUG:\tDIS_net_manager filter time list allocated "
        << (sizeof(int)*MAX_FILTER_ENTRIES) << " bytes." << endl;
#endif


#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager allocate_shared_memory finished." << endl;
#endif

   return (TRUE);
   }


int
DIS_net_manager::init_sockets()
   {
   int on = 1;
   valid_intrf_info     valid[4];
   int                  i, current, count, found;
   char                 buf[BUFSIZ];
   struct ifconf        ifc;
   struct ifreq         *ifr;
   struct sockaddr_in   *in_addr;
   struct ip_mreq	mreq;
   int			sendbuff = SEND_BUFFER_SIZE;
   int			rcvbuff = RECEIVE_BUFFER_SIZE;

   /* Open send socket */
   if ((sock_send = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:	DIS_net_manager unable to open send socket -\n   ");
      return (FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:	DIS_net_manager send socket opened." << endl;
#endif
  
   /* Resize the socket send buffer to be large enought not to overflow */
   if ((setsockopt(sock_send,SOL_SOCKET,SO_SNDBUF,
                   (char *)&sendbuff, sizeof(sendbuff))) < 0 )
      {
      perror("WARNING:\tDIS_net_manager unable to resize socket send buffer -\n\t");
      cerr << "\tAttempted size change to " << sendbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_net_manager send buffer resized to "
           << sendbuff << " bytes." << endl;
#endif

   /* Open receive socket */
   if ((sock_recv = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:	DIS_net_manager unable to open receive socket-\n   ");
      return (FALSE);
      }
#ifdef DEBUG
    else
      cerr << "DEBUG:	DIS_net_manager receive socket opened." << endl;
#endif

   /* Resize the socket receive buffer to be large enough not to overflow */
   if ((setsockopt(sock_recv,SOL_SOCKET,SO_RCVBUF,
                   (char *)&rcvbuff, sizeof(rcvbuff))) < 0 )
      {
      perror("WARNING:\tDIS_net_manager unable to resize socket receive buffer -\n\t");
      cerr << "\tAttempted size change to " << rcvbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tDIS_net_manager receive buffer resized to "
           << rcvbuff << " bytes." << endl;
#endif


   if ( network_mode == BCAST_MODE )
      {
      /* Mark send socket to allow broadcasting */
      if (setsockopt(sock_send, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
         {
         perror("ERROR:	DIS_net_manager unable to mark send socket for BCAST -\n   ");
         return (FALSE);
         }
      }
   else if ( network_mode == MCAST_MODE )
      {
      /* Configure send socket time-to-live for multicast */
      if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_TTL,
                     &mc_ttl,sizeof(mc_ttl)) < 0)
         {
         perror("ERROR:	DIS_net_manager unable to mark send socket for MCAST -\n   ");
         return(FALSE);
         }

      /* Configure local packet loopback for multicast -- */

      if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_LOOP,
                     &loopback,sizeof(loopback)) < 0)
         {
         perror("ERROR:	DIS_net_manager unable to configure loopback -\n   ");
         return(FALSE);
         }
      }

   /* Get InterFace CONFig */
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock_send, SIOCGIFCONF, (char *) &ifc) < 0)
      {
      perror("ERROR:	DIS_net_manager unable to get interface config -\n   ");
      close(sock_send);
      sock_send = -1;
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
      if (ioctl(sock_send, SIOCGIFFLAGS, (char *) ifr) < 0)
         {
         perror("ERROR:	DIS_net_manager unable to get interface flags -\n   ");
         close(sock_send);
         sock_send = -1;
         return (FALSE);
         }

      /*  Skip boring cases */
      if ( ((ifr->ifr_flags & IFF_UP) == 0) ||            /* interface down? */
           (ifr->ifr_flags & IFF_LOOPBACK)  ||            /* local loopback? */
           ((network_mode == BCAST_MODE)&&((ifr->ifr_flags&IFF_BROADCAST) == 0)) ||
           ((network_mode == MCAST_MODE)&&((ifr->ifr_flags&IFF_MULTICAST) == 0)) )
         {
         continue;
         }

      /* Get and save InterFace ADDRess */
      if (ioctl(sock_send, SIOCGIFADDR, (char *) ifr) == -1)
         {
         perror("ERROR: DIS_net_manager unable to determine inet address-\n   ");
         close(sock_send);
         sock_send = -1;
         return (FALSE);
         }
      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);

      valid[current].net_inet_addr = in_addr->sin_addr;
      strcpy ( valid[current].ascii_inet_addr,
               inet_ntoa(valid[current].net_inet_addr) );
      strcpy ( valid[current].interface, ifr->ifr_name );

      if ( network_mode == BCAST_MODE )
         {
         /* Get InterFace BRoaDcast ADDRess */
         if (ioctl(sock_send, SIOCGIFBRDADDR, (char *) ifr) < 0)
            {
            perror("ERROR:	DIS_net_manager unable to determine BCAST address -\n   ");
            close (sock_send);
            sock_send = -1;
            return (FALSE);
            }
         bcopy((char *) &(ifr->ifr_broadaddr), 
               (char *) &(valid[current].dest),
               sizeof(ifr->ifr_broadaddr));
         } 


#ifdef DATA
         cerr << "DATA:	DIS_net_manager detected ";
         if ( network_mode == BCAST_MODE )
            cerr << "broadcast";
         else if ( network_mode == MCAST_MODE )
            cerr << "multicast";
         cerr << " capable interface = " << valid[current].interface << endl;
         cerr << "DATA:	DIS_net_manager detected ";
         if ( network_mode == BCAST_MODE )
            cerr << "broadcast";
         else if ( network_mode == MCAST_MODE )
            cerr << "multicast";
         cerr << " capable interface inet = " 
              << valid[current].ascii_inet_addr << endl;
#endif

      current++;

      }

   if ( current < 1 )
      {
      close(sock_send);
      sock_send = -1;
      cerr << "ERROR:	DIS_net_manager cannot find a valid interface." << endl;
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
               cerr << "WARNING:	DIS_net_manager arbitrarily chose the "
                    << "interface:  " << valid[count].interface << endl;
               cerr << "	This machine has " << current << " interfaces."
                    << endl;
               cerr << "	Other possible choices were:" << endl;
               for ( int temp = 0; temp < current; temp++ )
                  cerr << "		" << valid[temp].interface << endl;
               }
#ifdef DEBUG
            cerr << "DEBUG:	DIS_net_manager chose interface "
                 << valid[count].interface << " by default." << endl;
#endif
            }
         else if ( !strcmp(interface,valid[count].interface ) )
            {
            found = TRUE;
#ifdef DEBUG
            cerr << "DEBUG:	DIS_net_manager chose interface "
                 << valid[count].interface << " as requested." << endl;
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
            cerr << "WARNING:	DIS_net_manager could not find the requested"
                 << " interface:  " << interface << endl;
            cerr << "	Using " << valid[count].interface << " instead." << endl;
            }
         else
            {
            cerr << "ERROR:	DIS_net_manager could not find the requested"
                 << " interface:  " << interface << endl;
            cerr << "	Your possible choices are:" << endl;
            for ( int temp = 0; temp < current; temp++ )
               cerr << "		" << valid[temp].interface << endl;
            close(sock_send);
            sock_send = -1;
            return (FALSE);
            }
         }
      net_inet_addr = valid[count].net_inet_addr;
      strcpy ( ascii_inet_addr, valid[count].ascii_inet_addr );
      strcpy ( interface, valid[count].interface );
      if ( network_mode == BCAST_MODE )
         bcopy((char *) &(valid[count].dest),
               (char *) &(dest),
               sizeof(ifr->ifr_broadaddr));

#ifdef DATA
      cerr << "DATA: DIS_net_manager interface = " << interface << endl;
      cerr << "DATA: DIS_net_manager interface inet = "
           << valid[count].ascii_inet_addr << endl;
      if ( network_mode == BCAST_MODE )
         cerr << "DATA:\tDIS_net_manager broadcast address = " 
              << inet_ntoa(dest.sin_addr) << endl;
#endif

      }

   /* Bind port numbers to sockets */
   sin_send.sin_family = AF_INET;
   if ( network_mode == BCAST_MODE )
      sin_send.sin_addr.s_addr = net_inet_addr.s_addr;
   else
      {
      if(setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_IF,&(net_inet_addr),
                    sizeof(IN_ADDR_SIZE)) < 0 )
         {
         perror("ERROR:	DIS_net_manager unable to assign multicast interface -\n   ");
         return (FALSE);
         }
      sin_send.sin_addr.s_addr = inet_addr(mc_group);
      } 
   sin_send.sin_port = htons(0);
   if ( network_mode == BCAST_MODE )
      {
      if (bind(sock_send, (struct sockaddr *) &sin_send, sizeof(sin_send)) < 0)
         {
         perror("ERROR:    DIS_net_manager unable to bind send socket port-\n   ");
         return (FALSE);
         }
      }

   sin_send.sin_port = htons(port);

   if ( network_mode == BCAST_MODE )
      {
      bcopy((char *) &(dest.sin_addr), (char *)&(sin_send.sin_addr),
            IN_ADDR_SIZE);
      }

   sin_recv.sin_family = AF_INET;
   if ( network_mode == BCAST_MODE )
      sin_recv.sin_addr.s_addr = dest.sin_addr.s_addr;
   else if ( network_mode == MCAST_MODE )
      {
      sin_recv.sin_addr.s_addr = INADDR_ANY;
      }
   sin_recv.sin_port = htons(port);
   if ( network_mode == MCAST_MODE )
      {
      mreq.imr_multiaddr.s_addr = inet_addr(mc_group);
      mreq.imr_interface.s_addr = net_inet_addr.s_addr;
      if (setsockopt(sock_recv,IPPROTO_IP,IP_ADD_MEMBERSHIP,
                     &mreq,sizeof(mreq)) < 0)
         {
         perror("ERROR:	DIS_net_manager unable to join multicast group -\n   ");
         return (FALSE);
         }
      net_inet_addr.s_addr = inet_addr(mc_group);
      }

   if ( setsockopt(sock_recv, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
      {
      perror("ERROR:	DIS_net_manager unable to reuse port address -\n   ");
      return(FALSE);
      }
   if (bind(sock_recv, (struct sockaddr *) &sin_recv, sizeof(sin_recv)) < 0)
      {
      perror("ERROR:	DIS_net_manager unable to bind receive socket port -\n   ");
      return (FALSE);
      }


   return (TRUE);

   }


int
DIS_net_manager::configure ()
   {

   /* Create the arena for shared memory variables */
   if ( init_arena() )
      {
      up_and_running = TRUE;
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager arena created." << endl;
#endif
       }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager failed to initialize arena." << endl;
#endif
      return (FALSE);
      }

   if ( allocate_shared_memory() )
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager shared memory allocated." << endl;
#endif
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager unable to allocate shared memory." << endl;
#endif
      up_and_running = FALSE;
      return (FALSE);
      }

#ifdef DATA
   cerr << "DATA:       DIS_net_manager 'current_net_buffer' = "
        << *current_net_buffer << endl;
   cerr << "DATA:       DIS_net_manager 'net_read_flag' = "
        << *net_read_flag << endl;
#endif

   if ( init_sockets() )
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager sockets opened and initialized." << endl;
#endif
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager failed to open/initialize sockets." << endl;
#endif
      up_and_running = FALSE;
      return (FALSE);
      }

   consumer_pid = int(getpid());

#ifdef DEBUG
   cerr << "DEBUG:      DIS_net_manager parent process id = "
        << consumer_pid << endl;
#endif

   signal ( SIGCLD, SIG_IGN );

   producer_pid = sproc ( receiver_process, PR_SALL, (void *)this );
   if ( producer_pid == -1 )
      {
      perror("ERROR:    DIS_net_manager unable to sproc receiver process-\n   ");
      up_and_running = FALSE;
      return(FALSE);
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   DIS_net_manager receiver process id = "
           << producer_pid << endl;
#endif
      }

   return (TRUE);
   }


/* Broadcast constructor */
DIS_net_manager::DIS_net_manager ( const unsigned char exercise,
                                 const unsigned char roundworld,
                                 const roundWorldStruct *roundfile,
                                 const int buffer_length,
                                 char *net_interface,
                                 const unsigned short receive_port  )
   {

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager Broadcast constructor started." << endl;
#endif

   init_member_vars();

   network_mode = BCAST_MODE;
   G_coord_error = 0;

   /* Initialize member variables from data passed in by user parameters */
   max_pdus = buffer_length;
   if (( net_interface == NULL ) || (strcasecmp(net_interface, "NONE") == 0))
      strcpy ( interface, "" );
   else
      strcpy ( interface, net_interface );
   exercise_id = exercise;
   round_world = roundworld;
   if ( roundfile != NULL ) {
      bcopy ((void *)roundfile, (void *) round_file,
             sizeof (roundWorldStruct));
   }
   else {
      round_file = NULL;
   }
   port = receive_port;

#ifdef DATA
   cerr << "DATA:	DIS_net_manager 'max_pdus' = " << max_pdus << endl;
   cerr << "DATA:	DIS_net_manager 'interface' = " << interface
        << endl;
   cerr << "DATA:	DIS_net_manager 'exercise_id' = " << int(exercise_id) 
        << endl;
   cerr << "DATA:	DIS_net_manager 'round_world' = " << int(round_world) 
        << endl;
//   cerr << "DATA:	DIS_net_manager 'round_file' = " << round_file << endl;
   cerr << "DATA:	DIS_net_manager 'port' = " << port << endl;
#endif

   up_and_running = TRUE;   
   if ( round_world )
      {
      if ( round_file == NULL )
         {
         cerr << "ERROR:	DIS_net_manager round world coordinates "
              << "requested but no coordinate file supplied." << endl;
         cerr << "	Continuing with flat world coordinates." << endl;
         round_world = 0;
         }
      else
         up_and_running = init_round_world ( round_file );
      }

   if ( up_and_running )
      up_and_running = configure();

   if ( net_interface != NULL )
      strcpy(net_interface,interface);

#ifdef TRACE
      cerr << "TRACE:	DIS_net_manager Broadcast constructor finished." << endl;
#endif

   }


/* Multicast constructor */
DIS_net_manager::DIS_net_manager ( const char *group,
                                 const unsigned char ttl,
                                 const unsigned short receive_port,
                                 const unsigned char exercise,
                                 const unsigned char roundworld,
                                 const roundWorldStruct *roundfile,
                                 const int buffer_length,
                                 char *net_interface )

   {

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager Multicast constructor started." << endl;
#endif

   init_member_vars();

   network_mode = MCAST_MODE;
   G_coord_error = 0;

   /* Initialize member variables from data passed in by user parameters */
   max_pdus = buffer_length;
   if ( net_interface != NULL ) {
      strcpy ( interface, net_interface );
   }
   else {
      strcpy ( interface, "" );
   }
   exercise_id = exercise;
   round_world = roundworld;
   if ( roundfile != NULL ) {
      bcopy ((void *)roundfile, (void *) round_file, 
             sizeof (roundWorldStruct));
   }
   else {
      round_file = NULL;
   }
   port = receive_port;
   if ( group != NULL ) {
      strcpy ( mc_group, group );
   }
   else {
      strcpy ( mc_group, DEFAULT_MC_GROUP );
   }
   mc_ttl = ttl;

#ifdef DATA
   cerr << "DATA:	DIS_net_manager 'max_pdus' = " << max_pdus << endl;
   cerr << "DATA:	DIS_net_manager 'interface' = " << interface
        << endl;
   cerr << "DATA:	DIS_net_manager 'exercise_id' = " << int(exercise_id)
        << endl;
   cerr << "DATA:	DIS_net_manager 'round_world' = " << int(round_world)
        << endl;
//   cerr << "DATA:	DIS_net_manager 'round_file' = " << round_file << endl;
   cerr << "DATA:	DIS_net_manager 'port' = " << port << endl;
   cerr << "DATA:	DIS_net_manager 'mc_group' = " << mc_group << endl;
   cerr << "DATA:	DIS_net_manager 'mc_ttl' = " << int(mc_ttl) << endl;
#endif

   up_and_running = TRUE;
   if ( round_world )
      {
      if ( round_file == NULL )
         {
         cerr << "ERROR:	DIS_net_manager round world coordinates "
              << "requested but no coordinate file supplied." << endl;
         cerr << "	Continuing with flat world coordinates." << endl;
         round_world = 0;
         }
      else
         up_and_running = init_round_world ( round_file );
      }

   if ( up_and_running )
      up_and_running = configure();

   if ( net_interface != NULL )
      strcpy(net_interface,interface);

#ifdef TRACE
      cerr << "TRACE:	DIS_net_manager Multicast constructor finished." << endl;
#endif

   }


void
DIS_net_manager::net_close()
   {

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager close started." << endl;
#endif

   if ( first_obj_read )
      {
      first_obj_read = FALSE;
      if (usvsema(start_reading_sema) == -1)
         {
         perror("ERROR:	DIS_net_manager unable to release reading sema -\n   ");
         }
      }

   if ( up_and_running && (producer_pid != -1) )
      { 
      *net_read_flag = FALSE;
      kill ( producer_pid, SIGTERM );
      //while (!*net_read_flag);
      up_and_running = FALSE;
      producer_pid = -1;
      }
 
   if ( buffer != NULL )
      {
      if ( buffer[0].pdu != NULL )
         {
         usfree ( buffer[0].pdu, netarena );
         buffer[0].pdu = NULL;
         }
      if ( buffer[1].pdu != NULL )
         {
         usfree ( buffer[1].pdu, netarena );
         buffer[1].pdu = NULL;
         }
      usfree ( (pdu_buffer *)buffer, netarena );
      buffer = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager memory deallocated." << endl;
#endif
      }

   if ( swap_buf_sema != NULL )
      {
      usfreesema ( swap_buf_sema, netarena ); 
      swap_buf_sema = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager swap semaphore memory deallocated." << endl;
#endif
      }

   if ( start_reading_sema != NULL )
      {
      usfreesema ( start_reading_sema, netarena );
      start_reading_sema = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager reading semaphore memory deallocated." 
           << endl;
#endif
      }

   if ( has_data != NULL )
      {
      usfreelock ( has_data, netarena );
      has_data = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager data lock memory deallocated." << endl;
#endif
      }

   if ( net_read_flag != NULL )
      {
      usfree ( (int *)net_read_flag, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager net_read_flag deallocated." << endl;
#endif
      net_read_flag = NULL;
      }

   if ( receive_all != NULL )
      {
      usfree ( (int *)receive_all, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager receive all flag deallocated." << endl;
#endif
      receive_all = NULL;
      }

   if ( receive_list != NULL )
      {
      usfree ( (unsigned char *)receive_list, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager receive list deallocated." << endl;
#endif
      receive_list = NULL;
      }

   
   if ( filter_count != NULL )
      {
      usfree ( (int *)filter_count, netarena );
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tDIS_net_manager filter_count deallocated." << endl;
#endif
      filter_count = NULL;
      }

   if ( filter_list != NULL )
      {
      usfree ( (EntityID *)filter_list, netarena );
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tDIS_net_manager filter list deallocated." << endl;
#endif
      filter_list = NULL;
      }

   if ( time_list != NULL )
      {
      usfree ( (int *)time_list, netarena );
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tDIS_net_manager filter time list deallocated." << endl;
#endif
      time_list = NULL;
      }


   if ( current_net_buffer != NULL )
      {
      usfree ( (int *)current_net_buffer, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager net buffer flag deallocated." << endl;
#endif  
      current_net_buffer = NULL;
      }

   if ( sock_send != -1 )
      {
      close(sock_send);
      sock_send = -1;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager send socket closed." << endl;
#endif
      }
   if ( sock_recv != -1 )
      {
      close(sock_recv);
      sock_recv = -1;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager receive socket closed." << endl;
#endif
      }

   if ( netarena != NULL )
      {
      usdetach(netarena);
      netarena = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager detached arena." << endl;
#endif
      }

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager close finished." << endl;
#endif

   }


/* Destructor */
DIS_net_manager::~DIS_net_manager()
   {
#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager destructor started." << endl;
#endif

   this->net_close();

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager destructor finished." << endl;
#endif

   }


int
DIS_net_manager::net_open ()
   {
   return up_and_running;
   }


int
DIS_net_manager::read_pdu ( char **apdu, 
                            PDUType *atype, 
                            sender_info &sender,
                            int &swapped_buffers  )
   {

   volatile pdu_buffer	*my_buffer;
   volatile pdu_buffer	*other_buffer;
   union PDU	*next_pdu;
   PDUHeader	*next_header;
   InfoPDU	*next_info;

   if ( first_obj_read )
      {
      first_obj_read = FALSE;
      if (usvsema(start_reading_sema) == -1)
         {
         perror("ERROR:	DIS_net_manager unable to release reading sema -\n   ");
         return FALSE;
         }
      }

   swapped_buffers = FALSE;
   my_buffer = buffer + !(*current_net_buffer); /* only works for two buffers */

   if ( (my_buffer->unread_pdus <= 0 ) && ustestlock(has_data) )
      {
      if (uspsema(swap_buf_sema) == -1)
         {
         perror("ERROR: DIS_net_manager read_pdu can't grab semaphore -\n   ");
         }
      else
         {
         other_buffer = buffer + (*current_net_buffer);
         my_buffer->current_pdu = 0;
         my_buffer->over_written_pdus = 0;
         my_buffer->over_write_mode = FALSE;
         my_buffer->unread_pdus = 0;
         if ( usunsetlock(has_data) == -1 )
            {
            perror("ERROR:	DIS_net_manager unable to unset lock -\n   ");
            }
         my_buffer = other_buffer;
         *current_net_buffer = !(*current_net_buffer);
         if ( !my_buffer->over_write_mode )
            my_buffer->current_pdu = 0;
         else
            {
            if ( my_buffer->current_pdu >= max_pdus )
               my_buffer->current_pdu = 0;
            }
#ifdef CONSUMER_DEBUG
         cerr << "DEBUG:	DIS_net_manager swapped buffers." << endl;
         cerr << "DEBUG:	DIS_net_manager " << my_buffer->unread_pdus
              << " acquired this swap." << endl;
#endif
         swapped_buffers = TRUE;
         if ( usvsema(swap_buf_sema) == -1 )
            {
            perror("ERROR:	DIS_net_manager read_pdu can't release semaphore -\n   ");
            }
         }
      }

/* DO NOT MAKE THIS AN ELSE */

   if ( my_buffer->unread_pdus > 0 )
      {
      next_pdu = (my_buffer->pdu + my_buffer->current_pdu);
      next_header = (PDUHeader *)next_pdu;
      next_info = (InfoPDU *)next_pdu;
      *apdu = (char *)next_pdu;
      *atype = next_header->type;
      bcopy ( (char *)&next_info->sender, (char *)&sender, sizeof(sender_info));
      my_buffer->current_pdu = (my_buffer->current_pdu + 1) % max_pdus;
      my_buffer->unread_pdus--;
      return (my_buffer->unread_pdus + 1);
      }
   else
      {
      *apdu = NULL;
      *atype = 0;
      return 0;
      }

   }


int
DIS_net_manager::write_pdu (char *pdu, PDUType type)
{
   int                          length, bytes_sent;
   PDUHeader                    *header;
   EntityStatePDU               *ESpdu;
   DetonationPDU                *Dpdu;
   FirePDU			*Fpdu;
   ServiceRequestPDU            *SRpdu;
   ResupplyPDU                  *Rpdu;
   DataPDU                      *DHRpdu;
   EnvironmentalPDU             *Epdu;
   int				sendcount;

   /* Fill in header data */
   header = (PDUHeader *) pdu;
#ifndef DIS_2v4
   header->protocol_version = DISVersionMay93;
#endif
#ifdef DIS_2v4
#ifdef IEEE1278_1_95
   header->protocol_version = DISVersionIEEE1278_1_95;
#else
   header->protocol_version = DISVersionDec95;
#endif
#endif
   if ( exercise_id <= 0 )
      header->exercise_ident = 1;
   else
      header->exercise_ident = exercise_id;
   header->type = type;

   bzero ( (void *)&(header->time_stamp), sizeof(unsigned int) );

   /* calculate length */
   switch (type) {
      case (EntityStatePDU_Type):
         ESpdu = (EntityStatePDU *) pdu;
#ifdef DIS_2v4
         header->protocol_family = (unsigned char)1;
#endif
#ifdef SIG96HACK
             ESpdu->entity_location.z -= SIG_Z_SHIFT;
#endif
         if ( round_world )
            convert_entity_state_to_round ( (EntityStatePDU *)pdu );
         length = sizeof(EntityStatePDU);       /* max */
#ifndef SIG96HACK
         length -= (sizeof(ArticulatParams) * MAX_ARTICULAT_PARAMS); /* base */
         length += (sizeof(ArticulatParams) * ESpdu->num_articulat_params);
#else
         length -= (sizeof(ArticulatParams) * MAX_ARTICULAT_PARAMS);
         ESpdu->num_articulat_params = 0;
#endif
         break;
      case (FirePDU_Type):
         Fpdu = (FirePDU *)pdu;
#ifdef DIS_2v4
         header->protocol_family = (unsigned char)2;
#endif
#ifdef SIG96HACK
             Fpdu->location_in_world.z -= SIG_Z_SHIFT;
#endif
         if ( round_world )
            convert_fire_to_round ( (FirePDU *)pdu );
         length = sizeof(FirePDU);
         break;
      case (DetonationPDU_Type):
         Dpdu = (DetonationPDU *) pdu;
#ifdef DIS_2v4
         header->protocol_family = (unsigned char)2;
#endif
#ifdef SIG96HACK
             Dpdu->location_in_world.z -= SIG_Z_SHIFT;
#endif
         if ( round_world )
            convert_detonation_to_round ( (DetonationPDU *)pdu );
         length = sizeof(DetonationPDU);        /* max */
         length -= (sizeof(ArticulatParams) * MAX_ARTICULAT_PARAMS); /* base */
         length += (sizeof(ArticulatParams) * Dpdu->num_articulat_params);
         break;
      case (CollisionPDU_Type):
         length = sizeof(CollisionPDU);
#ifdef DIS_2v4
         header->protocol_family = (unsigned char)1;
#endif
         break;
      case (ServiceRequestPDU_Type):
         SRpdu = (ServiceRequestPDU *) pdu;
         length = sizeof(ServiceRequestPDU);    /* max */
         length -= (sizeof(SupplyQuantity) * MAX_SUPPLY_QTY); /* base */
         length += (sizeof(SupplyQuantity) * SRpdu->num_supply_types);
         break;
      case (ResupplyOfferPDU_Type):
      case (ResupplyReceivedPDU_Type):
         Rpdu = (ResupplyPDU *) pdu;
         length = sizeof(ResupplyPDU);  /* max */
         length -= (sizeof(SupplyQuantity) * MAX_SUPPLY_QTY); /* base */
         length += (sizeof(SupplyQuantity) * Rpdu->num_supply_types);
         break;
      case (ResupplyCancelPDU_Type):
         length = sizeof(ResupplyCancelPDU);
         break;
      case (RepairCompletePDU_Type):
         length = sizeof(RepairCompletePDU);
         break;
      case (RepairResponsePDU_Type):
         length = sizeof(RepairResponsePDU);
         break;
      case (DataPDU_Type):
         DHRpdu = (DataPDU *) pdu;
         length = sizeof (DataPDU); // max
         length -= (sizeof (fixedDatumRecord) * MAX_FIXED_DATUM_SIZE); // base
         length += (sizeof (fixedDatumRecord) * DHRpdu->num_datum_fixed); 
         break;
      case (EnvironmentalPDU_Type):
         Epdu = (EnvironmentalPDU *) pdu;
         length = sizeof(EnvironmentalPDU); /* max */
         length -= (sizeof(ENV_RECORD) * MAX_ENV_RECORDS); /* base */
         length += (sizeof(ENV_RECORD) * Epdu->number_of_records);
         break;
      default:
         length = sizeof(PDU);
         cerr << "ERROR:	DIS_net_manager write_pdu unknow PDU type "
              << int(type) << endl;
         return (FALSE);
   }

   if ( round_world )
      {
      assert ( G_coord_error == 0 );
      if ( G_coord_error )
         {
         cerr << "ERROR:	DIS_net_manager coordinate conversion error "
              << "during write_pdu." << endl;
         cerr << "	PDU of type " << int(type) << " not being sent." << endl; 
         G_coord_error = 0;
         return (FALSE);
         }
      }

   if (length > sizeof(PDU))
      length = sizeof(PDU);
   header->length = length;

   /* Actually send it */
   sendcount = 0;
   while ( (sendcount++ < NUM_SEND_TRIES ) &&
          ((bytes_sent = sendto(sock_send, pdu, length, 0,
                                &sin_send, sizeof(sin_send))) < 0) )
      {
      perror("ERROR:\tDIS_net_manager sendto failed - \n   ");
      }
   if ( sendcount > 1 )
      {
      if ( sendcount <= NUM_SEND_TRIES )
         cerr << "WARNING:\tDIS_net_manager sent PDU on try # " << (sendcount-1)
              << endl;
      else
         {
         cerr << "ERROR:\tDIS_net_manager failed to send PDU after "
              << (sendcount-1) << " tries." << endl;
         return (FALSE);
         }
      }


   return (TRUE);
}


int
DIS_net_manager::write_buffer ( char *buffer, int buff_length )
{
   int sendcount = 0;
   int bytes_sent = 0;
   while ( (sendcount++ < NUM_SEND_TRIES ) &&
          ((bytes_sent = sendto(sock_send, buffer, buff_length, 0,
                                &sin_send, sizeof(sin_send))) < 0) )
      {
      perror("ERROR:\tDIS_net_manager sendto failed from write_buffer.");
      }
   return ( sendcount <= NUM_SEND_TRIES );

}


int
DIS_net_manager::pdutype_to_name ( PDUType pdutype, char *pduname )
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
      case EnvironmentalPDU_Type:
         strcpy ( pduname, "Environmental PDU" );
         identified = TRUE;
         break;
      default:
         strcpy ( pduname, "ERROR:  UNKNOWN PDU TYPE" );
         identified = FALSE;
         break;
      }
   return identified;
   }


int
DIS_net_manager::receive_all_pdus ()
   {
   unsigned char one = 1;
   if ( receive_all != NULL )
      *receive_all = TRUE;
   else
      return FALSE;
   if ( receive_list != NULL )
      bcopy ( (char *)&one, (char *)receive_list, sizeof(receive_list) );
   else
      return FALSE;
   return TRUE;
   }


int
DIS_net_manager::receive_no_pdus ()
   {
   if ( receive_all != NULL )
      *receive_all = FALSE;
   else
      return FALSE;
   if ( receive_list != NULL )
      bzero ( (char *)receive_list, sizeof(receive_list) );
   else
      return FALSE;
   return TRUE;
   }


int
DIS_net_manager::add_to_receive_list ( PDUType pdutype )
   {
   if ( receive_all != NULL )
      *receive_all = FALSE;
   else
      return FALSE;
   if ( receive_list != NULL )
      receive_list[pdutype] = TRUE;
   else 
      return FALSE;
   return TRUE;
   }


int
DIS_net_manager::remove_from_receive_list ( PDUType pdutype )
   {
   if ( receive_all != NULL )
      *receive_all = FALSE;
   else
      return FALSE;
   if ( receive_list != NULL )
      receive_list[pdutype] = FALSE;
   else
      return FALSE;
   return TRUE;
   }


int
DIS_net_manager::add_to_filter_list ( EntityID entity_info, int use_time )
   {
   if ( (filter_count == NULL) || (filter_list == NULL) ||
        (time_list == NULL) )
      {
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tadd_to_filter_list called before variables "
           << "allocated memory." << endl;
#endif
      return FALSE;
      }
   else
      {
      if ( *filter_count < MAX_FILTER_ENTRIES )
         {
         bcopy ( (char *)&entity_info,
                 (char *)&filter_list[*filter_count],
                 sizeof(EntityID) );
         if ( use_time )
            time_list[*filter_count] = (int)time(0);
         else
            time_list[*filter_count] = -1;
#if defined(DEBUG) || defined(FDEBUG)
         cerr << "FDEBUG:\tEntity ID "
              << entity_info.address.site << "/"
              << entity_info.address.host << "/"
              << entity_info.entity << " added to filter list at pos "
              << *filter_count << endl;
         cerr << "\tTime stamp is " << time_list[*filter_count] << endl;
#endif
         (*filter_count)++;
         return TRUE;
         }
      else
         {
#if defined(DEBUG) || defined(FDEBUG)
         cerr << "FDEBUG:\tadd_to_filter_list failed - " << endl;
         cerr << "\tThe list is full with " << MAX_FILTER_ENTRIES
              << " entries." << endl;
#endif
         return FALSE;
         }
      } 
   }


int
DIS_net_manager::remove_from_filter_list ( EntityID entity_info )
   {
   if ( (filter_count == NULL) || (filter_list == NULL) )
      {
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tremote_from_filter_list called before variables "
           << "allocated memory." << endl;
#endif
      return FALSE;
      }
   else
      {
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tremove_from_filter_list started." << endl;
#endif
      int found = FALSE;
      int count = 0;
      while ( !found && (count < *filter_count) )
         {
         found = ( (bcmp((char *)&entity_info,
                         (char *)&filter_list[count],
                         sizeof(EntityID))) == 0);
         if ( !found )
            count++;
         }
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tEntity ID "
           << entity_info.address.site << "/"
           << entity_info.address.host << "/"
           << entity_info.entity;
#endif
      if ( found )
         {
         int i;
#if defined(DEBUG) || defined(FDEBUG)
         cerr << " removed from filter list." << endl;
         cerr << "\tEntity was at position " << count << endl;
         cerr << "\tMoving ";
#endif
         for ( i = count; i < (*filter_count - 1); i++ )
            {
#if defined(DEBUG) || defined(FDEBUG)
            cerr << i+1 << "->" << i << ",";
#endif
            bcopy ( (char *)&filter_list[i+1], 
                    (char *)&filter_list[i],
                    sizeof(EntityID) );
            time_list[i] = time_list[i+1];
            }
         (*filter_count)--;
#if defined(DEBUG) || defined(FDEBUG)
         cerr << "DONE." << endl;
         cerr << "\tfilter_count is now " << *filter_count << endl;
#endif
         }
      else
         {
#if defined(DEBUG) || defined(FDEBUG)
         cerr << " not found in filter list." << endl;
#endif
         }
      return found;
      }
   }


inline int
DIS_net_manager::in_filter_list ( EntityID entity_info )
   {
   int found = FALSE;
   int count = 0;

   if ( (filter_count == NULL) || (filter_list == NULL) )
      return FALSE;

   if (*filter_count == 0) 
      return FALSE;

   while ( !found && (count < *filter_count))
      {

      if ( (filter_list[count].address.site == (unsigned short)0) ||
           (filter_list[count].address.site == entity_info.address.site) )
         { /* So far, we have a match with site */

         if ( (filter_list[count].address.host == (unsigned short)0) ||
              (filter_list[count].address.host == entity_info.address.host) )
            { /* So far, we have a match with site and host */

            found = ( (filter_list[count].entity == (unsigned short)0) ||
                      (filter_list[count].entity == entity_info.entity) );

            } /* Site and host match */ 

         } /* Site match */
   
      if ( !found )
         count++;

      } /* while not found */

   if ( found && (time_list[count] != -1) )
      {
      time_list[count] = (int)time(0);
#if defined(DEBUG) || defined(FDEBUG)
      cerr << "FDEBUG:\tUpdated time for filter object at pos "
           << count << endl;
      cerr << "\tNew time is " << time_list[count] << endl;
#endif
      }

   return found;
   }


void
DIS_net_manager::review_filter_list ()
   {
   int done = 0;
   int count = 0;
   int now = (int)time(0);

#if defined(DEBUG) || defined(FDEBUG)
   cerr << "FDEBUG:\tReviewing times, current time is " << now << ":" << endl;
#endif

   while ( !done )
      {
      if ( count < *filter_count )
         {
#if defined(DEBUG) || defined(FDEBUG)
         cerr << "\tpos " << count << " time is " << time_list[count] << endl;
#endif
         if ( (time_list[count] != -1) && ((time_list[count]+12) < now) ) {
#if defined(DEBUG) || defined(FDEBUG)
          cerr << "Time to remove item at pos " << count << endl;
          cerr << "Filter count is " << *filter_count << endl;
          cerr << "Filter ID is " << int(filter_list[count].address.site)
               << ", " << int(filter_list[count].address.host)
               << ", " << int(filter_list[count].entity) << endl;
#endif
            remove_from_filter_list ( filter_list[count] );
         }
         count++;
         }
      else
         done = TRUE;
      }      
   }


void
signal_handler(int sig, ...)
   {
   if ((signal ( sig, SIG_IGN )) == SIG_ERR)
      perror("DIS_net_manager:\tSignal Error -\n   ");
   switch ( sig )
      {
      case SIGTERM:
         exit(0);
         break;
      case SIGHUP:
         if ( getppid() == 1 )
            {
            cerr << "DEATH NOTICE:" << endl;
            cerr << "\tParent process terminated without calling net_close()."
                 << endl;
            cerr << "\tDIS_net_manager terminating to prevent orphan process."
                 << endl;
            exit(0);
            }
         break;
      case SIGINT:
         cerr << "DEATH NOTICE:	DIS_net_manager exiting due to interrupt "
              << "signal." << endl;
         exit(0);
         break;
      case SIGQUIT:
         cerr << "DEATH NOTICE:	DIS_net_manager exiting due to quit "
              << "signal." << endl;
         exit(0);
         break;
      default:
         cerr << "DEATH NOTICE:	DIS_net_manager exiting due to signal:  "
              << sig << endl;
         exit(0);
         break;
      }
   signal ( sig, (void (*)(int))signal_handler );
   }


void
receiver_process ( void *tempthis )
   {
   DIS_net_manager	*obj;
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
   DataPDU              *datpdu = (DataPDU *) &input_buf;
   EnvironmentalPDU *enpdu = (EnvironmentalPDU *) &input_buf;
   InfoPDU		*info_pdu = (InfoPDU *) &input_buf;
   InfoPDU		*info_dest;
   volatile pdu_buffer	*my_buffer;
   struct hostent	*entity;
#ifdef VERIFICATION
   int			print_error = FALSE;
#endif
   static               void (*temp_signal)(int);
#ifdef VERIFICATION
   int			correct_size;
#endif


#ifdef STATS
   long			packets_read = 0;
#endif

   obj = (DIS_net_manager *)tempthis;

   /* The following call is necessary for this child process to actually */
   /* respond to SIGHUP signals when the parent process dies.            */
   prctl(PR_TERMCHILD);
   if ((signal ( SIGTERM, (void (*)(int))signal_handler )) == SIG_ERR)
            perror("DIS_net_manager:\tError setting SIGTERM handler -\n   ");
   if ((signal ( SIGHUP, (void (*)(int))signal_handler )) == SIG_ERR)
            perror("DIS_net_manager:\tError setting SIGHUP handler -\n   ");
   if ( (temp_signal = signal ( SIGINT, (void (*)(int))signal_handler )) 
      != SIG_DFL )
      {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("DIS_net_manager:\tError setting SIGINT ignore -\n   ");
#ifdef DEBUG
      cerr << "*** DIS_net_manager process ignoring interupts." << endl;
#endif
      }
   if ((signal ( SIGQUIT, (void (*)(int))signal_handler )) == SIG_ERR)
      perror("DIS_net_manager:\tError setting SIGQUIT handler -\n   ");

   if (uspsema(obj->start_reading_sema) == -1)
      {
      perror("ERROR:	DIS_net_manager unable to initially get reading sema -\n   ");
      cerr << "ERROR:	DIS_net_manager receiver process is not starting." << endl;
      exit(0);
      }

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager receiver_process started." << endl;
#endif

#ifdef VERIFICATION

   cerr << "VERIFICATION: DIS_net_manager verifying PDU size." << endl;

#endif

   while ( *(obj->net_read_flag) )
      {

      /* Read packet off network into temp buffer */
      /*   Net read must be done while semaphore is released to prevent */
      /*   hanging application while no packets are incoming */

      if ((bytes_read = recvfrom(obj->sock_recv, (void *) &input_buf,
              sizeof(union PDU), 0, (struct sockaddr *) &from, &len)) == -1)
         {
         if ( (errno != EINTR) || *(obj->net_read_flag) )
            {
            perror("ERROR:	DIS_net_manager receiver process, recvfrom failed -\n   ");
            if ( errno == EINTR )
               cerr << "WARNING:	DIS_net_manager interrupted but not "
                    << "quitting because net_read_flag is still true." << endl;
            else
               cerr << "   ERRNO:  " << errno  << ", net_read_flag = "
                    << *(obj->net_read_flag) << endl;
            }
         good_read = FALSE;
         }
      else
         good_read = TRUE;

/*
      if ( getppid() == 1 )
         {
         cerr << "DEATH NOTICE:	DIS_net_manager exiting because parent"
              << " process died." << endl;
         obj->net_read_flag = 0;
         good_read = FALSE;
         }
*/

/* for now, we will see our own broadcasts and use the EntityID filter_list 

      if ( good_read &&
         (bcmp((void *) &(from.sin_addr), (void *) &(obj->net_inet_addr),
                                                       IN_ADDR_SIZE) == 0) )
         {
         continue;
         }
*/
 
      obj->review_filter_list ();


      if ( good_read )
         {
/* commented out for performance reasons 
         entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
*/
         strcpy ( info_pdu->sender.internet_address, 
                  inet_ntoa(from.sin_addr) );
/*
         strcpy ( info_pdu->sender.host_name, 
                  entity->h_name );
*/
         info_pdu->sender.host_name[0] = '\0';
         }

      if ( good_read && !*obj->receive_all )
         {
         if ( !obj->receive_list[int(header->type)] )
            {
#ifdef DEBUG
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "DEBUG:	Receive List Discarding pdu of type " 
                 << int(header->type) << " from "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
#endif
            continue;
            }
         }

      if ( good_read )
         {
         int filter;
         switch ( header->type )
            {
            case (FirePDU_Type):
               filter = obj->in_filter_list ( fpdu->firing_entity_id );
               if ( filter &&
                    ((fpdu->munition_id.address.site != (unsigned short)0) ||
                    (fpdu->munition_id.address.host != (unsigned short)0) ||
                    (fpdu->munition_id.entity != (unsigned short)0)) &&
                    (!obj->in_filter_list ( fpdu->munition_id )) )
                  {
                  obj->add_to_filter_list ( fpdu->munition_id, TRUE );
                  }
               break;
            case (EntityStatePDU_Type):
               filter = obj->in_filter_list ( epdu->entity_id );
               break;
            case (EnvironmentalPDU_Type):
                filter = obj->in_filter_list ( enpdu->env_id);
                break;
            case (DetonationPDU_Type):
               filter = obj->in_filter_list ( dpdu->firing_entity_id );
/*
               if ( filter &&
                    ((dpdu->munition_id.address.site != (unsigned short)0) ||
                    (dpdu->munition_id.address.host != (unsigned short)0) ||
                    (dpdu->munition_id.entity != (unsigned short)0)) )
                  {
                  obj->remove_from_filter_list ( dpdu->munition_id );
                  }
*/
               break;
            case (DataPDU_Type):
               filter = obj->in_filter_list ( datpdu->orig_entity_id );
               break;
            default:
               filter = FALSE;
               break;
            }
         if ( filter )
            {
#if defined(DEBUG) || defined(FDEBUG)
            cerr << "FDEBUG:\tFiltered pdu due to Entity ID." << endl;
#endif
            good_read = FALSE;
            continue; /* filter the pdu by ignoring it */
            }
         }

#ifdef VERIFICATION
      if ( good_read )
         {
         print_error = FALSE;
         switch ( header->type )
            {
            case (FirePDU_Type):
               if ( int(header->length) != sizeof(FirePDU) )
                  {
                  cerr << "WARNING:	DIS_net_manager specified length of "
                       << "incoming FIRE PDU is non-standard." << endl;
                  cerr << "	Standard Fire PDU bytes = " 
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
                  cerr << "WARNING:	DIS_net_manager specified length of "
                          "incoming Entity State PDU is non-standard." << endl;
                  cerr << "	Standard Entity State PDU bytes with "
                       <<  int(epdu->num_articulat_params)
                       << " articulations is = " << correct_size << endl;
                  print_error = TRUE;
                  }
               break;
            case (EnvironmentalPDU_Type):
/*
               correct_size = sizeof(EnvironmentalPDU +
                              ( sizeof(ENV_RECORD) *
                              (en->number_of_records - MAX_ENV_RECORDS));
               if ( int(header->length) != correct_size )
                  {
                  cerr << "WARNING: DIS_net_manager specified length of "
                          "incoming Environmental PDU is non-standard." << endl;
                  cerr << "   Standard Environmental PDU bytes with "
                       <<  int(en->number_of_records) 
                       << " records is  = " << correct_size << endl;
                  print_error = TRUE;
                  }
*/
               break;

            case (DetonationPDU_Type):
               correct_size = sizeof(DetonationPDU) +
                              ( sizeof(ArticulatParams) *
                              (dpdu->num_articulat_params - MAX_ARTICULAT_PARAMS));
               if ( int(header->length) != correct_size )
                  {
                  cerr << "WARNING:	DIS_net_manager specified length of "
                          "incoming Detonation PDU is non-standard." << endl;
                  cerr << "	Standard Detonation PDU bytes with "
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
               cerr << "WARNING:	DIS_net_manager specified length of "
                    << "incoming PDU is not equal to actual bytes read."
                    << endl;
               print_error = TRUE;
               }
   
         if ( print_error )
            {
            cerr << "	Actual bytes read = " << bytes_read << endl;
            cerr << "	Bytes specified in PDU header = "
                 << int(header->length) << endl;
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "	Packet came from:  "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
            }
   
         }
#endif

      if ( good_read && (header->length > sizeof(union PDU) ) )
         {
         cerr << "ERROR:	DIS_net_manager ignored PDU because "
              << "it was too large." << endl;
         good_read = FALSE;
         entity = gethostbyaddr ( (void *)&from.sin_addr,
                                  sizeof(struct in_addr), AF_INET );
         cerr << "	Packet came from:  "
              << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
         cerr << "	Packet type:  " << int(header->type)
              << ", Size:  " << int(header->length) << endl;
         }
  
      if ( good_read )
         {
         if ( (obj->exercise_id > 0) && 
              (int(header->exercise_ident) != int(obj->exercise_id)) )
            good_read = FALSE;
         }

      
      if ( good_read && obj->round_world )
         {
         switch ( header->type )
            {
            case (FirePDU_Type):
               convert_fire_from_round ( (FirePDU *)(&input_buf) );
#ifdef SIG96HACK
                fpdu->location_in_world.z += SIG_Z_SHIFT;
#endif
               break;
            case (EntityStatePDU_Type):
               convert_entity_state_from_round ( (EntityStatePDU *)(&input_buf) );
#ifdef SIG96HACK
                epdu->entity_location.z += SIG_Z_SHIFT;
#endif
               break;
            case (DetonationPDU_Type):
               convert_detonation_from_round ( (DetonationPDU *)(&input_buf) );
#ifdef SIG96HACK
               dpdu->location_in_world.z += SIG_Z_SHIFT;
#endif
               break;
#ifndef NO_DVW
            case (EnvironmentalPDU_Type):
               convert_environment_from_round ( (EnvironmentalPDU *)(&input_buf) );
                 break;
#endif // NO_DVW
            default:
               break;
            }
         if ( G_coord_error )
            {
            good_read = FALSE;
            G_coord_error = 0;
            cerr << "ERROR:	DIS_net_manager ignored PDU due to "
                 << "a conversion error." << endl;
            entity = gethostbyaddr ( (void *)&from.sin_addr, 
                                     sizeof(struct in_addr), AF_INET ); 
            cerr << "	Packet came from:  "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
            cerr << "	Packet type:  " << int(header->type)
                 << ", Size:  " << int(header->length) << endl;
            switch ( header->type )
               {
               case (EntityStatePDU_Type):
                  cerr << "	Site: " << int(epdu->entity_id.address.site)
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
#ifdef STATS
         packets_read++;
#endif
#ifdef PACKET
         cerr << "PACKET:	DIS_net_manager got a packet of type "
              << int(header->type) << endl;
#endif
 
         if (uspsema(obj->swap_buf_sema) == -1)
            {
            perror("ERROR:	DIS_net_manager can't grab semaphore -\n   ");
            }
         else
            {
            my_buffer = obj->buffer + *(obj->current_net_buffer);
            if ( my_buffer->current_pdu >= obj->max_pdus )
               {
               my_buffer->over_write_mode = TRUE;
               my_buffer->current_pdu = 0;
#ifdef DEBUG
               cerr << "DEBUG:	DIS_net_manager overflow occured." << endl;
               cerr << "	The net manager is receiving packets faster "
                    << "than the application is processing them." << endl;
               cerr << "	Either increase the buffer size or call "
                    << "read_pdu faster." << endl;
#endif
               }

            bcopy ( (void *) &input_buf, 
                    (void *) &(my_buffer->pdu[my_buffer->current_pdu]),
                    header->length );
            info_dest = (InfoPDU *) &(my_buffer->pdu[my_buffer->current_pdu]);
            bcopy ( (void *) &info_pdu->sender,
                    (void *) &info_dest->sender,
                    sizeof(sender_info) );
            my_buffer->current_pdu++;
            if ( my_buffer->over_write_mode )
               my_buffer->over_written_pdus++;
            else
               my_buffer->unread_pdus++;

            if (!ustestlock(obj->has_data))
               ussetlock(obj->has_data);
           
            if ( usvsema(obj->swap_buf_sema) == -1 )
               {
               perror("ERROR:	DIS_net_manager can't release semaphore -\n   ");
               } 

            }

         }

      }

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager receiver_process finished." << endl;
#endif

#ifdef STATS
   cerr << "STATS:	DIS_net_manager receiver_process got "
        << packets_read << " packets." << endl;
#endif

   *(obj->net_read_flag) = TRUE;
   exit(0);
   }

