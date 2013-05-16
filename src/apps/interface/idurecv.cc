// File: <idurecv.cc>

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

#include "idunetlib.h"
#include "appearance.h"

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f

#define MAX_LETTERS 70

int G_exit_flag = FALSE;

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
            cerr << "DEATH NOTICE:\tidudump exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }

void output_IDUHeader ( IDUHeader *header, char *leader )
   {

   cout << leader << "IDU HEADER:" << endl;
   cout << leader << "   Type:                     "
        << (int)header->type << endl;
   cout << leader << "   Version:                  "
        << (int)header->version << endl;
   cout << leader << "   Length:                   "
        << (int)header->length << endl;

   }

 

void output_TestIDU ( TestIDU *test, sender_info sender, char *leader )
   {

   char lead[120];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "TEST IDU ### from "
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_IDUHeader ( &(test->header), lead );
   cout << lead << "String component: " << test->string << endl;
   cout << lead << "Int component:    " << test->intnum << endl;
   cout << lead << "Float component:  " << test->floatnum << endl;
   cout << endl;

   }


void output_SSIDU ( SSToJackIDU *ssidu, sender_info sender, char *leader )
{
   char lead[120];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "SS to JACK IDU ### from "
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_IDUHeader ( &(ssidu->header), lead );
   cout << lead << "Speed: " << ssidu->speed << endl;
   cout << lead << "Body Heading: " << ssidu->body_heading << endl;
   cout << lead << "View Heading: " << ssidu->view_heading << endl;
   cout << lead << "View Pitch: " << ssidu->view_pitch << endl;
   cout << lead << "Posture: " << ssidu->posture << endl;
   cout << lead << "Weapon: " << ssidu->weapon << endl;
   cout << lead << "Fire: " << ssidu->fire << endl;
   cout << endl; 
}

void output_JackIDU ( JackToSSIDU *jackidu, sender_info sender, char *leader )
{
   char lead[120];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "Jack to SS IDU ### from "
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_IDUHeader ( &(jackidu->header), lead );
   cout << lead << "Position: "
        << jackidu->position_X << ", "
        << jackidu->position_Y << ", "
        << jackidu->position_Z << endl;
   cout << lead << "Status:" << endl;
   switch ( jackidu->status & Appearance_Damage_Mask ) {
      case Appearance_DamageSlight:
         cout << "\tSlight damage." << endl;
         break;
      case Appearance_DamageModerate:
         cout << "\tModerate damage." << endl;
         break;
      case Appearance_DamageDestroyed:
         cout << "\tDestoyed." << endl;
         break;
      default:
         cout << "\tNo damage." << endl;
         break;
   }
   switch ( jackidu->status & Appearance_LifeForm_State_Mask ) {
      case Appearance_LifeForm_UprightStill:
         cout << "\tUpright Still" << endl;
         break;
      case Appearance_LifeForm_UprightWalking:
         cout << "\tUpright Walking" << endl;
         break;
      case Appearance_LifeForm_UprightRunning:
         cout << "\tUpright Running" << endl;
         break;
      case Appearance_LifeForm_Kneeling:
         cout << "\tKneeling" << endl;
         break;
      case Appearance_LifeForm_Prone:
         cout << "\tProne" << endl;
         break;
      case Appearance_LifeForm_Crawling:
         cout << "\tCrawling" << endl;
         break;
      case Appearance_LifeForm_Swimming:
         cout << "\tSwimming" << endl;
         break;
      case Appearance_LifeForm_Parachuting:
         cout << "\tParachuting" << endl;
         break;
      case Appearance_LifeForm_Jumping:
         cout << "\tJumping" << endl;
         break;
      default:
         cout << "\tUnknown posture." << endl;
         break;
   }
   switch ( jackidu->status & Appearance_LifeForm_Weapon1_Mask ) {
      case Appearance_LifeForm_Weapon1_Stowed:
         cout << "\tWeapon 1 Stowed." << endl;
         break;
      case Appearance_LifeForm_Weapon1_Deployed:
         cout << "\tWeapon 1 Deployed." << endl;
         break;
      case Appearance_LifeForm_Weapon1_Firing:
         cout << "\tWeapon 1 Firing." << endl;
         break;
      default:
         cout << "\tUnknown Weapon 1 status." << endl;
   }
   cout << endl;
}


