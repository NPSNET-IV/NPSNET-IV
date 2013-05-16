/*
 * fblib.c
 *
 * routines that are to be called to open, read, & close the FlyBox
 *
 * Copyright 1992 BG Systems 
 *
 * 30-Apr-1992   John Green
 * 19-Apr-1993   John Green  Added ioctl to flush buffer (thanks Mike
 *                           Engledinger)
 * 28-Apr-1993   John Green  Added alternate open and read routines, and
 *                           low level calls to write and read
 *  1-Aug-1993   John Green  Added functions for rev 2.0 EPROM
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stropts.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>

#include "fb.h"
#include "fblib.h"

static RS_ERR rs_err;
static int fb_fd;

static Chans ch;
/***
old line
extern Chans ch;
***/

#ifdef __cplusplus
extern "C" {
#endif

char *getenv(const char*);
int ioctl (int fildes, int request, ...);

#ifdef __cplusplus
}
#endif


int set_baud(int b);
static convert_serial(char *str,float *inbuf, int *dioval);

int w_fb(char *mode)
{
   int st;

   st = write(fb_fd, mode, strlen(mode));
   if (st < 0)
      rs_err.wrt++;
   return(st);
}

int open_flybox(char *p, int flag) 
{
  int  st;
  char pt[32];
  char *ep;

  rs_err.wrt = 0;
  rs_err.rd = 0;
  rs_err.len = 0;
  rs_err.nl = 0;
  rs_err.cycles = 0;
  rs_err.thou = 0;
  
   ch.count   = 0;
   ch.n_AChan = 8;
   ch.n_DChan = 16;
   ch.AChan = 0xff;
   ch.DChan = 0x30;
   ch.Baud = BAUD192;
   ch.StrLen = 2 + (2 * ch.n_AChan) + (ch.n_DChan/4) ;
   ch.num_points = 100;
   ch.mode[0] = 'o';
   ch.mode[1] = '\0';


/*
 * Initialize flybox port.
 */

   if ( ep = getenv("FBPORT") )
      sprintf(pt,"%s",ep);
   else if (p == NULL)
      sprintf(pt,"%s",PORT);
   else
      sprintf(pt,"%s",p);

/**
CAUTION:  If you use FB_BLOCK, an error on the read() call will cause
          the program to block -- potentially forever
	  O_NDELAY => no block
**/

   fb_fd = open(pt, O_RDWR|O_NDELAY);
/**
   fb_fd = open(pt, O_RDWR|O_NDELAY);
   fb_fd = open(pt, O_RDWR);
**/

   if (fb_fd < 0)
   {
      perror(pt);
      return(-1);
   }
   st = set_baud(BAUD192);

   return(1);
}

int init_flybox( void )
{
   char c1, c2;
   char str[4];
   int st;

   c1 = ch.Baud;
   c1 |= (ch.AChan & 0xf);
   c2 = ch.DChan;

   c2 |= (ch.AChan & 0xf0) >> 4;
   str[0] = 'R';
   str[1] = c1;
   str[2] = c2;
   str[3] = '\0';
   st = w_fb(str);

   st = set_baud(ch.Baud);

   return(0);
}

int set_baud(int b)
{
   struct termio tios;
   int st;

   st = ioctl(fb_fd,TCGETA,&tios);
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
   st = ioctl(fb_fd,TCSETAF,&tios);
   return(st);
}

int r_fb(int *dio, float *ach)
{
   int st;
   int i;
   char str[36];

   int wval;
 
   rs_err.cycles++;
   if( rs_err.cycles % 1000 == 0 )
   {
      rs_err.cycles = 0;
      rs_err.thou++;
   }

   st = read(fb_fd,str,ch.StrLen);
   if (st < 0)
   {
      rs_err.rd++;
      printf("readflybox():  read error\n");
      return(-1);
   }
   i = 0;
   while ( st != ch.StrLen && i < 100)
   {
/**
      spin(1000);
sginap(1);
**/
      st = read(fb_fd,str,ch.StrLen);
      i++;
   }
   if ( i > 0 )
      printf("count: %ld  i = %d\n", ch.count,  i);

   if (str[0] != 'B' || str[ch.StrLen - 1] != '\n')
   {
      printf("%d:  %s\n", st, str);
      rs_err.rd++;
      return(-1);
   }
   if (str[0] == '\n' )
   { printf("%s", str); return (1); }
 
   st = convert_serial(str, ach, dio);
 
   return st;
}

int ll_write_fb(char *c)
{
   int st;

   st = write(fb_fd,c,1);
   if (st < 0)
      rs_err.wrt++;
   return(st);
}

int ll_read_fb(int *dioval, float *inbuf)
{
   int i,st;
   int wval;
   char str[100];
 
   rs_err.cycles++;
   if( rs_err.cycles % 1000 == 0 )
   {
      rs_err.cycles = 0;
      rs_err.thou++;
   }
   st = read(fb_fd,str,25);
   if (st < 0)
   {
      rs_err.rd++;
      printf("readflybox():  read error\n");
      return(-1);
   }
   if ( st != ch.StrLen)
   {
      rs_err.rd++;
   }
   if (str[0] != 'B' || str[ch.StrLen-1] != '\n')
   {
      rs_err.rd++;
   }
 
   st = convert_serial(str, inbuf, dioval);
 
   return st;
}

void close_flybox(void)
{
  int att;
  int st;

  ch.Baud = BAUD192;
  st = init_flybox();

  att = 1000*rs_err.thou + rs_err.cycles;
  close(fb_fd);
  printf("\nRead Attempts:  %d\n", att);
  printf("\nErrors Detected\n");
  printf("Read        Write    \n");
  printf("%5d      %5d     \n",rs_err.rd, rs_err.wrt);

}

static convert_serial(char *str,float *inbuf, int *dioval)
{
   int i,digp,cs;
   int k = 0;
   int checking = 0;
   float tmp[8];

   if ( ch.mode[0] == 'O' )
   { 
      k = 2;
      checking = 1;
      cs = str[0]+str[1];
   }
   digp = 0;
/*
 *  Load the digital input values into dioval
 */
   k = 1 + ch.n_DChan/4;
   if ( k > 1)
   {
      for (i = 1; i < k; i++)
      {
         digp = (digp << 4) | 0x0f & (str[i]-0x21);
         cs += str[i];
      }
      if ( ch.DChan == 0x20 )
         *dioval = digp << 8;
      else
         *dioval = digp;
   }

/*
 *  Load the 8 analog values into inbuf
 */
   for (i = k; i < ch.StrLen - 2; i += 2)
   {
      digp = ((0x3f & (str[i]-0x21)) << 6) |
                    (0x3f & (str[i+1]-0x21));
      tmp[(i-k)/2] = -1.0 + (2.0 * digp/4096);
      cs += str[i]+str[i+1];
   }
   for ( i = 0, k = 0; k < 8; k++ )
   {
      if ( ch.AChan >> k &0x1 )
      {
         inbuf[k] = tmp[i];
	 i++;
      }
   }

   digp =  ((0x0f & (str[22]-0x21)) <<  4) |
           (0x0f & (str[23]-0x21));
   cs &= 0xff;
   return (cs==digp?0:1);
}


void spin(int wval)
{
    int i = 0;
    int j = 0;
    while ( i++ < wval )
       j = i+3;
}
