// File: <bridge.cc>

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
//
// Written by:  Paul T. Barham
// Naval Postgraduate School, Computer Science Department
// Code CS/Barham, Monterey, CA 93940-5100 USA


#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <bstring.h>
#include <unistd.h>

#include <disbridge.h>

#define VERSION "NPS DIS_bridge, Version 1.0, BETA"

typedef struct
   {
   char                 interface[NET_NAMES_SIZE];
   unsigned short       port;
   char                 group[NET_NAMES_SIZE];
   unsigned char        ttl;
   network_mode         mode;
   convert_direction    round_conv_direction;
   unsigned char        exercise;
   char			address[NET_NAMES_SIZE];
   } one_way_info;

typedef struct
   {
   double               xmin,ymin,zmin;
   double               xmax,ymax,zmax;
   char                 round_file[FILE_NAME_SIZE];
   int                  bounds_check;
   EntityID		entity_id;
   int			bound_about_entity;
   convert_direction	bounds_mode;
   u_char               *receive_pdu;
   int			receive_all;
   int			num_pdu_types;
   int			verbose;
   int			size_check;
   } general_info;


int G_exit_flag;

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

InitManager *initman;

void
init_read_write_info ( one_way_info *rinfo, one_way_info *winfo,
                       general_info *ginfo )
   {
   u_char all_ones = 0xFF;
   PDUType pdutype;

   strcpy ( rinfo->interface, "" );
   rinfo->port = 0;
   strcpy ( rinfo->group, DEFAULT_MC_GROUP );
   rinfo->ttl = DEFAULT_MC_TTL;
   rinfo->mode = IP_BROADCAST;
   rinfo->round_conv_direction = NO_CONVERSION;
   rinfo->exercise = DIS_EXERCISE_ALL;
   bzero ( (char *)rinfo->address, NET_NAMES_SIZE );
   bcopy ( (char *)rinfo, (char *)winfo, sizeof(one_way_info) );

   ginfo->xmin = 0.0;
   ginfo->ymin = 0.0;
   ginfo->zmin = 0.0;
   ginfo->xmax = 0.0;
   ginfo->ymax = 0.0;
   ginfo->zmax = 0.0;
   strcpy ( ginfo->round_file, "" );
   ginfo->bounds_check = FALSE;
   bzero ( (char *)&ginfo->entity_id, sizeof(EntityID) );
   ginfo->bound_about_entity = FALSE;
   ginfo->bounds_mode = NO_CONVERSION;

   /* Determine the number of possible PDU types from the size of the */
   /* PDUType type definition.                                        */
   bcopy ( (char *)&all_ones, (char *)&pdutype, sizeof(pdutype) );
   ginfo->num_pdu_types = int(pdutype) + 1;

   ginfo->receive_pdu = (u_char *)new u_char[ginfo->num_pdu_types];
   if ( ginfo->receive_pdu == NULL )
      {
      cerr << "WARNING:\tBridge unable to allocate memory for "
           << "pdu-to-receive list." << endl;
      cerr << "\tAll PDUS will be passed through." << endl;
      }
   else
      bzero ( (char *)ginfo->receive_pdu, sizeof(u_char) );
   ginfo->receive_all = TRUE;

   ginfo->verbose = FALSE;
   ginfo->size_check = FALSE;

   }


void
clean_up_info ( general_info *ginfo )
   {
   if ( ginfo->receive_pdu != NULL )
      {
      delete ginfo->receive_pdu;
      ginfo->receive_pdu = NULL;
      }
   ginfo->receive_all = TRUE;
   }


