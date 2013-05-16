
#include "mc_ip_cl_sockcomm.h"
#include "uc_ip_cl_sockcomm.h"
#include "bc_ip_cl_sockcomm.h"
#include "ip_co_sockcomm.h"
#include "up_co_sockcomm.h"
#include "up_cl_sockcomm.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream.h>

typedef u_char IDUType;

typedef struct {
   unsigned char        version;
   IDUType              type;
   unsigned short       length;
} IDUHeader;

typedef struct {
   IDUHeader header;
   char   string[128];
   int    intnum;
   float  floatnum;
} TestIDU;

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


void output_IDUHeader ( IDUHeader *header, char *leader )
   {

   cerr << leader << "IDU HEADER:" << endl;
   cerr << leader << "   Type:                     "
        << (int)header->type << endl;
   cerr << leader << "   Version:                  "
        << (int)header->version << endl;
   cerr << leader << "   Length:                   "
        << (int)header->length << endl;

   }

void output_TestIDU ( TestIDU *test, char *sender, char *leader )
   {

   char lead[120];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cerr << leader << "TEST IDU ### from "
        << sender << endl;
   output_IDUHeader ( &(test->header), lead );
   cerr << lead << "String component: " << test->string << endl;
   cerr << lead << "Int component:    " << test->intnum << endl;
   cerr << lead << "Float component:  " << test->floatnum << endl;
   cerr << endl;

   }

