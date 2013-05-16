// File: <idunetlib.cc>

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

#include "idunetlib.h"

/* Local type defintions */
typedef struct 
   {
   struct in_addr	net_inet_addr;
   char			ascii_inet_addr[25];
   char			interface[25];
   struct sockaddr_in	dest;
   } valid_intrf_info;


/* Static member initializations */
int	IDU_net_manager::num_net_readers = 0;
int	IDU_net_manager::arena_counter = 0;

/* Member functions */

void
IDU_net_manager::init_member_vars()
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
   network_mode = IDU_ERROR_MODE;
   sock_send = -1;
   sock_recv = -1;
   bzero ( (char *)&sin_send, sizeof(sin_send) );
   bzero ( (char *)&sin_recv, sizeof(sin_recv) );
   strcpy ( mc_group, "" );
   mc_ttl = 1;
   loopback = NULL;
   port = 0;
   producer_pid = -1;
   consumer_pid = -1;
   up_and_running = FALSE;
   net_read_flag = NULL;
   buffer = NULL;
   current_net_buffer = NULL;
   max_idus = -1;
   num_idu_types = 0;
   receive_list = NULL;
   receive_all = NULL;
   }


int
IDU_net_manager::init_arena ()
   {
   int arena_size;
   u_char all_ones = 0xFF;
   IDUType idutype;

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager init_arena started." << endl;
#endif

   bcopy ( (char *)&all_ones, (char *)&idutype, sizeof(idutype) );
   num_idu_types = int(idutype) + 1;
#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager " << num_idu_types 
        << " possible IDU types from size of field." << endl;
#endif

   /* Compute arena size to hold 2 sets of idu_buffer data each with the  */
   /*    requested number of union IDUs.  Then add the size of the */
   /*    semaphores, data locks, and flags.  Then add the size of the */
   /*    filter list and then a fudge factor of 20%. */
   arena_size = ( ( (sizeof(union IDU) * max_idus) + sizeof(idu_buffer) )*2);
   /* have to use int instead of usema_t for compile */
   arena_size += ((2*sizeof(int)) + (sizeof(ulock_t)) + (5 * sizeof(int)));
   arena_size += num_idu_types * sizeof(u_char);
   arena_size = int ( arena_size * 1.2f );

#ifdef DATA
   cerr << "DATA:	IDU_net_manager requesting arena size of "
        << arena_size << endl;
   cerr << "DATA:	IDU_net_manager union IDU size is " 
        << sizeof(union IDU) << endl;
#endif

   /* Set arena size */
   if (usconfig(CONF_INITSIZE, arena_size) == -1)
      {
      perror("ERROR:	IDU_net_manager initializing arena size -\n   ");
      return (FALSE);
      }

   /* Set arena to set the size up front with no autogrow */
   if(usconfig(CONF_AUTOGROW,0) == -1)
      {
      perror("ERROR:	IDU_net_manager setting autogrow to 0 -\n   ");
      return (FALSE);
      }

   /* Set arena to be shared by parent and child only */
   if (usconfig(CONF_ARENATYPE, US_SHAREDONLY) == -1)
      {
      perror("ERROR:	IDU_net_manager configuring arena -\n   ");
      return (FALSE);
      }

   /* Name and open arena */
   arena_counter++;
   sprintf ( arena_name, "/usr/tmp/IDU_arena.%d.%d", getpid(), arena_counter );
#ifdef DATA
   cerr << "DATA:	IDU_net_manager 'arena_counter' = " << arena_counter << endl;
   cerr << "DATA:	IDU_net_manager 'arena_name' = " << arena_name << endl;
#endif
   if ( (netarena = usinit(arena_name)) == NULL )
      {
      arena_counter--;
      perror("ERROR:	IDU_net_manager creating arena -\n   ");
#ifdef DATA
   cerr << "DATA:	IDU_net_manager 'arena_counter' = " << arena_counter << endl;
#endif
      return (FALSE);
      }

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager init_arena finished." << endl;
#endif
 
   return (TRUE);
 
   }


