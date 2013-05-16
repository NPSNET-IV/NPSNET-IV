// File: <idusend.cc>

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
#include <unistd.h>

#include <idunetlib.h>


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
            cerr << "DEATH NOTICE:\tidusend exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }


void read_test_info (TestIDU *testidu)
   {
   cin >> testidu->string;
   cin >> testidu->intnum;
   cin >> testidu->floatnum;
   testidu->header.version = IDUVersionMarch95;
   cerr << "Sending TestIDU:" << endl;
   cerr << "\tString:\t" << testidu->string << endl;
   cerr << "\tInt:   \t" << testidu->intnum << endl;
   cerr << "\tFloat: \t" << testidu->floatnum << endl;

   }


void
main ( int argc, char **argv )
   {

   int op = 0 ;
   extern char *optarg;
   extern int optind, operr;

   int type;
   TestIDU testidu;

   // Multicast Defaults
   int multicast = TRUE;
   u_short port;
   char group[25];
   u_char ttl = IDU_DEF_MC_TTL;
   port = 0;
   int loopback = FALSE;

   float delay;

   char net_interface[20];

   IDU_net_manager *net;

   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );

   strncpy ( group, IDU_DEF_MC_GROUP,25 );
   strcpy ( net_interface, "" );

   cout.setf ( ios::fixed, ios::floatfield );
   cout << setprecision(4);

   while ((op = getopt(argc, argv, "P:p:G:g:T:t:BbLlI:i:")) != -1)
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
      net->add_to_receive_list(Test_Type);

   cerr << endl << "Interface Data Unit (IDU) SEND program:" << endl;
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

      cin >> type;

      switch((IDUType)type)
        {
        case (Test_Type):
          read_test_info(&testidu);
          if ( !(net->write_idu((char *)&testidu,Test_Type)) )
             cerr << "ERROR:\tCould not send Test IDU." << endl;
          break;
        case (OtherIDU_Type):
           G_exit_flag = TRUE;
           break;
        default:
          break;
        }
      if ( !G_exit_flag )
         {
         cin >> delay;
         sleep(delay);
         }

      }
   
   net->net_close();  
   }
