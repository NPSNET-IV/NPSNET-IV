/* Copyright (c) 1996, Carnegie Mellon University
 *
 *    Digital Mapping Laboratory (MAPSLab)
 *    Computer Science Department
 *    Carnegie Mellon University
 *    5000 Forbes Avenue
 *    Pittsburgh, PA 15213-3890
 *    maps@cs.cmu.edu
 *    http://www.cs.cmu.edu/~MAPSLab
 *
 *
 * Permission to use, copy, and modify this software and its
 * documentation for any NON-COMMERCIAL purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Digital Mapping Laboratory 
 * (MAPSLab) Computer Science Department, Carnegie Mellon University
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *
 * We request that users of this software to return to maps@cs.cmu.edu
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 * Redistribution of this code is forbidden without the express written 
 * consent of the MAPSLab Research Group. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 */
/* Read functions for TSG Buffer
 *****************************************************************
 * HISTORY
 *  20-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <tsgpf.h>
#include <tsgBuf.h>

/* tsgBufRead()
   Read arbitrary bytes out of a tsgAppendBuffer.
   */
int tsgBufRead(void *data, size_t size, struct tsgAppendBuffer *buf)
{
  if (buf->used+size > buf->allocated) {
    buf->used += size;
    return (-1);
  }

  memmove(data,&buf->data[buf->used],size);

  buf->used += size;
  return size;
}

/* tsgBufReadStr()
   Copy a string of arbtrary length out of a tsgAppendBuffer.
   */
char *tsgBufReadStr(struct tsgAppendBuffer *buf)
{
  char *str,*ret,*start;
  int len;

  if (buf->used >= buf->allocated)
    return NULL;

  start = &buf->data[buf->used];

  for (len=0,str=start;*str && (buf->used +len <= buf->allocated);len++,str++);
  if (*str)
    return(NULL);

  ret = pfMalloc(len+1,pfGetSharedArena());
  strcpy(ret,start);
  buf->used += len+1;

  return(ret);
}