int
IDU_net_manager::allocate_shared_memory()
   {
   int zero = 0;

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager allocate_shared_memory started." << endl;
#endif

   if ( (buffer = (idu_buffer *)usmalloc(2*sizeof(idu_buffer), netarena)) ==
        NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate control structures -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager control structure allocated." << endl;
#endif

   buffer[0].idu = (union IDU *)usmalloc(sizeof(union IDU)*max_idus, netarena);
   if ( buffer[0].idu == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate first IDU buffer-\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager first IDU buffer allocated." << endl;
#endif

   buffer[1].idu = (union IDU *)usmalloc(sizeof(union IDU)*max_idus, netarena);
   if ( buffer[1].idu == NULL )
      {
      perror("ERROR:    IDU_net_manager unable to allocate second IDU buffer-\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager second IDU buffer allocated." << endl;
#endif

   buffer[0].current_idu = 0;
   buffer[1].current_idu = 0;
   
   buffer[0].over_written_idus = 0;
   buffer[1].over_written_idus = 0;

   buffer[0].over_write_mode = FALSE;
   buffer[1].over_write_mode = FALSE;

   buffer[0].unread_idus = 0;
   buffer[1].unread_idus = 0;

   /* Init semaphore */
   if ((swap_buf_sema = usnewsema(netarena, 1)) == NULL)
      {
      perror("ERROR:	IDU_net_manager unable to allocate swap semaphore -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager swap semaphore allocated." << endl;
#endif

   if ((start_reading_sema = usnewsema(netarena, 1)) == NULL)
      {
      perror("ERROR:	IDU_net_manager unable to allocate reading semaphore -\n   ");
      return (FALSE);
      }

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager reading semaphore allocated." << endl;
#endif

   if (uspsema(start_reading_sema) == -1)
      {
      perror("ERROR:	IDU_net_manager unable to initially set reading sema -\n   ");
      return (FALSE);
      }

   if ( (has_data = usnewlock ( netarena )) == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to alloate data lock -\n   ");
      return (FALSE);
      }
   
   usinitlock ( has_data );

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager data lock allocated." << endl;
#endif

   net_read_flag = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( net_read_flag == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate read flag -\n   ");
      return (FALSE);
      }
   *net_read_flag = TRUE;

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager read flag allocated." << endl;
#endif

   current_net_buffer = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( current_net_buffer == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate net buffer flag -\n   ");
      return (FALSE);
      }
   *current_net_buffer = 0;

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager net buffer flag allocated." << endl;
#endif

   receive_all = (volatile int *)usmalloc(sizeof(int), netarena);
   if ( receive_all == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate receive_all flag -\n   ");
      return (FALSE);
      }
   *receive_all = 1;

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager receive_all flag allocated." << endl;
#endif

   receive_list = (volatile u_char *)usmalloc(num_idu_types, netarena);
   if ( receive_list == NULL )
      {
      perror("ERROR:	IDU_net_manager unable to allocate receive list -\n   ");
      return (FALSE);
      }
   bcopy ( (char *)&zero, (char *)receive_list, num_idu_types );

#ifdef DEBUG
   cerr << "DEBUG:	IDU_net_manager receive_list allocated "
        << num_idu_types << " bytes." << endl;
#endif

   loopback = (volatile u_char *)usmalloc(sizeof(u_char), netarena);
   if ( loopback == NULL )
      {
      perror("ERROR:\tIDU_net_manager unable to allocate loopback flag -\n ");
      return (FALSE);
      }
   *loopback = (u_char)FALSE;

#ifdef DEBUG
   cerr << "DEBUG:\tIDU_net_manager loopback flag allocated." << endl;
#endif


#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager allocate_shared_memory finished." << endl;
#endif

   return (TRUE);
   }


int
IDU_net_manager::init_sockets()
   {
   int on = 1;
   valid_intrf_info     valid[4];
   int                  i, current, count, found;
   char                 buf[BUFSIZ];
   struct ifconf        ifc;
   struct ifreq         *ifr;
   struct sockaddr_in   *in_addr;
   struct ip_mreq	mreq;
   int			sendbuff = IDU_SEND_BUFFER_SIZE;
   int			rcvbuff = IDU_RECEIVE_BUFFER_SIZE;

   /* Open send socket */
   if ((sock_send = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:	IDU_net_manager unable to open send socket -\n   ");
      return (FALSE);
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:	IDU_net_manager send socket opened." << endl;
#endif
  
   /* Resize the socket send buffer to be large enought not to overflow */
   if ((setsockopt(sock_send,SOL_SOCKET,SO_SNDBUF,
                   (char *)&sendbuff, sizeof(sendbuff))) < 0 )
      {
      perror("WARNING:\tIDU_net_manager unable to resize socket send buffer -\n\t");
      cerr << "\tAttempted size change to " << sendbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tIDU_net_manager send buffer resized to "
           << sendbuff << " bytes." << endl;
#endif

   /* Open receive socket */
   if ((sock_recv = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
      perror("ERROR:	IDU_net_manager unable to open receive socket-\n   ");
      return (FALSE);
      }
#ifdef DEBUG
    else
      cerr << "DEBUG:	IDU_net_manager receive socket opened." << endl;
#endif

   /* Resize the socket receive buffer to be large enough not to overflow */
   if ((setsockopt(sock_recv,SOL_SOCKET,SO_RCVBUF,
                   (char *)&rcvbuff, sizeof(rcvbuff))) < 0 )
      {
      perror("WARNING:\tIDU_net_manager unable to resize socket receive buffer -\n\t");
      cerr << "\tAttempted size change to " << rcvbuff << " bytes failed."
           << endl;
      }
#ifdef DEBUG
   else
      cerr << "DEBUG:\tIDU_net_manager receive buffer resized to "
           << rcvbuff << " bytes." << endl;
#endif


   if ( network_mode == IDU_BCAST_MODE )
      {
      /* Mark send socket to allow broadcasting */
      if (setsockopt(sock_send, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
         {
         perror("ERROR:	IDU_net_manager unable to mark send socket for BCAST -\n   ");
         return (FALSE);
         }
      }
   else if ( network_mode == IDU_MCAST_MODE )
      {
      /* Configure send socket time-to-live for multicast */
      if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_TTL,
                     &mc_ttl,sizeof(mc_ttl)) < 0)
         {
         perror("ERROR:	IDU_net_manager unable to mark send socket for MCAST -\n   ");
         return(FALSE);
         }

      /* Configure local packet loopback for multicast -- */

      if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_LOOP,
                     (void *)loopback,sizeof(u_char)) < 0)
         {
         perror("ERROR:	IDU_net_manager unable to configure loopback -\n   ");
         return(FALSE);
         }
      }

   /* Get InterFace CONFig */
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock_send, SIOCGIFCONF, (char *) &ifc) < 0)
      {
      perror("ERROR:	IDU_net_manager unable to get interface config -\n   ");
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
         perror("ERROR:	IDU_net_manager unable to get interface flags -\n   ");
         close(sock_send);
         sock_send = -1;
         return (FALSE);
         }

      /*  Skip boring cases */
      if ( ((ifr->ifr_flags & IFF_UP) == 0) ||            /* interface down? */
           (ifr->ifr_flags & IFF_LOOPBACK)  ||            /* local loopback? */
           ((network_mode == IDU_BCAST_MODE)&&((ifr->ifr_flags&IFF_BROADCAST) == 0)) ||
           ((network_mode == IDU_MCAST_MODE)&&((ifr->ifr_flags&IFF_MULTICAST) == 0)) )
         {
         continue;
         }

      /* Get and save InterFace ADDRess */
      if (ioctl(sock_send, SIOCGIFADDR, (char *) ifr) == -1)
         {
         perror("ERROR: IDU_net_manager unable to determine inet address-\n   ");
         close(sock_send);
         sock_send = -1;
         return (FALSE);
         }
      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);

      valid[current].net_inet_addr = in_addr->sin_addr;
      strcpy ( valid[current].ascii_inet_addr,
               inet_ntoa(valid[current].net_inet_addr) );
      strcpy ( valid[current].interface, ifr->ifr_name );

      if ( network_mode == IDU_BCAST_MODE )
         {
         /* Get InterFace BRoaDcast ADDRess */
         if (ioctl(sock_send, SIOCGIFBRDADDR, (char *) ifr) < 0)
            {
            perror("ERROR:	IDU_net_manager unable to determine BCAST address -\n   ");
            close (sock_send);
            sock_send = -1;
            return (FALSE);
            }
         bcopy((char *) &(ifr->ifr_broadaddr), 
               (char *) &(valid[current].dest),
               sizeof(ifr->ifr_broadaddr));
         } 


#ifdef DATA
         cerr << "DATA:	IDU_net_manager detected ";
         if ( network_mode == IDU_BCAST_MODE )
            cerr << "broadcast";
         else if ( network_mode == IDU_MCAST_MODE )
            cerr << "multicast";
         cerr << " capable interface = " << valid[current].interface << endl;
         cerr << "DATA:	IDU_net_manager detected ";
         if ( network_mode == IDU_BCAST_MODE )
            cerr << "broadcast";
         else if ( network_mode == IDU_MCAST_MODE )
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
      cerr << "ERROR:	IDU_net_manager cannot find a valid interface." << endl;
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
               cerr << "WARNING:	IDU_net_manager arbitrarily chose the "
                    << "interface:  " << valid[count].interface << endl;
               cerr << "	This machine has " << current << " interfaces."
                    << endl;
               cerr << "	Other possible choices were:" << endl;
               for ( int temp = 0; temp < current; temp++ )
                  cerr << "		" << valid[temp].interface << endl;
               }
#ifdef DEBUG
            cerr << "DEBUG:	IDU_net_manager chose interface "
                 << valid[count].interface << " by default." << endl;
#endif
            }
         else if ( !strcmp(interface,valid[count].interface ) )
            {
            found = TRUE;
#ifdef DEBUG
            cerr << "DEBUG:	IDU_net_manager chose interface "
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
            cerr << "WARNING:	IDU_net_manager could not find the requested"
                 << " interface:  " << interface << endl;
            cerr << "	Using " << valid[count].interface << " instead." << endl;
            }
         else
            {
            cerr << "ERROR:	IDU_net_manager could not find the requested"
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
      if ( network_mode == IDU_BCAST_MODE )
         bcopy((char *) &(valid[count].dest),
               (char *) &(dest),
               sizeof(ifr->ifr_broadaddr));

#ifdef DATA
      cerr << "DATA: IDU_net_manager interface = " << interface << endl;
      cerr << "DATA: IDU_net_manager interface inet = "
           << valid[count].ascii_inet_addr << endl;
      if ( network_mode == IDU_BCAST_MODE )
         cerr << "DATA:\tIDU_net_manager broadcast address = " 
              << inet_ntoa(dest.sin_addr) << endl;
#endif

      }

   /* Bind port numbers to sockets */
   sin_send.sin_family = AF_INET;
   if ( network_mode == IDU_BCAST_MODE )
      sin_send.sin_addr.s_addr = net_inet_addr.s_addr;
   else
      {
      if(setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_IF,&(net_inet_addr),
                    sizeof(IN_ADDR_SIZE)) < 0 )
         {
         perror("ERROR:	IDU_net_manager unable to assign multicast interface -\n   ");
         return (FALSE);
         }
      sin_send.sin_addr.s_addr = inet_addr(mc_group);
      } 
   sin_send.sin_port = htons(0);
   if ( network_mode == IDU_BCAST_MODE )
      {
      if (bind(sock_send, (struct sockaddr *) &sin_send, sizeof(sin_send)) < 0)
         {
         perror("ERROR:    IDU_net_manager unable to bind send socket port-\n   ");
         return (FALSE);
         }
      }

   sin_send.sin_port = htons(port);

   if ( network_mode == IDU_BCAST_MODE )
      {
      bcopy((char *) &(dest.sin_addr), (char *)&(sin_send.sin_addr),
            IN_ADDR_SIZE);
      }

   sin_recv.sin_family = AF_INET;
   if ( network_mode == IDU_BCAST_MODE )
      sin_recv.sin_addr.s_addr = dest.sin_addr.s_addr;
   else if ( network_mode == IDU_MCAST_MODE )
      {
      sin_recv.sin_addr.s_addr = INADDR_ANY;
      }
   sin_recv.sin_port = htons(port);
   if ( network_mode == IDU_MCAST_MODE )
      {
      mreq.imr_multiaddr.s_addr = inet_addr(mc_group);
      mreq.imr_interface.s_addr = net_inet_addr.s_addr;
      if (setsockopt(sock_recv,IPPROTO_IP,IP_ADD_MEMBERSHIP,
                     &mreq,sizeof(mreq)) < 0)
         {
         perror("ERROR:	IDU_net_manager unable to join multicast group -\n   ");
         return (FALSE);
         }
      net_inet_addr.s_addr = inet_addr(mc_group);
      }

   if ( setsockopt(sock_recv, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
      {
      perror("ERROR:	IDU_net_manager unable to reuse port address -\n   ");
      return(FALSE);
      }
   if (bind(sock_recv, (struct sockaddr *) &sin_recv, sizeof(sin_recv)) < 0)
      {
      perror("ERROR:	IDU_net_manager unable to bind receive socket port -\n   ");
      return (FALSE);
      }


   return (TRUE);

   }


int
IDU_net_manager::configure (const int local_loopback)
   {

   /* Create the arena for shared memory variables */
   if ( init_arena() )
      {
      up_and_running = TRUE;
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager arena created." << endl;
#endif
       }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager failed to initialize arena." << endl;
#endif
      return (FALSE);
      }

   if ( allocate_shared_memory() )
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager shared memory allocated." << endl;
#endif
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager unable to allocate shared memory." << endl;
#endif
      up_and_running = FALSE;
      return (FALSE);
      }

#ifdef DATA
   cerr << "DATA:       IDU_net_manager 'current_net_buffer' = "
        << *current_net_buffer << endl;
   cerr << "DATA:       IDU_net_manager 'net_read_flag' = "
        << *net_read_flag << endl;
#endif

   *loopback = (u_char)local_loopback;

   if ( init_sockets() )
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager sockets opened and initialized." << endl;
#endif
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager failed to open/initialize sockets." << endl;
#endif
      up_and_running = FALSE;
      return (FALSE);
      }

   consumer_pid = int(getpid());

#ifdef DEBUG
   cerr << "DEBUG:      IDU_net_manager parent process id = "
        << consumer_pid << endl;
#endif

   signal ( SIGCLD, SIG_IGN );

   producer_pid = sproc ( idu_receiver_process, PR_SALL, (void *)this );
   if ( producer_pid == -1 )
      {
      perror("ERROR:    IDU_net_manager unable to sproc receiver process-\n   ");
      up_and_running = FALSE;
      return(FALSE);
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:   IDU_net_manager receiver process id = "
           << producer_pid << endl;
#endif
      }

   return (TRUE);
   }


/* Broadcast constructor */
IDU_net_manager::IDU_net_manager ( const unsigned short receive_port,
                                   char *net_interface,
                                   const int local_loopback,
                                   const int buffer_length )
   {

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager Broadcast constructor started." << endl;
#endif

   init_member_vars();

   network_mode = IDU_BCAST_MODE;

   /* Initialize member variables from data passed in by user parameters */
   max_idus = buffer_length;
   if ( net_interface != NULL )
      strcpy ( interface, net_interface );
   else
      strcpy ( interface, "" );
   port = receive_port;

#ifdef DATA
   cerr << "DATA:	IDU_net_manager 'max_idus' = " << max_idus << endl;
   cerr << "DATA:	IDU_net_manager 'interface' = " << interface
        << endl;
   cerr << "DATA:	IDU_net_manager 'port' = " << port << endl;
#endif

   up_and_running = configure(local_loopback);

   if ( up_and_running && (net_interface != NULL) )
      strcpy(net_interface,interface);

#ifdef TRACE
      cerr << "TRACE:	IDU_net_manager Broadcast constructor finished." << endl;
#endif

   }


/* Multicast constructor */
IDU_net_manager::IDU_net_manager ( const char *group,
                                   const unsigned short receive_port,
                                   const unsigned char ttl,
                                   char *net_interface,
                                   const int local_loopback,
                                   const int buffer_length )

   {

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager Multicast constructor started." << endl;
#endif

   init_member_vars();

   network_mode = IDU_MCAST_MODE;

   /* Initialize member variables from data passed in by user parameters */
   max_idus = buffer_length;
   if ( net_interface != NULL )
      strcpy ( interface, net_interface );
   else
      strcpy ( interface, "" );
   port = receive_port;
   if ( group != NULL )
      strcpy ( mc_group, group );
   else
      strcpy ( mc_group, IDU_DEF_MC_GROUP );
   mc_ttl = ttl;

#ifdef DATA
   cerr << "DATA:	IDU_net_manager 'max_idus' = " << max_idus << endl;
   cerr << "DATA:	IDU_net_manager 'interface' = " << interface
        << endl;
   cerr << "DATA:	IDU_net_manager 'port' = " << port << endl;
   cerr << "DATA:	IDU_net_manager 'mc_group' = " << mc_group << endl;
   cerr << "DATA:	IDU_net_manager 'mc_ttl' = " << int(mc_ttl) << endl;
#endif

   up_and_running = configure(local_loopback);

   if ( up_and_running && (net_interface != NULL) )
      strcpy(net_interface,interface);

#ifdef TRACE
      cerr << "TRACE:\tIDU_net_manager Multicast constructor finished." << endl;
#endif

   }


void
IDU_net_manager::net_close()
   {

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager close started." << endl;
#endif

   if ( first_obj_read )
      {
      first_obj_read = FALSE;
      if (usvsema(start_reading_sema) == -1)
         {
         perror("ERROR:	IDU_net_manager unable to release reading sema -\n   ");
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
      if ( buffer[0].idu != NULL )
         {
         usfree ( buffer[0].idu, netarena );
         buffer[0].idu = NULL;
         }
      if ( buffer[1].idu != NULL )
         {
         usfree ( buffer[1].idu, netarena );
         buffer[1].idu = NULL;
         }
      usfree ( (idu_buffer *)buffer, netarena );
      buffer = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager memory deallocated." << endl;
#endif
      }

   if ( swap_buf_sema != NULL )
      {
      usfreesema ( swap_buf_sema, netarena ); 
      swap_buf_sema = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager swap semaphore memory deallocated." << endl;
#endif
      }

   if ( start_reading_sema != NULL )
      {
      usfreesema ( start_reading_sema, netarena );
      start_reading_sema = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager reading semaphore memory deallocated." 
           << endl;
#endif
      }

   if ( has_data != NULL )
      {
      usfreelock ( has_data, netarena );
      has_data = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager data lock memory deallocated." << endl;
#endif
      }

   if ( net_read_flag != NULL )
      {
      usfree ( (int *)net_read_flag, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager net_read_flag deallocated." << endl;
#endif
      net_read_flag = NULL;
      }

   if ( receive_all != NULL )
      {
      usfree ( (int *)receive_all, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager receive all flag deallocated." << endl;
#endif
      receive_all = NULL;
      }

   if ( receive_list != NULL )
      {
      usfree ( (unsigned char *)receive_list, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager receive list deallocated." << endl;
#endif
      receive_list = NULL;
      }

   
   if ( current_net_buffer != NULL )
      {
      usfree ( (int *)current_net_buffer, netarena );
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager net buffer flag deallocated." << endl;
#endif  
      current_net_buffer = NULL;
      }

   if ( loopback != NULL )
      {
      usfree ( (u_char *)loopback, netarena );
#ifdef DEBUG
      cerr << "DEBUG:\tIDU_net_manager loopback flag deallocated." << endl;
#endif
      loopback = NULL;
      }

   if ( sock_send != -1 )
      {
      close(sock_send);
      sock_send = -1;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager send socket closed." << endl;
#endif
      }
   if ( sock_recv != -1 )
      {
      close(sock_recv);
      sock_recv = -1;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager receive socket closed." << endl;
#endif
      }

   if ( netarena != NULL )
      {
      usdetach(netarena);
      netarena = NULL;
#ifdef DEBUG
      cerr << "DEBUG:	IDU_net_manager detached arena." << endl;
#endif
      }

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager close finished." << endl;
#endif

   }


/* Destructor */
IDU_net_manager::~IDU_net_manager()
   {
#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager destructor started." << endl;
#endif

   this->net_close();

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager destructor finished." << endl;
#endif

   }


int
IDU_net_manager::net_open ()
   {
   return up_and_running;
   }


int
IDU_net_manager::read_idu ( char **aidu, 
                            IDUType *atype, 
                            sender_info &sender,
                            int &swapped_buffers  )
   {

   volatile idu_buffer	*my_buffer;
   volatile idu_buffer	*other_buffer;
   union IDU	*next_idu;
   IDUHeader	*next_header;
   InfoIDU	*next_info;

   if ( first_obj_read )
      {
      first_obj_read = FALSE;
      if (usvsema(start_reading_sema) == -1)
         {
         perror("ERROR:	IDU_net_manager unable to release reading sema -\n   ");
         return FALSE;
         }
      }

   swapped_buffers = FALSE;
   my_buffer = buffer + !(*current_net_buffer); /* only works for two buffers */

   if ( (my_buffer->unread_idus <= 0 ) && ustestlock(has_data) )
      {
      if (uspsema(swap_buf_sema) == -1)
         {
         perror("ERROR: IDU_net_manager read_idu can't grab semaphore -\n   ");
         }
      else
         {
         other_buffer = buffer + (*current_net_buffer);
         my_buffer->current_idu = 0;
         my_buffer->over_written_idus = 0;
         my_buffer->over_write_mode = FALSE;
         my_buffer->unread_idus = 0;
         if ( usunsetlock(has_data) == -1 )
            {
            perror("ERROR:	IDU_net_manager unable to unset lock -\n   ");
            }
         my_buffer = other_buffer;
         *current_net_buffer = !(*current_net_buffer);
         if ( !my_buffer->over_write_mode )
            my_buffer->current_idu = 0;
         else
            {
            if ( my_buffer->current_idu >= max_idus )
               my_buffer->current_idu = 0;
            }
#ifdef CONSUMER_DEBUG
         cerr << "DEBUG:	IDU_net_manager swapped buffers." << endl;
         cerr << "DEBUG:	IDU_net_manager " << my_buffer->unread_idus
              << " acquired this swap." << endl;
#endif
         swapped_buffers = TRUE;
         if ( usvsema(swap_buf_sema) == -1 )
            {
            perror("ERROR:	IDU_net_manager read_idu can't release semaphore -\n   ");
            }
         }
      }

/* DO NOT MAKE THIS AN ELSE */

   if ( my_buffer->unread_idus > 0 )
      {
      next_idu = (my_buffer->idu + my_buffer->current_idu);
      next_header = (IDUHeader *)next_idu;
      next_info = (InfoIDU *)next_idu;
      *aidu = (char *)next_idu;
      *atype = next_header->type;
      bcopy ( (char *)&next_info->sender, (char *)&sender, sizeof(sender_info));
      my_buffer->current_idu = (my_buffer->current_idu + 1) % max_idus;
      my_buffer->unread_idus--;
      return (my_buffer->unread_idus + 1);
      }
   else
      {
      *aidu = NULL;
      *atype = 0;
      return 0;
      }

   }


void
IDU_net_manager::dump_idu ( char *idu, IDUType type )
{
   int                          length;
   IDUHeader                    *header;
   LargestIDU			*unformatted;
   char 			*current_byte;
   int				count;
   char				byte_string[4];

   /* Fill in header data */
   header = (IDUHeader *)idu;
   unformatted = (LargestIDU *)idu;

   /* calculate length */
   switch ( type )
      {
      case Test_Type:
         length = sizeof(TestIDU);
         break;
      default:
         length = sizeof(union IDU);
         break;
      }

   count = 0;
   current_byte = idu;
   while ( count < length )
      {
      sprintf(byte_string, "%3d", (int)*current_byte);
      cout << byte_string << " ";
      if ( (++count % 16) == 0 )
         cout << endl;
      current_byte++;
      }

   cout << endl;
}

   
int
IDU_net_manager::write_idu (char *idu, IDUType type)
{
   int                          length, bytes_sent;
   IDUHeader                    *header;
   int				sendcount;

   /* Fill in header data */
   header = (IDUHeader *) idu;
   header->type = type;

   /* calculate length */
   switch ( type )
      {
      case Test_Type:
         length = sizeof(TestIDU);
         break;
      case SS_To_Jack_Type:
         length = sizeof(SSToJackIDU);
         break;
      case Jack_To_SS_Type:
         length = sizeof(JackToSSIDU);
         break;
#ifndef NOSUB
      case NPSNET_To_SUB_Type:
         length = sizeof(NPSNETToSubIDU); 
         break;
      case SUB_Helm_To_NPSNET_Type:
         length = sizeof(HelmToNPSNETIDU); 
         break;
      case SUB_Ood_To_NPSNET_Type:
         length = sizeof(SubOodToNPSNETIDU);  
         length = sizeof(SubOodToNPSNETIDU);  
         break;
      case SUB_Weaps_To_NPSNET_Type:
         length = sizeof(WeapsToNPSNETIDU); 
         break;
#endif // NOSUB
      case NPSNET_To_SHIP_Type:
         length = sizeof(NPSNETToShipIDU); //NEW
         break;
      case SHIP_Ood_To_NPSNET_Type:
         length = sizeof(OodToNPSNETIDU);  //NEW
         break;
      default:
         length = sizeof(union IDU);
         break;
      }
   header->length = length;


   /* Actually send it */
   sendcount = 0;
   while ( (sendcount++ < IDU_NUM_SEND_TRIES ) &&
          ((bytes_sent = sendto(sock_send, idu, length, 0,
                                &sin_send, sizeof(sin_send))) < 0) )
      {
      perror("ERROR:\tIDU_net_manager sendto failed - \n   ");
      }
   if ( sendcount > 1 )
      {
      if ( sendcount <= IDU_NUM_SEND_TRIES )
         cerr << "WARNING:\tIDU_net_manager sent IDU on try # " << (sendcount-1)
              << endl;
      else
         {
         cerr << "ERROR:\tIDU_net_manager failed to send IDU after "
              << (sendcount-1) << " tries." << endl;
         return (FALSE);
         }
      }


   return (TRUE);
}


int
IDU_net_manager::enable_loopback ()
   {
   if ( loopback != NULL )
      {
      *loopback = (u_char)TRUE;
      if ( network_mode == IDU_MCAST_MODE )
         {
         if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_LOOP,
                        (void *)loopback,sizeof(u_char)) < 0)
            {
            perror("ERROR:\tIDU_net_manager unable to configure loopback -\n\t");
            return FALSE;
            }
         }
      return TRUE;
      }
   else
      return FALSE;
   }


int
IDU_net_manager::disable_loopback ()
   {
   if ( loopback != NULL )
      {
      *loopback = (u_char)FALSE;
      if ( network_mode == IDU_MCAST_MODE )
         {
         if (setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_LOOP,
                        (void *)loopback,sizeof(u_char)) < 0)
            {
            perror("ERROR:\tIDU_net_manager unable to configure loopback -\n\t");
            return(FALSE);
            }
         }
      return TRUE;
      }
   else
      return FALSE;
   }


int
IDU_net_manager::receive_all_idus ()
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
IDU_net_manager::receive_no_idus ()
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
IDU_net_manager::add_to_receive_list ( IDUType idutype )
   {
   if ( receive_all != NULL )
      *receive_all = FALSE;
   else
      return FALSE;
   if ( receive_list != NULL )
      receive_list[idutype] = TRUE;
   else 
      return FALSE;
   return TRUE;
   }


int
IDU_net_manager::remove_from_receive_list ( IDUType idutype )
   {
   if ( receive_all != NULL )
      *receive_all = FALSE;
   else
      return FALSE;
   if ( receive_list != NULL )
      receive_list[idutype] = FALSE;
   else
      return FALSE;
   return TRUE;
   }



static void
signal_handler(int sig, ...)
   {
   if ((signal ( sig, SIG_IGN )) == SIG_ERR)
      perror("IDU_net_manager:\tSignal Error -\n   ");
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
            cerr << "\tIDU_net_manager terminating to prevent orphan process."
                 << endl;
            exit(0);
            }
         break;
      case SIGINT:
         cerr << "DEATH NOTICE:	IDU_net_manager exitting due to interrupt "
              << "signal." << endl;
         exit(0);
         break;
      case SIGQUIT:
         cerr << "DEATH NOTICE:	IDU_net_manager exitting due to quit "
              << "signal." << endl;
         exit(0);
         break;
      default:
         cerr << "DEATH NOTICE:	IDU_net_manager exitting due to signal:  "
              << sig << endl;
         exit(0);
         break;
      }
   signal ( sig, (void (*)(int))signal_handler );
   }


