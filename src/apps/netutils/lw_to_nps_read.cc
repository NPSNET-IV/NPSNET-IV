
#include "ip_co_sockcomm.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream.h>

#include "lw-nps.h"

typedef struct {
   int packet_size;
   DIS_WSIF_rec data;
} NPS_TO_LW;

typedef struct {
   int packet_size;
   AER_rec data;
} LW_TO_NPS;


void output_isa ( SockCommClass *sock_class )
{
   if ( sock_class->isa(SC_SOCKET) )
      cerr << "isa->SC_SOCKET" << endl;
   if ( sock_class->isa(SC_UNIX_PROTO) )
      cerr << "isa->SC_UNIX_PROTO" << endl;
   if ( sock_class->isa(SC_IP_PROTO) )
      cerr << "isa->SC_IP_PROTO" << endl;
   if ( sock_class->isa(SC_CONNECT_ORIENTED) )
      cerr << "isa->SC_CONNECT_ORIENTED" << endl;
   if ( sock_class->isa(SC_CONNECT_LESS) )
      cerr << "isa->SC_CONNECT_LESS" << endl;
   if ( sock_class->isa(SC_UDP) )
      cerr << "isa->SC_UDP" << endl;
   if ( sock_class->isa(SC_TCP) )
      cerr << "isa->SC_TCP" << endl;
   if ( sock_class->isa(SC_UNIX_STREAM) )
      cerr << "isa->SC_UNIX_STREAM" << endl;
   if ( sock_class->isa(SC_UNIX_DATAGRAM) )
      cerr << "isa->SC_UNIX_DATAGRAM" << endl;
   if ( sock_class->isa(SC_TCP_STREAM) )
      cerr << "isa->SC_TCP_STREAM" << endl;
   if ( sock_class->isa(SC_UDP_BROADCAST) )
      cerr << "isa->SC_UDP_BROADCAST" << endl;
   if ( sock_class->isa(SC_UDP_MULTICAST) )
      cerr << "isa->SC_UDP_MULTICAST" << endl;
   if ( sock_class->isa(SC_UDP_UNICAST) )
      cerr << "isa->SC_UDP_UNICAST" << endl;
}


int
main ( int argc, char **argv )
{
   SockCommClass *net_in; //, *net_out;
/*
   NPS_TO_LW outgoing;
*/
   LW_TO_NPS incoming;
   char from[255];
//   char test_string[255];
//   char *test_char;

   if ( argc < 2) {
      cerr << "usage: " << argv[0] << " <machine>" << endl;
      exit(0);
   }
   net_in = new IP_CO_SockCommClass ( 5001, "", argv[1] );
/*
   net_out = new IP_CO_SockCommClass ( 5000, "", argv[1] );
*/
     
   if ( !net_in->open_communication(SC_SOCKET_READ) ) {
      cerr << "Test program unable to open socket for reading." << endl;
      delete net_in;
      exit(0);
   }
/*
   if ( !net_out->open_communication(SC_SOCKET_WRITE) ) {
      cerr << "Test program unable to open socket for writing." << endl;
      delete net_out;
      exit(0);
   }
*/

/*
   outgoing.packet_size = sizeof(DIS_WSIF_rec);
   outgoing.data.host_id = 36;
   outgoing.data.entity_id = 1;
   outgoing.data.site_id = 1001;
   outgoing.data.laser = 0;
   outgoing.data.trigger = 0;
   outgoing.data.video = 0;
*/
  
   do {
/* 
      cout << "lvt:";
      cin.getline(test_string, 255);
      cerr << "Sending ";
      if ( (test_char = strchr(test_string,(int)'T')) != NULL ) {
         outgoing.data.trigger = 1;
         cerr << "TRIGGER ";
      }
      else {  
         outgoing.data.trigger = 0;
      }
      if ( (test_char = strchr(test_string,(int)'L')) != NULL ) {
         outgoing.data.laser = 1;
         cerr << "LASER ";
      }
      else {  
         outgoing.data.laser = 0;
      }
      if ( (test_char = strchr(test_string,(int)'V')) != NULL ) {
         outgoing.data.video = 1;
         cerr << "VIDEO ";
      }
      else {  
         outgoing.data.video = 0;
      }
      cerr << endl;
      net_out->write_buffer ( (char *)&(outgoing), sizeof(outgoing) );
   
      if ( outgoing.data.laser ) {
         cerr << "Waiting for laser information back..." << endl;
*/

         if (net_in->read_buffer((char *)&(incoming.packet_size),
                                 sizeof(incoming.packet_size),from)) {
            if ( incoming.packet_size == 0 ) {
               cerr << "Heartbeat from NPS." << endl;
            }
            else {
               net_in->read_buffer((char *)&(incoming.data),
                                   sizeof(incoming.data),from);
               cerr << "Laser information back from NPS:" << endl;
               cerr << "\tSite = " << incoming.data.site_id << ", Host = "
                    << incoming.data.host_id << ", Entity = " 
                    << incoming.data.entity_id
                    << "." << endl;
               cerr << "\tElevation = " << incoming.data.elevation.whole
                    << "." << incoming.data.elevation.decimal
                    << ", Azimuth = " << incoming.data.azimuth.whole
                    << "." << incoming.data.azimuth.decimal
                    << ", Range = " << incoming.data.range.whole
                    << "." << incoming.data.range.decimal
                     << endl;
            }
         }
/*
      }
*/
   } while (1); /* ( strcmp(test_string,"quit") != 0 ); */
/*
   delete net_in;
   delete net_out;
   return 0;
*/
}

