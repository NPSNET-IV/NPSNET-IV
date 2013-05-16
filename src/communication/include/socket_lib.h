// File: <socket_lib.h>

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


#ifndef SOCKET_LIB_H
#define SOCKET_LIB_H

#ifndef FALSE
#   define FALSE    0
#endif

#ifndef TRUE
#   define TRUE	    !FALSE
#endif

#define UDP_SEND_PORT	3200
#define UDP_RECV_PORT	(UDP_SEND_PORT+1)

// Check for interfaces with "netstat -rn"
#define IN_ADDR_SIZE	4
#define MAX_INTERF	6
#define BCAST_INTERF	"ec0"	/* local */

// socket_read() status return struct
struct readstat {
   int		rs_wasted;	// wasted (overwritten) PDUs
   int		rs_swap_buffers_flag;
};

// Prototypes
void	assign_host_id();
void	init_arena();
void	socket_close();
int	socket_open(char *interf, unsigned short send_port,
                    unsigned short recv_port, int buf_len);
GUI_MSG_DATA *socket_read(struct readstat *rstat);
int	socket_write(char *pdu);
void	areceiver_process(void *);
void	swap_net_buffers();

#endif
