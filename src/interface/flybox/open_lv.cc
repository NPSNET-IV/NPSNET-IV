/*
 * Copyright 1992-4   BG Systems, Inc.
 * open_lv.c
 *
 * routines that are to be called to open & close the LV824
 *
 * Author         Date       Comments
 * John Green     21-Oct-94  Created from fblib.c
 * John Green     01-Feb-95  Final touches for relase of 3.0
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 * Paul Barham    24-Jul-95  Changed C I/O to C++ I/O
 */

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>

#include "lv3.h"

#define PORT "/dev/ttyd2"

extern int init_lv(bglv *);
extern int check_rev(bglv *);

RS_ERR rs_err;

int set_baud(int, int);

int open_lv(bglv *bgp, char *p, int flag) 
{
  int  st;
  char  port[4];
  char pt[32];
  char *ep;

  rs_err.wrt = 0;
  rs_err.rd = 0;
  rs_err.len = 0;
  rs_err.nl = 0;
  rs_err.cycles = 0;
  rs_err.thou = 0;
  

/*
 * Initialize port.
 */

   if ( ep = getenv("FBPORT") ){
      cerr << "Using the port specified by the env variable FBPORT "
           << ep << endl;
      sprintf(pt,"%s",ep);
   }
   else if (p == NULL)
      sprintf(pt,"%s",PORT);
   else
      sprintf(pt,"%s",p);

   port[0] = pt[strlen(pt)-1];
   bgp->port = atoi(port);
/**
CAUTION:  If you use FB_BLOCK, an error on the read() call will cause
          the program to block -- potentially forever
	  O_NDELAY => no block
**/

   if ( flag == FB_NOBLOCK )
      bgp->sp_fd = open(pt, O_RDWR|O_NDELAY);
   else if ( flag == FB_BLOCK )
      bgp->sp_fd = open(pt, O_RDWR);

   if (bgp->sp_fd < 0)
   {
      perror(pt);
      return(-1);
   }
   st = set_baud(bgp->sp_fd, BAUD192);

   st = check_rev(bgp);
   if ( st < 0 )
      return(st);
   else
      return(0);
}

int set_baud(int sp_fd, int b)
{
   struct termio tios;
   int st;

   st = ioctl(sp_fd,TCGETA,&tios);
   tios.c_iflag = IGNBRK|IXON|IXOFF;
   tios.c_oflag = 0;
   tios.c_lflag = ICANON;

   switch(b & 0x70)
   {
    case BAUD12:
      tios.c_cflag = B1200|CS8|CREAD|CLOCAL;
      break;
    case BAUD24:
      tios.c_cflag = B2400|CS8|CREAD|CLOCAL;
      break;
    case BAUD48:
      tios.c_cflag = B4800|CS8|CREAD|CLOCAL;
      break;
    case BAUD96:
      tios.c_cflag = B9600|CS8|CREAD|CLOCAL;
      break;
    case BAUD192:
      tios.c_cflag = B19200|CS8|CREAD|CLOCAL;
      break;
   }
   st = ioctl(sp_fd,TCSETAF,&tios);
   return(st);
}

void close_lv(bglv *bgp)
{
  int att;
  int st;

  bgp->baud = BAUD192;
  st = init_lv(bgp);

  att = 1000*rs_err.thou + rs_err.cycles;
  close(bgp->sp_fd);
  cerr << "Flybox Read Attempts:  " <<  att << endl;
  cerr << "Flybox Errors Detected:" << endl;
  cerr << "\tRead = " << rs_err.rd << ", Write = " << rs_err.wrt << endl;

}

