/*
 * Copyright 1992-4   BG Systems, Inc.
 * init_lv.c
 *
 * Routine to init a Rev 3.0 Box
 *
 * Author         Date       Comments
 * John Green     15-Oct-94  Major revision for v 3.0 EPROM
 * John Green     21-Oct-94  Broken out of fblib.c
 * John Green     14-Jan-95  Added acknowledge
 * John Green     01-Feb-95  Final touches for rev 3.0 release
 * John Green     21-Feb-95  Fixed bug when init_lv() called again
 * John Green     15-Mar-95  Fixed dig out setup bug
 * John Green     03-Apr-95  Allow 2.4 EPROM
 * John Green     17-Apr-95  return() rather than exit() if setup fails
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 */

#include <stdio.h>
#include <stdlib.h>

#include "lv3.h"

extern RS_ERR rs_err;

extern int w_lv(int, char *);
extern int get_ack(int );
extern int set_baud(int, int );
extern int check_rev(bglv *);
extern int check_setup(bglv *);

int init_lv(bglv *bgp)
{
   char c1, c2, c3;
   char str[5];
   int  st;
   int  i;

   st = check_setup(bgp);
   if ( st < 0 )
      return(st);

/*
 *  Compute the number of channels requested, and the 
 *  appropriate string length.
 */

/*
 *  Analog inputs
 */
   bgp->n_analog_in = 0;
   for ( i=0; i < 8; i++)
      if ( (bgp->analog_in >> i) & 0x1 )
         bgp->n_analog_in++;

/*
 *  Digital inputs
 */

   switch(bgp->dig_in)
   {
    case 0x0:
      bgp->n_dig_in = 0;
      break;
    case 0x10:
    case 0x20:
    case 0x40:
      bgp->n_dig_in = 8;
      break;
    case 0x30:
    case 0x50:
    case 0x60:
      bgp->n_dig_in = 16;
      break;
    case 0x70:
      bgp->n_dig_in = 24;
      break;
   }

/*
 *  Digital outputs
 */

   switch(bgp->dig_out)
   {
    case 0x0:
      bgp->n_dig_out = 0;
      break;
    case 0x10:
    case 0x20:
    case 0x40:
      bgp->n_dig_out = 8;
      break;
    case 0x30:
    case 0x50:
    case 0x60:
      bgp->n_dig_out = 16;
      break;
    case 0x70:
      bgp->n_dig_out = 24;
      break;
   }


/*
 *  Analog outputs
 */
   bgp->n_analog_out = 0;
   if ( bgp->analog_out > 0 )
   {
      for ( i=0; i < 3; i++)
         if ( (bgp->analog_out >> i) & 0x1 )
            bgp->n_analog_out++;
   }


/*
 *  Set the string length for receiving data
 */
   bgp->str_len  = 2 + (2*bgp->n_analog_in) + (bgp->n_dig_in/4);


/*
 *  First character has the baud rate and the lower 4 analog ins.
 */
   c1 =  bgp->baud;
   c1 |= (bgp->analog_in & 0xf);
/*
 *  Second character has the digital inputs and the upper 4 analog ins
 */
   c2 =  bgp->dig_in;
   c2 |= (bgp->analog_in & 0xf0) >> 4;

   if ( bgp->Rev.major == 3 )
   {
      str[0] = 's';
/*
 *  Third character (for rev 3 eproms only, has the digital outs (-F)
 *  and analog outs (-3G)
 */
      c3 =  bgp->analog_out & 0xf;
      c3 |= bgp->dig_out & 0xf0;
/*
 *  Add the OFFSET to each character to make sure they are not control
 *  characters
 */
      str[1] = c1 + OFFSET;
      str[2] = c2 + OFFSET;
      str[3] = c3 + OFFSET;
      str[4] = '\0';
      st = w_lv(bgp->sp_fd, str);
/*
 *  Make sure that the LV got the setup !
 */
      st = get_ack(bgp->sp_fd);
/*
 *  If we have a rev 3.00 eprom, just don't check the return
 *  value - just proceed and assume things are OK.
 *  (Bug fixed in 3.01)
 */
      if ( bgp->Rev.bug != 0 )
      {
         if ( st < 0 )
            return(st);
      }
   }
   else if ( bgp->Rev.major == 2 )
   {
      if ( bgp->Rev.minor == 2 )
      {
/*
 *  For rev 2.2 EPROMS use an 'R' and no offset -- so make sure c1 and
 *  c2 are not flow control characters !
 */
         str[0] = 'R';
         str[1] = c1;
         str[2] = c2;
      }
      else if ( bgp->Rev.minor >= 3 )
      {
/*
 *  For rev 2.3 EPROMS use an 'r' and offset the characters
 */
         str[0] = 'r';
         str[1] = c1 + OFFSET;
         str[2] = c2 + OFFSET;
      }
      str[3] = '\0';
      st = w_lv(bgp->sp_fd, str);
   }
 
   st = set_baud(bgp->sp_fd, bgp->baud);
 
   return(0);
}