int
main ( int argc, char **argv )
{
   TestIDU testidu;
   SockCommClass *net_in, *net_out;
   char sender[25];
   char otherhost[50];
   int i_start = 0;
   short port;
   char pathname1[15], pathname2[15], pathname3[15], pathname4[15];

   if ( argc < 2 ) {
      cerr << "Usage:" << endl;
      cerr << "\t./test [unicast <address>|broadcast|multicast|tcp|unixstream|unixdgram] [go]" << endl;
      exit(0);
   }
   else {
      if ( (strcmp(argv[1], "unicast")) == 0 ) {
         if ( argc > 2 ) {
            strcpy ( otherhost, argv[2] );
            if ( argc > 3 ) {
               i_start = 1;
            }
         }
         else {
            strcpy ( otherhost, "131.120.7.32" );
         }

         net_in = new UC_IP_CL_SockCommClass ( 3123, "",
                                               otherhost, 0 );
         net_out = new UC_IP_CL_SockCommClass ( 3123, "",
                                                otherhost, 0 );
         strcpy ( testidu.string, "Test of UC_IP_CL_SockCommClass" );
      }
      else if ( (strcmp(argv[1], "tcp")) == 0 ) {
         if ( argc > 2 ) {
            strcpy ( otherhost, argv[2] );
            if ( argc > 3 ) {
               i_start = 1;
            }
         }
         else {
            strcpy ( otherhost, "131.120.7.32" );
         }

         if ( i_start ) {
            port = 3126;
         }
         else {
            port = 3127;
         }
         net_in = new IP_CO_SockCommClass ( port, "",
                                               otherhost );
         if ( i_start ) {
            port = 3127;
         }
         else {
            port = 3126;
         }
         net_out = new IP_CO_SockCommClass ( port, "",
                                                otherhost );
         strcpy ( testidu.string, "Test of IP_CO_SockCommClass" );
      }

      else if ( (strcmp(argv[1], "broadcast")) == 0 ) {
         net_in = new BC_IP_CL_SockCommClass ( 3124, "", 0 );
         net_out = new BC_IP_CL_SockCommClass ( 3124, "", 0 );
         i_start = (argc > 2);
         strcpy ( testidu.string, "Test of BC_IP_CL_SockCommClass" );
      }
      else if ( (strcmp(argv[1], "multicast")) == 0 ) {
         net_in = new MC_IP_CL_SockCommClass ( 3125, "",
                                               DEFAULT_MC_GROUP,
                                               DEFAULT_MC_TTL, 0 );
         net_out = new MC_IP_CL_SockCommClass ( 3125, "",
                                                DEFAULT_MC_GROUP,
                                                DEFAULT_MC_TTL, 0 );
         i_start = (argc > 2);
         strcpy ( testidu.string, "Test of MC_IP_CL_SockCommClass" );
      }
      else if ( (strcmp(argv[1],"unixstream")) == 0 ) {
         strcpy(pathname1, "/tmp/us1NPSNET");
         strcpy(pathname2, "/tmp/us2NPSNET");
         strcpy(pathname3, "/tmp/us3NPSNET");
         strcpy(pathname4, "/tmp/us4NPSNET");
         i_start = (argc > 2);
         if ( i_start ) {
            net_in = new UP_CO_SockCommClass ( pathname1, pathname2 );
            net_out = new UP_CO_SockCommClass ( pathname3, pathname4 );
         }
         else {
            net_in = new UP_CO_SockCommClass ( pathname4, pathname3 );
            net_out = new UP_CO_SockCommClass ( pathname2, pathname1 );
         }
         strcpy ( testidu.string, "Test of UP_CO_SockCommClass" );
      }
      else if ( (strcmp(argv[1],"unixdgram")) == 0 ) {
         strcpy(pathname1, "/tmp/us5NPSNET");
         strcpy(pathname2, "/tmp/us6NPSNET");
         strcpy(pathname3, "/tmp/us7NPSNET");
         strcpy(pathname4, "/tmp/us8NPSNET");
         i_start = (argc > 2);
         if ( i_start ) {
            net_in = new UP_CL_SockCommClass ( pathname1, pathname2 );
            net_out = new UP_CL_SockCommClass ( pathname3, pathname4 );
         }
         else {
            net_in = new UP_CL_SockCommClass ( pathname4, pathname3 );
            net_out = new UP_CL_SockCommClass ( pathname2, pathname1 );
         }
         strcpy ( testidu.string, "Test of UP_CL_SockCommClass" );
      }
      else {
         cerr << "Usage:" << endl;
         cerr << "\t./test [unicast <address>|broadcast|multicast|tcp|unixstream|unixdgram]" << endl;
         exit(0);
      }
   }

     
   if ( i_start ) {
      if ( !net_in->open_communication(SC_SOCKET_READ) ) {
         cerr << "Test program unable to open socket for reading." << endl;
         delete net_in;
         exit(0);
      }
      if ( !net_out->open_communication(SC_SOCKET_WRITE) ) {
         cerr << "Test program unable to open socket for writing." << endl;
         delete net_out;
         exit(0);
      }
   }
   else { 
      if ( !net_out->open_communication(SC_SOCKET_WRITE) ) {
         cerr << "Test program unable to open socket for writing." << endl;
         delete net_out;
         exit(0);
      }
      if ( !net_in->open_communication(SC_SOCKET_READ) ) {
         cerr << "Test program unable to open socket for reading." << endl;
         delete net_in;
         exit(0);
      }
   }

   cerr << "----------------------------------------------------" << endl;
   cerr << "WRITE SOCKET-----" << endl;
   output_isa(net_out);
   cerr << "READ SOCKET------" << endl;
   output_isa(net_in);
   cerr << "----------------------------------------------------" << endl;

   testidu.header.version = (u_char)1; // IDUVersionMarch95
   testidu.header.type = (u_char)1;    // Test Type
   testidu.header.length = (u_short)sizeof(TestIDU);

   testidu.intnum = 1;
   testidu.floatnum = 1.0f;

   for ( int i = 0; i < 25; i++ ) {
      if ( i == 0 ) {
         if ( i_start ) {
            cerr << "SENDING..." << endl;
            net_out->write_buffer ( (char *) &(testidu), sizeof(testidu) );
         }
      }
      else {
         cerr << "SENDING..." << endl;
         net_out->write_buffer ( (char *) &(testidu), sizeof(testidu) );
      }
      if ( i == 24 ) {
         if ( !i_start ) {
            cerr << "RECEIVING..." << endl; 
            net_in->read_buffer ( (char *) &(testidu), sizeof(testidu), sender );
            output_TestIDU ( &testidu, sender, "" );
         }
      }
      else {
         cerr << "RECEIVING..." << endl;
         net_in->read_buffer ( (char *) &(testidu), sizeof(testidu), sender );
         output_TestIDU ( &testidu, sender, "" );
      }
      testidu.intnum += 1;
      testidu.floatnum *= 2.0f;
   }

   delete net_in;
   delete net_out;
   return 0;
}

