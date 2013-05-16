/*
 * Copyright 1992-4   BG Systems, Inc.
 * read_write.c
 *
 * routines that are to be called to read, & write to the LV824
 *
 * Author         Date       Comments
 * John Green     21-Oct-94  Major revision for v 3.0 EPROM
 * John Green     15-Jan-95  Updated for LV824.
 * John Green     01-Feb-95  Final touches for release 3.0
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 * Paul Barham    24-Jul-95  Changed C I/O to C++ I/O
 */

#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv3.h"

extern RS_ERR rs_err;


int w_lv(int, char *);

extern int pack_data(bglv *, char *);
extern int convert_serial(bglv *, char *);

//extern int sginap(long );

int send_outputs(bglv *bgp)
{
   int st;
   char buf[32];
   long count;

/*
 *  Pack the numbers into a character string
 */
   st = pack_data(bgp, buf);
  
   count = strlen(buf);
/*
 *  Simply write the characters to the output serial port (ofd)
 */
   st = write(bgp->sp_fd, buf, strlen(buf));
   if (st != count) {
      cerr << "error: write()\nOSErr: " << st << "\tcount:  "
           << count << endl;
   }
   return(st);
}

int check_inputs(bglv *bgp)
{
   int st;
   int i;
   char str[36];
 
   rs_err.cycles++;
   if( rs_err.cycles % 1000 == 0 ) {
      rs_err.cycles = 0;
      rs_err.thou++;
   }
 
/* 
 *  Read the serial port
 */
   st = read(bgp->sp_fd,str,bgp->str_len);

/*
 *  Read error
 */
   if (st < 0) {
      rs_err.rd++;
      cerr << "Major read error" << endl;
      return(-1);
   }

   i = 0;
/*
 *  We expect to get str_len characters -- if not, we will re-read 
 *  the port 100 times.
 *  If you repeatedly get messages when you are running this code
 *  to the effect that the characters were read after 25 re-tries,
 *  then you are trying to sample too fast.  If you repeatedly get
 *  100 re-tries, then you are probably have no communication at all.
 *  Check the chapter on "trouble shooting".
 */
   while ( st != bgp->str_len && i < 100)
   {
/*
 *  Check for LV824 requesting a handshake -- communication may have been 
 *  interupted, so we need to send an 'h' back.
 */
      if ( str[0] == 'h' ) {
         cerr << "Handshake requested (" << i << ")" << endl;
         w_lv(bgp->sp_fd, "h");
         sginap(10);
         return(-1);
      }
      st = read(bgp->sp_fd,str,bgp->str_len);
      i++;
   }
   if ( i > 0 )
      cerr << i << " read attempts." << endl;
 
   if (str[0] != 'p' || str[bgp->str_len - 1] != '\n') {
      cerr << "Unexpected string :  " << str << endl;
      rs_err.rd++;
      return(-1);
   }
   if (str[0] == '\n' )
      return (1);
 
/*
 *  Aha.  We got some real data !  So convert it from characters
 *  to meaningful numbers, and put them in the bgp data structure.
 */
   st = convert_serial(bgp, str);
 
   return st;
}

int r_lv(bglv *bgp)
{
   int st;
   int i = 0;
   char str[36];
 
   rs_err.cycles++;
   if( rs_err.cycles % 1000 == 0 ) {
      rs_err.cycles = 0;
      rs_err.thou++;
   }
 
   st = read(bgp->sp_fd,str,bgp->str_len);
   if (st < 0) {
      rs_err.rd++;
      cerr << "r_lv():  read error" << endl;
      return(-1);
   }
   while ( st != bgp->str_len && i < 100)
   {
/*
      sginap(1);
*/
      st = read(bgp->sp_fd,str,bgp->str_len);
      i++;
   }
   if ( i > 0 )
      cerr << i << " read attempts." << endl;
 
   if (str[0] != 'B' || str[bgp->str_len - 1] != '\n') {
      cerr << st << "  " << str << endl;
      rs_err.rd++;
      return(-1);
   }

   st = convert_serial(bgp, str);
 
   return st;
}

int w_lv(int sp_fd, char *mode)
{
   int st;

   st = write(sp_fd, mode, strlen(mode));
   if (st < 0)
      rs_err.wrt++;
   return(st);
}

int get_ack(int sp_fd)
{
   int st;
   int i = 0;
   int chars = 2;
   char str[36];
 
   st = read(sp_fd,str,chars);
   if (st < 0) {
      cerr << "get_ack():  read error" << endl;
      return(-1);
   }
   while ( st != 2 && i < 10000) {
      sginap(1);
      st = read(sp_fd,str,chars);
      i++;
   }
   if ( i > 1000 )
      cerr << "Timeout " << chars << " chars in buffer" << endl;
 
   if (str[0] == 'a' ) { 
      cerr << "FlyBox Setup OK." << endl;
      return (0); 
   }
   else if (str[0] == 'f' ) { 
      cerr << "FlyBox Setup failed." << endl;
      return (-1); 
   }
   //if we got here there was an error
   cerr << "Unexpected response from the Flybox: " << str << endl;
   return (-2); 
}