void
idu_receiver_process ( void *tempthis )
   {
   IDU_net_manager	*obj;
   struct sockaddr_in   from;
   union IDU            input_buf;
   int			len = sizeof(from);
   int			bytes_read;
   extern int		errno;
   int			good_read;
   IDUHeader            *header = (IDUHeader *) &input_buf;
   InfoIDU		*info_idu = (InfoIDU *) &input_buf;
   InfoIDU		*info_dest;
   volatile idu_buffer	*my_buffer;
   struct hostent	*entity;
#ifdef VERIFICATION
   int			print_error = FALSE;
#endif
   static               void (*temp_signal)(int);


#ifdef STATS
   long			packets_read = 0;
#endif

   obj = (IDU_net_manager *)tempthis;

   /* The following call is necessary for this child process to actually */
   /* respond to SIGHUP signals when the parent process dies.            */
   prctl(PR_TERMCHILD);
   if ((signal ( SIGTERM, (void (*)(int))signal_handler )) == SIG_ERR)
            perror("IDU_net_manager:\tError setting SIGTERM handler -\n   ");
   if ((signal ( SIGHUP, (void (*)(int))signal_handler )) == SIG_ERR)
            perror("IDU_net_manager:\tError setting SIGHUP handler -\n   ");
   if ( (temp_signal = signal ( SIGINT, (void (*)(int))signal_handler )) 
      != SIG_DFL )
      {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("IDU_net_manager:\tError setting SIGINT ignore -\n   ");
#ifdef DEBUG
      cerr << "*** IDU_net_manager process ignoring interupts." << endl;
#endif
      }
   if ((signal ( SIGQUIT, (void (*)(int))signal_handler )) == SIG_ERR)
      perror("IDU_net_manager:\tError setting SIGQUIT handler -\n   ");

   if (uspsema(obj->start_reading_sema) == -1)
      {
      perror("ERROR:	IDU_net_manager unable to initially get reading sema -\n   ");
      cerr << "ERROR:	IDU_net_manager receiver process is not starting." << endl;
      exit(0);
      }

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager receiver_process started." << endl;
#endif

#ifdef VERIFICATION

   cerr << "VERIFICATION: IDU_net_manager verifying IDU size." << endl;

#endif

   while ( *(obj->net_read_flag) )
      {

      /* Read packet off network into temp buffer */
      /*   Net read must be done while semaphore is released to prevent */
      /*   hanging application while no packets are incoming */

      if ((bytes_read = recvfrom(obj->sock_recv, (void *) &input_buf,
              sizeof(union IDU), 0, (struct sockaddr *) &from, &len)) == -1)
         {
         if ( (errno != EINTR) || *(obj->net_read_flag) )
            {
            perror("ERROR:	IDU_net_manager receiver process, recvfrom failed -\n   ");
            if ( errno == EINTR )
               cerr << "WARNING:	IDU_net_manager interrupted but not "
                    << "quitting because net_read_flag is still true." << endl;
            else
               cerr << "   ERRNO:  " << errno  << ", net_read_flag = "
                    << *(obj->net_read_flag) << endl;
            }
         good_read = FALSE;
         }
      else
         good_read = TRUE;



      if ( good_read &&
           (obj->network_mode == IDU_BCAST_MODE) &&
           !(*obj->loopback) && 
           (bcmp((void *) &(from.sin_addr), (void *) &(obj->net_inet_addr),
                                                       IN_ADDR_SIZE) == 0) )
         {
         continue;
         }
 

      if ( good_read )
         {
/* commented out for performance reasons 
         entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
*/
         strcpy ( info_idu->sender.internet_address, 
                  inet_ntoa(from.sin_addr) );
/*
         strcpy ( info_idu->sender.host_name, 
                  entity->h_name );
*/
         info_idu->sender.host_name[0] = '\0';
         }

      if ( good_read && !*obj->receive_all )
         {
         if ( !obj->receive_list[int(header->type)] )
            {
#ifdef DEBUG
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "DEBUG:	Receive List Discarding idu of type " 
                 << int(header->type) << " from "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
#endif
            continue;
            }
         }


#ifdef VERIFICATION
      if ( good_read )
         {
         print_error = FALSE;
         switch ( header->type )
            {
            case Test_Type:
               if ( bytes_read != sizeof(TestIDU) )
                  {
                  cerr << "WARNING:\tIDU_net_manager TestIDU not the correct";
                  cerr << " size of " << sizeof(TestIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            case SS_To_Jack_Type:
               if ( bytes_read != sizeof(SSToJackIDU) )
                  {
                  cerr << "WARNING:\tIDU_net_manager SSToJackIDU not the correct";
                  cerr << " size of " << sizeof(SSToJackIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            case Jack_To_SS_Type:
               if ( bytes_read != sizeof(JackToSSIDU) )
                  {
                  cerr << "WARNING:\tIDU_net_manager JackToSSIDU not the correct";
                  cerr << " size of " << sizeof(JackToSSIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
#ifndef NOSUB
            case NPSNET_To_SUB_Type:
               if ( bytes_read != sizeof(NPSNETToSubIDU) ) 
                  {
                  cerr << "WARNING:\tIDU_net_manager NPSNETToSubIDU not the correct";
                  cerr << " size of " << sizeof(NPSNETToSubIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
             case SUB_Helm_To_NPSNET_Type:
               if ( bytes_read != sizeof(HelmToNPSNETIDU) ) 
                  {
                  cerr << "WARNING:\tIDU_net_manager HelmToNPSNETIDU not the correct";
                  cerr << " size of " << sizeof(HelmToNPSNETIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            case SUB_Ood_To_NPSNET_Type:
               if ( bytes_read != sizeof(OodToNPSNETIDU) ) 
                  {
                  cerr << "WARNING:\tIDU_net_manager OodToNPSNETIDU not the correct";
                  cerr << " size of " << sizeof(OodToNPSNETIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            case SUB_Weaps_To_NPSNET_Type:
               if ( bytes_read != sizeof(WeapsToNPSNETIDU) )  
                  {
                  cerr << "WARNING:\tIDU_net_manager WeapsToNPSNETIDU not the correct";
                  cerr << " size of " << sizeof(WeapsToNPSNETIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break; 
#endif // NOSUB
                  
            case NPSNET_To_SHIP_Type:
               if ( bytes_read != sizeof(NPSNETToShipIDU) )  //NEW
                  {
                  cerr << "WARNING:\tIDU_net_manager NPSNETToShipIDU not the correct";
                  cerr << " size of " << sizeof(NPSNETToShipIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            case SHIP_Ood_To_NPSNET_Type:
               if ( bytes_read != sizeof(OodToNPSNETIDU) )  //NEW
                  {
                  cerr << "WARNING:\tIDU_net_manager OodToNPSNETIDU not the correct";
                  cerr << " size of " << sizeof(OodToNPSNETIDU) << " bytes." << endl;
                  print_error = TRUE;
                  }
               break;
            default:
               break;
            }
   
         if ( int(header->length) != bytes_read )
               {
               cerr << "WARNING:\tIDU_net_manager specified length of "
                    << "incoming IDU is not equal to actual bytes read."
                    << endl;
               print_error = TRUE;
               }
   
         if ( print_error )
            {
            cerr << "	Actual bytes read = " << bytes_read << endl;
            cerr << "	Bytes specified in IDU header = "
                 << int(header->length) << endl;
            entity = gethostbyaddr ( (void *)&from.sin_addr,
                                     sizeof(struct in_addr), AF_INET );
            cerr << "	Packet came from:  "
                 << inet_ntoa(from.sin_addr) << " " << entity->h_name  << endl;
            }
   
         }
#endif

      if ( good_read && (header->length > sizeof(LargestIDU) ) )
         {
         cerr << "ERROR:	IDU_net_manager ignored IDU because "
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
#ifdef STATS
         packets_read++;
#endif
#ifdef PACKET
         cerr << "PACKET:	IDU_net_manager got a packet of type "
              << int(header->type) << endl;
#endif
 
         if (uspsema(obj->swap_buf_sema) == -1)
            {
            perror("ERROR:	IDU_net_manager can't grab semaphore -\n   ");
            }
         else
            {
            my_buffer = obj->buffer + *(obj->current_net_buffer);
            if ( my_buffer->current_idu >= obj->max_idus )
               {
               my_buffer->over_write_mode = TRUE;
               my_buffer->current_idu = 0;
#ifdef DEBUG
               cerr << "DEBUG:	IDU_net_manager overflow occured." << endl;
               cerr << "	The net manager is receiving packets faster "
                    << "than the application is processing them." << endl;
               cerr << "	Either increase the buffer size or call "
                    << "read_idu faster." << endl;
#endif
               }

            bcopy ( (void *) &input_buf, 
                    (void *) &(my_buffer->idu[my_buffer->current_idu]),
                    header->length );
            info_dest = (InfoIDU *) &(my_buffer->idu[my_buffer->current_idu]);
            bcopy ( (void *) &info_idu->sender,
                    (void *) &info_dest->sender,
                    sizeof(sender_info) );
            my_buffer->current_idu++;
            if ( my_buffer->over_write_mode )
               my_buffer->over_written_idus++;
            else
               my_buffer->unread_idus++;

            if (!ustestlock(obj->has_data))
               ussetlock(obj->has_data);
           
            if ( usvsema(obj->swap_buf_sema) == -1 )
               {
               perror("ERROR:	IDU_net_manager can't release semaphore -\n   ");
               } 

            }

         }

      }

#ifdef TRACE
   cerr << "TRACE:	IDU_net_manager receiver_process finished." << endl;
#endif

#ifdef STATS
   cerr << "STATS:	IDU_net_manager receiver_process got "
        << packets_read << " packets." << endl;
#endif

   *(obj->net_read_flag) = TRUE;
   exit(0);
   }

