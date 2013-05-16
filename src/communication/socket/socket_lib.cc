// File: <socket_lib.cc>

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


//************************************************************************
//
//      /n/bossie/work2/mcmahan/thesis/panel/socket_lib.c
//
//************************************************************************

/* File:	client_lib.c
 * Description:	Client communications library - DIS package
 * Revision:	3.0 - 13Jul94
 *
 * Reference:	Military Standard--Protocol Data Units for Entity
 *		Information and Entity Interaction in a Distributed
 *		Interactive Simulation (DIS) (30Oct91)
 *
 * Author:	John Locke, Paul Barham
 *		CS Department, Naval Postgraduate School
 * Internet:	jxxl@cs.nps.navy.mil
 *		barham@cs.nps.navy.mil
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <bstring.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sysmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/soioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ulocks.h>
#include <signal.h>
#include <fcntl.h>

#include "socket_struct.h"
#include "socket_lib.h"

// Networking globals
int			length = sizeof(GUI_MSG_DATA); // size of the GUI_MSG_DATA structure
int			sock_send;
static int		sock_recv;
struct sockaddr_in	dest;
static char		my_in_addr[IN_ADDR_SIZE];
unsigned short		host_id;

// Arena globals
usptr_t			*netarena = NULL;	// arena handle
#define BUF1		0
#define BUF2		1
typedef struct {
   int			receiver_pid;
   int			application_pid;
   //
   int			current_net_buf;
			  // PDUs from net go into current buffer; the
			  //   application reads PDUs from the other buf
   usema_t		*swap_buf_sema;	// control access to swap_buf_flag
   int			swap_buf_flag;
   //
   int			buf1_oldest;
   int			buf1_newest;
   int			buf1_wasted;	// overwritten PDUs
   int			nodes_in_buf1;
   GUI_MSG_DATA		*pdu_buf1;	// PDU array; size selected through
					//   arg to socket_open()
   //
   int			buf2_oldest;
   int			buf2_newest;
   int			buf2_wasted;	// overwritten PDUs
   int			nodes_in_buf2;
   GUI_MSG_DATA		*pdu_buf2;
} PDUarena;
PDUarena		*PDUarena_ptr;
int			pdus_in_buffer;

int socket_open(char *interf, unsigned short send_port,
                unsigned short recv_port, int buf_len)
{
   int			i, on = 1;
   int			valid_interface = FALSE;
   char			buf[BUFSIZ];
   struct sockaddr_in	sin_send, sin_recv, *in_addr;
   struct ifconf	ifc;
   struct ifreq		*ifr;

   // Set ua arena
   pdus_in_buffer = buf_len;
   init_arena();

   // Create sockets
   if ((sock_send = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket(sock_send)");
      return (FALSE);
   }
   if ((sock_recv = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket(sock_recv)");
      return (FALSE);
   }
#ifdef DEBUG2
   printf("socket_open(): sock_recv = %d\n", sock_recv);
   fflush(stdout);
#endif

   // Mark send socket for broadcasting
   if (setsockopt(sock_send, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) == -1) {
      perror("setsockopt(SO_BROADCAST)");
      return (FALSE);
   }

#ifdef NONBLOCKING
   // Mark receive socket for non-blocking; CPU pig
   if (fcntl(sock_recv, F_SETFL, FNDELAY) < 0) {
      perror("fcntl(F_SETFL,FNDELAY)");
      return (FALSE);
   }
#endif // NONBLOCKING

   // Bind port numbers to sockets
   sin_send.sin_family = AF_INET;
   sin_send.sin_addr.s_addr = htonl(INADDR_ANY);
   sin_send.sin_port = htons(send_port);
   if (bind(sock_send, (struct sockaddr *) &sin_send, sizeof(sin_send)) == -1) {
      perror("bind(sock_send)");
      return (FALSE);
   }
   sin_recv.sin_family = AF_INET;
   sin_recv.sin_addr.s_addr = htonl(INADDR_ANY);
   sin_recv.sin_port = htons(recv_port);
   if (bind(sock_recv, (struct sockaddr *) &sin_recv, sizeof(sin_recv)) == -1) {
      perror("bind(sock_recv)");
      return (FALSE);
   }

   // Get InterFace CONFig
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock_send, SIOCGIFCONF, (char *) &ifc) == -1) {
      perror("ioctl(SIOCGIFCONF)");
      return (FALSE);
   }

   // Check interfaces (assumes same interface for sender and receiver)
   ifr = ifc.ifc_req;		// ptr to interface structure
   for (i = 0; i < (ifc.ifc_len / sizeof(struct ifreq)); i++, ifr++) {

      if (ifr->ifr_addr.sa_family != AF_INET)	// Internet only
         continue;

      // Get InterFace FLAGS
      if (ioctl(sock_send, SIOCGIFFLAGS, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFFLAGS)");
         return (FALSE);
      }

      // Skip boring cases
      if ((ifr->ifr_flags & IFF_UP) == 0 ||		// interface down?
          (ifr->ifr_flags & IFF_LOOPBACK) ||		// local loopback?
          (ifr->ifr_flags & IFF_BROADCAST) == 0) {	// no broadcast?
          continue;
      }

      // Get and save InterFace ADDRess
      if (ioctl(sock_send, SIOCGIFADDR, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFADDR)");
         return (FALSE);
      }
      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);
      bcopy((char *) &(in_addr->sin_addr), my_in_addr, IN_ADDR_SIZE);
      assign_host_id();

      // Get InterFace BRoaDcast ADDRess
      if (ioctl(sock_send, SIOCGIFBRDADDR, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFBRDADDR)");
         return (FALSE);
      }
      bcopy((char *) &(ifr->ifr_broadaddr), (char *) &dest,
                                             sizeof(ifr->ifr_broadaddr));

      // Assign destination port
      dest.sin_port = htons(UDP_RECV_PORT);

#ifdef DEBUG2
      {
      char *cptr;
      cptr = (char *) &dest;
      for (int j = 0; j < sizeof(dest); j++)
         printf("%03d ", (unsigned char) cptr[j]);
      printf("\n");
      }
#endif

      // Interface we want?
      if (strcmp(interf, ifr->ifr_name) == 0) {
         valid_interface = TRUE;
      }
   }

   if (valid_interface == FALSE) {
      return (FALSE);
   }

   // Save PID
   //PDUarena_ptr->application_pid = getpid();

   // Spawn the receiving process
   if ((PDUarena_ptr->receiver_pid = sproc(areceiver_process, PR_SALL)) == -1)  {
      perror("sproc()");
      return (FALSE);
   }
#ifdef DEBUG2
   printf("PDUarena_ptr->receiver_pid = %d\n", PDUarena_ptr->receiver_pid);
   fflush(stdout);
#endif

   signal(SIGCLD,SIG_IGN);

   return (TRUE);
}


void init_arena()
{
   int		i = 0;
   int		arenasize, fudge;
   char		arenaname[64];

#ifdef DEBUG2
   printf("Enter init_arena()\n");
   fflush(stdout);
#endif

   // Set arena size
   fudge = (1024 * 10);	// fudge factor
   arenasize =
         (2 * pdus_in_buffer * sizeof(GUI_MSG_DATA)) + sizeof(PDUarena) + fudge;
   if (usconfig(CONF_INITSIZE, arenasize) == -1)  {
      perror("usconfig(CONF_INITSIZE)");
   }

   // Set arena to be shared by parent and child only
   if (usconfig(CONF_ARENATYPE, US_SHAREDONLY) == -1)  {
       perror("usconfig(CONF_ARENATYPE)");
   }

   // Name and open arena
   do {
      sprintf(arenaname, "/usr/tmp/DIS-3.0.arena.%1d", i);
      if ((netarena = usinit(arenaname)) == NULL)  {
         perror("usinit()");
      }
      i++;
   } while (!netarena);
#ifdef DEBUG2
   printf("arenaname = %s\n", arenaname);
   fflush(stdout);
#endif

   // malloc root data structure
   if ((PDUarena_ptr =
              (PDUarena *) usmalloc(sizeof(PDUarena), netarena)) == NULL) {
      perror("usmalloc(PDUarena)");
   }

   // malloc PDU buffers
   if ((PDUarena_ptr->pdu_buf1 = (GUI_MSG_DATA *)
           usmalloc((sizeof(GUI_MSG_DATA)*pdus_in_buffer), netarena)) == NULL) {
      perror("usmalloc(buf1)");
   }
   if ((PDUarena_ptr->pdu_buf2 = (GUI_MSG_DATA *)
           usmalloc((sizeof(GUI_MSG_DATA)*pdus_in_buffer), netarena)) == NULL) {
      perror("usmalloc(buf2)");
   }

   // Init semaphore
   if ((PDUarena_ptr->swap_buf_sema = usnewsema(netarena, 1)) == NULL) {
      perror("usnewsema(swap_buf_sema)");
   }

   // Data initializations
   PDUarena_ptr->current_net_buf = BUF1;
   PDUarena_ptr->swap_buf_flag = FALSE;
   PDUarena_ptr->buf1_oldest = 0;
   PDUarena_ptr->buf1_newest = 0;
   PDUarena_ptr->nodes_in_buf1 = 0;
   PDUarena_ptr->buf2_oldest = 0;
   PDUarena_ptr->buf2_newest = 0;
   PDUarena_ptr->nodes_in_buf2 = 0;

} // init_arena()


void swap_net_buffers()
{
   // Simply switch current buffer var
   if (uspsema(PDUarena_ptr->swap_buf_sema) == 1) { // acquire semaphore
#ifdef DEBUG2
      printf("Swapping buffers\n");
      fflush(stdout);
#endif
      if (PDUarena_ptr->current_net_buf == BUF1) {
         // switch to buf2
         PDUarena_ptr->current_net_buf = BUF2;
      } else {
         // switch to buf1
         PDUarena_ptr->current_net_buf = BUF1;
      }
      PDUarena_ptr->swap_buf_flag = TRUE;
   }
   usvsema(PDUarena_ptr->swap_buf_sema); // release semaphore
}


GUI_MSG_DATA *socket_read(struct readstat *rstat)
   // Return pointer to oldest currently available PDU; fill rstat with
   //   status on individual PDU and buffer state; request buffer swap
   //   when buffer is emptied or for other reasons
{
   GUI_MSG_DATA	*tmp_ptr;
   int did_first_swap = FALSE;

   // Force buffer swap
   if (rstat->rs_swap_buffers_flag == TRUE) {
      swap_net_buffers();
      did_first_swap = TRUE;
   }

   rstat->rs_swap_buffers_flag = FALSE;

   // Use the buffer not in use by areceiver_process()
   if (PDUarena_ptr->current_net_buf == BUF1) {
      // Any PDUs available?
      if (PDUarena_ptr->nodes_in_buf2 == 0) {
         // BUF2 empty
         swap_net_buffers();
         rstat->rs_swap_buffers_flag = TRUE;
         if (PDUarena_ptr->nodes_in_buf1 == 0) {
            // both buffers empty
            return (NULL);
         } else {
            // PDUs available in BUF1
            tmp_ptr = &(PDUarena_ptr->pdu_buf1[PDUarena_ptr->buf1_oldest]);
         }
      } else {
         // PDUs available in BUF2
         tmp_ptr = &(PDUarena_ptr->pdu_buf2[PDUarena_ptr->buf2_oldest]);
      }
   } else { // (PDUarena_ptr->current_net_buf == BUF2)
      // Any PDUs available?
      if (PDUarena_ptr->nodes_in_buf1 == 0) {
         // BUF1 empty
         swap_net_buffers();
         rstat->rs_swap_buffers_flag = TRUE;
         if (PDUarena_ptr->nodes_in_buf2 == 0) {
            // both buffers empty
            return (NULL);
         } else {
            // PDUs available in BUF2
            tmp_ptr = &(PDUarena_ptr->pdu_buf2[PDUarena_ptr->buf2_oldest]);
         }
      } else {
         // PDUs available in BUF1
         tmp_ptr = &(PDUarena_ptr->pdu_buf1[PDUarena_ptr->buf1_oldest]);
      }
   }

   // Now we have a ptr to buffer with nodes; update buffer state
   if (PDUarena_ptr->current_net_buf == BUF1) {
      rstat->rs_wasted = PDUarena_ptr->buf2_wasted;
      PDUarena_ptr->nodes_in_buf2--;
      if (PDUarena_ptr->nodes_in_buf2 == 0) {
         // plan ahead for next socket_read()
         swap_net_buffers();
         rstat->rs_swap_buffers_flag = TRUE;
      } else {
         // update buffer state
         PDUarena_ptr->buf2_oldest++;
         if (PDUarena_ptr->buf2_oldest > (pdus_in_buffer-1))
            // circle the buffer
            PDUarena_ptr->buf2_oldest = 0;
      }
   } else { // (PDUarena_ptr->current_net_buf == BUF2)
      rstat->rs_wasted = PDUarena_ptr->buf1_wasted;
      PDUarena_ptr->nodes_in_buf1--;
      if (PDUarena_ptr->nodes_in_buf1 == 0) {
         // plan ahead for next socket_read()
         swap_net_buffers();
         rstat->rs_swap_buffers_flag = TRUE;
      } else {
         // update buffer state
         PDUarena_ptr->buf1_oldest++;
         if (PDUarena_ptr->buf1_oldest > (pdus_in_buffer-1))
            // circle the buffer
            PDUarena_ptr->buf1_oldest = 0;
      }
   }

   if ( did_first_swap ) {
      rstat->rs_swap_buffers_flag = TRUE;
   }

   return (tmp_ptr);
}


void areceiver_process(void *)
{
   int			bytes_read;
   register int		buf_dimension;
   struct sockaddr_in	from;
   int			len = sizeof(from);
   int			oldest = 0, newest = 0;	// array indexes
   int			circled = FALSE;
   GUI_MSG_DATA		input_buf, *curr_buf;


   signal ( SIGTERM, SIG_DFL );

   // Initialize local vars
   buf_dimension = (pdus_in_buffer-1);
   if (uspsema(PDUarena_ptr->swap_buf_sema) == 1) { // acquire semaphore
      switch (PDUarena_ptr->current_net_buf) {
         // get pointer to the correct buffer
         case (BUF1):
            // pointer to current buffer; must be handled
            //   as local var (i.e. not in arena) in case application
            //   swaps buffers between reading the packet
            curr_buf = (GUI_MSG_DATA *) PDUarena_ptr->pdu_buf1;
            break;
         case (BUF2):
            curr_buf = (GUI_MSG_DATA *) PDUarena_ptr->pdu_buf2;
            break;
         default:
            // shouldn't get here
            printf("Invalid net buffer.\n");
            fflush(stdout);
      }
   }
   usvsema(PDUarena_ptr->swap_buf_sema); // release semaphore


   // NETWORK READ LOOP
   while (TRUE) {

      // Read packet off network into temp buffer
      //   Net read must be done while semaphore is released to prevent
      //   hanging application while no packets are incoming
      if ((bytes_read = recvfrom(sock_recv, (void *) &input_buf,
              sizeof(GUI_MSG_DATA), 0, (struct sockaddr *) &from, &len)) == -1) {
         perror("recvfrom()");
      }

      // Reject our own broadcasts
      if (bcmp((void *) &(from.sin_addr), (void *) my_in_addr,
                                                       IN_ADDR_SIZE) == 0) {
         continue;	// don't process packet further
      }

      // Save packet, update buffer indexes
      //   (this must be done after acquiring semaphore to block
      //    buffer swap by application)
      if (uspsema(PDUarena_ptr->swap_buf_sema) == 1) { // acquire semaphore
         // Swap buffers?
         if (PDUarena_ptr->swap_buf_flag == TRUE) {
            switch (PDUarena_ptr->current_net_buf) {	// BUF1 or BUF2
               // get pointer to the correct buffer
               case (BUF1):
                  curr_buf = (GUI_MSG_DATA *) PDUarena_ptr->pdu_buf1;
                  PDUarena_ptr->nodes_in_buf1 = 0;
                  PDUarena_ptr->buf1_wasted = 0;
                  break;
               case (BUF2):
                  curr_buf = (GUI_MSG_DATA *) PDUarena_ptr->pdu_buf2;
                  PDUarena_ptr->nodes_in_buf2 = 0;
                  PDUarena_ptr->buf2_wasted = 0;
                  break;
               default:
                  // booboo time
                  printf("Invalid net buffer.\n");
                  fflush(stdout);
            }
            // Buffers swapped so start at beginning of buf
            oldest = newest = 0;
            circled = FALSE;
            //
            PDUarena_ptr->swap_buf_flag = FALSE;
         }

         // Copy new packet to open slot in current buffer
         bcopy((void *) &input_buf, (void *) &(curr_buf[newest]), length);

#ifdef DEBUG2
         printPDU((char *) &(curr_buf[newest]));
         fflush(stdout);
#endif
#ifdef DEBUG2
         j++;
         printf("%d:oldest,newest = %d,%d\n", j, oldest, newest);
         fflush(stdout);
#endif

         // Save buffer state in case swap occurs
         if (PDUarena_ptr->current_net_buf == BUF1) {
            PDUarena_ptr->buf1_oldest = oldest;
            PDUarena_ptr->buf1_newest = newest;
         } else {
            // BUF2
            PDUarena_ptr->buf2_oldest = oldest;
            PDUarena_ptr->buf2_newest = newest;
         }

         // Update oldest and newest node values of circular buffer for next
         //   packet
         newest++;
         if (!circled) {	// first pass through buf is the special case
            if (PDUarena_ptr->current_net_buf == BUF1) {
               PDUarena_ptr->nodes_in_buf1++;
            } else {
               // BUF2
               PDUarena_ptr->nodes_in_buf2++;
            }
            if (newest > buf_dimension) { // back to beginning
               newest = 0, oldest = 1;
               circled = TRUE;
            }
         } else {	// buf is full; all but first pass
            oldest = newest + 1;	// chasing our tail now
            if (oldest > buf_dimension)
               oldest = 0;
            if (newest > buf_dimension)
               newest = 0, oldest = 1;
            // track overwritten PDUs
            if (PDUarena_ptr->current_net_buf == BUF1) {
               PDUarena_ptr->buf1_wasted++;
            } else {
               PDUarena_ptr->buf2_wasted++;
            }
         }

#ifdef DEBUG2
         if (PDUarena_ptr->current_net_buf == BUF1)
            printf("nodes_in_buf1 = %d\n", PDUarena_ptr->nodes_in_buf1);
         else
            printf("nodes_in_buf2 = %d\n", PDUarena_ptr->nodes_in_buf2);
         fflush(stdout);
#endif
      } // end processing of single packet
      usvsema(PDUarena_ptr->swap_buf_sema); // release semaphore

   } // end while(TRUE)

} // areceiver_process()


int socket_write(char *pdu)
{
    int				bytes_sent;
    extern int			sock_send;
    extern struct sockaddr_in	dest;
    #ifdef DEBUG2
	printf("Enter socket_write()\n");
	fflush(stdout);
    #endif
    
    // Actually send it
    if ((bytes_sent = sendto(sock_send, pdu, length, 0,
	(struct sockaddr *) &dest, sizeof(dest))) == -1) 
    {
	perror("sendto()");
	return (FALSE);
    }
    
    return (TRUE);
}


void assign_host_id()
{
    host_id = (unsigned short) my_in_addr[3];    // unique on subnet
}


void socket_close()
{
    kill(PDUarena_ptr->receiver_pid, SIGTERM);
    sginap(5);
    close(sock_send);
    close(sock_recv);
}