void
print_read_write_info ( one_way_info *rinfo, one_way_info *winfo,
                        general_info *ginfo )
   {
   cerr << endl << "***** Bridge configuration information *****" << endl;
   cerr << "Read (In) information:" << endl;
   if ( strcmp(rinfo->interface,"") == 0 )
      cerr << "\tEthernet Interface:  Default - first available." << endl;
   else
      cerr << "\tEthernet Interface:  " << rinfo->interface << endl;
   cerr << "\tPort:  " << rinfo->port << endl;
   switch ( rinfo->mode )
      {
      case IP_BROADCAST:
         cerr << "\tMode:  IP BROADCAST" << endl;
         break;
      case IP_MULTICAST:
         cerr << "\tMode:  IP MULTICAST" << endl;
         cerr << "\tGroup:  " << rinfo->group << endl;
         cerr << "\tTTL:  " << int(rinfo->ttl) << endl;
         break;
      case IP_UNICAST:
         cerr << "\tMode:  IP UNICAST" << endl;
         cerr << "\tSource:  " << rinfo->address << endl;
         break;
      default:
         cerr << "\tMode:  ERROR MODE" << endl;
         break;
      }
   switch ( rinfo->round_conv_direction )
      {
      case FLAT_TO_ROUND:
         cerr << "\tFLAT TO ROUND Coordinate conversion." << endl;
         cerr << "\tRound world file:  " << ginfo->round_file << endl;
         break;
      case ROUND_TO_FLAT:
         cerr << "\tROUND TO FLAT Coordinate conversion." << endl;
         cerr << "\tRound world file:  " << ginfo->round_file << endl;
         break;
      default:
         cerr << "\tNO Coordinate conversion." << endl;
         break;
      }
   if ( rinfo->exercise == DIS_EXERCISE_ALL )
      cerr << "\tPassing ALL Exercises." << endl;
   else
      cerr << "\tExercise ID:  " << int(rinfo->exercise) << endl;

   cerr << endl;
   cerr << "Write (Out) information:" << endl;
   if ( strcmp(winfo->interface,"") == 0 )
      cerr << "\tEthernet Interface:  Default - first available." << endl;
   else
      cerr << "\tEthernet Interface:  " << winfo->interface << endl;
   cerr << "\tPort:  " << winfo->port << endl;
   switch ( winfo->mode )
      {
      case IP_BROADCAST:
         cerr << "\tMode:  IP BROADCAST" << endl;
         break;
      case IP_MULTICAST:
         cerr << "\tMode:  IP MULTICAST" << endl;
         cerr << "\tGroup:  " << winfo->group << endl;
         cerr << "\tTTL:  " << int(winfo->ttl) << endl;
         break;
      case IP_UNICAST:
         cerr << "\tMode:  IP UNICAST" << endl;
         cerr << "\tDestination:  " << winfo->address << endl;
         break;
      default:
         cerr << "\tMode:  ERROR MODE" << endl;
         break;
      }
   switch ( winfo->round_conv_direction )
      {
      case FLAT_TO_ROUND:
         cerr << "\tFLAT TO ROUND Coordinate conversion." << endl;
         cerr << "\tRound world file:  " << ginfo->round_file << endl;
         break;
      case ROUND_TO_FLAT:
         cerr << "\tROUND TO FLAT Coordinate conversion." << endl;
         cerr << "\tRound world file:  " << ginfo->round_file << endl;
         break;
      default:
         cerr << "\tNO Coordinate conversion." << endl;
         break;
      }
   if ( winfo->exercise == DIS_EXERCISE_ALL )
      cerr << "\tExercise ID will not be changed." << endl;
   else
      cerr << "\tExercise ID will be set to:  " << int(winfo->exercise) << endl;
   cerr << endl;

   }


