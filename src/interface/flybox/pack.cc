/*
 * Copyright 1992-4   BG Systems, Inc.
 * pack.c
 *
 * Routine to pack data for outputs
 *
 * Author         Date       Comments
 * John Green     21-Oct-94  Major revision for v 3.0 EPROM
 * John Green     28-Jan-95  Change order for packing outputs
 * John Green     01-Feb-95  Final touches for release
 * John Green     15-Mar-95  Fix bug for dig outputs
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 */

#include <stdio.h>

#include "lv3.h"

/*
 *  This routine converts the compressed data string from
 *  characters to 8 floats and a single 16 bit int for
 *  the discretes
 */

int pack_data(bglv *bgp, char * out_buf)
{
   int   i,k;
   out_buf[0] = 'p';
   i = 1;
  
/*
 *  Load the 3 digital values into out_buf
 */
   for ( k = 2; k >= 0; k-- )
   {
      if ( bgp->dig_out >> k & 0x10 )
      {
         out_buf[i++] = ((bgp->dout[k]>>4) & 0xf ) + 0x21;
         out_buf[i++] = (bgp->dout[k] & 0xf ) + 0x21;
      }
   }
/*
 *  Load the 3 analog values into out_buf
 */
   for (k = 0; k < 3; k++)
   {
      if ( ( bgp->analog_out >> k ) & 0x1 )
      {
         out_buf[i++] = (( bgp->aout[k] >> 6 ) & 0x3f) + 0x21;
         out_buf[i++] = (bgp->aout[k] & 0x3f) + 0x21;
      }
   }
/*
 *  And terminate the string
 */
   out_buf[i++] = '\n';
   out_buf[i++]   = '\0';

   return(0);
}
