// File: <cdump.cc>

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
#include <fstream.h>
#include <stream.h>
#include <strstream.h>
#include <iomanip.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <bstring.h>
#include <signal.h>

#include <disnetlib.h>
#include "pdu_screen.h"

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f

#define MAX_LETTERS 70

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

InitManager *initman;

int G_exit_flag = FALSE;
WINDOW *text_window = NULL;

void
handler(int sig, ...)
   {
   if ( !G_exit_flag )
      {
      signal ( sig, SIG_IGN );
      switch ( sig )
         {
         case SIGTERM:
         case SIGHUP:
         case SIGINT:
         case SIGQUIT:
            break;
         default:
            cerr << "DEATH NOTICE:\tpdudisplay exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }


int sameID ( EntityID arg1, EntityID arg2 )
   {
   if ( (arg1.address.site == (u_short)0) || 
        (arg1.address.site == arg2.address.site) )
      {
      if ( (arg1.address.host == (u_short)0) || 
           (arg1.address.host == arg2.address.host) ) 
         {
         if ( (arg1.entity == (u_short)0) || 
              (arg1.entity == arg2.entity) )
            return 1;
         }
      }
   return 0;
   }


void
main ( int argc, char **argv )
   {

   extern char *optarg;
   extern int optind, operr;
   EntityID filterID;
   EntityID targetID;

   PDUType type;
   char *pdu;
   FirePDU *firepdu;
   EntityStatePDU *entitystatepdu;
   DetonationPDU *detonationpdu;
   sender_info pdu_sender_info;

   // Multicast Defaults
   int multicast = 0;
   u_short port;
   char group[33];
   u_char ttl;

   char net_interface[20];
   int exercise;
   int round;
   char rfile[512];

   DIS_net_manager *net;
   int swap_bufs;
   int num_filters;
   struct FIL im_filters[MAX_PDU_FILTERS];

   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );

   // START Initialization Manager routines
   union query_data local;
   initman = new InitManager();         // defaults assigned
   initman->process_Master_Configs(argc, argv);
   initman->process_environment_vars();
   initman->process_cl_args(argc, argv);
   initman->parse_input_files();
   initman->validate_data();
   initman->dump_inits();       // let's keep a record of what we've got
   cout.flush();
   // END Initialization Manager routines

   initman->query(TARGET_ENTITY, &local, GET);
   targetID.address.site = (u_short)local.ints[0];
   targetID.address.host = (u_short)local.ints[1];
   targetID.entity = (u_short)local.ints[2];

   initman->query(IM_PORT, &local, GET);
   port = (u_short)local.ints[0];

   initman->query(MULTICAST, &local, GET);
   multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );

   initman->query(DIS_EXERCISE, &local, GET);
   exercise = (u_char) local.ints[0];

   initman->query(ROUND_WORLD_FILE, &local, GET);
   strcpy ( rfile, local.string );
   round = (strcmp(rfile,"NONE") != 0);

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( net_interface, local.string, 10 );

   roundWorldStruct *rw = NULL;

   if (round) {
      rw = new roundWorldStruct;
      struct RW initManRW;
      initman->get_round_world_data(&initManRW);
      rw->northing = initManRW.northing;
      rw->easting = initManRW.easting;
      rw->o_zone_num = initManRW.zone_num;
      rw->o_zone_letter = initManRW.zone_letter;
      rw->map_datum = initManRW.map_datum;
      rw->width = initManRW.width;
      rw->height = initManRW.height;
   }

   if ( multicast )
      {
      if ( port == (u_short)0 )
         port = DEFAULT_MC_PORT;
      net = new DIS_net_manager ( group, ttl, port, exercise, round, rw,
                                  DEFAULT_BUF_SIZE, net_interface );
      }
   else
      {
      if ( port == (u_short)0 )
         port = DEFAULT_BC_PORT;
      net = new DIS_net_manager ( exercise, round, rw, DEFAULT_BUF_SIZE,
                                  net_interface, port );
      }

   if ( !net->net_open() )
      {
      cerr << "Could not open network." << endl;
      exit(0);
      }
   else {
      net->add_to_receive_list(EntityStatePDU_Type);
      net->add_to_receive_list(FirePDU_Type);
      net->add_to_receive_list(DetonationPDU_Type);
      initman->get_filters((struct FIL**) im_filters, &num_filters);
      for (int i = 0; i < num_filters; i++) {
         if (im_filters[i].filter_type == TYPE_BLOCK) {
            filterID.address.site = (u_short)im_filters[i].Type.Block.site;
            filterID.address.host = (u_short)im_filters[i].Type.Block.host;
            filterID.entity = (u_short)im_filters[i].Type.Block.entity;
            net->add_to_filter_list(filterID);
         }
      }
   }


   while(!G_exit_flag)
      {
      static int first_time = TRUE;

      net->read_pdu(&pdu, &type, pdu_sender_info, swap_bufs);

      if ( first_time )
         {
         if ( !open_master_window() )
            cerr << "Could not open master window." << endl;
         else
            {
            int fire_lines = 5;
            int detonation_lines = 5;
            if ( !open_status_window() )
               cerr << "Could not open status window." << endl;
            if ( !open_fire_window(fire_lines) )
               cerr << "Could not open fire window." << endl;
            if ( !open_detonation_window(detonation_lines) )
               cerr << "Could not open detonation window." << endl;
            if ( !open_entity_window() )
               cerr << "Could not open entity window." << endl;
            }
         first_time = FALSE;
         }

      if ( pdu != NULL )
         {

         switch(type)
           {
           case (FirePDU_Type):
             firepdu = (FirePDU *)pdu;
             if ( sameID ( targetID, firepdu->firing_entity_id ) )
                {
                update_status_window (F_COUNT,NULL);
                if ( !update_list ( pdu_sender_info, firepdu ) )
                   beep();
                }
             break;
           case (EntityStatePDU_Type):
             entitystatepdu = (EntityStatePDU *)pdu;
             if ( sameID ( targetID, entitystatepdu->entity_id) )
                {
                update_status_window (ES_COUNT,NULL);
                if ( !update_list ( pdu_sender_info, entitystatepdu ) )
                   beep();
                }
             break;
           case (DetonationPDU_Type):
             detonationpdu = (DetonationPDU *)pdu;
             if ( sameID ( targetID, detonationpdu->firing_entity_id) )
                {
                update_status_window (D_COUNT,NULL);
                if ( !update_list ( pdu_sender_info, detonationpdu ) )
                   beep();
                }
             break;
           default:
              update_status_window (O_COUNT,NULL);
             break;
           }

        }

      review_lists();

      }
  
   if ( !close_master_window() )
      cerr << "Unable to close curses screen." << endl;  

   net->net_close();
   exit(0);
   }