void
main ( int argc, char **argv )
   {

   int op = 0 ;
   extern char *optarg;
   extern int optind, operr;

   IDUType type;
   char *idu;
   TestIDU *testidu;
   SSToJackIDU *ssidu;
   JackToSSIDU *jackidu;

   sender_info idu_sender_info;

   // Multicast Defaults
   int multicast = TRUE;
   u_short port;
   char group[25];
   u_char ttl = IDU_DEF_MC_TTL;
   port = 0;
   int loopback = FALSE;

   char net_interface[20];

   IDU_net_manager *net;
   int swap_bufs;

   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );

   strncpy ( group, IDU_DEF_MC_GROUP,25 );
   strcpy ( net_interface, "" );

   cout.setf ( ios::fixed, ios::floatfield );
   cout << setprecision(4);

   while ((op = getopt(argc, argv, "P:p:G:g:T:t:BblLI:i:")) != -1)
      {
      switch (op)
         {
         case 'p':
         case 'P':
            port = u_short(atoi(optarg));
            break;
         case 'G':
         case 'g':
            strncpy ( group, optarg, 25 );
            break;
         case 't':
         case 'T':
            ttl =  u_char(atoi(optarg));
            break;
         case 'b':
         case 'B':
            multicast = FALSE;
            break; 
         case 'l':
         case 'L':
            loopback = TRUE;
            break;
         case 'i':
         case 'I':
            strncpy ( net_interface, optarg, 19 );
            break;
         default:
            cerr << "Usage:  idudump [-p <network port>] \n"
                 << "                [-i <network interface>] \n"
                 << "                [-g <multicast group>] \n"
                 << "                [-t <multicast ttl>] \n"
                 << "                [-b   (to enable broadcast) \n";
            exit(0);
            break;
         }
      }

   if ( multicast )
      {
      if ( port == 0 )
         port = IDU_DEF_MC_PORT;
      net = new IDU_net_manager ( group, port, ttl, net_interface,
                                  loopback );
      }
   else
      {
      if ( port == 0 )
         port = IDU_DEF_BC_PORT;
      net = new IDU_net_manager ( port, net_interface, loopback );
      }

   if ( !net->net_open() )
      {
      cerr << "Could not open network." << endl;
      exit(0);
      }
   else
      {
      net->add_to_receive_list(Test_Type);
      net->add_to_receive_list(SS_To_Jack_Type);
      net->add_to_receive_list(Jack_To_SS_Type);
      }

   cerr << endl << "Interface Data Unit (IDU) RECEIVE program:" << endl;
   if ( multicast )
      {
      cerr << "\tMode:     \tMulticast" << endl;
      cerr << "\tPort:     \t" << (int)port << endl;
      cerr << "\tGroup:    \t" << group << endl;
      cerr << "\tTTL:      \t" << (int)ttl << endl;
      cerr << "\tInterface:\t" << net_interface << endl;
      cerr << "\tLoopback: \t";
      if ( loopback )
         cerr << "ON" << endl;
      else
         cerr << "OFF" << endl;
      cerr << endl;
      }
   else
      {
      cerr << "\tMode:     \tBroadcast" << endl;
      cerr << "\tPort:     \t" << (int)port << endl;
      cerr << "\tInterface:\t" << net_interface << endl << endl;
      cerr << "\tLoopback: \t";
      if ( loopback )
         cerr << "ON" << endl;
      else
         cerr << "OFF" << endl;
      cerr << endl;
      }

   while(!G_exit_flag)
      {

      net->read_idu(&idu, &type, idu_sender_info, swap_bufs);

      if ( idu != NULL )
         {

         switch(type)
           {
           case (Test_Type):
             testidu = (TestIDU *)idu;
             output_TestIDU ( testidu, idu_sender_info, "" );
             break;
           case (SS_To_Jack_Type):
             ssidu = (SSToJackIDU *)idu;
             output_SSIDU ( ssidu, idu_sender_info, "" );
             break;
           case (Jack_To_SS_Type):
             jackidu = (JackToSSIDU *)idu;
             output_JackIDU ( jackidu, idu_sender_info, "" );
             break;
           default:
             cerr << "\nUnknown type received: " << int(type)
                  << endl << endl;
             break;
           }

        }

      }
   
   net->net_close();  
   }
