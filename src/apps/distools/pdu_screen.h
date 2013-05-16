// File: <pdu_screen.h>

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

#ifndef __PDU_SCREEN__
#define __PDU_SCREEN__

#ifdef __cplusplus
extern "C" {
#endif
#define bool xbool
#include <curses.h>
#undef bool
#ifdef __cplusplus
}
#endif
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <bstring.h>
#include "disnetlib.h"

/***** Public #defines, typedefs and enums *****/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum status_field { ERROR = -1, S_RESET, S_REFRESH, CACHE, S_SCROLL,
                    ES_COUNT, F_COUNT, D_COUNT, O_COUNT, ENTITY_COUNT };


typedef struct
   {
   time_t last_time;
   sender_info sender;
   EntityStatePDU epdu;
   void *next_entity;
   } EntityRec;

typedef struct
   {
   time_t last_time;
   sender_info sender;
   FirePDU fpdu;
   void *next_fire;
   } FireRec;

typedef struct
   {
   time_t last_time;
   sender_info sender;
   DetonationPDU dpdu;
   void *next_detonation; 
   } DetonationRec;



/***** Public Prototypes with descriptions *****/

int lookup_name_from_address ( sender_info & );

int open_master_window ();
int close_master_window ();
int open_entity_window ();
int close_entity_window ();
int open_detonation_window ( int & );
int close_detonation_window ();
int open_fire_window ( int & );
int close_fire_window ();
int open_status_window ();
int close_status_window ();
int update_status_window ( status_field, char * );
void redraw_entity_list ( int );
void redraw_fires();
void redraw_detonations();
void update_entity_list ( int, EntityRec *, EntityStatePDU * );
int update_list ( sender_info &, EntityStatePDU * );
int remove_fire ();
int update_list ( sender_info &, FirePDU * );
int remove_detonation ();
int update_list ( sender_info &, DetonationPDU * );
void review_lists ();

#endif

/***** End of pdu_screen.h *****/
