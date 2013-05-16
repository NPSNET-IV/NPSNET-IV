
#include "ip_co_sockcomm.h"

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

   net_in = new IP_CO_SockCommClass ( 5001, "", "medusa" );
   net_out = new IP_CO_SockCommClass ( 5000, "", "medusa" );
     
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

   testidu.header.version = (u_char)1; // IDUVersionMarch95
   testidu.header.type = (u_char)1;    // Test Type
   testidu.header.length = (u_short)sizeof(TestIDU);

   testidu.intnum = 1;
   testidu.floatnum = 1.0f;

   delete net_in;
   delete net_out;
   return 0;
}

