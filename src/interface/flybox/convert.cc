/*
 * Copyright 1992-4   BG Systems, Inc.
 * convert.c
 *
 * Routine to convert input string 
 *
 * Author         Date       Comments
 * John Green     21-Oct-94  Major revision for v 3.0 EPROM
 * John Green     01-Feb-95  Final rev 3.0 cleaning
 * John Green     15-Mar-95  Fixed scaling bug
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 */

#include <stdio.h>

#include "lv3.h"

int convert_serial(bglv *bgp, char *str)
{
   int i, digp, j;
   int k = 0;
   float tmp[8];

   digp = 0;
/*
 *  Load the digital input values into dioval
 */
   k = 1 + bgp->n_dig_in/4;
   if ( k > 1)
   {
      i = 1;
      for ( j = 2; j >= 0; j-- )
      {
         if ( bgp->dig_in & 0x10<<j )
         {
            digp = 0x0f & (str[i++]-0x21);
            digp = (digp << 4) | 0x0f & (str[i++]-0x21);
            bgp->din[j] = digp;
         }
      }
   }

/*
 *  Load the 8 analog values into inbuf
 */
   for (i = k; i < bgp->str_len - 2; i += 2)
   {
      digp = ((0x3f & (str[i]-0x21)) << 6) |
                    (0x3f & (str[i+1]-0x21));
      tmp[(i-k)/2] = -1.0 + (2.0 * digp/4095);
   }
   for ( i = 0, k = 0; k < 8; k++ )
   {
      if ( bgp->analog_in >> k &0x1 )
      {
         bgp->ain[k] = tmp[i];
	 i++;
      }
   }

   digp =  ((0x0f & (str[22]-0x21)) <<  4) |
           (0x0f & (str[23]-0x21));
   return (0);
}
