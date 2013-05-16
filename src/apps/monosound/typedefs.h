// File: <typedefs.h>

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
// NPSMONO Version 4.0
// Written by:  Paul T. Barham and John T. Roesli
// Naval Postgraduate School, Computer Science Department
// Code CS, Monterey, CA 93940-5100 USA


#ifndef __NPS_SOUND_TYPEDEFS__
#define __NPS_SOUND_TYPEDEFS__

#include "setup_const.h"
#include "disnetlib.h"

enum MODE 
   {loading, no_vehicle, a_vehicle, my_vehicle_alive, my_vehicle_dead,
    override, exiting};

typedef struct
   {
   PDUType event_type;
   EntityLocation event_location;
   double event_radius;
   double event_time;
   int event_snd;
   } SOUND_EVENT_REC;


typedef struct
   {
   double            last_es_time;
   EntityLocation    location;
   VelocityVector    velocity;
   EntityOrientation orientation;
   EntityID          entity_id;
   char              hostname[255];
   char              host_net_addr[255];
   int               speed;
   int               maxspeed;
   int               alive;
   int               entity_sound;
   int               my_sound_on;
   DeadReckonParams  deadreckon;
   double            last_DR_time;
   } ENTITY_REC;

typedef struct
   {
   double last_es_time;
   double current_time;
   double mode_change_time;
   int    mode_change_active;
   MODE   mode;
   int    midifd;
   long   gwindow;
   int    banknum;
   int    num_environ;
   int    done;
   int    override_req;
   int    debug;
   int    window_open;
   } STATE_REC;

typedef struct
   {
   char master_name[255];
   char master_net_addr[255];
   char environmental_file[FILENAME_SIZE];
   char round_world_file[FILENAME_SIZE];
   int round;
   roundWorldStruct *rw;
   char host_file[FILENAME_SIZE];
   char sound_file[FILENAME_SIZE];
   char search_path[MAX_SEARCH_PATH];
   u_char exercise;
   u_short port;
   char group[FILENAME_SIZE];
   int  ttl;
   int multicast;
   EntityID entity_id;
   char ether_interf[10];
   char identify_file[FILENAME_SIZE];
   } CONFIG_REC;

typedef struct
   {
   int            midi_note_num;
   EntityLocation location;
   float          range_sqrd;
   double         repeat_time;
   double         last_play_time;
   } ENVIRON_REC;


#endif