void
handler(int sig, ...)
   {
   if ( !G_exit_flag )
      {
      signal ( sig, SIG_IGN );
      switch ( sig )
         {
         case SIGTERM:
            break;
         case SIGHUP:
            cerr << "DEATH NOTICE:\tBridge exitting due to death "
                 << "of parent process." << endl;
            break;
         case SIGINT:
            break;
         case SIGQUIT:
            cerr << "DEATH NOTICE:\tBridge exitting due to quit "
                 << "signal." << endl;
            break;
         default:
            cerr << "DEATH NOTICE:\tBridge exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }


char *myopts[] = {
#define INTERFACE	0
				"interface",
#define PORT		1
				"port",
#define GROUP		2
				"group",
#define TTL		3
				"ttl",
#define MODE		4
				"mode",
#define DIRECTION	5
				"direction",
#define EXERCISE	6
				"exercise",
#define PDUTYPE		7
				"pdutype",
#define XMIN		8
				"xmin",
#define YMIN		9
				"ymin",
#define ZMIN		10
				"zmin",
#define XMAX		11
				"xmax",
#define YMAX		12
				"ymax",
#define ZMAX		13
				"zmax",
#define ADDRESS		14
				"address",
#define SITE		15
				"site",
#define HOST		16
				"host",
#define ENTITY		17
				"entity",
		NULL};


void
main ( int argc, char **argv )
   {

   int op = 0;
   char *options, *value;
   extern char *optarg;
   extern int optind, operr;

   DIS_bridge *bridge;
   one_way_info read_info;
   one_way_info write_info;
   general_info gen_info;
   one_way_info *rinfo = &read_info;
   one_way_info *winfo = &write_info;
   one_way_info *current;
   general_info *ginfo = &gen_info;

   PDUType pdu_to_receive;
   char pdu_name[255];

   cerr << endl;
   cerr << "************************************************************" << endl;
   cerr << "You are running " << VERSION << ", developed at:" << endl;
   cerr << "\tThe Naval Postgraduate School," << endl;
   cerr << "\tComputer Graphics and Video Laboratory," << endl;
   cerr << "\tMonterey, CA 93943-5100 USA." << endl;
   cerr << "\tSoftware questions and comments can be sent to:" << endl;
   cerr << "\t\tPaul T. Barham, barham@cs.nps.navy.mil" << endl;
   cerr << "\tProgram & Principal Investigator Contacts:" << endl;
   cerr << "\t\tDave Pratt, pratt@cs.nps.navy.mil" << endl;
   cerr << "\t\tMike Zyda, zyda@cs.nps.navy.mil" << endl;
   cerr << "This software is free and is provided as is." << endl;
   cerr << "************************************************************" << endl;

   init_read_write_info ( rinfo, winfo, ginfo );
   int print_help = FALSE;
   G_exit_flag = FALSE;

   cout.setf ( ios::fixed, ios::floatfield );
   cout << setprecision(4);

   while ((op = getopt(argc, argv, "hHvVsSf:F:r:R:w:W:b:B:p:P:")) != -1)
      {
      current = NULL;
      switch (op)
         {
         case 'r':
         case 'R':
            current = rinfo;
            cerr << "NPS DIS_bridge Read Options:" << endl;
         case 'w':
         case 'W':
            if ( current == NULL )
               {
               current = winfo;
               cerr << "NPS DIS_bridge Write Options:" << endl;
               }
            options = optarg;
            while (*options != '\0')
               {
               switch(getsubopt(&options,myopts,&value))
                  {
                  case INTERFACE:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        strcpy ( current->interface, value );
                        cerr << "\tInterface set to "
                             << current->interface << endl;
                        }
                     break;
                  case PORT:
                     if (value==NULL)
                        print_help = TRUE;
                     else
                        {
                        current->port = u_short(atoi(value));
                        cerr << "\tPort set to " << current->port
                             << endl;
                        }
                     break;
                  case GROUP:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        strcpy ( current->group, value );
                        cerr << "\tMulticast group set to "
                             << current->group << endl;
                        }
                     break;
                  case TTL:
                     if (value==NULL)
                        print_help = TRUE;
                     else
                        {
                        current->ttl = u_char(atoi(value));
                        cerr << "\tMulticast ttl set to " << int(current->ttl)
                             << endl;
                        }
                     break;
                  case MODE:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        if ( (strncmp ( "multicast", value,1 )) == 0 )
                           {
                           current->mode = IP_MULTICAST;
                           cerr << "\tMode set to multicast"
                                << endl;
                           }
                        else if ( (strncmp ( "broadcast", value,1 )) == 0 )
                           {
                           current->mode = IP_BROADCAST;
                           cerr << "\tMode set to broadcast"
                                << endl;
                           }
                        else if ( (strncmp ( "unicast", value,1 )) == 0 )
                           {
                           current->mode = IP_UNICAST;
                           cerr << "\tMode set to unicast"
                                << endl;
                           }
                        else
                          {
                          current->mode = ERROR_MODE;
                          print_help = TRUE;
                          }
                        }
                     break;
                  case DIRECTION:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        if ( (strncmp ( "flatround", value,1 )) == 0 )
                           {
                           current->round_conv_direction = FLAT_TO_ROUND;
                           cerr << "\tConversion direction is FLAT to ROUND."
                                << endl;
                           }
                        else if ( (strncmp ( "roundflat", value,1 )) == 0 )
                           {
                           current->round_conv_direction = ROUND_TO_FLAT;
                           cerr << "\tConversion direction is ROUND to FLAT."
                                << endl;
                           }
                        else
                          {
                          current->round_conv_direction = NO_CONVERSION;
                          print_help = TRUE;
                          }
                        }
                     break;
                  case EXERCISE:
                     if (value==NULL)
                        print_help = TRUE;
                     else
                        {
                        current->exercise = u_char(atoi(value));
                        cerr << "\tExercise set to " 
                             << int(current->exercise) << endl;
                        }
                     break;
                  case ADDRESS:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        strcpy ( current->address, value );
                        cerr << "\tUnicast address set to "
                             << current->address << endl;
                        }
                     break;
                  default:
                     print_help = TRUE;
                     break;
                  }
               }
            break;
         case 'b':
         case 'B':
            options = optarg;
            ginfo->bounds_check = TRUE;
            cerr << "Bounds check enabled:" << endl;
            while (*options != '\0')
               {
               switch(getsubopt(&options,myopts,&value))
                  {
                  case MODE:
                     if ( value == NULL )
                        print_help = TRUE;
                     else
                        {
                        if ( (strncmp ( "roundflat", value,1 )) == 0 )
                           {
                           ginfo->bounds_mode = ROUND_TO_FLAT;
                           cerr << "\tBounds will be converted from round to flat."
                                << endl;
                           }
                        else if ( (strncmp ( "flatround", value,1 )) == 0 )
                           {
                           ginfo->bounds_mode = FLAT_TO_ROUND;
                           cerr << "\tBounds will be converted from flat to rounds."
                                << endl;
                           }
                        else
                          {
                          ginfo->bounds_mode = NO_CONVERSION;
                          print_help = TRUE;
                          }
                        }
                     break;
                  case XMIN:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->xmin = (double)atol(value);
                        cerr << "\tMINimum X = "
                             << ginfo->xmin << endl;
                        }
                     break;
                  case YMIN:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->ymin = (double)atol(value);
                        cerr << "\tMINimum Y = "
                             << ginfo->ymin << endl;
                        }
                     break;
                  case ZMIN:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->zmin = (double)atol(value);
                        cerr << "\tMINimum Z = "
                             << ginfo->zmin << endl;
                        }
                     break;
                  case XMAX:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->xmax = (double)atol(value);
                        cerr << "\tMAXimum X = "
                             << ginfo->xmax << endl;
                        }
                     break;
                  case YMAX:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->ymax = (double)atol(value);
                        cerr << "\tMAXimum Y = "
                             << ginfo->ymax << endl;
                        }
                     break;
                  case ZMAX:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->zmax = (double)atol(value);
                        cerr << "\tMAXimum Z = "
                             << ginfo->zmax << endl;
                        }
                     break;
                  case SITE:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->entity_id.address.site = (unsigned int)atoi(value);
                        cerr << "\tSite for bounding entity = "
                             << ginfo->entity_id.address.site << endl;
                        }
                     ginfo->bound_about_entity = TRUE;
                     break;
                  case HOST:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->entity_id.address.host = (unsigned int)atoi(value);
                        cerr << "\tHost for bounding entity = "
                             << ginfo->entity_id.address.host << endl;
                        }
                     ginfo->bound_about_entity = TRUE;
                     break;
                  case ENTITY:
                     if (value == NULL )
                        print_help = TRUE;
                     else
                        {
                        ginfo->entity_id.entity = (unsigned int)atoi(value);
                        cerr << "\tEntity for bounding entity = "
                             << ginfo->entity_id.entity << endl;
                        }
                     ginfo->bound_about_entity = TRUE;
                     break;
                  default:
                     print_help = TRUE;
                     break;
                  }
               }
            break;
         case 'f':
         case 'F':
            strcpy ( ginfo->round_file, optarg );
            cerr << "Defining round world conversion from file:  "
                 << ginfo->round_file << endl;
            break;
         case 'p':
         case 'P':
            pdu_to_receive = PDUType(atoi(optarg));
            if ( pdutype_to_name ( pdu_to_receive, pdu_name ) )
               {
               ginfo->receive_pdu[pdu_to_receive] = TRUE;
               ginfo->receive_all = FALSE;
               cerr << "Configured to pass:  " << pdu_name << endl;
               }
            else
               {
               cerr << "UNKNOWN PDU TYPE =\t" << atoi(optarg) << endl;
               cerr << "\tIgnoring error in command line." << endl;
               }
            break;
         case 'v':
         case 'V':
            ginfo->verbose = TRUE;
            cerr << "Verbose mode enabled." << endl;
            break;
         case 's':
         case 'S':
            ginfo->size_check = TRUE;
            cerr << "PDU size checks enabled." << endl;
            break;
         case 'h':
         case 'H':
            print_help = TRUE;
         default:
            break;
         }
      }

   if ( print_help )
      {
      cerr << "Usage:" << endl;
      cerr << "disbridge\t[-h] [-v] [-s] [-p <pdutype>] [-b <bounds_list>] " << endl;
      cerr << "\t[-f <conversion_filename>] [-r <options>] [-w <options>]" << endl;
      cerr << "Where:" << endl;
      cerr << "\t'h' requests help." << endl;
      cerr << "\t'v' turns on verbose mode." << endl;
      cerr << "\t's' enables PDU size checks." << endl; 
      cerr << "\t'p' enables PDU filtering so <pdutype> is passed (not filtered);"
           << endl;
      cerr << "\t\t<pdutype> is a non-negative integer 0 - 255." << endl;
      cerr << "\t'b' turns on bounds filtering:" << endl;
      cerr << "\t\t<bounds_list> is a comma, separated list of the following:"
           << endl;
      cerr << "\t\t\tmode=<convert mode>, where <convert mode> is:" << endl;
      cerr << "\t\t\t\troundflat or flatround." << endl;
      cerr << "\t\t\txmin=<integer minimum x>," << endl;
      cerr << "\t\t\tymin=<integer minimum y>," << endl;
      cerr << "\t\t\tzmin=<integer minimum z>," << endl;
      cerr << "\t\t\txmin=<integer maximum x>," << endl;
      cerr << "\t\t\tymax=<integer maximum y>," << endl;
      cerr << "\t\t\tzmax=<integer maximum z>," << endl;
      cerr << "\t\t\tsite=<site number>, (bounding around entity)" 
           << endl;
      cerr << "\t\t\thost=<host number>, (bounding around entity)" << endl;
      cerr << "\t\t\tentity=<entity number>, (bounding around entity)" << endl;
      cerr << "\t'f' specifies the round/flat conversion filename," << endl;
      cerr << "\t'r' specifies the inbound (read) translation options;" << endl;
      cerr << "\t'w' specifies the outbound (write) translation options;" << endl;
      cerr << "\t\t<options> is a comma, separated list of the following:" << endl;
      cerr << "\t\t\tinterface=<ethernet interface>," << endl;
      cerr << "\t\t\tport=<port number>," << endl;
      cerr << "\t\t\tgroup=<multicast group string>," << endl;
      cerr << "\t\t\tttl=<multicast time-to-live number>," << endl;
      cerr << "\t\t\tmode=<mode_type>, where mode_type is:" << endl;
      cerr << "\t\t\t\tbroadcast, multicast or unicast." << endl;
      cerr << "\t\t\taddress=<unicast address>, where <unicast address> is:" 
           << endl;
      cerr << "\t\t\t\tan internet address or a host name." << endl;
      cerr << "\t\t\tdirection=<conversion_direction>, " << endl;
      cerr << "\t\t\t\twhere <conversion_direction> is:" << endl;
      cerr << "\t\t\t\tflatround or roundflat." << endl;
      cerr << "\t\t\texercise=<exercise identifier number>," << endl;
      cerr << endl;
      cerr << "Typical example:" << endl;
      cerr << "bridge\t-v -s -p1 -p2 -p3 -r mode=broadcast,exercise=5 \\" << endl;
      cerr << "\t-w mode=multicast,exercise=7,ttl=128" << endl;
      cerr << endl;
      exit(0);
      }


   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );

   bridge = new DIS_bridge ( winfo->interface, winfo->mode,
                             rinfo->interface, rinfo->mode );

   if ( rinfo->mode == IP_BROADCAST )
      {
      if ( rinfo->port == 0 )
         rinfo->port = DEFAULT_BC_PORT;
      bridge->define_read_socket (rinfo->port);
      }
   else if ( rinfo->mode == IP_MULTICAST )
      {
      if ( rinfo->port == 0 )
         rinfo->port = DEFAULT_MC_PORT;
      bridge->define_read_socket(rinfo->port,rinfo->group,rinfo->ttl);
      }
   else if ( rinfo->mode == IP_UNICAST )
      {
      if ( rinfo->port == 0 )
         rinfo->port = DEFAULT_UC_PORT;
      if ( strcmp(rinfo->address,"") == 0 )
         strcpy ( rinfo->address, DEFAULT_UC_INET_ADDRESS );
      bridge->define_read_socket (rinfo->port, rinfo->address);
      }
   else
      cerr << "Something is terribly wrong, please hit <Ctrl>C." << endl;

   if ( winfo->mode == IP_BROADCAST )
      {
      if ( winfo->port == 0 )
         winfo->port = DEFAULT_BC_PORT;
      bridge->define_write_socket (winfo->port);
      }
   else if ( winfo->mode == IP_MULTICAST )
      {
      if ( winfo->port == 0 )
         winfo->port = DEFAULT_MC_PORT;
      bridge->define_write_socket(winfo->port,winfo->group,winfo->ttl);
      }
   else if ( winfo->mode == IP_UNICAST )
      {
      if ( winfo->port == 0 )
         winfo->port = DEFAULT_UC_PORT;
      if ( strcmp(winfo->address,"") == 0 )
         strcpy ( winfo->address, DEFAULT_UC_INET_ADDRESS );
      bridge->define_write_socket (winfo->port, winfo->address);
      }
   else
      cerr << "Something is terribly wrong, please hit <Ctrl>C." << endl;

   bridge->define_read_exercise ( rinfo->exercise );

   bridge->define_write_exercise ( winfo->exercise );

   if ( (strcmp(ginfo->round_file, "" )) != 0 ) {
//      bridge->define_round_world ( ginfo->round_file );
      cerr << "This functionality is broken at this time." << endl;
   }

   if (rinfo->round_conv_direction != NO_CONVERSION)
      bridge->define_read_coord_conversion ( rinfo->round_conv_direction );

   if (winfo->round_conv_direction != NO_CONVERSION)
      bridge->define_write_coord_conversion ( winfo->round_conv_direction );

   if ( ginfo->bounds_check )
      bridge->restrict_bounds ( ginfo->xmin, ginfo->ymin, ginfo->zmin,
                                ginfo->xmax, ginfo->ymax, ginfo->zmax,
                                ginfo->bounds_mode );

   if ( ginfo->bound_about_entity )
      bridge->bound_around_entity ( ginfo->entity_id );

   if ( !ginfo->receive_all )
      {
      bridge->pass_no_pdus();
      for ( int i = 0; i < ginfo->num_pdu_types; i++ )
         {
         if ( ginfo->receive_pdu[i] )
            bridge->pass_pdu ( (PDUType)i );
         }
      }

   bridge->print_feedback ( ginfo->verbose );
   bridge->verify_pdu_size ( ginfo->size_check );

   print_read_write_info ( rinfo, winfo, ginfo );

   if ( !bridge->net_open() )
      {
      cerr << "Unable to start NPS DIS_bridge in the given configuration." 
           << endl;
      G_exit_flag = TRUE;
      }
   else
      {
      cerr << "NPS DIS_bridge is running.  Press <Ctrl C> to exit." << endl;
      }

   while ( !G_exit_flag )
      {
      sleep(60);
      }

   delete bridge;
   clean_up_info ( ginfo );
   exit(0);

   }
